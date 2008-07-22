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

#include <KDChartAttributesModelSerializer.h>
#include <KDChartAttributesModelSerializer_p.h>

#include <KDChartSerializeCollector.h>
#include <KDChartAttributesSerializer.h>

#include "KDXMLTools.h"

#include <qglobal.h>
#include <QMessageBox>

#define d d_func()

using namespace KDChart;

AttributesModelSerializer::Private::Private( AttributesModelSerializer* qq )
    : q( qq )
{
}

AttributesModelSerializer::Private::~Private() {}


/**
  \class KDChart::AttributesModelSerializer KDChartAttributesModelSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */


AttributesModelSerializer::AttributesModelSerializer()
    : _d( new Private( this ) )
{
    // this space left empty intentionally
}

AttributesModelSerializer::~AttributesModelSerializer()
{
    delete _d; _d = 0;
}

void AttributesModelSerializer::init()
{
}

bool AttributesModelSerializer::parseAttributesModel(
        const QDomNode& rootNode,
        const QString& globalName,
        AttributesModel& model )const
{
    QDomElement container = SerializeCollector::findStoredGlobalElement(
            rootNode, globalName, "kdchart:attribute-models" );

    bool bOK = ! container.tagName().isEmpty();

    if( bOK ) {
        const QString modelName = container.tagName();
        //qDebug() << "\n    AttributesModelSerializer::parseAttributesModel() processing" << modelName;
        QDomNode node = container.firstChild();
        while( !node.isNull() )
        {
            QDomElement element = node.toElement();
            if( !element.isNull() )
            {   // was really an element
                QString tagName = element.tagName();
                //qDebug() << tagName;
                if( tagName == "DataMap" )
                {
                    QMap<int, QMap<int, QMap<int, QVariant> > > dataMap;
                    // parse the inner maps of DataMap
                    QDomNode node2 = element.firstChild();
                    while( !node2.isNull() )
                    {
                        QDomElement element2 = node2.toElement();
                        if( !element2.isNull() )
                        {   // was really an element
                            QString tagName2 = element2.tagName();
                            //qDebug() << tagName2;
                            int key1;
                            if( (tagName2 == "map") &&
                                KDXML::findIntAttribute( element2, "key", key1 ) )
                            {
                                //qDebug() << "key1:" << key1;
                                QMap<int, QMap<int, QVariant> > map2 = dataMap[ key1 ];
                                // parse the inner-most maps of DataMap/map
                                QDomNode node3 = element2.firstChild();
                                while( !node3.isNull() )
                                {
                                    QDomElement element3 = node3.toElement();
                                    if( !element3.isNull() )
                                    {   // was really an element
                                        QString tagName3 = element3.tagName();
                                        //qDebug() << tagName3;
                                        int key2;
                                        if( tagName3 == "map"  &&
                                            KDXML::findIntAttribute( element3, "key", key2 ) )
                                        {
                                            //qDebug() << "key2:" << key2;
                                            QMap<int, QVariant> map3 = map2[ key2 ];
                                            // parse the attributes stored in DataMap/map/map
                                            QDomNode node4 = element3.firstChild();
                                            while( !node4.isNull() )
                                            {
                                                QDomElement element4 = node4.toElement();
                                                if( !element4.isNull() )
                                                {   // was really an element
                                                    QString tagName4 = element4.tagName();
                                                    //qDebug() << tagName4;
                                                    if( tagName4 == "value" )
                                                    {
                                                        QVariant attrs;
                                                        int role;
                                                        if( parseAttributesNode(
                                                                element4, attrs, role ) ){
                                                            // store the successfully parsed element
                                                            map3[ role ] = attrs;
                                                            //qDebug() << "      inserted:" << role;
                                                        }else{
                                                            QString roleName;
                                                            if( ! KDXML::findStringAttribute( element4, "key", roleName ) )
                                                                roleName = "0x"+QString::number(role, 16);
                                                            qDebug()<< "Could not parse attribute role"
                                                                    << roleName << "into DataMap("
                                                                    << key1 << "," << key2
                                                                    << ")";
                                                        }
                                                    }
                                                }
                                                node4 = node4.nextSibling();
                                            }
                                            map2[ key2 ] = map3;
                                            //qDebug() << "    inserted:" << key2;
                                        }
                                    }
                                    node3 = node3.nextSibling();
                                }
                                dataMap[ key1 ] = map2;
                                //qDebug() << "  inserted:" << key1;
                            }
                        }
                        node2 = node2.nextSibling();
                    }
                    model.setDataMap( dataMap );

                }else if( tagName == "HorizontalHeaderDataMap" ){
                    QMap<int, QMap<int, QVariant> > horizHeaderDataMap;
                    // parse the inner maps of HorizontalHeaderDataMap
                    QDomNode node2 = element.firstChild();
                    while( !node2.isNull() )
                    {
                        QDomElement element2 = node2.toElement();
                        if( !element2.isNull() )
                        {   // was really an element
                            QString tagName2 = element2.tagName();
                            //qDebug() << tagName2;
                            int key1;
                            if( (tagName2 == "map") &&
                                KDXML::findIntAttribute( element2, "key", key1 ) )
                            {
                                QMap<int, QVariant> map2 = horizHeaderDataMap[ key1 ];
                                // parse the inner-most map of HorizontalHeaderDataMap/map
                                QDomNode node3 = element2.firstChild();
                                while( !node3.isNull() )
                                {
                                    QDomElement element3 = node3.toElement();
                                    if( !element3.isNull() )
                                    {   // was really an element
                                        QString tagName3 = element3.tagName();
                                        //qDebug() << tagName3;
                                        if( tagName3 == "value" )
                                        {
                                            QVariant attrs;
                                            int role;
                                            if( parseAttributesNode(
                                                element3, attrs, role ) ){
                                                // store the successfully parsed element
                                                map2[ role ] = attrs;
                                            }
                                        }
                                    }
                                    node3 = node3.nextSibling();
                                }
                                horizHeaderDataMap[ key1 ] = map2;
                            }
                        }
                        node2 = node2.nextSibling();
                    }
                    model.setHorizontalHeaderDataMap( horizHeaderDataMap );

                }else if( tagName == "VerticalHeaderDataMap" ){
                    QMap<int, QMap<int, QVariant> > vertHeaderDataMap;
                    // parse the inner maps of VerticalHeaderDataMap
                    QDomNode node2 = element.firstChild();
                    while( !node2.isNull() )
                    {
                        QDomElement element2 = node2.toElement();
                        if( !element2.isNull() )
                        {   // was really an element
                            QString tagName2 = element2.tagName();
                            //qDebug() << tagName2;
                            int key1;
                            if( (tagName2 == "map") &&
                                KDXML::findIntAttribute( element2, "key", key1 ) )
                            {
                                QMap<int, QVariant> map2 = vertHeaderDataMap[ key1 ];
                                // parse the values of VerticalHeaderDataMap/map
                                QDomNode node3 = element2.firstChild();
                                while( !node3.isNull() )
                                {
                                    QDomElement element3 = node3.toElement();
                                    if( !element3.isNull() )
                                    {   // was really an element
                                        QString tagName3 = element3.tagName();
                                        //qDebug() << tagName3;
                                        if( tagName3 == "value" )
                                        {
                                            QVariant attrs;
                                            int role;
                                            if( parseAttributesNode(
                                                element3, attrs, role ) ){
                                                // store the successfully parsed element
                                                map2[ role ] = attrs;
                                            }
                                        }
                                    }
                                    node3 = node3.nextSibling();
                                }
                                vertHeaderDataMap[ key1 ] = map2;
                            }
                        }
                        node2 = node2.nextSibling();
                    }
                    model.setVerticalHeaderDataMap( vertHeaderDataMap );

                }else if( tagName == "ModelDataMap" ){
                    QMap<int, QVariant> modelDataMap;
                    // parse the values of ModelDataMap
                    QDomNode node2 = element.firstChild();
                    while( !node2.isNull() )
                    {
                        QDomElement element2 = node2.toElement();
                        if( !element2.isNull() )
                        {   // was really an element
                            QString tagName2 = element2.tagName();
                            //qDebug() << tagName2;
                            QString roleName;
                            if( (tagName2 == "value") &&
                                KDXML::findStringAttribute( element2, "key", roleName ) )
                            {
                                QVariant attrs;
                                int role;
                                if( parseAttributesNode(
                                    element2, attrs, role ) )
                                {
                                    // store the successfully parsed element
                                    modelDataMap[ role ] = attrs;
                                }else{
                                    qDebug()<< "Could not parse attribute role"
                                            << roleName << "into ModelDataMap";
                                }
                            }
                        }
                        node2 = node2.nextSibling();
                    }
                    model.setModelDataMap( modelDataMap );

                }else if( tagName == "Palette" ){
                    QString s;
                    if( KDXML::readStringNode( element, s ) ){
                        if( s == "PaletteTypeDefault" )
                            model.setPaletteType( AttributesModel::PaletteTypeDefault );
                        else if( s == "PaletteTypeRainbow" )
                            model.setPaletteType( AttributesModel::PaletteTypeRainbow );
                        else if( s == "PaletteTypeSubdued" )
                            model.setPaletteType( AttributesModel::PaletteTypeSubdued );
                        else{
                            qDebug() << "Unknown PaletteType found:" << s;
                            bOK = false;
                        }
                    }
                }else{
                    qDebug() << "Unknown Element found:" << tagName;
                    // We continue, do not break here, since it might be
                    // intentionally that additional information was stored.
                }
            }
            node = node.nextSibling();
        }
    }
    return bOK;
}

