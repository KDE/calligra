/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
 ** Copyright (C) 2001-2003 Klar�vdalens Datakonsult AB.  All rights reserved.
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
#ifndef __KDCHARTDATAINTERN_H__
#define __KDCHARTDATAINTERN_H__

#include <qvariant.h>
#include <qstring.h>
#include <qdatetime.h>
#include "KDChartGlobal.h"

/**
   \file KDChartDataIntern.h

   \brief Provides a class to encapsulate one data value in a chart.
*/



// Please leave all methods in this class inline!
// It's necessary since it's part of the interface provided by KDChart
class KDCHART_EXPORT KDChartData
{
public:
    // 0. default c'tor: initializing all values as undefined

    KDChartData() :
        _valueType(  QVariant::Invalid ),
        _valueType2( QVariant::Invalid ),
        _propSetID(  0 )
    {}

    // 1. simple c'tors: used for 1-coordinate data

    KDChartData( double value ) :
        _valueType(  QVariant::Double ), dValue( value ),
        _valueType2( QVariant::Invalid ),
        _propSetID(  0 )
    {}
    /* string values are only supported for legend texts or axis labels */
    KDChartData( const QString& value ) :
        _valueType(  QVariant::String ), sValue( value ),
        _valueType2( QVariant::Invalid ),
        _propSetID(  0 )
    {}
    /* date/time values for /ordinate/ axes are not implemented yet
       KDChartData( QDateTime value ) :
       _valueType(  QVariant::DateTime ), dtValue( value ),
       _valueType2( QVariant::Invalid ),
       _propSetID(  0 )
       {}*/

    // 2. complex c'tors: used for 2-coordinate data

    // 2.a) with additional Date/Time: normally used when Date on x-axis
    //      e.g. for time related index numbers like water level measurements
    KDChartData( double yValue, QDateTime xValue ) :
        _valueType(  QVariant::Double   ), dValue(   yValue  ),
        _valueType2( QVariant::DateTime ), dtValue2( xValue ),
        _propSetID(  0 )
    {}


    KDChartData( const QVariant& _value1,
                 const QVariant& _value2 )
    {
        switch( _value1.type() ){
            case QVariant::Int:
            case QVariant::UInt:
            case QVariant::Double:
                _valueType = QVariant::Double;
                dValue     = _value1.toDouble();
                break;
            case QVariant::DateTime:
                _valueType = QVariant::DateTime;
                dtValue    = _value1.toDateTime();
                break;
            case QVariant::String:
                _valueType = QVariant::String;
                sValue     = _value1.toString();
                break;
            default:
                _valueType = QVariant::Invalid;
        }
        switch( _value2.type() ){
            case QVariant::Int:
            case QVariant::UInt:
            case QVariant::Double:
                _valueType2 = QVariant::Double;
                dValue2     = _value2.toDouble();
                break;
            case QVariant::DateTime:
                _valueType2 = QVariant::DateTime;
                dtValue2    = _value2.toDateTime();
                break;
            default:
                _valueType2 = QVariant::Invalid;
        }
        _propSetID = 0;
    }

    /* date/time values for /ordinate/ axes are not implemented yet
       KDChartData( QDateTime yValue, QDateTime xValue ) :
       _valueType(  DateTime ), dtValue(  yValue  ),
       _valueType2( DateTime ), dtValue2( xValue ),
       _propSetID(  0 )
       {}*/
    // 2.b) with additional Double: may be used for mathematical data...
    KDChartData( double yValue, double xValue ) :
        _valueType(  QVariant::Double ), dValue(  yValue  ),
        _valueType2( QVariant::Double ), dValue2( xValue ),
        _propSetID(  0 )
    {}
    /* date/time values for /ordinate/ axes are not implemented yet
       KDChartData( QDateTime yValue, double xValue ) :
       _valueType(  DateTime ), dtValue( yValue  ),
       _valueType2( Double   ), dValue2( xValue ),
       _propSetID(  0 )
       {}*/

/*
    /-**
       Copy constructor.

       \sa setData
    * /
    KDChartData( const KDChartData& other ) :
    {
        setData( other );
    }

    /-**
       Assignment operator.

       \sa setData
    * /
    KDChartData& operator=( const KDChartData& R )
    {
        setData( R );
        return *this;
    }
*/
    /**
       Compare operator.

       \sa isEqual
    */
    bool operator==( const KDChartData& it ) const
    {
        return isEqual(*this, it);
    }

    /**
       Compare operator.

       \sa isEqual
    */
    bool operator!=( const KDChartData& it ) const
    {
        return !isEqual(*this, it);
    }

