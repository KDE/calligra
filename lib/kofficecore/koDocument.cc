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

#include <config.h>

#include <assert.h>
#include <unistd.h>

#include <koDocument.h>
#include <koDocument_p.h>
#include <KoDocumentIface.h>
#include <koDocumentChild.h>
#include <koView.h>
#include <koApplication.h>
#include <koMainWindow.h>
#include <koStoreDevice.h>
#include <koQueryTrader.h>
#include <koFilterManager.h>
#include <koDocumentInfo.h>

#include <koStore.h>
#include <kio/netaccess.h>

#include <klocale.h>
#include <kmimetype.h>
#include <kapp.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpicture.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qtimer.h>

// Define the protocol used here for embedded documents' URL
// This used to "store:" but KURL didn't like it,
// so let's simply make it "tar:" !
#define STORE_PROTOCOL "tar:"
#define STORE_PROTOCOL_LENGTH 4
// Warning, keep it sync in koStore.cc

QList<KoDocument> *KoDocument::s_documentList=0L;

using namespace std;
class KoViewWrapperWidget;

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
    }
    ~KoDocumentPrivate()
    {
    }

    QList<KoView> m_views;
    QList<KoDocumentChild> m_children;
    QList<KoMainWindow> m_shells;

    bool m_bSingleViewMode;
    mutable bool m_changed;

    KoViewWrapperWidget *m_wrapperWidget;

    QValueList<QDomDocument> m_viewBuildDocuments;
    KoDocumentIface * m_dcopObject;

    KoDocumentInfo *m_docInfo;

    QCString outputMimeType; // The mimetype to use when saving
    KoFilterManager * filterManager; // The filter-manager to use when loading/saving [for the options]
    // It can be set by KoMainWindow (to pass options), or created here (without options)

    QTimer m_autoSaveTimer;
    int m_autoSaveDelay; // in seconds, 0 to disable.
    bool modifiedAfterAutosave;
};

// Used in singleViewMode
class KoViewWrapperWidget : public QWidget
{
public:
  KoViewWrapperWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
  {
    KGlobal::locale()->insertCatalogue("koffice");
    // Tell the iconloader about share/apps/koffice/icons
    KGlobal::iconLoader()->addAppDir("koffice");
    m_view = 0L;
  }

  virtual ~KoViewWrapperWidget() {}

  virtual void resizeEvent( QResizeEvent * )
  {
    QObject *wid = child( 0, "QWidget" );
    if ( wid )
      static_cast<QWidget *>(wid)->setGeometry( 0, 0, width(), height() );
  }

  virtual void childEvent( QChildEvent *ev )
  {
    if ( ev->type() == QEvent::ChildInserted )
      resizeEvent( 0L );
  }

  void setKoView( KoView * view ) { m_view = view; }
  KoView * koView() const { return m_view; }
private:
  KoView* m_view;
};

KoBrowserExtension::KoBrowserExtension( KoDocument * doc, const char * name )
    : KParts::BrowserExtension( doc, name )
{
    emit enableAction( "print", true );
}

void KoBrowserExtension::print()
{
    KoDocument * doc = static_cast<KoDocument *>( parent() );
    KoViewWrapperWidget * wrapper = static_cast<KoViewWrapperWidget *>( doc->widget() );
    KoView * view = wrapper->koView();
    // TODO remove code duplication (KoMainWindow), by moving this to KoView
    KPrinter printer;
    // ### TODO: apply global koffice settings here
    view->setupPrinter( printer );
    if ( printer.setup( view ) )
        view->print( printer );
};

