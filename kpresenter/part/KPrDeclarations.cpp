/* This file is part of the KDE project
*  Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*
*  Contact: Amit Aggarwal <amitcs06@gmail.com> 
*            <amit.5.aggarwal@nokia.com>

*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Library General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.

*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Library General Public License for more details.

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

bool KPrDeclarations::loadOdfDeclaration( const KoXmlElement & body, KoPALoadingContext & context )
{
    Q_UNUSED( context );

    KoXmlElement element;
    forEachElement( element, body ) {
        if (element.namespaceURI() == KoXmlNS::presentation) {
            if (element.tagName() == "header-decl") {
                QHash<QString /*name*/, QString /*text*/>declElement;
                const QString name = element.attributeNS(KoXmlNS::presentation, "name", QString());
                declElement.insert(name, element.text()); 
                declaration.insertMulti(HeaderType, declElement);
            } 
            else if(element.tagName() == "footer-decl") {
                QHash<QString /*name*/, QString /*text*/>declElement;
                const QString name = element.attributeNS(KoXmlNS::presentation, "name", QString());
                declElement.insert(name, element.text()); 
                declaration.insertMulti(FooterType, declElement);
            } 
            else if(element.tagName() == "date-time-decl") {
                QHash<QString /*name*/, QString /*text*/>declElement;
                const QString name = element.attributeNS(KoXmlNS::presentation, "name", QString());
                declElement.insert(name, element.text()); 
                declaration.insertMulti(DateTimeType, declElement);
            }
        } 
        else if (element.tagName() == "page" && element.namespaceURI() == KoXmlNS::draw) {
            break;
        }
    }
    return true;
}

const QString KPrDeclarations::findOdfDeclarationText(DeclarationType type, const QString& declName)
{
    const QString text = declaration.value(type).value(declName);
    return text;
}
