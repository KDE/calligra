/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include "KIllustrator.h"

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface View
KIllustrator::View::~View()
{
}

KIllustrator::View_ptr KIllustrator::View::_duplicate( View_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KIllustrator::View::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KIllustrator/View:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::View::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KIllustrator::View::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KIllustrator/View:1.0" ) == 0) {
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

KIllustrator::View_ptr KIllustrator::View::_narrow( CORBA::Object_ptr _obj )
{
  KIllustrator::View_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KIllustrator/View:1.0" ))
      return _duplicate( (KIllustrator::View_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KIllustrator::View_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KIllustrator/View:1.0" ) ) {
    _o = new KIllustrator::View_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KIllustrator::View_ptr KIllustrator::View::_nil()
{
  return NULL;
}

KIllustrator::View_stub::~View_stub()
{
}

struct _global_init_KIllustrator_View {
  _global_init_KIllustrator_View()
  {
    if( ::OPParts::View::_narrow_helpers == NULL )
      ::OPParts::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::View::_narrow_helpers->push_back( KIllustrator::View::_narrow_helper2 );
  }
} __global_init_KIllustrator_View;

vector<CORBA::Narrow_proto> *KIllustrator::View::_narrow_helpers;

CORBA::TypeCode_ptr KIllustrator::_tc_View = (new CORBA::TypeCode(
  "010000000e0000002d000000010000001a00000049444c3a4b496c6c7573"
  "747261746f722f566965773a312e30000000050000005669657700" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KIllustrator::View_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "View" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KIllustrator::View_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KIllustrator::View::_nil();
    return TRUE;
  }
  _obj = ::KIllustrator::View::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Document
KIllustrator::Document::~Document()
{
}

KIllustrator::Document_ptr KIllustrator::Document::_duplicate( Document_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KIllustrator::Document::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KIllustrator/Document:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Document::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KIllustrator::Document::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KIllustrator/Document:1.0" ) == 0) {
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

KIllustrator::Document_ptr KIllustrator::Document::_narrow( CORBA::Object_ptr _obj )
{
  KIllustrator::Document_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KIllustrator/Document:1.0" ))
      return _duplicate( (KIllustrator::Document_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KIllustrator::Document_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KIllustrator/Document:1.0" ) ) {
    _o = new KIllustrator::Document_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KIllustrator::Document_ptr KIllustrator::Document::_nil()
{
  return NULL;
}

KIllustrator::Document_stub::~Document_stub()
{
}

struct _global_init_KIllustrator_Document {
  _global_init_KIllustrator_Document()
  {
    if( ::OPParts::Document::_narrow_helpers == NULL )
      ::OPParts::Document::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Document::_narrow_helpers->push_back( KIllustrator::Document::_narrow_helper2 );
  }
} __global_init_KIllustrator_Document;

vector<CORBA::Narrow_proto> *KIllustrator::Document::_narrow_helpers;

CORBA::TypeCode_ptr KIllustrator::_tc_Document = (new CORBA::TypeCode(
  "010000000e00000035000000010000001e00000049444c3a4b496c6c7573"
  "747261746f722f446f63756d656e743a312e3000000009000000446f6375"
  "6d656e7400" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KIllustrator::Document_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Document" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KIllustrator::Document_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KIllustrator::Document::_nil();
    return TRUE;
  }
  _obj = ::KIllustrator::Document::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Factory
KIllustrator::Factory::~Factory()
{
}

KIllustrator::Factory_ptr KIllustrator::Factory::_duplicate( Factory_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KIllustrator::Factory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KIllustrator/Factory:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Factory::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KIllustrator::Factory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KIllustrator/Factory:1.0" ) == 0) {
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

KIllustrator::Factory_ptr KIllustrator::Factory::_narrow( CORBA::Object_ptr _obj )
{
  KIllustrator::Factory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KIllustrator/Factory:1.0" ))
      return _duplicate( (KIllustrator::Factory_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KIllustrator::Factory_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KIllustrator/Factory:1.0" ) ) {
    _o = new KIllustrator::Factory_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KIllustrator::Factory_ptr KIllustrator::Factory::_nil()
{
  return NULL;
}

KIllustrator::Factory_stub::~Factory_stub()
{
}

struct _global_init_KIllustrator_Factory {
  _global_init_KIllustrator_Factory()
  {
    if( ::OPParts::Factory::_narrow_helpers == NULL )
      ::OPParts::Factory::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Factory::_narrow_helpers->push_back( KIllustrator::Factory::_narrow_helper2 );
  }
} __global_init_KIllustrator_Factory;

vector<CORBA::Narrow_proto> *KIllustrator::Factory::_narrow_helpers;

CORBA::TypeCode_ptr KIllustrator::_tc_Factory = (new CORBA::TypeCode(
  "010000000e00000034000000010000001d00000049444c3a4b496c6c7573"
  "747261746f722f466163746f72793a312e30000000000800000046616374"
  "6f727900" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KIllustrator::Factory_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Factory" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KIllustrator::Factory_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KIllustrator::Factory::_nil();
    return TRUE;
  }
  _obj = ::KIllustrator::Factory::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface View
KIllustrator::View_skel::View_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KIllustrator/View:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<View_skel>( this ) );
}

KIllustrator::View_skel::View_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<View_skel>( this ) );
}

KIllustrator::View_skel::~View_skel()
{
}

bool KIllustrator::View_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KIllustrator::View_ptr KIllustrator::View_skel::_this()
{
  return KIllustrator::View::_duplicate( this );
}


// Dynamic Implementation Routine for interface Document
KIllustrator::Document_skel::Document_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KIllustrator/Document:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Document_skel>( this ) );
}

KIllustrator::Document_skel::Document_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Document_skel>( this ) );
}

KIllustrator::Document_skel::~Document_skel()
{
}

bool KIllustrator::Document_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KIllustrator::Document_ptr KIllustrator::Document_skel::_this()
{
  return KIllustrator::Document::_duplicate( this );
}


// Dynamic Implementation Routine for interface Factory
KIllustrator::Factory_skel::Factory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KIllustrator/Factory:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KIllustrator::Factory_skel::Factory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KIllustrator::Factory_skel::~Factory_skel()
{
}

bool KIllustrator::Factory_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KIllustrator::Factory_ptr KIllustrator::Factory_skel::_this()
{
  return KIllustrator::Factory::_duplicate( this );
}

