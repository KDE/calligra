/*
 *  kimageshop_doc.cc - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qprinter.h>
#include <qdatetm.h>
#include <qpainter.h>
#include <qrect.h>

#include <kapp.h>
#include <kimgio.h>
#include <kstddirs.h>
#include <kglobal.h>

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <komlMime.h>

#include <koStream.h>

#include "kimageshop_doc.h"
#include "kimageshop_shell.h"

#define MM_TO_POINT 2.83465
#define POINT_TO_MM 0.3527772388

KImageShopDoc::KImageShopDoc(int w, int h) : Canvas(w, h)
{
  ADD_INTERFACE("IDL:KOffice/Print:1.0");

  m_bModified = false;
  m_bEmpty = true;

  kimgioRegister();

  m_lstViews.setAutoDelete(false);
}

KImageShopDoc::~KImageShopDoc()
{
}

CORBA::Boolean KImageShopDoc::initDoc()
{
  // load some test layers
  QString _image = locate("appdata", "images/cam9b.jpg");	
  addRGBLayer(_image);
  setLayerOpacity(200);

  _image = locate("appdata", "images/cambw12.jpg");
  addRGBLayer(_image);
  moveLayer(256,384);
  setLayerOpacity(180);

  _image = locate("appdata", "images/cam05.jpg");
  addRGBLayer(_image);
  setLayerOpacity(255);

  _image = locate("appdata", "images/cam6.jpg");
  addRGBLayer(_image);
  moveLayer(240,280);
  setLayerOpacity(255);

  _image = locate("appdata", "images/img2.jpg");
  addRGBLayer(_image);
  setLayerOpacity(80);
  
  compositeImage(QRect());
  return true;
}

void KImageShopDoc::cleanUp()
{
  kdebug(KDEBUG_INFO, 0, "CleanUp KImageShopDoc");

  if (m_bIsClean)
    return;

  ASSERT(m_lstViews.count() == 0);

  m_lstAllChildren.clear();
  KoDocument::cleanUp();
}

KOffice::MainWindow_ptr KImageShopDoc::createMainWindow()
{
  KImageShopShell* shell = new KImageShopShell;
  shell->show();
  shell->setDocument(this);

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

void KImageShopDoc::removeView(KImageShopView* _view)
{
  m_lstViews.removeRef(_view);
}

KImageShopView* KImageShopDoc::createImageView( QWidget* _parent )
{
  KImageShopView *p = new KImageShopView( _parent, 0L, this );

  m_lstViews.append( p );
  return p;
}

OpenParts::View_ptr KImageShopDoc::createView()
{
  return OpenParts::View::_duplicate( createImageView() );
}

void KImageShopDoc::slotUpdateViews(const QRect &area)
{
  // canvas was modified -> send update signal to all views
  emit sigUpdateView(area);
}

void KImageShopDoc::viewList( OpenParts::Document::ViewList*& _list )
{
  _list->length( m_lstViews.count() );

  int i = 0;
  QListIterator<KImageShopView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OpenParts::View::_duplicate( it.current() );
  }
}

int KImageShopDoc::viewCount()
{
  return m_lstViews.count();
}

bool KImageShopDoc::save( ostream& out, const char* /* format */ )
{
  /*
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Torben Weis" << "\" email=\"" << "weis@kde.org" << "\" editor=\"" << "kimageshop"
      << "\" mime=\"" << "application/x-kimageshop" << "\" >" << endl;

  out << otag << "<PAPER format=\"" << paperFormatString().ascii() << "\" orientation=\"" << orientationString().ascii() << "\">" << endl;
  out << indent << "<PAPERBORDERS left=\"" << leftBorder() << "\" top=\"" << topBorder() << "\" right=\"" << rightBorder()
      << " bottom=\"" << bottomBorder() << "\"/>" << endl;
  out << indent << "<HEAD left=\"" << headLeft().ascii() << "\" center=\"" << headMid().ascii() << "\" right=\"" << headRight().ascii() << "\"/>" << endl;
  out << indent << "<FOOT left=\"" << footLeft().ascii() << "\" center=\"" << footMid().ascii() << "\" right=\"" << footRight().ascii() << "\"/>" << endl;
  out << etag << "</PAPER>" << endl;

  out << etag << "</DOC>" << endl;
  */

  setModified(false);
  return true;
}

bool KImageShopDoc::completeSaving( KOStore::Store_ptr _store )
{
  /*
  CORBA::String_var u = url();
  QString u2 = u.in();
  u2 += "/image";

  _store->open( u2, "image/bmp" );
  {
    ostorestream out( _store );
    out << m_image;
    out.flush();
  }
  _store->close();
  */
  return true;
}

