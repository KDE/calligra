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

#include <KDChartAxesSerializer>
#include <KDChartAxesSerializer_p.h>

#include <KDChartAttributesSerializer>
#include <KDChartAbstractAreaBaseSerializer>
#include <KDChartSerializeCollector>

#include <KDChartAbstractSerializerFactory>
#include <KDChartSerializer>

#include <KDXMLTools>

#include <KDChartCartesianAxis>
//TODO once PolarAxis is implemented: #include <KDChartPolarAxis>


#include <qglobal.h>
#include <QMessageBox>


#define d d_func()

using namespace KDChart;

/**
  \class KDChart::AxesSerializer KDChartAxesSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */

AxesSerializer::Private::Private( AxesSerializer* qq )
    : q( qq )
{
}

AxesSerializer::Private::~Private() {}


AxesSerializer::AxesSerializer()
    : _d( new Private( this ) )
{
    // this space left empty intentionally
}

AxesSerializer::~AxesSerializer()
{
    delete _d; _d = 0;
}

void AxesSerializer::init()
{
}

void AxesSerializer::saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const
{
    if( qobject_cast< const CartesianAxis* >( obj ) != 0 )
    {
        const CartesianAxis* ax = qobject_cast< const CartesianAxis* >( obj );
        d->saveCartesianAxis( doc, e, *ax );
    }
//    else if( qobject_cast< const PolarAxis* >( obj ) != 0 )
//    {
//        const PolarAxis* ax = qobject_cast< const PolarAxis* >( obj );
//        d->savePolarAxis( doc, e, *ax );
//    }
}

bool AxesSerializer::parseElement( const QDomElement& container, QObject* ptr ) const
{
    if( qobject_cast< CartesianAxis* >( ptr ) != 0 )
    {   
        CartesianAxis* axis = qobject_cast< CartesianAxis* >( ptr );
        return d->doParseCartesianAxis( container, axis );
    }
//    else if( qobject_cast< PolarAxis* >( ptr ) != 0 )
//    {   
//        PolarAxis* axis = qobject_cast< PolarAxis* >( ptr );
//        return d->doParsePolarAxis( container, axis );
//    }
    return false;
}

void AxesSerializer::saveAxes(
        QDomDocument& doc,
        QDomElement& e,
        const QList< const AbstractAxis* >& axes,
        const QString& title )const
{
    // access (or append, resp.) the global list
    QDomElement* axesList =
            SerializeCollector::instance()->findOrMakeElement( doc, title );

    // create the local list holding names pointing into the global list
    QDomElement pointersList =
            SerializeCollector::createPointersList( doc, e, title );

    Q_FOREACH ( const AbstractAxis* p, axes )
    {
        bool wasFound;
        QDomElement axisElement =
                SerializeCollector::findOrMakeChild(
                        doc,
                        *axesList,
                        pointersList,
                        "kdchart:axis",
                        p->metaObject()->className(),
                        p,
                        wasFound );
        if( ! wasFound ){
            const AbstractSerializerFactory* factory = Serializer::elementSerializerFactory( p );
            const QObject* obj = p;
            if( factory != 0 )
                factory->instance( p->metaObject()->className() )->saveElement( doc, axisElement, obj );
        }
    }
}

bool AxesSerializer::parseAxis(
        const QDomNode& rootNode,
        const QDomNode& pointerNode,
        AbstractAxis*& axisPtr )const
{
    bool bOK = true;
    axisPtr = 0;

    QObject* ptr;
    QString ptrName;
    bool wasParsed;
    const bool pointerFound =
            AttributesSerializer::parseQObjectPointerNode(
            pointerNode, ptr,
    ptrName, wasParsed, true ) && ptr;

    if( ptrName.isEmpty() ){
        qDebug()<< "Could not parse AbstractAxis. Global pointer node is invalid.";
        bOK = false;
    }else{
        if( pointerFound ){
            axisPtr = dynamic_cast<AbstractAxis*>(ptr);
            if( ! axisPtr ){
                qDebug()<< "Could not parse AbstractAxis. Global pointer"
                        << ptrName << "is no AbstractAxis-ptr.";
                bOK = false;
            }
        }else{
            qDebug()<< "Could not parse AbstractAxis. Global pointer"
                    << ptrName << "is no AbstractAxis-ptr.";
            bOK = false;
        }
    }


    if( bOK && wasParsed ) return true;


    QDomElement axisElement;
    if( bOK ){
        axisElement = SerializeCollector::findStoredGlobalElement(
                rootNode, ptrName, "kdchart:axes" );
        bOK = ! axisElement.tagName().isEmpty();
    }

    if( bOK ) {
        SerializeCollector::instance()->setWasParsed( axisPtr, true );

        const AbstractSerializerFactory* factory = Serializer::elementSerializerFactory( axisPtr );
        QObject* obj = axisPtr;
        if( factory != 0 )
            return factory->instance( axisPtr->metaObject()->className() )
                    ->parseElement( axisElement, obj );
        return false;
    }

    return bOK;
}



