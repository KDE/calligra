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
//#include <qprinter.h>
#include <qrect.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include <qdatetime.h>

#include <kstddirs.h>
#include <kdebug.h>
#include <kimgio.h>

#include <koStream.h>
#include <koStore.h>
#include <koStoreStream.h>
#include <koDocument.h>
#include <koPageLayoutDia.h>
#include <koQueryTypes.h>
#include <koFilterManager.h>

#include "kimage_doc.h"
#include "kimage_shell.h"
#include "kimage_factory.h"

//#define MM_TO_POINT 2.83465
//#define POINT_TO_MM 0.3527772388

KImageDocument::KImageDocument( KoDocument* parent, const char* name )
  : KoDocument( parent, name )
{
}

KImageDocument::~KImageDocument()
{
}

bool KImageDocument::initDoc()
{
  cout << "KImageDocument::initDoc"<< endl;

  m_bEmpty = true;

  m_leftBorder = 20.0;
  m_rightBorder = 20.0;
  m_topBorder = 20.0;
  m_bottomBorder = 20.0;
  m_paperFormat = PG_DIN_A4;
  m_paperWidth = PG_A4_WIDTH;
  m_paperHeight = PG_A4_HEIGHT;
  calcPaperSize();
  m_orientation = PG_PORTRAIT;

  return true;
}

View* KImageDocument::createView( QWidget* parent, const char* name )
{
  KImageView* view = new KImageView( this, parent, name );
  addView( view );

  return view;
}

Shell* KImageDocument::createShell()
{
  Shell* shell = new KImageShell;
  shell->setRootPart( this );
  shell->show();

  return shell;
}

void KImageDocument::paintContent( QPainter& _painter, const QRect& _rect, bool /* _transparent */ )
{
  cout << "KImageDocument::paintContent()" << endl;

  if( isEmpty() )
    return;

  QPixmap pix;
  double dh, dw, d;
	
  switch ( m_drawMode )
  {
    case KImageDocument::OriginalSize:
      pix.convertFromImage( m_image );
      break;
    case KImageDocument::FitToView:
      pix.convertFromImage( m_image.smoothScale( _rect.width(), _rect.height() ) );
      break;
    case KImageDocument::FitWithProps:
      dh = (double) _rect.height() / (double) m_image.height();
      dw = (double) _rect.width() / (double) m_image.width();
      d = ( dh < dw ? dh : dw );
      pix.convertFromImage( m_image.smoothScale( int( d * m_image.width() ), int ( d * m_image.height() ) ) );
      break;
    case KImageDocument::ZoomFactor:
      dw = (double) m_zoomFactorValue.x() / (double) 100.0;
      dh = (double) m_zoomFactorValue.y() / (double) 100.0;
      pix.convertFromImage( m_image.smoothScale( int( dw * m_image.width() ), int ( dh * m_image.height() ) ) );
      break;
  }

  if( positionMode() == Center )
    _painter.drawPixmap( ( _rect.width() - pix.width() ) / 2, ( _rect.height() - pix.height() ) / 2, pix );
  else
    _painter.drawPixmap( 0, 0, pix );
}

QCString KImageDocument::mimeType() const
{
  // FIXME: save the same file type as loaded, not allways KImage file format ;-)

  return "application/x-kimage";
}

QString KImageDocument::configFile() const
{
    return readConfigFile( locate("data", "kimage/kimage.rc", KImageFactory::global()) );
}

bool KImageDocument::loadFromURL( const QString& _url )
{
  cout << "KImageDocument::loadFromURL" << endl;

  // FIXME: crashes because of relative filenames
  if( KImageIO::isSupported( KImageIO::mimeType( _url ) ) )
  {
    initDoc();

    if( !m_image.load( _url ) )
      return false;

    setModified( true );
    m_bEmpty = false;
    return true;
  }

  return KoDocument::loadFromURL( _url );
}

