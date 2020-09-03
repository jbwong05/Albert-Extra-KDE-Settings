#pragma once
#include "ui_moduleeditordialog.h"
#include <QDialog>
#include "KCMService.h"

namespace ExtraKdeSettings {

    class ModuleEditorDialog final : public QDialog {
        Q_OBJECT

        public:

            ModuleEditorDialog(KCMService* currentService, QWidget *parent = nullptr);
            ~ModuleEditorDialog();
            Ui::ModuleEditorDialog ui;

        protected:

            void accept() override;
        
        private:
            KCMService* currentService;

            void onAddButtonPress();
            void onRemoveButtonPress();
    };
}