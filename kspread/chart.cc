/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include "chart.h"

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------
CORBA::TypeCode_ptr Chart::_tc_DoubleSeq = (new CORBA::TypeCode(
  "010000001500000044000000010000001800000049444c3a43686172742f"
  "446f75626c655365713a312e30000a000000446f75626c65536571000000"
  "130000000c000000010000000700000000000000" ))->mk_constant();

CORBA::TypeCode_ptr Chart::_tc_StringSeq = (new CORBA::TypeCode(
  "010000001500000048000000010000001800000049444c3a43686172742f"
  "537472696e675365713a312e30000a000000537472696e67536571000000"
  "130000001000000001000000120000000000000000000000" ))->mk_constant();

CORBA::TypeCode_ptr Chart::_tc_Matrix = (new CORBA::TypeCode(
  "010000000f00000074010000010000001500000049444c3a43686172742f"
  "4d61747269783a312e3000000000070000004d6174726978000005000000"
  "08000000636f6c756d6e73000300000005000000726f7773000000000300"
  "000012000000636f6c756d6e4465736372697074696f6e00000015000000"
  "48000000010000001800000049444c3a43686172742f537472696e675365"
  "713a312e30000a000000537472696e675365710000001300000010000000"
  "010000001200000000000000000000000f000000726f7744657363726970"
  "74696f6e00001500000048000000010000001800000049444c3a43686172"
  "742f537472696e675365713a312e30000a000000537472696e6753657100"
  "000013000000100000000100000012000000000000000000000007000000"
  "6d617472697800001500000044000000010000001800000049444c3a4368"
  "6172742f446f75626c655365713a312e30000a000000446f75626c655365"
  "71000000130000000c000000010000000700000000000000" ))->mk_constant();

Chart::Matrix::Matrix()
{
}

Chart::Matrix::Matrix( const Matrix& _s )
{
  columns = ((Matrix&)_s).columns;
  rows = ((Matrix&)_s).rows;
  columnDescription = ((Matrix&)_s).columnDescription;
  rowDescription = ((Matrix&)_s).rowDescription;
  matrix = ((Matrix&)_s).matrix;
}

Chart::Matrix::~Matrix()
{
}

Chart::Matrix&
Chart::Matrix::operator=( const Matrix& _s )
{
  columns = ((Matrix&)_s).columns;
  rows = ((Matrix&)_s).rows;
  columnDescription = ((Matrix&)_s).columnDescription;
  rowDescription = ((Matrix&)_s).rowDescription;
  matrix = ((Matrix&)_s).matrix;
  return *this;
}

CORBA::Boolean operator<<=( CORBA::Any &_a, const Chart::Matrix &_s )
{
  _a.type( Chart::_tc_Matrix );
  return (_a.struct_put_begin() &&
    (_a <<= ((Chart::Matrix&)_s).columns) &&
    (_a <<= ((Chart::Matrix&)_s).rows) &&
    (_a <<= ((Chart::Matrix&)_s).columnDescription) &&
    (_a <<= ((Chart::Matrix&)_s).rowDescription) &&
    (_a <<= ((Chart::Matrix&)_s).matrix) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Chart::Matrix &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.columns) &&
    (_a >>= _s.rows) &&
    (_a >>= _s.columnDescription) &&
    (_a >>= _s.rowDescription) &&
    (_a >>= _s.matrix) &&
    _a.struct_get_end() );
}

CORBA::TypeCode_ptr Chart::_tc_Range = (new CORBA::TypeCode(
  "010000000f0000007c000000010000001400000049444c3a43686172742f"
  "52616e67653a312e30000600000052616e67650000000500000004000000"
  "746f700003000000050000006c656674000000000300000007000000626f"
  "74746f6d0000030000000600000072696768740000000300000006000000"
  "7461626c650000001200000000000000" ))->mk_constant();

Chart::Range::Range()
{
}

