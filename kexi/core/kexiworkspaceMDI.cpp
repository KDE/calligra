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

#include <kdebug.h>

#include "kexidialogbase.h"
#include "kexiworkspaceMDI.h"
#include "kexiview.h"
#include "kexiproject.h"
#include "kexicontexthelp.h"

KexiWorkspaceMDI::KexiWorkspaceMDI(QWidget *parent, const char *name,KexiView *mw) 
	: QWorkspace(parent, name),KexiWorkspace()
{
	m_mainwindow=mw;
	no = 0;
	m_activeDialog=0;
	connect(this,SIGNAL(windowActivated(QWidget*)),this,SLOT(slotWindowActivated(QWidget*)));
}

void KexiWorkspaceMDI::addItem(KexiDialogBase *newItem)
{
	no++;
	connect(newItem, SIGNAL(closing(KexiDialogBase *)), this, SLOT(takeItem(KexiDialogBase *)));
	kdDebug() << "item added; realnumber now: " << no << endl;
}

void KexiWorkspaceMDI::takeItem(KexiDialogBase *delItem)
{
	no--;
	kdDebug() << "took item; realnumber now" << no << endl;
}


void KexiWorkspaceMDI::slotWindowActivated(QWidget* w)
{
	KexiDialogBase *tmp=static_cast<KexiDialogBase*>(w);
	if (tmp!=0)
		if (tmp->isRegistering()) return;
	if (!m_mainwindow->project()->isReadWrite()) return;
	kdDebug() << "KexiWorkspace::slotWindowActivated()" << endl;
	if (!m_mainwindow) return;
	setCaption("");
	if (m_activeDialog==w) return;
	KexiDialogBase *olddialog=m_activeDialog;
	m_activeDialog=static_cast<KexiDialogBase*>(w);
	if (olddialog)
	{
		olddialog->aboutToHide();
		if (m_activeDialog.isNull())
		{
			olddialog->deactivateActions();
			if (olddialog->guiClient()->factory()==m_mainwindow->factory())
				m_mainwindow->factory()->removeClient(olddialog->guiClient());
		}
		else
		{
			if (m_activeDialog->guiClient()!=olddialog->guiClient())
			{
				olddialog->deactivateActions();
				if (olddialog->guiClient()->factory()==m_mainwindow->factory())
					m_mainwindow->factory()->removeClient(olddialog->guiClient());
				m_mainwindow->factory()->addClient(m_activeDialog->guiClient());
			        if(m_mainwindow->help()) {
			 	       m_mainwindow->help()->setContextHelp(
						m_activeDialog->contextHelpTitle(),
						m_activeDialog->contextHelpMessage());
				}

				m_activeDialog->activateActions();
				m_activeDialog->aboutToShow();

			}
			else
			{
				olddialog->deactivateActions();
			        if(m_mainwindow->help()) {
			 	       m_mainwindow->help()->setContextHelp(
						m_activeDialog->contextHelpTitle(),
						m_activeDialog->contextHelpMessage());
				}

				m_activeDialog->activateActions();
				m_activeDialog->aboutToShow();
			}
		}

	}
	else
	if (!m_activeDialog.isNull())
	{
		m_mainwindow->factory()->addClient(m_activeDialog->guiClient());
		if(m_mainwindow->help()) {
			m_mainwindow->help()->setContextHelp(
			m_activeDialog->contextHelpTitle(),
			m_activeDialog->contextHelpMessage());
		}

		m_activeDialog->activateActions();
		m_activeDialog->aboutToShow();
	}
}

KexiDialogBase *
KexiWorkspaceMDI::activeDocumentView()
{
	return static_cast<KexiDialogBase*>(activeWindow());
}

void
KexiWorkspaceMDI::activateView(KexiDialogBase *kdb)
{
	if(!kdb)
		return;

	slotWindowActivated(kdb);
	if (kdb->isMaximized())
		kdb->showMaximized();
	else //normal or minimized
		kdb->showNormal();
	kdb->setFocus();
}

KexiWorkspaceMDI::~KexiWorkspaceMDI()
{
}

#include "kexiworkspaceMDI.moc"
