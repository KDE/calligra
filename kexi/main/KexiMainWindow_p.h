/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2009 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIMAINWINDOW_P_H
#define KEXIMAINWINDOW_P_H

#define KEXI_NO_PROCESS_EVENTS

#ifdef KEXI_NO_PROCESS_EVENTS
# define KEXI_NO_PENDING_DIALOGS
#endif

#define PROJECT_NAVIGATOR_TABBAR_ID 0
#define PROPERTY_EDITOR_TABBAR_ID 1

#include <KToolBar>
#include <QGroupBox>
#include <QHBoxLayout>

#include <kexiutils/SmallToolButton.h>
class KexiProjectNavigator;

//! @short Main application's tabbed toolbar
class KexiTabbedToolBar : public KTabWidget
{
    Q_OBJECT
public:
    KexiTabbedToolBar(QWidget *parent);
    virtual ~KexiTabbedToolBar();

    KToolBar *createWidgetToolBar() const;

    KToolBar *toolBar(const QString& name) const;

    void appendWidgetToToolbar(const QString& name, QWidget* widget);

    void setWidgetVisibleInToolbar(QWidget* widget, bool visible);
//    void removeWidgetFromToolbar(const QString& name);

//! @todo replace with the final Actions API
    void addAction(const QString& toolBarName, QAction *action);

protected:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void leaveEvent(QEvent* event);

protected slots:
    void slotCurrentChanged(int index);
    void slotDelayedTabRaise();
    void slotSettingsChanged(int category);
    //! Used for delayed loading of the "create" toolbar. Called only once.
    void setupCreateWidgetToolbar();
private:
    void addAction(KToolBar *tbar, const char* actionName);
    void addSeparatorAndAction(KToolBar *tbar, const char* actionName);

    class Private;
    Private * const d;
};

//! @internal window container created to speedup opening new tabs
class KexiWindowContainer : public QWidget
{
public:
    KexiWindowContainer(QWidget* parent)
            : QWidget(parent)
            , window(0)
            , lyr(new QVBoxLayout(this)) {
        lyr->setContentsMargins(0, 0, 0, 0);
    }
    void setWindow(KexiWindow* w) {
        window = w;
        if (w)
            lyr->addWidget(w);
    }
    KexiWindow *window;
private:
    QVBoxLayout *lyr;
};

//! @internal
class KexiTabbedToolBar::Private
{
public:
    Private(KexiTabbedToolBar *t)
            : q(t), createWidgetToolBar(0), tabToRaise(-1)
    {
        tabRaiseTimer.setSingleShot(true);
        tabRaiseTimer.setInterval(300);
    }

    KToolBar *createToolBar(const char *name, const QString& caption)
    {
        KToolBar *tbar = new KToolBar(q);
        toolbarsForName.insert(name, tbar);
        tbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        tbar->setObjectName(name);
        q->addTab(tbar, caption);
        return tbar;
    }

    KexiTabbedToolBar *q;
    KActionCollection *ac;
    int createId;
    KToolBar *createWidgetToolBar;
    //! Used for delayed tab raising
    int tabToRaise;
    //! Used for delayed tab raising
    QTimer tabRaiseTimer;
    //! Toolbars for name
    QHash<QString, KToolBar*> toolbarsForName;
    QHash<QWidget*, QAction*> extraActions;
};

KexiTabbedToolBar::KexiTabbedToolBar(QWidget *parent)
        : KTabWidget(parent)
        , d(new Private(this))
{
    setMouseTracking(true); // for mouseMoveEvent()
    setWhatsThis(i18n("Task-based tabbed toolbar groups commands for application using tabs."));
    connect(&d->tabRaiseTimer, SIGNAL(timeout()), this, SLOT(slotDelayedTabRaise()));

    d->ac = KexiMainWindowIface::global()->actionCollection();
    const bool userMode = KexiMainWindowIface::global()->userMode();
    KToolBar *tbar;
    QAction* a;

    KexiUtils::smallFont(this/*init*/);
    slotSettingsChanged(KGlobalSettings::FontChanged);
    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), this, SLOT(slotSettingsChanged(int)));

// KToolBar* helpToolBar = new KToolBar(this);
// helpToolBar->setFont(Kexi::smallFont());
    /*
      helpToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      helpToolBar->setIconSize( QSize(IconSize(KIconLoader::Small), IconSize(KIconLoader::Small)) );
      a = d->ac->action("help_whats_this");
      helpToolBar->addAction(a);
      a = d->ac->action("help_contents");
      helpToolBar->addAction(a);
      setCornerWidget(helpToolBar, Qt::TopRightCorner);*/

    QWidget *helpWidget = new QWidget(this);
    helpWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QHBoxLayout *helpLyr = new QHBoxLayout(helpWidget);
    helpLyr->setContentsMargins(0, 0, 0, 2);
    helpLyr->setSpacing(2);
    a = d->ac->action("help_whats_this");
    helpLyr->addWidget(new KexiSmallToolButton(a, helpWidget));
    a = d->ac->action("help_contents");
    helpLyr->addWidget(new KexiSmallToolButton(a, helpWidget));
    setCornerWidget(helpWidget, Qt::TopRightCorner);

    tbar = d->createToolBar("kexi", i18nc("Application name as menu entry", "Kexi"));
    addAction(tbar, "options_configure");
    addAction(tbar, "options_configure_keybinding");
    addSeparatorAndAction(tbar, "help_about_app");
    addAction(tbar, "help_about_kde");
