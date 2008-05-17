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
 ** Licensees holding valid commercial KDChart1 licenses may use this file in
 ** accordance with the KDChart1 Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart1 Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include <KDChartAttributesSerializer.h>
#include <KDChartAttributesSerializer_p.h>

#include <KDChartIdMapper.h>
#include <KDChartSerializeCollector.h>

#include <KDChartPosition.h>

#include <KDXMLTools.h>

#include <qglobal.h>
#include <QMessageBox>

#define d d_func()

using namespace KDChart;

AttributesSerializer::Private::Private( AttributesSerializer* qq )
    : q( qq )
{
}

AttributesSerializer::Private::~Private() {}


/**
  \class KDChart::AttributesSerializer KDChartAttributesSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */

AttributesSerializer::AttributesSerializer()
    : _d( new Private( this ) )
{
}

AttributesSerializer::~AttributesSerializer()
{
    delete _d; _d = 0;
}

void AttributesSerializer::init()
{
}

bool AttributesSerializer::parseLeading(
        const QDomElement& container, int& left, int& top, int& right, int& bottom )
{
    const bool bFoundLeft   = KDXML::findIntAttribute( container, "left",   left   );
    const bool bFoundTop    = KDXML::findIntAttribute( container, "top",    top    );
    const bool bFoundRight  = KDXML::findIntAttribute( container, "right",  right  );
    const bool bFoundBottom = KDXML::findIntAttribute( container, "bottom", bottom );
    return bFoundLeft || bFoundTop || bFoundRight || bFoundBottom;
}

void AttributesSerializer::saveLeading(
        QDomDocument& doc,
        QDomElement& e,
        int left, int top, int right, int bottom,
        const QString& title )
{
    QDomElement leadingElement =
        doc.createElement( title );
    e.appendChild( leadingElement );
    leadingElement.setAttribute( "left",   left   );
    leadingElement.setAttribute( "top",    top    );
    leadingElement.setAttribute( "right",  right  );
    leadingElement.setAttribute( "bottom", bottom );
}


bool AttributesSerializer::parseFrameAttributes(
        const QDomElement& container,
        FrameAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Visible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setVisible( b );
            } else if( tagName == "Pen" ) {
                QPen p;
                if( KDXML::readPenNode( element, p ) )
                    a.setPen( p );
            } else if( tagName == "Padding" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setPadding( i );
            } else {
                qDebug() << "Unknown subelement of FrameAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveFrameAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const FrameAttributes& a,
        const QString& title )
{
    QDomElement frameAttributesElement =
            doc.createElement( title );
    e.appendChild( frameAttributesElement );
    KDXML::createBoolNode( doc, frameAttributesElement, "Visible",
                           a.isVisible() );
    KDXML::createPenNode( doc, frameAttributesElement, "Pen",
                          a.pen() );
    KDXML::createIntNode( doc, frameAttributesElement, "Padding",
                          a.padding() );
}


bool AttributesSerializer::parseTextAttributes(
        const QDomElement& container,
        TextAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Visible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setVisible( b );
            } else if( tagName == "Pen" ) {
                QPen p;
                if( KDXML::readPenNode( element, p ) )
                    a.setPen( p );
            } else if( tagName == "Font" ) {
                QFont f;
                if( KDXML::readFontNode( element, f ) )
                    a.setFont( f );
            } else if( tagName == "FontSize" ) {
                Measure m;
                if( parseMeasure( element, m ) )
                    a.setFontSize( m );
            } else if( tagName == "MinimalFontSize" ) {
                Measure m;
                if( parseMeasure( element, m ) )
                    a.setMinimalFontSize( m );
            } else if( tagName == "AutoRotate" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setAutoRotate( b );
            } else if( tagName == "AutoShrink" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setAutoShrink( b );
            } else if( tagName == "Rotation" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setRotation( i );
            } else {
                qDebug() << "Unknown subelement of TextAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveTextAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const TextAttributes& a,
        const QString& title )
{
    QDomElement textAttributesElement =
            doc.createElement( title );
    e.appendChild( textAttributesElement );
    KDXML::createBoolNode( doc, textAttributesElement, "Visible",
                           a.isVisible() );
    KDXML::createPenNode( doc, textAttributesElement, "Pen",
                          a.pen() );
    KDXML::createFontNode( doc, textAttributesElement, "Font",
                           a.font() );
    saveMeasure( doc, textAttributesElement,
                 a.fontSize(), "FontSize" );
    saveMeasure( doc, textAttributesElement,
                 a.minimalFontSize(), "MinimalFontSize" );
    KDXML::createBoolNode( doc, textAttributesElement, "AutoRotate",
                           a.autoRotate() );
    KDXML::createBoolNode( doc, textAttributesElement, "AutoShrink",
                           a.autoShrink() );
    KDXML::createIntNode( doc, textAttributesElement, "Rotation",
                          a.rotation() );
}


