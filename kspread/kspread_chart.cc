/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include "kspread_chart.h"

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------
CORBA::TypeCode_ptr CHART::_tc_DoubleSeq = (new CORBA::TypeCode(
  "010000001500000044000000010000001800000049444c3a43484152542f"
  "446f75626c655365713a312e30000a000000446f75626c65536571000000"
  "130000000c000000010000000700000000000000" ))->mk_constant();

CORBA::TypeCode_ptr CHART::_tc_StringSeq = (new CORBA::TypeCode(
  "010000001500000048000000010000001800000049444c3a43484152542f"
  "537472696e675365713a312e30000a000000537472696e67536571000000"
  "130000001000000001000000120000000000000000000000" ))->mk_constant();

CORBA::TypeCode_ptr CHART::_tc_Matrix = (new CORBA::TypeCode(
  "010000000f00000074010000010000001500000049444c3a43484152542f"
  "4d61747269783a312e3000000000070000004d6174726978000005000000"
  "08000000636f6c756d6e73000300000005000000726f7773000000000300"
  "000012000000636f6c756d6e4465736372697074696f6e00000015000000"
  "48000000010000001800000049444c3a43484152542f537472696e675365"
  "713a312e30000a000000537472696e675365710000001300000010000000"
  "010000001200000000000000000000000f000000726f7744657363726970"
  "74696f6e00001500000048000000010000001800000049444c3a43484152"
  "542f537472696e675365713a312e30000a000000537472696e6753657100"
  "000013000000100000000100000012000000000000000000000007000000"
  "6d617472697800001500000044000000010000001800000049444c3a4348"
  "4152542f446f75626c655365713a312e30000a000000446f75626c655365"
  "71000000130000000c000000010000000700000000000000" ))->mk_constant();

CHART::Matrix::Matrix()
{
}

CHART::Matrix::Matrix( const Matrix& _s )
{
  columns = ((Matrix&)_s).columns;
  rows = ((Matrix&)_s).rows;
  columnDescription = ((Matrix&)_s).columnDescription;
  rowDescription = ((Matrix&)_s).rowDescription;
  matrix = ((Matrix&)_s).matrix;
}

CHART::Matrix::~Matrix()
{
}

CHART::Matrix&
CHART::Matrix::operator=( const Matrix& _s )
{
  columns = ((Matrix&)_s).columns;
  rows = ((Matrix&)_s).rows;
  columnDescription = ((Matrix&)_s).columnDescription;
  rowDescription = ((Matrix&)_s).rowDescription;
  matrix = ((Matrix&)_s).matrix;
  return *this;
}

CORBA::Boolean operator<<=( CORBA::Any &_a, const CHART::Matrix &_s )
{
  _a.type( CHART::_tc_Matrix );
  return (_a.struct_put_begin() &&
    (_a <<= ((CHART::Matrix&)_s).columns) &&
    (_a <<= ((CHART::Matrix&)_s).rows) &&
    (_a <<= ((CHART::Matrix&)_s).columnDescription) &&
    (_a <<= ((CHART::Matrix&)_s).rowDescription) &&
    (_a <<= ((CHART::Matrix&)_s).matrix) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CHART::Matrix &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.columns) &&
    (_a >>= _s.rows) &&
    (_a >>= _s.columnDescription) &&
    (_a >>= _s.rowDescription) &&
    (_a >>= _s.matrix) &&
    _a.struct_get_end() );
}

CORBA::TypeCode_ptr CHART::_tc_Range = (new CORBA::TypeCode(
  "010000000f0000007c000000010000001400000049444c3a43484152542f"
  "52616e67653a312e30000600000052616e67650000000500000004000000"
  "746f700003000000050000006c656674000000000300000007000000626f"
  "74746f6d0000030000000600000072696768740000000300000006000000"
  "7461626c650000001200000000000000" ))->mk_constant();

CHART::Range::Range()
{
}

CHART::Range::Range( const Range& _s )
{
  top = ((Range&)_s).top;
  left = ((Range&)_s).left;
  bottom = ((Range&)_s).bottom;
  right = ((Range&)_s).right;
  table = ((Range&)_s).table;
}

CHART::Range::~Range()
{
}

CHART::Range&
CHART::Range::operator=( const Range& _s )
{
  top = ((Range&)_s).top;
  left = ((Range&)_s).left;
  bottom = ((Range&)_s).bottom;
  right = ((Range&)_s).right;
  table = ((Range&)_s).table;
  return *this;
}

CORBA::Boolean operator<<=( CORBA::Any &_a, const CHART::Range &_s )
{
  _a.type( CHART::_tc_Range );
  return (_a.struct_put_begin() &&
    (_a <<= ((CHART::Range&)_s).top) &&
    (_a <<= ((CHART::Range&)_s).left) &&
    (_a <<= ((CHART::Range&)_s).bottom) &&
    (_a <<= ((CHART::Range&)_s).right) &&
    (_a <<= ((CHART::Range&)_s).table) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CHART::Range &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.top) &&
    (_a >>= _s.left) &&
    (_a >>= _s.bottom) &&
    (_a >>= _s.right) &&
    (_a >>= _s.table) &&
    _a.struct_get_end() );
}


// Stub interface Callback
CHART::Callback::~Callback()
{
}

