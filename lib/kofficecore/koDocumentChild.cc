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

#include "koDocumentChild.h"
#include "koDocument.h"
#include "koStore.h"
#include "koStream.h"
#include "koQueryTypes.h"

#include <qpainter.h>
#include <qapplication.h>

#include <kdebug.h>
#include <cassert>

using namespace std;

// Define the protocol used here for embedded documents' URL
// This used to "store:" but KURL didn't like it,
// so let's simply make it "tar:" !
#define STORE_PROTOCOL "tar:"
#define STORE_PROTOCOL_LENGTH 4
// Warning, keep it sync in koTarStore.cc

/**********************************************************
 *
 * KoDocumentChild
 *
 **********************************************************/

class KoDocumentChildPrivate
{
public:
  KoDocumentChildPrivate()
  {
  }
  ~KoDocumentChildPrivate()
  {
  }

  KoDocument *m_parent;
  KoDocument *m_doc;
  QRect m_geometry;

  double m_rotation;
  double m_shearX;
  double m_shearY;
  QPoint m_rotationPoint;
  double m_scaleX;
  double m_scaleY;
  QWMatrix m_matrix;
  bool m_lock;
  QPointArray m_old;
  bool m_transparent;
};

KoDocumentChild::KoDocumentChild( KoDocument* parent, KoDocument* doc, const QRect& geometry )
    : QObject( parent )
{
  d = new KoDocumentChildPrivate;
  d->m_parent = parent;
  d->m_doc = doc;
  d->m_geometry = geometry;

  init();
}

KoDocumentChild::KoDocumentChild( KoDocument* parent )
    : QObject( parent )
{
  d = new KoDocumentChildPrivate;
  d->m_parent = parent;
  d->m_doc = 0L;

  init();
}

void KoDocumentChild::init()
{
  d->m_scaleX = d->m_scaleY = 1.0;
  d->m_shearX = d->m_shearY = 0.0;
  d->m_rotation = 0.0;
  d->m_lock = false;
  d->m_transparent = false;

  updateMatrix();
}

void KoDocumentChild::setGeometry( const QRect &rect )
{
  if ( !d->m_lock )
    d->m_old = framePointArray();

  d->m_geometry = rect;

  updateMatrix();

  if ( !d->m_lock )
    emit changed( this );
}

void KoDocumentChild::setDocument( KoDocument *doc, const QRect &geometry )
{
  d->m_doc = doc;
  d->m_geometry = geometry; //hmmm, perhaps we should call setGeometry()  (Simon)

  updateMatrix();
}

QRect KoDocumentChild::geometry() const
{
  return d->m_geometry;
}

KoDocument *KoDocumentChild::document() const
{
  return d->m_doc;
}

KoDocument *KoDocumentChild::parentDocument() const
{
  return d->m_parent;
}

QRegion KoDocumentChild::region( const QWMatrix &matrix ) const
{
  return QRegion( pointArray( matrix ) );
}

QPointArray KoDocumentChild::pointArray( const QWMatrix &matrix ) const
{
  return pointArray( QRect( 0, 0, d->m_geometry.width(), d->m_geometry.height() ), matrix );
}

bool KoDocumentChild::contains( const QPoint &point ) const
{
  return region().contains( point );
}

QRect KoDocumentChild::boundingRect( const QWMatrix &matrix ) const
{
  return pointArray( matrix ).boundingRect();
}

bool KoDocumentChild::isRectangle() const
{
  return !( d->m_shearX != 0.0 || d->m_shearY != 0.0 || d->m_rotation != 0.0 );
}

void KoDocumentChild::setClipRegion( QPainter &painter, bool combine )
{
  painter.setClipping( true );
  if ( combine && !painter.clipRegion().isEmpty() )
    painter.setClipRegion( region( painter.worldMatrix() ).intersect( painter.clipRegion() ) );
  else
    painter.setClipRegion( region( painter.worldMatrix() ) );
}