bool AttributesSerializer::parseMeasure(
        const QDomElement& container,
        Measure& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "ReferenceArea" ) {
                QObject* ptr;
                QString ptrName;
                bool wasParsed;
                if( parseQObjectPointerNode( element.firstChild(), ptr,
                                             ptrName, wasParsed, true ) )
                    a.setReferenceArea( ptr );
            } else if( tagName == "Value" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setValue( r );
            } else if( tagName == "Mode" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    a.setCalculationMode(
                            KDChartEnums::stringToMeasureCalculationMode( s ) );
            } else if( tagName == "Orientation" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    a.setReferenceOrientation(
                            KDChartEnums::stringToMeasureOrientation( s ) );
            } else {
                qDebug() << "Unknown subelement of Measure found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveMeasure(
        QDomDocument& doc,
        QDomElement& e,
        const Measure& m,
        const QString& title )
{
    QDomElement measureElement =
            doc.createElement( title );
    e.appendChild( measureElement );
    saveQObjectPointer( doc, measureElement, m.referenceArea(), "ReferenceArea" );
    KDXML::createRealNode( doc, measureElement, "Value",
                           m.value() );
    KDXML::createStringNode( doc, measureElement, "Mode",
                             KDChartEnums::measureCalculationModeToString(
                                     m.calculationMode() ) );
    KDXML::createStringNode( doc, measureElement, "Orientation",
                             KDChartEnums::measureOrientationToString(
                                     m.referenceOrientation() ) );
}

bool AttributesSerializer::parseBackgroundAttributes(
        const QDomElement& container,
        BackgroundAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            //qDebug()<<tagName;
            if( tagName == "Visible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setVisible( b );
                else
                    qDebug() << "Error parsing BackgroundAttributes tag: " << tagName;
            } else if( tagName == "Brush" ) {
                QBrush b;
                if( KDXML::readBrushNode( element, b ) )
                    a.setBrush( b );
                else
                    qDebug() << "Error parsing BackgroundAttributes tag: " << tagName;
            } else if( tagName == "PixmapMode" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) ){
                    if( s == "BackgroundPixmapModeNone" )
                        a.setPixmapMode( BackgroundAttributes::BackgroundPixmapModeNone );
                    else if ( s == "BackgroundPixmapModeCentered" )
                        a.setPixmapMode( BackgroundAttributes::BackgroundPixmapModeCentered );
                    else if ( s == "BackgroundPixmapModeScaled" )
                        a.setPixmapMode( BackgroundAttributes::BackgroundPixmapModeScaled );
                    else if ( s == "BackgroundPixmapModeStretched" )
                        a.setPixmapMode( BackgroundAttributes::BackgroundPixmapModeStretched );
                    else{
                        qDebug() << "Unknown PixmapMode found in BackgroundAttributes:" << s;
                        bOK = false;
                    }
                    //qDebug() << s;
                }else{
                    qDebug() << "Error parsing BackgroundAttributes tag: " << tagName;
                }
            } else if( tagName == "Pixmap" ) {
                QPixmap p;
                if( KDXML::readPixmapNode( element, p ) )
                    a.setPixmap( p );
                else
                    qDebug() << "Error parsing BackgroundAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of BackgroundAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveBackgroundAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const BackgroundAttributes& a,
        const QString& title )
{
    QDomElement backgroundAttributesElement =
            doc.createElement( title );
    e.appendChild( backgroundAttributesElement );
    KDXML::createBoolNode( doc, backgroundAttributesElement, "Visible",
                           a.isVisible() );
    KDXML::createBrushNode( doc, backgroundAttributesElement, "Brush",
                            a.brush() );
    BackgroundAttributes::BackgroundPixmapMode mode = a.pixmapMode();
    bool hasPixmap = false;
    QString s;
    switch( mode ){
        case BackgroundAttributes::BackgroundPixmapModeNone:
            s = "BackgroundPixmapModeNone";
            break;
        case BackgroundAttributes::BackgroundPixmapModeCentered:
            hasPixmap = true;
            s = "BackgroundPixmapModeCentered";
            break;
        case BackgroundAttributes::BackgroundPixmapModeScaled:
            hasPixmap = true;
            s = "BackgroundPixmapModeScaled";
            break;
        case BackgroundAttributes::BackgroundPixmapModeStretched:
            hasPixmap = true;
            s = "BackgroundPixmapModeStretched";
            break;
        default:
            s = "BackgroundPixmapModeNone";
            break;
    }
    KDXML::createStringNode( doc, backgroundAttributesElement, "PixmapMode",
                             s );
    if( hasPixmap )
        KDXML::createPixmapNode( doc, backgroundAttributesElement, "Pixmap",
                                 a.pixmap() );
}

