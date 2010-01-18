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

#ifndef KEXIMAINWINDOW_H
#define KEXIMAINWINDOW_H

//#define KEXI_IMPL_WARNINGS

#include <KMainWindow>
#include <core/KexiMainWindowIface.h>
#include <core/kexiguimsghandler.h>

#include <QApplication>
#include <QMainWindow>

class QPaintEvent;
class KexiProjectData;
class KexiSimplePrintingSettings;

namespace KexiDB
{
class Object;
class ConnectionData;
}
namespace KexiPart
{
class Info;
class Part;
}

class KXMLGUIClient;
class KXMLGUIFactory;
class KexiMainWidget;

#include <KTabWidget>

//! @internal tab widget acting as central widget for KexiMainWindow
class KexiMainWindowTabWidget : public KTabWidget
{
    Q_OBJECT
public:
    KexiMainWindowTabWidget(QWidget *parent, KexiMainWidget *mainWidget);
    virtual ~KexiMainWindowTabWidget();
public slots:
    void closeTab();
protected:
    //! Implemented to update main window on creation of the first tab
    virtual void tabInserted(int index);

    //! Reimplemented to hide frame when no tabs are displayed
    virtual void paintEvent(QPaintEvent * event);

    KexiMainWidget *m_mainWidget;
};

#define KexiMainWindowSuper QWidget //KMainWindow

/**
 * @short Kexi's main window implementation
 */
