/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <typerepo.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------
#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_ServiceTypeNameSeq; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_ServiceTypeNameSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_PropertyMode; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_PropertyMode;
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::PropertyMode &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_PropertyMode );
  return (_a.enum_put( (CORBA::ULong) _e ));
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::PropertyMode &_e )
{
  CORBA::ULong _ul;
  if( !_a.enum_get( _ul ) )
    return FALSE;
  _e = (CosTradingRepos::ServiceTypeRepository::PropertyMode) _ul;
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_PropStruct; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_PropStruct;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::PropStruct::PropStruct()
{
}

CosTradingRepos::ServiceTypeRepository::PropStruct::PropStruct( const PropStruct& _s )
{
  name = ((PropStruct&)_s).name;
  value_type = ((PropStruct&)_s).value_type;
  mode = ((PropStruct&)_s).mode;
}

CosTradingRepos::ServiceTypeRepository::PropStruct::~PropStruct()
{
}

CosTradingRepos::ServiceTypeRepository::PropStruct&
CosTradingRepos::ServiceTypeRepository::PropStruct::operator=( const PropStruct& _s )
{
  name = ((PropStruct&)_s).name;
  value_type = ((PropStruct&)_s).value_type;
  mode = ((PropStruct&)_s).mode;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::PropStruct &_s )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_PropStruct );
  return (_a.struct_put_begin() &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::PropStruct&)_s).name) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::PropStruct&)_s).value_type) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::PropStruct&)_s).mode) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::PropStruct &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.name) &&
    (_a >>= _s.value_type) &&
    (_a >>= _s.mode) &&
    _a.struct_get_end() );
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_PropStructSeq; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_PropStructSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_Istring; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_Istring;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_PropertyName; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_PropertyName;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_PropertyValue; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_PropertyValue;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_Property; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_Property;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::Property::Property()
{
}

CosTradingRepos::ServiceTypeRepository::Property::Property( const Property& _s )
{
  value_type = ((Property&)_s).value_type;
  is_file = ((Property&)_s).is_file;
  name = ((Property&)_s).name;
  value = ((Property&)_s).value;
}

CosTradingRepos::ServiceTypeRepository::Property::~Property()
{
}

CosTradingRepos::ServiceTypeRepository::Property&
CosTradingRepos::ServiceTypeRepository::Property::operator=( const Property& _s )
{
  value_type = ((Property&)_s).value_type;
  is_file = ((Property&)_s).is_file;
  name = ((Property&)_s).name;
  value = ((Property&)_s).value;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::Property &_s )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_Property );
  return (_a.struct_put_begin() &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::Property&)_s).value_type) &&
    (_a <<= CORBA::Any::from_boolean( ((CosTradingRepos::ServiceTypeRepository::Property&)_s).is_file )) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::Property&)_s).name) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::Property&)_s).value) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::Property &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.value_type) &&
    (_a >>= CORBA::Any::to_boolean( _s.is_file )) &&
    (_a >>= _s.name) &&
    (_a >>= _s.value) &&
    _a.struct_get_end() );
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_PropertySeq; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_PropertySeq;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_Identifier; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_Identifier;
#endif

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_IncarnationNumber; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_IncarnationNumber;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::IncarnationNumber::IncarnationNumber()
{
}

CosTradingRepos::ServiceTypeRepository::IncarnationNumber::IncarnationNumber( const IncarnationNumber& _s )
{
  high = ((IncarnationNumber&)_s).high;
  low = ((IncarnationNumber&)_s).low;
}

CosTradingRepos::ServiceTypeRepository::IncarnationNumber::~IncarnationNumber()
{
}

CosTradingRepos::ServiceTypeRepository::IncarnationNumber&
CosTradingRepos::ServiceTypeRepository::IncarnationNumber::operator=( const IncarnationNumber& _s )
{
  high = ((IncarnationNumber&)_s).high;
  low = ((IncarnationNumber&)_s).low;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::IncarnationNumber &_s )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_IncarnationNumber );
  return (_a.struct_put_begin() &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::IncarnationNumber&)_s).high) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::IncarnationNumber&)_s).low) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::IncarnationNumber &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.high) &&
    (_a >>= _s.low) &&
    _a.struct_get_end() );
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_TypeStruct; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_TypeStruct;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::TypeStruct::TypeStruct()
{
}

CosTradingRepos::ServiceTypeRepository::TypeStruct::TypeStruct( const TypeStruct& _s )
{
  if_name = ((TypeStruct&)_s).if_name;
  props = ((TypeStruct&)_s).props;
  super_types = ((TypeStruct&)_s).super_types;
  values = ((TypeStruct&)_s).values;
  masked = ((TypeStruct&)_s).masked;
  incarnation = ((TypeStruct&)_s).incarnation;
}

CosTradingRepos::ServiceTypeRepository::TypeStruct::~TypeStruct()
{
}

