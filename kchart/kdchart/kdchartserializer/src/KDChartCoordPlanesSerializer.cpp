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

#include <KDChartCoordPlanesSerializer.h>
#include <KDChartCoordPlanesSerializer_p.h>

#include <KDChartSerializeCollector.h>
#include <KDChartIdMapper.h>
#include <KDChartDiagramsSerializer.h>
#include <KDChartAbstractAreaBaseSerializer.h>

#include <KDChartAbstractSerializerFactory>
#include <KDChartSerializer>

#include <KDXMLTools.h>

#include <qglobal.h>
#include <QMessageBox>

#define d d_func()

using namespace KDChart;

CoordPlanesSerializer::Private::Private( CoordPlanesSerializer* qq )
    : q( qq ),
      m_diagS( 0 ),
      m_model( 0 )
{
}

CoordPlanesSerializer::Private::~Private() {}


/**
  \class KDChart::CoordPlanesSerializer KDChartCoordPlanesSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */


static QString globalListName;

CoordPlanesSerializer::CoordPlanesSerializer(QAbstractItemModel * model)
    : _d( new Private( this ) )
{
    d->m_model = model;
    d->m_diagS = new DiagramsSerializer( this );
    globalListName = "kdchart:coordinate-planes"; // default value, can be
    // overwritten by the title passed to CoordPlanesSerializer::savePlanes()
}

CoordPlanesSerializer::~CoordPlanesSerializer()
{
    delete d->m_diagS;
    delete _d; _d = 0;
}

void CoordPlanesSerializer::init()
{
}

void CoordPlanesSerializer::saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const
{
    d->savePlane( doc, e, qobject_cast< const AbstractCoordinatePlane* >( obj ) );
}

bool CoordPlanesSerializer::parseElement( const QDomElement& container, QObject* ptr ) const
{
    AbstractCoordinatePlane* plane = qobject_cast< AbstractCoordinatePlane* >( ptr );
    return d->doParsePlane( container, plane );
}

void CoordPlanesSerializer::setModel(QAbstractItemModel * model)
{
    d->m_model = model;
}

void CoordPlanesSerializer::savePlanes(
        QDomDocument& doc,
        QDomElement& e,
        const CoordinatePlaneList& planes,
        const QString& title ) const
{
    if( ! title.isEmpty() )
        globalListName = title;

    // access (or append, resp.) the global list
    QDomElement* planesList =
            SerializeCollector::instance()->findOrMakeElement( doc, globalListName );

    // create the local list holding names pointing into the global list
    QDomElement pointersList =
            SerializeCollector::createPointersList( doc, e, globalListName );

    Q_FOREACH ( AbstractCoordinatePlane* p, planes )
    {
        //qDebug() << "\nAttempting to store coordinate-plane:" << p->metaObject()->className();
        bool wasFound;
        QDomElement planeElement =
                SerializeCollector::findOrMakeChild(
                doc,
                *planesList,
                pointersList,
                "kdchart:coordinate-plane",
                p->metaObject()->className(),
                p,
                wasFound );
        if( ! wasFound ){
            const AbstractSerializerFactory* factory = Serializer::elementSerializerFactory( p );
            QObject* obj = p;
            if( factory != 0 ){
                //qDebug() << "Storing coordinate-plane:" << p->metaObject()->className();
                factory->instance( p->metaObject()->className() )->saveElement( doc, planeElement, obj );
            }else{
                qDebug() << "\nProblem: Can not store coordinate-plane:" << p->metaObject()->className();
            }
        }
    }
}


