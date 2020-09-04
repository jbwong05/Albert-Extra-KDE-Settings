#pragma once
#include <QString>

namespace ExtraKdeSettings {
    
    class KCMService {
        public:
            KCMService(QString storageId, QString exec, QString name, QString comment, 
                    QString iconName, QStringList aliases) : 
                    storageId(storageId), exec(exec), name(name), comment(comment), 
                    iconName(iconName), aliases(aliases) {};
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

            QString storageId;
            QString exec;
            QString name;
            QString comment;
            QString iconName;
            QStringList aliases;
    };
}