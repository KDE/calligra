/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

/* this class is the class, wich holds all the
   widgets wich displays the content 		*/

#ifndef KEXITABBROWSER_H
#define KEXITABBROWSER_H

#include <qwidget.h>
#include <qobject.h>
#include <qintdict.h>

#include <kexidialogbase.h>
#include <ktoolbar.h>


class QWidgetStack;
class KMultiTabBar;
class KexiBrowser;
class KexiBrowserItem;

class KexiTabBrowser : public KexiDialogBase
//class KexiTabBrowser : public QDockWindow 
{
	Q_OBJECT

	public:
		KexiTabBrowser(KexiView *view,QWidget *parent=0, const char *name=0);
		~KexiTabBrowser();

		virtual KXMLGUIClient *guiClient();

	protected:
		void			addBrowser(KexiBrowser *browser, QPixmap icon,QString text);
		KexiProject*    kexiProject() const {return m_project;};

		void			generateTables();
		void			generateQueries();

		KMultiTabBar		*m_tabBar;
		QWidgetStack		*m_stack;
		QIntDict<QWidget>	m_browserDict;

		int			m_tabs;
		int			m_activeTab;

		KexiBrowser		*m_db;
		KexiBrowser		*m_tables;
		KexiBrowser		*m_queries;
		KexiBrowser		*m_forms;
		KexiBrowser		*m_reports;
		KexiBrowserItem	*m_dbTables;
		KexiBrowserItem	*m_dbQueries;
		KexiBrowserItem	*m_dbForms;
		KexiBrowserItem	*m_dbReports;

		KexiProject     *m_project;

	protected slots:
		void			slotTabActivated(int id);
		void			slotUpdateBrowsers();
};

#endif