bool KImageDocument::save( ostream& out, const char* )
{
  QDomDocument doc( "image" );
  doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
  QDomElement image = doc.createElement( "image" );
  image.setAttribute( "author", "Michael Koch" );
  image.setAttribute( "email", "koch@kde.org" );
  image.setAttribute( "editor", "KImage" );
  image.setAttribute( "mime", "application/x-kimage" );
  doc.appendChild( image );
  QDomElement paper = doc.createElement( "paper" );
  paper.setAttribute( "format", paperFormatString() );
  paper.setAttribute( "orientation", orientationString() );
  image.appendChild( paper );

  QDomElement borders = doc.createElement( "borders" );
  borders.setAttribute( "left", leftBorder() );
  borders.setAttribute( "top", topBorder() );
  borders.setAttribute( "right", rightBorder() );
  borders.setAttribute( "bottom", bottomBorder() );
  image.appendChild( borders );

  QDomElement head = doc.createElement( "head" );
  image.appendChild( head );
  if ( !headLeft().isEmpty() )
  {
    QDomElement left = doc.createElement( "left" );
    head.appendChild( left );
    left.appendChild( doc.createTextNode( headLeft() ) );
  }
  if ( !headMid().isEmpty() )
  {
    QDomElement center = doc.createElement( "center" );
    head.appendChild( center );
    center.appendChild( doc.createTextNode( headMid() ) );
  }
  if ( !headRight().isEmpty() )
  {
    QDomElement right = doc.createElement( "right" );
    head.appendChild( right );
    right.appendChild( doc.createTextNode( headRight() ) );
  }

  QDomElement foot = doc.createElement( "foot" );
  image.appendChild( foot );
  if ( !footLeft().isEmpty() )
  {
    QDomElement left = doc.createElement( "left" );
    foot.appendChild( left );
    left.appendChild( doc.createTextNode( footLeft() ) );
  }
  if ( !footMid().isEmpty() )
  {
    QDomElement center = doc.createElement( "center" );
    foot.appendChild( center );
    center.appendChild( doc.createTextNode( footMid() ) );
  }
  if ( !footRight().isEmpty() )
  {
    QDomElement right = doc.createElement( "right" );
    foot.appendChild( right );
    right.appendChild( doc.createTextNode( footRight() ) );
  }

  QDomElement drawmode = doc.createElement( "drawmode" );
  image.appendChild( drawmode );
  drawmode.setAttribute( "position", positionString() );
  drawmode.setAttribute( "size", sizeString() );

  QBuffer buffer;
  buffer.open( IO_WriteOnly );
  QTextStream str( &buffer );
  str << doc;
  buffer.close();
  out.write( buffer.buffer().data(), buffer.buffer().size() );
  setModified( false );

  return true;
}

QString KImageDocument::positionString()
{
  QString result;

  switch( m_posMode )
  {
  case LeftTop:
    result = "topleft";
    break;
  case Center:
    result = "centered";
    break;
  }

  return result;
}

void KImageDocument::setPositionString( QString name )
{
  if( name == "centered" )
  {
    kdebug( KDEBUG_INFO, 0, "Setting position 'centered'" );
    m_posMode = Center;
    return;
  }
  else if( name == "topleft" )
  {
    kdebug( KDEBUG_INFO, 0, "Setting position 'lefttop'" );
    m_posMode = LeftTop;
    return;
  }
  
  kdebug( KDEBUG_ERROR, 0, "Error: Unsupported position, using 'topleft' : " + name );

  m_posMode = LeftTop;
}

QString KImageDocument::sizeString()
{
  QString result;

  switch( m_drawMode )
  {
  case OriginalSize:
    result = "original";
    break;
  case FitToView:
    result = "fittoview";
    break;
  case FitWithProps:
    result = "fitwithprops";
    break;
  case ZoomFactor:
    result = "zoomfactor";
    break;
  }

  return result;
}

void KImageDocument::setSizeString( QString name )
{
  if( name == "original" )
  {
    kdebug( KDEBUG_INFO, 0, "Setting 'original size'" );
    m_drawMode = OriginalSize;
    return;
  }
  else if( name == "fittoview" )
  {
    kdebug( KDEBUG_INFO, 0, "Setting 'fit to view'" );
    m_drawMode = FitToView;
    return;
  }
  else if( name == "fitwithprops" )
  {
    kdebug( KDEBUG_INFO, 0, "Setting 'fit with props'" );
    m_drawMode = FitWithProps;
    return;
  }
  else if( name == "zoomfactor" )
  {
    kdebug( KDEBUG_INFO, 0, "Setting 'zoomfactor'" );
    m_drawMode = ZoomFactor;
    return;
  }
  
  kdebug( KDEBUG_ERROR, 0, "Error: Unsupported drawmode, using 'original' : " + name  );

  m_drawMode = OriginalSize;
}

