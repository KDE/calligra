/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>

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
#include <koStoreStream.h>

#include "kformeditor_doc.h"
#include "kformeditor_shell.h"
#include "kformeditor_view.h"

#define DEFAULT_WIDTH  500
#define DEFAULT_HEIGHT 300

/*****************************************************************************
 *
 * KformEditorDoc
 *
 *****************************************************************************/

KformEditorDoc::KformEditorDoc()
{
  kdebug( KDEBUG_INFO, 0, "KformEditorDoc::KformEditorDoc()" );

  ADD_INTERFACE("IDL:KOffice/Print:1.0");

  m_bEmpty = true;
  m_lstViews.setAutoDelete( false );

  m_FormWidth = DEFAULT_WIDTH;
  m_FormHeight = DEFAULT_HEIGHT;

  m_FormName = "unnamed";
}

KformEditorDoc::~KformEditorDoc()
{
}

bool KformEditorDoc::initDoc()
{
  return true;
}

void KformEditorDoc::cleanUp()
{
  kdebug( KDEBUG_INFO, 0, "CleanUp KformEditorDoc" );

  if ( m_bIsClean )
    return;

  ASSERT( m_lstViews.count() == 0 );

  m_lstFormObjects.clear();

  KoDocument::cleanUp();
}

KformEditorView* KformEditorDoc::createFormView( QWidget* _parent )
{
  KformEditorView *p = new KformEditorView( _parent, 0L, this );
 
  m_lstViews.append( p );
  return p;
}                                                                                                   

KOffice::MainWindow_ptr KformEditorDoc::createMainWindow()
{
  KformEditorShell* shell = new KformEditorShell;
  shell->show();
  shell->setDocument( this );

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

void KformEditorDoc::removeView( KformEditorView* _view )
{
  m_lstViews.removeRef( _view );
}

OpenParts::View_ptr KformEditorDoc::createView()
{
  return OpenParts::View::_duplicate( createFormView() );
}

void KformEditorDoc::viewList( OpenParts::Document::ViewList & _list )
{
  _list.clear();

  QListIterator<KformEditorView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    _list.append( OpenParts::View::_duplicate( it.current() ) );
  }
}

int KformEditorDoc::viewCount()
{
  return m_lstViews.count();
}

