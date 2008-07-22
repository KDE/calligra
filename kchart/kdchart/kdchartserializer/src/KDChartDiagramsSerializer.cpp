/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#include <KDChartDiagramsSerializer>
#include <KDChartDiagramsSerializer_p.h>

#include <KDChartSerializeCollector>
#include <KDChartCoordPlanesSerializer>
#include <KDChartAxesSerializer>
#include <KDChartAttributesModelSerializer>

#include <KDXMLTools>

#include <KDChartLineDiagram>
#include <KDChartBarDiagram>
#include <KDChartPieDiagram>
#include <KDChartPolarDiagram>
#include <KDChartRingDiagram>
#include <KDChartSerializer>
#include <KDChartAbstractSerializerFactory>

#include <qglobal.h>
#include <QMessageBox>
#include <QMetaObject>
// #include <QDomNodeList>
#include <QMetaProperty>

#define d d_func()

using namespace KDChart;

static const char* TagNameQtProperties = "properties";
/**
  \class KDChart::DiagramsSerializer KDChartDiagramsSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */

DiagramsSerializer::Private::Private( DiagramsSerializer* qq )
    : q( qq ),
      m_haveOwnCoordS( false ),
      m_coordS( 0 ),
      m_axesS( 0 ),
      m_attrModelS( 0 )
{
}

DiagramsSerializer::Private::~Private() {}


DiagramsSerializer::DiagramsSerializer( CoordPlanesSerializer* coordS )
    : _d( new Private( this ) )
{
    d->m_coordS = coordS;
    d->m_haveOwnCoordS = ( d->m_coordS == 0 );
    if( d->m_haveOwnCoordS )
        d->m_coordS = new CoordPlanesSerializer();
    d->m_axesS = new AxesSerializer();
    d->m_attrModelS = new AttributesModelSerializer();
    d->m_globalList = "kdchart:diagrams"; // default value, can be
    // overwritten by the title passed to DiagramsSerializer::saveDiagrams()
}

DiagramsSerializer::~DiagramsSerializer()
{
    delete d->m_attrModelS;
    delete d->m_axesS;
    if( d->m_haveOwnCoordS )
        delete d->m_coordS;

    delete _d; _d = 0;
}

void DiagramsSerializer::init()
{
}

void DiagramsSerializer::saveDiagrams(
        QDomDocument& doc,
        QDomElement& e,
        const ConstAbstractDiagramList& diags,
        const QString& title )const
{
    if( ! title.isEmpty() )
        d->m_globalList = title;

    // access (or append, resp.) the global list
    QDomElement* diagsList =
            SerializeCollector::instance()->findOrMakeElement( doc, d->m_globalList );

    // create the local list holding names pointing into the global list
    QDomElement pointersList =
            SerializeCollector::createPointersList( doc, e, d->m_globalList );

    Q_FOREACH ( const AbstractDiagram* p, diags )
    {
        bool wasFound;
        QDomElement diagElement =
                SerializeCollector::findOrMakeChild(
                doc,
                *diagsList,
                pointersList,
                "kdchart:diagram",
                p->metaObject()->className(),
                p,
                wasFound );
        if( ! wasFound ){
            const AbstractSerializerFactory* factory
                    = Serializer::elementSerializerFactory( p );
            if( factory ){
                factory->instance( p->metaObject()->className() )->saveElement( doc, diagElement, p );
            }else{
                qDebug() << "\nProblem: Can not store diagram:" << p->metaObject()->className();
            }
        }
    }
}


bool DiagramsSerializer::parseDiagram(
        const QDomNode& rootNode,
        const QDomNode& pointerNode,
        AbstractDiagram*& diagramPtr )const
{
    bool bOK = true;
    diagramPtr=0;

    QObject* ptr;
    QString ptrName;
    bool wasParsed;
    const bool pointerFound =
            AttributesSerializer::parseQObjectPointerNode(
                    pointerNode, ptr,
                    ptrName, wasParsed, true ) && ptr;

    if( ptrName.isEmpty() ){
        qDebug()<< "Could not parse diagram. Global pointer node is invalid.";
        bOK = false;
    }else{
        if( pointerFound ){
            diagramPtr = dynamic_cast<AbstractDiagram*>(ptr);
            if( ! diagramPtr ){
                qDebug()<< "Could not parse diagram. Global pointer"
                        << ptrName << "is no AbstractDiagram-ptr.";
                bOK = false;
            }
        }else{
            qDebug()<< "Could not parse diagram. Global pointer"
                << ptrName << "is no AbstractDiagram-ptr.";
            bOK = false;
        }
    }


    if( bOK && wasParsed ) return true;


    QDomElement container;
    if( bOK ){
        container = SerializeCollector::findStoredGlobalElement(
                rootNode, ptrName, "kdchart:diagrams" );
        bOK = ! container.tagName().isEmpty();
    }

    if( bOK ) {
        SerializeCollector::instance()->setWasParsed( diagramPtr, true );

        const AbstractSerializerFactory* const f = Serializer::elementSerializerFactory( diagramPtr );

        ptr = diagramPtr;
        bOK = f->instance( diagramPtr->metaObject()->className() )->parseElement( container, ptr );
    }
    return bOK;
}

