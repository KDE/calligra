/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <kformviewer.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface View
KformViewer::View::~View()
{
}

void *KformViewer::View::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KformViewer/View:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOffice::View::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KformViewer::View::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KformViewer/View:1.0" ) == 0) {
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

KformViewer::View_ptr KformViewer::View::_narrow( CORBA::Object_ptr _obj )
{
  KformViewer::View_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KformViewer/View:1.0" )))
      return _duplicate( (KformViewer::View_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KformViewer/View:1.0" ) ) ) {
      _o = new KformViewer::View_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KformViewer::View_ptr
KformViewer::View::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KformViewer::View_stub::~View_stub()
{
}

struct _global_init_KformViewer_View {
  _global_init_KformViewer_View()
  {
    if( ::KOffice::View::_narrow_helpers == NULL )
      ::KOffice::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOffice::View::_narrow_helpers->push_back( KformViewer::View::_narrow_helper2 );
  }
} __global_init_KformViewer_View;

#ifdef HAVE_NAMESPACE
namespace KformViewer { vector<CORBA::Narrow_proto> * View::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KformViewer::View::_narrow_helpers;
#endif

class _Marshaller_KformViewer_View : public CORBA::StaticTypeInfo {
    typedef KformViewer::View_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
};


CORBA::StaticValueType _Marshaller_KformViewer_View::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KformViewer_View::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KformViewer::View::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KformViewer_View::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KformViewer_View::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KformViewer::View::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_KformViewer_View::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::StaticTypeInfo *_marshaller_KformViewer_View;


// Stub interface Document
KformViewer::Document::~Document()
{
}

void *KformViewer::Document::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KformViewer/Document:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOffice::Document::_narrow_helper( _repoid )))
      return _p;
  }
  {
    void *_p;
    if( (_p = KOffice::Print::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KformViewer::Document::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KformViewer/Document:1.0" ) == 0) {
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

KformViewer::Document_ptr KformViewer::Document::_narrow( CORBA::Object_ptr _obj )
{
  KformViewer::Document_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KformViewer/Document:1.0" )))
      return _duplicate( (KformViewer::Document_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KformViewer/Document:1.0" ) ) ) {
      _o = new KformViewer::Document_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KformViewer::Document_ptr
KformViewer::Document::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KformViewer::Document_stub::~Document_stub()
{
}

struct _global_init_KformViewer_Document {
  _global_init_KformViewer_Document()
  {
    if( ::KOffice::Document::_narrow_helpers == NULL )
      ::KOffice::Document::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOffice::Document::_narrow_helpers->push_back( KformViewer::Document::_narrow_helper2 );
    if( ::KOffice::Print::_narrow_helpers == NULL )
      ::KOffice::Print::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOffice::Print::_narrow_helpers->push_back( KformViewer::Document::_narrow_helper2 );
  }
} __global_init_KformViewer_Document;

#ifdef HAVE_NAMESPACE
namespace KformViewer { vector<CORBA::Narrow_proto> * Document::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KformViewer::Document::_narrow_helpers;
#endif

class _Marshaller_KformViewer_Document : public CORBA::StaticTypeInfo {
    typedef KformViewer::Document_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
};


CORBA::StaticValueType _Marshaller_KformViewer_Document::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KformViewer_Document::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KformViewer::Document::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KformViewer_Document::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KformViewer_Document::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KformViewer::Document::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_KformViewer_Document::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::StaticTypeInfo *_marshaller_KformViewer_Document;


// Stub interface DocumentFactory
KformViewer::DocumentFactory::~DocumentFactory()
{
}

void *KformViewer::DocumentFactory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KformViewer/DocumentFactory:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOffice::DocumentFactory::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KformViewer::DocumentFactory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KformViewer/DocumentFactory:1.0" ) == 0) {
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

KformViewer::DocumentFactory_ptr KformViewer::DocumentFactory::_narrow( CORBA::Object_ptr _obj )
{
  KformViewer::DocumentFactory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KformViewer/DocumentFactory:1.0" )))
      return _duplicate( (KformViewer::DocumentFactory_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KformViewer/DocumentFactory:1.0" ) ) ) {
      _o = new KformViewer::DocumentFactory_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KformViewer::DocumentFactory_ptr
KformViewer::DocumentFactory::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KformViewer::DocumentFactory_stub::~DocumentFactory_stub()
{
}

struct _global_init_KformViewer_DocumentFactory {
  _global_init_KformViewer_DocumentFactory()
  {
    if( ::KOffice::DocumentFactory::_narrow_helpers == NULL )
      ::KOffice::DocumentFactory::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOffice::DocumentFactory::_narrow_helpers->push_back( KformViewer::DocumentFactory::_narrow_helper2 );
  }
} __global_init_KformViewer_DocumentFactory;

#ifdef HAVE_NAMESPACE
namespace KformViewer { vector<CORBA::Narrow_proto> * DocumentFactory::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KformViewer::DocumentFactory::_narrow_helpers;
#endif

class _Marshaller_KformViewer_DocumentFactory : public CORBA::StaticTypeInfo {
    typedef KformViewer::DocumentFactory_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
};


CORBA::StaticValueType _Marshaller_KformViewer_DocumentFactory::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KformViewer_DocumentFactory::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KformViewer::DocumentFactory::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KformViewer_DocumentFactory::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KformViewer_DocumentFactory::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KformViewer::DocumentFactory::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_KformViewer_DocumentFactory::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::StaticTypeInfo *_marshaller_KformViewer_DocumentFactory;

struct __tc_init_KFORMVIEWER {
  __tc_init_KFORMVIEWER()
  {
    _marshaller_KformViewer_View = new _Marshaller_KformViewer_View;
    _marshaller_KformViewer_Document = new _Marshaller_KformViewer_Document;
    _marshaller_KformViewer_DocumentFactory = new _Marshaller_KformViewer_DocumentFactory;
  }
};

static __tc_init_KFORMVIEWER __init_KFORMVIEWER;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

KformViewer::View_skel::View_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KformViewer/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KformViewer/View:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

KformViewer::View_skel::View_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KformViewer/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

KformViewer::View_skel::~View_skel()
{
}

bool KformViewer::View_skel::dispatch( CORBA::StaticServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

KformViewer::View_ptr KformViewer::View_skel::_this()
{
  return KformViewer::View::_duplicate( this );
}


KformViewer::Document_skel::Document_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KformViewer/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KformViewer/Document:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Document_skel>( this ) );
}

KformViewer::Document_skel::Document_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KformViewer/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Document_skel>( this ) );
}

KformViewer::Document_skel::~Document_skel()
{
}

bool KformViewer::Document_skel::dispatch( CORBA::StaticServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

KformViewer::Document_ptr KformViewer::Document_skel::_this()
{
  return KformViewer::Document::_duplicate( this );
}


KformViewer::DocumentFactory_skel::DocumentFactory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KformViewer/DocumentFactory:1.0", "DocumentFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KformViewer/DocumentFactory:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<DocumentFactory_skel>( this ) );
}

KformViewer::DocumentFactory_skel::DocumentFactory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KformViewer/DocumentFactory:1.0", "DocumentFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<DocumentFactory_skel>( this ) );
}

KformViewer::DocumentFactory_skel::~DocumentFactory_skel()
{
}

bool KformViewer::DocumentFactory_skel::dispatch( CORBA::StaticServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

KformViewer::DocumentFactory_ptr KformViewer::DocumentFactory_skel::_this()
{
  return KformViewer::DocumentFactory::_duplicate( this );
}