Chart::Range::Range( const Range& _s )
{
  top = ((Range&)_s).top;
  left = ((Range&)_s).left;
  bottom = ((Range&)_s).bottom;
  right = ((Range&)_s).right;
  table = ((Range&)_s).table;
}

Chart::Range::~Range()
{
}

Chart::Range&
Chart::Range::operator=( const Range& _s )
{
  top = ((Range&)_s).top;
  left = ((Range&)_s).left;
  bottom = ((Range&)_s).bottom;
  right = ((Range&)_s).right;
  table = ((Range&)_s).table;
  return *this;
}

CORBA::Boolean operator<<=( CORBA::Any &_a, const Chart::Range &_s )
{
  _a.type( Chart::_tc_Range );
  return (_a.struct_put_begin() &&
    (_a <<= ((Chart::Range&)_s).top) &&
    (_a <<= ((Chart::Range&)_s).left) &&
    (_a <<= ((Chart::Range&)_s).bottom) &&
    (_a <<= ((Chart::Range&)_s).right) &&
    (_a <<= ((Chart::Range&)_s).table) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Chart::Range &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.top) &&
    (_a >>= _s.left) &&
    (_a >>= _s.bottom) &&
    (_a >>= _s.right) &&
    (_a >>= _s.table) &&
    _a.struct_get_end() );
}


// Stub interface SimpleChart
Chart::SimpleChart::~SimpleChart()
{
}

Chart::SimpleChart_ptr Chart::SimpleChart::_duplicate( SimpleChart_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *Chart::SimpleChart::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:Chart/SimpleChart:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool Chart::SimpleChart::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:Chart/SimpleChart:1.0" ) == 0) {
    return true;
  }
  for( vector<CORBA::Narrow_proto>::size_type _i = 0;
       _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

Chart::SimpleChart_ptr Chart::SimpleChart::_narrow( CORBA::Object_ptr _obj )
{
  Chart::SimpleChart_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:Chart/SimpleChart:1.0" ))
      return _duplicate( (Chart::SimpleChart_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new Chart::SimpleChart_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:Chart/SimpleChart:1.0" ) ) {
    _o = new Chart::SimpleChart_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

Chart::SimpleChart_ptr Chart::SimpleChart::_nil()
{
  return NULL;
}

Chart::SimpleChart_stub::~SimpleChart_stub()
{
}

void Chart::SimpleChart_stub::fill( const Chart::Range& range, const Chart::Matrix& matrix )
{
  CORBA::Request_var _req = this->_request( "fill" );
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


vector<CORBA::Narrow_proto> *Chart::SimpleChart::_narrow_helpers;

CORBA::TypeCode_ptr Chart::_tc_SimpleChart = (new CORBA::TypeCode(
  "010000000e00000034000000010000001a00000049444c3a43686172742f"
  "53696d706c6543686172743a312e300000000c00000053696d706c654368"
  "61727400" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const Chart::SimpleChart_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "SimpleChart" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, Chart::SimpleChart_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::Chart::SimpleChart::_nil();
    return TRUE;
  }
  _obj = ::Chart::SimpleChart::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface SimpleChart
Chart::SimpleChart_skel::SimpleChart_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Chart/SimpleChart:1.0", "SimpleChart" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:Chart/SimpleChart:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<SimpleChart_skel>( this ) );
}

Chart::SimpleChart_skel::SimpleChart_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Chart/SimpleChart:1.0", "SimpleChart" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<SimpleChart_skel>( this ) );
}

Chart::SimpleChart_skel::~SimpleChart_skel()
{
}

bool Chart::SimpleChart_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "fill" ) == 0 ) {
    Chart::Range range;
    Chart::Matrix matrix;

    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 2, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( Chart::_tc_Range );
    _args->add( CORBA::ARG_IN );
    _args->item( 1 )->value()->type( Chart::_tc_Matrix );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= range;
    *_args->item( 1 )->value() >>= matrix;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      fill( range, matrix );
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

Chart::SimpleChart_ptr Chart::SimpleChart_skel::_this()
{
  return Chart::SimpleChart::_duplicate( this );
}

