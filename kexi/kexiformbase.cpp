/***************************************************************************
                          kexiformbase.cpp  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
			   (C) 2002 by Joseph Wenninger <jowenn@kde.org>
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klineedit.h>

#include <qptrlist.h>
#include <qsize.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcolor.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpushbutton.h>
#include <kurlrequester.h>


#include <qobjectlist.h>

#include "kexiformbase.h"

KexiFormBase::KexiFormBase(QWidget *parent, const char *name, QString datasource)
	: KexiDialogBase(parent,name)
{
	initActions();
	if(datasource == "")
	{
		setCaption(i18n("[new form]"));
	}

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));
	
	m_dotSpacing = 10;

	resize( 250, 250 );

	setBackgroundPixmap(m_dotBg);
	
	m_widgetRectRequested = false;
	m_widgetRect = false;
	m_activeWidget=0;
	m_activeMoveWidget=0;
}

void KexiFormBase::initActions()
{
        //own-actions
        KAction *actionWidgetLineEdit = new KAction(i18n("Line Edit"), "lineedit",
                Key_F5, this, SLOT(slotWidgetLineEdit()), actionCollection(), "widget_line_edit");

        KAction *actionWidgetPushButton = new KAction(i18n("Push Button"), "button",
                Key_F6, this, SLOT(slotWidgetPushButton()), actionCollection(), "widget_push_button");

        KAction *actionWidgetURLRequest = new KAction(i18n("URL Request"), "button",
                Key_F7, this, SLOT(slotWidgetURLRequester()), actionCollection(), "widget_url_requester");
	setXMLFile("kexiformeditorui.rc");
}

void KexiFormBase::setActions(QPtrList<KAction> *actions)
{
	kdDebug() << "actions set..." << endl;
	QPtrListIterator<KAction> it(*actions);
	for(; it.current() != 0; ++it)
	{
		registerAction(it.current());
	}
}

void KexiFormBase::unregisterActions(QPtrList<KAction> *actions)
{
	kdDebug() << "unregistering actions..." << endl;
	QPtrListIterator<KAction> it(*actions);
	for(; it.current() != 0; ++it)
	{
		disconnect(it.current(), 0, 0, 0);
	}
}

void KexiFormBase::registerAction(KAction *action)
{
	kdDebug() << "registerd: " << action->name() << endl;

	
	if(!strcmp(action->name(),"widget_line_edit")==1)
	{
		connect(action, SIGNAL(activated()), this, SLOT(slotWidgetLineEdit()));
	}
	else
	{
		connect(action, SIGNAL(activated()), this, SLOT(slotWidgetPushButton()));
	}
}

void KexiFormBase::slotWidgetLineEdit()
{
	kdDebug() << "add line edit widget at " << this << endl;
	m_pendingWidget = new KLineEdit(this);
	m_widgetRectRequested = true;
}

void KexiFormBase::slotWidgetPushButton()
{
	m_pendingWidget = new QPushButton("push button", this);
	m_widgetRectRequested = true;
}

void KexiFormBase::slotWidgetURLRequester()
{
	m_pendingWidget = new KURLRequester("urlrequest", this);
	m_widgetRectRequested = true;
}

void KexiFormBase::mouseMoveEvent(QMouseEvent *ev)
{
	if(m_widgetRectRequested)
	{
		m_widgetRect = true;
		m_widgetRectBX = (((float)ev->x())/((float)m_dotSpacing)+0.5);
		m_widgetRectBX*=m_dotSpacing;
		m_widgetRectBY = (((float)ev->y())/((float)m_dotSpacing)+0.5);
		m_widgetRectBY*=m_dotSpacing;
		m_widgetRectEX = m_widgetRectBX;
		m_widgetRectEY = m_widgetRectBY;
		m_widgetRectRequested = false;
	}

	if(m_widgetRect)
	{
                m_widgetRectEX = (((float)ev->x())/((float)m_dotSpacing)+0.5);
		m_widgetRectEX*=m_dotSpacing;
                m_widgetRectEY = (((float)ev->y())/((float)m_dotSpacing)+0.5);
		m_widgetRectEY*=m_dotSpacing;
//		m_widgetRectEX = ev->x();
//		m_widgetRectEY = ev->y();
		repaint();
	}
}

void KexiFormBase::resizeEvent(QResizeEvent *ev)
{
	QPainter *p = new QPainter();
	m_dotBg = QPixmap(size());
	p->begin(&m_dotBg, this);
	
	// drawing the dots -- got the dot?
	QColor c = paletteBackgroundColor();
//	QPen(c);
	p->setPen(QPen(c));
	QBrush bg(c);
	p->setBrush(bg);
	p->drawRect(0, 0, width(), height());
	QPen dots(black, 1);
	p->setPen(dots);
	int cols = width() / m_dotSpacing;
	int rows = height() / m_dotSpacing;
	
	for(int rowcursor = 1; rowcursor < rows; ++rowcursor)
	{
		for(int colcursor = 1; colcursor < cols; ++colcursor)
		{
			p->drawPoint(colcursor * m_dotSpacing, rowcursor * m_dotSpacing);
		}
	}
	
	p->end();
	setPaletteBackgroundPixmap(m_dotBg);
}

void KexiFormBase::paintEvent(QPaintEvent *ev)
{
	QPainter p(this);
	if(m_widgetRect)
	{
		QPen wpen(black, 2);
		p.setPen(wpen);
		p.drawRect(m_widgetRectBX, m_widgetRectBY, m_widgetRectEX-m_widgetRectBX, m_widgetRectEY-m_widgetRectBY);
	}
	p.end();
}


void KexiFormBase::installEventFilterRecursive(QObject *obj)
{
	obj->installEventFilter(this);
	if ( obj->children() )
	{
		QObjectListIt it( *obj->children() );
		QObject *obj1;
        	while( (obj1=it.current()) != 0 )
		{
			++it;
			if (obj1->isWidgetType())
		    	installEventFilterRecursive(obj1);
            	}
        }

}

void KexiFormBase::mouseReleaseEvent(QMouseEvent *ev)
{
	if(m_widgetRect)
	{
		insertWidget(m_pendingWidget, m_widgetRectBX, m_widgetRectBY,
			m_widgetRectEX-m_widgetRectBX, m_widgetRectEY-m_widgetRectBY);
		m_widgetRectBX = 0;
		m_widgetRectBY = 0;
		m_widgetRectEX = 0;
		m_widgetRectEY = 0;

		installEventFilterRecursive(m_pendingWidget);

		m_widgetRect = false;


		repaint();
	}
}

void KexiFormBase::insertWidget(QWidget *widget, int x, int y, int w, int h)
{
	widget->move(x, y);
	widget->resize(w, h);
	widget->show();
	widget->setFocusPolicy(QWidget::NoFocus);
//	widget->repaint();
//	grabMouse();
//	grabKeyboard();
}

bool KexiFormBase::eventFilter(QObject *obj, QEvent *ev)
{
	kdDebug() << "event!" << endl;
	switch (ev->type())
	{
		case QEvent::MouseButtonPress:
			m_activeWidget=static_cast<QWidget*>(obj);
			while (!(m_activeWidget->parentWidget(true)==this))
				m_activeWidget=m_activeWidget->parentWidget();
			m_activeMoveWidget=m_activeWidget;
			m_moveBX=static_cast<QMouseEvent*>(ev)->x();
			m_moveBY=static_cast<QMouseEvent*>(ev)->y();
			return true;
			break;
		case QEvent::MouseButtonRelease:
			m_activeMoveWidget=0;
			return true;
			break;
		case QEvent::MouseMove:
			if (m_activeMoveWidget)
			{
				int tmpx,tmpy;
		                tmpx = (((float)(m_activeMoveWidget->x()+static_cast<QMouseEvent*>(ev)->x()-m_moveBX))/
					((float)m_dotSpacing)+0.5);
		                tmpx*=m_dotSpacing;
		                tmpy = (((float)(m_activeMoveWidget->y()+static_cast<QMouseEvent*>(ev)->y()-m_moveBY))/
					((float)m_dotSpacing)+0.5);
		                tmpy*=m_dotSpacing;
				if ((tmpx!=m_activeMoveWidget->x()) ||(tmpy!=m_activeMoveWidget->y()) )
					m_activeMoveWidget->move(tmpx,tmpy);
			}
			return true;
			break;
		default:
			break;
	}
	switch( ev->type() )
	{
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseButtonDblClick:
		case QEvent::MouseMove:
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		case QEvent::Enter:
		case QEvent::Leave:
		case QEvent::Wheel:
		case QEvent::ContextMenu:
			return true; // ignore
		default:
			break;
	}
	return false;
}

KexiFormBase::~KexiFormBase(){
}

#include "kexiformbase.moc"
