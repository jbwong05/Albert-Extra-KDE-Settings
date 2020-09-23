#include <QDebug>
#include <QPointer>
#include <QRegularExpression>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <KService>
#include <KServiceTypeTrader>
#include "albert/util/standarditem.h"
#include "xdg/iconlookup.h"
#include "albert/util/shutil.h"
#include "albert/util/standardactions.h"
#include "albert/util/standarditem.h"
#include "albert/util/standardindexitem.h"
#include "albert/util/offlineindex.h"
#include "configwidget.h"
#include "extension.h"
#include "KCMService.h"
using namespace Core;
using namespace std;

#define ACTIVATED_SETTINGS ALBERT_EXTENSION_IID"-activated"
#define NAME_SETTINGS ALBERT_EXTENSION_IID"-names"
#define ICON_SETTINGS ALBERT_EXTENSION_IID"-icon_names"
#define COMMENT_SETTINGS ALBERT_EXTENSION_IID"-comments"
#define ALIAS_SETTINGS ALBERT_EXTENSION_IID"-aliases"

#define FALLBACK_ICON "preferences-system"

class ExtraKdeSettings::Private {
    public:
        QPointer<ConfigWidget> widget;

        vector<shared_ptr<KCMService>> index;
        OfflineIndex offlineIndex;
        QFutureWatcher<vector<shared_ptr<KCMService>>> futureWatcher;

        QMap<QString, shared_ptr<KCMService>> kcmServicesMap;

        QMap<QString, QVariant> activatedSettings;
        QMap<QString, QVariant> nameSettings;
        QMap<QString, QVariant> iconSettings;
        QMap<QString, QVariant> commentSettings;
        QMap<QString, QVariant> aliasSettings;

        ~Private() {
            /*auto iter = kcmServicesMap.keyBegin();
            while(iter != kcmServicesMap.keyEnd()) {
                delete kcmServicesMap.value(*iter);
                iter++;
            }*/
            kcmServicesMap.clear();
        }

        void startLoadingSettingsModules();
        void finishLoadingSettingsModules();
        vector<shared_ptr<ExtraKdeSettings::KCMService>> loadSettingsModules() const;
        QString generateQuery(const QString &strList) const;
        
};

void ExtraKdeSettings::Private::startLoadingSettingsModules() {
    // Never run concurrent
    if ( futureWatcher.future().isRunning() ) {
        //rerun = true;
        return;
    }

    // Run finishIndexing when the indexing thread finished
    futureWatcher.disconnect();
    QObject::connect(&futureWatcher, &QFutureWatcher<vector<shared_ptr<Core::StandardIndexItem>>>::finished,
                     std::bind(&Private::finishLoadingSettingsModules, this));

    // Run the indexer thread
    futureWatcher.setFuture(QtConcurrent::run(this, &Private::loadSettingsModules));
}

void ExtraKdeSettings::Private::finishLoadingSettingsModules() {

    // Get the thread results
    index = futureWatcher.future().result();

    // Rebuild the offline index
    offlineIndex.clear();
    for (const shared_ptr<ExtraKdeSettings::KCMService> &item : index) {
        offlineIndex.add(item);
        kcmServicesMap.insert(item->name, item);
    }
}

vector<shared_ptr<ExtraKdeSettings::KCMService>> ExtraKdeSettings::Private::loadSettingsModules() const {
    
    vector<shared_ptr<KCMService>> settingsModules;
    
    // Lookup kcm modules
    QString queryStr = generateQuery("");

    KService::List services = KServiceTypeTrader::self()->query(QStringLiteral("KCModule"), queryStr);
    for (const KService::Ptr &service : qAsConst(services)) {
        if(service->noDisplay()) {
            continue;
        } else {
            QString serviceStorageId = service->storageId();

            bool isActivated = activatedSettings.contains(serviceStorageId) ? activatedSettings.value(serviceStorageId).toBool() : true;
            QString serviceName = nameSettings.contains(serviceStorageId) ? nameSettings.value(serviceStorageId).toString() : service->name();
            
            QString serviceIcon;
            if(iconSettings.contains(serviceStorageId)) {
                serviceIcon = iconSettings.value(serviceStorageId).toString();
            } else if(!service->icon().isEmpty()) {
                serviceIcon = service->icon();
            } else {
                serviceIcon = FALLBACK_ICON;
            }

            QString serviceComment = commentSettings.contains(serviceStorageId) ? commentSettings.value(serviceStorageId).toString() : service->comment();

            QStringList serviceAliases;
            if(aliasSettings.contains(serviceStorageId)) {
                serviceAliases = aliasSettings.value(serviceStorageId).toStringList();
            }

            vector<IndexableItem::IndexString> indexStrings;
            indexStrings.emplace_back(serviceName, UINT_MAX);
            for(QString alias : serviceAliases) {
                indexStrings.emplace_back(alias, UINT_MAX);
            }

            //KCMService* currentService = new KCMService(isActivated, serviceStorgaeId, service->exec(), 
            //        serviceName, serviceComment, serviceIcon, serviceAliases);
            shared_ptr<KCMService> newService = std::make_shared<KCMService>(isActivated, serviceStorageId, service->exec(), 
                    serviceName, serviceComment, serviceIcon, serviceAliases);
            newService->setIconPath(XDG::IconLookup::iconPath(serviceIcon));
            newService->setId(serviceStorageId);
            newService->setText(serviceName);
            newService->setCompletion(serviceName);
            newService->setSubtext(serviceComment);
            newService->setIndexKeywords(std::move(indexStrings));
            newService->addAction(make_shared<ProcAction>(serviceName, QStringList(Core::ShUtil::split(service->exec()))));

            settingsModules.push_back(std::move(newService));
            //kcmServicesMap.insert(serviceName, newService);
        }
    }
    return settingsModules;
}

