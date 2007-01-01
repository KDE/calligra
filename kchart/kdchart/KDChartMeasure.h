/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHARTMEASURE_H
#define KDCHARTMEASURE_H

#include <QDebug>
#include <Qt>
#include "KDChartGlobal.h"
#include "KDChartEnums.h"

class QObject;

namespace KDChart {


class KDCHART_EXPORT Measure
{
public:
    Measure();
    /*implicit*/ Measure( qreal value,
                          KDChartEnums::MeasureCalculationMode mode = KDChartEnums::MeasureCalculationModeAuto,
                          KDChartEnums::MeasureOrientation orientation = KDChartEnums::MeasureOrientationAuto );
    Measure( const Measure& );
    Measure &operator= ( const Measure& );

    void setValue( qreal val ){ mValue = val; }
    qreal value() const { return mValue; }

    void setCalculationMode( KDChartEnums::MeasureCalculationMode mode ){ mMode = mode; }
    KDChartEnums::MeasureCalculationMode calculationMode() const { return mMode; }

    /**
      * The reference area must either be derived from AbstractArea
      * or be derived from QWidget, so e.g. it could be derived from
      * AbstractAreaWidget too.
      */
    void setRelativeMode( const QObject * area,
                          KDChartEnums::MeasureOrientation orientation )
    {
        mMode = KDChartEnums::MeasureCalculationModeRelative;
        mArea = area;
        mOrientation = orientation;
    }

    /**
      * The reference area must either be derived from AbstractArea
      * or be derived from QWidget, so e.g. it could be derived from
      * AbstractAreaWidget too.
      */
    void setReferenceArea( const QObject * area ){ mArea = area; }
    /**
      * The returned reference area will either be derived from AbstractArea
      * or be derived from QWidget.
      */
    const QObject * referenceArea() const { return mArea; }

    void setReferenceOrientation( KDChartEnums::MeasureOrientation orientation ){ mOrientation = orientation; }
    KDChartEnums::MeasureOrientation referenceOrientation() const { return mOrientation; }

    /**
      * The reference area must either be derived from AbstractArea
      * or be derived from QWidget, so e.g. it could be derived from
      * AbstractAreaWidget too.
      */
    qreal calculatedValue( const QObject * autoArea, KDChartEnums::MeasureOrientation autoOrientation ) const;

    bool operator==( const Measure& ) const;
    bool operator!=( const Measure& other ) const { return !operator==(other); }

private:
    qreal mValue;
    KDChartEnums::MeasureCalculationMode mMode;
    const QObject* mArea;
    KDChartEnums::MeasureOrientation mOrientation;
}; // End of class Measure

}

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::Measure& );
#endif /* QT_NO_DEBUG_STREAM */

#endif // KDCHARTMEASURE_H
