/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#include <KDChartLegendsSerializer>
#include <KDChartLegendsSerializer_p.h>

#include <KDChartAbstractAreaBaseSerializer>
#include <KDChartDiagramsSerializer>
#include <KDChartSerializeCollector>
#include <KDChartIdMapper>
#include <KDChartAbstractSerializerFactory>
#include <KDChartSerializer>

#include <KDXMLTools>

#include <qglobal.h>

#define d d_func()

using namespace KDChart;

/**
  \class KDChart::LegendsSerializer KDChartLegendsSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */


LegendsSerializer::Private::Private( LegendsSerializer* qq )
    : q( qq )
{
}

LegendsSerializer::Private::~Private()
{
}

LegendsSerializer::LegendsSerializer( QObject* parent )
    : QObject( parent ),
      _d( new Private( this ) )
{
}

LegendsSerializer::~LegendsSerializer()
{
    delete _d; _d = 0;
}

void LegendsSerializer::init()
{
}

void LegendsSerializer::saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const
{
    const Legend* l = qobject_cast< const Legend* >( obj );
    if( l != 0 )
        d->saveLegend( doc, e, *l );
}

bool LegendsSerializer::parseElement( const QDomElement& container, QObject* ptr ) const
{
    Legend* l =  qobject_cast< Legend* >( ptr );
    return d->doParseLegend( container, l );
}

void LegendsSerializer::saveLegends(
        QDomDocument& doc,
        QDomElement& e,
        const LegendList& legends,
        const QString& title )
{
    // access (or append, resp.) the global list
    QDomElement* legendsList =
            SerializeCollector::instance()->findOrMakeElement( doc, title );

    // create the local list holding names pointing into the global list
    QDomElement pointersList =
            SerializeCollector::createPointersList( doc, e, title );

    Q_FOREACH ( Legend* p, legends )
    {
        bool wasFound;
        QDomElement legendElement =
                SerializeCollector::findOrMakeChild(
                doc,
                *legendsList,
                pointersList,
                "kdchart:legend",
                "KDChart::Legend",
                p,
                wasFound );
        if( ! wasFound ){
            const AbstractSerializerFactory* factory
                    = Serializer::elementSerializerFactory( p );
            if( factory ){
                factory->instance( p->metaObject()->className() )->saveElement( doc, legendElement, p );
            }else{
                qDebug() << "\nProblem: Can not store legend:" << p->metaObject()->className();
            }
        }
    }
}

bool LegendsSerializer::parseLegend(
        const QDomNode& rootNode,
        const QDomNode& pointerNode,
        Legend*& legend )
{
    bool bOK = true;
    legend = 0;

    QObject* ptr;
    QString ptrName;
    bool wasParsed;
    const bool pointerFound =
            AttributesSerializer::parseQObjectPointerNode(
            pointerNode, ptr,
            ptrName, wasParsed, true ) && ptr;

    if( ptrName.isEmpty() ){
        qDebug()<< "Could not parse legend. Global pointer node is invalid.";
        bOK = false;
    }else{
        if( pointerFound ){
            legend = dynamic_cast<Legend*>(ptr);
            if( ! legend ){
                qDebug()<< "Could not parse legend. Global pointer"
                        << ptrName << "is no Legend-ptr.";
                bOK = false;
            }
        }else{
            qDebug()<< "Could not parse legend. Global pointer"
                    << ptrName << "is no Legend-ptr.";
            bOK = false;
        }
    }


    if( bOK && wasParsed ) return true;


    QDomElement container;
    if( bOK ){
        container = SerializeCollector::findStoredGlobalElement(
                rootNode, ptrName, "kdchart:legends" );
        if( container.isNull() ){
            qDebug()<< "Stored Element was not found.";
            bOK = false;
        }
        if( bOK && container.tagName().isEmpty() ){
            qDebug()<< "Stored Element is invalid.";
            bOK = false;
        }
    }

    if( bOK ) {
        SerializeCollector::instance()->setWasParsed( legend, true );

        return Private::doParseLegend( container, legend );
    }
    return false;
}

