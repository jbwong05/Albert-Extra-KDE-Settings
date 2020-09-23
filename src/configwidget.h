#pragma once
#include <QWidget>
#include "ui_configwidget.h"
#include "KCMService.h"
#include <vector>

namespace ExtraKdeSettings {
    class ConfigWidget final : public QWidget {
        Q_OBJECT
        public:
            explicit ConfigWidget(QMap<QString, std::shared_ptr<KCMService>> *kcmServicesMap, QWidget *parent = nullptr);
            ~ConfigWidget();
            Ui::ConfigWidget ui;

        signals:
            void activationUpdated(QString &storageId, bool activated);
            void displayNameUpdated(QString &storageId, QString &displayName);
            void iconNameUpdated(QString &storageId, QString &iconName);
            void commentUpdated(QString &storageId, QString &comment);
            void aliasesUpdated(QString &storageId, QStringList &aliases);
        
        private:
            QMap<QString, std::shared_ptr<KCMService>> *kcmServicesMap;
            std::vector<std::shared_ptr<KCMService>> serviceList;

            void selectRow(int row);
            void toggleServiceActivation(int row, int column);
            void onEditModulePress();

            QMetaObject::Connection cellChangeConnection;
    };
}
