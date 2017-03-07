/* This file is part of the KDE project
   Copyright 2007 Brad Hards <bradh@frogmouth.net>
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

#ifndef CALLIGRA_SHEETS_TEST_INFORMATION_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_INFORMATION_FUNCTIONS

#include <QObject>

#include <Value.h>
#include <Cell.h>

namespace Calligra
{
namespace Sheets
{
class Map;
class Cell;

class TestInformationFunctions: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void testAREAS();
    void testCELL();
    void testCOLUMN();
    void testCOLUMNS();
    void testCOUNT();
    void testCOUNTA();
    void testCOUNTBLANK();
    void testCOUNTIF();
    void testERRORTYPE();
    void testFORMULA();
    void testINFO();
    void testISBLANK();
    void testISERR();
    void testISERROR();
    void testISEVEN();
    void testISFORMULA();
    void testISLOGICAL();
    void testISNONTEXT();
    void testISNA();
    void testISNUMBER();
    void testISODD();
    void testISTEXT();
    void testISREF();
    void testMATCH();
    void testN();
    void testNA();
    void testROW();
    void testROWS();
    void testSHEET();
    void testSHEETS();
    void testTYPE();
    void testVALUE();

    void cleanupTestCase();

private:
    Value evaluate(const QString&, Value& ex, const Cell &cell = Cell());

    Map* m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif
