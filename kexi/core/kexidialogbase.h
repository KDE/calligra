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

class KexiView;
class KexiProject;

class KPrinter;

typedef QPtrList<QWidget> Widgets;

class KexiDialogBase : public QWidget
{
	Q_OBJECT

	public:

		KexiDialogBase(KexiView *view,QWidget *parent, const char *name);
		~KexiDialogBase();

		virtual KXMLGUIClient *guiClient()=0;
		virtual void activateActions();
		virtual void deactivateActions();
		KexiView *mainWindow()const{return m_mainWindow;};

		static KexiDialogBase *activeDocumentView() { return s_activeDocumentWindow;};
		KexiProject *kexiProject()const;
		KexiView *kexiView()const;
		bool isRegistering(){return m_registering;}

		void	setContextHelp(const QString &title, const QString &message);

		virtual void print(KPrinter &printer) {; }

		void aboutToShow();
		void aboutToHide();

	signals:
		void closing(KexiDialogBase *);

	protected:
		virtual void focusInEvent ( QFocusEvent *);
		enum WindowType {ToolWindow, DocumentWindow};
		void registerAs(KexiDialogBase::WindowType wt, const QString &identifier=QString::null);
		void registerChild(QWidget *w);
		void closeEvent(QCloseEvent *ev);
		virtual void finishUpForClosing(){;}
		static KexiDialogBase *s_activeDocumentWindow;
		static KexiDialogBase *s_activeToolWindow;
		static QPtrList<KexiDialogBase> *s_DocumentWindows;
		static QPtrList<KexiDialogBase> *s_ToolWindows;

		class KDockWidget *myDock;
	private:
		class	KexiView *m_mainWindow;
		class	KexiProject *m_project;
		class	KexiView *m_view;
		class	QDockWindow *w;
		bool	m_registered;
		enum	WindowType m_wt;
		bool	m_registering;
		QString	m_contextTitle;
		QString	m_contextMessage;
		Widgets	m_widgets;
		QString	m_identifier;
};

#endif