CosTradingRepos::ServiceTypeRepository::TypeStruct&
CosTradingRepos::ServiceTypeRepository::TypeStruct::operator=( const TypeStruct& _s )
{
  if_name = ((TypeStruct&)_s).if_name;
  props = ((TypeStruct&)_s).props;
  super_types = ((TypeStruct&)_s).super_types;
  values = ((TypeStruct&)_s).values;
  masked = ((TypeStruct&)_s).masked;
  incarnation = ((TypeStruct&)_s).incarnation;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::TypeStruct &_s )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_TypeStruct );
  return (_a.struct_put_begin() &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::TypeStruct&)_s).if_name) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::TypeStruct&)_s).props) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::TypeStruct&)_s).super_types) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::TypeStruct&)_s).values) &&
    (_a <<= CORBA::Any::from_boolean( ((CosTradingRepos::ServiceTypeRepository::TypeStruct&)_s).masked )) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::TypeStruct&)_s).incarnation) &&
    _a.struct_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::TypeStruct &_s )
{
  return (_a.struct_get_begin() &&
    (_a >>= _s.if_name) &&
    (_a >>= _s.props) &&
    (_a >>= _s.super_types) &&
    (_a >>= _s.values) &&
    (_a >>= CORBA::Any::to_boolean( _s.masked )) &&
    (_a >>= _s.incarnation) &&
    _a.struct_get_end() );
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_ListOption; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_ListOption;
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::ListOption &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_ListOption );
  return (_a.enum_put( (CORBA::ULong) _e ));
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::ListOption &_e )
{
  CORBA::ULong _ul;
  if( !_a.enum_get( _ul ) )
    return FALSE;
  _e = (CosTradingRepos::ServiceTypeRepository::ListOption) _ul;
  return TRUE;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_SpecifiedServiceTypes; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_SpecifiedServiceTypes;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::SpecifiedServiceTypes()
{
}

CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::SpecifiedServiceTypes( const SpecifiedServiceTypes&_u )
{
  _discriminator = _u._discriminator;
  _m.incarnation = ((SpecifiedServiceTypes&)_u)._m.incarnation;
}

CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::~SpecifiedServiceTypes()
{
}

CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes&
CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::operator=( const SpecifiedServiceTypes&_u )
{
  _discriminator = _u._discriminator;
  _m.incarnation = ((SpecifiedServiceTypes&)_u)._m.incarnation;
  return *this;
}
#endif

void CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::_d( CosTradingRepos::ServiceTypeRepository::ListOption _p )
{
  _discriminator = _p;
}

CosTradingRepos::ServiceTypeRepository::ListOption CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::_d() const
{
  return _discriminator;
}

void CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::incarnation( const CosTradingRepos::ServiceTypeRepository::IncarnationNumber& _p )
{
  _discriminator = CosTradingRepos::ServiceTypeRepository::since;
  _m.incarnation = _p;
}

const CosTradingRepos::ServiceTypeRepository::IncarnationNumber& CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::incarnation() const
{
  return (CosTradingRepos::ServiceTypeRepository::IncarnationNumber&) _m.incarnation;
}

CosTradingRepos::ServiceTypeRepository::IncarnationNumber& CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::incarnation()
{
  return _m.incarnation;
}

void CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes::_default()
{
  _discriminator = all;
}

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes &_u )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_SpecifiedServiceTypes );
  if (!_a.union_put_begin())
    return FALSE;
  if( !(_a <<= ((CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes&)_u)._discriminator) )
    return FALSE;
  switch( _u._d() ) {
    case CosTradingRepos::ServiceTypeRepository::since:
      if( !_a.union_put_selection( 0 ) )
        return FALSE;
      if( !(_a <<= ((CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes&)_u)._m.incarnation) )
        return FALSE;
      break;
    default:
      break;
  }
  return _a.union_put_end();
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes &_u )
{
  if( !_a.union_get_begin() )
    return FALSE;
  if( !(_a >>= _u._discriminator) )
    return FALSE;
  switch( _u._discriminator ) {
    case CosTradingRepos::ServiceTypeRepository::since:
      if( !_a.union_get_selection( 0 ) )
        return FALSE;
      if( !(_a >>= _u._m.incarnation) )
        return FALSE;
      break;
    default:
      break;
  }
  return _a.union_get_end();
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_ServiceTypeExists; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_ServiceTypeExists;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::ServiceTypeExists()
{
}

CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::ServiceTypeExists( const ServiceTypeExists& _s )
{
  name = ((ServiceTypeExists&)_s).name;
}

CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::~ServiceTypeExists()
{
}

CosTradingRepos::ServiceTypeRepository::ServiceTypeExists&
CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::operator=( const ServiceTypeExists& _s )
{
  name = ((ServiceTypeExists&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::ServiceTypeExists &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_ServiceTypeExists );
  return (_a.except_put_begin( "IDL:CosTradingRepos/ServiceTypeRepository/ServiceTypeExists:1.0" ) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::ServiceTypeExists&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::ServiceTypeExists &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw ServiceTypeExists_var( (CosTradingRepos::ServiceTypeRepository::ServiceTypeExists*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::_repoid() const
{
  return "IDL:CosTradingRepos/ServiceTypeRepository/ServiceTypeExists:1.0";
}

void CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::_clone() const
{
  return new ServiceTypeExists( *this );
}

CosTradingRepos::ServiceTypeRepository::ServiceTypeExists *CosTradingRepos::ServiceTypeRepository::ServiceTypeExists::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository/ServiceTypeExists:1.0" ) )
    return (ServiceTypeExists *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_InterfaceTypeMismatch; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_InterfaceTypeMismatch;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::InterfaceTypeMismatch()
{
}

CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::InterfaceTypeMismatch( const InterfaceTypeMismatch& _s )
{
  base_service = ((InterfaceTypeMismatch&)_s).base_service;
  base_if = ((InterfaceTypeMismatch&)_s).base_if;
  derived_service = ((InterfaceTypeMismatch&)_s).derived_service;
  derived_if = ((InterfaceTypeMismatch&)_s).derived_if;
}

CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::~InterfaceTypeMismatch()
{
}

CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch&
CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::operator=( const InterfaceTypeMismatch& _s )
{
  base_service = ((InterfaceTypeMismatch&)_s).base_service;
  base_if = ((InterfaceTypeMismatch&)_s).base_if;
  derived_service = ((InterfaceTypeMismatch&)_s).derived_service;
  derived_if = ((InterfaceTypeMismatch&)_s).derived_if;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_InterfaceTypeMismatch );
  return (_a.except_put_begin( "IDL:CosTradingRepos/ServiceTypeRepository/InterfaceTypeMismatch:1.0" ) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch&)_e).base_service) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch&)_e).base_if) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch&)_e).derived_service) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch&)_e).derived_if) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.base_service) &&
    (_a >>= _e.base_if) &&
    (_a >>= _e.derived_service) &&
    (_a >>= _e.derived_if) &&
    _a.except_get_end() );
}

void CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw InterfaceTypeMismatch_var( (CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::_repoid() const
{
  return "IDL:CosTradingRepos/ServiceTypeRepository/InterfaceTypeMismatch:1.0";
}

void CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::_clone() const
{
  return new InterfaceTypeMismatch( *this );
}

CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch *CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository/InterfaceTypeMismatch:1.0" ) )
    return (InterfaceTypeMismatch *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_HasSubTypes; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_HasSubTypes;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::HasSubTypes::HasSubTypes()
{
}

CosTradingRepos::ServiceTypeRepository::HasSubTypes::HasSubTypes( const HasSubTypes& _s )
{
  the_type = ((HasSubTypes&)_s).the_type;
  sub_type = ((HasSubTypes&)_s).sub_type;
}

CosTradingRepos::ServiceTypeRepository::HasSubTypes::~HasSubTypes()
{
}

CosTradingRepos::ServiceTypeRepository::HasSubTypes&
CosTradingRepos::ServiceTypeRepository::HasSubTypes::operator=( const HasSubTypes& _s )
{
  the_type = ((HasSubTypes&)_s).the_type;
  sub_type = ((HasSubTypes&)_s).sub_type;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::HasSubTypes &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_HasSubTypes );
  return (_a.except_put_begin( "IDL:CosTradingRepos/ServiceTypeRepository/HasSubTypes:1.0" ) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::HasSubTypes&)_e).the_type) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::HasSubTypes&)_e).sub_type) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::HasSubTypes &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.the_type) &&
    (_a >>= _e.sub_type) &&
    _a.except_get_end() );
}