bool DiagramsSerializer::Private::doParseDiagram(
        const QDomElement& container,
        AbstractDiagram*& diagramPtr )const
{
    bool result = false;

    LineDiagram*  lineDiag  = qobject_cast< LineDiagram* >(  diagramPtr );
    BarDiagram*   barDiag   = qobject_cast< BarDiagram* >(   diagramPtr );
    PieDiagram*   pieDiag   = qobject_cast< PieDiagram* >(   diagramPtr );
    PolarDiagram* polarDiag = qobject_cast< PolarDiagram* >( diagramPtr );
    RingDiagram*  ringDiag  = qobject_cast< RingDiagram* >(  diagramPtr );
    Plotter*      plotDiag  = qobject_cast< Plotter* >(      diagramPtr );

    if( lineDiag )
        result = parseLineDiagram(  container, *lineDiag );
    else if( barDiag )
        result = parseBarDiagram(   container,  *barDiag );
    else if( pieDiag )
        result = parsePieDiagram(   container, *pieDiag );
    else if( polarDiag )
        result = parsePolarDiagram( container, *polarDiag );
    else if( ringDiag )
        result = parseRingDiagram(  container, *ringDiag );
    else if( plotDiag )
        result = parsePlotter( container, *plotDiag );

    return result;
}

void DiagramsSerializer::saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const
{
    d->saveDiagram( doc, e, qobject_cast< const AbstractDiagram* >( obj ) );
}

bool DiagramsSerializer::parseElement( const QDomElement& container, QObject* ptr ) const
{
    AbstractDiagram* diag = qobject_cast< AbstractDiagram* >( ptr );
    return d->doParseDiagram( container, diag );
}

void DiagramsSerializer::Private::saveDiagram(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractDiagram* p )const
{
    if( p == 0 )
        return;

    const LineDiagram*  lineDiag  = dynamic_cast<const LineDiagram*> (  p );
    const BarDiagram*   barDiag   = dynamic_cast<const BarDiagram*> (   p );
    const PieDiagram*   pieDiag   = dynamic_cast<const PieDiagram*> (   p );
    const PolarDiagram* polarDiag = dynamic_cast<const PolarDiagram*> ( p );
    const RingDiagram*  ringDiag  = dynamic_cast<const RingDiagram*> (  p );

    if( lineDiag )
        saveLineDiagram( doc, e, *lineDiag );
    else if( barDiag )
        saveBarDiagram( doc, e, *barDiag );
    else if( pieDiag )
        savePieDiagram( doc, e, *pieDiag );
    else if( polarDiag )
        savePolarDiagram( doc, e, *polarDiag );
    else if( ringDiag )
        saveRingDiagram( doc, e, *ringDiag );
}

