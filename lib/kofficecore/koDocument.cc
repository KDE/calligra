/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000, 2001 David Faure <david@mandrakesoft.com>

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <unistd.h>

#include <koDocument.h>
#include <koDocument_p.h>
#include <KoDocumentIface.h>
#include <koDocumentChild.h>
#include <koView.h>
#include <koMainWindow.h>
#include <koStoreDevice.h>
#include <koFilterManager.h>
#include <koDocumentInfo.h>
#include <kprinter.h>

#include <kio/netaccess.h>
#include <kparts/partmanager.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qimage.h>
#include <kiconloader.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qcursor.h>

// Define the protocol used here for embedded documents' URL
// This used to "store" but KURL didn't like it,
// so let's simply make it "tar" !
#define STORE_PROTOCOL "tar"
// The internal path is a hack to make KURL happy and still pass
// some kind of relative path to KoDocumentChild
#define INTERNAL_PROTOCOL "intern"
#define INTERNAL_PREFIX "intern:/"
// Warning, keep it sync in koStore.cc and koDocumentChild.cc

QPtrList<KoDocument> *KoDocument::s_documentList=0L;

using namespace std;
class KoViewWrapperWidget;

/**********************************************************
 *
 * KoDocument
 *
 **********************************************************/

const int KoDocument::s_defaultAutoSave = 300; // 5 minutes

class KoDocument::Private
{
public:
    Private() :
        m_dcopObject( 0L ),
        filterManager( 0L ),
        m_specialOutputFlag( 0 ),
        m_numOperations( 0 ),
        modifiedAfterAutosave( false ),
        m_autosaving( false ),
        m_shouldCheckAutoSaveFile( true ),
        m_backupFile( true ),
        m_backupPath( QString::null ),
        m_doNotSaveExtDoc( false ) 
        {}

    QPtrList<KoView> m_views;
    QPtrList<KoDocumentChild> m_children;
    QPtrList<KoMainWindow> m_shells;
    QValueList<QDomDocument> m_viewBuildDocuments;

    KoViewWrapperWidget *m_wrapperWidget;
    KoDocumentIface * m_dcopObject;
    KoDocumentInfo *m_docInfo;

    KoFilterManager * filterManager; // The filter-manager to use when loading/saving [for the options]

    QCString outputMimeType; // The mimetype to use when saving
    int m_specialOutputFlag; // See KoFileDialog in koMainWindow.cc
    QTimer m_autoSaveTimer;
    QString lastErrorMessage; // see openFile()
    int m_autoSaveDelay; // in seconds, 0 to disable.
    int m_numOperations;
    bool modifiedAfterAutosave;
    bool m_bSingleViewMode;
    bool m_autosaving;
    bool m_shouldCheckAutoSaveFile; // usually true
    bool m_backupFile;
    QString m_backupPath;
    bool m_doNotSaveExtDoc; // makes it possible to save only internally stored child documents
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
        // Avoid warning from KParts - we'll have the KoView as focus proxy anyway
        setFocusPolicy( ClickFocus );
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

    // Called by openFile()
    void setKoView( KoView * view ) {
        m_view = view;
        setFocusProxy( m_view );
    }
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
        s_documentList=new QPtrList<KoDocument>;
    s_documentList->append(this);

    d = new Private;
    m_bEmpty = TRUE;
    connect( &d->m_autoSaveTimer, SIGNAL( timeout() ), this, SLOT( slotAutoSave() ) );
    setAutoSave( s_defaultAutoSave );
    d->m_bSingleViewMode = singleViewMode;


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

    m_pageLayout.ptWidth = 0;
    m_pageLayout.ptHeight = 0;
    m_pageLayout.ptTop = 0;
    m_pageLayout.ptBottom = 0;
    m_pageLayout.ptLeft = 0;
    m_pageLayout.ptRight = 0;
}

