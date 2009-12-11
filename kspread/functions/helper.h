/* This file is part of the KDE project
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
#ifndef kspread_functions_helper_h_
#define kspread_functions_helper_h_
// helper functions for other functions

#include "kspread_export.h"

class QDate;

namespace KSpread
{

/* 0: US 30 / 360
 * 1: real days
 * 2: real days / 360
 * 3: real days / 365
 * 4: European 30 / 360
 */
KSPREAD_EXPORT int daysPerYear(QDate const & date, int basis);

/* 0: US 30 / 360
 * 1: real days
 * 2: real days / 360
 * 3: real days / 365
 * 4: European 30 / 360
 */
KSPREAD_EXPORT int daysBetweenDates(QDate const & date1, QDate const & date2, int basis);


// ODF
KSPREAD_EXPORT int days360(const QDate& _date1, const QDate& _date2, bool european);
KSPREAD_EXPORT int days360(int day1, int month1, int year1, bool leapYear1, int  day2, int month2, int year2, bool usa);
KSPREAD_EXPORT long double yearFrac(const QDate& refDate, const QDate& startDate, const QDate& endDate, int basis);
KSPREAD_EXPORT long double duration(const QDate& refDate, const QDate& settlement, const QDate& maturity, const long double& coup_, const long double& yield_, const int& freq, const int& basis, const long double& numOfCoups);
KSPREAD_EXPORT long double pow1p(const long double& x, const long double& y);
KSPREAD_EXPORT long double pow1pm1(const long double& x, const long double& y);
} //namespace KSpread

#endif
