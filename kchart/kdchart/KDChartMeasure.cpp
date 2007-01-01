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

#include <QWidget>

#include "KDChartMeasure.h"
#include <QtXml/QDomDocumentFragment>
#include <KDChartAbstractArea.h>
#include <KDChartTextAttributes.h>
#include <KDChartFrameAttributes.h>
#include <KDChartBackgroundAttributes.h>

#include <KDABLibFakes>


namespace KDChart {


Measure::Measure()
  : mValue( 0.0 ),
    mMode(  KDChartEnums::MeasureCalculationModeAuto ),
    mArea(  0 ),
    mOrientation( KDChartEnums::MeasureOrientationAuto )
{
    // this bloc left empty intentionally
}

Measure::Measure( qreal value,
    KDChartEnums::MeasureCalculationMode mode,
    KDChartEnums::MeasureOrientation orientation )
  : mValue( value ),
    mMode(  mode ),
    mArea(  0 ),
    mOrientation( orientation )
{
    // this bloc left empty intentionally
}

Measure::Measure( const Measure& r )
  : mValue( r.value() ),
    mMode(  r.calculationMode() ),
    mArea(  r.referenceArea() ),
    mOrientation( r.referenceOrientation() )
{
    // this bloc left empty intentionally
}

Measure & Measure::operator=( const Measure& r )
{
    if( this != &r ){
        mValue = r.value();
        mMode  = r.calculationMode();
        mArea  = r.referenceArea();
        mOrientation = r.referenceOrientation();
    }

    return *this;
}


qreal Measure::calculatedValue( const QObject* autoArea,
                                KDChartEnums::MeasureOrientation autoOrientation) const
{
    if( mMode == KDChartEnums::MeasureCalculationModeAbsolute ){
        return mValue;
    }else{
        qreal value = 0.0;
        const QObject* area = mArea ? mArea : autoArea;
        KDChartEnums::MeasureOrientation orientation = mOrientation;
        switch( mMode ){
            case KDChartEnums::MeasureCalculationModeAuto:
                area = autoArea;
                orientation = autoOrientation;
                break;
            case KDChartEnums::MeasureCalculationModeAutoArea:
                area = autoArea;
                break;
            case KDChartEnums::MeasureCalculationModeAutoOrientation:
                orientation = autoOrientation;
                break;
            case KDChartEnums::MeasureCalculationModeAbsolute: // fall through intended
            case KDChartEnums::MeasureCalculationModeRelative:
                break;
        }
        if( area ){
            QSize size;
            const QWidget* widget = dynamic_cast<const QWidget*>(area);
            if( widget ){
                size = widget->geometry().size();
            }else{
                const AbstractArea* kdcArea = dynamic_cast<const AbstractArea*>(area);
                if( kdcArea ){
                    size = kdcArea->geometry().size();
                }else if( mMode != KDChartEnums::MeasureCalculationModeAbsolute ){
                    qDebug("Measure::calculatedValue() has no reference area.");
                    return 1.0;
                }
            }
            qreal referenceValue;
            switch( orientation ){
                case KDChartEnums::MeasureOrientationAuto: // fall through intended
                case KDChartEnums::MeasureOrientationMinimum:
                    referenceValue = qMin( size.width(), size.height() );
                    break;
                case KDChartEnums::MeasureOrientationMaximum:
                    referenceValue = qMax( size.width(), size.height() );
                    break;
                case KDChartEnums::MeasureOrientationHorizontal:
                    referenceValue = size.width();
                    break;
                case KDChartEnums::MeasureOrientationVertical:
                    referenceValue = size.height();
                    break;
            }
            value = mValue / 1000.0 * referenceValue;
        }
        return value;
    }
}


bool Measure::operator==( const Measure& r ) const
{
    return( mValue == r.value() &&
            mMode  == r.calculationMode() &&
            mArea  == r.referenceArea() &&
            mOrientation == r.referenceOrientation() );
}




}

#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator<<(QDebug dbg, const KDChart::Measure& m)
{
    dbg << "KDChart::Measure("
	<< "value="<<m.value()
	<< "calculationmode="<<m.calculationMode()
	<< "referencearea="<<m.referenceArea()
	<< "referenceorientation="<<m.referenceOrientation()
	<< ")";
    return dbg;
}
#endif /* QT_NO_DEBUG_STREAM */
