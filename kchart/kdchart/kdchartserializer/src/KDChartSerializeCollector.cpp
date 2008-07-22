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

#include <KDChartSerializeCollector>
#include <KDChartSerializeCollector_p.h>

#include <KDChartIdMapper>


// Global objects of the following classes can be
// instantiated by initializeGlobalPointers()
#include <KDChartAttributesModel>
#include <KDChartCartesianAxis>
// once that class is implemented: #include "KDChartPolarAxis"
#include <KDChartChart>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartPolarCoordinatePlane>
#include <KDChartHeaderFooter>
#include <KDChartLegend>
#include <KDChartAbstractSerializerFactory>
#include <KDChartSerializer>

#include <KDXMLTools.h>

#include <QDebug>

#define d d_func()

using namespace KDChart;

SerializeCollector::Private::Private( SerializeCollector* qq )
    : q( qq )
{
}

SerializeCollector::Private::~Private() {}

QString SerializeCollector::Private::unresolvedTagName()
{
    return QString::fromLatin1( "U_N_R_E_S_O_L_V_E_D" );
}
QString SerializeCollector::Private::unresolvedMapName()
{
    return QString::fromLatin1( "kdchart:unresolved-pointers" );
}

SerializeCollector::SerializeCollector()
    : _d( new Private( this ) )
{
    // this space left empty intentionally
}

SerializeCollector::~SerializeCollector()
{
    clear();
    delete _d; _d = 0;
}

void SerializeCollector::clear()
{
    // this bloc left empty intentionally
}

void SerializeCollector::init()
{
}

SerializeCollector* SerializeCollector::instance()
{
    static SerializeCollector instance;
    return &instance;
}


// ******************* parsing the data ***************************

QDomElement SerializeCollector::findStoredGlobalElement(
        const QDomNode& rootNode,
        const QString& globalPointerName,
        const QString& globalGroupName,
        const QString& globalListName )
{
    QDomElement container;
    if( ! globalPointerName.isEmpty() ){
        QDomNode node = rootNode.firstChild();
        while( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                QString tagName = element.tagName();
                if( tagName.compare(globalListName, Qt::CaseInsensitive) == 0 ){
                    QDomNode node2 = element.firstChild();
                    while( !node2.isNull() ) {
                        QDomElement ele2 = node2.toElement();
                        if( !ele2.isNull() ) { // was really an element
                            QString tagName2 = ele2.tagName();
                            if( tagName2.compare(globalGroupName, Qt::CaseInsensitive) == 0 ){
                                QDomNode node3 = ele2.firstChild();
                                while( !node3.isNull() ) {
                                    QDomElement ele3 = node3.toElement();
                                    if( !ele3.isNull() ) { // was really an element
                                        QString tagName3 = ele3.tagName();
                                        if( tagName3.compare(globalPointerName, Qt::CaseInsensitive) == 0 ){
                                            container = ele3;
                                        }
                                    }
                                    node3 = node3.nextSibling();
                                }
                            }
                        }
                        node2 = node2.nextSibling();
                    }
                }
            }
            node = node.nextSibling();
        }
    }
    return container;
}

