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

#include <fstream>

#include "koDocument.h"
#include "koDocumentChild.h"
#include "koView.h"
#include "koApplication.h"
#include "koStream.h"
#include "koQueryTypes.h"
#include "koFilterManager.h"

#include <koStore.h>
#include <koBinaryStore.h>
#include <koTarStore.h>
#include <koStoreStream.h>

#include <komlWriter.h>
#include <komlMime.h>
#include <komlStreamFeed.h>

#include <klocale.h>
#include <kapp.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <qpainter.h>
#include <qcolor.h>
#include <qpicture.h>
#include <qdom.h>

// Define the protocol used here for embedded documents' URL
// This used to "store:" but KURL didn't like it,
// so let's simply make it "tar:" !
#define STORE_PROTOCOL "tar:"
#define STORE_PROTOCOL_LENGTH 4
// Warning, keep it sync in koTarStore.cc

using namespace std;

/**********************************************************
 *
 * KoDocument
 *
 **********************************************************/

class KoDocumentPrivate
{
public:
  KoDocumentPrivate()
  {
    m_children.setAutoDelete( true );
  }
  ~KoDocumentPrivate()
  {
  }

  QList<KoView> m_views;
  QList<KoDocumentChild> m_children;

  bool m_bSingleViewMode;
};

KoDocument::KoDocument( QObject* parent, const char* name, bool singleViewMode )
    : KParts::ReadWritePart( parent, name )
{
    d = new KoDocumentPrivate;
    m_bEmpty = TRUE;

    d->m_bSingleViewMode = singleViewMode;

    // the parent setting *always* overrides! (Simon)
    if ( parent )
    {
      if ( parent->inherits( "KoDocument" ) )
        d->m_bSingleViewMode = ((KoDocument *)parent)->singleViewMode();
      else if ( parent->inherits( "KParts::Part" ) )
        d->m_bSingleViewMode = true;
    }
}

KoDocument::~KoDocument()
{
  delete d;
}

bool KoDocument::singleViewMode() const
{
  return d->m_bSingleViewMode;
}

bool KoDocument::saveFile()
{
  if ( !kapp->inherits( "KoApplication" ) )
    return false;

  return saveToURL( KURL( m_file ), nativeFormatMimeType( instance() ) );
}

bool KoDocument::openFile()
{
  return loadFromURL( KURL( m_file ) );
}

QWidget *KoDocument::widget()
{
  if ( !d->m_bSingleViewMode )
    return 0L;

  if ( d->m_views.count() == 0 )
  {
    QWidget *parentWidget = 0L;

    if ( parent() )
    {
      if ( parent()->inherits( "QWidget" ) )
        parentWidget = (QWidget *)parent();
      else if ( parent()->inherits( "KoDocument" ) )
      {
        KoDocument *parentDoc = (KoDocument *)parent();
	if ( parentDoc->singleViewMode() )
	  parentWidget = parentDoc->widget();
      }
    }

    QWidget *w = createView( parentWidget );
    assert( w );
    setWidget( w );
  }

  return d->m_views.getFirst();
}

QAction *KoDocument::action( const QDomElement &element )
{
  return d->m_views.getFirst()->action( element );
}

QDomDocument KoDocument::document() const
{
  return d->m_views.getFirst()->document();
}

void KoDocument::setManager( KParts::PartManager *manager )
{
  KParts::ReadWritePart::setManager( manager );
  if ( d->m_bSingleViewMode && d->m_views.count() == 1 )
    d->m_views.getFirst()->setPartManager( manager );
}

void KoDocument::setReadWrite( bool readwrite )
{
  KParts::ReadWritePart::setReadWrite( readwrite );

  QListIterator<KoView> vIt( d->m_views );
  for (; vIt.current(); ++vIt )
    vIt.current()->updateReadWrite( readwrite );

  QListIterator<KoDocumentChild> dIt( d->m_children );
  for (; dIt.current(); ++dIt )
    if ( dIt.current()->document() )
      dIt.current()->document()->setReadWrite( readwrite );
}

void KoDocument::addView( KoView *view )
{
  if ( !view )
    return;

  d->m_views.append( view );

  connect( view, SIGNAL( destroyed() ),
	   this, SLOT( slotViewDestroyed() ) );

  view->updateReadWrite( isReadWrite() );
}

KoView *KoDocument::firstView()
{
  return d->m_views.first();
}

KoView *KoDocument::nextView()
{
  return d->m_views.next();
}

void KoDocument::slotViewDestroyed()
{
  d->m_views.removeRef( (KoView *)sender() );
}

