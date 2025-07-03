#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QMap>

class ConfigManager : public QObject {
    Q_OBJECT
public:
    explicit ConfigManager(QObject* parent = nullptr);
    void loadSettings();
    QMap<QString, QVariant> getDefaultSettings() const;
    void saveSettings(const QMap<QString, QVariant>& settings = {});
    QVariant getSetting(const QString& key, const QVariant& default_value = QVariant()) const;
    void saveSetting(const QString& key, const QVariant& value);

private:
    QString config_dir_ = "data";
    QString config_file_ = "data/config.json";
    QMap<QString, QVariant> settings_;
};

#endif // CONFIGMANAGER_H