/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIWORKSPACE_TabbedMDI_H
#define KEXIWORKSPACE_TabbedMDI_H

#include <qguardedptr.h>

#include <kdeversion.h>

#if KDE_IS_VERSION(3,1,9)
#include <ktabwidget.h>
#else
#include <qtabwidget.h>
#define KTabWidget QTabWidget
#endif

#include "kexiworkspace.h"

class KexiDialogBase;
class KexiView;


class KEXICORE_EXPORT KexiWorkspaceTabbedMDI : public KTabWidget, public KexiWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspaceTabbedMDI(QWidget *parent=0, const char *name=0, KexiView* mw=0);
		virtual ~KexiWorkspaceTabbedMDI();
		
		virtual void addItem(KexiDialogBase *newItem);
		
		virtual unsigned int count() { return no; };
                virtual KexiDialogBase * activeDocumentView();		
		virtual void activateView(KexiDialogBase *kdb);
	protected:
		unsigned int no;

	protected slots:
		virtual void takeItem(KexiDialogBase *delItem);
		virtual void slotWindowActivated(QWidget*);
		void slotCloseRequest( QWidget * );
		void slotCloseCurrent();
	private:
	QGuardedPtr<KexiDialogBase> m_activeDialog;
	KexiView *m_mainwindow;
	class QToolButton *m_closeButton;
};

#endif
