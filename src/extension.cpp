#include <QDebug>
#include <QPointer>
#include <QRegularExpression>
#include <KService>
#include <KServiceTypeTrader>
#include "albert/util/standarditem.h"
#include "xdg/iconlookup.h"
#include "albert/util/standardactions.h"
#include "albert/util/standarditem.h"
#include "configwidget.h"
#include "extension.h"
#include "KCMService.h"
using namespace Core;
using namespace std;

#define ACTIVATED_SETTINGS "activated"
#define NAME_SETTINGS "names"
#define ICON_SETTINGS "icon_names"
#define COMMENT_SETTINGS "comments"
#define ALIAS_SETTINGS "aliases"

#define FALLBACK_ICON "preferences-system"

class ExtraKdeSettings::Private {
    public:
        QPointer<ConfigWidget> widget;

        QMap<QString, KCMService*> kcmServicesMap;
        QMap<KCMService*, shared_ptr<Core::StandardItem>> items;
        vector<KCMService*> widgetList;

        QMap<QString, QVariant> activatedSettings;
        QMap<QString, QVariant> nameSettings;
        QMap<QString, QVariant> iconSettings;
        QMap<QString, QVariant> commentSettings;
        QMap<QString, QVariant> aliasSettings;

        ~Private() {
            auto iter = kcmServicesMap.keyBegin();
            while(iter != kcmServicesMap.keyEnd()) {
                delete kcmServicesMap.value(*iter);
                iter++;
            }
            kcmServicesMap.clear();
            items.clear();
            widgetList.clear();
        }
        
};


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
    
    // Lookup kcm modules
    QString queryStr = generateQuery("");

    KService::List services = KServiceTypeTrader::self()->query(QStringLiteral("KCModule"), queryStr);
    for (const KService::Ptr &service : qAsConst(services)) {
        if(service->noDisplay()) {
            continue;
        } else {
            QString serviceStorgaeId = service->storageId();

            bool isActivated = d->activatedSettings.contains(serviceStorgaeId) ? d->activatedSettings.value(serviceStorgaeId).toBool() : true;
            QString serviceName = d->nameSettings.contains(serviceStorgaeId) ? d->nameSettings.value(serviceStorgaeId).toString() : service->name();
            
            QString serviceIcon;
            if(d->iconSettings.contains(serviceStorgaeId)) {
                serviceIcon = d->iconSettings.value(serviceStorgaeId).toString();
            } else if(!service->icon().isEmpty()) {
                serviceIcon = service->icon();
            } else {
                serviceIcon = FALLBACK_ICON;
            }

            // Cache the icon path during initialization for faster searching later
            QString iconPath = XDG::IconLookup::iconPath(serviceIcon);

            QString serviceComment = d->commentSettings.contains(serviceStorgaeId) ? d->commentSettings.value(serviceStorgaeId).toString() : service->comment();

            QStringList serviceAliases;
            if(d->aliasSettings.contains(serviceStorgaeId)) {
                serviceAliases = d->aliasSettings.value(serviceStorgaeId).toStringList();
            }

            auto item = make_shared<StandardItem>(serviceName);
            item->setText(serviceName);
            item->setSubtext(serviceComment);
            item->setIconPath(iconPath);
            item->addAction(make_shared<ProcAction>(serviceName, service->exec().split(" ")));

            KCMService *currentService = new KCMService(isActivated, serviceStorgaeId, service->exec(), 
                    serviceName, serviceComment, serviceIcon, serviceAliases);

            d->kcmServicesMap.insert(serviceName, currentService);
            d->items.insert(currentService, item);
        }
    }
}



/** ***************************************************************************/
ExtraKdeSettings::Extension::~Extension() {
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

    if ( query->string().isEmpty())
        return;
    
    vector<pair<shared_ptr<Core::StandardItem>, uint>> matches;

    // Checks for matches
    auto iter = d->kcmServicesMap.keyBegin();
    while(iter != d->kcmServicesMap.keyEnd()) {

        QString currentServiceName = *iter;
        KCMService* servicePtr = d->kcmServicesMap.value(currentServiceName);
        if(servicePtr->isActivated) {
            QString currentServiceComment = servicePtr->comment;

            bool matchFound = false;
            int score = 0;

            // Check service name
            if(currentServiceName.contains(query->string(), Qt::CaseInsensitive)) {
                score = static_cast<uint>(static_cast<float>(query->string().size()) / currentServiceName.size() * UINT_MAX);
                matchFound = true;

            } else {

                // Check aliases
                auto iter = servicePtr->aliases.begin();
                while(!matchFound && iter != servicePtr->aliases.end()) {
                    if((*iter).contains(query->string())) {
                        score = static_cast<uint>(static_cast<float>(query->string().size()) / (*iter).size() * UINT_MAX);
                        matchFound = true;
                    }
                    iter++;
                }

                if(!matchFound) {
                    // Check service comment
                    QStringList wordsInComment = QStringList(servicePtr->comment.split(" "));
                    auto iter = wordsInComment.begin();
                    while(!matchFound && iter != wordsInComment.end()) {
                        if((*iter).contains(query->string()), Qt::CaseInsensitive) {
                            score = static_cast<uint>(static_cast<float>(query->string().size()) / (*iter).size() * UINT_MAX);
                            matchFound = true;
                        }
                        iter++;
                    }
                }
            }

            if(matchFound) {
                matches.emplace_back(static_pointer_cast<Core::StandardItem>(d->items[servicePtr]), score);
            }
        }

        iter++;
    }

    query->addMatches(matches.begin(), matches.end());
}

QString ExtraKdeSettings::Extension::generateQuery(const QString &str) const {
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