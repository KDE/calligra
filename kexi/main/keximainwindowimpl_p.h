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
typedef QMap< int, QGuardedPtr<KexiDialogBase> > KexiDialogDict; 

//! @internal
class KexiMainWindowImpl::Private
{
public:
	Private(KexiMainWindowImpl* w)
//		: dialogs(401)
		: wnd(w)
		, m_openedCustomObjectsForItem(1019, true)
	{
		propEditor=0;
		propEditorToolWindow=0;
		propEditorTabWidget=0;
		userMode = false;
		nav=0;
		navToolWindow=0;
		prj = 0;
		curDialogGUIClient=0;
		curDialogViewGUIClient=0;
		closedDialogGUIClient=0;
		closedDialogViewGUIClient=0;
		nameDialog=0;
		curDialog=0;
		block_KMdiMainFrm_eventFilter=false;
		focus_before_popup=0;
//		relationPart=0;
		privateIDCounter=0;
		action_view_nav=0;
		action_view_propeditor=0;
		action_view_mainarea=0;
		action_open_recent_projects_title_id = -1;
		action_open_recent_connections_title_id = -1;
		forceDialogClosing=false;
		insideCloseDialog=false;
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
		dialogExistedBeforeCloseProject = false;
#ifndef KEXI_SHOW_UNIMPLEMENTED
		dummy_action = new KActionMenu("", wnd);
#endif
		maximizeFirstOpenedChildFrm = false;
#ifdef HAVE_KNEWSTUFF
		newStuff = 0;
#endif
		mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
		forceShowProjectNavigatorOnCreation = false;
		forceHideProjectNavigatorOnCreation = false;
		navWasVisibleBeforeProjectClosing = false;
		saveSettingsForShowProjectNavigator = true;
		m_openedCustomObjectsForItem.setAutoDelete(true);
	}
	~Private() {
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	//! Job type. Currently used for marking items as being opened or closed.
	enum PendingJobType {
		NoJob = 0,
		DialogOpeningJob,
		DialogClosingJob
	};

	KexiDialogBase *openedDialogFor( const KexiPart::Item* item, PendingJobType &pendingType )
	{
		return openedDialogFor( item->identifier(), pendingType );
	}

	KexiDialogBase *openedDialogFor( int identifier, PendingJobType &pendingType )
	{
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		QMap<int, PendingJobType>::ConstIterator it = pendingDialogs.find( identifier );
		if (it==pendingDialogs.constEnd())
			pendingType = NoJob;
		else
			pendingType = it.data();
		
		if (pendingType == DialogOpeningJob) {
			return 0;
		}
		return (KexiDialogBase*)dialogs[ identifier ];
	}
#else
	KexiDialogBase *openedDialogFor( const KexiPart::Item* item )
	{
		return openedDialogFor( item->identifier() );
	}

	KexiDialogBase *openedDialogFor( int identifier )
	{
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		return (KexiDialogBase*)dialogs[ identifier ];
	}
#endif

	void insertDialog(KexiDialogBase *dlg) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		dialogs.insert(dlg->id(), QGuardedPtr<KexiDialogBase>(dlg));
#ifndef KEXI_NO_PENDING_DIALOGS
		pendingDialogs.remove(dlg->id());
#endif
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	void addItemToPendingDialogs(const KexiPart::Item* item, PendingJobType jobType) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		pendingDialogs.replace( item->identifier(), jobType );
	}

	bool pendingDialogsExist() {
		if (pendingDialogs.constBegin()!=pendingDialogs.constEnd())
			kdDebug() << 	pendingDialogs.constBegin().key() << " " << (int)pendingDialogs.constBegin().data() << endl;
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		return !pendingDialogs.isEmpty();
	}
#endif

	void updateDialogId(KexiDialogBase *dlg, int oldItemID) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		dialogs.remove(oldItemID);
#ifndef KEXI_NO_PENDING_DIALOGS
		pendingDialogs.remove(oldItemID);
#endif
		dialogs.insert(dlg->id(), QGuardedPtr<KexiDialogBase>(dlg));
	}

	void removeDialog(int identifier) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		dialogs.remove(identifier);
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	void removePendingDialog(int identifier) {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		pendingDialogs.remove(identifier);
	}
