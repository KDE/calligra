/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__IMG_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __IMG_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

// Generate forward declarations for this scope

#include "/opt/kde/idl/kom.h"

#include "/opt/kde/idl/controls.h"

#include "/opt/kde/idl/parts.h"

// Module KImage
#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL KImage {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KImage)

// Generate forward declarations for this scope
class ImageView;
typedef ImageView *ImageView_ptr;
typedef ImageView_ptr ImageViewRef;
typedef ObjVar<ImageView> ImageView_var;

class ImageDocument;
typedef ImageDocument *ImageDocument_ptr;
typedef ImageDocument_ptr ImageDocumentRef;
typedef ObjVar<ImageDocument> ImageDocument_var;

class Factory;
typedef Factory *Factory_ptr;
typedef Factory_ptr FactoryRef;
typedef ObjVar<Factory> Factory_var;



// Common definitions for interface ImageView
class ImageView : 
  virtual public ::OPParts::View
{
  public:
    virtual ~ImageView();
    static ImageView_ptr _duplicate( ImageView_ptr obj );
    static ImageView_ptr _narrow( CORBA::Object_ptr obj );
    static ImageView_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void toggleFitToWindow() = 0;
    virtual void newView() = 0;
    virtual void insertImage() = 0;
    virtual void exportImage() = 0;
    virtual void importImage() = 0;
  protected:
    ImageView() {};
  private:
    ImageView( const ImageView& );
    void operator=( const ImageView& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_ImageView;

// Stub for interface ImageView
class ImageView_stub : virtual public ImageView,
  virtual public ::OPParts::View_stub
{
  public:
    virtual ~ImageView_stub();
    void toggleFitToWindow();
    void newView();
    void insertImage();
    void exportImage();
    void importImage();
  private:
    void operator=( const ImageView_stub& );
};

class ImageView_skel :
  virtual public MethodDispatcher,
  virtual public ImageView
{
  public:
    ImageView_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~ImageView_skel();
    ImageView_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    ImageView_ptr _this();

};


// Common definitions for interface ImageDocument
class ImageDocument : 
  virtual public ::OPParts::Document
{
  public:
    virtual ~ImageDocument();
    static ImageDocument_ptr _duplicate( ImageDocument_ptr obj );
    static ImageDocument_ptr _narrow( CORBA::Object_ptr obj );
    static ImageDocument_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual CORBA::Boolean import( const char* filename ) = 0;
    virtual CORBA::Boolean export( const char* filename, const char* format ) = 0;
  protected:
    ImageDocument() {};
  private:
    ImageDocument( const ImageDocument& );
    void operator=( const ImageDocument& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_ImageDocument;

// Stub for interface ImageDocument
class ImageDocument_stub : virtual public ImageDocument,
  virtual public ::OPParts::Document_stub
{
  public:
    virtual ~ImageDocument_stub();
    CORBA::Boolean import( const char* filename );
    CORBA::Boolean export( const char* filename, const char* format );
  private:
    void operator=( const ImageDocument_stub& );
};

class ImageDocument_skel :
  virtual public MethodDispatcher,
  virtual public ImageDocument
{
  public:
    ImageDocument_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~ImageDocument_skel();
    ImageDocument_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    ImageDocument_ptr _this();

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

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KImage)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const KImage::ImageView_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KImage::ImageView_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const KImage::ImageDocument_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KImage::ImageDocument_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const KImage::Factory_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, KImage::Factory_ptr &obj );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#ifndef MICO_NO_TOPLEVEL_MODULES
#include <mico/template_impl.h>
#endif

#endif
