#include "AuthWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTextEdit>
#include <QCryptographicHash>
#include <QDebug>

QString getChecksum() {
    QFile file(QCoreApplication::applicationFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    }
    return "dev_mode_checksum";
}

AuthThread::AuthThread(const QString& auth_type, const QStringList& credentials, KeyAuthAPI* keyauth_app, QObject* parent)
    : QThread(parent), auth_type_(auth_type), credentials_(credentials), keyauth_app_(keyauth_app) {}

void AuthThread::run() {
    emit progressUpdate("Initializing KeyAuth...");
    if (!keyauth_app_->init()) {
        emit authFailed("Failed to initialize KeyAuth");
        return;
    }

    emit progressUpdate("Authenticating...");
    bool success = false;
    if (auth_type_ == "login") {
        if (credentials_.size() >= 2) {
            success = keyauth_app_->login(credentials_[0], credentials_[1]);
        }
    } else if (auth_type_ == "register") {
        if (credentials_.size() >= 3) {
            success = keyauth_app_->registerUser(credentials_[0], credentials_[1], credentials_[2]);
        }
    } else if (auth_type_ == "license") {
        if (credentials_.size() >= 1) {
            success = keyauth_app_->license(credentials_[0]);
        }
    } else {
        emit authFailed("Invalid authentication type");
        return;
    }

    if (success) {
        emit progressUpdate("Authentication successful!");
        emit authSuccess(keyauth_app_->getUserInfo());
    } else {
        emit authFailed("Authentication failed");
    }
}

AuthWindow::AuthWindow(QWidget* parent) : QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint) {
    initKeyAuth();
    initUI();
}

void AuthWindow::initKeyAuth() {
    keyauth_app_ = new KeyAuthAPI("Ownerofcipk's Application", "PEc07ci4kE", "1.0", getChecksum(), this);
}

void AuthWindow::initUI() {
    setWindowTitle("Authentication Required");
    setFixedSize(400, 500);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel("Discord Rich Presence Manager");
    title->setAlignment(Qt::AlignCenter);
    QFont title_font;
    title_font.setPointSize(16);
    title_font.setBold(true);
    title->setFont(title_font);
    layout->addWidget(title);

    QLabel* subtitle = new QLabel("Please authenticate to continue");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #666; margin-bottom: 20px;");
    layout->addWidget(subtitle);

    tab_widget_ = new QTabWidget();
    tab_widget_->addTab(createLoginTab(), "Login");
    tab_widget_->addTab(createRegisterTab(), "Register");
    tab_widget_->addTab(createLicenseTab(), "License Key");
    layout->addWidget(tab_widget_);

    progress_bar_ = new QProgressBar();
    progress_bar_->setVisible(false);
    layout->addWidget(progress_bar_);

    status_label_ = new QLabel("");
    status_label_->setAlignment(Qt::AlignCenter);
    status_label_->setStyleSheet("color: #666; margin: 10px;");
    layout->addWidget(status_label_);

    QHBoxLayout* button_layout = new QHBoxLayout();
    cancel_button_ = new QPushButton("Exit");
    connect(cancel_button_, &QPushButton::clicked, this, &QDialog::reject);
    button_layout->addWidget(cancel_button_);

    button_layout->addStretch();

    auth_button_ = new QPushButton("Authenticate");
    auth_button_->setDefault(true);
    connect(auth_button_, &QPushButton::clicked, this, &AuthWindow::authenticate);
    button_layout->addWidget(auth_button_);

    layout->addLayout(button_layout);
    applyStyling();
}

QWidget* AuthWindow::createLoginTab() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(new QLabel("Username:"));
    username_edit_ = new QLineEdit();
    username_edit_->setPlaceholderText("Enter your username");
    layout->addWidget(username_edit_);

    layout->addWidget(new QLabel("Password:"));
    password_edit_ = new QLineEdit();
    password_edit_->setEchoMode(QLineEdit::Password);
    password_edit_->setPlaceholderText("Enter your password");
    layout->addWidget(password_edit_);

    layout->addStretch();
    return widget;
}

QWidget* AuthWindow::createRegisterTab() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(new QLabel("Username:"));
    reg_username_edit_ = new QLineEdit();
    reg_username_edit_->setPlaceholderText("Choose a username");
    layout->addWidget(reg_username_edit_);

    layout->addWidget(new QLabel("Password:"));
    reg_password_edit_ = new QLineEdit();
    reg_password_edit_->setEchoMode(QLineEdit::Password);
    reg_password_edit_->setPlaceholderText("Choose a password");
    layout->addWidget(reg_password_edit_);

    layout->addWidget(new QLabel("License Key:"));
    reg_license_edit_ = new QLineEdit();
    reg_license_edit_->setPlaceholderText("Enter your license key");
    layout->addWidget(reg_license_edit_);

    layout->addStretch();
    return widget;
}