class KEXIMAIN_EXPORT KexiMainWindow
            : public QWidget /*KMainWindow*/, public KexiMainWindowIface, public KexiGUIMessageHandler
{
    Q_OBJECT

public:
    /*! Creates an empty mainwindow. */
    KexiMainWindow(QWidget *parent = 0);
    virtual ~KexiMainWindow();

    virtual KActionCollection* actionCollection() const;

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO virtual QWidget* focusWidget() const;
#else
#pragma WARNING( TODO virtual QWidget* focusWidget() const; )
#endif
#endif
    virtual QWidget* focusWidget() const {
        return KexiMainWindowSuper::focusWidget();
    }

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO  virtual void plugActionList(const QString& name,
#else
#pragma WARNING( TODO  virtual void plugActionList(const QString& name, )
#endif
#endif
    virtual void plugActionList(const QString& name,
                                const QList<KAction *>& actionList) {
        Q_UNUSED(name)
        Q_UNUSED(actionList)
    }

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO KXMLGUIClient* guiClient() const;
#else
#pragma WARNING( TODO KXMLGUIClient* guiClient() const; )
#endif
#endif
    virtual KXMLGUIClient* guiClient() const;

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO virtual void unplugActionList (const QString &name);
#else
#pragma WARNING( TODO virtual void unplugActionList (const QString &name); )
#endif
#endif
    virtual void unplugActionList(const QString &name) {
        Q_UNUSED(name)
    }

    //! Implemented by KMainWindow
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO virtual KXMLGUIFactory * KMainWindow::guiFactory();
#else
#pragma WARNING( TODO virtual KXMLGUIFactory * KMainWindow::guiFactory(); )
#endif
#endif
    virtual KXMLGUIFactory* guiFactory();


    /*! Used by the main kexi routine. Creates a new Kexi main window and a new KApplication object.
     kdemain() has to destroy the latter on exit.
     \return result 1 on error and 0 on success (the result can be used as a result of kdemain()) */
    static int create(int argc, char *argv[], KAboutData* aboutdata = 0);

    //! \return KexiMainWindow singleton (if it is instantiated)
//  static KexiMainWindow* self();

    //! Project data of currently opened project or NULL if no project here yet.
    virtual KexiProject *project();

    /*! Registers window \a window for watching and adds it to the main window's stack. */
    virtual void registerChild(KexiWindow *window);

    /*! Activates a window by it's document identifier.
     \return false if doc couldn't be raised or isn't opened. */
    bool activateWindow(int id);

    /*! Like above, using \a window passed explicitly. Above method just calls this one. */
    bool activateWindow(KexiWindow& window);

    /*! Performs startup actions. \return false if application should exit immediately
     with an error status. */
    tristate startup();

    /*! \return true if the application window is in the User Mode. */
    virtual bool userMode() const;

    /*! \return true if opening of item \a item in \a viewMode mode is allowed.
     userMode() is taken into account as well
     as KexiPart::Part::supportedUserViewModes() for \a  item. */
    bool openingAllowed(KexiPart::Item* item, Kexi::ViewMode viewMode);

//TODO REMOVE?  virtual bool eventFilter( QObject *obj, QEvent * e );

    //! \return popup menu for \a popupName name.
//2.0 disabled  virtual Q3PopupMenu* findPopupMenu(const char *popupName);

    /*! Implemented for KexiMainWindow. */
    virtual QList<QAction*> allActions() const;

    /*! \return currently active window od 0 if there is no active window.
     Implemented for KexiWindow. */
    virtual KexiWindow* currentWindow() const;

    /*! Reimplemented */
//  virtual void readProperties(KConfig *config);
//  virtual void saveProperties(KConfig *config);
//  virtual void saveGlobalProperties( KConfig* sessionConfig );

//! @todo temporary solution before the tabbed toolbar framework emerges
    // see KexiMainWindowIface
    virtual void appendWidgetToToolbar(const QString& name, QWidget* widget);

//! @todo temporary solution before the tabbed toolbar framework emerges
    // see KexiMainWindowIface
    virtual void setWidgetVisibleInToolbar(QWidget* widget, bool visible);

//! @todo replace with the final Actions API
    // see KexiMainWindowIface
    virtual void addToolBarAction(const QString& toolBarName, QAction *action);

public slots:
    /*! Inherited from KMdiMainFrm: we need to do some tasks before child is closed.
      Just calls closeWindow(). Use closeWindow() if you need, not this one. */
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO virtual void closeWindow(KMdiChildView *pWnd, bool layoutTaskBar = true);
#else
#pragma WARNING( TODO virtual void closeWindow(KMdiChildView *pWnd, bool layoutTaskBar = true); )
#endif
#endif

    /*! Reimplemented for internal reasons. */
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO virtual void addWindow( KMdiChildView* pView, int flags = KMdi::StandardAdd );
#else
#pragma WARNING( TODO virtual void addWindow( KMdiChildView* pView, int flags = KMdi::StandardAdd ); )
#endif
#endif
    /*! Implemented for KexiMainWindow */
    virtual tristate closeWindow(KexiWindow *window);

    /*! Closes the current window. */
    tristate closeCurrentWindow() {
        return closeWindow(0);
    }

    /*! Internal implementation. If \a doNotSaveChanges is true,
     messages asking for saving the will be skipped and the changes will be dropped.
     This should not be usually used, maybe except for test suites
     (see kexi/tests/altertable/ directory). */
    tristate closeWindow(KexiWindow *window, bool layoutTaskBar, bool doNotSaveChanges = false);

    /*! Activates next window. */
    void activateNextWindow();

    /*! Activates previous window. */
    void activatePreviousWindow();

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO   virtual void detachWindow(KMdiChildView *pWnd,bool bShow=true);
#else
#pragma WARNING( TODO   virtual void detachWindow(KMdiChildView *pWnd,bool bShow=true); )
#endif
#ifdef __GNUC__
#warning TODO   virtual void attachWindow(KMdiChildView *pWnd,bool bShow=true,bool bAutomaticResize=false);
#else
#pragma WARNING( TODO   virtual void attachWindow(KMdiChildView *pWnd,bool bShow=true,bool bAutomaticResize=false); )
#endif
#endif

//! @todo move part of this to KexiProject, because currently KexiProject::openObject() allows multiple opens!
    /*! Opens object pointed by \a item in a view \a viewMode.
     \a staticObjectArgs can be passed for static object
     (only works when part for this item is of type KexiPart::StaticPart).
     \a openingCancelled is set to true is opening has been cancelled.
     \a errorMessage, if not 0, points to a string that can be set to error message
     if one encountered. */
    virtual KexiWindow* openObject(KexiPart::Item *item, Kexi::ViewMode viewMode,
                                   bool &openingCancelled, QMap<QString, QVariant>* staticObjectArgs = 0,
                                   QString* errorMessage = 0);

    //! For convenience
    virtual KexiWindow* openObject(const QString& partClass, const QString& name,
                                   Kexi::ViewMode viewMode, bool &openingCancelled, QMap<QString, QVariant>* staticObjectArgs = 0);

    /*! Closes the object for \a item.
     \return true on success (closing can be dealyed though), false on failure and cancelled
     if the object has "opening" job assigned. */
    virtual tristate closeObject(KexiPart::Item* item);

    /*! Implemented for KexiMainWindow */
    virtual tristate saveObject(KexiWindow *window,
                                const QString& messageWhenAskingForName = QString(), bool dontAsk = false);

    /*! Implemented for KexiMainWindow */
    virtual tristate getNewObjectInfo(KexiPart::Item *partItem, KexiPart::Part *part,
                                      bool& allowOverwriting, const QString& messageWhenAskingForName = QString());

    /*! Implemented for KexiMainWindow */
    virtual void highlightObject(const QString& partClass, const QString& name);

    /*! Opens project pointed by \a projectData.
     Application state (e.g. actions) is updated.
     \a projectData is copied into a project structures.
     \return true on success */
    tristate openProject(const KexiProjectData& projectData);

    /*! Helper. Opens project pointed by \a aFileName.
     If \a aFileName is empty, a connection shortcut (.kexic file name) is obtained from
     global connection set using \a cdata (if present).
     In this case:
     * If connection shortcut has been found and \a dbName (a server database name) is provided
      'kexi --skip-dialog --connection file.kexic dbName' is executed (or the project
      is opened directly if there's no project opened in the current Kexi main window.
     * If connection shortcut has been found and \a dbName is not provided,
      'kexi --skip-dialog file.kexic' is executed (or the connection is opened
      directly if there's no porject opened in the current Kexi main window. */
    tristate openProject(const QString& aFileName, KexiDB::ConnectionData *cdata,
                         const QString& dbName = QString(),
                         const KexiProjectData::AutoOpenObjects& autoopenObjects = KexiProjectData::AutoOpenObjects());

    /*! Helper. Opens project pointed by \a aFileName.
     Like above but \a fileNameForConnectionData can be passed instead of
     a pointer to connection data itself.
     \return false if \a fileNameForConnectionData is not empty but there is no such
     connection in Kexi::connset() for this filename.
     \a fileNameForConnectionData can be empty. */
    tristate openProject(const QString& aFileName,
                         const QString& fileNameForConnectionData, const QString& dbName = QString());

    /*! Creates a new project usign template pointed by \a projectData.
     Application state (e.g. actions) is updated.
     New project data is copied into a project structures.
     \return true on success */
    tristate createProjectFromTemplate(const KexiProjectData& projectData);

    /*! Closes current project, \return true on success.
     Application state (e.g. actions) is updated.
     \return true on success.
     If closing was cancelled by user, cancelled is returned. */
    tristate closeProject();

    //! Shows "print" dialog for \a item.
    //! \return true on success.
    virtual tristate printItem(KexiPart::Item* item);

    //! Shows "print preview" window.
    //! \return true on success.
    virtual tristate printPreviewForItem(KexiPart::Item* item);

    //! Shows "page setup" window for \a item.
    //! \return true on success and cancelled when the action was cancelled.
    virtual tristate showPageSetupForItem(KexiPart::Item* item);

    /*! Executes custom action for the main window, usually provided by a plugin.
     Also used by KexiFormEventAction. */
    virtual tristate executeCustomActionForObject(KexiPart::Item* item, const QString& actionName);

signals:
    //! Emitted to make sure the project can be close.
    //! Connect a slot here and set \a cancel to true to cancel the closing.
    virtual void acceptProjectClosingRequested(bool& cancel);

    //! Emitted before closing the project (and destroying all it's data members).
    //! You can do you cleanup of your structures here.
    virtual void beforeProjectClosing();

    //! Emitted after closing the project.
    virtual void projectClosed();

    //! Emitted after opening a project, even after slotAutoOpenObjectsLater().
    void projectOpened();

protected:
    /*! Setups main widget */
    void setupMainWidget();

    /*! Setups the User Mode: constructs window according to kexi__final database
     and loads the specified part.
     \return true on success or false if e.g. kexi__final does not exist
     or a fatal exception happened */
    bool setupUserMode(KexiProjectData *projectData);

    /*! Creates the Project Navigator (if it's not yet created),
     lookups items for current project and fills the nav. with not-opened items */
    void setupProjectNavigator();

    void setupContextHelp();

    void setupPropertyEditor();

    /*! Creates standard actions like new, open, save ... */
    void setupActions();

    /*! Creates user project-wide actions */
    void setupUserActions();

    /*! Sets up the window from user settings (e.g. mdi mode). */
    void restoreSettings();

    /*! Writes user settings back. */
    void storeSettings();

    /*! Invalidates availability of all actions for current application state. */
    void invalidateActions();

    /*! Invalidates action availability for current application state.
     These actions are dependent on active window. */
    virtual void invalidateSharedActions(QObject *o);

    /*! Invalidates action availability for current application state.
     These actions only depend on project availability, not on curently active window. */
    void invalidateProjectWideActions();

    /* UNUSED, see KexiToggleViewModeAction
         Invalidates action availability for current application state.
         These actions only depend on curently active window and currently selected view
         (KexiView derived object) within this window.
        void invalidateViewModeActions();*/

    /*! Shows dialog for creating new blank project,
     and creates one. The dialog is not shown if option for automatic creation
     is checked or Kexi::startupHandler().projectData() was provided from command line.
     \a cancelled is set to true if creation has been cancelled (e.g. user answered
     no when asked for database overwriting, etc.
     \return true if database was created, false on error or when cancel was pressed */
    tristate createBlankProject();

    /*! Shows dialog for creating new blank project,
     and return a data describing it. If the dialog was cancelled,
     \a cancelled will be set to true (false otherwise).
     \a shortcutFileName, if not 0, will be set to a shortcut filename
     (in case when server database project was selected). */
    KexiProjectData* createBlankProjectData(bool &cancelled, bool confirmOverwrites = true, QString *shortcutFileName = 0);

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO  void setWindowMenu(Q3PopupMenu *menu);
#else
#pragma WARNING( TODO  void setWindowMenu(Q3PopupMenu *menu); )
#endif
#endif
    /*! \return focused kexi window (KexiWindow or KexiDockBase subclass) */
//  QWidget* focusWindow() const;

    /*! Reimplemented from KexiSharedActionHost:
     accepts only KexiDockBase and KexiWindow subclasses.  */
    virtual bool acceptsSharedActions(QObject *w);

    /*! Performs lookup like in KexiSharedActionHost::focusWindow()
     but starting from \a w instead of a widget returned by QWidget::focusWidget().
     \return NULL if no widget matches acceptsSharedActions() or if \a w is NULL. */
    QWidget* findWindow(QWidget *w);

    /*! Updates application's caption - also shows project's name. */
    void updateAppCaption();

//  void restoreWindowConfiguration(KConfig *config);
//  void storeWindowConfiguration(KConfig *config);

    virtual void closeEvent(QCloseEvent *ev);

    //! Called by KexiMainWidget::queryClose()
    bool queryClose();

    //! Called by KexiMainWidget::queryExit()
    bool queryExit();

    /*! Implemented for KexiMainWindowIface.
     Switches \a window to view \a mode. Activates the window if it is not the current window. */
    virtual tristate switchToViewMode(KexiWindow& window, Kexi::ViewMode viewMode);

    /*! Helper. Removes and/or adds GUI client for current window's view;
     on switching to other window (activeWindowChanged())
     or on switching to other view within the same window (switchToViewMode()). */
    void updateWindowViewGUIClient(KXMLGUIClient *viewClient);

    /*! Helper. Updates setup of property panel's tabs. Used when switching
     from \a prevWindow window to a current window. */
    void updateCustomPropertyPanelTabs(KexiWindow *prevWindow, Kexi::ViewMode prevViewMode);

    /*! @overload void updateCustomPropertyPanelTabs(KexiWindow *prevWindow, int prevViewMode) */
    void updateCustomPropertyPanelTabs(KexiPart::Part *prevWindowPart,
                                       Kexi::ViewMode prevViewMode, KexiPart::Part *curWindowPart, Kexi::ViewMode curViewMode);

    /*! Used in openProject when running another Kexi process is required. */
    tristate openProjectInExternalKexiInstance(const QString& aFileName, KexiDB::ConnectionData *cdata, const QString& dbName);

    /*! Used in openProject when running another Kexi process is required. */
    tristate openProjectInExternalKexiInstance(const QString& aFileName, const QString& fileNameForConnectionData, const QString& dbName);

protected slots:

    /*! Called once after timeout (after ctors are executed). */
    void slotAutoOpenObjectsLater();

    /*! Called if a window (tab) changes from \a prevWindow to \a window. Both parameters can be 0. */
    void activeWindowChanged(KexiWindow *window, KexiWindow *prevWindow);

    /*! This slot is called if a window gets colsed and will unregister stuff */
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO  void childClosed(KMdiChildView *dlg);
#else
#pragma WARNING( TODO  void childClosed(KMdiChildView *dlg); )
#endif
#endif

    void slotPartLoaded(KexiPart::Part* p);

//  void slotCaptionForCurrentMDIChild(bool childrenMaximized);
    /*
    #ifdef __GNUC__
    #warning TODO  void slotNoMaximizedChildFrmLeft(KMdiChildFrm*);
    #else
    #pragma WARNING( TODO  void slotNoMaximizedChildFrmLeft(KMdiChildFrm*); )
    #endif
        void slotLastChildViewClosed();
        void slotChildViewIsDetachedNow(QWidget*);*/

    //! internal - creates and initializes kexi project
    void createKexiProject(KexiProjectData* new_data);

    /*! Handles event when user double clicked (or single -depending on settings)
     or pressed Return key on the part item in the navigator.
     This differs from openObject() signal in that if the object is already opened
     in view mode other than \a viewMode, the mode is not changed.
     \sa KexiBrowser::openOrActivateItem() */
    KexiWindow* openObjectFromNavigator(KexiPart::Item* item,
                                        Kexi::ViewMode viewMode, bool &openingCancelled);

    //! For convenience
    KexiWindow* openObjectFromNavigator(KexiPart::Item* item, Kexi::ViewMode viewMode);

    /*! Creates new object of type defined by \a info part info.
     \a openingCancelled is set to true is opening has been cancelled.
     \return true on success. */
    virtual bool newObject(KexiPart::Info *info, bool& openingCancelled);

    //! For convenience
    bool newObject(KexiPart::Info *info) {
        bool openingCancelled;
        return newObject(info, openingCancelled);
    }

    //! For convenience
    KexiWindow* openObject(KexiPart::Item *item, Kexi::ViewMode viewMode,
                           QMap<QString, QVariant>* staticObjectArgs = 0) {
        bool openingCancelled;
        return openObject(item, viewMode, openingCancelled, staticObjectArgs);
    }

    /*! Removes object pointed by \a item from current project.
     Asks for confirmation. \return true on success
     or cancelled if removing was cancelled (only possible if \a dontAsk is false). */
    tristate removeObject(KexiPart::Item *item, bool dontAsk = false);

    /*! Renames object pointed by \a item to a new name \a _newName.
     Sets \a success to false on failure. Used as a slot connected
     to KexiBrowser::renameItem() signal. */
    void renameObject(KexiPart::Item *item, const QString& _newName, bool &succes);

    /*! Reaction for object rename (signalled by KexiProject).
     If this item has opened window, it's caption is updated,
     and also optionally application's caption. */
    virtual void slotObjectRenamed(const KexiPart::Item &item, const QString& oldName);

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO  virtual void fillWindowMenu();
#else
#pragma WARNING( TODO  virtual void fillWindowMenu(); )
#endif
#endif
    void invalidateSharedActions();
    void invalidateSharedActionsLater();

    //! Updates the statusbar, navigator and "Insert->....." actions, dependent on read-only state.
    //! Only called on project opening and closing.
    void updateReadOnlyState();

    void slotProjectNew();
    void slotProjectOpen();
    void slotProjectOpenRecentAboutToShow();
    void slotProjectOpenRecent(int id);
    void slotProjectOpenRecentMore();
    void slotProjectSave();
    void slotProjectSaveAs();
    void slotProjectPrint();
    void slotProjectPrintPreview();
    void slotProjectPageSetup();
    void slotProjectProperties();
    void slotProjectClose();
    void slotProjectRelations();
    void slotProjectImportDataTable();
    void slotProjectExportDataTable();
    void slotProjectQuit();
    void slotEditPasteSpecialDataTable();
    void slotEditCopySpecialDataTable();
    void slotEditFind();
    void slotEditFind(bool next); //!< helper
    void slotEditFindNext();
    void slotEditFindPrevious();
    void slotEditReplace(bool all); //!< helper
    void slotEditReplace();
    void slotEditReplaceNext();
    void slotEditReplaceAll();
    void slotViewNavigator();
    void slotViewMainArea();
    void slotViewPropertyEditor();
    void slotViewDataMode();
    void slotViewDesignMode();
    void slotViewTextMode(); //!< sometimes called "SQL View"
    void slotShowSettings();
    void slotConfigureKeys();
    void slotConfigureToolbars();
    void slotToolsProjectMigration();
    void slotToolsImportTables();
    void slotToolsCompactDatabase();

    /// TMP: Display a dialog to download db examples from Internet
    void slotGetNewStuff();

    void slotTipOfTheDay();

    //! Shows 'important info' dialog, is \a onStartup is false, it's always shown
    void importantInfo(bool onStartup);
    void slotImportantInfo(); //!< just importantInfo(false);
    void slotStartFeedbackAgent();
    void slotImportFile();
    void slotImportServer();

    //! There are performed all actions that need to be done immediately after  ctro (using timer)
    void slotLastActions();

    virtual void acceptPropertySetEditing();

    virtual void propertySetSwitched(KexiWindow *window, bool force = false,
                                     bool preservePrevSelection = true, 
                                     bool sortedProperties = false,
                                     const QByteArray& propertyToSelect = QByteArray());

    /*! Handles changes in 'dirty' flag for windows. */
    void slotDirtyFlagChanged(KexiWindow* window);

#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO  void slotMdiModeHasBeenChangedTo(KMdi::MdiMode);
#else
#pragma WARNING( TODO  void slotMdiModeHasBeenChangedTo(KMdi::MdiMode); )
#endif
#endif
    //! reimplemented to add "restart is required" message box
//2.0: unused  virtual void switchToIDEAlMode();
//2.0: unused  void switchToIDEAlMode(bool showMessage);
//2.0: unused  virtual void switchToChildframeMode();
//2.0: unused  void switchToChildframeMode(bool showMessage);

    /*! Shows Project Migration Wizard. \return true on successful migration,
     cancelled on cancellation, and false on failure.
     If \a mimeType and \a databaseName are not empty, the wizard will only ask about
     parameters of destination project and skip pages related to source project.
     \a cdata connection data can be also provided to preselect server-based connections. */
    tristate showProjectMigrationWizard(const QString& mimeType, const QString& databaseName, const KexiDB::ConnectionData *cdata = 0);

    //! Receives "selectionChanged()" signal from navigator to update some actions.
    void slotPartItemSelectedInNavigator(KexiPart::Item* item);

    /*! Receives the "executeItem" signal from navigator to perform "execute" action
     on \a item. \return true on success */
    tristate executeItem(KexiPart::Item* item);

    //! Shows "export as data table" dialog for \a item.
    tristate exportItemAsDataTable(KexiPart::Item* item);

    //! Shows "copy special as data table" dialog for \a item.
    tristate copyItemToClipboardAsDataTable(KexiPart::Item* item);

    //! Shows "print" dialog for \a item.
    //! \return true on success.
    bool printItem(KexiPart::Item* item, const QString& titleText);

    //! Shows "print" dialog for \a item and \a settings.
    //! \return true on success.
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO reenable when ported  bool printItem(KexiPart::Item* item, const KexiSimplePrintingSettings& settings, const QString& titleText = QString());
#else
#pragma WARNING( TODO reenable when ported  bool printItem(KexiPart::Item* item, const KexiSimplePrintingSettings& settings, const QString& titleText = QString()); )
#endif
#endif
    /*! Shows "print preview" window for \a item.
     The preview windoe is cached, so \a reload == true is sometimes needed
     if data or print settings have changed in the meantime.
     \return true on success. */
    bool printPreviewForItem(KexiPart::Item* item, const QString& titleText, bool reload);

    //! Shows "print preview" window.
    //! \return true on success.
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO reenable when ported  bool printPreviewForItem(KexiPart::Item* item, const KexiSimplePrintingSettings& settings, const QString& titleText = QString(), bool reload = false);
#else
#pragma WARNING( TODO reenable when ported  bool printPreviewForItem(KexiPart::Item* item, const KexiSimplePrintingSettings& settings, const QString& titleText = QString(), bool reload = false); )
#endif
#endif
    /*! Implemented for KexiMainWindow. Helper for printItem() and printPreviewForItem().
     Also used by KexiFormEventAction.
     \return true on success and cancelled when the action was cancelled. */
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO reenable when ported  tristate printActionForItem(KexiPart::Item* item, PrintActionType action);
#else
#pragma WARNING( TODO reenable when ported  tristate printActionForItem(KexiPart::Item* item, PrintActionType action); )
#endif
#endif

    void slotSetProjectNavigatorVisible(bool set);
    void slotSetPropertyEditorVisible(bool set);
    void slotProjectNavigatorVisibilityChanged(bool visible);
    void slotMultiTabBarTabClicked(int id);

private:
    class MessageHandler;
    class Private;
    Private * const d;

    friend class KexiWindow;
    friend class KexiMainWidget;
};

#endif