void KoDocumentChild::setScaling( double x, double y )
{
  if ( !d->m_lock )
    d->m_old = framePointArray();

  d->m_scaleX = x;
  d->m_scaleY = y;

  // why is that commented out? (Simon)
  //updateMatrix()

  if ( !d->m_lock )
    emit changed( this );
}

double KoDocumentChild::xScaling() const
{
  return d->m_scaleX;
}

double KoDocumentChild::yScaling() const
{
  return d->m_scaleY;
}

void KoDocumentChild::setShearing( double x, double y )
{
  if ( !d->m_lock )
    d->m_old = framePointArray();

  d->m_shearX = x;
  d->m_shearY = y;

  updateMatrix();

  if ( !d->m_lock )
    emit changed( this );
}

double KoDocumentChild::xShearing() const
{
  return d->m_shearX;
}

double KoDocumentChild::yShearing() const
{
  return d->m_shearY;
}

void KoDocumentChild::setRotation( double rot )
{
  if ( !d->m_lock )
    d->m_old = framePointArray();

  d->m_rotation = rot;
  updateMatrix();

  if ( !d->m_lock )
    emit changed( this );
}

double KoDocumentChild::rotation() const
{
  return d->m_rotation;
}

void KoDocumentChild::setRotationPoint( const QPoint &pos )
{
  if ( !d->m_lock )
    d->m_old = framePointArray();

  d->m_rotationPoint = pos;
  updateMatrix();

  if ( !d->m_lock )
    emit changed( this );
}

QPoint KoDocumentChild::rotationPoint() const
{
  return d->m_rotationPoint;
}

void KoDocumentChild::transform( QPainter &painter )
{
  setClipRegion( painter, true );
  QWMatrix m = painter.worldMatrix();
  m = d->m_matrix * m;
  m.scale( d->m_scaleX, d->m_scaleY );
  painter.setWorldMatrix( m );
}

QRect KoDocumentChild::contentRect() const
{
  return QRect( 0, 0, d->m_geometry.width() / d->m_scaleX, d->m_geometry.height() / d->m_scaleY );
}

KoDocument *KoDocumentChild::hitTest( const QPoint &p, const QWMatrix &matrix )
{
  if ( !region( matrix ).contains( p ) || !document() )
    return 0L;

  QWMatrix m( matrix );
  m = d->m_matrix * m;
  m.scale( d->m_scaleX, d->m_scaleY );

  return document()->hitTest( p, m );
}

QPointArray KoDocumentChild::framePointArray( const QWMatrix &matrix ) const
{
  return pointArray( QRect( -6, -6, d->m_geometry.width() + 12, d->m_geometry.height() + 12 ), matrix );
}

QRegion KoDocumentChild::frameRegion( const QWMatrix &matrix, bool solid ) const
{
  QPointArray arr = framePointArray( matrix );

  if ( solid )
    return QRegion( arr );

  return QRegion( arr ).subtract( region( matrix ) );
}

KoDocumentChild::Gadget KoDocumentChild::gadgetHitTest( const QPoint &p, const QWMatrix &matrix )
{
  if ( !frameRegion( matrix ).contains( p ) )
    return NoGadget;

  if ( QRegion( pointArray( QRect( -5, -5, 5, 5 ), matrix ) ).contains( p ) )
      return TopLeft;
  if ( QRegion( pointArray( QRect( d->m_geometry.width() / 2 - 3, -5, 5, 5 ), matrix ) ).contains( p ) )
      return TopMid;
  if ( QRegion( pointArray( QRect( d->m_geometry.width(), -5, 5, 5 ), matrix ) ).contains( p ) )
      return TopRight;
  if ( QRegion( pointArray( QRect( -5, d->m_geometry.height() / 2 - 3, 5, 5 ), matrix ) ).contains( p ) )
      return MidLeft;
  if ( QRegion( pointArray( QRect( -5, d->m_geometry.height(), 5, 5 ), matrix ) ).contains( p ) )
      return BottomLeft;
  if ( QRegion( pointArray( QRect( d->m_geometry.width() / 2 - 3,
				   d->m_geometry.height(), 5, 5 ), matrix ) ).contains( p ) )
    return BottomMid;
  if ( QRegion( pointArray( QRect( d->m_geometry.width(), d->m_geometry.height(), 5, 5 ), matrix ) ).contains( p ) )
      return BottomRight;
  if ( QRegion( pointArray( QRect( d->m_geometry.width(),
				   d->m_geometry.height() / 2 - 3, 5, 5 ), matrix ) ).contains( p ) )
    return MidRight;

  return Move;
}

