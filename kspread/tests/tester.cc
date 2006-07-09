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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "tester.h"

#include <qstringlist.h>
#include <qtextstream.h>

using namespace KSpread;

Tester::Tester()
{
  testCount = 0;
}

unsigned Tester::count() const
{
  return testCount;
}

QStringList Tester::errors() const
{
  return errorList;
}

unsigned Tester::failed() const
{
  return errorList.count();
}

void Tester::fail( const char *file, int line, const QString& msg )
{
  QString error;
  QTextStream ts( &error, IO_WriteOnly );
  ts << file << "["<< line <<"]:   " << msg;
  errorList.append( error );
}

