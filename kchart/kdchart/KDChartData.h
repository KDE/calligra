/* -*- Mode: C++ -*-
   $Id$
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
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
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


#include "KDChartGlobal.h"


/**
   \file KDChartData.h

   \brief Provides a class to encapsulate one data value in a chart.
*/


// Please leave all methods in this class inline!
// It's necessary since it's part of the interface provided by KDChart
class KDChartData
{
public:
    // OK, so this is bad and should really be a static const
    // double. But then it is not possible to just use the KChart
    // interface without linking to KChart itself, because those
    // symbols would be missing.
#define POS_INFINITE DBL_MAX
#define NEG_INFINITE -DBL_MAX

    enum ValueType { NoValue, String, Double, DateTime };

    // 0. default c'tor: initializing all values as undefined

    KDChartData() :
        _valueType(  NoValue ),
        _valueType2( NoValue ),
        _propSetID(  0 )
        {}

    // 1. simple c'tors: used for 1-coordinate data

    KDChartData( double value ) :
        _valueType(  Double ), dValue( value ),
        _valueType2( NoValue ),
        _propSetID(  0 )
        {}
    /* string values are only supported for legend texts or axis labels */
    KDChartData( const QString& value ) :
        _valueType(  String ), sValue( value ),
        _valueType2( NoValue ),
        _propSetID(  0 )
        {}
    /* date/time values for /ordinate/ axes are not implemented yet
       KDChartData( QDateTime value ) :
       _valueType(  DateTime ), dtValue( value ),
       _valueType2( NoValue ),
       _propSetID(  0 )
       {}*/

    // 2. complex c'tors: used for 2-coordinate data

    // 2.a) with additional Date/Time: normally used when Date on x-axis
    //      e.g. for time related index numbers like water level measurements
    KDChartData( double yValue, QDateTime xValue ) :
        _valueType(  Double   ), dValue(   yValue  ),
        _valueType2( DateTime ), dtValue2( xValue ),
        _propSetID(  0 )
        {}
    /* date/time values for /ordinate/ axes are not implemented yet
       KDChartData( QDateTime yValue, QDateTime xValue ) :
       _valueType(  DateTime ), dtValue(  yValue  ),
       _valueType2( DateTime ), dtValue2( xValue ),
       _propSetID(  0 )
       {}*/
    // 2.b) with additional Double: may be used for mathematical data...
    KDChartData( double yValue, double xValue ) :
        _valueType(  Double ), dValue(  yValue  ),
        _valueType2( Double ), dValue2( xValue ),
        _propSetID(  0 )
        {}
    /* date/time values for /ordinate/ axes are not implemented yet
       KDChartData( QDateTime yValue, double xValue ) :
       _valueType(  DateTime ), dtValue( yValue  ),
       _valueType2( Double   ), dValue2( xValue ),
       _propSetID(  0 )
       {}*/

    ValueType valueType( int valNo=1 ) const
        {
            return (1 == valNo)
                       ? _valueType
                       : _valueType2;
        }
    bool hasValue( int valNo=1 ) const
        {
            return (1 == valNo)
                       ? (_valueType != NoValue)
                       : ((_valueType2 == Double) || (_valueType2 == DateTime));
        }
    /* string values are only supported for legend texts or axis labels */
    bool isString( int valNo=1 ) const
        {
            return (1 == valNo)
                       ? (_valueType == String)
                       : false;
        }
    bool isDouble( int valNo=1 ) const
        {
            return (1 == valNo)
                       ? (_valueType == Double)
                       : (_valueType2 == Double);
        }
    bool isNormalDouble( int valNo=1 ) const
        {
            if( !isDouble( valNo ) )
                return false;
            return doubleValue( valNo ) != POS_INFINITE
                && doubleValue( valNo ) != NEG_INFINITE;
        }
    bool isPosInfiniteDouble( int valNo=1 ) const
        {
            if( !isDouble( valNo ) )
                return false;
            return doubleValue( valNo ) == POS_INFINITE;
        }
    bool isNegInfiniteDouble( int valNo=1 ) const
        {
            if( !isDouble( valNo ) )
                return false;
            return doubleValue( valNo ) == NEG_INFINITE;
        }
    bool isDateTime( int valNo=1 ) const
        {
            return (1 == valNo)
                       ? (_valueType == DateTime)
                       : (_valueType2 == DateTime);
        }


