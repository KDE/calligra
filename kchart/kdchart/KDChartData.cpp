/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#include <KDChartData.h>

/**
   \class KDChartData KDChartData.h
 
   \brief Encapsulates one data value in a chart.
 
   Data values can either be of type QString, double nor non-existant.
*/

/**
   Constructor. Assigns a double value to this object.
 
   \param value the double value which this object should represent
*/
KDChartData::KDChartData( double value )
{
    dValue = value;
    _valueType = Double;
}


/**
   Constructor. Assigns a QString value to this object.
 
   \param value the QString value which this object should represent
*/
KDChartData::KDChartData( const QString& value ) :
sValue( value ),
_valueType( String )
{}


/**
   Constructor. Creates a KDChartData object that represents no value.
*/
KDChartData::KDChartData()
{
    _valueType = NoValue;
}


/**
   \fn bool KDChartData::hasValue() const
 
   Returns true if the object represents a value (double or QString)
   and false otherwise.
 
   \return true if the object represents a value
*/


/**
   \fn bool KDChartData::isString() const
 
   Returns true if the object represents a QString value and false
   otherwise.
 
   \return true if the object represents a QString value
*/


/**
   \fn bool KDChartData::isDouble() const
 
   Returns true if the object represents a double value and false
   otherwise.
 
   \return true if the object represents a double value
*/
