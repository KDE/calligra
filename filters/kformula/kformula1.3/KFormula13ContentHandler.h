/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULA13CONTENTHANDLER_H
#define KFORMULA13CONTENTHANDLER_H

#include <QXmlDefaultHandler>
#include <QDomDocument>

class KFormula13ContentHandler : public QXmlDefaultHandler {
public:
    KFormula13ContentHandler();
    ~KFormula13ContentHandler();

    bool startDocument();
    bool startElement( const QString& namespaceURI, const QString& localName,
                       const QString& qName, const QXmlAttributes& atts );
    bool endElement( const QString& namespaceURI, const QString & localName,
                     const QString& qName );
    bool characters( const QString& ch ); 
    QByteArray mathMlData() const;
    
private:
    void writeBracketAttributes( const QXmlAttributes& atts );
    
    QDomDocument m_mathMlDocument;
    
    QDomElement m_currentElement;
};
    
#endif // KFORMULA13CONTENTHANDLER_H
