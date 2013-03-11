#ifndef FAKE_KXMLGUIWINDOW_H
#define FAKE_KXMLGUIWINDOW_H

#include <QObject>
#include <QDebug>
#include <kxmlguiclient.h>
#include <kmainwindow.h>

class KXmlGuiWindow : public KMainWindow, public KXMLGUIBuilder, virtual public KXMLGUIClient
{
public:
    KXmlGuiWindow( QWidget* parent = 0 ) : KMainWindow(parent), KXMLGUIBuilder(), KXMLGUIClient() {}
    void setHelpMenuEnabled(bool showHelpMenu = true) {}
    bool isHelpMenuEnabled() const { return false; }
    virtual KXMLGUIFactory *guiFactory() { return 0; }
    void createGUI( const QString &xmlfile = QString() ) {}
    void setStandardToolBarMenuEnabled( bool enable ) {}
    bool isStandardToolBarMenuEnabled() const { return false; }
    void createStandardStatusBarAction() {}

    enum StandardWindowOption
    {
        ToolBar = 1,
        Keys = 2,
        StatusBar = 4,
        Save = 8,
        Create = 16,
        Default = ToolBar | Keys | StatusBar | Save | Create
    };
    Q_DECLARE_FLAGS(StandardWindowOptions, StandardWindowOption)

    void setupGUI( StandardWindowOptions options = Default, const QString& xmlfile = QString() ) {}
    void setupGUI( const QSize& defaultSize, StandardWindowOptions options = Default, const QString& xmlfile = QString() ) {}
    QAction *toolBarMenuAction() { return 0; }
    void setupToolbarMenuActions() {}

    virtual void finalizeGUI( KXMLGUIClient *client ) {}
    void finalizeGUI( bool force ) {}

//public Q_SLOTS:
    virtual void configureToolbars() {}
    virtual void slotStateChanged(const QString &newstate) {}
    void slotStateChanged(const QString &newstate, bool reverse) {}
};

#endif