bool AttributesSerializer::parseDataValueAttributes(
        const QDomElement& container,
        DataValueAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Visible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setVisible( b );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "TextAttributes" ) {
                TextAttributes attrs;
                if( parseTextAttributes( element, attrs ) )
                    a.setTextAttributes( attrs );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "FrameAttributes" ) {
                FrameAttributes attrs;
                if( parseFrameAttributes( element, attrs ) )
                    a.setFrameAttributes( attrs );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "BackgroundAttributes" ) {
                BackgroundAttributes attrs;
                if( parseBackgroundAttributes( element, attrs ) )
                    a.setBackgroundAttributes( attrs );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "MarkerAttributes" ) {
                MarkerAttributes attrs;
                if( parseMarkerAttributes( element, attrs ) )
                    a.setMarkerAttributes( attrs );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "DecimalDigits" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setDecimalDigits( i );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "Prefix" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    a.setPrefix( s );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "Suffix" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    a.setSuffix( s );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "DataLabel" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    a.setDataLabel( s );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "ShowRepetitiveDataLabels" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setShowRepetitiveDataLabels( b );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "ShowOverlappingDataLabels" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setShowOverlappingDataLabels( b );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "PowerOfTenDivisor" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setPowerOfTenDivisor( i );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "ShowInfinite" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setShowInfinite( b );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "NegativePosition" ) {
                RelativePosition pos;
                if( parseRelativePosition( element, pos ) )
                    a.setNegativePosition( pos );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else if( tagName == "PositivePosition" ) {
                RelativePosition pos;
                if( parseRelativePosition( element, pos ) )
                    a.setPositivePosition( pos );
                else
                    qDebug() << "Error parsing DataValueAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of DataValueAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveDataValueAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const DataValueAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    KDXML::createBoolNode( doc, element, "Visible",
                           a.isVisible() );
    // save the text attributes
    saveTextAttributes(
            doc, element,
            a.textAttributes(),
            "TextAttributes" );
    // save the frame attributes
    saveFrameAttributes(
            doc, element,
            a.frameAttributes(),
            "FrameAttributes" );
    // save the background attributes
    saveBackgroundAttributes(
            doc, element,
            a.backgroundAttributes(),
            "BackgroundAttributes" );
    // save the marker attributes
    saveMarkerAttributes(
            doc, element,
            a.markerAttributes(),
            "MarkerAttributes" );
    // save the number of decimal digits
    KDXML::createIntNode( doc, element, "DecimalDigits", a.decimalDigits() );
    // save the prefix string
    KDXML::createStringNodeIfContent( doc, element, "Prefix", a.prefix() );
    // save the suffix string
    KDXML::createStringNodeIfContent( doc, element, "Suffix", a.suffix() );
    // save the data label
    KDXML::createStringNodeIfContent( doc, element, "DataLabel", a.dataLabel() );
    // save the showRepetitiveDataLabels flag
    KDXML::createBoolNode( doc, element, "ShowRepetitiveDataLabels", a.showRepetitiveDataLabels() );
    // save the showOverlappingDataLabels flag
    KDXML::createBoolNode( doc, element, "ShowOverlappingDataLabels", a.showOverlappingDataLabels() );
    // save the power-of-ten divisor
    KDXML::createIntNode( doc, element, "PowerOfTenDivisor", a.powerOfTenDivisor() );
    // save the showInfinite flag
    KDXML::createBoolNode( doc, element, "ShowInfinite", a.showInfinite() );
    // save the negative positioning attributes
    saveRelativePosition(
            doc, element,
            a.negativePosition(),
            "NegativePosition" );
    // save the positive positioning attributes
    saveRelativePosition(
            doc, element,
            a.positivePosition(),
            "PositivePosition" );
}


bool AttributesSerializer::parseLineAttributes(
        const QDomElement& container, LineAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            //qDebug()<<tagName;
            if( tagName == "MissingValuesPolicy" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) ){
                    if( s == "MissingValuesAreBridged" )
                        a.setMissingValuesPolicy( LineAttributes::MissingValuesAreBridged );
                    else if( s == "MissingValuesHideSegments" )
                        a.setMissingValuesPolicy( LineAttributes::MissingValuesHideSegments );
                    else if( s == "MissingValuesShownAsZero" )
                        a.setMissingValuesPolicy( LineAttributes::MissingValuesShownAsZero );
                    else if( s == "MissingValuesPolicyIgnored" )
                        a.setMissingValuesPolicy( LineAttributes::MissingValuesPolicyIgnored );
                    else{
                        qDebug() << "Unknown MissingValuesPolicy found in LineAttributes:" << s;
                        bOK = false;
                    }
                    //qDebug() << s;
                }else{
                    qDebug() << "Error parsing LineAttributes tag: " << tagName;
                }
            } else if( tagName == "DisplayArea" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setDisplayArea( b );
                else
                    qDebug() << "Error parsing LineAttributes tag: " << tagName;
            } else if( tagName == "Transparency" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setTransparency( i );
                else
                    qDebug() << "Error parsing LineAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of LineAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveLineAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const LineAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );

    QString name;
    switch( a.missingValuesPolicy() ){
        case LineAttributes::MissingValuesAreBridged:
            name = "MissingValuesAreBridged";
            break;
        case LineAttributes::MissingValuesHideSegments:
            name = "MissingValuesHideSegments";
            break;
        case LineAttributes::MissingValuesShownAsZero:
            name = "MissingValuesShownAsZero";
            break;
        case LineAttributes::MissingValuesPolicyIgnored:
            name = "MissingValuesPolicyIgnored";
            break;
        default:
            Q_ASSERT( false ); // all of the types need to be handled
            break;
    }
    KDXML::createStringNode( doc, element, "MissingValuesPolicy", name );
    KDXML::createBoolNode(   doc, element, "DisplayArea",  a.displayArea() );
    KDXML::createIntNode(    doc, element, "Transparency", a.transparency() );
}


