/* This file is part of the KDE libraries
    Copyright (C) 2011 David Faure <faure@kde.org>

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

#include <QObject>

#include <QtTest/QtTest>
#include <ksharedconfig.h>

#include <kconfig.h>
#include <kconfiggroup.h>

class KConfigNoKdeHomeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testNoKdeHome();
};


void KConfigNoKdeHomeTest::testNoKdeHome()
{
    const QString xdgConfigHome = QDir::homePath() + "/.kde-unit-test-does-not-exist";
    QDir xdgConfigHomeDir(xdgConfigHome);
    qputenv("XDG_CONFIG_HOME", QFile::encodeName(xdgConfigHome));
    xdgConfigHomeDir.removeRecursively();
    QVERIFY(!QFile::exists(xdgConfigHome));

    // Do what kf5-config does, and ensure kdehome doesn't get created (#233892)
    QVERIFY(!QFile::exists(xdgConfigHome));
    KSharedConfig::openConfig();
    QVERIFY(!QFile::exists(xdgConfigHome));

    // Now try to actually save something, see if it works.
    KConfigGroup group(KSharedConfig::openConfig(), "Group");
    group.writeEntry("Key", "Value");
    group.sync();
    QVERIFY(QFile::exists(xdgConfigHome));
    QVERIFY(QFile::exists(xdgConfigHome + "/kconfignokdehometestrc"));

    // Cleanup
    xdgConfigHomeDir.removeRecursively();

    // Restore XDG_CONFIG_HOME -- only when there were more tests...
    //qputenv("XDG_CONFIG_HOME", QFile::encodeName(m_xdgConfigHome));
}

QTEST_MAIN(KConfigNoKdeHomeTest)

#include "kconfignokdehometest.moc"
