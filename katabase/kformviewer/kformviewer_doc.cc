/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpainter.h>

#include <kapp.h>

#include <komlParser.h>
#include <komlMime.h>

#include <koStream.h>

#include "kformviewer_doc.h"
#include "kformviewer_shell.h"
#include "kformviewer_view.h"

#define DEFAULT_WIDTH  500
#define DEFAULT_HEIGHT 300

/*****************************************************************************
 *
 * KformViewerDoc
 *
 *****************************************************************************/

KformViewerDoc::KformViewerDoc()
{
  ADD_INTERFACE("IDL:KOffice/Print:1.0");

  m_bEmpty = true;
  m_lstViews.setAutoDelete( false );
}

KformViewerDoc::~KformViewerDoc()
{
}

CORBA::Boolean KformViewerDoc::initDoc()
{
  return true;
}

void KformViewerDoc::cleanUp()
{
  kdebug( KDEBUG_INFO, 0, "CleanUp KformViewerDoc" );

  if ( m_bIsClean )
    return;

  ASSERT( m_lstViews.count() == 0 );

  m_lstAllChildren.clear();
  m_lstFormObjects.clear();

  KoDocument::cleanUp();
}

KformViewerView* KformViewerDoc::createFormView( QWidget* _parent )
{
  KformViewerView *p = new KformViewerView( _parent, 0L, this );
 
  m_lstViews.append( p );
  return p;
}                                                                                                   

KOffice::MainWindow_ptr KformViewerDoc::createMainWindow()
{
  KformViewerShell* shell = new KformViewerShell;
  shell->show();
  shell->setDocument( this );

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

void KformViewerDoc::removeView( KformViewerView* _view )
{
  m_lstViews.removeRef( _view );
}

OpenParts::View_ptr KformViewerDoc::createView()
{
  return OpenParts::View::_duplicate( createFormView() );
}

void KformViewerDoc::viewList( OpenParts::Document::ViewList*& _list )
{
  _list->length( m_lstViews.count() );

  int i = 0;
  QListIterator<KformViewerView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OpenParts::View::_duplicate( it.current() );
  }
}

int KformViewerDoc::viewCount()
{
  return m_lstViews.count();
}

bool KformViewerDoc::loadXML( KOMLParser& _parser, KOStore::Store_ptr _store )
{
  kdebug( KDEBUG_INFO, 0, "------------------------ LOADING --------------------" );

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  // DOC
  if ( !_parser.open( "DOC", tag ) )
  {
    kdebug( KDEBUG_INFO, 0, "Missing DOC" );
    return false;
  }

  KOMLParser::parseTag( tag.c_str(), name, lst );
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for( ; it != lst.end(); it++ )
  {
    if ( it->m_strName == "mime" )
    {
      if ( it->m_strValue != MIME_TYPE )
      {
        kdebug( KDEBUG_INFO, 0, "Unknown mime type %s", it->m_strValue.c_str() );

        return false;
      }
    }
  }

  // FORM
  while( _parser.open( "", tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );
 
    if ( name == "SIZE" )
    {
      cout << "SIZE found" << endl;

      KOMLParser::parseTag( tag.c_str(), name, lst );
      vector<KOMLAttrib>::const_iterator it = lst.begin();
      for( ; it != lst.end(); it++ )
      {
        if ( ( *it ).m_strName == "height" )
        {
          
        }
        else if ( ( *it ).m_strName == "width" )
        {
 
        }
        else kdebug( KDEBUG_INFO, 0, "Unknown attrib PAPER:'%s'", ( *it ).m_strName.c_str() );
      }
    }

    else if ( name == "BUTTON" )
    {
      cout << "BUTTON found" << endl;

      KOMLParser::parseTag( tag.c_str(), name, lst );
      vector<KOMLAttrib>::const_iterator it = lst.begin();
      for( ; it != lst.end(); it++ )
      {
        if ( ( *it ).m_strName == "height" )
        {
 
        }
        else if ( ( *it ).m_strName == "width" )
        {
 
        }
        else kdebug( KDEBUG_INFO, 0, "Unknown attrib PAPER:'%s'", ( *it ).m_strName.c_str() );
      }
    }
 
    else kdebug( KDEBUG_INFO, 0,  "Unknown tag '%s' in the DOCUMENT", tag.c_str() );
 
    if ( !_parser.close( tag ) )
    {
      kdebug( KDEBUG_INFO, 0, "ERR: Closing Child" );
      return false;
    }
  }

  kdebug( KDEBUG_INFO, 0, "------------------------ LOADING DONE --------------------" );

  return true;
}

bool KformViewerDoc::completeLoading( KOStore::Store_ptr  )
{
  kdebug( KDEBUG_INFO, 0, "------------------------ COMPLETION DONE --------------------" );

/*
  CORBA::String_var str = url();
  QString u = str.in();
  u += "/image";
  _store->open( u, 0L );
  {
    istorestream in( _store );
  }
  _store->close();
*/

  m_bEmpty = false;

  //emit sigUpdateView();

  return true;
}

void KformViewerDoc::draw( QPaintDevice* , CORBA::Long _width, CORBA::Long _height,
		      CORBA::Float  )
{
  kdebug( KDEBUG_INFO, 0, "DRAWING w=%li h=%li", _width, _height );
}

bool KformViewerDoc::openDocument( const char *, const char * )
{
  emit sigUpdateView();

  m_bEmpty = false;

  return true;
}

char* KformViewerDoc::mimeType()
{
  return CORBA::string_dup( MIME_TYPE );
}

bool KformViewerDoc::isEmpty()
{
  return m_bEmpty;
}

#include "kformviewer_doc.moc"
