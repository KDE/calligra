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


//#include <kmdidefines.h>

class KexiProject;
class KexiProjectData;
class KexiBrowser;
class KMdiChildView;
class KexiDialogBase;
class KToggleAction;
namespace KexiDB {
	class Object;
	class ConnectionData;
}

typedef QIntDict<KexiDialogBase> WidgetDict;

/**
 * @short Kexi's main window
 */
class KexiMainWindow : public KMdiMainFrm
{
	Q_OBJECT

	public:
		/**
		 * creates an empty mainwindow
		 */
		KexiMainWindow();
		virtual ~KexiMainWindow();

		//! Project data of currently opened project or NULL if no project here yet.
		KexiProject	*project() { return m_project; }

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
		
		/** Invalidates action availability for current appliacation state.
		*/
		void		invalidateActions();

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

	protected slots:

		/**
		 * parsers command line options and checks if we should open the startupdlg
		 * or a file direclty
		 */
//js		void		parseCmdLineOptions();

		/**
		 * creates browser and fills it with empty items
		 */
		void		initBrowser();

		/**
		 * this slot is called if a window changes
		 */
		void		activeWindowChanged(KMdiChildView *dlg);

		//! Shows an error message signaled by project's objects, connections, etc.
		void slotShowErrorMessageFor(const QString&,KexiDB::Object *obj);

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
		void slotQuit();

	private:
		KexiProject	*m_project;
		KexiBrowser	*m_browser;
		WidgetDict	m_docs;

		KXMLGUIClient   *m_currentDocumentGUIClient;

		class Private;
		Private *d;

	friend class KexiDialogBase;	
};

#endif

