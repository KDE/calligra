/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include "KIllustrator.h"

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface Shell
KIllustrator::Shell::~Shell()
{
}

KIllustrator::Shell_ptr KIllustrator::Shell::_duplicate( Shell_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KIllustrator::Shell::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KIllustrator/Shell:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::PartShell::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KIllustrator::Shell::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KIllustrator/Shell:1.0" ) == 0) {
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

KIllustrator::Shell_ptr KIllustrator::Shell::_narrow( CORBA::Object_ptr _obj )
{
  KIllustrator::Shell_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KIllustrator/Shell:1.0" ))
      return _duplicate( (KIllustrator::Shell_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KIllustrator::Shell_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KIllustrator/Shell:1.0" ) ) {
    _o = new KIllustrator::Shell_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KIllustrator::Shell_ptr KIllustrator::Shell::_nil()
{
  return NULL;
}

KIllustrator::Shell_stub::~Shell_stub()
{
}

void KIllustrator::Shell_stub::fileNew()
{
  CORBA::Request_var _req = this->_request( "fileNew" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::fileOpen()
{
  CORBA::Request_var _req = this->_request( "fileOpen" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::fileClose()
{
  CORBA::Request_var _req = this->_request( "fileClose" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::fileQuit()
{
  CORBA::Request_var _req = this->_request( "fileQuit" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::fileSave()
{
  CORBA::Request_var _req = this->_request( "fileSave" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::fileSaveAs()
{
  CORBA::Request_var _req = this->_request( "fileSaveAs" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::filePrint()
{
  CORBA::Request_var _req = this->_request( "filePrint" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::editCut()
{
  CORBA::Request_var _req = this->_request( "editCut" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::editCopy()
{
  CORBA::Request_var _req = this->_request( "editCopy" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::editPaste()
{
  CORBA::Request_var _req = this->_request( "editPaste" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::setZoomFactor( const char* size )
{
  CORBA::Request_var _req = this->_request( "setZoomFactor" );
  _req->add_in_arg( "size" ) <<= CORBA::Any::from_string( (char *) size, 0 );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::Shell_stub::helpAbout()
{
  CORBA::Request_var _req = this->_request( "helpAbout" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


struct _global_init_KIllustrator_Shell {
  _global_init_KIllustrator_Shell()
  {
    if( ::OPParts::PartShell::_narrow_helpers == NULL )
      ::OPParts::PartShell::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::PartShell::_narrow_helpers->push_back( KIllustrator::Shell::_narrow_helper2 );
  }
} __global_init_KIllustrator_Shell;

vector<CORBA::Narrow_proto> *KIllustrator::Shell::_narrow_helpers;

CORBA::TypeCode_ptr KIllustrator::_tc_Shell = (new CORBA::TypeCode(
  "010000000e0000002e000000010000001b00000049444c3a4b496c6c7573"
  "747261746f722f5368656c6c3a312e300000060000005368656c6c00" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KIllustrator::Shell_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Shell" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KIllustrator::Shell_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KIllustrator::Shell::_nil();
    return TRUE;
  }
  _obj = ::KIllustrator::Shell::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface View
KIllustrator::View::~View()
{
}

KIllustrator::View_ptr KIllustrator::View::_duplicate( View_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KIllustrator::View::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KIllustrator/View:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::View::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KIllustrator::View::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KIllustrator/View:1.0" ) == 0) {
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

KIllustrator::View_ptr KIllustrator::View::_narrow( CORBA::Object_ptr _obj )
{
  KIllustrator::View_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KIllustrator/View:1.0" ))
      return _duplicate( (KIllustrator::View_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KIllustrator::View_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KIllustrator/View:1.0" ) ) {
    _o = new KIllustrator::View_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KIllustrator::View_ptr KIllustrator::View::_nil()
{
  return NULL;
}

KIllustrator::View_stub::~View_stub()
{
}

void KIllustrator::View_stub::editUndo()
{
  CORBA::Request_var _req = this->_request( "editUndo" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editRedo()
{
  CORBA::Request_var _req = this->_request( "editRedo" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editCut()
{
  CORBA::Request_var _req = this->_request( "editCut" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editCopy()
{
  CORBA::Request_var _req = this->_request( "editCopy" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editPaste()
{
  CORBA::Request_var _req = this->_request( "editPaste" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editSelectAll()
{
  CORBA::Request_var _req = this->_request( "editSelectAll" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editDelete()
{
  CORBA::Request_var _req = this->_request( "editDelete" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editInsertOject()
{
  CORBA::Request_var _req = this->_request( "editInsertOject" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::editProperties()
{
  CORBA::Request_var _req = this->_request( "editProperties" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::transformPosition()
{
  CORBA::Request_var _req = this->_request( "transformPosition" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::transformDimension()
{
  CORBA::Request_var _req = this->_request( "transformDimension" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::transformRotation()
{
  CORBA::Request_var _req = this->_request( "transformRotation" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::transformMirror()
{
  CORBA::Request_var _req = this->_request( "transformMirror" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::arrangeAlign()
{
  CORBA::Request_var _req = this->_request( "arrangeAlign" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::arrangeToFront()
{
  CORBA::Request_var _req = this->_request( "arrangeToFront" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::arrangeToBack()
{
  CORBA::Request_var _req = this->_request( "arrangeToBack" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::arrangeOneForward()
{
  CORBA::Request_var _req = this->_request( "arrangeOneForward" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::arrangeOneBack()
{
  CORBA::Request_var _req = this->_request( "arrangeOneBack" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::arrangeGroup()
{
  CORBA::Request_var _req = this->_request( "arrangeGroup" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::arrangeUngroup()
{
  CORBA::Request_var _req = this->_request( "arrangeUngroup" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::toggleRuler()
{
  CORBA::Request_var _req = this->_request( "toggleRuler" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::toggleGrid()
{
  CORBA::Request_var _req = this->_request( "toggleGrid" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::setupGrid()
{
  CORBA::Request_var _req = this->_request( "setupGrid" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::alignToGrid()
{
  CORBA::Request_var _req = this->_request( "alignToGrid" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateSelectionTool()
{
  CORBA::Request_var _req = this->_request( "activateSelectionTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateEditPointTool()
{
  CORBA::Request_var _req = this->_request( "activateEditPointTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateLineTool()
{
  CORBA::Request_var _req = this->_request( "activateLineTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateBezierTool()
{
  CORBA::Request_var _req = this->_request( "activateBezierTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateRectangleTool()
{
  CORBA::Request_var _req = this->_request( "activateRectangleTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activatePolygonTool()
{
  CORBA::Request_var _req = this->_request( "activatePolygonTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateEllipseTool()
{
  CORBA::Request_var _req = this->_request( "activateEllipseTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateTextTool()
{
  CORBA::Request_var _req = this->_request( "activateTextTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


void KIllustrator::View_stub::activateZoomTool()
{
  CORBA::Request_var _req = this->_request( "activateZoomTool" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->send_oneway();
  #ifdef HAVE_EXCEPTIONS
  if( CORBA::Exception *_ex = _req->env()->exception() )
    mico_throw( *_ex );
  #else
  if( CORBA::Exception *_ex = _req->env()->exception() )
    CORBA::Exception::_throw_failed( _ex );
  #endif
}


struct _global_init_KIllustrator_View {
  _global_init_KIllustrator_View()
  {
    if( ::OPParts::View::_narrow_helpers == NULL )
      ::OPParts::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::View::_narrow_helpers->push_back( KIllustrator::View::_narrow_helper2 );
  }
} __global_init_KIllustrator_View;

vector<CORBA::Narrow_proto> *KIllustrator::View::_narrow_helpers;

CORBA::TypeCode_ptr KIllustrator::_tc_View = (new CORBA::TypeCode(
  "010000000e0000002d000000010000001a00000049444c3a4b496c6c7573"
  "747261746f722f566965773a312e30000000050000005669657700" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KIllustrator::View_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "View" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KIllustrator::View_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KIllustrator::View::_nil();
    return TRUE;
  }
  _obj = ::KIllustrator::View::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Document
KIllustrator::Document::~Document()
{
}

KIllustrator::Document_ptr KIllustrator::Document::_duplicate( Document_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KIllustrator::Document::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KIllustrator/Document:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Document::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KIllustrator::Document::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KIllustrator/Document:1.0" ) == 0) {
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

KIllustrator::Document_ptr KIllustrator::Document::_narrow( CORBA::Object_ptr _obj )
{
  KIllustrator::Document_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KIllustrator/Document:1.0" ))
      return _duplicate( (KIllustrator::Document_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KIllustrator::Document_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KIllustrator/Document:1.0" ) ) {
    _o = new KIllustrator::Document_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KIllustrator::Document_ptr KIllustrator::Document::_nil()
{
  return NULL;
}

KIllustrator::Document_stub::~Document_stub()
{
}

struct _global_init_KIllustrator_Document {
  _global_init_KIllustrator_Document()
  {
    if( ::OPParts::Document::_narrow_helpers == NULL )
      ::OPParts::Document::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Document::_narrow_helpers->push_back( KIllustrator::Document::_narrow_helper2 );
  }
} __global_init_KIllustrator_Document;

vector<CORBA::Narrow_proto> *KIllustrator::Document::_narrow_helpers;

CORBA::TypeCode_ptr KIllustrator::_tc_Document = (new CORBA::TypeCode(
  "010000000e00000035000000010000001e00000049444c3a4b496c6c7573"
  "747261746f722f446f63756d656e743a312e3000000009000000446f6375"
  "6d656e7400" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KIllustrator::Document_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Document" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KIllustrator::Document_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KIllustrator::Document::_nil();
    return TRUE;
  }
  _obj = ::KIllustrator::Document::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Factory
KIllustrator::Factory::~Factory()
{
}

KIllustrator::Factory_ptr KIllustrator::Factory::_duplicate( Factory_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KIllustrator::Factory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KIllustrator/Factory:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Factory::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KIllustrator::Factory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KIllustrator/Factory:1.0" ) == 0) {
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

KIllustrator::Factory_ptr KIllustrator::Factory::_narrow( CORBA::Object_ptr _obj )
{
  KIllustrator::Factory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KIllustrator/Factory:1.0" ))
      return _duplicate( (KIllustrator::Factory_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KIllustrator::Factory_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KIllustrator/Factory:1.0" ) ) {
    _o = new KIllustrator::Factory_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KIllustrator::Factory_ptr KIllustrator::Factory::_nil()
{
  return NULL;
}

KIllustrator::Factory_stub::~Factory_stub()
{
}

struct _global_init_KIllustrator_Factory {
  _global_init_KIllustrator_Factory()
  {
    if( ::OPParts::Factory::_narrow_helpers == NULL )
      ::OPParts::Factory::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Factory::_narrow_helpers->push_back( KIllustrator::Factory::_narrow_helper2 );
  }
} __global_init_KIllustrator_Factory;

vector<CORBA::Narrow_proto> *KIllustrator::Factory::_narrow_helpers;

CORBA::TypeCode_ptr KIllustrator::_tc_Factory = (new CORBA::TypeCode(
  "010000000e00000034000000010000001d00000049444c3a4b496c6c7573"
  "747261746f722f466163746f72793a312e30000000000800000046616374"
  "6f727900" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KIllustrator::Factory_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Factory" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KIllustrator::Factory_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KIllustrator::Factory::_nil();
    return TRUE;
  }
  _obj = ::KIllustrator::Factory::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface Shell
KIllustrator::Shell_skel::Shell_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Shell:1.0", "Shell" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KIllustrator/Shell:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Shell_skel>( this ) );
}

KIllustrator::Shell_skel::Shell_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Shell:1.0", "Shell" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Shell_skel>( this ) );
}

KIllustrator::Shell_skel::~Shell_skel()
{
}

bool KIllustrator::Shell_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "fileNew" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    fileNew();
    return true;
  }
  if( strcmp( _req->op_name(), "fileOpen" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    fileOpen();
    return true;
  }
  if( strcmp( _req->op_name(), "fileClose" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    fileClose();
    return true;
  }
  if( strcmp( _req->op_name(), "fileQuit" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    fileQuit();
    return true;
  }
  if( strcmp( _req->op_name(), "fileSave" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    fileSave();
    return true;
  }
  if( strcmp( _req->op_name(), "fileSaveAs" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    fileSaveAs();
    return true;
  }
  if( strcmp( _req->op_name(), "filePrint" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    filePrint();
    return true;
  }
  if( strcmp( _req->op_name(), "editCut" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editCut();
    return true;
  }
  if( strcmp( _req->op_name(), "editCopy" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editCopy();
    return true;
  }
  if( strcmp( _req->op_name(), "editPaste" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editPaste();
    return true;
  }
  if( strcmp( _req->op_name(), "setZoomFactor" ) == 0 ) {
    CORBA::String_var size;

    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 1, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_string );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= CORBA::Any::to_string( size, 0 );
    setZoomFactor( size );
    return true;
  }
  if( strcmp( _req->op_name(), "helpAbout" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    helpAbout();
    return true;
  }
  return false;
}

KIllustrator::Shell_ptr KIllustrator::Shell_skel::_this()
{
  return KIllustrator::Shell::_duplicate( this );
}


// Dynamic Implementation Routine for interface View
KIllustrator::View_skel::View_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KIllustrator/View:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<View_skel>( this ) );
}

KIllustrator::View_skel::View_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<View_skel>( this ) );
}

KIllustrator::View_skel::~View_skel()
{
}

bool KIllustrator::View_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  if( strcmp( _req->op_name(), "editUndo" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editUndo();
    return true;
  }
  if( strcmp( _req->op_name(), "editRedo" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editRedo();
    return true;
  }
  if( strcmp( _req->op_name(), "editCut" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editCut();
    return true;
  }
  if( strcmp( _req->op_name(), "editCopy" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editCopy();
    return true;
  }
  if( strcmp( _req->op_name(), "editPaste" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editPaste();
    return true;
  }
  if( strcmp( _req->op_name(), "editSelectAll" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editSelectAll();
    return true;
  }
  if( strcmp( _req->op_name(), "editDelete" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editDelete();
    return true;
  }
  if( strcmp( _req->op_name(), "editInsertOject" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editInsertOject();
    return true;
  }
  if( strcmp( _req->op_name(), "editProperties" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editProperties();
    return true;
  }
  if( strcmp( _req->op_name(), "transformPosition" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    transformPosition();
    return true;
  }
  if( strcmp( _req->op_name(), "transformDimension" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    transformDimension();
    return true;
  }
  if( strcmp( _req->op_name(), "transformRotation" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    transformRotation();
    return true;
  }
  if( strcmp( _req->op_name(), "transformMirror" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    transformMirror();
    return true;
  }
  if( strcmp( _req->op_name(), "arrangeAlign" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    arrangeAlign();
    return true;
  }
  if( strcmp( _req->op_name(), "arrangeToFront" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    arrangeToFront();
    return true;
  }
  if( strcmp( _req->op_name(), "arrangeToBack" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    arrangeToBack();
    return true;
  }
  if( strcmp( _req->op_name(), "arrangeOneForward" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    arrangeOneForward();
    return true;
  }
  if( strcmp( _req->op_name(), "arrangeOneBack" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    arrangeOneBack();
    return true;
  }
  if( strcmp( _req->op_name(), "arrangeGroup" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    arrangeGroup();
    return true;
  }
  if( strcmp( _req->op_name(), "arrangeUngroup" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    arrangeUngroup();
    return true;
  }
  if( strcmp( _req->op_name(), "toggleRuler" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    toggleRuler();
    return true;
  }
  if( strcmp( _req->op_name(), "toggleGrid" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    toggleGrid();
    return true;
  }
  if( strcmp( _req->op_name(), "setupGrid" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    setupGrid();
    return true;
  }
  if( strcmp( _req->op_name(), "alignToGrid" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    alignToGrid();
    return true;
  }
  if( strcmp( _req->op_name(), "activateSelectionTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateSelectionTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activateEditPointTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateEditPointTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activateLineTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateLineTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activateBezierTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateBezierTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activateRectangleTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateRectangleTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activatePolygonTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activatePolygonTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activateEllipseTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateEllipseTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activateTextTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateTextTool();
    return true;
  }
  if( strcmp( _req->op_name(), "activateZoomTool" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    activateZoomTool();
    return true;
  }
  return false;
}

KIllustrator::View_ptr KIllustrator::View_skel::_this()
{
  return KIllustrator::View::_duplicate( this );
}


// Dynamic Implementation Routine for interface Document
KIllustrator::Document_skel::Document_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KIllustrator/Document:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Document_skel>( this ) );
}

KIllustrator::Document_skel::Document_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Document:1.0", "Document" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Document_skel>( this ) );
}

KIllustrator::Document_skel::~Document_skel()
{
}

bool KIllustrator::Document_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KIllustrator::Document_ptr KIllustrator::Document_skel::_this()
{
  return KIllustrator::Document::_duplicate( this );
}


// Dynamic Implementation Routine for interface Factory
KIllustrator::Factory_skel::Factory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KIllustrator/Factory:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KIllustrator::Factory_skel::Factory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KIllustrator/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KIllustrator::Factory_skel::~Factory_skel()
{
}

bool KIllustrator::Factory_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KIllustrator::Factory_ptr KIllustrator::Factory_skel::_this()
{
  return KIllustrator::Factory::_duplicate( this );
}

