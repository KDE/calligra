/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Shell                                                  */
/******************************************************************/

#include <qtimer.h>
#include <qmsgbox.h>
#include <qprinter.h>

#include "kword_shell.h"
#include "kword_doc.h"
#include "kword_view.h"
#include "kword_page.h"

#include <kimgio.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kapp.h>
#include <kfiledialog.h>
#include <kmimetypes.h>
#include <kmimemagic.h>
#include <kurl.h>

#include <opMainWindowIf.h>
#include <opMenuIf.h>

#include <koFilterManager.h>
#include <koAboutDia.h>

#include "preview.h"

QList<KWordShell>* KWordShell::s_lstShells = 0L;
bool KWordShell::previewHandlerRegistered = FALSE;

/******************************************************************/
/* Class: KWordShell                                              */
/******************************************************************/

/*================================================================*/
KWordShell::KWordShell()
{
    m_pDoc = 0L;
    m_pView = 0L;

    if ( s_lstShells == 0L )
        s_lstShells = new QList<KWordShell>;

    s_lstShells->append( this );

    if ( !previewHandlerRegistered ) {
        QStringList list = KImageIO::types(KImageIO::Reading);
        QStringList::ConstIterator it;
        for (it = list.begin(); it != list.end(); it++)
            KFilePreviewDialog::registerPreviewModule( *it, pixmapPreviewHandler, PreviewPixmap );

        KFilePreviewDialog::registerPreviewModule( "wmf", wmfPreviewHandler, PreviewPixmap );
        KFilePreviewDialog::registerPreviewModule( "WMF", wmfPreviewHandler, PreviewPixmap );

        previewHandlerRegistered = TRUE;
    }
    interface()->setMaximumToolBarWraps( 1 );
}

/*================================================================*/
KWordShell::~KWordShell()
{
    cerr << "KWordShell::~KWordShell()" << endl;

    cleanUp();

    s_lstShells->removeRef( this );
}

/*================================================================*/
bool KWordShell::isModified()
{
    if ( m_pDoc )
        return ( bool )m_pDoc->isModified();

    return FALSE;
}

/*================================================================*/
bool KWordShell::requestClose()
{
    int res = QMessageBox::warning( 0L, i18n( "Warning" ), i18n( "The document has been modified\nDo you want to save it ?" ),
                                    i18n( "Yes" ), i18n( "No" ), i18n( "Cancel" ) );

    if ( res == 0 )
        return saveDocument();

    if ( res == 1 )
        return TRUE;

    return FALSE;
}

/*================================================================*/
void KWordShell::cleanUp()
{
    releaseDocument();

    KoMainWindow::cleanUp();
}

/*================================================================*/
void KWordShell::setDocument( KWordDocument *_doc )
{
    if ( m_pDoc )
        releaseDocument();

    m_pDoc = _doc;
    m_pDoc->_ref();
    m_pView = _doc->createWordView( frame() );
    m_pView->setShell( this );
    m_pView->incRef();
    m_pView->setMode( KOffice::View::RootMode );
    m_pView->setMainWindow( interface() );

    setRootPart( m_pView );
    interface()->setActivePart( m_pView->id() );

    if( m_pFileMenu ) {
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, TRUE );
    }

    opToolBar()->setItemEnabled( TOOLBAR_PRINT, TRUE );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, TRUE );
    opToolBar()->setFullWidth( FALSE );
}

/*================================================================*/
bool KWordShell::newDocument()
{
    if ( m_pDoc ) {
        KWordShell *s = new KWordShell();
        s->show();
        s->newDocument();
        return TRUE;
    }

    m_pDoc = new KWordDocument;
    if ( !m_pDoc->initDoc() ) {
        releaseDocument();
        cerr << "ERROR: Could not initialize document" << endl;
        return FALSE;
    }

    m_pView = m_pDoc->createWordView( frame() );
    m_pView->setShell( this );
    m_pView->incRef();
    m_pView->setMode( KOffice::View::RootMode );
    cerr << "*1 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
    m_pView->setMainWindow( interface() );

    setRootPart( m_pView );
    interface()->setActivePart( m_pView->id() );

    if( m_pFileMenu ) {
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, TRUE );
    }

    opToolBar()->setItemEnabled( TOOLBAR_PRINT, TRUE );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, TRUE );
    opToolBar()->setFullWidth( FALSE );

    cerr << "*2 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

    return TRUE;
}

/*================================================================*/
bool KWordShell::openDocument( const char *_url, const char *_format )
{
    if ( _format == 0L || *_format == 0 )
        _format = "application/x-kword";

    if ( m_pDoc && m_pDoc->isEmpty() )
        releaseDocument();
    else if ( m_pDoc && !m_pDoc->isEmpty() ) {
        KWordShell *s = new KWordShell();
        s->show();
        return s->openDocument( _url, _format );
    }

    cerr << "Creating new document" << endl;

    m_pDoc = new KWordDocument;
    if ( !m_pDoc->loadFromURL( _url, _format ) )
        return FALSE;

    m_pView = m_pDoc->createWordView( frame() );
    m_pView->setShell( this );
    m_pView->incRef();
    m_pView->setMode( KOffice::View::RootMode );
    m_pView->setMainWindow( interface() );

    setRootPart( m_pView );
    interface()->setActivePart( m_pView->id() );

    if ( m_pFileMenu ) {
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, TRUE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, TRUE );
    }

    opToolBar()->setItemEnabled( TOOLBAR_PRINT, TRUE );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, TRUE );
    opToolBar()->setFullWidth( FALSE );

    m_pDoc->setURL( _url );

    return TRUE;
}

