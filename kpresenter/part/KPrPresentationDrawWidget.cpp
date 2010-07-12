/* This file is part of the KDE project
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * Copyright (C) 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * Copyright (C) 2009 Johann Hingue <yoan1703@hotmail.fr>
 * Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>
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
#include <QList>
#include <QMenu>
#include <QIcon>

#include <klocale.h>
#include <kdebug.h>

#include <KoPACanvas.h>
#include <KoPointerEvent.h>

#include "KPrPresentationDrawWidget.h"

KPrPresentationDrawWidget::KPrPresentationDrawWidget(KoPACanvas * canvas)
: KPrPresentationToolEventForwarder(canvas)
, m_draw( false )
, m_penSize( 10 )
, m_penColor( Qt::black )
{
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );

    resize( canvas->size() );
}

KPrPresentationDrawWidget::~KPrPresentationDrawWidget()
{
}

void KPrPresentationDrawWidget::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QPainter painter( this );
    QBrush brush( Qt::SolidPattern );
    QPen pen( brush, m_penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    foreach ( const Path &path, m_pointVectors ) {
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
    if ( m_draw ) {
        m_pointVectors.last().points << e->pos();
        update();
    }
}

void KPrPresentationDrawWidget::mouseReleaseEvent( QMouseEvent* e )
{
    Q_UNUSED(e);
    m_draw = false;
}

void KPrPresentationDrawWidget::contextMenuEvent(QContextMenuEvent* event)
{
    // TODO rework to not recreate the menu all the time
    // make strings translateable.
    // maybe the options which are shown here should be configurable in the
    // config file
    QMenu menu( this );

    QMenu *color = new QMenu( i18n("Pen Color"), &menu );
    QMenu *size = new QMenu( i18n("Pen Size"), &menu );

    color->addAction( buildActionColor(Qt::black, i18n("Black")));
    color->addAction( buildActionColor(Qt::white, i18n("White")));
    color->addAction( buildActionColor(Qt::green, i18n("Green")));
    color->addAction( buildActionColor(Qt::red, i18n("Red")));
    color->addAction( buildActionColor(Qt::blue, i18n("Blue")));
    color->addAction( buildActionColor(Qt::yellow, i18n("Yellow")));
    connect( color, SIGNAL( triggered( QAction* ) ), this, SLOT( updateColor( QAction* ) ) );

    size->addAction( buildActionSize ( 9 ) );
    size->addAction( buildActionSize ( 10 ) );
    size->addAction( buildActionSize ( 12 ) );
    size->addAction( buildActionSize ( 14 ) );
    size->addAction( buildActionSize ( 16 ) );
    size->addAction( buildActionSize ( 18 ) );
    size->addAction( buildActionSize ( 20 ) );
    size->addAction( buildActionSize ( 22 ) );

    connect( size, SIGNAL( triggered( QAction* ) ), this, SLOT( updateSize( QAction* ) ) );

    menu.addMenu( color );
    menu.addMenu( size );

    menu.exec( event->globalPos() );
    m_draw = false;
}

QAction* KPrPresentationDrawWidget::buildActionSize( int size )
{
    QAction *action = new QAction( buildIconSize( size ), QString::number(size)+"px", this );
    action->setProperty( "size", size );
    return action;
}

QAction* KPrPresentationDrawWidget::buildActionColor( QColor color, QString name )
{
    QAction *action;
    action = new QAction( buildIconColor ( color ) , name, this );
    action->setProperty( "color", QVariant( color ) );
    return action;
}

QIcon KPrPresentationDrawWidget::buildIconSize( int size )
{
    QPen thumbPen( Qt::black, Qt::MiterJoin );
    thumbPen.setCapStyle( Qt::RoundCap );
    thumbPen.setWidth( size );
    QPixmap thumbPixmap( QSize ( 26, 26 ) );
    thumbPixmap.fill();
    QPainter thumbPainter( &thumbPixmap );
    thumbPainter.setPen( thumbPen );
    thumbPainter.drawPoint( 13, 13 );
    QIcon thumbIcon( thumbPixmap );
    return thumbIcon;
}

QIcon KPrPresentationDrawWidget::buildIconColor( QColor color )
{
    QPixmap thumbPixmap( QSize ( 24, 20 ) );
    thumbPixmap.fill( color );
    QIcon thumbIcon( thumbPixmap );
    return thumbIcon;
}

void KPrPresentationDrawWidget::updateSize( QAction *size )
{
    m_penSize = size->property( "size" ).toInt();
    m_draw = false;
}

void KPrPresentationDrawWidget::updateSize(int size)
{
    m_penSize=size;
    m_draw=false;
}

void KPrPresentationDrawWidget::updateColor( QAction *color )
{
    m_penColor = color->property( "color" ).value<QColor>();
    m_draw = false;
}

void KPrPresentationDrawWidget::updateColor(const QString &color)
{
    m_penColor.setNamedColor(color);
    m_draw=false;
}
