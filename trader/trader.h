/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__TRADER_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __TRADER_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL CosTrading {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_CosTrading)


class Link;
typedef Link *Link_ptr;
typedef Link_ptr LinkRef;
typedef ObjVar<Link> Link_var;
typedef Link_var Link_out;


// Common definitions for interface Link
class Link : virtual public CORBA::Object
{
  public:
    virtual ~Link();
    static Link_ptr _duplicate( Link_ptr obj );
    static Link_ptr _narrow( CORBA::Object_ptr obj );
    static Link_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    Link() {};
  private:
    Link( const Link& );
    void operator=( const Link& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Link;

class Link_skel :
  virtual public MethodDispatcher,
  virtual public Link
{
  public:
    Link_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Link_skel();
    Link_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Link_ptr _this();

};

class Proxy;
typedef Proxy *Proxy_ptr;
typedef Proxy_ptr ProxyRef;
typedef ObjVar<Proxy> Proxy_var;
typedef Proxy_var Proxy_out;


// Common definitions for interface Proxy
class Proxy : virtual public CORBA::Object
{
  public:
    virtual ~Proxy();
    static Proxy_ptr _duplicate( Proxy_ptr obj );
    static Proxy_ptr _narrow( CORBA::Object_ptr obj );
    static Proxy_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    Proxy() {};
  private:
    Proxy( const Proxy& );
    void operator=( const Proxy& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Proxy;

class Proxy_skel :
  virtual public MethodDispatcher,
  virtual public Proxy
{
  public:
    Proxy_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Proxy_skel();
    Proxy_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Proxy_ptr _this();

};

class Admin;
typedef Admin *Admin_ptr;
typedef Admin_ptr AdminRef;
typedef ObjVar<Admin> Admin_var;
typedef Admin_var Admin_out;


// Common definitions for interface Admin
class Admin : virtual public CORBA::Object
{
  public:
    virtual ~Admin();
    static Admin_ptr _duplicate( Admin_ptr obj );
    static Admin_ptr _narrow( CORBA::Object_ptr obj );
    static Admin_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    typedef SequenceTmpl<CORBA::OctetWrapper> OctetSeq;
    #ifdef _WINDOWS
    static OctetSeq _dummy_OctetSeq;
    #endif
    typedef TSeqVar<SequenceTmpl<CORBA::OctetWrapper> > OctetSeq_var;
    typedef OctetSeq_var OctetSeq_out;

    static CORBA::TypeCodeConst _tc_OctetSeq;

  protected:
    Admin() {};
  private:
    Admin( const Admin& );
    void operator=( const Admin& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Admin;

class Admin_skel :
  virtual public MethodDispatcher,
  virtual public Admin
{
  public:
    Admin_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Admin_skel();
    Admin_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Admin_ptr _this();

};

typedef char* Istring;
typedef CORBA::String_var Istring_var;
typedef CORBA::String_out Istring_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Istring;

typedef CORBA::Object TypeRepository;
typedef CORBA::Object_ptr TypeRepository_ptr;
typedef CORBA::ObjectRef TypeRepositoryRef;
typedef CORBA::Object_var TypeRepository_var;
typedef TypeRepository_var TypeRepository_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_TypeRepository;

typedef char* PropertyName;
typedef CORBA::String_var PropertyName_var;
typedef CORBA::String_out PropertyName_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PropertyName;

typedef SequenceTmpl<PropertyName_var> PropertyNameSeq;
#ifdef _WINDOWS
static PropertyNameSeq _dummy_PropertyNameSeq;
#endif
typedef TSeqVar<SequenceTmpl<PropertyName_var> > PropertyNameSeq_var;
typedef PropertyNameSeq_var PropertyNameSeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PropertyNameSeq;

typedef CORBA::Any PropertyValue;
#ifdef _WINDOWS
static PropertyValue _dummy_PropertyValue;
#endif
typedef CORBA::Any_var PropertyValue_var;
typedef PropertyValue_var PropertyValue_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PropertyValue;

struct Property {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  Property();
  ~Property();
  Property( const Property& s );
  Property& operator=( const Property& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  PropertyName_var name;
  PropertyValue value;
  CORBA::Boolean is_file;
};

typedef TVarVar<Property> Property_var;
typedef Property_var Property_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Property;

typedef SequenceTmpl<Property> PropertySeq;
#ifdef _WINDOWS
static PropertySeq _dummy_PropertySeq;
#endif
typedef TSeqVar<SequenceTmpl<Property> > PropertySeq_var;
typedef PropertySeq_var PropertySeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PropertySeq;

struct Offer {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  Offer();
  ~Offer();
  Offer( const Offer& s );
  Offer& operator=( const Offer& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  CORBA::Object_var reference;
  PropertySeq properties;
};

typedef TVarVar<Offer> Offer_var;
typedef Offer_var Offer_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Offer;

typedef SequenceTmpl<Offer> OfferSeq;
#ifdef _WINDOWS
static OfferSeq _dummy_OfferSeq;
#endif
typedef TSeqVar<SequenceTmpl<Offer> > OfferSeq_var;
typedef OfferSeq_var OfferSeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_OfferSeq;

typedef char* OfferId;
typedef CORBA::String_var OfferId_var;
typedef CORBA::String_out OfferId_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_OfferId;

typedef SequenceTmpl<OfferId_var> OfferIdSeq;
#ifdef _WINDOWS
static OfferIdSeq _dummy_OfferIdSeq;
#endif
typedef TSeqVar<SequenceTmpl<OfferId_var> > OfferIdSeq_var;
typedef OfferIdSeq_var OfferIdSeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_OfferIdSeq;

typedef char* ServiceTypeName;
typedef CORBA::String_var ServiceTypeName_var;
typedef CORBA::String_out ServiceTypeName_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_ServiceTypeName;

typedef char* Constraint;
typedef CORBA::String_var Constraint_var;
typedef CORBA::String_out Constraint_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Constraint;

enum FollowOption {
  local_only = 0,
  if_no_local,
  always
};

typedef FollowOption& FollowOption_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_FollowOption;

typedef char* PolicyName;
typedef CORBA::String_var PolicyName_var;
typedef CORBA::String_out PolicyName_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PolicyName;

typedef SequenceTmpl<PolicyName_var> PolicyNameSeq;
#ifdef _WINDOWS
static PolicyNameSeq _dummy_PolicyNameSeq;
#endif
typedef TSeqVar<SequenceTmpl<PolicyName_var> > PolicyNameSeq_var;
typedef PolicyNameSeq_var PolicyNameSeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PolicyNameSeq;

typedef CORBA::Any PolicyValue;
#ifdef _WINDOWS
static PolicyValue _dummy_PolicyValue;
#endif
typedef CORBA::Any_var PolicyValue_var;
typedef PolicyValue_var PolicyValue_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PolicyValue;

struct Policy {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  Policy();
  ~Policy();
  Policy( const Policy& s );
  Policy& operator=( const Policy& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  PolicyName_var name;
  PolicyValue value;
};

typedef TVarVar<Policy> Policy_var;
typedef Policy_var Policy_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Policy;

typedef SequenceTmpl<Policy> PolicySeq;
#ifdef _WINDOWS
static PolicySeq _dummy_PolicySeq;
#endif
typedef TSeqVar<SequenceTmpl<Policy> > PolicySeq_var;
typedef PolicySeq_var PolicySeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PolicySeq;

struct UnknownMaxLeft : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  UnknownMaxLeft();
  ~UnknownMaxLeft();
  UnknownMaxLeft( const UnknownMaxLeft& s );
  UnknownMaxLeft& operator=( const UnknownMaxLeft& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static UnknownMaxLeft *_narrow( CORBA::Exception *ex );
};

typedef ExceptVar<UnknownMaxLeft> UnknownMaxLeft_var;
typedef UnknownMaxLeft_var UnknownMaxLeft_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_UnknownMaxLeft;

struct NotImplemented : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  NotImplemented();
  ~NotImplemented();
  NotImplemented( const NotImplemented& s );
  NotImplemented& operator=( const NotImplemented& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static NotImplemented *_narrow( CORBA::Exception *ex );
};

typedef ExceptVar<NotImplemented> NotImplemented_var;
typedef NotImplemented_var NotImplemented_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_NotImplemented;

struct IllegalServiceType : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  IllegalServiceType();
  ~IllegalServiceType();
  IllegalServiceType( const IllegalServiceType& s );
  IllegalServiceType& operator=( const IllegalServiceType& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static IllegalServiceType *_narrow( CORBA::Exception *ex );
  ServiceTypeName_var type;
};

typedef ExceptVar<IllegalServiceType> IllegalServiceType_var;
typedef IllegalServiceType_var IllegalServiceType_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_IllegalServiceType;

struct UnknownServiceType : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  UnknownServiceType();
  ~UnknownServiceType();
  UnknownServiceType( const UnknownServiceType& s );
  UnknownServiceType& operator=( const UnknownServiceType& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static UnknownServiceType *_narrow( CORBA::Exception *ex );
  ServiceTypeName_var type;
};

typedef ExceptVar<UnknownServiceType> UnknownServiceType_var;
typedef UnknownServiceType_var UnknownServiceType_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_UnknownServiceType;

struct IllegalPropertyName : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  IllegalPropertyName();
  ~IllegalPropertyName();
  IllegalPropertyName( const IllegalPropertyName& s );
  IllegalPropertyName& operator=( const IllegalPropertyName& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static IllegalPropertyName *_narrow( CORBA::Exception *ex );
  PropertyName_var name;
};

typedef ExceptVar<IllegalPropertyName> IllegalPropertyName_var;
typedef IllegalPropertyName_var IllegalPropertyName_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_IllegalPropertyName;

struct DuplicatePropertyName : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  DuplicatePropertyName();
  ~DuplicatePropertyName();
  DuplicatePropertyName( const DuplicatePropertyName& s );
  DuplicatePropertyName& operator=( const DuplicatePropertyName& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static DuplicatePropertyName *_narrow( CORBA::Exception *ex );
  PropertyName_var name;
};

typedef ExceptVar<DuplicatePropertyName> DuplicatePropertyName_var;
typedef DuplicatePropertyName_var DuplicatePropertyName_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_DuplicatePropertyName;

struct PropertyTypeMismatch : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  PropertyTypeMismatch();
  ~PropertyTypeMismatch();
  PropertyTypeMismatch( const PropertyTypeMismatch& s );
  PropertyTypeMismatch& operator=( const PropertyTypeMismatch& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static PropertyTypeMismatch *_narrow( CORBA::Exception *ex );
  ServiceTypeName_var type;
  Property prop;
};

typedef ExceptVar<PropertyTypeMismatch> PropertyTypeMismatch_var;
typedef PropertyTypeMismatch_var PropertyTypeMismatch_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_PropertyTypeMismatch;

struct MissingMandatoryProperty : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  MissingMandatoryProperty();
  ~MissingMandatoryProperty();
  MissingMandatoryProperty( const MissingMandatoryProperty& s );
  MissingMandatoryProperty& operator=( const MissingMandatoryProperty& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static MissingMandatoryProperty *_narrow( CORBA::Exception *ex );
  ServiceTypeName_var type;
  PropertyName_var name;
};

typedef ExceptVar<MissingMandatoryProperty> MissingMandatoryProperty_var;
typedef MissingMandatoryProperty_var MissingMandatoryProperty_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_MissingMandatoryProperty;

struct ReadonlyDynamicProperty : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  ReadonlyDynamicProperty();
  ~ReadonlyDynamicProperty();
  ReadonlyDynamicProperty( const ReadonlyDynamicProperty& s );
  ReadonlyDynamicProperty& operator=( const ReadonlyDynamicProperty& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static ReadonlyDynamicProperty *_narrow( CORBA::Exception *ex );
  ServiceTypeName_var type;
  PropertyName_var name;
};

typedef ExceptVar<ReadonlyDynamicProperty> ReadonlyDynamicProperty_var;
typedef ReadonlyDynamicProperty_var ReadonlyDynamicProperty_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_ReadonlyDynamicProperty;

struct IllegalConstraint : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  IllegalConstraint();
  ~IllegalConstraint();
  IllegalConstraint( const IllegalConstraint& s );
  IllegalConstraint& operator=( const IllegalConstraint& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static IllegalConstraint *_narrow( CORBA::Exception *ex );
  Constraint_var constr;
};

typedef ExceptVar<IllegalConstraint> IllegalConstraint_var;
typedef IllegalConstraint_var IllegalConstraint_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_IllegalConstraint;

class Lookup;
typedef Lookup *Lookup_ptr;
typedef Lookup_ptr LookupRef;
typedef ObjVar<Lookup> Lookup_var;
typedef Lookup_var Lookup_out;

struct InvalidLookupRef : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  InvalidLookupRef();
  ~InvalidLookupRef();
  InvalidLookupRef( const InvalidLookupRef& s );
  InvalidLookupRef& operator=( const InvalidLookupRef& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static InvalidLookupRef *_narrow( CORBA::Exception *ex );
  Lookup_var target;
};

typedef ExceptVar<InvalidLookupRef> InvalidLookupRef_var;
typedef InvalidLookupRef_var InvalidLookupRef_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_InvalidLookupRef;

struct IllegalOfferId : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  IllegalOfferId();
  ~IllegalOfferId();
  IllegalOfferId( const IllegalOfferId& s );
  IllegalOfferId& operator=( const IllegalOfferId& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static IllegalOfferId *_narrow( CORBA::Exception *ex );
  OfferId_var id;
};

typedef ExceptVar<IllegalOfferId> IllegalOfferId_var;
typedef IllegalOfferId_var IllegalOfferId_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_IllegalOfferId;

struct UnknownOfferId : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  UnknownOfferId();
  ~UnknownOfferId();
  UnknownOfferId( const UnknownOfferId& s );
  UnknownOfferId& operator=( const UnknownOfferId& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static UnknownOfferId *_narrow( CORBA::Exception *ex );
  OfferId_var id;
};

typedef ExceptVar<UnknownOfferId> UnknownOfferId_var;
typedef UnknownOfferId_var UnknownOfferId_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_UnknownOfferId;

struct DuplicatePolicyName : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  DuplicatePolicyName();
  ~DuplicatePolicyName();
  DuplicatePolicyName( const DuplicatePolicyName& s );
  DuplicatePolicyName& operator=( const DuplicatePolicyName& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  CORBA::Exception *_clone() const;
  static DuplicatePolicyName *_narrow( CORBA::Exception *ex );
  PolicyName_var name;
};

typedef ExceptVar<DuplicatePolicyName> DuplicatePolicyName_var;
typedef DuplicatePolicyName_var DuplicatePolicyName_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_DuplicatePolicyName;

class Register;
typedef Register *Register_ptr;
typedef Register_ptr RegisterRef;
typedef ObjVar<Register> Register_var;
typedef Register_var Register_out;

class TraderComponents;
typedef TraderComponents *TraderComponents_ptr;
typedef TraderComponents_ptr TraderComponentsRef;
typedef ObjVar<TraderComponents> TraderComponents_var;
typedef TraderComponents_var TraderComponents_out;


// Common definitions for interface TraderComponents
class TraderComponents : virtual public CORBA::Object
{
  public:
    virtual ~TraderComponents();
    static TraderComponents_ptr _duplicate( TraderComponents_ptr obj );
    static TraderComponents_ptr _narrow( CORBA::Object_ptr obj );
    static TraderComponents_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual Lookup_ptr lookup_if() = 0;
    virtual Register_ptr register_if() = 0;
    virtual Link_ptr link_if() = 0;
    virtual Proxy_ptr proxy_if() = 0;
    virtual Admin_ptr admin_if() = 0;
  protected:
    TraderComponents() {};
  private:
    TraderComponents( const TraderComponents& );
    void operator=( const TraderComponents& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_TraderComponents;

class TraderComponents_skel :
  virtual public MethodDispatcher,
  virtual public TraderComponents
{
  public:
    TraderComponents_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~TraderComponents_skel();
    TraderComponents_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    TraderComponents_ptr _this();

};

class SupportAttributes;
typedef SupportAttributes *SupportAttributes_ptr;
typedef SupportAttributes_ptr SupportAttributesRef;
typedef ObjVar<SupportAttributes> SupportAttributes_var;
typedef SupportAttributes_var SupportAttributes_out;


// Common definitions for interface SupportAttributes
class SupportAttributes : virtual public CORBA::Object
{
  public:
    virtual ~SupportAttributes();
    static SupportAttributes_ptr _duplicate( SupportAttributes_ptr obj );
    static SupportAttributes_ptr _narrow( CORBA::Object_ptr obj );
    static SupportAttributes_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual CORBA::Boolean supports_modifiable_properties() = 0;
    virtual CORBA::Boolean supports_dynamic_properties() = 0;
    virtual CORBA::Boolean supports_proxy_offers() = 0;
    virtual TypeRepository_ptr type_repos() = 0;
  protected:
    SupportAttributes() {};
  private:
    SupportAttributes( const SupportAttributes& );
    void operator=( const SupportAttributes& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_SupportAttributes;

class SupportAttributes_skel :
  virtual public MethodDispatcher,
  virtual public SupportAttributes
{
  public:
    SupportAttributes_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~SupportAttributes_skel();
    SupportAttributes_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    SupportAttributes_ptr _this();

};

class ImportAttributes;
typedef ImportAttributes *ImportAttributes_ptr;
typedef ImportAttributes_ptr ImportAttributesRef;
typedef ObjVar<ImportAttributes> ImportAttributes_var;
typedef ImportAttributes_var ImportAttributes_out;


// Common definitions for interface ImportAttributes
class ImportAttributes : virtual public CORBA::Object
{
  public:
    virtual ~ImportAttributes();
    static ImportAttributes_ptr _duplicate( ImportAttributes_ptr obj );
    static ImportAttributes_ptr _narrow( CORBA::Object_ptr obj );
    static ImportAttributes_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual CORBA::ULong def_search_card() = 0;
    virtual CORBA::ULong max_search_card() = 0;
    virtual CORBA::ULong def_match_card() = 0;
    virtual CORBA::ULong max_match_card() = 0;
    virtual CORBA::ULong def_return_card() = 0;
    virtual CORBA::ULong max_return_card() = 0;
    virtual CORBA::ULong max_list() = 0;
    virtual CORBA::ULong def_hop_count() = 0;
    virtual CORBA::ULong max_hop_count() = 0;
    virtual FollowOption def_follow_policy() = 0;
    virtual FollowOption max_follow_policy() = 0;
  protected:
    ImportAttributes() {};
  private:
    ImportAttributes( const ImportAttributes& );
    void operator=( const ImportAttributes& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_ImportAttributes;

class ImportAttributes_skel :
  virtual public MethodDispatcher,
  virtual public ImportAttributes
{
  public:
    ImportAttributes_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~ImportAttributes_skel();
    ImportAttributes_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    ImportAttributes_ptr _this();

};

class OfferIterator;
typedef OfferIterator *OfferIterator_ptr;
typedef OfferIterator_ptr OfferIteratorRef;
typedef ObjVar<OfferIterator> OfferIterator_var;
typedef OfferIterator_var OfferIterator_out;


// Common definitions for interface Lookup
class Lookup : 
  virtual public ::CosTrading::TraderComponents,
  virtual public ::CosTrading::SupportAttributes,
  virtual public ::CosTrading::ImportAttributes
{
  public:
    virtual ~Lookup();
    static Lookup_ptr _duplicate( Lookup_ptr obj );
    static Lookup_ptr _narrow( CORBA::Object_ptr obj );
    static Lookup_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    typedef char* Preference;
    typedef CORBA::String_var Preference_var;
    typedef CORBA::String_out Preference_out;

    static CORBA::TypeCodeConst _tc_Preference;

    enum HowManyProps {
      none = 0,
      some,
      all
    };

    typedef HowManyProps& HowManyProps_out;

    static CORBA::TypeCodeConst _tc_HowManyProps;

    class SpecifiedProps {
      public:
        HowManyProps _discriminator;

        struct __m {
          PropertyNameSeq prop_names;
        } _m;

      public:
        #ifdef HAVE_EXPLICIT_STRUCT_OPS
        SpecifiedProps();
        ~SpecifiedProps();
        SpecifiedProps( const SpecifiedProps& s );
        SpecifiedProps& operator=( const SpecifiedProps& s );
        #endif //HAVE_EXPLICIT_STRUCT_OPS

        void _d( HowManyProps _p );
        HowManyProps _d() const;

        void prop_names( const PropertyNameSeq& _p );
        const PropertyNameSeq& prop_names() const;
        PropertyNameSeq& prop_names();

        void _default();

    };

    typedef TVarVar<SpecifiedProps> SpecifiedProps_var;
    typedef SpecifiedProps_var SpecifiedProps_out;

    static CORBA::TypeCodeConst _tc_SpecifiedProps;

    struct IllegalPreference : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      IllegalPreference();
      ~IllegalPreference();
      IllegalPreference( const IllegalPreference& s );
      IllegalPreference& operator=( const IllegalPreference& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static IllegalPreference *_narrow( CORBA::Exception *ex );
      Preference_var pref;
    };

    typedef ExceptVar<IllegalPreference> IllegalPreference_var;
    typedef IllegalPreference_var IllegalPreference_out;

    static CORBA::TypeCodeConst _tc_IllegalPreference;

    struct IllegalPolicyName : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      IllegalPolicyName();
      ~IllegalPolicyName();
      IllegalPolicyName( const IllegalPolicyName& s );
      IllegalPolicyName& operator=( const IllegalPolicyName& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static IllegalPolicyName *_narrow( CORBA::Exception *ex );
      PolicyName_var name;
    };

    typedef ExceptVar<IllegalPolicyName> IllegalPolicyName_var;
    typedef IllegalPolicyName_var IllegalPolicyName_out;

    static CORBA::TypeCodeConst _tc_IllegalPolicyName;

    struct PolicyTypeMismatch : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      PolicyTypeMismatch();
      ~PolicyTypeMismatch();
      PolicyTypeMismatch( const PolicyTypeMismatch& s );
      PolicyTypeMismatch& operator=( const PolicyTypeMismatch& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static PolicyTypeMismatch *_narrow( CORBA::Exception *ex );
      Policy the_policy;
    };

    typedef ExceptVar<PolicyTypeMismatch> PolicyTypeMismatch_var;
    typedef PolicyTypeMismatch_var PolicyTypeMismatch_out;

    static CORBA::TypeCodeConst _tc_PolicyTypeMismatch;

    struct InvalidPolicyValue : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      InvalidPolicyValue();
      ~InvalidPolicyValue();
      InvalidPolicyValue( const InvalidPolicyValue& s );
      InvalidPolicyValue& operator=( const InvalidPolicyValue& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static InvalidPolicyValue *_narrow( CORBA::Exception *ex );
      Policy the_policy;
    };

    typedef ExceptVar<InvalidPolicyValue> InvalidPolicyValue_var;
    typedef InvalidPolicyValue_var InvalidPolicyValue_out;

    static CORBA::TypeCodeConst _tc_InvalidPolicyValue;

    virtual void query( const char* type, const char* constr, const char* pref, const PolicySeq& policies, const SpecifiedProps& desired_props, CORBA::ULong how_many, OfferSeq*& offers, OfferIterator_ptr& offer_itr, PolicyNameSeq*& limits_applied ) = 0;
  protected:
    Lookup() {};
  private:
    Lookup( const Lookup& );
    void operator=( const Lookup& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Lookup;

class Lookup_skel :
  virtual public MethodDispatcher,
  virtual public Lookup
{
  public:
    Lookup_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Lookup_skel();
    Lookup_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Lookup_ptr _this();

};


// Common definitions for interface Register
class Register : virtual public CORBA::Object
{
  public:
    virtual ~Register();
    static Register_ptr _duplicate( Register_ptr obj );
    static Register_ptr _narrow( CORBA::Object_ptr obj );
    static Register_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    struct InterfaceTypeMismatch : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      InterfaceTypeMismatch();
      ~InterfaceTypeMismatch();
      InterfaceTypeMismatch( const InterfaceTypeMismatch& s );
      InterfaceTypeMismatch& operator=( const InterfaceTypeMismatch& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static InterfaceTypeMismatch *_narrow( CORBA::Exception *ex );
      ServiceTypeName_var type;
      CORBA::Object_var reference;
    };

    typedef ExceptVar<InterfaceTypeMismatch> InterfaceTypeMismatch_var;
    typedef InterfaceTypeMismatch_var InterfaceTypeMismatch_out;

    static CORBA::TypeCodeConst _tc_InterfaceTypeMismatch;

    struct InvalidObjectRef : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      InvalidObjectRef();
      ~InvalidObjectRef();
      InvalidObjectRef( const InvalidObjectRef& s );
      InvalidObjectRef& operator=( const InvalidObjectRef& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static InvalidObjectRef *_narrow( CORBA::Exception *ex );
      CORBA::Object_var ref;
    };

    typedef ExceptVar<InvalidObjectRef> InvalidObjectRef_var;
    typedef InvalidObjectRef_var InvalidObjectRef_out;

    static CORBA::TypeCodeConst _tc_InvalidObjectRef;

  protected:
    Register() {};
  private:
    Register( const Register& );
    void operator=( const Register& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Register;

class Register_skel :
  virtual public MethodDispatcher,
  virtual public Register
{
  public:
    Register_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Register_skel();
    Register_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Register_ptr _this();

};


// Common definitions for interface OfferIterator
class OfferIterator : virtual public CORBA::Object
{
  public:
    virtual ~OfferIterator();
    static OfferIterator_ptr _duplicate( OfferIterator_ptr obj );
    static OfferIterator_ptr _narrow( CORBA::Object_ptr obj );
    static OfferIterator_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual CORBA::ULong max_left() = 0;
    virtual CORBA::Boolean next_n( CORBA::ULong n, OfferSeq*& offers ) = 0;
    virtual void destroy() = 0;
  protected:
    OfferIterator() {};
  private:
    OfferIterator( const OfferIterator& );
    void operator=( const OfferIterator& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_OfferIterator;

class OfferIterator_skel :
  virtual public MethodDispatcher,
  virtual public OfferIterator
{
  public:
    OfferIterator_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~OfferIterator_skel();
    OfferIterator_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    OfferIterator_ptr _this();

};

typedef SequenceTmpl<CORBA::Long> LongList;
#ifdef _WINDOWS
static LongList _dummy_LongList;
#endif
typedef TSeqVar<SequenceTmpl<CORBA::Long> > LongList_var;
typedef LongList_var LongList_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_LongList;

typedef SequenceTmpl<CORBA::Float> FloatList;
#ifdef _WINDOWS
static FloatList _dummy_FloatList;
#endif
typedef TSeqVar<SequenceTmpl<CORBA::Float> > FloatList_var;
typedef FloatList_var FloatList_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_FloatList;

typedef SequenceTmpl<CORBA::String_var> StringList;
#ifdef _WINDOWS
static StringList _dummy_StringList;
#endif
typedef TSeqVar<SequenceTmpl<CORBA::String_var> > StringList_var;
typedef StringList_var StringList_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_StringList;

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_CosTrading)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif



#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::Link_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::Link_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::Proxy_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::Proxy_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::Admin_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::Admin_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTrading::Property &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTrading::Property &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTrading::Offer &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTrading::Offer &_s );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::FollowOption &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::FollowOption &e );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTrading::Policy &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTrading::Policy &_s );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::UnknownMaxLeft &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::UnknownMaxLeft &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::NotImplemented &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::NotImplemented &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::IllegalServiceType &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::IllegalServiceType &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::UnknownServiceType &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::UnknownServiceType &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::IllegalPropertyName &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::IllegalPropertyName &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::DuplicatePropertyName &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::DuplicatePropertyName &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::PropertyTypeMismatch &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::PropertyTypeMismatch &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::MissingMandatoryProperty &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::MissingMandatoryProperty &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::ReadonlyDynamicProperty &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::ReadonlyDynamicProperty &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::IllegalConstraint &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::IllegalConstraint &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::InvalidLookupRef &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::InvalidLookupRef &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::IllegalOfferId &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::IllegalOfferId &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::UnknownOfferId &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::UnknownOfferId &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::DuplicatePolicyName &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::DuplicatePolicyName &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::TraderComponents_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::TraderComponents_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::SupportAttributes_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::SupportAttributes_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::ImportAttributes_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::ImportAttributes_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::Lookup::HowManyProps &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::Lookup::HowManyProps &e );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTrading::Lookup::SpecifiedProps &_u );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTrading::Lookup::SpecifiedProps &_u );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::Lookup::IllegalPreference &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::Lookup::IllegalPreference &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::Lookup::IllegalPolicyName &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::Lookup::IllegalPolicyName &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::Lookup::PolicyTypeMismatch &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::Lookup::PolicyTypeMismatch &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::Lookup::InvalidPolicyValue &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::Lookup::InvalidPolicyValue &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::Lookup_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::Lookup_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::Register::InterfaceTypeMismatch &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::Register::InterfaceTypeMismatch &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTrading::Register::InvalidObjectRef &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTrading::Register::InvalidObjectRef &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::Register_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::Register_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTrading::OfferIterator_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTrading::OfferIterator_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::OctetWrapper> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::OctetWrapper> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTrading::PropertyName_var> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTrading::PropertyName_var> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTrading::Property> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTrading::Property> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTrading::Offer> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTrading::Offer> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTrading::Policy> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTrading::Policy> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::Long> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::Long> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::Float> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::Float> &_s );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