#ifdef KEXI_NO_REPORTBUG_COMMAND
    //remove "bug report" action to avoid confusion for with commercial technical support
    addSeparatorAndAction(tbar, "help_report_bug");
#endif
    addSeparatorAndAction(tbar, "quit");

    tbar = d->createToolBar("project", i18n("Project"));
    setCurrentWidget(tbar); // the default
    addAction(tbar, "project_new");
    addAction(tbar, "project_open");
//! @todo re-add    addAction(tbar, "project_print");
//! @todo re-add    addAction(tbar, "project_print_preview");
//! @todo re-add    addAction(tbar, "project_print_setup");
    //no "project_save" here...
//! @todo re-add    addSeparatorAndAction(tbar, "project_saveas");
//! @todo re-add    addSeparatorAndAction(tbar, "project_properties");
    addSeparatorAndAction(tbar, "project_close");

    if (!userMode) {
        d->createWidgetToolBar = d->createToolBar("create", i18n("Create"));
    }

    tbar = d->createToolBar("data", i18n("Data"));
    addAction(tbar, "edit_cut");
    addAction(tbar, "edit_copy");
//moved to project navigator    addAction(tbar, "edit_copy_special_data_table");
    addAction(tbar, "edit_paste");
    if (!userMode)
        addAction(tbar, "edit_paste_special_data_table");
    tbar->addSeparator();
// todo move undo/redo to quickbar:
/* moved to local toolbars
    addAction(tbar, "edit_undo");
    addAction(tbar, "edit_redo");
    tbar->addSeparator();*/
    addAction(tbar, "edit_find");

    tbar = d->createToolBar("external", i18n("External Data"));

    /*   QGroupBox *gbox = new QGroupBox( i18n("Import"), tbar );
      gbox->setFlat(true);
      gbox->setFont(Kexi::smallFont(this));
      tbar->addWidget( gbox );
      QVBoxLayout *gbox_lyr = new QVBoxLayout(gbox);
      gbox_lyr->setContentsMargins(0,0,0,0);
      gbox_lyr->setSpacing(2); //ok?
      QToolBar *sub_tbar = new KToolBar(gbox);
      gbox_lyr->addWidget(sub_tbar);
    */
    if (!userMode) {
        addAction(tbar, "project_import_data_table");
        addAction(tbar, "tools_import_tables");
    }
    /*   gbox = new QGroupBox( i18n("Export"), tbar );
      gbox->setFlat(true);
      gbox->setFont(Kexi::smallFont(this));
      tbar->addWidget( gbox );
      gbox_lyr = new QVBoxLayout(gbox);
      gbox_lyr->setContentsMargins(0,0,0,0);
      gbox_lyr->setSpacing(2); //ok?
      sub_tbar = new KToolBar(gbox);
      gbox_lyr->addWidget(sub_tbar);
    */
    addSeparatorAndAction(tbar, "project_export_data_table");

    tbar = d->createToolBar("tools", i18n("Tools"));
    addAction(tbar, "tools_import_project");
    addAction(tbar, "tools_compact_database");

//! @todo move to form plugin
    tbar = d->createToolBar("form", i18n("Form Design"));

//! @todo move to report plugin
    tbar = d->createToolBar("report", i18n("Report Design"));
    
// tbar = new KToolBar(this);
// addTab( tbar, i18n("Settings") );
//moved up addAction(tbar, "options_configure_keybinding");

    /*moved up
      tbar = new KToolBar(this);
      addTab( tbar, i18n("Help") );
      addAction(tbar, "help_contents");
      addAction(tbar, "help_whats_this");
      addAction(tbar, "help_report_bug");
      a = d->ac->action("help_report_bug");
      a->setIcon(KIcon("tools-report-bug"));
      addAction(tbar, "help_about_app");
      addAction(tbar, "help_about_kde");
    */

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));
    /*tmp
      QPalette pal(palette());
      QBrush bg( pal.brush( backgroundRole() ) );
      bg.setColor( Qt::red ); //pal.color( QPalette::Button ) );
      pal.setColor( QPalette::Window, Qt::red );
      setPalette( pal );*/
}

KexiTabbedToolBar::~KexiTabbedToolBar()
{
    delete d;
}

void KexiTabbedToolBar::slotSettingsChanged(int category)
{
    if (category == KGlobalSettings::FontChanged) {
        setFont(KGlobalSettings::menuFont());   // toolbar acts like a menu
    }
}

KToolBar* KexiTabbedToolBar::createWidgetToolBar() const
{
    return d->createWidgetToolBar;
}

void KexiTabbedToolBar::mouseMoveEvent(QMouseEvent* event)
{
    QPoint p = event->pos();
    int tab = tabBar()->tabAt(p);
    if (d->tabToRaise != -1 && (tab == -1 || tab == currentIndex())) {
        d->tabRaiseTimer.stop();
        d->tabToRaise = -1;
    } else if (d->tabToRaise != tab) {
        d->tabRaiseTimer.start();

        d->tabToRaise = tab;
    }
    KTabWidget::mouseMoveEvent(event);
}

