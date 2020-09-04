#pragma once
#include <QWidget>
#include "ui_configwidget.h"
#include "KCMService.h"
#include <vector>

namespace ExtraKdeSettings {
    class ConfigWidget final : public QWidget {
        Q_OBJECT
        public:
            explicit ConfigWidget(QMap<QString, KCMService*> &kcmServicesMap, QWidget *parent = nullptr);
            ~ConfigWidget();
            Ui::ConfigWidget ui;

        signals:
            void iconNameUpdated(QString &storageId, QString &iconName);
            void aliasesUpdated(QString &storageId, QStringList &aliases);
        
        private:
            std::vector<KCMService*> serviceList;

            void selectRow(int row);
            void onEditModulePress();
    };
}
