/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#include <QKeyEvent>
#include <QScopedPointer>
#include <QTabWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStackedLayout>
#include <QProxyStyle>
#include <QTabBar>
#include <QTimer>
#include <QDockWidget>
#include <QShortcut>

#include <KToolBar>
#include <KHelpMenu>
#include <KAboutData>
#include <KActionCollection>
#include <KMultiTabBar>
#include <KActionMenu>
#include <KMainWindow>
#include <KSharedConfig>

#include "KexiMainWindow.h"
#include "KexiSearchLineEdit.h"
#include "KexiUserFeedbackAgent.h"
#include "KexiMenuWidget.h"
#include "kexifinddialog.h"
#include "kexistatusbar.h"
#include "KexiStartup.h"
#include <kexiutils/utils.h>
#include <widget/utils/KexiDockableWidget.h>
#include <widget/properties/KexiPropertyEditorView.h>
#include <widget/KexiNameDialog.h>
#include <core/kexi.h>
#include <core/KexiWindow.h>
#include <core/kexipartinfo.h>

#define KEXI_NO_PROCESS_EVENTS

#ifdef KEXI_NO_PROCESS_EVENTS
# define KEXI_NO_PENDING_DIALOGS
#endif

#define PROJECT_NAVIGATOR_TABBAR_ID 0
#define PROPERTY_EDITOR_TABBAR_ID 1
#define KEXITABBEDTOOLBAR_SPACER_TAB_INDEX 1

class QPainter;
class KexiProjectNavigator;

//! @short Main application's tabbed toolbar
class KexiTabbedToolBar : public QTabWidget
{
    Q_OBJECT
public:
    explicit KexiTabbedToolBar(QWidget *parent);
    virtual ~KexiTabbedToolBar();

    KToolBar *createWidgetToolBar() const;

    KToolBar *toolBar(const QString& name) const;

    void appendWidgetToToolbar(const QString& name, QWidget* widget);

    void setWidgetVisibleInToolbar(QWidget* widget, bool visible);

//! @todo replace with the final Actions API
    void addAction(const QString& toolBarName, QAction *action);

    bool mainMenuVisible() const;

    QRect tabRect(int index) const;

    KHelpMenu *helpMenu() const;

    void addSearchableModel(KexiSearchableModel *model);

    KToolBar *createToolBar(const char *name, const QString& caption);

    void setCurrentTab(const QString& name);

    //! Sets current tab to @a index, counting from first visible (non-Kexi) tab.
    //! In non-user mode, the first visible tab is "create" tab.
    void setCurrentTab(int index);

    void hideTab(const QString& name);

    void showTab(const QString& name);

    bool isTabVisible(const QString& name) const;

    bool isRolledUp();

public Q_SLOTS:
    void setMainMenuContent(QWidget *w);
    void selectMainMenuItem(const char *actionName);
    void showMainMenu(const char* actionName = 0);
    void hideMainMenu();
    void toggleMainMenu();
    void activateSearchLineEdit();
    void toggleRollDown();

protected:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual bool eventFilter(QObject* watched, QEvent* event);

protected Q_SLOTS:
    void slotCurrentChanged(int index);
    void slotDelayedTabRaise();
    void slotSettingsChanged(int category);
    //! Used for delayed loading of the "create" toolbar. Called only once.
    void setupCreateWidgetToolbar();
    void slotTabDoubleClicked(int index);
    void tabBarAnimationFinished();

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
    explicit KexiWindowContainer(QWidget* parent);

    virtual ~KexiWindowContainer();

    void setWindow(KexiWindow* w);

    QPointer<KexiWindow> window;

private:
    QVBoxLayout *lyr;
};

class EmptyMenuContentWidget : public QWidget
{
public:
    explicit EmptyMenuContentWidget(QWidget* parent = 0);

    void alterBackground();

    virtual void changeEvent(QEvent *e);
};

//! @todo KEXI3 is KexiMenuWidgetStyle needed?
#if 0
//! A style proxy for KexiMenuWidget
class KexiMenuWidgetStyle : public KexiUtils::StyleProxy
{
public:
    explicit KexiMenuWidgetStyle(QStyle *style, QObject *parent = 0);

    virtual ~KexiMenuWidgetStyle();

    virtual void drawControl(ControlElement element, const QStyleOption *option,
                             QPainter *painter, const QWidget *widget = 0) const;
};
#endif

//! Main menu
class KexiMainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit KexiMainMenu(KexiTabbedToolBar *toolBar, QWidget* parent = 0);

    ~KexiMainMenu();

    virtual bool eventFilter(QObject * watched, QEvent* event);

    void setContent(QWidget *contentWidget);

    const QWidget *contentWidget() const;

    void setPersistentlySelectedAction(KexiMenuWidgetAction* action, bool set);

