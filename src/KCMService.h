#pragma once
#include <QString>

namespace ExtraKdeSettings {
    
    class KCMService {
        public:
            KCMService(QString desktopEntryName, QString storageId, QString exec, 
                    QString name, QString genericName, QString comment, QString iconName) : 
                    desktopEntryName(desktopEntryName), storageId(storageId), exec(exec),
                    name(name), genericName(genericName), comment(comment), iconName(iconName) {};
            ~KCMService() {};

            QString getAliasString() {
                QString aliasString = "";
                for(QString alias : aliases) {
                    if(aliasString != "") {
                        aliasString += ", ";
                    }
                    aliasString += alias;
                }
                return aliasString;
            }

            QString desktopEntryName;
            QString storageId;
            QString exec;
            QString name;
            QString genericName;
            QString comment;
            QString iconName;
            QSet<QString> aliases;
    };
}