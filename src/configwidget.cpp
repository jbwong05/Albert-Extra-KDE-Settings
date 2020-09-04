#include "configwidget.h"
#include "moduleeditordialog.h"
#include <QTableWidgetItem>
#include <QDebug>

#define NUM_COLS 4
#define NAME_COL_INDEX 0
#define ALIAS_COL_INDEX 1
#define ICON_COL_INDEX 2
#define COMMENT_COL_INDEX 3

/** ***************************************************************************/
ExtraKdeSettings::ConfigWidget::ConfigWidget(QMap<QString, KCMService*> & kcmServicesMap, QWidget *parent) 
        : QWidget(parent) {

    ui.setupUi(this);
    // Initialize the content and connect the signals

    // Inserts columns
    for(int i = 0; i < NUM_COLS; i++) {
        int column = ui.tableWidget->columnCount();
        ui.tableWidget->insertColumn(column);
    }

    QStringList headers = {"Display Name", "Aliases", "Icon", "Description"};
    ui.tableWidget->setHorizontalHeaderLabels(headers);

    auto iter = kcmServicesMap.keyBegin();
    while(iter != kcmServicesMap.keyEnd()) {
            
        int row = ui.tableWidget->rowCount();
        ui.tableWidget->insertRow(row);

        KCMService* servicePtr = kcmServicesMap.value(*iter);
        serviceList.push_back(servicePtr);

        QTableWidgetItem *nameItem = new QTableWidgetItem(*iter);
        Qt::CheckState checkState = servicePtr->isActivated ? Qt::Checked : Qt::Unchecked;
        nameItem->setCheckState(checkState);
        nameItem->setFlags(nameItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, NAME_COL_INDEX, nameItem);

        QString aliases = servicePtr->getAliasString();

        QTableWidgetItem *aliasItem = new QTableWidgetItem(aliases);
        aliasItem->setFlags(aliasItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, ALIAS_COL_INDEX, aliasItem);

        QTableWidgetItem *iconItem = new QTableWidgetItem(servicePtr->iconName);
        iconItem->setFlags(iconItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, ICON_COL_INDEX, iconItem);

        QTableWidgetItem *commentItem = new QTableWidgetItem(servicePtr->comment);
        commentItem->setFlags(commentItem->flags() & (~Qt::ItemIsEditable));
        ui.tableWidget->setItem(row, COMMENT_COL_INDEX, commentItem);

        iter++;
    }

    cellChangeConnection = connect(ui.tableWidget, &QTableWidget::cellChanged, this, &ConfigWidget::toggleServiceActivation);

    connect(ui.editModuleButton, &QPushButton::clicked, this, &ConfigWidget::onEditModulePress);

    // Highlights entire row upon selection
    connect(ui.tableWidget, &QTableWidget::cellClicked, this, &ConfigWidget::selectRow);
}



/** ***************************************************************************/
ExtraKdeSettings::ConfigWidget::~ConfigWidget() {
    // Disconnect cell change connection first to avoid triggers
    disconnect(cellChangeConnection);

    for(int row = serviceList.size() - 1; row >= 0; row--) {
        for(int col = NUM_COLS - 1; col >= 0; col--) {
            QTableWidgetItem* currentItem = ui.tableWidget->takeItem(row, col);
            delete currentItem;
        }
    }
}


void ExtraKdeSettings::ConfigWidget::selectRow(int row) {
    QTableWidgetSelectionRange selectionRange(row, 0, row, NUM_COLS - 1);
    ui.tableWidget->setRangeSelected(selectionRange, true);
}

void ExtraKdeSettings::ConfigWidget::toggleServiceActivation(int row, int column) {
    if(column == 0) {
        qDebug() << row << " " << column << " toggled";
        KCMService* selectedService = serviceList.at(row);
        QTableWidgetItem* nameItem = ui.tableWidget->item(row, NAME_COL_INDEX);
        selectedService->isActivated = nameItem->checkState() == Qt::Checked;

        emit activationUpdated(selectedService->storageId, selectedService->isActivated);
    }
}

void ExtraKdeSettings::ConfigWidget::onEditModulePress() {
    int selectedRow = ui.tableWidget->currentRow();

    if(selectedRow != -1) {
        KCMService* selectedService = serviceList.at(selectedRow);
        ModuleEditorDialog moduleEditorDialog(selectedService);
        
        if(moduleEditorDialog.exec()) {
            // Update display name
            QTableWidgetItem* nameItem = ui.tableWidget->item(selectedRow, NAME_COL_INDEX);
            nameItem->setText(selectedService->name);

            // Update icon name
            QTableWidgetItem* iconItem = ui.tableWidget->item(selectedRow, ICON_COL_INDEX);
            iconItem->setText(selectedService->iconName);

            // Update comment
            QTableWidgetItem* commentItem = ui.tableWidget->item(selectedRow, COMMENT_COL_INDEX);
            commentItem->setText(selectedService->comment);

            // Update aliases
            QTableWidgetItem* aliasItem = ui.tableWidget->item(selectedRow, ALIAS_COL_INDEX);
            aliasItem->setText(selectedService->getAliasString());

            // Send update signals to main extension object
            emit displayNameUpdated(selectedService->storageId, selectedService->name);
            emit iconNameUpdated(selectedService->storageId, selectedService->iconName);
            emit commentUpdated(selectedService->storageId, selectedService->comment);
            emit aliasesUpdated(selectedService->storageId, selectedService->aliases);
        }
    }
}