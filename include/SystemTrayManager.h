#ifndef SYSTEMTRAYMANAGER_H
#define SYSTEMTRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>

class MainWindow;

class SystemTrayManager : public QObject {
    Q_OBJECT
public:
    explicit SystemTrayManager(MainWindow* main_window, QObject* parent = nullptr);
    void setupTray();
    void showMessage(const QString& title, const QString& message);

private:
    void createTrayMenu();
    MainWindow* main_window_;
    QSystemTrayIcon* tray_icon_ = nullptr;
};

#endif // SYSTEMTRAYMANAGER_H