/*    void setActiveAction(QAction* action = 0);*/

    void selectFirstItem();

    tristate showProjectMigrationWizard(
        const QString& mimeType, const QString& databaseName, const KDbConnectionData *cdata);

Q_SIGNALS:
    void contentAreaPressed();
    void hideContentsRequested();

protected Q_SLOTS:
    //void contentWidgetDestroyed();

protected:
    virtual void showEvent(QShowEvent * event);

private:
    QPointer<KexiMenuWidget> m_menuWidget;
    KexiTabbedToolBar* m_toolBar;
    bool m_initialized;
    EmptyMenuContentWidget *m_content;
    QStackedLayout *m_contentLayout;
    QPointer<QWidget> m_contentWidget;
    QVBoxLayout* m_mainContentLayout;
    QPointer<KexiMenuWidgetAction> m_persistentlySelectedAction;
    bool m_selectFirstItem;
};

class KexiTabbedToolBarTabBar;

//! @internal
class KexiTabbedToolBar::Private : public QObject
{
    Q_OBJECT
public:
    explicit Private(KexiTabbedToolBar *t);

    KToolBar *createToolBar(const char *name, const QString& caption);

    int tabIndex;

public Q_SLOTS:
    void showMainMenu(const char* actionName = 0);
    void hideMainMenu();
    void hideContentsOrMainMenu();
    void toggleMainMenu();
    void updateMainMenuGeometry();

public:
    KexiTabbedToolBarTabBar *customTabBar;
    QPointer<KexiMainMenu> mainMenu;

    KexiTabbedToolBar *q;
    KActionCollection *ac;
    int createId;
    KToolBar *createWidgetToolBar;
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
    //! Used for delayed tab raising
    int tabToRaise;
    //! Used for delayed tab raising
    QTimer tabRaiseTimer;
#endif
    //! Toolbars for name
    QHash<QString, KToolBar*> toolbarsForName;
    QHash<QString, int> toolbarsIndexForName;
    QHash<QString, QString> toolbarsCaptionForName;
    QVector<bool> toolbarsVisibleForIndex;
    QHash<QWidget*, QAction*> extraActions;
    bool rolledUp;
    QPropertyAnimation tabBarAnimation;
    QGraphicsOpacityEffect tabBarOpacityEffect;
    int rolledUpIndex;
    KHelpMenu *helpMenu;
    KexiSearchLineEdit *searchLineEdit;
    void setCurrentTab(const QString& name);
    void hideTab(const QString& name);
    void showTab(const QString& name);
    bool isTabVisible(const QString& name) const;
#ifndef NDEBUG
    void debugToolbars() const;
#endif
    int lowestIndex;
};

class KexiTabbedToolBarStyle;

//! Tab bar reimplementation for KexiTabbedToolBar.
/*! The main its purpose is to alter the width of "Kexi" tab.
*/
class KexiTabbedToolBarTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit KexiTabbedToolBarTabBar(QWidget *parent = 0);
    virtual QSize originalTabSizeHint(int index) const;
    virtual QSize tabSizeHint(int index) const;

    KexiTabbedToolBarStyle* customStyle;
};

//! Style proxy for KexiTabbedToolBar, to get the "Kexi" tab style right.
class KexiTabbedToolBarStyle : public QProxyStyle
{
public:
    explicit KexiTabbedToolBarStyle(const QString &baseStyleName);

    virtual ~KexiTabbedToolBarStyle();

    virtual void drawControl(ControlElement element, const QStyleOption *option,
                             QPainter *painter, const QWidget *widget = 0) const;

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                               QPainter *painter, const QWidget *widget = 0) const;

    virtual int pixelMetric(PixelMetric metric, const QStyleOption* option = 0,
                            const QWidget* widget = 0) const;
};

class KexiMainWidget;

//! @internal tab widget acting as central widget for KexiMainWindow
class KexiMainWindowTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    KexiMainWindowTabWidget(QWidget *parent, KexiMainWidget *mainWidget);
    virtual ~KexiMainWindowTabWidget();
public Q_SLOTS:
    void closeTab();
    tristate closeAllTabs();

protected:
    //! Shows context menu for tab at @a index at point @a point.
    //! If @a index is -1, context menu for empty area is requested.
    void showContextMenuForTab(int index, const QPoint& point);

    //! Reimplemented to hide frame when no tabs are displayed
    virtual void paintEvent(QPaintEvent * event);

    virtual void mousePressEvent(QMouseEvent *event);

    KexiMainWidget *m_mainWidget;
    QAction *m_closeAction;
    QAction *m_closeAllTabsAction;

