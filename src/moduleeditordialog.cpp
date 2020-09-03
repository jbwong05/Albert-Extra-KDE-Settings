#include "moduleeditordialog.h"
#include <QMessageBox>
#include "aliasadder.h"

/** ***************************************************************************/
ExtraKdeSettings::ModuleEditorDialog::ModuleEditorDialog(KCMService* currentService, QWidget *parent) : 
        QDialog(parent), currentService(currentService) {

    ui.setupUi(this);

    for(QString alias : currentService->aliases) {
        int row = ui.aliasList->count();
        QListWidgetItem *item = new QListWidgetItem(alias);
        ui.aliasList->insertItem(row, item);
    }

    ui.iconField->insert(currentService->iconName);
    ui.nameDisplay->setText(currentService->name);

    connect(ui.addButton, &QPushButton::clicked, this, &ModuleEditorDialog::onAddButtonPress);
    connect(ui.removeButton, &QPushButton::clicked, this, &ModuleEditorDialog::onRemoveButtonPress);
}


/** ***************************************************************************/
ExtraKdeSettings::ModuleEditorDialog::~ModuleEditorDialog() {
    int numItems = ui.aliasList->count();
    while(numItems > 0) {
        QListWidgetItem* currentItem = ui.aliasList->takeItem(0);
        delete currentItem;
        numItems = ui.aliasList->count();
    }
}


/** ***************************************************************************/
void ExtraKdeSettings::ModuleEditorDialog::accept() {
    if (ui.iconField->text().trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Empty Icon Field"), tr("The icon field must not be empty."));
    } else {
        currentService->iconName = ui.iconField->text();
        QDialog::accept();
    }
}

void ExtraKdeSettings::ModuleEditorDialog::onAddButtonPress() {
    QString newAlias;
    AliasAdder aliasAdder(&newAlias, &(currentService->aliases));

    if(aliasAdder.exec()) {
        ui.aliasList->addItem(newAlias);
    }
}

void ExtraKdeSettings::ModuleEditorDialog::onRemoveButtonPress() {
    int selectedRow = ui.aliasList->currentRow();

    if(selectedRow != -1) {
        QListWidgetItem* item = ui.aliasList->takeItem(selectedRow);
        currentService->aliases.remove(item->text());
        delete item;
    }
}