bool AttributesSerializer::parseBarAttributes(
        const QDomElement& container, BarAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            //qDebug()<<tagName;
            if( tagName == "FixedDataValueGap" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setFixedDataValueGap( r );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "UseFixedDataValueGap" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setUseFixedDataValueGap( b );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "FixedValueBlockGap" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setFixedValueBlockGap( r );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "UseFixedValueBlockGap" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setUseFixedValueBlockGap( b );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "FixedBarWidth" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setFixedBarWidth( r );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "UseFixedBarWidth" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setUseFixedBarWidth( b );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "GroupGapFactor" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setGroupGapFactor( r );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "BarGapFactor" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setBarGapFactor( r );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else if( tagName == "DrawSolidExcessArrows" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setDrawSolidExcessArrows( b );
                else
                    qDebug() << "Error parsing BarAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of BarAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveBarAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const BarAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    KDXML::createRealNode( doc, element, "FixedDataValueGap",    a.fixedDataValueGap() );
    KDXML::createBoolNode( doc, element, "UseFixedDataValueGap", a.useFixedDataValueGap() );
    KDXML::createRealNode( doc, element, "FixedValueBlockGap",   a.fixedValueBlockGap() );
    KDXML::createBoolNode( doc, element, "UseFixedValueBlockGap",a.useFixedValueBlockGap() );
    KDXML::createRealNode( doc, element, "FixedBarWidth",        a.fixedBarWidth() );
    KDXML::createBoolNode( doc, element, "UseFixedBarWidth",     a.useFixedBarWidth() );
    KDXML::createRealNode( doc, element, "GroupGapFactor", a.groupGapFactor() );
    KDXML::createRealNode( doc, element, "BarGapFactor",   a.barGapFactor() );
    KDXML::createBoolNode( doc, element, "DrawSolidExcessArrows", a.drawSolidExcessArrows() );
}


bool AttributesSerializer::parsePieAttributes(
        const QDomElement& container, PieAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            //qDebug()<<tagName;
            if( tagName == "Explode" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setExplode( b );
                else
                    qDebug() << "Error parsing PieAttributes tag: " << tagName;
            } else if( tagName == "ExplodeFactor" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setExplodeFactor( r );
                else
                    qDebug() << "Error parsing PieAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of PieAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::savePieAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const PieAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    KDXML::createBoolNode( doc, element, "Explode",       a.explode() );
    KDXML::createRealNode( doc, element, "ExplodeFactor", a.explodeFactor() );
}


bool AttributesSerializer::parseAbstractThreeDAttributes(
        const QDomElement& container, AbstractThreeDAttributes& a )
{
    bool bFlag;
    double depth;
    const bool bOK =
            KDXML::findBoolAttribute(   container, "enabled", bFlag ) &&
            KDXML::findDoubleAttribute( container, "depth", depth );
    if( bOK ){
        a.setEnabled( bFlag );
        a.setDepth( depth );
    }
    return bOK;
}

void AttributesSerializer::saveAbstractThreeDAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractThreeDAttributes* a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    KDXML::setBoolAttribute(   element, "enabled", a->isEnabled() );
    KDXML::setDoubleAttribute( element, "depth",   a->depth() );
}


bool AttributesSerializer::parseThreeDBarAttributes(
        const QDomElement& container, ThreeDBarAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            //qDebug()<<tagName;
            if( tagName == "kdchart:abstract-three-D-attributes" ) {
                if( ! parseAbstractThreeDAttributes( element, a ) )
                    qDebug() << "Error parsing ThreeDBarAttributes tag: " << tagName;
            } else if( tagName == "UseShadowColors" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setUseShadowColors( b );
                else
                    qDebug() << "Error parsing ThreeDBarAttributes tag: " << tagName;
            } else if( tagName == "Angle" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setAngle( i );
                else
                    qDebug() << "Error parsing ThreeDBarAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of ThreeDBarAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveThreeDBarAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const ThreeDBarAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    saveAbstractThreeDAttributes( doc, element, &a,
                                  "kdchart:abstract-three-D-attributes" );
    // save the settings
    KDXML::createBoolNode( doc, element, "UseShadowColors",
                           a.useShadowColors() );
    KDXML::createIntNode( doc, element, "Angle", a.angle() );
}


bool AttributesSerializer::parseThreeDLineAttributes(
        const QDomElement& container, ThreeDLineAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            //qDebug()<<tagName;
            if( tagName == "kdchart:abstract-three-D-attributes" ) {
                if( ! parseAbstractThreeDAttributes( element, a ) )
                    qDebug() << "Error parsing ThreeDLineAttributes tag: " << tagName;
            } else if( tagName == "LineXRotation" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setLineXRotation( i );
                else
                    qDebug() << "Error parsing ThreeDLineAttributes tag: " << tagName;
            } else if( tagName == "LineYRotation" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    a.setLineYRotation( i );
                else
                    qDebug() << "Error parsing ThreeDLineAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of ThreeDLineAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveThreeDLineAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const ThreeDLineAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    saveAbstractThreeDAttributes( doc, element, &a,
                                  "kdchart:abstract-three-D-attributes" );
    // save the rotation settings
    KDXML::createIntNode( doc, element, "LineXRotation", a.lineXRotation() );
    KDXML::createIntNode( doc, element, "LineYRotation", a.lineYRotation() );
}


bool AttributesSerializer::parseThreeDPieAttributes(
        const QDomElement& container, ThreeDPieAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            //qDebug()<<tagName;
            if( tagName == "kdchart:abstract-three-D-attributes" ) {
                if( ! parseAbstractThreeDAttributes( element, a ) )
                    qDebug() << "Error parsing ThreeDPieAttributes tag: " << tagName;
            } else if( tagName == "UseShadowColors" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setUseShadowColors( b );
                else
                    qDebug() << "Error parsing ThreeDPieAttributes tag: " << tagName;
            } else {
                qDebug() << "Unknown subelement of ThreeDPieAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveThreeDPieAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const ThreeDPieAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    saveAbstractThreeDAttributes( doc, element, &a,
                                  "kdchart:abstract-three-D-attributes" );
    // save the settings
    KDXML::createBoolNode( doc, element, "UseShadowColors",
                           a.useShadowColors() );
}

MarkerAttributes::MarkerStyle AttributesSerializer::markerStyleFromName( QString name )
{
    MarkerAttributes::MarkerStyle style;
    if( name == "MarkerCircle" )
        style = MarkerAttributes::MarkerCircle;
    else if( name == "MarkerSquare" )
        style = MarkerAttributes::MarkerSquare;
    else if( name == "MarkerDiamond" )
        style = MarkerAttributes::MarkerDiamond;
    else if( name == "Marker1Pixel" )
        style = MarkerAttributes::Marker1Pixel;
    else if( name == "Marker4Pixels" )
        style = MarkerAttributes::Marker4Pixels;
    else if( name == "MarkerRing" )
        style = MarkerAttributes::MarkerRing;
    else if( name == "MarkerCross" )
        style = MarkerAttributes::MarkerCross;
    else if( name == "MarkerFastCross" )
        style = MarkerAttributes::MarkerFastCross;
    else 
        Q_ASSERT( false ); // all of the style types need to be handled
    return style;
}

QString AttributesSerializer::markerStyleToName( MarkerAttributes::MarkerStyle style )
{
    QString name;
    switch( style ){
        case MarkerAttributes::MarkerCircle:
            name = "MarkerCircle";
            break;
        case MarkerAttributes::MarkerSquare:
            name = "MarkerSquare";
            break;
        case MarkerAttributes::MarkerDiamond:
            name = "MarkerDiamond";
            break;
        case MarkerAttributes::Marker1Pixel:
            name = "Marker1Pixel";
            break;
        case MarkerAttributes::Marker4Pixels:
            name = "Marker4Pixels";
            break;
        case MarkerAttributes::MarkerRing:
            name = "MarkerRing";
            break;
        case MarkerAttributes::MarkerCross:
            name = "MarkerCross";
            break;
        case MarkerAttributes::MarkerFastCross:
            name = "MarkerFastCross";
            break;
        default:
            Q_ASSERT( false ); // all of the style types need to be handled
            break;
    }
    return name;
}


bool AttributesSerializer::parseMarkerAttributes(
        const QDomElement& container,
        MarkerAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Visible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setVisible( b );
            } else if( tagName == "MarkerStyle" ) {
                QString s;
                if( KDXML::findStringAttribute( element, "style", s ) ){
                    //qDebug() << "MarkerAttributes/MarkerStyle found: \"" << s << "\"";
                    const MarkerAttributes::MarkerStyle style = markerStyleFromName( s );
                    if( ! s.isEmpty() ){
                        a.setMarkerStyle( style );
                    } else {
                        qDebug() << "Invalid style attribute in MarkerAttributes/MarkerStyle found: \"" << s << "\"";
                        bOK = false;
                    }
                } else {
                    qDebug() << "Invalid style element in MarkerAttributes/MarkerStyle found.";
                    bOK = false;
                }
            } else if( tagName == "StylesMap" ) {
                MarkerAttributes::MarkerStylesMap map;
                QDomNode node2 = element.firstChild();
                while( ! node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( ! ele2.isNull() ) { // was really an element
                        QString tag2 = ele2.tagName();
                        if( tag2 == "MarkerStyle" ){
                            int key;
                            QString style;
                            if( KDXML::findIntAttribute( ele2, "key", key ) &&
                                KDXML::findStringAttribute( ele2, "style", style ) )
                            {
                                map[ key ] = markerStyleFromName( style );
                            } else {
                                qDebug() << "Invalid style element in MarkerAttributes/StylesMap found.";
                                bOK = false;
                            }
                        } else {
                            qDebug() << "Unknown subelement of MarkerAttributes/StylesMap found:" << tag2;
                            bOK = false;
                        }
                    }
                }
                if( bOK )
                    a.setMarkerStylesMap( map );
            } else if( tagName == "Size" ) {
                QSizeF size;
                if( KDXML::readSizeFNode( element, size ) )
                    a.setMarkerSize( size );
            } else if( tagName == "Color" ) {
                QColor c;
                if( KDXML::readColorNode( element, c ) )
                    a.setMarkerColor( c );
                //qDebug() << "---> " << c;
            } else if( tagName == "Pen" ) {
                QPen pen;
                if( KDXML::readPenNode( element, pen ) )
                    a.setPen( pen );
            } else {
                qDebug() << "Unknown subelement of MarkerAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveMarkerAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const MarkerAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );
    KDXML::createBoolNode( doc, element, "Visible",
                           a.isVisible() );
    // save the style
    QDomElement styleElement =
            doc.createElement( "MarkerStyle" );
    element.appendChild( styleElement );
    styleElement.setAttribute( "style", markerStyleToName( a.markerStyle() ) );
    // save the stylesMap
    const MarkerAttributes::MarkerStylesMap map( a.markerStylesMap() );
    if( map.count() ){
        QDomElement mapElement =
                doc.createElement( "StylesMap" );
        element.appendChild( mapElement );
        MarkerAttributes::MarkerStylesMap::const_iterator i = map.constBegin();
        while (i != map.constEnd()) {
            QDomElement styleElement =
                    doc.createElement( "MarkerStyle" );
            mapElement.appendChild( styleElement );
            styleElement.setAttribute( "key", i.key() );
            styleElement.setAttribute( "style", markerStyleToName( i.value() ) );
            ++i;
        }
    }
    // save the size
    KDXML::createSizeFNode( doc, element, "Size", a.markerSize() );
    // save the color
    KDXML::createColorNode( doc, element, "Color", a.markerColor() );
    // save the pen
    KDXML::createPenNode( doc, element, "Pen", a.pen() );
}


bool AttributesSerializer::parseRelativePosition(
        const QDomElement& container,
        RelativePosition& pos )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "ReferenceArea" ) {
                QObject* ptr;
                QString ptrName;
                bool wasParsed;
                if( parseQObjectPointerNode( element.firstChild(), ptr,
                                             ptrName, wasParsed, true ) )
                    pos.setReferenceArea( ptr );
            } else if( tagName == "PositionPoints" ) {
                PositionPoints points;
                QDomNode node2 = element.firstChild();
                while( ! node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( ! ele2.isNull() ) { // was really an element
                        QString tag2 = ele2.tagName();
                        QPointF pt;
                        if( tag2 =="PositionUnknown" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionUnknown = pt;
                        } else if( tag2 =="Center" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionCenter = pt;
                        } else if( tag2 =="NorthWest" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionNorthWest = pt;
                        } else if( tag2 =="North" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionNorth = pt;
                        } else if( tag2 =="NorthEast" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionNorthEast = pt;
                        } else if( tag2 =="East" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionEast = pt;
                        } else if( tag2 =="SouthEast" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionSouthEast = pt;
                        } else if( tag2 =="South" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionSouth = pt;
                        } else if( tag2 =="SouthWest" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionSouthWest = pt;
                        } else if( tag2 =="West" ) {
                            if( KDXML::readPointFNode( ele2, pt ) )
                                points.mPositionWest = pt;
                        } else {
                            qDebug() << "Unknown subelement of RelativePosition/PositionPoints found:" << tag2;
                            bOK = false;
                        }
                    }
                    node2 = node2.nextSibling();
                }
                if( bOK )
                    pos.setReferencePoints( points );
            } else if( tagName == "ReferencePosition" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    pos.setReferencePosition( Position::fromName( s.toLatin1().data() ) );
            } else if( tagName == "Alignment" ) {
                Qt::Alignment a;
                if( KDXML::readAlignmentNode( element, a ) )
                    pos.setAlignment( a );
            } else if( tagName == "HorizontalPadding" ) {
                Measure m;
                if( parseMeasure( element, m ) )
                    pos.setHorizontalPadding( m );
            } else if( tagName == "VerticalPadding" ) {
                Measure m;
                if( parseMeasure( element, m ) )
                    pos.setVerticalPadding( m );
            } else if( tagName == "Rotation" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    pos.setRotation( r );
            } else {
                qDebug() << "Unknown subelement of RelativePosition found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveRelativePosition(
        QDomDocument& doc,
        QDomElement& e,
        const RelativePosition& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );

    // We always store the pointer to the reference area - even if it
    // is Null because it might have been set to Null intentionally, to
    // change it from KD Chart's default setting that e.g. might have
    // been pointing to the Chart.
    //qDebug() << "AttributesSerializer::saveRelativePosition() storing a.referenceArea(): " << a.referenceArea();
    saveQObjectPointer( doc, element, a.referenceArea(), "ReferenceArea" );

    // save the reference points
    const PositionPoints points = a.referencePoints();
    if( ! points.isNull() ){
        QDomElement pointsElement =
                doc.createElement( "PositionPoints" );
        element.appendChild( pointsElement );
        // save the positions
        KDXML::createPointFNode(
                doc, pointsElement, "PositionUnknown", points.point( Position::Unknown ) );
        KDXML::createPointFNode(
                doc, pointsElement, "Center",    points.point( Position::Center ) );
        KDXML::createPointFNode(
                doc, pointsElement, "NorthWest", points.point( Position::NorthWest ) );
        KDXML::createPointFNode(
                doc, pointsElement, "North",     points.point( Position::North ) );
        KDXML::createPointFNode(
                doc, pointsElement, "NorthEast", points.point( Position::NorthEast ) );
        KDXML::createPointFNode(
                doc, pointsElement, "East",      points.point( Position::East ) );
        KDXML::createPointFNode(
                doc, pointsElement, "SouthEast", points.point( Position::SouthEast ) );
        KDXML::createPointFNode(
                doc, pointsElement, "South",     points.point( Position::South ) );
        KDXML::createPointFNode(
                doc, pointsElement, "SouthWest", points.point( Position::SouthWest ) );
        KDXML::createPointFNode(
                doc, pointsElement, "West",      points.point( Position::West ) );
    }
    // save the reference position
    KDXML::createStringNode( doc, element, "ReferencePosition", a.referencePosition().name() );
    // save the alignment
    KDXML::createAlignmentNode(
            doc, element, "Alignment", a.alignment() );
    // save padding
    saveMeasure( doc, element, a.horizontalPadding(), "HorizontalPadding" );
    saveMeasure( doc, element, a.verticalPadding(),   "VerticalPadding" );
    // save the rotation
    KDXML::createRealNode( doc, element, "Rotation", a.rotation() );
}


