/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__KSPREAD_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __KSPREAD_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

// Generate forward declarations for this scope

#include "/opt/kde/idl/kom.h"

#include "/opt/kde/idl/controls.h"

#include "/opt/kde/idl/parts.h"

// Module KSpread
#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL KSpread {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KSpread)

// Generate forward declarations for this scope
class View;
typedef View *View_ptr;
typedef View_ptr ViewRef;
typedef ObjVar<View> View_var;

class Document;
typedef Document *Document_ptr;
typedef Document_ptr DocumentRef;
typedef ObjVar<Document> Document_var;



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

    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void cutSelection() = 0;
    virtual void copySelection() = 0;
    virtual void paste() = 0;
    virtual void editCell() = 0;
    virtual void paperLayoutDlg() = 0;
    virtual void togglePageBorders() = 0;
    virtual void newView() = 0;
    virtual void insertNewTable() = 0;
    virtual void autoFill() = 0;
    virtual void editGlobalScripts() = 0;
    virtual void editLocalScripts() = 0;
    virtual void reloadScripts() = 0;
    virtual void helpAbout() = 0;
    virtual void helpUsing() = 0;
    virtual void deleteRow() = 0;
    virtual void deleteColumn() = 0;
    virtual void insertRow() = 0;
    virtual void insertColumn() = 0;
    virtual void bold() = 0;
    virtual void italic() = 0;
    virtual void moneyFormat() = 0;
    virtual void percent() = 0;
    virtual void alignLeft() = 0;
    virtual void alignCenter() = 0;
    virtual void alignRight() = 0;
    virtual void multiRow() = 0;
    virtual void precisionMinus() = 0;
    virtual void precisionPlus() = 0;
    virtual void insertChart() = 0;
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
    void undo();
    void redo();
    void cutSelection();
    void copySelection();
    void paste();
    void editCell();
    void paperLayoutDlg();
    void togglePageBorders();
    void newView();
    void insertNewTable();
    void autoFill();
    void editGlobalScripts();
    void editLocalScripts();
    void reloadScripts();
    void helpAbout();
    void helpUsing();
    void deleteRow();
    void deleteColumn();
    void insertRow();
    void insertColumn();
    void bold();
    void italic();
    void moneyFormat();
    void percent();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void multiRow();
    void precisionMinus();
    void precisionPlus();
    void insertChart();
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

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KSpread)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const KSpread::View_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KSpread::View_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const KSpread::Document_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KSpread::Document_ptr &obj );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#ifndef MICO_NO_TOPLEVEL_MODULES
#include <mico/template_impl.h>
#endif

#endif
