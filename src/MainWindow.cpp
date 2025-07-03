#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QTextEdit>
#include <QSplitter>
#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QAction>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Discord Rich Presence Manager v2.0");
    setMinimumSize(450, 350);

    config_manager_ = new ConfigManager(this);
    presence_manager_ = new PresenceManager(this);
    image_manager_ = new ImageManager(this);
    profile_manager_ = new ProfileManager(this);
    theme_manager_ = new ThemeManager(this);
    system_tray_ = new SystemTrayManager(this, this);

    auto_update_timer_ = new QTimer(this);
    connect(auto_update_timer_, &QTimer::timeout, this, &MainWindow::autoUpdatePresence);

    status_timer_ = new QTimer(this);
    connect(status_timer_, &QTimer::timeout, this, &MainWindow::checkConnectionStatus);
    status_timer_->start(5000);

    initUI();
    setupMenuBar();
    setupStatusBar();
    setupSystemTray();
    loadSettings();
    theme_manager_->applyTheme(this, config_manager_->getSetting("theme", "dark").toString());
    autoConnectDiscord();
}

void MainWindow::initUI() {
    QFont font;
    font.setPointSize(8);
    font.setFamily("Segoe UI");
    setFont(font);

    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
    main_layout->setSpacing(2);
    main_layout->setContentsMargins(3, 3, 3, 3);

    tab_widget_ = new QTabWidget();
    main_layout->addWidget(tab_widget_);

    createPresenceTab();
    createTimeSettingsTab();
    createProfilesTab();
    createSettingsTab();
}

