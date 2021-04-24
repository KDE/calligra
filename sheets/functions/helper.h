// This file is part of the KDE project
// SPDX-FileCopyrightText: 1998-2002 The KSpread Team <calligra-devel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef kspread_functions_helper_h_
#define kspread_functions_helper_h_
// helper functions for other functions

#include "sheets_common_export.h"

class QDate;

namespace Calligra
{
namespace Sheets
{

/* 0: US 30 / 360
 * 1: real days
 * 2: real days / 360
 * 3: real days / 365
 * 4: European 30 / 360
 */
CALLIGRA_SHEETS_COMMON_EXPORT int daysPerYear(QDate const & date, int basis);

/* 0: US 30 / 360
 * 1: real days
 * 2: real days / 360
 * 3: real days / 365
 * 4: European 30 / 360
 */
CALLIGRA_SHEETS_COMMON_EXPORT int daysBetweenDates(QDate const & date1, QDate const & date2, int basis);


// ODF
CALLIGRA_SHEETS_COMMON_EXPORT int days360(const QDate& _date1, const QDate& _date2, bool european);
CALLIGRA_SHEETS_COMMON_EXPORT int days360(int day1, int month1, int year1, bool leapYear1, int  day2, int month2, int year2, bool leapYear2, bool usa);
CALLIGRA_SHEETS_COMMON_EXPORT long double yearFrac(const QDate& refDate, const QDate& startDate, const QDate& endDate, int basis);
CALLIGRA_SHEETS_COMMON_EXPORT long double duration(const QDate& refDate, const QDate& settlement, const QDate& maturity, const long double& coup_, const long double& yield_, const int& freq, const int& basis, const long double& numOfCoups);
CALLIGRA_SHEETS_COMMON_EXPORT long double pow1p(const long double& x, const long double& y);
CALLIGRA_SHEETS_COMMON_EXPORT long double pow1pm1(const long double& x, const long double& y);
} // namespace Sheets
} // namespace Calligra

#endif
