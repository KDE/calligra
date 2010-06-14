/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2010 Jarosław Staniek <staniek@kde.org>

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

#include "KexiMainWindow.h"
#include <config-kexi.h>
#include <unistd.h>

#include <qapplication.h>
#include <qeventloop.h>
#include <qfile.h>
#include <qtimer.h>
#include <qobject.h>
#include <q3process.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qfiledialog.h>
#include <QPixmap>
#include <QFocusEvent>
#include <QTextStream>
#include <QEvent>
#include <QKeyEvent>
#include <QHash>
#include <QDockWidget>
#include <QMenuBar>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaction.h>
#include <KActionCollection>
#include <kactionmenu.h>
#include <ktoggleaction.h>
#include <klocale.h>
#include <kstandardshortcut.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kshortcutsdialog.h>
#include <kedittoolbar.h>

#include <kglobalsettings.h>
#include <ktip.h>
#include <kstandarddirs.h>
#include <kpushbutton.h>
#include <ktextbrowser.h>
#include <kiconloader.h>
#include <ktabwidget.h>
#include <kimageio.h>
#include <khelpmenu.h>
#include <kfiledialog.h>
#include <krecentdocument.h>
#include <KMenu>
#include <KXMLGUIFactory>
#include <KMultiTabBar>

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexidb/cursor.h>
#include <kexidb/dbobjectnamevalidator.h>
#include <kexidb/admin.h>
#include <kexiutils/utils.h>

#include <core/KexiWindow.h>
#include "kexiactionproxy.h"
#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartguiclient.h"
#include "kexiproject.h"
#include "kexiprojectdata.h"
#include "kexiprojectset.h"
#include "kexi.h"
#include "kexistatusbar.h"
#include "kexiinternalpart.h"
#include "kexiactioncategories.h"
#include "kexifinddialog.h"
#include "kexisearchandreplaceiface.h"
#include <kexi_global.h>

#include <widget/KexiProjectListView.h>
#include <widget/KexiPropertyEditorView.h>
#include <widget/utils/kexirecordnavigator.h>
#include <widget/utils/KexiDockableWidget.h>
#include <koproperty/EditorView.h>
#include <koproperty/Set.h>

#include "startup/KexiStartup.h"
#include "startup/KexiNewProjectWizard.h"
#include "startup/KexiStartupDialog.h"
#include "startup/KexiStartupFileWidget.h"
#include "kexinamedialog.h"
#include "printing/kexisimpleprintingpart.h"
#include "printing/kexisimpleprintingpagesetup.h"

//Extreme verbose debug
//#if defined(Q_WS_WIN)
//# include <krecentdirs.h>
//# include <win32_utils.h>
//# define KexiVDebug kDebug()
//#endif

#if !defined(KexiVDebug)
# define KexiVDebug if (0) kDebug()
#endif

//first fix the geometry
//#define KEXI_NO_CTXT_HELP 1

#ifndef KEXI_NO_CTXT_HELP
#include <kexicontexthelp.h>
#endif

#ifdef HAVE_KNEWSTUFF
#include <knewstuff/downloaddialog.h>
#include "kexinewstuff.h"
#endif

//! @todo REENABLE when blinking and dock
//! width changes will be removed in KMDI
//#define PROPEDITOR_VISIBILITY_CHANGES

//temporary fix to manage layout
//2.0: #include "ksplitter.h"
//2.0: #define KDOCKWIDGET_P 1

#ifndef KEXI_NO_FEEDBACK_AGENT
#ifdef FEEDBACK_INCLUDE
#include FEEDBACK_INCLUDE
#endif
#include <kapplication.h>
#include <kaboutdata.h>
#include <ktoolinvocation.h>
#endif

//-------------------------------------------------

//! @internal
class KexiDockWidget : public QDockWidget
{
public:
    KexiDockWidget(const QString & title, QWidget *parent)
            : QDockWidget(title, parent) {
    }
    void setSizeHint(const QSize& hint) {
        m_hint = hint;
    }
    QSize sizeHint() const {
        return m_hint.isValid() ? m_hint : QDockWidget::sizeHint();
    }
private:
    QSize m_hint;
};

//-------------------------------------------------

#include "KexiMainWindow_p.h"
#include "KexiMainWindow_p.moc"

//-------------------------------------------------

KexiMainWindowTabWidget::KexiMainWindowTabWidget(QWidget *parent, KexiMainWidget* mainWidget)
        : KTabWidget(parent)
        , m_mainWidget(mainWidget)
{
    // close-tab button:
    QToolButton* rightWidget = new QToolButton(this);
    connect(rightWidget, SIGNAL(clicked()), this, SLOT(closeTab()));
    rightWidget->setIcon(KIcon("tab-close"));
    rightWidget->setAutoRaise(true);
    rightWidget->adjustSize();
    rightWidget->setToolTip(i18n("Close the current tab"));
    setCornerWidget(rightWidget, Qt::TopRightCorner);
}

KexiMainWindowTabWidget::~KexiMainWindowTabWidget()
{
}

void KexiMainWindowTabWidget::paintEvent(QPaintEvent * event)
{
    if (count() > 0)
        KTabWidget::paintEvent(event);
    else
        QWidget::paintEvent(event);
}

void KexiMainWindowTabWidget::closeTab()
{
    dynamic_cast<KexiMainWindow*>(KexiMainWindowIface::global())->closeCurrentWindow();
}

void KexiMainWindowTabWidget::tabInserted(int index)
{
    KTabWidget::tabInserted(index);
    m_mainWidget->slotCurrentTabIndexChanged(index);
}

//-------------------------------------------------

//static
/*KexiMainWindow* KexiMainWindow::self()
{
  return kexiMainWindow;
}*/

//static
int KexiMainWindow::create(int argc, char *argv[], KAboutData* aboutdata)
{
    Kexi::initCmdLineArgs(argc, argv, aboutdata);

    bool GUIenabled = true;
    /// @note According to GCC 4.3 the following variable is not used, commented for now
    //QWidget *dummyWidget = 0; //needed to have icon for dialogs before KexiMainWindow is created
//! @todo switch GUIenabled off when needed
    KApplication* app = new KApplication(GUIenabled);

    KGlobal::locale()->insertCatalog("koffice");
    KGlobal::locale()->insertCatalog("koproperty");

#ifdef CUSTOM_VERSION
# include "custom_exec.h"
#endif

#ifdef KEXI_DEBUG_GUI
    QWidget* debugWindow = 0;
#endif
    if (GUIenabled) {
        /*2.0  dummyWidget = new QWidget();
            dummyWidget->setWindowIcon( DesktopIcon( "kexi" ) );
            QApplication::setMainWidget(dummyWidget);*/
#ifdef KEXI_DEBUG_GUI
        KConfigGroup generalGroup = KGlobal::config()->group("General");
        if (generalGroup.readEntry("ShowInternalDebugger", false)) {
            debugWindow = KexiUtils::createDebugWindow(0);
        }
#endif
    }

    tristate res = Kexi::startupHandler().init(argc, argv);
    if (!res || ~res) {
#ifdef KEXI_DEBUG_GUI
        delete debugWindow;
#endif
        delete app;
        return (~res) ? 0 : 1;
    }

    kDebug() << "startupActions OK";

    /* Exit requested, e.g. after database removing. */
    if (Kexi::startupHandler().action() == KexiStartupData::Exit) {
#ifdef KEXI_DEBUG_GUI
        delete debugWindow;
#endif
        delete app;
        return 0;
    }

    KexiMainWindow *win = new KexiMainWindow();
    QApplication::setMainWidget(win); // FIXME: Deprecated method
#ifdef KEXI_DEBUG_GUI
    //if (debugWindow)
    //debugWindow->reparent(win, QPoint(1,1));
#endif
// delete dummyWidget;

    if (true != win->startup()) {
        delete win;
        delete app;
        return 1;
    }

    win->show();
    app->processEvents();//allow refresh our app
    win->restoreSettings();
#ifdef KEXI_DEBUG_GUI
    win->raise();
    static_cast<QWidget*>(win)->activateWindow();
#endif
//#ifdef KEXI_DEBUG_GUI
// delete debugWindow;
//#endif
    return 0;
}

//-------------------------------------------------

KexiMainWindow::KexiMainWindow(QWidget *parent)
        : KexiMainWindowSuper(parent)
        , KexiMainWindowIface()
        , KexiGUIMessageHandler(this)
        , d(new KexiMainWindow::Private(this))
{
    setObjectName("KexiMainWindow");

//kde4: removed  KImageIO::registerFormats();

    if (d->userMode)
        kDebug() << "KexiMainWindow::KexiMainWindow(): starting up in the User Mode";

#ifdef __GNUC__
#warning TODO initialGeometrySet()
#else
#pragma WARNING( TODO initialGeometrySet() )
#endif
#if 0 //TODO port ??
    if (!initialGeometrySet()) {
        int scnum = QApplication::desktop()->screenNumber(parentWidget());
        QRect desk = QApplication::desktop()->screenGeometry(scnum);
        KConfigGroup mainWindowGroup = d->config->group("MainWindow");
        QSize s(
            mainWindowGroup.readEntry(QString::fromLatin1("Width %1").arg(desk.width()), 700),
            mainWindowGroup.readEntry(QString::fromLatin1("Height %1").arg(desk.height()), 480));
        resize(kMin(s.width(), desk.width()), qMin(s.height(), desk.height()));
    }
#endif

//2.0: unused setManagedDockPositionModeEnabled(true);//TODO(js): remove this if will be default in kmdi :)
//2.0: unused manager()->setSplitterHighResolution(true);
//2.0: unused manager()->setSplitterKeepSize(true);
//2.0: unused setStandardMDIMenuEnabled(false);
    setAsDefaultHost(); //this is default host now.
    KIconLoader::global()->addAppDir("kexi");
    KIconLoader::global()->addAppDir("koffice");

    //get informed
    connect(&Kexi::partManager(), SIGNAL(partLoaded(KexiPart::Part*)),
            this, SLOT(slotPartLoaded(KexiPart::Part*)));
//2.0: unused  connect( m_pMdi, SIGNAL(nowMaximized(bool)), this, SLOT(slotCaptionForCurrentMDIChild(bool)) );
//2.0: unused  connect( m_pMdi, SIGNAL(noMaximizedChildFrmLeft(KMdiChildFrm*)), this, SLOT(slotNoMaximizedChildFrmLeft(KMdiChildFrm*)));
// connect( this, SIGNAL(lastChildFrmClosed()), this, SLOT(slotLastChildFrmClosed()));
#ifdef __GNUC__
#warning TODO connect( this, SIGNAL(lastChildViewClosed()), this, SLOT(slotLastChildViewClosed()));
#else
#pragma WARNING( TODO connect( this, SIGNAL(lastChildViewClosed()), this, SLOT(slotLastChildViewClosed())); )
#endif

//2.0: unused  connect( this, SIGNAL(childViewIsDetachedNow(QWidget*)), this, SLOT(slotChildViewIsDetachedNow(QWidget*)));
//2.0: unused  connect( this, SIGNAL(mdiModeHasBeenChangedTo(KMdi::MdiMode)),
//  this, SLOT(slotMdiModeHasBeenChangedTo(KMdi::MdiMode)));


    //if (!userMode()) {
//2.0: moved to createGUI()  setXMLFile("kexiui.rc");
    setAcceptDrops(true);
    setupActions();
    setupMainWidget();
//2.0: unused  createShellGUI(true);
    //}

// d->origAppCaption = windowTitle();

    // Setup menu
// d->topDockWidget = new KexiTopDockWidget(this);
// addDockWidget(Qt::TopDockWidgetArea, d->topDockWidget, Qt::Horizontal);

//moved    restoreSettings();
    (void)KexiUtils::smallFont(this/*init*/);

    if (!d->userMode) {
        setupContextHelp();
        setupPropertyEditor();
    }

    /*2.0: unused
      KAction *kmdi_tooldock_menu_action = childClients()->getFirst() ? childClients()->getFirst()->actionCollection()->action("kmdi_tooldock_menu") : 0;
      if (kmdi_tooldock_menu_action) {
        kmdi_tooldock_menu_action->setEnabled(false);
      }*/

#ifdef __GNUC__
#warning TODO window menu
#else
#pragma WARNING( TODO window menu )
#endif
#ifdef __GNUC__
#warning TODO userMode
#else
#pragma WARNING( TODO userMode )
#endif
#if 0
    if (d->userMode) {
        //hide "insert" menu and disable "project_import", "edit_paste_special" menus
        QMenu *menu = d->popups["insert"];
        if (menu) {
            for (uint i = 0; i < menuBar()->count(); i++) {
                if (menuBar()->text(menuBar()->idAt(i)) == i18n("&Insert")) {
                    menuBar()->setItemVisible(menuBar()->idAt(i), false);
                    break;
                }
            }
        }
        d->disableMenuItem("file", i18n("&Import"));
        d->disableMenuItem("edit", i18n("Paste &Special"));
    }
#endif

//2.0: unused m_pTaskBar->setCaption(i18n("Task Bar")); //js TODO: move this to KMDIlib

// if (!d->userMode) {
    invalidateActions();
    d->timer.singleShot(0, this, SLOT(slotLastActions()));
// }

//2.0: unused setTabWidgetVisibility(KMdi::AlwaysShowTabs);
#ifdef __GNUC__
#warning TODO if (mdiMode()==KMdi::IDEAlMode) {
#else
#pragma WARNING( TODO if (mdiMode()==KMdi::IDEAlMode) { )
#endif
#if 0 //TODO
    d->config->setGroup("MainWindow");
    tabWidget()->setHoverCloseButton(d->config->readBoolEntry("HoverCloseButtonForTabs", false));
    //create special close button as corner widget for IDEAl mode
    QToolButton *closeButton = new QToolButton(tabWidget());
    closeButton->setAutoRaise(true);
    closeButton->setPixmap(QPixmap(kde2_closebutton));
    closeButton->setPaletteBackgroundColor(closeButton->palette().active().background());
//  closeButton->setIconSet(KIcon("tab-close"));
    tabWidget()->setCornerWidget(closeButton, Qt::TopRightCorner);
    closeButton->hide(); // hide until it's needed to avoid problems in "user mode"
    // when initially the main window is empty
    closeButton->setToolTip(
        i18nc("Close the current tab page in Kexi tab interface", "Close the current tab"));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(closeActiveView()));
}
#endif

#ifdef KEXI_ADD_CUSTOM_KexiMainWindow
# include "KexiMainWindow_ctor.h"
#endif

//    d->mainWidget->setAutoSaveSettings(QLatin1String("MainWindow"), /*saveWindowSize*/false);
}

KexiMainWindow::~KexiMainWindow()
{
    d->forceWindowClosing = true;
    closeProject();
    delete d;
}

KexiProject *KexiMainWindow::project()
{
    return d->prj;
}

KXMLGUIClient* KexiMainWindow::guiClient() const
{
    return d->dummy_KXMLGUIClient;
}

KXMLGUIFactory* KexiMainWindow::guiFactory()
{
    return d->dummy_KXMLGUIFactory;
}

#if 0 //------------------------

void KexiMainWindow::setWindowMenu(Q3PopupMenu *menu)
{
    delete m_pWindowMenu;
    m_pWindowMenu = menu;
    int count = menuBar()->count();
    //try to move "window" menu just before "Settings" menu (count-3)
    const QString txt = i18n("&Window");
    int i;
    for (i = 0; i < count; i++) {
        //kDebug() << menuBar()->text( menuBar()->idAt(i) );
        if (txt == menuBar()->text(menuBar()->idAt(i)))
            break;
    }
    if (i < count) {
        const int id = menuBar()->idAt(i);
        menuBar()->removeItemAt(i);
        menuBar()->insertItem(txt, m_pWindowMenu, id, count - 3);
    }
    m_pWindowMenu->setCheckable(true);
    QObject::connect(m_pWindowMenu, SIGNAL(aboutToShow()), this, SLOT(fillWindowMenu()));
}

void KexiMainWindow::fillWindowMenu()
{
    KexiMainWindow::fillWindowMenu();

    /* int i;
      for (i=0; i < (int)m_pWindowMenu->count(); i++) {
        if (m_pWindowMenu->text( m_pWindowMenu->idAt( i ) ) == i18n( "&MDI Mode" )) {
    //   kDebug() << m_pWindowMenu->text( m_pWindowMenu->idAt( i ) );
          m_pWindowMenu->removeItem( m_pWindowMenu->idAt( i ) );
          break;
        }
      }*/

    m_pMdiModeMenu->removeItem(m_pMdiModeMenu->idAt(0));     //hide toplevel mode
    m_pMdiModeMenu->removeItem(m_pMdiModeMenu->idAt(1));     //hide tabbed mode
    //update
    if (d->mdiModeToSwitchAfterRestart != (KMdi::MdiMode)0) {
        m_pMdiModeMenu->setItemChecked(m_pMdiModeMenu->idAt(0),
                                       d->mdiModeToSwitchAfterRestart == KMdi::ChildframeMode);
        m_pMdiModeMenu->setItemChecked(m_pMdiModeMenu->idAt(1),
                                       d->mdiModeToSwitchAfterRestart == KMdi::IDEAlMode);
    }

    //insert window_next, window_previous actions:
// const QString t = i18n("&Dock/Undock...");
    int i = m_pWindowMenu->count() - 1;
    for (int index;; i--) {
        index = m_pWindowMenu->idAt(i);
        if (index == -1 || m_pWindowMenu->text(index).isNull())
            break;
    }
    i++;
    d->action_window_next->plug(m_pWindowMenu, i++);
    d->action_window_previous->plug(m_pWindowMenu, i++);
    if (!m_pDocumentViews->isEmpty())
        m_pWindowMenu->addSeparator(i++);
}

void KexiMainWindow::switchToIDEAlMode()
{
    switchToIDEAlMode(true);
}

void KexiMainWindow::switchToIDEAlMode(bool showMessage)
{
    if (showMessage) {
        if ((int)d->mdiModeToSwitchAfterRestart == 0 && mdiMode() == KMdi::IDEAlMode)
            return;
        if (d->mdiModeToSwitchAfterRestart == KMdi::IDEAlMode)
            return;
        if (mdiMode() == KMdi::IDEAlMode) {//current mode
            d->mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
        } else {
            KMessageBox::information(this,
                                     i18n("User interface mode will be switched to IDEAl at next %1 application startup.")
                                     .arg(KEXI_APP_NAME));
            //delayed
            d->mdiModeToSwitchAfterRestart = KMdi::IDEAlMode;
        }
    } else
        KexiMainWindow::switchToIDEAlMode();
}

void KexiMainWindow::switchToChildframeMode()
{
    switchToChildframeMode(true);
}

void KexiMainWindow::switchToChildframeMode(bool showMessage)
{
    if (showMessage) {
        if ((int)d->mdiModeToSwitchAfterRestart == 0 && mdiMode() == KMdi::ChildframeMode)
            return;
        if (d->mdiModeToSwitchAfterRestart == KMdi::ChildframeMode)
            return;
        if (mdiMode() == KMdi::ChildframeMode) {//current mode
            d->mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
        } else {
            KMessageBox::information(this,
                                     i18n("User interface mode will be switched to Childframe at next %1 application startup.")
                                     .arg(KEXI_APP_NAME));
            //delayed
            d->mdiModeToSwitchAfterRestart = KMdi::ChildframeMode;
        }
    } else
        KexiMainWindow::switchToChildframeMode();
}

Q3PopupMenu* KexiMainWindow::findPopupMenu(const char *popupName)
{
    return d->popups[popupName];
}

#endif //0

QList<QAction*> KexiMainWindow::allActions() const
{
    return actionCollection()->actions();
}

KActionCollection *KexiMainWindow::actionCollection() const
{
    return d->actionCollection;
}

KexiWindow* KexiMainWindow::currentWindow() const
{
    if (!d->mainWidget->tabWidget())
        return 0;
    KexiWindowContainer *windowContainer
    = dynamic_cast<KexiWindowContainer*>(d->mainWidget->tabWidget()->currentWidget());
    if (!windowContainer)
        return 0;
    return windowContainer->window;
}

