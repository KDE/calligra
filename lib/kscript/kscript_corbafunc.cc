#if 0

#define WITH_CORBA
#include <CORBA.h>

#include "kscript_corbafunc.h"
#include "kscript_util.h"
#include "kscript_struct.h"
#include "kscript_proxy.h"
#include "kscript_types.h"
#include "kscript.h"
#include "kscript_parsenode.h"

#include <klocale.h>

/**********************************
 *
 * Marshalling and Demarshalling
 *
 **********************************/

/**
 * Unpack an Any in a KSValue
 */
bool ksUnpack( KSContext& context, KSValue* _arg, CORBA::Any& _any, CORBA::TypeCode_ptr _tc )
{
  switch( _tc->kind() )
  {
  case CORBA::tk_void:
    _arg->clear();
    return true;
  case CORBA::tk_any:
    {
      // TODO: CORBA::Any support
      /* PyObject* pany = pm_module->newAny();
      if ( pany == 0L )
      {
	cerr << "ERROR: Could not create a new any" << endl;
	return 0L;
      }
      int id = g_id++;
      CORBA::Any* any = new CORBA::Any();
      _any >>= *any;
      g_mapAny[ id ] = any;
      PyObject* p_id = Py_BuildValue( "i", id );
      PyObject_SetAttrString( pany, "_any_id_", p_id );
      Py_DECREF( p_id );
      return pany; */
      return false;
    }
  case CORBA::tk_enum:
    {
      CORBA::ULong i;
      assert( _any.enum_get( i ) );
      _arg->setValue( QString( _tc->member_name( i ) ) );
      return true;
    }
  case CORBA::tk_string:
    {
      char* text;
      bool res = ( _any >>= CORBA::Any::to_string( text, 0 ) );
      ASSERT( res );
      _arg->setValue( QString( text ) );
      return true;
    }
  case CORBA::tk_short:
    {
      CORBA::Short x;
      bool erg = ( _any >>= x );
      ASSERT( erg );
      _arg->setValue( (KScript::Long)x );
      return true;
    }
  case CORBA::tk_ushort:
    {
      CORBA::UShort x;
      bool erg = ( _any >>= x );
      ASSERT( erg );
      _arg->setValue( (KScript::Long)x );
      return true;
    }
  case CORBA::tk_long:
    {
      CORBA::Long x;
      bool erg = ( _any >>= x );
      ASSERT( erg );
      _arg->setValue( (KScript::Long)x );
      return true;
    }
  case CORBA::tk_ulong:
    {
      CORBA::ULong x;
      bool erg = ( _any >>= x );
      ASSERT( erg );
      _arg->setValue( (KScript::Long)x );
      return true;
    }
  case CORBA::tk_float:
    {
      CORBA::Float x;
      bool erg = ( _any >>= x );
      ASSERT( erg );
      _arg->setValue( (KScript::Double)x );
      return true;
    }
  case CORBA::tk_double:
    {
      CORBA::Double x;
      bool erg = ( _any >>= x );
      ASSERT( erg );
      _arg->setValue( (KScript::Double)x );
      return true;
    }
  case CORBA::tk_objref:
    {
      CORBA::Object_ptr x;
      if ( !( _any >>= CORBA::Any::to_object( x ) ) )
	return 0L;

      if ( CORBA::is_nil( x ) )
      {
	_arg->clear();
	return true;
      }

      if ( !x->_is_a( _tc->id() ) )
      {
	QString tmp( i18n("While unpacking a CORBA datatype, an object of type %1 was expected, but %2 was found") );
	context.setException( new KSException( "UnpackingError", tmp.arg( _tc->id() ).arg( x->_repoid() ) ) );
	return false;
      }

      // We try to instantiate the most specialized interface.
      // Please mention that C++-Corba does not do that, but since we
      // are dynamically typed ...
      KSValue *c = context.interpreter()->repoidImplementation( x->_repoid() );
      if ( !c )
	c = context.interpreter()->repoidImplementation( _tc->id() );

      if ( !c || !c->type() == KSValue::InterfaceType )
      {
	QString tmp(i18n( "Did not find an interface for the repoid %1. Seems to be an error of the idl compiler.") );
	context.setException( new KSException( "UnpackingError", tmp.arg( _tc->id() ) ) );
	return false;
      }

      KSProxy* proxy = c->interfaceValue()->constructor( (CORBA::Object*)x );
      ASSERT( proxy );
      _arg->setValue( proxy );
      return true;
    }
  case CORBA::tk_TypeCode:
    {
      CORBA::TypeCode_ptr x;
      bool erg = ( _any >>= x );
      ASSERT( erg );
      _arg->setValue( new KSTypeCode( (CORBA::TypeCode*)x ) );
      return true;
    }
  case CORBA::tk_boolean:
    {
      CORBA::Boolean x;
      bool erg = ( _any >>= CORBA::Any::to_boolean( x ) );
      ASSERT( erg );
      _arg->setValue( (KScript::Boolean)x );
      return true;
    }
  case CORBA::tk_octet:
    {
      CORBA::Octet x;
      bool erg = ( _any >>= CORBA::Any::to_octet( x ) );
      ASSERT( erg );
      _arg->setValue( (KScript::Long)x );
      return true;
    }
  case CORBA::tk_char:
    {
      CORBA::Char x;
      bool erg = ( _any >>= CORBA::Any::to_char( x ) );
      ASSERT( erg );
      char buf[ 2 ];
      buf[0] = x;
      buf[1] = 0;
      _arg->setValue( QString( buf ) );
      return true;
    }
  case CORBA::tk_sequence:
    {
      CORBA::ULong len;
      assert( _any.seq_get_begin( len ) );
      CORBA::TypeCode_var ctc = _tc->content_type();

      QValueList<KSValue::Ptr> lst;

      for( CORBA::ULong i = 0; i < len; i++ )
      {
	KSValue::Ptr v = new KSValue;
	if ( !ksUnpack( context, v, _any, ctc ) )
	  return false;
	lst.append( v );
      }
      assert( _any.seq_get_end() );

      _arg->setValue( lst );
      return true;
    }
  case CORBA::tk_struct:
    {
      KSValue *c = context.interpreter()->repoidImplementation( _tc->id() );

      if ( !c || !c->type() == KSValue::StructClassType )
      {
	QString tmp( i18n("Did not find a struct for the repoid %1. Seems to be an error of the idl compiler.") );
	context.setException( new KSException( "UnpackingError", tmp.arg( _tc->id() ) ) );
	return false;
      }

      KSStruct::Ptr s = c->structClassValue()->constructor();

      CORBA::ULong len = _tc->member_count();
      assert( _any.struct_get_begin() );
      for( CORBA::ULong i = 0; i < len; i++ )
      {
	CORBA::TypeCode_var ntc = _tc->member_type( i );
	KSValue::Ptr m = new KSValue;
	if ( !ksUnpack( context, m, _any, ntc ) )
	  return false;
	if ( !s->setMember( context, _tc->member_name( i ), m ) )
	  return false;
      }
      assert( _any.struct_get_end() );

      s->ref();
      _arg->setValue( &*s );
      return true;
    }
  case CORBA::tk_except:
    {
      KSValue *c = context.interpreter()->repoidImplementation( _tc->id() );

      if ( !c || !c->type() == KSValue::StructClassType )
      {
	QString tmp( i18n("Did not find a struct for the repoid %1. Seems to be an error of the idl compiler.") );
	context.setException( new KSException( "UnpackingError", tmp.arg( _tc->id() ) ) );
	return false;
      }

      KSStruct::Ptr s = c->structClassValue()->constructor();

      CORBA::ULong len = _tc->member_count();
      CORBA::String_var repoid;
      assert( _any.except_get_begin( repoid ) );
      for( CORBA::ULong i = 0; i < len; i++ )
      {
	CORBA::TypeCode_var ntc = _tc->member_type( i );
	KSValue::Ptr m = new KSValue;
	if ( !ksUnpack( context, m, _any, ntc ) )
	  return false;
	if ( !s->setMember( context, _tc->member_name( i ), m ) )
	  return false;
      }
      assert( _any.except_get_end() );

      s->ref();
      _arg->setValue( &*s );
      return true;
    }
  case CORBA::tk_union:
    {
      // TODO: Union
      /*
      cerr << "Extracting Union " << _tc->id() << endl;
      CORBA::TypeCode_var dt = _tc->discriminator_type();
      CORBA::Long defidx = _tc->default_index();

      assert( _any.union_get_begin() );

      PyObject* disc = parseResult( _any, dt );
      bool found = false;
      CORBA::ULong idx;
      if ( disc == 0L )
      {
	cerr << "ERROR: Could not extract discriminator of union " << _tc->id() << endl;
	return 0L;
      }
      for ( idx = 0; idx < _tc->member_count(); idx++ )
      {
	if ( defidx != -1 && (CORBA::ULong) defidx == idx )
	  continue;

	CORBA::Any_var lany = _tc->member_label( idx );
	PyObject* label = parseResult( *lany, dt );
	if ( label == 0L )
	{
	  cerr << "ERROR: Could not extract label of union " << _tc->id() << endl;
	  return 0L;
	}
	if ( PyObject_Compare( label, disc ) == 0 )
	{
	  found = true;
	  Py_DECREF( label );
	  break;
	}
	Py_DECREF( label );
      }

      PyObject* uni = 0L;

      if ( found )
      {
	CORBA::TypeCode_var mtype = _tc->member_type( idx );
	assert( _any.union_get_selection( idx ) );
	PyObject* memb = parseResult( _any, mtype );
	if ( memb == 0L )
	{
	  cerr << "ERROR: Could not extract value of union " << _tc->id() << endl;
	  return 0L;
	}
	uni = pm_module->newUnion();
	assert( uni != 0L );
	PyObject_SetAttrString( uni, "d", disc );
	PyObject_SetAttrString( uni, "v", memb );
      }
      else if ( defidx == -1 )
      {
	uni = pm_module->newUnion();
	assert( uni != 0L );
	PyObject_SetAttrString( uni, "d", Py_None );
	PyObject_SetAttrString( uni, "v", Py_None );
      }
      else
      {
	assert( defidx != -1 );
	CORBA::TypeCode_var deftype = _tc->member_type( (CORBA::ULong) defidx);
	assert( _any.union_get_selection( defidx ) );
	uni = pm_module->newUnion();
	assert( uni != 0L );
	PyObject* memb = parseResult( _any, deftype );
	if ( memb == 0L )
	{
	  cerr << "ERROR: Could not extract value of union " << _tc->id() << " using default discriminator" << endl;
	  return 0L;
	}
	PyObject_SetAttrString( uni, "d", Py_None );
	PyObject_SetAttrString( uni, "v", memb );
      }

      assert( _any.union_get_end() );

      return uni; */
    }
  case CORBA::tk_alias:
    {
      const CORBA::TypeCode_var ntc = _tc->content_type();
      return ksUnpack( context, _arg, _any, ntc );
    }
  default:
    printf( "INTERNAL ERROR: Did not handle tk_XXXX=%i", (int)_tc->kind() );
    ASSERT( 0 );
  }

  return false;
}