bool AttributesModelSerializer::parseAttributesNode(
        const QDomElement& e,
        QVariant& attributes,
        int& role )const
{
    bool bOK = false;
    QVariant value;
    int roleValue=0;
    QString roleName;
    if( KDXML::findStringAttribute( e, "key", roleName ) ){
        //qDebug() << roleName;
        QDomNode node = e.firstChild();
        if( !node.isNull() ) {
            QDomElement element = node.toElement();
            if( !element.isNull() ) { // was really an element
                if( roleName == "DataValueLabelAttributesRole" ){
                    roleValue =  DataValueLabelAttributesRole;
                    DataValueAttributes a;
                    bOK = AttributesSerializer::parseDataValueAttributes( element, a );
                    if( bOK )
                        value = qVariantFromValue( a );
                    else
                        qDebug() << "Error parsing DataValueLabelAttributesRole element";
                }else if( roleName == "DatasetBrushRole"){
                    roleValue       =  DatasetBrushRole;
                    QBrush b;
                    bOK = KDXML::readBrushNode( element, b );
                    if( bOK )
                        value = qVariantFromValue( b );
                    else
                        qDebug() << "Error parsing DatasetBrushRole element";
                }else if( roleName == "DatasetPenRole"){
                    roleValue       =  DatasetPenRole;
                    QPen pen;
                    bOK = KDXML::readPenNode( element, pen );
                    if( bOK )
                        value = qVariantFromValue( pen );
                    else
                        qDebug() << "Error parsing DatasetPenRole element";
                }else if( roleName == "LineAttributesRole" ){
                    roleValue       =  LineAttributesRole;
                    LineAttributes a;
                    bOK = AttributesSerializer::parseLineAttributes( element, a );
                    if( bOK )
                        value = qVariantFromValue( a );
                    else
                        qDebug() << "Error parsing LineAttributesRole element";
                }else if( roleName == "ThreeDLineAttributesRole" ){
                    roleValue       =  ThreeDLineAttributesRole;
                    ThreeDLineAttributes a;
                    bOK = AttributesSerializer::parseThreeDLineAttributes( element, a );
                    if( bOK )
                        value = qVariantFromValue( a );
                    else
                        qDebug() << "Error parsing ThreeDLineAttributesRole element";
                }else if( roleName == "BarAttributesRole" ){
                    roleValue       =  BarAttributesRole;
                    BarAttributes a;
                    bOK = AttributesSerializer::parseBarAttributes( element, a );
                    if( bOK )
                        value = qVariantFromValue( a );
                    else
                        qDebug() << "Error parsing BarAttributesRole element";
                }else if( roleName == "ThreeDBarAttributesRole" ){
                    roleValue       =  ThreeDBarAttributesRole;
                    ThreeDBarAttributes a;
                    bOK = AttributesSerializer::parseThreeDBarAttributes( element, a );
                    if( bOK )
                        value = qVariantFromValue( a );
                    else
                        qDebug() << "Error parsing ThreeDBarAttributesRole element";
                }else if( roleName == "PieAttributesRole" ){
                    roleValue       =  PieAttributesRole;
                    PieAttributes a;
                    bOK = AttributesSerializer::parsePieAttributes( element, a );
                    if( bOK )
                        value = qVariantFromValue( a );
                    else
                        qDebug() << "Error parsing PieAttributesRole element";
                }else if( roleName == "ThreeDPieAttributesRole" ){
                    roleValue       =  ThreeDPieAttributesRole;
                    ThreeDPieAttributes a;
                    bOK = AttributesSerializer::parseThreeDPieAttributes( element, a );
                    if( bOK )
                        value = qVariantFromValue( a );
                    else
                        qDebug() << "Error parsing ThreeDPieAttributesRole element";
                }else if( roleName == "DataHiddenRole"){
                    roleValue       =  DataHiddenRole;
                    bool b;
                    bOK = KDXML::readBoolNode( element, b );
                    if( bOK )
                        value = qVariantFromValue( b );
                    else
                        qDebug() << "Error parsing DataHiddenRole element";
                }else{
                    qDebug() << "Unknown"<< roleName <<"role found in element" << e.tagName();
                    Q_ASSERT( false ); // all of our own roles need to be handled
                }
            }
        }
    }
    if( bOK ){
        attributes = value;
        role = roleValue;
    }
    return bOK;
}




