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
