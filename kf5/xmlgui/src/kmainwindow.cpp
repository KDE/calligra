 /* This file is part of the KDE libraries
     Copyright
     (C) 2000 Reginald Stadlbauer (reggie@kde.org)
     (C) 1997 Stephan Kulow (coolo@kde.org)
     (C) 1997-2000 Sven Radej (radej@kde.org)
     (C) 1997-2000 Matthias Ettrich (ettrich@kde.org)
     (C) 1999 Chris Schlaeger (cs@kde.org)
     (C) 2002 Joseph Wenninger (jowenn@kde.org)
     (C) 2005-2006 Hamish Rodda (rodda@kde.org)
     (C) 2000-2008 David Faure (faure@kde.org)

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License version 2 as published by the Free Software Foundation.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.

     You should have received a copy of the GNU Library General Public License
     along with this library; see the file COPYING.LIB.  If not, write to
     the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
     Boston, MA 02110-1301, USA.
 */

#include "kmainwindow.h"

#include "kmainwindow_p.h"
#include "kmainwindowiface_p.h"
#include "ktoolbarhandler_p.h"
#include "khelpmenu.h"
#include "ktoolbar.h"

#include <QApplication>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QLayout>
#include <QMenuBar>
#include <QSessionManager>
#include <QStatusBar>
#include <QStyle>
#include <QWidget>

#include <ktoggleaction.h>
#include <kaboutdata.h>
#include <kconfig.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>
#include <kwindowsystem.h>
#include <kconfiggroup.h>
#include <kwindowconfig.h>
#include <kconfiggui.h>

//#include <ctype.h>

static QMenuBar *internalMenuBar(KMainWindow *mw)
{
    return mw->findChild<QMenuBar *>(QString(), Qt::FindDirectChildrenOnly);
}

static QStatusBar *internalStatusBar(KMainWindow *mw)
{
    return mw->findChild<QStatusBar *>(QString(), Qt::FindDirectChildrenOnly);
}

/**

 * Listens to resize events from QDockWidgets. The KMainWindow
 * settings are set as dirty, as soon as at least one resize
 * event occurred. The listener is attached to the dock widgets
 * by dock->installEventFilter(dockResizeListener) inside
 * KMainWindow::event().
 */
class DockResizeListener : public QObject
{
public:
    DockResizeListener(KMainWindow *win);
    virtual ~DockResizeListener();
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    KMainWindow *m_win;
};

DockResizeListener::DockResizeListener(KMainWindow *win) :
    QObject(win),
    m_win(win)
{
}

DockResizeListener::~DockResizeListener()
{
}

