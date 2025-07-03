#include <QApplication>
#include <QDir>
#include "AuthWindow.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Discord Rich Presence Manager");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("DRPM");

    QDir data_dir("data");
    data_dir.mkpath("profiles");
    data_dir.mkpath("images");

    AuthWindow auth_window;
    if (auth_window.exec() == QDialog::Accepted) {
        KeyAuthAPI* keyauth_instance = auth_window.getKeyAuthInstance();
        if (keyauth_instance && keyauth_instance->getUserInfo().contains("username")) {
            MainWindow window;
            window.show();
            return app.exec();
        }
    }
    return 0;
}