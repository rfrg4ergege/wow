#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QWidget>
#include <QJsonDocument>
#include <QMap>

class ProfileManager : public QWidget {
    Q_OBJECT
public:
    explicit ProfileManager(QWidget* parent = nullptr);
    void createSampleProfiles();
    void saveProfile(const QString& name, const QMap<QString, QVariant>& data);
    QMap<QString, QVariant> loadProfile(const QString& name);
    QStringList getProfileList() const;

signals:
    void profileLoaded(const QMap<QString, QVariant>& profile);

private:
    QString profiles_dir_ = "data/profiles";
    QString current_profile_;
};

#endif // PROFILEMANAGER_H