bool DockResizeListener::eventFilter(QObject *watched, QEvent *event)
{
    switch( event->type() ) {
    case QEvent::Resize:
    case QEvent::Move:
    case QEvent::Hide:
        m_win->k_ptr->setSettingsDirty(KMainWindowPrivate::CompressCalls);
        break;

    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}

KMWSessionManager::KMWSessionManager()
{
    connect(qApp, SIGNAL(saveStateRequest(QSessionManager&)),
            this, SLOT(saveState(QSessionManager&)));
}

KMWSessionManager::~KMWSessionManager()
{
}

bool KMWSessionManager::saveState(QSessionManager&)
{
    KConfig* config = KConfigGui::sessionConfig();
    if ( KMainWindow::memberList().count() ){
        // According to Jochen Wilhelmy <digisnap@cs.tu-berlin.de>, this
        // hook is useful for better document orientation
        KMainWindow::memberList().first()->saveGlobalProperties(config);
    }

    int n = 0;
    foreach (KMainWindow* mw, KMainWindow::memberList()) {
        n++;
        mw->savePropertiesInternal(config, n);
    }

    KConfigGroup group( config, "Number" );
    group.writeEntry("NumberOfWindows", n );
    return true;
}

Q_GLOBAL_STATIC(KMWSessionManager, ksm)
Q_GLOBAL_STATIC(QList<KMainWindow*>, sMemberList)

KMainWindow::KMainWindow( QWidget* parent, Qt::WindowFlags f )
    : QMainWindow(parent, f), k_ptr(new KMainWindowPrivate)
{
    k_ptr->init(this);
}

KMainWindow::KMainWindow(KMainWindowPrivate &dd, QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f), k_ptr(&dd)
{
    k_ptr->init(this);
}

void KMainWindowPrivate::init(KMainWindow *_q)
{
    // We set allow quit to true when the first mainwindow is created, so that when the refcounting
    // reaches 0 the application can quit. We don't want this to happen before the first mainwindow
    // is created, otherwise running a job in main would exit the app too early.
    QCoreApplication::setQuitLockEnabled(true);

    q = _q;

#if 0
    q->setAnimated(KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects);
#endif

    q->setAttribute( Qt::WA_DeleteOnClose );

    // TODO QT5: check if this call is still necessary
    // We handle this functionality (quitting the app) ourselves, with QEventLoopLocker
    // This makes apps stay alive even if they only have a systray icon visible, or
    // a progress widget with "keep open" checked, for instance.
    // So don't let the default Qt mechanism allow any toplevel widget to just quit the app on us.
    // Setting WA_QuitOnClose to false for all KMainWindows is not enough, any progress widget
    // or dialog box would still quit the app...
    if (qApp)
        qApp->setQuitOnLastWindowClosed(false);

    helpMenu = 0;

    //actionCollection()->setWidget( this );
    QObject::connect(qApp, SIGNAL(aboutToQuit()), q, SLOT(_k_shuttingDown()));
#if 0
    QObject::connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)),
                     q, SLOT(_k_slotSettingsChanged(int)));
#endif

    // force KMWSessionManager creation
    ksm();

    sMemberList()->append( q );

    settingsDirty = false;
    autoSaveSettings = false;
    autoSaveWindowSize = true; // for compatibility
    //d->kaccel = actionCollection()->kaccel();
    settingsTimer = 0;
    sizeTimer = 0;
    shuttingDown = false;

    dockResizeListener = new DockResizeListener(_q);
    letDirtySettings = true;

    sizeApplied = false;
}

static bool endsWithHashNumber( const QString& s )
{
    for( int i = s.length() - 1;
         i > 0;
         --i )
    {
        if( s[ i ] == QLatin1Char('#') && i != s.length() - 1 )
            return true; // ok
        if( !s[ i ].isDigit())
            break;
    }
    return false;
}

static inline bool isValidDBusObjectPathCharacter(const QChar &c)
{
    register ushort u = c.unicode();
    return (u >= QLatin1Char('a') && u <= QLatin1Char('z'))
            || (u >= QLatin1Char('A') && u <= QLatin1Char('Z'))
            || (u >= QLatin1Char('0') && u <= QLatin1Char('9'))
            || (u == QLatin1Char('_')) || (u == QLatin1Char('/'));
}

