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

#include <kparts/partmanager.h>

#include <koDocumentChild.h>
#include <koDocument.h>
#include <koStore.h>
#include <koStream.h>
#include <koQueryTrader.h>

#include <qpainter.h>
#include <qapplication.h>

#include <kdebug.h>
#include <assert.h>

// Define the protocol used here for embedded documents' URL
// This used to "store:" but KURL didn't like it,
// so let's simply make it "tar:" !
#define STORE_PROTOCOL "tar:"
#define STORE_PROTOCOL_LENGTH 4
// Warning, keep it sync in koStore.cc

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
};

KoDocumentChild::KoDocumentChild( KoDocument* parent, KoDocument* doc, const QRect& geometry )
    : KoChild( parent )
{
  d = new KoDocumentChildPrivate;
  d->m_parent = parent;
  d->m_doc = doc;
  setGeometry( geometry );
}

KoDocumentChild::KoDocumentChild( KoDocument* parent )
    : KoChild( parent )
{
  d = new KoDocumentChildPrivate;
  d->m_parent = parent;
  d->m_doc = 0L;
}

void KoDocumentChild::setDocument( KoDocument *doc, const QRect &geometry )
{
  d->m_doc = doc;
  setGeometry( geometry );

  updateMatrix();
}

KoDocument *KoDocumentChild::document() const
{
  return d->m_doc;
}

KoDocument *KoDocumentChild::parentDocument() const
{
  return d->m_parent;
}

KoDocument *KoDocumentChild::hitTest( const QPoint &p, const QWMatrix &_matrix )
{
  if ( !region( _matrix ).contains( p ) || !document() )
    return 0L;

  QWMatrix m( _matrix );
  m = matrix() * m;
  m.scale( xScaling(), yScaling() );

  return document()->hitTest( p, m );
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
            int x, y, w, h;
            x=y=w=h=0;
            if ( e.hasAttribute( "x" ) )
                x = e.attribute( "x" ).toInt(&brect);
            if ( e.hasAttribute( "y" ) )
                y = e.attribute( "y" ).toInt(&brect);
            if ( e.hasAttribute( "w" ) )
                w = e.attribute( "w" ).toInt(&brect);
            if ( e.hasAttribute( "h" ) )
                h = e.attribute( "h" ).toInt(&brect);
            m_tmpGeometry = QRect(x, y, w, h);
        }
    }

    if ( !brect )
    {
        kdDebug(30003) << "Missing RECT in OBJECT" << endl;
        return false;
    }

    return true;
}

bool KoDocumentChild::loadTag( KOMLParser&, const QString&, QValueList<KOMLAttrib>& )
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

  // see KoDocument::insertChild for an exaplanation what's going on
  // now :-)
  if ( parentDocument() )
  {
      KoDocument *parent = parentDocument();

      if ( parent->manager() && parent->manager()->parts() )
      {
          KParts::PartManager *manager = parent->manager();

          if ( !manager->parts()->containsRef( document() ) &&
               !parent->isSingleViewMode() )
              manager->addPart( document(), false );
      }

  }

  return res;
}

QDomElement KoDocumentChild::save( QDomDocument& doc )
{
    assert( document() );
    QDomElement e = doc.createElement( "object" );
    e.setAttribute( "url", document()->url().url() );
    e.setAttribute( "mime", document()->nativeFormatMimeType() );
    QDomElement rect = doc.createElement( "rect" );
    rect.setAttribute( "x", geometry().left() );
    rect.setAttribute( "y", geometry().top() );
    rect.setAttribute( "w", geometry().width() );
    rect.setAttribute( "h", geometry().height() );
    e.appendChild(rect);
    return e;
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
  if ( d->m_doc ) {
    delete d->m_doc;
    d->m_doc=0L;
  }
  delete d;
}

#include <koDocumentChild.moc>