/*================================================================*/
bool KWordShell::saveDocument( const char *_url, const char *_format )
{
    return KoMainWindow::saveDocument( "application/x-kword", "*.kwd" );
}

/*================================================================*/
bool KWordShell::printDlg()
{
    assert( m_pView != 0L );

    return m_pView->printDlg();
}

/*================================================================*/
void KWordShell::helpAbout()
{
    // TODO
    // KoAboutDia::about( KoAboutDia::KWord, "0.0.2" );
}

/*================================================================*/
bool KWordShell::closeDocument()
{
    if ( isModified() ) {
        if ( !requestClose() )
            return FALSE;
    }

    return TRUE;
}

/*================================================================*/
bool KWordShell::closeAllDocuments()
{
    KWordShell* s;
    for( s = s_lstShells->first(); s != 0L; s = s_lstShells->next() ) {
        if ( s->isModified() ) {
            if ( !s->requestClose() )
                return FALSE;
        }
    }

    return TRUE;
}

/*================================================================*/
int KWordShell::documentCount()
{
    return s_lstShells->count();
}

/*================================================================*/
void KWordShell::releaseDocument()
{
    int views = 0;
    if ( m_pDoc )
        views = m_pDoc->viewCount();
    cerr << "############## VIEWS=" << views << " #####################" << endl;

    if ( m_pView )
        cerr << "-1 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

    setRootPart( ( long unsigned int )( OpenParts::Id )0 );

    if ( m_pView )
        cerr << "-2 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

    interface()->setActivePart( 0 );

    // if ( m_pView )
    // cerr << "-3 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

    if ( m_pView )
        m_pView->decRef();

    /* if (m_pView)
       m_pView->cleanUp(); */

    // if ( m_pView )
    // cerr << "-4 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
    if ( m_pDoc && views <= 1 )
        m_pDoc->cleanUp();
    // if ( m_pView )
    // cerr << "-5 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
    // if ( m_pView )
    // CORBA::release( m_pView );
    // if ( m_pView )
    // cerr << "-6 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
    if ( m_pDoc )
        CORBA::release( m_pDoc );
    // if ( m_pView )
    // cerr << "-7 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
    m_pView = 0L;
    m_pDoc = 0L;

    if( m_pFileMenu )
    {
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, FALSE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, FALSE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, FALSE );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, FALSE );
    }

    opToolBar()->setItemEnabled( TOOLBAR_PRINT, FALSE );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, FALSE );
}

/*================================================================*/
void KWordShell::slotFileNew()
{
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( FALSE );
    if ( !newDocument() )
        QMessageBox::critical( this, i18n( "KWord Error" ), i18n( "Could not create new document" ), i18n( "Ok" ) );
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( TRUE );
    if ( m_pView )
        m_pView->getGUI()->getPaperWidget()->repaintScreen( FALSE );
}

/*================================================================*/
void KWordShell::slotFileOpen()
{
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( FALSE );

    QString filter = KoFilterManager::self()->fileSelectorList( KoFilterManager::Import,
                                                                "application/x-kword",
                                                                "*.kwd", "KWord",
                                                                TRUE );

    QString file = KFileDialog::getOpenFileName( getenv( "HOME" ), filter );
    if ( file.isNull() )
	return;

    file = KoFilterManager::self()->import( file, "application/x-kword" );
    if ( file.isNull() )
	return;

//     QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

//     if ( file.isNull() )
//         return;

    if ( !openDocument( file, "" ) ) {
        QString tmp;
        tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
        QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    }
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( TRUE );
    if ( m_pView )
        m_pView->getGUI()->getPaperWidget()->repaintScreen( FALSE );
}

/*================================================================*/
void KWordShell::slotFileSave()
{
    assert( m_pDoc != 0L );

    m_pDoc->enableEmbeddedParts( FALSE );

    (void) saveDocument();

    m_pDoc->enableEmbeddedParts( TRUE );
    m_pView->getGUI()->getPaperWidget()->repaintScreen( FALSE );
}

/*================================================================*/
void KWordShell::slotFileSaveAs()
{
    m_pDoc->enableEmbeddedParts( FALSE );

    QString _url = m_pDoc->url();
    m_pDoc->setURL( "" );

    if ( !saveDocument() )
      m_pDoc->setURL( _url );

    m_pDoc->enableEmbeddedParts( TRUE );
    m_pView->getGUI()->getPaperWidget()->repaintScreen( FALSE );
}

/*================================================================*/
void KWordShell::slotFileClose()
{
    if ( isModified() )
        if ( !requestClose() )
            return;

    releaseDocument();
}

/*================================================================*/
void KWordShell::slotFilePrint()
{
    assert( m_pView );

    ( void )m_pView->printDlg();
}

/*================================================================*/
void KWordShell::slotFileQuit()
{
    cerr << "EXIT 1" << endl;

    if ( !closeAllDocuments() )
        return;

    cerr << "EXIT 2" << endl;

    delete this;
    kapp->exit();
}

/*================================================================*/
KOffice::Document_ptr KWordShell::document()
{
    return KOffice::Document::_duplicate( m_pDoc );
}

/*================================================================*/
KOffice::View_ptr KWordShell::view()
{
    return KOffice::View::_duplicate( m_pView );
}

#include "kword_shell.moc"