void KexiMainWindow::setupActions()
{
    //kde4
#ifdef __GNUC__
#warning TODO setupGUI(KMainWindow::Keys|KMainWindow::StatusBar|KMainWindow::Save|KMainWindow::Create, "kexiui.rc" );
#endif

// d->actionMapper = new QSignalMapper(this, "act_map");
// connect(d->actionMapper, SIGNAL(mapped(const QString &)), this, SLOT(slotAction(const QString &)));

    KActionCollection *ac = actionCollection();

    // PROJECT MENU
    KAction *action;
    ac->addAction("project_new",
                  action = new KAction(KIcon("document-new"), i18n("&New..."), this));
    action->setShortcut(KStandardShortcut::openNew());
    action->setToolTip(i18n("Create a new project"));
    action->setWhatsThis(
        i18n("Creates a new project. Currently opened project is not affected."));
    connect(action, SIGNAL(triggered()), this, SLOT(slotProjectNew()));

    ac->addAction("project_open",
                  action = KStandardAction::open(this, SLOT(slotProjectOpen()), this));
    action->setToolTip(i18n("Open an existing project"));
    action->setWhatsThis(
        i18n("Opens an existing project. Currently opened project is not affected."));

#ifdef HAVE_KNEWSTUFF
    ac->addAction("project_download_examples",
                  action = new KAction(KIcon("go-down"), i18n("&Download Example Databases..."), this));
    action->setToolTip(i18n("Download example databases from the Internet"));
    action->setWhatsThis(i18n("Downloads example databases from the Internet."));
    connect(action, SIGNAL(triggered()), this, SLOT(slotGetNewStuff()));
#endif

// d->action_open_recent = KStandardAction::openRecent( this, SLOT(slotProjectOpenRecent(const KUrl&)), actionCollection(), "project_open_recent" );

//#ifdef KEXI_SHOW_UNIMPLEMENTED
#ifndef KEXI_NO_UNFINISHED
    ac->addAction("project_open_recent",
                  d->action_open_recent = new KActionMenu(i18n("Open Recent"), this));
    connect(d->action_open_recent->popupMenu(), SIGNAL(activated(int)),
            this, SLOT(slotProjectOpenRecent(int)));
    connect(d->action_open_recent->popupMenu(), SIGNAL(aboutToShow()),
            this, SLOT(slotProjectOpenRecentAboutToShow()));
//moved down  d->action_open_recent_projects_title_id =
//  d->action_open_recent->popupMenu()->insertTitle(i18n("Recently Opened Databases"));
//moved down d->action_open_recent_connections_title_id =
//  d->action_open_recent->popupMenu()->insertTitle(i18n("Recently Connected Database Servers"));
// d->action_open_recent->popupMenu()->insertSeparator();
// d->action_open_recent_more_id = d->action_open_recent->popupMenu()
//  ->insertItem(i18n("&More Projects..."), this, SLOT(slotProjectOpenRecentMore()), 0, 1000);
#else
    d->action_open_recent = d->dummy_action;
#endif

    ac->addAction("project_save",
                  d->action_save = KStandardAction::save(this, SLOT(slotProjectSave()), this));
// d->action_save = new KAction(i18n("&Save"), "document-save", KStdAccel::shortcut(KStdAccel::Save),
//  this, SLOT(slotProjectSave()), actionCollection(), "project_save");
    d->action_save->setToolTip(i18n("Save object changes"));
    d->action_save->setWhatsThis(i18n("Saves object changes from currently selected window."));

#ifdef KEXI_SHOW_UNIMPLEMENTED
    ac->addAction("project_saveas",
                  d->action_save_as = new KAction(
        KIcon("document-save-as"), i18n("Save &As..."), this));
    d->action_save_as->setToolTip(i18n("Save object as"));
    d->action_save_as->setWhatsThis(
        i18n("Saves object changes from currently selected window under a new name "
             "(within the same project)."));
    connect(d->action_save_as, SIGNAL(triggered()), this, SLOT(slotProjectSaveAs()));

    ac->addAction("project_properties",
                  d->action_project_properties = new KAction(
        KIcon("document-properties"), i18n("Project Properties"), this));
    connect(d->action_project_properties, SIGNAL(triggered()),
            this, SLOT(slotProjectProperties()));
#else
    d->action_save_as = d->dummy_action;
    d->action_project_properties = d->dummy_action;
#endif

    ac->addAction("project_close",
                  d->action_close = new KAction(KIcon("window-close"), i18n("&Close Project"), this));
    d->action_close->setToolTip(i18n("Close the current project"));
    d->action_close->setWhatsThis(i18n("Closes the current project."));
    connect(d->action_close, SIGNAL(triggered()),
            this, SLOT(slotProjectClose()));

    ac->addAction("quit",
                  action = KStandardAction::quit(this, SLOT(slotProjectQuit()), this));
    action->setWhatsThis(i18n("Quits Kexi application. Kexi prompts you to save changes."));

#ifdef KEXI_SHOW_UNIMPLEMENTED
    ac->addAction("project_relations",
                  d->action_project_relations = new KAction(
        KIcon("relation"), i18n("&Relationships..."), this));
    d->action_project_relations->setShortcut(Qt::CTRL + Qt::Key_R);
    d->action_project_relations->setToolTip(i18n("Project relationships"));
    d->action_project_relations->setWhatsThis(i18n("Shows project relationships."));
    connect(d->action_project_relations, SIGNAL(triggered()),
            this, SLOT(slotProjectRelations()));

#else
    d->action_project_relations = d->dummy_action;
#endif
    ac->addAction("tools_import_project",
                  d->action_tools_data_migration = new KAction(
        KIcon("document-import-database"), i18n("&Import Database..."), this));
    d->action_tools_data_migration->setToolTip(i18n("Import entire database as a Kexi project"));
    d->action_tools_data_migration->setWhatsThis(
        i18n("Imports entire database as a Kexi project."));
    connect(d->action_tools_data_migration, SIGNAL(triggered()),
            this, SLOT(slotToolsProjectMigration()));


    d->action_tools_data_import = new KAction(KIcon("document-import"), i18n("Import Tables"), this);
    d->action_tools_data_import->setToolTip(i18n("Import data from an external source into this database"));
    d->action_tools_data_import->setWhatsThis(i18n("Import data from an external source into this database"));
    ac->addAction("tools_import_tables", d->action_tools_data_import);
    connect(d->action_tools_data_import, SIGNAL(triggered()), this, SLOT(slotToolsImportTables()));

    ac->addAction("tools_compact_database",
                  d->action_tools_compact_database = new KAction(
//! @todo icon
        KIcon("kexi"),
        i18n("&Compact Database..."), this));
    d->action_tools_compact_database->setToolTip(i18n("Compact the current database project"));
    d->action_tools_compact_database->setWhatsThis(
        i18n("Compacts the current database project, so it will take less space and work faster."));
    connect(d->action_tools_compact_database, SIGNAL(triggered()),
            this, SLOT(slotToolsCompactDatabase()));

    if (d->userMode)
        d->action_project_import_data_table = 0;
    else {
        ac->addAction("project_import_data_table",
                      d->action_project_import_data_table = new KAction(
            KIcon("table"), /*! @todo: change to "file_import" with a table or so */
            i18nc("Import->Table Data From File...", "Import Data From &File..."), this));
//orig            i18nc("Import->Table Data From File...", "Table Data From &File..."), this));
        d->action_project_import_data_table->setToolTip(i18n("Import table data from a file"));
        d->action_project_import_data_table->setWhatsThis(i18n("Imports table data from a file."));
        connect(d->action_project_import_data_table, SIGNAL(triggered()),
                this, SLOT(slotProjectImportDataTable()));
    }

    ac->addAction("project_export_data_table",
                  d->action_project_export_data_table = new KAction(
        KIcon("table"), /*! @todo: change to "file_export" with a table or so */
        i18nc("Export->Table or Query Data to File...", "Export Data to &File..."), this));
//orig:        i18nc("Export->Table or Query Data to File...", "Table or Query Data to &File..."), this));
    d->action_project_export_data_table->setToolTip(
        i18n("Export data from the active table or query data to a file"));
    d->action_project_export_data_table->setWhatsThis(
        i18n("Exports data from the active table or query data to a file."));
    connect(d->action_project_export_data_table, SIGNAL(triggered()),
            this, SLOT(slotProjectExportDataTable()));

//TODO new KAction(i18n("From File..."), "document-open", 0,
//TODO  this, SLOT(slotImportFile()), actionCollection(), "project_import_file");
//TODO new KAction(i18n("From Server..."), "network-server-database", 0,
//TODO  this, SLOT(slotImportServer()), actionCollection(), "project_import_server");

    ac->addAction("project_print",
                  d->action_project_print = KStandardAction::print(this, SLOT(slotProjectPrint()), this));
    d->action_project_print->setToolTip(i18n("Print data from the active table or query"));
    d->action_project_print->setWhatsThis(i18n("Prints data from the active table or query."));

    ac->addAction("project_print_preview",
                  d->action_project_print_preview = KStandardAction::printPreview(
                                                        this, SLOT(slotProjectPrintPreview()), this));
    d->action_project_print_preview->setToolTip(
        i18n("Show print preview for the active table or query"));
    d->action_project_print_preview->setWhatsThis(
        i18n("Shows print preview for the active table or query."));

    ac->addAction("project_print_setup",
                  d->action_project_print_setup = new KAction(
        KIcon("document-page-setup"),
        i18n("Page Set&up..."), this));
    d->action_project_print_setup->setToolTip(
        i18n("Show page setup for printing the active table or query"));
    d->action_project_print_setup->setWhatsThis(
        i18n("Shows page setup for printing the active table or query."));
    connect(d->action_project_print_setup, SIGNAL(triggered()),
            this, SLOT(slotProjectPageSetup()));

    //EDIT MENU
    d->action_edit_cut = createSharedAction(KStandardAction::Cut, "edit_cut");
    d->action_edit_copy = createSharedAction(KStandardAction::Copy, "edit_copy");
    d->action_edit_paste = createSharedAction(KStandardAction::Paste, "edit_paste");

    if (d->userMode)
        d->action_edit_paste_special_data_table = 0;
    else {
        ac->addAction("edit_paste_special_data_table",
                      d->action_edit_paste_special_data_table = new KAction(
            KIcon("table"),
            i18nc("Paste Special->As Data &Table...", "Paste Special..."), this));
//orig            i18nc("Paste Special->As Data &Table...", "As Data &Table..."), this));
        d->action_edit_paste_special_data_table->setToolTip(
            i18n("Paste clipboard data as a table"));
        d->action_edit_paste_special_data_table->setWhatsThis(
            i18n("Pastes clipboard data to a table."));
        connect(d->action_edit_paste_special_data_table, SIGNAL(triggered()),
                this, SLOT(slotEditPasteSpecialDataTable()));
    }

    ac->addAction("edit_copy_special_data_table",
                  d->action_edit_copy_special_data_table = new KAction(
        KIcon("table"),
        i18nc("Copy Special->Table or Query Data...", "Copy Special..."),
//orig        i18nc("Copy Special->Table or Query Data...", "Table or Query as Data Table..."),
        this));
    d->action_edit_copy_special_data_table->setToolTip(
        i18n("Copy selected table or query data to clipboard"));
    d->action_edit_copy_special_data_table->setWhatsThis(
        i18n("Copies selected table or query data to clipboard."));
    connect(d->action_edit_copy_special_data_table, SIGNAL(triggered()),
            this, SLOT(slotEditCopySpecialDataTable()));

    d->action_edit_undo = createSharedAction(KStandardAction::Undo, "edit_undo");
    d->action_edit_undo->setWhatsThis(i18n("Reverts the most recent editing action."));
    d->action_edit_redo = createSharedAction(KStandardAction::Redo, "edit_redo");
    d->action_edit_redo->setWhatsThis(i18n("Reverts the most recent undo action."));

#if 0 //old
    d->action_edit_find = createSharedAction(KStandardAction::Find, "edit_find");
    d->action_edit_findnext = createSharedAction(KStandardAction::FindNext, "edit_findnext");
    d->action_edit_findprev = createSharedAction(KStandardAction::FindPrev, "edit_findprevious");
//! @todo d->action_edit_paste = createSharedAction( KStandardAction::Replace, "edit_replace");
#endif

    ac->addAction("edit_find",
                  d->action_edit_find = KStandardAction::find(
                                            this, SLOT(slotEditFind()), this));
// d->action_edit_find = createSharedAction( KStandardAction::Find, "edit_find");
    ac->addAction("edit_findnext",
                  d->action_edit_findnext = KStandardAction::findNext(
                                                this, SLOT(slotEditFindNext()), this));
    ac->addAction("edit_findprevious",
                  d->action_edit_findprev = KStandardAction::findPrev(
                                                this, SLOT(slotEditFindPrevious()), this));
    d->action_edit_replace = 0;
//! @todo d->action_edit_replace = KStandardAction::replace(
//!  this, SLOT(slotEditReplace()), actionCollection(), "project_print_preview" );
    d->action_edit_replace_all = 0;
//! @todo d->action_edit_replace_all = new KAction( i18n("Replace All"), "", 0,
//!   this, SLOT(slotEditReplaceAll()), actionCollection(), "edit_replaceall");

    d->action_edit_select_all =  createSharedAction(KStandardAction::SelectAll,
                                 "edit_select_all");

    d->action_edit_delete = createSharedAction(i18n("&Delete"), "edit-delete",
                            KShortcut(), "edit_delete");
    d->action_edit_delete->setToolTip(i18n("Delete selected object"));
    d->action_edit_delete->setWhatsThis(i18n("Deletes currently selected object."));

    d->action_edit_delete_row = createSharedAction(i18n("Delete Row"), "delete_table_row",
                                KShortcut(Qt::CTRL + Qt::Key_Delete), "edit_delete_row");
    d->action_edit_delete_row->setToolTip(i18n("Delete currently selected row"));
    d->action_edit_delete_row->setWhatsThis(i18n("Deletes currently selected row."));

    d->action_edit_clear_table = createSharedAction(i18n("Clear Table Contents"),
                                 "clear_table_contents", KShortcut(), "edit_clear_table");
    d->action_edit_clear_table->setToolTip(i18n("Clear table contents"));
    d->action_edit_clear_table->setWhatsThis(i18n("Clears table contents."));
    setActionVolatile(d->action_edit_clear_table, true);

    d->action_edit_edititem = createSharedAction(i18n("Edit Item"), 0,
                              KShortcut(), /* CONFLICT in TV: Qt::Key_F2,  */
                              "edit_edititem");
    d->action_edit_edititem->setToolTip(i18n("Edit currently selected item"));
    d->action_edit_edititem->setWhatsThis(i18n("Edits currently selected item."));

    d->action_edit_insert_empty_row = createSharedAction(i18n("&Insert Empty Row"),
                                      "insert_table_row", KShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_Insert),
                                      "edit_insert_empty_row");
    setActionVolatile(d->action_edit_insert_empty_row, true);
    d->action_edit_insert_empty_row->setToolTip(i18n("Insert one empty row above"));
    d->action_edit_insert_empty_row->setWhatsThis(
        i18n("Inserts one empty row above currently selected table row."));

    //VIEW MENU
    /* UNUSED, see KexiToggleViewModeAction
      if (!d->userMode) {
        d->action_view_mode = new QActionGroup(this);
        ac->addAction( "view_data_mode",
          d->action_view_data_mode = new KToggleAction(
            KIcon("state_data"), i18n("&Data View"), d->action_view_mode) );
    //  d->action_view_data_mode->setObjectName("view_data_mode");
        d->action_view_data_mode->setShortcut(Qt::Key_F6);
        //d->action_view_data_mode->setExclusiveGroup("view_mode");
        d->action_view_data_mode->setToolTip(i18n("Switch to data view"));
        d->action_view_data_mode->setWhatsThis(i18n("Switches to data view."));
        d->actions_for_view_modes.insert( Kexi::DataViewMode, d->action_view_data_mode );
        connect(d->action_view_data_mode, SIGNAL(triggered()),
          this, SLOT(slotViewDataMode()));
      }
      else {
        d->action_view_mode = 0;
        d->action_view_data_mode = 0;
      }

      if (!d->userMode) {
        ac->addAction( "view_design_mode",
          d->action_view_design_mode = new KToggleAction(
            KIcon("state_edit"), i18n("D&esign View"), d->action_view_mode) );
    //  d->action_view_design_mode->setObjectName("view_design_mode");
        d->action_view_design_mode->setShortcut(Qt::Key_F7);
        //d->action_view_design_mode->setExclusiveGroup("view_mode");
        d->action_view_design_mode->setToolTip(i18n("Switch to design view"));
        d->action_view_design_mode->setWhatsThis(i18n("Switches to design view."));
        d->actions_for_view_modes.insert( Kexi::DesignViewMode, d->action_view_design_mode );
        connect(d->action_view_design_mode, SIGNAL(triggered()),
          this, SLOT(slotViewDesignMode()));
      }
      else
        d->action_view_design_mode = 0;

      if (!d->userMode) {
        ac->addAction( "view_text_mode",
          d->action_view_text_mode = new KToggleAction(
            KIcon("state_sql"), i18n("&Text View"), d->action_view_mode) );
        d->action_view_text_mode->setObjectName("view_text_mode");
        d->action_view_text_mode->setShortcut(Qt::Key_F8);
        //d->action_view_text_mode->setExclusiveGroup("view_mode");
        d->action_view_text_mode->setToolTip(i18n("Switch to text view"));
        d->action_view_text_mode->setWhatsThis(i18n("Switches to text view."));
        d->actions_for_view_modes.insert( Kexi::TextViewMode, d->action_view_text_mode );
        connect(d->action_view_text_mode, SIGNAL(triggered()),
          this, SLOT(slotViewTextMode()));
      }
      else
        d->action_view_text_mode = 0;
    */
    if (d->isProjectNavigatorVisible) {
        ac->addAction("view_navigator",
                      d->action_view_nav = new KAction(i18n("Project Navigator"), this));
        d->action_view_nav->setShortcut(Qt::ALT | Qt::Key_1);
        d->action_view_nav->setToolTip(i18n("Go to project navigator panel"));
        d->action_view_nav->setWhatsThis(i18n("Goes to project navigator panel."));
        connect(d->action_view_nav, SIGNAL(triggered()),
                this, SLOT(slotViewNavigator()));
    } else
        d->action_view_nav = 0;

    ac->addAction("view_mainarea",
                  d->action_view_mainarea = new KAction(i18n("Main Area"), this));
    d->action_view_mainarea->setShortcut(Qt::ALT | Qt::Key_2);
    d->action_view_mainarea->setToolTip(i18n("Go to main area"));
    d->action_view_mainarea->setWhatsThis(i18n("Goes to main area."));
    connect(d->action_view_mainarea, SIGNAL(triggered()),
            this, SLOT(slotViewMainArea()));

    if (!d->userMode) {
        ac->addAction("view_propeditor",
                      d->action_view_propeditor = new KAction(i18n("Property Editor"), this));
        d->action_view_propeditor->setShortcut(Qt::ALT | Qt::Key_3);
        d->action_view_propeditor->setToolTip(i18n("Go to property editor panel"));
        d->action_view_propeditor->setWhatsThis(i18n("Goes to property editor panel."));
        connect(d->action_view_propeditor, SIGNAL(triggered()),
                this, SLOT(slotViewPropertyEditor()));
    } else
        d->action_view_propeditor = 0;

    //DATA MENU
    d->action_data_save_row = createSharedAction(i18n("&Save Row"), "dialog-ok",
                              KShortcut(Qt::SHIFT | Qt::Key_Return), "data_save_row");
    d->action_data_save_row->setToolTip(i18n("Save changes made to the current row"));
    d->action_data_save_row->setWhatsThis(i18n("Saves changes made to the current row."));
//temp. disable because of problems with volatile actions setActionVolatile( d->action_data_save_row, true );

    d->action_data_cancel_row_changes = createSharedAction(i18n("&Cancel Row Changes"),
                                        "dialog-cancel", KShortcut(), "data_cancel_row_changes");
    d->action_data_cancel_row_changes->setToolTip(
        i18n("Cancel changes made to the current row"));
    d->action_data_cancel_row_changes->setWhatsThis(
        i18n("Cancels changes made to the current row."));
//temp. disable because of problems with volatile actions setActionVolatile( d->action_data_cancel_row_changes, true );

    d->action_data_execute = createSharedAction(
                                 i18n("&Execute"), "media-playback-start", KShortcut(), "data_execute");
    //d->action_data_execute->setToolTip(i18n("")); //TODO
    //d->action_data_execute->setWhatsThis(i18n("")); //TODO

#ifndef KEXI_NO_UNFINISHED
    action = createSharedAction(i18n("&Filter"), "view-filter", KShortcut(), "data_filter");
    setActionVolatile(action, true);
#endif
// action->setToolTip(i18n("")); //todo
// action->setWhatsThis(i18n("")); //todo

// setSharedMenu("data_sort");
    /* moved to KexiStandardAction
      action = createSharedAction(i18n("&Ascending"), "sort_az", KShortcut(), "data_sort_az");
    //temp. disable because of problems with volatile actions setActionVolatile( action, true );
      action->setToolTip(i18n("Sort data in ascending order"));
      action->setWhatsThis(i18n("Sorts data in ascending order (from A to Z and from 0 to 9). Data from selected column is used for sorting."));

      action = createSharedAction(i18n("&Descending"), "sort_za", KShortcut(), "data_sort_za");
    //temp. disable because of problems with volatile actions setActionVolatile( action, true );
      action->setToolTip(i18n("Sort data in descending order"));
      action->setWhatsThis(i18n("Sorts data in descending (from Z to A and from 9 to 0). Data from selected column is used for sorting."));
    */
    // - record-navigation related actions
    createSharedAction(KexiRecordNavigator::Actions::moveToFirstRecord(), KShortcut(), "data_go_to_first_record");
    createSharedAction(KexiRecordNavigator::Actions::moveToPreviousRecord(), KShortcut(), "data_go_to_previous_record");
    createSharedAction(KexiRecordNavigator::Actions::moveToNextRecord(), KShortcut(), "data_go_to_next_record");
    createSharedAction(KexiRecordNavigator::Actions::moveToLastRecord(), KShortcut(), "data_go_to_last_record");
    createSharedAction(KexiRecordNavigator::Actions::moveToNewRecord(), KShortcut(), "data_go_to_new_record");

    //FORMAT MENU
    d->action_format_font = createSharedAction(i18n("&Font..."), "fonts",
                            KShortcut(), "format_font");
    d->action_format_font->setToolTip(i18n("Change font for selected object"));
    d->action_format_font->setWhatsThis(i18n("Changes font for selected object."));

    //TOOLS MENU

    //WINDOW MENU
#ifndef Q_WS_WIN
#ifdef __GNUC__
#warning kde4 TODO closeWindowAction->setShortcut(KSandardShortcut::close());
#else
#pragma WARNING( kde4 TODO closeWindowAction->setShortcut(KSandardShortcut::close()); )
#endif
    /* TODO???
      //KMDI <= 3.5.1 has no shortcut here:
      QAction *closeWindowAction = actionCollection()->action("window_close");
      if (closeWindowAction) {
        closeWindowAction->setShortcut(KSandardShortcut::close());
      // -- add a few missing tooltip (usable especially in Form's "Assign action" dialog)
        closeWindowAction->setToolTip(i18n("Close the current window"));
      }
    */
#endif

    //additional 'Window' menu items
    ac->addAction("window_next",
                  d->action_window_next = new KAction(i18n("&Next Window"), this));
    d->action_window_next->setShortcut(
#ifdef Q_WS_WIN
        Qt::CTRL | Qt::Key_Tab
#else
        Qt::ALT | Qt::Key_Right
#endif
    );
    d->action_window_next->setToolTip(i18n("Next window"));
    d->action_window_next->setWhatsThis(i18n("Switches to the next window."));
    connect(d->action_window_next, SIGNAL(triggered()),
            this, SLOT(activateNextWindow()));

    ac->addAction("window_previous",
                  d->action_window_previous = new KAction(i18n("&Previous Window"), this));
    d->action_window_previous->setShortcut(
#ifdef Q_WS_WIN
        Qt::CTRL | Qt::SHIFT | Qt::Key_Tab
#else
        Qt::ALT | Qt::Key_Left
#endif
    );
    d->action_window_previous->setToolTip(i18n("Previous window"));
    d->action_window_previous->setWhatsThis(i18n("Switches to the previous window."));
    connect(d->action_window_previous, SIGNAL(triggered()),
            this, SLOT(activatePreviousWindow()));

    //SETTINGS MENU
#ifdef __GNUC__
#warning TODO setStandardToolBarMenuEnabled( true );
#endif

    action = KStandardAction::keyBindings(this, SLOT(slotConfigureKeys()), this);
    ac->addAction(action->objectName(), action);
    action->setWhatsThis(i18n("Lets you configure shortcut keys."));

#ifdef KEXI_SHOW_UNIMPLEMENTED
    /*! @todo 2.0 - toolbars configuration will be handled in a special way
      action = KStandardAction::configureToolbars( this, SLOT( slotConfigureToolbars() ),
        actionCollection() );
      action->setWhatsThis(i18n("Lets you configure toolbars."));

      d->action_show_other = new KActionMenu(i18n("Other"),
        actionCollection(), "options_show_other");
        */
#endif

#ifndef KEXI_NO_CTXT_HELP
    /*! @todo 2.0
      d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", Qt::CTRL + Qt::Key_H,
       actionCollection(), "options_show_contexthelp");
      d->action_show_helper->setCheckedState(i18n("Hide Context Help"));
      */
#endif

#ifdef KEXI_MACROS_SUPPORT
    Kexi::tempShowMacros() = true;
#else
    Kexi::tempShowMacros() = false;
#endif

#ifdef KEXI_SCRIPTS_SUPPORT
    Kexi::tempShowScripts() = true;
#else
    Kexi::tempShowScripts() = false;
#endif

//#ifdef KEXI_SHOW_UNIMPLEMENTED
//! @todo 2.0 - implement settings window in a specific way
    d->action_configure = KStandardAction::preferences(this, SLOT(slotShowSettings()), actionCollection());
    d->action_configure->setWhatsThis(i18n("Lets you configure Kexi."));
//#endif

    //HELP MENU
    // add help menu actions... (KexiTabbedToolBar depends on them)
    d->helpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData(),
                                true/*showWhatsThis*/, ac);
    QAction* help_report_bug_action = ac->action("help_report_bug");
    help_report_bug_action->setIcon(KIcon("tools-report-bug")); // good icon for toolbar
    help_report_bug_action->setWhatsThis(i18n("Shows bug reporting tool for Kexi application."));
    QAction* help_whats_this_action =  ac->action("help_whats_this");
    help_whats_this_action->setWhatsThis(i18n("Shows \"What's This\" tool."));
    QAction* help_contents_action = ac->action("help_contents");
    help_contents_action->setText(i18n("Help"));
    help_contents_action->setWhatsThis(i18n("Shows Kexi Handbook."));
    QAction* help_about_app_action = ac->action("help_about_app");
    help_about_app_action->setWhatsThis(i18n("Shows information about Kexi application."));
    QAction* help_about_kde_action = ac->action("help_about_kde");
    help_about_kde_action->setWhatsThis(i18n("Shows information about K Desktop Environment."));

#if 0//js: todo reenable later
    KStandardAction::tipOfDay(this, SLOT(slotTipOfTheDayAction()), actionCollection())
    ->setWhatsThis(i18n("This shows useful tips on the use of this application."));
#endif
#if 0 //we don't have a time for updating info text for each new version
    new KAction(i18n("Important Information"), "dialog-information", 0,
                this, SLOT(slotImportantInfo()), actionCollection(), "help_show_important_info");
#endif
//TODO: UNCOMMENT TO REMOVE MDI MODES SETTING m_pMdiModeMenu->hide();

#ifndef KEXI_NO_FEEDBACK_AGENT
#ifdef FEEDBACK_CLASS
    new KAction(i18n("Give Feedback..."), "dialog-information", 0,
                this, SLOT(slotStartFeedbackAgent()), actionCollection(), "help_start_feedback_agent");
    connect(, SIGNAL(triggered()),
            this, SLOT(()));
#endif
#endif
// KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
//  actionCollection(), "kexi_settings");
// actionSettings->setWhatsThis(i18n("Lets you configure Kexi."));
// connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

    // ----- declare action categories, so form's "assign action to button"
    //       (and macros in the future) will be able to recognize category
    //       of actions and filter them -----------------------------------
