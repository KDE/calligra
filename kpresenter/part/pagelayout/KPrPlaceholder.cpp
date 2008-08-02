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

#include "KPrPlaceholder.h"

#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoUnit.h>

KPrPlaceholder::KPrPlaceholder()
{
}

KPrPlaceholder::~KPrPlaceholder()
{
}

bool KPrPlaceholder::loadOdf( const KoXmlElement & element, const QRectF & pageSize )
{
    if ( element.hasAttributeNS( KoXmlNS::presentation, "object" ) ) {
        m_presentationObject = element.attributeNS( KoXmlNS::presentation, "object" );
    }
    else {
        // TODO error message
        return false;
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "x" ) ) {
        m_relativePos.setX( percent( element, "x", pageSize.x() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "y" ) ) {
        m_relativePos.setY( percent( element, "y", pageSize.y() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "width" ) ) {
        m_relativePos.setWidth( percent( element, "width", pageSize.width() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "height" ) ) {
        m_relativePos.setHeight( percent( element, "height", pageSize.height() ) );
    }
    return true;
}

void KPrPlaceholder::saveOdf( KoPASavingContext & context )
{
}

QString KPrPlaceholder::presentationObject()
{
}

QRectF KPrPlaceholder::position( const QRectF & pageSize )
{
}

qreal KPrPlaceholder::percent( const KoXmlElement & element, const char * type, qreal absolute )
{
    qreal tmp = 0.0;
    QString value = element.attributeNS( KoXmlNS::svg, type );
    if ( value.indexOf( '%' ) > -1 ) { // percent value
        tmp = value.remove( '%' ).toDouble();
    }
    else { // fixed value
        tmp = absolute / KoUnit::parseValue( value );
    }

    return tmp;
}