void KexiTabbedToolBar::leaveEvent(QEvent* event)
{
    d->tabRaiseTimer.stop();
    d->tabToRaise = -1;
    KTabWidget::leaveEvent(event);
}

void KexiTabbedToolBar::slotCurrentChanged(int index)
{
    if (index == indexOf(d->createWidgetToolBar) && index != -1) {
        if (d->createWidgetToolBar->actions().isEmpty()) {
            QTimer::singleShot(10, this, SLOT(setupCreateWidgetToolbar()));
        }
    }
}

void KexiTabbedToolBar::setupCreateWidgetToolbar()
{
//! @todo separate core object types from custom....
    KexiPart::PartInfoList *plist = Kexi::partManager().partInfoList(); //this list is properly sorted
    foreach(KexiPart::Info *info, *plist) {
        QAction* a = d->ac->action(
                         KexiPart::nameForCreateAction(*info));
        if (a) {
            d->createWidgetToolBar->addAction(a);//->icon(), a->text());
        } else {
            //! @todo err
        }
    }
}

void KexiTabbedToolBar::slotDelayedTabRaise()
{
    QPoint p = mapFromGlobal(QCursor::pos()); // make sure cursor is still over the tab
    int tab = tabBar()->tabAt(p);
    if (tab != d->tabToRaise) {
        d->tabToRaise = -1;
    } else if (d->tabToRaise != -1) {
        setCurrentIndex(d->tabToRaise);
        d->tabToRaise = -1;
    }
}

KToolBar *KexiTabbedToolBar::toolBar(const QString& name) const
{
    return d->toolbarsForName[name];
}

void KexiTabbedToolBar::addAction(KToolBar *tbar, const char* actionName)
{
    QAction *a = d->ac->action(actionName);
    if (a)
        tbar->addAction(a);
}

void KexiTabbedToolBar::addAction(const QString& toolBarName, QAction *action)
{
    if (!action)
        return;
    KToolBar *tbar = d->toolbarsForName[toolBarName];
    if (!tbar)
        return;
    tbar->addAction(action);
}

void KexiTabbedToolBar::addSeparatorAndAction(KToolBar *tbar, const char* actionName)
{
    QAction *a = d->ac->action(actionName);
    if (a) {
        tbar->addSeparator();
        tbar->addAction(a);
    }
}

void KexiTabbedToolBar::appendWidgetToToolbar(const QString& name, QWidget* widget)
{
    KToolBar *tbar = d->toolbarsForName[name];
    if (!tbar) {
        return;
    }
    QAction *action = tbar->addWidget(widget);
    d->extraActions.insert(widget, action);
}

void KexiTabbedToolBar::setWidgetVisibleInToolbar(QWidget* widget, bool visible)
{
    QAction *action = d->extraActions[widget];
    if (!action) {
        return;
    }
    action->setVisible(visible);
}

/*
class KexiTopDockWidget : public QDockWidget
{
  public:
    KexiTopDockWidget(QWidget *parent)
     : QDockWidget(parent)
    {
      setFeatures(QDockWidget::NoDockWidgetFeatures);
      QWidget *topSpacer = new QWidget(this);
      topSpacer->setFixedHeight( 10 );//KDialog::marginHint() );
      setTitleBarWidget(topSpacer);
      m_widget = new KTabWidget(this);
      setWidget( m_widget );
//   QMenuBar *menu = new QMenuBar(m_widget);
//   menu->addAction( i18n("Open..") );
//   m_widget->addTab( menu, i18n("Project") );

      KToolBar *tbar = new KToolBar(m_widget);
      m_widget->addTab( tbar, i18n("Project") );

      tbar->addAction( KIcon("document-new"), i18n("&New...") );
      KAction* a = KStandardAction::open(0, 0, this);
      tbar->addAction( a->icon(), a->text() );
      a = KStandardAction::close(0, 0, this);
      tbar->addAction( a->icon(), a->text() );

      m_widget->addTab( new QWidget(m_widget), i18n("Create") );
      m_widget->addTab( new QWidget(m_widget), i18n("External Data") );
      m_widget->addTab( new QWidget(m_widget), i18n("Tools") );
      m_widget->addTab( new QWidget(m_widget), i18n("Help") );
    }
    ~KexiTopDockWidget()
    {
    }
  private:
    KTabWidget *m_widget;
};
*/

//! @short A widget being main part of KexiMainWindow
class KexiMainWidget : public KMainWindow
{
    Q_OBJECT
public:
    KexiMainWidget();
    virtual ~KexiMainWidget();

    void setParent(KexiMainWindow* mainWindow) {
        KMainWindow::setParent(mainWindow); m_mainWindow = mainWindow;
    }

    KexiMainWindowTabWidget* tabWidget() const {
        return m_tabWidget;
    }
protected:
    virtual bool queryClose();
    virtual bool queryExit();
protected slots:
    void slotCurrentTabIndexChanged(int index);

private:
    void setupCentralWidget();

