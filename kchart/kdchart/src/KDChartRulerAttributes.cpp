/****************************************************************************
 ** Copyright (C) 2007 Klarälvdalens Datakonsult AB.  All rights reserved.
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

#include "KDChartRulerAttributes.h"

#include <QPen>
#include <QDebug>

#include <KDABLibFakes>

#define d d_func()

using namespace KDChart;

class RulerAttributes::Private
{
    friend class RulerAttributes;
public:
    Private();
private:
    QPen tickMarkPen;
};

RulerAttributes::Private::Private()
    : tickMarkPen( QColor( 0x00, 0x00, 0x00 ) )
{
	tickMarkPen.setCapStyle( Qt::FlatCap );
}


RulerAttributes::RulerAttributes()
    : _d( new Private() )
{
    // this bloc left empty intentionally
}

RulerAttributes::RulerAttributes( const RulerAttributes& r )
    : _d( new Private( *r.d ) )
{
}

void RulerAttributes::setTickMarkPen( const QPen& pen )
{
	d->tickMarkPen = pen;
}

QPen RulerAttributes::tickMarkPen() const
{
	return d->tickMarkPen;
}

void RulerAttributes::setTickMarkColor( const QColor& color )
{
	d->tickMarkPen.setColor( color );
}

QColor RulerAttributes::tickMarkColor() const
{
	return d->tickMarkPen.color();
}

RulerAttributes & RulerAttributes::operator=( const RulerAttributes& r )
{
    if( this == &r )
        return *this;

    *d = *r.d;

    return *this;
}

RulerAttributes::~RulerAttributes()
{
    delete _d; _d = 0;
}


bool RulerAttributes::operator == ( const RulerAttributes& r ) const
{
    return  tickMarkPen() == r.tickMarkPen();
}

#if !defined( QT_NO_DEBUG_STREAM )
QDebug operator << ( QDebug dbg, const KDChart::RulerAttributes& a )
{
    dbg << "KDChart::RulerAttributes("
            << "tickMarkPen=" << a.tickMarkPen()
            << ")";
    return dbg;
}
#endif /* QT_NO_DEBUG_STREAM */