bool DiagramsSerializer::Private::parseAbstractDiagram(
        const QDomElement& container, AbstractDiagram& diagram )const
{
    bool bOK = true;
    QDomNode node;

    //TODO(khz): Find a way to serialize the Model data!
    //
    // pass #1: assign the data model to the diagram
    //
    // This will be done to set the correct model first, so that
    // setting a RootIndex will use the right model then.
    /*
    node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Model" ) {
                QDomNode node2 = element.firstChild();
                if( ! node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( ! ele2.isNull() ) { // was really an element
                        QObject* ptr;
                        if( AttributesSerializer::parseQObjectPointerNode( ele2, ptr ) ){
                            QAbstractItemModel *model = dynamic_cast<QAbstractItemModel*>(ptr);
                            if( model ){
                                diagram.setModel( model );
                            }else{
                                qDebug()<< "Could not parse AbstractDiagram. Global pointer"
                                        << ele2.tagName() << "is not a KDChart::QAbstractItemModel-ptr.";
                                bOK = false;
                            }
                        }
                    }
                }
            }
        }
        node = node.nextSibling();
    }
    if( ! bOK ) return false;
    */


    // pass #2: retrieve all of the other settings, assuming that
    //          the data model has been set correctly to the diagram
    //
    // note: ATM pass #1 just is not done, so we can not set any RootIndex yet
    //       (khz, 2007 April 12)
    //
    node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "AttributesModel" ) {
                QDomNode node2 = element.firstChild();
                if( ! node2.isNull() ) {
                    QDomElement ele2 = node2.toElement();
                    if( ! ele2.isNull() ) { // was really an element
                        AttributesModel* model=0;
                        QObject* ptr;
                        QString ptrName;
                        bool wasParsed;
                        const bool isExternalModel =
                                AttributesSerializer::parseQObjectPointerNode(
                                        ele2, ptr,
                                        ptrName, wasParsed, false ) && ptr;
                        if( ptrName.isEmpty() ){
                            qDebug()<< "Could not parse AbstractDiagram. Global pointer node"
                                    << ele2.tagName() << "is invalid.";
                            bOK = false;
                        }else{
                            if( isExternalModel ){
                                model = dynamic_cast<AttributesModel*>(ptr);
                                if( ! model ){
                                    qDebug()<< "Could not parse AbstractDiagram. Global pointer"
                                            << ptrName << "is no AttributesModel-ptr.";
                                    bOK = false;
                                }
                            }else{
                                // If no external model stored
                                // use the built-in attributes-model
                                model = diagram.attributesModel();
                            }
                        }

                        if( bOK && ! wasParsed ){
                            SerializeCollector::instance()->setWasParsed( model, true );

                            bOK = m_attrModelS->parseAttributesModel(
                                    container.ownerDocument().firstChild(),
                                    ptrName, *model );
                        }

                        if( bOK ){
                            if( isExternalModel )
                                diagram.setAttributesModel( model );
                            else
                                diagram.attributesModel()->initFrom( model );
                        }else{
                            qDebug()<< "Could not parse AbstractDiagram / AttributesModel"
                                    << ptrName;
                            bOK = false;
                        }
                    }else{
                        qDebug()<< "Could not parse AbstractDiagram. Node does not contain a valid element.";
                        bOK = false;
                    }
                }else{
                    qDebug()<< "Could not parse AbstractDiagram. Node does not contain a valid element.";
                    bOK = false;
                }
            } else if( tagName == "RootIndex" ) {
                bool isValid;
                int column, row;
                if( KDXML::readModelIndexNode( element, isValid, column, row ) &&
                    isValid &&
                    (column || row) )
                {
                    qDebug()<< "RootIndex(" << column << "," << row << ") was stored for AbstractDiagram in\n    "
                        << AttributesSerializer::showDomPath( container.parentNode().parentNode().toElement() ) << "\n"
                        "      Make sure to adjust it via setRootIndex() after you have called setModel().";
                }
            } else if( tagName == "CoodinatePlane" ) {
                AbstractCoordinatePlane* plane;
                if( m_coordS->parsePlane( container.ownerDocument().firstChild(), element.firstChild(), plane ) ){
                    diagram.setCoordinatePlane( plane );
                }else{
                    // We do *not* report an error, since this is no bug:
                    // The coordinate plane will be set when the diagram is added to the plane.
                    // qDebug()<< "Could not parse AbstractDiagram / CoodinatePlane. Global pointer is not a KDChart::AbstractCoordinatePlane-ptr.";
                    // bOK = false;
                }
            } else if( tagName == "AllowOverlappingDataValueTexts" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) ){
                    diagram.setAllowOverlappingDataValueTexts( b );
                }else{
                    qDebug()<< "Could not parse AbstractDiagram. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "AntiAliasing" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) ){
                    diagram.setAntiAliasing( b );
                }else{
                    qDebug()<< "Could not parse AbstractDiagram. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "PercentMode" ) {
                bool b;
                if( KDXML::readBoolNode( element, b ) ){
                    diagram.setPercentMode( b );
                }else{
                    qDebug()<< "Could not parse AbstractDiagram. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "DatasetDimension" ) {
                int i;
                if( KDXML::readIntNode( element, i ) ){
                    diagram.setDatasetDimension( i );
                }else{
                    qDebug()<< "Could not parse AbstractDiagram. Element"
                            << tagName << "has invalid content.";
                }
            } else if ( tagName == TagNameQtProperties ) {
                // now parse parent class properties:
                if ( ! parseQtProperties( container, diagram ) ) {
                    qDebug() << "Could not parse base class Qt properties. Element"
                             << tagName << "has invalid content.";
                    bOK = false;
                }
            } else {
                qDebug() << "Unknown subelement of AbstractDiagram found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void DiagramsSerializer::Private::saveAbstractDiagram(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractDiagram& diagram,
        const QString& title )const
{
    QDomElement diagElement =
        doc.createElement( title );
    e.appendChild( diagElement );

    // save the attributes model
    m_attrModelS->saveAttributesModel(
        doc,
        diagElement,
        diagram.attributesModel(),
        diagram.usesExternalAttributesModel() );

    // save the root index
    if( diagram.rootIndex().isValid() &&
        (diagram.rootIndex().column() || diagram.rootIndex().row()) )
        KDXML::createModelIndexNode( doc, diagElement, "RootIndex",
                                     diagram.rootIndex() );

    // save the pointer to the associated coordinate plane,
    // and save the plane in the global structure if not there yet
    const CartesianCoordinatePlane* coordPlane =
            static_cast<const CartesianCoordinatePlane*>( diagram.coordinatePlane() );
    if( coordPlane && m_coordS ){
        QDomElement coordPlanePtrElement =
                doc.createElement( "CoodinatePlane" );
        diagElement.appendChild( coordPlanePtrElement );
        // access (or append, resp.) the global list
        QDomElement* planesList = SerializeCollector::instance()->findOrMakeElement( doc, m_coordS->globalList() );

        bool wasFound;
        QDomElement globalListElement =
                SerializeCollector::findOrMakeChild(
                doc,
                *planesList,
                coordPlanePtrElement,
                "kdchart:coordinate-plane",
                coordPlane->metaObject()->className(),
                coordPlane,
                wasFound );
        if( ! wasFound ){
            // Since the diagram is stored in the global structure anyway,
            // it is save to store it right now.
            // So it will not be forgotten, in case it is not embedded in any
            // of the coordinate planes.
            // The wasFound test makes sure it will not be stored twice.
            const AbstractSerializerFactory* factory = Serializer::elementSerializerFactory( coordPlane );
            const QObject* const obj = coordPlane;
            if( factory != 0 )
                factory->instance( coordPlane->metaObject()->className() )->saveElement( doc, globalListElement, obj );
        }
    }

    KDXML::createBoolNode( doc, diagElement, "AllowOverlappingDataValueTexts",
                           diagram.allowOverlappingDataValueTexts() );
    KDXML::createBoolNode( doc, diagElement, "AntiAliasing",
                           diagram.antiAliasing() );
    KDXML::createBoolNode( doc, diagElement, "PercentMode",
                           diagram.percentMode() );
    KDXML::createIntNode( doc, diagElement, "DatasetDimension",
                          diagram.datasetDimension() );
    // serialize Qt properties inherited from superclasses:
    saveQtProperties( doc, diagElement, diagram );
}


bool DiagramsSerializer::Private::parseCartCoordDiagram(
        const QDomElement& container, AbstractCartesianDiagram& diagram )const
{
    bool bOK = true;
    AbstractCartesianDiagram *refDiag=0;
    QPointF refDiagOffset(0.0, 0.0);
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-diagram" ) {
                if( ! parseAbstractDiagram( element, diagram ) ){
                    qDebug() << "Could not parse base class of AbstractCartesianDiagram.";
                    bOK = false;
                }
            } else if( tagName == "kdchart:axes:pointers" ) {
                QDomNode node2 = element.firstChild();
                while( ! node2.isNull() ) {
                    AbstractAxis *axis;
                    if( m_axesS->parseAxis( container.ownerDocument().firstChild(), node2, axis )
                        && qobject_cast< CartesianAxis * >(axis) )
                    {
                        diagram.addAxis( qobject_cast< CartesianAxis * >(axis) );
                    }else{
                        qDebug()<< "Could not parse element of AbstractCartesianDiagram / kdchart:axes:pointers.";
                        bOK = false;
                    }
                    node2 = node2.nextSibling();
                }
            } else if( tagName == "ReferenceDiagram" ) {
                AbstractDiagram* diag;
                refDiag = 0;
                if( q->parseDiagram( container.ownerDocument().firstChild(), element.firstChild(), diag ) )
                    refDiag = dynamic_cast<AbstractCartesianDiagram *>(diag);
                if( ! refDiag ){
                    qDebug()<< "Could not parse AbstractCartesianDiagram. Reference-diagram of"
                            << container.tagName() << "is not a KDChart::AbstractCartesianDiagram-ptr.";
                    bOK = false;
                }
            } else if( tagName == "Offset" ) {
                QPointF pt;
                if( KDXML::readPointFNode( element, pt ) ){
                    refDiagOffset = pt;
                }else{
                    qDebug()<< "Could not parse AbstractCartesianDiagram. Element \"Offset\" is not a QPointF node.";
                }
            } else {
                qDebug() << "Unknown subelement of AbstractCartesianDiagram found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    if( refDiag )
        diagram.setReferenceDiagram( refDiag, refDiagOffset );
    return bOK;
}

void DiagramsSerializer::Private::saveCartCoordDiagram(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractCartesianDiagram& diagram,
        const QString& title )const
{
    QDomElement diagElement =
        doc.createElement( title );
    e.appendChild( diagElement );

    // first save the information hold by the base class
    saveAbstractDiagram( doc, diagElement, diagram,
                         "kdchart:abstract-diagram" );

    // then save what is stored in the derived class

    // save the axes
    QList< const AbstractAxis* > abstractAxes;
    CartesianAxisList axes = diagram.axes();
    Q_FOREACH( const AbstractAxis* ax, axes )
        abstractAxes << ax;
    m_axesS->saveAxes( doc, diagElement, abstractAxes, "kdchart:axes" );

    // save the reference diagram(-pointer) and the respective offset, if any
    const AbstractCartesianDiagram* refDiag = diagram.referenceDiagram();
    if( refDiag ){
        QDomElement refDiagPtrElement =
                doc.createElement( "ReferenceDiagram" );
        diagElement.appendChild( refDiagPtrElement );
        // access (or append, resp.) the global list
        QDomElement* diagsList = SerializeCollector::instance()->findOrMakeElement( doc, m_globalList );

        bool wasFound;
        QDomElement globalListElement =
                SerializeCollector::findOrMakeChild(
                doc,
                *diagsList,
                refDiagPtrElement,
                "kdchart:diagram",
                refDiag->metaObject()->className(),
                refDiag,
                wasFound );
        if( ! wasFound ){
            // Since the diagram is stored in the global structure anyway,
            // it is save to store it right now.
            // So it will not be forgotten, in case it is not embedded in any
            // of the coordinate planes.
            // The wasFound test makes sure it will not be stored twice.
            saveDiagram( doc, globalListElement, refDiag );
        }
        KDXML::createPointFNode(
                doc, refDiagPtrElement, "Offset", diagram.referenceDiagramOffset() );
    }
}


bool DiagramsSerializer::Private::parsePolCoordDiagram(
        const QDomElement& container, AbstractPolarDiagram& diagram )const
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-diagram" ) {
                if( ! parseAbstractDiagram( element, diagram ) ){
                    qDebug() << "Could not parse base class of AbstractPolarDiagram.";
                    bOK = false;
                }
            } else {
                qDebug() << "Unknown subelement of AbstractPolarDiagram found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void DiagramsSerializer::Private::savePolCoordDiagram(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractPolarDiagram& diagram,
        const QString& title )const
{
    QDomElement diagElement =
            doc.createElement( title );
    e.appendChild( diagElement );

    // first save the information hold by the base class
    saveAbstractDiagram( doc, diagElement, diagram,
                         "kdchart:abstract-diagram" );
    // that's all, there is no to-be-saved information in this class
}