    KexiMainWindowTabWidget* m_tabWidget;
    KexiMainWindow *m_mainWindow;
    QPointer<KexiWindow> m_previouslyActiveWindow;

    friend class KexiMainWindow;
    friend class KexiMainWindowTabWidget;
};

KexiMainWidget::KexiMainWidget()
        : KMainWindow(0, Qt::Widget)
        , m_mainWindow(0)
{
    setupCentralWidget();
}

KexiMainWidget::~KexiMainWidget()
{
}

void KexiMainWidget::setupCentralWidget()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralWidgetLyr = new QVBoxLayout(centralWidget);
    m_tabWidget = new KexiMainWindowTabWidget(centralWidget, this);
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabIndexChanged(int)));
    centralWidgetLyr->setContentsMargins(0, 0, 0, 0);
    //centralWidgetLyr->setContentsMargins( 0, KDialog::marginHint()/2, 0, 0 );
    centralWidgetLyr->addWidget(m_tabWidget);
    setCentralWidget(centralWidget);
// connect( m_tabWidget, SIGNAL( closeTab() ), m_mainWindow, SLOT(closeCurrentWindow()) );
}

bool KexiMainWidget::queryClose()
{
    return m_mainWindow ? m_mainWindow->queryClose() : true;
}

bool KexiMainWidget::queryExit()
{
    return m_mainWindow ? m_mainWindow->queryExit() : true;
}

void KexiMainWidget::slotCurrentTabIndexChanged(int index)
{
    KexiWindowContainer* cont = dynamic_cast<KexiWindowContainer*>(m_tabWidget->widget(index));
    if (! cont || (KexiWindow*)m_previouslyActiveWindow == cont->window)
        return;
    if (m_mainWindow)
        m_mainWindow->activeWindowChanged(cont->window, (KexiWindow*)m_previouslyActiveWindow);
    m_previouslyActiveWindow = cont->window;
}

//------------------------------------------

//! @internal safer dictionary
typedef QMap< int, KexiWindow* > KexiWindowDict;

//! @internal
class KexiMainWindow::Private
{
public:
    Private(KexiMainWindow* w)
//  : dialogs(401)
            : wnd(w) {
        dummy_KXMLGUIClient = new KXMLGUIClient();
        dummy_KXMLGUIFactory = new KXMLGUIFactory(0);

        actionCollection = new KActionCollection(w);
        propEditor = 0;
        propEditorDockWidget = 0;
        navDockWidget = 0;
//2.0: unused    propEditorToolWindow=0;
        propEditorTabWidget = 0;
        KexiProjectData *pdata = Kexi::startupHandler().projectData();
        userMode = Kexi::startupHandler().forcedUserMode() /* <-- simply forced the user mode */
                   /* project has 'user mode' set as default and not 'design mode' override is found: */
                   || (pdata && pdata->userMode() && !Kexi::startupHandler().forcedDesignMode());
        isProjectNavigatorVisible = Kexi::startupHandler().isProjectNavigatorVisible();
        navigator = 0;
//2.0: unused    navToolWindow=0;
        prj = 0;
        config = KGlobal::config();
        curWindowGUIClient = 0;
        curWindowViewGUIClient = 0;
        closedWindowGUIClient = 0;
        closedWindowViewGUIClient = 0;
        nameDialog = 0;
//  curWindow=0;
        m_findDialog = 0;
//2.0: unused  block_KMdiMainFrm_eventFilter=false;
        focus_before_popup = 0;
//  relationPart=0;
//moved  privateIDCounter=0;
        action_view_nav = 0;
        action_view_propeditor = 0;
        action_view_mainarea = 0;
        action_open_recent_projects_title_id = -1;
        action_open_recent_connections_title_id = -1;
        forceWindowClosing = false;
        insideCloseWindow = false;
#ifndef KEXI_NO_PENDING_DIALOGS
        actionToExecuteWhenPendingJobsAreFinished = NoAction;
#endif
//  callSlotLastChildViewClosedAfterCloseDialog=false;
//        createMenu = 0;
        showImportantInfoOnStartup = true;
//  disableErrorMessages=false;
//  last_checked_mode=0;
        propEditorDockSeparatorPos = -1;
        navDockSeparatorPos = -1;
//  navDockSeparatorPosWithAutoOpen=-1;
        wasAutoOpen = false;
        windowExistedBeforeCloseProject = false;
#ifndef KEXI_SHOW_UNIMPLEMENTED
        dummy_action = new KActionMenu(QString(), wnd);
#endif
        maximizeFirstOpenedChildFrm = false;
#ifdef HAVE_KNEWSTUFF
        newStuff = 0;
#endif
//2.0: unused  mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
        forceShowProjectNavigatorOnCreation = false;
        forceHideProjectNavigatorOnCreation = false;
        navWasVisibleBeforeProjectClosing = false;
        saveSettingsForShowProjectNavigator = true;
        propertyEditorCollapsed = false;
        enable_slotPropertyEditorVisibilityChanged = true;
    }
    ~Private() {
        qDeleteAll(m_openedCustomObjectsForItem);
        delete dummy_KXMLGUIClient;
        delete dummy_KXMLGUIFactory;
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    //! Job type. Currently used for marking items as being opened or closed.
    enum PendingJobType {
        NoJob = 0,
        WindowOpeningJob,
        WindowClosingJob
    };

    KexiWindow *openedWindowFor(const KexiPart::Item* item, PendingJobType &pendingType) {
        return openedWindowFor(item->identifier(), pendingType);
    }

    KexiWindow *openedWindowFor(int identifier, PendingJobType &pendingType) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        QHash<int, PendingJobType>::ConstIterator it = pendingWindows.find(identifier);
        if (it == pendingWindows.end())
            pendingType = NoJob;
        else
            pendingType = it.value();

        if (pendingType == WindowOpeningJob) {
            return 0;
        }
        return windows.contains(identifier) ? (KexiWindow*)windows.value(identifier) : 0;
    }
#else
    KexiWindow *openedWindowFor(const KexiPart::Item* item) {
        return openedWindowFor(item->identifier());
    }

