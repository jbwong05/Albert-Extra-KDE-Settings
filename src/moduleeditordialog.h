#pragma once
#include "ui_moduleeditordialog.h"
#include <QDialog>

namespace ExtraKdeSettings {

    class ModuleEditorDialog final : public QDialog {
        Q_OBJECT

        public:

            ModuleEditorDialog(QString name, QString iconName, QSet<QString> &aliases, 
                    QWidget *parent = nullptr);
            ~ModuleEditorDialog();
            Ui::ModuleEditorDialog ui;

        protected:

            void accept() override;
        
        private:
            QString name;
            QString iconName;
            QSet<QString> aliases;
    };
}