#pragma once
#include <QString>
#include <QStringBuilder>
#include <QStringList>
#include "albert/indexable.h"

namespace ExtraKdeSettings {
    
    class KCMService : public Core::IndexableItem {
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

            QString id() const override;
            void setId(QString id);

            QString iconPath() const override;
            void setIconPath(QString iconPath);

            QString text() const override;
            void setText(QString text);

            QString subtext() const override;
            void setSubtext(QString subtext);

            QString completion() const override;
            void setCompletion(QString completion);

            Item::Urgency urgency() const override;
            void setUrgency(Item::Urgency urgency);

            std::vector<std::shared_ptr<Core::Action>> actions() override;
            void setActions(std::vector<std::shared_ptr<Core::Action>> actions);
            void addAction(std::shared_ptr<Core::Action> action);

            virtual std::vector<Core::IndexableItem::IndexString> indexStrings() const override;
            void setIndexKeywords(std::vector<Core::IndexableItem::IndexString> indexStrings);

            bool isActivated;
            QString storageId;
            QString exec;
            QString name;
            QString comment;
            QString iconName;
            QStringList aliases;

        protected:
            QString id_;
            QString iconPath_;
            QString text_;
            QString subtext_;
            QString completion_;
            Item::Urgency urgency_;
            std::vector<std::shared_ptr<Core::Action>> actions_;
            std::vector<IndexableItem::IndexString> indexStrings_;
    };
}