void KoDocument::insertChild( KoDocumentChild *child )
{
  setModified( true );

  d->m_children.append( child );

  connect( child, SIGNAL( changed( KoDocumentChild * ) ),
	   this, SIGNAL( childChanged( KoDocumentChild * ) ) );
}

QList<KoDocumentChild> &KoDocument::children() const
{
  return d->m_children;
}

KParts::Part *KoDocument::hitTest( QWidget *widget, const QPoint &globalPos )
{
  QListIterator<KoView> it( d->m_views );
  for (; it.current(); ++it )
    if ( (QWidget *)it.current() == widget )
    {
      QPoint canvasPos( it.current()->canvas()->mapFromGlobal( globalPos ) );
      canvasPos.rx() -= it.current()->canvasXOffset();
      canvasPos.ry() -= it.current()->canvasYOffset();

      KParts::Part *part = it.current()->hitTest( canvasPos );
      if ( part )
        return part;
    }

  return 0L;
}

KoDocument *KoDocument::hitTest( const QPoint &pos, const QWMatrix &matrix )
{
  QListIterator<KoDocumentChild> it( d->m_children );
  for (; it.current(); ++it )
  {
    KoDocument *doc = it.current()->hitTest( pos, matrix );
    if ( doc )
      return doc;
  }

  return this;
}

KoDocumentChild *KoDocument::child( KoDocument *doc )
{
  QListIterator<KoDocumentChild> it( d->m_children );
  for (; it.current(); ++it )
    if ( it.current()->document() == doc )
      return it.current();

  return 0L;
}

void KoDocument::paintEverything( QPainter &painter, const QRect &rect, bool transparent, KoView *view )
{
  paintContent( painter, rect, transparent );
  paintChildren( painter, rect, view );
}

void KoDocument::paintChildren( QPainter &painter, const QRect &/*rect*/, KoView *view )
{
  QListIterator<KoDocumentChild> it( d->m_children );
  for (; it.current(); ++it )
  {
    // #### todo: paint only if child is visible inside rect
    painter.save();
    paintChild( it.current(), painter, view );
    painter.restore();
  }
}

void KoDocument::paintChild( KoDocumentChild *child, QPainter &painter, KoView *view )
{
  QRegion rgn = painter.clipRegion();

  child->transform( painter );
  child->document()->paintEverything( painter, child->contentRect(), child->isTransparent(), view );

  if ( view && view->partManager() )
  {
    KParts::PartManager *manager = view->partManager();

    painter.scale( 1.0 / child->xScaling(), 1.0 / child->yScaling() );

    int w = int( (double)child->contentRect().width() * child->xScaling() );
    int h = int( (double)child->contentRect().height() * child->yScaling() );
    if ( ( manager->selectedPart() == (KParts::Part *)child->document() &&
	   manager->selectedWidget() == (QWidget *)view ) ||
	 ( manager->activePart() == (KParts::Part *)child->document() &&
	   manager->activeWidget() == (QWidget *)view ) )
        {
	  painter.setClipRegion( rgn );

	  painter.setPen( black );
	  painter.fillRect( -5, -5, w + 10, 5, white );
	  painter.fillRect( -5, h, w + 10, 5, white );
	  painter.fillRect( -5, -5, 5, h + 10, white );
	  painter.fillRect( w, -5, 5, h + 10, white );
	  painter.fillRect( -5, -5, w + 10, 5, BDiagPattern );
	  painter.fillRect( -5, h, w + 10, 5, BDiagPattern );		
	  painter.fillRect( -5, -5, 5, h + 10, BDiagPattern );
	  painter.fillRect( w, -5, 5, h + 10, BDiagPattern );
	
	  if ( manager->selectedPart() == (KParts::Part *)child->document() &&
	       manager->selectedWidget() == (QWidget *)view )
	  {
	    QColor color;
	    if ( view->koDocument() == this )
	      color = black;
	    else
	      color = gray;
	    painter.fillRect( -5, -5, 5, 5, color );
	    painter.fillRect( -5, h, 5, 5, color );
	    painter.fillRect( w, h, 5, 5, color );
	    painter.fillRect( w, -5, 5, 5, color );
	    painter.fillRect( w / 2 - 3, -5, 5, 5, color );
	    painter.fillRect( w / 2 - 3, h, 5, 5, color );
	    painter.fillRect( -5, h / 2 - 3, 5, 5, color );
	    painter.fillRect( w, h / 2 - 3, 5, 5, color );
	  }
      }
  }
}

