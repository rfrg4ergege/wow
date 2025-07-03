#include "ImageManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ImageManager::ImageManager(QWidget* parent) : QWidget(parent) {
    QDir().mkpath(images_dir_);
    loadAssets();
}

void ImageManager::loadAssets() {
    QFile file(assets_file_);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            assets_[it.key()] = it.value().toObject().toVariantMap();
        }
        file.close();
        emit imagesUpdated();
    }
}

void ImageManager::saveAssets() {
    QFile file(assets_file_);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        for (auto it = assets_.constBegin(); it != assets_.constEnd(); ++it) {
            obj[it.key()] = QJsonObject::fromVariantMap(it.value());
        }
        QJsonDocument doc(obj);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qDebug() << "Error saving assets to" << assets_file_;
    }
}

QStringList ImageManager::getAvailableImages() const {
    return assets_.keys();
}

QString ImageManager::getAssetDisplayName(const QString& asset_key) const {
    return assets_.value(asset_key).value("name", asset_key).toString();
}