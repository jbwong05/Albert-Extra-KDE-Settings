#pragma once
#include <QString>

namespace ExtraKdeSettings {
    
    class KCMService {
        public:
            KCMService(bool isActivated, QString storageId, QString exec, QString name, 
                    QString comment, QString iconName, QStringList aliases) : 
                    isActivated(isActivated), storageId(storageId), exec(exec), name(name), 
                    comment(comment), iconName(iconName), aliases(aliases) {};
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

            bool isActivated;
            QString storageId;
            QString exec;
            QString name;
            QString comment;
            QString iconName;
            QStringList aliases;
    };
}