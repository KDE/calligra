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

#include <KDChartAbstractAreaBaseSerializer.h>
#include <KDChartAbstractAreaBaseSerializer_p.h>

#include <KDChartAttributesSerializer.h>

#include <KDXMLTools.h>

#include <qglobal.h>
#include <QMessageBox>


#define d d_func()

using namespace KDChart;

AbstractAreaBaseSerializer::Private::Private( AbstractAreaBaseSerializer* qq )
    : q( qq )
{
}

AbstractAreaBaseSerializer::Private::~Private()
{
}

/**
  \class KDChart::AbstractAreaBaseSerializer KDChartAbstractAreaBaseSerializer.h

  \brief Auxiliary methods reading/saving KD Chart data and configuration in streams.
  */

AbstractAreaBaseSerializer::AbstractAreaBaseSerializer()
    : _d( new Private( this ) )
{
}

AbstractAreaBaseSerializer::~AbstractAreaBaseSerializer()
{
    delete _d; _d = 0;
}

void AbstractAreaBaseSerializer::init()
{
}

bool AbstractAreaBaseSerializer::parseAbstractAreaBase(
        const QDomElement& e,
        AbstractAreaBase& area )
{
    bool bOK = true;
    QDomNode n = e.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            // the node really is an element
            if( e.tagName() == "kdchart:frame-attributes" ){
                FrameAttributes a;
                if( AttributesSerializer::parseFrameAttributes( e, a ) )
                    area.setFrameAttributes( a );
            } else if( e.tagName() == "kdchart:background-attributes" ){
                BackgroundAttributes a;
                if( AttributesSerializer::parseBackgroundAttributes( e, a ) )
                    area.setBackgroundAttributes( a );
            }
        }
        n = n.nextSibling();
    }
    return bOK;
}

void AbstractAreaBaseSerializer::saveAbstractAreaBase(
        QDomDocument& doc,
        QDomElement& e,
        const AbstractAreaBase& area,
        const QString& title )
{
    QDomElement areaElement =
            doc.createElement( title );
    e.appendChild( areaElement );

    // save the frame attributes
    AttributesSerializer::saveFrameAttributes(
            doc, areaElement,
            area.frameAttributes(),
            "kdchart:frame-attributes" );

    // save the background attributes
    AttributesSerializer::saveBackgroundAttributes(
            doc, areaElement,
            area.backgroundAttributes(),
            "kdchart:background-attributes" );
}