#endif

	uint openedDialogsCount() {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		return dialogs.count();
	}

	//! Used in KexiMainWindowImple::closeProject()
	void clearDialogs() {
//todo(threads)		QMutexLocker dialogsLocker( &dialogsMutex );
		dialogs.clear();
#ifndef KEXI_NO_PENDING_DIALOGS
		pendingDialogs.clear();
#endif
	}

	/*! Toggles last checked view mode radio action, if available. */
	void toggleLastCheckedMode()
	{
		if (curDialog.isNull())
			return;
		KRadioAction *ra = actions_for_view_modes[ curDialog->currentViewMode() ];
		if (ra)
			ra->setChecked(true);
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
						propEditorDockSeparatorPos = config->readNumEntry("RightDockPosition", 80);
						ds->setSeparatorPos(propEditorDockSeparatorPos, true);
				}*/
			/*}
#endif
		}
	}*/

	void showStartProcessMsg(const QStringList& args)
	{
		wnd->showErrorMessage(i18n("Could not start %1 application.").arg(KEXI_APP_NAME),
			i18n("Command \"%1\" failed.").arg(args.join(" ")));
	}

	void hideMenuItem(const QString& menuName, const QString& itemText, bool alsoSeparator)
	{
		QPopupMenu *pm = popups[menuName.ascii()];
		if (!pm)
			return;
		uint i=0;
		const uint c = pm->count();
		for (;i<c;i++) {
			kdDebug() << pm->text( pm->idAt(i) ) <<endl;
			if (pm->text( pm->idAt(i) ).lower().stripWhiteSpace()==itemText.lower().stripWhiteSpace())
				break;
		}
		if (i<c) {
			pm->setItemVisible( pm->idAt(i), false );
			if (alsoSeparator)
				pm->setItemVisible( pm->idAt(i+1), false ); //also separator
		}
	}
	
	void disableMenuItem(const QString& menuName, const QString& itemText)
	{
		QPopupMenu *pm = popups[menuName.ascii()];
		if (!pm)
			return;
		uint i=0;
		const uint c = pm->count();
		for (;i<c;i++) {
			if (pm->text( pm->idAt(i) ).lower().stripWhiteSpace()==itemText.lower().stripWhiteSpace())
				break;
		}
		if (i<c)
			pm->setItemEnabled( pm->idAt(i), false );
	}

	void updatePropEditorVisibility(int viewMode)
	{
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
				ds->setSeparatorPosInPercent(config->readNumEntry("RightDockPosition", 80));//%
#endif*/
			}
		}
	}

	void restoreNavigatorWidth()
	{
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
						QMAX(QMAX( config->readNumEntry("LeftDockPositionWithAutoOpen",20),
						config->readNumEntry("LeftDockPosition",20)),20)
					);
				else
					ds->setSeparatorPosInPercent(
					QMAX(20, config->readNumEntry("LeftDockPosition", 20/* % */)));

	//			dw->resize( d->config->readNumEntry("LeftDockPosition", 115/* % */), dw->height() );
# else
				//there were problems on KDE < 3.4
				ds->setSeparatorPosInPercent( 20 );
# endif
				//if (!wasAutoOpen) //(dw2->isVisible())
//					ds->setSeparatorPos( ds->separatorPos(), true );
			}
#endif
	}

	template<class type>
	type *openedCustomObjectsForItem(KexiPart::Item* item, const char* name)
	{
		if (!item || !name) {
			kdWarning() << 
				"KexiMainWindowImpl::Private::openedCustomObjectsForItem(): !item || !name" << endl;
			return 0;
		}
		QString key( QString::number(item->identifier()) + name );
		return dynamic_cast<type*>( m_openedCustomObjectsForItem.find( key.latin1() ) );
	}

	void addOpenedCustomObjectForItem(KexiPart::Item* item, QObject* object, const char* name)
	{
		QString key = QString::number(item->identifier()) + name;
		m_openedCustomObjectsForItem.insert( key.latin1(), object );
	}

		KexiMainWindowImpl *wnd;
		KexiStatusBar *statusBar;
		KexiProject *prj;
		KConfig *config;