void AttributesModelSerializer::saveAttributesModel(
        QDomDocument& doc,
        QDomElement& e,
        const AttributesModel* model,
        bool isExternalModel )const
{
    if( ! model ) return;

    const QString title("kdchart:attribute-models");
    // access (or append, resp.) the global list
    QDomElement* modelsList =
            SerializeCollector::instance()->findOrMakeElement( doc, title );

    QDomElement attrModelPtrElement =
            doc.createElement( "AttributesModel" );
    e.appendChild( attrModelPtrElement );

    bool wasFound;
    QDomElement modelElement =
            SerializeCollector::findOrMakeChild(
            doc,
            *modelsList,
            attrModelPtrElement,
            "kdchart:attribute-model",
            "KDChart::AttributesModel",
            model,
            wasFound );
    if( ! wasFound ){
        KDXML::setBoolAttribute( modelElement, "external", isExternalModel );
        // save the dataMap
        {
            QDomElement dataMapElement =
                    doc.createElement( "DataMap" );
            modelElement.appendChild( dataMapElement );
            const QMap<int, QMap<int, QMap<int, QVariant> > > dataMap( model->dataMap() );
            QMap<int, QMap<int, QMap<int, QVariant> > >::const_iterator i0 = dataMap.constBegin();
            while (i0 != dataMap.constEnd()) {
                QDomElement innerMapElement1 =
                        doc.createElement( "map" );
                dataMapElement.appendChild( innerMapElement1 );
                innerMapElement1.setAttribute( "key", i0.key() );
                QMap<int, QMap<int, QVariant> >::const_iterator i1 = i0.value().constBegin();
                while (i1 != i0.value().constEnd()) {
                    QDomElement innerMapElement2 =
                            doc.createElement( "map" );
                    innerMapElement1.appendChild( innerMapElement2 );
                    innerMapElement2.setAttribute( "key", i1.key() );
                    QMap<int, QVariant>::const_iterator i2 = i1.value().constBegin();
                    while (i2 != i1.value().constEnd()) {
                        createAttributesNode(
                                doc, innerMapElement2, model, i2.key(), i2.value() );
                        ++i2;
                    }
                    ++i1;
                }
                ++i0;
            }
        }
        // save the horizontalHeaderDataMap
        {
            QDomElement horizMapElement =
                    doc.createElement( "HorizontalHeaderDataMap" );
            modelElement.appendChild( horizMapElement );
            const QMap<int, QMap<int, QVariant> > horizMap( model->horizontalHeaderDataMap() );
            QMap<int, QMap<int, QVariant> >::const_iterator i0 = horizMap.constBegin();
            while (i0 != horizMap.constEnd()) {
                QDomElement innerMapElement1 =
                        doc.createElement( "map" );
                horizMapElement.appendChild( innerMapElement1 );
                innerMapElement1.setAttribute( "key", i0.key() );
                QMap<int, QVariant>::const_iterator i1 = i0.value().constBegin();
                while (i1 != i0.value().constEnd()) {
                    createAttributesNode(
                            doc, innerMapElement1, model, i1.key(), i1.value() );
                    ++i1;
                }
                ++i0;
            }
        }
        // save the verticalHeaderDataMap
        {
            QDomElement vertMapElement =
                    doc.createElement( "VerticalHeaderDataMap" );
            modelElement.appendChild( vertMapElement );
            const QMap<int, QMap<int, QVariant> > vertMap( model->verticalHeaderDataMap() );
            QMap<int, QMap<int, QVariant> >::const_iterator i0 = vertMap.constBegin();
            while (i0 != vertMap.constEnd()) {
                QDomElement innerMapElement1 =
                        doc.createElement( "map" );
                vertMapElement.appendChild( innerMapElement1 );
                innerMapElement1.setAttribute( "key", i0.key() );
                QMap<int, QVariant>::const_iterator i1 = i0.value().constBegin();
                while (i1 != i0.value().constEnd()) {
                    createAttributesNode(
                            doc, innerMapElement1, model, i1.key(), i1.value() );
                    ++i1;
                }
                ++i0;
            }
        }
        // save the modelDataMap
        {
            QDomElement modelMapElement =
                    doc.createElement( "ModelDataMap" );
            modelElement.appendChild( modelMapElement );
            QMap<int, QVariant > modelMap( model->modelDataMap() );
            QMap<int, QVariant>::const_iterator i = modelMap.constBegin();
            while (i != modelMap.constEnd()) {
                createAttributesNode(
                        doc, modelMapElement, model, i.key(), i.value() );
                ++i;
            }
        }
        // save the palette type
        QString name;
        switch( model->paletteType() ){
            case AttributesModel::PaletteTypeDefault:
                name = "PaletteTypeDefault";
                break;
            case AttributesModel::PaletteTypeRainbow:
                name = "PaletteTypeRainbow";
                break;
            case AttributesModel::PaletteTypeSubdued:
                name = "PaletteTypeSubdued";
                break;
            default:
                Q_ASSERT( false ); // all of the types need to be handled
                break;
        }
        KDXML::createStringNode( doc, modelElement, "Palette", name );
    }
}

