#include "aliasadder.h"
#include<QMessageBox>

/** ***************************************************************************/
ExtraKdeSettings::AliasAdder::AliasAdder(QString *enteredAlias, QSet<QString> *existingAliases, QWidget *parent) : 
    QDialog(parent), enteredAlias(enteredAlias), existingAliases(existingAliases) {
    ui.setupUi(this);
}


/** ***************************************************************************/
ExtraKdeSettings::AliasAdder::~AliasAdder() {

}


/** ***************************************************************************/
void ExtraKdeSettings::AliasAdder::accept()
{
    if (ui.lineEdit->text().trimmed().isEmpty()) {
        QMessageBox::information(this, tr("No alias entered"), tr("The alias field must not be empty"));
    } else if(existingAliases->contains(ui.lineEdit->text())) {
        QMessageBox::information(this, tr("Alias already exists"), tr("The entered alias already exists"));
    } else {
        *enteredAlias = ui.lineEdit->text();
        existingAliases->insert(ui.lineEdit->text());
        QDialog::accept();
    }
}
