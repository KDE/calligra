/* This file is part of the KDE project
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>
             (C) 1999 Stephan Kulow <coolo@kde.org>

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

#include <QPainter>
//Added by qt3to4:
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>

#include "View.h"

#include "Handler.h"

#define KS_MIN_RECTSIZE 3

using namespace KSpread;

InsertHandler::InsertHandler( View* view, QWidget* widget )
    : KoEventHandler( widget )
{
    m_view = view;
    m_started = false;
    m_clicked = false;

    widget->setCursor( Qt::CrossCursor );
}

InsertHandler::~InsertHandler()
{
    ((QWidget*)target())->setCursor( Qt::ArrowCursor );
    m_view->resetInsertHandle();
}

bool InsertHandler::eventFilter( QObject*, QEvent* ev )
{
    if ( ev->type() == QEvent::MouseButtonPress )
    {
    QMouseEvent* e = (QMouseEvent*)ev;

    m_geometryStart = e->pos();
    m_geometryEnd = e->pos();
    m_started = false;
    m_clicked = true;

    return true;
    }
    else if ( ev->type() == QEvent::MouseMove )
    {
    if ( !m_clicked )
        return true;

    QMouseEvent* e = (QMouseEvent*)ev;

    QPainter painter;
    painter.begin( (QWidget*)target() );
    painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );

    QPen pen;
    pen.setStyle( Qt::DashLine );
    painter.setPen( pen );

    if ( m_started )
        {
        int x = m_geometryStart.x();
        int y = m_geometryStart.y();
        if ( x > m_geometryEnd.x() )
        x = m_geometryEnd.x();
        if ( y > m_geometryEnd.y() )
        y = m_geometryEnd.y();
        int w = m_geometryEnd.x() - m_geometryStart.x();
        if ( w < 0 ) w *= -1;
        int h = m_geometryEnd.y() - m_geometryStart.y();
        if ( h < 0 ) h *= -1;

        if ( w < KS_MIN_RECTSIZE ) w = KS_MIN_RECTSIZE;
        if ( h < KS_MIN_RECTSIZE ) h = KS_MIN_RECTSIZE;

        painter.drawRect( x, y, w, h );
    }
    else
        m_started = true;

    m_geometryEnd = e->pos();

    int x = m_geometryStart.x();
    int y = m_geometryStart.y();
    if ( x > m_geometryEnd.x() )
        x = m_geometryEnd.x();
    if ( y > m_geometryEnd.y() )
        y = m_geometryEnd.y();
    int w = m_geometryEnd.x() - m_geometryStart.x();
    if ( w < 0 ) w *= -1;
    int h = m_geometryEnd.y() - m_geometryStart.y();
    if ( h < 0 ) h *= -1;

    if ( w < KS_MIN_RECTSIZE ) w = KS_MIN_RECTSIZE;
    if ( h < KS_MIN_RECTSIZE ) h = KS_MIN_RECTSIZE;

    painter.drawRect( x, y, w, h );
    painter.end();

    return true;
    }
    else if ( ev->type() == QEvent::MouseButtonRelease )
    {
    QMouseEvent* e = (QMouseEvent*)ev;

    if ( !m_started )
        {
        delete this;
        return true;
    }

    m_geometryEnd = e->pos();

    int x = m_geometryStart.x();
    int y = m_geometryStart.y();
    if ( x > m_geometryEnd.x() )
        x = m_geometryEnd.x();
    if ( y > m_geometryEnd.y() )
        y = m_geometryEnd.y();
    int w = m_geometryEnd.x() - m_geometryStart.x();
    if ( w < 0 ) w *= -1;
    int h = m_geometryEnd.y() - m_geometryStart.y();
    if ( h < 0 ) h *= -1;

    if ( w < KS_MIN_RECTSIZE ) w = KS_MIN_RECTSIZE;
    if ( h < KS_MIN_RECTSIZE ) h = KS_MIN_RECTSIZE;

    QPainter painter;
    painter.begin( (QWidget*)target() );

    QPen pen;
    pen.setStyle( Qt::DashLine );
    painter.setPen( pen );

    painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );
    painter.drawRect( x, y, w, h );
    painter.end();

    insertObject( QRect( x, y, w, h)  );

    delete this;
    return true;
    }
    else if ( ev->type() == QEvent::KeyPress )
    {
    QKeyEvent* e = (QKeyEvent*)ev;
    if ( e->key() != Qt::Key_Escape )
        return false;

    delete this;
    return true;
    }

    return false;
}

InsertPartHandler::InsertPartHandler( View* view, QWidget* widget, const KoDocumentEntry& entry )
  : InsertHandler( view, widget ), m_entry( entry )
{
}
InsertPartHandler::~InsertPartHandler()
{
}
void InsertPartHandler::insertObject( QRect r)
{
  m_view->insertChild( r, m_entry );
}


InsertChartHandler::InsertChartHandler( View* view, QWidget* widget, const KoDocumentEntry& entry )
  : InsertHandler( view, widget ), m_entry( entry )
{
}
InsertChartHandler::~InsertChartHandler()
{
}
void InsertChartHandler::insertObject( QRect r)
{
  m_view->insertChart( r, m_entry );
}

InsertPictureHandler::InsertPictureHandler( View* view, QWidget* widget, const KUrl &file )
  : InsertHandler( view, widget )
{
  m_file = file;
}
InsertPictureHandler::~InsertPictureHandler()
{
}
void InsertPictureHandler::insertObject( QRect r)
{
  Q_UNUSED(r)
  //m_view->insertPicture(/* r,*/ m_file );
}

#include "Handler.moc"
