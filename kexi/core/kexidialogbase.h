/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include <qwidget.h>
#include <kxmlguiclient.h>
#include <qptrlist.h>
class KexiDialogBase : public QWidget
{
	Q_OBJECT
	
	public:

		KexiDialogBase(QWidget *parent, const char *name);
		~KexiDialogBase();
	
		virtual KXMLGUIClient *guiClient()=0;
		virtual void activateActions();
		virtual void deactivateActions();
	signals:
		void closing(KexiDialogBase *);
	
	protected:
		enum WindowType {ToolWindow, DocumentWindow};
		void registerAs(KexiDialogBase::WindowType wt);
		void closeEvent(QCloseEvent *ev);

		static KexiDialogBase *s_activeDocumentWindow;
		static KexiDialogBase *s_activeToolWindow;
		static QPtrList<KexiDialogBase> *s_DocumentWindows;
		static QPtrList<KexiDialogBase> *s_ToolWindows;

		class KDockWidget *myDock;
	private:
		class KexiMainWindow *m_mainWindow;
};

#endif
