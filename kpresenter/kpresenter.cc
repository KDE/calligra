/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include "kpresenter.h"

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface KPresenterView
KPresenter::KPresenterView::~KPresenterView()
{
}

KPresenter::KPresenterView_ptr KPresenter::KPresenterView::_duplicate( KPresenterView_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KPresenter::KPresenterView::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KPresenter/KPresenterView:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::View::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KPresenter::KPresenterView::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KPresenter/KPresenterView:1.0" ) == 0) {
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

KPresenter::KPresenterView_ptr KPresenter::KPresenterView::_narrow( CORBA::Object_ptr _obj )
{
  KPresenter::KPresenterView_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KPresenter/KPresenterView:1.0" ))
      return _duplicate( (KPresenter::KPresenterView_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KPresenter::KPresenterView_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KPresenter/KPresenterView:1.0" ) ) {
    _o = new KPresenter::KPresenterView_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KPresenter::KPresenterView_ptr KPresenter::KPresenterView::_nil()
{
  return NULL;
}

KPresenter::KPresenterView_stub::~KPresenterView_stub()
{
}

void KPresenter::KPresenterView_stub::editCut()
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


void KPresenter::KPresenterView_stub::editCopy()
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


void KPresenter::KPresenterView_stub::editPaste()
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


void KPresenter::KPresenterView_stub::editDelete()
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


void KPresenter::KPresenterView_stub::editSelectAll()
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


void KPresenter::KPresenterView_stub::newView()
{
  CORBA::Request_var _req = this->_request( "newView" );
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


void KPresenter::KPresenterView_stub::insertPage()
{
  CORBA::Request_var _req = this->_request( "insertPage" );
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


void KPresenter::KPresenterView_stub::insertPicture()
{
  CORBA::Request_var _req = this->_request( "insertPicture" );
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


void KPresenter::KPresenterView_stub::insertClipart()
{
  CORBA::Request_var _req = this->_request( "insertClipart" );
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


void KPresenter::KPresenterView_stub::insertLine()
{
  CORBA::Request_var _req = this->_request( "insertLine" );
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


void KPresenter::KPresenterView_stub::insertRectangle()
{
  CORBA::Request_var _req = this->_request( "insertRectangle" );
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


void KPresenter::KPresenterView_stub::insertCircleOrEllipse()
{
  CORBA::Request_var _req = this->_request( "insertCircleOrEllipse" );
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


void KPresenter::KPresenterView_stub::insertText()
{
  CORBA::Request_var _req = this->_request( "insertText" );
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


void KPresenter::KPresenterView_stub::insertAutoform()
{
  CORBA::Request_var _req = this->_request( "insertAutoform" );
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


void KPresenter::KPresenterView_stub::insertObject()
{
  CORBA::Request_var _req = this->_request( "insertObject" );
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


void KPresenter::KPresenterView_stub::insertLineHidl()
{
  CORBA::Request_var _req = this->_request( "insertLineHidl" );
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


void KPresenter::KPresenterView_stub::insertLineVidl()
{
  CORBA::Request_var _req = this->_request( "insertLineVidl" );
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


void KPresenter::KPresenterView_stub::insertLineD1idl()
{
  CORBA::Request_var _req = this->_request( "insertLineD1idl" );
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


void KPresenter::KPresenterView_stub::insertLineD2idl()
{
  CORBA::Request_var _req = this->_request( "insertLineD2idl" );
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


void KPresenter::KPresenterView_stub::insertNormRectidl()
{
  CORBA::Request_var _req = this->_request( "insertNormRectidl" );
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


void KPresenter::KPresenterView_stub::insertRoundRectidl()
{
  CORBA::Request_var _req = this->_request( "insertRoundRectidl" );
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


void KPresenter::KPresenterView_stub::extraPenBrush()
{
  CORBA::Request_var _req = this->_request( "extraPenBrush" );
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


void KPresenter::KPresenterView_stub::extraRaise()
{
  CORBA::Request_var _req = this->_request( "extraRaise" );
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


void KPresenter::KPresenterView_stub::extraLower()
{
  CORBA::Request_var _req = this->_request( "extraLower" );
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


void KPresenter::KPresenterView_stub::extraRotate()
{
  CORBA::Request_var _req = this->_request( "extraRotate" );
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


void KPresenter::KPresenterView_stub::extraBackground()
{
  CORBA::Request_var _req = this->_request( "extraBackground" );
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


void KPresenter::KPresenterView_stub::extraLayout()
{
  CORBA::Request_var _req = this->_request( "extraLayout" );
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


void KPresenter::KPresenterView_stub::extraOptions()
{
  CORBA::Request_var _req = this->_request( "extraOptions" );
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


void KPresenter::KPresenterView_stub::screenConfigPages()
{
  CORBA::Request_var _req = this->_request( "screenConfigPages" );
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


void KPresenter::KPresenterView_stub::screenAssignEffect()
{
  CORBA::Request_var _req = this->_request( "screenAssignEffect" );
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


void KPresenter::KPresenterView_stub::screenStart()
{
  CORBA::Request_var _req = this->_request( "screenStart" );
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


void KPresenter::KPresenterView_stub::screenStop()
{
  CORBA::Request_var _req = this->_request( "screenStop" );
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


void KPresenter::KPresenterView_stub::screenPause()
{
  CORBA::Request_var _req = this->_request( "screenPause" );
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


void KPresenter::KPresenterView_stub::screenFirst()
{
  CORBA::Request_var _req = this->_request( "screenFirst" );
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


void KPresenter::KPresenterView_stub::screenPrev()
{
  CORBA::Request_var _req = this->_request( "screenPrev" );
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


void KPresenter::KPresenterView_stub::screenNext()
{
  CORBA::Request_var _req = this->_request( "screenNext" );
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


void KPresenter::KPresenterView_stub::screenLast()
{
  CORBA::Request_var _req = this->_request( "screenLast" );
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


void KPresenter::KPresenterView_stub::screenSkip()
{
  CORBA::Request_var _req = this->_request( "screenSkip" );
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


void KPresenter::KPresenterView_stub::screenFullScreen()
{
  CORBA::Request_var _req = this->_request( "screenFullScreen" );
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


void KPresenter::KPresenterView_stub::screenPen()
{
  CORBA::Request_var _req = this->_request( "screenPen" );
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


void KPresenter::KPresenterView_stub::helpContents()
{
  CORBA::Request_var _req = this->_request( "helpContents" );
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


void KPresenter::KPresenterView_stub::helpAbout()
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


void KPresenter::KPresenterView_stub::helpAboutKOffice()
{
  CORBA::Request_var _req = this->_request( "helpAboutKOffice" );
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


void KPresenter::KPresenterView_stub::helpAboutKDE()
{
  CORBA::Request_var _req = this->_request( "helpAboutKDE" );
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


void KPresenter::KPresenterView_stub::sizeSelected( const char* size )
{
  CORBA::Request_var _req = this->_request( "sizeSelected" );
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


void KPresenter::KPresenterView_stub::fontSelected( const char* font )
{
  CORBA::Request_var _req = this->_request( "fontSelected" );
  _req->add_in_arg( "font" ) <<= CORBA::Any::from_string( (char *) font, 0 );
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


void KPresenter::KPresenterView_stub::textBold()
{
  CORBA::Request_var _req = this->_request( "textBold" );
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


void KPresenter::KPresenterView_stub::textItalic()
{
  CORBA::Request_var _req = this->_request( "textItalic" );
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


void KPresenter::KPresenterView_stub::textUnderline()
{
  CORBA::Request_var _req = this->_request( "textUnderline" );
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


void KPresenter::KPresenterView_stub::textColor()
{
  CORBA::Request_var _req = this->_request( "textColor" );
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


void KPresenter::KPresenterView_stub::textAlignLeft()
{
  CORBA::Request_var _req = this->_request( "textAlignLeft" );
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


void KPresenter::KPresenterView_stub::textAlignCenter()
{
  CORBA::Request_var _req = this->_request( "textAlignCenter" );
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


void KPresenter::KPresenterView_stub::textAlignRight()
{
  CORBA::Request_var _req = this->_request( "textAlignRight" );
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


void KPresenter::KPresenterView_stub::mtextAlignLeft()
{
  CORBA::Request_var _req = this->_request( "mtextAlignLeft" );
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


void KPresenter::KPresenterView_stub::mtextAlignCenter()
{
  CORBA::Request_var _req = this->_request( "mtextAlignCenter" );
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


void KPresenter::KPresenterView_stub::mtextAlignRight()
{
  CORBA::Request_var _req = this->_request( "mtextAlignRight" );
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


void KPresenter::KPresenterView_stub::mtextFont()
{
  CORBA::Request_var _req = this->_request( "mtextFont" );
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


void KPresenter::KPresenterView_stub::textEnumList()
{
  CORBA::Request_var _req = this->_request( "textEnumList" );
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


void KPresenter::KPresenterView_stub::textUnsortList()
{
  CORBA::Request_var _req = this->_request( "textUnsortList" );
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


void KPresenter::KPresenterView_stub::textNormalText()
{
  CORBA::Request_var _req = this->_request( "textNormalText" );
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


struct _global_init_KPresenter_KPresenterView {
  _global_init_KPresenter_KPresenterView()
  {
    if( ::OPParts::View::_narrow_helpers == NULL )
      ::OPParts::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::View::_narrow_helpers->push_back( KPresenter::KPresenterView::_narrow_helper2 );
  }
} __global_init_KPresenter_KPresenterView;

vector<CORBA::Narrow_proto> *KPresenter::KPresenterView::_narrow_helpers;

CORBA::TypeCode_ptr KPresenter::_tc_KPresenterView = (new CORBA::TypeCode(
  "010000000e0000003f000000010000002200000049444c3a4b5072657365"
  "6e7465722f4b50726573656e746572566965773a312e300000000f000000"
  "4b50726573656e7465725669657700" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KPresenter::KPresenterView_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "KPresenterView" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KPresenter::KPresenterView_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KPresenter::KPresenterView::_nil();
    return TRUE;
  }
  _obj = ::KPresenter::KPresenterView::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface KPresenterDocument
KPresenter::KPresenterDocument::~KPresenterDocument()
{
}

KPresenter::KPresenterDocument_ptr KPresenter::KPresenterDocument::_duplicate( KPresenterDocument_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KPresenter::KPresenterDocument::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KPresenter/KPresenterDocument:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Document::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KPresenter::KPresenterDocument::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KPresenter/KPresenterDocument:1.0" ) == 0) {
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

KPresenter::KPresenterDocument_ptr KPresenter::KPresenterDocument::_narrow( CORBA::Object_ptr _obj )
{
  KPresenter::KPresenterDocument_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KPresenter/KPresenterDocument:1.0" ))
      return _duplicate( (KPresenter::KPresenterDocument_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KPresenter::KPresenterDocument_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KPresenter/KPresenterDocument:1.0" ) ) {
    _o = new KPresenter::KPresenterDocument_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KPresenter::KPresenterDocument_ptr KPresenter::KPresenterDocument::_nil()
{
  return NULL;
}

KPresenter::KPresenterDocument_stub::~KPresenterDocument_stub()
{
}

struct _global_init_KPresenter_KPresenterDocument {
  _global_init_KPresenter_KPresenterDocument()
  {
    if( ::OPParts::Document::_narrow_helpers == NULL )
      ::OPParts::Document::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Document::_narrow_helpers->push_back( KPresenter::KPresenterDocument::_narrow_helper2 );
  }
} __global_init_KPresenter_KPresenterDocument;

vector<CORBA::Narrow_proto> *KPresenter::KPresenterDocument::_narrow_helpers;

CORBA::TypeCode_ptr KPresenter::_tc_KPresenterDocument = (new CORBA::TypeCode(
  "010000000e00000047000000010000002600000049444c3a4b5072657365"
  "6e7465722f4b50726573656e746572446f63756d656e743a312e30000000"
  "130000004b50726573656e746572446f63756d656e7400" ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KPresenter::KPresenterDocument_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "KPresenterDocument" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KPresenter::KPresenterDocument_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KPresenter::KPresenterDocument::_nil();
    return TRUE;
  }
  _obj = ::KPresenter::KPresenterDocument::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}


// Stub interface Factory
KPresenter::Factory::~Factory()
{
}

KPresenter::Factory_ptr KPresenter::Factory::_duplicate( Factory_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *KPresenter::Factory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KPresenter/Factory:1.0" ) == 0 )
    return (void *)this;
  if( void *_p = OPParts::Factory::_narrow_helper( _repoid ))
    return _p;
  return NULL;
}

bool KPresenter::Factory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KPresenter/Factory:1.0" ) == 0) {
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

KPresenter::Factory_ptr KPresenter::Factory::_narrow( CORBA::Object_ptr _obj )
{
  KPresenter::Factory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    if( void *_p = _obj->_narrow_helper( "IDL:KPresenter/Factory:1.0" ))
      return _duplicate( (KPresenter::Factory_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new KPresenter::Factory_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  if( _obj->_is_a_remote( "IDL:KPresenter/Factory:1.0" ) ) {
    _o = new KPresenter::Factory_stub;
    _o->CORBA::Object::operator=( *_obj );
    return _o;
  }
  return _nil();
}

KPresenter::Factory_ptr KPresenter::Factory::_nil()
{
  return NULL;
}

KPresenter::Factory_stub::~Factory_stub()
{
}

struct _global_init_KPresenter_Factory {
  _global_init_KPresenter_Factory()
  {
    if( ::OPParts::Factory::_narrow_helpers == NULL )
      ::OPParts::Factory::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OPParts::Factory::_narrow_helpers->push_back( KPresenter::Factory::_narrow_helper2 );
  }
} __global_init_KPresenter_Factory;

vector<CORBA::Narrow_proto> *KPresenter::Factory::_narrow_helpers;

CORBA::TypeCode_ptr KPresenter::_tc_Factory = (new CORBA::TypeCode(
  "010000000e00000030000000010000001b00000049444c3a4b5072657365"
  "6e7465722f466163746f72793a312e30000008000000466163746f727900"
   ))->mk_constant();

CORBA::Boolean
operator<<=( CORBA::Any &_a, const KPresenter::Factory_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "Factory" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KPresenter::Factory_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::KPresenter::Factory::_nil();
    return TRUE;
  }
  _obj = ::KPresenter::Factory::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface KPresenterView
KPresenter::KPresenterView_skel::KPresenterView_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KPresenter/KPresenterView:1.0", "KPresenterView" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KPresenter/KPresenterView:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<KPresenterView_skel>( this ) );
}

KPresenter::KPresenterView_skel::KPresenterView_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KPresenter/KPresenterView:1.0", "KPresenterView" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<KPresenterView_skel>( this ) );
}

KPresenter::KPresenterView_skel::~KPresenterView_skel()
{
}

bool KPresenter::KPresenterView_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
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
  if( strcmp( _req->op_name(), "editDelete" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    editDelete();
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
  if( strcmp( _req->op_name(), "newView" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    newView();
    return true;
  }
  if( strcmp( _req->op_name(), "insertPage" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertPage();
    return true;
  }
  if( strcmp( _req->op_name(), "insertPicture" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertPicture();
    return true;
  }
  if( strcmp( _req->op_name(), "insertClipart" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertClipart();
    return true;
  }
  if( strcmp( _req->op_name(), "insertLine" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertLine();
    return true;
  }
  if( strcmp( _req->op_name(), "insertRectangle" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertRectangle();
    return true;
  }
  if( strcmp( _req->op_name(), "insertCircleOrEllipse" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertCircleOrEllipse();
    return true;
  }
  if( strcmp( _req->op_name(), "insertText" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertText();
    return true;
  }
  if( strcmp( _req->op_name(), "insertAutoform" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertAutoform();
    return true;
  }
  if( strcmp( _req->op_name(), "insertObject" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertObject();
    return true;
  }
  if( strcmp( _req->op_name(), "insertLineHidl" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertLineHidl();
    return true;
  }
  if( strcmp( _req->op_name(), "insertLineVidl" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertLineVidl();
    return true;
  }
  if( strcmp( _req->op_name(), "insertLineD1idl" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertLineD1idl();
    return true;
  }
  if( strcmp( _req->op_name(), "insertLineD2idl" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertLineD2idl();
    return true;
  }
  if( strcmp( _req->op_name(), "insertNormRectidl" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertNormRectidl();
    return true;
  }
  if( strcmp( _req->op_name(), "insertRoundRectidl" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    insertRoundRectidl();
    return true;
  }
  if( strcmp( _req->op_name(), "extraPenBrush" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    extraPenBrush();
    return true;
  }
  if( strcmp( _req->op_name(), "extraRaise" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    extraRaise();
    return true;
  }
  if( strcmp( _req->op_name(), "extraLower" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    extraLower();
    return true;
  }
  if( strcmp( _req->op_name(), "extraRotate" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    extraRotate();
    return true;
  }
  if( strcmp( _req->op_name(), "extraBackground" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    extraBackground();
    return true;
  }
  if( strcmp( _req->op_name(), "extraLayout" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    extraLayout();
    return true;
  }
  if( strcmp( _req->op_name(), "extraOptions" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    extraOptions();
    return true;
  }
  if( strcmp( _req->op_name(), "screenConfigPages" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenConfigPages();
    return true;
  }
  if( strcmp( _req->op_name(), "screenAssignEffect" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenAssignEffect();
    return true;
  }
  if( strcmp( _req->op_name(), "screenStart" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenStart();
    return true;
  }
  if( strcmp( _req->op_name(), "screenStop" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenStop();
    return true;
  }
  if( strcmp( _req->op_name(), "screenPause" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenPause();
    return true;
  }
  if( strcmp( _req->op_name(), "screenFirst" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenFirst();
    return true;
  }
  if( strcmp( _req->op_name(), "screenPrev" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenPrev();
    return true;
  }
  if( strcmp( _req->op_name(), "screenNext" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenNext();
    return true;
  }
  if( strcmp( _req->op_name(), "screenLast" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenLast();
    return true;
  }
  if( strcmp( _req->op_name(), "screenSkip" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenSkip();
    return true;
  }
  if( strcmp( _req->op_name(), "screenFullScreen" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenFullScreen();
    return true;
  }
  if( strcmp( _req->op_name(), "screenPen" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    screenPen();
    return true;
  }
  if( strcmp( _req->op_name(), "helpContents" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    helpContents();
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
  if( strcmp( _req->op_name(), "helpAboutKOffice" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    helpAboutKOffice();
    return true;
  }
  if( strcmp( _req->op_name(), "helpAboutKDE" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    helpAboutKDE();
    return true;
  }
  if( strcmp( _req->op_name(), "sizeSelected" ) == 0 ) {
    CORBA::String_var size;

    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 1, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_string );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= CORBA::Any::to_string( size, 0 );
    sizeSelected( size );
    return true;
  }
  if( strcmp( _req->op_name(), "fontSelected" ) == 0 ) {
    CORBA::String_var font;

    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 1, _args );
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_string );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= CORBA::Any::to_string( font, 0 );
    fontSelected( font );
    return true;
  }
  if( strcmp( _req->op_name(), "textBold" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textBold();
    return true;
  }
  if( strcmp( _req->op_name(), "textItalic" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textItalic();
    return true;
  }
  if( strcmp( _req->op_name(), "textUnderline" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textUnderline();
    return true;
  }
  if( strcmp( _req->op_name(), "textColor" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textColor();
    return true;
  }
  if( strcmp( _req->op_name(), "textAlignLeft" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textAlignLeft();
    return true;
  }
  if( strcmp( _req->op_name(), "textAlignCenter" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textAlignCenter();
    return true;
  }
  if( strcmp( _req->op_name(), "textAlignRight" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textAlignRight();
    return true;
  }
  if( strcmp( _req->op_name(), "mtextAlignLeft" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    mtextAlignLeft();
    return true;
  }
  if( strcmp( _req->op_name(), "mtextAlignCenter" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    mtextAlignCenter();
    return true;
  }
  if( strcmp( _req->op_name(), "mtextAlignRight" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    mtextAlignRight();
    return true;
  }
  if( strcmp( _req->op_name(), "mtextFont" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    mtextFont();
    return true;
  }
  if( strcmp( _req->op_name(), "textEnumList" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textEnumList();
    return true;
  }
  if( strcmp( _req->op_name(), "textUnsortList" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textUnsortList();
    return true;
  }
  if( strcmp( _req->op_name(), "textNormalText" ) == 0 ) {
    CORBA::NVList_ptr _args;
    _orbnc()->create_list( 0, _args );

    if (!_req->params( _args ))
      return true;

    textNormalText();
    return true;
  }
  return false;
}

KPresenter::KPresenterView_ptr KPresenter::KPresenterView_skel::_this()
{
  return KPresenter::KPresenterView::_duplicate( this );
}


// Dynamic Implementation Routine for interface KPresenterDocument
KPresenter::KPresenterDocument_skel::KPresenterDocument_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KPresenter/KPresenterDocument:1.0", "KPresenterDocument" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KPresenter/KPresenterDocument:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<KPresenterDocument_skel>( this ) );
}

KPresenter::KPresenterDocument_skel::KPresenterDocument_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KPresenter/KPresenterDocument:1.0", "KPresenterDocument" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<KPresenterDocument_skel>( this ) );
}

KPresenter::KPresenterDocument_skel::~KPresenterDocument_skel()
{
}

bool KPresenter::KPresenterDocument_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KPresenter::KPresenterDocument_ptr KPresenter::KPresenterDocument_skel::_this()
{
  return KPresenter::KPresenterDocument::_duplicate( this );
}


// Dynamic Implementation Routine for interface Factory
KPresenter::Factory_skel::Factory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KPresenter/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KPresenter/Factory:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KPresenter::Factory_skel::Factory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KPresenter/Factory:1.0", "Factory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<Factory_skel>( this ) );
}

KPresenter::Factory_skel::~Factory_skel()
{
}

bool KPresenter::Factory_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env )
{
  return false;
}

KPresenter::Factory_ptr KPresenter::Factory_skel::_this()
{
  return KPresenter::Factory::_duplicate( this );
}

