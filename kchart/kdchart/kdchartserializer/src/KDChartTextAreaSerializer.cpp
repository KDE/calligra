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

#include <KDChartTextAreaSerializer>
#include <KDChartTextAreaSerializer_p.h>

#include <KDChartAttributesSerializer>
#include <KDChartAbstractAreaBaseSerializer>
#include <KDChartSerializeCollector>
#include <KDChartAbstractSerializerFactory>
#include <KDChartSerializer>

#include <KDXMLTools>

#include <qglobal.h>

#define d d_func()

using namespace KDChart;

/**
  \class KDChart::TextAreaSerializer KDChartTextAreaSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */

TextAreaSerializer::Private::Private( TextAreaSerializer* qq )
    : q( qq )
{
}

TextAreaSerializer::Private::~Private()
{
}

TextAreaSerializer::TextAreaSerializer()
    : _d( new Private( this ) )
{
}

TextAreaSerializer::~TextAreaSerializer()
{
    delete _d; _d = 0;
}

void TextAreaSerializer::init()
{
}

void TextAreaSerializer::saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const
{
    const HeaderFooter* hf = qobject_cast< const HeaderFooter* >( obj );
    if( hf != 0 )
        Private::saveHeaderFooter( doc, e, *hf );
}

bool TextAreaSerializer::parseElement( const QDomElement& container, QObject* ptr ) const
{
    HeaderFooter* hf = qobject_cast< HeaderFooter* >( ptr );
    return d->doParseHeaderFooter( container, hf );
}

void TextAreaSerializer::saveHeadersFooters(
        QDomDocument& doc,
        QDomElement& e,
        const KDChart::HeaderFooterList& areas,
        const QString& title )
{
    // access (or append, resp.) the global list
    QDomElement* hdFtList =
            SerializeCollector::instance()->findOrMakeElement( doc, title );

    // create the local list holding names pointing into the global list
    QDomElement pointersList =
            SerializeCollector::createPointersList( doc, e, title );

    Q_FOREACH ( const HeaderFooter* p, areas )
    {
        bool wasFound;
        QDomElement hdFtElement =
                SerializeCollector::findOrMakeChild(
                doc,
                *hdFtList,
                pointersList,
                "kdchart:header-footer",
                p->metaObject()->className(),
                p,
                wasFound );
        if( ! wasFound )
        {
            const AbstractSerializerFactory* factory = Serializer::elementSerializerFactory( p );
            const QObject* obj = p;
            if( factory != 0 ){
                factory->instance( p->metaObject()->className() )->saveElement( doc, hdFtElement, obj );
            }else{
                qDebug() << "\nProblem: Can not store header-footer:" << p->metaObject()->className();
            }
        }
    }
}


bool TextAreaSerializer::parseHeaderFooter(
        const QDomNode& rootNode,
        const QDomNode& pointerNode,
        HeaderFooter*& hdFt )
{
    bool bOK = true;
    hdFt=0;

    QObject* ptr;
    QString ptrName;
    bool wasParsed;
    const bool pointerFound =
            AttributesSerializer::parseQObjectPointerNode(
                    pointerNode, ptr,
                    ptrName, wasParsed, true ) && ptr;

    if( ptrName.isEmpty() ){
        qDebug()<< "Could not parse header/footer. Global pointer node is invalid.";
        bOK = false;
    }else{
        if( pointerFound ){
            hdFt = dynamic_cast<HeaderFooter*>(ptr);
            if( ! hdFt ){
                qDebug()<< "Could not parse header/footer. Global pointer"
                        << ptrName << "is no HeaderFooter-ptr.";
                bOK = false;
            }
        }else{
            qDebug()<< "Could not parse header/footer. Global pointer"
                    << ptrName << "is no HeaderFooter-ptr.";
            bOK = false;
        }
    }


    if( bOK && wasParsed ) return true;


    QDomElement container;
    if( bOK ){
        container = SerializeCollector::findStoredGlobalElement(
                rootNode, ptrName, "kdchart:headers-footers" );
        bOK = ! container.tagName().isEmpty();
    }

    if( bOK ) {
        SerializeCollector::instance()->setWasParsed( hdFt, true );
        const AbstractSerializerFactory* factory = Serializer::elementSerializerFactory( hdFt );
        QObject* obj = hdFt;
        if( factory != 0 )
            return factory->instance( hdFt->metaObject()->className() )->parseElement( container, obj );
        return false;
    }

    return bOK;
}

