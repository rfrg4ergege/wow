#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "ConfigManager.h"
#include "PresenceManager.h"
#include "ImageManager.h"
#include "ProfileManager.h"
#include "ThemeManager.h"
#include "SystemTrayManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void initUI();
    void createPresenceTab();
    void createTimeSettingsTab();
    void createProfilesTab();
    void createSettingsTab();
    void setupMenuBar();
    void setupStatusBar();
    void setupSystemTray();
    void loadSettings();
    void autoConnectDiscord();
    void updatePreview();
    void togglePresence(int state);
    void updatePresence();
    void toggleAutoUpdate(int state);
    void autoUpdatePresence();
    void checkConnectionStatus();
    void enableTimeControls();
    void updateCustomTime();
    void resetPlayingTime();
    void refreshProfileList();
    void loadSelectedProfile();
    void loadProfileData(const QMap<QString, QVariant>& data);
    void saveNewProfile();
    void deleteSelectedProfile();
    QMap<QString, QVariant> getCurrentProfileData();
    void changeTheme(const QString& theme_name);
    void updateClientId(const QString& client_id);
    void toggleAutosave(int state);
    void autoSaveCurrentState();
    void clearAllFields();

private:
    ConfigManager* config_manager_;
    PresenceManager* presence_manager_;
    ImageManager* image_manager_;
    ProfileManager* profile_manager_;
    ThemeManager* theme_manager_;
    SystemTrayManager* system_tray_;
    QTabWidget* tab_widget_;
    QLineEdit* details_input_;
    QLineEdit* state_input_;
    QComboBox* large_image_combo_;
    QLineEdit* large_image_text_;
    QComboBox* small_image_combo_;
    QLineEdit* small_image_text_;
    QLineEdit* button1_label_;
    QLineEdit* button1_url_;
    QLineEdit* button2_label_;
    QLineEdit* button2_url_;
    QCheckBox* enable_presence_;
    QCheckBox* auto_update_checkbox_;
    QSpinBox* auto_update_interval_;
    QTextEdit* preview_text_;
    QLabel* status_label_;
    QLabel* connection_status_;
    QLabel* last_update_label_;
    QTimer* auto_update_timer_;
    QTimer* status_timer_;
    QCheckBox* autosave_checkbox_;
    QComboBox* theme_combo_;
    QLineEdit* client_id_input_;
    QListWidget* profile_list_;
    QPushButton* enable_time_button_;
    QSpinBox* hours_spinbox_;
    QSpinBox* minutes_spinbox_;
    QSpinBox* seconds_spinbox_;
    QGroupBox* time_input_frame_;
    bool time_enabled_ = false;
    QLabel* connection_indicator_;
    QStatusBar* status_bar_;
};

#endif // MAINWINDOW_H