/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef kspread_functions_helper_h_
#define kspread_functions_helper_h_
// helper functions for other functions

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <qdatetime.h>
#include <qvaluelist.h>

#define SECSPERDAY 86400
#define HALFSEC ( 0.5 / SECSPERDAY )

// namespace is necessary so that e.g. string doesn't conflict with the STL string
namespace KSpreadDB
{
enum Comp { isEqual, isLess, isGreater, lessEqual, greaterEqual, notEqual };
enum Type { numeric, string };

struct Condition
{
  Comp     comp;
  int      index;
  double   value;
  QString  stringValue;
  Type     type;
};

typedef QValueList<Condition> ConditionList;
}

class EDate : public QDate
{
 public:
  static uint greg2jul( int y, int m, int d );

  static uint greg2jul( QDate const & date );

  static void jul2greg( double num, int & y, int & m, int & d );
};

bool getDate( KSContext & context, KSValue::Ptr & arg, QDate & date );
bool getTime( KSContext & context, KSValue::Ptr & arg, QTime & time );

void addMonths( QDate & date, int months );

void subMonths( QDate & date, int months );

/*	0: US 30 / 360
 *	1: real days
 *	2: real days / 360
 *	3: real days / 365
 *	4: European 30 / 360
 */
int daysPerYear( QDate const & date, int basis );

/*	0: US 30 / 360
 *	1: real days
 *	2: real days / 360
 *	3: real days / 365
 *	4: European 30 / 360
 */
int daysBetweenDates( QDate const & date1, QDate const & date2, int basis );

double fact( int n );
double combin( int n, int k );
double gaussinv_helper( double x );

/*********************************************************************
 *
 * Helper function to avoid problems with rounding floating point
 * values. Idea for this kind of solution taken from Openoffice.
 *
 *********************************************************************/
bool approx_equal( double a, double b );

bool kspreadfunc_average_helper( KSContext & context, QValueList<KSValue::Ptr> & args,
                                 double & result,int & number, bool aMode );
bool kspreadfunc_stddev_helper( KSContext & context, QValueList<KSValue::Ptr> & args,
                                double & result, double & avera, bool aMode );

bool kspreadfunc_variance_helper( KSContext & context, QValueList<KSValue::Ptr> & args,
                                  double & result, double avera, bool aMode );

/**
  This method parses the condition in string text to the condition cond.
  It sets the condition's type and value.
*/
void getCond( KSpreadDB::Condition &cond, QString text );

/**
  Returns true if double value d matches the condition cond, built with getCond().
  Otherwise, it returns false.
*/
bool conditionMatches( KSpreadDB::Condition &cond, const double &d );
/**
  Returns true if string value d matches the condition cond, built with getCond().
  Otherwise, it returns false.
*/
bool conditionMatches( KSpreadDB::Condition &cond, const QString &d );

#endif
