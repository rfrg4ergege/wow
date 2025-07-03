#include "SystemTrayManager.h"
#include <QMenu>
#include <QAction>
#include "MainWindow.h"

SystemTrayManager::SystemTrayManager(MainWindow* main_window, QObject* parent)
    : QObject(parent), main_window_(main_window) {}

void SystemTrayManager::setupTray() {
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        tray_icon_ = new QSystemTrayIcon(main_window_);
        tray_icon_->setIcon(QIcon(":/icon.png")); // Placeholder: Add icon resource
        createTrayMenu();
        tray_icon_->show();
    }
}

void SystemTrayManager::createTrayMenu() {
    if (!tray_icon_) return;

    QMenu* menu = new QMenu(main_window_);
    QAction* show_action = new QAction("Show", this);
    connect(show_action, &QAction::triggered, main_window_, &QMainWindow::show);
    menu->addAction(show_action);

    QAction* quit_action = new QAction("Quit", this);
    connect(quit_action, &QAction::triggered, main_window_, &QMainWindow::close);
    menu->addAction(quit_action);

    tray_icon_->setContextMenu(menu);
}

void SystemTrayManager::showMessage(const QString& title, const QString& message) {
    if (tray_icon_) {
        tray_icon_->showMessage(title, message);
    }
}