#include "PresenceManager.h"
#include <QDebug>
#include <QDateTime>

void discordCallback(const DiscordRichPresence* /*presence*/) {
    // Discord RPC callback (simplified)
}

PresenceManager::PresenceManager(QObject* parent) : QObject(parent) {
    start_time_ = QDateTime::currentMSecsSinceEpoch() / 1000;
}

PresenceManager::~PresenceManager() {
    disconnect();
}

void PresenceManager::setClientId(const QString& client_id) {
    if (client_id != client_id_) {
        client_id_ = client_id;
        if (connected_) {
            disconnect();
            connect();
        }
    }
}

bool PresenceManager::connect() {
    if (connected_) return true;

    DiscordEventHandlers handlers = {};
    handlers.ready = [](const DiscordUser* /*user*/) { qDebug() << "Discord RPC connected"; };
    handlers.disconnected = [](int errorCode, const char* message) { qDebug() << "Disconnected:" << message; };
    handlers.errored = [](int errorCode, const char* message) { qDebug() << "Error:" << message; };

    Discord_Initialize(client_id_.toUtf8().constData(), &handlers, 1, nullptr);
    connected_ = true;
    emit connectionChanged(true);
    return true;
}

bool PresenceManager::disconnect() {
    if (connected_) {
        Discord_ClearPresence();
        Discord_Shutdown();
        connected_ = false;
        emit connectionChanged(false);
    }
    return true;
}

bool PresenceManager::isConnected() const {
    return connected_;
}

bool PresenceManager::updatePresence(const QMap<QString, QVariant>& presence_data) {
    if (!isConnected()) {
        qDebug() << "Discord not connected - Cannot update presence";
        return false;
    }

    DiscordRichPresence presence = {};
    presence.startTimestamp = custom_start_time_ >= 0 ? custom_start_time_ : start_time_;

    if (presence_data.contains("details")) presence.details = strdup(presence_data["details"].toString().toUtf8().constData());
    if (presence_data.contains("state")) presence.state = strdup(presence_data["state"].toString().toUtf8().constData());
    if (presence_data.contains("large_image")) presence.largeImageKey = strdup(presence_data["large_image"].toString().toUtf8().constData());
    if (presence_data.contains("large_text")) presence.largeImageText = strdup(presence_data["large_text"].toString().toUtf8().constData());
    if (presence_data.contains("small_image")) presence.smallImageKey = strdup(presence_data["small_image"].toString().toUtf8().constData());
    if (presence_data.contains("small_text")) presence.smallImageText = strdup(presence_data["small_text"].toString().toUtf8().constData());

    DiscordButton buttons[2] = {};
    int button_count = 0;
    if (presence_data.contains("button1_label") && presence_data.contains("button1_url")) {
        QString label = presence_data["button1_label"].toString().left(32);
        QString url = presence_data["button1_url"].toString();
        if (!label.isEmpty() && !url.isEmpty()) {
            buttons[button_count].label = strdup(label.toUtf8().constData());
            buttons[button_count].url = strdup(url.toUtf8().constData());
            button_count++;
        }
    }
    if (presence_data.contains("button2_label") && presence_data.contains("button2_url")) {
        QString label = presence_data["button2_label"].toString().left(32);
        QString url = presence_data["button2_url"].toString();
        if (!label.isEmpty() && !url.isEmpty()) {
            buttons[button_count].label = strdup(label.toUtf8().constData());
            buttons[button_count].url = strdup(url.toUtf8().constData());
            button_count++;
        }
    }
    if (button_count > 0) {
        presence.buttons = buttons;
        presence.buttonCount = button_count;
    }

    Discord_UpdatePresence(&presence);

    // Free allocated strings
    if (presence.details) free((void*)presence.details);
    if (presence.state) free((void*)presence.state);
    if (presence.largeImageKey) free((void*)presence.largeImageKey);
    if (presence.largeImageText) free((void*)presence.largeImageText);
    if (presence.smallImageKey) free((void*)presence.smallImageKey);
    if (presence.smallImageText) free((void*)presence.smallImageText);
    for (int i = 0; i < button_count; ++i) {
        if (buttons[i].label) free((void*)buttons[i].label);
        if (buttons[i].url) free((void*)buttons[i].url);
    }

    emit presenceUpdated(true);
    return true;
}

bool PresenceManager::clearPresence() {
    if (isConnected()) {
        Discord_ClearPresence();
        return true;
    }
    return false;
}

void PresenceManager::resetStartTime() {
    start_time_ = QDateTime::currentMSecsSinceEpoch() / 1000;
    custom_start_time_ = -1;
}

void PresenceManager::setElapsedTime(int hours, int minutes, int seconds) {
    qint64 total_seconds = hours * 3600 + minutes * 60 + seconds;
    custom_start_time_ = QDateTime::currentMSecsSinceEpoch() / 1000 - total_seconds;
}

void PresenceManager::clearCustomTime() {
    custom_start_time_ = -1;
}