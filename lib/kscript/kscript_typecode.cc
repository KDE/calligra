#if 0

#define WITH_CORBA
#include <CORBA.h>

#include "kscript_typecode.h"
#include "kscript_interface.h"
#include "kscript_struct.h"

KSTypeCode::KSTypeCode()
{
  m_typecode = new CORBA::TypeCode();
}

KSTypeCode::KSTypeCode( void* tc )
{
  m_typecode = (CORBA::TypeCode*)tc;
  m_typecode->_ref();
}

KSTypeCode::KSTypeCode( const QString& stringified )
{
  m_typecode = new CORBA::TypeCode( stringified.ascii() );
}

KSTypeCode::~KSTypeCode()
{
  m_typecode->_deref();
}

bool KSTypeCode::fromString( const QString& stringified )
{
  return m_typecode->from_string( stringified.ascii() );
}

void* KSTypeCode::tc()
{
  return m_typecode;
}

bool KSTypeCode::convertToTypeCode( KSContext& context, KSValue* v )
{
  if ( v->type() == KSValue::TypeCodeType )
    return true;

  if ( v->type() != KSValue::StringType )
    return false;

  QString tc = v->stringValue();
  v->setValue( new KSTypeCode() );
  if ( !v->typeCodeValue()->fromString( tc ) )
  {
    context.setException( new KSException( "InvalidTypeCode", tc, -1 ) );
    return false;
  }
  return true;
}

KSTypeCode::Ptr KSTypeCode::typeCode( KSContext& context, KSValue* v )
{
  if( v->type() == KSValue::TypeCodeType )
  {
    v->typeCodeValue()->ref();
    return v->typeCodeValue();
  }
  else if( v->type() == KSValue::StringType )
  {
    if ( !convertToTypeCode( context, v ) )
      return 0;
    KSTypeCode* tc = v->typeCodeValue();
    tc->ref();
    return tc;
  }
  else if( v->type() == KSValue::StructClassType )
  {
    KSValue::Ptr p = v->structClassValue()->member( context, "typecode" );
    if ( !p )
      return 0;
    if ( !convertToTypeCode( context, p ) )
      return 0;
    KSTypeCode* tc = p->typeCodeValue();
    tc->ref();
    return tc;
  }
  else if( v->type() == KSValue::InterfaceType )
  {
    KSValue::Ptr p = v->interfaceValue()->member( context, "typecode" );
    if ( !p )
      return 0;
    printf("   tc=%s\n",p->toString().ascii());
    if ( !convertToTypeCode( context, p ) )
      return 0;
    KSTypeCode* tc = p->typeCodeValue();
    tc->ref();
    return tc;
  }

  context.setException( new KSException( "InvalidTypeCode", v->toString(), -1 ) );
  return 0;
}

#endif
