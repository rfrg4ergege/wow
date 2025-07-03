#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QDialog>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include "KeyAuthAPI.h"

class AuthThread : public QThread {
    Q_OBJECT
public:
    AuthThread(const QString& auth_type, const QStringList& credentials, KeyAuthAPI* keyauth_app, QObject* parent = nullptr);
    void run() override;

signals:
    void authSuccess(const QMap<QString, QVariant>& user_info);
    void authFailed(const QString& error_message);
    void progressUpdate(const QString& message);

private:
    QString auth_type_;
    QStringList credentials_;
    KeyAuthAPI* keyauth_app_;
};

class AuthWindow : public QDialog {
    Q_OBJECT
public:
    explicit AuthWindow(QWidget* parent = nullptr);
    KeyAuthAPI* getKeyAuthInstance() const;

private:
    void initKeyAuth();
    void initUI();
    QWidget* createLoginTab();
    QWidget* createRegisterTab();
    QWidget* createLicenseTab();
    void applyStyling();
    void authenticate();
    void setUIEnabled(bool enabled);
    void showError(const QString& message);

private slots:
    void onAuthSuccess(const QMap<QString, QVariant>& user_info);
    void onAuthFailed(const QString& error_message);
    void onProgressUpdate(const QString& message);

private:
    KeyAuthAPI* keyauth_app_ = nullptr;
    AuthThread* auth_thread_ = nullptr;
    QTabWidget* tab_widget_;
    QLineEdit* username_edit_;
    QLineEdit* password_edit_;
    QLineEdit* reg_username_edit_;
    QLineEdit* reg_password_edit_;
    QLineEdit* reg_license_edit_;
    QLineEdit* license_edit_;
    QProgressBar* progress_bar_;
    QLabel* status_label_;
    QPushButton* auth_button_;
    QPushButton* cancel_button_;
};

#endif // AUTHWINDOW_H