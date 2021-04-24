// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_DATABASE_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_DATABASE_FUNCTIONS

#include <QObject>

#include <Value.h>

namespace Calligra
{
namespace Sheets
{
class Map;

class TestDatabaseFunctions: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testDAVERAGE();
    void testDCOUNT();
    void testDCOUNTA();
    void testDGET();
    void testDMAX();
    void testDMIN();
    void testDPRODUCT();
    void testDSTDEV();
    void testDSTDEVP();
    void testDSUM();
    void testDVAR();
    void testDVARP();
    void cleanupTestCase();

private:
    Value evaluate(const QString&, Value& ex);

    Map* m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_DATABASE_FUNCTIONS
