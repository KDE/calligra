/* This file is part of the KDE project
   Copyright (C) 2007 Sascha Pfau <MrPeacock@gmail.com>
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "helper.h"

#include <kdebug.h>

#include <QDate>

#include <math.h>

/*  DISABLED - we use KCalendarSystem instead
void addMonths( QDate & date, int months )
{
  int d = date.day();
  int m = date.month() + months;
  int y = date.year();

  if ( m > 12 )
  {
    y += (int) ( m / 12 );
    m %= 12;
  }

  // e.g. 31 Feb: decrease day...
  while ( !QDate::isValid( y, m, d ) && d > 0 )
    --d;

  date.setYMD( y, m, d );
}

void subMonths( QDate & date, int months )
{
  int d = date.day();
  int m = date.month() - months;
  int y = date.year();

  while ( m < 1 )
  {
    m += 12;
    y -= 1;
  }

  // e.g. 31 Feb: decrease day
  while ( !QDate::isValid( y, m, d ) && d > 0 )
    --d;

  date.setYMD( y, m, d );
}

*/

int KSpread::daysPerYear(QDate const & date, int basis)
{
    switch (basis) {
    case 0:
        return 360;

    case 1:
        if (QDate::isLeapYear(date.year()))
            return 366;
        return 365;

    case 2:
        return 360;
    case 3:
        return 365;
    case 4:
        return 360;
    }

    return -1;
}

int KSpread::daysBetweenDates(QDate const & date1, QDate const & date2, int basis)
{
    int day1, day2, month1, month2, year1, year2;
    bool isLeapYear = false;
    int days, months, years;

    day1   = date1.day();
    month1 = date1.month();
    year1  = date1.year();
    day2   = date2.day();
    month2 = date2.month();
    year2  = date2.year();

    years  = year2  - year1;
    months = month2 - month1 + years * 12;
    days   = day2   - day1;

    isLeapYear = QDate::isLeapYear(year1);

    switch (basis) {
    case 0:
        if (month1 == 2 && month2 != 2 && year1 == year2) {
            if (isLeapYear)
                return months * 30 + days - 1;
            else
                return months * 30 + days - 2;
        }
        return months * 30 + days;

    case 1: // TODO: real days for difference between months!
        //    return ( month2 - month1 ) * 30 + years * 360 + days;

    case 2: // TODO: real days for difference between months!
        //    return ( month2 - month1 ) * 30 + years * 365 + days;

    case 3:
        return date1.daysTo(date2);

    case 4:
        return months * 30 + days;
    }

    return -1;
}

int KSpread::days360(int day1, int month1, int year1, bool leapYear1,
                     int day2, int month2, int year2,
                     bool usaMethod)
{
    if (day1 == 31)
        day1--;
    else if (usaMethod && (month1 == 2 && (day1 == 29 || (day1 == 28 && ! leapYear1))))
        day1 = 30;

    if (day2 == 31) {
        if (usaMethod && day1 != 30) {
            // date2 += 1
            day2 = 1;
            if (month2 == 12) {
                year2++;
                month2 = 1;
            } else
                month2++;
        } else
            day2 = 30;
    }
    return day2 + month2 * 30 + year2 * 360 - day1 - month1 * 30 - year1 * 360;
}


// days360
int KSpread::days360(const QDate& _date1, const QDate& _date2, bool european)
{
    int day1, month1, year1, day2, month2, year2;

    day1 = _date1.day();
    month1 = _date1.month();
    year1 = _date1.year();

    day2 = _date2.day();
    month2 = _date2.month();
    year2 = _date2.year();

    return days360(day1, month1, year1, QDate::isLeapYear(_date1.year()), day2, month2, year2, !european);
}




// // days360
// int KSpread::days360( const QDate& _date1, const QDate& _date2, bool european )
// {
//   int day1, day2;
//   int month1, month2;
//   int year1, year2;
//   bool negative = false;
//   QDate date1( _date1 );
//   QDate date2( _date2 );
//
//   if (date1.daysTo( date2 ) < 0)
//   {
//     QDate tmp( date1 );
//     date1 = date2;
//     date2 = tmp;
//     negative = true;
//   }
//
//   day1   = date1.day();
//   day2   = date2.day();
//   month1 = date1.month();
//   month2 = date2.month();
//   year1  = date1.year();
//   year2  = date2.year();
//
//   if ( european )
//   {
//     if ( day1 == 31 )
//       day1 = 30;
//     if ( day2 == 31 )
//       day2 = 30;
//   }
//   else
//   {
//     // thanks to the Gnumeric developers for this...
//     if ( month1 == 2 && month2 == 2
//          && date1.daysInMonth() == day1
//          && date2.daysInMonth() == day2 )
//       day2 = 30;
//
//     if ( month1 == 2 && date1.daysInMonth() == day1 )
//       day1 = 30;
//
//     if ( day2 == 31 && day1 >= 30 )
//       day2 = 30;
//
//     if ( day1 == 31 )
//       day1 = 30;
//   }
//
//   return ( ( year2 - year1 ) * 12 + ( month2 - month1 ) ) * 30
//     + ( day2 - day1 );
// }

