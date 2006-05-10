/***************************************************************************
             mutil.h  -  Kugar utlity class
             -------------------
   begin     : Tue Aug 17 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2002 Alexander Dymo
   email     : cloudtemple@mksat.net
***************************************************************************/

#ifndef MUTIL_H
#define MUTIL_H

#include <qobject.h>
#include <QString>
#include <qdatetime.h>
#include <q3memarray.h>


/**Kugar utlity class
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MUtil
{

public:
    /** Formatting constants */
    enum DateFormatType {
        MDY_SLASH = 0, MDY_DASH,
        MMDDY_SLASH, MMDDY_DASH,
        MDYYYY_SLASH, MDYYYY_DASH,
        MMDDYYYY_SLASH, MMDDYYYY_DASH,
        YYYYMD_SLASH, YYYYMD_DASH,
        DDMMYY_PERIOD, DDMMYYYY_PERIOD
    };

    /** Formats a date using one of the pre-defined formats */
    static QString formatDate( const QDate& value, int format );

    /** Returns the count for an array of doubles */
    static int count( Q3MemArray<double>* values );
    /** Returns the sum for an array of doubles */
    static double sum( Q3MemArray<double>* values );
    /** Returns the average value for an array of doubles */
    static double average( Q3MemArray<double>* values );
    /** Returns the variance for an array of doubles */
    static double variance( Q3MemArray<double>* values );
    /** Returns the standard deviation for an array of doubles */
    static double stdDeviation( Q3MemArray<double>* values );

};

}

#endif