/**
 * Pack a KScript value into a Any
 */
bool ksPack( KSContext& context, CORBA::Any& _any, KSValue* _arg, CORBA::TypeCode_ptr _tc )
{
  switch( _tc->kind() )
  {
  case CORBA::tk_void:
    return true;
  case CORBA::tk_any:
    {
      // TODO: Need any support
      // _any <<= *(it->second);
      return true;
    }
  case CORBA::tk_enum:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::StringType ) )
	return false;

      const char* value = _arg->stringValue().ascii();
      for ( CORBA::ULong i = 0; i < _tc->member_count(); i++ )
      {
	if ( strcmp( value, (char *) _tc->member_name (i) ) == 0 )
	{
	  assert( _any.enum_put( i ) );
	  return true;
	}
      }

      QString tmp( i18n("The value %1 is not a known enumerator\nPossible values are:\n") );
      for ( CORBA::ULong i = 0; i < _tc->member_count(); i++ )
      {
	tmp += _tc->member_name(i);
	tmp += " ";
      }
      context.setException( new KSException( "UnknownEnumerator", tmp.arg( value ) ) );
      return false;
    }
  case CORBA::tk_string:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::StringType ) )
	return false;
      _any <<= CORBA::Any::from_string( (char *) _arg->stringValue().ascii(), 0 );
      return true;
    }
  case CORBA::tk_short:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::IntType ) )
	return false;
      CORBA::Short x = _arg->intValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_ushort:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::IntType ) )
	return false;
      CORBA::UShort x = _arg->intValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_long:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::IntType ) )
	return false;
      CORBA::Long x = _arg->intValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_longlong:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::IntType ) )
	return false;
      CORBA::LongLong x = _arg->intValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_ulong:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::IntType ) )
	return false;
      CORBA::ULong x = _arg->intValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_double:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::DoubleType ) )
	return false;
      CORBA::Double x = _arg->doubleValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_float:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::DoubleType ) )
	return false;
      CORBA::Float x = _arg->doubleValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_longdouble:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::DoubleType ) )
	return false;
      CORBA::LongDouble x = _arg->doubleValue();
      _any <<= x;
      return true;
    }
  case CORBA::tk_boolean:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::BoolType ) )
	return false;
      CORBA::Boolean x = _arg->boolValue();
      _any <<= CORBA::Any::from_boolean( x );
      return true;
    }
  case CORBA::tk_char:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::StringType ) )
	return false;
      QString value = _arg->stringValue();
      if ( value.length() != 1 )
      {
	KSUtil::castingError( context, "multi-character String", "Character" );
	return false;
      }
      CORBA::Char x = *value.ascii();
      _any <<= CORBA::Any::from_char( x );
      return true;
    }
  case CORBA::tk_octet:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::IntType ) )
	return false;
      CORBA::Octet x = _arg->intValue();
      _any <<= CORBA::Any::from_octet( x );
      return true;
    }
  case CORBA::tk_TypeCode:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::TypeCodeType ) )
	return false;
      _any <<= (CORBA::TypeCode*)_arg->typeCodeValue()->tc();
      return true;
    }
  case CORBA::tk_objref:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::ProxyType ) )
	return false;
      CORBA::Object_ptr obj = (CORBA::Object*)_arg->proxyValue()->object();
      // TODO: Handle None here
      // if ( _arg == Py_None )
      // {
      // _any <<= CORBA::Any::from_object( obj, _tc->name() );
      // return true;
      //}

      if ( !obj->_is_a( _tc->id() ) )
      {
	KSUtil::castingError( context, obj->_repoid(), _tc->id() );
	return false;
      }
      _any <<= CORBA::Any::from_object( obj, _tc->name() );
      return true;
    }
  case CORBA::tk_sequence:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::ListType ) )
	return false;

      const CORBA::TypeCode_var ctc = _tc->content_type();

      CORBA::ULong len = _arg->listValue().count();
      assert( _any.seq_put_begin( len ) );
      QValueList<KSValue::Ptr>::Iterator it = _arg->listValue().begin();
      for ( CORBA::ULong i = 0; i < len; i++ )
      {
	if ( !ksPack( context, _any, *it, ctc ) )
	  return false;
      }
      assert( _any.seq_put_end() );
      return true;
    }
  case CORBA::tk_struct:
    {
      if ( !KSUtil::checkType( context, _arg, KSValue::StructType ) )
	return false;

      assert( _any.struct_put_begin() );
      CORBA::ULong len = _tc->member_count();
      for( CORBA::ULong i = 0; i < len; i++ )
      {
	const CORBA::TypeCode_var ntc = _tc->member_type( i );
	KSValue::Ptr v = _arg->structValue()->member( context, _tc->member_name( i ) );
	if ( !v )
	{
	  QString tmp( i18n("The struct misses the member %1") );
	  context.setException( new KSException( "MissingMember", tmp.arg( _tc->member_name( i ) ) ) );
	  return false;
	}
	if ( !ksPack( context, _any, v, ntc ) )
	  return false;
      }
      assert( _any.struct_put_end() );
      return true;
    }
  case CORBA::tk_union:
  {
    // TODO
    return false;
      /*
    cerr << "Packing Union " << _tc->id() << endl;

    CORBA::TypeCode_var dt = _tc->discriminator_type();
    CORBA::Long defidx = _tc->default_index();
    assert( _any.union_put_begin() );

    bool found = false;
    CORBA::ULong idx;
    PyObject* disc = PyObject_GetAttrString( _arg, "d" );
    if ( disc == 0L || disc == Py_None )
    {
      cerr << "ERROR: Can not pack union " << _tc->id() << " because the descriminator is not set" << endl;
      return 0L;
    }
    for ( idx = 0; idx < _tc->member_count(); idx++ )
    {
      if ( defidx != -1 && (CORBA::ULong) defidx == idx )
	continue;

      CORBA::Any_var lany = _tc->member_label( idx );
      PyObject* label = parseResult( *lany, dt );
      if ( label == 0L )
      {
	cerr << "ERROR: Could not extract label of union " << _tc->id() << endl;
	return 0L;
      }
      if ( PyObject_Compare( label, disc ) == 0 )
      {
	found = true;
	Py_DECREF( label );
	break;
      }
      Py_DECREF( label );
    }

    if ( !parseResult( _any, disc, dt ) )
    {
      cerr << "ERROR: Could not pack discriminator for union " << _tc->id() << endl;
      return 0L;
    }

    PyObject* value = PyObject_GetAttrString( _arg, "v" );

    if ( found )
    {
      CORBA::TypeCode_var mtype = _tc->member_type( idx );
      assert( _any.union_put_selection( idx ) );
      if ( !parseResult( _any, value, mtype ) )
      {
	cerr << "ERROR: Could not pack value for union " << _tc->id() << endl;
	return 0L;
      }
    }
    else if ( defidx == -1 )
    {
      // Do not pack any value at all
    }
    else
    {
      CORBA::TypeCode_var mtype = _tc->member_type( idx );
      assert( _any.union_put_selection( idx ) );
      if ( !parseResult( _any, value, mtype ) )
      {
	cerr << "ERROR: Could not pack value for union " << _tc->id() << endl;
	return 0L;
      }
    }

    assert( _any.union_put_end() );

    return true; */
  }
  case CORBA::tk_alias:
  {
    const CORBA::TypeCode_var ntc = _tc->content_type();
    return ksPack( context, _any, _arg, ntc );
  }
  /**
   * Not handled here ...
   * tk_array, tk_alias, tk_except,
   */
  /* if ( _t == CORBA::_tc_wchar )
  if ( _t == CORBA::_tc_wstring )
  if ( _t == CORBA::_tc_Principal )
  if ( _t == CORBA::_tc_Context ) */
  default:
    assert( 0 );
  }
}

