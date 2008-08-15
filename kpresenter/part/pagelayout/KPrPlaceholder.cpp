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
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoUnit.h>
#include <KoPASavingContext.h>

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
        m_relativeSize.setX( percent( element, "x", pageSize.x() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "y" ) ) {
        m_relativeSize.setY( percent( element, "y", pageSize.y() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "width" ) ) {
        m_relativeSize.setWidth( percent( element, "width", pageSize.width() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "height" ) ) {
        m_relativeSize.setHeight( percent( element, "height", pageSize.height() ) );
    }
    return true;
}

void KPrPlaceholder::saveOdf( KoXmlWriter & xmlWriter )
{
    xmlWriter.startElement( "presentation:placeholder" );
    xmlWriter.addAttribute( "presentation:object", m_presentationObject );
    xmlWriter.addAttribute( "svg:x", QString( "%1%%" ).arg( m_relativeSize.x() ) );
    xmlWriter.addAttribute( "svg:y", QString( "%1%%" ).arg( m_relativeSize.y() ));
    xmlWriter.addAttribute( "svg:width", QString( "%1%%" ).arg( m_relativeSize.width() ));
    xmlWriter.addAttribute( "svg:height", QString( "%1%%" ).arg( m_relativeSize.height() ));
}

QString KPrPlaceholder::presentationObject()
{
    return m_presentationObject;
}

QRectF KPrPlaceholder::size( const QRectF & pageSize )
{
    QRectF s;
    s.setX( pageSize.width() * m_relativeSize.x() );
    s.setY( pageSize.height() * m_relativeSize.y() );
    s.setWidth( pageSize.width() * m_relativeSize.width() );
    s.setY( pageSize.height() * m_relativeSize.height() );
    return s;
}

qreal KPrPlaceholder::percent( const KoXmlElement & element, const char * type, qreal absolute )
{
    qreal tmp = 0.0;
    QString value = element.attributeNS( KoXmlNS::svg, type, QString( "0%" ) );
    if ( value.indexOf( '%' ) > -1 ) { // percent value
        tmp = value.remove( '%' ).toDouble();
    }
    else { // fixed value
        tmp = absolute / KoUnit::parseValue( value );
    }

    return tmp;
}