KoDocument::KoDocument( QWidget * parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KParts::ReadWritePart( parent, name )
{
  if(s_documentList==0L)
    s_documentList=new QList<KoDocument>;
  s_documentList->append(this);

  d = new KoDocumentPrivate;
  m_bEmpty = TRUE;
  d->m_changed=false;
  d->m_dcopObject = 0L;
  connect( &d->m_autoSaveTimer, SIGNAL( timeout() ), this, SLOT( slotAutoSave() ) );
  setAutoSave( s_defaultAutoSave );
  d->m_bSingleViewMode = singleViewMode;
  d->filterManager = 0L;
  d->modifiedAfterAutosave=false;

  // the parent setting *always* overrides! (Simon)
  if ( parent )
  {
    if ( parent->inherits( "KoDocument" ) )
      d->m_bSingleViewMode = ((KoDocument *)parent)->isSingleViewMode();
    else if ( parent->inherits( "KParts::Part" ) )
      d->m_bSingleViewMode = true;
  }

  if ( singleViewMode )
  {
      d->m_wrapperWidget = new KoViewWrapperWidget( parentWidget, widgetName );
      setWidget( d->m_wrapperWidget );
      kdDebug(30003) << "creating KoBrowserExtension" << endl;
      (void) new KoBrowserExtension( this ); // ## only if embedded into a browser?
    }

  d->m_docInfo = new KoDocumentInfo( this, "document info" );
}

KoDocument::~KoDocument()
{
  QListIterator<KoDocumentChild> childIt( d->m_children );
  for (; childIt.current(); ++childIt )
    disconnect( childIt.current(), SIGNAL( destroyed() ),
                this, SLOT( slotChildDestroyed() ) );

  // Tell our views that the document is already destroyed and
  // that they shouldn't try to access it.
  QListIterator<KoView> vIt( d->m_views );
  for (; vIt.current(); ++vIt )
      vIt.current()->setDocumentDeleted();

  d->m_children.setAutoDelete( true );
  d->m_children.clear();

  d->m_shells.setAutoDelete( true );
  d->m_shells.clear();

  delete d->m_dcopObject;
  delete d;
  s_documentList->removeRef(this);
  // last one?
  if(s_documentList->count()==0) {
      delete s_documentList;
      s_documentList=0;
  }
}

bool KoDocument::isSingleViewMode() const
{
  return d->m_bSingleViewMode;
}

bool KoDocument::isEmbedded() const {
  return dynamic_cast<KoDocument *>( parent() ) != 0;
}

KoView *KoDocument::createView( QWidget *parent, const char *name ) {

    KoView *view=createViewInstance(parent, name);
    addView(view);
    return view;
}

bool KoDocument::saveFile()
{
  kdDebug(30003) << "KoDocument::saveFile()" << endl;
  if ( !kapp->inherits( "KoApplication" ) )
    return false;

  QCString _native_format = nativeFormatMimeType();
  // The output format is set by koMainWindow, and by openFile
  QCString outputMimeType = d->outputMimeType;
  ASSERT( !outputMimeType.isEmpty() );
  if ( outputMimeType.isEmpty() )
      outputMimeType = _native_format;

  QApplication::setOverrideCursor( waitCursor );

  if ( KIO::NetAccess::exists( m_url ) ) { // this file exists => backup
        // TODO : make this configurable ?
        KURL backup( m_url );
        backup.setPath( backup.path() + QString::fromLatin1("~") );
        (void) KIO::NetAccess::del( backup );
        (void) KIO::NetAccess::copy( m_url, backup );

        // This is noticeably faster, but not network transparent, and more importantly
        // it fails with '(' and other special chars in the filename.
        //QString cmd = QString( "rm -rf %1~" ).arg( url.path() );
        //system( cmd.local8Bit() );
        //cmd = QString("cp %1 %2~").arg( url.path() ).arg( url.path() );
        //system( cmd.local8Bit() );
  }
  bool ret;
  if ( outputMimeType != _native_format ) {
    kdDebug(30003) << "Saving to format " << outputMimeType << " in " << m_file << endl;
    // Not native format : save using export filter
    d->m_changed=false;
    if ( !d->filterManager )
        d->filterManager = new KoFilterManager();
    QString nativeFile = d->filterManager->prepareExport( m_file, _native_format, outputMimeType, this);
    kdDebug(30003) << "Temp native file " << nativeFile << endl;

    if(d->m_changed==false && nativeFile!=m_file) {
        ret = saveNativeFormat( nativeFile );
        if ( !ret )
            kdError(30003) << "Couldn't save in native format!" << endl;
        else
            ret = d->filterManager->export_();
    } else {
      // How can this happen ? m_changed = true ?
      // No -> nativeFile==m_file :) (Werner)
      ret = true;
    }
  } else {
    // Native format => normal save
    ret = saveNativeFormat( m_file );
  }

  if ( ret )
  {
    // Eliminate any auto-save file
    QString asf = autoSaveFile( m_file );
    if ( QFile::exists( asf ) )
      unlink( QFile::encodeName( asf ) );
  }

  QApplication::restoreOverrideCursor();
  if ( !ret )
  {
    KMessageBox::error( 0L, i18n( "Could not save\n%1" ).arg( m_file ) );
  }
  return ret;
}

void KoDocument::setOutputMimeType( const QCString & mimeType )
{
    d->outputMimeType = mimeType;
}

QCString KoDocument::outputMimeType() const
{
    return d->outputMimeType;
}

void KoDocument::setFilterManager( KoFilterManager * manager )
{
    delete d->filterManager;
    d->filterManager = manager;
}


void KoDocument::slotAutoSave()
{
    //kdDebug() << "KoDocument::slotAutoSave m_file=" << m_file << endl;
    //kdDebug()<<"Autosave : modifiedAfterAutosave "<<d->modifiedAfterAutosave<<endl;
    if ( !m_file.isEmpty() && isModified() && d->modifiedAfterAutosave )
    {
        // TODO temporary message in statusbar ?
        /*bool ret =*/ saveNativeFormat( autoSaveFile( m_file ) );
        setModified( true );
        d->modifiedAfterAutosave=false;
    }
}

KAction *KoDocument::action( const QDomElement &element ) const
{
  return d->m_views.getFirst()->action( element );
}

QDomDocument KoDocument::domDocument() const
{
//  assert(!d->m_views.isEmpty());
//  return d->m_views.getFirst()->domDocument();
    return QDomDocument();
}

void KoDocument::setManager( KParts::PartManager *manager )
{
  KParts::ReadWritePart::setManager( manager );
  if ( d->m_bSingleViewMode && d->m_views.count() == 1 )
    d->m_views.getFirst()->setPartManager( manager );

  QListIterator<KoDocumentChild> it( d->m_children );
  for (; it.current(); ++it )
      if ( it.current()->document() )
          manager->addPart( it.current()->document(), false );

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

  setAutoSave( d->m_autoSaveDelay );
}

void KoDocument::setAutoSave( int delay )
{
  d->m_autoSaveDelay = delay;
  if ( isReadWrite() && d->m_autoSaveDelay > 0 )
    d->m_autoSaveTimer.start( d->m_autoSaveDelay * 1000 );
  else
    d->m_autoSaveTimer.stop();
}

void KoDocument::addView( KoView *view )
{
  if ( !view )
    return;

  d->m_views.append( view );
  view->updateReadWrite( isReadWrite() );
}

void KoDocument::removeView( KoView *view )
{
    d->m_views.removeRef( view );
}

const QList<KoView>& KoDocument::views() const
{
    return d->m_views;
}

int KoDocument::viewCount() const
{
  return d->m_views.count();
}

void KoDocument::insertChild( KoDocumentChild *child )
{
  setModified( true );

  d->m_children.append( child );

  connect( child, SIGNAL( changed( KoChild * ) ),
           this, SLOT( slotChildChanged( KoChild * ) ) );
  connect( child, SIGNAL( destroyed() ),
           this, SLOT( slotChildDestroyed() ) );

  // It may be that insertChild is called without the KoDocumentChild
  // having a KoDocument attached, yet. This happens for example
  // when KPresenter loads a document with embedded objects. For those
  // KPresenterChild objects are allocated and insertChild is called.
  // Later in loadChildren() KPresenter iterates over the child list
  // and calls loadDocument for each child. That's exactly where we
  // will try to do what we cannot do now: Register the child document
  // at the partmanager (Simon)
  if ( manager() && !isSingleViewMode() && child->document() )
    manager()->addPart( child->document(), false );
}

void KoDocument::slotChildChanged( KoChild *c )
{
  assert( c->inherits( "KoDocumentChild" ) );
  emit childChanged( static_cast<KoDocumentChild *>( c ) );
}

void KoDocument::slotChildDestroyed()
{
    setModified( true );

    const KoDocumentChild *child = static_cast<const KoDocumentChild *>( sender() );
    d->m_children.removeRef( child );
}

const QList<KoDocumentChild>& KoDocument::children() const
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
      canvasPos.rx() += it.current()->canvasXOffset();
      canvasPos.ry() += it.current()->canvasYOffset();

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

KoDocumentInfo *KoDocument::documentInfo() const
{
  return d->m_docInfo;
}

void KoDocument::setViewBuildDocument( KoView *view, const QDomDocument &doc )
{
  if ( d->m_views.find( view ) == -1 )
    return;

  uint viewIdx = d->m_views.at();

  if ( d->m_viewBuildDocuments.count() == viewIdx )
  {
    d->m_viewBuildDocuments.append( doc );
  }
  else if ( d->m_viewBuildDocuments.count() > viewIdx )
  {
    d->m_viewBuildDocuments[ viewIdx ] = doc;
  }
}

QDomDocument KoDocument::viewBuildDocument( KoView *view )
{
  QDomDocument res;

  if ( d->m_views.find( view ) == -1 )
    return res;

  uint viewIdx = d->m_views.at();

  if ( viewIdx >= d->m_viewBuildDocuments.count() )
    return res;

  res = d->m_viewBuildDocuments[ viewIdx ];

  // make this entry empty. otherwise we get a segfault in QMap ;-(
  d->m_viewBuildDocuments[ viewIdx ] = QDomDocument();

  return res;
}

void KoDocument::paintEverything( QPainter &painter, const QRect &rect, bool transparent, KoView *view, double zoomX, double zoomY )
{
  paintContent( painter, rect, transparent, zoomX, zoomY );
  paintChildren( painter, rect, view, zoomX, zoomY );
}

void KoDocument::paintChildren( QPainter &painter, const QRect &/*rect*/, KoView *view, double zoomX, double zoomY )
{
  QListIterator<KoDocumentChild> it( d->m_children );
  for (; it.current(); ++it )
  {
    // #### todo: paint only if child is visible inside rect
    painter.save();
    paintChild( it.current(), painter, view, zoomX, zoomY );
    painter.restore();
  }
}

void KoDocument::paintChild( KoDocumentChild *child, QPainter &painter, KoView *view, double zoomX, double zoomY )
{
    // QRegion rgn = painter.clipRegion();

  child->transform( painter );
  child->document()->paintEverything( painter, child->contentRect(), child->isTransparent(), view, zoomX, zoomY );

  if ( view && view->partManager() )
  {
      // ### do we need to apply zoomX and zoomY here ?
    KParts::PartManager *manager = view->partManager();

    painter.scale( 1.0 / child->xScaling(), 1.0 / child->yScaling() );

    int w = int( (double)child->contentRect().width() * child->xScaling() );
    int h = int( (double)child->contentRect().height() * child->yScaling() );
    if ( ( manager->selectedPart() == (KParts::Part *)child->document() &&
           manager->selectedWidget() == (QWidget *)view ) ||
         ( manager->activePart() == (KParts::Part *)child->document() &&
           manager->activeWidget() == (QWidget *)view ) )
        {
            // painter.setClipRegion( rgn );
          painter.setClipping( FALSE );

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

          painter.setClipping( TRUE );
      }
  }
}

bool KoDocument::saveChildren( KoStore* /*_store*/, const QString& /*_path*/ )
{
  // Lets assume that we do not have children
  kdWarning(30003) << "KoDocument::saveChildren( KoStore*, const QString & )" << endl;
  kdWarning(30003) << "Not implemented ( not really an error )" << endl;
  return true;
}

bool KoDocument::saveNativeFormat( const QString & file )
{
  kdDebug(30003) << "Saving to store" << endl;

  QCString appIdentification( "KOffice " ); // We are limited in the number of chars.
  appIdentification += nativeFormatMimeType();
  appIdentification += '\004'; // Two magic bytes to make the identification
  appIdentification += '\006'; // more reliable (DF)
  KoStore* store = new KoStore( file, KoStore::Write, appIdentification );

  // Save childen first since they might get a new url
  if ( store->bad() || !saveChildren( store, STORE_PROTOCOL ) )
  {
    kdDebug(30003) << "store->bad()=" << store->bad() << "   aborting saving !" << endl;
    delete store;
    return false;
  }

  kdDebug(30003) << "Saving root" << endl;
  if ( store->open( "root" ) )
  {
    KoStoreDevice dev( store );
    if ( !saveToStream( &dev ) )
    {
      delete store;
      return false;
    }
    store->close();
  }
  else
  {
    delete store;
    return false;
  }
  if ( store->open( "documentinfo.xml" ) )
  {
    QDomDocument doc = d->m_docInfo->save();
    KoStoreDevice dev( store );

    QCString s = doc.toCString(); // this is already Utf8!

    dev.writeBlock( s.data(), s.length() );
    store->close();
  }

  bool ret = completeSaving( store );
  kdDebug(30003) << "Saving done" << endl;
  delete store;
  return ret;
}

bool KoDocument::saveToStream( QIODevice * dev )
{
  QDomDocument doc = saveXML();
  // Save to buffer
  QCString s = doc.toCString(); // utf8 already!!!
  return dev->writeBlock( s.data(), s.length() ) == (int)s.length();
}

bool KoDocument::saveToStore( KoStore* _store, const QString & _path )
{
  kdDebug(30003) << "Saving document to store" << endl;

  // Use the path as the internal url
  m_url = _path;

  // Save childen first since they might get a new url
  if ( !saveChildren( _store, _path ) )
    return false;

  QString u = url().url();
  if ( _store->open( u ) )
  {
    KoStoreDevice dev( _store );
    if ( !saveToStream( &dev ) )
    {
      _store->close();
      return false;
    }

    _store->close();
  }

  if ( !completeSaving( _store ) )
    return false;

  kdDebug(30003) << "Saved document to store" << endl;

  return true;
}

QString KoDocument::autoSaveFile( const QString & path ) const
{
    KURL url( path );
    ASSERT( url.isLocalFile() );
    QString dir = url.directory(false);
    QString filename = url.filename();
    return dir + '.' + filename + ".autosave";
}

bool KoDocument::openURL( const KURL & _url )
{
  // Reimplemented, to add a check for autosave files
  if ( _url.isMalformed() )
    return false;
  if ( !closeURL() )
    return false;
  KURL url( _url );
  bool autosaveOpened = false;
  if ( url.isLocalFile() )
  {
    QString file = url.path();
    QString asf = autoSaveFile( file );
    if ( QFile::exists( asf ) )
    {
      kdDebug() << "KoDocument::openURL asf=" << asf << endl;
      // ## TODO compare timestamps ?
      int res = KMessageBox::warningYesNoCancel( 0,
            i18n( "An autosaved file exists for this document.\nDo you want to open it instead ?" ));
      switch(res) {
      case KMessageBox::Yes :
        url.setPath( asf );
        autosaveOpened = true;
        break;
      case KMessageBox::No :
        unlink( QFile::encodeName( asf ) );
        break;
      default: // Cancel
        return false;
      }
    }
  }
  bool ret = false;
  if ( url.isLocalFile() )
  {
      // ReadOnlyPart::openURL does something wrong for local files: it emits completed
      // even if openFile returned false :(
      // So, fixing here:
      m_url = url;
      m_file = m_url.path();
      ret = openFile();
      if ( ret )
          emit completed();
  }
  else
  {
      ret = KParts::ReadWritePart::openURL( url );
  }
  if ( autosaveOpened )
      m_url = KURL(); // Force save to act like 'Save As'
  else
  {
      if ( d->m_shells.isEmpty() )
          kdWarning() << "KoDocument::openURL no shell yet !" << endl;
      // Add to recent actions list in our shells
      QListIterator<KoMainWindow> it( d->m_shells );
      for (; it.current(); ++it )
          it.current()->addRecentURL( _url );
  }
  return ret;
}

bool KoDocument::openFile()
{
  kdDebug(30003) << "KoDocument::openFile for " << m_file << endl;
  if ( ! QFile::exists(m_file) )
  {
    // Maybe offer to create a new document with that name ?
    KMessageBox::error(0L, i18n("The file %1 doesn't exist.").arg(m_file) );
    return false;
  }

  QApplication::setOverrideCursor( waitCursor );

  if ( d->m_bSingleViewMode && !d->m_views.isEmpty() )
  {
     // We already had a view (this happens when doing reload in konqueror)
     removeView( d->m_views.first() );
     delete d->m_views.first();
     ASSERT( d->m_views.isEmpty() );
  }

  d->m_changed=false;
  QCString _native_format = nativeFormatMimeType();

  // Launch a filter if we need one for this url ?
  if ( !d->filterManager )
      d->filterManager = new KoFilterManager();
  QString importedFile = d->filterManager->import( m_file, _native_format, this );

  kdDebug(30003) << "KoDocument::openFile - importedFile " << importedFile << endl;

  QApplication::restoreOverrideCursor();

  bool ok = true;

  if (!importedFile.isEmpty()) // Something to load (tmp or native file) ?
  {
    // The filter, if any, has been applied. It's all native format now.
    if ( !loadNativeFormat( importedFile ) )
    {
      ok = false;
      KMessageBox::error( 0L, i18n( "Could not open\n%1" ).arg( url().prettyURL() ) );
    }
  }
  else {
    // The filter did it all. Ok if it changed something...
    ok = d->m_changed;
  }

  if ( importedFile != m_file )
  {
    // We opened a temporary file (result of an import filter)
    // Set document URL to empty - we don't want to save in /tmp !
    // But only if in readwrite mode (no saving problem otherwise)
    if ( isReadWrite() )
      m_url = KURL();
    // and remove temp file - uncomment this to debug import filters
    if(!importedFile.isEmpty())
      unlink( QFile::encodeName(importedFile) );
  }

  if ( ok && d->m_bSingleViewMode )
  {
    KoView *view = createView( d->m_wrapperWidget );
    d->m_wrapperWidget->setKoView( view );
    view->show();
  }

  // We decided not to save in the file's original format by default
  // ( KWord isn't a text editor or a MSWord editor :)
  // The risk of losing formatting information is too high currently.
  d->outputMimeType = _native_format;

  return ok;
}

bool KoDocument::loadNativeFormat( const QString & file )
{
  QApplication::setOverrideCursor( waitCursor );

  kdDebug(30003) << "KoDocument::loadNativeFormat( " << file << " )" << endl;

  QFile in(file);
  if ( !in.open( IO_ReadOnly ) )
  {
    QApplication::restoreOverrideCursor();
    return false;
  }

  // Try to find out whether it is a mime multi part file
  char buf[5];
  if ( in.readBlock( buf, 4 ) < 4 )
  {
    QApplication::restoreOverrideCursor();
    in.close();
    return false;
  }

  //kdDebug(30003) << "PATTERN=" << buf << endl;

  // Is it plain XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    in.at(0);
    QDomDocument doc;
    doc.setContent( &in );
    bool res = loadXML( &in, doc );
    if ( res )
      res = completeLoading( 0L );

    QApplication::restoreOverrideCursor();
    in.close();
    m_bEmpty = false;
    return res;
  } else
  { // It's a koffice store (tar.gz)
    in.close();
    KoStore * store = new KoStore( file, KoStore::Read );

    if ( store->bad() )
    {
      delete store;
      QApplication::restoreOverrideCursor();
      return false;
    }

    if ( store->open( "root" ) )
    {
      KoStoreDevice dev( store );
      QDomDocument doc;
      doc.setContent( &dev );
      if ( !loadXML( &dev, doc ) )
      {
        delete store;
        QApplication::restoreOverrideCursor();
        return false;
      }
      store->close();
    } else
    {
      kdError(30003) << "ERROR: No maindoc.xml" << endl;
      delete store;
      QApplication::restoreOverrideCursor();
      return false;
    }

    if ( !loadChildren( store ) )
    {
      kdError(30003) << "ERROR: Could not load children" << endl;
      delete store;
      QApplication::restoreOverrideCursor();
      return false;
    }
    if ( store->open( "documentinfo.xml" ) )
    {
      KoStoreDevice dev( store );
      QDomDocument doc;
      doc.setContent( &dev );
      d->m_docInfo->load( doc );
      store->close();
    }
    else
    {
      kdDebug( 30003 ) << "cannot open document info" << endl;
      delete d->m_docInfo;
      d->m_docInfo = new KoDocumentInfo( this, "document info" );
    }

    bool res = completeLoading( store );
    delete store;
    QApplication::restoreOverrideCursor();
    m_bEmpty = false;
    return res;
  }
}

