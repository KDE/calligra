/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <trader.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface Link
CosTrading::Link::~Link()
{
}

CosTrading::Link_ptr CosTrading::Link::_duplicate( Link_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::Link::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/Link:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::Link::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/Link:1.0" ) == 0) {
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

CosTrading::Link_ptr CosTrading::Link::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/Link:1.0" )))
      return _duplicate( (CosTrading::Link_ptr) _p );
  }
  return _nil();
}

CosTrading::Link_ptr CosTrading::Link::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * Link::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::Link::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Link; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Link;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::Link_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Link" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::Link_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::Link::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::Link::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Proxy
CosTrading::Proxy::~Proxy()
{
}

CosTrading::Proxy_ptr CosTrading::Proxy::_duplicate( Proxy_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::Proxy::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/Proxy:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::Proxy::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/Proxy:1.0" ) == 0) {
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

CosTrading::Proxy_ptr CosTrading::Proxy::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/Proxy:1.0" )))
      return _duplicate( (CosTrading::Proxy_ptr) _p );
  }
  return _nil();
}

CosTrading::Proxy_ptr CosTrading::Proxy::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * Proxy::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::Proxy::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Proxy; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Proxy;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::Proxy_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Proxy" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::Proxy_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::Proxy::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::Proxy::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Admin::_tc_OctetSeq; };
#else
CORBA::TypeCodeConst CosTrading::Admin::_tc_OctetSeq;
#endif


// Stub interface Admin
CosTrading::Admin::~Admin()
{
}

CosTrading::Admin_ptr CosTrading::Admin::_duplicate( Admin_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::Admin::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/Admin:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::Admin::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/Admin:1.0" ) == 0) {
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

CosTrading::Admin_ptr CosTrading::Admin::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/Admin:1.0" )))
      return _duplicate( (CosTrading::Admin_ptr) _p );
  }
  return _nil();
}

CosTrading::Admin_ptr CosTrading::Admin::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * Admin::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::Admin::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Admin; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Admin;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::Admin_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Admin" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::Admin_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::Admin::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::Admin::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Istring; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Istring;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_TypeRepository; };
#else
CORBA::TypeCodeConst CosTrading::_tc_TypeRepository;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PropertyName; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PropertyName;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PropertyNameSeq; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PropertyNameSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PropertyValue; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PropertyValue;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Property; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Property;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Property::Property()
{
}

CosTrading::Property::Property( const Property& _s )
{
  name = ((Property&)_s).name;
  value = ((Property&)_s).value;
  is_file = ((Property&)_s).is_file;
}

CosTrading::Property::~Property()
{
}

CosTrading::Property&
CosTrading::Property::operator=( const Property& _s )
{
  name = ((Property&)_s).name;
  value = ((Property&)_s).value;
  is_file = ((Property&)_s).is_file;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Property &_s )
{
  _a.type( CosTrading::_tc_Property );
  return (_a.struct_put_begin() &&
    (_a <<= ((CosTrading::Property&)_s).name) &&
    (_a <<= ((CosTrading::Property&)_s).value) &&
    (_a <<= CORBA::Any::from_boolean( ((CosTrading::Property&)_s).is_file )) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Property &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.name) &&
    (_a >>= _s.value) &&
    (_a >>= CORBA::Any::to_boolean( _s.is_file )) &&
    _a.struct_get_end() );
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PropertySeq; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PropertySeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Offer; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Offer;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Offer::Offer()
{
}

CosTrading::Offer::Offer( const Offer& _s )
{
  reference = ((Offer&)_s).reference;
  properties = ((Offer&)_s).properties;
}

CosTrading::Offer::~Offer()
{
}

CosTrading::Offer&
CosTrading::Offer::operator=( const Offer& _s )
{
  reference = ((Offer&)_s).reference;
  properties = ((Offer&)_s).properties;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Offer &_s )
{
  _a.type( CosTrading::_tc_Offer );
  return (_a.struct_put_begin() &&
    (_a <<= CORBA::Any::from_object( ((CosTrading::Offer&)_s).reference, "Object" )) &&
    (_a <<= ((CosTrading::Offer&)_s).properties) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Offer &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= CORBA::Any::to_object( _s.reference )) &&
    (_a >>= _s.properties) &&
    _a.struct_get_end() );
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_OfferSeq; };
#else
CORBA::TypeCodeConst CosTrading::_tc_OfferSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_OfferId; };
#else
CORBA::TypeCodeConst CosTrading::_tc_OfferId;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_OfferIdSeq; };
#else
CORBA::TypeCodeConst CosTrading::_tc_OfferIdSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_ServiceTypeName; };
#else
CORBA::TypeCodeConst CosTrading::_tc_ServiceTypeName;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Constraint; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Constraint;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_FollowOption; };
#else
CORBA::TypeCodeConst CosTrading::_tc_FollowOption;
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::FollowOption &_e )
{
  _a.type( CosTrading::_tc_FollowOption );
  return (_a.enum_put( (CORBA::ULong) _e ));
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::FollowOption &_e )
{
  CORBA::ULong _ul;
  if( !_a.enum_get( _ul ) )
    return FALSE;
  _e = (CosTrading::FollowOption) _ul;
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PolicyName; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PolicyName;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PolicyNameSeq; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PolicyNameSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PolicyValue; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PolicyValue;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Policy; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Policy;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Policy::Policy()
{
}

CosTrading::Policy::Policy( const Policy& _s )
{
  name = ((Policy&)_s).name;
  value = ((Policy&)_s).value;
}

CosTrading::Policy::~Policy()
{
}

CosTrading::Policy&
CosTrading::Policy::operator=( const Policy& _s )
{
  name = ((Policy&)_s).name;
  value = ((Policy&)_s).value;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Policy &_s )
{
  _a.type( CosTrading::_tc_Policy );
  return (_a.struct_put_begin() &&
    (_a <<= ((CosTrading::Policy&)_s).name) &&
    (_a <<= ((CosTrading::Policy&)_s).value) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Policy &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.name) &&
    (_a >>= _s.value) &&
    _a.struct_get_end() );
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PolicySeq; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PolicySeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_UnknownMaxLeft; };
#else
CORBA::TypeCodeConst CosTrading::_tc_UnknownMaxLeft;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::UnknownMaxLeft::UnknownMaxLeft()
{
}

CosTrading::UnknownMaxLeft::UnknownMaxLeft( const UnknownMaxLeft& _s )
{
}

CosTrading::UnknownMaxLeft::~UnknownMaxLeft()
{
}

CosTrading::UnknownMaxLeft&
CosTrading::UnknownMaxLeft::operator=( const UnknownMaxLeft& _s )
{
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::UnknownMaxLeft &_e )
{
  _a.type( CosTrading::_tc_UnknownMaxLeft );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/UnknownMaxLeft:1.0" ) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::UnknownMaxLeft &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    _a.except_get_end() );
}

