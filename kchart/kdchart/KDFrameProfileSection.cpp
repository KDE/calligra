/* -*- Mode: C++ -*-

  $Id$

  KDFrame - a multi-platform framing engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

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
    Direction tempDirection;
    Curvature tempCurvature;
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
