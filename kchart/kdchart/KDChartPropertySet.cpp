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
#include "KDChartRingPainter.h"
#include "KDChartParams.h"

#include <qpainter.h>
#include <qvaluestack.h>

#include <stdlib.h>

/**
  \file KDChartPropertySet.cpp

  \brief Implementation of property settings of a single KDChart data cell.
  */

/**
  Copy Constructor.
  */
KDChartPropertySet& KDChartPropertySet::operator=( const KDChartPropertySet& R )
{
    mOwnID = R.mOwnID;
    mName  = R.mName;
    mIdShowBar           = R.mIdShowBar;           mShowBar           = R.mShowBar;
    mIdBarColor          = R.mIdBarColor;          mBarColor          = R.mBarColor;
    mIdLineWidth         = R.mIdLineWidth;         mLineWidth         = R.mLineWidth;
    mIdLineColor         = R.mIdLineColor;         mLineColor         = R.mLineColor;
    mIdLineStyle         = R.mIdLineStyle;         mLineStyle         = R.mLineStyle;
    mIdShowMarker        = R.mIdShowMarker;        mShowMarker        = R.mShowMarker;
    mIdExtraLinesAlign   = R.mIdExtraLinesAlign;   mExtraLinesAlign   = R.mExtraLinesAlign;
    mIdExtraLinesInFront = R.mIdExtraLinesInFront; mExtraLinesInFront = R.mExtraLinesInFront;
    mIdExtraLinesLength  = R.mIdExtraLinesLength;  mExtraLinesLength  = R.mExtraLinesLength;
    mIdExtraLinesWidth   = R.mIdExtraLinesWidth;   mExtraLinesWidth   = R.mExtraLinesWidth;
    mIdExtraLinesColor   = R.mIdExtraLinesColor;   mExtraLinesColor   = R.mExtraLinesColor;
    mIdExtraLinesStyle   = R.mIdExtraLinesStyle;   mExtraLinesStyle   = R.mExtraLinesStyle;
    mIdExtraMarkersAlign = R.mIdExtraMarkersAlign; mExtraMarkersAlign = R.mExtraMarkersAlign;
    mIdExtraMarkersSize  = R.mIdExtraMarkersSize;  mExtraMarkersSize  = R.mExtraMarkersSize;
    mIdExtraMarkersColor = R.mIdExtraMarkersColor; mExtraMarkersColor = R.mExtraMarkersColor;
    mIdExtraMarkersStyle = R.mIdExtraMarkersStyle; mExtraMarkersStyle = R.mExtraMarkersStyle;
    return *this;
};


void KDChartPropertySet::fillValueMembersWithDummyValues()
{
    // fill with dummy values to avoid problems when saving us into a stream
    mLineWidth         = 1;
    mLineColor         = Qt::black;
    mLineStyle         = Qt::SolidLine;
    mShowMarker        = true;
    mExtraLinesAlign   = Qt::AlignLeft|Qt::AlignTop;
    mExtraLinesInFront = false;
    mExtraLinesLength  = -20;
    mExtraLinesWidth   = 1;
    mExtraLinesColor   = Qt::black;
    mExtraLinesStyle   = Qt::SolidLine;
    mExtraMarkersAlign = Qt::AlignLeft|Qt::AlignTop;
    mExtraMarkersSize  = QSize(6,6);
    mExtraMarkersColor = Qt::black;
    mExtraMarkersStyle = 0;
    mShowBar           = true;
    mBarColor          = Qt::black;
}


