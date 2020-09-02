// Copyright (C) 2014-2018 Manuel Schneider

#include "moduleeditordialog.h"
#include <QMessageBox>
#include <vector>

/** ***************************************************************************/
ExtraKdeSettings::ModuleEditorDialog::ModuleEditorDialog(QString name, QString iconName, 
        QSet<QString> &theAliases, QWidget *parent) : 
        QDialog(parent), name(name), iconName(iconName) {
    ui.setupUi(this);

    for(QString alias : theAliases) {
        aliases.insert(alias);
        int row = ui.aliasList->count();
        QListWidgetItem *item = new QListWidgetItem(alias);
        ui.aliasList->insertItem(row, item);
    }

    ui.iconField->insert(iconName);
    ui.nameDisplay->setText(name);
}


/** ***************************************************************************/
ExtraKdeSettings::ModuleEditorDialog::~ModuleEditorDialog() {

}


/** ***************************************************************************/
void ExtraKdeSettings::ModuleEditorDialog::accept()
{
    /*if (ui.lineEdit->text().trimmed().isEmpty())
        QMessageBox::information(this, tr("Empty title."),
                                 tr("The title field must not be empty."));
    else
        QDialog::accept();*/
}
