/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__TYPEREPO_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __TYPEREPO_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <trader.h>
#else
#define MICO_IN_GENERATED_CODE
#include <trader.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL CosTradingRepos {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_CosTradingRepos)


class ServiceTypeRepository;
typedef ServiceTypeRepository *ServiceTypeRepository_ptr;
typedef ServiceTypeRepository_ptr ServiceTypeRepositoryRef;
typedef ObjVar<ServiceTypeRepository> ServiceTypeRepository_var;
typedef ServiceTypeRepository_var ServiceTypeRepository_out;


// Common definitions for interface ServiceTypeRepository
class ServiceTypeRepository : virtual public CORBA::Object
{
  public:
    virtual ~ServiceTypeRepository();
    static ServiceTypeRepository_ptr _duplicate( ServiceTypeRepository_ptr obj );
    static ServiceTypeRepository_ptr _narrow( CORBA::Object_ptr obj );
    static ServiceTypeRepository_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    typedef SequenceTmpl<CosTrading::ServiceTypeName_var> ServiceTypeNameSeq;
    #ifdef _WINDOWS
    static ServiceTypeNameSeq _dummy_ServiceTypeNameSeq;
    #endif
    typedef TSeqVar<SequenceTmpl<CosTrading::ServiceTypeName_var> > ServiceTypeNameSeq_var;
    typedef ServiceTypeNameSeq_var ServiceTypeNameSeq_out;

    static CORBA::TypeCodeConst _tc_ServiceTypeNameSeq;

    enum PropertyMode {
      PROP_NORMAL = 0,
      PROP_READONLY,
      PROP_MANDATORY,
      PROP_MANDATORY_READONLY
    };

    typedef PropertyMode& PropertyMode_out;

    static CORBA::TypeCodeConst _tc_PropertyMode;

    struct PropStruct {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      PropStruct();
      ~PropStruct();
      PropStruct( const PropStruct& s );
      PropStruct& operator=( const PropStruct& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      CosTrading::PropertyName_var name;
      CORBA::TypeCode_var value_type;
      PropertyMode mode;
    };

    typedef TVarVar<PropStruct> PropStruct_var;
    typedef PropStruct_var PropStruct_out;

    static CORBA::TypeCodeConst _tc_PropStruct;

    typedef SequenceTmpl<PropStruct> PropStructSeq;
    #ifdef _WINDOWS
    static PropStructSeq _dummy_PropStructSeq;
    #endif
    typedef TSeqVar<SequenceTmpl<PropStruct> > PropStructSeq_var;
    typedef PropStructSeq_var PropStructSeq_out;

    static CORBA::TypeCodeConst _tc_PropStructSeq;

    typedef char* Istring;
    typedef CORBA::String_var Istring_var;
    typedef CORBA::String_out Istring_out;

    static CORBA::TypeCodeConst _tc_Istring;

    typedef char* PropertyName;
    typedef CORBA::String_var PropertyName_var;
    typedef CORBA::String_out PropertyName_out;

    static CORBA::TypeCodeConst _tc_PropertyName;

    typedef CORBA::Any PropertyValue;
    #ifdef _WINDOWS
    static PropertyValue _dummy_PropertyValue;
    #endif
    typedef CORBA::Any_var PropertyValue_var;
    typedef PropertyValue_var PropertyValue_out;

    static CORBA::TypeCodeConst _tc_PropertyValue;

    struct Property {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      Property();
      ~Property();
      Property( const Property& s );
      Property& operator=( const Property& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      CORBA::TypeCode_var value_type;
      CORBA::Boolean is_file;
      PropertyName_var name;
      PropertyValue value;
    };

    typedef TVarVar<Property> Property_var;
    typedef Property_var Property_out;

    static CORBA::TypeCodeConst _tc_Property;

    typedef SequenceTmpl<Property> PropertySeq;
    #ifdef _WINDOWS
    static PropertySeq _dummy_PropertySeq;
    #endif
    typedef TSeqVar<SequenceTmpl<Property> > PropertySeq_var;
    typedef PropertySeq_var PropertySeq_out;

    static CORBA::TypeCodeConst _tc_PropertySeq;

    typedef char* Identifier;
    typedef CORBA::String_var Identifier_var;
    typedef CORBA::String_out Identifier_out;

