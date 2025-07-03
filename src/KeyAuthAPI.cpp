#include "KeyAuthAPI.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QDebug>

KeyAuthAPI::KeyAuthAPI(const QString& name, const QString& ownerid, const QString& version, const QString& hash_to_check, QObject* parent)
    : QObject(parent), name_(name), ownerid_(ownerid), version_(version), hash_to_check_(hash_to_check) {
    network_manager_ = new QNetworkAccessManager(this);
}

bool KeyAuthAPI::init() {
    QMap<QString, QString> init_data = {
        {"type", "init"},
        {"ver", version_},
        {"hash", hash_to_check_},
        {"enckey", name_},
        {"name", name_},
        {"ownerid", ownerid_}
    };
    QJsonDocument response;
    if (makeRequest(init_data, response)) {
        QJsonObject obj = response.object();
        if (obj["success"].toBool()) {
            sessionid_ = obj["sessionid"].toString();
            initialized_ = true;
            return true;
        }
    }
    return false;
}

bool KeyAuthAPI::login(const QString& username, const QString& password) {
    if (!initialized_) return false;
    QMap<QString, QString> login_data = {
        {"type", "login"},
        {"username", username},
        {"pass", password},
        {"sessionid", sessionid_},
        {"name", name_},
        {"ownerid", ownerid_}
    };
    QJsonDocument response;
    if (makeRequest(login_data, response)) {
        QJsonObject obj = response.object();
        if (obj["success"].toBool()) {
            logged_in_ = true;
            user_data_ = obj["info"].toObject().toVariantMap();
            return true;
        }
    }
    return false;
}

bool KeyAuthAPI::registerUser(const QString& username, const QString& password, const QString& license_key) {
    if (!initialized_) return false;
    QMap<QString, QString> register_data = {
        {"type", "register"},
        {"username", username},
        {"pass", password},
        {"key", license_key},
        {"sessionid", sessionid_},
        {"name", name_},
        {"ownerid", ownerid_}
    };
    QJsonDocument response;
    if (makeRequest(register_data, response)) {
        QJsonObject obj = response.object();
        if (obj["success"].toBool()) {
            logged_in_ = true;
            user_data_ = obj["info"].toObject().toVariantMap();
            return true;
        }
    }
    return false;
}

bool KeyAuthAPI::license(const QString& license_key) {
    if (!initialized_) return false;
    QMap<QString, QString> license_data = {
        {"type", "license"},
        {"key", license_key},
        {"sessionid", sessionid_},
        {"name", name_},
        {"ownerid", ownerid_}
    };
    QJsonDocument response;
    if (makeRequest(license_data, response)) {
        QJsonObject obj = response.object();
        if (obj["success"].toBool()) {
            logged_in_ = true;
            user_data_ = obj["info"].toObject().toVariantMap();
            return true;
        }
    }
    return false;
}

bool KeyAuthAPI::makeRequest(const QMap<QString, QString>& data, QJsonDocument& response) {
    QNetworkRequest request(QUrl(base_url_));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", QString("KeyAuth-C++/%1").arg(name_).toUtf8());

    QUrlQuery query;
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        query.addQueryItem(it.key(), it.value());
    }
    QByteArray post_data = query.toString(QUrl::FullyEncoded).toUtf8();

    QNetworkReply* reply = network_manager_->post(request, post_data);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        response = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();
        emit requestFinished(true, response);
        return true;
    }
    reply->deleteLater();
    emit requestFinished(false, QJsonDocument());
    return false;
}

QMap<QString, QVariant> KeyAuthAPI::getUserInfo() const {
    return logged_in_ ? user_data_ : QMap<QString, QVariant>();
}