    KexiWindow *openedWindowFor(int identifier) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        return windows.contains(identifier) ? (KexiWindow*)windows.value(identifier) : 0;
    }
#endif

    void insertWindow(KexiWindow *window) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.insert(window->id(), window);
#ifndef KEXI_NO_PENDING_DIALOGS
        pendingWindows.remove(window->id());
#endif
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    void addItemToPendingWindows(const KexiPart::Item* item, PendingJobType jobType) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        pendingWindows.insert(item->identifier(), jobType);
    }

    bool pendingWindowsExist() {
        if (pendingWindows.begin() != pendingWindows.end())
            kDebug() <<  pendingWindows.constBegin().key() << " " << (int)pendingWindows.constBegin().value();
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        return !pendingWindows.isEmpty();
    }
#endif

    void updateWindowId(KexiWindow *window, int oldItemID) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.remove(oldItemID);
#ifndef KEXI_NO_PENDING_DIALOGS
        pendingWindows.remove(oldItemID);
#endif
        windows.insert(window->id(), window);
    }

    void removeWindow(int identifier) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.remove(identifier);
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    void removePendingWindow(int identifier) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        pendingWindows.remove(identifier);
    }
#endif

    uint openedWindowsCount() {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        return windows.count();
    }

    //! Used in KexiMainWindowe::closeProject()
    void clearWindows() {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.clear();
#ifndef KEXI_NO_PENDING_DIALOGS
        pendingWindows.clear();
#endif
    }

    /* UNUSED, see KexiToggleViewModeAction
      // Toggles last checked view mode radio action, if available.
      void toggleLastCheckedMode()
      {
        if (!wnd->currentWindow())
          return;
        KToggleAction *ta = actions_for_view_modes.value( (int)wnd->currentWindow()->currentViewMode() );
        if (ta)
          ta->setChecked(true);
    //  if (!last_checked_mode)
    //   return;
    //  last_checked_mode->setChecked(true);
      }*/

    /*
    void updatePropEditorDockWidthInfo() {
        if (propEditor) {
          KDockWidget *dw = (KDockWidget *)propEditor->parentWidget();
    #if defined(KDOCKWIDGET_P)
          KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
          if (ds) {
            propEditorDockSeparatorPos = ds->separatorPosInPercent();*/
    /*    if (propEditorDockSeparatorPos<=0) {
                config->setGroup("MainWindow");
                propEditorDockSeparatorPos = config->readEntry("RightDockPosition", 80);
                ds->setSeparatorPos(propEditorDockSeparatorPos, true);
            }*/
    /*}
    #endif
    }
    }*/

    void showStartProcessMsg(const QStringList& args) {
        wnd->showErrorMessage(i18n("Could not start %1 application.", QString(KEXI_APP_NAME)),
                              i18n("Command \"%1\" failed.", args.join(" ")));
    }

    /* void hideMenuItem(const QString& menuName, const QString& itemText, bool alsoSeparator)
      {
        Q3PopupMenu *pm = popups[menuName.toLatin1()];
        if (!pm)
          return;
        const QList<QAction*> actions( pm->actions() );
        bool nowHideSeparator = false;
        foreach( QAction *action, actions ) {
          kDebug() << action->text();
          if (nowHideSeparator) {
            if (action->isSeparator())
              action->setVisible(false);
            break;
          }
          else if (action->text().toLower().trimmed() == itemText.toLower().trimmed()) {
    #ifdef __GNUC__
    #warning OK? this should only hide menu item, not global action... action->setVisible(false);
    #endif
            action->setVisible(false);
            if (alsoSeparator)
              nowHideSeparator = true; //continue look to find separtator, if present
            else
              break;
          }
        }
      }

      void disableMenuItem(const QString& menuName, const QString& itemText)
      {
    #ifdef __GNUC__
    #warning reimplement disableMenuItem()
    #else
    #pragma WARNING( reimplement disableMenuItem() )
    #endif
        Q3PopupMenu *pm = popups[menuName.toLatin1()];
        if (!pm)
          return;
        const QList<QAction*> actions( pm->actions() );
        foreach( QAction *action, actions ) {
          if (action->text().toLower().trimmed()==itemText.toLower().trimmed()) {
            action->setEnabled(false);
            break;
          }
        }
      }*/

    //! Updates Property Editor Pane's visibility for the current window and the @a viewMode view mode.
    /*! @a info can be provided to hadle cases when current window is not yet defined (in openObject()). */
    void updatePropEditorVisibility(Kexi::ViewMode viewMode, KexiPart::Info *info = 0) {
        if (!propEditorDockWidget)
            return;
        KexiWindow *currentWindow = wnd->currentWindow();
        if (!info && currentWindow) {
            info = currentWindow->part()->info();
        }
        const bool visible = (viewMode == Kexi::DesignViewMode)
            && ((currentWindow && currentWindow->propertySet()) || info->isPropertyEditorAlwaysVisibleInDesignMode());
        kDebug() << "visible == " << visible;
        enable_slotPropertyEditorVisibilityChanged = false;
        if (visible && propertyEditorCollapsed) { // used when we're switching back to a window with propeditor available but collapsed
            propEditorDockWidget->setVisible(!visible);
            setPropertyEditorTabBarVisible(true);
        }
        else {
            propEditorDockWidget->setVisible(visible);
            setPropertyEditorTabBarVisible(false);
        }
        enable_slotPropertyEditorVisibilityChanged = true;
    }

    void setPropertyEditorTabBarVisible(bool visible) {
        KMultiTabBar *mtbar = multiTabBars[KMultiTabBar::Right];
        int id = PROPERTY_EDITOR_TABBAR_ID;
        if (!visible) {
            mtbar->removeTab(id);
        }
        else if (!mtbar->tab(id)) {
            QString t(propEditorDockWidget->windowTitle());
            t.remove('&');
            mtbar->appendTab(QPixmap(), id, t);
            KMultiTabBarTab *tab = mtbar->tab(id);
            QObject::connect(tab, SIGNAL(clicked(int)), wnd, SLOT(slotMultiTabBarTabClicked(int)));
        }
    }