void KMainWindowPrivate::polish(KMainWindow *q)
{
    // Set a unique object name. Required by session management, window management, and for the dbus interface.
    QString objname;
    QString s;
    int unusedNumber = 1;
    const QString name = q->objectName();
    bool startNumberingImmediately = true;
    bool tryReuse = false;
    if ( name.isEmpty() )
    {   // no name given
        objname = QStringLiteral("MainWindow#");
    }
    else if( name.endsWith( QLatin1Char( '#' ) ) )
    {   // trailing # - always add a number  - KWin uses this for better grouping
        objname = name;
    }
    else if( endsWithHashNumber( name ))
    {   // trailing # with a number - like above, try to use the given number first
        objname = name;
        tryReuse = true;
        startNumberingImmediately = false;
    }
    else
    {
        objname = name;
        startNumberingImmediately = false;
    }

    s = objname;
    if ( startNumberingImmediately )
        s += QLatin1Char('1');

    for(;;) {
        const QList<QWidget*> list = qApp->topLevelWidgets();
        bool found = false;
        foreach ( QWidget* w, list ) {
            if( w != q && w->objectName() == s )
            {
                found = true;
                break;
            }
        }
        if( !found )
            break;
        if( tryReuse ) {
            objname = name.left( name.length() - 1 ); // lose the hash
            unusedNumber = 0; // start from 1 below
            tryReuse = false;
        }
        s.setNum( ++unusedNumber );
        s = objname + s;
    }
    q->setObjectName( s );
    q->winId(); // workaround for setWindowRole() crashing, and set also window role, just in case TT
    q->setWindowRole( s ); // will keep insisting that object name suddenly should not be used for window role

    dbusName = QLatin1Char('/') + QCoreApplication::applicationName() + QLatin1Char('/');
    dbusName += q->objectName().replace(QLatin1Char('/'), QLatin1Char('_'));
    // Clean up for dbus usage: any non-alphanumeric char should be turned into '_'
    const int len = dbusName.length();
    for ( int i = 0; i < len; ++i ) {
        if ( !isValidDBusObjectPathCharacter( dbusName[i] ) )
            dbusName[i] = QLatin1Char('_');
    }

    QDBusConnection::sessionBus().registerObject(dbusName, q, QDBusConnection::ExportScriptableSlots |
                                       QDBusConnection::ExportScriptableProperties |
                                       QDBusConnection::ExportNonScriptableSlots |
                                       QDBusConnection::ExportNonScriptableProperties |
                                       QDBusConnection::ExportAdaptors);
}

void KMainWindowPrivate::setSettingsDirty(CallCompression callCompression)
{
    if (!letDirtySettings) {
        return;
    }

    settingsDirty = true;
    if (autoSaveSettings) {
        if (callCompression == CompressCalls) {
            if (!settingsTimer) {
                settingsTimer = new QTimer(q);
                settingsTimer->setInterval(500);
                settingsTimer->setSingleShot(true);
                QObject::connect(settingsTimer, SIGNAL(timeout()), q, SLOT(saveAutoSaveSettings()));
            }
            settingsTimer->start();
        } else {
            q->saveAutoSaveSettings();
        }
    }
}

void KMainWindowPrivate::setSizeDirty()
{
    if (autoSaveWindowSize) {
        if (!sizeTimer) {
            sizeTimer = new QTimer(q);
            sizeTimer->setInterval(500);
            sizeTimer->setSingleShot(true);
            QObject::connect(sizeTimer, SIGNAL(timeout()), q, SLOT(_k_slotSaveAutoSaveSize()));
        }
        sizeTimer->start();
    }
}

KMainWindow::~KMainWindow()
{
    sMemberList()->removeAll( this );
    delete static_cast<QObject *>(k_ptr->dockResizeListener);  //so we don't get anymore events after k_ptr is destroyed
    delete k_ptr;
}

QMenu* KMainWindow::helpMenu( const QString &aboutAppText, bool showWhatsThis )
{
    K_D(KMainWindow);
    if(!d->helpMenu) {
        if ( aboutAppText.isEmpty() )
            d->helpMenu = new KHelpMenu( this, KAboutData::applicationData(), showWhatsThis);
        else
            d->helpMenu = new KHelpMenu( this, aboutAppText, showWhatsThis );

        if (!d->helpMenu)
            return 0;
    }

    return d->helpMenu->menu();
}

QMenu* KMainWindow::customHelpMenu( bool showWhatsThis )
{
    K_D(KMainWindow);
    if (!d->helpMenu) {
        d->helpMenu = new KHelpMenu( this, QString(), showWhatsThis );
        connect(d->helpMenu, SIGNAL(showAboutApplication()),
                this, SLOT(showAboutApplication()));
    }

    return d->helpMenu->menu();
}

bool KMainWindow::canBeRestored( int number )
{
    if ( !qApp->isSessionRestored() )
        return false;
    KConfig *config = KConfigGui::sessionConfig();
    if ( !config )
        return false;

    KConfigGroup group( config, QStringLiteral("Number") );
    const int n = group.readEntry( QStringLiteral("NumberOfWindows"), 1 );
    return number >= 1 && number <= n;
}