bool KoDocument::saveChildren( KoStore* /*_store*/, const char */*_path*/ )
{
  // Lets assume that we do not have children
  kDebugWarning( 30003, "KoDocument::saveChildren( KoStore*, const char * )");
  kDebugWarning( 30003, "Not implemented ( not really an error )" );
  return true;
}

bool KoDocument::saveToURL( const KURL &url, const QCString &_format )
{
  if ( url.isMalformed() )
  {
    kDebugInfo( 30003, "malformed URL" );
    return false;
  }

  if ( !url.isLocalFile() )
  {
    KMessageBox::error( 0L, i18n( "Can not save to remote URL (not implemented yet)" ) );
    return false;
  }

  if ( hasToWriteMultipart() )
  {
    kDebugInfo( 30003, "Saving to store" );

    //Use this to save to a binary store (deprecated)
    //KoStore * store = new KoBinaryStore ( url.path(), KOStore::Write );

    KoStore* store = new KoTarStore( url.path(), KoStore::Write );

    // Save childen first since they might get a new url
    if ( store->bad() || !saveChildren( store, STORE_PROTOCOL ) )
    {
      delete store;
      return false;
    }

    kDebugInfo( 30003, "Saving root" );
    if ( store->open( "root", _format ) )
    {
      ostorestream out( store );
      if ( !save( out, _format ) )
      {
	store->close();
	return false;
      }
      out.flush();
      store->close();
    }
    else
      return false;

    bool ret = completeSaving( store );
    kDebugInfo( 30003, "Saving done" );
    delete store;
    return ret;
  }
  else
  {
    ofstream out( url.path() );
    if ( !out )
    {
      KMessageBox::error( 0L, i18n("Could not write to\n%1" ).arg(url.path()) );
      return false;
    }

    return save( out, _format );
  }
}

bool KoDocument::saveToStore( KoStore* _store, const QCString & _format, const QString & _path )
{
  kDebugInfo( 30003, "Saving document to store" );

  // Use the path as the internal url
  setURL( _path );

  // Save childen first since they might get a new url
  if ( !saveChildren( _store, _path ) )
    return false;

  QString u = url().url();
  if ( _store->open( u, _format ) )
  {
    ostorestream out( _store );
    if ( !save( out, _format ) )
      return false;
    out.flush();
    _store->close();
  }

  if ( !completeSaving( _store ) )
    return false;

  kDebugInfo( 30003, "Saved document to store" );

  return true;
}

bool KoDocument::loadFromURL( const KURL & url )
{
  kDebugInfo( 30003, QString("KoDocument::loadFromURL( %1 )").arg(url.url()) );
  // TODO : assert local url
  QString file = url.path();

  QApplication::setOverrideCursor( waitCursor );


  // Launch a filter if we need one for this url ?
  QString importedFile = KoFilterManager::self()->import( file, nativeFormatMimeType( instance() ) );

  // The filter, if any, has been applied. It's all native format now.
  bool loadOk = (!importedFile.isEmpty()) &&        // Empty = an error occured in the filter
    loadNativeFormat( importedFile );

  if (!loadOk)
    KMessageBox::error( 0L, i18n( "Could not open\n%1" ).arg(importedFile) );

  if ( importedFile != file )
  {
    // We opened a temporary file (result of an import filter)
    // Set document URL to empty - we don't want to save in /tmp !
    setURL(KURL());
    // and remove temp file
    unlink( importedFile.ascii() );
  }
  QApplication::restoreOverrideCursor();
  return loadOk;
}