bool CoordPlanesSerializer::parsePlane(
        const QDomNode& rootNode,
        const QDomNode& pointerNode,
        AbstractCoordinatePlane*& planePtr )const
{
    bool bOK = true;

    QObject* ptr;
    QString ptrName;
    bool wasParsed;
    const bool pointerFound =
            AttributesSerializer::parseQObjectPointerNode(
                    pointerNode, ptr,
                    ptrName, wasParsed, true ) && ptr;

    if( ptrName.isEmpty() ){
        qDebug()<< "Could not parse coord-plane. Global pointer node is invalid.";
        bOK = false;
    }else{
        if( pointerFound ){
            planePtr = dynamic_cast<AbstractCoordinatePlane*>(ptr);
            if( ! planePtr ){
                qDebug()<< "Could not parse coord-plane. Global pointer"
                        << ptrName << "is no AbstractCoordinatePlane-ptr.";
                bOK = false;
            }
        }else{
            qDebug()<< "Could not parse coord-plane. Global pointer"
                    << ptrName << "is no AbstractCoordinatePlane-ptr.";
            bOK = false;
        }
    }


    if( bOK && wasParsed ) return true;


    QDomElement container;
    if( bOK ){
        container = SerializeCollector::findStoredGlobalElement(
                rootNode, ptrName, "kdchart:coordinate-planes" );
        bOK = ! container.tagName().isEmpty();
    }

    if( bOK ) {
        SerializeCollector::instance()->setWasParsed( planePtr, true );
        const AbstractSerializerFactory* factory = Serializer::elementSerializerFactory( planePtr );
        QObject* obj = planePtr;
        if( factory != 0 )
            bOK = factory->instance( planePtr->metaObject()->className() )->parseElement( container, obj );
        if( bOK && d->m_model != 0 )
        {
            Q_FOREACH( AbstractDiagram* diag, planePtr->diagrams() )
                diag->setModel( d->m_model );
        }
    }

    return bOK;
}

bool CoordPlanesSerializer::Private::doParsePlane( const QDomElement& container, AbstractCoordinatePlane* planePtr ) const
{
    bool bOK = true;
    CartesianCoordinatePlane* const cartPlane = qobject_cast< CartesianCoordinatePlane* >( planePtr );
    PolarCoordinatePlane* const polPlane = qobject_cast< PolarCoordinatePlane* >( planePtr );

    if( cartPlane != 0 )
        bOK = parseCartPlane( container, *cartPlane );
    else if( polPlane != 0 )
        bOK = parsePolPlane( container, *polPlane );
    else
        bOK = false;
    
    return bOK;
}

void CoordPlanesSerializer::Private::savePlane(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractCoordinatePlane* p )const
{
    if( ! p ) return;

    const CartesianCoordinatePlane* const cartPlane = qobject_cast< const CartesianCoordinatePlane* >( p );
    const PolarCoordinatePlane* const polPlane = qobject_cast< const PolarCoordinatePlane* >( p );
    if( cartPlane != 0 )
        saveCartPlane( doc, e, *cartPlane );
    else if( polPlane != 0 )
        savePolPlane( doc, e, *polPlane );
}

