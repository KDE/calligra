/* This file is part of the KDE project
   Copyright (C) 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// KoUserStyle/KoUserStyleCollection test

#include <kunittest/runner.h>
#include <kunittest/module.h>

#include <KoUserStyleCollection.h>
#include <KoUserStyle.h>
#include <kdebug.h>
#include <kglobal.h>

#include "KoUserStyleTester.h"
#include "KoUserStyleTester.moc"

KUNITTEST_MODULE(kunittest_KoUserStyleTester, "KoUserStyle Tester");
KUNITTEST_MODULE_REGISTER_TESTER(KoUserStyleTester);

#undef COMPARE
/// for source-compat with qttestlib: use COMPARE(x,y) if you plan to port to qttestlib later.
#define COMPARE CHECK

/// for source-compat with qttestlib: use VERIFY(x) if you plan to port to qttestlib later.
#undef VERIFY
#define VERIFY( x ) CHECK( x, true )

void KoUserStyleTester::testEmptyCollection()
{
    KoUserStyleCollection coll( "test" );
    VERIFY( coll.isEmpty() );
    COMPARE( coll.count(), 0 );
    VERIFY( coll.styleList().isEmpty() );
}

void KoUserStyleTester::testAddStyle()
{
    KoUserStyleCollection coll( "test" );

    KoUserStyle* style = new KoUserStyle( "test1" );
    COMPARE( style->name(), QString( "test1" ) );
    COMPARE( style->displayName(), QString( "test1" ) );

    KoUserStyle* ret = coll.addStyle( style );
    COMPARE( ret, style );

    KoUserStyle* style2 = new KoUserStyle( "test1" );
    COMPARE( style2->name(), QString( "test1" ) );
    ret = coll.addStyle( style2 );
    // here style2 got deleted.
    COMPARE( ret, style );

    VERIFY( !coll.isEmpty() );
    COMPARE( coll.count(), 1 );
    COMPARE( (int)coll.styleList().count(), 1 );

    QStringList displayNames = coll.displayNameList();
    COMPARE( (int)displayNames.count(), 1 );
    COMPARE( displayNames.first(), style->name() );
}

void KoUserStyleTester::testFindStyle()
{
    KoUserStyleCollection coll( "test" );
    KoUserStyle* style = new KoUserStyle( "test1" );
    coll.addStyle( style );

    KoUserStyle* ret = coll.findStyle( "test1", QString::null );
    COMPARE( ret, style );

    ret = coll.findStyle( "foo", QString::null );
    COMPARE( ret, (KoUserStyle*)0 );

    ret = coll.findStyle( "foo", "test1" ); // fallback not used for style 'foo'
    COMPARE( ret, (KoUserStyle*)0 );

    ret = coll.findStyle( "test1", "test1" ); // fallback used for standard style test1
    COMPARE( ret, style );
}
