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

#include "kspread_scripting.h"

namespace kspread_scripting_LNS
{
  KSpreadScripter scripter;
  KSpreadScripter * g_scriptEngine = &scripter;
}

using namespace kspread_scripting_LNS;

KSpreadScripter::KSpreadScripter()
{
}

KSpreadScripter * KSpreadScripting::engine()
{
  return g_scriptEngine;
}

void KSpreadScripting::setEngine( KSpreadScripter * engine )
{
  if ( engine == 0 )
    g_scriptEngine = &scripter;
  else
    g_scriptEngine = engine;
}

void KSpreadScripting::setAction( KSpreadScripting::Action action, QString const & data )
{
  g_scriptEngine->setAction( action, data );
}

void KSpreadScripting::setAction( Action action, int data )
{
  g_scriptEngine->setAction( action, data );
}

QStringList KSpreadScripting::functions()
{
  return g_scriptEngine->functions();
}

bool KSpreadScripting::evaluate( QString const & functionName )
{
  return g_scriptEngine->evaluate( functionName );
}

