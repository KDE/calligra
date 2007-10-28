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

#include <KDChartDiagramSerializerFactory.h>
#include <KDChartDiagramSerializerFactory_p.h>

#include <KDChartDiagramsSerializer.h>

#include <KDChartCoordPlanesSerializer.h>

#include <KDChartLineDiagram>
#include <KDChartPieDiagram>
#include <KDChartPolarDiagram>
#include <KDChartRingDiagram>
#include <KDChartBarDiagram>
#include <KDChartPlotter>

#define d d_func()

using namespace KDChart;

DiagramSerializerFactory::Private::Private( DiagramSerializerFactory* qq )
    : q( qq ),
      m_instance( 0 )
{
}

DiagramSerializerFactory::Private::~Private()
{
    if( m_instance != 0 )
        delete m_instance;
}

DiagramSerializerFactory::DiagramSerializerFactory( QObject* parent )
    : AbstractSerializerFactory( parent ),
      _d( new Private( this ) )
{
}

DiagramSerializerFactory::~DiagramSerializerFactory()
{
    delete _d; _d = 0;
}

void DiagramSerializerFactory::init()
{
}

AbstractSerializer* DiagramSerializerFactory::instance( const QString& className ) const
{
    Q_UNUSED( className );

    if( d->m_instance == 0 )
        d->m_instance = new DiagramsSerializer;

    return d->m_instance;
}

QObject* DiagramSerializerFactory::createNewObject( const QString& className ) const
{
    if( className == LineDiagram::staticMetaObject.className() )
        return new LineDiagram;
    if( className == PieDiagram::staticMetaObject.className() )
        return new PieDiagram;
    if( className == PolarDiagram::staticMetaObject.className() )
        return new PolarDiagram;
    if( className == RingDiagram::staticMetaObject.className() )
        return new RingDiagram;
    if( className == BarDiagram::staticMetaObject.className() )
        return new BarDiagram;
    if( className == Plotter::staticMetaObject.className() )
        return new Plotter;
    return 0;
}