bool CoordPlanesSerializer::Private::parseAbstractPlane(
        const QDomElement& container, AbstractCoordinatePlane& plane )const
{
    bool bOK = true;
    QDomNode node;

    bool bNoDiagramParsedYet = true;
    node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-area-base" ) {
                if( ! AbstractAreaBaseSerializer::parseAbstractAreaBase( element, plane ) ){
                    qDebug()<< "Could not parse AbstractCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "kdchart:diagrams:pointers" ) {
                QDomNode node2 = element.firstChild();
                while( ! node2.isNull() ) {
                    AbstractDiagram* diagram=0;
                    if( m_diagS->parseDiagram(
                            container.ownerDocument().firstChild(), node2, diagram ) )
                    {
                        if( m_model )
                            diagram->setModel( m_model );
                        if( bNoDiagramParsedYet ){
                            plane.replaceDiagram( diagram );
                            bNoDiagramParsedYet = false;
                        }else{
                            plane.addDiagram( diagram );
                        }
                    }else{
                        qDebug()<< "Could not parse AbstractCoordinatePlane / kdchart:diagrams. Global pointer is not a KDChart::AbstractDiagram-ptr.";
                        bOK = false;
                    }
                    node2 = node2.nextSibling();
                }
            } else if( tagName == "ZoomFactorX" ) {
                double factor;
                if( KDXML::readDoubleNode( element, factor ) ){
                    plane.setZoomFactorX( factor );
                }else{
                    qDebug()<< "Could not parse AbstractCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "ZoomFactorY" ) {
                double factor;
                if( KDXML::readDoubleNode( element, factor ) ){
                    plane.setZoomFactorY( factor );
                }else{
                    qDebug()<< "Could not parse AbstractCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "ZoomCenter" ) {
                QPointF pt;
                if( KDXML::readPointFNode( element, pt ) ){
                    plane.setZoomCenter( pt );
                }else{
                    qDebug()<< "Could not parse AbstractCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "GlobalGridAttributes" ) {
                GridAttributes a;
                if( AttributesSerializer::parseGridAttributes( element, a ) ){
                    plane.setGlobalGridAttributes( a );
                }else{
                    qDebug()<< "Could not parse AbstractCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "ReferencePlane" ) {
                AbstractCoordinatePlane* refPlane;
                if( q->parsePlane( container.ownerDocument().firstChild(), element.firstChild(), refPlane ) ){
                    plane.setReferenceCoordinatePlane( refPlane );
                }else{
                    qDebug()<< "Could not parse AbstractCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "Chart" ) {
                QDomNode node2 = element.firstChild();
                if( ! node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( ! ele2.isNull() ) { // was really an element
                        Chart* chart=0;
                        QObject* ptr;
                        QString ptrName;
                        bool wasParsed;
                        if( ! AttributesSerializer::parseQObjectPointerNode(
                                        ele2, ptr,
                                        ptrName, wasParsed, false ) && ptr )
                        {
                            qDebug()<< "Could not parse AbstractDiagram. Global pointer node"
                                    << ele2.tagName() << "has invalid content.";
                            bOK = false;
                        }else{
                            chart = dynamic_cast<Chart*>(ptr);
                            if( ! chart ){
                                qDebug()<< "Could not parse AbstractDiagram. Global pointer"
                                        << ptrName << "is no KDChart::Chart-ptr.";
                                bOK = false;
                            }else{
                                plane.setParent( chart );
                            }
                        }
                    }else{
                        qDebug()<< "Could not parse AbstractDiagram. Node does not contain a valid element.";
                        bOK = false;
                    }
                }else{
                    qDebug()<< "Could not parse AbstractDiagram. Node does not contain a valid element.";
                    bOK = false;
                }
            } else {
                qDebug() << "Unknown subelement of AbstractCoordinatePlane found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void CoordPlanesSerializer::Private::saveAbstractPlane(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractCoordinatePlane& plane,
        const QString& title )const
{
    QDomElement planeElement =
        doc.createElement( title );
    e.appendChild( planeElement );

    // first save the information hold by the base class
    AbstractAreaBaseSerializer::saveAbstractAreaBase(
            doc,
            planeElement,
            plane,
            "kdchart:abstract-area-base" );

    // save the associated diagrams
    m_diagS->saveDiagrams( doc,
                          planeElement,
                          plane.diagrams(),
                          "kdchart:diagrams" );

    KDXML::createDoubleNode( doc, planeElement, "ZoomFactorX",
                             plane.zoomFactorX() );
    KDXML::createDoubleNode( doc, planeElement, "ZoomFactorY",
                             plane.zoomFactorY() );
    KDXML::createPointFNode( doc, planeElement, "ZoomCenter",
                             plane.zoomCenter() );
    AttributesSerializer::saveGridAttributes(
            doc, planeElement, plane.globalGridAttributes(), "GlobalGridAttributes" );

    // save the reference plane(-pointer), if any
    const AbstractCoordinatePlane* refPlane = plane.referenceCoordinatePlane();
    if( refPlane ){
        QDomElement refPlanePtrElement =
                doc.createElement( "ReferencePlane" );
        planeElement.appendChild( refPlanePtrElement );
        // access (or append, resp.) the global list
        QDomElement* planesList =
                SerializeCollector::instance()->findOrMakeElement( doc, globalListName );

        bool wasFound;
        QDomElement globalListElement =
                SerializeCollector::findOrMakeChild(
                doc,
                *planesList,
                refPlanePtrElement,
                "kdchart:coordinate-plane",
                refPlane->metaObject()->className(),
                refPlane,
                wasFound );
        if( ! wasFound ){
            // Since the plane is stored in the global structure anyway,
            // it is save to store it right now.
            // So it will not be forgotten, in case it is not embedded in a
            // chart.
            // The wasFound test makes sure it will not be stored twice.
            savePlane( doc, globalListElement, refPlane );
        }
    }

    // save the pointer to the associated chart,
    // and save the chart in the global structure if not there yet
    const Chart* chart =
            static_cast<const Chart*>( plane.parent() );
    if( chart ){
        // try to access the global charts list:
        // If there is none, searching for the chart pointer's name makes no sense.
        bool bOK = SerializeCollector::instance()->findElement( "kdchart:charts" ) != 0;
        if( bOK ){
            const QString chartName( IdMapper::instance()->findName( chart ) );
            bOK = ! chartName.isEmpty();
            if( bOK ){
                QDomElement chartPtrElement =
                        doc.createElement( "Chart" );
                planeElement.appendChild( chartPtrElement );
                SerializeCollector::instance()->storePointerName(
                        doc, chartPtrElement, chartName );
            }else{
                qDebug() << "--- CoordPlanesSerializer ---";
                qDebug() << "Warning: The coordinate plane's parent (" << chart << ") is not in the global charts list.";
                IdMapper::instance()->debugOut();
            }
        }else{
            qDebug() << "--- CoordPlanesSerializer ---";
            qDebug() << "Warning: No global charts list found.";
        }
        if( ! bOK ){
            // We warn via debug, but we do not break,
            // since the result can still be used.
            qDebug() << "Wrong usage of KD Chart Serializer ??";
            qDebug() << "KDChart::CoordPlanesSerializer::savePlanes() was called before the associated chart was stored";
            qDebug() << "So we can not store the kdchart-pointer in the coordinate planes sub-tree.";
            qDebug() << "--- CoordPlanesSerializer ---";
        }
    }
}


bool CoordPlanesSerializer::Private::parseCartPlane(
        const QDomElement& container, CartesianCoordinatePlane& plane )const
{
    //qDebug() << "-------->" << container.tagName();
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-coordinate-plane" ) {
                if( ! parseAbstractPlane( element, plane ) ){
                    qDebug() << "Could not parse base class of CartesianCoordinatePlane.";
                    bOK = false;
                }
            } else if( tagName == "IsometricScaling" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) ){
                    plane.setIsometricScaling( b );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "HorizontalRange" ) {
                QPair<qreal, qreal> pair;
                if( KDXML::readRealPairNode( element, pair ) ){
                    plane.setHorizontalRange( pair );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "VerticalRange" ) {
                QPair<qreal, qreal> pair;
                if( KDXML::readRealPairNode( element, pair ) ){
                    plane.setVerticalRange( pair );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "AutoAdjustHorizontalRangeToData" ) {
                int i;
                if( KDXML::readIntNode( element, i ) ){
                    plane.setAutoAdjustHorizontalRangeToData( i );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "AutoAdjustVerticalRangeToData" ) {
                int i;
                if( KDXML::readIntNode( element, i ) ){
                    plane.setAutoAdjustVerticalRangeToData( i );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "HorizontalGridAttributes" ) {
                GridAttributes a;
                if( AttributesSerializer::parseGridAttributes( element, a ) ){
                    plane.setGridAttributes( Qt::Horizontal, a );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "VerticalGridAttributes" ) {
                GridAttributes a;
                if( AttributesSerializer::parseGridAttributes( element, a ) ){
                    plane.setGridAttributes( Qt::Vertical, a );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "AutoAdjustGridToZoom" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) ){
                    plane.setAutoAdjustGridToZoom( b );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "AxesCalcModeX" ) {
                AbstractCoordinatePlane::AxesCalcMode mode;
                if( parseAxesCalcMode( element, mode ) ){
                    plane.setAxesCalcModeX( mode );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else if( tagName == "AxesCalcModeY" ) {
                AbstractCoordinatePlane::AxesCalcMode mode;
                if( parseAxesCalcMode( element, mode ) ){
                    plane.setAxesCalcModeY( mode );
                }else{
                    qDebug()<< "Could not parse CartesianCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                    bOK = false;
                }
            } else {
                qDebug() << "Unknown subelement of CartesianCoordinatePlane found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void CoordPlanesSerializer::Private::saveCartPlane(
        QDomDocument& doc,
        QDomElement& planeElement,
        const CartesianCoordinatePlane& plane )const
{
    // first save the information hold by the base class
    saveAbstractPlane( doc, planeElement, plane,
                       "kdchart:abstract-coordinate-plane" );

    // then save any plane type specific information
    KDXML::createBoolNode( doc, planeElement, "IsometricScaling",
                           plane.doesIsometricScaling() );
    KDXML::createRealPairNode( doc, planeElement, "HorizontalRange",
                               plane.horizontalRange() );
    KDXML::createRealPairNode( doc, planeElement, "VerticalRange",
                               plane.verticalRange() );
    KDXML::createIntNode( doc, planeElement, "AutoAdjustHorizontalRangeToData",
                          plane.autoAdjustHorizontalRangeToData() );
    KDXML::createIntNode( doc, planeElement, "AutoAdjustVerticalRangeToData",
                          plane.autoAdjustVerticalRangeToData() );
    if( plane.hasOwnGridAttributes( Qt::Horizontal ) ){
        AttributesSerializer::saveGridAttributes(
                doc, planeElement, plane.gridAttributes( Qt::Horizontal ),
                "HorizontalGridAttributes" );
    }
    if( plane.hasOwnGridAttributes( Qt::Vertical ) ){
        AttributesSerializer::saveGridAttributes(
                doc, planeElement, plane.gridAttributes( Qt::Vertical ),
                "VerticalGridAttributes" );
    }
    KDXML::createBoolNode( doc, planeElement, "AutoAdjustGridToZoom",
                           plane.autoAdjustGridToZoom() );
    saveAxesCalcMode(
            doc, planeElement, plane.axesCalcModeX(), "AxesCalcModeX" );
    saveAxesCalcMode(
            doc, planeElement, plane.axesCalcModeY(), "AxesCalcModeY" );
}


bool CoordPlanesSerializer::Private::parsePolPlane(
        const QDomElement& container, PolarCoordinatePlane& plane )const
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-coordinate-plane" ) {
                if( ! parseAbstractPlane( element, plane ) ){
                    qDebug() << "Could not parse base class of PolarCoordinatePlane.";
                    bOK = false;
                }
            } else if( tagName == "StartPosition" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) ){
                    plane.setStartPosition( r );
                }else{
                    qDebug()<< "Could not parse PolarCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "CircularGridAttributes" ) {
                GridAttributes a;
                if( AttributesSerializer::parseGridAttributes( element, a ) ){
                    plane.setGridAttributes( true, a );
                }else{
                    qDebug()<< "Could not parse PolarCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "SagittalGridAttributes" ) {
                GridAttributes a;
                if( AttributesSerializer::parseGridAttributes( element, a ) ){
                    plane.setGridAttributes( false, a );
                }else{
                    qDebug()<< "Could not parse PolarCoordinatePlane. Element"
                            << tagName << "has invalid content.";
                }
            } else {
                qDebug() << "Unknown subelement of PolarCoordinatePlane found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void CoordPlanesSerializer::Private::savePolPlane(
        QDomDocument& doc,
        QDomElement& planeElement,
        const PolarCoordinatePlane& plane )const
{
    // first save the information hold by the base class
    saveAbstractPlane( doc, planeElement, plane,
                       "kdchart:abstract-coordinate-plane" );

    // then save any plane type specific information
    KDXML::createRealNode( doc, planeElement, "StartPosition",
                           plane.startPosition() );
    if( plane.hasOwnGridAttributes( true ) ){
        AttributesSerializer::saveGridAttributes(
                doc, planeElement, plane.gridAttributes( true ),
                "CircularGridAttributes" );
    }
    if( plane.hasOwnGridAttributes( false ) ){
        AttributesSerializer::saveGridAttributes(
                doc, planeElement, plane.gridAttributes( false ),
                "SagittalGridAttributes" );
    }
}

bool CoordPlanesSerializer::Private::parseAxesCalcMode(
        const QDomElement& container, AbstractCoordinatePlane::AxesCalcMode& mode )const
{
    bool bOK = true;
    QString s;
    if( KDXML::readStringNode( container, s ) ){
        if( s.compare("Linear", Qt::CaseInsensitive) == 0 )
            mode = AbstractCoordinatePlane::Linear;
        else if( s.compare("Logarithmic", Qt::CaseInsensitive) == 0 )
            mode = AbstractCoordinatePlane::Logarithmic;
        else{
            bOK = false;
            Q_ASSERT( false ); // all of the modes need to be handled
        }
    }
    return bOK;
}

void CoordPlanesSerializer::Private::saveAxesCalcMode(
        QDomDocument& doc,
        QDomElement& e,
        const CartesianCoordinatePlane::AxesCalcMode& mode,
        const QString& title )const
{
    QString name;
    switch( mode ){
        case CartesianCoordinatePlane::Linear:
            name = "Linear";
            break;
        case CartesianCoordinatePlane::Logarithmic:
            name = "Logarithmic";
            break;
        default:
            Q_ASSERT( false ); // all of the types need to be handled
            break;
    }
    KDXML::createStringNode( doc, e, title, name );
}

const QString CoordPlanesSerializer::globalList()const
{
    return globalListName;
}
