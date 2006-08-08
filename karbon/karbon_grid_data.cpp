/*
 * Karbon - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "karbon_grid_data.h"
#include <KoViewConverter.h>
#include <qdom.h>
#include <QPainter>
#include <QRectF>

KarbonGridData::KarbonGridData()
    : m_spacing( 14.173, 14.173 ), m_snap( 20, 20 ), m_color( 228, 228, 228 )
    , m_isSnap( false ), m_isShow( false )
{
}

KarbonGridData::~KarbonGridData()
{
}

void KarbonGridData::save(QDomElement& /*element*/, const QString& /*name*/)
{
/*  Karbon::saveSize(element, name + "Freg", freq);
  Karbon::saveSize(element, name + "Snap", snap);
  XmlWriteColor(element, name + "Color", color);
  element.setAttribute(name + "IsSnap", (int)isSnap);
  element.setAttribute(name + "IsShow", (int)isShow);*/
}

void KarbonGridData::load(const QDomElement& /*element*/, const QString& /*name*/)
{
/*  QSizeF sdef;
  sdef = QSizeF(10.0, 10.0);
  freq = Karbon::loadSize(element, name + "Freg", sdef);
  snap = Karbon::loadSize(element, name + "Snap", sdef);

  QColor def(QColor(228, 228, 228));
  color = XmlReadColor(element, name + "Color", def);

  isSnap = (bool)element.attribute(name + "IsSnap", "1").toInt();
  isShow = (bool)element.attribute(name + "IsShow", "1").toInt();*/
}

void KarbonGridData::setSpacing( double spacingX, double spacingY )
{
    m_spacing.setWidth( spacingX );
    m_spacing.setHeight( spacingY );
}

void KarbonGridData::spacing( double *spacingX, double *spacingY )
{
    if( spacingX ) 
        *spacingX = m_spacing.width();
    if( spacingY )
        *spacingY = m_spacing.height();
}

void KarbonGridData::setSnap( double snapX, double snapY )
{
    m_snap.setWidth( snapX );
    m_snap.setHeight( snapY );
}

void KarbonGridData::snap( double *snapX, double *snapY )
{
    if( snapX ) 
        *snapX = m_snap.width();
    if( snapY )
        *snapY = m_snap.height();
}

void KarbonGridData::paint( QPainter &painter, const KoViewConverter &converter, const QRectF &area )
{
    painter.setPen( m_color );
    double x = 0.0;
    do {
        painter.drawLine( converter.documentToView( QPointF( x, area.top() ) ), 
                          converter.documentToView( QPointF( x, area.bottom() ) ) );
        x += m_spacing.width();
    } while( x <= area.right() );

    x = - m_spacing.width();
    while( x >= area.left() )
    {
        painter.drawLine( converter.documentToView( QPointF( x, area.top() ) ),
                          converter.documentToView( QPointF( x, area.bottom() ) ) );
        x -= m_spacing.width();
    };

    double y = 0.0;
    do {
        painter.drawLine( converter.documentToView( QPointF( area.left(), y ) ), 
                          converter.documentToView( QPointF( area.right(), y ) ) );
        y += m_spacing.height();
    } while( y <= area.bottom() );

    y = - m_spacing.width();
    while( y >= area.top() )
    {
        painter.drawLine( converter.documentToView( QPointF( area.left(), y ) ), 
                          converter.documentToView( QPointF( area.right(), y ) ) );
        y -= m_spacing.width();
    };
}
