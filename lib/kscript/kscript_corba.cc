/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#if 0

#define WITH_CORBA
#include <CORBA.h>

#include "kscript_corba.h"
#include "kscript_typecode.h"

KSModule::Ptr ksCreateModule_Corba( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "corba" );

  module->addObject( "void", new KSValue( new KSTypeCode( CORBA::_tc_void ) ) );
  module->addObject( "short", new KSValue( new KSTypeCode( CORBA::_tc_short ) ) );
  module->addObject( "long", new KSValue( new KSTypeCode( CORBA::_tc_long ) ) );
  module->addObject( "longlong", new KSValue( new KSTypeCode( CORBA::_tc_longlong ) ) );
  module->addObject( "ushort", new KSValue( new KSTypeCode( CORBA::_tc_ushort ) ) );
  module->addObject( "ulong", new KSValue( new KSTypeCode( CORBA::_tc_ulong ) ) );
  module->addObject( "ulonglong", new KSValue( new KSTypeCode( CORBA::_tc_ulonglong ) ) );
  module->addObject( "float", new KSValue( new KSTypeCode( CORBA::_tc_float ) ) );
  module->addObject( "double", new KSValue( new KSTypeCode( CORBA::_tc_double ) ) );
  module->addObject( "longdouble", new KSValue( new KSTypeCode( CORBA::_tc_longdouble ) ) );
  module->addObject( "boolean", new KSValue( new KSTypeCode( CORBA::_tc_boolean ) ) );
  module->addObject( "char", new KSValue( new KSTypeCode( CORBA::_tc_char ) ) );
  module->addObject( "wchar", new KSValue( new KSTypeCode( CORBA::_tc_wchar ) ) );
  module->addObject( "octet", new KSValue( new KSTypeCode( CORBA::_tc_octet ) ) );
  module->addObject( "any", new KSValue( new KSTypeCode( CORBA::_tc_any ) ) );
  module->addObject( "TypeCode", new KSValue( new KSTypeCode( CORBA::_tc_TypeCode ) ) );
  module->addObject( "Principal", new KSValue( new KSTypeCode( CORBA::_tc_Principal ) ) );
  module->addObject( "Object", new KSValue( new KSTypeCode( CORBA::_tc_Object ) ) );
  module->addObject( "string", new KSValue( new KSTypeCode( CORBA::_tc_string ) ) );
  module->addObject( "wstring", new KSValue( new KSTypeCode( CORBA::_tc_wstring ) ) );

  return module;
}

#endif
