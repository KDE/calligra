/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_scripting__
#define __kspread_scripting__

#include <qstring.h>
#include <qstringlist.h>

class KSpreadScripter;

class KSpreadScripting
{
 public:
  // add the action the macro recorder supports here..
  enum Action { ActivateSheet, DeleteColumn, DeleteRow, InsertColumn, InsertRow, SetText };

  static KSpreadScripter * engine();
  static void setEngine( KSpreadScripter * );

  // add actions to the macro recorder
  static void setAction( Action action, QString const & data );
  static void setAction( Action action, int data );

  static QStringList functions();
  static bool evaluate( QString const & functionName );

 private:
  KSpreadScripting() {}
};

class KSpreadScripter
{
 public:
  KSpreadScripter();
  virtual ~KSpreadScripter() {}

  virtual void setAction( KSpreadScripting::Action /* action*/, QString const & /*data*/ ) {}
  virtual void setAction( KSpreadScripting::Action /* action*/, int /*data*/ ) {}
  
  virtual QStringList functions() { return QStringList(); }
  virtual bool evaluate( QString const & /*functionName*/ ) { return false; }
};

#endif