// yearFrac
long double KSpread::yearFrac(const QDate& refDate, const QDate& startDate, const QDate& endDate, int basis)
{
    QDate date1 = startDate;
    QDate date2 = endDate;

    //
    // calculation
    //

    QDate date0 = refDate; // referenceDate

    if (date2 < date1) {
        // exchange dates
        QDate Temp1 = date1;
        date1 = date2;
        date2 = Temp1;
    }

    int days = date1.daysTo(date2);

//   kDebug(36002) <<"date1 =" << date1 <<"    date2 =" << date2 <<"    days =" << days <<"    basis =" << basis;

    long double res = 0;
    long double peryear = 0;
    int nYears = 0;

    switch (basis) {
    case 1: {
        nYears = date2.year() - date1.year() + 1;
        for (int y = date1.year(); y <= date2.year(); ++y) {
            peryear += QDate::isLeapYear(y) ? 366 : 365;
        }
        // less than one year - even if it does span two consequentive years ...
        if (QDate(date1.year() + 1, date1.month(), date1.day()) >= date2) {
            nYears = 1;
            peryear = 365;
            if (QDate::isLeapYear(date1.year()) && (date1.month() <= 2)) peryear = 366;
            if (QDate::isLeapYear(date2.year()) && (date2.month() > 2)) peryear = 366;
        }
        peryear = peryear / (long double) nYears;
        nYears = 0;
        break;

    }
    case 2: {
        // Actual/360
        peryear = 360;
        break;
    }
    case 3: {
        // Actual/365
        peryear = 365;
        break;
    }
    case 4: {
        // 30/360 Europe

        // calc datedif360 (start, end, Europe)
        days = days360(date1, date2, 1);

        peryear = 360;
        break;
    }
    default: {
        // NASD 30/360
        //basis = 0;

        // calc datedif360 (start, end, US)
        days = days360(date1, date2, 0);

        peryear = 360;
    }
    }

    res = (long double)(nYears) + (long double)days / (long double) peryear;
//   kDebug(36002)<<"getYearFrac res="<<res;
    return res;
}

// pow1p calculate (1+x)^y accurately
long double KSpread::pow1p(const long double& x, const long double& y)
{
    if (fabs(x) > 0.5)
        return pow(1 + x, y);
    else
        return exp(y * log1p(x));
}

// pow1pm1 calculate ((1+x)^y)-1 accurately
long double KSpread::pow1pm1(const long double& x, const long double& y)
{
    if (x <= -1)
        return pow(1 + x, y) - 1;
    else
        return expm1(y * log1p(x));
}

long double KSpread::duration(const QDate& refDate, const QDate& settlement, const QDate& maturity,
                              const long double& coup_, const long double& yield_, const int& freq, const int& basis, const long double& numOfCoups)
{
    long double yield = yield_;
    long double coup = coup_;

//   kDebug(36002)<<"DURATION_HELPER";
//   kDebug(36002)<<"sett ="<<settlement<<" mat ="<<maturity<<" coup ="<<coup<<" yield ="<<yield<<" freq ="<<freq<<" basis ="<<basis;


    long double yearfrac = yearFrac(refDate, settlement, maturity, basis);
    long double res = 0.0l;
    const long double f100 = 100.0l;
    coup *= f100 / (long double)(freq);

    yield /= freq;
    yield += 1.0;

    long double diff = yearfrac * freq - numOfCoups;

    long double t;

    for (t = 1.0l ; t < numOfCoups ; t += 1.0l)
        res += (t + diff) * (coup) / pow(yield, t + diff);

    res += (numOfCoups + diff) * (coup + f100) / pow(yield, numOfCoups + diff);

    long double p = 0.0l;
    for (t = 1.0l ; t < numOfCoups ; t += 1.0l)
        p += coup / pow(yield, t + diff);

    p += (coup + f100) / pow(yield, numOfCoups + diff);

    res /= p;
    res /= (long double)(freq);

    return(res);
}