private:
    int m_tabIndex;

    void setTabIndexFromContextMenu(int clickedIndex);
};

//! @short A widget being main part of KexiMainWindow
class KexiMainWidget : public KMainWindow
{
    Q_OBJECT
public:
    KexiMainWidget();

    virtual ~KexiMainWidget();

    void setParent(KexiMainWindow* mainWindow);

    KexiMainWindowTabWidget* tabWidget() const;

protected:
    virtual bool queryClose();
protected Q_SLOTS:
    void slotCurrentTabIndexChanged(int index);
Q_SIGNALS:
    void currentTabIndexChanged(int index);

private:
    void setupCentralWidget();

    KexiMainWindowTabWidget* m_tabWidget;
    KexiMainWindow *m_mainWindow;
    QPointer<KexiWindow> m_previouslyActiveWindow;

    friend class KexiMainWindow;
    friend class KexiMainWindowTabWidget;
};

//------------------------------------------

//! @internal Dock widget with floating disabled but still collapsible
class KexiDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    KexiDockWidget(const QString &title, QWidget *parent);

    virtual ~KexiDockWidget();

    virtual void setSizeHint(const QSize& hint);

    virtual QSize sizeHint() const;

    QToolButton* closeButton() const;

protected:
    virtual void paintEvent(QPaintEvent *pe);

protected Q_SLOTS:
    void slotCloseClicked();

private:
    class Private;
    Private * const d;
};

//------------------------------------------

//! @internal safer dictionary
typedef QMap< int, KexiWindow* > KexiWindowDict;

//! @todo KEXI3 remove when Migation is ported
class KexiMigrateManagerTemp : public QObject, public KexiMigrateManagerInterface
{
public:
    virtual ~KexiMigrateManagerTemp();

    //! Implement to return the list offile MIME types that are supported by migration drivers
    virtual QStringList supportedFileMimeTypes();
};

//! @internal
class KexiMainWindow::Private
{
public:
    explicit Private(KexiMainWindow* w);

    ~Private();

#ifndef KEXI_NO_PENDING_DIALOGS
    //! Job type. Currently used for marking items as being opened or closed.
    enum PendingJobType {
        NoJob = 0,
        WindowOpeningJob,
        WindowClosingJob
    };

    KexiWindow *openedWindowFor(const KexiPart::Item* item, PendingJobType &pendingType);

    KexiWindow *openedWindowFor(int identifier, PendingJobType &pendingType);

    void addItemToPendingWindows(const KexiPart::Item* item, PendingJobType jobType);

    bool pendingWindowsExist();

    void removePendingWindow(int identifier);
#else
    KexiWindow *openedWindowFor(int identifier);
#endif

    void insertWindow(KexiWindow *window);

    bool windowContainerExistsFor(int identifier) const;

    void setWindowContainerExistsFor(int identifier, bool set);

    void updateWindowId(KexiWindow *window, int oldItemID);

    void removeWindow(int identifier);

    int openedWindowsCount();

    //! Used in KexiMainWindowe::closeProject()
    void clearWindows();

    void showStartProcessMsg(const QStringList& args);

    //! Updates Property Editor Pane's visibility for the current window and the @a viewMode view mode.
    /*! @a info can be provided to hadle cases when current window is not yet defined (in openObject()). */
    void updatePropEditorVisibility(Kexi::ViewMode viewMode, KexiPart::Info *info = 0);

    void setTabBarVisible(KMultiTabBar::KMultiTabBarPosition position, int id,
                          KexiDockWidget *dockWidget, bool visible);

    void setPropertyEditorTabBarVisible(bool visible);

    QObject *openedCustomObjectsForItem(KexiPart::Item* item, const char* name);

    void addOpenedCustomObjectForItem(KexiPart::Item* item, QObject* object, const char* name);

    KexiFindDialog *findDialog();

    /*! Updates the find/replace dialog depending on the active view.
     Nothing is performed if the dialog is not instantiated yet or is invisible. */
    void updateFindDialogContents(bool createIfDoesNotExist = false);

    //! \return the current view if it supports \a actionName, otherwise returns 0.
    KexiView *currentViewSupportingAction(const char* actionName) const;

    //! \return the current view if it supports KexiSearchAndReplaceViewInterface.
    KexiSearchAndReplaceViewInterface* currentViewSupportingSearchAndReplaceInterface() const;

    tristate showProjectMigrationWizard(
        const QString& mimeType, const QString& databaseName, const KDbConnectionData *cdata);