bool AxesSerializer::Private::doParseCartesianAxis( const QDomElement& axisElement, CartesianAxis*& axisPtr )const
{
    bool bOK = true;
    const QString axisName = axisElement.tagName();
    //qDebug() << "\n    AxesSerializer::parseCartesianAxis() processing" << axisName;
    QDomNode node = axisElement.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-axis" ) {
                if( ! q->parseAbstractAxis( element, *axisPtr ) )
                    bOK = false;
            } else if( tagName == "Title" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    axisPtr->setTitleText( s );
                else
                    bOK = false;
            } else if( tagName == "TitleTextAttributes" ) {
                TextAttributes ta;
                if( AttributesSerializer::parseTextAttributes( element, ta ) )
                    axisPtr->setTitleTextAttributes( ta );
                else
                    bOK = false;
            } else if( tagName == "Position" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) ){
                    CartesianAxis::Position pos;
                    if( s.compare("bottom", Qt::CaseInsensitive) == 0 )
                        pos = CartesianAxis::Bottom;
                    else if( s.compare("top", Qt::CaseInsensitive) == 0 )
                        pos = CartesianAxis::Top;
                    else if( s.compare("right", Qt::CaseInsensitive) == 0 )
                        pos = CartesianAxis::Right;
                    else if( s.compare("left", Qt::CaseInsensitive) == 0 )
                        pos = CartesianAxis::Left;
                    else{
                        qDebug()<< "Unknown value of CartesianAxis/Position found:"
                                << s;
                        bOK = false;
                    }
                    if( bOK )
                        axisPtr->setPosition( pos );
                }else{
                    qDebug() << "Invalid CartesianAxis/Position element found.";
                    bOK = false;
                }
            } else {
                qDebug()<< "Unknown subelement of CartesianAxis found:"
                        << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    
    return bOK;
}

void AxesSerializer::Private::saveCartesianAxis(
        QDomDocument& doc,
        QDomElement& axisElement,
        const CartesianAxis& axis )const
{
    // first save the information hold by the base class
    saveAbstractAxis( doc, axisElement, axis,
                      "kdchart:abstract-axis" );

    // save the title
    KDXML::createStringNodeIfContent(
            doc, axisElement,
            "Title", axis.titleText() );
    if( ! axis.hasDefaultTitleTextAttributes() ){
        // save the title text attributes
        AttributesSerializer::saveTextAttributes(
                doc, axisElement,
                axis.titleTextAttributes(),
                "TitleTextAttributes" );
    }
    QString s;
    switch( axis.position() ){
        case CartesianAxis::Bottom:
            s = "bottom";
            break;
        case CartesianAxis::Top:
            s = "top";
            break;
        case CartesianAxis::Right:
            s = "right";
            break;
        case CartesianAxis::Left:
            s = "left";
            break;
        default:
            Q_ASSERT( false ); // all of the positions need to be handled
            break;
    }
    KDXML::createStringNode(
            doc, axisElement,
            "Position", s );
}

//TODO once PolarAxis is implemented:
/*
bool AxesSerializer::Private::doParsePolarAxis( const QDomElement& container, CartesianAxis*& axisPtr )const
{
    bool bOK = true;
    // ..
    return bOK;
}

void AxesSerializer::Private::savePolarAxis(
        QDomDocument& doc,
        QDomElement& axisElement,
        const PolarAxis& axis )const
{
    // first save the information hold by the base class
    saveAbstractAxis( doc, axisElement, axis,
                      "kdchart:abstract-axis" );

    // ...
}

*/


bool AxesSerializer::parseAbstractAxis(
        const QDomElement& container,
        AbstractAxis& axis )const
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-area-base" ) {
                if( ! AbstractAreaBaseSerializer::parseAbstractAreaBase( element, axis ) )
                    bOK = false;
            } else if( tagName == "TextAttributes" ) {
                TextAttributes ta;
                if( AttributesSerializer::parseTextAttributes( element, ta ) )
                    axis.setTextAttributes( ta );
                else
                    bOK = false;
            } else if( tagName == "Labels" ) {
                QStringList list;
                if( KDXML::readStringListNode( element, list ) )
                    axis.setLabels( list );
                else
                    bOK = false;
            } else if( tagName == "ShortLabels" ) {
                QStringList list;
                if( KDXML::readStringListNode( element, list ) )
                    axis.setShortLabels( list );
                else
                    bOK = false;
            } else {
                qDebug() << "Unknown subelement of AbstractAxis found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void AxesSerializer::Private::saveAbstractAxis(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractAxis& axis,
        const QString& title )const
{
    QDomElement axisElement =
            doc.createElement( title );
    e.appendChild( axisElement );

    // save the area information
    AbstractAreaBaseSerializer::saveAbstractAreaBase(
            doc,
            axisElement,
            axis,
            "kdchart:abstract-area-base" );
    // save the text attributes
    AttributesSerializer::saveTextAttributes(
            doc, axisElement,
            axis.textAttributes(),
            "TextAttributes" );
    // save the labels
    QStringList list( axis.labels() );
    if( list.count() )
        KDXML::createStringListNodes(
                doc, axisElement,
                "Labels", &list );
    // save the short labels
    list = axis.shortLabels();
    if( list.count() )
        KDXML::createStringListNodes(
                doc, axisElement,
                "ShortLabels", &list );
}
