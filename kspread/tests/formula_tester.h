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

#ifndef KSPREAD_FORMULA_TESTER
#define KSPREAD_FORMULA_TESTER

#include <qstring.h>

#include "tester.h"

#include <kspread_value.h>

class KLocale;

namespace KSpread
{

class FormulaParserTester: public Tester
{
public:
  FormulaParserTester(KLocale *locale);
  virtual QString name();
  virtual void run();
private:
  void checkParse( const char *file, int line, const char* msg, const QString&, const QString&, KLocale* );
};

class FormulaEvalTester: public Tester
{
public:
  FormulaEvalTester(KLocale *locale);
  virtual QString name();
  virtual void run();
private:
  void checkEval( const char *file, int line, const char* msg, const QString&, const KSpreadValue&, KLocale* );
};

} // namespace KSpread

#endif // KSPREAD_FORMULA_TESTER
