/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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


#include <QVariant>
#include <QDebug>
#include "KDChartDataValueAttributes.h"
#include "KDChartRelativePosition.h"
#include <KDChartTextAttributes.h>
#include <KDChartFrameAttributes.h>
#include <KDChartBackgroundAttributes.h>
#include <KDChartMarkerAttributes.h>

#include <KDABLibFakes>

// FIXME till
#define KDCHART_DATA_VALUE_AUTO_DIGITS 4


#define d d_func()

using namespace KDChart;

class DataValueAttributes::Private
{
    friend class DataValueAttributes;
public:
    Private();
private:
    bool visible;
    TextAttributes textAttributes;
    FrameAttributes frameAttributes;
    BackgroundAttributes backgroundAttributes;
    MarkerAttributes markerAttributes;
    int decimalDigits;
    QString prefix;
    QString suffix;
    QString dataLabel;
    int powerOfTenDivisor;
    bool showInfinite;
    RelativePosition    negativeRelPos;
    RelativePosition    positiveRelPos;
};

DataValueAttributes::Private::Private() :
    visible( false ),
    decimalDigits( KDCHART_DATA_VALUE_AUTO_DIGITS ),
    powerOfTenDivisor( 0 ),
    showInfinite( true ),
    negativeRelPos(),  // use the default: centered alignment, no padding, no rotation
    positiveRelPos()
{
  textAttributes.setRotation( -45 );
}


DataValueAttributes::DataValueAttributes()
    : _d( new Private() )
{
}

DataValueAttributes::DataValueAttributes( const DataValueAttributes& r )
    : _d( new Private( *r.d ) )
{
}

DataValueAttributes & DataValueAttributes::operator=( const DataValueAttributes& r )
{
    if( this == &r )
        return *this;

    *d = *r.d;

    return *this;
}

DataValueAttributes::~DataValueAttributes()
{
    delete _d; _d = 0;
}


bool DataValueAttributes::operator==( const DataValueAttributes& r ) const
{
    return ( isVisible() == r.isVisible() &&
	     textAttributes() == r.textAttributes() &&
	     frameAttributes() == r.frameAttributes() &&
	     backgroundAttributes() == r.backgroundAttributes() &&
	     markerAttributes() == r.markerAttributes() &&
	     decimalDigits() == r.decimalDigits() &&
             prefix() == r.prefix() &&
             suffix() == r.suffix() &&
             dataLabel() == r.dataLabel() &&
	     powerOfTenDivisor() == r.powerOfTenDivisor() &&
	     showInfinite() == r.showInfinite() &&
	     negativePosition() == r.negativePosition() &&
	     positivePosition() == r.positivePosition() );
}

/*static*/
const DataValueAttributes& DataValueAttributes::defaultAttributes()
{
    static const DataValueAttributes theDefaultDataValueAttributes;
    return theDefaultDataValueAttributes;
}

/*static*/
const QVariant& DataValueAttributes::defaultAttributesAsVariant()
{
    static const QVariant theDefaultDataValueAttributesVariant = qVariantFromValue(defaultAttributes());
    return theDefaultDataValueAttributesVariant;
}


void DataValueAttributes::setVisible( bool visible )
{
    d->visible = visible;
}

bool DataValueAttributes::isVisible() const
{
    return d->visible;
}

void DataValueAttributes::setTextAttributes( const TextAttributes &a )
{
    d->textAttributes = a;
}

TextAttributes DataValueAttributes::textAttributes() const
{
    return d->textAttributes;
}

void DataValueAttributes::setFrameAttributes( const FrameAttributes &a )
{
    d->frameAttributes = a;
}

FrameAttributes DataValueAttributes::frameAttributes() const
{
    return d->frameAttributes;
}

void DataValueAttributes::setBackgroundAttributes( const BackgroundAttributes &a )
{
    d->backgroundAttributes = a;
}

BackgroundAttributes DataValueAttributes::backgroundAttributes() const
{
    return d->backgroundAttributes;
}

void DataValueAttributes::setMarkerAttributes( const MarkerAttributes &a )
{
    d->markerAttributes = a;
}

MarkerAttributes DataValueAttributes::markerAttributes() const
{
    return d->markerAttributes;
}


void DataValueAttributes::setDecimalDigits( int digits )
{
    d->decimalDigits = digits;
}

int DataValueAttributes::decimalDigits() const
{
    return d->decimalDigits;
}

void DataValueAttributes::setPrefix( const QString prefixString )
{
    d->prefix = prefixString;
}

QString DataValueAttributes::prefix() const
{
    return d->prefix;
}

void DataValueAttributes::setSuffix( const QString suffixString )
{
    d->suffix  = suffixString;
}

QString DataValueAttributes::suffix() const
{
    return d->suffix;
}

void DataValueAttributes::setDataLabel( const QString label )
{
    d->dataLabel =  label;
}

QString DataValueAttributes::dataLabel() const
{
    return d->dataLabel;
}

void DataValueAttributes::setPowerOfTenDivisor( int powerOfTenDivisor )
{
    d->powerOfTenDivisor = powerOfTenDivisor;
}

int DataValueAttributes::powerOfTenDivisor() const
{
    return d->powerOfTenDivisor;
}

void DataValueAttributes::setShowInfinite( bool infinite )
{
    d->showInfinite = infinite;
}

bool DataValueAttributes::showInfinite() const
{
    return d->showInfinite;
}

void DataValueAttributes::setNegativePosition( const RelativePosition& relPosition )
{
    d->negativeRelPos = relPosition;
}

RelativePosition DataValueAttributes::negativePosition() const
{
    return d->negativeRelPos;
}

void DataValueAttributes::setPositivePosition( const RelativePosition& relPosition )
{
    d->positiveRelPos = relPosition;
}

RelativePosition DataValueAttributes::positivePosition() const
{
    return d->positiveRelPos;
}

#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator<<(QDebug dbg, const KDChart::DataValueAttributes& val )
{
    dbg << "RelativePosition DataValueAttributes("
	<< "visible="<<val.isVisible()
	<< "textattributes="<<val.textAttributes()
	<< "frameattributes="<<val.frameAttributes()
	<< "backgroundattributes="<<val.backgroundAttributes()
	<< "decimaldigits="<<val.decimalDigits()
	<< "poweroftendivisor="<<val.powerOfTenDivisor()
	<< "showinfinite="<<val.showInfinite()
	<< "negativerelativeposition="<<val.negativePosition()
	<< "positiverelativeposition="<<val.positivePosition()
	<<")";
    return dbg;
}
#endif /* QT_NO_DEBUG_STREAM */
