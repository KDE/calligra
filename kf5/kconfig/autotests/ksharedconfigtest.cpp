/*
   This file is part of the KDE libraries
   Copyright (c) 2012 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QtTest>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

class KSharedConfigTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testUnicity();
    void testReadWrite();
    void testReadWriteSync();
private:
    QString m_path;
};

void KSharedConfigTest::initTestCase()
{
    QStandardPaths::enableTestMode(true);

    m_path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/ksharedconfigtestrc";
    QFile::remove(m_path);
}

void KSharedConfigTest::testUnicity()
{
    KSharedConfig::Ptr cfg1 = KSharedConfig::openConfig();
    KSharedConfig::Ptr cfg2 = KSharedConfig::openConfig();
    QCOMPARE(cfg1.data(), cfg2.data());
}

void KSharedConfigTest::testReadWrite()
{
    const int value = 1;
    {
        KConfigGroup cg(KSharedConfig::openConfig(), "KSharedConfigTest");
        cg.writeEntry("NumKey", value);
    }
    {
        KConfigGroup cg(KSharedConfig::openConfig(), "KSharedConfigTest");
        QCOMPARE(cg.readEntry("NumKey", 0), 1);
    }
}

void KSharedConfigTest::testReadWriteSync()
{
    const int value = 1;
    {
        KConfigGroup cg(KSharedConfig::openConfig(), "KSharedConfigTest");
        cg.writeEntry("NumKey", value);
    }
    QVERIFY(!QFile::exists(m_path));
    QVERIFY(KSharedConfig::openConfig()->sync());
    QVERIFY(QFile::exists(m_path));
    {
        KConfigGroup cg(KSharedConfig::openConfig(), "KSharedConfigTest");
        QCOMPARE(cg.readEntry("NumKey", 0), 1);
    }
}

QTEST_MAIN(KSharedConfigTest)

#include "ksharedconfigtest.moc"