void CosTradingRepos::ServiceTypeRepository::HasSubTypes::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw HasSubTypes_var( (CosTradingRepos::ServiceTypeRepository::HasSubTypes*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTradingRepos::ServiceTypeRepository::HasSubTypes::_repoid() const
{
  return "IDL:CosTradingRepos/ServiceTypeRepository/HasSubTypes:1.0";
}

void CosTradingRepos::ServiceTypeRepository::HasSubTypes::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTradingRepos::ServiceTypeRepository::HasSubTypes::_clone() const
{
  return new HasSubTypes( *this );
}

CosTradingRepos::ServiceTypeRepository::HasSubTypes *CosTradingRepos::ServiceTypeRepository::HasSubTypes::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository/HasSubTypes:1.0" ) )
    return (HasSubTypes *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_AlreadyMasked; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_AlreadyMasked;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::AlreadyMasked::AlreadyMasked()
{
}

CosTradingRepos::ServiceTypeRepository::AlreadyMasked::AlreadyMasked( const AlreadyMasked& _s )
{
  name = ((AlreadyMasked&)_s).name;
}

CosTradingRepos::ServiceTypeRepository::AlreadyMasked::~AlreadyMasked()
{
}

CosTradingRepos::ServiceTypeRepository::AlreadyMasked&
CosTradingRepos::ServiceTypeRepository::AlreadyMasked::operator=( const AlreadyMasked& _s )
{
  name = ((AlreadyMasked&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::AlreadyMasked &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_AlreadyMasked );
  return (_a.except_put_begin( "IDL:CosTradingRepos/ServiceTypeRepository/AlreadyMasked:1.0" ) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::AlreadyMasked&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::AlreadyMasked &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTradingRepos::ServiceTypeRepository::AlreadyMasked::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw AlreadyMasked_var( (CosTradingRepos::ServiceTypeRepository::AlreadyMasked*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTradingRepos::ServiceTypeRepository::AlreadyMasked::_repoid() const
{
  return "IDL:CosTradingRepos/ServiceTypeRepository/AlreadyMasked:1.0";
}

void CosTradingRepos::ServiceTypeRepository::AlreadyMasked::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTradingRepos::ServiceTypeRepository::AlreadyMasked::_clone() const
{
  return new AlreadyMasked( *this );
}

CosTradingRepos::ServiceTypeRepository::AlreadyMasked *CosTradingRepos::ServiceTypeRepository::AlreadyMasked::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository/AlreadyMasked:1.0" ) )
    return (AlreadyMasked *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_NotMasked; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_NotMasked;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::NotMasked::NotMasked()
{
}

CosTradingRepos::ServiceTypeRepository::NotMasked::NotMasked( const NotMasked& _s )
{
  name = ((NotMasked&)_s).name;
}

CosTradingRepos::ServiceTypeRepository::NotMasked::~NotMasked()
{
}

CosTradingRepos::ServiceTypeRepository::NotMasked&
CosTradingRepos::ServiceTypeRepository::NotMasked::operator=( const NotMasked& _s )
{
  name = ((NotMasked&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::NotMasked &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_NotMasked );
  return (_a.except_put_begin( "IDL:CosTradingRepos/ServiceTypeRepository/NotMasked:1.0" ) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::NotMasked&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::NotMasked &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTradingRepos::ServiceTypeRepository::NotMasked::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw NotMasked_var( (CosTradingRepos::ServiceTypeRepository::NotMasked*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTradingRepos::ServiceTypeRepository::NotMasked::_repoid() const
{
  return "IDL:CosTradingRepos/ServiceTypeRepository/NotMasked:1.0";
}

void CosTradingRepos::ServiceTypeRepository::NotMasked::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTradingRepos::ServiceTypeRepository::NotMasked::_clone() const
{
  return new NotMasked( *this );
}

CosTradingRepos::ServiceTypeRepository::NotMasked *CosTradingRepos::ServiceTypeRepository::NotMasked::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository/NotMasked:1.0" ) )
    return (NotMasked *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_ValueTypeRedefinition; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_ValueTypeRedefinition;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::ValueTypeRedefinition()
{
}

CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::ValueTypeRedefinition( const ValueTypeRedefinition& _s )
{
  type_1 = ((ValueTypeRedefinition&)_s).type_1;
  definition_1 = ((ValueTypeRedefinition&)_s).definition_1;
  type_2 = ((ValueTypeRedefinition&)_s).type_2;
  definition_2 = ((ValueTypeRedefinition&)_s).definition_2;
}

CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::~ValueTypeRedefinition()
{
}

CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition&
CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::operator=( const ValueTypeRedefinition& _s )
{
  type_1 = ((ValueTypeRedefinition&)_s).type_1;
  definition_1 = ((ValueTypeRedefinition&)_s).definition_1;
  type_2 = ((ValueTypeRedefinition&)_s).type_2;
  definition_2 = ((ValueTypeRedefinition&)_s).definition_2;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_ValueTypeRedefinition );
  return (_a.except_put_begin( "IDL:CosTradingRepos/ServiceTypeRepository/ValueTypeRedefinition:1.0" ) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition&)_e).type_1) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition&)_e).definition_1) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition&)_e).type_2) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition&)_e).definition_2) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.type_1) &&
    (_a >>= _e.definition_1) &&
    (_a >>= _e.type_2) &&
    (_a >>= _e.definition_2) &&
    _a.except_get_end() );
}

void CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw ValueTypeRedefinition_var( (CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::_repoid() const
{
  return "IDL:CosTradingRepos/ServiceTypeRepository/ValueTypeRedefinition:1.0";
}

void CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::_clone() const
{
  return new ValueTypeRedefinition( *this );
}

CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition *CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository/ValueTypeRedefinition:1.0" ) )
    return (ValueTypeRedefinition *) _ex;
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst ServiceTypeRepository::_tc_DuplicateServiceTypeName; };
#else
CORBA::TypeCodeConst CosTradingRepos::ServiceTypeRepository::_tc_DuplicateServiceTypeName;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::DuplicateServiceTypeName()
{
}

CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::DuplicateServiceTypeName( const DuplicateServiceTypeName& _s )
{
  name = ((DuplicateServiceTypeName&)_s).name;
}

CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::~DuplicateServiceTypeName()
{
}

CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName&
CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::operator=( const DuplicateServiceTypeName& _s )
{
  name = ((DuplicateServiceTypeName&)_s).name;
  return *this;
}
#endif

CORBA::Boolean operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName &_e )
{
  _a.type( CosTradingRepos::ServiceTypeRepository::_tc_DuplicateServiceTypeName );
  return (_a.except_put_begin( "IDL:CosTradingRepos/ServiceTypeRepository/DuplicateServiceTypeName:1.0" ) &&
    (_a <<= ((CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName&)_e).name) &&
    _a.except_put_end() );
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName &_e )
{
  CORBA::String_var _repoid;
  return (_a.except_get_begin( _repoid ) &&
    (_a >>= _e.name) &&
    _a.except_get_end() );
}

void CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  throw DuplicateServiceTypeName_var( (CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName*)_clone() );
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::_repoid() const
{
  return "IDL:CosTradingRepos/ServiceTypeRepository/DuplicateServiceTypeName:1.0";
}

void CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::_encode( CORBA::DataEncoder &_en ) const
{
  CORBA::Any _a;
  _a <<= *this;
  _a.marshal( _en );
}

CORBA::Exception *CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::_clone() const
{
  return new DuplicateServiceTypeName( *this );
}

CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName *CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName::_narrow( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository/DuplicateServiceTypeName:1.0" ) )
    return (DuplicateServiceTypeName *) _ex;
  return NULL;
}


// Stub interface ServiceTypeRepository
CosTradingRepos::ServiceTypeRepository::~ServiceTypeRepository()
{
}

CosTradingRepos::ServiceTypeRepository_ptr CosTradingRepos::ServiceTypeRepository::_duplicate( ServiceTypeRepository_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *CosTradingRepos::ServiceTypeRepository::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:CosTradingRepos/ServiceTypeRepository:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool CosTradingRepos::ServiceTypeRepository::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:CosTradingRepos/ServiceTypeRepository:1.0" ) == 0) {
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

CosTradingRepos::ServiceTypeRepository_ptr CosTradingRepos::ServiceTypeRepository::_narrow( CORBA::Object_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:CosTradingRepos/ServiceTypeRepository:1.0" )))
      return _duplicate( (CosTradingRepos::ServiceTypeRepository_ptr) _p );
  }
  return _nil();
}

CosTradingRepos::ServiceTypeRepository_ptr CosTradingRepos::ServiceTypeRepository::_nil()
{
  return NULL;
}

#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { vector<CORBA::Narrow_proto> * ServiceTypeRepository::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * CosTradingRepos::ServiceTypeRepository::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace CosTradingRepos { CORBA::TypeCodeConst _tc_ServiceTypeRepository; };
#else
CORBA::TypeCodeConst CosTradingRepos::_tc_ServiceTypeRepository;
#endif

CORBA::Boolean
operator<<=( CORBA::Any &_a, const CosTradingRepos::ServiceTypeRepository_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "ServiceTypeRepository" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, CosTradingRepos::ServiceTypeRepository_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::CosTradingRepos::ServiceTypeRepository::_nil();
    return TRUE;
  }
  _obj = ::CosTradingRepos::ServiceTypeRepository::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTradingRepos::ServiceTypeRepository::PropStruct> &_s )
{
  static CORBA::TypeCodeConst _tc =
    "0100000013000000e0010000010000000f000000d0010000010000003900"
    "000049444c3a436f7354726164696e675265706f732f5365727669636554"
    "7970655265706f7369746f72792f50726f705374727563743a312e300000"
    "00000b00000050726f70537472756374000003000000050000006e616d65"
    "00000000150000008c000000010000002800000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f50726f70657274794e616d653a312e3000"
    "0d00000050726f70657274794e616d650000000015000000400000000100"
    "00002300000049444c3a6f6d672e6f72672f436f7354726164696e672f49"
    "737472696e673a312e3000000800000049737472696e6700120000000000"
    "00000b00000076616c75655f7479706500000c000000050000006d6f6465"
    "0000000011000000b0000000010000003b00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f70657274794d6f64653a312e3000000d00000050726f706572"
    "74794d6f646500000000040000000c00000050524f505f4e4f524d414c00"
    "0e00000050524f505f524541444f4e4c590000000f00000050524f505f4d"
    "414e4441544f525900001800000050524f505f4d414e4441544f52595f52"
    "4541444f4e4c590000000000";
  _a.type( _tc );
  if( !_a.seq_put_begin( _s.length() ) )
    return FALSE;
  for( CORBA::ULong _i = 0; _i < _s.length(); _i++ )
    if( !(_a <<= ((SequenceTmpl<CosTradingRepos::ServiceTypeRepository::PropStruct>&)_s)[ _i ]) )
      return FALSE;
  return _a.seq_put_end();
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTradingRepos::ServiceTypeRepository::PropStruct> &_s )
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


CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTradingRepos::ServiceTypeRepository::Property> &_s )
{
  static CORBA::TypeCodeConst _tc =
    "0100000013000000c0010000010000000f000000b0010000010000003700"
    "000049444c3a436f7354726164696e675265706f732f5365727669636554"
    "7970655265706f7369746f72792f50726f70657274793a312e3000000900"
    "000050726f706572747900000000040000000b00000076616c75655f7479"
    "706500000c0000000800000069735f66696c650008000000050000006e61"
    "6d650000000015000000b4000000010000003b00000049444c3a436f7354"
    "726164696e675265706f732f53657276696365547970655265706f736974"
    "6f72792f50726f70657274794e616d653a312e3000000d00000050726f70"
    "657274794e616d6500000000150000005400000001000000360000004944"
    "4c3a436f7354726164696e675265706f732f536572766963655479706552"
    "65706f7369746f72792f49737472696e673a312e30000000080000004973"
    "7472696e670012000000000000000600000076616c756500000015000000"
    "5c000000010000003c00000049444c3a436f7354726164696e675265706f"
    "732f53657276696365547970655265706f7369746f72792f50726f706572"
    "747956616c75653a312e30000e00000050726f706572747956616c756500"
    "00000b00000000000000";
  _a.type( _tc );
  if( !_a.seq_put_begin( _s.length() ) )
    return FALSE;
  for( CORBA::ULong _i = 0; _i < _s.length(); _i++ )
    if( !(_a <<= ((SequenceTmpl<CosTradingRepos::ServiceTypeRepository::Property>&)_s)[ _i ]) )
      return FALSE;
  return _a.seq_put_end();
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTradingRepos::ServiceTypeRepository::Property> &_s )
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