QPointArray KoDocumentChild::pointArray( const QRect &r, const QWMatrix &matrix ) const
{
  QPoint topleft = d->m_matrix.map( QPoint( r.left(), r.top() ) );
  QPoint topright = d->m_matrix.map( QPoint( r.right(), r.top() ) );
  QPoint bottomleft = d->m_matrix.map( QPoint( r.left(), r.bottom() ) );
  QPoint bottomright = d->m_matrix.map( QPoint( r.right(), r.bottom() ) );

  QPointArray arr( 4 );
  arr.setPoint( 0, topleft );
  arr.setPoint( 1, topright );
  arr.setPoint( 2, bottomright );
  arr.setPoint( 3, bottomleft );

  for( int i = 0; i < 4; ++i )
      arr.setPoint( i, matrix.map( arr.point( i ) ) );

  return arr;
}

void KoDocumentChild::updateMatrix()
{
  QWMatrix r;
  r.rotate( - d->m_rotation );
  QPoint p = r.map( QPoint( d->m_rotationPoint.x(),
			    d->m_rotationPoint.y() ) );

  QWMatrix m;
  m.rotate( d->m_rotation );
  m.translate( -d->m_rotationPoint.x() + d->m_geometry.x(), -d->m_rotationPoint.y() + d->m_geometry.y() );
  m.translate( p.x(), p.y() );
  m.shear( d->m_shearX, d->m_shearY );

  d->m_matrix = m;
}

QWMatrix KoDocumentChild::matrix() const
{
  return d->m_matrix;
}

void KoDocumentChild::lock()
{
  if ( d->m_lock )
    return;

  d->m_old = framePointArray();
  d->m_lock = true;
}

void KoDocumentChild::unlock()
{
  if ( !d->m_lock )
    return;

  d->m_lock = false;
  emit changed( this );
}

QPointArray KoDocumentChild::oldPointArray( const QWMatrix &matrix )
{
  QPointArray arr = d->m_old;

  for( int i = 0; i < 4; ++i )
      arr.setPoint( i, matrix.map( arr.point( i ) ) );

  return arr;
}

void KoDocumentChild::setTransparent( bool transparent )
{
  d->m_transparent = transparent;
}

bool KoDocumentChild::isTransparent() const
{
  return d->m_transparent;
}

bool KoDocumentChild::load( KOMLParser& parser, vector<KOMLAttrib>& _attribs )
{
  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "url" )
    {
      m_tmpURL = (*it).m_strValue.c_str();
    }
    else if ( (*it).m_strName == "mime" )
    {
      m_tmpMimeType = (*it).m_strValue.c_str();
    }
    else
      kdDebug(30003) << "Unknown attrib 'OBJECT:" << (*it).m_strName.c_str() << "'" << endl;
  }

  if ( m_tmpURL.isEmpty() )
  {	
    kdDebug(30003) << "Empty 'url' attribute in OBJECT" << endl;
    return false;
  }
  else if ( m_tmpMimeType.isEmpty() )
  {
    kdDebug(30003) << "Empty 'mime' attribute in OBJECT" << endl;
    return false;
  }

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  bool brect = false;

  // RECT
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    if ( name == "RECT" )
    {
      brect = true;
      m_tmpGeometry = tagToRect( lst );
      setGeometry( m_tmpGeometry );
    }
    else
      kdDebug(30003) << "Unknown tag '" << tag.c_str() << "' in OBJECT" << endl;

    if ( !parser.close( tag ) )
    {
      kdDebug(30003) << "ERR: Closing Child in OBJECT" << endl;
      return false;
    }
  }

  if ( !brect )
  {
    kdDebug(30003) << "Missing RECT in OBJECT" << endl;
    return false;
  }

  return true;
}