/**************************************
 *
 * KSCorbaFunc
 *
 **************************************/

KSCorbaFunc::KSCorbaFunc( KSModule* m, KSParseNode* node )
  : KSFunction( m ), m_node( node )
{
  m_name = node->getIdent();
}

bool KSCorbaFunc::init( KSContext& context )
{
  // Did we already initialize ?
  if ( !m_node )
    return true;

  KSContext d( context );

  // Get the return type
  ASSERT( m_node->branch1() );
  if ( !m_node->branch1()->eval( d ) )
  {
    context.setException( d );
    return false;
  }

  KSTypeCode::Ptr tc = KSTypeCode::typeCode( context, d.value() );
  if ( !tc )
    return false;

  setReturnTypeCode( tc );

  ref();
  d.setValue( new KSValue( this ) );

  ASSERT( d.value() && d.value()->type() == KSValue::FunctionType );
  // Get the list of parameters
  if ( m_node->branch2() )
  {
    if ( !m_node->branch2()->eval( d ) )
    {
      context.setException( d );
      return false;
    }
  }

  // Get the list of exceptions
  if ( m_node->branch3() )
  {
    if ( !m_node->branch3()->eval( d ) )
    {
      context.setException( d );
      return false;
    }
  }

  // Save memory since the code is no longer needed
  m_node->clear();
  m_node = 0;

  return true;
}

