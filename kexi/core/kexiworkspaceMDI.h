/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIWORKSPACE_MDI_H
#define KEXIWORKSPACE_MDI_H

#include <qguardedptr.h>

#include <qworkspace.h>
#include "kexiworkspace.h"

class KexiDialogBase;
class KexiView;

class KexiWorkspaceMDI : public QWorkspace, public KexiWorkspace
{
	Q_OBJECT
	
	public:
		KexiWorkspaceMDI(QWidget *parent=0, const char *name=0, KexiView* mw=0);
		virtual ~KexiWorkspaceMDI();
		
		virtual void addItem(KexiDialogBase *newItem);
		
		virtual unsigned int count() { return no; };
//                virtual KexiDialogBase * activeDocumentView() {return m_activeDialog;};		
                virtual KexiDialogBase * activeDocumentView();		
		virtual void activateView(KexiDialogBase *kdb);
	protected:
		unsigned int no;

	protected slots:
		virtual void takeItem(KexiDialogBase *delItem);
		virtual void slotWindowActivated(QWidget*);
	private:
	QGuardedPtr<KexiDialogBase> m_activeDialog;
	KexiView *m_mainwindow;
};

#endif
