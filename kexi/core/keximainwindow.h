/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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
#include <kmdidefines.h>

class KexiProject;
class KexiBrowser;
class KMdiChildView;
class KexiDialogBase;

/**
 * @short Kexi's main window
 */
class KexiMainWindow : public KMdiMainFrm
{
	Q_OBJECT

	public:
		/**
		 * creates a emtpy mainwindow
		 */
		KexiMainWindow();
		~KexiMainWindow();

		KexiProject	*project() { return m_project; }

		/**
		 * registers a dialog for watching and adds it to the view
		 */
		void		registerChild(KexiDialogBase *dlg);


	protected:
		//reimplementation of events
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

	protected slots:

		/**
		 * parsers command line options and checks if we should open the startupdlg
		 * or a file direclty
		 */
		void		parseCmdLineOptions();

		/**
		 * creates browser and fills it with empty items
		 */
		void		initBrowser();

		/**
		 * this slot is called if a window changes
		 */
		void		activeWindowChanged(KMdiChildView *dlg);

	private:
		KexiProject	*m_project;
		KexiBrowser	*m_browser;
};

#endif