const QString KMainWindow::classNameOfToplevel( int number )
{
    if ( !qApp->isSessionRestored() )
        return QString();
    KConfig *config = KConfigGui::sessionConfig();
    if ( !config )
        return QString();
    QString s;
    s.setNum( number );
    s.prepend( QStringLiteral("WindowProperties") );

    KConfigGroup group( config, s );
    if ( !group.hasKey( QStringLiteral("ClassName") ) )
        return QString();
    else
        return group.readEntry( QStringLiteral("ClassName") );
}

bool KMainWindow::restore( int number, bool show )
{
    if ( !canBeRestored( number ) )
        return false;
    KConfig *config = KConfigGui::sessionConfig();
    if ( readPropertiesInternal( config, number ) ){
        if ( show )
            KMainWindow::show();
        return false;
    }
    return false;
}

void KMainWindow::setCaption( const QString &caption )
{
    setPlainCaption( caption );
}

void KMainWindow::setCaption( const QString &caption, bool modified )
{
    setPlainCaption( caption );
    setWindowModified( modified );
}

void KMainWindow::setPlainCaption( const QString &caption )
{
	setWindowTitle(caption);
}

void KMainWindow::appHelpActivated( void )
{
    K_D(KMainWindow);
    if( !d->helpMenu ) {
        d->helpMenu = new KHelpMenu( this );
        if ( !d->helpMenu )
            return;
    }
    d->helpMenu->appHelpActivated();
}

void KMainWindow::closeEvent ( QCloseEvent *e )
{
    K_D(KMainWindow);

    // Save settings if auto-save is enabled, and settings have changed
    if (d->settingsTimer && d->settingsTimer->isActive()) {
        d->settingsTimer->stop();
        saveAutoSaveSettings();
    }
    if (d->sizeTimer && d->sizeTimer->isActive()) {
        d->sizeTimer->stop();
        d->_k_slotSaveAutoSaveSize();
    }

    if (queryClose()) {
        e->accept();

        int not_withdrawn = 0;
        foreach (KMainWindow* mw, KMainWindow::memberList()) {
            if ( !mw->isHidden() && mw->isTopLevel() && mw != this )
                not_withdrawn++;
        }

        if ( not_withdrawn <= 0 ) { // last window close accepted?
            if ( !queryExit() || d->shuttingDown ) {
              // cancel closing, it's stupid to end up with no windows at all....
              e->ignore();
            }
        }
    } else e->ignore(); //if the window should not be closed, don't close it
}

bool KMainWindow::queryExit()
{
    return true;
}

bool KMainWindow::queryClose()
{
    return true;
}

void KMainWindow::saveGlobalProperties( KConfig*  )
{
}

void KMainWindow::readGlobalProperties( KConfig*  )
{
}

void KMainWindow::showAboutApplication()
{
}

void KMainWindow::savePropertiesInternal( KConfig *config, int number )
{
    K_D(KMainWindow);
    const bool oldASWS = d->autoSaveWindowSize;
    d->autoSaveWindowSize = true; // make saveMainWindowSettings save the window size

    QString s;
    s.setNum(number);
    s.prepend(QStringLiteral("WindowProperties"));
    KConfigGroup cg(config, s);

    // store objectName, className, Width and Height  for later restoring
    // (Only useful for session management)
    cg.writeEntry("ObjectName", objectName());
    cg.writeEntry("ClassName", metaObject()->className());

    saveMainWindowSettings(cg); // Menubar, statusbar and Toolbar settings.

    s.setNum(number);
    cg = KConfigGroup(config, s);
    saveProperties(cg);

    d->autoSaveWindowSize = oldASWS;
}

