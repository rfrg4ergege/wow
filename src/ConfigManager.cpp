#include "ConfigManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ConfigManager::ConfigManager(QObject* parent) : QObject(parent) {
    QDir().mkpath(config_dir_);
    loadSettings();
}

void ConfigManager::loadSettings() {
    QFile file(config_file_);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        settings_ = doc.object().toVariantMap();
        file.close();
    } else {
        settings_ = getDefaultSettings();
    }
}

QMap<QString, QVariant> ConfigManager::getDefaultSettings() const {
    return {
        {"theme", "dark"},
        {"client_id", "1385804140697096233"},
        {"autosave", true},
        {"auto_update", false},
        {"auto_update_interval", 10},
        {"window_geometry", QVariant()},
        {"window_state", QVariant()},
        {"last_profile", QVariant()},
        {"system_tray", true}
    };
}

void ConfigManager::saveSettings(const QMap<QString, QVariant>& settings) {
    settings_.insert(settings);
    QFile file(config_file_);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(QJsonObject::fromVariantMap(settings_));
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qDebug() << "Error saving settings to" << config_file_;
    }
}

QVariant ConfigManager::getSetting(const QString& key, const QVariant& default_value) const {
    return settings_.value(key, default_value);
}

void ConfigManager::saveSetting(const QString& key, const QVariant& value) {
    settings_[key] = value;
    saveSettings();
}