//! @todo shouldn't we move this to core?
    Kexi::ActionCategories *acat = Kexi::actionCategories();
    acat->addAction("data_execute", Kexi::PartItemActionCategory);

    //! @todo unused for now
    acat->addWindowAction("data_filter",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addWindowAction("data_save_row",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addWindowAction("data_cancel_row_changes",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addWindowAction("delete_table_row",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    //! @todo support this in KexiPart::FormObjectType as well
    acat->addWindowAction("data_sort_az",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType);

    //! @todo support this in KexiPart::FormObjectType as well
    acat->addWindowAction("data_sort_za",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType);

    //! @todo support this in KexiPart::FormObjectType as well
    acat->addWindowAction("edit_clear_table",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType);

    //! @todo support this in KexiPart::FormObjectType as well
    acat->addWindowAction("edit_copy_special_data_table",
                          KexiPart::TableObjectType, KexiPart::QueryObjectType);

    // GlobalActions, etc.
    acat->addAction("edit_copy", Kexi::GlobalActionCategory | Kexi::PartItemActionCategory);

    acat->addAction("edit_cut", Kexi::GlobalActionCategory | Kexi::PartItemActionCategory);

    acat->addAction("edit_paste", Kexi::GlobalActionCategory | Kexi::PartItemActionCategory);

    acat->addAction("edit_delete", Kexi::GlobalActionCategory | Kexi::PartItemActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addAction("edit_delete_row", Kexi::GlobalActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addAction("edit_edititem", Kexi::PartItemActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType);

    acat->addAction("edit_find", Kexi::GlobalActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addAction("edit_findnext", Kexi::GlobalActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addAction("edit_findprevious", Kexi::GlobalActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addAction("edit_replace", Kexi::GlobalActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    acat->addAction("edit_paste_special_data_table", Kexi::GlobalActionCategory);

    acat->addAction("help_about_app", Kexi::GlobalActionCategory);

    acat->addAction("help_about_kde", Kexi::GlobalActionCategory);

    acat->addAction("help_contents", Kexi::GlobalActionCategory);

    acat->addAction("help_report_bug", Kexi::GlobalActionCategory);

    acat->addAction("help_whats_this", Kexi::GlobalActionCategory);

    acat->addAction("options_configure_keybinding", Kexi::GlobalActionCategory);

    acat->addAction("project_close", Kexi::GlobalActionCategory);

    //! @todo support this in FormObjectType as well
    acat->addAction("project_export_data_table", Kexi::GlobalActionCategory | Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType);

    acat->addAction("project_import_data_table", Kexi::GlobalActionCategory);

    acat->addAction("project_new", Kexi::GlobalActionCategory);

    acat->addAction("project_open", Kexi::GlobalActionCategory);

    //! @todo support this in FormObjectType, ReportObjectType as well as others
    acat->addAction("project_print", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType);

    //! @todo support this in FormObjectType, ReportObjectType as well as others
    acat->addAction("project_print_preview", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType);

    //! @todo support this in FormObjectType, ReportObjectType as well as others
    acat->addAction("project_print_setup", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType);

    acat->addAction("quit", Kexi::GlobalActionCategory);

    acat->addAction("tools_compact_database", Kexi::GlobalActionCategory);

    acat->addAction("tools_import_project", Kexi::GlobalActionCategory);

    acat->addAction("tools_import_tables", Kexi::GlobalActionCategory);
    
    acat->addAction("view_data_mode", Kexi::GlobalActionCategory);

    acat->addAction("view_design_mode", Kexi::GlobalActionCategory);

    acat->addAction("view_text_mode", Kexi::GlobalActionCategory);

    acat->addAction("view_mainarea", Kexi::GlobalActionCategory);

    acat->addAction("view_navigator", Kexi::GlobalActionCategory);

    acat->addAction("view_propeditor", Kexi::GlobalActionCategory);

    acat->addAction("window_close", Kexi::GlobalActionCategory | Kexi::WindowActionCategory);
    acat->setAllObjectTypesSupported("window_close", true);

    acat->addAction("window_next", Kexi::GlobalActionCategory);

    acat->addAction("window_previous", Kexi::GlobalActionCategory);

    //skipped - design view only
    acat->addAction("format_font", Kexi::NoActionCategory);
    acat->addAction("project_save", Kexi::NoActionCategory);
    acat->addAction("edit_insert_empty_row", Kexi::NoActionCategory);
    //! @todo support this in KexiPart::TableObjectType, KexiPart::QueryObjectType later
    acat->addAction("edit_select_all", Kexi::NoActionCategory);
    //! @todo support this in KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType later
    acat->addAction("edit_redo", Kexi::NoActionCategory);
    //! @todo support this in KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType later
    acat->addAction("edit_undo", Kexi::NoActionCategory);

    //record-navigation related actions
    acat->addAction("data_go_to_first_record", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
    acat->addAction("data_go_to_previous_record", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
    acat->addAction("data_go_to_next_record", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
    acat->addAction("data_go_to_last_record", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
    acat->addAction("data_go_to_new_record", Kexi::WindowActionCategory,
                    KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

    //skipped - internal:
    acat->addAction("tablepart_create", Kexi::NoActionCategory);
    acat->addAction("querypart_create", Kexi::NoActionCategory);
    acat->addAction("formpart_create", Kexi::NoActionCategory);
    acat->addAction("reportpart_create", Kexi::NoActionCategory);
    acat->addAction("macropart_create", Kexi::NoActionCategory);
    acat->addAction("scriptpart_create", Kexi::NoActionCategory);
}

void KexiMainWindow::invalidateActions()
{
    invalidateProjectWideActions();
    invalidateSharedActions();
}

void KexiMainWindow::invalidateSharedActions(QObject *o)
{
    //TODO: enabling is more complex...
    /* d->action_edit_cut->setEnabled(true);
      d->action_edit_copy->setEnabled(true);
      d->action_edit_paste->setEnabled(true);*/

    if (!o)
        o = focusWindow();
    KexiSharedActionHost::invalidateSharedActions(o);
}

void KexiMainWindow::invalidateSharedActions()
{
    invalidateSharedActions(0);
}

// unused, I think
void KexiMainWindow::invalidateSharedActionsLater()
{
    QTimer::singleShot(1, this, SLOT(invalidateSharedActions()));
}

void KexiMainWindow::invalidateProjectWideActions()
{
// stateChanged("project_opened",d->prj ? StateNoReverse : StateReverse);

    const bool has_window = currentWindow();
    const bool window_dirty = currentWindow() && currentWindow()->isDirty();
    const bool readOnly = d->prj && d->prj->dbConnection() && d->prj->dbConnection()->isReadOnly();

    //PROJECT MENU
    d->action_save->setEnabled(has_window && window_dirty && !readOnly);
    d->action_save_as->setEnabled(has_window && !readOnly);
    d->action_project_properties->setEnabled(d->prj);
    d->action_close->setEnabled(d->prj);
    d->action_project_relations->setEnabled(d->prj);
    if (d->action_project_import_data_table)
        d->action_project_import_data_table->setEnabled(d->prj && !readOnly);
    d->action_project_export_data_table->setEnabled(
        currentWindow() && currentWindow()->part()->info()->isDataExportSupported()
        && !currentWindow()->neverSaved());

    const bool printingActionsEnabled =
        currentWindow() && currentWindow()->part()->info()->isPrintingSupported()
        && !currentWindow()->neverSaved();
    d->action_project_print->setEnabled(printingActionsEnabled);
    d->action_project_print_preview->setEnabled(printingActionsEnabled);
    d->action_project_print_setup->setEnabled(printingActionsEnabled);

    //EDIT MENU
    if (d->action_edit_paste_special_data_table)
        d->action_edit_paste_special_data_table->setEnabled(d->prj && !readOnly);

//! @todo "copy special" is currently enabled only for data view mode;
//!  what about allowing it to enable in design view for "kexi/table" ?
    if (currentWindow() && currentWindow()->currentViewMode() == Kexi::DataViewMode) {
        KexiPart::Info *activePartInfo = currentWindow()->part()->info();
        d->action_edit_copy_special_data_table->setEnabled(
            activePartInfo ? activePartInfo->isDataExportSupported() : false);
    } else
        d->action_edit_copy_special_data_table->setEnabled(false);
    d->action_edit_find->setEnabled(d->prj);

    //VIEW MENU
    if (d->action_view_nav)
        d->action_view_nav->setEnabled(d->prj);
    d->action_view_mainarea->setEnabled(d->prj);
    if (d->action_view_propeditor)
        d->action_view_propeditor->setEnabled(d->prj);
    /* UNUSED, see KexiToggleViewModeAction
      if (d->action_view_data_mode) {
        d->action_view_data_mode->setEnabled(
          has_window && currentWindow()->supportsViewMode(Kexi::DataViewMode) );
        if (!d->action_view_data_mode->isEnabled())
          d->action_view_data_mode->setChecked(false);
      }
      if (d->action_view_design_mode) {
        d->action_view_design_mode->setEnabled(
          has_window && currentWindow()->supportsViewMode(Kexi::DesignViewMode) );
        if (!d->action_view_design_mode->isEnabled())
          d->action_view_design_mode->setChecked(false);
      }
      if (d->action_view_text_mode) {
        d->action_view_text_mode->setEnabled(
          has_window && currentWindow()->supportsViewMode(Kexi::TextViewMode) );
        if (!d->action_view_text_mode->isEnabled())
          d->action_view_text_mode->setChecked(false);
      }*/
#ifndef KEXI_NO_CTXT_HELP
    d->action_show_helper->setEnabled(d->prj);
#endif

    //CREATE MENU
    /*replaced by d->tabbedToolBar
      if (d->createMenu)
        d->createMenu->setEnabled(d->prj);*/
    if (d->tabbedToolBar->createWidgetToolBar())
        d->tabbedToolBar->createWidgetToolBar()->setEnabled(d->prj);

    // DATA MENU
    //d->action_data_execute->setEnabled( currentWindow() && currentWindow()->part()->info()->isExecuteSupported() );

    //TOOLS MENU
    // "compact db" supported if there's no db or the current db supports compacting and is opened r/w:
    d->action_tools_compact_database->setEnabled(
        !d->prj
        || (!readOnly && d->prj && d->prj->dbConnection()
            && (d->prj->dbConnection()->driver()->features() & KexiDB::Driver::CompactingDatabaseSupported))
    );

    //WINDOW MENU
    if (d->action_window_next) {
#ifdef __GNUC__
#warning TODO  d->action_window_next->setEnabled(!m_pDocumentViews->isEmpty());
#else
#pragma WARNING( TODO  d->action_window_next->setEnabled(!m_pDocumentViews->isEmpty()); )
#endif
#ifdef __GNUC__
#warning TODO  d->action_window_previous->setEnabled(!m_pDocumentViews->isEmpty());
#else
#pragma WARNING( TODO  d->action_window_previous->setEnabled(!m_pDocumentViews->isEmpty()); )
#endif
    }

    //DOCKS
    if (d->nav)
        d->nav->setEnabled(d->prj);
    if (d->propEditor)
        d->propEditorTabWidget->setEnabled(d->prj);
}

/* UNUSED, see KexiToggleViewModeAction
void KexiMainWindow::invalidateViewModeActions()
{
  if (currentWindow()) {
    //update toggle action
    if (currentWindow()->currentViewMode()==Kexi::DataViewMode) {
      if (d->action_view_data_mode)
        d->action_view_data_mode->setChecked( true );
    }
    else if (currentWindow()->currentViewMode()==Kexi::DesignViewMode) {
      if (d->action_view_design_mode)
        d->action_view_design_mode->setChecked( true );
    }
    else if (currentWindow()->currentViewMode()==Kexi::TextViewMode) {
      if (d->action_view_text_mode)
        d->action_view_text_mode->setChecked( true );
    }
  }
}*/

tristate KexiMainWindow::startup()
{
    tristate result = true;
    switch (Kexi::startupHandler().action()) {
    case KexiStartupHandler::CreateBlankProject:
        d->updatePropEditorVisibility(Kexi::NoViewMode);
        result = createBlankProject();
        break;
    case KexiStartupHandler::CreateFromTemplate:
        result = createProjectFromTemplate(*Kexi::startupHandler().projectData());
        break;
    case KexiStartupHandler::OpenProject:
        result = openProject(*Kexi::startupHandler().projectData());
        break;
    case KexiStartupHandler::ImportProject:
        result = showProjectMigrationWizard(
                   Kexi::startupHandler().importActionData().mimeType,
                   Kexi::startupHandler().importActionData().fileName
               );
        break;
    default:
        d->updatePropEditorVisibility(Kexi::NoViewMode);
    }
    
    //    d->mainWidget->setAutoSaveSettings(QLatin1String("MainWindow"), /*saveWindowSize*/false);
    return result;
}

static QString internalReason(KexiDB::Object *obj)
{
    const QString &s = obj->errorMsg();
    if (s.isEmpty())
        return s;
    return QString("<br>(%1) ").arg(i18n("reason:") + " <i>" + s + "</i>");
}

tristate KexiMainWindow::openProject(const KexiProjectData& projectData)
{
    KexiProjectData *newProjectData = new KexiProjectData(projectData);
// if (d->userMode) {
    //TODO: maybe also auto allow to open objects...
//  return setupUserModeMode(newProjectData);
// }
    createKexiProject(newProjectData);
    if (!newProjectData->connectionData()->savePassword
            && newProjectData->connectionData()->password.isEmpty()
            && newProjectData->connectionData()->fileName().isEmpty() //! @todo temp.: change this if there are file-based drivers requiring a password
       ) {
        //ask for password
        KexiDBPasswordDialog pwdDlg(this, *newProjectData->connectionData(),
                                    false /*!showDetailsButton*/);
        if (QDialog::Accepted != pwdDlg.exec()) {
            delete d->prj;
            d->prj = 0;
            return cancelled;
        }
    }
    bool incompatibleWithKexi;
    tristate res = d->prj->open(incompatibleWithKexi);
    if (~res) {
        delete d->prj;
        d->prj = 0;
        return cancelled;
    } else if (!res) {
        delete d->prj;
        d->prj = 0;
        if (incompatibleWithKexi) {
            if (KMessageBox::Yes == KMessageBox::questionYesNo(this,
                    i18n("<qt>Database project %1 does not appear to have been created using Kexi.<br><br>Do you want to import it as a new Kexi project?</qt>",
                         projectData.infoString()),
                    0, KGuiItem(i18nc("Import Database", "&Import..."), "database_import"),
                    KStandardGuiItem::quit())) {
                const bool anotherProjectAlreadyOpened = d->prj;
                tristate res = showProjectMigrationWizard("application/x-kexi-connectiondata",
                               projectData.databaseName(), projectData.constConnectionData());

                if (!anotherProjectAlreadyOpened) //the project could have been opened within this instance
                    return res;

                //always return cancelled because even if migration succeeded, new Kexi instance
                //will be started if user wanted to open the imported db
                return cancelled;
            }
            return cancelled;
        }
        return false;
    }
    setupProjectNavigator();
    Kexi::recentProjects().addProjectData(newProjectData);
    updateReadOnlyState();
    invalidateActions();
// d->disableErrorMessages = true;
    enableMessages(false);

    QTimer::singleShot(1, this, SLOT(slotAutoOpenObjectsLater()));
    return true;
}

tristate KexiMainWindow::createProjectFromTemplate(const KexiProjectData& projectData)
{
    QStringList mimetypes;
    mimetypes.append(KexiDB::defaultFileBasedDriverMimeType());
    QString fname;
    const QString startDir("kfiledialog:///OpenExistingOrCreateNewProject"/*as in KexiNewProjectWizard*/);
    const QString caption(i18n("Select New Project's Location"));

    while (true) {
#ifdef __GNUC__
#warning TODO - remove win32 case
#else
#pragma WARNING( TODO - remove win32 case )
#endif
        /*TODO?
        #ifdef Q_WS_WIN
          //! @todo remove
            QString recentDir = KGlobalSettings::documentPath();
            if (fname.isEmpty() && !projectData.constConnectionData()->dbFileName().isEmpty()) //propose filename from db template name
              fname = KFileDialog::getStartURL(startDir, recentDir).path()
                + '/' + projectData.constConnectionData()->dbFileName();
            fname = Q3FileDialog::getSaveFileName(
              KFileDialog::getStartURL(fname.isEmpty() ? startDir : fname, recentDir).path(),
              KexiUtils::fileDialogFilterStrings(mimetypes, false),
              this, "CreateProjectFromTemplate", caption);
            if ( !fname.isEmpty() ) {
              //save last visited path
              KUrl url;
              url.setPath( fname );
              if (url.isLocalFile())
                KRecentDirs::add(startDir, url.directory());
            }
        #else*/
        Q_UNUSED(projectData);
        if (fname.isEmpty() &&
                !projectData.constConnectionData()->dbFileName().isEmpty()) {
            //propose filename from db template name
            fname = projectData.constConnectionData()->dbFileName();
        }
        const bool specialDir = fname.isEmpty();
        kDebug() << fname << ".............";
        KFileDialog dlg(specialDir ? KUrl(startDir) : KUrl(),
                        QString(), this);
        dlg.setModal(true);
        dlg.setMimeFilter(mimetypes);
        if (!specialDir)
            dlg.setSelection(fname);   // may also be a filename
        dlg.setOperationMode(KFileDialog::Saving);
        dlg.setWindowTitle(caption);
        dlg.exec();
        fname = dlg.selectedFile();
        if (!fname.isEmpty())
            KRecentDocument::add(fname);
//#endif
        if (fname.isEmpty())
            return cancelled;
        if (KexiStartupFileWidget::askForOverwriting(fname, this))
            break;
    }

    QFile sourceFile(projectData.constConnectionData()->fileName());
    if (!sourceFile.copy(fname)) {
//! @todo show error from with QFile::FileError
        return false;
    }

    return openProject(fname, 0, QString(), projectData.autoopenObjects/*copy*/);
}

void KexiMainWindow::updateReadOnlyState()
{
    const bool readOnly = d->prj && d->prj->dbConnection() && d->prj->dbConnection()->isReadOnly();
    d->statusBar->setReadOnlyFlag(readOnly);
    if (d->nav)
        d->nav->setReadOnly(readOnly);
    // update "insert ....." actions for every part
    KActionCollection *ac = actionCollection();
    foreach(KexiPart::Info *info, *Kexi::partManager().partInfoList()) {
        QAction *a = ac->action(KexiPart::nameForCreateAction(*info));
        if (a)
            a->setEnabled(!readOnly);
    }
}

void KexiMainWindow::slotAutoOpenObjectsLater()
{
    QString not_found_msg;
    bool openingCancelled;
    //ok, now open "autoopen: objects
    if (d->prj) {
        foreach(KexiProjectData::ObjectInfo* info, d->prj->data()->autoopenObjects) {
            KexiPart::Info *i = Kexi::partManager().infoForClass(info->value("type"));
            if (!i) {
                not_found_msg += "<li>";
                if (!info->value("name").isEmpty())
                    not_found_msg += (QString("\"") + info->value("name") + "\" - ");
                if (info->value("action") == "new")
                    not_found_msg += i18n("cannot create object - unknown object type \"%1\"", info->value("type"));
                else
                    not_found_msg += i18n("unknown object type \"%1\"", info->value("type"));
                not_found_msg += internalReason(&Kexi::partManager()) + "<br></li>";
                continue;
            }
            // * NEW
            if (info->value("action") == "new") {
                if (!newObject(i, openingCancelled) && !openingCancelled) {
                    not_found_msg += "<li>";
                    not_found_msg += (i18n("cannot create object of type \"%1\"", info->value("type")) +
                                      internalReason(d->prj) + "<br></li>");
                } else
                    d->wasAutoOpen = true;
                continue;
            }

            KexiPart::Item *item = d->prj->item(i, info->value("name"));

            if (!item) {
                QString taskName;
                if (info->value("action") == "print-preview")
                    taskName = i18n("making print preview for");
                else if (info->value("action") == "print")
                    taskName = i18n("printing");
                else if (info->value("action") == "execute")
                    taskName = i18nc("\"executing object\" action", "executing");
                else
                    taskName = i18n("opening");

                not_found_msg += (QString("<li>") + taskName + " \"" + info->value("name") + "\" - ");
                if ("table" == info->value("type").toLower())
                    not_found_msg += i18n("table not found");
                else if ("query" == info->value("type").toLower())
                    not_found_msg += i18n("query not found");
                else if ("macro" == info->value("type").toLower())
                    not_found_msg += i18n("macro not found");
                else if ("script" == info->value("type").toLower())
                    not_found_msg += i18n("script not found");
                else
                    not_found_msg += i18n("object not found");
                not_found_msg += (internalReason(d->prj) + "<br></li>");
                continue;
            }
            // * EXECUTE, PRINT, PRINT PREVIEW
            if (info->value("action") == "execute") {
                tristate res = executeItem(item);
                if (false == res) {
                    not_found_msg += (QString("<li>\"") + info->value("name") + "\" - " + i18n("cannot execute object") +
                                      internalReason(d->prj) + "<br></li>");
                }
                continue;
            } else if (info->value("action") == "print") {
                tristate res = printItem(item);
                if (false == res) {
                    not_found_msg += (QString("<li>\"") + info->value("name") + "\" - " + i18n("cannot print object") +
                                      internalReason(d->prj) + "<br></li>");
                }
                continue;
            } else if (info->value("action") == "print-preview") {
                tristate res = printPreviewForItem(item);
                if (false == res) {
                    not_found_msg += (QString("<li>\"") + info->value("name") + "\" - " + i18n("cannot make print preview of object") +
                                      internalReason(d->prj) + "<br></li>");
                }
                continue;
            }

            Kexi::ViewMode viewMode;
            if (info->value("action") == "open")
                viewMode = Kexi::DataViewMode;
            else if (info->value("action") == "design")
                viewMode = Kexi::DesignViewMode;
            else if (info->value("action") == "edittext")
                viewMode = Kexi::TextViewMode;
            else
                continue; //sanity

            QString openObjectMessage;
            if (!openObject(item, viewMode, openingCancelled, 0, &openObjectMessage)
                    && (!openingCancelled || !openObjectMessage.isEmpty())) {
                not_found_msg += (QString("<li>\"") + info->value("name") + "\" - ");
                if (openObjectMessage.isEmpty())
                    not_found_msg += i18n("cannot open object");
                else
                    not_found_msg += openObjectMessage;
                not_found_msg += internalReason(d->prj) + "<br></li>";
                continue;
            } else {
                d->wasAutoOpen = true;
            }
        }
    }
    enableMessages(true);
// d->disableErrorMessages = false;

    if (!not_found_msg.isEmpty())
        showErrorMessage(i18n("You have requested selected objects to be automatically opened "
                              "or processed on startup. Several objects cannot be opened or processed."),
                         QString("<ul>%1</ul>").arg(not_found_msg));

    d->updatePropEditorVisibility(currentWindow() ? currentWindow()->currentViewMode() : Kexi::NoViewMode);
#if defined(KDOCKWIDGET_P)
    if (d->propEditor) {
        KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
        KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
        if (ds)
            ds->setSeparatorPosInPercent(d->config->readEntry("RightDockPosition", 80/* % */));
    }
#endif

    updateAppCaption();

// d->navToolWindow->wrapperWidget()->setFixedWidth(200);
//js TODO: make visible FOR OTHER MODES if needed
    /*2.0: if (mdiMode()==KMdi::ChildframeMode || mdiMode()==KMdi::TabPageMode) {
        //make docks visible again
        if (!d->navToolWindow->wrapperWidget()->isVisible())
          static_cast<KDockWidget*>(d->navToolWindow->wrapperWidget())->makeDockVisible();
        if (!d->propEditorToolWindow->wrapperWidget()->isVisible())
          static_cast<KDockWidget*>(d->propEditorToolWindow->wrapperWidget())->makeDockVisible();
      }*/

    // if (!d->prj->data()->autoopenObjects.isEmpty())
//2.0 d->restoreNavigatorWidth();

    if (d->nav) {
#ifdef __GNUC__
#warning NEEDED?  d->nav->updateGeometry();
#else
#pragma WARNING( NEEDED?  d->nav->updateGeometry(); )
#endif
    }
    qApp->processEvents();
    emit projectOpened();
}

tristate KexiMainWindow::closeProject()
{
#ifndef KEXI_NO_PENDING_DIALOGS
    if (d->pendingWindowsExist()) {
        kDebug() << "KexiMainWindow::closeProject() pendingWindowsExist...";
        d->actionToExecuteWhenPendingJobsAreFinished = Private::CloseProjectAction;
        return cancelled;
    }
#endif

    //only save nav. visibility setting if there is project opened
    d->saveSettingsForShowProjectNavigator = d->prj && d->isProjectNavigatorVisible;

    if (!d->prj)
        return true;

    {
        // make sure the project can be closed
        bool cancel = false;
        emit acceptProjectClosingRequested(cancel);
        if (cancel)
            return cancelled;
    }

    d->windowExistedBeforeCloseProject = currentWindow();

#if defined(KDOCKWIDGET_P)
    //remember docks position - will be used on storeSettings()
    if (d->propEditor) {
        KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
        KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
        if (ds)
            d->propEditorDockSeparatorPos = ds->separatorPosInPercent();
    }
    if (d->nav) {
//  makeDockInvisible( manager()->findWidgetParentDock(d->propEditor) );

        if (d->propEditor) {
#ifdef __GNUC__
#warning TODO   if (d->openedWindowsCount() == 0)
#else
#pragma WARNING( TODO   if (d->openedWindowsCount() == 0) )
#endif
#ifdef __GNUC__
#warning TODO    makeWidgetDockVisible(d->propEditorTabWidget);
#else
#pragma WARNING( TODO    makeWidgetDockVisible(d->propEditorTabWidget); )
#endif
            KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
            KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
            if (ds)
                ds->setSeparatorPosInPercent(80);
        }

        KDockWidget *dw = (KDockWidget *)d->nav->parentWidget();
        KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
        int dwWidth = dw->width();
        if (ds) {
            if (d->openedWindowsCount() != 0 && d->propEditorTabWidget && d->propEditorTabWidget->isVisible())
                d->navDockSeparatorPos = ds->separatorPosInPercent();
            else
                d->navDockSeparatorPos = (100 * dwWidth) / width();

//    int navDockSeparatorPosWithAutoOpen = (100 * dw->width()) / width() + 4;
//    d->navDockSeparatorPos = (100 * dw->width()) / width() + 1;
        }
    }
#endif

    //close each window, optionally asking if user wants to close (if data changed)
    while (currentWindow()) {
        tristate res = closeWindow(currentWindow());
        if (!res || ~res)
            return res;
    }

    // now we will close for sure
    emit beforeProjectClosing();

    if (!d->prj->closeConnection())
        return false;

    if (d->nav) {
        d->navWasVisibleBeforeProjectClosing = d->navDockWidget->isVisible();
        d->navDockWidget->hide();
        d->nav->clear();
    }

    if (d->propEditorDockWidget)
        d->propEditorDockWidget->hide();

    d->clearWindows(); //sanity!
    delete d->prj;
    d->prj = 0;

// Kexi::partManager().unloadAllParts();

    updateReadOnlyState();
    invalidateActions();
    updateAppCaption();

    emit projectClosed();
    return true;
}

void KexiMainWindow::setupContextHelp()
{
#ifndef KEXI_NO_CTXT_HELP
    d->ctxHelp = new KexiContextHelp(d->mainWidget, this);
    /*todo
      d->ctxHelp->setContextHelp(i18n("Welcome"),i18n("The <B>KEXI team</B> wishes you a lot of productive work, "
        "with this product. <BR><HR><BR>If you have found a <B>bug</B> or have a <B>feature</B> request, please don't "
        "hesitate to report it at our <A href=\"http://www.kexi-project.org/cgi-bin/bug.pl\"> issue "
        "tracking system </A>.<BR><HR><BR>If you would like to <B>join</B> our effort, the <B>development</B> documentation "
        "at <A href=\"http://www.kexi-project.org\">www.kexi-project.org</A> is a good starting point."),0);
    */
    addToolWindow(d->ctxHelp, KDockWidget::DockBottom | KDockWidget::DockLeft, getMainDockWidget(), 20);
#endif
}

void KexiMainWindow::setupMainWidget()
{
    QVBoxLayout *vlyr = new QVBoxLayout(this);
    vlyr->setContentsMargins(0, 0, 0, 0);
    vlyr->setSpacing(0);

    QWidget *tabbedToolBarContainer = new QWidget(this);
    vlyr->addWidget(tabbedToolBarContainer);
    QVBoxLayout *tabbedToolBarContainerLyr = new QVBoxLayout(tabbedToolBarContainer);
    tabbedToolBarContainerLyr->setSpacing(0);
    tabbedToolBarContainerLyr->setContentsMargins(
        KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2);

    d->tabbedToolBar = new KexiTabbedToolBar(tabbedToolBarContainer);
    tabbedToolBarContainerLyr->addWidget(d->tabbedToolBar);

    QWidget *mainWidgetContainer = new QWidget();
    vlyr->addWidget(mainWidgetContainer, 1);
    QHBoxLayout *mainWidgetContainerLyr = new QHBoxLayout(mainWidgetContainer);
    mainWidgetContainerLyr->setContentsMargins(0, 0, 0, 0);
    mainWidgetContainerLyr->setSpacing(0);

    KMultiTabBar *mtbar = new KMultiTabBar(KMultiTabBar::Left);
    mtbar->setStyle(KMultiTabBar::KDEV3ICON);
    mainWidgetContainerLyr->addWidget(mtbar, 0);
    d->multiTabBars.insert(mtbar->position(), mtbar);

    d->mainWidget = new KexiMainWidget();
    d->mainWidget->setParent(this);
    mainWidgetContainerLyr->addWidget(d->mainWidget, 1);

    mtbar = new KMultiTabBar(KMultiTabBar::Right);
    mtbar->setStyle(KMultiTabBar::KDEV3ICON);
    mainWidgetContainerLyr->addWidget(mtbar, 0);
    d->multiTabBars.insert(mtbar->position(), mtbar);

    d->statusBar = new KexiStatusBar(this);
#if 0 // still disabled, see KexiStatusBar
    connect(d->statusBar->m_showNavigatorAction, SIGNAL(triggered(bool)),
        this, SLOT(slotSetProjectNavigatorVisible(bool)));
    connect(d->statusBar->m_showPropertyEditorAction, SIGNAL(triggered(bool)),
        this, SLOT(slotSetPropertyEditorVisible(bool)));
#endif
    vlyr->addWidget(d->statusBar);
}

void KexiMainWindow::slotSetProjectNavigatorVisible(bool set)
{
    if (d->navDockWidget)
        d->navDockWidget->setVisible(set);
}

void KexiMainWindow::slotSetPropertyEditorVisible(bool set)
{
    if (d->propEditorDockWidget)
        d->propEditorDockWidget->setVisible(set);
}

void KexiMainWindow::slotProjectNavigatorVisibilityChanged(bool visible)
{
    KMultiTabBar *mtbar = d->multiTabBars[KMultiTabBar::Left];
    int id = PROJECT_NAVIGATOR_TABBAR_ID;
    if (visible) {
        mtbar->removeTab(id);
    }
    else {
        QString t(d->navDockWidget->windowTitle());
        t.remove('&');
        mtbar->appendTab(QPixmap(), id, t);
        KMultiTabBarTab *tab = mtbar->tab(id);
        connect(tab, SIGNAL(clicked(int)), this, SLOT(slotMultiTabBarTabClicked(int)));
    }
}

void KexiMainWindow::slotPropertyEditorVisibilityChanged(bool visible)
{
    if (!d->enable_slotPropertyEditorVisibilityChanged)
        return;
    d->setPropertyEditorTabBarVisible(!visible);
    if (!visible)
        d->propertyEditorCollapsed = true;
}

void KexiMainWindow::slotMultiTabBarTabClicked(int id)
{
    if (id == PROJECT_NAVIGATOR_TABBAR_ID) {
        slotProjectNavigatorVisibilityChanged(true);
        d->navDockWidget->show();
    }
    else if (id == PROPERTY_EDITOR_TABBAR_ID) {
        slotPropertyEditorVisibilityChanged(true);
        d->propEditorDockWidget->show();
        d->propertyEditorCollapsed = false;
    }
}

static Qt::DockWidgetArea loadDockAreaSetting(KConfigGroup& group, const char* configEntry, Qt::DockWidgetArea defaultArea)
{
        const QString areaName = group.readEntry(configEntry).toLower();
        if (areaName == "left")
            return Qt::LeftDockWidgetArea;
        else if (areaName == "right")
            return Qt::RightDockWidgetArea;
        else if (areaName == "top")
            return Qt::TopDockWidgetArea;
        else if (areaName == "bottom")
            return Qt::BottomDockWidgetArea;
        return defaultArea;
}

static void saveDockAreaSetting(KConfigGroup& group, const char* configEntry, Qt::DockWidgetArea area)
{
    QString areaName;
    switch (area) {
    case Qt::LeftDockWidgetArea: areaName = "left"; break;
    case Qt::RightDockWidgetArea: areaName = "right"; break;
    case Qt::TopDockWidgetArea: areaName = "top"; break;
    case Qt::BottomDockWidgetArea: areaName = "bottom"; break;
    default: areaName = "left"; break;
    }
    if (areaName.isEmpty())
        group.deleteEntry(configEntry);
    else
        group.writeEntry(configEntry, areaName);
}

void KexiMainWindow::setupProjectNavigator()
{
    if (!d->isProjectNavigatorVisible)
        return;

    if (d->nav) {
         d->navDockWidget->show();
    }
    else {
        d->navDockWidget = new KexiDockWidget(QString(), d->mainWidget);
        d->navDockWidget->setObjectName("ProjectNavigatorDockWidget");
//        d->navDockWidget->setMinimumWidth(300);
        KConfigGroup mainWindowGroup(d->config->group("MainWindow"));
        d->mainWidget->addDockWidget(
            loadDockAreaSetting(mainWindowGroup, "ProjectNavigatorArea", Qt::LeftDockWidgetArea),
            d->navDockWidget
//            static_cast<Qt::Orientation>(mainWindowGroup.readEntry("PropertyEditorOrientation", (int)Qt::Vertical))
        );

        KexiDockableWidget* navDockableWidget = new KexiDockableWidget(d->navDockWidget);
        d->nav = new KexiProjectListView(navDockableWidget);
        navDockableWidget->setWidget(d->nav);
//TODO REMOVE?  d->nav->installEventFilter(this);
        d->navDockWidget->setWindowTitle(d->nav->windowTitle());
        d->navDockWidget->setWidget(navDockableWidget);

//        const bool showProjectNavigator = mainWindowGroup.readEntry("ShowProjectNavigator", true);
        const QSize projectNavigatorSize = mainWindowGroup.readEntry<QSize>("ProjectNavigatorSize", QSize());
        if (!projectNavigatorSize.isNull()) {
            navDockableWidget->setSizeHint(projectNavigatorSize);
        }

#ifdef __GNUC__
#warning TODO d->navToolWindow = addToolWindow(d->nav, KDockWidget::DockLeft, getMainDockWidget(), 20/*, lv, 35, "2"*/);
#else
#pragma WARNING( TODO d->navToolWindow = addToolWindow(d->nav, KDockWidget::DockLeft, getMainDockWidget(), 20/*, lv, 35, "2"*/); )
#endif
//  d->navToolWindow->hide();

        connect(d->nav, SIGNAL(openItem(KexiPart::Item*, Kexi::ViewMode)),
                this, SLOT(openObject(KexiPart::Item*, Kexi::ViewMode)));
        connect(d->nav, SIGNAL(openOrActivateItem(KexiPart::Item*, Kexi::ViewMode)),
                this, SLOT(openObjectFromNavigator(KexiPart::Item*, Kexi::ViewMode)));
        connect(d->nav, SIGNAL(newItem(KexiPart::Info*)),
                this, SLOT(newObject(KexiPart::Info*)));
        connect(d->nav, SIGNAL(removeItem(KexiPart::Item*)),
                this, SLOT(removeObject(KexiPart::Item*)));
        connect(d->nav, SIGNAL(renameItem(KexiPart::Item*, const QString&, bool&)),
                this, SLOT(renameObject(KexiPart::Item*, const QString&, bool&)));
        connect(d->nav, SIGNAL(executeItem(KexiPart::Item*)),
                this, SLOT(executeItem(KexiPart::Item*)));
        connect(d->nav, SIGNAL(exportItemToClipboardAsDataTable(KexiPart::Item*)),
                this, SLOT(copyItemToClipboardAsDataTable(KexiPart::Item*)));
        connect(d->nav, SIGNAL(exportItemToFileAsDataTable(KexiPart::Item*)),
                this, SLOT(exportItemAsDataTable(KexiPart::Item*)));
        connect(d->nav, SIGNAL(printItem(KexiPart::Item*)),
                this, SLOT(printItem(KexiPart::Item*)));
        connect(d->nav, SIGNAL(pageSetupForItem(KexiPart::Item*)),
                this, SLOT(showPageSetupForItem(KexiPart::Item*)));
        if (d->prj) {//connect to the project
            connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)),
                    d->nav, SLOT(slotRemoveItem(const KexiPart::Item&)));
        }
        connect(d->nav, SIGNAL(selectionChanged(KexiPart::Item*)),
                this, SLOT(slotPartItemSelectedInNavigator(KexiPart::Item*)));
        connect(d->navDockWidget, SIGNAL(visibilityChanged(bool)),
            this, SLOT(slotProjectNavigatorVisibilityChanged(bool)));

//  d->restoreNavigatorWidth();
    }
    if (d->prj->isConnected()) {
        QString partManagerErrorMessages;
        d->nav->setProject(d->prj, QString()/*all classes*/, &partManagerErrorMessages);
        if (!partManagerErrorMessages.isEmpty()) {
            showWarningContinueMessage(partManagerErrorMessages, QString(),
                                       "dontShowWarningsRelatedToPluginsLoading");
        }
    }
    connect(d->prj, SIGNAL(newItemStored(KexiPart::Item&)), d->nav, SLOT(addItem(KexiPart::Item&)));
    d->nav->setFocus();

    if (d->forceShowProjectNavigatorOnCreation) {
        slotViewNavigator();
        d->forceShowProjectNavigatorOnCreation = false;
    } else if (d->forceHideProjectNavigatorOnCreation) {
#ifdef __GNUC__
#warning TODO d->navToolWindow->hide();
#else
#pragma WARNING( TODO d->navToolWindow->hide(); )
#endif
//  makeDockInvisible( manager()->findWidgetParentDock(d->nav) );
        d->forceHideProjectNavigatorOnCreation = false;
    }

    invalidateActions();
}

void KexiMainWindow::slotLastActions()
{
    /*2.0:
    #if defined(KDOCKWIDGET_P)
      if (mdiMode()==KMdi::ChildframeMode || mdiMode()==KMdi::TabPageMode) {
    //  KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
        //KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
    //  Q_UNUSED(ds);
    //1  ds->resize(ds->width()*3, ds->height());
    //1  ds->setSeparatorPos(30, true);
    //1  ds->setForcedFixedWidth( dw, 200 );
      }
    #endif
    #ifdef Q_WS_WIN
      showMaximized();//js: workaround for not yet completed layout settings storage on win32
    #endif
    */
}

void KexiMainWindow::setupPropertyEditor()
{
    if (!d->propEditor) {
        KConfigGroup mainWindowGroup(d->config->group("MainWindow"));
//TODO: FIX LAYOUT PROBLEMS
        d->propEditorDockWidget = new KexiDockWidget(i18n("Property Editor"), d->mainWidget);
        d->propEditorDockWidget->setObjectName("PropertyEditorDockWidget");
        d->mainWidget->addDockWidget(
            loadDockAreaSetting(mainWindowGroup, "PropertyEditorArea", Qt::RightDockWidgetArea),
            d->propEditorDockWidget,
            Qt::Vertical
        );
        connect(d->propEditorDockWidget, SIGNAL(visibilityChanged(bool)),
            this, SLOT(slotPropertyEditorVisibilityChanged(bool)));

        d->propEditorDockableWidget = new KexiDockableWidget(d->propEditorDockWidget);
        d->propEditorDockWidget->setWidget(d->propEditorDockableWidget);
        const QSize propertyEditorSize = mainWindowGroup.readEntry<QSize>("PropertyEditorSize", QSize());
        if (!propertyEditorSize.isNull()) {
            d->propEditorDockableWidget->setSizeHint(propertyEditorSize);
        }

        QWidget *propEditorDockWidgetContents = new QWidget(d->propEditorDockableWidget);
        d->propEditorDockableWidget->setWidget(propEditorDockWidgetContents);
        QVBoxLayout *propEditorDockWidgetContentsLyr = new QVBoxLayout(propEditorDockWidgetContents);
        KexiUtils::setMargins(propEditorDockWidgetContentsLyr, KDialog::marginHint() / 2);

        d->propEditorTabWidget = new KTabWidget(propEditorDockWidgetContents);
        propEditorDockWidgetContentsLyr->addWidget(d->propEditorTabWidget);
//  d->propEditorTabWidget->hide();
        d->propEditor = new KexiPropertyEditorView(d->propEditorTabWidget);
        d->propEditorTabWidget->setWindowTitle(d->propEditor->windowTitle());
        d->propEditorTabWidget->addTab(d->propEditor, i18n("Properties"));
//TODO REMOVE?  d->propEditor->installEventFilter(this);

        KConfigGroup propertyEditorGroup(d->config->group("PropertyEditor"));
        int size = propertyEditorGroup.readEntry("FontSize", -1);
        QFont f(KexiUtils::smallFont());
        if (size > 0)
            f.setPixelSize(size);
        d->propEditorTabWidget->setFont(f);

        /*2.0:
            if (mdiMode()==KMdi::ChildframeMode || mdiMode()==KMdi::TabPageMode) {
            KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
          #if defined(KDOCKWIDGET_P)
              KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
              makeWidgetDockVisible(d->propEditorTabWidget);

              d->config->setGroup("MainWindow");
              ds->setSeparatorPosInPercent(d->config->readEntry("RightDockPosition", 80));
          #endif
            }*/
        d->enable_slotPropertyEditorVisibilityChanged = false;
        d->propEditorDockWidget->setVisible(false);
        d->enable_slotPropertyEditorVisibilityChanged = true;
    }
}

void KexiMainWindow::slotPartLoaded(KexiPart::Part* p)
{
    if (!p)
        return;
    connect(p, SIGNAL(newObjectRequest(KexiPart::Info*)),
            this, SLOT(newObject(KexiPart::Info*)));
    p->createGUIClients();//this);
}

#if 0
//! internal
void KexiMainWindow::slotCaptionForCurrentMDIChild(bool childrenMaximized)
{
#ifdef __GNUC__
#warning TODO: slotCaptionForCurrentMDIChild
#else
#pragma WARNING( TODO: slotCaptionForCurrentMDIChild )
#endif
#if 0//TODO
//! @todo allow to set custom "static" app caption

    KMdiChildView *view = 0L;
    if (!currentWindow())
        view = 0;
    else if (currentWindow()->isAttached()) {
        view = currentWindow();
    } else {
        //current dialog isn't attached! - find top level child
        if (m_pMdi->topChild()) {
            view = m_pMdi->topChild()->m_pClient;
            childrenMaximized = view->mdiParent()->state() == KMdiChildFrm::Maximized;
        } else
            view = 0;
    }

    if (childrenMaximized && view) {
        setWindowTitle(currentWindow()->caption()
                       + (d->appCaptionPrefix.isEmpty() ? QString() : (QString::fromLatin1(" - ") + d->appCaptionPrefix)));
    } else {
        setWindowTitle((d->appCaptionPrefix.isEmpty() ? QString() : (d->appCaptionPrefix + QString::fromLatin1(" - ")))
                       + d->origAppCaption);
    }
#endif
}
#endif //0

void KexiMainWindow::updateAppCaption()
{
//! @todo allow to set custom "static" app caption

    d->appCaptionPrefix.clear();
    if (d->prj && d->prj->data()) {//add project name
        d->appCaptionPrefix = d->prj->data()->caption();
        if (d->appCaptionPrefix.isEmpty())
            d->appCaptionPrefix = d->prj->data()->databaseName();
    }
// if (!d->appCaptionPrefix.isEmpty())
//  d->appCaptionPrefix = d->appCaptionPrefix;

// bool max = false;
#ifdef __GNUC__
#warning TODO if (currentWindow() && currentWindow()->mdiParent()) max = currentWindow()->mdiParent()->state()==KMdiChildFrm::Maximized;
#else
#pragma WARNING( TODO if (currentWindow() && currentWindow()->mdiParent()) max = currentWindow()->mdiParent()->state()==KMdiChildFrm::Maximized; )
#endif

// slotCaptionForCurrentMDIChild(max);
    setWindowTitle((d->appCaptionPrefix.isEmpty() ? QString() : (d->appCaptionPrefix + QString::fromLatin1(" - ")))
                   + KGlobal::mainComponent().aboutData()->programName());

    /*
      KMdiChildView *view;
      if (!currentWindow())
        view = 0;
      else if (currentWindow()->isAttached()) {
        view = currentWindow();
      } else {
        //current dialog isn't attached! - find top level child
        if (m_pMdi->topChild()) {
          view = m_pMdi->topChild()->m_pClient;
        }
        else
          view = 0;
      }

      kApp->setCaption( d->appCaption );
      if (view && view->mdiParent()->state()==KMdiChildFrm::Maximized) {
        setCaption( view->caption() );
      }
      else {
        setCaption( d->appCaption );
      }*/
}

#if 0 //2.0: unused
void KexiMainWindow::slotNoMaximizedChildFrmLeft(KMdiChildFrm*)
{
    slotCaptionForCurrentMDIChild(false);
}
#endif

/*
void KexiMainWindow::slotLastChildViewClosed() //slotLastChildFrmClosed()
{
#ifdef __GNUC__
#warning TODO if (m_pDocumentViews->count()>0) //a fix for KMDI bug (will be fixed in KDE 3.4)
#else
#pragma WARNING( TODO if (m_pDocumentViews->count()>0) //a fix for KMDI bug (will be fixed in KDE 3.4) )
#endif
#ifdef __GNUC__
#warning TODO  return;
#else
#pragma WARNING( TODO  return; )
#endif

  slotCaptionForCurrentMDIChild(false);
  activeWindowChanged(0, 0);
}*/

/*
void KexiMainWindow::slotChildViewIsDetachedNow(QWidget*)
{
  slotCaptionForCurrentMDIChild(false);
}*/

bool KexiMainWindow::queryClose()
{
#ifndef KEXI_NO_PENDING_DIALOGS
    if (d->pendingWindowsExist()) {
        kDebug() << "KexiMainWindow::queryClose() pendingWindowsExist...";
        d->actionToExecuteWhenPendingJobsAreFinished = Private::QuitAction;
        return false;
    }
#endif
// storeSettings();
    const tristate res = closeProject();
    if (~res)
        return false;

    if (res == true)
        storeSettings();

    return ! ~res;
}

bool KexiMainWindow::queryExit()
{
    //storeSettings();
    return true;
}

void KexiMainWindow::closeEvent(QCloseEvent *ev)
{
    d->mainWidget->closeEvent(ev);
/*    if (queryClose()) {
        ev->accept();
    }*/
}

void
KexiMainWindow::restoreSettings()
{
    KConfigGroup mainWindowGroup(d->config->group("MainWindow"));
// restoreWindowSize( mainWindowGroup );
// d->mainWidget->applyMainWindowSettings( mainWindowGroup );
// saveState()
    const bool maximize = mainWindowGroup.readEntry("Maximized", false);
    const QRect geometry(mainWindowGroup.readEntry("Geometry", QRect()));
    if (geometry.isValid())
        setGeometry(geometry);
    else if (maximize)
        setWindowState(windowState() | Qt::WindowMaximized);
//    return;

    // Saved settings
#ifdef __GNUC__
#warning TODO applyMainWindowSettings()
#else
#pragma WARNING( TODO applyMainWindowSettings() )
#endif
#if 0//TODO ?
// applyMainWindowSettings( d->config, "MainWindow" );

    //small hack - set the default -- bottom
// d->config->setGroup(QString(name()) + " KMdiTaskBar Toolbar style");
    d->config->setGroup("MainWindow Toolbar KMdiTaskBar");
    const bool tbe = d->config->readEntry("Position").isEmpty();
    if (tbe || d->config->readEntry("Position") == "Bottom") {
        if (tbe)
            d->config->writeEntry("Position", "Bottom");
        moveDockWindow(m_pTaskBar, Qt::DockBottom);
    }
#endif

#if 0
    if (showProjectNavigator) {
        //it's invisible by default but we want to show it on navigator creation
        d->forceShowProjectNavigatorOnCreation = true;
    }
#endif
}

void
KexiMainWindow::storeSettings()
{
    kDebug() << "KexiMainWindow::storeSettings()";
    KConfigGroup mainWindowGroup(d->config->group("MainWindow"));
    //saveWindowSize( mainWindowGroup );

    if (isMaximized()) {
        mainWindowGroup.writeEntry("Maximized", true);
        mainWindowGroup.deleteEntry("Geometry");
    } else {
        mainWindowGroup.deleteEntry("Maximized");
        mainWindowGroup.writeEntry("Geometry", geometry());
    }

    saveDockAreaSetting(mainWindowGroup, "ProjectNavigatorArea", d->mainWidget->dockWidgetArea(d->navDockWidget));
    saveDockAreaSetting(mainWindowGroup, "PropertyEditorArea", d->mainWidget->dockWidgetArea(d->propEditorDockWidget));

// mainWindowGroup.writeEntry("PropertyEditor", mb->isHidden() ? "Disabled" : "Enabled");
// d->mainWidget->saveMainWindowSettings( mainWindowGroup );
// d->mainWidget->saveState();

    if (d->nav)
        mainWindowGroup.writeEntry("ProjectNavigatorSize", d->nav->parentWidget()->size());

    if (d->propEditorDockableWidget)
        mainWindowGroup.writeEntry("PropertyEditorSize", d->propEditorDockableWidget->size());

    KGlobal::config()->sync();
    return;


// saveWindowSize( d->config ); //componentData().config() );
#ifdef __GNUC__
#warning TODO saveMainWindowSettings( d->config, "MainWindow" );
#else
#pragma WARNING( TODO saveMainWindowSettings( d->config, "MainWindow" ); )
#endif

    /*2.0: KMdi::MdiMode modeToSave = mdiMode();
      if (d->mdiModeToSwitchAfterRestart!=(KMdi::MdiMode)0)
        modeToSave = d->mdiModeToSwitchAfterRestart;
      if (modeToSave == DEFAULT_MDI_MODE)
        d->config->deleteEntry("MDIMode");
      else
        d->config->writeEntry("MDIMode", modeToSave);
      d->config->writeEntry("maximized childframes", isInMaximizedChildFrmMode());
    */
    if (d->saveSettingsForShowProjectNavigator) {
        if (d->navWasVisibleBeforeProjectClosing)
            mainWindowGroup.deleteEntry("ShowProjectNavigator");
        else
            mainWindowGroup.writeEntry("ShowProjectNavigator", false);
    }

    /*2.0:
      if (modeToSave==KMdi::ChildframeMode || modeToSave==KMdi::TabPageMode) {
        if (d->propEditor && d->propEditorDockSeparatorPos >= 0 && d->propEditorDockSeparatorPos <= 100) {
          d->config->setGroup("MainWindow");
          d->config->writeEntry("RightDockPosition", d->propEditorDockSeparatorPos);
        }
        else
          d->propEditorDockSeparatorPos = 80;
        if (d->nav && d->navDockSeparatorPos >= 0 && d->navDockSeparatorPos <= 100) {
          d->config->setGroup("MainWindow");
          //KDockWidget *dw = (KDockWidget *)d->nav->parentWidget();
          //int w = dw->width();
          //int ww = width();
          //int d1 = (100 * dw->width()) / width() + 1;
          //KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
          //int d2 = ds->separatorPosInPercent();
          if (d->wasAutoOpen && d->windowExistedBeforeCloseProject) {
    #ifdef Q_WS_WIN
            d->config->writeEntry("LeftDockPositionWithAutoOpen",
              d->navDockSeparatorPos);
    #endif
    //   d->config->writeEntry("LeftDockPosition", dw->width());
    //   d->config->writeEntry("LeftDockPosition", d->nav->width());
          } else {
    #ifdef Q_WS_WIN
            if (d->windowExistedBeforeCloseProject)
              d->config->writeEntry("LeftDockPosition", d->navDockSeparatorPos);
            else
              d->config->writeEntry("LeftDockPosition", qRound(double(d->navDockSeparatorPos) / 0.77
               / (double(d->propEditorDockSeparatorPos) / 80) ));
    #endif
          }
        }
      }*/

    if (d->propEditor) {
        KConfigGroup propertyEditorGroup(d->config->group("PropertyEditor"));
        propertyEditorGroup.writeEntry("FontSize", d->propEditorTabWidget->font().pixelSize());
    }
}

/*
void
KexiMainWindow::restoreWindowConfiguration(KConfig *config)
{
#ifdef __GNUC__
#warning TODO restoreWindowConfiguration()
#else
#pragma WARNING( TODO restoreWindowConfiguration() )
#endif
#if 0//TODO?
  kDebug()<<"preparing session restoring";

  config->setGroup("MainWindow");

  QString dockGrp;

  if (kapp->isSessionRestored())
    dockGrp=config->group()+"-Docking";
  else
    dockGrp="MainWindow0-Docking";

  if (config->hasGroup(dockGrp))
    readDockConfig(config,dockGrp);
#endif
}

void
KexiMainWindow::storeWindowConfiguration(KConfig *config)
{
#ifdef __GNUC__
#warning TODO storeWindowConfiguration()
#else
#pragma WARNING( TODO storeWindowConfiguration() )
#endif
#if 0//TODO?
  kDebug()<<"preparing session saving";
  config->setGroup("MainWindow");
  QString dockGrp;

#if !defined(Q_WS_WIN)
  if (kapp->sessionSaving())
    dockGrp=config->group()+"-Docking";
  else
#endif
    dockGrp="MainWindow0-Docking";

  kDebug()<<"Before write dock config";
  writeDockConfig(config,dockGrp);
  kDebug()<<"After write dock config";
#endif
}

void
KexiMainWindow::readProperties(KConfig *config) {
  restoreWindowConfiguration(config);
}

void
KexiMainWindow::saveProperties(KConfig *config)
{
  storeWindowConfiguration(config);
//        m_docManager->saveDocumentList (config);
  //      m_projectManager->saveProjectList (config);
}

void
KexiMainWindow::saveGlobalProperties( KConfig* sessionConfig )
{
  storeWindowConfiguration(sessionConfig);
}*/

void
KexiMainWindow::registerChild(KexiWindow *window)
{
    kDebug() << "KexiMainWindow::registerChild()";
    /*#ifdef __GNUC__
    #warning TODO connect(window, SIGNAL(activated(KMdiChildView *)), this, SLOT(activeWindowChanged(KMdiChildView *)));
    #else
    #pragma WARNING( TODO connect(window, SIGNAL(activated(KMdiChildView *)), this, SLOT(activeWindowChanged(KMdiChildView *))); )
    #endif*/
    connect(window, SIGNAL(dirtyChanged(KexiWindow*)), this, SLOT(slotDirtyFlagChanged(KexiWindow*)));
#ifdef __GNUC__
#warning TODO connect(window, SIGNAL(dirtyChanged(KexiWindow*)), this, SLOT(slotDirtyFlagChanged(KexiWindow*)));
#else
#pragma WARNING( TODO connect(window, SIGNAL(dirtyChanged(KexiWindow*)), this, SLOT(slotDirtyFlagChanged(KexiWindow*))); )
#endif

    if (window->id() != -1) {
        d->insertWindow(window);
    }
    kDebug() << "KexiMainWindow::registerChild() ID = " << window->id();

    /*2.0: if (m_mdiMode==KMdi::ToplevelMode || m_mdiMode==KMdi::ChildframeMode) {//kmdi fix
        //js TODO: check if taskbar is switched in menu
        if (m_pTaskBar && !m_pTaskBar->isSwitchedOn())
          m_pTaskBar->switchOn(true);
      }*/
}

void
KexiMainWindow::updateWindowViewGUIClient(KXMLGUIClient *viewClient)
{
    Q_UNUSED(viewClient);
#ifdef __GNUC__
#warning TODO updateDialogViewGUIClient()
#else
#pragma WARNING( TODO updateDialogViewGUIClient() )
#endif
#if 0//TODO
    if (viewClient != d->curWindowViewGUIClient) {
        //view clients differ
        kDebug() << "KexiMainWindow::updateWindowViewGUIClient(): old view gui client:"
        << (d->curWindowViewGUIClient ? d->curWindowViewGUIClient->xmlFile() : "")
        << " new view gui client: " << (viewClient ? viewClient->xmlFile() : "");
        if (d->curWindowViewGUIClient) {
            guiFactory()->removeClient(d->curWindowViewGUIClient);
        }
        if (viewClient) {
            if (d->closedDialogViewGUIClient) {
                //ooh, there is a client which dialog is already closed -- BUT it is the same client as our
                //so: give up
            } else {
                guiFactory()->addClient(viewClient);
            }
        }
    }
#endif
}

void KexiMainWindow::updateCustomPropertyPanelTabs(KexiWindow *prevWindow,
        Kexi::ViewMode prevViewMode)
{
    updateCustomPropertyPanelTabs(
        prevWindow ? prevWindow->part() : 0,
        prevWindow ? prevWindow->currentViewMode() : prevViewMode,
        currentWindow() ? currentWindow()->part() : 0,
        currentWindow() ? currentWindow()->currentViewMode() : Kexi::NoViewMode
    );
}

void KexiMainWindow::updateCustomPropertyPanelTabs(
    KexiPart::Part *prevWindowPart, Kexi::ViewMode prevViewMode,
    KexiPart::Part *curWindowPart, Kexi::ViewMode curViewMode)
{
    if (!d->propEditorTabWidget)
        return;

    if (   !curWindowPart
        || (/*prevWindowPart &&*/ curWindowPart
             && (prevWindowPart != curWindowPart || prevViewMode != curViewMode)
           )
       )
    {
        if (d->partForPreviouslySetupPropertyPanelTabs) {
            //remember current page number for this part
            if ((   prevViewMode == Kexi::DesignViewMode
                 && static_cast<KexiPart::Part*>(d->partForPreviouslySetupPropertyPanelTabs) != curWindowPart) //part changed
                || curViewMode != Kexi::DesignViewMode)
            { //..or switching to other view mode
                d->recentlySelectedPropertyPanelPages.insert(
                        d->partForPreviouslySetupPropertyPanelTabs,
                        d->propEditorTabWidget->currentIndex());
            }
        }

        //delete old custom tabs (other than 'property' tab)
        const uint count = d->propEditorTabWidget->count();
        for (uint i = 1; i < count; i++)
            d->propEditorTabWidget->removeTab(1);
    }

    //don't change anything if part is not switched nor view mode changed
    if ((!prevWindowPart && !curWindowPart)
            || (prevWindowPart == curWindowPart && prevViewMode == curViewMode)
            || (curWindowPart && curViewMode != Kexi::DesignViewMode))
    {
        //new part for 'previously setup tabs'
        d->partForPreviouslySetupPropertyPanelTabs = curWindowPart;
        return;
    }

    if (curWindowPart) {
        //recreate custom tabs
        curWindowPart->setupCustomPropertyPanelTabs(d->propEditorTabWidget);

        //restore current page number for this part
        if (d->recentlySelectedPropertyPanelPages.contains(curWindowPart)) {
            d->propEditorTabWidget->setCurrentIndex(
                d->recentlySelectedPropertyPanelPages[ curWindowPart ]
            );
        }
    }

    //new part for 'previously setup tabs'
    d->partForPreviouslySetupPropertyPanelTabs = curWindowPart;
}

void KexiMainWindow::activeWindowChanged(KexiWindow *window, KexiWindow *prevWindow)
{
    kDebug() << "to=" << (window ? window->windowTitle() : "<none>");

//! @todo gui clients?
#if 0
    KXMLGUIClient *client = 0; //common for all views
    KXMLGUIClient *viewClient = 0; //specific for current dialog's view
// KexiWindow* prevWindow = currentWindow();

    if (!window)
        client = 0;
    else if (window->isRegistered()) {
        client = window->commonGUIClient();
        viewClient = window->guiClient();
        if (d->closedWindowGUIClient) {
            if (client != d->closedWindowGUIClient) {
                //ooh, there is a client which dialog is already closed -- and we don't want it
                guiFactory()->removeClient(d->closedWindowGUIClient);
                d->closedWindowGUIClient = 0;
            }
        }
        if (d->closedWindowViewGUIClient) {
            if (viewClient != d->closedWindowViewGUIClient) {
                //ooh, there is a client which dialog is already closed -- and we don't want it
                guiFactory()->removeClient(d->closedWindowViewGUIClient);
                d->closedWindowViewGUIClient = 0;
            }
        }
        if (client != d->curWindowGUIClient) {
            //clients differ
            kDebug() << "KexiMainWindow::activeWindowChanged(): old gui client:"
            << (d->curWindowGUIClient ? d->curWindowGUIClient->xmlFile() : "")
            << " new gui client: " << (client ? client->xmlFile() : "");
            if (d->curWindowGUIClient) {
                guiFactory()->removeClient(d->curWindowGUIClient);
                currentWindow()->detachFromGUIClient();
            }
            if (client) {
                if (d->closedWindowGUIClient) {
                    //ooh, there is a client which dialog is already closed -- BUT it is the same client as our
                    //so: give up
                } else {
                    guiFactory()->addClient(client);
                }
                window->attachToGUIClient();
            }
        } else {
            //clients are the same
            if (prevWindow != window) {
                if (prevWindow)
                    prevWindow->detachFromGUIClient();
                if (window)
                    window->attachToGUIClient();
            }
        }
        updateWindowViewGUIClient(viewClient);
        /*  if (viewClient!=d->curWindowViewGUIClient) {
              //view clients differ
              kDebug()<<"KexiMainWindow::activeWindowChanged(): old view gui client:"
                <<d->curWindowViewGUIClient<<" new view gui client: "<<viewClient;
              if (d->curWindowViewGUIClient) {
                guiFactory()->removeClient(d->curWindowViewGUIClient);
              }
              if (viewClient) {
                if (d->closedWindowViewGUIClient) {
                  //ooh, there is a client which dialog is already closed -- BUT it is the same client as our
                  //so: give up
                }
                else {
                  guiFactory()->addClient(viewClient);
                }
              }
            }*/
    }
#endif // gui clients

#ifdef __GNUC__
#warning TODO update_dlg_caption
#else
#pragma WARNING( TODO update_dlg_caption )
#endif
// bool update_dlg_caption = window && window != prevWindow && window->mdiParent();

//! @todo gui clients?
#if 0
    if (d->curWindowGUIClient && !client)
        guiFactory()->removeClient(d->curWindowGUIClient);
    d->curWindowGUIClient = client;

    if (d->curWindowViewGUIClient && !viewClient)
        guiFactory()->removeClient(d->curWindowViewGUIClient);
    d->curWindowViewGUIClient = viewClient;
#endif

    bool windowChanged = prevWindow != window;

//?? window->setUpdatesEnabled(false);

    if (windowChanged) {
        if (prevWindow) {
            //inform previously activated dialog about deactivation
            prevWindow->deactivate();
        }
    }
// currentWindow() = dlg;

//moved below: propertySetSwitched(currentWindow());
    updateCustomPropertyPanelTabs(prevWindow, prevWindow ? prevWindow->currentViewMode() : Kexi::NoViewMode);

    // inform the current view of the new dialog about property switching
    // (this will also call KexiMainWindow::propertySetSwitched() to update the current property editor's set
    if (windowChanged && currentWindow())
        currentWindow()->selectedView()->propertySetSwitched();

    if (windowChanged) {
//  invalidateSharedActions();
        //update property editor's contents...
//  if ((KexiPropertyBuffer*)d->propBuffer!=currentWindow()->propertyBuffer()) {
//  propertyBufferSwitched();//currentWindow());
//   d->propBuffer = currentWindow()->propertyBuffer();
//   d->propEditor->editor()->setBuffer( d->propBuffer );
//  }
        if (currentWindow() && currentWindow()->currentViewMode() != 0 && window) //on opening new dialog it can be 0; we don't want this
            d->updatePropEditorVisibility(currentWindow()->currentViewMode());
    }

    //update caption...
#ifdef __GNUC__
#warning TODO slotCaptionForCurrentMDIChild()
#else
#pragma WARNING( TODO slotCaptionForCurrentMDIChild() )
#endif
    /* if (update_dlg_caption) {//currentWindow() is != null for sure
        slotCaptionForCurrentMDIChild(currentWindow()->mdiParent()->state()==KMdiChildFrm::Maximized);
      }*/
//UNUSED invalidateViewModeActions();
    invalidateActions();
    d->updateFindDialogContents();
//?? window->setUpdatesEnabled(true);
    if (window)
        window->setFocus();
}

bool
KexiMainWindow::activateWindow(int id)
{
    kDebug() << "KexiMainWindow::activateWindow()";
#ifndef KEXI_NO_PENDING_DIALOGS
    Private::PendingJobType pendingType;
    return activateWindow(*d->openedWindowFor(id, pendingType));
#else
    return activateWindow(*d->openedWindowFor(id));
#endif
}

bool
KexiMainWindow::activateWindow(KexiWindow& window)
{
    kDebug() << "KexiMainWindow::activateWindow(KexiWindow&)";

    d->focus_before_popup = &window;
    d->mainWidget->tabWidget()->setCurrentWidget(window.parentWidget()/*container*/);
    window.activate();
    return true;
}

void KexiMainWindow::activateNextWindow()
{
#ifdef __GNUC__
#warning TODO activateNextWindow()
#else
#pragma WARNING( TODO activateNextWindow() )
#endif
}

void KexiMainWindow::activatePreviousWindow()
{
#ifdef __GNUC__
#warning TODO activatePreviousWindow()
#else
#pragma WARNING( TODO activatePreviousWindow() )
#endif
}

#if 0 //TODO
void
KexiMainWindow::childClosed(KMdiChildView *v)
{
    KexiWindowBase *dlg = static_cast<KexiWindow *>(v);
    d->removeWindow(dlg->id());
#ifndef KEXI_NO_PENDING_DIALOGS
    d->removePendingWindow(dlg->id());
#endif

    //focus navigator if nothing else available
    if (d->openedWindowsCount() == 0)
        d->nav->setFocus();
}
#endif

void
KexiMainWindow::slotShowSettings()
{
    KEXI_UNFINISHED(d->action_configure->text());
//TODO KexiSettings s(this);
// s.exec();
}

void
KexiMainWindow::slotConfigureKeys()
{
    /*    KShortcutsDialog dlg;
        dlg.insert( actionCollection() );
        dlg.configure();*/
    KShortcutsDialog::configure(actionCollection(),
                                KShortcutsEditor::LetterShortcutsDisallowed, this);
}

void
KexiMainWindow::slotConfigureToolbars()
{
    KEditToolBar edit(actionCollection());//factory());
//    connect(&edit,SIGNAL(newToolbarConfig()),this,SLOT(slotNewToolbarConfig()));
    (void) edit.exec();
}

void
KexiMainWindow::slotProjectNew()
{
    if (!d->prj) {
        //create within this instance
        createBlankProject();
        return;
    }
//TODO use KexiStartupDialog(KexiStartupDialog::Templates...)

    bool cancel;
    QString fileName;
    KexiProjectData *new_data = createBlankProjectData(
                                    cancel,
                                    false, /* do not confirm prj overwrites: user will be asked on process startup */
                                    &fileName //shortcut fname
                                );
    if (!new_data)
        return;

    QStringList args;
    args << qApp->applicationFilePath() << "-create-opendb";
    if (new_data->connectionData()->fileName().isEmpty()) {
        //server based - pass .kexic file
        if (fileName.isEmpty())
            return;
        args << new_data->databaseName() << fileName;
        //args << "--skip-conn-dialog"; //user does not expect conn. dialog to be shown here
    } else {
        //file based
        fileName = new_data->connectionData()->fileName();
        args << fileName;
    }
//todo:   pass new_data->caption()
    //start new instance
//! @todo use KProcess?
    Q3Process proc(args, this, "process");
    proc.setCommunication((Q3Process::Communication)0);
//  proc.setWorkingDirectory( QFileInfo(new_data->connectionData()->fileName()).dir(true) );
    proc.setWorkingDirectory(QFileInfo(fileName).absoluteDir());
    if (!proc.start()) {
        d->showStartProcessMsg(args);
    }
    delete new_data;
}

void
KexiMainWindow::createKexiProject(KexiProjectData* new_data)
{
    d->prj = new KexiProject(new_data, this);
// d->prj = ::createKexiProject(new_data);
//provided by KexiMessageHandler connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
//provided by KexiMessageHandler connect(d->prj, SIGNAL(error(const QString&,const QString&)), this, SLOT(showErrorMessage(const QString&,const QString&)));
    connect(d->prj, SIGNAL(itemRenamed(const KexiPart::Item&, const QString&)), this, SLOT(slotObjectRenamed(const KexiPart::Item&, const QString&)));

    if (d->nav)
        connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)), d->nav, SLOT(slotRemoveItem(const KexiPart::Item&)));
}

KexiProjectData*
KexiMainWindow::createBlankProjectData(bool &cancelled, bool confirmOverwrites,
                                       QString* shortcutFileName)
{
    cancelled = false;
    KexiNewProjectWizard wiz(Kexi::connset(), 0);
    wiz.setConfirmOverwrites(confirmOverwrites);
    if (wiz.exec() != QDialog::Accepted) {
        cancelled = true;
        return 0;
    }

    KexiProjectData *new_data;

    if (shortcutFileName)
        shortcutFileName->clear();
    if (wiz.projectConnectionData()) {
        //server-based project
        KexiDB::ConnectionData *cdata = wiz.projectConnectionData();
        kDebug() << "DBNAME: " << wiz.projectDBName() << " SERVER: " << cdata->serverInfoString();
        new_data = new KexiProjectData(*cdata, wiz.projectDBName(), wiz.projectCaption());
        if (shortcutFileName)
            *shortcutFileName = Kexi::connset().fileNameForConnectionData(cdata);
    } else if (!wiz.projectDBName().isEmpty()) {
        //file-based project
        KexiDB::ConnectionData cdata;
        cdata.caption = wiz.projectCaption();
        cdata.driverName = KexiDB::defaultFileBasedDriverName();
        cdata.setFileName(wiz.projectDBName());
        new_data = new KexiProjectData(cdata, wiz.projectDBName(), wiz.projectCaption());
    } else {
        cancelled = true;
        return 0;
    }
    return new_data;
}

tristate
KexiMainWindow::createBlankProject()
{
    bool cancel;
    KexiProjectData *new_data = createBlankProjectData(cancel);
    if (cancel)
        return cancelled;
    if (!new_data)
        return false;

    createKexiProject(new_data);

    tristate res = d->prj->create(true /*overwrite*/);
    if (res != true) {
        delete d->prj;
        d->prj = 0;
        return res;
    }
    kDebug() << "KexiMainWindow::slotProjectNew(): new project created --- ";
    setupProjectNavigator();
    Kexi::recentProjects().addProjectData(new_data);

    invalidateActions();
    updateAppCaption();
    return true;
}

void
KexiMainWindow::slotProjectOpen()
{
    KexiStartupDialog dlg(
        KexiStartupDialog::OpenExisting, 0, Kexi::connset(), Kexi::recentProjects(),
        this);

    if (dlg.exec() != QDialog::Accepted)
        return;

    openProject(dlg.selectedFileName(), dlg.selectedExistingConnection());
}

tristate KexiMainWindow::openProject(const QString& aFileName,
                                     const QString& fileNameForConnectionData, const QString& dbName)
{
    if (d->prj)
        return openProjectInExternalKexiInstance(aFileName, fileNameForConnectionData, dbName);

    KexiDB::ConnectionData *cdata = 0;
    if (!fileNameForConnectionData.isEmpty()) {
        cdata = Kexi::connset().connectionDataForFileName(fileNameForConnectionData);
        if (!cdata) {
            kWarning() << "KexiMainWindow::openProject() cdata?";
            return false;
        }
    }
    return openProject(aFileName, cdata, dbName);
}

tristate KexiMainWindow::openProject(const QString& aFileName,
                                     KexiDB::ConnectionData *cdata, const QString& dbName,
                                     const KexiProjectData::AutoOpenObjects& autoopenObjects)
{
    if (d->prj) {
        return openProjectInExternalKexiInstance(aFileName, cdata, dbName);
    }

    KexiProjectData* projectData = 0;
    bool deleteAfterOpen = false;
    if (cdata) {
        //server-based project
        if (dbName.isEmpty()) {//no database name given, ask user
            bool cancel;
            projectData = Kexi::startupHandler().selectProject(cdata, cancel, this);
            if (cancel)
                return cancelled;
        } else {
//! @todo caption arg?
            projectData = new KexiProjectData(*cdata, dbName);
            deleteAfterOpen = true;
        }
    } else {
//  QString selFile = dlg.selectedExistingFile();
        if (aFileName.isEmpty()) {
            kWarning() << "KexiMainWindow::openProject(): aFileName.isEmpty()";
            return false;
        }
        //file-based project
        kDebug() << "Project File: " << aFileName;
        KexiDB::ConnectionData cdata;
        cdata.setFileName(aFileName);
//   cdata.driverName = KexiStartupHandler::detectDriverForFile( cdata.driverName, fileName, this );
        QString detectedDriverName;
        KexiStartupData::Import importActionData;
        const tristate res = KexiStartupHandler::detectActionForFile(
                                 importActionData, detectedDriverName, cdata.driverName, aFileName, this);
        if (true != res)
            return res;

        if (importActionData) { //importing requested
            return showProjectMigrationWizard(importActionData.mimeType, importActionData.fileName);
        }
        cdata.driverName = detectedDriverName;

        if (cdata.driverName.isEmpty())
            return false;

        //opening requested
        projectData = new KexiProjectData(cdata, aFileName);
        deleteAfterOpen = true;
    }
    if (!projectData)
        return false;
    projectData->autoopenObjects = autoopenObjects;
    const tristate res = openProject(*projectData);
    if (deleteAfterOpen) //projectData object has been copied
        delete projectData;
    return res;
}

tristate KexiMainWindow::openProjectInExternalKexiInstance(const QString& aFileName,
        KexiDB::ConnectionData *cdata, const QString& dbName)
{
    QString fileNameForConnectionData;
    if (aFileName.isEmpty()) { //try .kexic file
        if (cdata)
            fileNameForConnectionData = Kexi::connset().fileNameForConnectionData(cdata);
    }
    return openProjectInExternalKexiInstance(aFileName, fileNameForConnectionData, dbName);
}

tristate KexiMainWindow::openProjectInExternalKexiInstance(const QString& aFileName,
        const QString& fileNameForConnectionData, const QString& dbName)
{
    QString fileName(aFileName);
    QStringList args;
    args << qApp->applicationFilePath();
    // open a file-based project or a server connection provided as a .kexic file
    // (we have no other simple way to provide the startup data to a new process)
    if (fileName.isEmpty()) { //try .kexic file
        if (!fileNameForConnectionData.isEmpty())
            args << "--skip-conn-dialog"; //user does not expect conn. dialog to be shown here

        if (dbName.isEmpty()) { //use 'kexi --skip-conn-dialog file.kexic'
            fileName = fileNameForConnectionData;
        } else { //use 'kexi --skip-conn-dialog --connection file.kexic dbName'
            args << "--connection" << fileNameForConnectionData;
            fileName = dbName;
        }
    }
    if (fileName.isEmpty()) {
        kWarning() << "KexiMainWindow::openProjectInExternalKexiInstance() fileName?";
        return false;
    }
//! @todo use KRun
    args << fileName;
    Q3Process proc(args, this, "process");
    proc.setWorkingDirectory(QFileInfo(fileName).absoluteDir());
    const bool ok = proc.start();
    if (!ok) {
        d->showStartProcessMsg(args);
    }
    return ok;
}

void
KexiMainWindow::slotProjectOpenRecentAboutToShow()
{
    /*
    //setup
    KMenu *popup = d->action_open_recent->popupMenu();
    const int cnt = popup->count();
    //remove older
    for (int i = 0; i<cnt; i++) {
      int id = popup->idAt(0);
      if (id==d->action_open_recent_more_id)
        break;
      if (id>=0) {
        popup->removeItem(id);
      }
    }
    //insert current items
    int cur_id = 0, cur_idx = 0;
    //TODO:
    cur_id = popup->insertItem("My example project 1", ++cur_id, cur_idx++);
    cur_id = popup->insertItem("My example project 2", ++cur_id, cur_idx++);
    cur_id = popup->insertItem("My example project 3", ++cur_id, cur_idx++);
    */

    //show recent databases
    KMenu *popup = d->action_open_recent->menu();
    popup->clear();
#if 0
    d->action_open_recent_projects_title_id = popup->insertTitle(i18n("Recently Opened Databases"));
#endif
// int action_open_recent_projects_title_index = popup->indexOf(d->action_open_recent_projects_title_id);
// int count = popup->count();
// int action_open_recent_connections_title_index = popup->indexOf(d->action_open_recent_connections_title_id);
// for (int i=action_open_recent_projects_title_index+1;
//  i<action_open_recent_connections_title_index; i++)
// {
//  popup->removeItemAt(action_open_recent_projects_title_index+1);
// }

// int cur_idx = action_open_recent_projects_title_index+1;
    popup->addAction(KIcon("kexiproject_sqlite"), "My project 1");
    popup->addAction(KIcon("kexiproject_sqlite"), "My project 2");
    popup->addAction(KIcon("kexiproject_sqlite"), "My project 3");

#if 0
    //show recent connections
    d->action_open_recent_connections_title_id =
        d->action_open_recent->popupMenu()->insertTitle(i18n("Recently Connected Database Servers"));

// cur_idx = popup->indexOf(d->action_open_recent_connections_title_id) + 1;
// for (int i=cur_idx; i<count; i++) {
//  popup->removeItemAt(cur_idx);
// }
    popup->insertItem(KIcon("socket"), "My connection 1");
    popup->insertItem(KIcon("socket"), "My connection 2");
    popup->insertItem(KIcon("socket"), "My connection 3");
    popup->insertItem(KIcon("socket"), "My connection 4");
#endif
}

void
KexiMainWindow::slotProjectOpenRecent(int id)
{
    if (id < 0) // || id==d->action_open_recent_more_id)
        return;
    kDebug() << "KexiMainWindow::slotProjectOpenRecent(" << id << ")";
}

void
KexiMainWindow::slotProjectOpenRecentMore()
{
    KEXI_UNFINISHED(i18n("Open Recent"));
}

void
KexiMainWindow::slotProjectSave()
{
    if (!currentWindow())
        return;
    saveObject(currentWindow());
    updateAppCaption();
    invalidateActions();
}

void
KexiMainWindow::slotProjectSaveAs()
{
    KEXI_UNFINISHED(i18n("Save object as"));
}

void
KexiMainWindow::slotProjectPrint()
{
    if (currentWindow() && currentWindow()->partItem())
        printItem(currentWindow()->partItem());
}

void
KexiMainWindow::slotProjectPrintPreview()
{
    if (currentWindow() && currentWindow()->partItem())
        printPreviewForItem(currentWindow()->partItem());
}

void
KexiMainWindow::slotProjectPageSetup()
{
    if (currentWindow() && currentWindow()->partItem())
        showPageSetupForItem(currentWindow()->partItem());
}

void KexiMainWindow::slotProjectExportDataTable()
{
    if (currentWindow() && currentWindow()->partItem())
        exportItemAsDataTable(currentWindow()->partItem());
}

void
KexiMainWindow::slotProjectProperties()
{
    //TODO: load the implementation not the ui :)
// ProjectSettingsUI u(this);
// u.exec();
}

void
KexiMainWindow::slotProjectClose()
{
    closeProject();
}

void KexiMainWindow::slotProjectRelations()
{
    if (!d->prj)
        return;
    KexiWindow *w = KexiInternalPart::createKexiWindowInstance("relation", this);
    activateWindow(*w);
}

void KexiMainWindow::slotImportFile()
{
    KEXI_UNFINISHED("Import: " + i18n("From File..."));
}

void KexiMainWindow::slotImportServer()
{
    KEXI_UNFINISHED("Import: " + i18n("From Server..."));
}

void
KexiMainWindow::slotProjectQuit()
{
    if (~ closeProject())
        return;
    close();
}

void KexiMainWindow::slotViewNavigator()
{
    if (!d->nav
#ifdef __GNUC__
#warning TODO || !d->navToolWindow
#else
#pragma WARNING( TODO || !d->navToolWindow )
#endif
       )
        return;
#ifdef __GNUC__
#warning TODO  if (!d->nav->isVisible())
#else
#pragma WARNING( TODO  if (!d->nav->isVisible()) )
#endif
#ifdef __GNUC__
#warning TODO   makeWidgetDockVisible(d->nav);
#else
#pragma WARNING( TODO   makeWidgetDockVisible(d->nav); )
#endif
//  makeDockVisible(dynamic_cast<KDockWidget*>(d->navToolWindow->wrapperWidget()));
//  d->navToolWindow->wrapperWidget()->show();
//  d->navToolWindow->show(KDockWidget::DockLeft, getMainDockWidget());

#ifdef __GNUC__
#warning TODO d->navToolWindow->wrapperWidget()->raise();
#else
#pragma WARNING( TODO d->navToolWindow->wrapperWidget()->raise(); )
#endif
//
//2.0: unused d->block_KMdiMainFrm_eventFilter=true;
    d->nav->setFocus();
//2.0: unused d->block_KMdiMainFrm_eventFilter=false;
}

void KexiMainWindow::slotViewMainArea()
{
    if (currentWindow())
        currentWindow()->setFocus();
}

void KexiMainWindow::slotViewPropertyEditor()
{
    if (!d->propEditor
#ifdef __GNUC__
#warning TODO || !d->propEditorToolWindow
#else
#pragma WARNING( TODO || !d->propEditorToolWindow )
#endif
       )
        return;

//js  d->config->setGroup("MainWindow");
//js  ds->setSeparatorPos(d->config->readEntry("RightDockPosition", 80/* % */), true);

#ifdef __GNUC__
#warning TODO if (!d->propEditorTabWidget->isVisible())
#else
#pragma WARNING( TODO if (!d->propEditorTabWidget->isVisible()) )
#endif
#ifdef __GNUC__
#warning TODO  makeWidgetDockVisible(d->propEditorTabWidget);
#else
#pragma WARNING( TODO  makeWidgetDockVisible(d->propEditorTabWidget); )
#endif


#ifdef __GNUC__
#warning TODO d->propEditorToolWindow->wrapperWidget()->raise();
#else
#pragma WARNING( TODO d->propEditorToolWindow->wrapperWidget()->raise(); )
#endif

//2.0: unused d->block_KMdiMainFrm_eventFilter=true;
    if (d->propEditorTabWidget->currentWidget())
        d->propEditorTabWidget->currentWidget()->setFocus();
//2.0: unused d->block_KMdiMainFrm_eventFilter=false;

    /*#if defined(KDOCKWIDGET_P)
        KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
        KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
        ds->setSeparatorPos(80,true);//d->config->readEntry("RightDockPosition", 80), true);
    #endif*/
}

tristate KexiMainWindow::switchToViewMode(KexiWindow& window, Kexi::ViewMode viewMode)
{
    const Kexi::ViewMode prevViewMode = currentWindow()->currentViewMode();
    if (prevViewMode == viewMode)
        return true;
    if (!activateWindow(window))
        return false;
    if (!currentWindow()) {
        /* UNUSED, see KexiToggleViewModeAction
            d->toggleLastCheckedMode();*/
        return false;
    }
    if (&window != currentWindow())
        return false;
    if (!currentWindow()->supportsViewMode(viewMode)) {
        showErrorMessage(i18n("Selected view is not supported for \"%1\" object.",
                              currentWindow()->partItem()->name()),
                         i18n("Selected view (%1) is not supported by this object type (%2).",
                              Kexi::nameForViewMode(viewMode),
                              currentWindow()->part()->instanceCaption()));
        /* UNUSED, see KexiToggleViewModeAction
            d->toggleLastCheckedMode();*/
        return false;
    }
    updateCustomPropertyPanelTabs(currentWindow()->part(), prevViewMode,
                                  currentWindow()->part(), viewMode);
    tristate res = currentWindow()->switchToViewMode(viewMode);
    if (!res) {
        updateCustomPropertyPanelTabs(0, Kexi::NoViewMode); //revert
        showErrorMessage(i18n("Switching to other view failed (%1).",
                              Kexi::nameForViewMode(viewMode)), currentWindow());
        /* UNUSED, see KexiToggleViewModeAction
            d->toggleLastCheckedMode();*/
        return false;
    }
    if (~res) {
        updateCustomPropertyPanelTabs(0, Kexi::NoViewMode); //revert
        /* UNUSED, see KexiToggleViewModeAction
            d->toggleLastCheckedMode();*/
        return cancelled;
    }

    //view changed: switch to this view's gui client
    KXMLGUIClient *viewClient = currentWindow()->guiClient();
    updateWindowViewGUIClient(viewClient);
    if (d->curWindowViewGUIClient && !viewClient)
        guiFactory()->removeClient(d->curWindowViewGUIClient);
    d->curWindowViewGUIClient = viewClient; //remember

    invalidateSharedActions();
    invalidateProjectWideActions();
    d->updateFindDialogContents();
    d->updatePropEditorVisibility(viewMode);
    return true;
}

void KexiMainWindow::slotViewDataMode()
{
    if (currentWindow())
        switchToViewMode(*currentWindow(), Kexi::DataViewMode);
}

void KexiMainWindow::slotViewDesignMode()
{
    if (currentWindow())
        switchToViewMode(*currentWindow(), Kexi::DesignViewMode);
}

void KexiMainWindow::slotViewTextMode()
{
    if (currentWindow())
        switchToViewMode(*currentWindow(), Kexi::TextViewMode);
}

#if 0 //TODO
void KexiMainWindow::closeWindow(KMdiChildView *pWnd, bool layoutTaskBar)
{
    if (d->insideCloseWindow && dynamic_cast<KexiWindow *>(pWnd)) {
        d->windowsToClose.append(dynamic_cast<KexiWindow *>(pWnd));
        return;
    }
    /*moved to closeWindow()
    if (pWnd == currentWindow() && !pWnd->isAttached()) {
      if (d->propEditor) {
        // ah, closing detached window - better switch off property buffer right now...
        d->propBuffer = 0;
        d->propEditor->editor()->setBuffer( 0, false );
      }
    }
    */
    closeWindow(dynamic_cast<KexiWindow *>(pWnd), layoutTaskBar);
}
#endif

tristate KexiMainWindow::getNewObjectInfo(
    KexiPart::Item *partItem, KexiPart::Part *part,
    bool& allowOverwriting, const QString& messageWhenAskingForName)
{
    //data was never saved in the past -we need to create a new object at the backend
    KexiPart::Info *info = part->info();
#ifdef KEXI_ADD_CUSTOM_OBJECT_CREATION
# include "KexiMainWindow_customobjcreation.h"
#endif
    if (!d->nameDialog) {
        d->nameDialog = new KexiNameDialog(
            messageWhenAskingForName, this);
        //check if that name is allowed
        d->nameDialog->widget()->addNameSubvalidator(
            new KexiDB::ObjectNameValidator(project()->dbConnection()->driver()));
    } else {
        d->nameDialog->widget()->setMessageText(messageWhenAskingForName);
    }
    d->nameDialog->widget()->setCaptionText(partItem->caption());
    d->nameDialog->widget()->setNameText(partItem->name());
    d->nameDialog->setWindowTitle(i18n("Save Object As"));
    d->nameDialog->setDialogIcon(DesktopIcon(info->itemIcon(), KIconLoader::SizeMedium));
    allowOverwriting = false;
    bool found;
    do {
        if (d->nameDialog->exec() != QDialog::Accepted)
            return cancelled;
        //check if that name already exists
        KexiDB::SchemaData tmp_sdata;
        tristate result = project()->dbConnection()->loadObjectSchemaData(
                              project()->idForClass(info->partClass()),
                              d->nameDialog->widget()->nameText(), tmp_sdata);
        if (!result)
            return false;
        found = result == true;
        if (found) {
            if (allowOverwriting) {
                int res = KMessageBox::warningYesNoCancel(this,
                          "<p>"
                          + part->i18nMessage("Object \"%1\" already exists.", 0)
                          .subs(d->nameDialog->widget()->nameText()).toString()
                          + "</p><p>" + i18n("Do you want to replace it?") + "</p>",
                          QString(),
                          KGuiItem(i18n("&Replace"), "button_yes"),
                          KGuiItem(i18n("&Choose Other Name...")),
                          KStandardGuiItem::cancel(),
                          QString(),
                          KMessageBox::Notify | KMessageBox::Dangerous);
                if (res == KMessageBox::No)
                    continue;
                else if (res == KMessageBox::Cancel)
                    return cancelled;
                else {//yes
                    allowOverwriting = true;
                    break;
                }
            } else {
                KMessageBox::information(this,
                                         "<p>"
                                         + part->i18nMessage("Object \"%1\" already exists.", 0)
                                         .subs(d->nameDialog->widget()->nameText()).toString()
                                         + "</p><p>" + i18n("Please choose other name.") + "</p>");
//    " For example: Table \"my_table\" already exists" ,
//    "%1 \"%2\" already exists.\nPlease choose other name.")
//    .arg(dlg->part().componentName()).arg(d->nameDialog->widget()->nameText()));
                continue;
            }
        }
    } while (found);

    //update name and caption
    partItem->setName(d->nameDialog->widget()->nameText());
    partItem->setCaption(d->nameDialog->widget()->captionText());
    return true;
}

tristate KexiMainWindow::saveObject(KexiWindow *window, const QString& messageWhenAskingForName,
                                    bool dontAsk)
{
    tristate res;
    if (!window->neverSaved()) {
        //data was saved in the past -just save again
        res = window->storeData(dontAsk);
        if (!res)
            showErrorMessage(i18n("Saving \"%1\" object failed.", window->partItem()->name()),
                             currentWindow());
        return res;
    }

    const int oldItemID = window->partItem()->identifier();

    bool allowOverwriting = false;
    res = getNewObjectInfo(window->partItem(), window->part(), allowOverwriting,
                           messageWhenAskingForName);
    if (res != true)
        return res;

    res = window->storeNewData();
    if (~res)
        return cancelled;
    if (!res) {
        showErrorMessage(i18n("Saving new \"%1\" object failed.", window->partItem()->name()),
                         currentWindow());
        return false;
    }

    //update navigator
//this is alreday done in KexiProject::addStoredItem(): d->nav->addItem(window->partItem());
    //item id changed to final one: update association in dialogs' dictionary
// d->dialogs.take(oldItemID);
    d->updateWindowId(window, oldItemID);
    invalidateProjectWideActions();
    return true;
}

tristate KexiMainWindow::closeWindow(KexiWindow *window)
{
    return closeWindow(window ? window : currentWindow(), true);
}

tristate KexiMainWindow::closeWindow(KexiWindow *window, bool layoutTaskBar, bool doNotSaveChanges)
{
#ifdef __GNUC__
#warning TODO KexiMainWindow::closeWindow()
#else
#pragma WARNING( TODO KexiMainWindow::closeWindow() )
#endif
    ///@note Q_UNUSED layoutTaskBar
    Q_UNUSED(layoutTaskBar);
    
    if (!window)
        return true;
    if (d->insideCloseWindow)
        return true;

#ifndef KEXI_NO_PENDING_DIALOGS
    d->addItemToPendingWindows(window->partItem(), Private::WindowClosingJob);
#endif

    d->insideCloseWindow = true;

    if (window == currentWindow() && !window->isAttached()) {
        if (d->propEditor) {
            // ah, closing detached window - better switch off property buffer right now...
            d->propertySet = 0;
            d->propEditor->editor()->changeSet(0);
        }
    }

    bool remove_on_closing = window->partItem() ? window->partItem()->neverSaved() : false;
    if (window->isDirty() && !d->forceWindowClosing && !doNotSaveChanges) {
        //more accurate tool tips and what's this
        KGuiItem saveChanges(KStandardGuiItem::save());
        saveChanges.setToolTip(i18n("Save changes"));
        saveChanges.setWhatsThis(
            i18n("Pressing this button will save all recent changes made in \"%1\" object.",
                 window->partItem()->name()));
        KGuiItem discardChanges(KStandardGuiItem::discard());
        discardChanges.setWhatsThis(
            i18n("Pressing this button will discard all recent changes made in \"%1\" object.",
                 window->partItem()->name()));

        //dialog's data is dirty:
        //--adidional message, e.g. table designer will return
        //  "Note: This table is already filled with data which will be removed."
        //  if the window is in design view mode.
        const KLocalizedString additionalMessage(
            window->part()->i18nMessage(":additional message before saving design", window));
        QString additionalMessageString;
        if (!additionalMessage.isEmpty())
            additionalMessageString = additionalMessage.toString();

        if (additionalMessageString.startsWith(":"))
            additionalMessageString.clear();
        if (!additionalMessageString.isEmpty())
            additionalMessageString = "<p>" + additionalMessageString + "</p>";

        const int questionRes = KMessageBox::warningYesNoCancel(this,
                                "<p>"
                                + window->part()->i18nMessage("Design of object \"%1\" has been modified.", window)
                                .subs(window->partItem()->name()).toString()
                                + "</p><p>" + i18n("Do you want to save changes?") + "</p>"
                                + additionalMessageString /*may be empty*/,
                                QString(),
                                saveChanges,
                                discardChanges);
        if (questionRes == KMessageBox::Cancel) {
#ifndef KEXI_NO_PENDING_DIALOGS
            d->removePendingWindow(window->id());
#endif
            d->insideCloseWindow = false;
            d->windowsToClose.clear(); //give up with 'close all'
            return cancelled;
        }
        if (questionRes == KMessageBox::Yes) {
            //save it
//   if (!window->storeData())
            tristate res = saveObject(window, QString(), true /*dontAsk*/);
            if (!res || ~res) {
//js:TODO show error info; (retry/ignore/cancel)
#ifndef KEXI_NO_PENDING_DIALOGS
                d->removePendingWindow(window->id());
#endif
                d->insideCloseWindow = false;
                d->windowsToClose.clear(); //give up with 'close all'
                return res;
            }
            remove_on_closing = false;
        }
    }

    const int window_id = window->id(); //remember now, because removeObject() can destruct partitem object

    if (remove_on_closing) {
        //we won't save this object, and it was never saved -remove it
        if (!removeObject(window->partItem(), true)) {
#ifndef KEXI_NO_PENDING_DIALOGS
            d->removePendingWindow(window->id());
#endif
            //msg?
            //TODO: ask if we'd continue and return true/false
            d->insideCloseWindow = false;
            d->windowsToClose.clear(); //give up with 'close all'
            return false;
        }
    } else {
        //not dirty now
        if (d->nav)
            d->nav->updateItemName(*window->partItem(), false);
    }

    d->removeWindow(window_id); //don't remove -KMDI will do that
    QWidget *windowContainer = window->parentWidget();
    d->mainWidget->tabWidget()->removeTab(
        d->mainWidget->tabWidget()->indexOf(windowContainer));

    //also remove from 'print setup dialogs' cache, if needed
    int printedObjectID = 0;
    if (d->pageSetupWindowItemID2dataItemID_map.contains(window_id))
        printedObjectID = d->pageSetupWindowItemID2dataItemID_map[ window_id ];
    d->pageSetupWindows.remove(printedObjectID);

    KXMLGUIClient *client = window->commonGUIClient();
    KXMLGUIClient *viewClient = window->guiClient();
    if (d->curWindowGUIClient == client) {
        d->curWindowGUIClient = 0;
    }
    if (d->curWindowViewGUIClient == viewClient) {
        d->curWindowViewGUIClient = 0;
    }
    if (client) {
        //sanity: ouch, it is not removed yet? - do it now
        if (d->closedWindowGUIClient && d->closedWindowGUIClient != client)
            guiFactory()->removeClient(d->closedWindowGUIClient);
        if (d->openedWindowsCount() == 0) {//now there is no dialogs - remove client RIGHT NOW!
            d->closedWindowGUIClient = 0;
            guiFactory()->removeClient(client);
        } else {
            //remember this - and MAYBE remove later, if needed
            d->closedWindowGUIClient = client;
        }
    }
    if (viewClient) {
        //sanity: ouch, it is not removed yet? - do it now
        if (d->closedWindowViewGUIClient && d->closedWindowViewGUIClient != viewClient)
            guiFactory()->removeClient(d->closedWindowViewGUIClient);
        if (d->openedWindowsCount() == 0) {//now there is no dialogs - remove client RIGHT NOW!
            d->closedWindowViewGUIClient = 0;
            guiFactory()->removeClient(viewClient);
        } else {
            //remember this - and MAYBE remove later, if needed
            d->closedWindowViewGUIClient = viewClient;
        }
    }

//2.0: unused const bool isInMaximizedChildFrmMode = this->isInMaximizedChildFrmMode();

#ifdef __GNUC__
#warning TODO KMdiMainFrm::closeWindow(window, layoutTaskBar);
#else
#pragma WARNING( TODO KMdiMainFrm::closeWindow(window, layoutTaskBar); )
#endif
    delete windowContainer;

    //focus navigator if nothing else available
    if (d->openedWindowsCount() == 0) {
//2.0: unused  d->maximizeFirstOpenedChildFrm = isInMaximizedChildFrmMode;
        if (d->nav)
            d->nav->setFocus();
        d->updatePropEditorVisibility(Kexi::NoViewMode);
    }

    invalidateActions();
    d->insideCloseWindow = false;
    if (!d->windowsToClose.isEmpty()) {//continue 'close all'
        KexiWindow* w = d->windowsToClose.takeAt(0);
        closeWindow(w, true);
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    d->removePendingWindow(window_id);

    //perform pending global action that was suspended:
    if (!d->pendingWindowsExist()) {
        d->executeActionWhenPendingJobsAreFinished();
    }
#endif
    return true;
}

#if 0//TODO
void KexiMainWindow::detachWindow(KMdiChildView *pWnd, bool bShow)
{
    KMdiMainFrm::detachWindow(pWnd, bShow);
    // update icon - from small to large
    pWnd->setIcon(DesktopIcon(static_cast<KexiWindow *>(pWnd)->itemIcon()));
// pWnd->setIcon( DesktopIcon( static_cast<KexiWindow *>(pWnd)->part()->info()->itemIcon() ) );
    if (dynamic_cast<KexiWindow*>(pWnd))
        dynamic_cast<KexiWindow*>(pWnd)->sendDetachedStateToCurrentView();
}

void KexiMainWindow::attachWindow(KMdiChildView *pWnd, bool /*bShow*/, bool bAutomaticResize)
{
// if (bAutomaticResize || w->size().isEmpty() || (w->size() == QSize(1,1))) {
    KMdiMainFrm::attachWindow(pWnd, true, bAutomaticResize);
    //for dialogs in normal state: decrease dialog's height if it exceeds area contents
    if (pWnd->mdiParent()->state() == KMdiChildFrm::Normal
            && pWnd->geometry().bottom() > pWnd->mdiParent()->mdiAreaContentsRect().bottom()) {
        QRect r = pWnd->geometry();
        r.setBottom(pWnd->mdiParent()->mdiAreaContentsRect().bottom() - 5);
        pWnd->setGeometry(r);
    }
    // update icon - from large to small
    pWnd->mdiParent()->setIcon(SmallIcon(static_cast<KexiWindow *>(pWnd)->itemIcon()));
    if (dynamic_cast<KexiWindow*>(pWnd))
        dynamic_cast<KexiWindow*>(pWnd)->sendAttachedStateToCurrentView();
}
#endif

QWidget* KexiMainWindow::findWindow(QWidget *w)
{
    while (w && !acceptsSharedActions(w))
        w = w->parentWidget();
    return w;
}

bool KexiMainWindow::acceptsSharedActions(QObject *w)
{
    return w->inherits("KexiWindow") || w->inherits("KexiView");
}

#if 0 // remove?
bool KexiMainWindow::eventFilter(QObject *obj, QEvent * e)
{
    //KexiVDebug << "eventFilter: " <<e->type() << " " <<obj->name();
    if (e->type() == QEvent::KeyPress) {
        KexiVDebug << "KEY EVENT " << QString::number(static_cast<QKeyEvent*>(e)->key(), 16);
        KexiVDebug;
    }
    if (e->type() == QEvent::ShortcutOverride) {
        //KexiVDebug << "AccelOverride EVENT " << static_cast<QKeyEvent*>(e)->key() << " " << static_cast<QKeyEvent*>(e)->state() == Qt::ControlModifier;

        //avoid sending CTRL+Tab key twice for tabbed/ideal mode, epecially for win32
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Tab
                && static_cast<QKeyEvent*>(e)->modifiers() == Qt::ControlModifier) {
            if (d->action_window_next->shortcut().primary() == QKeySequence(Qt::Key_Tab | Qt::CTRL) && d->action_window_next->shortcut().primary().count() == 1
                    /*2.0: && (mdiMode()==KMdi::TabPageMode || mdiMode()==KMdi::IDEAlMode)*/) {
                static_cast<QKeyEvent*>(e)->accept();
            }
        }
    }
    if (e->type() == QEvent::Close) {
        KexiVDebug << "Close EVENT";
    }
    if (e->type() == QEvent::Resize) {
        KexiVDebug << "Resize EVENT";
    }
    if (e->type() == QEvent::WindowStateChange) {
        KexiVDebug << "WindowStateChange EVENT";
    }

    /* if (obj==d->propEditor) {
        if (e->type()==QEvent::Resize) {
          d->updatePropEditorDockWidthInfo();
        }
      }*/

    QWidget *focus_w = 0;
    if (obj->inherits("QMenu")) {
        /* Fixes for popup menus behaviour:
         For hiding/showing: focus previously (d->focus_before_popup)
         focused window, if known, otherwise focus currently focused one.
         And: just invalidate actions.
        */
        if (e->type() == QEvent::Hide || e->type() == QEvent::Show) {
            KexiVDebug << e->type();
            focus_w = focusWindow();
            if (!d->focus_before_popup.isNull()) {
                d->focus_before_popup->setFocus();
                d->focus_before_popup = 0;
                invalidateSharedActions();
            } else {
                if (focus_w) {
                    focus_w->setFocus();
                    invalidateSharedActions();
                }
            }
        }
        return false;
    }

    /*! On mouse click on the findow, make sure it's focused and actions are invalidated */
    if (e->type() == QEvent::MouseButtonPress) {
        QWidget *w = findWindow(static_cast<QWidget*>(obj));
        KexiVDebug << "MouseButtonPress EVENT " << (w ? w->objectName() : 0);
        if (w) {
            w->setFocus();
            invalidateSharedActions(currentWindow());
        }
    }
    QWidget *w = findWindow(static_cast<QWidget*>(obj));
    if (e->type() == QEvent::FocusIn) {
        focus_w = focusWindow();
        KexiVDebug << "Focus EVENT";
        KexiVDebug << (focus_w ? focus_w->objectName() : "");
        KexiVDebug << "eventFilter: " << e->type() << " " << obj->objectName();
#ifdef KEXI_STATUSBAR_DEBUG
        QWidget *focus_widget = focus_w ? focus_w->focusWidget() : 0;
        d->statusBar->setStatus(QString("FOCUS VIEW: %1 %2, FOCUS WIDGET: %3 %4")
                                .arg(focus_w ? focus_w->metaObject()->className() : "").arg(focus_w ? focus_w->objectName() : "")
                                .arg(focus_widget ? focus_widget->metaObject()->className() : "").arg(focus_widget ? focus_widget->objectName() : "")
                               );
#endif
    } else if (e->type() == QEvent::FocusOut) {
        focus_w = focusWindow();
        KexiVDebug << "Focus OUT EVENT";
        KexiVDebug << (focus_w ? focus_w->objectName() : "");
        KexiVDebug << "eventFilter: " << e->type() << " " << obj->objectName();
#ifdef KEXI_STATUSBAR_DEBUG
        QWidget *focus_widget = focus_w ? focus_w->focusWidget() : 0;
        d->statusBar->setStatus(QString("FOCUS VIEW: %1 %2, FOCUS WIDGET: %3 %4")
                                .arg(focus_w ? focus_w->metaObject()->className() : "").arg(focus_w ? focus_w->objectName() : "")
                                .arg(focus_widget ? focus_widget->metaObject()->className() : "").arg(focus_widget ? focus_widget->objectName() : "")
                               );
#endif
    }
    if (e->type() == QEvent::WindowActivate) {
        KexiVDebug << "WindowActivate EVENT";
        KexiVDebug << "eventFilter: " << e->type() << " " << obj->objectName();
    }
#if 0
    if (e->type() == QEvent::FocusIn) {
        if (focus_w) {
//   if (d->actionProxies[ w ])
//   if (d->actionProxies[ focus_w ]) {
            if (actionProxyFor(focus_w)) {
//    invalidateSharedActions();
            } else {
                /*   QObject* o = focusWidget();
                      while (o && !o->inherits("KexiWindow") && !o->inherits("KexiDockBase"))
                        o = o->parent();*/
//js    invalidateSharedActions(focus_w);
            }
        }
//  /*|| e->type()==QEvent::FocusOut*/) && /*(!obj->inherits("KexiWindow")) &&*/ d->actionProxies[ obj ]) {
//  invalidateSharedActions();
    }
    if (e->type() == QEvent::FocusOut && focus_w && focus_w == currentWindow() && actionProxyFor(obj)) {
        invalidateSharedActions(currentWindow());
    }
#endif

    if (!d->focus_before_popup.isNull() && e->type() == QEvent::FocusOut && obj->inherits("KMenuBar")) {
        //d->nav->setFocus();
        d->focus_before_popup->setFocus();
        d->focus_before_popup = 0;
        invalidateSharedActions(currentWindow());
        return true;
    }

    //remember currently focued window invalidate act.
    if (e->type() == QEvent::FocusOut) {
        if (static_cast<QFocusEvent*>(e)->reason() == Qt::PopupFocusReason) {
            if (KexiUtils::hasParent(currentWindow(), focus_w)) {
                invalidateSharedActions(currentWindow());
                d->focus_before_popup = currentWindow();
            } else {
//not needed???   invalidateSharedActions(focus_w);
                d->focus_before_popup = focus_w;
            }
        }
    }

    //keep focus in main window:
    if (w && w == d->nav) {
//  kDebug() << "NAV";
        if (e->type() == QEvent::FocusIn) {
            return true;
        } else if (e->type() == QEvent::WindowActivate && w == d->focus_before_popup) {
//   d->nav->setFocus();
            d->focus_before_popup = 0;
            return true;
        } else if (e->type() == QEvent::FocusOut) {
            if (static_cast<QFocusEvent*>(e)->reason() == Qt::TabFocusReason) {
                //activate current child:
                if (currentWindow()) {
                    currentWindow()->activate();
                    return true;
                }
            } else if (static_cast<QFocusEvent*>(e)->reason() == Qt::PopupFocusReason) {
                d->focus_before_popup = w;
            }
            //invalidateSharedActions();
        } else if (e->type() == QEvent::Hide) {
            setFocus();
            return false;
        }
    }
//2.0: unused if (d->block_KMdiMainFrm_eventFilter)//we don't want KMDI to eat our event!
//2.0: unused  return false;
    return KexiMainWindowSuper::eventFilter(obj, e);//let KMDI do its work
}
#endif

bool KexiMainWindow::openingAllowed(KexiPart::Item* item, Kexi::ViewMode viewMode, QString* errorMessage)
{
    kDebug() << viewMode;
    //! @todo this can be more complex once we deliver ACLs...
    if (!d->userMode)
        return true;
    KexiPart::Part * part = Kexi::partManager().partForClass(item->partClass());
    if (!part) {
        if (errorMessage) {
            *errorMessage = Kexi::partManager().errorMsg();
        }
    }
    kDebug() << part << item->partClass();
    if (part)
        kDebug() << item->partClass() << part->supportedUserViewModes();
    return part && (part->supportedUserViewModes() & viewMode);
}

KexiWindow *
KexiMainWindow::openObject(const QString& partClass, const QString& name,
                           Kexi::ViewMode viewMode, bool &openingCancelled, QMap<QString, QVariant>* staticObjectArgs)
{
    KexiPart::Item *item = d->prj->itemForClass(partClass, name);
    if (!item)
        return 0;
    return openObject(item, viewMode, openingCancelled, staticObjectArgs);
}

KexiWindow *
KexiMainWindow::openObject(KexiPart::Item* item, Kexi::ViewMode viewMode, bool &openingCancelled,
                           QMap<QString, QVariant>* staticObjectArgs, QString* errorMessage)
{
    if (!openingAllowed(item, viewMode, errorMessage)) {
        if (errorMessage)
            *errorMessage = i18nc(
                                "opening is not allowed in \"data view/design view/text view\" mode",
                                "opening is not allowed in \"%1\" mode", Kexi::nameForViewMode(viewMode));
        openingCancelled = true;
        return 0;
    }
    kDebug() << d->prj << item;

    if (!d->prj || !item)
        return 0;
    KexiUtils::WaitCursor wait;
#ifndef KEXI_NO_PENDING_DIALOGS
    Private::PendingJobType pendingType;
    KexiWindow *window = d->openedWindowFor(item, pendingType);
    if (pendingType != Private::NoJob) {
        openingCancelled = true;
        return 0;
    }
#else
    KexiWindow *window = d->openedWindowFor(item);
#endif
    openingCancelled = false;

    bool needsUpdateViewGUIClient = true;
    bool alreadyOpened = false;
    KexiWindowContainer *windowContainer = 0;
    if (window) {
        //window->activate();
        if (viewMode != window->currentViewMode()) {
            if (true != switchToViewMode(*window, viewMode))
                return 0;
        } else
            activateWindow(*window);
        needsUpdateViewGUIClient = false;
        alreadyOpened = true;
    } else {
        KexiPart::Part *part = Kexi::partManager().partForClass(item->partClass());
        d->updatePropEditorVisibility(viewMode, part ? part->info() : 0);
        //update tabs before opening
        updateCustomPropertyPanelTabs(currentWindow() ? currentWindow()->part() : 0,
                                      currentWindow() ? currentWindow()->currentViewMode() : Kexi::NoViewMode,
                                      part, viewMode);

        // open new tab earlier
        windowContainer = new KexiWindowContainer(d->mainWidget->tabWidget());
        d->mainWidget->tabWidget()->addTab(windowContainer,
                                           KIcon(part ? part->info()->itemIcon() : QString()), KexiPart::fullCaptionForItem(*item, part));
        d->mainWidget->tabWidget()->setCurrentWidget(windowContainer);

#ifndef KEXI_NO_PENDING_DIALOGS
        d->addItemToPendingWindows(item, Private::WindowOpeningJob);
#endif
//  window = d->prj->openObject(d->mainWidget->tabWidget(), *item, viewMode, staticObjectArgs);
        window = d->prj->openObject(windowContainer, *item, viewMode, staticObjectArgs);
        if (window) {
            windowContainer->setWindow(window);
            // update text and icon
            d->mainWidget->tabWidget()->setTabText(
                d->mainWidget->tabWidget()->indexOf(windowContainer),
                window->windowTitle());
            d->mainWidget->tabWidget()->setTabIcon(
                d->mainWidget->tabWidget()->indexOf(windowContainer),
                window->windowIcon());
        }
    }

    if (!window || !activateWindow(*window)) {
#ifndef KEXI_NO_PENDING_DIALOGS
        d->removePendingWindow(item->identifier());
#endif
        d->mainWidget->tabWidget()->removeTab(
            d->mainWidget->tabWidget()->indexOf(windowContainer));
        delete windowContainer;
        updateCustomPropertyPanelTabs(0, Kexi::NoViewMode); //revert
        //js TODO: add error msg...
        return 0;
    }

    if (needsUpdateViewGUIClient /*&& !d->userMode*/) {
        //view changed: switch to this view's gui client
        KXMLGUIClient *viewClient = window->guiClient();
        updateWindowViewGUIClient(viewClient);
        if (d->curWindowViewGUIClient && !viewClient)
            guiFactory()->removeClient(d->curWindowViewGUIClient);
        d->curWindowViewGUIClient = viewClient; //remember
    }

//UNUSED invalidateViewModeActions();
    if (viewMode != window->currentViewMode())
        invalidateSharedActions();

#ifndef KEXI_NO_PENDING_DIALOGS
    d->removePendingWindow(window->id());

    //perform pending global action that was suspended:
    if (!d->pendingWindowsExist()) {
        d->executeActionWhenPendingJobsAreFinished();
    }
#endif
    if (window && !alreadyOpened) {
//  window->setParent(d->tabWidget);
//  KexiWindow* previousWindow = currentWindow();
//moved  d->mainWidget->tabWidget()->addTab(window, window->windowIcon(), window->windowTitle());
//moved  d->mainWidget->tabWidget()->setCurrentWidget(window);
        // Call switchToViewMode() and propertySetSwitched() again here because
        // this is the time when then new window is the current one - previous call did nothing.
        switchToViewMode(*window, window->currentViewMode());
        currentWindow()->selectedView()->propertySetSwitched();
//  activeWindowChanged(window, previousWindow);
    }
    return window;
}

KexiWindow *
KexiMainWindow::openObjectFromNavigator(KexiPart::Item* item, Kexi::ViewMode viewMode)
{
    bool openingCancelled;
    return openObjectFromNavigator(item, viewMode, openingCancelled);
}

KexiWindow *
KexiMainWindow::openObjectFromNavigator(KexiPart::Item* item, Kexi::ViewMode viewMode,
                                        bool &openingCancelled)
{
    if (!openingAllowed(item, viewMode)) {
        openingCancelled = true;
        return 0;
    }
    if (!d->prj || !item)
        return false;
#ifndef KEXI_NO_PENDING_DIALOGS
    Private::PendingJobType pendingType;
    KexiWindow *window = d->openedWindowFor(item, pendingType);
    if (pendingType != Private::NoJob) {
        openingCancelled = true;
        return 0;
    }
#else
    KexiWindow *window = d->openedWindowFor(item);
#endif
    openingCancelled = false;
    if (window) {
        if (activateWindow(*window)) {//item->identifier())) {//just activate
//UNUSED   invalidateViewModeActions();
            return window;
        }
    }
    //if DataViewMode is not supported, try Design, then Text mode (currently useful for script part)
    KexiPart::Part *part = Kexi::partManager().partForClass(item->partClass());
    if (!part)
        return 0;
    if (viewMode == Kexi::DataViewMode && !(part->supportedViewModes() & Kexi::DataViewMode)) {
        if (part->supportedViewModes() & Kexi::DesignViewMode)
            return openObjectFromNavigator(item, Kexi::DesignViewMode, openingCancelled);
        else if (part->supportedViewModes() & Kexi::TextViewMode)
            return openObjectFromNavigator(item, Kexi::TextViewMode, openingCancelled);
    }
    //do the same as in openObject()
    return openObject(item, viewMode, openingCancelled);
}

tristate KexiMainWindow::closeObject(KexiPart::Item* item)
{
#ifndef KEXI_NO_PENDING_DIALOGS
    Private::PendingJobType pendingType;
    KexiWindow *window = d->openedWindowFor(item, pendingType);
    if (pendingType == Private::WindowClosingJob)
        return true;
    else if (pendingType == Private::WindowOpeningJob)
        return cancelled;
#else
    KexiWindow *window = d->openedWindowFor(item);
#endif
    if (!window)
        return cancelled;
    return closeWindow(window);
}

bool KexiMainWindow::newObject(KexiPart::Info *info, bool& openingCancelled)
{
    if (d->userMode) {
        openingCancelled = true;
        return false;
    }
    openingCancelled = false;
    if (!d->prj || !info)
        return false;
    KexiPart::Part *part = Kexi::partManager().partForClass(info->partClass());
    if (!part)
        return false;

#ifdef KEXI_ADD_CUSTOM_OBJECT_CREATION
# include "KexiMainWindow_customobjcreation.h"
#endif

    KexiPart::Item *it = d->prj->createPartItem(info);
    if (!it) {
        //js: todo: err
        return false;
    }

    if (!it->neverSaved()) //only add stored objects to the browser
        d->nav->addItem(*it);
    return openObject(it, Kexi::DesignViewMode, openingCancelled);
}

tristate KexiMainWindow::removeObject(KexiPart::Item *item, bool dontAsk)
{
    if (d->userMode)
        return cancelled;
    if (!d->prj || !item)
        return false;

    KexiPart::Part *part = Kexi::partManager().partForClass(item->partClass());
    if (!part)
        return false;

    if (!dontAsk) {
        if (KMessageBox::No == KMessageBox::warningYesNo(this,
                "<p>" + i18n("Do you want to permanently delete:\n"
                             "%1\n"
                             "If you click \"Delete\", you will not be able to undo the deletion.",
                             "</p><p>" + part->instanceCaption() + " \"" + item->name() + "\"?</p>"),
                0, KGuiItem(i18n("Delete"), "edit-delete"), KStandardGuiItem::no())) {
            return cancelled;
        }
    }

    //also close 'print setup' dialog for this item, if any
    tristate res = true;
// int printedObjectID = 0;
// if (d->pageSetupWindowItemID2dataItemID_map.contains(item->identifier()))
//  printedObjectID = d->pageSetupWindowItemID2dataItemID_map[ item->identifier() ];
    KexiWindow * pageSetupWindow = d->pageSetupWindows[ item->identifier()];
    const bool oldInsideCloseWindow = d->insideCloseWindow;
    {
        d->insideCloseWindow = false;
        if (pageSetupWindow)
            res = closeWindow(pageSetupWindow);
    }
    d->insideCloseWindow = oldInsideCloseWindow;
    if (!res || ~res) {
        return res;
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    Private::PendingJobType pendingType;
    KexiWindow *window = d->openedWindowFor(item, pendingType);
    if (pendingType != Private::NoJob) {
        return cancelled;
    }
#else
    KexiWindow *window = d->openedWindowFor(item);
#endif

    if (window) {//close existing window
        const bool tmp = d->forceWindowClosing;
        /*const bool remove_on_closing = */window->partItem()->neverSaved();
        d->forceWindowClosing = true;
        res = closeWindow(window);
        d->forceWindowClosing = tmp; //restore
        if (!res || ~res) {
            return res;
        }
    }

    //in case the dialog is a 'print setup' dialog, also update d->pageSetupWindows
    int dataItemID = d->pageSetupWindowItemID2dataItemID_map[item->identifier()];
    d->pageSetupWindowItemID2dataItemID_map.remove(item->identifier());
    d->pageSetupWindows.remove(dataItemID);

    if (!d->prj->removeObject(*item)) {
        //TODO(js) better msg
        showSorryMessage(i18n("Could not remove object."));
        return false;
    }
    return true;
}

void KexiMainWindow::renameObject(KexiPart::Item *item, const QString& _newName, bool &success)
{
    if (d->userMode) {
        success = false;
        return;
    }
    QString newName = _newName.trimmed();
    if (newName.isEmpty()) {
        showSorryMessage(i18n("Could not set empty name for this object."));
        success = false;
        return;
    }
    enableMessages(false); //to avoid double messages
    const bool res = d->prj->renameObject(*item, newName);
    enableMessages(true);
    if (!res) {
        showErrorMessage(d->prj, i18n("Renaming object \"%1\" failed.", newName));
        success = false;
        return;
    }
}

void KexiMainWindow::slotObjectRenamed(const KexiPart::Item &item, const QString& /*oldName*/)
{
#ifndef KEXI_NO_PENDING_DIALOGS
    Private::PendingJobType pendingType;
    KexiWindow *window = d->openedWindowFor(&item, pendingType);
    if (pendingType != Private::NoJob)
        return;
#else
    KexiWindow *window = d->openedWindowFor(&item);
#endif
    if (!window)
        return;

    //change item
    window->updateCaption();
    if (static_cast<KexiWindow*>(currentWindow()) == window)//optionally, update app. caption
        updateAppCaption();
}

void KexiMainWindow::acceptPropertySetEditing()
{
    if (d->propEditor)
        d->propEditor->editor()->acceptInput();
}

void KexiMainWindow::propertySetSwitched(KexiWindow *window, bool force,
        bool preservePrevSelection, bool sortedProperties, const QByteArray& propertyToSelect)
{
    KexiWindow* _currentWindow = currentWindow();
    kDebug() << "currentWindow(): "
    << (_currentWindow ? _currentWindow->windowTitle() : QString("NULL"))
    << " window: " << (window ? window->windowTitle() : QString("NULL"));
    if (_currentWindow && _currentWindow != window) {
        d->propertySet = 0; //we'll need to move to another prop. set
        return;
    }
    if (d->propEditor) {
        KoProperty::Set *newSet = _currentWindow ? _currentWindow->propertySet() : 0;
        if (!newSet || (force || static_cast<KoProperty::Set*>(d->propertySet) != newSet)) {
            d->propertySet = newSet;
            if (preservePrevSelection || force) {
                KoProperty::EditorView::SetOptions options = KoProperty::EditorView::ExpandChildItems;
                if (preservePrevSelection) {
                    options |= KoProperty::EditorView::PreservePreviousSelection;
                }
                if (sortedProperties) {
                    options |= KoProperty::EditorView::AlphabeticalOrder;
                }

                if (propertyToSelect.isEmpty()) {
                    d->propEditor->editor()->changeSet(d->propertySet, options);
                }
                else {
                    d->propEditor->editor()->changeSet(d->propertySet, propertyToSelect, options);
                }
            }
        }
/*moved to d->updatePropEditorVisibility()
        const bool inDesignMode = _currentWindow && _currentWindow->currentViewMode() == Kexi::DesignViewMode;
        if (   (newSet && inDesignMode)
            || (!newSet && inDesignMode && _currentWindow->part()->info()->isPropertyEditorAlwaysVisibleInDesignMode()))
        {
            d->propEditorDockWidget->setVisible(true);
        }
        else if (!inDesignMode) {
            d->propEditorDockWidget->setVisible(false);
        }*/
    }
}

void KexiMainWindow::slotDirtyFlagChanged(KexiWindow* window)
{
    KexiPart::Item *item = window->partItem();
    //update text in navigator and app. caption
    if (!d->userMode)
        d->nav->updateItemName(*item, window->isDirty());

    invalidateActions();
    updateAppCaption();
    d->mainWidget->tabWidget()->setTabText(
        d->mainWidget->tabWidget()->indexOf(window->parentWidget()),
        window->windowTitle());
}

#if 0//TODO?
void KexiMainWindow::slotMdiModeHasBeenChangedTo(KMdi::MdiMode)
{
    //after switching to other MDI mode, pointer to current dialog needs to be updated
    activateFirstWin();
    activeWindowChanged(activeWindow());
}
#endif

void KexiMainWindow::slotTipOfTheDay()
{
    //todo
}

void KexiMainWindow::slotImportantInfo()
{
    importantInfo(false);
}

void KexiMainWindow::slotStartFeedbackAgent()
{
#ifndef KEXI_NO_FEEDBACK_AGENT
#ifdef FEEDBACK_CLASS
    const KAboutData* about = KApplication::kApplication()->aboutData();
    FEEDBACK_CLASS* wizard = new FEEDBACK_CLASS(about->programName(),
            about->version(), 0, 0, 0, FEEDBACK_CLASS::AllPages);

    if (wizard->exec()) {
        KToolInvocation::invokeMailer("kexi-reports-dummy@kexi.org",
                                      QString(), QString(),
                                      about->appName() + QString::fromLatin1(" [feedback]"),
                                      wizard->feedbackDocument().toString(2).local8Bit());
    }

    delete wizard;
#endif
#endif
}

void KexiMainWindow::importantInfo(bool /*onStartup*/)
{
#if 0
    if (onStartup && !d->showImportantInfoOnStartup)
        return;

    QString key = QString("showImportantInfo %1").arg(KEXI_VERSION_STRING);
    d->config->setGroup("Startup");
    bool show = d->config->readBoolEntry(key, true);

    if (show || !onStartup) { //if !onStartup - dialog is always shown
        d->config->setGroup("TipOfDay");
        if (!d->config->hasKey("RunOnStart"))
            d->config->writeEntry("RunOnStart", true);

        QString lang = KGlobal::locale()->language();
        QString fname = locate("data", QString("kexi/readme_") + lang);
        if (fname.isEmpty())//back to default
            fname = locate("data", "kexi/readme_en");
        KTipDialog tipDialog(new KTipDatabase(QString()), 0);
        tipDialog.setWindowTitle(i18n("Important Information"));
        QObjectList *l = tipDialog.queryList("KPushButton");  //hack: hide <- -> buttons
        int i = 0;
        for (QObjectListIt it(*l); it.current() && i < 2; ++it, i++)
            static_cast<KPushButton*>(it.current())->hide();
        QFile f(fname);
        if (f.open(QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setCodec(KGlobal::locale()->codecForEncoding());
            QTextBrowser *tb = KexiUtils::findFirstChild<KTextBrowser>(&tipDialog, "KTextBrowser");
            if (tb) {
                tb->setText(QString("<qt>%1</qt>").arg(ts.read()));
            }
            f.close();
        }

        tipDialog.adjustSize();
        QRect desk = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(this));
        tipDialog.resize(qMax(tipDialog.width(), desk.width()*3 / 5), qMax(tipDialog.height(), desk.height()*3 / 5));
        KDialog::centerOnScreen(&tipDialog);
        tipDialog.setModal(true);
        tipDialog.exec();
        //a hack: get user's settings
        d->config->setGroup("TipOfDay");
        show = d->config->readBoolEntry("RunOnStart", show);
    }

    //write our settings back
    d->config->setGroup("Startup");
    d->config->writeEntry(key, show);
    d->showImportantInfoOnStartup = false;
#endif
}

bool KexiMainWindow::userMode() const
{
    return d->userMode;
}

bool
KexiMainWindow::setupUserMode(KexiProjectData *projectData)
{
// Kexi::tempShowMacros() = true;
// Kexi::tempShowScripts() = true;
    if (!projectData)
        return false;

    createKexiProject(projectData); //initialize project
// d->prj->setFinal(true);         //announce that we are in fianl mode

    tristate res = d->prj->open();             //try to open database
    if (!res || ~res) {
        delete d->prj;
        d->prj = 0;
        return false;
    }

#if 0 //todo reenable; autoopen objects are handled elsewhere
    KexiDB::TableSchema *sch = d->prj->dbConnection()->tableSchema("kexi__final");
    QString err_msg = i18n("Could not start project \"%1\" in Final Mode.",
                           static_cast<KexiDB::SchemaData*>(projectData)->name());
    if (!sch) {
        hide();
        showErrorMessage(err_msg, i18n("No Final Mode data found."));
        return false;
    }

    KexiDB::Cursor *c = d->prj->dbConnection()->executeQuery(*sch);
    if (!c) {
        hide();
        showErrorMessage(err_msg, i18n("Error reading Final Mode data."));
        return false;
    }

    QString startupPart;
    QString startupItem;
    while (c->moveNext()) {
        kDebug() << "KexiMainWinImpl::setupUserMode(): property: [" << c->value(1).toString() << "] " << c->value(2).toString();
        if (c->value(1).toString() == "startup-part")
            startupPart = c->value(2).toString();
        else if (c->value(1).toString() == "startup-item")
            startupItem = c->value(2).toString();
        else if (c->value(1).toString() == "mainxmlui")
            setXML(c->value(2).toString());
    }
    d->prj->dbConnection()->deleteCursor(c);

    kDebug() << "KexiMainWinImpl::setupUserMode(): part: " << startupPart;
    kDebug() << "KexiMainWinImpl::setupUserMode(): item: " << startupItem;

    setupActions();
    setupUserActions();
    guiFactory()->addClient(this);
    setStandardToolBarMenuEnabled(false);
    setHelpMenuEnabled(false);

    KexiPart::Info *i = Kexi::partManager().infoForClass(startupPart);
    if (!i) {
        hide();
        showErrorMessage(err_msg, i18n("Specified plugin does not exist."));
        return false;
    }

    Kexi::partManager().part(i);
    KexiPart::Item *item = d->prj->item(i, startupItem);
    bool openingCancelled;
    if (!openObject(item, Kexi::DataViewMode, openingCancelled) && !openingCancelled) {
        hide();
        showErrorMessage(err_msg, i18n("Specified object could not be opened."));
        return false;
    }

    QWidget::setWindowTitle("MyApp");//TODO
#endif
    return true;
}

void
KexiMainWindow::setupUserActions()
{
#if 0 //unused for now
    KexiDB::Cursor *c = d->prj->dbConnection()->executeQuery("SELECT p_id, name, text, icon, method, arguments FROM kexi__useractions WHERE scope = 0");
    if (!c)
        return;

    while (c->moveNext()) {
        KexiUserAction::fromCurrentRecord(this, actionCollection(), c);
    }
    d->prj->dbConnection()->deleteCursor(c);
    /*
      KexiUserAction *a1 = new KexiUserAction(this, actionCollection(), "user_dataview", "Change to dataview", "table");
      Arguments args;
      args.append(QVariant("kexi/table"));
      args.append(QVariant("persons"));
      a1->setMethod(KexiUserAction::OpenObject, args);
    */
#endif
}

void KexiMainWindow::slotToolsProjectMigration()
{
    showProjectMigrationWizard(QString(), QString());
}

void KexiMainWindow::slotToolsImportTables()
{
    if (project()) {
        QDialog *dlg = KexiInternalPart::createModalDialogInstance("migration", "importtable", this, 0);
        if (!dlg)
            return; //error msg has been shown by KexiInternalPart
            
            const int result = dlg->exec();
        delete dlg;
        //raise();
        if (result != QDialog::Accepted)
            return;
    }
}

void KexiMainWindow::slotToolsCompactDatabase()
{
    KexiProjectData *data = 0;
    KexiDB::Driver *drv = 0;
    const bool projectWasOpened = d->prj;

    if (!d->prj) {
        KexiStartupDialog dlg(
            KexiStartupDialog::OpenExisting, 0, Kexi::connset(), Kexi::recentProjects(),
            this);

        if (dlg.exec() != QDialog::Accepted)
            return;

        if (dlg.selectedFileName().isEmpty()) {
//! @todo add support for server based if needed?
            return;
        }
        KexiDB::ConnectionData cdata;
        cdata.setFileName(dlg.selectedFileName());

        //detect driver name for the selected file
        KexiStartupData::Import detectedImportAction;
        tristate res = KexiStartupHandler::detectActionForFile(
                           detectedImportAction, cdata.driverName,
                           "" /*suggestedDriverName*/, cdata.fileName(), 0,
                           KexiStartupHandler::SkipMessages | KexiStartupHandler::ThisIsAProjectFile
                           | KexiStartupHandler::DontConvert);

        if (true == res && !detectedImportAction)
            drv = Kexi::driverManager().driver(cdata.driverName);
        if (!drv || !(drv->features() & KexiDB::Driver::CompactingDatabaseSupported)) {
            KMessageBox::information(this, "<qt>" +
                                     i18n("Compacting database file <nobr>\"%1\"</nobr> is not supported.",
                                          QDir::convertSeparators(cdata.fileName())));
            return;
        }
        data = new KexiProjectData(cdata, cdata.fileName());
    } else {
        //sanity
        if (!(d->prj && d->prj->dbConnection()
                && (d->prj->dbConnection()->driver()->features() & KexiDB::Driver::CompactingDatabaseSupported)))
            return;

        if (KMessageBox::Continue != KMessageBox::warningContinueCancel(this,
                i18n("The current project has to be closed before compacting the database. "
                     "It will be open again after compacting.\n\nDo you want to continue?")))
            return;

        data = new KexiProjectData(*d->prj->data()); // a copy
        drv = d->prj->dbConnection()->driver();
        const tristate res = closeProject();
        if (~res || !res) {
            delete data;
            return;
        }
    }

    if (!drv->adminTools().vacuum(*data->connectionData(), data->databaseName())) {
        //err msg
        showErrorMessage(&drv->adminTools());
    }

    if (data && projectWasOpened)
        openProject(*data);
    delete data;
}

tristate KexiMainWindow::showProjectMigrationWizard(
    const QString& mimeType, const QString& databaseName, const KexiDB::ConnectionData *cdata)
{
    //pass arguments
    QMap<QString, QString> args;
    args.insert("mimeType", mimeType);
    args.insert("databaseName", databaseName);
    if (cdata) { //pass ConnectionData serialized as a string...
        QString str;
        KexiUtils::serializeMap(KexiDB::toMap(*cdata), str);
        args.insert("connectionData", str);
    }

    QDialog *dlg = KexiInternalPart::createModalDialogInstance("migration", "migration", this, 0, &args);
    if (!dlg)
        return false; //error msg has been shown by KexiInternalPart

    const int result = dlg->exec();
    delete dlg;
    //raise();
    if (result != QDialog::Accepted)
        return cancelled;

    //open imported project in a new Kexi instance
    QString destinationDatabaseName(args["destinationDatabaseName"]);
    QString fileName, destinationConnectionShortcut, dbName;
    if (!destinationDatabaseName.isEmpty()) {
        if (args.contains("destinationConnectionShortcut")) {
            // server-based
            destinationConnectionShortcut = args["destinationConnectionShortcut"];
        } else {
            // file-based
            fileName = destinationDatabaseName;
            destinationDatabaseName.clear();
        }
        tristate res = openProject(fileName, destinationConnectionShortcut,
                                   destinationDatabaseName);
        raise();
        return res;
//   KexiDB::ConnectionData *connData = new KexiDB::ConnectionData();
//   KexiDB::fromMap( KexiUtils::deserializeMap( args["destinationConnectionData"] ), *connData );
//  return openProject(destinationFileName, 0);
    }
    return true;
}

tristate KexiMainWindow::executeItem(KexiPart::Item* item)
{
    KexiPart::Info *info = item ? Kexi::partManager().infoForClass(item->partClass()) : 0;
    if ((! info) || (! info->isExecuteSupported()))
        return false;
    KexiPart::Part *part = Kexi::partManager().part(info);
    if (!part)
        return false;
    return part->execute(item);
}

void KexiMainWindow::slotProjectImportDataTable()
{
//! @todo allow data appending (it is not possible now)
    if (d->userMode)
        return;
    QMap<QString, QString> args;
    args.insert("sourceType", "file");
    QDialog *dlg = KexiInternalPart::createModalDialogInstance(
                       "csv_importexport", "KexiCSVImportDialog", this, 0, &args);
    if (!dlg)
        return; //error msg has been shown by KexiInternalPart
    dlg->exec();
    delete dlg;
}

tristate KexiMainWindow::executeCustomActionForObject(KexiPart::Item* item,
        const QString& actionName)
{
    if (actionName == "exportToCSV")
        return exportItemAsDataTable(item);
    else if (actionName == "copyToClipboardAsCSV")
        return copyItemToClipboardAsDataTable(item);

    kWarning() << "KexiMainWindow::executeCustomActionForObject(): no such action: "
    << actionName;
    return false;
}

tristate KexiMainWindow::exportItemAsDataTable(KexiPart::Item* item)
{
    if (!item)
        return false;
//! @todo: check if changes to this are saved, if not: ask for saving
//! @todo: accept row changes...

    QMap<QString, QString> args;
    args.insert("destinationType", "file");
    args.insert("itemId", QString::number(item->identifier()));
    QDialog *dlg = KexiInternalPart::createModalDialogInstance(
                       "csv_importexport", "KexiCSVExportWizard", this, 0, &args);
    if (!dlg)
        return false; //error msg has been shown by KexiInternalPart
    int result = dlg->exec();
    delete dlg;
    return result == QDialog::Rejected ? cancelled : true;
}

bool KexiMainWindow::printItem(KexiPart::Item* item, const QString& titleText)
{
#ifdef __GNUC__
#warning TODO printItem(item, KexiSimplePrintingSettings::load(), titleText);
#else
#pragma WARNING( TODO printItem(item, KexiSimplePrintingSettings::load(), titleText); )
#endif
    Q_UNUSED(item)
    Q_UNUSED(titleText)
    return false;
}

tristate KexiMainWindow::printItem(KexiPart::Item* item)
{
    return printItem(item, QString());
}

bool KexiMainWindow::printPreviewForItem(KexiPart::Item* item, const QString& titleText, bool reload)
{
#ifdef __GNUC__
#warning TODO printPreviewForItem(item, KexiSimplePrintingSettings::load(), titleText, reload);
#else
#pragma WARNING( TODO printPreviewForItem(item, KexiSimplePrintingSettings::load(), titleText, reload); )
#endif
    Q_UNUSED(item)
    Q_UNUSED(titleText)
    Q_UNUSED(reload)
    return false;
}

tristate KexiMainWindow::printPreviewForItem(KexiPart::Item* item)
{
    return printPreviewForItem(item, QString(),
//! @todo store cached row data?
                               true/*reload*/);
}

tristate KexiMainWindow::showPageSetupForItem(KexiPart::Item* item)
{
    Q_UNUSED(item)
//! @todo: check if changes to this object's design are saved, if not: ask for saving
//! @todo: accept row changes...
#ifdef __GNUC__
#warning TODO printActionForItem(item, PageSetupForItem);
#else
#pragma WARNING( TODO printActionForItem(item, PageSetupForItem); )
#endif
    return false;
}

#ifdef __GNUC__
#warning TODO reenable printItem() when ported
#else
#pragma WARNING( TODO reenable printItem() when ported )
#endif
#if 0//TODO
bool KexiMainWindow::printItem(KexiPart::Item* item, const KexiSimplePrintingSettings& settings,
                               const QString& titleText)
{
//! @todo: check if changes to this object's design are saved, if not: ask for saving
//! @todo: accept row changes...
    KexiSimplePrintingCommand cmd(this, item->identifier());
    //modal
    return cmd.print(settings, titleText);
}

bool KexiMainWindow::printPreviewForItem(KexiPart::Item* item,
        const KexiSimplePrintingSettings& settings, const QString& titleText, bool reload)
{
//! @todo: check if changes to this object's design are saved, if not: ask for saving
//! @todo: accept row changes...
    KexiSimplePrintingCommand* cmd = d->openedCustomObjectsForItem<KexiSimplePrintingCommand>(
                                         item, "KexiSimplePrintingCommand");
    if (!cmd) {
        d->addOpenedCustomObjectForItem(
            item,
            cmd = new KexiSimplePrintingCommand(this, item->identifier()),
            "KexiSimplePrintingCommand"
        );
    }
    return cmd->showPrintPreview(settings, titleText, reload);
}

tristate KexiMainWindow::printActionForItem(KexiPart::Item* item, PrintActionType action)
{
    if (!item)
        return false;
    KexiPart::Info *info = Kexi::partManager().infoForClass(item->partClass());
    if (!info->isPrintingSupported())
        return false;

    KexiWindow *printingWindow = d->pageSetupWindows[ item->identifier()];
    if (printingWindow) {
        if (!activateWindow(*printingWindow))
            return false;
        if (action == PreviewItem || action == PrintItem) {
            QTimer::singleShot(0, printingWindow->selectedView(),
                               (action == PreviewItem) ? SLOT(printPreview()) : SLOT(print()));
        }
        return true;
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    Private::PendingJobType pendingType;
    KexiWindow *window = d->openedWindowFor(item, pendingType);
    if (pendingType != Private::NoJob)
        return cancelled;
#else
    KexiWindow *window = d->openedWindowFor(item);
#endif

    if (window) {
        // accept row changes
        QWidget *prevFocusWidget = focusWidget();
        window->setFocus();
        d->action_data_save_row->activate(QAction::Trigger);
        if (prevFocusWidget)
            prevFocusWidget->setFocus();

        // opened: check if changes made to this dialog are saved, if not: ask for saving
        if (window->neverSaved()) //sanity check
            return false;
        if (window->isDirty()) {
            KGuiItem saveChanges(KStandardGuiItem::save());
            saveChanges.setToolTip(i18n("Save changes"));
            saveChanges.setWhatsThis(
                i18n("Pressing this button will save all recent changes made in \"%1\" object.",
                     item->name()));
            KGuiItem doNotSave(KStandardGuiItem::no());
            doNotSave.setWhatsThis(
                i18n("Pressing this button will ignore all unsaved changes made in \"%1\" object.",
                     window->partItem()->name()));

            QString question;
            if (action == PrintItem)
                question = i18n("Do you want to save changes before printing?");
            else if (action == PreviewItem)
                question = i18n("Do you want to save changes before making print preview?");
            else if (action == PageSetupForItem)
                question = i18n("Do you want to save changes before showing page setup?");
            else
                return false;

            const int questionRes = KMessageBox::warningYesNoCancel(this,
                                    "<p>"
                                    + window->part()->i18nMessage("Design of object \"%1\" has been modified.", window)
                                    .subs(item->name())
                                    + "</p><p>" + question + "</p>",
                                    QString(),
                                    saveChanges,
                                    doNotSave);
            if (KMessageBox::Cancel == questionRes)
                return cancelled;
            if (KMessageBox::Yes == questionRes) {
                tristate savingRes = saveObject(window, QString(), true /*dontAsk*/);
                if (true != savingRes)
                    return savingRes;
            }
        }
    }
    KexiPart::Part * printingPart = Kexi::partManager().partForClass("org.kexi-project.simpleprinting");
    if (!printingPart)
        printingPart = new KexiSimplePrintingPart(); //hardcoded as there're no .desktop file
    KexiPart::Item* printingPartItem = d->prj->createPartItem(
                                           printingPart, item->name() //<-- this will look like "table1 : printing" on the window list
                                       );
    QMap<QString, QVariant> staticObjectArgs;
    staticObjectArgs["identifier"] = QString::number(item->identifier());
    if (action == PrintItem)
        staticObjectArgs["action"] = "print";
    else if (action == PreviewItem)
        staticObjectArgs["action"] = "printPreview";
    else if (action == PageSetupForItem)
        staticObjectArgs["action"] = "pageSetup";
    else
        return false;
    bool openingCancelled;
    printingWindow = openObject(printingPartItem, Kexi::DesignViewMode,
                                openingCancelled, &staticObjectArgs);
    if (openingCancelled)
        return cancelled;
    if (!printingWindow) //sanity
        return false;
    d->pageSetupWindows.insert(item->identifier(), printingWindow);
    d->pageSetupWindowItemID2dataItemID_map.insert(
        printingWindow->partItem()->identifier(), item->identifier());

    return true;
}
#endif

void KexiMainWindow::slotEditCopySpecialDataTable()
{
    KexiPart::Item* item = d->nav->selectedPartItem();
    if (item)
        copyItemToClipboardAsDataTable(item);
}

tristate KexiMainWindow::copyItemToClipboardAsDataTable(KexiPart::Item* item)
{
    if (!item)
        return false;

    QMap<QString, QString> args;
    args.insert("destinationType", "clipboard");
    args.insert("itemId", QString::number(item->identifier()));
    QDialog *dlg = KexiInternalPart::createModalDialogInstance(
                       "csv_importexport", "KexiCSVExportWizard", this, 0, &args);
    if (!dlg)
        return false; //error msg has been shown by KexiInternalPart
    const int result = dlg->exec();
    delete dlg;
    return result == QDialog::Rejected ? cancelled : true;
}

void KexiMainWindow::slotEditPasteSpecialDataTable()
{
//! @todo allow data appending (it is not possible now)
    if (d->userMode)
        return;
    QMap<QString, QString> args;
    args.insert("sourceType", "clipboard");
    QDialog *dlg = KexiInternalPart::createModalDialogInstance(
                       "csv_importexport", "KexiCSVImportDialog", this, 0, &args);
    if (!dlg)
        return; //error msg has been shown by KexiInternalPart
    dlg->exec();
    delete dlg;
}

void KexiMainWindow::slotEditFind()
{
// KexiView *view = d->currentViewSupportingAction("edit_findnext");
    KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
    if (!iface)
        return;
    d->updateFindDialogContents(true/*create if does not exist*/);
    d->findDialog()->setReplaceMode(false);

    d->findDialog()->show();
    d->findDialog()->activateWindow();
    d->findDialog()->raise();
}

void KexiMainWindow::slotEditFind(bool next)
{
    KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
    if (!iface)
        return;
    tristate res = iface->find(
                       d->findDialog()->valueToFind(), d->findDialog()->options(), next);
    if (~res)
        return;
    d->findDialog()->updateMessage(true == res);
//! @todo result
}

void KexiMainWindow::slotEditFindNext()
{
    slotEditFind(true);
}

void KexiMainWindow::slotEditFindPrevious()
{
    slotEditFind(false);
}

void KexiMainWindow::slotEditReplace()
{
    KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
    if (!iface)
        return;
    d->updateFindDialogContents(true/*create if does not exist*/);
    d->findDialog()->setReplaceMode(true);
//! @todo slotEditReplace()
    d->findDialog()->show();
    d->findDialog()->activateWindow();
}

void KexiMainWindow::slotEditReplaceNext()
{
    slotEditReplace(false);
}

void KexiMainWindow::slotEditReplace(bool all)
{
    KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
    if (!iface)
        return;
//! @todo add question: "Do you want to replace every occurrence of \"%1\" with \"%2\"?
//!       You won't be able to undo this." + "Do not ask again".
    tristate res = iface->findNextAndReplace(
                       d->findDialog()->valueToFind(), d->findDialog()->valueToReplaceWith(),
                       d->findDialog()->options(), all);
    d->findDialog()->updateMessage(true == res);
//! @todo result
}

void KexiMainWindow::slotEditReplaceAll()
{
    slotEditReplace(true);
}

#ifdef __GNUC__
#warning TODO addWindow(()
#else
#pragma WARNING( TODO addWindow(() )
#endif
#if 0//TODO
void KexiMainWindow::addWindow(KMdiChildView* pView, int flags)
{
    //maximize this window, if it's
//!@todo Certain windows' sizes, e.g. forms could have own size configation specified!
//!      Query for this, and if so: give up.
    if (d->maximizeFirstOpenedChildFrm) {
        flags |= KMdi::Maximize;
        d->maximizeFirstOpenedChildFrm = false;
    }
    KexiMainWindow::addWindow(pView, flags);
}
#endif

/// TMP (until there's true template support)
void KexiMainWindow::slotGetNewStuff()
{
#ifdef HAVE_KNEWSTUFF
    if (!d->newStuff)
        d->newStuff = new KexiNewStuff(this);
    d->newStuff->download();

    //KNS::DownloadDialog::open(newstuff->customEngine(), "kexi/template");
#endif
}

void KexiMainWindow::highlightObject(const QString& partClass, const QString& name)
{
    slotViewNavigator();
    if (!d->prj)
        return;
    KexiPart::Item *item = d->prj->itemForClass(partClass, name);
    if (!item)
        return;
    if (d->nav) {
        d->nav->selectItem(*item);
    }
}

void KexiMainWindow::slotPartItemSelectedInNavigator(KexiPart::Item* item)
{
    Q_UNUSED(item);
}

KToolBar *KexiMainWindow::toolBar(const QString& name) const
{
    return d->tabbedToolBar->toolBar(name);
}

void KexiMainWindow::appendWidgetToToolbar(const QString& name, QWidget* widget)
{
    d->tabbedToolBar->appendWidgetToToolbar(name, widget);
}

void KexiMainWindow::setWidgetVisibleInToolbar(QWidget* widget, bool visible)
{
    d->tabbedToolBar->setWidgetVisibleInToolbar(widget, visible);
}

void KexiMainWindow::addToolBarAction(const QString& toolBarName, QAction *action)
{
    d->tabbedToolBar->addAction(toolBarName, action);
}

void KexiMainWindow::updatePropertyEditorInfoLabel(const QString& textToDisplayForNullSet)
{
    d->propEditor->updateInfoLabelForPropertySet(d->propertySet, textToDisplayForNullSet);
}

#include "KexiMainWindow.moc"
