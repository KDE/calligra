/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
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

#include <qlayout.h>
#include <qtimer.h>

#include <kdockwidget.h>
#include <kiconloader.h>
#include <netwm_def.h>
#include <kdebug.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <kdebug.h>

#include "kexicontexthelp.h"

#include "kexidialogbase.h"
#include "kexiview.h"
#include "kexiproject.h"
#include "kexiworkspace.h"
#include "kexiprojecthandleritem.h"

KexiDialogBase *KexiDialogBase::s_activeDocumentWindow=0;
KexiDialogBase *KexiDialogBase::s_activeToolWindow=0;
QPtrList<KexiDialogBase> *KexiDialogBase::s_DocumentWindows=0;
QPtrList<KexiDialogBase> *KexiDialogBase::s_ToolWindows=0;

KexiDialogBase::KexiDialogBase(KexiView* view, QString identifier, QWidget *parent, const char *name) 
	: QWidget(parent, 
		name ? name : identifier.latin1(), WDestructiveClose)
	,KXMLGUIClient()
	,m_view(view)
	,m_partItem(0)
	,m_identifier( identifier )
//	,m_windowTypeName( windowTypeName )
{
	init();

//	m_project=view->project();
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

KexiDialogBase::KexiDialogBase(KexiView *view, KexiProjectHandlerItem *item, QWidget *parent, const char *name )
	: QWidget(parent, 
		name ? name : item->fullIdentifier().latin1(), WDestructiveClose)
	,KXMLGUIClient()
 //	: KexiDialogBase(view,parent, item->fullIdentifier().latin1())
	,m_view(view)
	,m_partItem(item)
	,m_identifier( item ? item->fullIdentifier() : QString::null )
//	,m_windowTypeName( windowTypeName )
{
	init();
}

void KexiDialogBase::init()
{
	m_registered=false;
	m_registering=false;
	m_contextTitle = QString::null;
	m_contextMessage = QString::null;
	m_toggleAction = 0;

	updateCaption();
	if (m_partItem)
		setIcon( m_partItem->handler()->itemPixmap() );

	m_gridLyr = new QGridLayout(this);
	m_gridLyr->setMargin(4);

	//resize to fit inside the workspace
	QWidget * wid = m_view->workspaceWidget();

//	reparent(m_view->workspaceWidget(),QPoint(0,0),true);
  m_registering=false;

//	m_view->workspace()->activateView(this);
	if(!m_identifier.isEmpty())
	{
		m_view->registerDialog(this, m_identifier);
		kdDebug() << "KexiDialogBase registered as " << m_identifier << endl;
		m_registered = true;
	}
}

QString KexiDialogBase::windowTypeName()
{ 
	if (!m_windowTypeName.isEmpty())
		return m_windowTypeName;
	return m_partItem ? m_partItem->handler()->name() : QString::null;
}

/* Overrides window's type name from part name with custom one.
	Used eg. in KexiAlterTable window. 
	Use tn==QString::null to restore default type name. 
*/
void KexiDialogBase::setCustomWindowTypeName( const QString &tn )
{
	m_windowTypeName = tn;
	updateCaption();
}

QSize KexiDialogBase::sizeHint() const
{
	if (!parentWidget()) return QWidget::sizeHint();
/*	QWidget *wi = parentWidget();
	if (parentWidget()->isA("QDockWindow"))
		return QWidget::sizeHint();*/
	if (m_wt==ToolWindow) return QWidget::sizeHint();
	QSize new_size = frameSize();
	int max_w = m_view->workspaceWidget()->width() - 10;
	int max_h = m_view->workspaceWidget()->height() - 10 - m_view->statusBar()->height();
	if (max_h < 0 || max_h < new_size.height())
		max_h = new_size.height();
	return QSize(max_w, max_h);
}

/*! If you use this, windowTypeName() won't be used to construct 
	the window's caption.
*/
void KexiDialogBase::setCustomCaption( const QString &caption )
{
	m_customCaption = caption;
	if (m_customCaption.isEmpty())
		updateCaption();
	else
		QWidget::setCaption( m_customCaption );
}

/*! Very clever internal method for computing caption 
	depending on window contents.
*/
void KexiDialogBase::updateCaption()
{
	if (windowTypeName().isEmpty()) {
		//no data type name
		if (!m_customCaption.isEmpty()) {
			//just custom caption
			QWidget::setCaption(m_customCaption);
		}
		else if (m_partItem && !m_partItem->title().isEmpty()) {
			//caption from item title
			QWidget::setCaption(m_partItem->title());
		}
		//??
	}
	else {
		//we've got data type name
		if (!m_customCaption.isEmpty()) {
			//custom caption overwrites typename
			QWidget::setCaption(m_customCaption);
		}
		else if (m_partItem && !m_partItem->title().isEmpty()) {
			//item title with typename
			QWidget::setCaption(QString("%1 - %2").arg(m_partItem->title()).arg(windowTypeName()));
		}
		else {
			//just a typename
			QWidget::setCaption(windowTypeName());
		}
	}
}

KexiProject *KexiDialogBase::kexiProject()const
{
    return m_view->project();
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

const QString& KexiDialogBase::contextHelpTitle() const {
	return m_contextTitle;
}

const QString& KexiDialogBase::contextHelpMessage() const {
	return m_contextMessage;
}

void KexiDialogBase::registerAs(KexiDialogBase::WindowType wt, const QString &identifier)
{
	m_wt=wt;
	m_registered=true;

	if (parentWidget()) return;
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

  //resize to fit inside the workspace
  QSize new_size = frameSize();
  QSize max_size = m_view->workspaceWidget()->size()-QSize(10,10+m_view->statusBar()->height());
  if (new_size.width() > max_size.width())
    new_size.setWidth( max_size.width() );
  if (new_size.height() > max_size.height())
    new_size.setHeight( max_size.height() );
  if (new_size!=frameSize())
    resize(new_size);
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
	if ((m_wt!=ToolWindow) && m_registered)
///	if (m_registered)
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
//js	if (m_registered && (m_wt==ToolWindow))
//js		m_view->removeQDockWindow(w);
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

/*! Plugs toggle action, so action is updated after window's visiblility
	was changed and conversely */
void KexiDialogBase::plugToggleAction(KToggleAction *toggle_action)
{
	m_toggleAction = toggle_action;
	connect( toggle_action, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)) );
}

void KexiDialogBase::hideEvent(QHideEvent *ev)
{
	emit hidden();
	if (m_toggleAction)
		m_toggleAction->setChecked(false);
}

void KexiDialogBase::showEvent(QShowEvent *ev)
{
	emit shown();
	if (m_toggleAction)
		m_toggleAction->setChecked(true);
}

KXMLGUIClient *KexiDialogBase::guiClient() { 
	return this;	
}

KexiProjectHandler *KexiDialogBase::part() { return m_partItem ? m_partItem->handler() : 0; }

#include "kexidialogbase.moc"