    /**
       Assignment operator.

       \note The property set ID is <b>not</b> changed by the assignment operator.
       If you want to set it please call \c KDChartData::setPropertySet() explicitely.
    */
    KDChartData& operator=( const KDChartData& R )
        {
            _valueType = R._valueType;
            _valueType2 = R._valueType2;
            switch ( valueType( 1 ) ) {
            case String:
                sValue  = R.sValue;
                break;
            case Double:
                dValue  = R.dValue;
                break;
            case DateTime:
                dtValue = R.dtValue;
                break;
            default:
                /* NOOP */;
            }
            switch ( valueType( 2 ) ) {
                // note: the 2nd value can not be a string
                //       - must be a date or a number!
            case Double:
                dValue2  = R.dValue2;
                break;
            case DateTime:
                dtValue2 = R.dtValue2;
                break;
            default:
                /* NOOP */;
            }
            // Note: We do *not* copy the _propSetID here since it contains
            //       no values but is used to handle some layout information...
            return *this;
        }

    /**
       Compare operator.

       \note The property set ID is <b>not</b> taken into account while comparing.
       Two KDChartData are considered equal if their data values are equal - the
       property set ID is ignored.
    */
    bool operator==( const KDChartData& it ) const
        {
            bool bRet = (hasValue( 1 ) == it.hasValue( 1 )) &&
                        (hasValue( 2 ) == it.hasValue( 2 ));
            if ( bRet && hasValue( 1 ) ) {
                bRet = valueType( 1 ) == it.valueType( 1 );
                if ( bRet ) {
                    switch ( valueType( 1 ) ) {
                    case String:
                        bRet = stringValue( 1 ) == it.stringValue( 1 );
                        break;
                    case Double:
                        bRet = doubleValue( 1 ) == it.doubleValue( 1 );
                        break;
                    case DateTime:
                        bRet = dateTimeValue( 1 ) == it.dateTimeValue( 1 );
                        break;
                    default:
                        bRet = false;
                    }
                }
                if ( bRet &&  hasValue( 2 ) ) {
                    bRet = valueType( 2 ) == it.valueType( 2 );
                    if ( bRet ) {
                        switch ( valueType( 2 ) ) {
                            // note: the 2nd value can not be a string
                            //       - must be a date or a number!
                        case Double:
                            bRet = doubleValue( 2 ) == it.doubleValue( 2 );
                            break;
                        case DateTime:
                            bRet = dateTimeValue( 2 ) == it.dateTimeValue( 2 );
                            break;
                        default:
                            bRet = false;
                        }
                    }
                }
                // Note: We do *not* compare the _propSetID here since it contains
                //       no values but is used to handle some layout information...
            }
            return bRet;
        }

    void clearValue()
        {
            _valueType = NoValue;
            _valueType2 = NoValue;
            _propSetID  = 0;
        }

    /* string values are only supported for legend texts or axis labels */
    QString stringValue( int valNo=1 ) const
        {
            // note: the 2nd value can not be a string
            //       - must be a date or a number!
            if ((1 == valNo) && isString( valNo ))
                return sValue;
            else
                return QString::null;
        }
    double doubleValue( int valNo=1 ) const
        {
            return isDouble( valNo )
                ? ((1 == valNo) ? dValue : dValue2)
                : DBL_MIN;
        }
    QDateTime dateTimeValue( int valNo=1 ) const
        {
            return isDateTime( valNo )
                ? ((1 == valNo) ? dtValue : dtValue2)
                : QDateTime();
        }

    /**
       Assign a property set to a data cell.

       \param propSetID The ID of the property set to be assigned to this data cell.
       This ID can either be one of the built-in IDs documented
       at KDChartPropertySet::BuiltinDataPropertySetIDs or
       a special ID that was given back by a
       KDChartParams::registerProperties function call.

       \sa propertySet
       \sa KDChartParams::KDCHART_PROPSET_NORMAL_DATA, KDChartParams::KDCHART_PROPSET_TRANSPARENT_DATA
    */
    void setPropertySet( int propSetID = 0 )
        {
            _propSetID = propSetID;
        }
    /**
       Return the ID of the property set that is assigned to this data cell.

       Use KDChartParams::properties( int ID ) for accessing the respective property set information.

       \sa setPropertySet
       \sa KDChartParams::KDCHART_PROPSET_NORMAL_DATA, KDChartParams::KDCHART_PROPSET_TRANSPARENT_DATA
    */
    int propertySet() const
        {
            return _propSetID;
        }


private:
    // OBLIGATORY 1st value: usually used for ordinate axis
    ValueType _valueType;
    QDateTime dtValue;
    double dValue;  // Sorry, d(t)Value and sValue cannot be a union,
    QString sValue; // since QString has a non-default constructor.

    // OPTIONAL 2nd value: if valid, normally used for abscissa axis
    // note: this 2nd value can not be a string - must be a date or a number!
    ValueType _valueType2;
    QDateTime dtValue2;
    double dValue2;

    // ID number of the property set assigned to this cell
    int _propSetID;
};

#endif