    static CORBA::TypeCodeConst _tc_Identifier;

    struct IncarnationNumber {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      IncarnationNumber();
      ~IncarnationNumber();
      IncarnationNumber( const IncarnationNumber& s );
      IncarnationNumber& operator=( const IncarnationNumber& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      CORBA::ULong high;
      CORBA::ULong low;
    };

    typedef TFixVar<IncarnationNumber> IncarnationNumber_var;
    typedef IncarnationNumber_var IncarnationNumber_out;

    static CORBA::TypeCodeConst _tc_IncarnationNumber;

    struct TypeStruct {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      TypeStruct();
      ~TypeStruct();
      TypeStruct( const TypeStruct& s );
      TypeStruct& operator=( const TypeStruct& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      Identifier_var if_name;
      PropStructSeq props;
      ServiceTypeNameSeq super_types;
      PropertySeq values;
      CORBA::Boolean masked;
      IncarnationNumber incarnation;
    };

    typedef TVarVar<TypeStruct> TypeStruct_var;
    typedef TypeStruct_var TypeStruct_out;

    static CORBA::TypeCodeConst _tc_TypeStruct;

    enum ListOption {
      all = 0,
      since
    };

    typedef ListOption& ListOption_out;

    static CORBA::TypeCodeConst _tc_ListOption;

    class SpecifiedServiceTypes {
      public:
        ListOption _discriminator;

        struct __m {
          IncarnationNumber incarnation;
        } _m;

      public:
        #ifdef HAVE_EXPLICIT_STRUCT_OPS
        SpecifiedServiceTypes();
        ~SpecifiedServiceTypes();
        SpecifiedServiceTypes( const SpecifiedServiceTypes& s );
        SpecifiedServiceTypes& operator=( const SpecifiedServiceTypes& s );
        #endif //HAVE_EXPLICIT_STRUCT_OPS

        void _d( ListOption _p );
        ListOption _d() const;

        void incarnation( const IncarnationNumber& _p );
        const IncarnationNumber& incarnation() const;
        IncarnationNumber& incarnation();

        void _default();

    };

    typedef TFixVar<SpecifiedServiceTypes> SpecifiedServiceTypes_var;
    typedef SpecifiedServiceTypes_var SpecifiedServiceTypes_out;

    static CORBA::TypeCodeConst _tc_SpecifiedServiceTypes;

    struct ServiceTypeExists : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      ServiceTypeExists();
      ~ServiceTypeExists();
      ServiceTypeExists( const ServiceTypeExists& s );
      ServiceTypeExists& operator=( const ServiceTypeExists& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static ServiceTypeExists *_narrow( CORBA::Exception *ex );
      CosTrading::ServiceTypeName_var name;
    };

    typedef ExceptVar<ServiceTypeExists> ServiceTypeExists_var;
    typedef ServiceTypeExists_var ServiceTypeExists_out;

    static CORBA::TypeCodeConst _tc_ServiceTypeExists;

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
      CosTrading::ServiceTypeName_var base_service;
      Identifier_var base_if;
      CosTrading::ServiceTypeName_var derived_service;
      Identifier_var derived_if;
    };

    typedef ExceptVar<InterfaceTypeMismatch> InterfaceTypeMismatch_var;
    typedef InterfaceTypeMismatch_var InterfaceTypeMismatch_out;

    static CORBA::TypeCodeConst _tc_InterfaceTypeMismatch;

    struct HasSubTypes : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      HasSubTypes();
      ~HasSubTypes();
      HasSubTypes( const HasSubTypes& s );
      HasSubTypes& operator=( const HasSubTypes& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static HasSubTypes *_narrow( CORBA::Exception *ex );
      CosTrading::ServiceTypeName_var the_type;
      CosTrading::ServiceTypeName_var sub_type;
    };

    typedef ExceptVar<HasSubTypes> HasSubTypes_var;
    typedef HasSubTypes_var HasSubTypes_out;

    static CORBA::TypeCodeConst _tc_HasSubTypes;

    struct AlreadyMasked : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      AlreadyMasked();
      ~AlreadyMasked();
      AlreadyMasked( const AlreadyMasked& s );
      AlreadyMasked& operator=( const AlreadyMasked& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static AlreadyMasked *_narrow( CORBA::Exception *ex );
      CosTrading::ServiceTypeName_var name;
    };