void AttributesModelSerializer::createAttributesNode(
        QDomDocument& doc,
        QDomElement& e,
        const AttributesModel* model,
        int role,
        const QVariant& attributes )const
{
    if( ! model ) return;

    QDomElement element =
            doc.createElement( "value" );
    e.appendChild( element );
    QString name;
    if( model->isKnownAttributesRole( role ) ){
        switch( role ) {
            case DataValueLabelAttributesRole:
                name = QString::fromLatin1("DataValueLabelAttributesRole");
                AttributesSerializer::saveDataValueAttributes(
                        doc,
                        element,
                        qVariantValue<DataValueAttributes>( attributes ),
                        "DataValueLabelAttributes" );
                break;
            case DatasetBrushRole:
                name = QString::fromLatin1("DatasetBrushRole");
                KDXML::createBrushNode( doc, element, "DatasetBrush",
                                        qVariantValue<QBrush>( attributes ) );
                break;
            case DatasetPenRole:
                name = QString::fromLatin1("DatasetPenRole");
                KDXML::createPenNode( doc, element, "DatasetPen",
                                        qVariantValue<QPen>( attributes ) );
                break;
            case ThreeDAttributesRole:
                // As of yet there is no ThreeDAttributes class,
                // and the AbstractThreeDAttributes class is pure virtual,
                // so we ignore this role for now.
                // (khz, 17.03.2007)
                /*
                name = QString::fromLatin1("ThreeDAttributesRole");
                AttributesSerializer::saveThreeDAttributes(
                        doc,
                        element,
                        qVariantValue<ThreeDAttributes>( attributes ),
                        "ThreeDAttributes" );
                */
                break;
            case LineAttributesRole:
                name = QString::fromLatin1("LineAttributesRole");
                AttributesSerializer::saveLineAttributes(
                        doc,
                        element,
                        qVariantValue<LineAttributes>( attributes ),
                        "LineAttributes" );
                break;
            case ThreeDLineAttributesRole:
                name = QString::fromLatin1("ThreeDLineAttributesRole");
                AttributesSerializer::saveThreeDLineAttributes(
                        doc,
                        element,
                        qVariantValue<ThreeDLineAttributes>( attributes ),
                        "ThreeDLineAttributes" );
                break;
            case BarAttributesRole:
                name = QString::fromLatin1("BarAttributesRole");
                AttributesSerializer::saveBarAttributes(
                        doc,
                        element,
                        qVariantValue<BarAttributes>( attributes ),
                        "BarAttributes" );
                break;
            case ThreeDBarAttributesRole:
                name = QString::fromLatin1("ThreeDBarAttributesRole");
                AttributesSerializer::saveThreeDBarAttributes(
                        doc,
                        element,
                        qVariantValue<ThreeDBarAttributes>( attributes ),
                        "ThreeDBarAttributes" );
                break;
            case PieAttributesRole:
                name = QString::fromLatin1("PieAttributesRole");
                AttributesSerializer::savePieAttributes(
                        doc,
                        element,
                        qVariantValue<PieAttributes>( attributes ),
                        "PieAttributes" );
                break;
            case ThreeDPieAttributesRole:
                name = QString::fromLatin1("ThreeDPieAttributesRole");
                AttributesSerializer::saveThreeDPieAttributes(
                        doc,
                        element,
                        qVariantValue<ThreeDPieAttributes>( attributes ),
                        "ThreeDPieAttributes" );
                break;
            case DataHiddenRole:
                name = QString::fromLatin1("DataHiddenRole");
                KDXML::createBoolNode( doc, element, "DataHidden",
                                       qVariantValue<bool>( attributes ) );
                break;
            default:
                Q_ASSERT( false ); // all of our own roles need to be handled
                break;
        }
    }else{
        name = QString::fromLatin1("ROLE:%1").arg( role );
    }
    element.setAttribute( "key", name );
}
