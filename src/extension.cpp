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


class ExtraKdeSettings::Private {
    public:
        QPointer<ConfigWidget> widget;
        vector<QString> iconPaths;
        vector<QString> commands;

        vector<unique_ptr<KCMService>> kcmServices;

        ~Private() {
            kcmServices.clear();
        }
        
};


/** ***************************************************************************/
ExtraKdeSettings::Extension::Extension()
    : Core::Extension("org.albert.extension.extrakdesettings"),
      Core::QueryHandler(Core::Plugin::id()),
      d(new Private) {

    registerQueryHandler(this);

    // Lookup kcm modules
    QString queryStr = generateQuery("");

    KService::List services = KServiceTypeTrader::self()->query(QStringLiteral("KCModule"), queryStr);
    for (const KService::Ptr &service : qAsConst(services)) {
        if(service->noDisplay()) {
            continue;
        } else {
            QString iconName = "system-settings";
            if(!service->icon().isEmpty()) {
                iconName = service->icon();
            }

            d->kcmServices.push_back(std::make_unique<KCMService>(service->desktopEntryName(), service->storageId(), service->exec(), 
                    service->name(), service->genericName(), service->comment(), iconName));
        }
    }
}



/** ***************************************************************************/
ExtraKdeSettings::Extension::~Extension() {

}


// Will handle user configuration of extension in albert settings menu
/** ***************************************************************************/
QWidget *ExtraKdeSettings::Extension::widget(QWidget *parent) {
    if (d->widget.isNull()) {
        d->widget = new ConfigWidget(parent);
    }
    return d->widget;
}



/** ***************************************************************************/
void ExtraKdeSettings::Extension::handleQuery(Core::Query * query) const {

    if ( query->string().isEmpty())
        return;

    // Checks for matches
    auto iter = d->kcmServices.begin();
    while(iter != d->kcmServices.end()) {

        QString currentServiceName = (*iter)->name;
        QString currentServiceComment = (*iter)->comment;
        
        // Service name is closer to query
        if(currentServiceName.contains(query->string(), Qt::CaseInsensitive)) {

            auto item = make_shared<StandardItem>(currentServiceName);
            item->setText(currentServiceName);

            if (!(*iter)->genericName.isEmpty() && (*iter)->genericName != currentServiceName) {
                item->setSubtext((*iter)->genericName);
            } else if (!currentServiceComment.isEmpty()) {
                item->setSubtext(currentServiceComment);
            }

            item->setIconPath(XDG::IconLookup::iconPath((*iter)->iconName));
            item->addAction(make_shared<ProcAction>(currentServiceName, QStringList(Core::ShUtil::split((*iter)->exec))));
            query->addMatch(std::move(item), static_cast<uint>(static_cast<float>(query->string().size()) / currentServiceName.size() * UINT_MAX));
        
        // Service comment is closer to query
        } else if(currentServiceComment.contains(query->string(), Qt::CaseInsensitive)) {
            qDebug() << currentServiceComment << "matches";

            auto item = make_shared<StandardItem>(currentServiceComment);
            item->setText(currentServiceComment);

            if (!(*iter)->genericName.isEmpty() && (*iter)->genericName != currentServiceComment) {
                item->setSubtext((*iter)->genericName);
            } else if (!currentServiceComment.isEmpty()) {
                item->setSubtext(currentServiceName);
            }

            item->setIconPath(XDG::IconLookup::iconPath((*iter)->iconName));
            item->addAction(make_shared<ProcAction>(currentServiceComment, QStringList(Core::ShUtil::split((*iter)->exec))));
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

    qDebug() << "Final query : " << finalQuery;
    return finalQuery;
}