QString ExtraKdeSettings::Private::generateQuery(const QString &str) const {
    QString nameTemplate = QStringLiteral("exist Name");
    QString commentTemplate = QStringLiteral("exist Comment");

    // Search for applications which are executable and the term case-insensitive matches any of
    // * a substring of one of the comments
    // * a substring of the Name field
    // Note that before asking for the content of e.g. Keywords and GenericName we need to ask if
    // they exist to prevent a tree evaluation error if they are not defined.

    nameTemplate += QStringLiteral(" and '%1' ~~ Name").arg(str);
    commentTemplate += QStringLiteral(" and '%1' ~~ Comment").arg(str);

    QString finalQuery = QStringLiteral("exist Exec and ( (%1) or ('%2' ~~ Exec) or (%3) )")
        .arg(nameTemplate, str, commentTemplate);

    return finalQuery;
}

/** ***************************************************************************/
ExtraKdeSettings::Extension::Extension()
    : Core::Extension("org.albert.extension.extrakdesettings"),
      Core::QueryHandler(Core::Plugin::id()),
      d(new Private) {

    registerQueryHandler(this);

    // Lookup settings
    d->activatedSettings = settings().value(ACTIVATED_SETTINGS).toMap();
    d->nameSettings = settings().value(NAME_SETTINGS).toMap();
    d->iconSettings = settings().value(ICON_SETTINGS).toMap();
    d->commentSettings = settings().value(COMMENT_SETTINGS).toMap();
    d->aliasSettings = settings().value(ALIAS_SETTINGS).toMap();

    d->offlineIndex.setFuzzy(true);
    loadSettingsModules();
}

void ExtraKdeSettings::Extension::loadSettingsModules() {
    d->startLoadingSettingsModules();
}



/** ***************************************************************************/
ExtraKdeSettings::Extension::~Extension() {
    d->futureWatcher.waitForFinished();
    settings().setValue(ACTIVATED_SETTINGS, d->activatedSettings);
    settings().setValue(NAME_SETTINGS, d->nameSettings);
    settings().setValue(ICON_SETTINGS, d->iconSettings);
    settings().setValue(COMMENT_SETTINGS, d->commentSettings);
    settings().setValue(ALIAS_SETTINGS, d->aliasSettings);
}


// Will handle user configuration of extension in albert settings menu
/** ***************************************************************************/
QWidget *ExtraKdeSettings::Extension::widget(QWidget *parent) {
    if (d->widget.isNull()) {
        d->widget = new ConfigWidget(&(d->kcmServicesMap), parent);

        connect(d->widget, &ConfigWidget::activationUpdated, this, [this](QString &storageId,
                bool activated) {
            d->activatedSettings.insert(storageId, activated);
        });

        connect(d->widget, &ConfigWidget::displayNameUpdated, this, [this](QString &storageId,
                QString &displayName) {
            d->nameSettings.insert(storageId, displayName);
        });

        connect(d->widget, &ConfigWidget::iconNameUpdated, this, [this](QString &storageId, 
                QString &iconName) {
            d->iconSettings.insert(storageId, iconName);
            // Cache new icon for faster searching later
            XDG::IconLookup::iconPath(iconName);
        });

        connect(d->widget, &ConfigWidget::commentUpdated, this, [this](QString &storageId,
                QString &comment) {
            d->commentSettings.insert(storageId, comment);
        });

        connect(d->widget, &ConfigWidget::aliasesUpdated, this, [this](QString &storageId, 
                QStringList &aliases) {
            d->aliasSettings.insert(storageId, aliases);
        });
    }
    return d->widget;
}



/** ***************************************************************************/
void ExtraKdeSettings::Extension::handleQuery(Core::Query * query) const {

    if (query->string().isEmpty())
        return;
    
    const vector<shared_ptr<Core::IndexableItem>> &indexables = d->offlineIndex.search(query->string());

    vector<pair<shared_ptr<Core::Item>,uint>> results;
    for (const shared_ptr<Core::IndexableItem> &item : indexables) {
        std::shared_ptr<KCMService> servicePtr = std::static_pointer_cast<KCMService>(item);
        if(servicePtr->isActivated) {
            results.emplace_back(servicePtr, 1);
        }
    }

    query->addMatches(std::make_move_iterator(results.begin()),
                      std::make_move_iterator(results.end()));
}