    /**
       Compare method.

       \note The property set ID is <b>not</b> taken into account while comparing.
       Two KDChartData are considered equal if their data values are equal - the
       property set ID is ignored.
    */
    bool isEqual( const KDChartData& a, const KDChartData& b ) const
    {
        bool bRet = (a.hasValue( 1 ) == b.hasValue( 1 )) &&
                    (a.hasValue( 2 ) == b.hasValue( 2 ));
        if ( bRet && a.hasValue( 1 ) ) {
            bRet = a.valueType( 1 ) == b.valueType( 1 );
            if ( bRet ) {
                switch ( a.valueType( 1 ) ) {
                case QVariant::String:
                    bRet = a.stringValue( 1 ) == b.stringValue( 1 );
                    break;
                case QVariant::Double:
                    bRet = a.doubleValue( 1 ) == b.doubleValue( 1 );
                    break;
                case QVariant::DateTime:
                    bRet = a.dateTimeValue( 1 ) == b.dateTimeValue( 1 );
                    break;
                default:
                    bRet = false;
                }
            }
            if ( bRet && a.hasValue( 2 ) ) {
                bRet = a.valueType( 2 ) == b.valueType( 2 );
                if ( bRet ) {
                    switch ( a.valueType( 2 ) ) {
                        // note: the 2nd value can not be a string
                        //       - must be a date or a number!
                    case QVariant::Double:
                        bRet = a.doubleValue( 2 ) == b.doubleValue( 2 );
                        break;
                    case QVariant::DateTime:
                        bRet = a.dateTimeValue( 2 ) == b.dateTimeValue( 2 );
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

    /**
       Method setAll.

       \note The property set ID is <b>also</b> changed by the setAll method.

       \sa setData
    */
    void setAll( const KDChartData& R )
    {
        setData( R );
        setPropertySet( R.propertySet() );
    }


    /**
       Method setData.

       \note The property set ID is <b>not</b> changed by the setData method.
       If you want to set it please call \c KDChartData::setPropertySet() explicitely.

       \sa setAll
    */
    void setData( const KDChartData& R )
    {
        if( &R != this ){
            _valueType  = R._valueType;
            _valueType2 = R._valueType2;
            switch ( valueType( 1 ) ) {
            case QVariant::String:
                sValue  = R.sValue;
                break;
            case QVariant::Double:
                dValue  = R.dValue;
                break;
            case QVariant::DateTime:
                dtValue = R.dtValue;
                break;
            default:
                /* NOOP */;
            }
            switch ( valueType( 2 ) ) {
                // note: the 2nd value can not be a string
                //       - must be a date or a number!
            case QVariant::Double:
                dValue2  = R.dValue2;
                break;
            case QVariant::DateTime:
                dtValue2 = R.dtValue2;
                break;
            default:
                /* NOOP */;
            }
            // Note: We do *not* copy the _propSetID here since it contains
            //       no values but is used to handle some layout information...
        }
    }

    QVariant::Type valueType( int valNo=1 ) const
    {
        return (1 == valNo)
                    ? _valueType
                    : _valueType2;
    }
    bool hasValue( int valNo=1 ) const
    {
        return (1 == valNo)
            ?  (_valueType != QVariant::Invalid)
            : ((_valueType2 == QVariant::Double) || (_valueType2 == QVariant::DateTime));
    }
    /* string values are only supported for legend texts or axis labels */
    bool isString( int valNo=1 ) const
    {
        return (1 == valNo)
                    ? (_valueType == QVariant::String)
                    : false;
    }
    bool isDouble( int valNo=1 ) const
    {
        return (1 == valNo)
                    ? (_valueType == QVariant::Double)
                    : (_valueType2 == QVariant::Double);
    }
    bool isDateTime( int valNo=1 ) const
    {
        return (1 == valNo)
                    ? (_valueType == QVariant::DateTime)
                    : (_valueType2 == QVariant::DateTime);
    }


    void clearValue()
    {
        _valueType  = QVariant::Invalid;
        _valueType2 = QVariant::Invalid;
        _propSetID  = 0;
    }

    QVariant value( int valNo=1 ) const
    {
        if( 1 == valNo )
            switch ( valueType( 1 ) ) {
            case QVariant::String:
                return sValue;
            case QVariant::Double:
                return dValue;
            case QVariant::DateTime:
                return dtValue;
            default:
                return QVariant();
            }
        else if( 2 == valNo )
            switch ( valueType( 2 ) ) {
            case QVariant::Double:
                return dValue2;
            case QVariant::DateTime:
                return dtValue2;
            default:
                return QVariant();
            }
        else
            return QVariant();
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
       \sa KDCHART_PROPSET_NORMAL_DATA, KDCHART_PROPSET_TRANSPARENT_DATA
    */
    void setPropertySet( int propSetID = 0 )
    {
        _propSetID = propSetID;
    }
    /**
       Return the ID of the property set that is assigned to this data cell.

       Use KDChartParams::properties( int ID ) for accessing the respective property set information.

       \sa setPropertySet
       \sa KDCHART_PROPSET_NORMAL_DATA, KDCHART_PROPSET_TRANSPARENT_DATA
    */
    int propertySet() const
    {
        return _propSetID;
    }


private:
    // OBLIGATORY 1st value: usually used for ordinate axis
    QVariant::Type _valueType;
    QDateTime dtValue;
    double dValue;  // Sorry, d(t)Value and sValue cannot be a union,
    QString sValue; // since QString has a non-default constructor.

    // OPTIONAL 2nd value: if valid, normally used for abscissa axis
    // note: this 2nd value can not be a string - must be a date or a number!
    QVariant::Type _valueType2;
    QDateTime dtValue2;
    double dValue2;

    // ID number of the property set assigned to this cell
    int _propSetID;
};

#endif