    typedef ExceptVar<AlreadyMasked> AlreadyMasked_var;
    typedef AlreadyMasked_var AlreadyMasked_out;

    static CORBA::TypeCodeConst _tc_AlreadyMasked;

    struct NotMasked : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      NotMasked();
      ~NotMasked();
      NotMasked( const NotMasked& s );
      NotMasked& operator=( const NotMasked& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static NotMasked *_narrow( CORBA::Exception *ex );
      CosTrading::ServiceTypeName_var name;
    };

    typedef ExceptVar<NotMasked> NotMasked_var;
    typedef NotMasked_var NotMasked_out;

    static CORBA::TypeCodeConst _tc_NotMasked;

    struct ValueTypeRedefinition : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      ValueTypeRedefinition();
      ~ValueTypeRedefinition();
      ValueTypeRedefinition( const ValueTypeRedefinition& s );
      ValueTypeRedefinition& operator=( const ValueTypeRedefinition& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static ValueTypeRedefinition *_narrow( CORBA::Exception *ex );
      CosTrading::ServiceTypeName_var type_1;
      PropStruct definition_1;
      CosTrading::ServiceTypeName_var type_2;
      PropStruct definition_2;
    };

    typedef ExceptVar<ValueTypeRedefinition> ValueTypeRedefinition_var;
    typedef ValueTypeRedefinition_var ValueTypeRedefinition_out;

    static CORBA::TypeCodeConst _tc_ValueTypeRedefinition;

    struct DuplicateServiceTypeName : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      DuplicateServiceTypeName();
      ~DuplicateServiceTypeName();
      DuplicateServiceTypeName( const DuplicateServiceTypeName& s );
      DuplicateServiceTypeName& operator=( const DuplicateServiceTypeName& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static DuplicateServiceTypeName *_narrow( CORBA::Exception *ex );
      CosTrading::ServiceTypeName_var name;
    };

    typedef ExceptVar<DuplicateServiceTypeName> DuplicateServiceTypeName_var;
    typedef DuplicateServiceTypeName_var DuplicateServiceTypeName_out;

    static CORBA::TypeCodeConst _tc_DuplicateServiceTypeName;

    virtual IncarnationNumber incarnation() = 0;
  protected:
    ServiceTypeRepository() {};
  private:
    ServiceTypeRepository( const ServiceTypeRepository& );
    void operator=( const ServiceTypeRepository& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_ServiceTypeRepository;

class ServiceTypeRepository_skel :
  virtual public MethodDispatcher,
  virtual public ServiceTypeRepository
{
  public:
    ServiceTypeRepository_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~ServiceTypeRepository_skel();
    ServiceTypeRepository_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    ServiceTypeRepository_ptr _this();

};

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_CosTradingRepos)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif



#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::PropertyMode &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::PropertyMode &e );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTradingRepos::ServiceTypeRepository::PropStruct &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTradingRepos::ServiceTypeRepository::PropStruct &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTradingRepos::ServiceTypeRepository::Property &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTradingRepos::ServiceTypeRepository::Property &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTradingRepos::ServiceTypeRepository::IncarnationNumber &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTradingRepos::ServiceTypeRepository::IncarnationNumber &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTradingRepos::ServiceTypeRepository::TypeStruct &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTradingRepos::ServiceTypeRepository::TypeStruct &_s );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::ListOption &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::ListOption &e );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes &_u );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes &_u );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::ServiceTypeExists &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::ServiceTypeExists &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::InterfaceTypeMismatch &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::HasSubTypes &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::HasSubTypes &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::AlreadyMasked &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::AlreadyMasked &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::NotMasked &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::NotMasked &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const ::CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName &e );

CORBA::Boolean operator<<=( CORBA::Any &a, const CosTradingRepos::ServiceTypeRepository_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CosTradingRepos::ServiceTypeRepository_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTradingRepos::ServiceTypeRepository::PropStruct> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTradingRepos::ServiceTypeRepository::PropStruct> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CosTradingRepos::ServiceTypeRepository::Property> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CosTradingRepos::ServiceTypeRepository::Property> &_s );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
