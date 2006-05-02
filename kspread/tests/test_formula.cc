/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
 * Boston, MA 02110-1301, USA.
*/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>

#include "formula_tester.h"
#include "tester.h"

using namespace KSpread;

void run( Tester* test )
{
  kDebug() << test->name() << endl;
  test->run();
  int failed = test->failed();
  if ( failed )
  {
    kDebug() << test->failed() << " of " << test->count() << " tests failed:" << endl;
    QStringList errors = test->errors();
    QStringList::ConstIterator end = errors.end();
    for (QStringList::ConstIterator it = errors.begin(); it != end; ++it )
    {
      kDebug() << (*it) << endl;
    }
  }
  else
  {
    kDebug() << "All " << test->count() << " tests successfully passed." << endl;
  }
  delete test;
}

int main( int argc, char** argv )
{
  // Initialize command line args
  KCmdLineArgs::init(argc, argv, "formulatest", "formulatest", "unit test", "0.1");
  KApplication app(false);

  run( new FormulaParserTester() );
  run( new FormulaEvalTester() );
  run( new FormulaOasisConversionTester() );
}