bool KImageDocument::completeSaving( KoStore* _store )
{
  QString file = "image.bmp";

  if ( _store->open( file, "image/bmp" ) )
  {
    ostorestream out( _store );
    writeImageToStream( out, m_image, "BMP" );
    out.flush();
    _store->close();
  }

  return true;
}

bool KImageDocument::load( istream& in, KoStore* store )
{
  int anz;
  char buf[ 4096 ];
  QBuffer buffer;

  buffer.open( IO_WriteOnly );
  do
  {
    in.read( buf, 4096 );
    anz = in.gcount();
    buffer.writeBlock( buf, anz );
  } while( anz > 0 );
  buffer.close();

  buffer.open( IO_ReadOnly );
  QDomDocument doc( &buffer );
  bool b = loadXML( doc, store );
  buffer.close();

  m_bEmpty = false;
  setModified( false );

  return b;
}

bool KImageDocument::loadXML( const QDomDocument& doc, KoStore* /* store */ )
{
  QString format = "A4", orientation = "Portrait";
  float left = 20.0, right = 20.0, bottom = 20.0, top = 20.0;
  QString hl="", hm="", hr="";
  QString fl="", fm="", fr="";

  kdebug( KDEBUG_INFO, 0, "KImageDocument::loadXML()" );

  if( doc.doctype().name() != "image" )
    return false;

  QDomElement image = doc.documentElement();
  if( image.attribute( "mime" ) != "application/x-kimage" )
    return false;

  QDomElement paper = image.namedItem("paper").toElement();
  setPaperFormatString( paper.attribute( "format" ) );
  setOrientationString( paper.attribute( "orientation" ) );

  QDomElement borders = image.namedItem("borders").toElement();
  m_leftBorder = borders.attribute( "left" ).toFloat();
  m_topBorder = borders.attribute( "top" ).toFloat();
  m_rightBorder = borders.attribute( "right" ).toFloat();
  m_bottomBorder = borders.attribute( "bottom" ).toFloat();

  QDomElement head = image.namedItem("head").toElement();
  head.attribute( "left" );
  // hier dann schauen of vorhanden : left center right

  QDomElement foot = image.namedItem("foot").toElement();
  foot.attribute( "left" );
  // hier dann schauen of vorhanden : left center right

  QDomElement drawmode = image.namedItem( "drawmode" ).toElement();

  cout << "Michael : position : " << drawmode.attribute( "position" ).data() << endl;
  cout << "Michael : size : " << drawmode.attribute( "size" ).data() << endl;

  setPositionString( drawmode.attribute( "position" ) );
  setSizeString( drawmode.attribute( "size" ) );

  setPaperLayout( left, top, right, bottom, format, orientation );
  setHeadFootLine( hl, hm, hr, fl, fm, fr );

  if( m_posMode == Center ) cout << "Michael : immernoch centered" << endl;

  return true;
}

bool KImageDocument::completeLoading( KoStore* _store )
{
  kdebug( KDEBUG_INFO, 0, "KImageDocument::completeLoading()" );

  if( _store->open( "image.bmp", "" ) )
  {
    istorestream in( _store );
    in >> m_image;
    _store->close();
  }
  setModified( false );
  m_bEmpty = false;
  emit sigUpdateView();
  return true;
}

/*
void KImageDocument::print( QPaintDevice* _dev )
{
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
}
*/

/*
void KImageDocument::draw( QPaintDevice* _dev, long int _width, long int _height,
		      float _scale )
{
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
}
*/

