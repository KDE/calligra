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

#include <qsize.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcolor.h>
#include <qcursor.h>

#include "kexiformbase.h"

KexiFormBase::KexiFormBase(QWidget *parent, const char *name )
	: QWidget(parent,name)
{
	setMouseTracking(true);
	
	xmove = false;
	ymove = false;
	
	m_arrowCursor = true;
	m_dotSpacing = 10;
}

void KexiFormBase::addWidgetLineEdit()
{
	kdDebug() << "add line edit widget at " << this << endl;
}

void KexiFormBase::mouseMoveEvent(QMouseEvent *ev)
{

	if(ev->x() > width() - 5 && ev->y() > height() - 5)
	{
		setCursor(QCursor(SizeFDiagCursor));
		m_arrowCursor = false;
	}
	else if(ev->x() > width() - 5)
	{
		setCursor(QCursor(SizeHorCursor));
		m_arrowCursor = false;
	}
	else if(ev->y() > height() - 5)
	{
		setCursor(QCursor(SizeVerCursor));
		m_arrowCursor = false;
	}
	else
	{
		if(!m_arrowCursor)
		{
			setCursor(QCursor(ArrowCursor));
		}
		m_arrowCursor = true;
	}
	
	if(xmove)
	{
		setFixedSize(QSize(ev->x(), height()));
	}
	if(ymove)
	{
		setFixedSize(QSize(width(), ev->y()));
	}
		
}

void KexiFormBase::mousePressEvent(QMouseEvent *ev)
{
	if(ev->x() > width() - 5)
	{
		xmove = true;
	}
	if(ev->y() > height() - 5)
	{
		ymove = true;
	}
}

void KexiFormBase::mouseReleaseEvent(QMouseEvent *ev)
{
	xmove = false;
	ymove = false;
	
}

void KexiFormBase::paintEvent(QPaintEvent *ev)
{
	QPainter *p = new QPainter(this);
	QPen high(white, 2);
	QPen low(gray, 2);
	p->setPen(high);
	
	// painting the 3d-frame-decoration
	p->drawLine(0, 0, width(), 0);
	p->drawLine(0, 0, 0, height());
	p->setPen(low);
	p->drawLine(width(), 0, width(), height());
	p->drawLine(0, height(), width(), height());
	
	// drawing the dots -- got the dot?
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
	
}

KexiFormBase::~KexiFormBase(){
}

//#include "kexiformbase.moc"
#include "kexiformbase.moc"