bool AttributesSerializer::parseGridAttributes(
        const QDomElement& container,
        GridAttributes& a )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "GridVisible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setGridVisible( b );
            } else if( tagName == "GridStepWidth" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setGridStepWidth( r );
            } else if( tagName == "GridSubStepWidth" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) )
                    a.setGridSubStepWidth( r );
            } else if( tagName == "GranularitySequence" ) {
                QString name;
                if( KDXML::readStringNode( element, name ) ){
                    KDChartEnums::GranularitySequence seq;
                    if( name == "10_20" )
                        seq = KDChartEnums::GranularitySequence_10_20;
                    else if( name == "10_50" )
                        seq = KDChartEnums::GranularitySequence_10_50;
                    else if( name == "25_50" )
                        seq = KDChartEnums::GranularitySequence_25_50;
                    else if( name == "125_25" )
                        seq = KDChartEnums::GranularitySequence_125_25;
                    else if( name == "Irregular" )
                        seq = KDChartEnums::GranularitySequenceIrregular;
                    else
                        Q_ASSERT( false ); // all of the values need to be handled
                    a.setGridGranularitySequence( seq );
                }
            }else if( tagName == "AdjustLowerBound" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setAdjustBoundsToGrid( b, a.adjustUpperBoundToGrid() );
            }else if( tagName == "AdjustUpperBound" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setAdjustBoundsToGrid( a.adjustLowerBoundToGrid(), b );
            } else if( tagName == "GridPen" ) {
                QPen pen;
                if( KDXML::readPenNode( element, pen ) )
                    a.setGridPen( pen );
            } else if( tagName == "SubGridVisible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    a.setSubGridVisible( b );
            } else if( tagName == "SubGridPen" ) {
                QPen pen;
                if( KDXML::readPenNode( element, pen ) )
                    a.setSubGridPen( pen );
            } else if( tagName == "ZeroLinePen" ) {
                QPen pen;
                if( KDXML::readPenNode( element, pen ) )
                    a.setZeroLinePen( pen );
            } else {
                qDebug() << "Unknown subelement of GridAttributes found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AttributesSerializer::saveGridAttributes(
        QDomDocument& doc,
        QDomElement& e,
        const GridAttributes& a,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );

    KDXML::createBoolNode( doc, element, "GridVisible",
                           a.isGridVisible() );
    KDXML::createRealNode( doc, element, "GridStepWidth",
                           a.gridStepWidth() );
    KDXML::createRealNode( doc, element, "GridSubStepWidth",
                           a.gridSubStepWidth() );
    QString name;
    switch( a.gridGranularitySequence() ){
        case KDChartEnums::GranularitySequence_10_20:
            name = "10_20";
            break;
        case KDChartEnums::GranularitySequence_10_50:
            name = "10_50";
            break;
        case KDChartEnums::GranularitySequence_25_50:
            name = "25_50";
            break;
        case KDChartEnums::GranularitySequence_125_25:
            name = "125_25";
            break;
        case KDChartEnums::GranularitySequenceIrregular:
            name = "Irregular";
            break;
        default:
            Q_ASSERT( false ); // all of the types need to be handled
            break;
    }
    KDXML::createStringNode( doc, element, "GranularitySequence", name );
    KDXML::createBoolNode( doc, element, "AdjustLowerBound",
                           a.adjustLowerBoundToGrid() );
    KDXML::createBoolNode( doc, element, "AdjustUpperBound",
                           a.adjustUpperBoundToGrid() );
    KDXML::createPenNode( doc, element, "GridPen",
                          a.gridPen() );
    KDXML::createBoolNode( doc, element, "SubGridVisible",
                           a.isSubGridVisible() );
    KDXML::createPenNode( doc, element, "SubGridPen",
                          a.subGridPen() );
    KDXML::createPenNode( doc, element, "ZeroLinePen",
                          a.zeroLinePen() );
}



