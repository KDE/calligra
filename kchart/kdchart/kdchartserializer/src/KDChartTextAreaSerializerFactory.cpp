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

#include <KDChartTextAreaSerializerFactory.h>
#include <KDChartTextAreaSerializerFactory_p.h>

#include <KDChartTextAreaSerializer.h>

#include <KDChartHeaderFooter>

#define d d_func()

using namespace KDChart;

TextAreaSerializerFactory::Private::Private( TextAreaSerializerFactory* qq )
    : q( qq ),
      m_instance( 0 )
{
}

TextAreaSerializerFactory::Private::~Private()
{
    if( m_instance != 0 )
        delete m_instance;
}

TextAreaSerializerFactory::TextAreaSerializerFactory( QObject* parent )
    : AbstractSerializerFactory( parent ),
      _d( new Private( this ) )
{
}

TextAreaSerializerFactory::~TextAreaSerializerFactory()
{
    delete _d; _d = 0;
}

void TextAreaSerializerFactory::init()
{
}

AbstractSerializer* TextAreaSerializerFactory::instance( const QString& className ) const
{
    Q_UNUSED( className );

    if( d->m_instance == 0 )
        d->m_instance = new TextAreaSerializer;

    return d->m_instance;
}

QObject* TextAreaSerializerFactory::createNewObject( const QString& className ) const
{
    if( className == HeaderFooter::staticMetaObject.className() )
        return new HeaderFooter;
    return 0;
}
