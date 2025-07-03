 

ProfileManager::ProfileManager(QWidget* parent) : QWidget(parent) {
    QDir().mkpath(profiles_dir_);
    createSampleProfiles();
}

void ProfileManager::createSampleProfiles() {
    QMap<QString, QVariant> echo_bot_profile = {
        {"details", "Managing Discord Servers"},
        {"state", "Echo Bot v2.0"},
        {"large_image", "echo_shield"},
        {"large_text", "Echo Bot - Your Discord Assistant"},
        {"small_image", "online_status"},
        {"small_text", "Online and Ready"},
        {"button1_label", "Invite Bot"},
        {"button1_url", "https://discord.com/oauth2/authorize?client_id=1234567890"},
        {"button2_label", "Discord Server"},
        {"button2_url", "https://discord.gg/example"}
    };

    QMap<QString, QVariant> gaming_profile = {
        {"details", "In Game"},
        {"state", "Playing Solo"},
        {"large_image", "game_icon"},
        {"large_text", "Epic Adventure Game"},
        {"small_image", "status_online"},
        {"small_text", "Online"},
        {"button1_label", "Join Game"},
        {"button1_url", "https://example.com/join"},
        {"button2_label", ""},
        {"button2_url", ""}
    };

    saveProfile("echo_bot", echo_bot_profile);
    saveProfile("gaming_session", gaming_profile);
}

void ProfileManager::saveProfile(const QString& name, const QMap<QString, QVariant>& data) {
    QFile file(profiles_dir_ + "/" + name + ".json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(QJsonObject::fromVariantMap(data));
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qDebug() << "Error saving profile:" << name;
    }
}

QMap<QString, QVariant> ProfileManager::loadProfile(const QString& name) {
    QFile file(profiles_dir_ + "/" + name + ".json");
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        return doc.object().toVariantMap();
    }
    qDebug() << "Error loading profile:" << name;
    return {};
}

QStringList ProfileManager::getProfileList() const {
    QDir dir(profiles_dir_);
    QStringList profiles;
    for (const QFileInfo& file : dir.entryInfoList({"*.json"}, QDir::Files)) {
        profiles.append(file.baseName());
    }
    return profiles;
}