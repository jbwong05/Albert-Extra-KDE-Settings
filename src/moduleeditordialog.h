#pragma once
#include "ui_moduleeditordialog.h"
#include <QDialog>
#include "KCMService.h"

namespace ExtraKdeSettings {

    class ModuleEditorDialog final : public QDialog {
        Q_OBJECT

        public:

            ModuleEditorDialog(std::shared_ptr<KCMService> currentService, QWidget *parent = nullptr);
            ~ModuleEditorDialog();
            Ui::ModuleEditorDialog ui;

        protected:

            void accept() override;
        
        private:
            std::shared_ptr<KCMService> currentService;

            void onAddButtonPress();
            void onRemoveButtonPress();
    };
}