void KImageDocument::paperLayoutDlg()
{
  KoPageLayout pl;
  pl.format = paperFormat();
  pl.orientation = orientation();
  pl.unit = PG_MM;
  pl.width = paperWidth();
  pl.height = paperHeight();
  pl.left = leftBorder();
  pl.right = rightBorder();
  pl.top = topBorder();
  pl.bottom = bottomBorder();

  KoHeadFoot hf;
  hf.headLeft = headLeft();
  hf.headRight = headRight();
  hf.headMid = headMid();
  hf.footLeft = footLeft();
  hf.footRight = footRight();
  hf.footMid = footMid();

  if ( !KoPageLayoutDia::pageLayout( pl, hf, FORMAT_AND_BORDERS | HEADER_AND_FOOTER ) )
    return;

  if ( pl.format == PG_CUSTOM )
  {
    m_paperWidth = pl.width;
    m_paperHeight = pl.height;
  }

  setPaperLayout( pl.left, pl.top, pl.right, pl.bottom, pl.format, pl.orientation );

  setHeadFootLine( hf.headLeft, hf.headMid, hf.headRight, hf.footLeft, hf.footMid, hf.footRight );

  emit sigUpdateView();
}

void KImageDocument::setHeadFootLine( const char *_headl, const char *_headm, const char *_headr,
				    const char *_footl, const char *_footm, const char *_footr )
{
  m_headLeft = _headl;
  m_headRight = _headr;
  m_headMid = _headm;
  m_footLeft = _footl;
  m_footRight = _footr;
  m_footMid = _footm;

  setModified( TRUE );
}

void KImageDocument::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
				const char * _paper, const char* _orientation )
{
    KoFormat f = paperFormat();
    KoOrientation o = orientation();

    if ( strcmp( "A3", _paper ) == 0L )
	f = PG_DIN_A3;
    else if ( strcmp( "A4", _paper ) == 0L )
	f = PG_DIN_A4;
    else if ( strcmp( "A5", _paper ) == 0L )
	f = PG_DIN_A5;
    else if ( strcmp( "B5", _paper ) == 0L )
	f = PG_DIN_B5;
    else if ( strcmp( "Executive", _paper ) == 0L )
	f = PG_US_EXECUTIVE;
    else if ( strcmp( "Letter", _paper ) == 0L )
	f = PG_US_LETTER;
    else if ( strcmp( "Legal", _paper ) == 0L )
	f = PG_US_LEGAL;
    else if ( strcmp( "Screen", _paper ) == 0L )
	f = PG_SCREEN;
    else if ( strcmp( "Custom", _paper ) == 0L )
    {
      f = PG_CUSTOM;
      QString tmp( _paper );
      m_paperWidth = atof( _paper );
      int i = tmp.find( 'x' );
      if ( i != -1 )
	m_paperHeight = atof( tmp.data() + i + 1 );
      if ( m_paperWidth < 10.0 )
	m_paperWidth = PG_A4_WIDTH;
      if ( m_paperHeight < 10.0 )
	m_paperWidth = PG_A4_HEIGHT;
    }

    if ( strcmp( "Portrait", _orientation ) == 0L )
	o = PG_PORTRAIT;
    else if ( strcmp( "Landscape", _orientation ) == 0L )
	o = PG_LANDSCAPE;

    setPaperLayout( _leftBorder, _topBorder, _rightBorder, _bottomBorder, f, o );
}

void KImageDocument::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
				   KoFormat _paper, KoOrientation _orientation )
{
  m_leftBorder = _leftBorder;
  m_rightBorder = _rightBorder;
  m_topBorder = _topBorder;
  m_bottomBorder = _bottomBorder;
  m_orientation = _orientation;
  m_paperFormat = _paper;

  calcPaperSize();

  setModified( TRUE );
}

