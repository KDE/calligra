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
class Shell;
typedef Shell *Shell_ptr;
typedef Shell_ptr ShellRef;
typedef ObjVar<Shell> Shell_var;

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



// Common definitions for interface Shell
class Shell : 
  virtual public ::OPParts::PartShell
{
  public:
    virtual ~Shell();
    static Shell_ptr _duplicate( Shell_ptr obj );
    static Shell_ptr _narrow( CORBA::Object_ptr obj );
    static Shell_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void fileNew() = 0;
    virtual void fileOpen() = 0;
    virtual void fileClose() = 0;
    virtual void fileQuit() = 0;
    virtual void fileSave() = 0;
    virtual void fileSaveAs() = 0;
    virtual void filePrint() = 0;
    virtual void editCut() = 0;
    virtual void editCopy() = 0;
    virtual void editPaste() = 0;
    virtual void setZoomFactor( const char* size ) = 0;
    virtual void helpAbout() = 0;
  protected:
    Shell() {};
  private:
    Shell( const Shell& );
    void operator=( const Shell& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_Shell;

// Stub for interface Shell
class Shell_stub : virtual public Shell,
  virtual public ::OPParts::PartShell_stub
{
  public:
    virtual ~Shell_stub();
    void fileNew();
    void fileOpen();
    void fileClose();
    void fileQuit();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void editCut();
    void editCopy();
    void editPaste();
    void setZoomFactor( const char* size );
    void helpAbout();
  private:
    void operator=( const Shell_stub& );
};

class Shell_skel :
  virtual public MethodDispatcher,
  virtual public Shell
{
  public:
    Shell_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Shell_skel();
    Shell_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Shell_ptr _this();

};


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

    virtual void editUndo() = 0;
    virtual void editRedo() = 0;
    virtual void editCut() = 0;
    virtual void editCopy() = 0;
    virtual void editPaste() = 0;
    virtual void editSelectAll() = 0;
    virtual void editDelete() = 0;
    virtual void editInsertOject() = 0;
    virtual void editProperties() = 0;
    virtual void transformPosition() = 0;
    virtual void transformDimension() = 0;
    virtual void transformRotation() = 0;
    virtual void transformMirror() = 0;
    virtual void arrangeAlign() = 0;
    virtual void arrangeToFront() = 0;
    virtual void arrangeToBack() = 0;
    virtual void arrangeOneForward() = 0;
    virtual void arrangeOneBack() = 0;
    virtual void arrangeGroup() = 0;
    virtual void arrangeUngroup() = 0;
    virtual void toggleRuler() = 0;
    virtual void toggleGrid() = 0;
    virtual void setupGrid() = 0;
    virtual void alignToGrid() = 0;
    virtual void activateSelectionTool() = 0;
    virtual void activateEditPointTool() = 0;
    virtual void activateLineTool() = 0;
    virtual void activateBezierTool() = 0;
    virtual void activateRectangleTool() = 0;
    virtual void activatePolygonTool() = 0;
    virtual void activateEllipseTool() = 0;
    virtual void activateTextTool() = 0;
    virtual void activateZoomTool() = 0;
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
    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editSelectAll();
    void editDelete();
    void editInsertOject();
    void editProperties();
    void transformPosition();
    void transformDimension();
    void transformRotation();
    void transformMirror();
    void arrangeAlign();
    void arrangeToFront();
    void arrangeToBack();
    void arrangeOneForward();
    void arrangeOneBack();
    void arrangeGroup();
    void arrangeUngroup();
    void toggleRuler();
    void toggleGrid();
    void setupGrid();
    void alignToGrid();
    void activateSelectionTool();
    void activateEditPointTool();
    void activateLineTool();
    void activateBezierTool();
    void activateRectangleTool();
    void activatePolygonTool();
    void activateEllipseTool();
    void activateTextTool();
    void activateZoomTool();
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

CORBA::Boolean operator<<=( CORBA::Any &a, const KIllustrator::Shell_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KIllustrator::Shell_ptr &obj );

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