void KMainWindow::saveMainWindowSettings(KConfigGroup &cg)
{
    K_D(KMainWindow);
    //qDebug(200) << "KMainWindow::saveMainWindowSettings " << cg.name();

    // Called by session management - or if we want to save the window size anyway
    if ( d->autoSaveWindowSize )
        KWindowConfig::saveWindowSize( this, cg );

    // One day will need to save the version number, but for now, assume 0
    // Utilise the QMainWindow::saveState() functionality.
    const QByteArray state = saveState();
    cg.writeEntry("State", state.toBase64());

    QStatusBar* sb = internalStatusBar(this);
    if (sb) {
       if(!cg.hasDefault("StatusBar") && !sb->isHidden() )
           cg.revertToDefault("StatusBar");
       else
           cg.writeEntry("StatusBar", sb->isHidden() ? "Disabled" : "Enabled");
    }

    QMenuBar* mb = internalMenuBar(this);
    if (mb) {
       const QString MenuBar = QStringLiteral("MenuBar");
       if(!cg.hasDefault("MenuBar") && !mb->isHidden() )
           cg.revertToDefault("MenuBar");
       else
           cg.writeEntry("MenuBar", mb->isHidden() ? "Disabled" : "Enabled");
    }

    if ( !autoSaveSettings() || cg.name() == autoSaveGroup() ) {
        // TODO should be cg == d->autoSaveGroup, to compare both kconfig and group name
        if(!cg.hasDefault("ToolBarsMovable") && !KToolBar::toolBarsLocked())
            cg.revertToDefault("ToolBarsMovable");
        else
            cg.writeEntry("ToolBarsMovable", KToolBar::toolBarsLocked() ? "Disabled" : "Enabled");
    }

    int n = 1; // Toolbar counter. toolbars are counted from 1,
    foreach (KToolBar* toolbar, toolBars()) {
        QString group(QStringLiteral("Toolbar"));
        // Give a number to the toolbar, but prefer a name if there is one,
        // because there's no real guarantee on the ordering of toolbars
        group += (toolbar->objectName().isEmpty() ? QString::number(n) : QLatin1Char(' ')+toolbar->objectName());

        KConfigGroup toolbarGroup(&cg, group);
        toolbar->saveSettings(toolbarGroup);
        n++;
    }
}

bool KMainWindow::readPropertiesInternal( KConfig *config, int number )
{
    K_D(KMainWindow);

    const bool oldLetDirtySettings = d->letDirtySettings;
    d->letDirtySettings = false;

    if ( number == 1 )
        readGlobalProperties( config );

    // in order they are in toolbar list
    QString s;
    s.setNum(number);
    s.prepend(QStringLiteral("WindowProperties"));

    KConfigGroup cg(config, s);

    // restore the object name (window role)
    if ( cg.hasKey("ObjectName" ) )
        setObjectName(cg.readEntry("ObjectName"));

    d->sizeApplied = false; // since we are changing config file, reload the size of the window
                            // if necessary. Do it before the call to applyMainWindowSettings.
    applyMainWindowSettings(cg); // Menubar, statusbar and toolbar settings.

    s.setNum(number);
    KConfigGroup grp(config, s);
    readProperties(grp);

    d->letDirtySettings = oldLetDirtySettings;

    return true;
}

void KMainWindow::applyMainWindowSettings(const KConfigGroup &cg, bool force)
{
    K_D(KMainWindow);
    //qDebug(200) << "KMainWindow::applyMainWindowSettings " << cg.name();

    QWidget *focusedWidget = QApplication::focusWidget();

    const bool oldLetDirtySettings = d->letDirtySettings;
    d->letDirtySettings = false;

    if (!d->sizeApplied) {
        KWindowConfig::restoreWindowSize(this, cg);
        d->sizeApplied = true;
    }

    QStatusBar* sb = internalStatusBar(this);
    if (sb) {
        QString entry = cg.readEntry("StatusBar", "Enabled");
        if ( entry == QStringLiteral("Disabled") )
           sb->hide();
        else
           sb->show();
    }

    QMenuBar* mb = internalMenuBar(this);
    if (mb) {
        QString entry = cg.readEntry ("MenuBar", "Enabled");
        if ( entry == QStringLiteral("Disabled") )
           mb->hide();
        else
           mb->show();
    }

    if ( !autoSaveSettings() || cg.name() == autoSaveGroup() ) { // TODO should be cg == d->autoSaveGroup, to compare both kconfig and group name
        QString entry = cg.readEntry ("ToolBarsMovable", "Disabled");
        if ( entry == QStringLiteral("Disabled") )
            KToolBar::setToolBarsLocked(true);
        else
            KToolBar::setToolBarsLocked(false);
    }

    int n = 1; // Toolbar counter. toolbars are counted from 1,
    foreach (KToolBar* toolbar, toolBars()) {
        QString group(QStringLiteral("Toolbar"));
        // Give a number to the toolbar, but prefer a name if there is one,
        // because there's no real guarantee on the ordering of toolbars
        group += (toolbar->objectName().isEmpty() ? QString::number(n) : QLatin1Char(' ')+toolbar->objectName());

        KConfigGroup toolbarGroup(&cg, group);
        toolbar->applySettings(toolbarGroup, force);
        n++;
    }

    QByteArray state;
    if (cg.hasKey("State")) {
        state = cg.readEntry("State", state);
        state = QByteArray::fromBase64(state);
        // One day will need to load the version number, but for now, assume 0
        restoreState(state);
    }

    if (focusedWidget) {
        focusedWidget->setFocus();
    }

    d->settingsDirty = false;
    d->letDirtySettings = oldLetDirtySettings;
}

