/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <koffice.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface Document
KOffice::Document::~Document()
{
}

KOffice::Document_ptr KOffice::Document::_duplicate( Document_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::Document::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/Document:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = OpenParts::Document::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::Document::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/Document:1.0" ) == 0) {
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

KOffice::Document_ptr KOffice::Document::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::Document_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/Document:1.0" )))
      return _duplicate( (KOffice::Document_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/Document:1.0" ) ) ) {
      _o = new KOffice::Document_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::Document_ptr
KOffice::Document::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::Document_ptr
KOffice::Document::_nil()
{
  return NULL;
}

KOffice::Document_stub::~Document_stub()
{
}

void KOffice::Document_stub::setURL( const char* url )
{
  CORBA::StaticAny _url( CORBA::_stc_string, &url );
  CORBA::StaticRequest __req( this, "setURL" );
  __req.add_in_arg( &_url );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


char* KOffice::Document_stub::url()
{
  char* _res;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "url" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


char* KOffice::Document_stub::mimeType()
{
  char* _res;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "mimeType" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean KOffice::Document_stub::loadFromURL( const char* url, const char* format )
{
  CORBA::StaticAny _url( CORBA::_stc_string, &url );
  CORBA::StaticAny _format( CORBA::_stc_string, &format );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "loadFromURL" );
  __req.add_in_arg( &_url );
  __req.add_in_arg( &_format );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean KOffice::Document_stub::saveToURL( const char* url, const char* format )
{
  CORBA::StaticAny _url( CORBA::_stc_string, &url );
  CORBA::StaticAny _format( CORBA::_stc_string, &format );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "saveToURL" );
  __req.add_in_arg( &_url );
  __req.add_in_arg( &_format );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean KOffice::Document_stub::loadFromStore( KOStore::Store_ptr store, const char* id )
{
  CORBA::StaticAny _store( _marshaller_KOStore_Store, &store );
  CORBA::StaticAny _id( CORBA::_stc_string, &id );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "loadFromStore" );
  __req.add_in_arg( &_store );
  __req.add_in_arg( &_id );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean KOffice::Document_stub::saveToStore( KOStore::Store_ptr store, const char* format )
{
  CORBA::StaticAny _store( _marshaller_KOStore_Store, &store );
  CORBA::StaticAny _format( CORBA::_stc_string, &format );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "saveToStore" );
  __req.add_in_arg( &_store );
  __req.add_in_arg( &_format );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean KOffice::Document_stub::initDoc()
{
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "initDoc" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean KOffice::Document_stub::isModified()
{
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "isModified" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


void KOffice::Document_stub::makeChildList( KOffice::Document_ptr root, const char* name )
{
  CORBA::StaticAny _root( _marshaller_KOffice_Document, &root );
  CORBA::StaticAny _name( CORBA::_stc_string, &name );
  CORBA::StaticRequest __req( this, "makeChildList" );
  __req.add_in_arg( &_root );
  __req.add_in_arg( &_name );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KOffice::Document_stub::addToChildList( KOffice::Document_ptr child, const char* name )
{
  CORBA::StaticAny _child( _marshaller_KOffice_Document, &child );
  CORBA::StaticAny _name( CORBA::_stc_string, &name );
  CORBA::StaticRequest __req( this, "addToChildList" );
  __req.add_in_arg( &_child );
  __req.add_in_arg( &_name );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


KOffice::MainWindow_ptr KOffice::Document_stub::createMainWindow()
{
  KOffice::MainWindow_ptr _res;
  CORBA::StaticAny __res( _marshaller_KOffice_MainWindow, &_res );

  CORBA::StaticRequest __req( this, "createMainWindow" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


struct _global_init_KOffice_Document {
  _global_init_KOffice_Document()
  {
    if( ::OpenParts::Document::_narrow_helpers == NULL )
      ::OpenParts::Document::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OpenParts::Document::_narrow_helpers->push_back( KOffice::Document::_narrow_helper2 );
  }
} __global_init_KOffice_Document;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * Document::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::Document::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_Document; };
#else
CORBA::TypeCodeConst KOffice::_tc_Document;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::Document_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Document, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::Document_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_Document, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::Document_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Document, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_Document : public CORBA::StaticTypeInfo {
    typedef KOffice::Document_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_Document::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_Document::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::Document::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_Document::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_Document::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::Document::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_Document::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_Document::typecode()
{
  return KOffice::_tc_Document;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_Document;


// Stub interface Print
KOffice::Print::~Print()
{
}

KOffice::Print_ptr KOffice::Print::_duplicate( Print_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::Print::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/Print:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool KOffice::Print::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/Print:1.0" ) == 0) {
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

KOffice::Print_ptr KOffice::Print::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::Print_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/Print:1.0" )))
      return _duplicate( (KOffice::Print_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/Print:1.0" ) ) ) {
      _o = new KOffice::Print_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::Print_ptr
KOffice::Print::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::Print_ptr
KOffice::Print::_nil()
{
  return NULL;
}

KOffice::Print_stub::~Print_stub()
{
}

char* KOffice::Print_stub::encodedMetaFile( CORBA::Long width, CORBA::Long height, CORBA::Float scale )
{
  CORBA::StaticAny _width( CORBA::_stc_long, &width );
  CORBA::StaticAny _height( CORBA::_stc_long, &height );
  CORBA::StaticAny _scale( CORBA::_stc_float, &scale );
  char* _res;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "encodedMetaFile" );
  __req.add_in_arg( &_width );
  __req.add_in_arg( &_height );
  __req.add_in_arg( &_scale );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * Print::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::Print::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_Print; };
#else
CORBA::TypeCodeConst KOffice::_tc_Print;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::Print_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Print, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::Print_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_Print, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::Print_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Print, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_Print : public CORBA::StaticTypeInfo {
    typedef KOffice::Print_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_Print::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_Print::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::Print::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_Print::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_Print::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::Print::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_Print::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_Print::typecode()
{
  return KOffice::_tc_Print;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_Print;

#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst View::_tc_Mode; };
#else
CORBA::TypeCodeConst KOffice::View::_tc_Mode;
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const KOffice::View::Mode &_e )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_View_Mode, &_e);
  return _a.from_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, KOffice::View::Mode &_e )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_View_Mode, &_e);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_View_Mode : public CORBA::StaticTypeInfo {
    typedef KOffice::View::Mode _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_View_Mode::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_KOffice_View_Mode::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_KOffice_View_Mode::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_View_Mode::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::ULong ul;
  if( !dc.enumeration( ul ) )
    return FALSE;
  *(_MICO_T*) v = (_MICO_T) ul;
  return TRUE;
}

void _Marshaller_KOffice_View_Mode::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.enumeration( (CORBA::ULong) *(_MICO_T *) v );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_View_Mode::typecode()
{
  return KOffice::View::_tc_Mode;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_View_Mode;

#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst View::_tc_EventNewPart; };
#else
CORBA::TypeCodeConst KOffice::View::_tc_EventNewPart;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
KOffice::View::EventNewPart::EventNewPart()
{
}

KOffice::View::EventNewPart::EventNewPart( const EventNewPart& _s )
{
  view = ((EventNewPart&)_s).view;
}

KOffice::View::EventNewPart::~EventNewPart()
{
}

KOffice::View::EventNewPart&
KOffice::View::EventNewPart::operator=( const EventNewPart& _s )
{
  view = ((EventNewPart&)_s).view;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const KOffice::View::EventNewPart &_s )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_View_EventNewPart, &_s);
  return _a.from_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, KOffice::View::EventNewPart &_s )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_View_EventNewPart, &_s);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_View_EventNewPart : public CORBA::StaticTypeInfo {
    typedef KOffice::View::EventNewPart _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_View_EventNewPart::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_KOffice_View_EventNewPart::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_KOffice_View_EventNewPart::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_View_EventNewPart::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    _marshaller_KOffice_View->demarshal( dc, &((_MICO_T*)v)->view.inout() ) &&
    dc.struct_end();
}

void _Marshaller_KOffice_View_EventNewPart::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  _marshaller_KOffice_View->marshal( ec, &((_MICO_T*)v)->view.inout() );
  ec.struct_end();
}

CORBA::TypeCode_ptr _Marshaller_KOffice_View_EventNewPart::typecode()
{
  return KOffice::View::_tc_EventNewPart;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_View_EventNewPart;

#ifdef HAVE_NAMESPACE
namespace KOffice { const char* View::eventNewPart = "KOffice/View/NewPart"; };
#else
const char* KOffice::View::eventNewPart = "KOffice/View/NewPart";
#endif

// Stub interface View
KOffice::View::~View()
{
}

KOffice::View_ptr KOffice::View::_duplicate( View_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::View::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/View:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = OpenParts::View::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::View::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/View:1.0" ) == 0) {
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

KOffice::View_ptr KOffice::View::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::View_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/View:1.0" )))
      return _duplicate( (KOffice::View_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/View:1.0" ) ) ) {
      _o = new KOffice::View_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::View_ptr
KOffice::View::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::View_ptr
KOffice::View::_nil()
{
  return NULL;
}

KOffice::View_stub::~View_stub()
{
}

CORBA::Boolean KOffice::View_stub::isMarked()
{
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "isMarked" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


void KOffice::View_stub::setMarked( CORBA::Boolean marked )
{
  CORBA::StaticAny _marked( CORBA::_stc_boolean, &marked );
  CORBA::StaticRequest __req( this, "setMarked" );
  __req.add_in_arg( &_marked );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KOffice::View_stub::setMode( KOffice::View::Mode mode )
{
  CORBA::StaticAny _mode( _marshaller_KOffice_View_Mode, &mode );
  CORBA::StaticRequest __req( this, "setMode" );
  __req.add_in_arg( &_mode );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


KOffice::View::Mode KOffice::View_stub::mode()
{
  KOffice::View::Mode _res;
  CORBA::StaticAny __res( _marshaller_KOffice_View_Mode, &_res );

  CORBA::StaticRequest __req( this, "mode" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::ULong KOffice::View_stub::leftGUISize()
{
  CORBA::ULong _res;
  CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );

  CORBA::StaticRequest __req( this, "leftGUISize" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::ULong KOffice::View_stub::rightGUISize()
{
  CORBA::ULong _res;
  CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );

  CORBA::StaticRequest __req( this, "rightGUISize" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::ULong KOffice::View_stub::topGUISize()
{
  CORBA::ULong _res;
  CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );

  CORBA::StaticRequest __req( this, "topGUISize" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::ULong KOffice::View_stub::bottomGUISize()
{
  CORBA::ULong _res;
  CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );

  CORBA::StaticRequest __req( this, "bottomGUISize" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean KOffice::View_stub::printDlg()
{
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "printDlg" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


struct _global_init_KOffice_View {
  _global_init_KOffice_View()
  {
    if( ::OpenParts::View::_narrow_helpers == NULL )
      ::OpenParts::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OpenParts::View::_narrow_helpers->push_back( KOffice::View::_narrow_helper2 );
  }
} __global_init_KOffice_View;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * View::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::View::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_View; };
#else
CORBA::TypeCodeConst KOffice::_tc_View;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::View_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_View, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::View_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_View, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::View_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_View, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_View : public CORBA::StaticTypeInfo {
    typedef KOffice::View_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_View::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_View::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::View::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_View::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_View::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::View::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_View::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_View::typecode()
{
  return KOffice::_tc_View;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_View;


// Stub interface DocumentFactory
KOffice::DocumentFactory::~DocumentFactory()
{
}

KOffice::DocumentFactory_ptr KOffice::DocumentFactory::_duplicate( DocumentFactory_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::DocumentFactory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/DocumentFactory:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOM::ComponentFactory::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::DocumentFactory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/DocumentFactory:1.0" ) == 0) {
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

KOffice::DocumentFactory_ptr KOffice::DocumentFactory::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::DocumentFactory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/DocumentFactory:1.0" )))
      return _duplicate( (KOffice::DocumentFactory_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/DocumentFactory:1.0" ) ) ) {
      _o = new KOffice::DocumentFactory_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::DocumentFactory_ptr
KOffice::DocumentFactory::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::DocumentFactory_ptr
KOffice::DocumentFactory::_nil()
{
  return NULL;
}

KOffice::DocumentFactory_stub::~DocumentFactory_stub()
{
}

KOffice::Document_ptr KOffice::DocumentFactory_stub::create()
{
  KOffice::Document_ptr _res;
  CORBA::StaticAny __res( _marshaller_KOffice_Document, &_res );

  CORBA::StaticRequest __req( this, "create" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


struct _global_init_KOffice_DocumentFactory {
  _global_init_KOffice_DocumentFactory()
  {
    if( ::KOM::ComponentFactory::_narrow_helpers == NULL )
      ::KOM::ComponentFactory::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOM::ComponentFactory::_narrow_helpers->push_back( KOffice::DocumentFactory::_narrow_helper2 );
  }
} __global_init_KOffice_DocumentFactory;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * DocumentFactory::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::DocumentFactory::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_DocumentFactory; };
#else
CORBA::TypeCodeConst KOffice::_tc_DocumentFactory;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::DocumentFactory_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_DocumentFactory, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::DocumentFactory_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_DocumentFactory, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::DocumentFactory_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_DocumentFactory, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_DocumentFactory : public CORBA::StaticTypeInfo {
    typedef KOffice::DocumentFactory_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_DocumentFactory::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_DocumentFactory::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::DocumentFactory::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_DocumentFactory::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_DocumentFactory::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::DocumentFactory::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_DocumentFactory::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_DocumentFactory::typecode()
{
  return KOffice::_tc_DocumentFactory;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_DocumentFactory;


// Stub interface Frame
KOffice::Frame::~Frame()
{
}

KOffice::Frame_ptr KOffice::Frame::_duplicate( Frame_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::Frame::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/Frame:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOM::Base::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::Frame::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/Frame:1.0" ) == 0) {
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

KOffice::Frame_ptr KOffice::Frame::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::Frame_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/Frame:1.0" )))
      return _duplicate( (KOffice::Frame_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/Frame:1.0" ) ) ) {
      _o = new KOffice::Frame_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::Frame_ptr
KOffice::Frame::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::Frame_ptr
KOffice::Frame::_nil()
{
  return NULL;
}

KOffice::Frame_stub::~Frame_stub()
{
}

KOffice::View_ptr KOffice::Frame_stub::view()
{
  KOffice::View_ptr _res;
  CORBA::StaticAny __res( _marshaller_KOffice_View, &_res );

  CORBA::StaticRequest __req( this, "view" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


void KOffice::Frame_stub::viewChangedState( CORBA::Boolean is_marked, CORBA::Boolean has_focus )
{
  CORBA::StaticAny _is_marked( CORBA::_stc_boolean, &is_marked );
  CORBA::StaticAny _has_focus( CORBA::_stc_boolean, &has_focus );
  CORBA::StaticRequest __req( this, "viewChangedState" );
  __req.add_in_arg( &_is_marked );
  __req.add_in_arg( &_has_focus );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


struct _global_init_KOffice_Frame {
  _global_init_KOffice_Frame()
  {
    if( ::KOM::Base::_narrow_helpers == NULL )
      ::KOM::Base::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOM::Base::_narrow_helpers->push_back( KOffice::Frame::_narrow_helper2 );
  }
} __global_init_KOffice_Frame;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * Frame::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::Frame::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_Frame; };
#else
CORBA::TypeCodeConst KOffice::_tc_Frame;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::Frame_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Frame, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::Frame_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_Frame, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::Frame_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Frame, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_Frame : public CORBA::StaticTypeInfo {
    typedef KOffice::Frame_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_Frame::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_Frame::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::Frame::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_Frame::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_Frame::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::Frame::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_Frame::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_Frame::typecode()
{
  return KOffice::_tc_Frame;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_Frame;


// Stub interface MainWindow
KOffice::MainWindow::~MainWindow()
{
}

KOffice::MainWindow_ptr KOffice::MainWindow::_duplicate( MainWindow_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::MainWindow::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/MainWindow:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = OpenParts::MainWindow::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::MainWindow::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/MainWindow:1.0" ) == 0) {
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

KOffice::MainWindow_ptr KOffice::MainWindow::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::MainWindow_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/MainWindow:1.0" )))
      return _duplicate( (KOffice::MainWindow_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/MainWindow:1.0" ) ) ) {
      _o = new KOffice::MainWindow_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::MainWindow_ptr
KOffice::MainWindow::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::MainWindow_ptr
KOffice::MainWindow::_nil()
{
  return NULL;
}

KOffice::MainWindow_stub::~MainWindow_stub()
{
}

void KOffice::MainWindow_stub::setMarkedPart( OpenParts::Id id )
{
  CORBA::StaticAny _id( CORBA::_stc_ulong, &id );
  CORBA::StaticRequest __req( this, "setMarkedPart" );
  __req.add_in_arg( &_id );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


KOffice::Document_ptr KOffice::MainWindow_stub::document()
{
  KOffice::Document_ptr _res;
  CORBA::StaticAny __res( _marshaller_KOffice_Document, &_res );

  CORBA::StaticRequest __req( this, "document" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


KOffice::View_ptr KOffice::MainWindow_stub::view()
{
  KOffice::View_ptr _res;
  CORBA::StaticAny __res( _marshaller_KOffice_View, &_res );

  CORBA::StaticRequest __req( this, "view" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


struct _global_init_KOffice_MainWindow {
  _global_init_KOffice_MainWindow()
  {
    if( ::OpenParts::MainWindow::_narrow_helpers == NULL )
      ::OpenParts::MainWindow::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OpenParts::MainWindow::_narrow_helpers->push_back( KOffice::MainWindow::_narrow_helper2 );
  }
} __global_init_KOffice_MainWindow;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * MainWindow::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::MainWindow::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_MainWindow; };
#else
CORBA::TypeCodeConst KOffice::_tc_MainWindow;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::MainWindow_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_MainWindow, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::MainWindow_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_MainWindow, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::MainWindow_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_MainWindow, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_MainWindow : public CORBA::StaticTypeInfo {
    typedef KOffice::MainWindow_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_MainWindow::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_MainWindow::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::MainWindow::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_MainWindow::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_MainWindow::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::MainWindow::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_MainWindow::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_MainWindow::typecode()
{
  return KOffice::_tc_MainWindow;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_MainWindow;

#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst Filter::_tc_Data; };
#else
CORBA::TypeCodeConst KOffice::Filter::_tc_Data;
#endif

#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst Filter::_tc_FormatError; };
#else
CORBA::TypeCodeConst KOffice::Filter::_tc_FormatError;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
KOffice::Filter::FormatError::FormatError()
{
}

KOffice::Filter::FormatError::FormatError( const FormatError& _s )
{
}

KOffice::Filter::FormatError::~FormatError()
{
}

KOffice::Filter::FormatError&
KOffice::Filter::FormatError::operator=( const FormatError& _s )
{
  return *this;
}
#endif

class _Marshaller_KOffice_Filter_FormatError : public CORBA::StaticTypeInfo {
    typedef ::KOffice::Filter::FormatError _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_Filter_FormatError::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_KOffice_Filter_FormatError::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_KOffice_Filter_FormatError::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_Filter_FormatError::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    dc.except_end();
}

void _Marshaller_KOffice_Filter_FormatError::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:KOffice/Filter/FormatError:1.0" );
  ec.except_end();
}

CORBA::TypeCode_ptr _Marshaller_KOffice_Filter_FormatError::typecode()
{
  return KOffice::Filter::_tc_FormatError;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_Filter_FormatError;

CORBA::Boolean operator<<=( CORBA::Any &_a, const KOffice::Filter::FormatError &_e )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Filter_FormatError, &_e);
  return _a.from_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, KOffice::Filter::FormatError &_e )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Filter_FormatError, &_e);
  return _a.to_static_any (_sa);
}

void KOffice::Filter::FormatError::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw FormatError_var( (KOffice::Filter::FormatError*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *KOffice::Filter::FormatError::_repoid() const
{
  return "IDL:KOffice/Filter/FormatError:1.0";
}

void KOffice::Filter::FormatError::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_KOffice_Filter_FormatError->marshal( _en, (void*) this );
}

CORBA::Exception *KOffice::Filter::FormatError::_clone() const
{
  return new FormatError( *this );
}

KOffice::Filter::FormatError *KOffice::Filter::FormatError::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:KOffice/Filter/FormatError:1.0" ) )
    return (FormatError *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst Filter::_tc_UnsupportedFormat; };
#else
CORBA::TypeCodeConst KOffice::Filter::_tc_UnsupportedFormat;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
KOffice::Filter::UnsupportedFormat::UnsupportedFormat()
{
}

KOffice::Filter::UnsupportedFormat::UnsupportedFormat( const UnsupportedFormat& _s )
{
  format = ((UnsupportedFormat&)_s).format;
}

KOffice::Filter::UnsupportedFormat::~UnsupportedFormat()
{
}

KOffice::Filter::UnsupportedFormat&
KOffice::Filter::UnsupportedFormat::operator=( const UnsupportedFormat& _s )
{
  format = ((UnsupportedFormat&)_s).format;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
KOffice::Filter::UnsupportedFormat::UnsupportedFormat()
{
}

#endif

KOffice::Filter::UnsupportedFormat::UnsupportedFormat( const char* _m0 )
{
  format = _m0;
}

class _Marshaller_KOffice_Filter_UnsupportedFormat : public CORBA::StaticTypeInfo {
    typedef ::KOffice::Filter::UnsupportedFormat _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_Filter_UnsupportedFormat::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_KOffice_Filter_UnsupportedFormat::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_KOffice_Filter_UnsupportedFormat::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_Filter_UnsupportedFormat::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->format.inout() ) &&
    dc.except_end();
}

void _Marshaller_KOffice_Filter_UnsupportedFormat::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:KOffice/Filter/UnsupportedFormat:1.0" );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->format.inout() );
  ec.except_end();
}

CORBA::TypeCode_ptr _Marshaller_KOffice_Filter_UnsupportedFormat::typecode()
{
  return KOffice::Filter::_tc_UnsupportedFormat;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_Filter_UnsupportedFormat;

CORBA::Boolean operator<<=( CORBA::Any &_a, const KOffice::Filter::UnsupportedFormat &_e )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Filter_UnsupportedFormat, &_e);
  return _a.from_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, KOffice::Filter::UnsupportedFormat &_e )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Filter_UnsupportedFormat, &_e);
  return _a.to_static_any (_sa);
}

void KOffice::Filter::UnsupportedFormat::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw UnsupportedFormat_var( (KOffice::Filter::UnsupportedFormat*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *KOffice::Filter::UnsupportedFormat::_repoid() const
{
  return "IDL:KOffice/Filter/UnsupportedFormat:1.0";
}

void KOffice::Filter::UnsupportedFormat::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_KOffice_Filter_UnsupportedFormat->marshal( _en, (void*) this );
}

CORBA::Exception *KOffice::Filter::UnsupportedFormat::_clone() const
{
  return new UnsupportedFormat( *this );
}

KOffice::Filter::UnsupportedFormat *KOffice::Filter::UnsupportedFormat::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:KOffice/Filter/UnsupportedFormat:1.0" ) )
    return (UnsupportedFormat *) _ex;
  return NULL;
}


// Stub interface Filter
KOffice::Filter::~Filter()
{
}

KOffice::Filter_ptr KOffice::Filter::_duplicate( Filter_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::Filter::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/Filter:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOM::Component::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::Filter::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/Filter:1.0" ) == 0) {
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

KOffice::Filter_ptr KOffice::Filter::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::Filter_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/Filter:1.0" )))
      return _duplicate( (KOffice::Filter_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/Filter:1.0" ) ) ) {
      _o = new KOffice::Filter_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::Filter_ptr
KOffice::Filter::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::Filter_ptr
KOffice::Filter::_nil()
{
  return NULL;
}

KOffice::Filter_stub::~Filter_stub()
{
}

void KOffice::Filter_stub::filter( KOffice::Filter::Data& data, const char* from, const char* to )
{
  CORBA::StaticAny _data( CORBA::_stcseq_octet, &data );
  CORBA::StaticAny _from( CORBA::_stc_string, &from );
  CORBA::StaticAny _to( CORBA::_stc_string, &to );
  CORBA::StaticRequest __req( this, "filter" );
  __req.add_inout_arg( &_data );
  __req.add_in_arg( &_from );
  __req.add_in_arg( &_to );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_KOffice_Filter_FormatError, "IDL:KOffice/Filter/FormatError:1.0",
    _marshaller_KOffice_Filter_UnsupportedFormat, "IDL:KOffice/Filter/UnsupportedFormat:1.0",
    0);
}


struct _global_init_KOffice_Filter {
  _global_init_KOffice_Filter()
  {
    if( ::KOM::Component::_narrow_helpers == NULL )
      ::KOM::Component::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOM::Component::_narrow_helpers->push_back( KOffice::Filter::_narrow_helper2 );
  }
} __global_init_KOffice_Filter;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * Filter::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::Filter::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_Filter; };
#else
CORBA::TypeCodeConst KOffice::_tc_Filter;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::Filter_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Filter, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::Filter_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_Filter, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::Filter_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Filter, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_Filter : public CORBA::StaticTypeInfo {
    typedef KOffice::Filter_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_Filter::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_Filter::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::Filter::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_Filter::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_Filter::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::Filter::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_Filter::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_Filter::typecode()
{
  return KOffice::_tc_Filter;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_Filter;


// Stub interface FilterFactory
KOffice::FilterFactory::~FilterFactory()
{
}

KOffice::FilterFactory_ptr KOffice::FilterFactory::_duplicate( FilterFactory_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::FilterFactory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/FilterFactory:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOM::ComponentFactory::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::FilterFactory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/FilterFactory:1.0" ) == 0) {
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

KOffice::FilterFactory_ptr KOffice::FilterFactory::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::FilterFactory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/FilterFactory:1.0" )))
      return _duplicate( (KOffice::FilterFactory_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/FilterFactory:1.0" ) ) ) {
      _o = new KOffice::FilterFactory_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::FilterFactory_ptr
KOffice::FilterFactory::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::FilterFactory_ptr
KOffice::FilterFactory::_nil()
{
  return NULL;
}

KOffice::FilterFactory_stub::~FilterFactory_stub()
{
}

KOffice::Filter_ptr KOffice::FilterFactory_stub::create()
{
  KOffice::Filter_ptr _res;
  CORBA::StaticAny __res( _marshaller_KOffice_Filter, &_res );

  CORBA::StaticRequest __req( this, "create" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


struct _global_init_KOffice_FilterFactory {
  _global_init_KOffice_FilterFactory()
  {
    if( ::KOM::ComponentFactory::_narrow_helpers == NULL )
      ::KOM::ComponentFactory::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOM::ComponentFactory::_narrow_helpers->push_back( KOffice::FilterFactory::_narrow_helper2 );
  }
} __global_init_KOffice_FilterFactory;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * FilterFactory::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::FilterFactory::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_FilterFactory; };
#else
CORBA::TypeCodeConst KOffice::_tc_FilterFactory;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::FilterFactory_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_FilterFactory, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::FilterFactory_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_FilterFactory, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::FilterFactory_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_FilterFactory, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_FilterFactory : public CORBA::StaticTypeInfo {
    typedef KOffice::FilterFactory_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_FilterFactory::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_FilterFactory::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::FilterFactory::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_FilterFactory::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_FilterFactory::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::FilterFactory::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_FilterFactory::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_FilterFactory::typecode()
{
  return KOffice::_tc_FilterFactory;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_FilterFactory;


// Stub interface Callback
KOffice::Callback::~Callback()
{
}

KOffice::Callback_ptr KOffice::Callback::_duplicate( Callback_ptr _obj )
{
  CORBA::Object::_duplicate (_obj);
  return _obj;
}

void *KOffice::Callback::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KOffice/Callback:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = KOM::Base::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KOffice::Callback::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KOffice/Callback:1.0" ) == 0) {
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

KOffice::Callback_ptr KOffice::Callback::_narrow( CORBA::Object_ptr _obj )
{
  KOffice::Callback_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KOffice/Callback:1.0" )))
      return _duplicate( (KOffice::Callback_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KOffice/Callback:1.0" ) ) ) {
      _o = new KOffice::Callback_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KOffice::Callback_ptr
KOffice::Callback::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KOffice::Callback_ptr
KOffice::Callback::_nil()
{
  return NULL;
}

KOffice::Callback_stub::~Callback_stub()
{
}

void KOffice::Callback_stub::callback()
{
  CORBA::StaticRequest __req( this, "callback" );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


struct _global_init_KOffice_Callback {
  _global_init_KOffice_Callback()
  {
    if( ::KOM::Base::_narrow_helpers == NULL )
      ::KOM::Base::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::KOM::Base::_narrow_helpers->push_back( KOffice::Callback::_narrow_helper2 );
  }
} __global_init_KOffice_Callback;

#ifdef HAVE_NAMESPACE
namespace KOffice { vector<CORBA::Narrow_proto> * Callback::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KOffice::Callback::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KOffice { CORBA::TypeCodeConst _tc_Callback; };
#else
CORBA::TypeCodeConst KOffice::_tc_Callback;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KOffice::Callback_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Callback, &_obj);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator<<=( CORBA::Any &_a, KOffice::Callback_ptr* _obj_ptr )
{
  CORBA::Object_var _obj = *_obj_ptr;
  CORBA::StaticAny _sa (_marshaller_KOffice_Callback, _obj_ptr);
  return _a.from_static_any (_sa);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KOffice::Callback_ptr &_obj )
{
  CORBA::StaticAny _sa (_marshaller_KOffice_Callback, &_obj);
  return _a.to_static_any (_sa);
}

class _Marshaller_KOffice_Callback : public CORBA::StaticTypeInfo {
    typedef KOffice::Callback_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KOffice_Callback::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KOffice_Callback::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KOffice::Callback::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KOffice_Callback::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KOffice_Callback::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if(!CORBA::_stc_Object->demarshal( dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KOffice::Callback::_narrow( obj );
  if (!CORBA::is_nil (obj) && CORBA::is_nil (*(_MICO_T*)v)) {
    CORBA::release (obj);
    return FALSE;
  }
  return TRUE;
}

void _Marshaller_KOffice_Callback::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KOffice_Callback::typecode()
{
  return KOffice::_tc_Callback;
}

CORBA::StaticTypeInfo *_marshaller_KOffice_Callback;

struct __tc_init_KOFFICE {
  __tc_init_KOFFICE()
  {
    KOffice::_tc_Document = 
    "010000000e00000031000000010000001900000049444c3a4b4f66666963"
    "652f446f63756d656e743a312e300000000009000000446f63756d656e74"
    "00";
    _marshaller_KOffice_Document = new _Marshaller_KOffice_Document;
    KOffice::_tc_Print = 
    "010000000e0000002a000000010000001600000049444c3a4b4f66666963"
    "652f5072696e743a312e30000000060000005072696e7400";
    _marshaller_KOffice_Print = new _Marshaller_KOffice_Print;
    KOffice::View::_tc_Mode = 
    "010000001100000051000000010000001a00000049444c3a4b4f66666963"
    "652f566965772f4d6f64653a312e30000000050000004d6f646500000000"
    "020000000a0000004368696c644d6f646500000009000000526f6f744d6f"
    "646500";
    _marshaller_KOffice_View_Mode = new _Marshaller_KOffice_View_Mode;
    KOffice::View::_tc_EventNewPart = 
    "010000000f00000081000000010000002200000049444c3a4b4f66666963"
    "652f566965772f4576656e744e6577506172743a312e300000000d000000"
    "4576656e744e657750617274000000000100000005000000766965770000"
    "00000e00000029000000010000001500000049444c3a4b4f66666963652f"
    "566965773a312e3000000000050000005669657700";
    _marshaller_KOffice_View_EventNewPart = new _Marshaller_KOffice_View_EventNewPart;
    KOffice::_tc_View = 
    "010000000e00000029000000010000001500000049444c3a4b4f66666963"
    "652f566965773a312e3000000000050000005669657700";
    _marshaller_KOffice_View = new _Marshaller_KOffice_View;
    KOffice::_tc_DocumentFactory = 
    "010000000e0000003c000000010000002000000049444c3a4b4f66666963"
    "652f446f63756d656e74466163746f72793a312e300010000000446f6375"
    "6d656e74466163746f727900";
    _marshaller_KOffice_DocumentFactory = new _Marshaller_KOffice_DocumentFactory;
    KOffice::_tc_Frame = 
    "010000000e0000002a000000010000001600000049444c3a4b4f66666963"
    "652f4672616d653a312e30000000060000004672616d6500";
    _marshaller_KOffice_Frame = new _Marshaller_KOffice_Frame;
    KOffice::_tc_MainWindow = 
    "010000000e00000033000000010000001b00000049444c3a4b4f66666963"
    "652f4d61696e57696e646f773a312e3000000b0000004d61696e57696e64"
    "6f7700";
    _marshaller_KOffice_MainWindow = new _Marshaller_KOffice_MainWindow;
    KOffice::Filter::_tc_Data = 
    "010000001500000044000000010000001c00000049444c3a4b4f66666963"
    "652f46696c7465722f446174613a312e3000050000004461746100000000"
    "130000000c000000010000000a00000000000000";
    KOffice::Filter::_tc_FormatError = 
    "010000001600000040000000010000002300000049444c3a4b4f66666963"
    "652f46696c7465722f466f726d61744572726f723a312e3000000c000000"
    "466f726d61744572726f720000000000";
    _marshaller_KOffice_Filter_FormatError = new _Marshaller_KOffice_Filter_FormatError;
    KOffice::Filter::_tc_UnsupportedFormat = 
    "010000001600000064000000010000002900000049444c3a4b4f66666963"
    "652f46696c7465722f556e737570706f72746564466f726d61743a312e30"
    "0000000012000000556e737570706f72746564466f726d61740000000100"
    "000007000000666f726d617400001200000000000000";
    _marshaller_KOffice_Filter_UnsupportedFormat = new _Marshaller_KOffice_Filter_UnsupportedFormat;
    KOffice::_tc_Filter = 
    "010000000e0000002b000000010000001700000049444c3a4b4f66666963"
    "652f46696c7465723a312e3000000700000046696c74657200";
    _marshaller_KOffice_Filter = new _Marshaller_KOffice_Filter;
    KOffice::_tc_FilterFactory = 
    "010000000e0000003a000000010000001e00000049444c3a4b4f66666963"
    "652f46696c746572466163746f72793a312e300000000e00000046696c74"
    "6572466163746f727900";
    _marshaller_KOffice_FilterFactory = new _Marshaller_KOffice_FilterFactory;
    KOffice::_tc_Callback = 
    "010000000e00000031000000010000001900000049444c3a4b4f66666963"
    "652f43616c6c6261636b3a312e30000000000900000043616c6c6261636b"
    "00";
    _marshaller_KOffice_Callback = new _Marshaller_KOffice_Callback;
  }
};

static __tc_init_KOFFICE __init_KOFFICE;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

KOffice::Document_skel::Document_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/Document:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Document_skel>( this ) );
}

KOffice::Document_skel::Document_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Document_skel>( this ) );
}

KOffice::Document_skel::~Document_skel()
{
}

bool KOffice::Document_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "setURL" ) == 0 ) {
      CORBA::String_var url;
      CORBA::StaticAny _url( CORBA::_stc_string, &url.inout() );

      _req->add_in_arg( &_url );

      if( !_req->read_args() )
        return true;

      setURL( url );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "url" ) == 0 ) {
      char* _res;
      CORBA::StaticAny __res( CORBA::_stc_string );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = url();
      __res.value( CORBA::_stc_string, &_res );
      _req->write_results();
      CORBA::string_free( _res );
      return true;
    }
    if( strcmp( _req->op_name(), "mimeType" ) == 0 ) {
      char* _res;
      CORBA::StaticAny __res( CORBA::_stc_string );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = mimeType();
      __res.value( CORBA::_stc_string, &_res );
      _req->write_results();
      CORBA::string_free( _res );
      return true;
    }
    if( strcmp( _req->op_name(), "loadFromURL" ) == 0 ) {
      CORBA::String_var url;
      CORBA::StaticAny _url( CORBA::_stc_string, &url.inout() );
      CORBA::String_var format;
      CORBA::StaticAny _format( CORBA::_stc_string, &format.inout() );

      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->add_in_arg( &_url );
      _req->add_in_arg( &_format );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = loadFromURL( url, format );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "saveToURL" ) == 0 ) {
      CORBA::String_var url;
      CORBA::StaticAny _url( CORBA::_stc_string, &url.inout() );
      CORBA::String_var format;
      CORBA::StaticAny _format( CORBA::_stc_string, &format.inout() );

      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->add_in_arg( &_url );
      _req->add_in_arg( &_format );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = saveToURL( url, format );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "loadFromStore" ) == 0 ) {
      KOStore::Store_var store;
      CORBA::StaticAny _store( _marshaller_KOStore_Store, &store.inout() );
      CORBA::String_var id;
      CORBA::StaticAny _id( CORBA::_stc_string, &id.inout() );

      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->add_in_arg( &_store );
      _req->add_in_arg( &_id );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = loadFromStore( store, id );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "saveToStore" ) == 0 ) {
      KOStore::Store_var store;
      CORBA::StaticAny _store( _marshaller_KOStore_Store, &store.inout() );
      CORBA::String_var format;
      CORBA::StaticAny _format( CORBA::_stc_string, &format.inout() );

      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->add_in_arg( &_store );
      _req->add_in_arg( &_format );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = saveToStore( store, format );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "initDoc" ) == 0 ) {
      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = initDoc();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "isModified" ) == 0 ) {
      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = isModified();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "makeChildList" ) == 0 ) {
      Document_var root;
      CORBA::StaticAny _root( _marshaller_KOffice_Document, &root.inout() );
      CORBA::String_var name;
      CORBA::StaticAny _name( CORBA::_stc_string, &name.inout() );

      _req->add_in_arg( &_root );
      _req->add_in_arg( &_name );

      if( !_req->read_args() )
        return true;

      makeChildList( root, name );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "addToChildList" ) == 0 ) {
      Document_var child;
      CORBA::StaticAny _child( _marshaller_KOffice_Document, &child.inout() );
      CORBA::String_var name;
      CORBA::StaticAny _name( CORBA::_stc_string, &name.inout() );

      _req->add_in_arg( &_child );
      _req->add_in_arg( &_name );

      if( !_req->read_args() )
        return true;

      addToChildList( child, name );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "createMainWindow" ) == 0 ) {
      MainWindow_ptr _res;
      CORBA::StaticAny __res( _marshaller_KOffice_MainWindow );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = createMainWindow();
      __res.value( _marshaller_KOffice_MainWindow, &_res );
      _req->write_results();
      CORBA::release( _res );
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::Document_ptr KOffice::Document_skel::_this()
{
  return KOffice::Document::_duplicate( this );
}


KOffice::Print_skel::Print_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Print:1.0", "Print" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/Print:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Print_skel>( this ) );
}

KOffice::Print_skel::Print_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Print:1.0", "Print" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Print_skel>( this ) );
}

KOffice::Print_skel::~Print_skel()
{
}

bool KOffice::Print_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "encodedMetaFile" ) == 0 ) {
      CORBA::Long width;
      CORBA::StaticAny _width( CORBA::_stc_long, &width );
      CORBA::Long height;
      CORBA::StaticAny _height( CORBA::_stc_long, &height );
      CORBA::Float scale;
      CORBA::StaticAny _scale( CORBA::_stc_float, &scale );

      char* _res;
      CORBA::StaticAny __res( CORBA::_stc_string );
      _req->add_in_arg( &_width );
      _req->add_in_arg( &_height );
      _req->add_in_arg( &_scale );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = encodedMetaFile( width, height, scale );
      __res.value( CORBA::_stc_string, &_res );
      _req->write_results();
      CORBA::string_free( _res );
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::Print_ptr KOffice::Print_skel::_this()
{
  return KOffice::Print::_duplicate( this );
}


KOffice::View_skel::View_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/View:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

KOffice::View_skel::View_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

KOffice::View_skel::~View_skel()
{
}

bool KOffice::View_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "isMarked" ) == 0 ) {
      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = isMarked();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "setMarked" ) == 0 ) {
      CORBA::Boolean marked;
      CORBA::StaticAny _marked( CORBA::_stc_boolean, &marked );

      _req->add_in_arg( &_marked );

      if( !_req->read_args() )
        return true;

      setMarked( marked );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "setMode" ) == 0 ) {
      Mode mode;
      CORBA::StaticAny _mode( _marshaller_KOffice_View_Mode, &mode );

      _req->add_in_arg( &_mode );

      if( !_req->read_args() )
        return true;

      setMode( mode );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "mode" ) == 0 ) {
      Mode _res;
      CORBA::StaticAny __res( _marshaller_KOffice_View_Mode, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = mode();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "leftGUISize" ) == 0 ) {
      CORBA::ULong _res;
      CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = leftGUISize();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "rightGUISize" ) == 0 ) {
      CORBA::ULong _res;
      CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = rightGUISize();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "topGUISize" ) == 0 ) {
      CORBA::ULong _res;
      CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = topGUISize();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "bottomGUISize" ) == 0 ) {
      CORBA::ULong _res;
      CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = bottomGUISize();
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "printDlg" ) == 0 ) {
      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = printDlg();
      _req->write_results();
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::View_ptr KOffice::View_skel::_this()
{
  return KOffice::View::_duplicate( this );
}


KOffice::DocumentFactory_skel::DocumentFactory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/DocumentFactory:1.0", "DocumentFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/DocumentFactory:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<DocumentFactory_skel>( this ) );
}

KOffice::DocumentFactory_skel::DocumentFactory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/DocumentFactory:1.0", "DocumentFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<DocumentFactory_skel>( this ) );
}

KOffice::DocumentFactory_skel::~DocumentFactory_skel()
{
}

bool KOffice::DocumentFactory_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "create" ) == 0 ) {
      Document_ptr _res;
      CORBA::StaticAny __res( _marshaller_KOffice_Document );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = create();
      __res.value( _marshaller_KOffice_Document, &_res );
      _req->write_results();
      CORBA::release( _res );
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::DocumentFactory_ptr KOffice::DocumentFactory_skel::_this()
{
  return KOffice::DocumentFactory::_duplicate( this );
}


KOffice::Frame_skel::Frame_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Frame:1.0", "Frame" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/Frame:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Frame_skel>( this ) );
}

KOffice::Frame_skel::Frame_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Frame:1.0", "Frame" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Frame_skel>( this ) );
}

KOffice::Frame_skel::~Frame_skel()
{
}

bool KOffice::Frame_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "view" ) == 0 ) {
      View_ptr _res;
      CORBA::StaticAny __res( _marshaller_KOffice_View );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = view();
      __res.value( _marshaller_KOffice_View, &_res );
      _req->write_results();
      CORBA::release( _res );
      return true;
    }
    if( strcmp( _req->op_name(), "viewChangedState" ) == 0 ) {
      CORBA::Boolean is_marked;
      CORBA::StaticAny _is_marked( CORBA::_stc_boolean, &is_marked );
      CORBA::Boolean has_focus;
      CORBA::StaticAny _has_focus( CORBA::_stc_boolean, &has_focus );

      _req->add_in_arg( &_is_marked );
      _req->add_in_arg( &_has_focus );

      if( !_req->read_args() )
        return true;

      viewChangedState( is_marked, has_focus );
      _req->write_results();
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::Frame_ptr KOffice::Frame_skel::_this()
{
  return KOffice::Frame::_duplicate( this );
}


KOffice::MainWindow_skel::MainWindow_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/MainWindow:1.0", "MainWindow" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/MainWindow:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<MainWindow_skel>( this ) );
}

KOffice::MainWindow_skel::MainWindow_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/MainWindow:1.0", "MainWindow" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<MainWindow_skel>( this ) );
}

KOffice::MainWindow_skel::~MainWindow_skel()
{
}

bool KOffice::MainWindow_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "setMarkedPart" ) == 0 ) {
      OpenParts::Id id;
      CORBA::StaticAny _id( CORBA::_stc_ulong, &id );

      _req->add_in_arg( &_id );

      if( !_req->read_args() )
        return true;

      setMarkedPart( id );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "document" ) == 0 ) {
      Document_ptr _res;
      CORBA::StaticAny __res( _marshaller_KOffice_Document );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = document();
      __res.value( _marshaller_KOffice_Document, &_res );
      _req->write_results();
      CORBA::release( _res );
      return true;
    }
    if( strcmp( _req->op_name(), "view" ) == 0 ) {
      View_ptr _res;
      CORBA::StaticAny __res( _marshaller_KOffice_View );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = view();
      __res.value( _marshaller_KOffice_View, &_res );
      _req->write_results();
      CORBA::release( _res );
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::MainWindow_ptr KOffice::MainWindow_skel::_this()
{
  return KOffice::MainWindow::_duplicate( this );
}


KOffice::Filter_skel::Filter_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Filter:1.0", "Filter" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/Filter:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Filter_skel>( this ) );
}

KOffice::Filter_skel::Filter_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Filter:1.0", "Filter" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Filter_skel>( this ) );
}

KOffice::Filter_skel::~Filter_skel()
{
}

bool KOffice::Filter_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "filter" ) == 0 ) {
      Data data;
      CORBA::StaticAny _data( CORBA::_stcseq_octet, &data );
      CORBA::String_var from;
      CORBA::StaticAny _from( CORBA::_stc_string, &from.inout() );
      CORBA::String_var to;
      CORBA::StaticAny _to( CORBA::_stc_string, &to.inout() );

      _req->add_inout_arg( &_data );
      _req->add_in_arg( &_from );
      _req->add_in_arg( &_to );

      if( !_req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        filter( data, from, to );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::KOffice::Filter::FormatError_var &_ex ) {
        _req->set_exception( _ex->_clone() );
        _req->write_results();
        return true;

      } catch( ::KOffice::Filter::UnsupportedFormat_var &_ex ) {
        _req->set_exception( _ex->_clone() );
        _req->write_results();
        return true;

      }
      #endif
      _req->write_results();
      //delete data;
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::Filter_ptr KOffice::Filter_skel::_this()
{
  return KOffice::Filter::_duplicate( this );
}


KOffice::FilterFactory_skel::FilterFactory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/FilterFactory:1.0", "FilterFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/FilterFactory:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<FilterFactory_skel>( this ) );
}

KOffice::FilterFactory_skel::FilterFactory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/FilterFactory:1.0", "FilterFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<FilterFactory_skel>( this ) );
}

KOffice::FilterFactory_skel::~FilterFactory_skel()
{
}

bool KOffice::FilterFactory_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "create" ) == 0 ) {
      Filter_ptr _res;
      CORBA::StaticAny __res( _marshaller_KOffice_Filter );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = create();
      __res.value( _marshaller_KOffice_Filter, &_res );
      _req->write_results();
      CORBA::release( _res );
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::FilterFactory_ptr KOffice::FilterFactory_skel::_this()
{
  return KOffice::FilterFactory::_duplicate( this );
}


KOffice::Callback_skel::Callback_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Callback:1.0", "Callback" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KOffice/Callback:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Callback_skel>( this ) );
}

KOffice::Callback_skel::Callback_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KOffice/Callback:1.0", "Callback" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<Callback_skel>( this ) );
}

KOffice::Callback_skel::~Callback_skel()
{
}

bool KOffice::Callback_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "callback" ) == 0 ) {

      if( !_req->read_args() )
        return true;

      callback();
      _req->write_results();
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_var &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KOffice::Callback_ptr KOffice::Callback_skel::_this()
{
  return KOffice::Callback::_duplicate( this );
}

