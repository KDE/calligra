/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__KOFFICE_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __KOFFICE_H__

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
#include <koStore.h>
#else
#define MICO_IN_GENERATED_CODE
#include <koStore.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL KOffice {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KOffice)


class Document;
typedef Document *Document_ptr;
typedef Document_ptr DocumentRef;
typedef ObjVar<Document> Document_var;
typedef Document_var Document_out;

class MainWindow;
typedef MainWindow *MainWindow_ptr;
typedef MainWindow_ptr MainWindowRef;
typedef ObjVar<MainWindow> MainWindow_var;
typedef MainWindow_var MainWindow_out;


// Common definitions for interface Document
class Document : 
  virtual public ::OpenParts::Document
{
  public:
    virtual ~Document();

    typedef Document_ptr _ptr_type;
    typedef Document_var _var_type;

    static Document_ptr _duplicate( Document_ptr obj );
    static Document_ptr _narrow( CORBA::Object_ptr obj );
    static Document_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Document_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void setURL( const char* url ) = 0;
    virtual char* url() = 0;
    virtual char* mimeType() = 0;
    virtual CORBA::Boolean loadFromURL( const char* url, const char* format ) = 0;
    virtual CORBA::Boolean saveToURL( const char* url, const char* format ) = 0;
    virtual CORBA::Boolean loadFromStore( KOStore::Store_ptr store, const char* id ) = 0;
    virtual CORBA::Boolean saveToStore( KOStore::Store_ptr store, const char* format ) = 0;
    virtual CORBA::Boolean initDoc() = 0;
    virtual CORBA::Boolean isModified() = 0;
    virtual void makeChildList( Document_ptr root, const char* name ) = 0;
    virtual void addToChildList( Document_ptr child, const char* name ) = 0;
    virtual MainWindow_ptr createMainWindow() = 0;

  protected:
    Document() {};
  private:
    Document( const Document& );
    void operator=( const Document& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_Document;

// Stub for interface Document
class Document_stub:
  virtual public Document,
  virtual public ::OpenParts::Document_stub
{
  public:
    virtual ~Document_stub();
    void setURL( const char* url );
    char* url();
    char* mimeType();
    CORBA::Boolean loadFromURL( const char* url, const char* format );
    CORBA::Boolean saveToURL( const char* url, const char* format );
    CORBA::Boolean loadFromStore( KOStore::Store_ptr store, const char* id );
    CORBA::Boolean saveToStore( KOStore::Store_ptr store, const char* format );
    CORBA::Boolean initDoc();
    CORBA::Boolean isModified();
    void makeChildList( Document_ptr root, const char* name );
    void addToChildList( Document_ptr child, const char* name );
    MainWindow_ptr createMainWindow();

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

class Print;
typedef Print *Print_ptr;
typedef Print_ptr PrintRef;
typedef ObjVar<Print> Print_var;
typedef Print_var Print_out;


// Common definitions for interface Print
class Print : 
  virtual public CORBA::Object
{
  public:
    virtual ~Print();

    typedef Print_ptr _ptr_type;
    typedef Print_var _var_type;

    static Print_ptr _duplicate( Print_ptr obj );
    static Print_ptr _narrow( CORBA::Object_ptr obj );
    static Print_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Print_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual char* encodedMetaFile( CORBA::Long width, CORBA::Long height, CORBA::Float scale ) = 0;

  protected:
    Print() {};
  private:
    Print( const Print& );
    void operator=( const Print& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_Print;

// Stub for interface Print
class Print_stub:
  virtual public Print
{
  public:
    virtual ~Print_stub();
    char* encodedMetaFile( CORBA::Long width, CORBA::Long height, CORBA::Float scale );

  private:
    void operator=( const Print_stub& );
};

class Print_skel :
  virtual public StaticMethodDispatcher,
  virtual public Print
{
  public:
    Print_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Print_skel();
    Print_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    Print_ptr _this();

};

class View;
typedef View *View_ptr;
typedef View_ptr ViewRef;
typedef ObjVar<View> View_var;
typedef View_var View_out;


// Common definitions for interface View
class View : 
  virtual public ::OpenParts::View
{
  public:
    virtual ~View();

    typedef View_ptr _ptr_type;
    typedef View_var _var_type;

    static View_ptr _duplicate( View_ptr obj );
    static View_ptr _narrow( CORBA::Object_ptr obj );
    static View_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static View_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    enum Mode {
      ChildMode = 0,
      RootMode
    };

    typedef Mode& Mode_out;

    static CORBA::TypeCodeConst _tc_Mode;

    struct EventNewPart;
    typedef TVarVar<EventNewPart> EventNewPart_var;
    typedef EventNewPart_var EventNewPart_out;

    struct EventNewPart {
      typedef EventNewPart_var _var_type;
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      EventNewPart();
      ~EventNewPart();
      EventNewPart( const EventNewPart& s );
      EventNewPart& operator=( const EventNewPart& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS

      View_var view;
    };

    static CORBA::TypeCodeConst _tc_EventNewPart;

    static const char* eventNewPart;
    virtual CORBA::Boolean isMarked() = 0;
    virtual void setMarked( CORBA::Boolean marked ) = 0;
    virtual void setMode( Mode mode ) = 0;
    virtual Mode mode() = 0;
    virtual CORBA::ULong leftGUISize() = 0;
    virtual CORBA::ULong rightGUISize() = 0;
    virtual CORBA::ULong topGUISize() = 0;
    virtual CORBA::ULong bottomGUISize() = 0;
    virtual CORBA::Boolean printDlg() = 0;

  protected:
    View() {};
  private:
    View( const View& );
    void operator=( const View& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_View;

// Stub for interface View
class View_stub:
  virtual public View,
  virtual public ::OpenParts::View_stub
{
  public:
    virtual ~View_stub();
    CORBA::Boolean isMarked();
    void setMarked( CORBA::Boolean marked );
    void setMode( Mode mode );
    Mode mode();
    CORBA::ULong leftGUISize();
    CORBA::ULong rightGUISize();
    CORBA::ULong topGUISize();
    CORBA::ULong bottomGUISize();
    CORBA::Boolean printDlg();

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

class DocumentFactory;
typedef DocumentFactory *DocumentFactory_ptr;
typedef DocumentFactory_ptr DocumentFactoryRef;
typedef ObjVar<DocumentFactory> DocumentFactory_var;
typedef DocumentFactory_var DocumentFactory_out;


// Common definitions for interface DocumentFactory
class DocumentFactory : 
  virtual public ::KOM::ComponentFactory
{
  public:
    virtual ~DocumentFactory();

    typedef DocumentFactory_ptr _ptr_type;
    typedef DocumentFactory_var _var_type;

    static DocumentFactory_ptr _duplicate( DocumentFactory_ptr obj );
    static DocumentFactory_ptr _narrow( CORBA::Object_ptr obj );
    static DocumentFactory_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static DocumentFactory_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual Document_ptr create() = 0;

  protected:
    DocumentFactory() {};
  private:
    DocumentFactory( const DocumentFactory& );
    void operator=( const DocumentFactory& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_DocumentFactory;

// Stub for interface DocumentFactory
class DocumentFactory_stub:
  virtual public DocumentFactory,
  virtual public ::KOM::ComponentFactory_stub
{
  public:
    virtual ~DocumentFactory_stub();
    Document_ptr create();

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

class Frame;
typedef Frame *Frame_ptr;
typedef Frame_ptr FrameRef;
typedef ObjVar<Frame> Frame_var;
typedef Frame_var Frame_out;


// Common definitions for interface Frame
class Frame : 
  virtual public ::KOM::Base
{
  public:
    virtual ~Frame();

    typedef Frame_ptr _ptr_type;
    typedef Frame_var _var_type;

    static Frame_ptr _duplicate( Frame_ptr obj );
    static Frame_ptr _narrow( CORBA::Object_ptr obj );
    static Frame_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Frame_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual View_ptr view() = 0;
    virtual void viewChangedState( CORBA::Boolean is_marked, CORBA::Boolean has_focus ) = 0;

  protected:
    Frame() {};
  private:
    Frame( const Frame& );
    void operator=( const Frame& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_Frame;

// Stub for interface Frame
class Frame_stub:
  virtual public Frame,
  virtual public ::KOM::Base_stub
{
  public:
    virtual ~Frame_stub();
    View_ptr view();
    void viewChangedState( CORBA::Boolean is_marked, CORBA::Boolean has_focus );

  private:
    void operator=( const Frame_stub& );
};

class Frame_skel :
  virtual public StaticMethodDispatcher,
  virtual public Frame
{
  public:
    Frame_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Frame_skel();
    Frame_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    Frame_ptr _this();

};


// Common definitions for interface MainWindow
class MainWindow : 
  virtual public ::OpenParts::MainWindow
{
  public:
    virtual ~MainWindow();

    typedef MainWindow_ptr _ptr_type;
    typedef MainWindow_var _var_type;

    static MainWindow_ptr _duplicate( MainWindow_ptr obj );
    static MainWindow_ptr _narrow( CORBA::Object_ptr obj );
    static MainWindow_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static MainWindow_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void setMarkedPart( OpenParts::Id id ) = 0;
    virtual Document_ptr document() = 0;
    virtual View_ptr view() = 0;

  protected:
    MainWindow() {};
  private:
    MainWindow( const MainWindow& );
    void operator=( const MainWindow& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_MainWindow;

// Stub for interface MainWindow
class MainWindow_stub:
  virtual public MainWindow,
  virtual public ::OpenParts::MainWindow_stub
{
  public:
    virtual ~MainWindow_stub();
    void setMarkedPart( OpenParts::Id id );
    Document_ptr document();
    View_ptr view();

  private:
    void operator=( const MainWindow_stub& );
};

class MainWindow_skel :
  virtual public StaticMethodDispatcher,
  virtual public MainWindow
{
  public:
    MainWindow_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~MainWindow_skel();
    MainWindow_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    MainWindow_ptr _this();

};

class Filter;
typedef Filter *Filter_ptr;
typedef Filter_ptr FilterRef;
typedef ObjVar<Filter> Filter_var;
typedef Filter_var Filter_out;


// Common definitions for interface Filter
class Filter : 
  virtual public ::KOM::Component
{
  public:
    virtual ~Filter();

    typedef Filter_ptr _ptr_type;
    typedef Filter_var _var_type;

    static Filter_ptr _duplicate( Filter_ptr obj );
    static Filter_ptr _narrow( CORBA::Object_ptr obj );
    static Filter_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Filter_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    typedef SequenceTmpl<CORBA::Octet,MICO_TID_OCTET> Data;
    #ifdef _WINDOWS
    static Data _dummy_Data;
    #endif
    typedef TSeqVar<SequenceTmpl<CORBA::Octet,MICO_TID_OCTET> > Data_var;
    typedef Data_var Data_out;

    static CORBA::TypeCodeConst _tc_Data;

    struct FormatError : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      FormatError();
      ~FormatError();
      FormatError( const FormatError& s );
      FormatError& operator=( const FormatError& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS

      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static FormatError *_downcast( CORBA::Exception *ex );
    };

    typedef ExceptVar<FormatError> FormatError_var;
    typedef FormatError_var FormatError_out;

    static CORBA::TypeCodeConst _tc_FormatError;

    struct UnsupportedFormat : public CORBA::UserException {
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      UnsupportedFormat();
      ~UnsupportedFormat();
      UnsupportedFormat( const UnsupportedFormat& s );
      UnsupportedFormat& operator=( const UnsupportedFormat& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS

      #ifndef HAVE_EXPLICIT_STRUCT_OPS
      UnsupportedFormat();
      #endif //HAVE_EXPLICIT_STRUCT_OPS
      UnsupportedFormat( const char* _m0 );
      void _throwit() const;
      const char *_repoid() const;
      void _encode( CORBA::DataEncoder &en ) const;
      CORBA::Exception *_clone() const;
      static UnsupportedFormat *_downcast( CORBA::Exception *ex );
      CORBA::String_var format;
    };

    typedef ExceptVar<UnsupportedFormat> UnsupportedFormat_var;
    typedef UnsupportedFormat_var UnsupportedFormat_out;

    static CORBA::TypeCodeConst _tc_UnsupportedFormat;

    virtual void filter( Data& data, const char* from, const char* to ) = 0;

  protected:
    Filter() {};
  private:
    Filter( const Filter& );
    void operator=( const Filter& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_Filter;

// Stub for interface Filter
class Filter_stub:
  virtual public Filter,
  virtual public ::KOM::Component_stub
{
  public:
    virtual ~Filter_stub();
    void filter( Data& data, const char* from, const char* to );

  private:
    void operator=( const Filter_stub& );
};

class Filter_skel :
  virtual public StaticMethodDispatcher,
  virtual public Filter
{
  public:
    Filter_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Filter_skel();
    Filter_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    Filter_ptr _this();

};

class FilterFactory;
typedef FilterFactory *FilterFactory_ptr;
typedef FilterFactory_ptr FilterFactoryRef;
typedef ObjVar<FilterFactory> FilterFactory_var;
typedef FilterFactory_var FilterFactory_out;


// Common definitions for interface FilterFactory
class FilterFactory : 
  virtual public ::KOM::ComponentFactory
{
  public:
    virtual ~FilterFactory();

    typedef FilterFactory_ptr _ptr_type;
    typedef FilterFactory_var _var_type;

    static FilterFactory_ptr _duplicate( FilterFactory_ptr obj );
    static FilterFactory_ptr _narrow( CORBA::Object_ptr obj );
    static FilterFactory_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static FilterFactory_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual Filter_ptr create() = 0;

  protected:
    FilterFactory() {};
  private:
    FilterFactory( const FilterFactory& );
    void operator=( const FilterFactory& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_FilterFactory;

// Stub for interface FilterFactory
class FilterFactory_stub:
  virtual public FilterFactory,
  virtual public ::KOM::ComponentFactory_stub
{
  public:
    virtual ~FilterFactory_stub();
    Filter_ptr create();

  private:
    void operator=( const FilterFactory_stub& );
};

class FilterFactory_skel :
  virtual public StaticMethodDispatcher,
  virtual public FilterFactory
{
  public:
    FilterFactory_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~FilterFactory_skel();
    FilterFactory_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    FilterFactory_ptr _this();

};

class Callback;
typedef Callback *Callback_ptr;
typedef Callback_ptr CallbackRef;
typedef ObjVar<Callback> Callback_var;
typedef Callback_var Callback_out;


// Common definitions for interface Callback
class Callback : 
  virtual public ::KOM::Base
{
  public:
    virtual ~Callback();

    typedef Callback_ptr _ptr_type;
    typedef Callback_var _var_type;

    static Callback_ptr _duplicate( Callback_ptr obj );
    static Callback_ptr _narrow( CORBA::Object_ptr obj );
    static Callback_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Callback_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void callback() = 0;

  protected:
    Callback() {};
  private:
    Callback( const Callback& );
    void operator=( const Callback& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_Callback;

// Stub for interface Callback
class Callback_stub:
  virtual public Callback,
  virtual public ::KOM::Base_stub
{
  public:
    virtual ~Callback_stub();
    void callback();

  private:
    void operator=( const Callback_stub& );
};

class Callback_skel :
  virtual public StaticMethodDispatcher,
  virtual public Callback
{
  public:
    Callback_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Callback_skel();
    Callback_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    Callback_ptr _this();

};

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KOffice)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif



#ifndef MICO_CONF_NO_POA

#endif // MICO_CONF_NO_POA

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::Document_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::Document_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::Document_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_Document;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::Print_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::Print_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::Print_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_Print;

CORBA::Boolean operator<<=( CORBA::Any &a, const ::KOffice::View::Mode &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, ::KOffice::View::Mode &e );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_View_Mode;

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::KOffice::View::EventNewPart &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::KOffice::View::EventNewPart &_s );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_View_EventNewPart;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::View_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::View_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::View_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_View;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::DocumentFactory_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::DocumentFactory_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::DocumentFactory_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_DocumentFactory;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::Frame_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::Frame_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::Frame_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_Frame;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::MainWindow_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::MainWindow_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::MainWindow_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_MainWindow;

CORBA::Boolean operator<<=( CORBA::Any &a, const ::KOffice::Filter::FormatError &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::KOffice::Filter::FormatError &e );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_Filter_FormatError;

CORBA::Boolean operator<<=( CORBA::Any &a, const ::KOffice::Filter::UnsupportedFormat &e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::KOffice::Filter::UnsupportedFormat &e );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_Filter_UnsupportedFormat;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::Filter_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::Filter_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::Filter_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_Filter;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::FilterFactory_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::FilterFactory_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::FilterFactory_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_FilterFactory;

CORBA::Boolean operator<<=( CORBA::Any &a, const KOffice::Callback_ptr obj );
CORBA::Boolean operator<<=( CORBA::Any &a, KOffice::Callback_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KOffice::Callback_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KOffice_Callback;

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
