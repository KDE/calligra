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

#include <kdebug.h>

#include "kexidialogbase.h"
#include "kexiworkspaceTabbedMDI.h"
#include "kexiview.h"
#include "kexiproject.h"
#include "kexicontexthelp.h"
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qapplication.h>
#include <qevent.h>

KexiWorkspaceTabbedMDI::KexiWorkspaceTabbedMDI(QWidget *parent, const char *name,KexiView *mw) 
	: KTabWidget(parent, name),KexiWorkspace(),no(0),m_activeDialog(0),m_mainwindow(mw)
{

	setHoverCloseButton(true);	
	m_closeButton = new QToolButton( this );
    	connect( m_closeButton, SIGNAL( clicked() ), this, SLOT( slotCloseCurrent() ) );
	m_closeButton->setIconSet( SmallIcon( "tab_remove" ) );
        m_closeButton->adjustSize();
        QToolTip::add(m_closeButton, i18n("Close the current tab"));
        setCornerWidget( m_closeButton, TopRight );
	m_closeButton->hide();
	connect(this,SIGNAL(currentChanged(QWidget*)),this,SLOT(slotWindowActivated(QWidget*)));
	connect(this,SIGNAL(closeRequest(QWidget*)),this,SLOT(slotCloseRequest(QWidget*)));

}


void KexiWorkspaceTabbedMDI::slotCloseRequest(QWidget* w) {
	QCloseEvent cev;
	QApplication::sendEvent(w,&cev);
	if (cev.isAccepted()) delete w;
	no--;
	if (!no) m_closeButton->hide();
	kdDebug() << "deleted item; realnumber now" << no << endl;

}

void KexiWorkspaceTabbedMDI::slotCloseCurrent() {
	slotCloseRequest(currentPage());
}

void KexiWorkspaceTabbedMDI::addItem(KexiDialogBase *newItem)
{
	if (!newItem->icon())
		addTab(newItem,newItem->caption());
	else
		addTab(newItem,QIconSet(*newItem->icon()),newItem->caption());
	no++;
	m_closeButton->show();

//	connect(newItem, SIGNAL(closing(KexiDialogBase *)), this, SLOT(takeItem(KexiDialogBase *)));
	kdDebug() << "item added; realnumber now: " << no << endl;
}

void KexiWorkspaceTabbedMDI::takeItem(KexiDialogBase *delItem)
{
	no--;
	if (!no) m_closeButton->hide();
	kdDebug() << "took item; realnumber now" << no << endl;
}


void KexiWorkspaceTabbedMDI::slotWindowActivated(QWidget* w)
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
#ifndef KEXI_NO_CTXT_HELP
				if(m_mainwindow->help()) {
					m_mainwindow->help()->setContextHelp(
						m_activeDialog->contextHelpTitle(),
						m_activeDialog->contextHelpMessage());
				}
#endif

				m_activeDialog->activateActions();
				m_activeDialog->aboutToShow();

			}
			else
			{
				olddialog->deactivateActions();
#ifndef KEXI_NO_CTXT_HELP
				if(m_mainwindow->help()) {
					m_mainwindow->help()->setContextHelp(
						m_activeDialog->contextHelpTitle(),
						m_activeDialog->contextHelpMessage());
				}
#endif

				m_activeDialog->activateActions();
				m_activeDialog->aboutToShow();
			}
		}

	}
	else
	if (!m_activeDialog.isNull())
	{
		m_mainwindow->factory()->addClient(m_activeDialog->guiClient());
#ifndef KEXI_NO_CTXT_HELP
		if(m_mainwindow->help()) {
			m_mainwindow->help()->setContextHelp(
			m_activeDialog->contextHelpTitle(),
			m_activeDialog->contextHelpMessage());
		}
#endif

		m_activeDialog->activateActions();
		m_activeDialog->aboutToShow();
	}
}

KexiDialogBase *
KexiWorkspaceTabbedMDI::activeDocumentView()
{
	return static_cast<KexiDialogBase*>(currentPage());
}

void
KexiWorkspaceTabbedMDI::activateView(KexiDialogBase *kdb)
{
	if(!kdb)
		return;
	showPage(kdb);

//	slotWindowActivated(kdb);
	kdb->setFocus();
}

KexiWorkspaceTabbedMDI::~KexiWorkspaceTabbedMDI()
{
}

#include "kexiworkspaceTabbedMDI.moc"