bool SerializeCollector::initializeGlobalPointers(
        const QDomNode& rootNode,
        const QString& name )
{
    instance()->initializedPointersMap().clear();

    bool globalObjectsNodeFound;
    QDomElement globalObjectsNode;
    QDomNode n = rootNode.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            // the node really is an element
            //qDebug() << e.tagName();
            if( e.tagName().compare(name, Qt::CaseInsensitive) == 0 ){
                globalObjectsNode = e;
                globalObjectsNodeFound = true;
            }
        }
        n = n.nextSibling();
    }
    if( ! globalObjectsNodeFound ){
        qDebug() << "CRITICAL information by SerializeCollector::initializeGlobalPointers()\n"
                "    No global-objects node found.";
        return false;
    }

    n = globalObjectsNode.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            // the node really is an element
            QString tagName = e.tagName();
            if( tagName == "kdchart:attribute-models" ||
                tagName == "kdchart:axes" ||
                tagName == "kdchart:charts" ||
                tagName == "kdchart:coordinate-planes" ||
                tagName == "kdchart:diagrams" ||
                tagName == "kdchart:headers-footers" ||
                tagName == "kdchart:legends" ){
                QDomNode n2 = e.firstChild();
                while(!n2.isNull()) {
                    QDomElement e2 = n2.toElement(); // try to convert the node to an element.
                    if(!e2.isNull()) {
                        // the node really is an element
                        QString objectName = e2.tagName();
                        QString className;
                        if( KDXML::findStringAttribute( e2, "Classname", className ) ){
                            //qDebug() << "object:" << objectName << "class:" << className;
                            AbstractSerializerFactory* f = Serializer::elementSerializerFactory( className );
                            if( f != 0 )
                            {
                                instance()->initializedPointersMap().insert(
                                        objectName,
                                        InitializedPointersMapItem(
                                            f->createNewObject( className ) ) );
                            }
                            else if( className == Chart::staticMetaObject.className() )
                            {
                                instance()->initializedPointersMap().insert(
                                        objectName,
                                        InitializedPointersMapItem(
                                                new Chart ) );
                            }
                            else if( className == "KDChart::AttributesModel" )
                            {
                                bool bExternalFlag;
                                if( KDXML::findBoolAttribute( e2, "external", bExternalFlag ) )
                                {
                                    if( bExternalFlag )
                                    {
                                        instance()->initializedPointersMap().insert(
                                                objectName,
                                                InitializedPointersMapItem(
                                                        new AttributesModel(0, 0) ) );
                                        // Only external attribute-models are instantiated explicitely,
                                        // the other objects are found in their respective diagrams.
                                    }
                                }
                                else
                                {
                                    qDebug()<< "Could not parse AttributesModel. Node"<<objectName<<"is invalid.";
                                }
                            }
                            else
                            {
                                qDebug() << "Non-critical information by SerializeCollector::initializeGlobalPointers()\n"
                                        "    Unknown subelement of " << tagName
                                        << " found: " << objectName << "\n"
                                        "    Make sure to instantiate this object\n"
                                        "    and store its pointer in the map of parsed pointers:\n"
                                        "    KDChart::SerializeCollector::instance()->initializedPointersMap()";
                                // It might well be that someone has stored additional
                                // top-level information here, so we just ignore them.
                            }
                        }
                        else
                        {
                            qDebug() << "CRITICAL information by SerializeCollector::initializeGlobalPointers()\n"
                                    "    Subelement of " << tagName
                                    << " has no \"Classname\" attribute: " << objectName << "\n"
                                    "    Can not parse that.";
                        }
                    }
                    n2 = n2.nextSibling();
                }
            } else {
                qDebug() << "Non-critical information by SerializeCollector::initializeGlobalPointers()\n"
                "    Unknown subelement of " << globalObjectsNode.tagName()
                        << " found: " << tagName << "\n"
                "    Make sure to instantiate its top-level objects\n"
                "    and store these object's pointers in the map of parsed pointers:\n"
                "    KDChart::SerializeCollector::instance()->initializedPointersMap()";
                // It might well be that someone has stored additional
                // top-level information here, so we just ignore them.
            }
        }
        n = n.nextSibling();
    }
    return true;
}

InitializedPointersMap& SerializeCollector::initializedPointersMap()
{
    return d->m_initializedPointersMap;
}

bool SerializeCollector::foundInitializedPointer(
        const QString& globalName,
        QObject*& p,
        bool& wasParsed )
{
    InitializedPointersMap& mapRef = instance()->initializedPointersMap();
    const bool bFound = mapRef.contains( globalName );
    if( bFound ){
        InitializedPointersMapItem item( mapRef.value( globalName ) );
        p = item.pointer;
        wasParsed = item.wasParsed;
    }else{
        p = 0;
        wasParsed = false;
    }
    return bFound;
}

void SerializeCollector::setWasParsed(
        QObject* p, bool parsed )
{
    InitializedPointersMap& mapRef = instance()->initializedPointersMap();
    InitializedPointersMap::iterator i = mapRef.begin();
    while (i != mapRef.end()) {
        if( i.value().pointer == p )
            i.value().wasParsed = parsed;
        ++i;
    }
}



// ************** storing the data *******************************

QDomElement* SerializeCollector::findOrMakeElement(
        QDomDocument& doc,
        const QString& name )
{
    QDomElement* e = findElement( name );
    if( ! e ){
        e = new QDomElement( doc.createElement( name ) );
        d->m_map[ name ] = e;
    }
    return e;
}

QDomElement* SerializeCollector::findElement( const QString& name )const
{
    if( d->m_map.contains( name ) )
        return d->m_map.value( name );
    return 0;
}

