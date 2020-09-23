#include "KCMService.h"

QString ExtraKdeSettings::KCMService::id() const { return id_; }

void ExtraKdeSettings::KCMService::setId(QString id) { id_ = std::move(id); }

QString ExtraKdeSettings::KCMService::iconPath() const { return iconPath_; }

void ExtraKdeSettings::KCMService::setIconPath(QString iconPath) { iconPath_ = std::move(iconPath); }

QString ExtraKdeSettings::KCMService::text() const { return text_; }

void ExtraKdeSettings::KCMService::setText(QString text) { text_ = std::move(text); }

QString ExtraKdeSettings::KCMService::subtext() const { return subtext_; }

void ExtraKdeSettings::KCMService::setSubtext(QString subtext) { subtext_ = std::move(subtext); }

QString ExtraKdeSettings::KCMService::completion() const { return completion_; }

void ExtraKdeSettings::KCMService::setCompletion(QString completion) { completion_ = std::move(completion); }

Core::Item::Urgency ExtraKdeSettings::KCMService::urgency() const { return urgency_; }

void ExtraKdeSettings::KCMService::setUrgency(Core::Item::Urgency urgency) { urgency_ = urgency; }

std::vector<std::shared_ptr<Core::Action> > ExtraKdeSettings::KCMService::actions() { return actions_; }

void ExtraKdeSettings::KCMService::setActions(std::vector<std::shared_ptr<Core::Action> > actions) { actions_ = std::move(actions); }

void ExtraKdeSettings::KCMService::addAction(std::shared_ptr<Core::Action> action) { actions_.push_back(std::move(action)); }

std::vector<Core::IndexableItem::IndexString> ExtraKdeSettings::KCMService::indexStrings() const { return indexStrings_; }

void ExtraKdeSettings::KCMService::setIndexKeywords(std::vector<Core::IndexableItem::IndexString> indexStrings) { indexStrings_ = std::move(indexStrings); }