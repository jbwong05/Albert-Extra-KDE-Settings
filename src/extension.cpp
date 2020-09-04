#include <QDebug>
#include <QPointer>
#include <QRegularExpression>
#include <KService>
#include <KServiceTypeTrader>
#include "albert/util/standarditem.h"
#include "xdg/iconlookup.h"
#include "albert/util/shutil.h"
#include "albert/util/standardactions.h"
#include "albert/util/standarditem.h"
#include "configwidget.h"
#include "extension.h"
#include "KCMService.h"
using namespace Core;
using namespace std;

#define NAME_SETTINGS ALBERT_EXTENSION_IID"-names"
#define ICON_SETTINGS ALBERT_EXTENSION_IID"-icon_names"
#define ALIAS_SETTINGS ALBERT_EXTENSION_IID"-aliases"

class ExtraKdeSettings::Private {
    public:
        QPointer<ConfigWidget> widget;

        QMap<QString, KCMService*> kcmServicesMap;
        vector<KCMService*> widgetList;

        QMap<QString, QVariant> nameSettings;
        QMap<QString, QVariant> iconSettings;
        QMap<QString, QVariant> aliasSettings;

        ~Private() {
            auto iter = kcmServicesMap.keyBegin();
            while(iter != kcmServicesMap.keyEnd()) {
                delete kcmServicesMap.value(*iter);
                iter++;
            }
            kcmServicesMap.clear();
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
    d->nameSettings = settings().value(NAME_SETTINGS).toMap();
    d->iconSettings = settings().value(ICON_SETTINGS).toMap();
    d->aliasSettings = settings().value(ALIAS_SETTINGS).toMap();
    
    // Lookup kcm modules
    QString queryStr = generateQuery("");

    KService::List services = KServiceTypeTrader::self()->query(QStringLiteral("KCModule"), queryStr);
    for (const KService::Ptr &service : qAsConst(services)) {
        if(service->noDisplay()) {
            continue;
        } else {
            QString serviceStorgaeId = service->storageId();

            QString serviceName = d->nameSettings.contains(serviceStorgaeId) ? d->nameSettings.value(serviceStorgaeId).toString() : service->name();
            
            QString serviceIcon;
            if(d->iconSettings.contains(serviceStorgaeId)) {
                serviceIcon = d->iconSettings.value(serviceStorgaeId).toString();
            } else if(!service->icon().isEmpty()) {
                serviceIcon = service->icon();
            } else {
                serviceIcon = "system-settings";
            }

            QStringList serviceAliases;
            if(d->aliasSettings.contains(serviceStorgaeId)) {
                serviceAliases = d->aliasSettings.value(serviceStorgaeId).toStringList();
            }

            d->kcmServicesMap.insert(serviceName, new KCMService(service->desktopEntryName(), serviceStorgaeId, 
                    service->exec(), serviceName, service->genericName(), service->comment(), serviceIcon, serviceAliases));
        }
    }
}



/** ***************************************************************************/
ExtraKdeSettings::Extension::~Extension() {
    settings().setValue(ICON_SETTINGS, d->iconSettings);
    settings().setValue(ALIAS_SETTINGS, d->aliasSettings);
}


// Will handle user configuration of extension in albert settings menu
/** ***************************************************************************/
QWidget *ExtraKdeSettings::Extension::widget(QWidget *parent) {
    if (d->widget.isNull()) {
        d->widget = new ConfigWidget(d->kcmServicesMap, parent);

        connect(d->widget, &ConfigWidget::iconNameUpdated, this, [this](QString &storageId, 
                QString &iconName) {
            d->iconSettings.insert(storageId, iconName);
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

    // Checks for matches
    auto iter = d->kcmServicesMap.keyBegin();
    while(iter != d->kcmServicesMap.keyEnd()) {

        QString currentServiceName = *iter;
        KCMService* servicePtr = d->kcmServicesMap.value(currentServiceName);
        QString currentServiceComment = servicePtr->comment;;
        
        // Service name is closer to query
        if(currentServiceName.contains(query->string(), Qt::CaseInsensitive)) {

            auto item = make_shared<StandardItem>(currentServiceName);
            item->setText(currentServiceName);

            if (!servicePtr->genericName.isEmpty() && servicePtr->genericName != currentServiceName) {
                item->setSubtext(servicePtr->genericName);
            } else if (!currentServiceComment.isEmpty()) {
                item->setSubtext(currentServiceComment);
            }

            item->setIconPath(XDG::IconLookup::iconPath(servicePtr->iconName));
            item->addAction(make_shared<ProcAction>(currentServiceName, QStringList(Core::ShUtil::split(servicePtr->exec))));
            query->addMatch(std::move(item), static_cast<uint>(static_cast<float>(query->string().size()) / currentServiceName.size() * UINT_MAX));
        
        // Service comment is closer to query
        } else if(currentServiceComment.contains(query->string(), Qt::CaseInsensitive)) {

            auto item = make_shared<StandardItem>(currentServiceComment);
            item->setText(currentServiceComment);

            if (!servicePtr->genericName.isEmpty() && servicePtr->genericName != currentServiceComment) {
                item->setSubtext(servicePtr->genericName);
            } else if (!currentServiceComment.isEmpty()) {
                item->setSubtext(currentServiceName);
            }

            item->setIconPath(XDG::IconLookup::iconPath(servicePtr->iconName));
            item->addAction(make_shared<ProcAction>(currentServiceComment, QStringList(Core::ShUtil::split(servicePtr->exec))));
            query->addMatch(std::move(item), static_cast<uint>(static_cast<float>(query->string().size()) / currentServiceName.size() * UINT_MAX));
        }
        iter++;
    }
}

QString ExtraKdeSettings::Extension::generateQuery(const QString &str) const {
    QString genericNameTemplate = QStringLiteral("exist GenericName");
    QString nameTemplate = QStringLiteral("exist Name");
    QString commentTemplate = QStringLiteral("exist Comment");

    // Search for applications which are executable and the term case-insensitive matches any of
    // * a substring of one of the keywords
    // * a substring of the GenericName field
    // * a substring of the Name field
    // Note that before asking for the content of e.g. Keywords and GenericName we need to ask if
    // they exist to prevent a tree evaluation error if they are not defined.

    genericNameTemplate += QStringLiteral(" and '%1' ~~ GenericName").arg(str);
    nameTemplate += QStringLiteral(" and '%1' ~~ Name").arg(str);
    commentTemplate += QStringLiteral(" and '%1' ~~ Comment").arg(str);

    QString finalQuery = QStringLiteral("exist Exec and ( (%1) or (%2) or ('%3' ~~ Exec) or (%4) )")
        .arg(genericNameTemplate, nameTemplate, str, commentTemplate);

    return finalQuery;
}