/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__KPRESENTER_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __KPRESENTER_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

// Generate forward declarations for this scope

#include "/usr/local/kde/include/idl/kom.h"

#include "/usr/local/kde/include/idl/controls.h"

#include "/usr/local/kde/include/idl/parts.h"

// Module KPresenter
#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL KPresenter {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KPresenter)

// Generate forward declarations for this scope
class KPresenterView;
typedef KPresenterView *KPresenterView_ptr;
typedef KPresenterView_ptr KPresenterViewRef;
typedef ObjVar<KPresenterView> KPresenterView_var;

class KPresenterDocument;
typedef KPresenterDocument *KPresenterDocument_ptr;
typedef KPresenterDocument_ptr KPresenterDocumentRef;
typedef ObjVar<KPresenterDocument> KPresenterDocument_var;

class Factory;
typedef Factory *Factory_ptr;
typedef Factory_ptr FactoryRef;
typedef ObjVar<Factory> Factory_var;



// Common definitions for interface KPresenterView
class KPresenterView : 
  virtual public ::OPParts::View
{
  public:
    virtual ~KPresenterView();
    static KPresenterView_ptr _duplicate( KPresenterView_ptr obj );
    static KPresenterView_ptr _narrow( CORBA::Object_ptr obj );
    static KPresenterView_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void editCut() = 0;
    virtual void editCopy() = 0;
    virtual void editPaste() = 0;
    virtual void editDelete() = 0;
    virtual void editSelectAll() = 0;
    virtual void newView() = 0;
    virtual void insertPage() = 0;
    virtual void insertPicture() = 0;
    virtual void insertClipart() = 0;
    virtual void insertLine() = 0;
    virtual void insertRectangle() = 0;
    virtual void insertCircleOrEllipse() = 0;
    virtual void insertText() = 0;
    virtual void insertAutoform() = 0;
    virtual void insertLineHidl() = 0;
    virtual void insertLineVidl() = 0;
    virtual void insertLineD1idl() = 0;
    virtual void insertLineD2idl() = 0;
    virtual void insertNormRectidl() = 0;
    virtual void insertRoundRectidl() = 0;
    virtual void extraPenBrush() = 0;
    virtual void extraRaise() = 0;
    virtual void extraLower() = 0;
    virtual void extraRotate() = 0;
    virtual void extraBackground() = 0;
    virtual void extraLayout() = 0;
    virtual void extraOptions() = 0;
    virtual void screenConfigPages() = 0;
    virtual void screenAssignEffect() = 0;
    virtual void screenStart() = 0;
    virtual void screenStop() = 0;
    virtual void screenPause() = 0;
    virtual void screenFirst() = 0;
    virtual void screenPrev() = 0;
    virtual void screenNext() = 0;
    virtual void screenLast() = 0;
    virtual void screenSkip() = 0;
    virtual void screenFullScreen() = 0;
    virtual void screenPen() = 0;
    virtual void helpContents() = 0;
    virtual void helpAbout() = 0;
    virtual void helpAboutKOffice() = 0;
    virtual void helpAboutKDE() = 0;
    virtual void sizeSelected( const char* size ) = 0;
    virtual void fontSelected( const char* font ) = 0;
    virtual void textBold() = 0;
    virtual void textItalic() = 0;
    virtual void textUnderline() = 0;
    virtual void textColor() = 0;
    virtual void textAlignLeft() = 0;
    virtual void textAlignCenter() = 0;
    virtual void textAlignRight() = 0;
    virtual void mtextAlignLeft() = 0;
    virtual void mtextAlignCenter() = 0;
    virtual void mtextAlignRight() = 0;
    virtual void mtextFont() = 0;
    virtual void textEnumList() = 0;
    virtual void textUnsortList() = 0;
    virtual void textNormalText() = 0;
  protected:
    KPresenterView() {};
  private:
    KPresenterView( const KPresenterView& );
    void operator=( const KPresenterView& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_KPresenterView;

// Stub for interface KPresenterView
class KPresenterView_stub : virtual public KPresenterView,
  virtual public ::OPParts::View_stub
{
  public:
    virtual ~KPresenterView_stub();
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void newView();
    void insertPage();
    void insertPicture();
    void insertClipart();
    void insertLine();
    void insertRectangle();
    void insertCircleOrEllipse();
    void insertText();
    void insertAutoform();
    void insertLineHidl();
    void insertLineVidl();
    void insertLineD1idl();
    void insertLineD2idl();
    void insertNormRectidl();
    void insertRoundRectidl();
    void extraPenBrush();
    void extraRaise();
    void extraLower();
    void extraRotate();
    void extraBackground();
    void extraLayout();
    void extraOptions();
    void screenConfigPages();
    void screenAssignEffect();
    void screenStart();
    void screenStop();
    void screenPause();
    void screenFirst();
    void screenPrev();
    void screenNext();
    void screenLast();
    void screenSkip();
    void screenFullScreen();
    void screenPen();
    void helpContents();
    void helpAbout();
    void helpAboutKOffice();
    void helpAboutKDE();
    void sizeSelected( const char* size );
    void fontSelected( const char* font );
    void textBold();
    void textItalic();
    void textUnderline();
    void textColor();
    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void mtextAlignLeft();
    void mtextAlignCenter();
    void mtextAlignRight();
    void mtextFont();
    void textEnumList();
    void textUnsortList();
    void textNormalText();
  private:
    void operator=( const KPresenterView_stub& );
};

class KPresenterView_skel :
  virtual public MethodDispatcher,
  virtual public KPresenterView
{
  public:
    KPresenterView_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~KPresenterView_skel();
    KPresenterView_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    KPresenterView_ptr _this();

};


// Common definitions for interface KPresenterDocument
class KPresenterDocument : 
  virtual public ::OPParts::Document
{
  public:
    virtual ~KPresenterDocument();
    static KPresenterDocument_ptr _duplicate( KPresenterDocument_ptr obj );
    static KPresenterDocument_ptr _narrow( CORBA::Object_ptr obj );
    static KPresenterDocument_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    KPresenterDocument() {};
  private:
    KPresenterDocument( const KPresenterDocument& );
    void operator=( const KPresenterDocument& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_KPresenterDocument;

// Stub for interface KPresenterDocument
class KPresenterDocument_stub : virtual public KPresenterDocument,
  virtual public ::OPParts::Document_stub
{
  public:
    virtual ~KPresenterDocument_stub();
  private:
    void operator=( const KPresenterDocument_stub& );
};

class KPresenterDocument_skel :
  virtual public MethodDispatcher,
  virtual public KPresenterDocument
{
  public:
    KPresenterDocument_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~KPresenterDocument_skel();
    KPresenterDocument_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    KPresenterDocument_ptr _this();

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

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KPresenter)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const KPresenter::KPresenterView_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KPresenter::KPresenterView_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const KPresenter::KPresenterDocument_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KPresenter::KPresenterDocument_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const KPresenter::Factory_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KPresenter::Factory_ptr &obj );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#ifndef MICO_NO_TOPLEVEL_MODULES
#include <mico/template_impl.h>
#endif

#endif
