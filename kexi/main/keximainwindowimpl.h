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

#include "core/keximainwindow.h"

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
class KEXIMAIN_EXPORT KexiMainWindowImpl : public KexiMainWindow
{
	Q_OBJECT

	public:
		/**
		 * creates an empty mainwindow
		 */
		KexiMainWindowImpl();
		virtual ~KexiMainWindowImpl();

		//! Project data of currently opened project or NULL if no project here yet.
		virtual KexiProject *project();

		/**
		 * registers a dialog for watching and adds it to the view
		 */
		virtual void registerChild(KexiDialogBase *dlg);

		/**
		 * activates a window by it's document identifier
		 * @returns false if doc couldn't be raised or isn't opened
		 */
		bool activateWindow(int id);

		/*! Like above, using \a dlg passed explicity. Above method just calls this one. */
		bool activateWindow(KexiDialogBase *dlg);

		void startup(KexiProjectData* pdata);

		virtual bool eventFilter( QObject *obj, QEvent * e );

		virtual QPopupMenu* findPopupMenu(const char *popupName);

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

		/*! Closes dialog \a dlg. If dialog's data (see KexiDialoBase::dirty()) is unsaved,
		 used will be asked if saving should be perforemed.
		 \return true on successull closing or false on closing error.
		 If closing was cancelled by user, true is returned and cancelled is true. */
		bool closeDialog(KexiDialogBase *dlg, bool &cancelled, bool layoutTaskBar = true);

		virtual void detachWindow(KMdiChildView *pWnd,bool bShow=true);
		virtual void attachWindow(KMdiChildView *pWnd,bool bShow=true,bool bAutomaticResize=false);

		//! Opens object pointed by \a item in a view \a viewMode
		virtual KexiDialogBase * openObject(KexiPart::Item *item, int viewMode = Kexi::DataViewMode);

		//! For convenience
		virtual KexiDialogBase * openObject(const QCString& mime, const QString& name, int viewMode = Kexi::DataViewMode);

		/*! Saves dialog's \a dlg data. It dialog's data is never saved,
		 User is asked for name and caption, before saving.
		 \return true on successul closing or false on saving error.
		 If saving was cancelled by user, true is returned and cancelled is true.
		 \a messageWhenAskingForName is a i18n'ed text that will be visible
		 within name/caption dialog (see KexiNameDialog), which is popped 
		 up for never saved objects. */
		virtual bool saveObject( KexiDialogBase *dlg, bool &cancelled,
			const QString& messageWhenAskingForName = QString::null );

	protected:
		/**
		 * Creates navigator (if it's not yet created),
		 * lookups items for current project and fills the nav. with not-opened items
		 */
		void initNavigator();
		
		void initContextHelp();
		
		void initPropertyEditor();
		
		//! reimplementation of events
//		virtual void	closeEvent(QCloseEvent *);

		/**
		 * creates standard actions like new, open, save ...
		 */
		void		initActions();
		
		/**
		 * sets up the window from user settings (e.g. mdi mode)
		 */
		void		restoreSettings();

		/**
		 * writes user settings back
		 */
		void		storeSettings();
		
		/** Invalidates availability of all actions for current application state.
		*/
		void invalidateActions();

		/** Invalidates action availability for current application state.
		 These actions are dependent on curently selected dialog.
		*/
		void invalidateSharedActions(QWidget *w);

		/** Invalidates action availability for current application state.
		 These actions are only dependent on project availbility, not on curently selected dialog.
		*/
		void invalidateProjectWideActions();

		void invalidateViewModeActions();

		/*! Opens project pointed by \a projectData, \return true on success.
		 Application state (e.g. actions) is updated. */
		bool openProject(KexiProjectData *projectData);
		
		/*! Allows user to select a project with KexiProjectSelectorDialog.
			\return selected project's data or NULL if dialog was cancelled.
		*/
		KexiProjectData* selectProject(KexiDB::ConnectionData *cdata);
		
		/*! Closes current project, \return true on success.
		 Application state (e.g. actions) is updated. 
		 \return true on success. 
		 If closing was cancelled by user, true is returned and cancelled is true. */
		bool closeProject(bool &cancelled);
		
		/*! Shows dialog for creating new blank database,
		 ans creates one. Dialog is not shown if option for automatic creation 
		 is checkekd.
		 \return true if database was created, false on error or when cancel pressed
		*/
		bool createBlankDatabase();

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

	protected slots:

		/**
		 * parsers command line options and checks if we should open the startupdlg
		 * or a file direclty
		 */
//js		void		parseCmdLineOptions();

		/**
		 * this slot is called if a window changes
		 */
		void activeWindowChanged(KMdiChildView *dlg);

		/**
		 * this slot is called if a window gets colsed
		 * and will unregister stuff
		 */
		void childClosed(KMdiChildView *dlg);

		void slotPartLoaded(KexiPart::Part* p);

		void slotCaptionForCurrentMDIChild(bool childrenMaximized);
		void slotNoMaximizedChildFrmLeft(KMdiChildFrm*);
		void slotLastChildFrmClosed();
		void slotChildViewIsDetachedNow(QWidget*);

		//! Shows an error message signaled by project's objects, connections, etc.
		void showErrorMessage(const QString&,KexiDB::Object *obj);
		void showErrorMessage(const QString &title, const QString &details = QString::null);

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
		 Asks for confirmation. \return true on success. 
		 if \a dontAsk is true. */
		bool removeObject( KexiPart::Item *item, bool dontAsk = true );

		void invalidateSharedActions();
		void invalidateSharedActionsLater();

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
		void slotQuit();

		//! Shows tipOfTheDay dialog, is \a onStartup is false, it's always shown
		void tipOfTheDay(bool onStartup);
		void slotTipOfTheDayAction(); //!< internal; calls tipOfTheDay(false)

		void slotImportFile();
		void slotImportServer();

		//! There are performed all actions that need to be done immediately after  ctro (using timer)
		void slotLastActions();

		virtual void propertyBufferSwitched(KexiDialogBase *dlg, bool force=false);

		/*! Handles changes in 'dirty' flag for dialogs. */
		void slotDirtyFlagChanged(KexiDialogBase*);

		void slotMdiModeHasBeenChangedTo(KMdi::MdiMode);

	private:

		class Private;
		Private *d;

	friend class KexiDialogBase;	
};

#endif