#ifndef KDE_NO_DEPRECATED
void KMainWindow::restoreWindowSize( const KConfigGroup & cg )
{
    KWindowConfig::restoreWindowSize(this, cg);
}
#endif

#ifndef KDE_NO_DEPRECATED
void KMainWindow::saveWindowSize( KConfigGroup & cg ) const
{
    KWindowConfig::saveWindowSize(this, cg);
}
#endif

void KMainWindow::setSettingsDirty()
{
    K_D(KMainWindow);
    d->setSettingsDirty();
}

bool KMainWindow::settingsDirty() const
{
    K_D(const KMainWindow);
    return d->settingsDirty;
}

void KMainWindow::setAutoSaveSettings( const QString & groupName, bool saveWindowSize )
{
    setAutoSaveSettings(KConfigGroup(KSharedConfig::openConfig(), groupName), saveWindowSize);
}

void KMainWindow::setAutoSaveSettings( const KConfigGroup & group,
                                       bool saveWindowSize )
{
    K_D(KMainWindow);
    d->autoSaveSettings = true;
    d->autoSaveGroup = group;
    d->autoSaveWindowSize = saveWindowSize;

    if (!saveWindowSize && d->sizeTimer) {
        d->sizeTimer->stop();
    }

    // Now read the previously saved settings
    applyMainWindowSettings(d->autoSaveGroup);
}

void KMainWindow::resetAutoSaveSettings()
{
    K_D(KMainWindow);
    d->autoSaveSettings = false;
    if (d->settingsTimer) {
        d->settingsTimer->stop();
    }
}

bool KMainWindow::autoSaveSettings() const
{
    K_D(const KMainWindow);
    return d->autoSaveSettings;
}

QString KMainWindow::autoSaveGroup() const
{
    K_D(const KMainWindow);
    return d->autoSaveSettings ? d->autoSaveGroup.name() : QString();
}

KConfigGroup KMainWindow::autoSaveConfigGroup() const
{
    K_D(const KMainWindow);
    return d->autoSaveSettings ? d->autoSaveGroup : KConfigGroup();
}

void KMainWindow::saveAutoSaveSettings()
{
    K_D(KMainWindow);
    Q_ASSERT( d->autoSaveSettings );
    //qDebug(200) << "KMainWindow::saveAutoSaveSettings -> saving settings";
    saveMainWindowSettings(d->autoSaveGroup);
    d->autoSaveGroup.sync();
    d->settingsDirty = false;
}