bool KoDocument::loadFromStore( KoStore* _store, const KURL & url )
{
  if ( _store->open( url.url() ) )
  {
    KoStoreDevice dev( _store );
    QDomDocument doc;
    doc.setContent( &dev );
    if ( !loadXML( &dev, doc ) )
      return false;
    _store->close();
  }
  // Store as document URL
  m_url = url;

  if ( !loadChildren( _store ) )
  {
    kdError(30003) << "ERROR: Could not load children" << endl;
    return false;
  }

  return completeLoading( _store );
}

bool KoDocument::isStoredExtern()
{
  return ( m_url.protocol() != STORE_PROTOCOL );
}

void KoDocument::setModified( bool mod )
{
    if ( mod == isModified() )
        return;

    kdDebug(30003) << "KoDocument::setModified( " << (mod ? "true" : "false") << ")" << endl;
    KParts::ReadWritePart::setModified( mod );

    if ( mod )
        m_bEmpty = FALSE;

    // This influences the title
    setTitleModified();
    d->modifiedAfterAutosave=mod;

}

void KoDocument::setTitleModified()
{
    // Update caption in all related windows
    QListIterator<KoMainWindow> it( d->m_shells );
    for (; it.current(); ++it )
        it.current()->updateCaption();
}

void KoDocument::changedByFilter( bool changed ) const
{
    kdDebug(30003) << "KoDocument::changedByFilter " << (changed ? "true" : "false") << ")" << endl;
    d->m_changed=changed;
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

QDomDocument KoDocument::saveXML()
{
    kdError(30003) << "KoDocument::saveXML not implemented" << endl;
    return QDomDocument();
}

KService::Ptr KoDocument::nativeService()
{
  if ( !m_nativeService )
      m_nativeService = readNativeService( instance() );

  return m_nativeService;
}

QCString KoDocument::nativeFormatMimeType()
{
    KService::Ptr service = nativeService();
    if ( !service )
        return QCString();
    return service->property( "X-KDE-NativeMimeType" ).toString().latin1();
}

//static
KService::Ptr KoDocument::readNativeService( KInstance *instance )
{
  QString instname = instance ? instance->instanceName() : kapp->instanceName();

  // Try by path first, so that we find the global one (which has the native mimetype)
  // even if the user created a kword.desktop in ~/.kde/share/applnk or any subdir of it.
  // If he created it under ~/.kde/share/applnk/Office/ then no problem anyway.
  KService::Ptr service = KService::serviceByDesktopPath( QString::fromLatin1("Office/%1.desktop").arg(instname) );
  if ( !service )
  {
    service = KService::serviceByDesktopName( instname );
  }

  if ( !service )
    return service;

  if ( service->property( "X-KDE-NativeMimeType" ).toString().isEmpty() )
  {
      // It may be that the servicetype "KOfficePart" is missing, which leads to this property not being known
      if ( KServiceType::serviceType( "KOfficePart" ) == 0L )
        kdError(30003) << "The serviceType KOfficePart is missing. Check that you have a kofficepart.desktop file in the share/servicetypes directory." << endl;
      else if ( instname != "koshell" ) // hack for koshell
        kdWarning(30003) << service->desktopEntryPath() << ": no X-KDE-NativeMimeType entry!" << endl;
  }

  return service;
}

QCString KoDocument::readNativeFormatMimeType( KInstance *instance )
{
    KService::Ptr service = readNativeService( instance );
    if ( !service )
        return QCString();
    return service->property( "X-KDE-NativeMimeType" ).toString().latin1();
}

void KoDocument::addShell( KoMainWindow *shell )
{
  d->m_shells.append( shell );
}

void KoDocument::removeShell( KoMainWindow *shell )
{
  d->m_shells.removeRef( shell );
}

const QList<KoMainWindow>& KoDocument::shells() const
{
    return d->m_shells;
}

int KoDocument::shellCount() const
{
    return d->m_shells.count();
}

DCOPObject * KoDocument::dcopObject()
{
  if ( !d->m_dcopObject )
  {
      static int s_docIFNumber = 0;
      QCString name; name.setNum( s_docIFNumber ); name.prepend("Document-");
      d->m_dcopObject = new KoDocumentIface( this, name );
  }
  return d->m_dcopObject;
}

#include "koDocument.moc"
#include "koDocument_p.moc"