void CosTrading::UnknownMaxLeft::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw UnknownMaxLeft_var( (CosTrading::UnknownMaxLeft*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::UnknownMaxLeft::_repoid() const
{
  return "IDL:omg.org/CosTrading/UnknownMaxLeft:1.0";
}

void CosTrading::UnknownMaxLeft::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::UnknownMaxLeft::_clone() const
{
  return new UnknownMaxLeft( *this );
}

CosTrading::UnknownMaxLeft *CosTrading::UnknownMaxLeft::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/UnknownMaxLeft:1.0" ) )
    return (UnknownMaxLeft *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_NotImplemented; };
#else
CORBA::TypeCodeConst CosTrading::_tc_NotImplemented;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::NotImplemented::NotImplemented()
{
}

CosTrading::NotImplemented::NotImplemented( const NotImplemented& _s )
{
}

CosTrading::NotImplemented::~NotImplemented()
{
}

CosTrading::NotImplemented&
CosTrading::NotImplemented::operator=( const NotImplemented& _s )
{
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::NotImplemented &_e )
{
  _a.type( CosTrading::_tc_NotImplemented );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/NotImplemented:1.0" ) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::NotImplemented &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    _a.except_get_end() );
}

void CosTrading::NotImplemented::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw NotImplemented_var( (CosTrading::NotImplemented*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::NotImplemented::_repoid() const
{
  return "IDL:omg.org/CosTrading/NotImplemented:1.0";
}

void CosTrading::NotImplemented::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::NotImplemented::_clone() const
{
  return new NotImplemented( *this );
}

CosTrading::NotImplemented *CosTrading::NotImplemented::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/NotImplemented:1.0" ) )
    return (NotImplemented *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_IllegalServiceType; };
#else
CORBA::TypeCodeConst CosTrading::_tc_IllegalServiceType;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::IllegalServiceType::IllegalServiceType()
{
}

CosTrading::IllegalServiceType::IllegalServiceType( const IllegalServiceType& _s )
{
  type = ((IllegalServiceType&)_s).type;
}

CosTrading::IllegalServiceType::~IllegalServiceType()
{
}

CosTrading::IllegalServiceType&
CosTrading::IllegalServiceType::operator=( const IllegalServiceType& _s )
{
  type = ((IllegalServiceType&)_s).type;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::IllegalServiceType &_e )
{
  _a.type( CosTrading::_tc_IllegalServiceType );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/IllegalServiceType:1.0" ) &&
    (_a <<= ((CosTrading::IllegalServiceType&)_e).type) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::IllegalServiceType &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.type) &&
    _a.except_get_end() );
}

void CosTrading::IllegalServiceType::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw IllegalServiceType_var( (CosTrading::IllegalServiceType*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::IllegalServiceType::_repoid() const
{
  return "IDL:omg.org/CosTrading/IllegalServiceType:1.0";
}

void CosTrading::IllegalServiceType::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::IllegalServiceType::_clone() const
{
  return new IllegalServiceType( *this );
}

CosTrading::IllegalServiceType *CosTrading::IllegalServiceType::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/IllegalServiceType:1.0" ) )
    return (IllegalServiceType *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_UnknownServiceType; };
#else
CORBA::TypeCodeConst CosTrading::_tc_UnknownServiceType;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::UnknownServiceType::UnknownServiceType()
{
}

CosTrading::UnknownServiceType::UnknownServiceType( const UnknownServiceType& _s )
{
  type = ((UnknownServiceType&)_s).type;
}

CosTrading::UnknownServiceType::~UnknownServiceType()
{
}

CosTrading::UnknownServiceType&
CosTrading::UnknownServiceType::operator=( const UnknownServiceType& _s )
{
  type = ((UnknownServiceType&)_s).type;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::UnknownServiceType &_e )
{
  _a.type( CosTrading::_tc_UnknownServiceType );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/UnknownServiceType:1.0" ) &&
    (_a <<= ((CosTrading::UnknownServiceType&)_e).type) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::UnknownServiceType &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.type) &&
    _a.except_get_end() );
}

void CosTrading::UnknownServiceType::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw UnknownServiceType_var( (CosTrading::UnknownServiceType*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::UnknownServiceType::_repoid() const
{
  return "IDL:omg.org/CosTrading/UnknownServiceType:1.0";
}

void CosTrading::UnknownServiceType::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::UnknownServiceType::_clone() const
{
  return new UnknownServiceType( *this );
}

CosTrading::UnknownServiceType *CosTrading::UnknownServiceType::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/UnknownServiceType:1.0" ) )
    return (UnknownServiceType *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_IllegalPropertyName; };
#else
CORBA::TypeCodeConst CosTrading::_tc_IllegalPropertyName;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::IllegalPropertyName::IllegalPropertyName()
{
}

CosTrading::IllegalPropertyName::IllegalPropertyName( const IllegalPropertyName& _s )
{
  name = ((IllegalPropertyName&)_s).name;
}

CosTrading::IllegalPropertyName::~IllegalPropertyName()
{
}

CosTrading::IllegalPropertyName&
CosTrading::IllegalPropertyName::operator=( const IllegalPropertyName& _s )
{
  name = ((IllegalPropertyName&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::IllegalPropertyName &_e )
{
  _a.type( CosTrading::_tc_IllegalPropertyName );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/IllegalPropertyName:1.0" ) &&
    (_a <<= ((CosTrading::IllegalPropertyName&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::IllegalPropertyName &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTrading::IllegalPropertyName::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw IllegalPropertyName_var( (CosTrading::IllegalPropertyName*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::IllegalPropertyName::_repoid() const
{
  return "IDL:omg.org/CosTrading/IllegalPropertyName:1.0";
}

void CosTrading::IllegalPropertyName::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::IllegalPropertyName::_clone() const
{
  return new IllegalPropertyName( *this );
}

CosTrading::IllegalPropertyName *CosTrading::IllegalPropertyName::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/IllegalPropertyName:1.0" ) )
    return (IllegalPropertyName *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_DuplicatePropertyName; };
#else
CORBA::TypeCodeConst CosTrading::_tc_DuplicatePropertyName;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::DuplicatePropertyName::DuplicatePropertyName()
{
}

CosTrading::DuplicatePropertyName::DuplicatePropertyName( const DuplicatePropertyName& _s )
{
  name = ((DuplicatePropertyName&)_s).name;
}

CosTrading::DuplicatePropertyName::~DuplicatePropertyName()
{
}

CosTrading::DuplicatePropertyName&
CosTrading::DuplicatePropertyName::operator=( const DuplicatePropertyName& _s )
{
  name = ((DuplicatePropertyName&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::DuplicatePropertyName &_e )
{
  _a.type( CosTrading::_tc_DuplicatePropertyName );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/DuplicatePropertyName:1.0" ) &&
    (_a <<= ((CosTrading::DuplicatePropertyName&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::DuplicatePropertyName &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTrading::DuplicatePropertyName::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw DuplicatePropertyName_var( (CosTrading::DuplicatePropertyName*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::DuplicatePropertyName::_repoid() const
{
  return "IDL:omg.org/CosTrading/DuplicatePropertyName:1.0";
}

void CosTrading::DuplicatePropertyName::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::DuplicatePropertyName::_clone() const
{
  return new DuplicatePropertyName( *this );
}

CosTrading::DuplicatePropertyName *CosTrading::DuplicatePropertyName::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/DuplicatePropertyName:1.0" ) )
    return (DuplicatePropertyName *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_PropertyTypeMismatch; };
#else
CORBA::TypeCodeConst CosTrading::_tc_PropertyTypeMismatch;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::PropertyTypeMismatch::PropertyTypeMismatch()
{
}

CosTrading::PropertyTypeMismatch::PropertyTypeMismatch( const PropertyTypeMismatch& _s )
{
  type = ((PropertyTypeMismatch&)_s).type;
  prop = ((PropertyTypeMismatch&)_s).prop;
}

CosTrading::PropertyTypeMismatch::~PropertyTypeMismatch()
{
}

CosTrading::PropertyTypeMismatch&
CosTrading::PropertyTypeMismatch::operator=( const PropertyTypeMismatch& _s )
{
  type = ((PropertyTypeMismatch&)_s).type;
  prop = ((PropertyTypeMismatch&)_s).prop;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::PropertyTypeMismatch &_e )
{
  _a.type( CosTrading::_tc_PropertyTypeMismatch );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/PropertyTypeMismatch:1.0" ) &&
    (_a <<= ((CosTrading::PropertyTypeMismatch&)_e).type) &&
    (_a <<= ((CosTrading::PropertyTypeMismatch&)_e).prop) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::PropertyTypeMismatch &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.type) &&
    (_a >>= _e.prop) &&
    _a.except_get_end() );
}

void CosTrading::PropertyTypeMismatch::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw PropertyTypeMismatch_var( (CosTrading::PropertyTypeMismatch*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::PropertyTypeMismatch::_repoid() const
{
  return "IDL:omg.org/CosTrading/PropertyTypeMismatch:1.0";
}

void CosTrading::PropertyTypeMismatch::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::PropertyTypeMismatch::_clone() const
{
  return new PropertyTypeMismatch( *this );
}

CosTrading::PropertyTypeMismatch *CosTrading::PropertyTypeMismatch::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/PropertyTypeMismatch:1.0" ) )
    return (PropertyTypeMismatch *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_MissingMandatoryProperty; };
#else
CORBA::TypeCodeConst CosTrading::_tc_MissingMandatoryProperty;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::MissingMandatoryProperty::MissingMandatoryProperty()
{
}

CosTrading::MissingMandatoryProperty::MissingMandatoryProperty( const MissingMandatoryProperty& _s )
{
  type = ((MissingMandatoryProperty&)_s).type;
  name = ((MissingMandatoryProperty&)_s).name;
}

CosTrading::MissingMandatoryProperty::~MissingMandatoryProperty()
{
}

CosTrading::MissingMandatoryProperty&
CosTrading::MissingMandatoryProperty::operator=( const MissingMandatoryProperty& _s )
{
  type = ((MissingMandatoryProperty&)_s).type;
  name = ((MissingMandatoryProperty&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::MissingMandatoryProperty &_e )
{
  _a.type( CosTrading::_tc_MissingMandatoryProperty );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/MissingMandatoryProperty:1.0" ) &&
    (_a <<= ((CosTrading::MissingMandatoryProperty&)_e).type) &&
    (_a <<= ((CosTrading::MissingMandatoryProperty&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::MissingMandatoryProperty &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.type) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTrading::MissingMandatoryProperty::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw MissingMandatoryProperty_var( (CosTrading::MissingMandatoryProperty*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::MissingMandatoryProperty::_repoid() const
{
  return "IDL:omg.org/CosTrading/MissingMandatoryProperty:1.0";
}

void CosTrading::MissingMandatoryProperty::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::MissingMandatoryProperty::_clone() const
{
  return new MissingMandatoryProperty( *this );
}

CosTrading::MissingMandatoryProperty *CosTrading::MissingMandatoryProperty::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/MissingMandatoryProperty:1.0" ) )
    return (MissingMandatoryProperty *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_ReadonlyDynamicProperty; };
#else
CORBA::TypeCodeConst CosTrading::_tc_ReadonlyDynamicProperty;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::ReadonlyDynamicProperty::ReadonlyDynamicProperty()
{
}

CosTrading::ReadonlyDynamicProperty::ReadonlyDynamicProperty( const ReadonlyDynamicProperty& _s )
{
  type = ((ReadonlyDynamicProperty&)_s).type;
  name = ((ReadonlyDynamicProperty&)_s).name;
}

CosTrading::ReadonlyDynamicProperty::~ReadonlyDynamicProperty()
{
}

CosTrading::ReadonlyDynamicProperty&
CosTrading::ReadonlyDynamicProperty::operator=( const ReadonlyDynamicProperty& _s )
{
  type = ((ReadonlyDynamicProperty&)_s).type;
  name = ((ReadonlyDynamicProperty&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::ReadonlyDynamicProperty &_e )
{
  _a.type( CosTrading::_tc_ReadonlyDynamicProperty );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/ReadonlyDynamicProperty:1.0" ) &&
    (_a <<= ((CosTrading::ReadonlyDynamicProperty&)_e).type) &&
    (_a <<= ((CosTrading::ReadonlyDynamicProperty&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::ReadonlyDynamicProperty &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.type) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTrading::ReadonlyDynamicProperty::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw ReadonlyDynamicProperty_var( (CosTrading::ReadonlyDynamicProperty*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::ReadonlyDynamicProperty::_repoid() const
{
  return "IDL:omg.org/CosTrading/ReadonlyDynamicProperty:1.0";
}

void CosTrading::ReadonlyDynamicProperty::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::ReadonlyDynamicProperty::_clone() const
{
  return new ReadonlyDynamicProperty( *this );
}

CosTrading::ReadonlyDynamicProperty *CosTrading::ReadonlyDynamicProperty::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/ReadonlyDynamicProperty:1.0" ) )
    return (ReadonlyDynamicProperty *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_IllegalConstraint; };
#else
CORBA::TypeCodeConst CosTrading::_tc_IllegalConstraint;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::IllegalConstraint::IllegalConstraint()
{
}

CosTrading::IllegalConstraint::IllegalConstraint( const IllegalConstraint& _s )
{
  constr = ((IllegalConstraint&)_s).constr;
}

CosTrading::IllegalConstraint::~IllegalConstraint()
{
}

CosTrading::IllegalConstraint&
CosTrading::IllegalConstraint::operator=( const IllegalConstraint& _s )
{
  constr = ((IllegalConstraint&)_s).constr;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::IllegalConstraint &_e )
{
  _a.type( CosTrading::_tc_IllegalConstraint );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/IllegalConstraint:1.0" ) &&
    (_a <<= ((CosTrading::IllegalConstraint&)_e).constr) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::IllegalConstraint &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.constr) &&
    _a.except_get_end() );
}

void CosTrading::IllegalConstraint::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw IllegalConstraint_var( (CosTrading::IllegalConstraint*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::IllegalConstraint::_repoid() const
{
  return "IDL:omg.org/CosTrading/IllegalConstraint:1.0";
}

void CosTrading::IllegalConstraint::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::IllegalConstraint::_clone() const
{
  return new IllegalConstraint( *this );
}

CosTrading::IllegalConstraint *CosTrading::IllegalConstraint::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/IllegalConstraint:1.0" ) )
    return (IllegalConstraint *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_InvalidLookupRef; };
#else
CORBA::TypeCodeConst CosTrading::_tc_InvalidLookupRef;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::InvalidLookupRef::InvalidLookupRef()
{
}

CosTrading::InvalidLookupRef::InvalidLookupRef( const InvalidLookupRef& _s )
{
  target = ((InvalidLookupRef&)_s).target;
}

CosTrading::InvalidLookupRef::~InvalidLookupRef()
{
}

CosTrading::InvalidLookupRef&
CosTrading::InvalidLookupRef::operator=( const InvalidLookupRef& _s )
{
  target = ((InvalidLookupRef&)_s).target;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::InvalidLookupRef &_e )
{
  _a.type( CosTrading::_tc_InvalidLookupRef );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/InvalidLookupRef:1.0" ) &&
    (_a <<= (CosTrading::Lookup_ptr) ((CosTrading::InvalidLookupRef&)_e).target) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::InvalidLookupRef &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= (CosTrading::Lookup_ptr&) _e.target) &&
    _a.except_get_end() );
}

void CosTrading::InvalidLookupRef::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw InvalidLookupRef_var( (CosTrading::InvalidLookupRef*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::InvalidLookupRef::_repoid() const
{
  return "IDL:omg.org/CosTrading/InvalidLookupRef:1.0";
}

void CosTrading::InvalidLookupRef::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::InvalidLookupRef::_clone() const
{
  return new InvalidLookupRef( *this );
}

CosTrading::InvalidLookupRef *CosTrading::InvalidLookupRef::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/InvalidLookupRef:1.0" ) )
    return (InvalidLookupRef *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_IllegalOfferId; };
#else
CORBA::TypeCodeConst CosTrading::_tc_IllegalOfferId;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::IllegalOfferId::IllegalOfferId()
{
}

CosTrading::IllegalOfferId::IllegalOfferId( const IllegalOfferId& _s )
{
  id = ((IllegalOfferId&)_s).id;
}

CosTrading::IllegalOfferId::~IllegalOfferId()
{
}

CosTrading::IllegalOfferId&
CosTrading::IllegalOfferId::operator=( const IllegalOfferId& _s )
{
  id = ((IllegalOfferId&)_s).id;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::IllegalOfferId &_e )
{
  _a.type( CosTrading::_tc_IllegalOfferId );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/IllegalOfferId:1.0" ) &&
    (_a <<= ((CosTrading::IllegalOfferId&)_e).id) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::IllegalOfferId &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.id) &&
    _a.except_get_end() );
}

void CosTrading::IllegalOfferId::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw IllegalOfferId_var( (CosTrading::IllegalOfferId*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::IllegalOfferId::_repoid() const
{
  return "IDL:omg.org/CosTrading/IllegalOfferId:1.0";
}

void CosTrading::IllegalOfferId::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::IllegalOfferId::_clone() const
{
  return new IllegalOfferId( *this );
}

CosTrading::IllegalOfferId *CosTrading::IllegalOfferId::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/IllegalOfferId:1.0" ) )
    return (IllegalOfferId *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_UnknownOfferId; };
#else
CORBA::TypeCodeConst CosTrading::_tc_UnknownOfferId;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::UnknownOfferId::UnknownOfferId()
{
}

CosTrading::UnknownOfferId::UnknownOfferId( const UnknownOfferId& _s )
{
  id = ((UnknownOfferId&)_s).id;
}

CosTrading::UnknownOfferId::~UnknownOfferId()
{
}

CosTrading::UnknownOfferId&
CosTrading::UnknownOfferId::operator=( const UnknownOfferId& _s )
{
  id = ((UnknownOfferId&)_s).id;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::UnknownOfferId &_e )
{
  _a.type( CosTrading::_tc_UnknownOfferId );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/UnknownOfferId:1.0" ) &&
    (_a <<= ((CosTrading::UnknownOfferId&)_e).id) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::UnknownOfferId &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.id) &&
    _a.except_get_end() );
}

void CosTrading::UnknownOfferId::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw UnknownOfferId_var( (CosTrading::UnknownOfferId*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::UnknownOfferId::_repoid() const
{
  return "IDL:omg.org/CosTrading/UnknownOfferId:1.0";
}

void CosTrading::UnknownOfferId::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::UnknownOfferId::_clone() const
{
  return new UnknownOfferId( *this );
}

CosTrading::UnknownOfferId *CosTrading::UnknownOfferId::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/UnknownOfferId:1.0" ) )
    return (UnknownOfferId *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_DuplicatePolicyName; };
#else
CORBA::TypeCodeConst CosTrading::_tc_DuplicatePolicyName;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::DuplicatePolicyName::DuplicatePolicyName()
{
}

CosTrading::DuplicatePolicyName::DuplicatePolicyName( const DuplicatePolicyName& _s )
{
  name = ((DuplicatePolicyName&)_s).name;
}

CosTrading::DuplicatePolicyName::~DuplicatePolicyName()
{
}

CosTrading::DuplicatePolicyName&
CosTrading::DuplicatePolicyName::operator=( const DuplicatePolicyName& _s )
{
  name = ((DuplicatePolicyName&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::DuplicatePolicyName &_e )
{
  _a.type( CosTrading::_tc_DuplicatePolicyName );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/DuplicatePolicyName:1.0" ) &&
    (_a <<= ((CosTrading::DuplicatePolicyName&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::DuplicatePolicyName &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTrading::DuplicatePolicyName::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw DuplicatePolicyName_var( (CosTrading::DuplicatePolicyName*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::DuplicatePolicyName::_repoid() const
{
  return "IDL:omg.org/CosTrading/DuplicatePolicyName:1.0";
}

void CosTrading::DuplicatePolicyName::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::DuplicatePolicyName::_clone() const
{
  return new DuplicatePolicyName( *this );
}

CosTrading::DuplicatePolicyName *CosTrading::DuplicatePolicyName::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/DuplicatePolicyName:1.0" ) )
    return (DuplicatePolicyName *) _ex;
  return NULL;
}


// Stub interface TraderComponents
CosTrading::TraderComponents::~TraderComponents()
{
}

CosTrading::TraderComponents_ptr CosTrading::TraderComponents::_duplicate( TraderComponents_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::TraderComponents::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/TraderComponents:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::TraderComponents::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/TraderComponents:1.0" ) == 0) {
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

CosTrading::TraderComponents_ptr CosTrading::TraderComponents::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/TraderComponents:1.0" )))
      return _duplicate( (CosTrading::TraderComponents_ptr) _p );
  }
  return _nil();
}

CosTrading::TraderComponents_ptr CosTrading::TraderComponents::_nil()
{
  return NULL;
}


#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * TraderComponents::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::TraderComponents::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_TraderComponents; };
#else
CORBA::TypeCodeConst CosTrading::_tc_TraderComponents;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::TraderComponents_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "TraderComponents" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::TraderComponents_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::TraderComponents::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::TraderComponents::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface SupportAttributes
CosTrading::SupportAttributes::~SupportAttributes()
{
}

CosTrading::SupportAttributes_ptr CosTrading::SupportAttributes::_duplicate( SupportAttributes_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::SupportAttributes::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/SupportAttributes:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::SupportAttributes::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/SupportAttributes:1.0" ) == 0) {
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

CosTrading::SupportAttributes_ptr CosTrading::SupportAttributes::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/SupportAttributes:1.0" )))
      return _duplicate( (CosTrading::SupportAttributes_ptr) _p );
  }
  return _nil();
}

CosTrading::SupportAttributes_ptr CosTrading::SupportAttributes::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * SupportAttributes::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::SupportAttributes::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_SupportAttributes; };
#else
CORBA::TypeCodeConst CosTrading::_tc_SupportAttributes;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::SupportAttributes_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "SupportAttributes" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::SupportAttributes_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::SupportAttributes::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::SupportAttributes::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface ImportAttributes
CosTrading::ImportAttributes::~ImportAttributes()
{
}

CosTrading::ImportAttributes_ptr CosTrading::ImportAttributes::_duplicate( ImportAttributes_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::ImportAttributes::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/ImportAttributes:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::ImportAttributes::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/ImportAttributes:1.0" ) == 0) {
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

CosTrading::ImportAttributes_ptr CosTrading::ImportAttributes::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/ImportAttributes:1.0" )))
      return _duplicate( (CosTrading::ImportAttributes_ptr) _p );
  }
  return _nil();
}

CosTrading::ImportAttributes_ptr CosTrading::ImportAttributes::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * ImportAttributes::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::ImportAttributes::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_ImportAttributes; };
#else
CORBA::TypeCodeConst CosTrading::_tc_ImportAttributes;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::ImportAttributes_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "ImportAttributes" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::ImportAttributes_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::ImportAttributes::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::ImportAttributes::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Lookup::_tc_Preference; };
#else
CORBA::TypeCodeConst CosTrading::Lookup::_tc_Preference;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Lookup::_tc_HowManyProps; };
#else
CORBA::TypeCodeConst CosTrading::Lookup::_tc_HowManyProps;
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Lookup::HowManyProps &_e )
{
  _a.type( CosTrading::Lookup::_tc_HowManyProps );
  return (_a.enum_put( (CORBA::ULong) _e ));
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Lookup::HowManyProps &_e )
{
  CORBA::ULong _ul;
  if( !_a.enum_get( _ul ) )
    return FALSE;
  _e = (CosTrading::Lookup::HowManyProps) _ul;
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Lookup::_tc_SpecifiedProps; };
#else
CORBA::TypeCodeConst CosTrading::Lookup::_tc_SpecifiedProps;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Lookup::SpecifiedProps::SpecifiedProps()
{
}

CosTrading::Lookup::SpecifiedProps::SpecifiedProps( const SpecifiedProps&_u )
{
  _discriminator = _u._discriminator;
  _m.prop_names = ((SpecifiedProps&)_u)._m.prop_names;
}

CosTrading::Lookup::SpecifiedProps::~SpecifiedProps()
{
}

CosTrading::Lookup::SpecifiedProps&
CosTrading::Lookup::SpecifiedProps::operator=( const SpecifiedProps&_u )
{
  _discriminator = _u._discriminator;
  _m.prop_names = ((SpecifiedProps&)_u)._m.prop_names;
  return *this;
}
#endif

void CosTrading::Lookup::SpecifiedProps::_d( CosTrading::Lookup::HowManyProps _p )
{
  _discriminator = _p;
}

CosTrading::Lookup::HowManyProps CosTrading::Lookup::SpecifiedProps::_d() const
{
  return _discriminator;
}

void CosTrading::Lookup::SpecifiedProps::prop_names( const CosTrading::PropertyNameSeq& _p )
{
  _discriminator = CosTrading::Lookup::some;
  _m.prop_names = _p;
}

const CosTrading::PropertyNameSeq& CosTrading::Lookup::SpecifiedProps::prop_names() const
{
  return (CosTrading::PropertyNameSeq&) _m.prop_names;
}

CosTrading::PropertyNameSeq& CosTrading::Lookup::SpecifiedProps::prop_names()
{
  return _m.prop_names;
}

void CosTrading::Lookup::SpecifiedProps::_default()
{
  _discriminator = none;
}

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Lookup::SpecifiedProps &_u )
{
  _a.type( CosTrading::Lookup::_tc_SpecifiedProps );
  if (!_a.union_put_begin())
    return FALSE;
  if( !(_a <<= ((CosTrading::Lookup::SpecifiedProps&)_u)._discriminator) )
    return FALSE;
  switch( _u._d() ) {
    case CosTrading::Lookup::some:
      if( !_a.union_put_selection( 0 ) )
        return FALSE;
      if( !(_a <<= ((CosTrading::Lookup::SpecifiedProps&)_u)._m.prop_names) )
        return FALSE;
      break;
    default:
      break;
  }
  return _a.union_put_end();
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Lookup::SpecifiedProps &_u )
{
  if( !_a.union_get_begin() )
    return FALSE;
  if( !(_a >>= _u._discriminator) )
    return FALSE;
  switch( _u._discriminator ) {
    case CosTrading::Lookup::some:
      if( !_a.union_get_selection( 0 ) )
        return FALSE;
      if( !(_a >>= _u._m.prop_names) )
        return FALSE;
      break;
    default:
      break;
  }
  return _a.union_get_end();
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Lookup::_tc_IllegalPreference; };
#else
CORBA::TypeCodeConst CosTrading::Lookup::_tc_IllegalPreference;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Lookup::IllegalPreference::IllegalPreference()
{
}

CosTrading::Lookup::IllegalPreference::IllegalPreference( const IllegalPreference& _s )
{
  pref = ((IllegalPreference&)_s).pref;
}

CosTrading::Lookup::IllegalPreference::~IllegalPreference()
{
}

CosTrading::Lookup::IllegalPreference&
CosTrading::Lookup::IllegalPreference::operator=( const IllegalPreference& _s )
{
  pref = ((IllegalPreference&)_s).pref;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Lookup::IllegalPreference &_e )
{
  _a.type( CosTrading::Lookup::_tc_IllegalPreference );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/Lookup/IllegalPreference:1.0" ) &&
    (_a <<= ((CosTrading::Lookup::IllegalPreference&)_e).pref) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Lookup::IllegalPreference &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.pref) &&
    _a.except_get_end() );
}

void CosTrading::Lookup::IllegalPreference::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw IllegalPreference_var( (CosTrading::Lookup::IllegalPreference*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::Lookup::IllegalPreference::_repoid() const
{
  return "IDL:omg.org/CosTrading/Lookup/IllegalPreference:1.0";
}

void CosTrading::Lookup::IllegalPreference::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::Lookup::IllegalPreference::_clone() const
{
  return new IllegalPreference( *this );
}

CosTrading::Lookup::IllegalPreference *CosTrading::Lookup::IllegalPreference::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/Lookup/IllegalPreference:1.0" ) )
    return (IllegalPreference *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Lookup::_tc_IllegalPolicyName; };
#else
CORBA::TypeCodeConst CosTrading::Lookup::_tc_IllegalPolicyName;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Lookup::IllegalPolicyName::IllegalPolicyName()
{
}

CosTrading::Lookup::IllegalPolicyName::IllegalPolicyName( const IllegalPolicyName& _s )
{
  name = ((IllegalPolicyName&)_s).name;
}

CosTrading::Lookup::IllegalPolicyName::~IllegalPolicyName()
{
}

CosTrading::Lookup::IllegalPolicyName&
CosTrading::Lookup::IllegalPolicyName::operator=( const IllegalPolicyName& _s )
{
  name = ((IllegalPolicyName&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Lookup::IllegalPolicyName &_e )
{
  _a.type( CosTrading::Lookup::_tc_IllegalPolicyName );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/Lookup/IllegalPolicyName:1.0" ) &&
    (_a <<= ((CosTrading::Lookup::IllegalPolicyName&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Lookup::IllegalPolicyName &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTrading::Lookup::IllegalPolicyName::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw IllegalPolicyName_var( (CosTrading::Lookup::IllegalPolicyName*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::Lookup::IllegalPolicyName::_repoid() const
{
  return "IDL:omg.org/CosTrading/Lookup/IllegalPolicyName:1.0";
}

void CosTrading::Lookup::IllegalPolicyName::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::Lookup::IllegalPolicyName::_clone() const
{
  return new IllegalPolicyName( *this );
}

CosTrading::Lookup::IllegalPolicyName *CosTrading::Lookup::IllegalPolicyName::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/Lookup/IllegalPolicyName:1.0" ) )
    return (IllegalPolicyName *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Lookup::_tc_PolicyTypeMismatch; };
#else
CORBA::TypeCodeConst CosTrading::Lookup::_tc_PolicyTypeMismatch;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Lookup::PolicyTypeMismatch::PolicyTypeMismatch()
{
}

CosTrading::Lookup::PolicyTypeMismatch::PolicyTypeMismatch( const PolicyTypeMismatch& _s )
{
  the_policy = ((PolicyTypeMismatch&)_s).the_policy;
}

CosTrading::Lookup::PolicyTypeMismatch::~PolicyTypeMismatch()
{
}

CosTrading::Lookup::PolicyTypeMismatch&
CosTrading::Lookup::PolicyTypeMismatch::operator=( const PolicyTypeMismatch& _s )
{
  the_policy = ((PolicyTypeMismatch&)_s).the_policy;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Lookup::PolicyTypeMismatch &_e )
{
  _a.type( CosTrading::Lookup::_tc_PolicyTypeMismatch );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/Lookup/PolicyTypeMismatch:1.0" ) &&
    (_a <<= ((CosTrading::Lookup::PolicyTypeMismatch&)_e).the_policy) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Lookup::PolicyTypeMismatch &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.the_policy) &&
    _a.except_get_end() );
}

void CosTrading::Lookup::PolicyTypeMismatch::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw PolicyTypeMismatch_var( (CosTrading::Lookup::PolicyTypeMismatch*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::Lookup::PolicyTypeMismatch::_repoid() const
{
  return "IDL:omg.org/CosTrading/Lookup/PolicyTypeMismatch:1.0";
}

void CosTrading::Lookup::PolicyTypeMismatch::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::Lookup::PolicyTypeMismatch::_clone() const
{
  return new PolicyTypeMismatch( *this );
}

CosTrading::Lookup::PolicyTypeMismatch *CosTrading::Lookup::PolicyTypeMismatch::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/Lookup/PolicyTypeMismatch:1.0" ) )
    return (PolicyTypeMismatch *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Lookup::_tc_InvalidPolicyValue; };
#else
CORBA::TypeCodeConst CosTrading::Lookup::_tc_InvalidPolicyValue;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Lookup::InvalidPolicyValue::InvalidPolicyValue()
{
}

CosTrading::Lookup::InvalidPolicyValue::InvalidPolicyValue( const InvalidPolicyValue& _s )
{
  the_policy = ((InvalidPolicyValue&)_s).the_policy;
}

CosTrading::Lookup::InvalidPolicyValue::~InvalidPolicyValue()
{
}

CosTrading::Lookup::InvalidPolicyValue&
CosTrading::Lookup::InvalidPolicyValue::operator=( const InvalidPolicyValue& _s )
{
  the_policy = ((InvalidPolicyValue&)_s).the_policy;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Lookup::InvalidPolicyValue &_e )
{
  _a.type( CosTrading::Lookup::_tc_InvalidPolicyValue );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/Lookup/InvalidPolicyValue:1.0" ) &&
    (_a <<= ((CosTrading::Lookup::InvalidPolicyValue&)_e).the_policy) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Lookup::InvalidPolicyValue &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.the_policy) &&
    _a.except_get_end() );
}

void CosTrading::Lookup::InvalidPolicyValue::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw InvalidPolicyValue_var( (CosTrading::Lookup::InvalidPolicyValue*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::Lookup::InvalidPolicyValue::_repoid() const
{
  return "IDL:omg.org/CosTrading/Lookup/InvalidPolicyValue:1.0";
}

void CosTrading::Lookup::InvalidPolicyValue::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::Lookup::InvalidPolicyValue::_clone() const
{
  return new InvalidPolicyValue( *this );
}

CosTrading::Lookup::InvalidPolicyValue *CosTrading::Lookup::InvalidPolicyValue::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/Lookup/InvalidPolicyValue:1.0" ) )
    return (InvalidPolicyValue *) _ex;
  return NULL;
}


// Stub interface Lookup
CosTrading::Lookup::~Lookup()
{
}

CosTrading::Lookup_ptr CosTrading::Lookup::_duplicate( Lookup_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::Lookup::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/Lookup:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = TraderComponents::_narrow_helper( _repoid )))
      return _p;
  }
  {
    void *_p;
    if( (_p = SupportAttributes::_narrow_helper( _repoid )))
      return _p;
  }
  {
    void *_p;
    if( (_p = ImportAttributes::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool CosTrading::Lookup::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/Lookup:1.0" ) == 0) {
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

CosTrading::Lookup_ptr CosTrading::Lookup::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/Lookup:1.0" )))
      return _duplicate( (CosTrading::Lookup_ptr) _p );
  }
  return _nil();
}

CosTrading::Lookup_ptr CosTrading::Lookup::_nil()
{
  return NULL;
}

struct _global_init_CosTrading_Lookup {
  _global_init_CosTrading_Lookup()
  {
    if( ::CosTrading::TraderComponents::_narrow_helpers == NULL )
      ::CosTrading::TraderComponents::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::CosTrading::TraderComponents::_narrow_helpers->push_back( CosTrading::Lookup::_narrow_helper2 );
    if( ::CosTrading::SupportAttributes::_narrow_helpers == NULL )
      ::CosTrading::SupportAttributes::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::CosTrading::SupportAttributes::_narrow_helpers->push_back( CosTrading::Lookup::_narrow_helper2 );
    if( ::CosTrading::ImportAttributes::_narrow_helpers == NULL )
      ::CosTrading::ImportAttributes::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::CosTrading::ImportAttributes::_narrow_helpers->push_back( CosTrading::Lookup::_narrow_helper2 );
  }
} __global_init_CosTrading_Lookup;

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * Lookup::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::Lookup::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Lookup; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Lookup;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::Lookup_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Lookup" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::Lookup_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::Lookup::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::Lookup::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Register::_tc_InterfaceTypeMismatch; };
#else
CORBA::TypeCodeConst CosTrading::Register::_tc_InterfaceTypeMismatch;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Register::InterfaceTypeMismatch::InterfaceTypeMismatch()
{
}

CosTrading::Register::InterfaceTypeMismatch::InterfaceTypeMismatch( const InterfaceTypeMismatch& _s )
{
  type = ((InterfaceTypeMismatch&)_s).type;
  reference = ((InterfaceTypeMismatch&)_s).reference;
}

CosTrading::Register::InterfaceTypeMismatch::~InterfaceTypeMismatch()
{
}

CosTrading::Register::InterfaceTypeMismatch&
CosTrading::Register::InterfaceTypeMismatch::operator=( const InterfaceTypeMismatch& _s )
{
  type = ((InterfaceTypeMismatch&)_s).type;
  reference = ((InterfaceTypeMismatch&)_s).reference;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Register::InterfaceTypeMismatch &_e )
{
  _a.type( CosTrading::Register::_tc_InterfaceTypeMismatch );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/Register/InterfaceTypeMismatch:1.0" ) &&
    (_a <<= ((CosTrading::Register::InterfaceTypeMismatch&)_e).type) &&
    (_a <<= CORBA::Any::from_object( ((CosTrading::Register::InterfaceTypeMismatch&)_e).reference, "Object" )) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Register::InterfaceTypeMismatch &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.type) &&
    (_a >>= CORBA::Any::to_object( _e.reference )) &&
    _a.except_get_end() );
}

void CosTrading::Register::InterfaceTypeMismatch::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw InterfaceTypeMismatch_var( (CosTrading::Register::InterfaceTypeMismatch*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::Register::InterfaceTypeMismatch::_repoid() const
{
  return "IDL:omg.org/CosTrading/Register/InterfaceTypeMismatch:1.0";
}

void CosTrading::Register::InterfaceTypeMismatch::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::Register::InterfaceTypeMismatch::_clone() const
{
  return new InterfaceTypeMismatch( *this );
}

CosTrading::Register::InterfaceTypeMismatch *CosTrading::Register::InterfaceTypeMismatch::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/Register/InterfaceTypeMismatch:1.0" ) )
    return (InterfaceTypeMismatch *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst Register::_tc_InvalidObjectRef; };
#else
CORBA::TypeCodeConst CosTrading::Register::_tc_InvalidObjectRef;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTrading::Register::InvalidObjectRef::InvalidObjectRef()
{
}

CosTrading::Register::InvalidObjectRef::InvalidObjectRef( const InvalidObjectRef& _s )
{
  ref = ((InvalidObjectRef&)_s).ref;
}

CosTrading::Register::InvalidObjectRef::~InvalidObjectRef()
{
}

CosTrading::Register::InvalidObjectRef&
CosTrading::Register::InvalidObjectRef::operator=( const InvalidObjectRef& _s )
{
  ref = ((InvalidObjectRef&)_s).ref;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTrading::Register::InvalidObjectRef &_e )
{
  _a.type( CosTrading::Register::_tc_InvalidObjectRef );
  return (_a.except_put_begin( "IDL:omg.org/CosTrading/Register/InvalidObjectRef:1.0" ) &&
    (_a <<= CORBA::Any::from_object( ((CosTrading::Register::InvalidObjectRef&)_e).ref, "Object" )) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTrading::Register::InvalidObjectRef &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= CORBA::Any::to_object( _e.ref )) &&
    _a.except_get_end() );
}

void CosTrading::Register::InvalidObjectRef::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw InvalidObjectRef_var( (CosTrading::Register::InvalidObjectRef*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTrading::Register::InvalidObjectRef::_repoid() const
{
  return "IDL:omg.org/CosTrading/Register/InvalidObjectRef:1.0";
}

void CosTrading::Register::InvalidObjectRef::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTrading::Register::InvalidObjectRef::_clone() const
{
  return new InvalidObjectRef( *this );
}

CosTrading::Register::InvalidObjectRef *CosTrading::Register::InvalidObjectRef::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:omg.org/CosTrading/Register/InvalidObjectRef:1.0" ) )
    return (InvalidObjectRef *) _ex;
  return NULL;
}


// Stub interface Register
CosTrading::Register::~Register()
{
}

CosTrading::Register_ptr CosTrading::Register::_duplicate( Register_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::Register::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/Register:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::Register::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/Register:1.0" ) == 0) {
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

CosTrading::Register_ptr CosTrading::Register::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/Register:1.0" )))
      return _duplicate( (CosTrading::Register_ptr) _p );
  }
  return _nil();
}

CosTrading::Register_ptr CosTrading::Register::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * Register::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::Register::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_Register; };
#else
CORBA::TypeCodeConst CosTrading::_tc_Register;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::Register_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Register" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::Register_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::Register::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::Register::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface OfferIterator
CosTrading::OfferIterator::~OfferIterator()
{
}

CosTrading::OfferIterator_ptr CosTrading::OfferIterator::_duplicate( OfferIterator_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTrading::OfferIterator::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:omg.org/CosTrading/OfferIterator:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTrading::OfferIterator::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/OfferIterator:1.0" ) == 0) {
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

CosTrading::OfferIterator_ptr CosTrading::OfferIterator::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:omg.org/CosTrading/OfferIterator:1.0" )))
      return _duplicate( (CosTrading::OfferIterator_ptr) _p );
  }
  return _nil();
}

CosTrading::OfferIterator_ptr CosTrading::OfferIterator::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { vector<CORBA::Narrow_proto> * OfferIterator::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTrading::OfferIterator::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_OfferIterator; };
#else
CORBA::TypeCodeConst CosTrading::_tc_OfferIterator;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTrading::OfferIterator_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "OfferIterator" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTrading::OfferIterator_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTrading::OfferIterator::_nil();
    return TRUE;
  }
  _obj = ::CosTrading::OfferIterator::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_LongList; };
#else
CORBA::TypeCodeConst CosTrading::_tc_LongList;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_FloatList; };
#else
CORBA::TypeCodeConst CosTrading::_tc_FloatList;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTrading { CORBA::TypeCodeConst _tc_StringList; };
#else
CORBA::TypeCodeConst CosTrading::_tc_StringList;
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTrading::Property> &_s )
{
  static CORBA::TypeCodeConst _tc =
    "010000001300000060010000010000000f00000050010000010000002400"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f50726f7065"
    "7274793a312e30000900000050726f706572747900000000030000000500"
    "00006e616d6500000000150000008c000000010000002800000049444c3a"
    "6f6d672e6f72672f436f7354726164696e672f50726f70657274794e616d"
    "653a312e30000d00000050726f70657274794e616d650000000015000000"
    "40000000010000002300000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f49737472696e673a312e3000000800000049737472696e6700"
    "12000000000000000600000076616c7565000000150000004c0000000100"
    "00002900000049444c3a6f6d672e6f72672f436f7354726164696e672f50"
    "726f706572747956616c75653a312e30000000000e00000050726f706572"
    "747956616c75650000000b0000000800000069735f66696c650008000000"
    "00000000";
  _a.type( _tc );
  if( !_a.seq_put_begin( _s.length() ) )
    return FALSE;
  for( CORBA::ULong _i = 0; _i < _s.length(); _i++ )
    if( !(_a <<= ((SequenceTmpl<CosTrading::Property>&)_s)[ _i ]) )
      return FALSE;
  return _a.seq_put_end();
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTrading::Property> &_s )
{
  CORBA::ULong _len;

  if( !_a.seq_get_begin( _len ) )
    return FALSE;
  _s.length( _len );
  for( CORBA::ULong _i = 0; _i < _len; _i++ )
    if( !(_a >>= _s[ _i ]) )
      return FALSE;
  return _a.seq_get_end();
}


CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTrading::Offer> &_s )
{
  static CORBA::TypeCodeConst _tc =
    "010000001300000058020000010000000f00000048020000010000002100"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f4f66666572"
    "3a312e3000000000060000004f66666572000000020000000a0000007265"
    "666572656e63650000000e00000033000000010000001d00000049444c3a"
    "6f6d672e6f72672f434f5242412f4f626a6563743a312e30000000000700"
    "00004f626a65637400000b00000070726f70657274696573000015000000"
    "a8010000010000002700000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f50726f70657274795365713a312e3000000c00000050726f70"
    "65727479536571001300000060010000010000000f000000500100000100"
    "00002400000049444c3a6f6d672e6f72672f436f7354726164696e672f50"
    "726f70657274793a312e30000900000050726f7065727479000000000300"
    "0000050000006e616d6500000000150000008c0000000100000028000000"
    "49444c3a6f6d672e6f72672f436f7354726164696e672f50726f70657274"
    "794e616d653a312e30000d00000050726f70657274794e616d6500000000"
    "1500000040000000010000002300000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f49737472696e673a312e3000000800000049737472"
    "696e670012000000000000000600000076616c7565000000150000004c00"
    "0000010000002900000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f50726f706572747956616c75653a312e30000000000e0000005072"
    "6f706572747956616c75650000000b0000000800000069735f66696c6500"
    "080000000000000000000000";
  _a.type( _tc );
  if( !_a.seq_put_begin( _s.length() ) )
    return FALSE;
  for( CORBA::ULong _i = 0; _i < _s.length(); _i++ )
    if( !(_a <<= ((SequenceTmpl<CosTrading::Offer>&)_s)[ _i ]) )
      return FALSE;
  return _a.seq_put_end();
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTrading::Offer> &_s )
{
  CORBA::ULong _len;

  if( !_a.seq_get_begin( _len ) )
    return FALSE;
  _s.length( _len );
  for( CORBA::ULong _i = 0; _i < _len; _i++ )
    if( !(_a >>= _s[ _i ]) )
      return FALSE;
  return _a.seq_get_end();
}


CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTrading::Policy> &_s )
{
  static CORBA::TypeCodeConst _tc =
    "010000001300000000010000010000000f000000f0000000010000002200"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f506f6c6963"
    "793a312e3000000007000000506f6c696379000002000000050000006e61"
    "6d65000000001500000048000000010000002600000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f506f6c6963794e616d653a312e3000"
    "00000b000000506f6c6963794e616d650000120000000000000006000000"
    "76616c75650000001500000044000000010000002700000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f506f6c69637956616c75653a31"
    "2e3000000c000000506f6c69637956616c7565000b00000000000000";
  _a.type( _tc );
  if( !_a.seq_put_begin( _s.length() ) )
    return FALSE;
  for( CORBA::ULong _i = 0; _i < _s.length(); _i++ )
    if( !(_a <<= ((SequenceTmpl<CosTrading::Policy>&)_s)[ _i ]) )
      return FALSE;
  return _a.seq_put_end();
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTrading::Policy> &_s )
{
  CORBA::ULong _len;

  if( !_a.seq_get_begin( _len ) )
    return FALSE;
  _s.length( _len );
  for( CORBA::ULong _i = 0; _i < _len; _i++ )
    if( !(_a >>= _s[ _i ]) )
      return FALSE;
  return _a.seq_get_end();
}


struct __tc_init_TRADER {
  __tc_init_TRADER()
  {
    CosTrading::_tc_Link = "010000000e00000031000000010000002000000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c696e6b3a312e3000050000004c696e6b"
    "00";
    CosTrading::_tc_Proxy = "010000000e00000036000000010000002100000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f78793a312e300000000006000000"
    "50726f787900";
    CosTrading::Admin::_tc_OctetSeq = "010000001500000058000000010000002a00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f41646d696e2f4f637465745365713a312e"
    "30000000090000004f6374657453657100000000130000000c0000000100"
    "00000a00000000000000";
    CosTrading::_tc_Admin = "010000000e00000036000000010000002100000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f41646d696e3a312e300000000006000000"
    "41646d696e00";
    CosTrading::_tc_Istring = "010000001500000040000000010000002300000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f49737472696e673a312e30000008000000"
    "49737472696e67001200000000000000";
    CosTrading::_tc_TypeRepository = "010000001500000083000000010000002a00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f547970655265706f7369746f72793a312e"
    "300000000f000000547970655265706f7369746f727900000e0000003300"
    "0000010000001d00000049444c3a6f6d672e6f72672f434f5242412f4f62"
    "6a6563743a312e3000000000070000004f626a65637400";
    CosTrading::_tc_PropertyName = "01000000150000008c000000010000002800000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f70657274794e616d653a312e3000"
    "0d00000050726f70657274794e616d650000000015000000400000000100"
    "00002300000049444c3a6f6d672e6f72672f436f7354726164696e672f49"
    "737472696e673a312e3000000800000049737472696e6700120000000000"
    "0000";
    CosTrading::_tc_PropertyNameSeq = "0100000015000000ec000000010000002b00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f70657274794e616d655365713a31"
    "2e3000001000000050726f70657274794e616d6553657100130000009c00"
    "000001000000150000008c000000010000002800000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f50726f70657274794e616d653a312e"
    "30000d00000050726f70657274794e616d65000000001500000040000000"
    "010000002300000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f49737472696e673a312e3000000800000049737472696e670012000000"
    "0000000000000000";
    CosTrading::_tc_PropertyValue = "01000000150000004c000000010000002900000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f706572747956616c75653a312e30"
    "000000000e00000050726f706572747956616c75650000000b000000";
    CosTrading::_tc_Property = "010000000f00000050010000010000002400000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f70657274793a312e300009000000"
    "50726f70657274790000000003000000050000006e616d65000000001500"
    "00008c000000010000002800000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f50726f70657274794e616d653a312e30000d0000005072"
    "6f70657274794e616d650000000015000000400000000100000023000000"
    "49444c3a6f6d672e6f72672f436f7354726164696e672f49737472696e67"
    "3a312e3000000800000049737472696e6700120000000000000006000000"
    "76616c7565000000150000004c000000010000002900000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f50726f706572747956616c7565"
    "3a312e30000000000e00000050726f706572747956616c75650000000b00"
    "00000800000069735f66696c650008000000";
    CosTrading::_tc_PropertySeq = "0100000015000000a8010000010000002700000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f70657274795365713a312e300000"
    "0c00000050726f7065727479536571001300000060010000010000000f00"
    "000050010000010000002400000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f50726f70657274793a312e30000900000050726f706572"
    "74790000000003000000050000006e616d6500000000150000008c000000"
    "010000002800000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f50726f70657274794e616d653a312e30000d00000050726f7065727479"
    "4e616d65000000001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e670012000000000000000600000076616c756500"
    "0000150000004c000000010000002900000049444c3a6f6d672e6f72672f"
    "436f7354726164696e672f50726f706572747956616c75653a312e300000"
    "00000e00000050726f706572747956616c75650000000b00000008000000"
    "69735f66696c65000800000000000000";
    CosTrading::_tc_Offer = "010000000f00000048020000010000002100000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4f666665723a312e300000000006000000"
    "4f66666572000000020000000a0000007265666572656e63650000000e00"
    "000033000000010000001d00000049444c3a6f6d672e6f72672f434f5242"
    "412f4f626a6563743a312e3000000000070000004f626a65637400000b00"
    "000070726f70657274696573000015000000a80100000100000027000000"
    "49444c3a6f6d672e6f72672f436f7354726164696e672f50726f70657274"
    "795365713a312e3000000c00000050726f70657274795365710013000000"
    "60010000010000000f00000050010000010000002400000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f50726f70657274793a312e3000"
    "0900000050726f70657274790000000003000000050000006e616d650000"
    "0000150000008c000000010000002800000049444c3a6f6d672e6f72672f"
    "436f7354726164696e672f50726f70657274794e616d653a312e30000d00"
    "000050726f70657274794e616d6500000000150000004000000001000000"
    "2300000049444c3a6f6d672e6f72672f436f7354726164696e672f497374"
    "72696e673a312e3000000800000049737472696e67001200000000000000"
    "0600000076616c7565000000150000004c00000001000000290000004944"
    "4c3a6f6d672e6f72672f436f7354726164696e672f50726f706572747956"
    "616c75653a312e30000000000e00000050726f706572747956616c756500"
    "00000b0000000800000069735f66696c65000800000000000000";
    CosTrading::_tc_OfferSeq = "01000000150000009c020000010000002400000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4f666665725365713a312e300009000000"
    "4f66666572536571000000001300000058020000010000000f0000004802"
    "0000010000002100000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f4f666665723a312e3000000000060000004f666665720000000200"
    "00000a0000007265666572656e63650000000e0000003300000001000000"
    "1d00000049444c3a6f6d672e6f72672f434f5242412f4f626a6563743a31"
    "2e3000000000070000004f626a65637400000b00000070726f7065727469"
    "6573000015000000a8010000010000002700000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f70657274795365713a312e300000"
    "0c00000050726f7065727479536571001300000060010000010000000f00"
    "000050010000010000002400000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f50726f70657274793a312e30000900000050726f706572"
    "74790000000003000000050000006e616d6500000000150000008c000000"
    "010000002800000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f50726f70657274794e616d653a312e30000d00000050726f7065727479"
    "4e616d65000000001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e670012000000000000000600000076616c756500"
    "0000150000004c000000010000002900000049444c3a6f6d672e6f72672f"
    "436f7354726164696e672f50726f706572747956616c75653a312e300000"
    "00000e00000050726f706572747956616c75650000000b00000008000000"
    "69735f66696c6500080000000000000000000000";
    CosTrading::_tc_OfferId = "010000001500000040000000010000002300000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4f6666657249643a312e30000008000000"
    "4f666665724964001200000000000000";
    CosTrading::_tc_OfferIdSeq = "010000001500000098000000010000002600000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4f6666657249645365713a312e30000000"
    "0b0000004f66666572496453657100001300000050000000010000001500"
    "000040000000010000002300000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f4f6666657249643a312e300000080000004f6666657249"
    "6400120000000000000000000000";
    CosTrading::_tc_ServiceTypeName = "010000001500000090000000010000002b00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f53657276696365547970654e616d653a31"
    "2e3000001000000053657276696365547970654e616d6500150000004000"
    "0000010000002300000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f49737472696e673a312e3000000800000049737472696e67001200"
    "000000000000";
    CosTrading::_tc_Constraint = "010000001500000088000000010000002600000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f436f6e73747261696e743a312e30000000"
    "0b000000436f6e73747261696e7400001500000040000000010000002300"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f4973747269"
    "6e673a312e3000000800000049737472696e67001200000000000000";
    CosTrading::_tc_FollowOption = "010000001100000073000000010000002800000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f466f6c6c6f774f7074696f6e3a312e3000"
    "0d000000466f6c6c6f774f7074696f6e00000000030000000b0000006c6f"
    "63616c5f6f6e6c7900000c00000069665f6e6f5f6c6f63616c0007000000"
    "616c7761797300";
    CosTrading::_tc_PolicyName = "010000001500000048000000010000002600000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f506f6c6963794e616d653a312e30000000"
    "0b000000506f6c6963794e616d6500001200000000000000";
    CosTrading::_tc_PolicyNameSeq = "0100000015000000a8000000010000002900000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f506f6c6963794e616d655365713a312e30"
    "000000000e000000506f6c6963794e616d65536571000000130000005800"
    "0000010000001500000048000000010000002600000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f506f6c6963794e616d653a312e3000"
    "00000b000000506f6c6963794e616d650000120000000000000000000000"
    ;
    CosTrading::_tc_PolicyValue = "010000001500000044000000010000002700000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f506f6c69637956616c75653a312e300000"
    "0c000000506f6c69637956616c7565000b000000";
    CosTrading::_tc_Policy = "010000000f000000f0000000010000002200000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f506f6c6963793a312e3000000007000000"
    "506f6c696379000002000000050000006e616d6500000000150000004800"
    "0000010000002600000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f506f6c6963794e616d653a312e300000000b000000506f6c696379"
    "4e616d65000012000000000000000600000076616c756500000015000000"
    "44000000010000002700000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f506f6c69637956616c75653a312e3000000c000000506f6c69"
    "637956616c7565000b000000";
    CosTrading::_tc_PolicySeq = "010000001500000048010000010000002500000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f506f6c6963795365713a312e3000000000"
    "0a000000506f6c6963795365710000001300000000010000010000000f00"
    "0000f0000000010000002200000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f506f6c6963793a312e3000000007000000506f6c696379"
    "000002000000050000006e616d6500000000150000004800000001000000"
    "2600000049444c3a6f6d672e6f72672f436f7354726164696e672f506f6c"
    "6963794e616d653a312e300000000b000000506f6c6963794e616d650000"
    "12000000000000000600000076616c756500000015000000440000000100"
    "00002700000049444c3a6f6d672e6f72672f436f7354726164696e672f50"
    "6f6c69637956616c75653a312e3000000c000000506f6c69637956616c75"
    "65000b00000000000000";
    CosTrading::_tc_UnknownMaxLeft = "01000000160000004c000000010000002a00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f556e6b6e6f776e4d61784c6566743a312e"
    "300000000f000000556e6b6e6f776e4d61784c656674000000000000";
    CosTrading::_tc_NotImplemented = "01000000160000004c000000010000002a00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4e6f74496d706c656d656e7465643a312e"
    "300000000f0000004e6f74496d706c656d656e746564000000000000";
    CosTrading::_tc_IllegalServiceType = "0100000016000000f8000000010000002e00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f496c6c6567616c53657276696365547970"
    "653a312e3000000013000000496c6c6567616c5365727669636554797065"
    "000001000000050000007479706500000000150000009000000001000000"
    "2b00000049444c3a6f6d672e6f72672f436f7354726164696e672f536572"
    "76696365547970654e616d653a312e300000100000005365727669636554"
    "7970654e616d65001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e67001200000000000000";
    CosTrading::_tc_UnknownServiceType = "0100000016000000f8000000010000002e00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f556e6b6e6f776e53657276696365547970"
    "653a312e3000000013000000556e6b6e6f776e5365727669636554797065"
    "000001000000050000007479706500000000150000009000000001000000"
    "2b00000049444c3a6f6d672e6f72672f436f7354726164696e672f536572"
    "76696365547970654e616d653a312e300000100000005365727669636554"
    "7970654e616d65001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e67001200000000000000";
    CosTrading::_tc_IllegalPropertyName = "0100000016000000f4000000010000002f00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f496c6c6567616c50726f70657274794e61"
    "6d653a312e30000014000000496c6c6567616c50726f70657274794e616d"
    "650001000000050000006e616d6500000000150000008c00000001000000"
    "2800000049444c3a6f6d672e6f72672f436f7354726164696e672f50726f"
    "70657274794e616d653a312e30000d00000050726f70657274794e616d65"
    "000000001500000040000000010000002300000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f49737472696e673a312e30000008000000"
    "49737472696e67001200000000000000";
    CosTrading::_tc_DuplicatePropertyName = "0100000016000000fc000000010000003100000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4475706c696361746550726f7065727479"
    "4e616d653a312e3000000000160000004475706c696361746550726f7065"
    "7274794e616d6500000001000000050000006e616d650000000015000000"
    "8c000000010000002800000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f50726f70657274794e616d653a312e30000d00000050726f70"
    "657274794e616d6500000000150000004000000001000000230000004944"
    "4c3a6f6d672e6f72672f436f7354726164696e672f49737472696e673a31"
    "2e3000000800000049737472696e67001200000000000000";
    CosTrading::_tc_PropertyTypeMismatch = "010000001600000060020000010000003000000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f7065727479547970654d69736d61"
    "7463683a312e30001500000050726f7065727479547970654d69736d6174"
    "636800000000020000000500000074797065000000001500000090000000"
    "010000002b00000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f53657276696365547970654e616d653a312e3000001000000053657276"
    "696365547970654e616d6500150000004000000001000000230000004944"
    "4c3a6f6d672e6f72672f436f7354726164696e672f49737472696e673a31"
    "2e3000000800000049737472696e67001200000000000000050000007072"
    "6f70000000000f00000050010000010000002400000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f50726f70657274793a312e30000900"
    "000050726f70657274790000000003000000050000006e616d6500000000"
    "150000008c000000010000002800000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f50726f70657274794e616d653a312e30000d000000"
    "50726f70657274794e616d65000000001500000040000000010000002300"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f4973747269"
    "6e673a312e3000000800000049737472696e670012000000000000000600"
    "000076616c7565000000150000004c000000010000002900000049444c3a"
    "6f6d672e6f72672f436f7354726164696e672f50726f706572747956616c"
    "75653a312e30000000000e00000050726f706572747956616c7565000000"
    "0b0000000800000069735f66696c650008000000";
    CosTrading::_tc_MissingMandatoryProperty = "0100000016000000a4010000010000003400000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4d697373696e674d616e6461746f727950"
    "726f70657274793a312e3000190000004d697373696e674d616e6461746f"
    "727950726f70657274790000000002000000050000007479706500000000"
    "1500000090000000010000002b00000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f53657276696365547970654e616d653a312e300000"
    "1000000053657276696365547970654e616d650015000000400000000100"
    "00002300000049444c3a6f6d672e6f72672f436f7354726164696e672f49"
    "737472696e673a312e3000000800000049737472696e6700120000000000"
    "0000050000006e616d6500000000150000008c0000000100000028000000"
    "49444c3a6f6d672e6f72672f436f7354726164696e672f50726f70657274"
    "794e616d653a312e30000d00000050726f70657274794e616d6500000000"
    "1500000040000000010000002300000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f49737472696e673a312e3000000800000049737472"
    "696e67001200000000000000";
    CosTrading::_tc_ReadonlyDynamicProperty = "0100000016000000a0010000010000003300000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f526561646f6e6c7944796e616d69635072"
    "6f70657274793a312e30000018000000526561646f6e6c7944796e616d69"
    "6350726f7065727479000200000005000000747970650000000015000000"
    "90000000010000002b00000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f53657276696365547970654e616d653a312e30000010000000"
    "53657276696365547970654e616d65001500000040000000010000002300"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f4973747269"
    "6e673a312e3000000800000049737472696e670012000000000000000500"
    "00006e616d6500000000150000008c000000010000002800000049444c3a"
    "6f6d672e6f72672f436f7354726164696e672f50726f70657274794e616d"
    "653a312e30000d00000050726f70657274794e616d650000000015000000"
    "40000000010000002300000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f49737472696e673a312e3000000800000049737472696e6700"
    "1200000000000000";
    CosTrading::_tc_IllegalConstraint = "0100000016000000f0000000010000002d00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f496c6c6567616c436f6e73747261696e74"
    "3a312e300000000012000000496c6c6567616c436f6e73747261696e7400"
    "00000100000007000000636f6e7374720000150000008800000001000000"
    "2600000049444c3a6f6d672e6f72672f436f7354726164696e672f436f6e"
    "73747261696e743a312e300000000b000000436f6e73747261696e740000"
    "1500000040000000010000002300000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f49737472696e673a312e3000000800000049737472"
    "696e67001200000000000000";
    CosTrading::_tc_InvalidLookupRef = "01000000160000009b000000010000002c00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f496e76616c69644c6f6f6b75705265663a"
    "312e300011000000496e76616c69644c6f6f6b7570526566000000000100"
    "00000700000074617267657400000e000000370000000100000022000000"
    "49444c3a6f6d672e6f72672f436f7354726164696e672f4c6f6f6b75703a"
    "312e30000000070000004c6f6f6b757000";
    CosTrading::_tc_IllegalOfferId = "01000000160000009c000000010000002a00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f496c6c6567616c4f6666657249643a312e"
    "300000000f000000496c6c6567616c4f6666657249640000010000000300"
    "0000696400001500000040000000010000002300000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f4f6666657249643a312e3000000800"
    "00004f666665724964001200000000000000";
    CosTrading::_tc_UnknownOfferId = "01000000160000009c000000010000002a00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f556e6b6e6f776e4f6666657249643a312e"
    "300000000f000000556e6b6e6f776e4f6666657249640000010000000300"
    "0000696400001500000040000000010000002300000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f4f6666657249643a312e3000000800"
    "00004f666665724964001200000000000000";
    CosTrading::_tc_DuplicatePolicyName = "0100000016000000b0000000010000002f00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4475706c6963617465506f6c6963794e61"
    "6d653a312e300000140000004475706c6963617465506f6c6963794e616d"
    "650001000000050000006e616d6500000000150000004800000001000000"
    "2600000049444c3a6f6d672e6f72672f436f7354726164696e672f506f6c"
    "6963794e616d653a312e300000000b000000506f6c6963794e616d650000"
    "1200000000000000";
    CosTrading::_tc_TraderComponents = "010000000e00000049000000010000002c00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f547261646572436f6d706f6e656e74733a"
    "312e300011000000547261646572436f6d706f6e656e747300";
    CosTrading::_tc_SupportAttributes = "010000000e0000004e000000010000002d00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f537570706f727441747472696275746573"
    "3a312e300000000012000000537570706f72744174747269627574657300"
    ;
    CosTrading::_tc_ImportAttributes = "010000000e00000049000000010000002c00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f496d706f7274417474726962757465733a"
    "312e300011000000496d706f72744174747269627574657300";
    CosTrading::Lookup::_tc_Preference = "010000001500000090000000010000002d00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75702f507265666572656e6365"
    "3a312e30000000000b000000507265666572656e63650000150000004000"
    "0000010000002300000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f49737472696e673a312e3000000800000049737472696e67001200"
    "000000000000";
    CosTrading::Lookup::_tc_HowManyProps = "010000001100000070000000010000002f00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75702f486f774d616e7950726f"
    "70733a312e3000000d000000486f774d616e7950726f7073000000000300"
    "0000050000006e6f6e650000000005000000736f6d650000000004000000"
    "616c6c00";
    CosTrading::Lookup::_tc_SpecifiedProps = "0100000010000000d8010000010000003100000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75702f53706563696669656450"
    "726f70733a312e30000000000f00000053706563696669656450726f7073"
    "00001100000070000000010000002f00000049444c3a6f6d672e6f72672f"
    "436f7354726164696e672f4c6f6f6b75702f486f774d616e7950726f7073"
    "3a312e3000000d000000486f774d616e7950726f70730000000003000000"
    "050000006e6f6e650000000005000000736f6d650000000004000000616c"
    "6c00ffffffff01000000010000000b00000070726f705f6e616d65730000"
    "15000000ec000000010000002b00000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f50726f70657274794e616d655365713a312e300000"
    "1000000050726f70657274794e616d6553657100130000009c0000000100"
    "0000150000008c000000010000002800000049444c3a6f6d672e6f72672f"
    "436f7354726164696e672f50726f70657274794e616d653a312e30000d00"
    "000050726f70657274794e616d6500000000150000004000000001000000"
    "2300000049444c3a6f6d672e6f72672f436f7354726164696e672f497374"
    "72696e673a312e3000000800000049737472696e67001200000000000000"
    "00000000";
    CosTrading::Lookup::_tc_IllegalPreference = "0100000016000000fc000000010000003400000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75702f496c6c6567616c507265"
    "666572656e63653a312e300012000000496c6c6567616c50726566657265"
    "6e6365000000010000000500000070726566000000001500000090000000"
    "010000002d00000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f4c6f6f6b75702f507265666572656e63653a312e30000000000b000000"
    "507265666572656e63650000150000004000000001000000230000004944"
    "4c3a6f6d672e6f72672f436f7354726164696e672f49737472696e673a31"
    "2e3000000800000049737472696e67001200000000000000";
    CosTrading::Lookup::_tc_IllegalPolicyName = "0100000016000000b4000000010000003400000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75702f496c6c6567616c506f6c"
    "6963794e616d653a312e300012000000496c6c6567616c506f6c6963794e"
    "616d6500000001000000050000006e616d65000000001500000048000000"
    "010000002600000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f506f6c6963794e616d653a312e300000000b000000506f6c6963794e61"
    "6d6500001200000000000000";
    CosTrading::Lookup::_tc_PolicyTypeMismatch = "010000001600000064010000010000003500000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75702f506f6c69637954797065"
    "4d69736d617463683a312e300000000013000000506f6c69637954797065"
    "4d69736d617463680000010000000b0000007468655f706f6c6963790000"
    "0f000000f0000000010000002200000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f506f6c6963793a312e3000000007000000506f6c69"
    "6379000002000000050000006e616d650000000015000000480000000100"
    "00002600000049444c3a6f6d672e6f72672f436f7354726164696e672f50"
    "6f6c6963794e616d653a312e300000000b000000506f6c6963794e616d65"
    "000012000000000000000600000076616c75650000001500000044000000"
    "010000002700000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f506f6c69637956616c75653a312e3000000c000000506f6c6963795661"
    "6c7565000b000000";
    CosTrading::Lookup::_tc_InvalidPolicyValue = "010000001600000064010000010000003500000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75702f496e76616c6964506f6c"
    "69637956616c75653a312e300000000013000000496e76616c6964506f6c"
    "69637956616c75650000010000000b0000007468655f706f6c6963790000"
    "0f000000f0000000010000002200000049444c3a6f6d672e6f72672f436f"
    "7354726164696e672f506f6c6963793a312e3000000007000000506f6c69"
    "6379000002000000050000006e616d650000000015000000480000000100"
    "00002600000049444c3a6f6d672e6f72672f436f7354726164696e672f50"
    "6f6c6963794e616d653a312e300000000b000000506f6c6963794e616d65"
    "000012000000000000000600000076616c75650000001500000044000000"
    "010000002700000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f506f6c69637956616c75653a312e3000000c000000506f6c6963795661"
    "6c7565000b000000";
    CosTrading::_tc_Lookup = "010000000e00000037000000010000002200000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6f6b75703a312e3000000007000000"
    "4c6f6f6b757000";
    CosTrading::Register::_tc_InterfaceTypeMismatch = "010000001600000053010000010000003a00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f52656769737465722f496e746572666163"
    "65547970654d69736d617463683a312e3000000016000000496e74657266"
    "616365547970654d69736d61746368000000020000000500000074797065"
    "000000001500000090000000010000002b00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f53657276696365547970654e616d653a31"
    "2e3000001000000053657276696365547970654e616d6500150000004000"
    "0000010000002300000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f49737472696e673a312e3000000800000049737472696e67001200"
    "0000000000000a0000007265666572656e63650000000e00000033000000"
    "010000001d00000049444c3a6f6d672e6f72672f434f5242412f4f626a65"
    "63743a312e3000000000070000004f626a65637400";
    CosTrading::Register::_tc_InvalidObjectRef = "01000000160000009f000000010000003500000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f52656769737465722f496e76616c69644f"
    "626a6563745265663a312e300000000011000000496e76616c69644f626a"
    "656374526566000000000100000004000000726566000e00000033000000"
    "010000001d00000049444c3a6f6d672e6f72672f434f5242412f4f626a65"
    "63743a312e3000000000070000004f626a65637400";
    CosTrading::_tc_Register = "010000000e00000039000000010000002400000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f52656769737465723a312e300009000000"
    "526567697374657200";
    CosTrading::_tc_OfferIterator = "010000000e00000046000000010000002900000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4f666665724974657261746f723a312e30"
    "000000000e0000004f666665724974657261746f7200";
    CosTrading::_tc_LongList = "010000001500000050000000010000002400000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f4c6f6e674c6973743a312e300009000000"
    "4c6f6e674c69737400000000130000000c00000001000000030000000000"
    "0000";
    CosTrading::_tc_FloatList = "010000001500000054000000010000002500000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f466c6f61744c6973743a312e3000000000"
    "0a000000466c6f61744c697374000000130000000c000000010000000600"
    "000000000000";
    CosTrading::_tc_StringList = "010000001500000058000000010000002600000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f537472696e674c6973743a312e30000000"
    "0b000000537472696e674c69737400001300000010000000010000001200"
    "00000000000000000000";
  }
};

static __tc_init_TRADER __init_TRADER;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface Link
CosTrading::Link_skel::Link_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Link:1.0", "Link" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/Link:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Link_skel>( this ) );
}

CosTrading::Link_skel::Link_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Link:1.0", "Link" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Link_skel>( this ) );
}

CosTrading::Link_skel::~Link_skel()
{
}

bool CosTrading::Link_skel::dispatch( CORBA::ServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

CosTrading::Link_ptr CosTrading::Link_skel::_this()
{
  return CosTrading::Link::_duplicate( this );
}


// Dynamic Implementation Routine for interface Proxy
CosTrading::Proxy_skel::Proxy_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Proxy:1.0", "Proxy" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/Proxy:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Proxy_skel>( this ) );
}

CosTrading::Proxy_skel::Proxy_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Proxy:1.0", "Proxy" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Proxy_skel>( this ) );
}

CosTrading::Proxy_skel::~Proxy_skel()
{
}

bool CosTrading::Proxy_skel::dispatch( CORBA::ServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

CosTrading::Proxy_ptr CosTrading::Proxy_skel::_this()
{
  return CosTrading::Proxy::_duplicate( this );
}


// Dynamic Implementation Routine for interface Admin
CosTrading::Admin_skel::Admin_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Admin:1.0", "Admin" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/Admin:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Admin_skel>( this ) );
}

CosTrading::Admin_skel::Admin_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Admin:1.0", "Admin" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Admin_skel>( this ) );
}

CosTrading::Admin_skel::~Admin_skel()
{
}

bool CosTrading::Admin_skel::dispatch( CORBA::ServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

CosTrading::Admin_ptr CosTrading::Admin_skel::_this()
{
  return CosTrading::Admin::_duplicate( this );
}


// Dynamic Implementation Routine for interface TraderComponents
CosTrading::TraderComponents_skel::TraderComponents_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/TraderComponents:1.0", "TraderComponents" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/TraderComponents:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<TraderComponents_skel>( this ) );
}

CosTrading::TraderComponents_skel::TraderComponents_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/TraderComponents:1.0", "TraderComponents" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<TraderComponents_skel>( this ) );
}

CosTrading::TraderComponents_skel::~TraderComponents_skel()
{
}

bool CosTrading::TraderComponents_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  if( strcmp( _req->op_name(), "_get_lookup_if" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    Lookup_ptr _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = lookup_if();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= (Lookup_ptr) _res;
    CORBA::release( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_register_if" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    Register_ptr _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = register_if();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= (Register_ptr) _res;
    CORBA::release( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_link_if" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    Link_ptr _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = link_if();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= (Link_ptr) _res;
    CORBA::release( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_proxy_if" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    Proxy_ptr _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = proxy_if();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= (Proxy_ptr) _res;
    CORBA::release( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_admin_if" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    Admin_ptr _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = admin_if();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= (Admin_ptr) _res;
    CORBA::release( _res );
    _req->result( _any_res );
    return true;
  }
  return false;
}

CosTrading::TraderComponents_ptr CosTrading::TraderComponents_skel::_this()
{
  return CosTrading::TraderComponents::_duplicate( this );
}


// Dynamic Implementation Routine for interface SupportAttributes
CosTrading::SupportAttributes_skel::SupportAttributes_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/SupportAttributes:1.0", "SupportAttributes" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/SupportAttributes:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<SupportAttributes_skel>( this ) );
}

CosTrading::SupportAttributes_skel::SupportAttributes_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/SupportAttributes:1.0", "SupportAttributes" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<SupportAttributes_skel>( this ) );
}

CosTrading::SupportAttributes_skel::~SupportAttributes_skel()
{
}

bool CosTrading::SupportAttributes_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  if( strcmp( _req->op_name(), "_get_supports_modifiable_properties" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::Boolean _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = supports_modifiable_properties();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= CORBA::Any::from_boolean( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_supports_dynamic_properties" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::Boolean _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = supports_dynamic_properties();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= CORBA::Any::from_boolean( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_supports_proxy_offers" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::Boolean _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = supports_proxy_offers();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= CORBA::Any::from_boolean( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_type_repos" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    TypeRepository_ptr _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = type_repos();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= CORBA::Any::from_object( _res, "Object" );
    CORBA::release( _res );
    _req->result( _any_res );
    return true;
  }
  return false;
}

CosTrading::SupportAttributes_ptr CosTrading::SupportAttributes_skel::_this()
{
  return CosTrading::SupportAttributes::_duplicate( this );
}


// Dynamic Implementation Routine for interface ImportAttributes
CosTrading::ImportAttributes_skel::ImportAttributes_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/ImportAttributes:1.0", "ImportAttributes" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/ImportAttributes:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<ImportAttributes_skel>( this ) );
}

CosTrading::ImportAttributes_skel::ImportAttributes_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/ImportAttributes:1.0", "ImportAttributes" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<ImportAttributes_skel>( this ) );
}

CosTrading::ImportAttributes_skel::~ImportAttributes_skel()
{
}

bool CosTrading::ImportAttributes_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  if( strcmp( _req->op_name(), "_get_def_search_card" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = def_search_card();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_max_search_card" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = max_search_card();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_def_match_card" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = def_match_card();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_max_match_card" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = max_match_card();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_def_return_card" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = def_return_card();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_max_return_card" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = max_return_card();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_max_list" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = max_list();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_def_hop_count" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = def_hop_count();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_max_hop_count" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = max_hop_count();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_def_follow_policy" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    FollowOption _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = def_follow_policy();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "_get_max_follow_policy" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    FollowOption _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = max_follow_policy();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  return false;
}

CosTrading::ImportAttributes_ptr CosTrading::ImportAttributes_skel::_this()
{
  return CosTrading::ImportAttributes::_duplicate( this );
}


// Dynamic Implementation Routine for interface Lookup
CosTrading::Lookup_skel::Lookup_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Lookup:1.0", "Lookup" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/Lookup:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Lookup_skel>( this ) );
}

CosTrading::Lookup_skel::Lookup_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Lookup:1.0", "Lookup" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Lookup_skel>( this ) );
}

CosTrading::Lookup_skel::~Lookup_skel()
{
}

bool CosTrading::Lookup_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  if( strcmp( _req->op_name(), "query" ) == 0 ) {
    ServiceTypeName_var type;
    Constraint_var constr;
    Preference_var pref;
    PolicySeq policies;
    SpecifiedProps desired_props;
    CORBA::ULong how_many;
    OfferSeq* offers;
    OfferIterator_ptr offer_itr;
    PolicyNameSeq* limits_applied;

    CORBA::NVList_ptr _args = new CORBA::NVList (9);
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CosTrading::_tc_ServiceTypeName );
    _args->add( CORBA::ARG_IN );
    _args->item( 1 )->value()->type( CosTrading::_tc_Constraint );
    _args->add( CORBA::ARG_IN );
    _args->item( 2 )->value()->type( CosTrading::Lookup::_tc_Preference );
    _args->add( CORBA::ARG_IN );
    _args->item( 3 )->value()->type( CosTrading::_tc_PolicySeq );
    _args->add( CORBA::ARG_IN );
    _args->item( 4 )->value()->type( CosTrading::Lookup::_tc_SpecifiedProps );
    _args->add( CORBA::ARG_IN );
    _args->item( 5 )->value()->type( CORBA::_tc_ulong );
    _args->add( CORBA::ARG_OUT );
    _args->item( 6 )->value()->type( CosTrading::_tc_OfferSeq );
    _args->add( CORBA::ARG_OUT );
    _args->item( 7 )->value()->type( CosTrading::_tc_OfferIterator );
    _args->add( CORBA::ARG_OUT );
    _args->item( 8 )->value()->type( CosTrading::_tc_PolicyNameSeq );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= CORBA::Any::to_string( type, 0 );
    *_args->item( 1 )->value() >>= CORBA::Any::to_string( constr, 0 );
    *_args->item( 2 )->value() >>= CORBA::Any::to_string( pref, 0 );
    *_args->item( 3 )->value() >>= policies;
    *_args->item( 4 )->value() >>= desired_props;
    *_args->item( 5 )->value() >>= how_many;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      query( type, constr, pref, policies, desired_props, how_many, offers, offer_itr, limits_applied );
    #ifdef HAVE_EXCEPTIONS
    } catch( ::CosTrading::IllegalServiceType_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::UnknownServiceType_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::IllegalConstraint_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::Lookup::IllegalPreference_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::Lookup::IllegalPolicyName_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::Lookup::PolicyTypeMismatch_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::Lookup::InvalidPolicyValue_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::IllegalPropertyName_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::DuplicatePropertyName_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ::CosTrading::DuplicatePolicyName_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    *_args->item( 6 )->value() <<= *offers;
    delete offers;
    *_args->item( 7 )->value() <<= (OfferIterator_ptr) offer_itr;
    CORBA::release( offer_itr );
    *_args->item( 8 )->value() <<= *limits_applied;
    delete limits_applied;
    return true;
  }
  return false;
}

CosTrading::Lookup_ptr CosTrading::Lookup_skel::_this()
{
  return CosTrading::Lookup::_duplicate( this );
}


// Dynamic Implementation Routine for interface Register
CosTrading::Register_skel::Register_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Register:1.0", "Register" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/Register:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Register_skel>( this ) );
}

CosTrading::Register_skel::Register_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/Register:1.0", "Register" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Register_skel>( this ) );
}

CosTrading::Register_skel::~Register_skel()
{
}

bool CosTrading::Register_skel::dispatch( CORBA::ServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

CosTrading::Register_ptr CosTrading::Register_skel::_this()
{
  return CosTrading::Register::_duplicate( this );
}


// Dynamic Implementation Routine for interface OfferIterator
CosTrading::OfferIterator_skel::OfferIterator_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/OfferIterator:1.0", "OfferIterator" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:omg.org/CosTrading/OfferIterator:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<OfferIterator_skel>( this ) );
}

CosTrading::OfferIterator_skel::OfferIterator_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:omg.org/CosTrading/OfferIterator:1.0", "OfferIterator" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<OfferIterator_skel>( this ) );
}

CosTrading::OfferIterator_skel::~OfferIterator_skel()
{
}

bool CosTrading::OfferIterator_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  if( strcmp( _req->op_name(), "max_left" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = max_left();
    #ifdef HAVE_EXCEPTIONS
    } catch( ::CosTrading::UnknownMaxLeft_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "next_n" ) == 0 ) {
    CORBA::ULong n;
    OfferSeq* offers;

    CORBA::NVList_ptr _args = new CORBA::NVList (2);
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_ulong );
    _args->add( CORBA::ARG_OUT );
    _args->item( 1 )->value()->type( CosTrading::_tc_OfferSeq );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= n;
    CORBA::Boolean _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = next_n( n, offers );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    *_args->item( 1 )->value() <<= *offers;
    delete offers;
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= CORBA::Any::from_boolean( _res );
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "destroy" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      destroy();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    return true;
  }
  return false;
}

CosTrading::OfferIterator_ptr CosTrading::OfferIterator_skel::_this()
{
  return CosTrading::OfferIterator::_duplicate( this );
}

