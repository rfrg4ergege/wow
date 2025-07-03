#include "ThemeManager.h"
#include <QWidget>

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {}

void ThemeManager::applyTheme(QWidget* widget, const QString& theme_name) {
    if (theme_name.toLower() == "dark") {
        applyDarkTheme(widget);
    } else {
        applyLightTheme(widget);
    }
}

void ThemeManager::applyDarkTheme(QWidget* widget) {
    widget->setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
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
        QLineEdit {
            background-color: #3c3c3c;
            border: 2px solid #555555;
            border-radius: 5px;
            padding: 8px;
            color: #ffffff;
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
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #106ebe;
        }
        QGroupBox {
            border: 2px solid #555555;
            border-radius: 5px;
            margin-top: 10px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QComboBox {
            background-color: #3c3c3c;
            border: 2px solid #555555;
            border-radius: 5px;
            padding: 8px;
            color: #ffffff;
        }
        QTextEdit {
            background-color: #3c3c3c;
            border: 2px solid #555555;
            border-radius: 5px;
            color: #ffffff;
        }
    )");
}

void ThemeManager::applyLightTheme(QWidget* widget) {
    widget->setStyleSheet(R"(
        QMainWindow {
            background-color: #ffffff;
            color: #000000;
        }
        QWidget {
            background-color: #ffffff;
            color: #000000;
        }
        QLineEdit {
            background-color: #ffffff;
            border: 2px solid #cccccc;
            border-radius: 5px;
            padding: 8px;
            color: #000000;
        }
        QPushButton {
            background-color: #0078d4;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #106ebe;
        }
    )");
}