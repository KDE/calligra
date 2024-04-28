// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2006 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_ENGINEERING_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_ENGINEERING_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{

class TestEngineeringFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void testBIN2DEC();
    void testBIN2OCT();
    void testBIN2HEX();
    void testCOMPLEX();
    void testCONVERT();
    void testDEC2HEX();
    void testDEC2BIN();
    void testDEC2OCT();
    void testDELTA();
    void testERF();
    void testERFC();
    void testGESTEP();
    void testHEX2BIN();
    void testHEX2DEC();
    void testHEX2OCT();
    void testIMABS();
    void testIMAGINARY();
    void testIMARGUMENT();
    void testIMCONJUGATE();
    void testIMCOS();
    void testIMCOSH();
    void testIMCOT();
    void testIMCSC();
    void testIMCSCH();
    void testIMDIV();
    void testIMEXP();
    void testIMLN();
    void testIMLOG10();
    void testIMLOG2();
    void testIMPOWER();
    void testIMPRODUCT();
    void testIMREAL();
    void testIMSEC();
    void testIMSECH();
    void testIMSIN();
    void testIMSINH();
    void testIMSQRT();
    void testIMSUB();
    void testIMSUM();
    void testIMTAN();
    void testIMTANH();
    void testOCT2BIN();
    void testOCT2DEC();
    void testOCT2HEX();

private:
    Value evaluate(const QString &);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_ENGINEERING_FUNCTIONS