bool KSCorbaFunc::call( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QValueList<KSValue::Ptr>::Iterator it = args.begin();

  if ( !init( context) )
    return false;

  uint params = m_parameters.count();
  // "params+1" because the object itself is passes as first parameter
  if ( !KSUtil::checkArgumentsCount( context, params + 1, m_name ) )
    return false;

  // Find the object
  if ( !KSUtil::checkType( context, *it, KSValue::ProxyType ) )
    return false;

  KSProxy* proxy = (*it)->proxyValue();
  CORBA::Object_ptr obj = (CORBA::Object*)proxy->object();
  ++it;

  CORBA::Request_var req = obj->_request( m_name );
  req->result()->value()->type( (CORBA::TypeCode*)m_returnTypeCode->tc() );

  // Pack the argument
  int i = 0;
  QValueList<Parameter>::Iterator pit = m_parameters.begin();
  for( ; pit != m_parameters.end(); ++pit, ++it, ++i )
  {
    CORBA::Any any( (CORBA::TypeCode*)(*pit).typecode->tc(), (void*)NULL );
    if ( !ksPack( context, any, *it, (CORBA::TypeCode*)(*pit).typecode->tc() ) )
      return false;
    if ( (*pit).mode == T_IN )
      req->add_in_arg( (*pit).name.ascii() ) = any;
    else if ( (*pit).mode == T_INOUT )
      req->add_inout_arg( (*pit).name.ascii() ) = any;
    else if ( (*pit).mode == T_OUT )
    {
      req->add_out_arg( (*pit).name.ascii() );
      req->arguments()->item( i )->value()->type( (CORBA::TypeCode*)(*pit).typecode->tc() );
    }
  }

  // Invoke the method
  req->invoke();

  // Check for exceptions
  if ( req->env()->exception() )
  {
    CORBA::Exception *_ex = req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_downcast( _ex );
    if( _uuex )
    {
      KSValue* s = context.interpreter()->repoidImplementation( _uuex->_except_repoid() );
      if ( s && s->type() == KSValue::StructClassType )
      {
	KSTypeCode::Ptr tc = KSTypeCode::typeCode( context, s );
	if ( !tc )
	  return false;

	KSValue::Ptr v = new KSValue;
	// Unpack the exception
	if ( !ksUnpack( context, v, _uuex->exception( (CORBA::TypeCode*)tc->tc() ), (CORBA::TypeCode*)tc->tc() ) )
	  return false;

	// Emit a custom exception
	s->ref();
	context.setException( new KSException( s, v ) );
	return false;
      }
    }

    // Raise a default excpetion
    QString tmp( i18n("An unexpected CORBA exception occured\n%1") );
    context.setException( new KSException( "CORBAException", tmp.arg( _ex->_repoid() ), -1 ) );
    return false;
  }

  // Unpack out/inout parameters
  i = 0;
  // Go to first parameter
  it = args.begin();
  ++it;
  pit = m_parameters.begin();
  for( ; pit != m_parameters.end(); ++pit, ++it, ++i )
  {
    if ( (*pit).mode == T_INOUT || (*pit).mode == T_OUT )
    {
      if ( !ksUnpack( context, *it, *req->arguments()->item( i )->value(), (CORBA::TypeCode*)(*pit).typecode->tc() ) )
	return false;
    }
  }

  // Unpack the return value
  KSValue::Ptr v = new KSValue;
  if ( !ksUnpack( context, v, *req->result()->value(), (CORBA::TypeCode*)m_returnTypeCode->tc() ) )
    return false;
  context.setValue( v );

  printf("YEAH, back from CORBA\n");

  return true;
}

void KSCorbaFunc::addParameter( ParameterMode m, const QString& name, const KSTypeCode::Ptr &tc )
{
  Parameter p;
  p.mode = m;
  p.name = name;
  p.typecode = tc;
  m_parameters.append( p );
}

void KSCorbaFunc::addException( const KSValue::Ptr& exc )
{
  m_exceptions.append( exc );
}

void KSCorbaFunc::setReturnTypeCode( const KSTypeCode::Ptr& tc )
{
  m_returnTypeCode = tc;
}

#endif
