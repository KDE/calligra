/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIMAINWINDOWIMPL_H
#define KEXIMAINWINDOWIMPL_H

#include <kmessagebox.h>
#include "core/keximainwindow.h"
#include "core/kexiguimsghandler.h"

class KexiProjectData;
class KexiActionProxy;
class KMdiChildView;

namespace KexiDB {
	class Object;
	class ConnectionData;
}
namespace KexiPart {
	class Info;
	class Part;
}

/**
 * @short Kexi's main window implementation
 */
class KEXIMAIN_EXPORT KexiMainWindowImpl : public KexiMainWindow, public KexiGUIMessageHandler
{
	Q_OBJECT

	public:
		/*! Creates an empty mainwindow. */
		KexiMainWindowImpl();
		virtual ~KexiMainWindowImpl();

		//! Project data of currently opened project or NULL if no project here yet.
		virtual KexiProject *project();

		/*! Registers dialog \a dlg for watching and adds it to the main window's stack. */
		virtual void registerChild(KexiDialogBase *dlg);

		/*! Activates a window by it's document identifier.
		 \return false if doc couldn't be raised or isn't opened. */
		bool activateWindow(int id);

		/*! Like above, using \a dlg passed explicity. Above method just calls this one. */
		bool activateWindow(KexiDialogBase *dlg);

//		void startup(KexiProjectData* pdata);
		/*! Performs startup actions. \return false if application should exit immediately
		 with an error status. */
		tristate startup();

		/*! \return true if this window is in the Final Mode. */
		bool inFinalMode() const;

		virtual bool eventFilter( QObject *obj, QEvent * e );

		virtual QPopupMenu* findPopupMenu(const char *popupName);

		/*! Implemented for KexiMainWindow. */
		virtual KActionPtrList allActions() const;

		/**
		 * @returns a pointer to the relation parts loads it if needed
		 */
//		KexiRelationPart	*relationPart();

//TODO: move to kexiproject
		/*! Generates ID for private "document" like Relations window.
		 Private IDs are negative numbers (while ID regular part instance's IDs are >0)
		 Private means that the object is not stored as-is in the project but is somewhat
		 generated and in most cases there is at most one unique instance document of such type (part).
		 To generate this ID, just app-wide internal counter is used. */
		virtual int generatePrivateID();

		/*! Reimplemented */
		virtual void readProperties(KConfig *config);
    	virtual void saveProperties(KConfig *config);
    	virtual void saveGlobalProperties( KConfig* sessionConfig );

	public slots:
		/*! Inherited from KMdiMainFrm: we need to do some tasks before child is closed.
			Just calls closeDialog(). Use closeDialog() if you need, not this one. */
		virtual void closeWindow(KMdiChildView *pWnd, bool layoutTaskBar = true);

		/*! Reimplemented for internal reasons. */
		virtual void addWindow( KMdiChildView* pView, int flags = KMdi::StandardAdd );

		/*! Implemented for KexiMainWindow */
		virtual tristate closeDialog(KexiDialogBase *dlg);

		/*! Implemented for KexiMainWindow */
		tristate closeDialog(KexiDialogBase *dlg, bool layoutTaskBar);

		virtual void detachWindow(KMdiChildView *pWnd,bool bShow=true);
		virtual void attachWindow(KMdiChildView *pWnd,bool bShow=true,bool bAutomaticResize=false);

		//! Opens object pointed by \a item in a view \a viewMode
		virtual KexiDialogBase * openObject(KexiPart::Item *item, int viewMode = Kexi::DataViewMode);

		//! For convenience
		virtual KexiDialogBase * openObject(const QCString& mime, const QString& name, int viewMode = Kexi::DataViewMode);

		/*! Implemented for KexiMainWindow */
		virtual tristate saveObject( KexiDialogBase *dlg,
			const QString& messageWhenAskingForName = QString::null );

		/*! Implemented for KexiMainWindow */
		virtual tristate getNewObjectInfo( KexiPart::Item *partItem, KexiPart::Part *part, 
			bool& allowOverwriting, const QString& messageWhenAskingForName = QString::null );

		/*! Implemented for KexiMainWindow */
		virtual void highlightObject(const QCString& mime, const QCString& name);

