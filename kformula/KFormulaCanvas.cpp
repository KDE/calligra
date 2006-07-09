/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KFormulaCanvas.h"

//#include <FormulaShape.h>
#include <QPaintEvent>
#include <QPainter>

KFormulaCanvas::KFormulaCanvas( QWidget* parent, Qt::WFlags f) : QWidget( parent, f )
{
    setFocusPolicy( Qt::StrongFocus );
    setBackgroundRole( QPalette::NoRole );
    m_dirtyBuffer = true;
}

KFormulaCanvas::~KFormulaCanvas()
{
}

void KFormulaCanvas::paintEvent( QPaintEvent* event )
{
/*    QPainter p;

    if( m_dirtyBuffer )
    {
	 m_paintBuffer = QPixmap();// m_formulaShape->size() );
         p.begin( &m_painBuffer );
//	 m_formulaShape->draw( &p );
	 p.end();
    }
	 
    p.begin( this );
    p.drawPixmap( event->rect(), m_paintBuffer, event->rect() );
    p.end();*/
}

void KFormulaCanvas::keyPressEvent( QKeyEvent* event )
{
//    m_formulaShape->keyPressEvent( event );
}

void KFormulaCanvas::focusInEvent( QFocusEvent* event )
{
//    m_formulaShape->focusInEvent( event );
}

void KFormulaCanvas::focusOutEvent( QFocusEvent* event )
{
//    m_formulaShape->focusOutEvent( event );
}

void KFormulaCanvas::mousePressEvent( QMouseEvent* event )
{
//    m_formulaShape->mousePressEvent( event );
}

void KFormulaCanvas::mouseReleaseEvent( QMouseEvent* event )
{
  //  m_formulaShape->mouseReleaseEvent( event );
}

void KFormulaCanvas::mouseDoubleClickEvent( QMouseEvent* event )
{
  //  m_formulaShape->mouseDoubleClickEvent( event );
}

void KFormulaCanvas::mouseMoveEvent( QMouseEvent* event )
{
//    m_formulaShape->mouseMoveEvent( event );
}

