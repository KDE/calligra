/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrPlaceholderShape.h"

#include <QPainter>
#include <KoViewConverter.h>

KPrPlaceholderShape::KPrPlaceholderShape()
{
}

KPrPlaceholderShape::~KPrPlaceholderShape()
{
}

void KPrPlaceholderShape::paint( QPainter &painter, const KoViewConverter &converter )
{
    QRectF target = converter.documentToView( QRectF( QPointF( 0,0 ), size() ) );
    QPen pen( Qt::gray );
    pen.setStyle( Qt::DashLine );
    painter.setPen( pen );
    painter.drawRect( target );
}

bool KPrPlaceholderShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context )
{
    // TODO
    loadOdfAttributes( element, context, OdfAllAttributes );
    Q_UNUSED( element );
    Q_UNUSED( context );
    return true;
}

void KPrPlaceholderShape::saveOdf( KoShapeSavingContext & context ) const
{
    // TODO
    Q_UNUSED( context );
}