CHART::Callback_ptr CHART::Callback::_duplicate( Callback_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CHART::Callback::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:CHART/Callback:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CHART::Callback::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:CHART/Callback:1.0" ) == 0) {
    return true;
  }
  for( int _i = 0; _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

CHART::Callback_ptr CHART::Callback::_narrow( CORBA::Object_ptr _obj )
{
  CHART::Callback_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:CHART/Callback:1.0" ))
      return _duplicate( (CHART::Callback_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new CHART::Callback_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

CHART::Callback_ptr CHART::Callback::_nil()
{
  return NULL;
}

CHART::Callback_stub::~Callback_stub()
{
}

CHART::Matrix* CHART::Callback_stub::request( const CHART::Range& range )
{
  CORBA::Request_var _req = this->_request( "request" );
  _req->add_in_arg( "range" ) <<= range;
  _req->result()->value()->type( CHART::_tc_Matrix );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
  CHART::Matrix* _res = new ::CHART::Matrix;
  *_req->result()->value() >>= *_res;
  return _res;
}


vector<CORBA::Narrow_proto> *CHART::Callback::_narrow_helpers;

CORBA::TypeCode_ptr CHART::_tc_Callback = (new CORBA::TypeCode(
  "010000000e0000002d000000010000001700000049444c3a43484152542f"
  "43616c6c6261636b3a312e3000000900000043616c6c6261636b00" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CHART::Callback_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Callback" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CHART::Callback_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CHART::Callback::_nil();
    return TRUE;
  }
  _obj = ::CHART::Callback::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Chart
CHART::Chart::~Chart()
{
}

CHART::Chart_ptr CHART::Chart::_duplicate( Chart_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CHART::Chart::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:CHART/Chart:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CHART::Chart::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:CHART/Chart:1.0" ) == 0) {
    return true;
  }
  for( int _i = 0; _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

CHART::Chart_ptr CHART::Chart::_narrow( CORBA::Object_ptr _obj )
{
  CHART::Chart_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:CHART/Chart:1.0" ))
      return _duplicate( (CHART::Chart_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new CHART::Chart_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

CHART::Chart_ptr CHART::Chart::_nil()
{
  return NULL;
}

CHART::Chart_stub::~Chart_stub()
{
}

void CHART::Chart_stub::fill( const CHART::Range& range, const CHART::Matrix& matrix, CHART::Callback_ptr cb )
{
  CORBA::Request_var _req = this->_request( "fill" );
  _req->add_in_arg( "range" ) <<= range;
  _req->add_in_arg( "matrix" ) <<= matrix;
  _req->add_in_arg( "cb" ) <<= (CHART::Callback_ptr) cb;
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void CHART::Chart_stub::fill_dummy( const CHART::Range& range, const CHART::Matrix& matrix )
{
  CORBA::Request_var _req = this->_request( "fill_dummy" );
  _req->add_in_arg( "range" ) <<= range;
  _req->add_in_arg( "matrix" ) <<= matrix;
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


vector<CORBA::Narrow_proto> *CHART::Chart::_narrow_helpers;

CORBA::TypeCode_ptr CHART::_tc_Chart = (new CORBA::TypeCode(
  "010000000e00000026000000010000001400000049444c3a43484152542f"
  "43686172743a312e300006000000436861727400" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CHART::Chart_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Chart" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CHART::Chart_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CHART::Chart::_nil();
    return TRUE;
  }
  _obj = ::CHART::Chart::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface Callback
CHART::Callback_skel::Callback_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_ptr _impl =
    _find_impl( "IDL:CHART/Callback:1.0", "Callback" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:CHART/Callback:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Callback_skel>( this ) );
}

CHART::Callback_skel::Callback_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_ptr _impl =
    _find_impl( "IDL:CHART/Callback:1.0", "Callback" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Callback_skel>( this ) );
}

CHART::Callback_skel::~Callback_skel()
{
}

bool CHART::Callback_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "request" ) == 0 ) {
    CHART::Range range;

    CORBA::NVList_ptr _args;
    _orb()->create_list( 1, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CHART::_tc_Range );

    _req->params( _args );

    *_args->item( 0 )->value() >>= range;
    CHART::Matrix* _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = request( range );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= *_res;
    delete _res;
    _req->result( _any_res );
    return true;
  }
  return false;
}

CHART::Callback_ptr CHART::Callback_skel::_this()
{
  return CHART::Callback::_duplicate( this );
}


// Dynamic Implementation Routine for interface Chart
CHART::Chart_skel::Chart_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_ptr _impl =
    _find_impl( "IDL:CHART/Chart:1.0", "Chart" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:CHART/Chart:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Chart_skel>( this ) );
}

CHART::Chart_skel::Chart_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_ptr _impl =
    _find_impl( "IDL:CHART/Chart:1.0", "Chart" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Chart_skel>( this ) );
}

CHART::Chart_skel::~Chart_skel()
{
}

bool CHART::Chart_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "fill" ) == 0 ) {
    CHART::Range range;
    CHART::Matrix matrix;
    CHART::Callback_var cb;

    CORBA::NVList_ptr _args;
    _orb()->create_list( 3, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CHART::_tc_Range );
    _args->add( CORBA::ARG_IN );
    _args->item( 1 )->value()->type( CHART::_tc_Matrix );
    _args->add( CORBA::ARG_IN );
    _args->item( 2 )->value()->type( CHART::_tc_Callback );

    _req->params( _args );

    *_args->item( 0 )->value() >>= range;
    *_args->item( 1 )->value() >>= matrix;
    *_args->item( 2 )->value() >>= (CHART::Callback_ptr&) cb;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      fill( range, matrix, cb );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
    }
    #endif
    return true;
  }
  if( strcmp( _req->op_name(), "fill_dummy" ) == 0 ) {
    CHART::Range range;
    CHART::Matrix matrix;

    CORBA::NVList_ptr _args;
    _orb()->create_list( 2, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CHART::_tc_Range );
    _args->add( CORBA::ARG_IN );
    _args->item( 1 )->value()->type( CHART::_tc_Matrix );

    _req->params( _args );

    *_args->item( 0 )->value() >>= range;
    *_args->item( 1 )->value() >>= matrix;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      fill_dummy( range, matrix );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
    }
    #endif
    return true;
  }
  return false;
}

CHART::Chart_ptr CHART::Chart_skel::_this()
{
  return CHART::Chart::_duplicate( this );
}