	protected:
		/*! Initialises final mode: constructs window according to kexi__final database
		 and loads the specified part.
		 \return true on success or false if e.g. kexi__final does not exist
		 or a fatal exception happened */
		bool initFinalMode(KexiProjectData *projectData);

		/*!
		 Creates navigator (if it's not yet created),
		 lookups items for current project and fills the nav. with not-opened items
		 */
		void initNavigator();

		void initContextHelp();

		void initPropertyEditor();

		//! reimplementation of events
//		virtual void	closeEvent(QCloseEvent *);

		/*! Creates standard actions like new, open, save ... */
		void		initActions();

		/*! Creates user project-wide actions */
		void		initUserActions();

		/*! Sets up the window from user settings (e.g. mdi mode). */
		void		restoreSettings();

		/*! Writes user settings back. */
		void		storeSettings();

		/*! Invalidates availability of all actions for current application state. */
		void invalidateActions();

		/*! Invalidates action availability for current application state.
		 These actions are dependent on curently selected dialog. */
		virtual void invalidateSharedActions(QObject *o);

		/*! Invalidates action availability for current application state.
		 These actions only depend on project availability, not on curently selected dialog. */
		void invalidateProjectWideActions();

		/*! Invalidates action availability for current application state.
		 These actions only depend on curently selected dialog and currently selected view
		 (KexiViewBase derived object) within this dialog. */
		void invalidateViewModeActions();

		/*! Opens project pointed by \a projectData, \return true on success.
		 Application state (e.g. actions) is updated. */
		bool openProject(KexiProjectData *projectData);

		/*! Closes current project, \return true on success.
		 Application state (e.g. actions) is updated.
		 \return true on success.
		 If closing was cancelled by user, cancelled is returned. */
		tristate closeProject();

		/*! Shows dialog for creating new blank project,
		 and creates one. Dialog is not shown if option for automatic creation
		 is checked or Kexi::startupHandler().projectData() was provided from command line.
		 \a cancelled is set to true if creation has been cancelled (e.g. user answered
		 no when asked for database overwriting, etc.
		 \return true if database was created, false on error or when cancel was pressed */
		tristate createBlankProject();

		/*! Shows dialog for creating new blank project,
		 and return a data describing it. It the dialog was cancelled,
		 \a cancelled will be set to true (false otherwise). */
		KexiProjectData* createBlankProjectData(bool &cancelled, bool confirmOverwrites = true);

		void setWindowMenu(QPopupMenu *menu);

		/*! \return focused kexi window (KexiDialogBase or KexiDockBase subclass) */
//		QWidget* focusWindow() const;

		/*! Reimplemented from KexiSharedActionHost:
		 accepts only KexiDockBase and KexiDialogBase subclasses.  */
		virtual bool acceptsSharedActions(QObject *w);

		/*! Performs lookup like in KexiSharedActionHost::focusWindow()
		 but starting from \a w instead of a widget returned by QWidget::focusWidget().
		 \return NULL if no widget matches acceptsSharedActions() or if \a w is NULL. */
		QWidget* findWindow(QWidget *w);

		/*! Updates application's caption - also shows project's name. */
		void updateAppCaption();

		void restoreWindowConfiguration(KConfig *config);
		void storeWindowConfiguration(KConfig *config);

		virtual bool queryClose();
		virtual bool queryExit();

		/*! Helper: switches to view \a mode. */
		bool switchToViewMode(int viewMode);

		/*! Helper. Removes and/or adds GUI client for current dialog's view;
		 on switching to other dialog (activeWindowChanged())
		 or on switching to other view within the same dialog (switchToViewMode()). */
		void updateDialogViewGUIClient(KXMLGUIClient *viewClient);

		/*! Helper. Updates setup of property panel's tabs. Used when switching
		 from \a prevDialog dialog to a current dialog.	*/
		void updateCustomPropertyPanelTabs(KexiDialogBase *prevDialog, int prevViewMode);

		/*! @overload void updateCustomPropertyPanelTabs(KexiDialogBase *prevDialog, int prevViewMode) */
		void updateCustomPropertyPanelTabs(
			KexiPart::Part *prevDialogPart, int prevViewMode, KexiPart::Part *curDialogPart, int curViewMode );