void SerializeCollector::appendDataToElement(
        QDomDocument& doc,
        QDomElement& element,
        const QString& name )const
{
    QDomElement list = doc.createElement( name );
    element.appendChild( list );
    Q_FOREACH (QDomElement* e, d->m_map)
    {
        //qDebug() << e->tagName();
        if( e->tagName() != Private::unresolvedMapName() ){
            list.appendChild( *e );
        }
    }
}

QDomElement SerializeCollector::createPointersList(
        QDomDocument& doc, QDomElement& e, const QString& title )
{
    QDomElement list = doc.createElement( title + ":pointers" );
    e.appendChild( list );
    return list;
}

QDomElement SerializeCollector::findOrMakeChild(
        QDomDocument& doc,
        QDomElement& elementsList,
        QDomElement& pointerContainer,
        const QString& title,
        const QString& classname,
        const void* p,
        bool& wasFound )
{
    const QString pointerName( IdMapper::instance()->findOrMakeName( p, title, wasFound ) );

    //KDXML::createStringNode( doc, pointerContainer, "kdchart:pointer", pointerName );
    KDXML::createNodeWithAttribute( doc, pointerContainer,
                                    "kdchart:pointer", "name", pointerName );

    if( ! wasFound ){
        QDomElement storeElement = doc.createElement( pointerName );
        elementsList.appendChild( storeElement );
        storeElement.setAttribute( "Classname", classname );
        return storeElement;
    }
    return QDomElement();
}


void SerializeCollector::storeUnresolvedPointer(
        QDomDocument& doc,
        const QObject* p,
        QDomElement& pointerContainer )
{
    if( ! p ){
        KDXML::createNodeWithAttribute( doc, pointerContainer,
                                        "kdchart:pointer", "name", "Null" );
    }else{
        // access (or append, resp.) the global list
        bool wasFound;
        const QString pointerName(
                IdMapper::instance()->findOrMakeName(
                        p, "kdchart:q-object-pointer", wasFound, false ) );

        //qDebug() << "SerializeCollector::storeUnresolvedPointer() storing" << pointerName << " wasFound:" << wasFound;
        KDXML::createNodeWithAttribute( doc, pointerContainer,
                                        "kdchart:unresolved-pointer", "name", pointerName );
    }
}


void SerializeCollector::resolvePointers(
        QDomDocument& doc,
        QDomElement& rootNode )
{
    //qDebug() << "SerializeCollector::resolvePointers()";

    // make a list of all unresolved pointer names in the rootNode
    QDomNodeList unresolvedList
            = rootNode.elementsByTagName( "kdchart:unresolved-pointer" );

    if( ! unresolvedList.isEmpty() ){
        // access (or append, resp.) the global list
        const QMap<const void*, QString> unresolvedMap
                = IdMapper::instance()->unresolvedMap();
        QMapIterator<const void*, QString> unresolvedIter( unresolvedMap );
        while( unresolvedIter.hasNext() ) {
            unresolvedIter.next();
            const void* p = unresolvedIter.key();
            const QString foundName  = IdMapper::instance()->findName( p );
            const bool wasFound = ! foundName.isEmpty();
            const QString globalName(
                    p ? (wasFound ? foundName : Private::unresolvedTagName()): QString("Null") );

            //qDebug() << "pointer:" << p << " name:" << unresolvedIter.value()
            //        << " wasFound: " << wasFound << " globalName:" << globalName;

            //qDebug() << "unresolvedList.count() initially is" << unresolvedList.count();
            for( int i=0; i<unresolvedList.count(); ++i ){
                QDomElement e = unresolvedList.item(i).toElement();
                if( e.attribute( "name" )
                    == unresolvedIter.value() )
                {
                    //qDebug() << "resolving" << e.attribute( "name" );
                    e.removeAttribute( "name" );
                    QDomText elementContent =
                            doc.createTextNode( globalName );
                    e.appendChild( elementContent );
                    e.setTagName( "kdchart:pointer" );
                    //qDebug() << "unresolvedList.count() is" << unresolvedList.count();

                    // We go back one item, since the e.setTagName() command
                    // has automatically removed the element from the unresolvedList
                    // by some of Qt's built-in magic.
                    --i;
                }
            }
        }
    }
}

void SerializeCollector::storePointerName(
        QDomDocument& doc,
        QDomElement& pointerContainer,
        const QString& pointerName )
{
    KDXML::createNodeWithAttribute( doc, pointerContainer,
                                    "kdchart:pointer", "name", pointerName );
}
