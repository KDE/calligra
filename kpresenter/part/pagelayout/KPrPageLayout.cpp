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

#include <KoXmlReader.h>
#include <KoXmlNS.h>
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
#if 0
            KPrPlaceholder * placeholder = new KPrPlaceholder;
            if ( placeholder->loadOdf( child, loadingContext ) ) {
                m_placeholders.append( placeholder );
            }
            else {
                // TODO warning
                delete placeholder;
            }
#endif
        }
    }
    return true;
}

void KPrPageLayout::saveOdf( KoPASavingContext & context )
{
}
