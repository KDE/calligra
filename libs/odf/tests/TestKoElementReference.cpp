/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestKoElementReference.h"

#include <KoElementReference.h>

#include <QLoggingCategory>
#include <QTest>

void TestKoElementReference::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.store=true");
}

void TestKoElementReference::testElementReference()
{
    KoElementReference ref1;
    KoElementReference ref2;

    QVERIFY(ref1 != ref2);

    KoElementReference ref3(ref1);
    QVERIFY(ref1 == ref3);

    {
        KoElementReference ref4;
        ref3 = ref4;

        QVERIFY(ref3 == ref4);
    }

    QVERIFY(ref3 != ref1);
}

QTEST_MAIN(TestKoElementReference)