	protected slots:

		/*! Called once after timeout (after ctors are executed). */
		void slotAutoOpenObjectsLater();

		/*! This slot is called if a window changes */
		void activeWindowChanged(KMdiChildView *dlg);

		/*! Tthis slot is called if a window gets colsed and will unregister stuff */
		void childClosed(KMdiChildView *dlg);

		void slotPartLoaded(KexiPart::Part* p);

		void slotCaptionForCurrentMDIChild(bool childrenMaximized);
		void slotNoMaximizedChildFrmLeft(KMdiChildFrm*);
		void slotLastChildViewClosed();
		void slotChildViewIsDetachedNow(QWidget*);

		//! internal - creates and initializes kexi project
		void createKexiProject(KexiProjectData* new_data);

		/*! this slot handles event when user double clicked (or single -depending on settings)
		 or pressed return ky on the part item in the navigator.
		 This differs from openObject() signal in that if the object is already opened
		 in view mode other than \a viewMode, the mode is not changed.
		 \sa KexiBrowser::openOrActivateItem() */
		KexiDialogBase * openObjectFromNavigator(KexiPart::Item* item, int viewMode);

		bool newObject( KexiPart::Info *info );

		/*! Removes object pointed by \a item from current project.
		 Asks for confirmation. \return true on success
		 or cancelled if removing was cancelled (only possible if \a dontAsk is false). */
		tristate removeObject( KexiPart::Item *item, bool dontAsk = false );

		/*! Renames object pointed by \a item to a new name \a _newName.
		 Sets \a success to false on failure. Used as a slot connected
		 to KexiBrowser::renameItem() signal. */
		void renameObject( KexiPart::Item *item, const QString& _newName, bool &succes );

		/*! Reaction for object rename (signalled by KexiProject).
		 If this item has opened dialog, it's caption is updated,
		 and also optionally application's caption. */
		virtual void slotObjectRenamed(const KexiPart::Item &item, const QCString& oldName);

		virtual void fillWindowMenu();

		void invalidateSharedActions();
		void invalidateSharedActionsLater();

		void slotEditPasteSpecialDataTable();
		void slotViewNavigator();
		void slotViewPropertyEditor();
		void slotViewDataMode();
		void slotViewDesignMode();
		void slotViewTextMode(); //!< sometimes called "SQL View"
		void slotShowSettings();
		void slotConfigureKeys();
		void slotConfigureToolbars();
		void slotProjectNew();
		void slotProjectOpen();
		void slotProjectOpenRecentAboutToShow();
		void slotProjectOpenRecent(int id);
		void slotProjectOpenRecentMore();
		void slotProjectSave();
		void slotProjectSaveAs();
		void slotProjectProperties();
		void slotProjectClose();
		void slotProjectRelations();
		void slotToolsProjectMigration();
		void slotToolsProjectImportDataTable();
		void slotQuit();
		/// TMP: Display a dialog to download db examples from internet
		void  slotGetNewStuff();

		void slotTipOfTheDay();

		//! Shows 'important info' dialog, is \a onStartup is false, it's always shown
		void importantInfo(bool onStartup);
		void slotImportantInfo(); //!< just importantInfo(false);
		void slotStartFeedbackAgent();

		void slotOptionsEnableForms(bool show, bool noMessage = false); //temp.

		void slotImportFile();
		void slotImportServer();

		//! There are performed all actions that need to be done immediately after  ctro (using timer)
		void slotLastActions();

		virtual void acceptPropertySetEditing();

		virtual void propertySetSwitched(KexiDialogBase *dlg, bool force=false,
			bool preservePrevSelection = true);

		/*! Handles changes in 'dirty' flag for dialogs. */
		void slotDirtyFlagChanged(KexiDialogBase*);

		void slotMdiModeHasBeenChangedTo(KMdi::MdiMode);

		//reimplemented to add "restart is required" message box
		virtual void switchToIDEAlMode();
		void switchToIDEAlMode(bool showMessage);
		virtual void switchToChildframeMode();
		void switchToChildframeMode(bool showMessage);

	private:

		class MessageHandler;
		class Private;
		Private *d;

	friend class KexiDialogBase;
};

#endif

