/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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

   aReturns true if the object represents a value (double or QString)
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
