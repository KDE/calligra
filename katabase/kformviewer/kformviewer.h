/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__KFORMVIEWER_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __KFORMVIEWER_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <kom.h>
#else
#define MICO_IN_GENERATED_CODE
#include <kom.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <openparts_ui.h>
#else
#define MICO_IN_GENERATED_CODE
#include <openparts_ui.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <openparts.h>
#else
#define MICO_IN_GENERATED_CODE
#include <openparts.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <koStoreIf.h>
#else
#define MICO_IN_GENERATED_CODE
#include <koStoreIf.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <koffice.h>
#else
#define MICO_IN_GENERATED_CODE
#include <koffice.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL KformViewer {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KformViewer)


class View;
typedef View *View_ptr;
typedef View_ptr ViewRef;
typedef ObjVar<View> View_var;
typedef View_var View_out;


// Common definitions for interface View
class View : 
  virtual public ::KOffice::View
{
  public:
    virtual ~View();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef View_ptr _ptr_type;
    typedef View_var _var_type;
    #endif

    static View_ptr _narrow( CORBA::Object_ptr obj );
    static View_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static View_ptr _duplicate( View_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static View_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    View() {};
  private:
    View( const View& );
    void operator=( const View& );
};

// Stub for interface View
class View_stub:
  virtual public View,
  virtual public ::KOffice::View_stub
{
  public:
    virtual ~View_stub();
  private:
    void operator=( const View_stub& );
};

class View_skel :
  virtual public StaticMethodDispatcher,
  virtual public View
{
  public:
    View_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~View_skel();
    View_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    View_ptr _this();

};

class Document;
typedef Document *Document_ptr;
typedef Document_ptr DocumentRef;
typedef ObjVar<Document> Document_var;
typedef Document_var Document_out;


// Common definitions for interface Document
class Document : 
  virtual public ::KOffice::Document,
  virtual public ::KOffice::Print
{
  public:
    virtual ~Document();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef Document_ptr _ptr_type;
    typedef Document_var _var_type;
    #endif

    static Document_ptr _narrow( CORBA::Object_ptr obj );
    static Document_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Document_ptr _duplicate( Document_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static Document_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    Document() {};
  private:
    Document( const Document& );
    void operator=( const Document& );
};

// Stub for interface Document
class Document_stub:
  virtual public Document,
  virtual public ::KOffice::Document_stub,
  virtual public ::KOffice::Print_stub
{
  public:
    virtual ~Document_stub();
  private:
    void operator=( const Document_stub& );
};

class Document_skel :
  virtual public StaticMethodDispatcher,
  virtual public Document
{
  public:
    Document_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Document_skel();
    Document_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    Document_ptr _this();

};

class DocumentFactory;
typedef DocumentFactory *DocumentFactory_ptr;
typedef DocumentFactory_ptr DocumentFactoryRef;
typedef ObjVar<DocumentFactory> DocumentFactory_var;
typedef DocumentFactory_var DocumentFactory_out;


// Common definitions for interface DocumentFactory
class DocumentFactory : 
  virtual public ::KOffice::DocumentFactory
{
  public:
    virtual ~DocumentFactory();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef DocumentFactory_ptr _ptr_type;
    typedef DocumentFactory_var _var_type;
    #endif

    static DocumentFactory_ptr _narrow( CORBA::Object_ptr obj );
    static DocumentFactory_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static DocumentFactory_ptr _duplicate( DocumentFactory_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static DocumentFactory_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    DocumentFactory() {};
  private:
    DocumentFactory( const DocumentFactory& );
    void operator=( const DocumentFactory& );
};

// Stub for interface DocumentFactory
class DocumentFactory_stub:
  virtual public DocumentFactory,
  virtual public ::KOffice::DocumentFactory_stub
{
  public:
    virtual ~DocumentFactory_stub();
  private:
    void operator=( const DocumentFactory_stub& );
};

class DocumentFactory_skel :
  virtual public StaticMethodDispatcher,
  virtual public DocumentFactory
{
  public:
    DocumentFactory_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~DocumentFactory_skel();
    DocumentFactory_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    DocumentFactory_ptr _this();

};

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KformViewer)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif



#ifndef MICO_CONF_NO_POA

#endif // MICO_CONF_NO_POA

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

extern CORBA::StaticTypeInfo *_marshaller_KformViewer_View;

extern CORBA::StaticTypeInfo *_marshaller_KformViewer_Document;

extern CORBA::StaticTypeInfo *_marshaller_KformViewer_DocumentFactory;

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
