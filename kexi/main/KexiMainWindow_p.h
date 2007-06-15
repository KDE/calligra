/* This file is part of the KDE projec
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifdef KEXI_NO_PROCESS_EVENTS
# define KEXI_NO_PENDING_DIALOGS
#endif

//! @internal safer dictionary
typedef QMap< int, QPointer<KexiWindow> > KexiWindowDict;

//! @internal
class KexiMainWindow::Private
{
public:
	Private(KexiMainWindow* w)
//		: dialogs(401)
		: wnd(w)
	{
		dummy_KXMLGUIClient = new KXMLGUIClient();
		dummy_KXMLGUIFactory = new KXMLGUIFactory(0);
		
		actionCollection = new KActionCollection(w);
		propEditor=0;
//2.0: unused				propEditorToolWindow=0;
//2.0: unused				propEditorTabWidget=0;
		KexiProjectData *pdata = Kexi::startupHandler().projectData();
		userMode = Kexi::startupHandler().forcedUserMode() /* <-- simply forced the user mode */
		/* project has 'user mode' set as default and not 'design mode' override is found: */
		|| (pdata && pdata->userMode() && !Kexi::startupHandler().forcedDesignMode());
		isProjectNavigatorVisible = Kexi::startupHandler().isProjectNavigatorVisible();
		nav=0;
//2.0: unused				navToolWindow=0;
		prj = 0;
		config = KGlobal::config();
		curWindowGUIClient=0;
		curWindowViewGUIClient=0;
		closedWindowGUIClient=0;
		closedWindowViewGUIClient=0;
		nameDialog=0;
		curWindow=0;
		m_findDialog=0;
//2.0: unused		block_KMdiMainFrm_eventFilter=false;
		focus_before_popup=0;
//		relationPart=0;
//moved		privateIDCounter=0;
		action_view_nav=0;
		action_view_propeditor=0;
		action_view_mainarea=0;
		action_open_recent_projects_title_id = -1;
		action_open_recent_connections_title_id = -1;
		forceWindowClosing=false;
		insideCloseWindow=false;
#ifndef KEXI_NO_PENDING_DIALOGS
		actionToExecuteWhenPendingJobsAreFinished = NoAction;
#endif
//		callSlotLastChildViewClosedAfterCloseDialog=false;
		createMenu=0;
		showImportantInfoOnStartup=true;
//		disableErrorMessages=false;
//		last_checked_mode=0;
		propEditorDockSeparatorPos=-1;
		navDockSeparatorPos=-1;
//		navDockSeparatorPosWithAutoOpen=-1;
		wasAutoOpen = false;
		windowExistedBeforeCloseProject = false;
#ifndef KEXI_SHOW_UNIMPLEMENTED
		dummy_action = new KActionMenu("", wnd);
#endif
		maximizeFirstOpenedChildFrm = false;
#ifdef HAVE_KNEWSTUFF
		newStuff = 0;
#endif
//2.0: unused		mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
		forceShowProjectNavigatorOnCreation = false;
		forceHideProjectNavigatorOnCreation = false;
		navWasVisibleBeforeProjectClosing = false;
		saveSettingsForShowProjectNavigator = true;
	}
	~Private() {
		qDeleteAll(m_openedCustomObjectsForItem);
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	//! Job type. Currently used for marking items as being opened or closed.
	enum PendingJobType {
		NoJob = 0,
		WindowOpeningJob,
		WindowClosingJob
	};

	KexiWindow *openedWindowFor( const KexiPart::Item* item, PendingJobType &pendingType )
	{
		return openedWindowFor( item->identifier(), pendingType );
	}

	KexiWindow *openedWindowFor( int identifier, PendingJobType &pendingType )
	{
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		QHash<int, PendingJobType>::ConstIterator it = pendingWindows.find( identifier );
		if (it==pendingWindows.end())
			pendingType = NoJob;
		else
			pendingType = it.value();
		
		if (pendingType == WindowOpeningJob) {
			return 0;
		}
		return (KexiWindow*)windows[ identifier ];
	}
#else
	KexiWindow *openedWindowFor( const KexiPart::Item* item )
	{
		return openedWindowFor( item->identifier() );
	}

	KexiWindow *openedWindowFor( int identifier )
	{
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		return (KexiWindow*)windows[ identifier ];
	}
#endif

	void insertWindow(KexiWindow *window) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		windows.insert(window->id(), QPointer<KexiWindow>(window));
#ifndef KEXI_NO_PENDING_DIALOGS
		pendingWindows.remove(window->id());
#endif
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	void addItemToPendingWindows(const KexiPart::Item* item, PendingJobType jobType) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		pendingWindows.insert( item->identifier(), jobType );
	}

	bool pendingWindowsExist() {
		if (pendingWindows.begin()!=pendingWindows.end())
			kDebug() << 	pendingWindows.constBegin().key() << " " << (int)pendingWindows.constBegin().value() << endl;
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		return !pendingWindows.isEmpty();
	}
