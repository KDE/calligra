/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QMenu>

#include <QMenu>
#include <QIcon>

#include <klocale.h>
#include <KoPACanvas.h>
#include <KoPointerEvent.h>
#include <kdebug.h>

#include "KPrPresentationDrawWidget.h"

KPrPresentationDrawWidget::KPrPresentationDrawWidget(KoPACanvas * canvas) : QWidget(canvas)
{
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    m_size = canvas->size();

    resize( m_size );
    
    m_draw = false;
    m_penSize = 10;
    m_penColor = Qt::black;
}

//KPrPresentationDrawWidget::~KPrPresentationDrawWidget()
//{
//}

void KPrPresentationDrawWidget::paintEvent(QPaintEvent * event)
{
    struct Path path;
    QPainter painter( this );    
    QBrush brush( Qt::SolidPattern );
    QPen pen( brush, m_penSize, Qt::CustomDashLine, Qt::RoundCap, Qt::RoundJoin );
    painter.setPen( pen );
    for( int i=0; i < m_pointVectors.count(); i++ ){
        path = m_pointVectors.at(i);
        pen.setColor( path.color );
        pen.setWidth( path.size );
        painter.setPen( pen );
        painter.drawPolyline( QPolygonF( path.points ) );
    }
}

void KPrPresentationDrawWidget::mousePressEvent( QMouseEvent* e )
{
    struct Path path;
    path.color = m_penColor;
    path.size = m_penSize;
    path.points = QVector<QPointF>() << e->pos();
    m_pointVectors.append( path );
    m_draw = true;
}

void KPrPresentationDrawWidget::mouseMoveEvent( QMouseEvent* e )
{
    if(m_draw)
    {
	m_pointVectors.last().points << e->pos();
	update();
    }
}

void KPrPresentationDrawWidget::mouseReleaseEvent( QMouseEvent* e )
{
    m_draw = false;
}

void KPrPresentationDrawWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu( this );
    
    QMenu *color = new QMenu( QString( "Color of the pen"), this );
    QMenu *size = new QMenu( QString( "Size of the pen"), this );

    color->addAction ( buildActionColor( Qt::black, "&Black" ) );
    color->addAction ( buildActionColor( Qt::white, "&White" ) );
    color->addAction ( buildActionColor( Qt::green, "&Green" ) );
    color->addAction ( buildActionColor( Qt::red, "&Red" ) );
    color->addAction ( buildActionColor( Qt::blue, "&Blue" ) );
    color->addAction ( buildActionColor( Qt::yellow, "&Yellow" ) );
    connect(color, SIGNAL(triggered(QAction*)), this, SLOT(updateColor(QAction*)));


    size->addAction( QString("9 px") );
    size->addAction( QString("10 px") );
    size->addAction( QString("12 px") );
    size->addAction( QString("14 px") );
    size->addAction( QString("16 px") );
    size->addAction( QString("18 px") );
    size->addAction( QString("20 px") );
    size->addAction( QString("22 px") );
    size->addAction( QString("24 px") );
    connect(size, SIGNAL(triggered(QAction*)), this, SLOT(updateSize(QAction*)));
    
    // Not connected yet
    menu.addMenu( color );
    menu.addMenu( size );
    
    menu.exec(event->globalPos());
    m_draw = false;
}

QAction* KPrPresentationDrawWidget::buildActionColor ( QColor color, QString name )
{
    QAction *action;
    action = new QAction( buildIconColor ( color ) , name, this );
    return action;
}

QIcon KPrPresentationDrawWidget::buildIconColor ( QColor color )
{

    QPen thumbPen ( color, Qt::MiterJoin );
    thumbPen.setWidth ( 50 );
    QPixmap thumbPixmap ( QSize ( 24, 20 ) );
    thumbPixmap.fill ( );
    QPainter thumbPainter ( &thumbPixmap );
    thumbPainter.setBackground ( QBrush( color ) );
    thumbPainter.setPen ( thumbPen );
    thumbPainter.drawRect ( 2, 2, 20, 16 );
    QIcon thumbIcon ( thumbPixmap );
    return thumbIcon;
}

void KPrPresentationDrawWidget::updateSize ( QAction *size )
{
    QString str(size->text());
    str.replace("&", "");
    str.replace(" px", "");

    if(str == "9")
            m_penSize = 9;
    else if(str == "10")
            m_penSize = 10;
    else if(str == "12")
            m_penSize = 12;
    else if(str == "14")
            m_penSize = 14;
    else if(str == "16")
            m_penSize = 16;
    else if(str == "18")
            m_penSize = 18;
    else if(str == "20")
            m_penSize = 20;
    else if(str == "22")
            m_penSize = 22;
    else if(str == "24")
            m_penSize = 24;
    m_draw = false;
}

void KPrPresentationDrawWidget::updateColor ( QAction *color )
{
    QString str(color->text());
    str.replace("&", "");

    if(str == "Black")
            m_penColor = Qt::black;
    else if(str == "White")
            m_penColor = Qt::white;
    else if(str == "Green")
            m_penColor = Qt::green;
    else if(str == "Red")
            m_penColor = Qt::red;
    else if(str == "Yellow")
            m_penColor = Qt::yellow;
    else if(str == "Blue")
            m_penColor = Qt::blue;
    m_draw = false;
}
 