    KexiMainWindow *wnd;
    KexiMainWidget *mainWidget;
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
    QMap<int, QString> tabsToActivateOnShow;
    KexiDockWidget *navDockWidget;
    QTabWidget *propEditorTabWidget;
    KexiDockWidget *propEditorDockWidget;
    QPointer<KexiDockableWidget> propEditorDockableWidget;
    //! poits to kexi part which has been previously used to setup proppanel's tabs using
    //! KexiPart::setupCustomPropertyPanelTabs(), in updateCustomPropertyPanelTabs().
    QPointer<KexiPart::Part> partForPreviouslySetupPropertyPanelTabs;
    QMap<KexiPart::Part*, int> recentlySelectedPropertyPanelPages;
    QPointer<KexiPropertyEditorView> propEditor;
    QPointer<KPropertySet> propertySet;

    KexiNameDialog *nameDialog;
    QTimer timer; //!< helper timer
    QString appCaptionPrefix; //<! application's caption prefix - prj name (if opened), else: null

#ifndef KEXI_SHOW_UNIMPLEMENTED
    KActionMenu *dummy_action;
#endif

    //! Kexi menu
    QAction *action_save, *action_save_as,
    *action_project_import_export_send, *action_close,
    *action_project_properties,
    *action_project_relations, *action_project_import_data_table,
    *action_project_export_data_table;
#ifndef KEXI_NO_QUICK_PRINTING
    QAction *action_project_print, *action_project_print_preview,
        *action_project_print_setup;
#endif
    QAction *action_project_welcome;
    QAction *action_show_other;
    int action_welcome_projects_title_id,
    action_welcome_connections_title_id;
    QAction *action_settings;

    //! edit menu
    QAction *action_edit_delete, *action_edit_delete_row,
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
    QAction *action_view_nav, *action_view_propeditor, *action_view_mainarea;
#ifndef KEXI_NO_CTXT_HELP
    KToggleAction *action_show_helper;
#endif
    //! data menu
    QAction *action_data_save_row;
    QAction *action_data_cancel_row_changes;
    QAction *action_data_execute;

    //! format menu
    QAction *action_format_font;

    //! tools menu
    QAction *action_tools_import_project, *action_tools_compact_database, *action_tools_data_import;
    KActionMenu *action_tools_scripts;

    //! window menu
    QAction *action_window_next, *action_window_previous, *action_window_fullscreen;

    //! global
    QAction *action_show_help_menu;
    QAction *action_view_global_search;
    //! for dock windows

    QPointer<QWidget> focus_before_popup;

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

    void executeActionWhenPendingJobsAreFinished();
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

    //! Indicates if project is started in User Mode
    bool userMode;

    //! Indicates if project navigator should be visible
    bool isProjectNavigatorVisible;

    //! Indicates if the main menu should be visible
    bool isMainMenuVisible;

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

    bool wasAutoOpen;
    bool windowExistedBeforeCloseProject;

    QMap<KMultiTabBar::KMultiTabBarPosition, KMultiTabBar*> multiTabBars;
    bool propertyEditorCollapsed;

    bool enable_slotPropertyEditorVisibilityChanged;

    KexiUserFeedbackAgent userFeedback;

    //! @todo KEXI3
#if 0
    QScopedPointer<KexiMigration::MigrateManager> migrateManager;
#else
    QScopedPointer<KexiMigrateManagerTemp> migrateManager;
#endif

private:
    //! @todo move to KexiProject
    KexiWindowDict windows;
    //! A set of item identifiers for whose there are KexiWindowContainer instances already.
    //! This lets to verify that KexiWindow is about to be constructed and opened so multiple
    //! opening can be avoided.
    QSet<int> windowContainers;
#ifndef KEXI_NO_PROCESS_EVENTS
    QHash<int, PendingJobType> pendingWindows; //!< part item identifiers for windows whoose opening has been started
    //! @todo QMutex dialogsMutex; //!< used for locking windows and pendingWindows dicts
#endif
    KexiFindDialog *m_findDialog;
};

//------------------------------------------

//! Action shortcut used by KexiMainWindow::setupMainMenuActionShortcut(QAction *)
//! Activates action only if enabled.
class KexiMainMenuActionShortcut : public QShortcut
{
    Q_OBJECT
public:
    KexiMainMenuActionShortcut(const QKeySequence& key, QAction *action, QWidget *parent);

    virtual ~KexiMainMenuActionShortcut();

protected Q_SLOTS:
    //! Triggers associated action only when this action is enabled
    void slotActivated();

private:
    QPointer<QAction> m_action;
};

#endif