QString KImageDocument::completeHeading( const char* _data,
				    int /* _page */, const char* /* _table */ )
{
/*
  QString page;

  page.sprintf( "%i", _page );

  QString f = m_strFileURL.data();

  if ( f.isNull() )
    f = "";

  QString n = "";

  if ( f != "" )
  {
    KURL u( f.data() );
    n = u.filename();
  }
*/

    QString t = QTime::currentTime().toString().copy();
    QString d = QDate::currentDate().toString().copy();

    QString tmp = _data;
    int pos = 0;
    // while ( ( pos = tmp.find( "<file>", pos ) ) != -1 )
    // tmp.replace( pos, 6, f.data() );/
    pos = 0;
    // while ( ( pos = tmp.find( "<name>", pos ) ) != -1 )
    //  tmp.replace( pos, 6, n.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<time>", pos ) ) != -1 )
	tmp.replace( pos, 6, t.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<date>", pos ) ) != -1 )
	tmp.replace( pos, 6, d.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<author>", pos ) ) != -1 )
	tmp.replace( pos, 8, "??" );
    pos = 0;
    while ( ( pos = tmp.find( "<email>", pos ) ) != -1 )
	tmp.replace( pos, 7, "??" );

    return QString( tmp.data() );
}

void KImageDocument::calcPaperSize()
{
    switch( m_paperFormat )
    {
    case PG_DIN_A5:
        m_paperWidth = PG_A5_WIDTH;
	m_paperHeight = PG_A5_HEIGHT;
	break;
    case PG_DIN_A4:
	m_paperWidth = PG_A4_WIDTH;
	m_paperHeight = PG_A4_HEIGHT;
	break;
    case PG_DIN_A3:
	m_paperWidth = PG_A3_WIDTH;
	m_paperHeight = PG_A3_HEIGHT;
	break;
    case PG_DIN_B5:
	m_paperWidth = PG_B5_WIDTH;
	m_paperHeight = PG_B5_HEIGHT;
	break;
    case PG_US_EXECUTIVE:
	m_paperWidth = PG_US_EXECUTIVE_WIDTH;
	m_paperHeight = PG_US_EXECUTIVE_HEIGHT;
	break;
    case PG_US_LETTER:
	m_paperWidth = PG_US_LETTER_WIDTH;
	m_paperHeight = PG_US_LETTER_HEIGHT;
	break;
    case PG_US_LEGAL:
	m_paperWidth = PG_US_LEGAL_WIDTH;
	m_paperHeight = PG_US_LEGAL_HEIGHT;
	break;
    case PG_SCREEN:
        m_paperWidth = PG_SCREEN_WIDTH;
        m_paperHeight = PG_SCREEN_HEIGHT;
    case PG_CUSTOM:
        return;
    }
}

QString KImageDocument::paperFormatString()
{
  QString paperFormatStr;

  switch( m_paperFormat )
  {
    case PG_DIN_A5:
	  paperFormatStr = "A5";
	  break;
    case PG_DIN_A4:
	  paperFormatStr = "A4";
	  break;
    case PG_DIN_A3:
	  paperFormatStr = "A3";
	  break;
    case PG_DIN_B5:
	  paperFormatStr = "B5";
	  break;
    case PG_US_EXECUTIVE:
	  paperFormatStr = "Executive";
	  break;
    case PG_US_LETTER:
	  paperFormatStr = "Letter";
	  break;
    case PG_US_LEGAL:
	  paperFormatStr = "Legal";
	  break;
    case PG_SCREEN:
	  paperFormatStr = "Screen";
	  break;
    case PG_CUSTOM:
      {
        QString tmp;
        tmp.sprintf( "%fx%f", m_paperWidth, m_paperHeight );
        paperFormatStr = tmp;
        break;
      }
    default :
      cout << "KImageDocument::paperFormatString() - unknown paper format type" << endl;

      paperFormatStr = "error";
      break;
  }
  return paperFormatStr;
}

void KImageDocument::setPaperFormatString( QString _format )
{
  if( _format == "A3" )
    m_paperFormat = PG_DIN_A3;
  else if( _format == "A4" )
    m_paperFormat = PG_DIN_A4;
  else if( _format == "A5" )
    m_paperFormat = PG_DIN_A5;
  else if( _format == "B5" )
    m_paperFormat = PG_DIN_B5;
  else if( _format == "EXECUTIVE" )
    m_paperFormat = PG_US_EXECUTIVE;
  else if( _format == "LETTER" )
    m_paperFormat = PG_US_LETTER;
  else if( _format == "LEGAL" )
    m_paperFormat = PG_US_LEGAL;
  else if( _format == "SCREEN" )
    m_paperFormat = PG_SCREEN;
  else // customized
  {
    m_paperFormat = PG_CUSTOM;

    // format : "%fx%f"

    // TODO : set the right values

    m_paperWidth = 10.0;
    m_paperHeight = 10.0;
  }
}

QString KImageDocument::orientationString()
{
  QString orientationStr;

  cout << "Orientation " << (int) m_orientation << endl;

  switch( m_orientation )
  {
    case PG_PORTRAIT :
      orientationStr = "Portrait";
      break;
    case PG_LANDSCAPE :
      orientationStr = "Landscape";
      break;
  default :
      cout << "KImageDocument::orientationString() - unknown orientation type" << endl;

      orientationStr = "error";
      break;
  }
  return orientationStr;
}

void KImageDocument::setOrientationString( QString _orient )
{
  if( _orient == "Portrait" )
    m_orientation = PG_PORTRAIT;
  else if( _orient == "Landscape" )
    m_orientation = PG_LANDSCAPE;
  else
  {
    cout << "KImageDocument::setOrientationString() - unknown orientation string" << endl;
    cout << "KImageDocument::setOrientationString() - using portrait" << endl;

    m_orientation = PG_PORTRAIT;
  }
}

void KImageDocument::transformImage( const QWMatrix& matrix )
{
  QPixmap pix, newpix;

  pix.convertFromImage( m_image );
  newpix = pix.xForm( matrix );
  m_image = newpix.convertToImage();
  emit sigUpdateView();
  setModified( true );
  m_bEmpty = false;

  kdebug( KDEBUG_INFO, 0, "Image manipulated with matrix" );
}

float KImageDocument::printableWidth()
{
  return m_paperWidth - m_leftBorder - m_rightBorder;
}

float KImageDocument::printableHeight()
{
  return m_paperHeight - m_topBorder - m_bottomBorder;
}

float KImageDocument::paperHeight()
{
  return m_paperHeight;
}

float KImageDocument::paperWidth()
{
  return m_paperWidth;
}

float KImageDocument::leftBorder()
{
  return m_leftBorder;
}

float KImageDocument::rightBorder()
{
  return m_rightBorder;
}

float KImageDocument::topBorder()
{
  return m_topBorder;
}

float KImageDocument::bottomBorder()
{
  return m_bottomBorder;
}

KoOrientation KImageDocument::orientation()
{
  return m_orientation;
}

KoFormat KImageDocument::paperFormat()
{
  return m_paperFormat;
}

QString KImageDocument::headLeft( int _p, const char* _t )
{
  if( m_headLeft.isNull() )
  {
    return "";
  }
  return completeHeading( m_headLeft.data(), _p, _t );
}

QString KImageDocument::headRight( int _p, const char* _t )
{
  if( m_headRight.isNull() )
  {
    return "";
  }
  return completeHeading( m_headRight.data(), _p, _t );
}

QString KImageDocument::headMid( int _p, const char* _t )
{
  if( m_headMid.isNull() )
  {
    return "";
  }
  return completeHeading( m_headMid.data(), _p, _t );
}

QString KImageDocument::footLeft( int _p, const char* _t )
{
  if( m_footLeft.isNull() )
  {
    return "";
  }
  return completeHeading( m_footLeft.data(), _p, _t );
}

QString KImageDocument::footMid( int _p, const char* _t )
{
  if( m_footMid.isNull() )
  {
    return "";
  }
  return completeHeading( m_footMid.data(), _p, _t );
}

QString KImageDocument::footRight( int _p, const char* _t )
{
  if( m_footRight.isNull() )
  {
    return "";
  }
  return completeHeading( m_footRight.data(), _p, _t );
}

QString KImageDocument::headLeft()
{
  if( m_headLeft.isNull() )
  {
    return "";
  }
  return m_headLeft.data();
}

QString KImageDocument::headMid()
{
  if( m_headMid.isNull() )
  {
    return "";
  }
  return m_headMid.data();
}

QString KImageDocument::headRight()
{
  if( m_headRight.isNull() )
  {
    return "";
  }
  return m_headRight.data();
}

QString KImageDocument::footLeft()
{
  if( m_footLeft.isNull() )
  {
    return "";
  }
  return m_footLeft.data();
}

QString KImageDocument::footMid()
{
  if( m_footMid.isNull() )
  {
    return "";
  }
  return m_footMid.data();
}

QString KImageDocument::footRight()
{
  if( m_footRight.isNull() )
  {
    return "";
  }
  return m_footRight.data();
}

const QImage& KImageDocument::image()
{
  return m_image;
}

#include "kimage_doc.moc"
