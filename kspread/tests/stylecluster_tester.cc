/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "tester.h"
#include "stylecluster_tester.h"

#include <kspread_value.h>


#define CHECK_PARSE(x,y)  checkParse(__FILE__,__LINE__,#x,x,y,t_locale)
#define CHECK_EVAL(x,y)  checkEval(__FILE__,__LINE__,#x,x,y,t_locale)

using namespace KSpread;

StyleClusterTester::StyleClusterTester(): Tester()
{
}

QString StyleClusterTester::name()
{
  return QString("Stylecluster");
}

void StyleClusterTester::run()
{
  testCount = 0;
  errorList.clear();

  // simple, single-token formulas
//  CHECK_PARSE( "True", "b" );
}