//2.0: unused
#if 0
    void restoreNavigatorWidth() {
#if defined(KDOCKWIDGET_P)
        if (wnd->mdiMode() == KMdi::ChildframeMode || wnd->mdiMode() == KMdi::TabPageMode) {
            KDockWidget *dw = (KDockWidget *)nav->parentWidget();
            KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
//    ds->setKeepSize(true);

            config->setGroup("MainWindow");
            if (wasAutoOpen) //(dw2->isVisible())
//    ds->setSeparatorPosInPercent( 100 * nav->width() / wnd->width() );
                ds->setSeparatorPosInPercent(
                    qMax(qMax(config->readEntry("LeftDockPositionWithAutoOpen", 20),
                              config->readEntry("LeftDockPosition", 20)), 20)
                );
            else
                ds->setSeparatorPosInPercent(
                    qMax(20, config->readEntry("LeftDockPosition", 20/* % */)));

            //   dw->resize( d->config->readEntry("LeftDockPosition", 115/* % */), dw->height() );
            //if (!wasAutoOpen) //(dw2->isVisible())
//     ds->setSeparatorPos( ds->separatorPos(), true );
        }
#endif

    }
#endif

    template<class type>
    type *openedCustomObjectsForItem(KexiPart::Item* item, const char* name) {
        if (!item || !name) {
            kWarning() <<
            "KexiMainWindow::Private::openedCustomObjectsForItem(): !item || !name";
            return 0;
        }
        QByteArray key(QByteArray::number(item->identifier()) + name);
        return qobject_cast<type*>(m_openedCustomObjectsForItem.value(key));
    }

    void addOpenedCustomObjectForItem(KexiPart::Item* item, QObject* object, const char* name) {
        QByteArray key(QByteArray::number(item->identifier()) + name);
        m_openedCustomObjectsForItem.insert(key, object);
    }

    KexiFindDialog *findDialog() {
        if (!m_findDialog) {
            m_findDialog = new KexiFindDialog(wnd);
            m_findDialog->setActions(action_edit_findnext, action_edit_findprev,
                                     action_edit_replace, action_edit_replace_all);
            /*   connect(m_findDialog, SIGNAL(findNext()), action_edit_findnext, SLOT(trigger()));
                  connect(m_findDialog, SIGNAL(find()), wnd, SLOT(slotEditFindNext()));
                  connect(m_findDialog, SIGNAL(replace()), wnd, SLOT(slotEditReplaceNext()));
                  connect(m_findDialog, SIGNAL(replaceAll()), wnd, SLOT(slotEditReplaceAll()));*/
        }
        return m_findDialog;
    }

    /*! Updates the find/replace dialog depending on the active view.
     Nothing is performed if the dialog is not instantiated yet or is invisible. */
    void updateFindDialogContents(bool createIfDoesNotExist = false) {
        if (!wnd->currentWindow())
            return;
        if (!createIfDoesNotExist && (!m_findDialog || !m_findDialog->isVisible()))
            return;
        KexiSearchAndReplaceViewInterface* iface = currentViewSupportingSearchAndReplaceInterface();
        if (!iface) {
            if (m_findDialog) {
                m_findDialog->setButtonsEnabled(false);
                m_findDialog->setLookInColumnList(QStringList(), QStringList());
            }
            return;
        }
//! @todo use ->caption() here, depending on global settings related to displaying captions
        findDialog()->setObjectNameForCaption(wnd->currentWindow()->partItem()->name());

        QStringList columnNames;
        QStringList columnCaptions;
        QString currentColumnName; // for 'look in'
        if (!iface->setupFindAndReplace(columnNames, columnCaptions, currentColumnName)) {
            m_findDialog->setButtonsEnabled(false);
            m_findDialog->setLookInColumnList(QStringList(), QStringList());
            return;
        }
        m_findDialog->setButtonsEnabled(true);

        /* //update "look in" list
          KexiTableViewColumn::List columns( dataAwareObject()->data()->columns );
          QStringList columnNames;
          QStringList columnCaptions;
          for (KexiTableViewColumn::ListIterator it(columns); it.current(); ++it) {
            if (!it.current()->visible())
              continue;
            columnNames.append( it.current()->field()->name() );
            columnCaptions.append( it.current()->captionAliasOrName() );
          }*/
        const QString prevColumnName(m_findDialog->currentLookInColumnName());
        m_findDialog->setLookInColumnList(columnNames, columnCaptions);
        m_findDialog->setCurrentLookInColumnName(prevColumnName);
    }

    //! \return the current view if it supports \a actionName, otherwise returns 0.
    KexiView *currentViewSupportingAction(const char* actionName) const {
        if (!wnd->currentWindow())
            return 0;
        KexiView *view = wnd->currentWindow()->selectedView();
        if (!view)
            return 0;
        QAction *action = view->sharedAction(actionName);
        if (!action || !action->isEnabled())
            return 0;
        return view;
    }

    //! \return the current view if it supports KexiSearchAndReplaceViewInterface.
    KexiSearchAndReplaceViewInterface* currentViewSupportingSearchAndReplaceInterface() const {
        if (!wnd->currentWindow())
            return 0;
        KexiView *view = wnd->currentWindow()->selectedView();
        if (!view)
            return 0;
        return dynamic_cast<KexiSearchAndReplaceViewInterface*>(view);
    }

    KXMLGUIClient* dummy_KXMLGUIClient;
    KXMLGUIFactory* dummy_KXMLGUIFactory;

    KexiMainWindow *wnd;
    KexiMainWidget *mainWidget;
