/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include "kspread.h"

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface View
KSpread::View::~View()
{
}

KSpread::View_ptr KSpread::View::_duplicate( View_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KSpread::View::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KSpread/View:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::View::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KSpread::View::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KSpread/View:1.0" ) == 0) {
    return true;
  }
  for( int _i = 0; _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

KSpread::View_ptr KSpread::View::_narrow( CORBA::Object_ptr _obj )
{
  KSpread::View_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KSpread/View:1.0" ))
      return _duplicate( (KSpread::View_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KSpread::View_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

KSpread::View_ptr KSpread::View::_nil()
{
  return NULL;
}

KSpread::View_stub::~View_stub()
{
}

void KSpread::View_stub::undo()
{
  CORBA::Request_var _req = this->_request( "undo" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::redo()
{
  CORBA::Request_var _req = this->_request( "redo" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::cutSelection()
{
  CORBA::Request_var _req = this->_request( "cutSelection" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::copySelection()
{
  CORBA::Request_var _req = this->_request( "copySelection" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::paste()
{
  CORBA::Request_var _req = this->_request( "paste" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::editCell()
{
  CORBA::Request_var _req = this->_request( "editCell" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::paperLayoutDlg()
{
  CORBA::Request_var _req = this->_request( "paperLayoutDlg" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::togglePageBorders()
{
  CORBA::Request_var _req = this->_request( "togglePageBorders" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::newView()
{
  CORBA::Request_var _req = this->_request( "newView" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::insertNewTable()
{
  CORBA::Request_var _req = this->_request( "insertNewTable" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::autoFill()
{
  CORBA::Request_var _req = this->_request( "autoFill" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::editGlobalScripts()
{
  CORBA::Request_var _req = this->_request( "editGlobalScripts" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::editLocalScripts()
{
  CORBA::Request_var _req = this->_request( "editLocalScripts" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::reloadScripts()
{
  CORBA::Request_var _req = this->_request( "reloadScripts" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::helpAbout()
{
  CORBA::Request_var _req = this->_request( "helpAbout" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::helpUsing()
{
  CORBA::Request_var _req = this->_request( "helpUsing" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::deleteRow()
{
  CORBA::Request_var _req = this->_request( "deleteRow" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::deleteColumn()
{
  CORBA::Request_var _req = this->_request( "deleteColumn" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::insertRow()
{
  CORBA::Request_var _req = this->_request( "insertRow" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::insertColumn()
{
  CORBA::Request_var _req = this->_request( "insertColumn" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::bold()
{
  CORBA::Request_var _req = this->_request( "bold" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::italic()
{
  CORBA::Request_var _req = this->_request( "italic" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::moneyFormat()
{
  CORBA::Request_var _req = this->_request( "moneyFormat" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::percent()
{
  CORBA::Request_var _req = this->_request( "percent" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::alignLeft()
{
  CORBA::Request_var _req = this->_request( "alignLeft" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::alignCenter()
{
  CORBA::Request_var _req = this->_request( "alignCenter" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::alignRight()
{
  CORBA::Request_var _req = this->_request( "alignRight" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::multiRow()
{
  CORBA::Request_var _req = this->_request( "multiRow" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::precisionMinus()
{
  CORBA::Request_var _req = this->_request( "precisionMinus" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::precisionPlus()
{
  CORBA::Request_var _req = this->_request( "precisionPlus" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KSpread::View_stub::insertChart()
{
  CORBA::Request_var _req = this->_request( "insertChart" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


struct _global_init_KSpread_View {
  _global_init_KSpread_View()
  {
    if( ::OPParts::View::_narrow_helpers == NULL )
      ::OPParts::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::View::_narrow_helpers->push_back( KSpread::View::_narrow_helper2 );
  }
} __global_init_KSpread_View;

vector<CORBA::Narrow_proto> *KSpread::View::_narrow_helpers;

CORBA::TypeCode_ptr KSpread::_tc_View = (new CORBA::TypeCode(
  "010000000e00000029000000010000001500000049444c3a4b5370726561"
  "642f566965773a312e3000000000050000005669657700" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KSpread::View_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "View" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KSpread::View_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KSpread::View::_nil();
    return TRUE;
  }
  _obj = ::KSpread::View::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Document
KSpread::Document::~Document()
{
}

KSpread::Document_ptr KSpread::Document::_duplicate( Document_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KSpread::Document::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KSpread/Document:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Document::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KSpread::Document::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KSpread/Document:1.0" ) == 0) {
    return true;
  }
  for( int _i = 0; _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

KSpread::Document_ptr KSpread::Document::_narrow( CORBA::Object_ptr _obj )
{
  KSpread::Document_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KSpread/Document:1.0" ))
      return _duplicate( (KSpread::Document_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KSpread::Document_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

KSpread::Document_ptr KSpread::Document::_nil()
{
  return NULL;
}

KSpread::Document_stub::~Document_stub()
{
}

struct _global_init_KSpread_Document {
  _global_init_KSpread_Document()
  {
    if( ::OPParts::Document::_narrow_helpers == NULL )
      ::OPParts::Document::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Document::_narrow_helpers->push_back( KSpread::Document::_narrow_helper2 );
  }
} __global_init_KSpread_Document;

vector<CORBA::Narrow_proto> *KSpread::Document::_narrow_helpers;

CORBA::TypeCode_ptr KSpread::_tc_Document = (new CORBA::TypeCode(
  "010000000e00000031000000010000001900000049444c3a4b5370726561"
  "642f446f63756d656e743a312e300000000009000000446f63756d656e74"
  "00" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KSpread::Document_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Document" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KSpread::Document_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KSpread::Document::_nil();
    return TRUE;
  }
  _obj = ::KSpread::Document::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface View
KSpread::View_skel::View_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KSpread/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KSpread/View:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<View_skel>( this ) );
}

KSpread::View_skel::View_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KSpread/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<View_skel>( this ) );
}

KSpread::View_skel::~View_skel()
{
}

bool KSpread::View_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "undo" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    undo();
    return true;
  }
  if( strcmp( _req->op_name(), "redo" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    redo();
    return true;
  }
  if( strcmp( _req->op_name(), "cutSelection" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    cutSelection();
    return true;
  }
  if( strcmp( _req->op_name(), "copySelection" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    copySelection();
    return true;
  }
  if( strcmp( _req->op_name(), "paste" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    paste();
    return true;
  }
  if( strcmp( _req->op_name(), "editCell" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    editCell();
    return true;
  }
  if( strcmp( _req->op_name(), "paperLayoutDlg" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    paperLayoutDlg();
    return true;
  }
  if( strcmp( _req->op_name(), "togglePageBorders" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    togglePageBorders();
    return true;
  }
  if( strcmp( _req->op_name(), "newView" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    newView();
    return true;
  }
  if( strcmp( _req->op_name(), "insertNewTable" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    insertNewTable();
    return true;
  }
  if( strcmp( _req->op_name(), "autoFill" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    autoFill();
    return true;
  }
  if( strcmp( _req->op_name(), "editGlobalScripts" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    editGlobalScripts();
    return true;
  }
  if( strcmp( _req->op_name(), "editLocalScripts" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    editLocalScripts();
    return true;
  }
  if( strcmp( _req->op_name(), "reloadScripts" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    reloadScripts();
    return true;
  }
  if( strcmp( _req->op_name(), "helpAbout" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    helpAbout();
    return true;
  }
  if( strcmp( _req->op_name(), "helpUsing" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    helpUsing();
    return true;
  }
  if( strcmp( _req->op_name(), "deleteRow" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    deleteRow();
    return true;
  }
  if( strcmp( _req->op_name(), "deleteColumn" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    deleteColumn();
    return true;
  }
  if( strcmp( _req->op_name(), "insertRow" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    insertRow();
    return true;
  }
  if( strcmp( _req->op_name(), "insertColumn" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    insertColumn();
    return true;
  }
  if( strcmp( _req->op_name(), "bold" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    bold();
    return true;
  }
  if( strcmp( _req->op_name(), "italic" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    italic();
    return true;
  }
  if( strcmp( _req->op_name(), "moneyFormat" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    moneyFormat();
    return true;
  }
  if( strcmp( _req->op_name(), "percent" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    percent();
    return true;
  }
  if( strcmp( _req->op_name(), "alignLeft" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    alignLeft();
    return true;
  }
  if( strcmp( _req->op_name(), "alignCenter" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    alignCenter();
    return true;
  }
  if( strcmp( _req->op_name(), "alignRight" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    alignRight();
    return true;
  }
  if( strcmp( _req->op_name(), "multiRow" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    multiRow();
    return true;
  }
  if( strcmp( _req->op_name(), "precisionMinus" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    precisionMinus();
    return true;
  }
  if( strcmp( _req->op_name(), "precisionPlus" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    precisionPlus();
    return true;
  }
  if( strcmp( _req->op_name(), "insertChart" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    insertChart();
    return true;
  }
  return false;
}

KSpread::View_ptr KSpread::View_skel::_this()
{
  return KSpread::View::_duplicate( this );
}


// Dynamic Implementation Routine for interface Document
KSpread::Document_skel::Document_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KSpread/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KSpread/Document:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Document_skel>( this ) );
}

KSpread::Document_skel::Document_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KSpread/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Document_skel>( this ) );
}

KSpread::Document_skel::~Document_skel()
{
}

bool KSpread::Document_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KSpread::Document_ptr KSpread::Document_skel::_this()
{
  return KSpread::Document::_duplicate( this );
}

