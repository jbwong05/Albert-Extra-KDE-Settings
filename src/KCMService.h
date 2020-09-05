#pragma once
#include <QString>
#include <QStringBuilder>

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
                QString separator = ", ";
                for(QString alias : aliases) {
                    if(aliasString != "") {
                        aliasString = aliasString % separator;
                    }
                    aliasString = aliasString % alias;
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