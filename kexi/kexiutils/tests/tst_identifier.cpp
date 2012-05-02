/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QTest>
#include "kexiutils/identifier.h"

class tst_identifier : public QObject
{
    Q_OBJECT
private slots:
    void isIdentifier() {
        QVERIFY(!KexiUtils::isIdentifier(""));
        QVERIFY(!KexiUtils::isIdentifier(QString()));
        QVERIFY(!KexiUtils::isIdentifier("\0"));
        QVERIFY(!KexiUtils::isIdentifier(" "));
        QVERIFY(!KexiUtils::isIdentifier("7"));
        QVERIFY(KexiUtils::isIdentifier("_"));
        QVERIFY(KexiUtils::isIdentifier("abc_2"));
        QVERIFY(KexiUtils::isIdentifier("Abc_2"));
        QVERIFY(KexiUtils::isIdentifier("_7"));
    }

    void string2FileName() {
        QCOMPARE("", "");
        QCOMPARE(" \n   \t", "");
        QCOMPARE("", "");
        QCOMPARE(": \\-abc", "-_--abc");
        QCOMPARE(" */$", "--_");
    }
};

QTEST_MAIN(tst_identifier)

#include "tst_identifier.moc"
