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

#include <qsize.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcolor.h>
#include <qcursor.h>

#include "kexiformbase.h"

KexiFormBase::KexiFormBase(QWidget *parent, const char *name, QString datasource)
	: QWidget(parent,name)
{
	if(datasource == "")
	{
		setCaption(i18n("[new form]"));
	}

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));
	
	m_dotSpacing = 10;

	resize( 250, 250 );
}

void KexiFormBase::addWidgetLineEdit()
{
	kdDebug() << "add line edit widget at " << this << endl;
}

void KexiFormBase::paintEvent(QPaintEvent *ev)
{
	QPainter *p = new QPainter(this);
	
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

#include "kexiformbase.moc"
