/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KDChart library.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid commercial KDChart licenses may use this file in
** accordance with the KDChart Commercial License Agreement provided with
** the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#ifndef __KDCHARTDATA_H__
#define __KDCHARTDATA_H__

#include <qstring.h>
#include <qdatetime.h>
#include <float.h>

#include "KDChartGlobal.h"

// Please leave all methods in this class inline!
// It's necessary since it's part of the interface provided by KDChart
class KDChartData
{
public:
    enum ValueType { NoValue, String, Double, DateTime };
    KDChartData( double value ) :
        _valueType( Double ), dValue( value )
    {}
    KDChartData( const QString& value ) :
        _valueType( String ), sValue( value )
    {}
    KDChartData() :
            _valueType( NoValue )
    {}

    ValueType valueType() const
    {
        return _valueType;
    }
    bool hasValue() const
    {
        return _valueType != NoValue;
    }
    bool isString() const
    {
        return _valueType == String;
    }
    bool isDouble() const
    {
        return _valueType == Double;
    }
    bool isDateTime() const
    {
        return _valueType == DateTime;
    }

    bool operator==( const KDChartData& it ) const
    {
        bool bRet = hasValue() == it.hasValue();
        if ( bRet && hasValue() ) {
        bRet = valueType() == it.valueType();
            if ( bRet ) {
                switch ( valueType() ) {
                case String:
                    bRet = stringValue() == it.stringValue();
                    break;
                case Double:
                    bRet = doubleValue() == it.doubleValue();
                    break;
                case DateTime:
                    bRet = dateTimeValue()
                           == it.dateTimeValue();
                    break;
                default:
                    bRet = false;
                }
            }
        }
        return bRet;
    }

    void clearValue()
    {
        _valueType = NoValue;
    }

    QString stringValue() const
    {
        return isString() ? sValue : QString::null;
    }
    double doubleValue() const
    {
        return isDouble() ? dValue : DBL_MIN;
    }
    QDateTime dateTimeValue() const
    {
        return isDateTime() ? dtValue : QDateTime();
    }

private:
    ValueType _valueType;
    QDateTime dtValue;
    double dValue;
    QString sValue; // dValue and sValue should be a union,
    // but QString has a non-default constructor
}
;

#endif
