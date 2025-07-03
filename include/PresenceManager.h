#ifndef PRESENCEMANAGER_H
#define PRESENCEMANAGER_H

#include <QObject>
#include <QTimer>
#include <discord_rpc.h> // Requires Discord RPC SDK

class PresenceManager : public QObject {
    Q_OBJECT
public:
    explicit PresenceManager(QObject* parent = nullptr);
    ~PresenceManager();
    void setClientId(const QString& client_id);
    bool connect();
    bool disconnect();
    bool isConnected() const;
    bool updatePresence(const QMap<QString, QVariant>& presence_data);
    bool clearPresence();
    void resetStartTime();
    void setElapsedTime(int hours, int minutes, int seconds);
    void clearCustomTime();

signals:
    void connectionChanged(bool connected);
    void presenceUpdated(bool success);

private:
    QString client_id_ = "1385804140697096233";
    bool connected_ = false;
    qint64 start_time_;
    qint64 custom_start_time_ = -1;
};

#endif // PRESENCEMANAGER_H