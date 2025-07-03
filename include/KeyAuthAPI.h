#ifndef KEYAUTHAPI_H
#define KEYAUTHAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QMap>

class KeyAuthAPI : public QObject {
    Q_OBJECT
public:
    explicit KeyAuthAPI(const QString& name, const QString& ownerid, const QString& version, const QString& hash_to_check, QObject* parent = nullptr);
    bool init();
    bool login(const QString& username, const QString& password);
    bool registerUser(const QString& username, const QString& password, const QString& license_key);
    bool license(const QString& license_key);
    QMap<QString, QVariant> getUserInfo() const;

signals:
    void requestFinished(bool success, const QJsonDocument& response);

private:
    bool makeRequest(const QMap<QString, QString>& data, QJsonDocument& response);
    QString name_;
    QString ownerid_;
    QString version_;
    QString hash_to_check_;
    QString sessionid_;
    bool initialized_ = false;
    bool logged_in_ = false;
    QMap<QString, QVariant> user_data_;
    QString base_url_ = "https://keyauth.win/api/1.2/";
    QNetworkAccessManager* network_manager_;
};

#endif // KEYAUTHAPI_H