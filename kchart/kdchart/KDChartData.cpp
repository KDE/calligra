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


/**
   Constructor. Assigns a QString value to this object.

   \param value the QString value which this object should represent
*/


/**
   Constructor. Creates a KDChartData object that represents no value.
*/


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
