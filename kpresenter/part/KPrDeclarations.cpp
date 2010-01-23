/* This file is part of the KDE project
*  Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*
*  Contact: Amit Aggarwal <amitcs06@gmail.com> 
*            <amit.5.aggarwal@nokia.com>
*
*  Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Library General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Library General Public License for more details.
*
*  You should have received a copy of the GNU Library General Public License
*  along with this library; see the file COPYING.LIB.  If not, write to
*  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
*  Boston, MA 02110-1301, USA.
*/
#include "KPrDeclarations.h"

#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoPALoadingContext.h>

KPrDeclarations::KPrDeclarations()
{
}

KPrDeclarations::~KPrDeclarations()
{
}

bool KPrDeclarations::loadOdf(const KoXmlElement &body, KoPALoadingContext &context)
{
    Q_UNUSED(context);

    KoXmlElement element;
    forEachElement( element, body ) {
        if (element.namespaceURI() == KoXmlNS::presentation) {
            if (element.tagName() == "header-decl") {
                const QString name = element.attributeNS(KoXmlNS::presentation, "name", QString());
                m_declarations[Header].insert(name, element.text());
            }
            else if(element.tagName() == "footer-decl") {
                const QString name = element.attributeNS(KoXmlNS::presentation, "name", QString());
                m_declarations[Footer].insert(name, element.text());
            }
            else if(element.tagName() == "date-time-decl") {
                const QString name = element.attributeNS(KoXmlNS::presentation, "name", QString());
                m_declarations[DateTime].insert(name, element.text());
            }
        }
        else if (element.tagName() == "page" && element.namespaceURI() == KoXmlNS::draw) {
            break;
        }
    }
    return true;
}

const QString KPrDeclarations::declaration(Type type, const QString &key)
{
    return m_declarations.value(type).value(key);
}