bool AttributesSerializer::parseQObjectPointerNode(
        const QDomNode& node,
        QObject*& p,
        QString& ptrName,
        bool& wasParsed,
        bool bErrorIfNotFound )
{
    bool bOK = true;
    p = 0;
    ptrName = "";
    wasParsed = false;

    if( node.isNull() )
        return bOK;
    
    QDomElement element = node.toElement();
    if( element.isNull() )  // was really an element
        return bOK;

    const QString tagName = element.tagName();
    if( tagName == "kdchart:pointer" ) {
        QString s;
        if( KDXML::findStringAttribute( element, "name", s ) )
        {
            ptrName = s;
            QObject* p0;
            if( findQObjectPointer( s, p0, wasParsed, bErrorIfNotFound ) )
            {
                p = p0;
            }
            else if( bErrorIfNotFound )
            {
                qDebug() << "    Could not resolve pointer in\n    "+showDomPath( element );
                bOK = false;
            }
        }
        else
        {
            qDebug() << "    Invalid pointer element:\n    "+showDomPath( element );
            bOK = false;
        }
    }
    else if( tagName == "kdchart:unresolved-pointer" ) {
        /*
        qDebug() << "\n"
        "    Non-critical information by AttributesSerializer::parseQObjectPointerNode():\n"
        "    Unresolved pointer found, setting value to zero.\n"
        "    Location:\n    "+showDomPath( element );
        */
        p = 0;
    }
    else {
        qDebug() << "Unknown subelement found:" << tagName;
        bOK = false;
    }
    
    return bOK;
}


