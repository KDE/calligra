/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
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

#include <qtimer.h>

#include <kdockwidget.h>
#include <kiconloader.h>
#include <netwm_def.h>
#include <kdebug.h>

#include "kexicontexthelp.h"

#include "kexidialogbase.h"
#include "kexiview.h"
#include "kexiproject.h"
#include "kexiworkspace.h"

KexiDialogBase *KexiDialogBase::s_activeDocumentWindow=0;
KexiDialogBase *KexiDialogBase::s_activeToolWindow=0;
QPtrList<KexiDialogBase> *KexiDialogBase::s_DocumentWindows=0;
QPtrList<KexiDialogBase> *KexiDialogBase::s_ToolWindows=0;

KexiDialogBase::KexiDialogBase(KexiView* view,QWidget *parent, const char *name) : QWidget(parent, name, WDestructiveClose)
{
	m_registered=false;
	m_registering=false;
	m_view=view;
	m_project=view->project();
	m_contextTitle = QString::null;
	m_contextMessage = QString::null;
	m_identifier = QString::null;
#if 0
	if (s_DocumentWindows==0) s_DocumentWindows=new QPtrList<KexiDialogBase>();
	if (s_ToolWindows==0) s_ToolWindows=new QPtrList<KexiDialogBase>();
	m_mainWindow=(parent==0)?((KexiApplication*)kapp)->mainWindow():
			(parent->qt_cast("KexiView")==0)?
			((KexiApplication*)kapp)->mainWindow():
			static_cast<KexiView*>(parent->qt_cast("KexiView"));
	myDock=0;
#endif
}

KexiProject *KexiDialogBase::kexiProject()const
{
    return m_project;
}

KexiView *KexiDialogBase::kexiView()const
{
    return m_view;
}

void
KexiDialogBase::setContextHelp(const QString &title, const QString &message)
{
#ifndef KEXI_NO_CTXT_HELP
	if(!m_view->help())
		return;

	m_contextTitle = title;
	m_contextMessage = message;
	m_view->help()->setContextHelp(title, message);
#endif
}

void KexiDialogBase::registerAs(KexiDialogBase::WindowType wt, const QString &identifier)
{
	m_wt=wt;
	m_registered=true;
	if (wt==ToolWindow)
	{
		w=new QDockWindow(m_view->mainWindow());
		w->setResizeEnabled(true);
		w->setCloseMode(QDockWindow::Always);
		 reparent(w,QPoint(0,0),true);
		w->setWidget(this);
		m_view->mainWindow()->moveDockWindow(w, DockLeft);
		w->setCaption(this->caption());
		kexiView()->addQDockWindow(w);
		return;
	}
	m_registering=true;
	reparent(m_view->workspaceWidget(),QPoint(0,0),true);
	m_registering=false;
	m_view->workspace()->activateView(this);
	if(!identifier.isNull())
	{
		m_view->registerDialog(this, identifier);
		m_identifier = identifier;
		kdDebug() << "KexiDialogBase::registerAs() " << identifier << endl;
	}
//	showMaximized();
	return;
#if 0
	myDock=0;
	if (! ((m_mainWindow->windowMode()==KexiView::MDIWindowMode) && (wt==DocumentWindow)))
        {
		myDock=m_mainWindow->createDockWidget( "Widget",
			(icon()?(*(icon())):SmallIcon("kexi")), 0, caption());
		myDock->setWidget(this);
		myDock->setEnableDocking(KDockWidget::DockFullDocking);
		myDock->setDockSite(KDockWidget::DockFullDocking);
		myDock->setDockWindowType(NET::Normal);
		myDock->setDockWindowTransient(m_mainWindow,true);
	}
	if (wt==DocumentWindow) {
		if (myDock!=0) {
			if ((s_activeDocumentWindow==0) || (s_activeDocumentWindow->myDock==0)) {
				if (m_mainWindow->windowMode()==KexiView::SingleWindowMode)
					myDock->manualDock(m_mainWindow->getMainDockWidget(),KDockWidget::DockTop, 100);
				else
					myDock->toDesktop();
			}
			else
				myDock->manualDock(s_activeDocumentWindow->myDock,KDockWidget::DockCenter);
		}
		s_DocumentWindows->insert(0,this);
		s_activeDocumentWindow=this;
	}
	else {
		if ((s_activeToolWindow==0) || (s_activeToolWindow->myDock==0)) {
			if ( (m_mainWindow->windowMode()==KexiView::SingleWindowMode) ||
				(m_mainWindow->windowMode()==KexiView::MDIWindowMode))
				myDock->manualDock(m_mainWindow->getMainDockWidget(),KDockWidget::DockLeft, 20);
			else
				myDock->toDesktop();
		}
		else
			myDock->manualDock(s_activeToolWindow->myDock,KDockWidget::DockCenter);
		s_ToolWindows->insert(0,this);
		s_activeToolWindow=this;
	}

	if (myDock) myDock->makeDockVisible();
#endif
}

void KexiDialogBase::focusInEvent ( QFocusEvent *)
{
	kdDebug()<<"KexiDialogBase::FocusInEvent()"<<endl;
	if(!m_contextMessage.isNull())
	{
#ifndef KEXI_NO_CTXT_HELP
		setContextHelp(m_contextTitle, m_contextMessage);
#endif
	}
}
void KexiDialogBase::closeEvent(QCloseEvent *ev)
{
	if ((m_wt!=ToolWindow) && (m_registered==true))
	{
		m_view->workspace()->slotWindowActivated(0);
	}
	finishUpForClosing();
	emit closing(this);
//	close();

	if(!m_identifier.isNull() && m_view)
		m_view->removeDialog(m_identifier);

	ev->accept();
}

void KexiDialogBase::registerChild(QWidget *child)
{
	m_widgets.append(child);
}

void KexiDialogBase::aboutToShow()
{
	kdDebug() << "KexiDialogBase::aboutToShow()" << endl;
	QWidget *it;
	for(it = m_widgets.first(); it; it = m_widgets.next())
	{
		kdDebug() << "KexiDialogBase::aboutToShow(): it " << it << endl;
		it->show();
	}
}

void KexiDialogBase::aboutToHide()
{
	kdDebug() << "KexiDialogBase::aboutToHide()" << endl;
	QWidget *it;
	for(it = m_widgets.first(); it; it = m_widgets.next())
	{
		kdDebug() << "KexiDialogBase::aboutToHide(): it " << it << endl;
		it->hide();
	}
}

KexiDialogBase::~KexiDialogBase()
{
	if (m_registered && (m_wt==ToolWindow))
		m_view->removeQDockWindow(w);
}

void KexiDialogBase::activateActions(){;}

void KexiDialogBase::deactivateActions(){;}

/*! If dialog is registered as child of QDockWindow (ie. it is a tool window)
    showing it equals also showing its parent.
*/
void KexiDialogBase::show()
{
	if (parentWidget() && parentWidget()->isA("QDockWindow")) {
		parentWidget()->show();
		QWidget::show();
		return;
	}
	QWidget::show();
}

/*! If dialog is registered as child of QDockWindow (ie. it is a tool window)
    hiding it equals also hiding its parent.
*/
void KexiDialogBase::hide()
{
	if (parentWidget() && parentWidget()->isA("QDockWindow")) {
		parentWidget()->hide();
		return;
	}
	QWidget::hide();
}
/*! Convenient slot for connecting with KToggleAction::toggle(bool) signal.
*/
void KexiDialogBase::setVisible(bool on)
{
	on ? show() : hide();
}

#include "kexidialogbase.moc"