bool LegendsSerializer::Private::doParseLegend( const QDomElement& container, Legend* legend )
{
    bool bOK = true;

    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-area-base" ) {
                if( ! AbstractAreaBaseSerializer::parseAbstractAreaBase( element, *legend ) ){
                    qDebug() << "Could not parse base class of Legend.";
                    bOK = false;
                }
            } else if( tagName == "Visible" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    legend->setVisible( b );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
        } else if( tagName == "ReferenceArea" ) {
                QObject* ptr;
                QString ptrName;
                bool wasParsed;
                //qDebug() << " a ";
                if( AttributesSerializer::parseQObjectPointerNode(
                        element.firstChild(), ptr, ptrName, wasParsed, true ) ){
                    if( ptr ){
                        QWidget* wPtr = dynamic_cast<QWidget*>(ptr);
                        if( wPtr ){
                            legend->setReferenceArea( wPtr );
                        }else{
                            qDebug() << "Error: Value of Legend/ReferenceArea must be a QWidget*";
                            bOK = false;
                        }
                    }else{
                        legend->setReferenceArea( 0 ); // a Null pointer means no bug
                    }
                }else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
                //qDebug() << " b ";
            } else if( tagName == "kdchart:diagrams:pointers" ) {
                // parse the map of associated diagrams
                QDomNode node2 = element.firstChild();
                while( !node2.isNull() ) {
                    AbstractDiagram* diag;
                    DiagramsSerializer diagS;
                    if( diagS.parseDiagram( container.ownerDocument().firstChild(), node2, diag ) ){
                        legend->addDiagram( diag );
                    }else{
                        qDebug()<< "Values of Legend/kdchart:diagrams:pointers must be AbstractDiagram pointers";
                        bOK = false;
                    }
                    node2 = node2.nextSibling();
                }
            } else if( tagName == "Alignment" ) {
                Qt::Alignment a;
                if( KDXML::readAlignmentNode( element, a ) )
                    legend->setAlignment( a );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "Position" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    legend->setPosition( Position::fromName( s.toLatin1() ) );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "LegendStyle" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) ){
                    if( s.compare("MarkersOnly", Qt::CaseInsensitive) == 0 )
                        legend->setLegendStyle( Legend::MarkersOnly );
                    else if( s.compare("LinesOnly", Qt::CaseInsensitive) == 0 )
                        legend->setLegendStyle( Legend::LinesOnly );
                    else if( s.compare("MarkersAndLines", Qt::CaseInsensitive) == 0 )
                        legend->setLegendStyle( Legend::MarkersAndLines );
                    else{
                        qDebug()<< "Could not parse Legend. Unknown value in element" << tagName;
                        bOK = false;
                    }
                }else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "FloatingPosition" ) {
                RelativePosition pos;
                if( AttributesSerializer::parseRelativePosition( element, pos ) )
                    legend->setFloatingPosition( pos );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "Orientation" ) {
                Qt::Orientation o;
                if( KDXML::readOrientationNode( element, o ) )
                    legend->setOrientation( o );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "ShowLines" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    legend->setShowLines( b );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "TextsMap" ) {
                // parse the map of explicitely set texts
                QDomNode node2 = element.firstChild();
                while( !node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( !ele2.isNull() ) { // was really an element
                        QString tagName2 = ele2.tagName();
                        if( tagName2 == "item" ) {
                            int dataset;
                            QString txt;
                            if( KDXML::findIntAttribute( ele2, "dataset", dataset ) &&
                                KDXML::findStringAttribute( ele2, "text", txt ) ){
                                legend->setText( dataset, txt );
                            }else{
                                qDebug() << "Invalid item in Legend/TextsMap found.";
                                bOK = false;
                            }
                        }else{
                            qDebug() << "Unknown subelement of Legend/TextsMap found:" << tagName2;
                            bOK = false;
                        }
                    }
                    node2 = node2.nextSibling();
                }
            } else if( tagName == "BrushesMap" ) {
                // parse the map of brushes
                QDomNode node2 = element.firstChild();
                while( !node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( !ele2.isNull() ) { // was really an element
                        QString tagName2 = ele2.tagName();
                        if( tagName2 == "item" ) {
                            int dataset;
                            QBrush brush;
                            bool brushFound = false;
                            if( KDXML::findIntAttribute( ele2, "dataset", dataset ) ){
                                QDomNode node3 = ele2.firstChild();
                                while( !node3.isNull() ) {
                                    QDomElement ele3 = node3.toElement();
                                    if( !ele3.isNull() ) { // was really an element
                                        QString tagName3 = ele3.tagName();
                                        if( tagName3 == "brush" ) {
                                            QBrush b;
                                            if( KDXML::readBrushNode( ele3, b ) ){
                                                brush = b;
                                                brushFound = true;
                                            }else{
                                                qDebug() << "Error parsing item in Legend/BrushesMap.";
                                            }
                                        }else{
                                            qDebug() << "Unknown subelement of Legend/BrushesMap found:" << tagName3;
                                        }
                                    }
                                    node3 = node3.nextSibling();
                                }
                                if( brushFound ){
                                    legend->setBrush( dataset, brush );
                                }else{
                                    qDebug() << "Error parsing Legend/BrushesMap.";
                                    bOK = false;
                                }
                            }else{
                                qDebug() << "Invalid item in Legend/BrushesMap found.";
                                bOK = false;
                            }
                        }else{
                            qDebug() << "Unknown subelement of Legend/BrushesMap found:" << tagName2;
                            bOK = false;
                        }
                    }
                    node2 = node2.nextSibling();
                }
            } else if( tagName == "PensMap" ) {
                // parse the map of brushes
                QDomNode node2 = element.firstChild();
                while( !node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( !ele2.isNull() ) { // was really an element
                        QString tagName2 = ele2.tagName();
                        if( tagName2 == "item" ) {
                            int dataset;
                            QPen pen;
                            bool penFound = false;
                            if( KDXML::findIntAttribute( ele2, "dataset", dataset ) ){
                                QDomNode node3 = ele2.firstChild();
                                while( !node3.isNull() ) {
                                    QDomElement ele3 = node3.toElement();
                                    if( !ele3.isNull() ) { // was really an element
                                        QString tagName3 = ele3.tagName();
                                        if( tagName3 == "pen" ) {
                                            QPen pe;
                                            if( KDXML::readPenNode( ele3, pe ) ){
                                                pen = pe;
                                                penFound = true;
                                            }else{
                                                qDebug() << "Error parsing item in Legend/PensMap.";
                                            }
                                        }else{
                                            qDebug() << "Unknown subelement of Legend/PensMap found:" << tagName3;
                                        }
                                    }
                                    node3 = node3.nextSibling();
                                }
                                if( penFound ){
                                    legend->setPen( dataset, pen );
                                }else{
                                    qDebug() << "Error parsing Legend/PensMap.";
                                    bOK = false;
                                }
                            }else{
                                qDebug() << "Invalid item in Legend/PensMap found.";
                                bOK = false;
                            }
                        }else{
                            qDebug() << "Unknown subelement of Legend/PensMap found:" << tagName2;
                            bOK = false;
                        }
                    }
                    node2 = node2.nextSibling();
                }
            } else if( tagName == "MarkerAttributesMap" ) {
                // parse the map of brushes
                QDomNode node2 = element.firstChild();
                while( !node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( !ele2.isNull() ) { // was really an element
                        QString tagName2 = ele2.tagName();
                        if( tagName2 == "item" ) {
                            int dataset;
                            MarkerAttributes attrs;
                            bool attrsFound = false;
                            if( KDXML::findIntAttribute( ele2, "dataset", dataset ) ){
                                QDomNode node3 = ele2.firstChild();
                                while( !node3.isNull() ) {
                                    QDomElement ele3 = node3.toElement();
                                    if( !ele3.isNull() ) { // was really an element
                                        QString tagName3 = ele3.tagName();
                                        if( tagName3 == "MarkerAttributes" ) {
                                            MarkerAttributes a;
                                            if( AttributesSerializer::parseMarkerAttributes( ele3, a ) ){
                                                attrs = a;
                                                attrsFound = true;
                                            }else{
                                                qDebug() << "Error parsing item in Legend/MarkerAttributesMap.";
                                            }
                                        }else{
                                            qDebug() << "Unknown subelement of Legend/MarkerAttributesMap found:" << tagName3;
                                        }
                                    }
                                    node3 = node3.nextSibling();
                                }
                                if( attrsFound ){
                                    legend->setMarkerAttributes( dataset, attrs );
                                }else{
                                    qDebug() << "Error parsing Legend/MarkerAttributesMap.";
                                    bOK = false;
                                }
                            }else{
                                qDebug() << "Invalid item in Legend/MarkerAttributesMap found.";
                                bOK = false;
                            }
                        }else{
                            qDebug() << "Unknown subelement of Legend/MarkerAttributesMap found:" << tagName2;
                            bOK = false;
                        }
                    }
                    node2 = node2.nextSibling();
                }
            } else if( tagName == "UseAutomaticMarkerSize" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) )
                    legend->setUseAutomaticMarkerSize( b );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "TextAttributes" ) {
                TextAttributes ta;
                if( AttributesSerializer::parseTextAttributes( element, ta ) )
                    legend->setTextAttributes( ta );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "TitleText" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    legend->setTitleText( s );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "TitleTextAttributes" ) {
                TextAttributes ta;
                if( AttributesSerializer::parseTextAttributes( element, ta ) )
                    legend->setTitleTextAttributes( ta );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else if( tagName == "Spacing" ) {
                int i;
                if( KDXML::readIntNode( element, i ) )
                    legend->setSpacing( i );
                else{
                    qDebug()<< "Could not parse Legend. Error in element" << tagName;
                    bOK = false;
                }
            } else {
                qDebug() << "Unknown subelement of Legend found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void LegendsSerializer::Private::saveLegend(
        QDomDocument& doc,
        QDomElement& element,
        const Legend& legend )
{
    // save the area information
    AbstractAreaBaseSerializer::saveAbstractAreaBase(
            doc,
            element,
            legend,
            "kdchart:abstract-area-base" );

    KDXML::createBoolNode( doc, element, "Visible", legend.isVisible() );

    AttributesSerializer::saveQObjectPointer(
            doc, element, legend.referenceArea(), "ReferenceArea" );
    //qDebug() << legend.referenceArea();

    // save the associated diagrams
    DiagramsSerializer diagS;
    diagS.saveDiagrams( doc,
                        element,
                        legend.constDiagrams(),
                        "kdchart:diagrams" );
    KDXML::createAlignmentNode( doc, element, "Alignment", legend.alignment() );
    KDXML::createStringNode( doc, element, "Position", legend.position().name() );
    if( legend.position() == Position::Floating )
        AttributesSerializer::saveRelativePosition(
                doc, element, legend.floatingPosition(), "FloatingPosition" );
    KDXML::createOrientationNode(
            doc, element, "Orientation", legend.orientation() );
    KDXML::createBoolNode(
            doc, element, "ShowLines", legend.showLines() );

    switch( legend.legendStyle() ){
        case Legend::MarkersOnly:
            KDXML::createStringNode( doc, element, "LegendStyle", "MarkersOnly" );
            break;
        case Legend::LinesOnly:
            KDXML::createStringNode( doc, element, "LegendStyle", "LinesOnly" );
            break;
        case Legend::MarkersAndLines:
            KDXML::createStringNode( doc, element, "LegendStyle", "MarkersAndLines" );
            break;
        default:
            Q_ASSERT( false ); // all of the styles need to be handled
            break;
    }

    // save the texts map
    {
        const QMap<uint,QString> map( legend.texts() );
        if( map.count() ){
            QDomElement mapElement =
                    doc.createElement( "TextsMap" );
            element.appendChild( mapElement );
            QMap<uint,QString>::const_iterator i = map.constBegin();
            while (i != map.constEnd()) {
                QDomElement textElement =
                        doc.createElement( "item" );
                mapElement.appendChild( textElement );
                textElement.setAttribute( "dataset", i.key() );
                textElement.setAttribute( "text",    i.value() );
                ++i;
            }
        }
    }
    // save the brushes map
    {
        const QMap<uint,QBrush> map( legend.brushes() );
        if( map.count() ){
            QDomElement mapElement =
                    doc.createElement( "BrushesMap" );
            element.appendChild( mapElement );
            QMap<uint,QBrush>::const_iterator i = map.constBegin();
            while (i != map.constEnd()) {
                QDomElement brushElement =
                        doc.createElement( "item" );
                mapElement.appendChild( brushElement );
                brushElement.setAttribute( "dataset", i.key() );
                KDXML::createBrushNode( doc, brushElement, "brush", i.value() );
                ++i;
            }
        }
    }
    // save the pens map
    {
        const QMap<uint,QPen> map( legend.pens() );
        if( map.count() ){
            QDomElement mapElement =
                    doc.createElement( "PensMap" );
            element.appendChild( mapElement );
            QMap<uint,QPen>::const_iterator i = map.constBegin();
            while (i != map.constEnd()) {
                QDomElement penElement =
                        doc.createElement( "item" );
                mapElement.appendChild( penElement );
                penElement.setAttribute( "dataset", i.key() );
                KDXML::createPenNode( doc, penElement, "pen", i.value() );
                ++i;
            }
        }
    }
    // save the marker attributes map
    {
        const QMap<uint,MarkerAttributes> map( legend.markerAttributes() );
        if( map.count() ){
            QDomElement mapElement =
                    doc.createElement( "MarkerAttributesMap" );
            element.appendChild( mapElement );
            QMap<uint,MarkerAttributes>::const_iterator i = map.constBegin();
            while (i != map.constEnd()) {
                QDomElement attrsElement =
                        doc.createElement( "item" );
                mapElement.appendChild( attrsElement );
                attrsElement.setAttribute( "dataset", i.key() );
                AttributesSerializer::saveMarkerAttributes(
                        doc, attrsElement, i.value(), "MarkerAttributes" );
                ++i;
            }
        }
    }
    KDXML::createBoolNode(
            doc, element, "UseAutomaticMarkerSize", legend.useAutomaticMarkerSize() );
    AttributesSerializer::saveTextAttributes(
            doc, element, legend.textAttributes(), "TextAttributes" );
    KDXML::createStringNode(
            doc, element, "TitleText", legend.titleText() );
    AttributesSerializer::saveTextAttributes(
            doc, element, legend.titleTextAttributes(), "TitleTextAttributes" );
    KDXML::createIntNode(
            doc, element, "Spacing", legend.spacing() );
}
