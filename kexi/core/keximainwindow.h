/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIMAINWINDOW_H
#define KEXIMAINWINDOW_H

#include <kmdimainfrm.h>
#include <qintdict.h>

class KexiProject;
class KexiProjectData;
class KexiBrowser;
class KexiActionProxy;
class KMdiChildView;
class KexiDialogBase;
class KToggleAction;
class KexiRelationPart;

namespace KexiDB {
	class Object;
	class ConnectionData;
}
namespace KexiPart {
	class Item;
	class Info;
	class Part;
}

/**
 * @short Kexi's main window
 */
class KEXICORE_EXPORT KexiMainWindow : public KMdiMainFrm
{
	Q_OBJECT

	public:
		/**
		 * creates an empty mainwindow
		 */
		KexiMainWindow();
		virtual ~KexiMainWindow();

		//! Project data of currently opened project or NULL if no project here yet.
		KexiProject	*project();

		/**
		 * registers a dialog for watching and adds it to the view
		 */
		void		registerChild(KexiDialogBase *dlg);

		/**
		 * activates a window by it's document identifier
		 * @returns false if doc couldn't be raised or isn't opened
		 */
		bool		activateWindow(int id);


		void startup(KexiProjectData* pdata);

		virtual bool eventFilter( QObject *obj, QEvent * e );

		void plugActionProxy(KexiActionProxy *proxy); //, const char *action_name);
		void updateActionAvailable(const char *action_name, bool set, QObject *obj);

		QPopupMenu* findPopupMenu(const char *popupName);

		/**
		 * @returns a pointer to the relation parts loads it if needed
		 */
		KexiRelationPart	*relationPart();

	public slots:
		/** Inherited from KMdiMainFrm: we need to do some tasks before child is closed */
		virtual void closeWindow(KMdiChildView *pWnd, bool layoutTaskBar = true); 

		virtual void detachWindow(KMdiChildView *pWnd,bool bShow=true);
		virtual void attachWindow(KMdiChildView *pWnd,bool bShow=true,bool bAutomaticResize=false);
	protected:
		//! reimplementation of events
		virtual void	closeEvent(QCloseEvent *);

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
		
		/*! Creates shared action. Action's data is owned by the main window. */
		KAction* createSharedAction(const QString &text, const QString &pix_name,
			const KShortcut &cut, const char *name);

		/*! Like above - creates shared action, but from standard action identified by \a id. 
		 Action's data is owned by the main window. */
		KAction* createSharedAction( KStdAction::StdAction id, const char *name);

		/*! Helper function for createSharedAction(). */
		KAction* createSharedActionInternal( KAction *action );

		/** Invalidates availability of all actions for current application state.
		*/
		void invalidateActions();

		/** Invalidates action availability for current application state.
		 These actions are dependent on curently selected dialog.
		*/
		void invalidateSharedActions(QWidget *w = 0);

		/** Invalidates action availability for current application state.
		 These actions are only dependent on project availbility, not on curently selected dialog.
		*/
		void invalidateProjectWideActions();

		/*! Opens project pointed by \a projectData, \return true on success.
		 Application state (e.g. actions) is updated. */
		bool openProject(KexiProjectData *projectData);
		
		/*! Allows user to select a project with KexiProjectSelectorDialog.
			\return selected project's data or NULL if dialog was cancelled.
		*/
		KexiProjectData* selectProject(KexiDB::ConnectionData *cdata);
		
		/*! Closes current project, \return true on success.
		 Application state (e.g. actions) is updated. */
		bool closeProject();
		
		/*! Shows dialog for creating new blank database,
		 ans creates one. Dialog is not shown if option for automatic creation 
		 is checkekd.
		 \return true is database was created, false on error or when cancel pressed
		*/
		bool createBlankDatabase();

		void setWindowMenu(QPopupMenu *menu);

		/*! \return focused kexi window (KexiDialogBase or KexiDockBase subclass) */
		QWidget* focusWindow() const;

		static bool isWindow(QObject *o);

		QWidget* findWindow(QWidget *w) const;

		/*! Updates application's caption - also shows project's name. */
		void updateAppCaption();

	protected slots:

		/**
		 * parsers command line options and checks if we should open the startupdlg
		 * or a file direclty
		 */
//js		void		parseCmdLineOptions();

		/**
		 * Creates navigator, lookups items for current project and fills the nav. with not-opened items
		 */
		void		initNavigator();
		void		initContextHelp();

		/**
		 * this slot is called if a window changes
		 */
		void		activeWindowChanged(KMdiChildView *dlg);

		/**
		 * this slot is called if a window gets colsed
		 * and will unregister stuff
		 */
		void		childClosed(KMdiChildView *dlg);

		bool openObject(KexiPart::Item *item, bool designMode = false);
		//! for convenience
		bool openObject(const QString& mime, const QString& name, bool designMode = false);
		bool newObject( KexiPart::Info *info );
		bool removeObject( KexiPart::Item *item );

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

		void setActionAvailable(const char *name, bool avail);
		void slotAction(const QString& act_id);

		void slotViewNavigator();
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

		void slotImportFile();

	private:

		class Private;
		Private *d;

	friend class KexiDialogBase;	
};

#endif

