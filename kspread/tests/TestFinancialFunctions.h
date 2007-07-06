/* This file is part of the KDE project
   Copyright 2006 Ariya Hidayat <ariya@kde.org>
   Copyright 2007 Sascha Pfau <MrPeacock@gmail.com>

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

#ifndef KSPREAD_TEST_FINANCIAL_FUNCTIONS
#define KSPREAD_TEST_FINANCIAL_FUNCTIONS

#include <QtGui>
#include <qtest_kde.h>

#include <Value.h>

namespace KSpread
{

class TestFinancialFunctions: public QObject
{
    Q_OBJECT

private slots:
  void testDB();
  void testDDB();
  void testDISC();
  void testDOLLARDE();
  void testDOLLARFR();
  void testDURATION();
  void testPDURATION();
  void testEURO();
  void testEUROCONVERT();
  void testINTRATE();
  void testISPMT();
  void testLEVELCOUPON();
  void testNOMINAL();
  void testNPER();
  void testNPV();
  void testPMT();
  void testSLN();
  void testSYD();
  void testZEROCOUPON();

private:
  Value evaluate(const QString&);
  Value evaluateShort(const QString&);
};

} // namespace KSpread

#endif // KSPREAD_TEST_FINANCIAL_FUNCTIONS
