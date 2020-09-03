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
#include "moduleeditordialog.h"
using namespace Core;
using namespace std;


class ExtraKdeSettings::Private {
    public:
        QPointer<ConfigWidget> widget;
        vector<QString> iconPaths;
        vector<QString> commands;

        QMap<QString, KCMService*> kcmServicesMap;
        vector<KCMService*> widgetList;

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

            d->kcmServicesMap.insert(service->name(), new KCMService(service->desktopEntryName(), service->storageId(), 
                    service->exec(), service->name(), service->genericName(), service->comment(), iconName));
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

        // Initialize the content and connect the signals

        // Inserts columns
        for(int i = 0; i < 4; i++) {
            int column = d->widget->ui.tableWidget->columnCount();
            d->widget->ui.tableWidget->insertColumn(column);
        }

        QStringList headers = {"Name", "Aliases", "Icon", "Description"};
        d->widget->ui.tableWidget->setHorizontalHeaderLabels(headers);

        auto iter = d->kcmServicesMap.keyBegin();
        while(iter != d->kcmServicesMap.keyEnd()) {
            
            int row = d->widget->ui.tableWidget->rowCount();
            d->widget->ui.tableWidget->insertRow(row);

            auto servicePtr = d->kcmServicesMap.value(*iter);
            d->widgetList.push_back(servicePtr);

            QTableWidgetItem *nameItem = new QTableWidgetItem(*iter);
            nameItem->setCheckState(Qt::Checked);
            nameItem->setFlags(nameItem->flags() & (~Qt::ItemIsEditable));
            d->widget->ui.tableWidget->setItem(row, 0, nameItem);

            QString aliases = "";
            for(QString alias : servicePtr->aliases) {
                if(aliases != "") {
                    aliases += ", ";
                }
                aliases += alias;
            }

            QTableWidgetItem *aliasItem = new QTableWidgetItem(aliases);
            aliasItem->setFlags(aliasItem->flags() & (~Qt::ItemIsEditable));
            d->widget->ui.tableWidget->setItem(row, 1, aliasItem);

            QTableWidgetItem *iconItem = new QTableWidgetItem(servicePtr->iconName);
            iconItem->setFlags(iconItem->flags() & (~Qt::ItemIsEditable));
            d->widget->ui.tableWidget->setItem(row, 2, iconItem);

            QTableWidgetItem *commentItem = new QTableWidgetItem(servicePtr->comment);
            commentItem->setFlags(commentItem->flags() & (~Qt::ItemIsEditable));
            d->widget->ui.tableWidget->setItem(row, 3, commentItem);

            iter++;
        }

        connect(d->widget->ui.pushButton, &QPushButton::clicked, this, [this]() {
            int selectedRow = d->widget->ui.tableWidget->currentRow();
            qDebug() << "Row " << selectedRow << " clicked";

            if(selectedRow != -1) {
                KCMService* selectedService = d->widgetList.at(selectedRow);
                ModuleEditorDialog moduleEditorDialog(selectedService->name, selectedService->iconName, selectedService->aliases);
                moduleEditorDialog.exec();
            }
        });

        // Highlights entire row upon selection
        connect(d->widget->ui.tableWidget, &QTableWidget::cellClicked, this, 
                [this](int row) {
            QTableWidgetSelectionRange selectionRange(row, 0, row, 3);
            d->widget->ui.tableWidget->setRangeSelected(selectionRange, true);
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
        KCMService *servicePtr = d->kcmServicesMap.value(currentServiceName);
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

    qDebug() << "Final query : " << finalQuery;
    return finalQuery;
}