QDomElement KDChartPropertySet::saveXML(QDomDocument& doc) const
{
    QDomElement propertySetElement = doc.createElement( "PropertySet" );
    propertySetElement.setAttribute( "OwnID", mOwnID );
    KDXML::createStringNode( doc, propertySetElement, "Name", mName );
    // normal bar properties:
    KDXML::createIntNode(    doc, propertySetElement, "IDShowBar",    mIdShowBar   );
    KDXML::createBoolNode(   doc, propertySetElement,   "ShowBar",      mShowBar   );
    KDXML::createIntNode(    doc, propertySetElement, "IDBarColor",   mIdBarColor  );
    KDXML::createColorNode(  doc, propertySetElement,   "BarColor",     mBarColor  );
    // normal line properties:
    KDXML::createIntNode(    doc, propertySetElement, "IDLineWidth",  mIdLineWidth );
    KDXML::createIntNode(    doc, propertySetElement,   "LineWidth",    mLineWidth );
    KDXML::createIntNode(    doc, propertySetElement, "IDLineColor",  mIdLineColor );
    KDXML::createColorNode(  doc, propertySetElement,   "LineColor",    mLineColor );
    KDXML::createIntNode(    doc, propertySetElement, "IDLineStyle",  mIdLineStyle );
    QDomElement lineStyleElement = doc.createElement(   "LineStyle" );
    propertySetElement.appendChild( lineStyleElement );
    lineStyleElement.setAttribute( "Style", KDXML::penStyleToString(    mLineStyle));
    // normal marker properties:
    KDXML::createIntNode(    doc, propertySetElement, "IDShowMarker", mIdShowMarker);
    KDXML::createBoolNode(   doc, propertySetElement,   "ShowMarker",   mShowMarker);
    // extra lines:
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraLinesAlign",  mIdExtraLinesAlign );
    KDXML::createIntNode(    doc, propertySetElement,   "ExtraLinesAlign",    mExtraLinesAlign );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraLinesInFront",mIdExtraLinesInFront );
    KDXML::createBoolNode(   doc, propertySetElement,   "ExtraLinesInFront",  mExtraLinesInFront );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraLinesLength", mIdExtraLinesLength );
    KDXML::createIntNode(    doc, propertySetElement,   "ExtraLinesLength",   mExtraLinesLength );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraLinesWidth",  mIdExtraLinesWidth );
    KDXML::createIntNode(    doc, propertySetElement,   "ExtraLinesWidth",    mExtraLinesWidth );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraLinesColor",  mIdExtraLinesColor );
    KDXML::createColorNode(  doc, propertySetElement,   "ExtraLinesColor",    mExtraLinesColor );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraLinesStyle",  mIdExtraLinesStyle );
    QDomElement specLineStElem = doc.createElement(     "ExtraLinesStyle" );
    propertySetElement.appendChild( specLineStElem );
    specLineStElem.setAttribute( "Style", KDXML::penStyleToString(           mExtraLinesStyle));
    // extra markers:
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraMarkersAlign", mIdExtraMarkersAlign );
    KDXML::createIntNode(    doc, propertySetElement,   "ExtraMarkersAlign",   mExtraMarkersAlign );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraMarkersSize",  mIdExtraMarkersSize   );
    KDXML::createSizeNode(   doc, propertySetElement,   "ExtraMarkersSize",    mExtraMarkersSize  );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraMarkersColor", mIdExtraMarkersColor );
    KDXML::createColorNode(  doc, propertySetElement,   "ExtraMarkersColor",   mExtraMarkersColor );
    KDXML::createIntNode(    doc, propertySetElement, "IDExtraMarkersStyle", mIdExtraMarkersStyle );
    QDomElement specMarkerStElem = doc.createElement(   "ExtraMarkersStyle" );
    propertySetElement.appendChild( specMarkerStElem );
    specMarkerStElem.setAttribute("Style",
            KDChartParams::lineMarkerStyleToString(    (KDChartParams::LineMarkerStyle)mExtraMarkersStyle));
    return propertySetElement;
};