struct __tc_init_TYPEREPO {
  __tc_init_TYPEREPO()
  {
    CosTradingRepos::ServiceTypeRepository::_tc_ServiceTypeNameSeq = "01000000150000000c010000010000004100000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f53657276696365547970654e616d655365713a312e30000000001300"
    "000053657276696365547970654e616d65536571000013000000a0000000"
    "010000001500000090000000010000002b00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f53657276696365547970654e616d653a31"
    "2e3000001000000053657276696365547970654e616d6500150000004000"
    "0000010000002300000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f49737472696e673a312e3000000800000049737472696e67001200"
    "00000000000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_PropertyMode = "0100000011000000b0000000010000003b00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f70657274794d6f64653a312e3000000d00000050726f706572"
    "74794d6f646500000000040000000c00000050524f505f4e4f524d414c00"
    "0e00000050524f505f524541444f4e4c590000000f00000050524f505f4d"
    "414e4441544f525900001800000050524f505f4d414e4441544f52595f52"
    "4541444f4e4c5900";
    CosTradingRepos::ServiceTypeRepository::_tc_PropStruct = "010000000f000000d0010000010000003900000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f705374727563743a312e30000000000b00000050726f705374"
    "72756374000003000000050000006e616d6500000000150000008c000000"
    "010000002800000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f50726f70657274794e616d653a312e30000d00000050726f7065727479"
    "4e616d65000000001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e670012000000000000000b00000076616c75655f"
    "7479706500000c000000050000006d6f64650000000011000000b0000000"
    "010000003b00000049444c3a436f7354726164696e675265706f732f5365"
    "7276696365547970655265706f7369746f72792f50726f70657274794d6f"
    "64653a312e3000000d00000050726f70657274794d6f6465000000000400"
    "00000c00000050524f505f4e4f524d414c000e00000050524f505f524541"
    "444f4e4c590000000f00000050524f505f4d414e4441544f525900001800"
    "000050524f505f4d414e4441544f52595f524541444f4e4c5900";
    CosTradingRepos::ServiceTypeRepository::_tc_PropStructSeq = "010000001500000040020000010000003c00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f705374727563745365713a312e30000e00000050726f705374"
    "7275637453657100000013000000e0010000010000000f000000d0010000"
    "010000003900000049444c3a436f7354726164696e675265706f732f5365"
    "7276696365547970655265706f7369746f72792f50726f70537472756374"
    "3a312e30000000000b00000050726f705374727563740000030000000500"
    "00006e616d6500000000150000008c000000010000002800000049444c3a"
    "6f6d672e6f72672f436f7354726164696e672f50726f70657274794e616d"
    "653a312e30000d00000050726f70657274794e616d650000000015000000"
    "40000000010000002300000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f49737472696e673a312e3000000800000049737472696e6700"
    "12000000000000000b00000076616c75655f7479706500000c0000000500"
    "00006d6f64650000000011000000b0000000010000003b00000049444c3a"
    "436f7354726164696e675265706f732f5365727669636554797065526570"
    "6f7369746f72792f50726f70657274794d6f64653a312e3000000d000000"
    "50726f70657274794d6f646500000000040000000c00000050524f505f4e"
    "4f524d414c000e00000050524f505f524541444f4e4c590000000f000000"
    "50524f505f4d414e4441544f525900001800000050524f505f4d414e4441"
    "544f52595f524541444f4e4c590000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_Istring = "010000001500000054000000010000003600000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f49737472696e673a312e300000000800000049737472696e67001200"
    "000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_PropertyName = "0100000015000000b4000000010000003b00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f70657274794e616d653a312e3000000d00000050726f706572"
    "74794e616d65000000001500000054000000010000003600000049444c3a"
    "436f7354726164696e675265706f732f5365727669636554797065526570"
    "6f7369746f72792f49737472696e673a312e300000000800000049737472"
    "696e67001200000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_PropertyValue = "01000000150000005c000000010000003c00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f706572747956616c75653a312e30000e00000050726f706572"
    "747956616c75650000000b000000";
    CosTradingRepos::ServiceTypeRepository::_tc_Property = "010000000f000000b0010000010000003700000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f70657274793a312e3000000900000050726f70657274790000"
    "0000040000000b00000076616c75655f7479706500000c00000008000000"
    "69735f66696c650008000000050000006e616d650000000015000000b400"
    "0000010000003b00000049444c3a436f7354726164696e675265706f732f"
    "53657276696365547970655265706f7369746f72792f50726f7065727479"
    "4e616d653a312e3000000d00000050726f70657274794e616d6500000000"
    "1500000054000000010000003600000049444c3a436f7354726164696e67"
    "5265706f732f53657276696365547970655265706f7369746f72792f4973"
    "7472696e673a312e300000000800000049737472696e6700120000000000"
    "00000600000076616c7565000000150000005c000000010000003c000000"
    "49444c3a436f7354726164696e675265706f732f53657276696365547970"
    "655265706f7369746f72792f50726f706572747956616c75653a312e3000"
    "0e00000050726f706572747956616c75650000000b000000";
    CosTradingRepos::ServiceTypeRepository::_tc_PropertySeq = "01000000150000001c020000010000003a00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f70657274795365713a312e300000000c00000050726f706572"
    "74795365710013000000c0010000010000000f000000b001000001000000"
    "3700000049444c3a436f7354726164696e675265706f732f536572766963"
    "65547970655265706f7369746f72792f50726f70657274793a312e300000"
    "0900000050726f706572747900000000040000000b00000076616c75655f"
    "7479706500000c0000000800000069735f66696c65000800000005000000"
    "6e616d650000000015000000b4000000010000003b00000049444c3a436f"
    "7354726164696e675265706f732f53657276696365547970655265706f73"
    "69746f72792f50726f70657274794e616d653a312e3000000d0000005072"
    "6f70657274794e616d650000000015000000540000000100000036000000"
    "49444c3a436f7354726164696e675265706f732f53657276696365547970"
    "655265706f7369746f72792f49737472696e673a312e3000000008000000"
    "49737472696e670012000000000000000600000076616c75650000001500"
    "00005c000000010000003c00000049444c3a436f7354726164696e675265"
    "706f732f53657276696365547970655265706f7369746f72792f50726f70"
    "6572747956616c75653a312e30000e00000050726f706572747956616c75"
    "650000000b00000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_Identifier = "01000000150000009c000000010000003900000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f4964656e7469666965723a312e30000000000b0000004964656e7469"
    "6669657200001500000040000000010000002300000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f49737472696e673a312e3000000800"
    "000049737472696e67001200000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_IncarnationNumber = "010000000f00000080000000010000004000000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f496e6361726e6174696f6e4e756d6265723a312e300012000000496e"
    "6361726e6174696f6e4e756d626572000000020000000500000068696768"
    "0000000005000000040000006c6f770005000000";
    CosTradingRepos::ServiceTypeRepository::_tc_TypeStruct = "010000000f00000058070000010000003900000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f547970655374727563743a312e30000000000b000000547970655374"
    "727563740000060000000800000069665f6e616d6500150000009c000000"
    "010000003900000049444c3a436f7354726164696e675265706f732f5365"
    "7276696365547970655265706f7369746f72792f4964656e746966696572"
    "3a312e30000000000b0000004964656e7469666965720000150000004000"
    "0000010000002300000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f49737472696e673a312e3000000800000049737472696e67001200"
    "0000000000000600000070726f7073000000150000004002000001000000"
    "3c00000049444c3a436f7354726164696e675265706f732f536572766963"
    "65547970655265706f7369746f72792f50726f705374727563745365713a"
    "312e30000e00000050726f7053747275637453657100000013000000e001"
    "0000010000000f000000d0010000010000003900000049444c3a436f7354"
    "726164696e675265706f732f53657276696365547970655265706f736974"
    "6f72792f50726f705374727563743a312e30000000000b00000050726f70"
    "537472756374000003000000050000006e616d6500000000150000008c00"
    "0000010000002800000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f50726f70657274794e616d653a312e30000d00000050726f706572"
    "74794e616d65000000001500000040000000010000002300000049444c3a"
    "6f6d672e6f72672f436f7354726164696e672f49737472696e673a312e30"
    "00000800000049737472696e670012000000000000000b00000076616c75"
    "655f7479706500000c000000050000006d6f64650000000011000000b000"
    "0000010000003b00000049444c3a436f7354726164696e675265706f732f"
    "53657276696365547970655265706f7369746f72792f50726f7065727479"
    "4d6f64653a312e3000000d00000050726f70657274794d6f646500000000"
    "040000000c00000050524f505f4e4f524d414c000e00000050524f505f52"
    "4541444f4e4c590000000f00000050524f505f4d414e4441544f52590000"
    "1800000050524f505f4d414e4441544f52595f524541444f4e4c59000000"
    "00000c00000073757065725f747970657300150000000c01000001000000"
    "4100000049444c3a436f7354726164696e675265706f732f536572766963"
    "65547970655265706f7369746f72792f53657276696365547970654e616d"
    "655365713a312e30000000001300000053657276696365547970654e616d"
    "65536571000013000000a000000001000000150000009000000001000000"
    "2b00000049444c3a6f6d672e6f72672f436f7354726164696e672f536572"
    "76696365547970654e616d653a312e300000100000005365727669636554"
    "7970654e616d65001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e6700120000000000000000000000070000007661"
    "6c7565730000150000001c020000010000003a00000049444c3a436f7354"
    "726164696e675265706f732f53657276696365547970655265706f736974"
    "6f72792f50726f70657274795365713a312e300000000c00000050726f70"
    "657274795365710013000000c0010000010000000f000000b00100000100"
    "00003700000049444c3a436f7354726164696e675265706f732f53657276"
    "696365547970655265706f7369746f72792f50726f70657274793a312e30"
    "00000900000050726f706572747900000000040000000b00000076616c75"
    "655f7479706500000c0000000800000069735f66696c6500080000000500"
    "00006e616d650000000015000000b4000000010000003b00000049444c3a"
    "436f7354726164696e675265706f732f5365727669636554797065526570"
    "6f7369746f72792f50726f70657274794e616d653a312e3000000d000000"
    "50726f70657274794e616d65000000001500000054000000010000003600"
    "000049444c3a436f7354726164696e675265706f732f5365727669636554"
    "7970655265706f7369746f72792f49737472696e673a312e300000000800"
    "000049737472696e670012000000000000000600000076616c7565000000"
    "150000005c000000010000003c00000049444c3a436f7354726164696e67"
    "5265706f732f53657276696365547970655265706f7369746f72792f5072"
    "6f706572747956616c75653a312e30000e00000050726f70657274795661"
    "6c75650000000b00000000000000070000006d61736b6564000008000000"
    "0c000000696e6361726e6174696f6e000f00000080000000010000004000"
    "000049444c3a436f7354726164696e675265706f732f5365727669636554"
    "7970655265706f7369746f72792f496e6361726e6174696f6e4e756d6265"
    "723a312e300012000000496e6361726e6174696f6e4e756d626572000000"
    "0200000005000000686967680000000005000000040000006c6f77000500"
    "0000";
    CosTradingRepos::ServiceTypeRepository::_tc_ListOption = "01000000110000006a000000010000003900000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f4c6973744f7074696f6e3a312e30000000000b0000004c6973744f70"
    "74696f6e00000200000004000000616c6c000600000073696e636500";
    CosTradingRepos::ServiceTypeRepository::_tc_SpecifiedServiceTypes = "010000001000000080010000010000004400000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f5370656369666965645365727669636554797065733a312e30001600"
    "000053706563696669656453657276696365547970657300000011000000"
    "6a000000010000003900000049444c3a436f7354726164696e675265706f"
    "732f53657276696365547970655265706f7369746f72792f4c6973744f70"
    "74696f6e3a312e30000000000b0000004c6973744f7074696f6e00000200"
    "000004000000616c6c000600000073696e6365000000ffffffff01000000"
    "010000000c000000696e6361726e6174696f6e000f000000800000000100"
    "00004000000049444c3a436f7354726164696e675265706f732f53657276"
    "696365547970655265706f7369746f72792f496e6361726e6174696f6e4e"
    "756d6265723a312e300012000000496e6361726e6174696f6e4e756d6265"
    "720000000200000005000000686967680000000005000000040000006c6f"
    "770005000000";
    CosTradingRepos::ServiceTypeRepository::_tc_ServiceTypeExists = "010000001600000008010000010000004000000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f53657276696365547970654578697374733a312e3000120000005365"
    "72766963655479706545786973747300000001000000050000006e616d65"
    "000000001500000090000000010000002b00000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f53657276696365547970654e616d653a31"
    "2e3000001000000053657276696365547970654e616d6500150000004000"
    "0000010000002300000049444c3a6f6d672e6f72672f436f735472616469"
    "6e672f49737472696e673a312e3000000800000049737472696e67001200"
    "000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_InterfaceTypeMismatch = "010000001600000028030000010000004400000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f496e74657266616365547970654d69736d617463683a312e30001600"
    "0000496e74657266616365547970654d69736d6174636800000004000000"
    "0d000000626173655f736572766963650000000015000000900000000100"
    "00002b00000049444c3a6f6d672e6f72672f436f7354726164696e672f53"
    "657276696365547970654e616d653a312e30000010000000536572766963"
    "65547970654e616d65001500000040000000010000002300000049444c3a"
    "6f6d672e6f72672f436f7354726164696e672f49737472696e673a312e30"
    "00000800000049737472696e670012000000000000000800000062617365"
    "5f696600150000009c000000010000003900000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f4964656e7469666965723a312e30000000000b0000004964656e7469"
    "6669657200001500000040000000010000002300000049444c3a6f6d672e"
    "6f72672f436f7354726164696e672f49737472696e673a312e3000000800"
    "000049737472696e6700120000000000000010000000646572697665645f"
    "73657276696365001500000090000000010000002b00000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f53657276696365547970654e61"
    "6d653a312e3000001000000053657276696365547970654e616d65001500"
    "000040000000010000002300000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f49737472696e673a312e3000000800000049737472696e"
    "670012000000000000000b000000646572697665645f6966000015000000"
    "9c000000010000003900000049444c3a436f7354726164696e675265706f"
    "732f53657276696365547970655265706f7369746f72792f4964656e7469"
    "666965723a312e30000000000b0000004964656e74696669657200001500"
    "000040000000010000002300000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f49737472696e673a312e3000000800000049737472696e"
    "67001200000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_HasSubTypes = "0100000016000000a8010000010000003a00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f48617353756254797065733a312e300000000c000000486173537562"
    "54797065730002000000090000007468655f747970650000000015000000"
    "90000000010000002b00000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f53657276696365547970654e616d653a312e30000010000000"
    "53657276696365547970654e616d65001500000040000000010000002300"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f4973747269"
    "6e673a312e3000000800000049737472696e670012000000000000000900"
    "00007375625f74797065000000001500000090000000010000002b000000"
    "49444c3a6f6d672e6f72672f436f7354726164696e672f53657276696365"
    "547970654e616d653a312e3000001000000053657276696365547970654e"
    "616d65001500000040000000010000002300000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f49737472696e673a312e30000008000000"
    "49737472696e67001200000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_AlreadyMasked = "010000001600000000010000010000003c00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f416c72656164794d61736b65643a312e30000e000000416c72656164"
    "794d61736b656400000001000000050000006e616d650000000015000000"
    "90000000010000002b00000049444c3a6f6d672e6f72672f436f73547261"
    "64696e672f53657276696365547970654e616d653a312e30000010000000"
    "53657276696365547970654e616d65001500000040000000010000002300"
    "000049444c3a6f6d672e6f72672f436f7354726164696e672f4973747269"
    "6e673a312e3000000800000049737472696e67001200000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_NotMasked = "0100000016000000f8000000010000003800000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f4e6f744d61736b65643a312e30000a0000004e6f744d61736b656400"
    "000001000000050000006e616d6500000000150000009000000001000000"
    "2b00000049444c3a6f6d672e6f72672f436f7354726164696e672f536572"
    "76696365547970654e616d653a312e300000100000005365727669636554"
    "7970654e616d65001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e67001200000000000000";
    CosTradingRepos::ServiceTypeRepository::_tc_ValueTypeRedefinition = "01000000160000008c050000010000004400000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f56616c7565547970655265646566696e6974696f6e3a312e30001600"
    "000056616c7565547970655265646566696e6974696f6e00000004000000"
    "07000000747970655f3100001500000090000000010000002b0000004944"
    "4c3a6f6d672e6f72672f436f7354726164696e672f536572766963655479"
    "70654e616d653a312e3000001000000053657276696365547970654e616d"
    "65001500000040000000010000002300000049444c3a6f6d672e6f72672f"
    "436f7354726164696e672f49737472696e673a312e300000080000004973"
    "7472696e670012000000000000000d000000646566696e6974696f6e5f31"
    "000000000f000000d0010000010000003900000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f50726f705374727563743a312e30000000000b00000050726f705374"
    "72756374000003000000050000006e616d6500000000150000008c000000"
    "010000002800000049444c3a6f6d672e6f72672f436f7354726164696e67"
    "2f50726f70657274794e616d653a312e30000d00000050726f7065727479"
    "4e616d65000000001500000040000000010000002300000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f49737472696e673a312e300000"
    "0800000049737472696e670012000000000000000b00000076616c75655f"
    "7479706500000c000000050000006d6f64650000000011000000b0000000"
    "010000003b00000049444c3a436f7354726164696e675265706f732f5365"
    "7276696365547970655265706f7369746f72792f50726f70657274794d6f"
    "64653a312e3000000d00000050726f70657274794d6f6465000000000400"
    "00000c00000050524f505f4e4f524d414c000e00000050524f505f524541"
    "444f4e4c590000000f00000050524f505f4d414e4441544f525900001800"
    "000050524f505f4d414e4441544f52595f524541444f4e4c590007000000"
    "747970655f3200001500000090000000010000002b00000049444c3a6f6d"
    "672e6f72672f436f7354726164696e672f53657276696365547970654e61"
    "6d653a312e3000001000000053657276696365547970654e616d65001500"
    "000040000000010000002300000049444c3a6f6d672e6f72672f436f7354"
    "726164696e672f49737472696e673a312e3000000800000049737472696e"
    "670012000000000000000d000000646566696e6974696f6e5f3200000000"
    "0f000000d0010000010000003900000049444c3a436f7354726164696e67"
    "5265706f732f53657276696365547970655265706f7369746f72792f5072"
    "6f705374727563743a312e30000000000b00000050726f70537472756374"
    "000003000000050000006e616d6500000000150000008c00000001000000"
    "2800000049444c3a6f6d672e6f72672f436f7354726164696e672f50726f"
    "70657274794e616d653a312e30000d00000050726f70657274794e616d65"
    "000000001500000040000000010000002300000049444c3a6f6d672e6f72"
    "672f436f7354726164696e672f49737472696e673a312e30000008000000"
    "49737472696e670012000000000000000b00000076616c75655f74797065"
    "00000c000000050000006d6f64650000000011000000b000000001000000"
    "3b00000049444c3a436f7354726164696e675265706f732f536572766963"
    "65547970655265706f7369746f72792f50726f70657274794d6f64653a31"
    "2e3000000d00000050726f70657274794d6f646500000000040000000c00"
    "000050524f505f4e4f524d414c000e00000050524f505f524541444f4e4c"
    "590000000f00000050524f505f4d414e4441544f52590000180000005052"
    "4f505f4d414e4441544f52595f524541444f4e4c5900";
    CosTradingRepos::ServiceTypeRepository::_tc_DuplicateServiceTypeName = "010000001600000018010000010000004700000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "792f4475706c696361746553657276696365547970654e616d653a312e30"
    "0000190000004475706c696361746553657276696365547970654e616d65"
    "0000000001000000050000006e616d650000000015000000900000000100"
    "00002b00000049444c3a6f6d672e6f72672f436f7354726164696e672f53"
    "657276696365547970654e616d653a312e30000010000000536572766963"
    "65547970654e616d65001500000040000000010000002300000049444c3a"
    "6f6d672e6f72672f436f7354726164696e672f49737472696e673a312e30"
    "00000800000049737472696e67001200000000000000";
    CosTradingRepos::_tc_ServiceTypeRepository = "010000000e00000052000000010000002e00000049444c3a436f73547261"
    "64696e675265706f732f53657276696365547970655265706f7369746f72"
    "793a312e300000001600000053657276696365547970655265706f736974"
    "6f727900";
  }
};

static __tc_init_TYPEREPO __init_TYPEREPO;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface ServiceTypeRepository
CosTradingRepos::ServiceTypeRepository_skel::ServiceTypeRepository_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:CosTradingRepos/ServiceTypeRepository:1.0", "ServiceTypeRepository" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:CosTradingRepos/ServiceTypeRepository:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<ServiceTypeRepository_skel>( this ) );
}

CosTradingRepos::ServiceTypeRepository_skel::ServiceTypeRepository_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:CosTradingRepos/ServiceTypeRepository:1.0", "ServiceTypeRepository" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<ServiceTypeRepository_skel>( this ) );
}

CosTradingRepos::ServiceTypeRepository_skel::~ServiceTypeRepository_skel()
{
}

bool CosTradingRepos::ServiceTypeRepository_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  if( strcmp( _req->op_name(), "_get_incarnation" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    IncarnationNumber _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = incarnation();
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

CosTradingRepos::ServiceTypeRepository_ptr CosTradingRepos::ServiceTypeRepository_skel::_this()
{
  return CosTradingRepos::ServiceTypeRepository::_duplicate( this );
}