bool KMainWindow::event( QEvent* ev )
{
    K_D(KMainWindow);
    switch( ev->type() ) {
#ifdef Q_OS_WIN
    case QEvent::Move:
#endif
    case QEvent::Resize:
        d->setSizeDirty();
        break;
    case QEvent::Polish:
        d->polish(this);
        break;
    case QEvent::ChildPolished:
        {
            QChildEvent *event = static_cast<QChildEvent*>(ev);
            QDockWidget *dock = qobject_cast<QDockWidget*>(event->child());
            KToolBar *toolbar = qobject_cast<KToolBar*>(event->child());
            QMenuBar *menubar = qobject_cast<QMenuBar*>(event->child());
            if (dock) {
                connect(dock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
                        this, SLOT(setSettingsDirty()));
                connect(dock, SIGNAL(visibilityChanged(bool)),
                        this, SLOT(setSettingsDirty()), Qt::QueuedConnection);
                connect(dock, SIGNAL(topLevelChanged(bool)),
                        this, SLOT(setSettingsDirty()));

                // there is no signal emitted if the size of the dock changes,
                // hence install an event filter instead
                dock->installEventFilter(k_ptr->dockResizeListener);
            } else if (toolbar) {
                // there is no signal emitted if the size of the toolbar changes,
                // hence install an event filter instead
                toolbar->installEventFilter(k_ptr->dockResizeListener);
            } else if (menubar) {
                // there is no signal emitted if the size of the menubar changes,
                // hence install an event filter instead
                menubar->installEventFilter(k_ptr->dockResizeListener);
            }
        }
        break;
    case QEvent::ChildRemoved:
        {
            QChildEvent *event = static_cast<QChildEvent*>(ev);
            QDockWidget *dock = qobject_cast<QDockWidget*>(event->child());
            KToolBar *toolbar = qobject_cast<KToolBar*>(event->child());
            QMenuBar *menubar = qobject_cast<QMenuBar*>(event->child());
            if (dock) {
                disconnect(dock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
                           this, SLOT(setSettingsDirty()));
                disconnect(dock, SIGNAL(visibilityChanged(bool)),
                           this, SLOT(setSettingsDirty()));
                disconnect(dock, SIGNAL(topLevelChanged(bool)),
                           this, SLOT(setSettingsDirty()));
                dock->removeEventFilter(k_ptr->dockResizeListener);
            } else if (toolbar) {
                toolbar->removeEventFilter(k_ptr->dockResizeListener);
            } else if (menubar) {
                menubar->removeEventFilter(k_ptr->dockResizeListener);
            }
        }
        break;
    default:
        break;
    }
    return QMainWindow::event( ev );
}

bool KMainWindow::hasMenuBar()
{
    return internalMenuBar(this);
}

void KMainWindowPrivate::_k_shuttingDown()
{
    // Needed for Qt <= 3.0.3 at least to prevent reentrancy
    // when queryExit() shows a dialog. Check before removing!
    static bool reentrancy_protection = false;
    if (!reentrancy_protection)
    {
       reentrancy_protection = true;
       shuttingDown = true;
       // call the virtual queryExit
       q->queryExit();
       reentrancy_protection = false;
    }
}

void KMainWindowPrivate::_k_slotSettingsChanged(int category)
{
    Q_UNUSED(category);

    // This slot will be called when the style KCM changes settings that need
    // to be set on the already running applications.

    // At this level (KMainWindow) the only thing we need to restore is the
    // animations setting (whether the user wants builtin animations or not).

#if 0
    q->setAnimated(KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects);
#endif
}

void KMainWindowPrivate::_k_slotSaveAutoSaveSize()
{
    if (autoSaveGroup.isValid()) {
        KWindowConfig::saveWindowSize(q, autoSaveGroup);
    }
}

KToolBar *KMainWindow::toolBar( const QString& name )
{
    QString childName = name;
    if (childName.isEmpty())
       childName = QStringLiteral("mainToolBar");

    KToolBar *tb = findChild<KToolBar*>(childName);
    if ( tb )
        return tb;

    KToolBar* toolbar = new KToolBar(childName, this); // non-XMLGUI toolbar
    return toolbar;
}

QList<KToolBar*> KMainWindow::toolBars() const
{
    QList<KToolBar*> ret;

    foreach (QObject* child, children())
        if (KToolBar* toolBar = qobject_cast<KToolBar*>(child))
            ret.append(toolBar);

    return ret;
}

QList<KMainWindow*> KMainWindow::memberList() { return *sMemberList(); }

QString KMainWindow::dbusName() const
{
    return k_func()->dbusName;
}

#include "moc_kmainwindow.cpp"