bool DiagramsSerializer::Private::parseLineDiagram(
        const QDomElement& container, LineDiagram& diagram )const
{
    //qDebug() << "-------->" << container.tagName();

    bool bOK = true;
    if( !container.isNull() ) {
        //qDebug() << "\n    DiagramsSerializer::parseLineDiagram() processing" << diagName;
        QDomNode node = container.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "kdchart:cartesian-coordinate-diagram" ) {
                    if( ! parseCartCoordDiagram( element, diagram ) ){
                        qDebug() << "Could not parse base class of LineDiagram.";
                        bOK = false;
                    }
                } else if( tagName == "LineType" ) {
                    QString s;
                    if( KDXML::readStringNode( element, s ) ){
                        if( s.compare("Normal", Qt::CaseInsensitive) == 0 )
                            diagram.setType( LineDiagram::Normal );
                        else if( s.compare("Stacked", Qt::CaseInsensitive) == 0 )
                            diagram.setType( LineDiagram::Stacked );
                        else if( s.compare("Percent", Qt::CaseInsensitive) == 0 )
                            diagram.setType( LineDiagram::Percent );
                        else{
                            bOK = false;
                            Q_ASSERT( false ); // all of the types need to be handled
                        }
                    }
                    else
                        bOK = false;
                } else {
                    qDebug() << "Unknown subelement of LineDiagram found:" << tagName;
                    bOK = false;
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

void DiagramsSerializer::Private::saveLineDiagram(
        QDomDocument& doc,
        QDomElement& diagElement,
        const LineDiagram& diagram )const
{
    // first save the information hold by the base class
    saveCartCoordDiagram( doc, diagElement, diagram,
                          "kdchart:cartesian-coordinate-diagram" );
    // then save what is stored in the derived class
    QString s;
    switch( diagram.type() ){
        case LineDiagram::Normal:
                s = "Normal";
                break;
        case LineDiagram::Stacked:
                s = "Stacked";
                break;
        case LineDiagram::Percent:
                s = "Percent";
                break;
        default:
            Q_ASSERT( false ); // all of the types need to be handled
            break;
    }
    KDXML::createStringNode( doc, diagElement, "LineType", s );
}

bool DiagramsSerializer::Private::parsePlotter(
        const QDomElement& container, Plotter& diagram )const
{
    //qDebug() << "-------->" << container.tagName();

    bool bOK = true;
    if( !container.isNull() ) {
        //qDebug() << "\n    DiagramsSerializer::parsePlotter() processing" << diagName;
        QDomNode node = container.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "kdchart:cartesian-coordinate-diagram" ) {
                    if( ! parseCartCoordDiagram( element, diagram ) ){
                        qDebug() << "Could not parse base class of Platter.";
                        bOK = false;
                    }
                } else if( tagName == "PlotType" ) {
                    QString s;
                    if( KDXML::readStringNode( element, s ) ){
                        if( s.compare("Normal", Qt::CaseInsensitive) == 0 )
                            diagram.setType( Plotter::Normal );
                        else{
                            bOK = false;
                            Q_ASSERT( false ); // all of the types need to be handled
                        }
                    }
                    else
                        bOK = false;
                } else {
                    qDebug() << "Unknown subelement of Plotter found:" << tagName;
                    bOK = false;
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

void DiagramsSerializer::Private::savePlotter(
        QDomDocument& doc,
        QDomElement& diagElement,
        const Plotter& diagram )const
{
    // first save the information hold by the base class
    saveCartCoordDiagram( doc, diagElement, diagram,
                          "kdchart:cartesian-coordinate-diagram" );
    // then save what is stored in the derived class
    QString s;
    switch( diagram.type() ){
        case Plotter::Normal:
                s = "Normal";
                break;
        default:
            Q_ASSERT( false ); // all of the types need to be handled
            break;
    }
    KDXML::createStringNode( doc, diagElement, "PlotType", s );
}

bool DiagramsSerializer::Private::parseBarDiagram(
        const QDomElement& container, BarDiagram& diagram )const
{
    bool bOK = true;
    if( !container.isNull() ) {
        //qDebug() << "\n    DiagramsSerializer::parseBarDiagram() processing" << diagName;
        QDomNode node = container.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "kdchart:cartesian-coordinate-diagram" ) {
                    if( ! parseCartCoordDiagram( element, diagram ) ){
                        qDebug() << "Could not parse base class of BarDiagram.";
                        bOK = false;
                    }
                } else if( tagName == "BarType" ) {
                    QString s;
                    if( KDXML::readStringNode( element, s ) ){
                        if( s.compare("Normal", Qt::CaseInsensitive) == 0 )
                            diagram.setType( BarDiagram::Normal );
                        else if( s.compare("Stacked", Qt::CaseInsensitive) == 0 )
                            diagram.setType( BarDiagram::Stacked );
                        else if( s.compare("Percent", Qt::CaseInsensitive) == 0 )
                            diagram.setType( BarDiagram::Percent );
                        else if( s.compare("Rows", Qt::CaseInsensitive) == 0 )
                            diagram.setType( BarDiagram::Rows );
                        else{
                            bOK = false;
                            Q_ASSERT( false ); // all of the types need to be handled
                        }
                    }
                    else
                        bOK = false;
                } else {
                    qDebug() << "Unknown subelement of BarDiagram found:" << tagName;
                    bOK = false;
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

void DiagramsSerializer::Private::saveBarDiagram(
        QDomDocument& doc,
        QDomElement& diagElement,
        const BarDiagram& diagram )const
{
    // first save the information hold by the base class
    saveCartCoordDiagram( doc, diagElement, diagram,
                          "kdchart:cartesian-coordinate-diagram" );

    // then save what is stored in the derived class
    QString s;
    switch( diagram.type() )
    {
    case BarDiagram::Normal:
        s = "Normal";
        break;
    case BarDiagram::Stacked:
        s = "Stacked";
        break;
    case BarDiagram::Percent:
        s = "Percent";
        break;
    case BarDiagram::Rows:
        s = "Rows";
        break;
    default:
        Q_ASSERT( false ); // all of the types need to be handled
        break;
    }
    KDXML::createStringNode( doc, diagElement, "BarType", s );
}


bool DiagramsSerializer::Private::parseAbstractPieDiagram(
        const QDomElement& container, AbstractPieDiagram& diagram )const
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:polar-coordinate-diagram" ) {
                if( ! parsePolCoordDiagram( element, diagram ) ){
                    qDebug() << "Could not parse base class of AbstractPieDiagram.";
                    bOK = false;
                }
            } else if( tagName == "Granularity" ) {
                qreal r;
                if( KDXML::readRealNode( element, r ) ){
                    diagram.setGranularity( r );
                }else{
                    qDebug()<< "Could not parse AbstractPieDiagram. Element"
                            << tagName << "has invalid content.";
                }
            } else if( tagName == "StartPosition" ) {
                // setStartPosition is deprecated!
                /*
                int i;
                if( KDXML::readIntNode( element, i ) ){
                    diagram.setStartPosition( i );
                }else{
                    qDebug()<< "Could not parse AbstractPieDiagram. Element"
                            << tagName << "has invalid content.";
                }
                */
            } else {
                qDebug() << "Unknown subelement of AbstractPieDiagram found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void DiagramsSerializer::Private::saveAbstractPieDiagram(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractPieDiagram& diagram,
        const QString& title )const
{
    QDomElement diagElement =
            doc.createElement( title );
    e.appendChild( diagElement );

    // first save the information hold by the base class
    savePolCoordDiagram( doc, diagElement, diagram,
                         "kdchart:polar-coordinate-diagram" );

    // then save what is stored in the derived class
    KDXML::createRealNode( doc, diagElement, "Granularity",   diagram.granularity() );
    // setStartPosition is deprecated!
    //KDXML::createIntNode(  doc, diagElement, "StartPosition", diagram.startPosition() );
}


bool DiagramsSerializer::Private::parsePieDiagram(
        const QDomElement& container, PieDiagram& diagram )const
{
    bool bOK = true;
    if( !container.isNull() ) {
        //qDebug() << "\n    DiagramsSerializer::parsePieDiagram() processing" << diagName;
        QDomNode node = container.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "kdchart:abstract-pie-diagram" ) {
                    if( ! parseAbstractPieDiagram( element, diagram ) ){
                        qDebug() << "Could not parse base class of PieDiagram.";
                        bOK = false;
                    }
                } else {
                    qDebug() << "Unknown subelement of PieDiagram found:" << tagName;
                    bOK = false;
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

void DiagramsSerializer::Private::savePieDiagram(
        QDomDocument& doc,
        QDomElement& diagElement,
        const PieDiagram& diagram )const
{
    // first save the information hold by the base class
    saveAbstractPieDiagram( doc, diagElement, diagram,
                            "kdchart:abstract-pie-diagram" );
    // that's all, there is no to-be-saved information in this class
}


bool DiagramsSerializer::Private::parsePolarDiagram(
        const QDomElement& container, PolarDiagram& diagram )const
{
    bool bOK = true;
    if( !container.isNull() ) {
        //qDebug() << "\n    DiagramsSerializer::parsePolarDiagram() processing" << diagName;
        QDomNode node = container.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "kdchart:polar-coordinate-diagram" ) {
                    if( ! parsePolCoordDiagram( element, diagram ) ){
                        qDebug() << "Could not parse base class of PolarDiagram.";
                        bOK = false;
                    }
                } else if( tagName == "ZeroDegreePosition" ) {
                    int i;
                    if( KDXML::readIntNode( element, i ) ){
                        diagram.setZeroDegreePosition( i );
                    }else{
                        qDebug()<< "Could not parse PolarDiagram. Element"
                                << tagName << "has invalid content.";
                    }
                } else if( tagName == "RotateCircularLabels" ) {
                    bool b;
                    if( KDXML::readBoolNode( element, b ) ){
                        diagram.setRotateCircularLabels( b );
                    }else{
                        qDebug()<< "Could not parse PolarDiagram. Element"
                                << tagName << "has invalid content.";
                    }
                } else if( tagName == "CloseDatasets" ) {
                    bool b;
                    if( KDXML::readBoolNode( element, b ) ){
                        diagram.setCloseDatasets( b );
                    }else{
                        qDebug()<< "Could not parse PolarDiagram. Element"
                                << tagName << "has invalid content.";
                    }
                } else if( tagName == "ShowDelimitersAtPosition" ) {
                    bool unknown = false;
                    bool center = false;
                    bool northWest = false;
                    bool north = false;
                    bool northEast = false;
                    bool east = false;
                    bool southEast = false;
                    bool south = false;
                    bool southWest = false;
                    bool west = false;
                    bool floating = false;
                    if( KDXML::readPositionBooleansNode(
                            element,
                            unknown, center,
                            northWest, north, northEast,
                            east, southEast, south, southWest, west,
                            floating ) )
                    {
                        diagram.setShowDelimitersAtPosition( Position::Unknown,  unknown );
                        diagram.setShowDelimitersAtPosition( Position::Center,   center );
                        diagram.setShowDelimitersAtPosition( Position::NorthWest,northWest );
                        diagram.setShowDelimitersAtPosition( Position::North,    north );
                        diagram.setShowDelimitersAtPosition( Position::NorthEast,northEast );
                        diagram.setShowDelimitersAtPosition( Position::East,     east );
                        diagram.setShowDelimitersAtPosition( Position::SouthEast,southEast );
                        diagram.setShowDelimitersAtPosition( Position::South,    south );
                        diagram.setShowDelimitersAtPosition( Position::SouthWest,southWest );
                        diagram.setShowDelimitersAtPosition( Position::West,     west );
                        diagram.setShowDelimitersAtPosition( Position::Floating, floating );
                    }else{
                        qDebug()<< "Could not parse PolarDiagram. Element"
                                << tagName << "has invalid content.";
                    }
                } else if( tagName == "ShowLabelsAtPosition" ) {
                    bool unknown = false;
                    bool center = false;
                    bool northWest = false;
                    bool north = false;
                    bool northEast = false;
                    bool east = false;
                    bool southEast = false;
                    bool south = false;
                    bool southWest = false;
                    bool west = false;
                    bool floating = false;
                    if( KDXML::readPositionBooleansNode(
                            element,
                            unknown, center,
                            northWest, north, northEast,
                            east, southEast, south, southWest, west,
                            floating ) )
                    {
                        diagram.setShowLabelsAtPosition( Position::Unknown,  unknown );
                        diagram.setShowLabelsAtPosition( Position::Center,   center );
                        diagram.setShowLabelsAtPosition( Position::NorthWest,northWest );
                        diagram.setShowLabelsAtPosition( Position::North,    north );
                        diagram.setShowLabelsAtPosition( Position::NorthEast,northEast );
                        diagram.setShowLabelsAtPosition( Position::East,     east );
                        diagram.setShowLabelsAtPosition( Position::SouthEast,southEast );
                        diagram.setShowLabelsAtPosition( Position::South,    south );
                        diagram.setShowLabelsAtPosition( Position::SouthWest,southWest );
                        diagram.setShowLabelsAtPosition( Position::West,     west );
                        diagram.setShowLabelsAtPosition( Position::Floating, floating );
                    }else{
                        qDebug()<< "Could not parse PolarDiagram. Element"
                                << tagName << "has invalid content.";
                    }
                } else {
                    qDebug() << "Unknown subelement of PolarDiagram found:" << tagName;
                    bOK = false;
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

void DiagramsSerializer::Private::savePolarDiagram(
        QDomDocument& doc,
        QDomElement& diagElement,
        const PolarDiagram& diagram )const
{
    // first save the information hold by the base class
    savePolCoordDiagram( doc, diagElement, diagram,
                         "kdchart:polar-coordinate-diagram" );

    // then save what is stored in the derived class
    KDXML::createIntNode(  doc, diagElement, "ZeroDegreePosition",   diagram.zeroDegreePosition() );
    KDXML::createBoolNode( doc, diagElement, "RotateCircularLabels", diagram.rotateCircularLabels() );
    KDXML::createBoolNode( doc, diagElement, "CloseDatasets", diagram.closeDatasets() );
    KDXML::createPositionBooleansNode(
            doc, diagElement, "ShowDelimitersAtPosition",
            diagram.showDelimitersAtPosition( Position::Unknown ),
            diagram.showDelimitersAtPosition( Position::Center ),
            diagram.showDelimitersAtPosition( Position::NorthWest ),
            diagram.showDelimitersAtPosition( Position::North ),
            diagram.showDelimitersAtPosition( Position::NorthEast ),
            diagram.showDelimitersAtPosition( Position::East ),
            diagram.showDelimitersAtPosition( Position::SouthEast ),
            diagram.showDelimitersAtPosition( Position::South ),
            diagram.showDelimitersAtPosition( Position::SouthWest ),
            diagram.showDelimitersAtPosition( Position::West ),
            diagram.showDelimitersAtPosition( Position::Floating ) );
    KDXML::createPositionBooleansNode(
            doc, diagElement, "ShowLabelsAtPosition",
            diagram.showLabelsAtPosition( Position::Unknown ),
            diagram.showLabelsAtPosition( Position::Center ),
            diagram.showLabelsAtPosition( Position::NorthWest ),
            diagram.showLabelsAtPosition( Position::North ),
            diagram.showLabelsAtPosition( Position::NorthEast ),
            diagram.showLabelsAtPosition( Position::East ),
            diagram.showLabelsAtPosition( Position::SouthEast ),
            diagram.showLabelsAtPosition( Position::South ),
            diagram.showLabelsAtPosition( Position::SouthWest ),
            diagram.showLabelsAtPosition( Position::West ),
            diagram.showLabelsAtPosition( Position::Floating ) );
}

bool DiagramsSerializer::Private::parseRingDiagram(
        const QDomElement& container, RingDiagram& diagram )const
{
    bool bOK = true;
    if( !container.isNull() ) {
        //qDebug() << "\n    DiagramsSerializer::parseRingDiagram() processing" << diagName;
        QDomNode node = container.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "kdchart:abstract-pie-diagram" ) {
                    if( ! parseAbstractPieDiagram( element, diagram ) ){
                        qDebug() << "Could not parse base class of RingDiagram.";
                        bOK = false;
                    }
                } else if( tagName == "RelativeThickness" ) {
                    bool b;
                    if( KDXML::readBoolNode( element, b ) ){
                        diagram.setRelativeThickness( b );
                    }else{
                        qDebug()<< "Could not parse RingDiagram. Element"
                                << tagName << "has invalid content.";
                    }
                } else {
                    qDebug() << "Unknown subelement of RingDiagram found:" << tagName;
                    bOK = false;
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

void DiagramsSerializer::Private::saveRingDiagram(
        QDomDocument& doc,
        QDomElement& diagElement,
        const RingDiagram& diagram )const
{
    // first save the information hold by the base class
    saveAbstractPieDiagram( doc, diagElement, diagram,
                            "kdchart:abstract-pie-diagram" );

    // then save what is stored in the derived class
    KDXML::createBoolNode(  doc, diagElement, "RelativeThickness",   diagram.relativeThickness() );
}

void DiagramsSerializer::Private::saveQtProperties(
    QDomDocument& doc,
    QDomElement& e,
    const AbstractDiagram& diagram ) const
{   // this function saves all properties of a QObject that are in the
    // white list, creating a element for the properties and one child
    // element per property
    static const QStringList SerializedPropertyWhiteList
        ( QStringList() << "frameStyle" << "lineWidth" << "midLineWidth" );

    QDomElement element = doc.createElement( TagNameQtProperties );
    e.appendChild( element );
    for ( int i = 0; i < diagram.metaObject()->propertyCount(); ++i ) {
        QMetaProperty p = diagram.metaObject()->property( i );
        if ( SerializedPropertyWhiteList.contains( p.name() ) ) {
            QVariant value = diagram.property( p.name() );
            KDXML::createQVariantNode( doc, element, p.name(), value );
        }
    }
}

bool DiagramsSerializer::Private::parseQtProperties(
    const QDomElement& container,
    AbstractDiagram& diagram ) const
{
    bool error = false;
    QDomNodeList elements = container.elementsByTagName( "properties" );
    if ( elements.size() != 1 ) {
        qDebug() << "DiagramsSerializer::Private::parseQtProperties: XML syntax error, more than one properties elements";
        error = true;
    }
    if ( elements.size() > 0 ) {
        QDomElement properties = elements.at(0).toElement();

        QDomNode n = properties.firstChild();
        while ( !n.isNull() ) {
            QDomElement e = n.toElement();
            if ( !e.isNull() ) {
                QVariant value;
                QString name;
                if ( ! KDXML::readQVariantNode( e, value, name ) ) {
                    qDebug() << "DiagramsSerializer::Private::parseQtProperties: error parsing property" << e.tagName();
                    error = true;
                } else {
                    diagram.setProperty( qPrintable( name ), value );
                }
            }
            n = n.nextSibling();
        }
    }
    return ! error;
}
