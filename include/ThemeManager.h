#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>

class ThemeManager : public QObject {
    Q_OBJECT
public:
    explicit ThemeManager(QObject* parent = nullptr);
    void applyTheme(QWidget* widget, const QString& theme_name);
    void applyDarkTheme(QWidget* widget);
    void applyLightTheme(QWidget* widget);
};

#endif // THEMEMANAGER_H