void MainWindow::createPresenceTab() {
    QWidget* presence_widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(presence_widget);

    QFrame* left_frame = new QFrame();
    left_frame->setFrameStyle(QFrame::StyledPanel);
    QVBoxLayout* left_layout = new QVBoxLayout(left_frame);
    left_layout->setSpacing(12);
    left_layout->setContentsMargins(8, 8, 8, 8);

    QGroupBox* basic_group = new QGroupBox("Basic Information");
    QFormLayout* basic_layout = new QFormLayout(basic_group);
    basic_layout->setSpacing(6);
    basic_layout->setVerticalSpacing(8);

    details_input_ = new QLineEdit();
    details_input_->setMaxLength(128);
    details_input_->setPlaceholderText("What you're doing");
    connect(details_input_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    basic_layout->addRow("Details:", details_input_);

    state_input_ = new QLineEdit();
    state_input_->setMaxLength(128);
    state_input_->setPlaceholderText("Your current state");
    connect(state_input_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    basic_layout->addRow("State:", state_input_);

    left_layout->addWidget(basic_group);

    QGroupBox* images_group = new QGroupBox("Images");
    QFormLayout* images_layout = new QFormLayout(images_group);

    large_image_combo_ = new QComboBox();
    large_image_combo_->setEditable(true);
    large_image_combo_->addItems(image_manager_->getAvailableImages());
    connect(large_image_combo_, &QComboBox::currentTextChanged, this, &MainWindow::updatePreview);
    images_layout->addRow("Large Image:", large_image_combo_);

    large_image_text_ = new QLineEdit();
    large_image_text_->setPlaceholderText("Large image tooltip");
    connect(large_image_text_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    images_layout->addRow("Large Image Text:", large_image_text_);

    small_image_combo_ = new QComboBox();
    small_image_combo_->setEditable(true);
    small_image_combo_->addItems(image_manager_->getAvailableImages());
    connect(small_image_combo_, &QComboBox::currentTextChanged, this, &MainWindow::updatePreview);
    images_layout->addRow("Small Image:", small_image_combo_);

    small_image_text_ = new QLineEdit();
    small_image_text_->setPlaceholderText("Small image tooltip");
    connect(small_image_text_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    images_layout->addRow("Small Image Text:", small_image_text_);

    left_layout->addWidget(images_group);

    QGroupBox* buttons_group = new QGroupBox("Buttons");
    QFormLayout* buttons_layout = new QFormLayout(buttons_group);

    button1_label_ = new QLineEdit();
    button1_label_->setMaxLength(32);
    button1_label_->setPlaceholderText("Button 1 label");
    connect(button1_label_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    buttons_layout->addRow("Button 1 Label:", button1_label_);

    button1_url_ = new QLineEdit();
    button1_url_->setPlaceholderText("https://example.com");
    connect(button1_url_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    buttons_layout->addRow("Button 1 URL:", button1_url_);

    button2_label_ = new QLineEdit();
    button2_label_->setMaxLength(32);
    button2_label_->setPlaceholderText("Button 2 label");
    connect(button2_label_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    buttons_layout->addRow("Button 2 Label:", button2_label_);

    button2_url_ = new QLineEdit();
    button2_url_->setPlaceholderText("https://example.com");
    connect(button2_url_, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    buttons_layout->addRow("Button 2 URL:", button2_url_);

    left_layout->addWidget(buttons_group);

    QGroupBox* control_group = new QGroupBox("Controls");
    QVBoxLayout* control_layout = new QVBoxLayout(control_group);

    enable_presence_ = new QCheckBox("Enable Rich Presence");
    connect(enable_presence_, &QCheckBox::stateChanged, this, &MainWindow::togglePresence);
    control_layout->addWidget(enable_presence_);

    QHBoxLayout* auto_update_layout = new QHBoxLayout();
    auto_update_checkbox_ = new QCheckBox("Auto-update every");
    connect(auto_update_checkbox_, &QCheckBox::stateChanged, this, &MainWindow::toggleAutoUpdate);
    auto_update_layout->addWidget(auto_update_checkbox_);

    auto_update_interval_ = new QSpinBox();
    auto_update_interval_->setRange(1, 300);
    auto_update_interval_->setValue(10);
    auto_update_interval_->setSuffix(" seconds");
    auto_update_layout->addWidget(auto_update_interval_);

    control_layout->addLayout(auto_update_layout);
    left_layout->addWidget(control_group);

    QFrame* right_frame = new QFrame();
    right_frame->setFrameStyle(QFrame::StyledPanel);
    QVBoxLayout* right_layout = new QVBoxLayout(right_frame);

    QGroupBox* preview_group = new QGroupBox("Live Preview");
    QVBoxLayout* preview_layout = new QVBoxLayout(preview_group);

    preview_text_ = new QTextEdit();
    preview_text_->setReadOnly(true);
    preview_text_->setMaximumHeight(120);
    preview_text_->setMinimumHeight(80);
    preview_text_->setStyleSheet("font-family: 'Consolas', monospace; font-size: 9pt;");
    preview_layout->addWidget(preview_text_);

    right_layout->addWidget(preview_group);

    QGroupBox* status_group = new QGroupBox("Status");
    QVBoxLayout* status_layout = new QVBoxLayout(status_group);

    status_label_ = new QLabel("Status: Disconnected");
    status_label_->setAlignment(Qt::AlignCenter);
    status_label_->setStyleSheet("font-weight: bold; padding: 10px;");
    status_layout->addWidget(status_label_);

    connection_status_ = new QLabel("Connection: Not connected");
    status_layout->addWidget(connection_status_);

    last_update_label_ = new QLabel("Last update: Never");
    status_layout->addWidget(last_update_label_);

    right_layout->addWidget(status_group);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(left_frame);
    splitter->addWidget(right_frame);
    splitter->setSizes({500, 350});

    layout->addWidget(splitter);
    tab_widget_->addTab(presence_widget, "Rich Presence");
}

void MainWindow::createTimeSettingsTab() {
    QWidget* time_widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(time_widget);
    layout->setSpacing(4);
    layout->setContentsMargins(6, 6, 6, 6);

    enable_time_button_ = new QPushButton("Enable Custom Time");
    connect(enable_time_button_, &QPushButton::clicked, this, &MainWindow::enableTimeControls);
    layout->addWidget(enable_time_button_);

    QGroupBox* time_group = new QGroupBox("Playing Time");
    QHBoxLayout* time_layout = new QHBoxLayout(time_group);
    time_layout->setSpacing(8);

    time_layout->addWidget(new QLabel("H:"));
    hours_spinbox_ = new QSpinBox();
    hours_spinbox_->setRange(0, 23);
    hours_spinbox_->setValue(0);
    connect(hours_spinbox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateCustomTime);
    time_layout->addWidget(hours_spinbox_);

    time_layout->addWidget(new QLabel("M:"));
    minutes_spinbox_ = new QSpinBox();
    minutes_spinbox_->setRange(0, 59);
    minutes_spinbox_->setValue(0);
    connect(minutes_spinbox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateCustomTime);
    time_layout->addWidget(minutes_spinbox_);

    time_layout->addWidget(new QLabel("S:"));
    seconds_spinbox_ = new QSpinBox();
    seconds_spinbox_->setRange(0, 59);
    seconds_spinbox_->setValue(0);
    connect(seconds_spinbox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateCustomTime);
    time_layout->addWidget(seconds_spinbox_);

    time_layout->addStretch();

    QPushButton* reset_time_button = new QPushButton("Reset");
    connect(reset_time_button, &QPushButton::clicked, this, &MainWindow::resetPlayingTime);
    time_layout->addWidget(reset_time_button);

    time_input_frame_ = time_group;
    time_input_frame_->setEnabled(false);
    layout->addWidget(time_group);

    layout->addStretch();
    tab_widget_->addTab(time_widget, "Time");
}

void MainWindow::createProfilesTab() {
    QWidget* profiles_widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(profiles_widget);

    QGroupBox* profile_group = new QGroupBox("Profiles");
    QVBoxLayout* profile_layout = new QVBoxLayout(profile_group);

    profile_list_ = new QListWidget();
    refreshProfileList();
    connect(profile_list_, &QListWidget::itemClicked, this, &MainWindow::loadSelectedProfile);
    profile_layout->addWidget(profile_list_);

    QHBoxLayout* button_layout = new QHBoxLayout();

    QPushButton* load_button = new QPushButton("Load");
    connect(load_button, &QPushButton::clicked, this, &MainWindow::loadSelectedProfile);
    button_layout->addWidget(load_button);

    QPushButton* save_button = new QPushButton("Save");
    connect(save_button, &QPushButton::clicked, this, &MainWindow::saveNewProfile);
    button_layout->addWidget(save_button);

    QPushButton* delete_button = new QPushButton("Delete");
    connect(delete_button, &QPushButton::clicked, this, &MainWindow::deleteSelectedProfile);
    button_layout->addWidget(delete_button);

    profile_layout->addLayout(button_layout);
    layout->addWidget(profile_group);

    tab_widget_->addTab(profiles_widget, "Profiles");
}

void MainWindow::createSettingsTab() {
    QWidget* settings_widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(settings_widget);

    QGroupBox* theme_group = new QGroupBox("Appearance");
    QFormLayout* theme_layout = new QFormLayout(theme_group);

    theme_combo_ = new QComboBox();
    theme_combo_->addItems({"Dark", "Light"});
    connect(theme_combo_, &QComboBox::currentTextChanged, this, &MainWindow::changeTheme);
    theme_layout->addRow("Theme:", theme_combo_);

    layout->addWidget(theme_group);

    QGroupBox* discord_group = new QGroupBox("Discord Settings");
    QFormLayout* discord_layout = new QFormLayout(discord_group);

    client_id_input_ = new QLineEdit();
    client_id_input_->setPlaceholderText("Discord Application Client ID");
    connect(client_id_input_, &QLineEdit::textChanged, this, &MainWindow::updateClientId);
    discord_layout->addRow("Client ID:", client_id_input_);

    layout->addWidget(discord_group);

    QGroupBox* autosave_group = new QGroupBox("Auto-save");
    QVBoxLayout* autosave_layout = new QVBoxLayout(autosave_group);

    autosave_checkbox_ = new QCheckBox("Enable auto-save");
    connect(autosave_checkbox_, &QCheckBox::stateChanged, this, &MainWindow::toggleAutosave);
    autosave_layout->addWidget(autosave_checkbox_);

    layout->addWidget(autosave_group);

    layout->addStretch();
    tab_widget_->addTab(settings_widget, "Settings");
}

void MainWindow::setupMenuBar() {
    QMenuBar* menubar = menuBar();
    QMenu* file_menu = menubar->addMenu("File");

    QAction* new_profile_action = new QAction("New Profile", this);
    new_profile_action->setShortcut(QKeySequence::New);
    connect(new_profile_action, &QAction::triggered, this, &MainWindow::clearAllFields);
    file_menu->addAction(new_profile_action);

    QAction* save_profile_action = new QAction("Save Profile", this);
    save_profile_action->setShortcut(QKeySequence::Save);
    connect(save_profile_action, &QAction::triggered, this, &MainWindow::saveNewProfile);
    file_menu->addAction(save_profile_action);

    file_menu->addSeparator();

    QAction* exit_action = new QAction("Exit", this);
    exit_action->setShortcut(QKeySequence::Quit);
    connect(exit_action, &QAction::triggered, this, &QMainWindow::close);
    file_menu->addAction(exit_action);
}

void MainWindow::setupStatusBar() {
    status_bar_ = statusBar();
    status_bar_->showMessage("Ready");

    connection_indicator_ = new QLabel("●");
    connection_indicator_->setStyleSheet("color: red; font-size: 12pt;");
    status_bar_->addPermanentWidget(connection_indicator_);
}

void MainWindow::setupSystemTray() {
    system_tray_->setupTray();
}

void MainWindow::loadSettings() {
    QVariant theme = config_manager_->getSetting("theme", "dark");
    theme_combo_->setCurrentText(theme.toString().toUpper()[0] + theme.toString().mid(1).toLower());

    QString client_id = config_manager_->getSetting("client_id", "1385804140697096233").toString();
    client_id_input_->setText(client_id);
    presence_manager_->setClientId(client_id);

    bool autosave = config_manager_->getSetting("autosave", true).toBool();
    autosave_checkbox_->setChecked(autosave);

    bool auto_update = config_manager_->getSetting("auto_update", false).toBool();
    auto_update_checkbox_->setChecked(auto_update);

    int interval = config_manager_->getSetting("auto_update_interval", 10).toInt();
    auto_update_interval_->setValue(interval);
}

void MainWindow::updatePreview() {
    QString preview_text = "Discord Rich Presence Preview:\n\n";

    if (!details_input_->text().isEmpty()) {
        preview_text += QString("Details: %1\n").arg(details_input_->text());
    }
    if (!state_input_->text().isEmpty()) {
        preview_text += QString("State: %1\n").arg(state_input_->text());
    }
    preview_text += "\n";

    if (!large_image_combo_->currentText().isEmpty()) {
        preview_text += QString("Large Image: %1\n").arg(large_image_combo_->currentText());
        if (!large_image_text_->text().isEmpty()) {
            preview_text += QString("  Tooltip: %1\n").arg(large_image_text_->text());
        }
    }
    if (!small_image_combo_->currentText().isEmpty()) {
        preview_text += QString("Small Image: %1\n").arg(small_image_combo_->currentText());
        if (!small_image_text_->text().isEmpty()) {
            preview_text += QString("  Tooltip: %1\n").arg(small_image_text_->text());
        }
    }
    if (!button1_label_->text().isEmpty() && !button1_url_->text().isEmpty()) {
        preview_text += QString("\nButton 1: %1\n").arg(button1_label_->text());
        preview_text += QString("  URL: %1\n").arg(button1_url_->text());
    }
    if (!button2_label_->text().isEmpty() && !button2_url_->text().isEmpty()) {
        preview_text += QString("\nButton 2: %1\n").arg(button2_label_->text());
        preview_text += QString("  URL: %1\n").arg(button2_url_->text());
    }

    preview_text += QString("\nTimestamp: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    preview_text_->setPlainText(preview_text);

    if (autosave_checkbox_->isChecked()) {
        autoSaveCurrentState();
    }
}

void MainWindow::togglePresence(int state) {
    if (state == Qt::Checked) {
        if (presence_manager_->connect()) {
            updatePresence();
            status_label_->setText("Status: Presence Enabled ✅");
            status_bar_->showMessage("Rich Presence enabled");
            status_label_->setStyleSheet("color: green; font-weight: bold; padding: 10px;");
            connection_indicator_->setStyleSheet("color: green; font-size: 12pt;");
        } else {
            enable_presence_->setChecked(false);
            status_label_->setText("Status: Discord Not Running");
            status_label_->setStyleSheet("color: orange; font-weight: bold; padding: 10px;");
            connection_indicator_->setStyleSheet("color: orange; font-size: 12pt;");
            status_bar_->showMessage("Please start Discord first, then try again", 5000);
            QMessageBox::information(this, "Discord Not Found",
                                     "Discord is not running or not found.\n\n"
                                     "Please:\n"
                                     "1. Start Discord\n"
                                     "2. Wait a moment for it to load\n"
                                     "3. Try enabling presence again");
        }
    } else {
        presence_manager_->clearPresence();
        presence_manager_->disconnect();
        status_label_->setText("Status: Presence Disabled");
        status_label_->setStyleSheet("color: gray; font-weight: bold; padding: 10px;");
        connection_indicator_->setStyleSheet("color: gray; font-size: 12pt;");
        status_bar_->showMessage("Rich Presence disabled");
    }
}

void MainWindow::updatePresence() {
    if (!enable_presence_->isChecked()) return;

    if (!presence_manager_->isConnected()) {
        status_bar_->showMessage("Discord not connected - Please enable presence first", 5000);
        return;
    }

    QMap<QString, QVariant> presence_data;
    if (!details_input_->text().isEmpty()) presence_data["details"] = details_input_->text();
    if (!state_input_->text().isEmpty()) presence_data["state"] = state_input_->text();
    if (large_image_combo_->currentText() != "None") presence_data["large_image"] = large_image_combo_->currentText();
    if (!large_image_text_->text().isEmpty()) presence_data["large_text"] = large_image_text_->text();
    if (small_image_combo_->currentText() != "None") presence_data["small_image"] = small_image_combo_->currentText();
    if (!small_image_text_->text().isEmpty()) presence_data["small_text"] = small_image_text_->text();
    if (!button1_label_->text().isEmpty()) presence_data["button1_label"] = button1_label_->text();
    if (!button1_url_->text().isEmpty()) presence_data["button1_url"] = button1_url_->text();
    if (!button2_label_->text().isEmpty()) presence_data["button2_label"] = button2_label_->text();
    if (!button2_url_->text().isEmpty()) presence_data["button2_url"] = button2_url_->text();

    if (presence_manager_->updatePresence(presence_data)) {
        last_update_label_->setText(QString("Last update: %1").arg(QTime::currentTime().toString("hh:mm:ss")));
        status_bar_->showMessage("Presence updated successfully", 2000);
        autoSaveCurrentState();
    } else {
        status_bar_->showMessage("Failed to update presence - Discord connection lost", 5000);
    }
}

void MainWindow::toggleAutoUpdate(int state) {
    if (state == Qt::Checked) {
        int interval = auto_update_interval_->value() * 1000;
        auto_update_timer_->start(interval);
        status_bar_->showMessage(QString("Auto-update enabled (every %1s)").arg(auto_update_interval_->value()));
    } else {
        auto_update_timer_->stop();
        status_bar_->showMessage("Auto-update disabled");
    }
}

void MainWindow::autoUpdatePresence() {
    if (enable_presence_->isChecked()) {
        updatePresence();
    }
}

void MainWindow::checkConnectionStatus() {
    if (presence_manager_->isConnected()) {
        connection_status_->setText("Connection: Connected to Discord");
        connection_indicator_->setStyleSheet("color: green; font-size: 12pt;");
    } else {
        connection_status_->setText("Connection: Not connected");
        connection_indicator_->setStyleSheet("color: red; font-size: 12pt;");
        if (enable_presence_->isChecked()) {
            if (presence_manager_->connect()) {
                status_bar_->showMessage("Reconnected to Discord", 2000);
            }
        }
    }
}

void MainWindow::autoConnectDiscord() {
    if (presence_manager_->connect()) {
        status_label_->setText("Status: Connected to Discord ✅");
        status_label_->setStyleSheet("color: green; font-weight: bold; padding: 10px;");
        connection_status_->setText("Connection: Connected");
        qDebug() << "Successfully connected to Discord on startup";
    } else {
        status_label_->setText("Status: Ready (Discord not running)");
        status_label_->setStyleSheet("color: orange; font-weight: bold; padding: 10px;");
        connection_status_->setText("Connection: Discord not found");
        qDebug() << "Discord not running - application ready for when Discord starts";
    }
}

void MainWindow::enableTimeControls() {
    time_enabled_ = !time_enabled_;
    enable_time_button_->setText(time_enabled_ ? "Disable Custom Playing Time" : "Enable Custom Playing Time");
    time_input_frame_->setEnabled(time_enabled_);
    if (time_enabled_) {
        updateCustomTime();
    } else {
        presence_manager_->clearCustomTime();
        if (enable_presence_->isChecked()) {
            updatePresence();
        }
    }
}

void MainWindow::updateCustomTime() {
    if (time_enabled_) {
        int hours = hours_spinbox_->value();
        int minutes = minutes_spinbox_->value();
        int seconds = seconds_spinbox_->value();
        presence_manager_->setElapsedTime(hours, minutes, seconds);
        if (enable_presence_->isChecked()) {
            updatePresence();
        }
    }
}

void MainWindow::resetPlayingTime() {
    hours_spinbox_->setValue(0);
    minutes_spinbox_->setValue(0);
    seconds_spinbox_->setValue(0);
    presence_manager_->resetStartTime();
    if (enable_presence_->isChecked()) {
        updatePresence();
    }
    status_bar_->showMessage("Playing time reset to now", 2000);
}

void MainWindow::refreshProfileList() {
    profile_list_->clear();
    QStringList profiles = profile_manager_->getProfileList();
    for (const QString& profile : profiles) {
        profile_list_->addItem(profile);
    }
}

void MainWindow::loadSelectedProfile() {
    QListWidgetItem* current_item = profile_list_->currentItem();
    if (current_item) {
        QString profile_name = current_item->text();
        QMap<QString, QVariant> profile_data = profile_manager_->loadProfile(profile_name);
        if (!profile_data.isEmpty()) {
            loadProfileData(profile_data);
            status_bar_->showMessage(QString("Loaded profile: %1").arg(profile_name), 2000);
        }
    }
}

void MainWindow::loadProfileData(const QMap<QString, QVariant>& data) {
    details_input_->setText(data.value("details", "").toString());
    state_input_->setText(data.value("state", "").toString());
    large_image_combo_->setCurrentText(data.value("large_image", "").toString());
    large_image_text_->setText(data.value("large_text", "").toString());
    small_image_combo_->setCurrentText(data.value("small_image", "").toString());
    small_image_text_->setText(data.value("small_text", "").toString());
    button1_label_->setText(data.value("button1_label", "").toString());
    button1_url_->setText(data.value("button1_url", "").toString());
    button2_label_->setText(data.value("button2_label", "").toString());
    button2_url_->setText(data.value("button2_url", "").toString());
    updatePreview();
}

void MainWindow::saveNewProfile() {
    bool ok;
    QString profile_name = QInputDialog::getText(this, "Save Profile", "Enter profile name:", QLineEdit::Normal, "", &ok);
    if (ok && !profile_name.isEmpty()) {
        QMap<QString, QVariant> profile_data = getCurrentProfileData();
        profile_manager_->saveProfile(profile_name, profile_data);
        refreshProfileList();
        status_bar_->showMessage(QString("Profile saved: %1").arg(profile_name), 2000);
    }
}

void MainWindow::deleteSelectedProfile() {
    QListWidgetItem* current_item = profile_list_->currentItem();
    if (current_item) {
        QString profile_name = current_item->text();
        if (QMessageBox::question(this, "Delete Profile", QString("Delete profile '%1'?").arg(profile_name)) == QMessageBox::Yes) {
            QFile file(QString("data/profiles/%1.json").arg(profile_name));
            if (file.exists()) {
                file.remove();
                refreshProfileList();
                status_bar_->showMessage(QString("Profile deleted: %1").arg(profile_name), 2000);
            }
        }
    }
}

QMap<QString, QVariant> MainWindow::getCurrentProfileData() {
    return {
        {"details", details_input_->text()},
        {"state", state_input_->text()},
        {"large_image", large_image_combo_->currentText()},
        {"large_text", large_image_text_->text()},
        {"small_image", small_image_combo_->currentText()},
        {"small_text", small_image_text_->text()},
        {"button1_label", button1_label_->text()},
        {"button1_url", button1_url_->text()},
        {"button2_label", button2_label_->text()},
        {"button2_url", button2_url_->text()}
    };
}

void MainWindow::changeTheme(const QString& theme_name) {
    theme_manager_->applyTheme(this, theme_name.toLower());
    config_manager_->saveSetting("theme", theme_name.toLower());
}

void MainWindow::updateClientId(const QString& client_id) {
    presence_manager_->setClientId(client_id);
    config_manager_->saveSetting("client_id", client_id);
    if (enable_presence_->isChecked()) {
        updatePresence();
    }
}

void MainWindow::toggleAutosave(int state) {
    config_manager_->saveSetting("autosave", state == Qt::Checked);
    if (state == Qt::Checked) {
        autoSaveCurrentState();
    }
}

void MainWindow::autoSaveCurrentState() {
    QString last_profile = config_manager_->getSetting("last_profile", "").toString();
    if (!last_profile.isEmpty()) {
        profile_manager_->saveProfile(last_profile, getCurrentProfileData());
    }
}

void MainWindow::clearAllFields() {
    details_input_->clear();
    state_input_->clear();
    large_image_combo_->setCurrentIndex(0);
    large_image_text_->clear();
    small_image_combo_->setCurrentIndex(0);
    small_image_text_->clear();
    button1_label_->clear();
    button1_url_->clear();
    button2_label_->clear();
    button2_url_->clear();
    updatePreview();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (config_manager_->getSetting("system_tray", true).toBool() && system_tray_->isSystemTrayAvailable()) {
        hide();
        system_tray_->showMessage("Discord Rich Presence Manager", "Application minimized to system tray");
        event->ignore();
    } else {
        QMainWindow::closeEvent(event);
    }
}