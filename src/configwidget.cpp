#include "configwidget.h"
#include "moduleeditordialog.h"

/** ***************************************************************************/
ExtraKdeSettings::ConfigWidget::ConfigWidget(QMap<QString, KCMService*> & kcmServicesMap, QWidget *parent) 
        : QWidget(parent) {

    ui.setupUi(this);
    // Initialize the content and connect the signals

    // Inserts columns
    for(int i = 0; i < 4; i++) {
        int column = ui.tableWidget->columnCount();
        ui.tableWidget->insertColumn(column);
    }

    QStringList headers = {"Name", "Aliases", "Icon", "Description"};
    ui.tableWidget->setHorizontalHeaderLabels(headers);

    auto iter = kcmServicesMap.keyBegin();
    while(iter != kcmServicesMap.keyEnd()) {
            
        int row = ui.tableWidget->rowCount();
        ui.tableWidget->insertRow(row);

        auto servicePtr = kcmServicesMap.value(*iter);
        serviceList.push_back(servicePtr);

        QTableWidgetItem *nameItem = new QTableWidgetItem(*iter);
        nameItem->setCheckState(Qt::Checked);
        nameItem->setFlags(nameItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, 0, nameItem);

        QString aliases = "";
        for(QString alias : servicePtr->aliases) {
            if(aliases != "") {
                aliases += ", ";
            }
            aliases += alias;
        }

        QTableWidgetItem *aliasItem = new QTableWidgetItem(aliases);
        aliasItem->setFlags(aliasItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, 1, aliasItem);

        QTableWidgetItem *iconItem = new QTableWidgetItem(servicePtr->iconName);
        iconItem->setFlags(iconItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, 2, iconItem);

        QTableWidgetItem *commentItem = new QTableWidgetItem(servicePtr->comment);
        commentItem->setFlags(commentItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, 3, commentItem);

        iter++;
    }

    connect(ui.editModuleButton, &QPushButton::clicked, this, &ConfigWidget::onEditModulePress);

    // Highlights entire row upon selection
    connect(ui.tableWidget, &QTableWidget::cellClicked, this, &ConfigWidget::selectRow);
}



/** ***************************************************************************/
ExtraKdeSettings::ConfigWidget::~ConfigWidget() {

}


void ExtraKdeSettings::ConfigWidget::selectRow(int row) {
    QTableWidgetSelectionRange selectionRange(row, 0, row, 3);
    ui.tableWidget->setRangeSelected(selectionRange, true);
}

void ExtraKdeSettings::ConfigWidget::onEditModulePress() {
    int selectedRow = ui.tableWidget->currentRow();

    if(selectedRow != -1) {
        KCMService* selectedService = serviceList.at(selectedRow);
        ModuleEditorDialog moduleEditorDialog(selectedService->name, selectedService->iconName, selectedService->aliases);
        
        if(moduleEditorDialog.exec()) {
                
        }
    }
}