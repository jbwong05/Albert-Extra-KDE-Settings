// Copyright (C) 2014-2018 Manuel Schneider

#pragma once
#include "ui_aliasadder.h"
#include <QString>

namespace ExtraKdeSettings {

    class AliasAdder final : public QDialog {
        Q_OBJECT

        public:

            AliasAdder(QString *enteredAlias, QStringList *existingAliases, QWidget *parent = nullptr);
            ~AliasAdder();
            Ui::AliasAdder ui;

        protected:

            void accept() override;
        
        private:
            QString *enteredAlias;
            QStringList *existingAliases;
    };
}