#ifndef KEXI_NO_CTXT_HELP
		KexiContextHelp *ctxHelp;
#endif
		KexiBrowser *nav;
		KTabWidget *propEditorTabWidget;
		//! poits to kexi part which has been previously used to setup proppanel's tabs using 
		//! KexiPart::setupCustomPropertyPanelTabs(), in updateCustomPropertyPanelTabs().
		QGuardedPtr<KexiPart::Part> partForPreviouslySetupPropertyPanelTabs;
		QMap<KexiPart::Part*, int> recentlySelectedPropertyPanelPages;
		QGuardedPtr<KexiPropertyEditorView> propEditor;
		QGuardedPtr<KoProperty::Set> propBuffer;

		KXMLGUIClient *curDialogGUIClient, *curDialogViewGUIClient,
			*closedDialogGUIClient, *closedDialogViewGUIClient;
		QGuardedPtr<KexiDialogBase> curDialog;

		KexiNameDialog *nameDialog;

		QTimer timer; //helper timer
//		QSignalMapper *actionMapper;

		QAsciiDict<QPopupMenu> popups; //list of menu popups
		QPopupMenu *createMenu;

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
			*action_edit_select_all,
			*action_edit_undo, *action_edit_redo,
			*action_edit_insert_empty_row,
			*action_edit_edititem, *action_edit_clear_table,
			*action_edit_paste_special_data_table,
			*action_edit_copy_special_data_table;

		//! view menu
		KAction *action_view_nav, *action_view_propeditor, *action_view_mainarea;
		KRadioAction *action_view_data_mode, *action_view_design_mode, *action_view_text_mode;
		QIntDict<KRadioAction> actions_for_view_modes;
//		KRadioAction *last_checked_mode;
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
		KMdiToolViewAccessor* navToolWindow;
		KMdiToolViewAccessor* propEditorToolWindow;

		QGuardedPtr<QWidget> focus_before_popup;
//		KexiRelationPart *relationPart;

		int privateIDCounter; //!< counter: ID for private "document" like Relations window

		bool block_KMdiMainFrm_eventFilter : 1;

		//! Set to true only in destructor, used by closeDialog() to know if
		//! user can cancel dialog closing. If true user even doesn't see any messages
		//! before closing a dialog. This is for extremely sanity... and shouldn't be even needed.
		bool forceDialogClosing : 1;

		//! Indicates that we're inside closeDialog() method - to avoid inf. recursion
		//! on dialog removing
		bool insideCloseDialog : 1;

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

		//! Used for delayed dialogs closing for 'close all'
		QPtrList<KexiDialogBase> windowsToClose;

		//! Opened page setup dialogs, used by printOrPrintPreviewForItem().
		QIntDict<KexiDialogBase> pageSetupDialogs;

		/*! A map from Kexi dialog to "print setup" part item's ID of the data item
		 used by closeDialog() to find an ID of the data item, so the entry 
		 can be removed from pageSetupDialogs dictionary. */
		QMap<int, int> pageSetupDialogItemID2dataItemID_map;

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
		QAsciiDict<QObject> m_openedCustomObjectsForItem;

	int propEditorDockSeparatorPos, navDockSeparatorPos;
//	int navDockSeparatorPosWithAutoOpen;
	bool wasAutoOpen;
	bool dialogExistedBeforeCloseProject;

	KMdi::MdiMode mdiModeToSwitchAfterRestart;

protected:
	//! @todo move to KexiProject
	KexiDialogDict dialogs;
#ifndef KEXI_NO_PROCESS_EVENTS
	QMap<int, PendingJobType> pendingDialogs; //!< part item identifiers for dialogs whoose opening has been started
//todo(threads)	QMutex dialogsMutex; //!< used for locking dialogs and pendingDialogs dicts
#endif
};
