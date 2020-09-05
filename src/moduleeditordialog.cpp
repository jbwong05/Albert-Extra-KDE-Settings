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

    ui.moduleDisplayLabel->setText(currentService->exec.split(' ')[1]);
    ui.displayNameField->setText(currentService->name);
    ui.iconField->setText(currentService->iconName);
    ui.commentBox->setText(currentService->comment);

    connect(ui.addButton, &QPushButton::clicked, this, &ModuleEditorDialog::onAddButtonPress);
    connect(ui.removeButton, &QPushButton::clicked, this, &ModuleEditorDialog::onRemoveButtonPress);
    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &ModuleEditorDialog::accept);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, [this]() {
        QDialog::reject();
    });
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
    if(ui.displayNameField->text().trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Empty Display Name Field"), tr("The display name field must not be empty."));
    } else if (ui.iconField->text().trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Empty Icon Field"), tr("The icon field must not be empty."));
    } else {
        currentService->name = ui.displayNameField->text();
        currentService->iconName = ui.iconField->text();
        currentService->comment = ui.commentBox->toPlainText();

        currentService->aliases.clear();
        
        QListWidgetItem* currentItem = ui.aliasList->takeItem(0);
        while(currentItem) {
            currentService->aliases.append(currentItem->text());

            delete currentItem;
            currentItem = ui.aliasList->takeItem(0);
        }

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
        currentService->aliases.removeOne(item->text());
        delete item;
    }
}