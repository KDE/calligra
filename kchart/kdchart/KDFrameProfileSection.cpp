/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include <KDFrameProfileSection.h>
#include <KDXMLTools.h>

KDFrameProfileSection::~KDFrameProfileSection()
{
    // Intentionally left blank for now.
}


void KDFrameProfileSection::createFrameProfileSectionNode( QDomDocument& document,
        QDomNode& parent,
        const QString& elementName,
        const KDFrameProfileSection* section )

{
    QDomElement sectionElement = document.createElement( elementName );
    parent.appendChild( sectionElement );
    KDXML::createStringNode( document, sectionElement, "Direction",
            KDFrameProfileSection::directionToString( section->_direction ) );
    KDXML::createStringNode( document, sectionElement, "Curvature",
            KDFrameProfileSection::curvatureToString( section->_curvature ) );
    KDXML::createIntNode( document, sectionElement, "Width", section->_width );
    KDXML::createPenNode( document, sectionElement, "Style", section->_pen );
}


bool KDFrameProfileSection::readFrameProfileSectionNode( const QDomElement& element,
        KDFrameProfileSection* section )
{
    bool ok = true;
    Direction tempDirection = DirPlain;
    Curvature tempCurvature = CvtPlain;
    int tempWidth;
    QPen tempPen;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Direction" ) {
                QString value;
                ok = ok & KDXML::readStringNode( element, value );
                tempDirection = stringToDirection( value );
            } else if( tagName == "Curvature" ) {
                QString value;
                ok = ok & KDXML::readStringNode( element, value );
                tempCurvature = stringToCurvature( value );
            } else if( tagName == "Width" ) {
                ok = ok & KDXML::readIntNode( element, tempWidth );
            } else if( tagName == "Pen" ) {
                ok = ok & KDXML::readPenNode( element, tempPen );
            } else {
                qDebug( "Unknown tag in frame" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
        section->_direction = tempDirection;
        section->_curvature = tempCurvature;
        section->_width = tempWidth;
        section->_pen = tempPen;
    }

    return ok;
}