KoDocument::~KoDocument()
{
    QPtrListIterator<KoDocumentChild> childIt( d->m_children );
    for (; childIt.current(); ++childIt )
        disconnect( childIt.current(), SIGNAL( destroyed() ),
                    this, SLOT( slotChildDestroyed() ) );

    // Tell our views that the document is already destroyed and
    // that they shouldn't try to access it.
    QPtrListIterator<KoView> vIt( d->m_views );
    for (; vIt.current(); ++vIt )
        vIt.current()->setDocumentDeleted();

    d->m_children.setAutoDelete( true );
    d->m_children.clear();

    d->m_shells.setAutoDelete( true );
    d->m_shells.clear();

    delete d->m_dcopObject;
    delete d->filterManager;
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

bool KoDocument::isEmbedded() const
{
    return dynamic_cast<KoDocument *>( parent() ) != 0;
}

KoView *KoDocument::createView( QWidget *parent, const char *name )
{
    KoView *view=createViewInstance(parent, name);
    addView(view);
    return view;
}

bool KoDocument::saveFile()
{
    kdDebug(30003) << "KoDocument::saveFile() doc='" << url().url() <<"'"<< endl;
    if ( !kapp->inherits( "KoApplication" ) )
    {
        d->lastErrorMessage = i18n( "Internal error: not a KOffice application, saving not allowed" );
        return false;
    }

    QCString _native_format = nativeFormatMimeType();
    // The output format is set by koMainWindow, and by openFile
    QCString outputMimeType = d->outputMimeType;
    Q_ASSERT( !outputMimeType.isEmpty() );
    if ( outputMimeType.isEmpty() )
        outputMimeType = _native_format;

    QApplication::setOverrideCursor( waitCursor );

    if ( KIO::NetAccess::exists( url() ) ) { // this file exists => backup
        KURL backup;
        if ( d->m_backupPath.isEmpty())
            backup= url();
        else
            backup = d->m_backupPath +"/"+url().fileName();
        backup.setPath( backup.path() + QString::fromLatin1("~") );
        (void) KIO::NetAccess::del( backup );
        if ( backupFile() )
            (void) KIO::NetAccess::copy( url(), backup );

        // This is noticeably faster, but not network transparent, and more importantly
        // it fails with '(' and other special chars in the filename.
        //QString cmd = QString( "rm -rf %1~" ).arg( url.path() );
        //system( cmd.local8Bit() );
        //cmd = QString("cp %1 %2~").arg( url.path() ).arg( url.path() );
        //system( cmd.local8Bit() );
    }

    bool ret = false;
    if ( outputMimeType != _native_format ) {
        kdDebug(30003) << "Saving to format " << outputMimeType << " in " << m_file << endl;
        // Not native format : save using export filter
        if ( !d->filterManager )
            d->filterManager = new KoFilterManager( this );

        ret = d->filterManager->exp0rt( m_file, outputMimeType ) == KoFilter::OK;
    } else
        // Native format => normal save
        ret = saveNativeFormat( m_file );

    if ( ret )
        removeAutoSaveFiles();

    QApplication::restoreOverrideCursor();
    if ( !ret )
    {
        if ( d->lastErrorMessage.isEmpty() )
            KMessageBox::error( 0L, i18n( "Could not save\n%1" ).arg( m_file ) );
        else if ( d->lastErrorMessage != "USER_CANCELED" )
        {
            KMessageBox::error( 0L, i18n( "Could not save %1\nReason: %2" ).arg( m_file ).arg( d->lastErrorMessage ) );
            //don't store url when we are not autorize to save file.
            resetURL();
        }
    }
    return ret;
}

void KoDocument::setOutputMimeType( const QCString & mimeType, int specialOutputFlag )
{
    d->outputMimeType = mimeType;
    d->m_specialOutputFlag = specialOutputFlag;
}

QCString KoDocument::outputMimeType() const
{
    return d->outputMimeType;
}

int KoDocument::specialOutputFlag() const
{
    return d->m_specialOutputFlag;
}

void KoDocument::setCheckAutoSaveFile( bool b )
{
    d->m_shouldCheckAutoSaveFile = b;
}

void KoDocument::slotAutoSave()
{
    //kdDebug(30003)<<"Autosave : modifiedAfterAutosave "<<d->modifiedAfterAutosave<<endl;
    if ( isModified() && d->modifiedAfterAutosave )
    {
        connect( this, SIGNAL( sigProgress( int ) ), shells().current(), SLOT( slotProgress( int ) ) );
        d->m_autosaving = true;
        /*bool ret =*/ saveNativeFormat( autoSaveFile( m_file ) );
        setModified( true );
        d->modifiedAfterAutosave=false;
        d->m_autosaving = false;
        disconnect( this, SIGNAL( sigProgress( int ) ), shells().current(), SLOT( slotProgress( int ) ) );
    }
}

KAction *KoDocument::action( const QDomElement &element ) const
{
    // First look in the document itself
    KAction* act = KParts::ReadWritePart::action( element );
    if ( act )
        return act;

    Q_ASSERT( d->m_bSingleViewMode );
    // Then look in the first view (this is for the single view mode)
    if ( !d->m_views.isEmpty() )
        return d->m_views.getFirst()->action( element );
    else
        return 0L;
}

QDomDocument KoDocument::domDocument() const
{
    // When embedded into e.g. konqueror, we want the view's GUI (hopefully a reduced one)
    // to be used.
    Q_ASSERT( d->m_bSingleViewMode );
    if ( d->m_views.isEmpty() )
        return QDomDocument();
    else
        return d->m_views.getFirst()->domDocument();
}

void KoDocument::setManager( KParts::PartManager *manager )
{
    KParts::ReadWritePart::setManager( manager );
    if ( d->m_bSingleViewMode && d->m_views.count() == 1 )
        d->m_views.getFirst()->setPartManager( manager );

    QPtrListIterator<KoDocumentChild> it( d->m_children );
    for (; it.current(); ++it )
        if ( it.current()->document() )
            manager->addPart( it.current()->document(), false );

}

void KoDocument::setReadWrite( bool readwrite )
{
    KParts::ReadWritePart::setReadWrite( readwrite );

    QPtrListIterator<KoView> vIt( d->m_views );
    for (; vIt.current(); ++vIt )
        vIt.current()->updateReadWrite( readwrite );

    QPtrListIterator<KoDocumentChild> dIt( d->m_children );
    for (; dIt.current(); ++dIt )
        if ( dIt.current()->document() )
            dIt.current()->document()->setReadWrite( readwrite );

    setAutoSave( d->m_autoSaveDelay );
}

void KoDocument::setAutoSave( int delay )
{
    d->m_autoSaveDelay = delay;
    if ( isReadWrite() && !isEmbedded() && d->m_autoSaveDelay > 0 )
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

const QPtrList<KoView>& KoDocument::views() const
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

const QPtrList<KoDocumentChild>& KoDocument::children() const
{
    return d->m_children;
}

KParts::Part *KoDocument::hitTest( QWidget *widget, const QPoint &globalPos )
{
    QPtrListIterator<KoView> it( d->m_views );
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
    QPtrListIterator<KoDocumentChild> it( d->m_children );
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
    QPtrListIterator<KoDocumentChild> it( d->m_children );
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
        d->m_viewBuildDocuments.append( doc );
    else if ( d->m_viewBuildDocuments.count() > viewIdx )
        d->m_viewBuildDocuments[ viewIdx ] = doc;
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
    QPtrListIterator<KoDocumentChild> it( d->m_children );
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
    if ( child->isDeleted() )
        return;
        
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

bool KoDocument::isModified()
{
    if ( KParts::ReadWritePart::isModified() )
    {
        kdDebug()<<k_funcinfo<<" Modified doc='"<<url().url()<<"'"<<endl;
        return true;
    }
    // Then go through internally stored children (considdered to be part of this doc)
    QPtrListIterator<KoDocumentChild> it = children();
    for (; it.current(); ++it )
    {
        if ( !it.current()->isStoredExtern() && !it.current()->isDeleted() )
        {
            KoDocument *doc = it.current()->document();
            if ( doc && doc->isModified() )
                return true;
        }
    }
    return false;
}

bool KoDocument::saveChildren( KoStore* _store )
{
    //kdDebug()<<k_funcinfo<<" checking children of doc='"<<url().url()<<"'"<<endl;
    int i = 0;
    QPtrListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        KoDocument* childDoc = it.current()->document();
        if (childDoc && !it.current()->isDeleted())
        {
            if ( !childDoc->isStoredExtern() ) 
            {
                //kdDebug(32001) << "KoDocument::saveChildren internal url:" << childDoc->url().url() << endl;
                if ( !childDoc->saveToStore( _store, QString::number( i++ ) ) )
                    return FALSE;
                childDoc->setModified( false );
            }
            //else kdDebug()<<k_funcinfo<<" external (don't save) url:" << childDoc->url().url()<<endl;
        } 
    }
    return true;
}

bool KoDocument::saveExternalChildren()
{
    if ( d->m_doNotSaveExtDoc )
    {
        //kdDebug()<<k_funcinfo<<" Don't save external docs in doc='"<<url().url()<<"'"<<endl;    
        d->m_doNotSaveExtDoc = false;
        return true;
    }
        
    //kdDebug()<<k_funcinfo<<" checking children of doc='"<<url().url()<<"'"<<endl;    
    KoDocument *doc;
    KoDocumentChild *ch;
    QPtrListIterator<KoDocumentChild> it = children();
    for (; (ch = it.current()); ++it )
    {
        if ( !ch->isDeleted() )
        {
            doc = ch->document();
            if ( doc->isStoredExtern() && doc->isModified() )
            {
                kdDebug()<<k_funcinfo<<" save external doc='"<<url().url()<<"'"<<endl;
                doc->setDoNotSaveExtDoc(); // Only save doc + it's internal children
                if ( !doc->save() ) 
                    return false; // error
            }
            //kdDebug()<<k_funcinfo<<" not modified doc='"<<url().url()<<"'"<<endl;
            // save possible external docs inside doc 
            if ( !doc->saveExternalChildren() )
                return false;
        }
    }
    return true;
}

bool KoDocument::saveNativeFormat( const QString & _file )
{
    QString file( _file );
    d->lastErrorMessage = QString::null;
    //kdDebug(30003) << "Saving to store" << endl;

    KoStore::Backend backend = KoStore::Auto;
    if ( d->m_specialOutputFlag == SaveAsKOffice1dot1 )
    {
        kdDebug(30003) << "Saving as KOffice-1.1 format, using a tar.gz" << endl;
        backend = KoStore::Tar; // KOffice-1.0/1.1 used tar.gz for the native mimetype
        //// TODO more backwards compat stuff (embedded docs etc.)
    }
    else if ( d->m_specialOutputFlag == SaveAsDirectoryStore )
    {
        backend = KoStore::Directory;
        kdDebug(30003) << "Saving as uncompressed XML, using directory store." << endl;
    }

    kdDebug(30003) << "KoDocument::saveNativeFormat nativeFormatMimeType=" << nativeFormatMimeType() << endl;
    KoStore* store = KoStore::createStore( file, KoStore::Write, nativeFormatMimeType(), backend );
    if ( store->bad() )
    {
        d->lastErrorMessage = i18n( "Couldn't open the file for saving" ); // more details needed?
        delete store;
        return false;
    }

    // Save internal children first since they might get a new url
    if ( !saveChildren( store ) )
    {
        if ( d->lastErrorMessage.isEmpty() )
            d->lastErrorMessage = i18n( "Error while saving embedded documents" ); // more details needed
        delete store;
        return false;
    }

    kdDebug(30003) << "Saving root" << endl;
    if ( store->open( "root" ) )
    {
        KoStoreDevice dev( store );
        if ( !saveToStream( &dev ) )
        {
            kdDebug(30003) << "saveToStream failed" << endl;
            delete store;
            return false;
        }
        store->close();
    }
    else
    {
        d->lastErrorMessage = i18n( "Not able to write maindoc.xml" );
        delete store;
        return false;
    }
    if ( store->open( "documentinfo.xml" ) )
    {
        QDomDocument doc = d->m_docInfo->save();
        KoStoreDevice dev( store );

        QCString s = doc.toCString(); // this is already Utf8!
        (void)dev.writeBlock( s.data(), s.size()-1 );
        store->close();
    }
    if ( store->open( "preview.png" ) )
    {
        savePreview( store );
        store->close();
    }

    bool ret = completeSaving( store );
    kdDebug(30003) << "Saving done of url: " << url().url() << endl;
    delete store;
    
    if ( !saveExternalChildren() )
    {        
        return false;
    }
    return ret;
}

bool KoDocument::saveToStream( QIODevice * dev )
{
    QDomDocument doc = saveXML();
    // Save to buffer
    QCString s = doc.toCString(); // utf8 already
    // Important: don't use s.length() here. It's slow, and dangerous (in case of a '\0' somewhere)
    // The -1 is because we don't want to write the final \0.
    int nwritten = dev->writeBlock( s.data(), s.size()-1 );
    if ( nwritten != (int)s.size()-1 )
        kdWarning(30003) << "KoDocument::saveToStream wrote " << nwritten << "   - expected " << s.size()-1 << endl;
    return nwritten == (int)s.size()-1;
}

bool KoDocument::saveToStore( KoStore* _store, const QString & _path )
{
    kdDebug(30003) << "Saving document to store " << _path << endl;

    // Use the path as the internal url
    if ( _path.startsWith( STORE_PROTOCOL ) )
        m_url = KURL( _path );
    else // ugly hack to pass a relative URI
        m_url = KURL( INTERNAL_PREFIX +  _path );

    // To make the children happy cd to the correct directory
    _store->pushDirectory();
    _store->enterDirectory( _path );

    // Save childen first since they might get a new url
    if ( !saveChildren( _store ) )
        return false;

    // In the current directory we're the king :-)
    if ( _store->open( "root" ) )
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

    // Now that we're done leave the directory again
    _store->popDirectory();

    kdDebug(30003) << "Saved document to store" << endl;

    return true;
}

void KoDocument::savePreview( KoStore* store )
{
    QPixmap pix = generatePreview(QSize(256, 256));
    // Reducing to 8bpp reduces file sizes quite a lot.
    QImage image = pix.convertToImage().convertDepth(8, Qt::AvoidDither | Qt::DiffuseDither);

    QByteArray imageData;
    QDataStream imageStream(imageData, IO_WriteOnly);
    imageStream << image;
    store->write( imageData );
}

QPixmap KoDocument::generatePreview( const QSize& size )
{
    double docWidth, docHeight;
    int pixmapSize = QMAX(size.width(), size.height());

    if (m_pageLayout.ptWidth > 1.0) {
        docWidth = m_pageLayout.ptWidth / 72 * QPaintDevice::x11AppDpiX();
        docHeight = m_pageLayout.ptHeight / 72 * QPaintDevice::x11AppDpiX();

    } else {
        // If we don't have a page layout, just draw the top left hand corner
        docWidth = 500.0;
        docHeight = 500.0;
    }

    double ratio = docWidth / docHeight;

    QPixmap pix;
    int previewWidth, previewHeight;
    if (ratio > 1.0)
    {
        previewWidth = (int) pixmapSize;
        previewHeight = (int) (pixmapSize / ratio);
    }
    else
    {
        previewWidth = (int) (pixmapSize * ratio);
        previewHeight = (int) pixmapSize;
    }

    pix.resize((int)docWidth, (int)docHeight);

    pix.fill( QColor( 245, 245, 245 ) );

    QRect rc(0, 0, pix.width(), pix.height());

    QPainter p;
    p.begin(&pix);
    paintEverything(p, rc, false);
    p.end();

    pix.convertFromImage(pix.convertToImage().smoothScale(previewWidth, previewHeight));

    return pix;
}

QString KoDocument::autoSaveFile( const QString & path ) const
{
    // Using the extension allows to avoid relying on the mime magic when opening
    KMimeType::Ptr mime = KMimeType::mimeType( nativeFormatMimeType() );
    QString extension = mime->property( "X-KDE-NativeExtension" ).toString();
    if ( path.isEmpty() )
    {
        // Never saved? Use a temp file in $HOME then
        // Yes, two open unnamed docs will overwrite each other's autosave file,
        // but hmm, we can only do something if that's in the same process anyway...
        QString ret = QDir::homeDirPath() + "/." + QString::fromLatin1(instance()->instanceName()) + ".autosave" + extension;
        return ret;
    }
    else
    {
        KURL url( path );
        Q_ASSERT( url.isLocalFile() );
        QString dir = url.directory(false);
        QString filename = url.fileName();
        return dir + "." + filename + ".autosave" + extension;
    }
}

bool KoDocument::checkAutoSaveFile()
{
    QString asf = autoSaveFile( QString::null ); // the one in $HOME
    //kdDebug(30003) << "asf=" << asf << endl;
    if ( QFile::exists( asf ) )
    {
        QDateTime date = QFileInfo(asf).lastModified();
        QString dateStr = date.toString(Qt::LocalDate);
        int res = KMessageBox::warningYesNoCancel(
            0, i18n( "An autosaved file for an unnamed document exists in %1.\nThis file is dated %2\nDo you want to open it?" )
            .arg(asf).arg(dateStr) );
        switch(res) {
        case KMessageBox::Yes : {
            KURL url;
            url.setPath( asf );
            bool ret = openURL( url );
            if ( ret )
                resetURL();
            return ret;
        }
        case KMessageBox::No :
            unlink( QFile::encodeName( asf ) );
            return false;
        default: // Cancel
            return false;
        }
    }
    return false;
}

bool KoDocument::openURL( const KURL & _url )
{
    kdDebug(30003) << "KoDocument::openURL url=" << _url.url() << endl;
    d->lastErrorMessage = QString::null;

    // Reimplemented, to add a check for autosave files and to improve error reporting
    if ( _url.isMalformed() )
    {
        d->lastErrorMessage = i18n( "Malformed URL\n%1" ).arg( _url.url() ); // ## used anywhere ?
        return false;
    }
    if ( !closeURL() )
        return false;
    KURL url( _url );
    bool autosaveOpened = false;
    if ( url.isLocalFile() && d->m_shouldCheckAutoSaveFile )
    {
        QString file = url.path();
        QString asf = autoSaveFile( file );
        if ( QFile::exists( asf ) )
        {
            //kdDebug(30003) << "KoDocument::openURL asf=" << asf << endl;
            // ## TODO compare timestamps ?
            int res = KMessageBox::warningYesNoCancel( 0,
                                                       i18n( "An autosaved file exists for this document.\nDo you want to open it instead?" ));
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
    bool ret = KParts::ReadWritePart::openURL( url );

    if ( autosaveOpened )
        resetURL(); // Force save to act like 'Save As'
    else
    {
        // We have no koffice shell when we are being embedded as a readonly part.
        //if ( d->m_shells.isEmpty() )
        //    kdWarning(30003) << "KoDocument::openURL no shell yet !" << endl;
        // Add to recent actions list in our shells
        QPtrListIterator<KoMainWindow> it( d->m_shells );
        for (; it.current(); ++it )
            it.current()->addRecentURL( _url );
    }
    return ret;
}

bool KoDocument::openFile()
{
    //kdDebug(30003) << "KoDocument::openFile for " << m_file << endl;
    if ( ! QFile::exists(m_file) )
    {
        QApplication::restoreOverrideCursor();
        // Maybe offer to create a new document with that name ?
        KMessageBox::error(0L, i18n("The file %1 doesn't exist.").arg(m_file) );
        return false;
    }

    QApplication::setOverrideCursor( waitCursor );

    if ( d->m_bSingleViewMode && !d->m_views.isEmpty() )
    {
        // We already had a view (this happens when doing reload in konqueror)
        KoView* v = d->m_views.first();
        removeView( v );
        delete v;
        Q_ASSERT( d->m_views.isEmpty() );
    }

    d->m_specialOutputFlag = 0;
    QCString _native_format = nativeFormatMimeType();

    KURL u;
    u.setPath( m_file );
    QString typeName = KMimeType::findByURL( u, 0, true )->name();

    // Special case for flat XML files (e.g. using directory store)
    if ( u.fileName() == "maindoc.xml" || typeName == "inode/directory" )
    {
        typeName = _native_format; // Hmm, what if it's from another app? ### Check mimetype
        d->m_specialOutputFlag = SaveAsDirectoryStore;
        kdDebug(30003) << "KoDocument::openFile loading maindoc.xml, using directory store for " << m_file << endl;
    }
    kdDebug(30003) << "KoDocument::openFile " << m_file << " type:" << typeName << endl;

    QString importedFile = m_file;

    if ( typeName == KMimeType::defaultMimeType() ) {
        kdError(30003) << "No mimetype found for " << m_file << endl;
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0L, i18n( "Could not open\n%1" ).arg( url().prettyURL() ) );
        return false;
    }

    if ( typeName.latin1() != _native_format ) {
        if ( !d->filterManager )
            d->filterManager = new KoFilterManager( this );
        KoFilter::ConversionStatus status;
        importedFile = d->filterManager->import( m_file, status );
        if ( status != KoFilter::OK )
        {
            QApplication::restoreOverrideCursor();
            if ( status != KoFilter::UserCancelled )
                // Any way of passing a better error message from the filter?
                KMessageBox::error( 0L, i18n( "Could not open\n%1" ).arg( url().prettyURL() ) );
            return false;
        }
        kdDebug(30003) << "KoDocument::openFile - importedFile '" << importedFile
                       << "', status: " << static_cast<int>( status ) << endl;
    }

    QApplication::restoreOverrideCursor();

    bool ok = true;

    if (!importedFile.isEmpty()) // Something to load (tmp or native file) ?
    {
        // The filter, if any, has been applied. It's all native format now.
        if ( !loadNativeFormat( importedFile ) )
        {
            ok = false;
            if ( d->lastErrorMessage.isEmpty() )
                KMessageBox::error( 0L, i18n( "Could not open\n%1" ).arg( url().prettyURL() ) );
            else if ( d->lastErrorMessage != "USER_CANCELED" )
                KMessageBox::error( 0L, i18n( "Could not open %1\nReason: %2" ).arg( url().prettyURL() ).arg( d->lastErrorMessage ) );
        }
    }

    if ( importedFile != m_file )
    {
        // We opened a temporary file (result of an import filter)
        // Set document URL to empty - we don't want to save in /tmp !
        // But only if in readwrite mode (no saving problem otherwise)
        if ( isReadWrite() )
            resetURL();
        // and remove temp file - uncomment this to debug import filters
        if(!importedFile.isEmpty())
            unlink( QFile::encodeName(importedFile) );
    }

    if ( ok && d->m_bSingleViewMode )
    {
        // See addClient below
        KXMLGUIFactory* guiFactory = factory();
        if( guiFactory ) // 0L when splitting views in konq, for some reason
            guiFactory->removeClient( this );

        KoView *view = createView( d->m_wrapperWidget );
        d->m_wrapperWidget->setKoView( view );
        view->show();

        // Ok, now we have a view, so action() and domDocument() will work as expected
        // -> rebuild GUI
        if ( guiFactory )
            guiFactory->addClient( this );
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

    QFile in;
    bool isRawXML = false;
    if ( d->m_specialOutputFlag != SaveAsDirectoryStore ) // Don't try to open a directory ;)
    {
        in.setName(file);
        if ( !in.open( IO_ReadOnly ) )
        {
            QApplication::restoreOverrideCursor();
            d->lastErrorMessage = i18n( "Couldn't open the file for reading (check read permissions)" );
            return false;
        }

        // Try to find out whether it is a mime multi part file
        char buf[5];
        if ( in.readBlock( buf, 4 ) < 4 )
        {
            QApplication::restoreOverrideCursor();
            in.close();
            d->lastErrorMessage = i18n( "Couldn't read the beginning of the file" );
            return false;
        }
        isRawXML = (strncasecmp( buf, "<?xm", 4 ) == 0);
        //kdDebug(30003) << "PATTERN=" << buf << endl;
    }
    // Is it plain XML?
    if ( isRawXML )
    {
        in.at(0);
        QString errorMsg;
        int errorLine;
        int errorColumn;
        QDomDocument doc;
        bool res;
        if ( doc.setContent( &in , &errorMsg, &errorLine, &errorColumn ) )
        {
            res = loadXML( &in, doc );
            if ( res )
                res = completeLoading( 0L );
        }
        else
        {
            kdError (30003) << "Parsing Error! Aborting! (in KoDocument::loadNativeFormat (QFile))" << endl
                            << "  Line: " << errorLine << " Column: " << errorColumn << endl
                            << "  Message: " << errorMsg << endl;
            d->lastErrorMessage = i18n( "Parsing error in file at line %1, column %2" )
                                  .arg( errorLine ).arg( errorColumn );
            res=false;
        }

        QApplication::restoreOverrideCursor();
        in.close();
        m_bEmpty = false;
        return res;
    } else
    { // It's a koffice store (tar.gz, zip, directory, etc.)
        in.close();
        KoStore::Backend backend = (d->m_specialOutputFlag == SaveAsDirectoryStore) ? KoStore::Directory : KoStore::Auto;
        KoStore * store = KoStore::createStore( file, KoStore::Read, "", backend );

        if ( store->bad() )
        {
            d->lastErrorMessage = i18n( "Not a valid KOffice file" );
            delete store;
            QApplication::restoreOverrideCursor();
            return false;
        }

        if ( store->open( "root" ) )
        {
            QString errorMsg;
            int errorLine;
            int errorColumn;
            QDomDocument doc;
            if ( !doc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn ) )
            {
                kdError (30003) << "Parsing Error! Aborting! (in KoDocument::loadNativeFormat (KoStore))" << endl
                                << "  Line: " << errorLine << " Column: " << errorColumn << endl
                                << "  Message: " << errorMsg << endl;
                d->lastErrorMessage = i18n( "Parsing error in file at line %1, column %2" )
                                      .arg( errorLine ).arg( errorColumn );
                delete store;
                QApplication::restoreOverrideCursor();
                return false;
            }
            if ( !loadXML( store->device(), doc ) )
            {
                delete store;
                QApplication::restoreOverrideCursor();
                return false;
            }
            store->close();
        } else
        {
            kdError(30003) << "ERROR: No maindoc.xml" << endl;
            d->lastErrorMessage = i18n( "Invalid document: no file 'maindoc.xml'" );
            delete store;
            QApplication::restoreOverrideCursor();
            return false;
        }

        if ( !loadChildren( store ) )
        {
            kdError(30003) << "ERROR: Could not load children" << endl;
// Proceed nonetheless
#if 0
            if ( d->lastErrorMessage.isEmpty() )
                d->lastErrorMessage = i18n( "Couldn't load embedded objects" );
            delete store;
            QApplication::restoreOverrideCursor();
            return false;
#endif
        }
        if ( store->open( "documentinfo.xml" ) )
        {
            QDomDocument doc;
            doc.setContent( store->device() );
            d->m_docInfo->load( doc );
            store->close();
        }
        else
        {
            //kdDebug( 30003 ) << "cannot open document info" << endl;
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

bool KoDocument::loadFromStore( KoStore* _store, const QString& url )
{
    if ( _store->open( url ) )
    {
        QDomDocument doc;
        doc.setContent( _store->device() );
        if ( !loadXML( _store->device(), doc ) )
        {
            _store->close();
            return false;
        }
        _store->close();
    }

    _store->pushDirectory();
    // Store as document URL
    if ( url.startsWith( STORE_PROTOCOL ) )
        m_url = KURL( url );
    else {
        m_url = KURL( INTERNAL_PREFIX + url );
        _store->enterDirectory( url );
    }

    if ( !loadChildren( _store ) )
    {
        kdError(30003) << "ERROR: Could not load children" << endl;
#if 0
        return false;
#endif
    }

    bool result = completeLoading( _store );

    // Restore the "old" path
    _store->popDirectory();

    return result;
}

bool KoDocument::isInOperation()
{
    return d->m_numOperations > 0;
}

void KoDocument::emitBeginOperation(bool waitCursor)
{
    if (waitCursor)
    {
        QApplication::setOverrideCursor(Qt::waitCursor);
    }
    /* just duplicate the current cursor on the stack, then */
    else if (QApplication::overrideCursor() != NULL)
    {
        QApplication::setOverrideCursor(QApplication::overrideCursor()->shape());
    }

    /* if we're already in an operation, don't send the signal again */
    if (!isInOperation())
        emit sigBeginOperation();
    d->m_numOperations++;
}

void KoDocument::emitEndOperation()
{
    d->m_numOperations--;

    /* don't end the operation till we've cleared all the nested operations */
    if (d->m_numOperations == 0)
        emit sigEndOperation();
    else if (d->m_numOperations < 0)
        /* ignore 'end' calls with no matching 'begin' call */
        d->m_numOperations = 0;
    QApplication::restoreOverrideCursor();
}


bool KoDocument::isStoredExtern()
{
    return ( !url().protocol().isEmpty() && url().protocol() != STORE_PROTOCOL && url().protocol() != INTERNAL_PROTOCOL );
}

void KoDocument::setModified( bool mod )
{
    //kdDebug()<<k_funcinfo<<" url:" << m_url.path() << endl;
    //kdDebug()<<k_funcinfo<<" mod="<<mod<<" MParts mod="<<KParts::ReadWritePart::isModified()<<" isModified="<<isModified()<<endl;
    
    d->modifiedAfterAutosave=mod;
    if ( mod == KParts::ReadWritePart::isModified() )
        return;
    
    KParts::ReadWritePart::setModified( mod );

    if ( mod )
        m_bEmpty = FALSE;

    // This influences the title
    setTitleModified();
}

void KoDocument::setDoNotSaveExtDoc( bool on )
{ 
    d->m_doNotSaveExtDoc = on; 
}

int KoDocument::queryCloseDia()
{
    //kdDebug()<<k_funcinfo<<endl;

    QString name;
    if ( documentInfo() )
    {
        name = documentInfo()->title();
    }
    if ( name.isEmpty() )
        name = url().fileName();

    if ( name.isEmpty() )
        name = i18n( "Untitled" );
    
    int res = KMessageBox::warningYesNoCancel( 0L,
                    i18n( "<p>The document <b>'%1'</b> has been modified.</p><p>Do you want to save it?</p>" ).arg(name));

    switch(res)
    {
        case KMessageBox::Yes :
            setDoNotSaveExtDoc(); // Let save() only save myself and my internal docs
            save(); // NOTE: External files always in native format. ###TODO: Handle non-native format
            setModified( false ); // Now when queryClose() is called by closeEvent it won't do anything.
            break;
        case KMessageBox::No :
            removeAutoSaveFiles();
            setModified( false ); // Now when queryClose() is called by closeEvent it won't do anything.
            break;
        default : // case KMessageBox::Cancel :
            return res; // cancels the rest of the files
    }
    return res;
}

int KoDocument::queryCloseExternalChildren()
{
    //kdDebug()<<k_funcinfo<<" checking for children in: "<<url().url()<<endl;
    setDoNotSaveExtDoc(false);
    QPtrListIterator<KoDocumentChild> it( children() );
    for (; it.current(); ++it )
    {
        if ( !it.current()->isDeleted() )
        {
            KoDocument *doc = it.current()->document();
            if ( doc )
            {
                if ( it.current()->isStoredExtern() ) //###TODO: Handle non-native mimetype docs 
                {
                    if ( doc->isModified() )
                    {   
                        kdDebug()<<k_funcinfo<<" found modified child: "<<doc->url().url()<<endl;
                        if ( doc->queryCloseDia() == KMessageBox::Cancel )
                            return  KMessageBox::Cancel;
                    }
                }
                if ( doc->queryCloseExternalChildren() == KMessageBox::Cancel )
                    return KMessageBox::Cancel;
            }
        }
    }
    return KMessageBox::Ok;
}

void KoDocument::setTitleModified()
{
    //kdDebug()<<k_funcinfo<<" this doc: "<<url().url()<<" shells="<<shellCount()<<endl;
    if ( !isStoredExtern() )  //###TODO: Handle caption when external embedded doc is selected/modified
    {
        KoDocument *doc = dynamic_cast<KoDocument *>( parent() );
        if ( doc )
        {
            //kdDebug()<<k_funcinfo<<" Embedded in: "<<doc->url().url()<<endl;
            doc->setTitleModified();
        }
        return;
    }
        
    // Update caption in all related windows
    QPtrListIterator<KoMainWindow> it( d->m_shells );
    for (; it.current(); ++it )
        it.current()->updateCaption();
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

QDomDocument KoDocument::createDomDocument( const QString& tagName, const QString& version ) const
{
    return createDomDocument( instance()->instanceName(), tagName, version );
}

//static
QDomDocument KoDocument::createDomDocument( const QString& appName, const QString& tagName, const QString& version )
{
    QDomImplementation impl;
    QString url = QString("http://www.koffice.org/DTD/%1-%1.dtd").arg(appName).arg(version);
    QDomDocumentType dtype = impl.createDocumentType( tagName,
                                                      QString("-//KDE//DTD %1 %1//EN").arg(appName).arg(version),
                                                      url );
    // The namespace URN doesn't need to include the version number.
    QString namespaceURN = QString("http://www.koffice.org/DTD/%1").arg(appName);
    QDomDocument doc = impl.createDocument( namespaceURN, tagName, dtype );
    doc.insertBefore( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ), doc.documentElement() );
    return doc;
}

QDomDocument KoDocument::saveXML()
{
    kdError(30003) << "KoDocument::saveXML not implemented" << endl;
    d->lastErrorMessage = i18n( "Internal error: saveXML not implemented" );
    return QDomDocument();
}

KService::Ptr KoDocument::nativeService()
{
    if ( !m_nativeService )
        m_nativeService = readNativeService( instance() );

    return m_nativeService;
}

QCString KoDocument::nativeFormatMimeType() const
{
    KService::Ptr service = const_cast<KoDocument *>(this)->nativeService();
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
        service = KService::serviceByDesktopName( instname );

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

const QPtrList<KoMainWindow>& KoDocument::shells() const
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
        d->m_dcopObject = new KoDocumentIface( this );
    return d->m_dcopObject;
}

QCString KoDocument::dcopObjectId() const
{
    return const_cast<KoDocument *>(this)->dcopObject()->objId();
}

void KoDocument::setErrorMessage( const QString& errMsg )
{
    d->lastErrorMessage = errMsg;
}

bool KoDocument::isAutosaving()
{
    return d->m_autosaving;
}

void KoDocument::removeAutoSaveFiles()
{
        // Eliminate any auto-save file
        QString asf = autoSaveFile( m_file ); // the one in the current dir
        if ( QFile::exists( asf ) )
            unlink( QFile::encodeName( asf ) );
        asf = autoSaveFile( QString::null ); // and the one in $HOME
        if ( QFile::exists( asf ) )
            unlink( QFile::encodeName( asf ) );
}

void KoDocument::setBackupFile( bool _b )
{
    d->m_backupFile = _b;
}

bool KoDocument::backupFile()const
{
    return d->m_backupFile;
}


void KoDocument::setBackupPath( const QString & _path)
{
    d->m_backupPath = _path;
}

QString KoDocument::backupPath()const
{
    return d->m_backupPath;
}


#include <koDocument.moc>
#include <koDocument_p.moc>