bool KImageShopDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr  )
{
  /*
  kdebug( KDEBUG_INFO, 0, "------------------------ LOADING --------------------" );

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  // DOC
  if ( !parser.open( "DOC", tag ) )
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
      if ( it->m_strValue != "application/x-kimageshop" )
      {
		kdebug( KDEBUG_INFO, 0, "Unknown mime type %s", it->m_strValue.c_str() );
		return false;
      }
    }
  }

  QString format = "A4", orientation = "Portrait";
  float left = 20.0, right = 20.0, bottom = 20.0, top = 20.0;
  QString hl="", hm="", hr="";
  QString fl="", fm="", fr="";

  // PAPER
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    if ( name == "PAPER" )
    {
      KOMLParser::parseTag( tag.c_str(), name, lst );
      vector<KOMLAttrib>::const_iterator it = lst.begin();
      for( ; it != lst.end(); it++ )
      {
		if ( it->m_strName == "format" )
		{
		  format = it->m_strValue.c_str();
		}
		else if ( it->m_strName == "orientation" )
		{
		  orientation = it->m_strValue.c_str();
		}
		else
		  kdebug( KDEBUG_INFO, 0, "Unknown attrib PAPER:'%s'",it->m_strName.c_str() );
      }

      // PAPERBORDERS, HEAD, FOOT
      while( parser.open( 0L, tag ) )
      {
		KOMLParser::parseTag( tag.c_str(), name, lst );

		if ( name == "PAPERBORDERS" )
		{
		  KOMLParser::parseTag( tag.c_str(), name, lst );
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for( ; it != lst.end(); it++ )
		  {
		    if ( it->m_strName == "left" )
		    {
		      left = atof( it->m_strValue.c_str() );
		    }
		    else if ( it->m_strName == "top" )
		    {
		      top = atof( it->m_strValue.c_str() );
		    }
		    else if ( it->m_strName == "right" )
		    {
		      right = atof( it->m_strValue.c_str() );
		    }
		    else if ( it->m_strName == "bottom" )
		    {
		      bottom = atof( it->m_strValue.c_str() );
		    }
		    else
		      kdebug( KDEBUG_INFO, 0, "Unknown attrib 'PAPERBORDERS:%s'", it->m_strName.c_str() );
		  }
		}
      	else if ( name == "HEAD" )
		{
		  KOMLParser::parseTag( tag.c_str(), name, lst );
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for( ; it != lst.end(); it++ )
		  {
		    if ( it->m_strName == "left" )
		    {
		      hl = it->m_strValue.c_str();
		    }
		    else if ( it->m_strName == "center" )
		    {
		      hm = it->m_strValue.c_str();
		    }
		    else if ( it->m_strName == "right" )
		    {
		      hr = it->m_strValue.c_str();
		    }
	        else
	          kdebug( KDEBUG_INFO, 0, "Unknown attrib 'HEAD:%s'", it->m_strName.c_str() );
	      }
	    }
        else if ( name == "FOOT" )
	    {
		  KOMLParser::parseTag( tag.c_str(), name, lst );
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for( ; it != lst.end(); it++ )
		  {
	 	    if ( it->m_strName == "left" )
	        {
	          fl = it->m_strValue.c_str();
	        }
	        else if ( it->m_strName == "center" )
	        {
	        	fm = it->m_strValue.c_str();
	        }
	        else if ( it->m_strName == "right" )
	        {
	          fr = it->m_strValue.c_str();
	        }
	        else
	          kdebug( KDEBUG_INFO, 0, "Unknown attrib 'FOOT:%s'", it->m_strName.c_str() );
	      }
	    }
	    else
		  kdebug( KDEBUG_INFO, 0, "Unknown tag '%s' in PAPER", tag.c_str() );
	
	    if ( !parser.close( tag ) )
        {
	      kdebug( KDEBUG_INFO, 0, "ERROR: Closing PAPER" );
	      return false;
	    }
      }
    }
    else
      kdebug( KDEBUG_INFO, 0, "Unknown tag '%s' in DOC", tag.c_str() );

    if ( !parser.close( tag ) )
    {
      kdebug( KDEBUG_INFO, 0, "ERROR: Closing DOC" );
      return false;
    }
  }

  parser.close( tag );

  setPaperLayout( left, top, right, bottom, format, orientation );
  setHeadFootLine( hl, hm, hr, fl, fm, fr );

  kdebug( KDEBUG_INFO, 0, "------------------------ LOADING DONE --------------------" );
  */
  return true;
}

