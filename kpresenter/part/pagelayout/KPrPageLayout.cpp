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

#include "KPrPageLayout.h"

#include <QBuffer>
#include <kdebug.h>

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoPASavingContext.h>
#include "KPrPlaceholder.h"

KPrPageLayout::KPrPageLayout()
{
}

KPrPageLayout::~KPrPageLayout()
{
    qDeleteAll( m_placeholders );
}

bool KPrPageLayout::loadOdf( const KoXmlElement &element, KoPALoadingContext &loadingContext )
{
    if ( element.hasAttributeNS( KoXmlNS::style, "display-name" ) ) {
        m_name = element.attributeNS( KoXmlNS::style, "display-name" );
    }
    else {
        m_name = element.attributeNS( KoXmlNS::style, "name" );
    }

    KoXmlElement child;
    forEachElement( child, element ) {
        if ( child.tagName() == "placeholder" && element.namespaceURI() == KoXmlNS::presentation ) {
            KPrPlaceholder * placeholder = new KPrPlaceholder;
            if ( placeholder->loadOdf( child, QRectF( 0, 0, 100, 100 ) /* TODO */ ) ) {
                m_placeholders.append( placeholder );
            }
            else {
                kWarning(33000) << "loading placeholder failed";
                delete placeholder;
            }
        }
        else {
            kWarning(33000) << "unknown tag" << child.tagName() << "when loading page layout";
        }
    }

    bool retval = true;
    if ( m_placeholders.isEmpty() ) {
        kWarning(33000) << "no placehoslders for page layout" << m_name << "found";
        retval = false;
    }
    return retval;
}

QString KPrPageLayout::saveOdf( KoPASavingContext & context ) const
{
    KoGenStyle style( KoGenStyle::StylePresentationPageLayout );
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    KoXmlWriter elementWriter( &buffer );
    QList<KPrPlaceholder *>::const_iterator it( m_placeholders.begin() );
    for ( ; it != m_placeholders.end(); ++it ) {
        ( *it )->saveOdf( elementWriter );
    }

    QString placeholders = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    style.addChildElement( "placeholders", placeholders );

    // return the style name so we can save the ptr -> style in the saving context so the pages can use it during saving
    return context.mainStyles().lookup( style, "pl" );
}
