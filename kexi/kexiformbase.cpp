/***************************************************************************
                          kexiformbase.cpp  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
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

#include "kexiformbase.h"

KexiFormBase::KexiFormBase(QWidget *parent, const char *name, QString datasource)
	: KexiDialogBase(parent,name)
{
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

void KexiFormBase::mouseMoveEvent(QMouseEvent *ev)
{
	if(m_widgetRectRequested)
	{
		m_widgetRect = true;
		m_widgetRectBX = ev->x();
		m_widgetRectBY = ev->y();
		m_widgetRectEX = ev->x();
		m_widgetRectEY = ev->x();
		m_widgetRectRequested = false;
	}

	if(m_widgetRect)
	{
		m_widgetRectEX = ev->x();
		m_widgetRectEY = ev->y();
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
		p.drawRect(m_widgetRectBX, m_widgetRectBY, m_widgetRectEX, m_widgetRectEY);
	}
	p.end();
}

void KexiFormBase::mouseReleaseEvent(QMouseEvent *ev)
{
	if(m_widgetRect)
	{
		insertWidget(m_pendingWidget, m_widgetRectBX, m_widgetRectBY, m_widgetRectEX, m_widgetRectEY);
		m_widgetRectBX = 0;
		m_widgetRectBY = 0;
		m_widgetRectEX = 0;
		m_widgetRectEY = 0;
		
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
//	grabMouse();
//	grabKeyboard();
}

bool KexiFormBase::eventFilter(QObject *obj, QEvent *ev)
{
	kdDebug() << "event!" << endl;
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
