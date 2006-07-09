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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef kspread_functions_helper_h_
#define kspread_functions_helper_h_
// helper functions for other functions

class QDate;

namespace KSpread
{

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

} //namespace KSpread

#endif
