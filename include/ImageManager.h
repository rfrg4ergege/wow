#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QWidget>
#include <QJsonDocument>
#include <QMap>

class ImageManager : public QWidget {
    Q_OBJECT
public:
    explicit ImageManager(QWidget* parent = nullptr);
    QStringList getAvailableImages() const;
    QString getAssetDisplayName(const QString& asset_key) const;

signals:
    void imagesUpdated();

private:
    void loadAssets();
    void saveAssets();
    QString images_dir_ = "data/images";
    QString assets_file_ = "data/images/assets.json";
    QMap<QString, QMap<QString, QString>> assets_;
};

#endif // IMAGEMANAGER_H