bool KImageShopDoc::completeLoading( KOStore::Store_ptr _store )
{
  /*
  kdebug( KDEBUG_INFO, 0, "------------------------ COMPLETION DONE --------------------" );

  CORBA::String_var str = url();
  QString u = str.in();
  u += "/image";
  _store->open( u, 0L );
  {
    istorestream in( _store );
    in >> m_image;
  }
  _store->close();

  m_bModified = false;
  m_bEmpty = false;
  */
  return true;
}

void KImageShopDoc::print( QPaintDevice* _dev )
{
  /*
  QPainter painter;
  painter.begin( _dev );

  // HACK
  int pagenr = 1;
  QString m_strName = "";

  // print head line
  QFont font( "Times", 10 );
  painter.setFont( font );
  QFontMetrics fm = painter.fontMetrics();
  int w = fm.width( headLeft( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() ),
		      (int)( MM_TO_POINT * 10.0 ), headLeft( pagenr, m_strName ) );
  w = fm.width( headMid( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     ( MM_TO_POINT * printableWidth() - (float)w ) / 2.0 ),
		      (int)( MM_TO_POINT * 10.0 ), headMid( pagenr, m_strName ) );
  w = fm.width( headRight( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     MM_TO_POINT * printableWidth() - (float)w ),
		      (int)( MM_TO_POINT * 10.0 ), headRight( pagenr, m_strName ) );

  // print foot line
  w = fm.width( footLeft( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() ),
		      (int)( MM_TO_POINT * ( paperHeight() - 10.0 ) ),
		      footLeft( pagenr, m_strName ) );
  w = fm.width( footMid( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     ( MM_TO_POINT * printableWidth() - (float)w ) / 2.0 ),
		      (int)( MM_TO_POINT * ( paperHeight() - 10.0 ) ),
		      footMid( pagenr, m_strName ) );
  w = fm.width( footRight( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     MM_TO_POINT * printableWidth() - (float)w ),
		      (int)( MM_TO_POINT * ( paperHeight() - 10.0 ) ),
		      footRight( pagenr, m_strName ) );

  painter.translate( MM_TO_POINT * m_leftBorder, MM_TO_POINT * m_topBorder );

  // Print centered
  painter.drawImage( ( MM_TO_POINT * printableWidth() - m_image.width() ) / 2,
		     ( MM_TO_POINT * printableHeight() - m_image.height() ) / 2,
		     m_image );

  painter.end();
  */
}

void KImageShopDoc::draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
		      CORBA::Float _scale )
{
  /*
  kdebug( KDEBUG_INFO, 0, "DRAWING w=%li h=%li", _width, _height );

  QPainter painter;
  painter.begin( _dev );

  if ( _scale != 1.0 )
    painter.scale( _scale, _scale );

  // Print centered
  int x = ( _width - m_image.width() ) / 2;
  int y = ( _height - m_image.height() ) / 2;
  QPoint p( x, y );

  QRect rect;
  if ( x >= 0 )
  {
    rect.setLeft( 0 );
    rect.setWidth( m_image.width() );
  }
  else
  {
    rect.setLeft( -x );
    rect.setWidth( m_image.width() + 2*x );
  }
  if ( y >= 0 )
  {
    rect.setTop( 0 );
    rect.setHeight( m_image.height() );
  }
  else
  {
    rect.setTop( -x );
    rect.setHeight( m_image.height() + 2*x );
  }

  painter.drawImage( p, m_image, rect );

  painter.end();
  */
}

bool KImageShopDoc::openDocument( const char */*_filename*/, const char */*_format*/ )
{
  /*
  if ( !m_image.load( _filename, _format ) )
    return false;

  if ( _format )
    m_strImageFormat = _format;
  else
    m_strImageFormat = QImage::imageFormat( _filename );


  m_bModified = true;
  m_bEmpty = false;
  */
  return true;
}

bool KImageShopDoc::saveDocument( const char */*_filename*/, 
				  const char */*_format*/ )
{
  // ASSERT( !isEmpty() );
  // return m_image.save( _filename, m_strImageFormat );
  return true;
}

char* KImageShopDoc::mimeType()
{
  return CORBA::string_dup( MIME_TYPE );
}

CORBA::Boolean KImageShopDoc::isModified()
{
  return m_bModified;
}

void KImageShopDoc::setModified( bool _c )
{
  m_bModified = _c;
  if ( _c )
  {
    m_bEmpty = false;
  }
}

bool KImageShopDoc::isEmpty()
{
  return m_bEmpty;
}

#include "kimageshop_doc.moc"