bool AttributesSerializer::findQObjectPointer(
        const QString& name,
        QObject*& p,
        bool& wasParsed,
        bool reportNotFound )
{
    bool bOK = true;

    p = 0;
    wasParsed = false;
    //qDebug() << "parsed pointer:" << name;
    if( name.compare("Null", Qt::CaseInsensitive) != 0 ){
        QObject* ptr;
        bOK = SerializeCollector::instance()->foundInitializedPointer( name, ptr, wasParsed );
        if( bOK ){
            p = ptr;
        }else if( reportNotFound ){
            qDebug() << "\n"
                    "    CRITICAL information by AttributesSerializer::findQObjectPointer():\n"
                    "    Could not resolve pointer \"" << name << "\", setting pointer value to zero.";
        }
    }
    return bOK;
}

void AttributesSerializer::saveQObjectPointer(
        QDomDocument& doc,
        QDomElement& e,
        const QObject* p,
        const QString& title )
{
    QDomElement refAreaElement = doc.createElement( title );
    e.appendChild( refAreaElement );
    const QString globalRefAreaName( IdMapper::instance()->findName( p ) );
    const bool bOK = ! globalRefAreaName.isEmpty();
    if( bOK ){
        //qDebug() << "AttributesSerializer::saveQObjectPointer() storing  resolved  ptr:" << p;
        SerializeCollector::instance()->storePointerName(
                doc, refAreaElement, globalRefAreaName );
    }else{
        //qDebug() << "AttributesSerializer::saveQObjectPointer() storing UNRESOLVED ptr:" << p;
        SerializeCollector::instance()->storeUnresolvedPointer(
                doc, p, refAreaElement );
    }
}

const QString AttributesSerializer::showDomPath( const QDomElement& container )
{
    //return e.ownerDocument().toString();
    QString path( container.tagName() );
    QDomNode n = container.parentNode();
    while( !n.isNull() ) {
        QDomElement element = n.toElement(); // try to convert the node to an element.
        if( element.isNull() )
            return path;
        path.prepend( element.tagName()+"/" );
        n = n.parentNode();
    }
    return path;
}

