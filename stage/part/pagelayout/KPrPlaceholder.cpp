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

#include "StageDebug.h"
#include "KPrPlaceholderStrategy.h"

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
        if ( ! KPrPlaceholderStrategy::supported( m_presentationObject ) ) {
            debugStage << "unsupported presentation:object" << m_presentationObject;
            return false;
        }
    }
    else {
        warnStage << "no presentation:object found in placeholder";
        return false;
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "x" ) ) {
        m_relativeSize.setX( percent( element, "x", pageSize.width() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "y" ) ) {
        m_relativeSize.setY( percent( element, "y", pageSize.height() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "width" ) ) {
        m_relativeSize.setWidth( percent( element, "width", pageSize.width() ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::svg, "height" ) ) {
        m_relativeSize.setHeight( percent( element, "height", pageSize.height() ) );
    }

    debugStage << "convert" << pageSize << m_relativeSize;

    return true;
}

void KPrPlaceholder::saveOdf( KoXmlWriter & xmlWriter )
{
    xmlWriter.startElement( "presentation:placeholder" );
    xmlWriter.addAttribute( "presentation:object", m_presentationObject );
    xmlWriter.addAttribute( "svg:x", QString( "%1%" ).arg( m_relativeSize.x() * 100.0 ) );
    xmlWriter.addAttribute( "svg:y", QString( "%1%" ).arg( m_relativeSize.y() * 100.0 ));
    xmlWriter.addAttribute( "svg:width", QString( "%1%" ).arg( m_relativeSize.width() * 100.0 ));
    xmlWriter.addAttribute( "svg:height", QString( "%1%" ).arg( m_relativeSize.height() * 100.0 ));
    xmlWriter.endElement();
}

QString KPrPlaceholder::presentationObject() const
{
    return m_presentationObject;
}

QRectF KPrPlaceholder::rect( const QSizeF & pageSize )
{
    QRectF r;
    r.setX( pageSize.width() * m_relativeSize.x() );
    r.setY( pageSize.height() * m_relativeSize.y() );
    r.setWidth( pageSize.width() * m_relativeSize.width() );
    r.setHeight( pageSize.height() * m_relativeSize.height() );
    return r;
}

void KPrPlaceholder::fix( const QRectF & rect )
{
    if ( m_relativeSize.width() < 0 ) {
        m_relativeSize.setWidth( rect.width() );
    }
    if ( m_relativeSize.height() < 0 ) {
        m_relativeSize.setHeight( rect.height() );
    }
}

qreal KPrPlaceholder::percent( const KoXmlElement & element, const char * type, qreal absolute )
{
    qreal tmp = 0.0;
    QString value = element.attributeNS( KoXmlNS::svg, type, QString( "0%" ) );
    if ( value.indexOf( '%' ) > -1 ) { // percent value
        tmp = value.remove( '%' ).toDouble() / 100.0;
    }
    else { // fixed value
        tmp = KoUnit::parseValue( value ) / absolute;
        // The following is done so that we get the same data as when we save/load the placeholder.
        // This is needed that we don't get the same layout twice in the docker due to rounding
        // differences of absolute and relative placeholders of the same layout
        QString value = QString( "%1" ).arg( tmp * 100.0 );
        tmp = value.toDouble() / 100;
    }

    return tmp;
}

bool KPrPlaceholder::operator==( const KPrPlaceholder & other ) const
{
    return m_presentationObject == other.m_presentationObject && m_relativeSize == other.m_relativeSize;
}

bool KPrPlaceholder::operator<( const KPrPlaceholder & other ) const
{
    if ( m_presentationObject == other.m_presentationObject ) {
        return comparePosition( *this, other );
    }
    return m_presentationObject < other.m_presentationObject;
}

bool KPrPlaceholder::comparePosition( const KPrPlaceholder & p1, const KPrPlaceholder & p2 )
{
    if ( p1.m_relativeSize.x() == p2.m_relativeSize.x() ) {
        if ( p1.m_relativeSize.y() == p2.m_relativeSize.y() ) {
            if ( p1.m_relativeSize.width() == p2.m_relativeSize.width() ) {
                return p1.m_relativeSize.height() < p2.m_relativeSize.height();
            }
            return p1.m_relativeSize.width() < p2.m_relativeSize.width();
        }
        return p1.m_relativeSize.y() < p2.m_relativeSize.y();
    }
    return p1.m_relativeSize.x() < p2.m_relativeSize.x();
}

