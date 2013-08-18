#ifndef FAKE_KMAINWINDOW_H
#define FAKE_KMAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDebug>

#include <kmainwindow.h>
#include <kmenu.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kglobal.h>

#include "kofake_export.h"

class KOFAKE_EXPORT KMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    KMainWindow(QWidget* parent = 0, Qt::WindowFlags f = 0) : QMainWindow(parent) {
    }
    KMenu* helpMenu( const QString &aboutAppText = QString(), bool showWhatsThis = true ) { return 0; }
    KMenu* customHelpMenu( bool showWhatsThis = true ) { return 0; }
    static bool canBeRestored( int number ) { return false; }
    static const QString classNameOfToplevel( int number );
    bool restore( int number, bool show = true ) { return false; }
    bool hasMenuBar() { return false; }
    KMenuBar *menuBar() { return 0; }
    KStatusBar *statusBar()  {
        QStatusBar *bar = QMainWindow::statusBar();
        return bar ? new KStatusBar(bar) : 0;
    }
    static QList<KMainWindow*> memberList() { return QList<KMainWindow*>(); }
    KToolBar *toolBar( const QString& name = QString() ) { return 0; }
    QList<KToolBar*> toolBars() const { return QList<KToolBar*>(); }
    void setAutoSaveSettings( const QString & groupName = QLatin1String("MainWindow"), bool saveWindowSize = true ) {}
    void setAutoSaveSettings(const KConfigGroup & group, bool saveWindowSize = true) {}
    void resetAutoSaveSettings() {}
    bool autoSaveSettings() const { return false; }
    QString autoSaveGroup() const { return QString(); }
    KConfigGroup autoSaveConfigGroup() const { return KConfigGroup(); }
    virtual void applyMainWindowSettings( const KConfigGroup &config, bool forceGlobal = false) {}
    void saveMainWindowSettings(const KConfigGroup &config) {}
    bool initialGeometrySet() const { return true; }
    void ignoreInitialGeometry() {}
    QString dbusName() const { return QString(); }

public Q_SLOTS:
    virtual void setCaption( const QString &caption ) { setWindowTitle(caption); }
    virtual void setCaption( const QString &caption, bool modified ) { setCaption(caption); }
    virtual void setPlainCaption( const QString &caption ) { setCaption(caption); }
    void appHelpActivated( void ) {}
    void setSettingsDirty() {}

protected:
    //virtual bool queryExit() { return false; }
    //virtual bool queryClose() { return false; }
    virtual void saveProperties( KConfigGroup & ) {}
    virtual void readProperties( const KConfigGroup & ) {}
    virtual void saveGlobalProperties( KConfig* sessionConfig ) {}
    virtual void readGlobalProperties( KConfig* sessionConfig ) {}
    void savePropertiesInternal( KConfig*, int ) {}
    bool readPropertiesInternal( KConfig*, int ) { return false; }
    bool settingsDirty() const { return false; }
    void saveWindowSize( const KConfigGroup &config ) const {}
    void restoreWindowSize( const KConfigGroup & config ) {}
    void parseGeometry(bool parsewidth) {}

protected Q_SLOTS:
    virtual void showAboutApplication() {}
    void saveAutoSaveSettings() {}
};

#endif
