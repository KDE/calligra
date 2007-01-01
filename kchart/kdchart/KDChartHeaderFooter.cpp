/****************************************************************************
 ** Copyright (C) 2006 Klar�vdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include "KDChartChart.h"
#include "KDChartHeaderFooter.h"
#include "KDChartHeaderFooter_p.h"
#include <KDChartTextAttributes.h>
#include <QFont>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QtDebug>
#include <QLabel>
#include "KDTextDocument.h"

#include <KDABLibFakes>

using namespace KDChart;

HeaderFooter::Private::Private() :
    type( Header ),
    position( Position::North )
{
}

HeaderFooter::Private::~Private()
{
}

#define d d_func()

HeaderFooter::HeaderFooter( Chart* parent ) :
    TextArea( new Private() )
{
    setParent( parent );
    init();
}

HeaderFooter::~HeaderFooter()
{
    emit destroyedHeaderFooter( this );
}

void HeaderFooter::setParent( QObject* parent )
{
    QObject::setParent( parent );
    if( parent && ! autoReferenceArea() )
        setAutoReferenceArea( parent );
}

void HeaderFooter::init()
{
    TextAttributes ta;
    ta.setPen( QPen(Qt::black) );
    ta.setFont( QFont( QLatin1String( "helvetica" ), 10, QFont::Bold, false ) );

    Measure m( 35.0 );
    m.setRelativeMode( autoReferenceArea(), KDChartEnums::MeasureOrientationMinimum );
    ta.setFontSize( m );

    m.setValue( 8.0 );
    m.setCalculationMode( KDChartEnums::MeasureCalculationModeAbsolute );
    ta.setMinimalFontSize( m  );

    setTextAttributes( ta );
}

HeaderFooter * HeaderFooter::clone() const
{
    HeaderFooter* headerFooter = new HeaderFooter( new Private( *d ), 0 );
    headerFooter->setType( type() );
    headerFooter->setPosition( position() );
    headerFooter->setTextAttributes( textAttributes() );
    return headerFooter;
}

void HeaderFooter::setType( HeaderFooterType type )
{
    d->type = type;
    emit positionChanged( this );
}

HeaderFooter::HeaderFooterType HeaderFooter::type() const
{
    return d->type;
}

void HeaderFooter::setPosition( Position position )
{
    d->position = position;
    emit positionChanged( this );
}

Position HeaderFooter::position() const
{
    return d->position;
}