bool KDChartPropertySet::loadXML( const QDomElement& element, KDChartPropertySet& set )
{
    bool bOwnIDFound = false;
    QString s;
    QColor color;
    QSize size;
    bool bValue;
    int i;
    // initialize the property set with default values
    set = KDChartPropertySet("");
    // parse the element
    if( element.hasAttribute("OwnID") ){
        i = element.attribute( "OwnID" ).toInt( &bOwnIDFound );
        if( bOwnIDFound ){
            set.mOwnID = i;
            QDomNode node = element.firstChild();
            while( !node.isNull() ) {
                QDomElement element = node.toElement();
                if( !element.isNull() ) { // was really an element
                    QString tagName = element.tagName();
                    if( tagName == "Name" ) {
                        if( KDXML::readStringNode( element, s ) )
                            set.mName = s;
                    } else
                    // normal bar properties:
                        if( tagName == "IDShowBar" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdShowBar = i;
                    } else if( tagName ==   "ShowBar" ) {
                        if( KDXML::readBoolNode( element, bValue ) )
                            set.mShowBar = bValue;
                    } else if( tagName == "IDBarColor" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdBarColor = i;
                    } else if( tagName ==   "BarColor" ) {
                        if( KDXML::readColorNode( element, color ) )
                            set.mBarColor = color;
                    } else
                    // normal line properties:
                        if( tagName == "IDLineWidth" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdLineWidth = i;
                    } else if( tagName ==   "LineWidth" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mLineWidth = i;
                    } else if( tagName == "IDLineColor" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdLineColor = i;
                    } else if( tagName ==   "LineColor" ) {
                        if( KDXML::readColorNode( element, color ) )
                            set.mLineColor = color;
                    } else if( tagName == "IDLineStyle" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdLineStyle = i;
                    } else if( tagName ==   "LineStyle" ) {
                        if( element.hasAttribute( "Style" ) )
                            set.mLineStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                    } else
                        // normal marker properties:
                        if( tagName == "IDShowMarker" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdShowMarker = i;
                    } else if( tagName ==   "ShowMarker" ) {
                        if( KDXML::readBoolNode( element, bValue ) )
                            set.mShowMarker = bValue;
                    } else
                        // extra lines:
                        if( tagName == "IDExtraLinesAlign" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraLinesAlign = i;
                    } else if( tagName ==   "ExtraLinesAlign" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mExtraLinesAlign = i;
                    } else if( tagName == "IDExtraLinesInFront" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraLinesInFront = i;
                    } else if( tagName ==   "ExtraLinesInFront" ) {
                        if( KDXML::readBoolNode( element, bValue ) )
                            set.mExtraLinesInFront = bValue;
                    } else if( tagName == "IDExtraLinesLength" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraLinesLength = i;
                    } else if( tagName ==   "ExtraLinesLength" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mExtraLinesLength = i;
                    } else if( tagName == "IDExtraLinesWidth" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraLinesWidth = i;
                    } else if( tagName ==   "ExtraLinesWidth" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mExtraLinesWidth = i;
                    } else if( tagName == "IDExtraLinesColor" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraLinesColor = i;
                    } else if( tagName ==   "ExtraLinesColor" ) {
                        if( KDXML::readColorNode( element, color ) )
                            set.mExtraLinesColor = color;
                    } else if( tagName == "IDExtraLinesStyle" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraLinesStyle = i;
                    } else if( tagName ==   "ExtraLinesStyle" ) {
                        if( element.hasAttribute( "Style" ) )
                            set.mExtraLinesStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                    } else
                        // extra markers:
                        if( tagName == "IDExtraMarkersAlign" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraMarkersAlign = i;
                    } else if( tagName ==   "ExtraMarkersAlign" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mExtraMarkersAlign = i;
                    } else if( tagName == "IDExtraMarkersSize" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraMarkersSize = i;
                    } else if( tagName ==   "ExtraMarkersSize" ) {
                        if( KDXML::readSizeNode( element, size ) )
                            set.mExtraMarkersSize = size;
                    } else if( tagName == "IDExtraMarkersColor" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraMarkersColor = i;
                    } else if( tagName ==   "ExtraMarkersColor" ) {
                        if( KDXML::readColorNode( element, color ) )
                            set.mExtraMarkersColor = color;
                    } else if( tagName == "IDExtraMarkersStyle" ) {
                        if( KDXML::readIntNode( element, i ) )
                            set.mIdExtraMarkersStyle = i;
                    } else if( tagName ==   "ExtraMarkersStyle" ) {
                        if( element.hasAttribute( "Style" ) )
                            set.mExtraMarkersStyle
                                = KDChartParams::stringToLineMarkerStyle( element.attribute( "Style" ) );
                    } else {
                        qDebug( "Unknown subelement of KDChartPropertySet found: %s", tagName.latin1() );
                    }
                }
                node = node.nextSibling();
            }
        }
    }
    return bOwnIDFound;
}