bool KformEditorDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr )
{
  kdebug( KDEBUG_INFO, 0, "------------------------ LOADING --------------------" );

  string tag;
  vector<KOMLAttrib> lst;
  vector<KOMLAttrib>::const_iterator it;
  string name;

  // DOC
  if ( !parser.open( "DOC", tag ) )
  {
    kdebug( KDEBUG_INFO, 0, "Missing DOC" );
    return false;
  }

  KOMLParser::parseTag( tag.c_str(), name, lst );
  for( it = lst.begin(); it != lst.end(); it++ )
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
  if( !parser.open( "FORM", tag ) )
  {
    kdebug( KDEBUG_INFO, 0, "Unknown tag:'%s'", tag.c_str() );
    return false;
  }

  KOMLParser::parseTag( tag.c_str(), name, lst );
  for( it = lst.begin(); it != lst.end(); it++ )
  {
    if ( it->m_strName == "name" )
    {
      m_FormName = *it->m_strValue.c_str();
    }
    else if ( (*it).m_strName == "height" )
    {
      m_FormHeight = atoi( (*it).m_strValue.c_str() );
    }
    else if ( ( *it ).m_strName == "width" )
    {
      m_FormWidth = atoi( (*it).m_strValue.c_str() );
    }                                                                                           
    else kdebug( KDEBUG_INFO, 0, "Unknown attrib FORM:'%s'", (*it).m_strName.c_str() );
  }

  // BUTTON, LABEL, LINEEDIT, LISTBOX
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    // BGCOLOR
    if ( name == "BGCOLOR" )
    {
      int red = 0;
      int green = 0;
      int blue = 0;

      KOMLParser::parseTag( tag.c_str(), name, lst );
      for( it = lst.begin(); it != lst.end(); it++ )
      {
        if ( (*it).m_strName == "red" )
        {
          red = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "green" )
        {
          green = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "blue" )
        {
          blue = atoi( (*it).m_strValue.c_str() );
        }
        else kdebug( KDEBUG_INFO, 0, "Unknown attrib BGCOLOR:'%s'", ( *it ).m_strName.c_str() );
      }

      setBackgroundColor( QColor( red, green, blue ) );
    }

    // BUTTON
    else if ( name == "BUTTON" )
    {
      QString qname = "unbekannt";
      int posx = 0;
      int posy = 0;
      int width = 100;
      int height = 30;
      QString text = "Button";
      QString action = "quit";

      KOMLParser::parseTag( tag.c_str(), name, lst );
      for( it = lst.begin(); it != lst.end(); it++ )
      {
        if ( (*it).m_strName == "name" )
        {
          qname = (*it).m_strValue.c_str();
        }
        else if ( (*it).m_strName == "posx" )
        {
          posx = atoi( (*it).m_strValue.c_str() ); 
        }
        else if ( (*it).m_strName == "posy" )
        {
           posy = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "height" )
        {
           height = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "width" )
        {
          width = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "text" )
        {
           text = (*it).m_strValue.c_str();
        }
        else if ( (*it).m_strName == "action" )
        {
           action = (*it).m_strValue.c_str();
        }
        else kdebug( KDEBUG_INFO, 0, "Unknown attrib BUTTON:'%s'", ( *it ).m_strName.c_str() );
      }

      FormObject* button = new FormObject( FormObject::Button, qname, posx, posy, width, height, text );
      button->setAction( action );
      m_lstFormObjects.append( button );
    }

    // LABEL
    else if ( name == "LABEL" )
    {
      QString qname = "unbekannt";
      int posx = 0;
      int posy = 0;
      int width = 100;
      int height = 30;
      QString text = "Label";
 
      KOMLParser::parseTag( tag.c_str(), name, lst );
      for( it = lst.begin(); it != lst.end(); it++ )
      {
        if ( (*it).m_strName == "name" )
        {
          qname = (*it).m_strValue.c_str();
        }
        else if ( (*it).m_strName == "posx" )
        {
          posx = atoi( (*it).m_strValue.c_str() );
        }                                                                                           
        else if ( (*it).m_strName == "posy" )
        {
           posy = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "height" )
        {
           height = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "width" )
        {
          width = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "text" )
        {
           text = (*it).m_strValue.c_str();
        }
        else kdebug( KDEBUG_INFO, 0, "Unknown attrib LABEL:'%s'", ( *it ).m_strName.c_str() );
      }
 
      FormObject* label = new FormObject( FormObject::Label, qname, posx, posy, width, height, text );
      m_lstFormObjects.append( label );
    }                                                                                               

    // LINEEDIT
    else if ( name == "LINEEDIT" )
    {
      QString qname = "unbekannt";
      int posx = 0;
      int posy = 0;
      int width = 100;
      int height = 30;
      QString text = "Inputline";
 
      KOMLParser::parseTag( tag.c_str(), name, lst );
      for( it = lst.begin(); it != lst.end(); it++ )
      {
        if ( (*it).m_strName == "name" )
        {
          qname = (*it).m_strValue.c_str();
        }
        else if ( (*it).m_strName == "posx" )
        {
          posx = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "posy" )
        {
           posy = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "height" )
        {
           height = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "width" )
        {
          width = atoi( (*it).m_strValue.c_str() );
        }                                                                                           
        else kdebug( KDEBUG_INFO, 0, "Unknown attrib LINEEDIT:'%s'", ( *it ).m_strName.c_str() );
      }
 
      FormObject* lineedit = new FormObject( FormObject::LineEdit, qname, posx, posy, width, height, text );
      m_lstFormObjects.append( lineedit );
    }                                                                                               

    // LISTBOX
    else if ( name == "LISTBOX" )
    {
      QString qname = "unbekannt";
      int posx = 0;
      int posy = 0;
      int width = 100;
      int height = 30;
      QString text = "Label";
 
      KOMLParser::parseTag( tag.c_str(), name, lst );
      for( it = lst.begin(); it != lst.end(); it++ )
      {
        if ( (*it).m_strName == "name" )
        {
          qname = (*it).m_strValue.c_str();
        }
        else if ( (*it).m_strName == "posx" )
        {
          posx = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "posy" )
        {
           posy = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "height" )
        {
           height = atoi( (*it).m_strValue.c_str() );
        }
        else if ( (*it).m_strName == "width" )
        {
          width = atoi( (*it).m_strValue.c_str() );
        }
        else kdebug( KDEBUG_INFO, 0, "Unknown attrib LISTBOX:'%s'", ( *it ).m_strName.c_str() );
      }
 
      FormObject* label = new FormObject( FormObject::ListBox, qname, posx, posy, width, height );
      m_lstFormObjects.append( label );
    }                                                                                               

    else kdebug( KDEBUG_INFO, 0,  "Unknown tag '%s' in the DOCUMENT", tag.c_str() );

    if ( !parser.close( tag ) )
    {
      KOMLParser::parseTag( tag.c_str(), name, lst );
      if( name == "FORM" )
      {
        kdebug( KDEBUG_INFO, 0, "ERR: Only one form per file allowed" );
      }
      else
      {
        kdebug( KDEBUG_INFO, 0, "ERR: Closing FORM" );
      }
      return false;
    }
  }

  kdebug( KDEBUG_INFO, 0, "------------------------ LOADING DONE --------------------" );

  return true;
}

bool KformEditorDoc::completeLoading( KOStore::Store_ptr  )
{
  kdebug( KDEBUG_INFO, 0, "------------------------ COMPLETION DONE --------------------" );

/*
  QString str = url();
  QString u = str.in();
  u += "/image";
  if ( _store->open( u, 0L ) )
  {
    istorestream in( _store );
    in >> ... 
    _store->close();
  }
*/

  m_bEmpty = false;

  //emit sigUpdateView();

  return true;
}

void KformEditorDoc::setBackgroundColor( const QColor& _color )
{
  m_backgroundColor = _color;

  emit sigUpdateView();
};

QColor KformEditorDoc::backgroundColor()
{
  return m_backgroundColor;
};

void KformEditorDoc::setFormName( const QString& _name )
{
  m_FormName = _name;

  emit sigUpdateView();
}

QString KformEditorDoc::formName()
{
  return m_FormName;
}

void KformEditorDoc::draw( QPaintDevice* , long int _width, long int _height, float )
{
  kdebug( KDEBUG_INFO, 0, "DRAWING w=%li h=%li", _width, _height );
}

bool KformEditorDoc::isEmpty()
{
  return m_bEmpty;
}

#include "kformeditor_doc.moc"
