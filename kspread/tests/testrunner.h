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

#ifndef KSPREAD_TEST_RUNNER
#define KSPREAD_TEST_RUNNER

#include <kdialogbase.h>

namespace KSpread
{

class Tester;

class TestRunner : public KDialogBase
{
Q_OBJECT

public:
  TestRunner();
  ~TestRunner();
  
  void addTester( Tester* tester );
  
private slots:
  void runTest();
  
private:
  class Private;
  Private* d;
};

}

#endif // KSPREAD_TEST_RUNNER