#endif

	void updateWindowId(KexiWindow *window, int oldItemID) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		windows.remove(oldItemID);
#ifndef KEXI_NO_PENDING_DIALOGS
		pendingWindows.remove(oldItemID);
#endif
		windows.insert(window->id(), QPointer<KexiWindow>(window));
	}

	void removeWindow(int identifier) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		windows.remove(identifier);
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	void removePendingWindow(int identifier) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		pendingWindows.remove(identifier);
	}
#endif

	uint openedWindowsCount() {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		return windows.count();
	}

	//! Used in KexiMainWindowe::closeProject()
	void clearWindows() {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		windows.clear();
#ifndef KEXI_NO_PENDING_DIALOGS
		pendingWindows.clear();
#endif
	}

	/*! Toggles last checked view mode radio action, if available. */
	void toggleLastCheckedMode()
	{
		if (curWindow.isNull())
			return;
		KToggleAction *ta = actions_for_view_modes.value( (int)curWindow->currentViewMode() );
		if (ta)
			ta->setChecked(true);
//		if (!last_checked_mode)
//			return;
//		last_checked_mode->setChecked(true);
	}

/*
void updatePropEditorDockWidthInfo() {
		if (propEditor) {
			KDockWidget *dw = (KDockWidget *)propEditor->parentWidget();
#if defined(KDOCKWIDGET_P)
			KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
			if (ds) {
				propEditorDockSeparatorPos = ds->separatorPosInPercent();*/