//  KexiMainWindowTabWidget *tabWidget;
    KActionCollection *actionCollection;
    KexiStatusBar *statusBar;
    KHelpMenu *helpMenu;
    KexiProject *prj;
    KSharedConfig::Ptr config;
#ifndef KEXI_NO_CTXT_HELP
    KexiContextHelp *ctxHelp;
#endif
    KexiProjectNavigator *navigator;
    KexiTabbedToolBar *tabbedToolBar;
    KexiDockWidget *navDockWidget;
    KTabWidget *propEditorTabWidget;
    KexiDockWidget *propEditorDockWidget;
    QPointer<KexiDockableWidget> propEditorDockableWidget;
    //! poits to kexi part which has been previously used to setup proppanel's tabs using
    //! KexiPart::setupCustomPropertyPanelTabs(), in updateCustomPropertyPanelTabs().
    QPointer<KexiPart::Part> partForPreviouslySetupPropertyPanelTabs;
    QMap<KexiPart::Part*, int> recentlySelectedPropertyPanelPages;
    QPointer<KexiPropertyEditorView> propEditor;
    QPointer<KoProperty::Set> propertySet;

    KXMLGUIClient *curWindowGUIClient, *curWindowViewGUIClient,
    *closedWindowGUIClient, *closedWindowViewGUIClient;
//unused  QPointer<KexiWindow> curWindow;

    KexiNameDialog *nameDialog;

    QTimer timer; //helper timer
//  QSignalMapper *actionMapper;

//! @todo move menu handling outside
//  Q3AsciiDict<QPopupMenu> popups; //list of menu popups
//    QMenu *createMenu;

//  QString origAppCaption; //<! original application's caption (without project name)
    QString appCaptionPrefix; //<! application's caption prefix - prj name (if opened), else: null

#ifndef KEXI_SHOW_UNIMPLEMENTED
    KActionMenu *dummy_action;
#endif

    //! project menu
    KAction *action_save, *action_save_as, *action_close,
    *action_project_properties, *action_open_recent_more,
    *action_project_relations, *action_project_import_data_table,
    *action_project_export_data_table;
#ifndef KEXI_NO_QUICK_PRINTING
    KAction *action_project_print, *action_project_print_preview,
        *action_project_print_setup;
#endif
//  KRecentFilesAction *action_open_recent;
    KActionMenu *action_open_recent, *action_show_other;
//  int action_open_recent_more_id;
    int action_open_recent_projects_title_id,
    action_open_recent_connections_title_id;

    //! edit menu
    KAction *action_edit_delete, *action_edit_delete_row,
    *action_edit_cut, *action_edit_copy, *action_edit_paste,
    *action_edit_find, *action_edit_findnext, *action_edit_findprev,
    *action_edit_replace, *action_edit_replace_all,
    *action_edit_select_all,
    *action_edit_undo, *action_edit_redo,
    *action_edit_insert_empty_row,
    *action_edit_edititem, *action_edit_clear_table,
    *action_edit_paste_special_data_table,
    *action_edit_copy_special_data_table;

    //! view menu
    KAction *action_view_nav, *action_view_propeditor, *action_view_mainarea;