bool TextAreaSerializer::Private::doParseHeaderFooter( const QDomElement& container, HeaderFooter*& hdFt )
{
    bool bOK = true;
    QString s;
    if( KDXML::findStringAttribute( container, "type", s ) ){
        if( ! s.isEmpty() ){
            if( s.compare("Header", Qt::CaseInsensitive) == 0 ){
                hdFt->setType( HeaderFooter::Header );
            }else if( s.compare("Footer", Qt::CaseInsensitive) == 0 ){
                hdFt->setType( HeaderFooter::Footer );
            }else{
                qDebug() << "Empty type attribute found in HeaderFooter.";
                bOK = false;
            }
        } else {
            qDebug() << "Invalid type attribute found in HeaderFooter: \"" << s << "\"";
            bOK = false;
        }
    } else {
        qDebug() << "No type attribute found in HeaderFooter element.";
        bOK = false;
    }

    if( bOK ){
        QDomNode node = container.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName == "Position" ) {
                    QString s;
                    if( KDXML::readStringNode( element, s ) )
                        hdFt->setPosition( Position::fromName( s.toLatin1() ) );
                    else
                        bOK = false;
                } else if( tagName == "kdchart:text-area" ) {
                    // parse the base class:
                    if( ! parseTextArea( element, *hdFt ) )
                        bOK = false;
                } else {
                    qDebug() << "Unknown subelement of HeaderFooter found:" << tagName;
                    bOK = false;
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

void TextAreaSerializer::Private::saveHeaderFooter(
        QDomDocument& doc,
        QDomElement& hdFtElement,
        const KDChart::HeaderFooter& hdFt )
{
    // first save the information hold by the base class
    QString type;
    switch( hdFt.type() ){
        case HeaderFooter::Header:
            type = "Header";
            break;
        case HeaderFooter::Footer:
            type = "Footer";
            break;
        default:
            Q_ASSERT( false ); // all of the types need to be handled
            break;
    }
    hdFtElement.setAttribute( "type", type );
    saveTextArea( doc, hdFtElement, hdFt,
                    "kdchart:text-area" );

    // then save what is stored in the derived class
    KDXML::createStringNode( doc, hdFtElement, "Position", hdFt.position().name() );
}


bool TextAreaSerializer::Private::parseTextArea(
        const QDomElement& container,
        TextArea& area )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "kdchart:abstract-area-base" ) {
                if( ! AbstractAreaBaseSerializer::parseAbstractAreaBase( element, area ) )
                    bOK = false;
            } else if( tagName == "kdchart:text-layout-item" ) {
                if( ! parseTextLayoutItem( element, area ) )
                    bOK = false;
            } else {
                qDebug() << "Unknown subelement of TextArea found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void TextAreaSerializer::Private::saveTextArea(
        QDomDocument& doc,
        QDomElement& e,
        const TextArea& area,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );

    // first save the information hold by both of the base classes
    AbstractAreaBaseSerializer::saveAbstractAreaBase(
            doc,
            element,
            area,
            "kdchart:abstract-area-base" );
    saveTextLayoutItem( doc, element, area, "kdchart:text-layout-item" );
    // that's all, there is no to-be-saved information in this class
}


bool TextAreaSerializer::Private::parseTextLayoutItem(
        const QDomElement& container,
        TextLayoutItem& item )
{
    bool bOK = true;
    QDomNode node = container.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "AutoReferenceArea" ) {
                QObject* ptr;
                QString ptrName;
                bool wasParsed;
                if( AttributesSerializer::parseQObjectPointerNode(
                        element.firstChild(), ptr, ptrName, wasParsed, true ) )
                    item.setAutoReferenceArea( ptr );
                else
                    bOK = false;
            } else if( tagName == "Text" ) {
                QString s;
                if( KDXML::readStringNode( element, s ) )
                    item.setText( s );
                else
                    bOK = false;
            } else if( tagName == "TextAttributes" ) {
                TextAttributes ta;
                if( AttributesSerializer::parseTextAttributes( element, ta ) )
                    item.setTextAttributes( ta );
                else
                    bOK = false;
            } else {
                qDebug() << "Unknown subelement of TextLayoutItem found:" << tagName;
                bOK = false;
            }
        }
        node = node.nextSibling();
    }
    return bOK;
}

void TextAreaSerializer::Private::saveTextLayoutItem(
        QDomDocument& doc,
        QDomElement& e,
        const TextLayoutItem& item,
        const QString& title )
{
    QDomElement element = doc.createElement( title );
    e.appendChild( element );

    AttributesSerializer::saveQObjectPointer(
            doc, element, item.autoReferenceArea(), "AutoReferenceArea" );
    KDXML::createStringNode( doc, element, "Text", item.text() );
    AttributesSerializer::saveTextAttributes(
            doc, element, item.textAttributes(), "TextAttributes" );
}
