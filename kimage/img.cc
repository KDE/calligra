/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include "img.h"

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface ImageView
KImage::ImageView::~ImageView()
{
}

KImage::ImageView_ptr KImage::ImageView::_duplicate( ImageView_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KImage::ImageView::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KImage/ImageView:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::View::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KImage::ImageView::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KImage/ImageView:1.0" ) == 0) {
    return true;
  }
  for( int _i = 0; _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

KImage::ImageView_ptr KImage::ImageView::_narrow( CORBA::Object_ptr _obj )
{
  KImage::ImageView_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KImage/ImageView:1.0" ))
      return _duplicate( (KImage::ImageView_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KImage::ImageView_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KImage/ImageView:1.0" ) ) {
    _o = new KImage::ImageView_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KImage::ImageView_ptr KImage::ImageView::_nil()
{
  return NULL;
}

KImage::ImageView_stub::~ImageView_stub()
{
}

void KImage::ImageView_stub::toggleFitToWindow()
{
  CORBA::Request_var _req = this->_request( "toggleFitToWindow" );
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


void KImage::ImageView_stub::newView()
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


void KImage::ImageView_stub::insertObject()
{
  CORBA::Request_var _req = this->_request( "insertObject" );
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


void KImage::ImageView_stub::exportImage()
{
  CORBA::Request_var _req = this->_request( "exportImage" );
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


void KImage::ImageView_stub::importImage()
{
  CORBA::Request_var _req = this->_request( "importImage" );
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


struct _global_init_KImage_ImageView {
  _global_init_KImage_ImageView()
  {
    if( ::OPParts::View::_narrow_helpers == NULL )
      ::OPParts::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::View::_narrow_helpers->push_back( KImage::ImageView::_narrow_helper2 );
  }
} __global_init_KImage_ImageView;

vector<CORBA::Narrow_proto> *KImage::ImageView::_narrow_helpers;

CORBA::TypeCode_ptr KImage::_tc_ImageView = (new CORBA::TypeCode(
  "010000000e00000032000000010000001900000049444c3a4b496d616765"
  "2f496d616765566965773a312e30000000000a000000496d616765566965"
  "7700" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KImage::ImageView_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "ImageView" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KImage::ImageView_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KImage::ImageView::_nil();
    return TRUE;
  }
  _obj = ::KImage::ImageView::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface ImageDocument
KImage::ImageDocument::~ImageDocument()
{
}

KImage::ImageDocument_ptr KImage::ImageDocument::_duplicate( ImageDocument_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KImage::ImageDocument::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KImage/ImageDocument:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Document::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KImage::ImageDocument::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KImage/ImageDocument:1.0" ) == 0) {
    return true;
  }
  for( int _i = 0; _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

KImage::ImageDocument_ptr KImage::ImageDocument::_narrow( CORBA::Object_ptr _obj )
{
  KImage::ImageDocument_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KImage/ImageDocument:1.0" ))
      return _duplicate( (KImage::ImageDocument_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KImage::ImageDocument_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KImage/ImageDocument:1.0" ) ) {
    _o = new KImage::ImageDocument_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KImage::ImageDocument_ptr KImage::ImageDocument::_nil()
{
  return NULL;
}

KImage::ImageDocument_stub::~ImageDocument_stub()
{
}

CORBA::Boolean KImage::ImageDocument_stub::import( const char* filename )
{
  CORBA::Request_var _req = this->_request( "import" );
  _req->add_in_arg( "filename" ) <<= CORBA::Any::from_string( (char *) filename, 0 );
  _req->result()->value()->type( CORBA::_tc_boolean );
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
  CORBA::Boolean _res;
  *_req->result()->value() >>= CORBA::Any::to_boolean( _res );
  return _res;
}


CORBA::Boolean KImage::ImageDocument_stub::export( const char* filename, const char* format )
{
  CORBA::Request_var _req = this->_request( "export" );
  _req->add_in_arg( "filename" ) <<= CORBA::Any::from_string( (char *) filename, 0 );
  _req->add_in_arg( "format" ) <<= CORBA::Any::from_string( (char *) format, 0 );
  _req->result()->value()->type( CORBA::_tc_boolean );
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
  CORBA::Boolean _res;
  *_req->result()->value() >>= CORBA::Any::to_boolean( _res );
  return _res;
}


struct _global_init_KImage_ImageDocument {
  _global_init_KImage_ImageDocument()
  {
    if( ::OPParts::Document::_narrow_helpers == NULL )
      ::OPParts::Document::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Document::_narrow_helpers->push_back( KImage::ImageDocument::_narrow_helper2 );
  }
} __global_init_KImage_ImageDocument;

vector<CORBA::Narrow_proto> *KImage::ImageDocument::_narrow_helpers;

CORBA::TypeCode_ptr KImage::_tc_ImageDocument = (new CORBA::TypeCode(
  "010000000e0000003a000000010000001d00000049444c3a4b496d616765"
  "2f496d616765446f63756d656e743a312e30000000000e000000496d6167"
  "65446f63756d656e7400" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KImage::ImageDocument_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "ImageDocument" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KImage::ImageDocument_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KImage::ImageDocument::_nil();
    return TRUE;
  }
  _obj = ::KImage::ImageDocument::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Factory
KImage::Factory::~Factory()
{
}

KImage::Factory_ptr KImage::Factory::_duplicate( Factory_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KImage::Factory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KImage/Factory:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Factory::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KImage::Factory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KImage/Factory:1.0" ) == 0) {
    return true;
  }
  for( int _i = 0; _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

KImage::Factory_ptr KImage::Factory::_narrow( CORBA::Object_ptr _obj )
{
  KImage::Factory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KImage/Factory:1.0" ))
      return _duplicate( (KImage::Factory_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KImage::Factory_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KImage/Factory:1.0" ) ) {
    _o = new KImage::Factory_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KImage::Factory_ptr KImage::Factory::_nil()
{
  return NULL;
}

KImage::Factory_stub::~Factory_stub()
{
}

struct _global_init_KImage_Factory {
  _global_init_KImage_Factory()
  {
    if( ::OPParts::Factory::_narrow_helpers == NULL )
      ::OPParts::Factory::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Factory::_narrow_helpers->push_back( KImage::Factory::_narrow_helper2 );
  }
} __global_init_KImage_Factory;

vector<CORBA::Narrow_proto> *KImage::Factory::_narrow_helpers;

CORBA::TypeCode_ptr KImage::_tc_Factory = (new CORBA::TypeCode(
  "010000000e0000002c000000010000001700000049444c3a4b496d616765"
  "2f466163746f72793a312e30000008000000466163746f727900" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KImage::Factory_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Factory" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KImage::Factory_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KImage::Factory::_nil();
    return TRUE;
  }
  _obj = ::KImage::Factory::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface ImageView
KImage::ImageView_skel::ImageView_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KImage/ImageView:1.0", "ImageView" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KImage/ImageView:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<ImageView_skel>( this ) );
}

KImage::ImageView_skel::ImageView_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KImage/ImageView:1.0", "ImageView" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<ImageView_skel>( this ) );
}

KImage::ImageView_skel::~ImageView_skel()
{
}

bool KImage::ImageView_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "toggleFitToWindow" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    toggleFitToWindow();
    return true;
  }
  if( strcmp( _req->op_name(), "newView" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    newView();
    return true;
  }
  if( strcmp( _req->op_name(), "insertObject" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    insertObject();
    return true;
  }
  if( strcmp( _req->op_name(), "exportImage" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    exportImage();
    return true;
  }
  if( strcmp( _req->op_name(), "importImage" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orb()->create_list( 0, _args );

    _req->params( _args );

    importImage();
    return true;
  }
  return false;
}

KImage::ImageView_ptr KImage::ImageView_skel::_this()
{
  return KImage::ImageView::_duplicate( this );
}


// Dynamic Implementation Routine for interface ImageDocument
KImage::ImageDocument_skel::ImageDocument_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KImage/ImageDocument:1.0", "ImageDocument" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KImage/ImageDocument:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<ImageDocument_skel>( this ) );
}

KImage::ImageDocument_skel::ImageDocument_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KImage/ImageDocument:1.0", "ImageDocument" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<ImageDocument_skel>( this ) );
}

KImage::ImageDocument_skel::~ImageDocument_skel()
{
}

bool KImage::ImageDocument_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "import" ) == 0 ) {
    CORBA::String_var filename;

    CORBA::NVList_ptr _args;
    _orb()->create_list( 1, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_string );

    _req->params( _args );

    *_args->item( 0 )->value() >>= CORBA::Any::to_string( filename, 0 );
    CORBA::Boolean _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = import( filename );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= CORBA::Any::from_boolean( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "export" ) == 0 ) {
    CORBA::String_var filename;
    CORBA::String_var format;

    CORBA::NVList_ptr _args;
    _orb()->create_list( 2, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_string );
    _args->add( CORBA::ARG_IN );
    _args->item( 1 )->value()->type( CORBA::_tc_string );

    _req->params( _args );

    *_args->item( 0 )->value() >>= CORBA::Any::to_string( filename, 0 );
    *_args->item( 1 )->value() >>= CORBA::Any::to_string( format, 0 );
    CORBA::Boolean _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = export( filename, format );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= CORBA::Any::from_boolean( _res );
    _req->result( _any_res );
    return true;
  }
  return false;
}

KImage::ImageDocument_ptr KImage::ImageDocument_skel::_this()
{
  return KImage::ImageDocument::_duplicate( this );
}


// Dynamic Implementation Routine for interface Factory
KImage::Factory_skel::Factory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KImage/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KImage/Factory:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KImage::Factory_skel::Factory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KImage/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KImage::Factory_skel::~Factory_skel()
{
}

bool KImage::Factory_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KImage::Factory_ptr KImage::Factory_skel::_this()
{
  return KImage::Factory::_duplicate( this );
}