bool KoDocument::loadNativeFormat( const KURL & url )
{
  QApplication::setOverrideCursor( waitCursor );

  kDebugInfo( 30003, QString("KoDocument::loadNativeFormat( %1 )").arg(url.url()) );

  if ( url.isMalformed() )
  {
    kDebugWarning( 30003, QString("Malformed URL %1").arg(url.url()) );
    QApplication::restoreOverrideCursor();
    return false;
  }

  if ( !url.isLocalFile() )
  {
    kDebugInfo( 30003, "Can not load remote URL (not implemented yet)" );
    QApplication::restoreOverrideCursor();
    return false;
  }

  ifstream in( url.path() );
  if ( !in )
  {
    kDebugWarning( 30003, QString("Could not open %1").arg(url.path()) );
    QApplication::restoreOverrideCursor();
    return false;
  }

  // Try to find out whether it is a mime multi part file
  char buf[5];
  in.get( buf[0] ); in.get( buf[1] ); in.get( buf[2] ); in.get( buf[3] ); buf[4] = 0;
  in.unget(); in.unget(); in.unget(); in.unget();

  //kDebugInfo( 30003, "PATTERN=%s", buf );

  // Store the URL as the Document URL
  setURL( url );

  // Is it plain XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    bool res = load( in, 0L );
    in.close();
    if ( res )
      res = completeLoading( 0L );

    QApplication::restoreOverrideCursor();
    return res;
  } else
  { // It's a koffice store (binary or tar.gz)
    in.close();
    KoStore * store;
    if ( strncasecmp( buf, "KS01", 4 ) == 0 )
    {
      store = new KoBinaryStore( url.path(), KoStore::Read );
    }
    else // new (tar.gz)
    {
      store = new KoTarStore( url.path(), KoStore::Read );
    }

    if ( store->bad() )
    {
      delete store;
      QApplication::restoreOverrideCursor();
      return false;
    }

    if ( store->open( "root", "" ) )
    {
      istorestream in( store );
      if ( !load( in, store ) )
      {
        delete store;
        QApplication::restoreOverrideCursor();
        return false;
      }
      store->close();
    }

    if ( !loadChildren( store ) )
    {	
      kDebugInfo( 30003, "ERROR: Could not load children" );
      delete store;
      QApplication::restoreOverrideCursor();
      return false;
    }

    bool res = completeLoading( store );
    delete store;
    QApplication::restoreOverrideCursor();
    return res;
  }
}

bool KoDocument::loadFromStore( KoStore* _store, const KURL & url )
{
  if ( _store->open( url.url(), "" ) )
  {
    istorestream in( _store );
    if ( !load( in, _store ) )
      return false;
    _store->close();
  }
  // Store as document URL
  setURL( url );

  if ( !loadChildren( _store ) )
  {	
    kDebugInfo( 30003, "ERROR: Could not load children" );
    return false;
  }

  return completeLoading( _store );
}

bool KoDocument::load( istream& in, KoStore* _store )
{
  kDebugInfo( 30003, "KoDocument::load( istream& in, KoStore* _store )");
  // Try to find out whether it is a mime multi part file
  char buf[5];
  in.get( buf[0] ); in.get( buf[1] ); in.get( buf[2] ); in.get( buf[3] ); buf[4] = 0;
  in.unget(); in.unget(); in.unget(); in.unget();

  kDebugInfo( 30003, "PATTERN2=%s", buf );

  // Load XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    KOMLStreamFeed feed( in );
    KOMLParser parser( &feed );

    if ( !loadXML( parser, _store ) )
      return false;
  }
  // Load binary data
  else
  {
    if ( !loadBinary( in, false, _store ) )
      return false;
  }

  return true;
}

bool KoDocument::isStoredExtern()
{
  return ( m_strURL.protocol() != STORE_PROTOCOL );
}

void KoDocument::setModified( bool _mod )
{
    KParts::ReadWritePart::setModified( _mod );

    if ( _mod )
	m_bEmpty = FALSE;
}

bool KoDocument::isEmpty() const
{
    return m_bEmpty;
}

void KoDocument::setURL( const KURL& url )
{
    m_strURL = url;
}

const KURL & KoDocument::url() const
{
    return m_strURL;
}

bool KoDocument::loadBinary( istream& , bool, KoStore* )
{
    kDebugError( 30003, "KoDocument::loadBinary not implemented" );
    return false;
}

bool KoDocument::loadXML( KOMLParser&, KoStore*  )
{
    kDebugError( 30003, "KoDocument::loadXML not implemented" );
    return false;
}

bool KoDocument::loadChildren( KoStore* )
{
    return true;
}

bool KoDocument::completeLoading( KoStore* )
{
    return true;
}

bool KoDocument::completeSaving( KoStore* )
{
    return true;
}

bool KoDocument::save( ostream&, const char* )
{
    kDebugError( 30003, "KoDocument::save not implemented" );
    return false;
}

QString KoDocument::copyright() const
{
    return "";
}

QString KoDocument::comment() const
{
    return "";
}

bool KoDocument::hasToWriteMultipart()
{
    return FALSE;
}

QCString KoDocument::nativeFormatMimeType( KInstance *instance )
{
  QString instname = kapp->instanceName();
  if ( instance )
    instname = instance->instanceName();
  
  KService::Ptr service = KService::service( instname );
  
  if ( !service )
    return QCString();
  
  KDesktopFile deFile( service->desktopEntryPath(), true );
  
  return deFile.readEntry( "MimeType" ).utf8(); // ??????????
} 

#include "koDocument.moc"
