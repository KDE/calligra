/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)

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

#include <QtTest/QtTestGui>
#include "kconfigguitest.h"

#include <kconfig.h>
#include <QDir>
#include <QFont>
#include <kconfiggroup.h>
#include <kconfigskeleton.h>

QTEST_MAIN( KConfigTest )

#define COLORENTRY1 QColor("steelblue")
#define COLORENTRY2 QColor(235, 235, 100, 125)
#define COLORENTRY3 QColor(234, 234, 127)
#define FONTENTRY QFont("Times", 16, QFont::Normal)

void KConfigTest::initTestCase()
{
  // cheat the linker on windows to link against kconfiggui
  KConfigSkeleton foo;
  Q_UNUSED(foo);

  KConfig sc( "kconfigtest" );

  KConfigGroup cg(&sc, "ComplexTypes");
  cg.writeEntry( "colorEntry1", COLORENTRY1 );
  cg.writeEntry( "colorEntry2", COLORENTRY2 );
  cg.writeEntry( "colorEntry3", (QList<int>() << 234 << 234 << 127));
  cg.writeEntry( "colorEntry4",  (QList<int>() << 235 << 235 << 100 << 125));
  cg.writeEntry( "fontEntry", FONTENTRY );
  QVERIFY(sc.sync());

  KConfig sc1("kdebugrc");
  KConfigGroup sg0(&sc1, "0");
  sg0.writeEntry("AbortFatal", false);
  sg0.writeEntry("WarnOutput", 0);
  sg0.writeEntry("FatalOutput", 0);
  QVERIFY(sc1.sync());
}

void KConfigTest::cleanupTestCase()
{
  QDir local(QDir::homePath() + "/.kde-unit-test/share/config");

  Q_FOREACH(const QString &file, local.entryList(QDir::Files))
    if(!local.remove(file))
      qWarning("%s: removing failed", qPrintable( file ));

  QCOMPARE((int)local.entryList(QDir::Files).count(), 0);

  local.cdUp();
  local.rmpath("config");
}

void KConfigTest::testComplex()
{
  KConfig sc2( "kconfigtest" );
  KConfigGroup sc3(&sc2, "ComplexTypes");

  QCOMPARE( QVariant(sc3.readEntry( "colorEntry1", QColor(Qt::black) )).toString(),
            QVariant(COLORENTRY1).toString() );
  QCOMPARE( sc3.readEntry( "colorEntry1", QColor() ), COLORENTRY1 );
  QCOMPARE( sc3.readEntry( "colorEntry2", QColor() ), COLORENTRY2 );
  QCOMPARE( sc3.readEntry( "colorEntry3", QColor() ), COLORENTRY3 );
  QCOMPARE( sc3.readEntry( "colorEntry4", QColor() ), COLORENTRY2 );
  QCOMPARE( sc3.readEntry( "defaultColorTest", QColor("black") ), QColor("black") );
  QCOMPARE( sc3.readEntry( "fontEntry", QFont() ), FONTENTRY );
}

void KConfigTest::testInvalid()
{
  KConfig sc( "kconfigtest" );

  // all of these should print a message to the kdebug.dbg file
  KConfigGroup sc3(&sc, "InvalidTypes" );

  QList<int> list;

  // 1 element list
  list << 1;
  sc3.writeEntry( QString("badList"), list);
  QVERIFY(sc.sync());

  QVERIFY( sc3.readEntry( "badList", QColor() ) == QColor() );

  // 2 element list
  list << 2;
  sc3.writeEntry( "badList", list);
  QVERIFY(sc.sync());

  QVERIFY( sc3.readEntry( "badList", QColor() ) == QColor() );

  // 3 element list
  list << 303;
  sc3.writeEntry( "badList", list);
  QVERIFY(sc.sync());

  QVERIFY( sc3.readEntry( "badList", QColor() ) == QColor() ); // out of bounds

  // 4 element list
  list << 4;
  sc3.writeEntry( "badList", list );
  QVERIFY(sc.sync());

  QVERIFY( sc3.readEntry( "badList", QColor() ) == QColor() ); // out of bounds

  list[2] = -3;
  sc3.writeEntry( "badList", list );
  QVERIFY(sc.sync());
  QVERIFY( sc3.readEntry( "badList", QColor() ) == QColor() ); // out of bounds

  // 5 element list
  list[2] = 3;
  list << 5;
  sc3.writeEntry( "badList", list);
  QVERIFY(sc.sync());

  QVERIFY( sc3.readEntry( "badList", QColor() ) == QColor() );

  // 6 element list
  list << 6;
  sc3.writeEntry( "badList", list);
  QVERIFY(sc.sync());

  QVERIFY( sc3.readEntry( "badList", QColor() ) == QColor() );
}