/*				if (propEditorDockSeparatorPos<=0) {
						config->setGroup("MainWindow");
						propEditorDockSeparatorPos = config->readEntry("RightDockPosition", 80);
						ds->setSeparatorPos(propEditorDockSeparatorPos, true);
				}*/
			/*}
#endif
		}
	}*/

	void showStartProcessMsg(const QStringList& args)
	{
		wnd->showErrorMessage(i18n("Could not start %1 application.", QString(KEXI_APP_NAME)),
			i18n("Command \"%1\" failed.", args.join(" ")));
	}

	void hideMenuItem(const QString& menuName, const QString& itemText, bool alsoSeparator)
	{
		Q3PopupMenu *pm = popups[menuName.toLatin1()];
		if (!pm)
			return;
		const QList<QAction*> actions( pm->actions() );
		bool nowHideSeparator = false;
		foreach( QAction *action, actions ) {
			kDebug() << action->text() << endl;
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
#warning reimplement disableMenuItem()
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
	}

	void updatePropEditorVisibility(Kexi::ViewMode viewMode)
	{
#warning TODO updatePropEditorVisibility
#if 0 //TODO reenable
		if (propEditorToolWindow) {
			if (viewMode==0 || viewMode==Kexi::DataViewMode) {
#ifdef PROPEDITOR_VISIBILITY_CHANGES
				wnd->makeDockInvisible( wnd->manager()->findWidgetParentDock(propEditor) );
//				propEditorToolWindow->hide();
#endif
			} else {
				//propEditorToolWindow->show();
				QWidget *origFocusWidget = qApp->focusWidget();
				wnd->makeWidgetDockVisible(propEditorTabWidget);
				if (origFocusWidget)
					origFocusWidget->setFocus();
/*moved
#if defined(KDOCKWIDGET_P)
				KDockWidget *dw = (KDockWidget *)propEditor->parentWidget();
				KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
				ds->setSeparatorPosInPercent(config->readEntry("RightDockPosition", 80));//%
#endif*/
			}
		}
#endif
	}

	void restoreNavigatorWidth()
	{
//2.0: unused
#if 0
#if defined(KDOCKWIDGET_P)
			if (wnd->mdiMode()==KMdi::ChildframeMode || wnd->mdiMode()==KMdi::TabPageMode) {
				KDockWidget *dw = (KDockWidget *)nav->parentWidget();
				KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
//				ds->setKeepSize(true);

				config->setGroup("MainWindow");
# if KDE_VERSION >= KDE_MAKE_VERSION(3,4,0)

				if (wasAutoOpen) //(dw2->isVisible())
//				ds->setSeparatorPosInPercent( 100 * nav->width() / wnd->width() );
					ds->setSeparatorPosInPercent(
						qMax(qMax( config->readEntry("LeftDockPositionWithAutoOpen",20),
						config->readEntry("LeftDockPosition",20)),20)
					);
				else
					ds->setSeparatorPosInPercent(
					qMax(20, config->readEntry("LeftDockPosition", 20/* % */)));

	//			dw->resize( d->config->readEntry("LeftDockPosition", 115/* % */), dw->height() );
# else
				//there were problems on KDE < 3.4
				ds->setSeparatorPosInPercent( 20 );
# endif
				//if (!wasAutoOpen) //(dw2->isVisible())
//					ds->setSeparatorPos( ds->separatorPos(), true );
			}
#endif

#endif
	}

	template<class type>
	type *openedCustomObjectsForItem(KexiPart::Item* item, const char* name)
	{
		if (!item || !name) {
			kWarning() << 
				"KexiMainWindow::Private::openedCustomObjectsForItem(): !item || !name" << endl;
			return 0;
		}
		QByteArray key( QByteArray::number(item->identifier()) + name );
		return qobject_cast<type*>( m_openedCustomObjectsForItem.value( key ) );
	}

	void addOpenedCustomObjectForItem(KexiPart::Item* item, QObject* object, const char* name)
	{
		QByteArray key( QByteArray::number(item->identifier()) + name );
		m_openedCustomObjectsForItem.insert( key, object );
	}

	KexiFindDialog *findDialog() {
		if (!m_findDialog) {
			m_findDialog = new KexiFindDialog(wnd);
			m_findDialog->setActions( action_edit_findnext, action_edit_findprev,
				action_edit_replace, action_edit_replace_all );
/*			connect(m_findDialog, SIGNAL(findNext()), action_edit_findnext, SLOT(activate()));
			connect(m_findDialog, SIGNAL(find()), wnd, SLOT(slotEditFindNext()));
			connect(m_findDialog, SIGNAL(replace()), wnd, SLOT(slotEditReplaceNext()));
			connect(m_findDialog, SIGNAL(replaceAll()), wnd, SLOT(slotEditReplaceAll()));*/
		}
		return m_findDialog;
	}

	/*! Updates the find/replace dialog depending on the active view.
	 Nothing is performed if the dialog is not instantiated yet or is invisible. */
	void updateFindDialogContents(bool createIfDoesNotExist = false) {
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
		findDialog()->setObjectNameForCaption(curWindow->partItem()->name());

		QStringList columnNames;
		QStringList columnCaptions;
		QString currentColumnName; // for 'look in'
		if (!iface->setupFindAndReplace(columnNames, columnCaptions, currentColumnName)) {
			m_findDialog->setButtonsEnabled(false);
			m_findDialog->setLookInColumnList(QStringList(), QStringList());
			return;
		}
		m_findDialog->setButtonsEnabled(true);

	/*	//update "look in" list
		KexiTableViewColumn::List columns( dataAwareObject()->data()->columns );
		QStringList columnNames;
		QStringList columnCaptions;
		for (KexiTableViewColumn::ListIterator it(columns); it.current(); ++it) {
			if (!it.current()->visible())
				continue;
			columnNames.append( it.current()->field()->name() );
			columnCaptions.append( it.current()->captionAliasOrName() );
		}*/
		const QString prevColumnName( m_findDialog->currentLookInColumnName());
		m_findDialog->setLookInColumnList(columnNames, columnCaptions);
		m_findDialog->setCurrentLookInColumnName( prevColumnName );
	}

	//! \return the current view if it supports \a actionName, otherwise returns 0.
	KexiView *currentViewSupportingAction(const char* actionName) const
	{
		if (!curWindow)
			return 0;
		KexiView *view = curWindow->selectedView();
		if (!view)
			return 0;
		QAction *action = view->sharedAction(actionName);
		if (!action || !action->isEnabled())
			return 0;
		return view;
	}

	//! \return the current view if it supports KexiSearchAndReplaceViewInterface.
	KexiSearchAndReplaceViewInterface* currentViewSupportingSearchAndReplaceInterface() const
	{
		if (!curWindow)
			return 0;
		KexiView *view = curWindow->selectedView();
		if (!view)
			return 0;
		return dynamic_cast<KexiSearchAndReplaceViewInterface*>(view);
	}

		KXMLGUIClient* dummy_KXMLGUIClient;
		KXMLGUIFactory* dummy_KXMLGUIFactory;

		KexiMainWindow *wnd;
		KexiMainWindowTabWidget *tabWidget;
		KActionCollection *actionCollection;
		KexiStatusBar *statusBar;
		KexiProject *prj;
		KSharedConfig::Ptr config;
#ifndef KEXI_NO_CTXT_HELP
		KexiContextHelp *ctxHelp;
#endif
		KexiBrowser *nav;
		QDockWidget *navDockWidget;
		KTabWidget *propEditorTabWidget;
		//! poits to kexi part which has been previously used to setup proppanel's tabs using 
		//! KexiPart::setupCustomPropertyPanelTabs(), in updateCustomPropertyPanelTabs().
		QPointer<KexiPart::Part> partForPreviouslySetupPropertyPanelTabs;
		QMap<KexiPart::Part*, int> recentlySelectedPropertyPanelPages;
		QPointer<KexiPropertyEditorView> propEditor;
		QPointer<KoProperty::Set> propBuffer;

		KXMLGUIClient *curWindowGUIClient, *curWindowViewGUIClient,
			*closedWindowGUIClient, *closedWindowViewGUIClient;
		QPointer<KexiWindow> curWindow;

		KexiNameDialog *nameDialog;

		QTimer timer; //helper timer
//		QSignalMapper *actionMapper;

//! @todo move menu handling outside
		Q3AsciiDict<Q3PopupMenu> popups; //list of menu popups
		Q3PopupMenu *createMenu;

		QString origAppCaption; //<! original application's caption (without project name)
		QString appCaptionPrefix; //<! application's caption prefix - prj name (if opened), else: null

#ifndef KEXI_SHOW_UNIMPLEMENTED
		KActionMenu *dummy_action;
#endif

		//! project menu
		KAction *action_save, *action_save_as, *action_close,
			*action_project_properties, *action_open_recent_more,
			*action_project_relations, *action_project_import_data_table,
 			*action_project_export_data_table,
			*action_project_print, *action_project_print_preview, 
			*action_project_print_setup;
//		KRecentFilesAction *action_open_recent;
		KActionMenu *action_open_recent, *action_show_other;
//		int action_open_recent_more_id;
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
		QActionGroup* action_view_mode;
		KToggleAction *action_view_data_mode, *action_view_design_mode, *action_view_text_mode;
		QHash<int, KToggleAction*> actions_for_view_modes;
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
		KAction *action_tools_data_migration, *action_tools_compact_database;
		KActionMenu *action_tools_scripts;

		//! window menu
		KAction *action_window_next, *action_window_previous;

		//! settings menu
		KAction *action_configure;

		//! for dock windows
//2.0: unused		KMdiToolViewAccessor* navToolWindow;
//2.0: unused		KMdiToolViewAccessor* propEditorToolWindow;

		QPointer<QWidget> focus_before_popup;
//		KexiRelationPart *relationPart;

//moved		int privateIDCounter; //!< counter: ID for private "document" like Relations window

//2.0: unused	bool block_KMdiMainFrm_eventFilter : 1;

		//! Set to true only in destructor, used by closeWindow() to know if
		//! user can cancel window closing. If true user even doesn't see any messages
		//! before closing a window. This is for extremely sanity... and shouldn't be even needed.
		bool forceWindowClosing : 1;

		//! Indicates that we're inside closeWindow() method - to avoid inf. recursion
		//! on window removing
		bool insideCloseWindow : 1;

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

		//! Opened page setup dialogs, used by printOrPrintPreviewForItem().
		QHash<int, KexiWindow*> pageSetupWindows;

		/*! A map from Kexi dialog to "print setup" part item's ID of the data item
		 used by closeWindow() to find an ID of the data item, so the entry 
		 can be removed from pageSetupWindows dictionary. */
		QMap<int, int> pageSetupWindowItemID2dataItemID_map;

		//! Used in several places to show info dialog at startup (only once per session)
		//! before displaying other stuff
		bool showImportantInfoOnStartup : 1;

//		//! Used sometimes to block showErrorMessage()
//		bool disableErrorMessages : 1;

		//! Indicates if project is started in User Mode
		bool userMode : 1;

		//! Indicates if project navigator should be visible
		bool isProjectNavigatorVisible : 1;

		//! Used on opening 1st child window
		bool maximizeFirstOpenedChildFrm : 1;

		//! Set in restoreSettings() and used in initNavigator() 
		//! to customize navigator visibility on startup
		bool forceShowProjectNavigatorOnCreation : 1;
		bool forceHideProjectNavigatorOnCreation : 1;

		bool navWasVisibleBeforeProjectClosing : 1;
		bool saveSettingsForShowProjectNavigator : 1;
#ifdef HAVE_KNEWSTUFF
		KexiNewStuff  *newStuff;
#endif

		//! Used by openedCustomObjectsForItem() and addOpenedCustomObjectForItem()
		QHash<QByteArray, QObject*> m_openedCustomObjectsForItem;

		int propEditorDockSeparatorPos, navDockSeparatorPos;
//	int navDockSeparatorPosWithAutoOpen;
		bool wasAutoOpen;
		bool windowExistedBeforeCloseProject;

//2.0: unused		KMdi::MdiMode mdiModeToSwitchAfterRestart;

	private:
		//! @todo move to KexiProject
		KexiWindowDict windows;
#ifndef KEXI_NO_PROCESS_EVENTS
		QHash<int, PendingJobType> pendingWindows; //!< part item identifiers for windows whoose opening has been started
	//todo(threads)	QMutex dialogsMutex; //!< used for locking windows and pendingWindows dicts
#endif
		KexiFindDialog *m_findDialog;
};