bool KoDocumentChild::load( const QDomElement& element )
{
    if ( element.hasAttribute( "url" ) )
	m_tmpURL = element.attribute("url");
    if ( element.hasAttribute("mime") )
	m_tmpMimeType = element.attribute("mime");

    if ( m_tmpURL.isEmpty() )
    {	
	kdDebug(30003) << "Empty 'url' attribute in OBJECT" << endl;
	return false;
    }
    if ( m_tmpMimeType.isEmpty() )
    {
	kdDebug(30003) << "Empty 'mime' attribute in OBJECT" << endl;
	return false;
    }

    bool brect = FALSE;
    QDomElement e = element.firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if ( e.tagName() == "rect" )
        {
	    brect = true;
	    m_tmpGeometry = e.toRect();
	}
    }

    if ( !brect )
    {
	kdDebug(30003) << "Missing RECT in OBJECT" << endl;
	return false;
    }

    return true;
}

bool KoDocumentChild::loadTag( KOMLParser&, const string&, vector<KOMLAttrib>& )
{
    return FALSE;
}

bool KoDocumentChild::loadDocument( KoStore* _store )
{
  assert( !m_tmpURL.isEmpty() );

  kdDebug(30003) << "Trying to load " << m_tmpURL << endl;

  KoDocumentEntry e = KoDocumentEntry::queryByMimeType( m_tmpMimeType );
  if ( e.isEmpty() )
  {
    kdDebug(30003) << "ERROR: Could not create child document" << endl;
    return false;
  }

  KoDocument * doc = e.createDoc( (KoDocument*)parent() );
  if (!doc)
      return false;
  setDocument( doc, m_tmpGeometry );

  bool res;
  if ( m_tmpURL.left( STORE_PROTOCOL_LENGTH ) == STORE_PROTOCOL )
      res = document()->loadFromStore( _store, m_tmpURL );
  else
  {
      // Reference to an external document. Hmmm...
      res = document()->openURL( m_tmpURL );
      // Still waiting...
      QApplication::setOverrideCursor( waitCursor );
  }

  m_tmpURL = QString::null;

  return res;
}

QDomElement KoDocumentChild::save( QDomDocument& doc )
{
    assert( document() );
    QDomElement e = doc.createElement( "object" );
    e.setAttribute( "url", document()->url().url() );
    e.setAttribute( "mime", document()->mimeType() );
    QDomElement rect = doc.createElement( "rect" );
    rect.setAttribute( "x", geometry().left() );
    rect.setAttribute( "y", geometry().top() );
    rect.setAttribute( "w", geometry().width() );
    rect.setAttribute( "h", geometry().height() );
    e.appendChild(rect);
    return e;
}

bool KoDocumentChild::save( ostream& out )
{
  assert( document() );
  QString u = document()->url().url();
  QString mime = document()->mimeType();

  out << indent << "<OBJECT url=\"" << u.ascii() << "\" mime=\"" << mime.ascii() << "\">"
      << geometry() << "</OBJECT>" << endl;

  return true;
}

bool KoDocumentChild::isStoredExtern()
{
  const KURL & url = document()->url();
  if ( !url.hasPath() )
    return false;
  if ( url.protocol() == STORE_PROTOCOL )
    return false;

  return true;
}

KURL KoDocumentChild::url()
{
    return ( document() ? document()->url() : KURL() );
}

KoDocumentChild::~KoDocumentChild()
{
  if ( d->m_doc )
    delete d->m_doc;

  delete d;
}