//! todo see FormManager::createActions() for usage
    /* UNUSED, see KexiToggleViewModeAction
        QActionGroup* action_view_mode;
        KToggleAction *action_view_data_mode, *action_view_design_mode, *action_view_text_mode;
        QHash<int, KToggleAction*> actions_for_view_modes;*/
#ifndef KEXI_NO_CTXT_HELP
    KToggleAction *action_show_helper;
#endif
    //! data menu
    KAction *action_data_save_row;
    KAction *action_data_cancel_row_changes;
    KAction *action_data_execute;

    //! format menu
    KAction *action_format_font;

    //! tools menu
    KAction *action_tools_data_migration, *action_tools_compact_database, *action_tools_data_import;
    KActionMenu *action_tools_scripts;

    //! window menu
    KAction *action_window_next, *action_window_previous;

    //! settings menu
    KAction *action_configure;

    //! for dock windows
//2.0: unused  KMdiToolViewAccessor* navToolWindow;
//2.0: unused  KMdiToolViewAccessor* propEditorToolWindow;

    QPointer<QWidget> focus_before_popup;
//  KexiRelationPart *relationPart;

//moved  int privateIDCounter; //!< counter: ID for private "document" like Relations window

    //! Set to true only in destructor, used by closeWindow() to know if
    //! user can cancel window closing. If true user even doesn't see any messages
    //! before closing a window. This is for extremely sanity... and shouldn't be even needed.
    bool forceWindowClosing;

    //! Indicates that we're inside closeWindow() method - to avoid inf. recursion
    //! on window removing
    bool insideCloseWindow;

#ifndef KEXI_NO_PENDING_DIALOGS
    //! Used in executeActionWhenPendingJobsAreFinished().
    enum ActionToExecuteWhenPendingJobsAreFinished {
        NoAction,
        QuitAction,
        CloseProjectAction
    };
    ActionToExecuteWhenPendingJobsAreFinished actionToExecuteWhenPendingJobsAreFinished;

    void executeActionWhenPendingJobsAreFinished() {
        ActionToExecuteWhenPendingJobsAreFinished a = actionToExecuteWhenPendingJobsAreFinished;
        actionToExecuteWhenPendingJobsAreFinished = NoAction;
        switch (a) {
        case QuitAction:
            qApp->quit();
            break;
        case CloseProjectAction:
            wnd->closeProject();
            break;
        default:;
        }
    }
#endif

    //! Used for delayed windows closing for 'close all'
    QList<KexiWindow*> windowsToClose;

#ifndef KEXI_NO_QUICK_PRINTING
    //! Opened page setup dialogs, used by printOrPrintPreviewForItem().
    QHash<int, KexiWindow*> pageSetupWindows;

    /*! A map from Kexi dialog to "print setup" part item's ID of the data item
     used by closeWindow() to find an ID of the data item, so the entry
     can be removed from pageSetupWindows dictionary. */
    QMap<int, int> pageSetupWindowItemID2dataItemID_map;
#endif

    //! Used in several places to show info dialog at startup (only once per session)
    //! before displaying other stuff
    bool showImportantInfoOnStartup;

//  //! Used sometimes to block showErrorMessage()
//  bool disableErrorMessages;

    //! Indicates if project is started in User Mode
    bool userMode;

    //! Indicates if project navigator should be visible
    bool isProjectNavigatorVisible;

    //! Used on opening 1st child window
    bool maximizeFirstOpenedChildFrm;

    //! Set in restoreSettings() and used in initNavigator()
    //! to customize navigator visibility on startup
    bool forceShowProjectNavigatorOnCreation;
    bool forceHideProjectNavigatorOnCreation;

    bool navWasVisibleBeforeProjectClosing;
    bool saveSettingsForShowProjectNavigator;
#ifdef HAVE_KNEWSTUFF
    KexiNewStuff  *newStuff;
#endif

    //! Used by openedCustomObjectsForItem() and addOpenedCustomObjectForItem()
    QHash<QByteArray, QObject*> m_openedCustomObjectsForItem;

    int propEditorDockSeparatorPos, navDockSeparatorPos;
// int navDockSeparatorPosWithAutoOpen;
    bool wasAutoOpen;
    bool windowExistedBeforeCloseProject;

//2.0: unused  KMdi::MdiMode mdiModeToSwitchAfterRestart;

    QMap<KMultiTabBar::KMultiTabBarPosition, KMultiTabBar*> multiTabBars;
    bool propertyEditorCollapsed;

    bool enable_slotPropertyEditorVisibilityChanged;

private:
    //! @todo move to KexiProject
    KexiWindowDict windows;
#ifndef KEXI_NO_PROCESS_EVENTS
    QHash<int, PendingJobType> pendingWindows; //!< part item identifiers for windows whoose opening has been started
    //todo(threads) QMutex dialogsMutex; //!< used for locking windows and pendingWindows dicts
#endif
    KexiFindDialog *m_findDialog;
};

#endif