QWidget* AuthWindow::createLicenseTab() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QTextEdit* info_text = new QTextEdit();
    info_text->setReadOnly(true);
    info_text->setMaximumHeight(80);
    info_text->setPlainText("If you have a license key, you can use it directly without creating an account.");
    layout->addWidget(info_text);

    layout->addWidget(new QLabel("License Key:"));
    license_edit_ = new QLineEdit();
    license_edit_->setPlaceholderText("Enter your license key");
    layout->addWidget(license_edit_);

    layout->addStretch();
    return widget;
}

void AuthWindow::applyStyling() {
    setStyleSheet(R"(
        QDialog {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QLabel {
            color: #ffffff;
            font-size: 12px;
        }
        QLineEdit {
            background-color: #3c3c3c;
            border: 2px solid #555555;
            border-radius: 5px;
            padding: 8px;
            color: #ffffff;
            font-size: 12px;
        }
        QLineEdit:focus {
            border-color: #0078d4;
        }
        QPushButton {
            background-color: #0078d4;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #106ebe;
        }
        QTabWidget::pane {
            border: 1px solid #555555;
            background-color: #2b2b2b;
        }
        QTabBar::tab {
            background-color: #3c3c3c;
            color: #ffffff;
            padding: 8px 16px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: #0078d4;
        }
        QProgressBar {
            border: 2px solid #555555;
            border-radius: 5px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #0078d4;
            border-radius: 3px;
        }
        QTextEdit {
            background-color: #3c3c3c;
            border: 1px solid #555555;
            color: #ffffff;
            border-radius: 5px;
            padding: 5px;
        }
    )");
}

void AuthWindow::authenticate() {
    setUIEnabled(false);
    progress_bar_->setVisible(true);
    progress_bar_->setRange(0, 0);

    int current_tab = tab_widget_->currentIndex();
    QStringList credentials;

    if (current_tab == 0) { // Login tab
        QString username = username_edit_->text().trimmed();
        QString password = password_edit_->text().trimmed();
        if (username.isEmpty() || password.isEmpty()) {
            showError("Please enter both username and password");
            return;
        }
        credentials << username << password;
        auth_thread_ = new AuthThread("login", credentials, keyauth_app_, this);
    } else if (current_tab == 1) { // Register tab
        QString username = reg_username_edit_->text().trimmed();
        QString password = reg_password_edit_->text().trimmed();
        QString license_key = reg_license_edit_->text().trimmed();
        if (username.isEmpty() || password.isEmpty() || license_key.isEmpty()) {
            showError("Please fill in all fields");
            return;
        }
        credentials << username << password << license_key;
        auth_thread_ = new AuthThread("register", credentials, keyauth_app_, this);
    } else if (current_tab == 2) { // License tab
        QString license_key = license_edit_->text().trimmed();
        if (license_key.isEmpty()) {
            showError("Please enter your license key");
            return;
        }
        credentials << license_key;
        auth_thread_ = new AuthThread("license", credentials, keyauth_app_, this);
    }

    if (auth_thread_) {
        connect(auth_thread_, &AuthThread::authSuccess, this, &AuthWindow::onAuthSuccess);
        connect(auth_thread_, &AuthThread::authFailed, this, &AuthWindow::onAuthFailed);
        connect(auth_thread_, &AuthThread::progressUpdate, this, &AuthWindow::onProgressUpdate);
        auth_thread_->start();
    }
}

void AuthWindow::onAuthSuccess(const QMap<QString, QVariant>& /*user_info*/) {
    setUIEnabled(true);
    progress_bar_->setVisible(false);
    status_label_->setText("Authentication successful!");
    status_label_->setStyleSheet("color: #00ff00; margin: 10px;");
    accept();
}

void AuthWindow::onAuthFailed(const QString& error_message) {
    setUIEnabled(true);
    progress_bar_->setVisible(false);
    showError(error_message);
}

void AuthWindow::onProgressUpdate(const QString& message) {
    status_label_->setText(message);
    status_label_->setStyleSheet("color: #ffffff; margin: 10px;");
}

void AuthWindow::setUIEnabled(bool enabled) {
    tab_widget_->setEnabled(enabled);
    auth_button_->setEnabled(enabled);
}

void AuthWindow::showError(const QString& message) {
    setUIEnabled(true);
    progress_bar_->setVisible(false);
    status_label_->setText("Error: " + message);
    status_label_->setStyleSheet("color: #ff0000; margin: 10px;");
    QMessageBox::critical(this, "Authentication Error", message);
}

KeyAuthAPI* AuthWindow::getKeyAuthInstance() const {
    return (keyauth_app_ && keyauth_app_->getUserInfo().contains("username")) ? keyauth_app_ : nullptr;
}