/***************************************************************************
              mutil.h  -  Kugar utlity class
              -------------------
    begin     : Tue Aug 17 1999                                           
    copyright : (C) 1999 by Mutiny Bay Software                         
    email     : info@mutinybaysoftware.com                                     
 ***************************************************************************/

#ifndef MUTIL_H
#define MUTIL_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qarray.h>


/**Kugar utlity class
  *@author Mutiny Bay Software
  */

class MUtil {

public:
	/** Formatting constants */
  enum DateFormatType {
		MDY_SLASH = 0, MDY_DASH,
		MMDDY_SLASH, MMDDY_DASH,
		MDYYYY_SLASH, MDYYYY_DASH,
		MMDDYYYY_SLASH, MMDDYYYY_DASH,
		YYYYMD_SLASH, YYYYMD_DASH
	};

	/** Formats a date using one of the pre-defined formats */
  static QString formatDate(const QDate& value, int format);

  /** Returns the count for an array of doubles */
  static int count(QArray<double>* values);
  /** Returns the sum for an array of doubles */
  static double sum(QArray<double>* values);
  /** Returns the average value for an array of doubles */
  static double average(QArray<double>* values);
  /** Returns the variance for an array of doubles */
  static double variance(QArray<double>* values);
  /** Returns the standard deviation for an array of doubles */
  static double stdDeviation(QArray<double>* values);

};

#endif
