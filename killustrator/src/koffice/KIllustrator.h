/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__KILLUSTRATOR_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __KILLUSTRATOR_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

// Generate forward declarations for this scope

#include "/opt/kde/include/idl/kom.h"

#include "/opt/kde/include/idl/controls.h"

#include "/opt/kde/include/idl/parts.h"

// Module KIllustrator
#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL KIllustrator {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KIllustrator)

// Generate forward declarations for this scope
class View;
typedef View *View_ptr;
typedef View_ptr ViewRef;
typedef ObjVar<View> View_var;

class Document;
typedef Document *Document_ptr;
typedef Document_ptr DocumentRef;
typedef ObjVar<Document> Document_var;

class Factory;
typedef Factory *Factory_ptr;
typedef Factory_ptr FactoryRef;
typedef ObjVar<Factory> Factory_var;



// Common definitions for interface View
class View : 
  virtual public ::OPParts::View
{
  public:
    virtual ~View();
    static View_ptr _duplicate( View_ptr obj );
    static View_ptr _narrow( CORBA::Object_ptr obj );
    static View_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    View() {};
  private:
    View( const View& );
    void operator=( const View& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_View;

// Stub for interface View
class View_stub : virtual public View,
  virtual public ::OPParts::View_stub
{
  public:
    virtual ~View_stub();
  private:
    void operator=( const View_stub& );
};

class View_skel :
  virtual public MethodDispatcher,
  virtual public View
{
  public:
    View_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~View_skel();
    View_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    View_ptr _this();

};


// Common definitions for interface Document
class Document : 
  virtual public ::OPParts::Document
{
  public:
    virtual ~Document();
    static Document_ptr _duplicate( Document_ptr obj );
    static Document_ptr _narrow( CORBA::Object_ptr obj );
    static Document_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    Document() {};
  private:
    Document( const Document& );
    void operator=( const Document& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_Document;

// Stub for interface Document
class Document_stub : virtual public Document,
  virtual public ::OPParts::Document_stub
{
  public:
    virtual ~Document_stub();
  private:
    void operator=( const Document_stub& );
};

class Document_skel :
  virtual public MethodDispatcher,
  virtual public Document
{
  public:
    Document_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Document_skel();
    Document_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Document_ptr _this();

};


// Common definitions for interface Factory
class Factory : 
  virtual public ::OPParts::Factory
{
  public:
    virtual ~Factory();
    static Factory_ptr _duplicate( Factory_ptr obj );
    static Factory_ptr _narrow( CORBA::Object_ptr obj );
    static Factory_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    Factory() {};
  private:
    Factory( const Factory& );
    void operator=( const Factory& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_Factory;

// Stub for interface Factory
class Factory_stub : virtual public Factory,
  virtual public ::OPParts::Factory_stub
{
  public:
    virtual ~Factory_stub();
  private:
    void operator=( const Factory_stub& );
};

class Factory_skel :
  virtual public MethodDispatcher,
  virtual public Factory
{
  public:
    Factory_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Factory_skel();
    Factory_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Factory_ptr _this();

};

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KIllustrator)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const KIllustrator::View_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KIllustrator::View_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const KIllustrator::Document_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KIllustrator::Document_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const KIllustrator::Factory_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KIllustrator::Factory_ptr &obj );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#ifndef MICO_NO_TOPLEVEL_MODULES
#include <mico/template_impl.h>
#endif

#endif
