/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Shell                                       */
/******************************************************************/

#include <qprinter.h>
#include <qmsgbox.h>
#include <qtimer.h>

#include "kpresenter_shell.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "page.h"
#include "preview.h"

#include <koAboutDia.h>

#include <opMainWindowIf.h>
#include <opMenuIf.h>

#include <kfiledialog.h>
#include <kapp.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kimgio.h>
#include <koFilterManager.h>
#include <kurl.h>

#include <stdlib.h>

QList<KPresenterShell>* KPresenterShell::s_lstShells = 0L;
bool KPresenterShell::previewHandlerRegistered = false;

/*****************************************************************/
/* class KPresenterShell                                         */
/*****************************************************************/

/*================================================================*/
KPresenterShell::KPresenterShell()
{
    m_pDoc = 0L;
    m_pView = 0L;

    if ( !previewHandlerRegistered )
    {
        KFilePreviewDialog::registerPreviewModule( "wmf", wmfPreviewHandler, PreviewPixmap );
	KFilePreviewDialog::registerPreviewModule( "WMF", wmfPreviewHandler, PreviewPixmap );
	
	QStringList list = KImageIO::types(KImageIO::Reading);
	QStringList::ConstIterator it;
	for (it = list.begin(); it != list.end(); it++)
        KFilePreviewDialog::registerPreviewModule( *it, pixmapPreviewHandler, PreviewPixmap );
    }

    if ( s_lstShells == 0L )
        s_lstShells = new QList<KPresenterShell>;

    s_lstShells->append( this );
    interface()->setMaximumToolBarWraps( 1 );
}

/*================================================================*/
KPresenterShell::~KPresenterShell()
{
    cerr << "KPresenterShell::~KPresenterShell()" << endl;

    cleanUp();

    s_lstShells->removeRef( this );
}

/*================================================================*/
bool KPresenterShell::isModified()
{
    if ( m_pDoc )
        return ( bool )m_pDoc->isModified();

    return false;
}

/*================================================================*/
bool KPresenterShell::requestClose()
{
    int res = QMessageBox::warning( 0L, i18n( "Warning" ), i18n( "The document has been modified\nDo you want to save it ?" ),
                                    i18n( "Yes" ), i18n( "No" ), i18n( "Cancel" ) );

    if ( res == 0 )
        return saveDocument();

    if ( res == 1 )
        return true;

    return false;
}

/*================================================================*/
void KPresenterShell::cleanUp()
{
    releaseDocument();

    KoMainWindow::cleanUp();
}

/*================================================================*/
void KPresenterShell::setDocument( KPresenterDoc *_doc )
{
    if ( m_pDoc )
        releaseDocument();

    m_pDoc = _doc;
    m_pDoc->_ref();
    m_pView = _doc->createPresenterView( frame() );
    m_pView->setShell( this );
    m_pView->incRef();
    m_pView->setMode( KOffice::View::RootMode );
    m_pView->setMainWindow( interface() );

    setRootPart( m_pView );
    interface()->setActivePart( m_pView->id() );

    if( m_pFileMenu )
    {
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
    }

    opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
    opToolBar()->setFullWidth( false );
}

/*================================================================*/
bool KPresenterShell::newDocument()
{
    if ( m_pDoc )
    {
        KPresenterShell *s = new KPresenterShell();
        s->show();
        s->newDocument();
        return true;
    }

    m_pDoc = new KPresenterDoc;
    if ( !m_pDoc->initDoc() )
    { // user selected cancel
        releaseDocument();
        return false;
    }

    m_pView = m_pDoc->createPresenterView( frame() );
    m_pView->setShell( this );
    m_pView->incRef();
    m_pView->setMode( KOffice::View::RootMode );
    cerr << "*1 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
    m_pView->setMainWindow( interface() );

    setRootPart( m_pView );

    interface()->setActivePart( m_pView->id() );

    if ( m_pFileMenu )
    {
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
    }

    opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
    opToolBar()->setFullWidth( false );

    cerr << "*2 ) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

    return true;
}

/*================================================================*/
bool KPresenterShell::openDocument( const char *_url, const char *_format )
{
    if ( _format == 0L || *_format == 0 )
        _format = "application/x-kpresenter";

    if ( m_pDoc && m_pDoc->isEmpty() )
        releaseDocument();
    else if ( m_pDoc && !m_pDoc->isEmpty() )
    {
        KPresenterShell *s = new KPresenterShell();
        s->show();
        return s->openDocument( _url, _format );
    }

    cerr << "Creating new document" << endl;

    m_pDoc = new KPresenterDoc;
    if ( !m_pDoc->loadFromURL( _url, _format ) )
        return false;

    m_pView = m_pDoc->createPresenterView( frame() );
    m_pView->setShell( this );
    m_pView->incRef();
    m_pView->setMode( KOffice::View::RootMode );
    m_pView->setMainWindow( interface() );

    setRootPart( m_pView );
    interface()->setActivePart( m_pView->id() );

    if ( m_pFileMenu )
    {
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
    }

    opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
    opToolBar()->setFullWidth( false );

    m_pDoc->setURL( _url );

    return true;
}

/*================================================================*/
bool KPresenterShell::saveDocument()
{
    return KoMainWindow::saveDocument( "application/x-kpresenter", "*.kpr", "KPresenter" );
}

/*================================================================*/
bool KPresenterShell::printDlg()
{
    assert( m_pView != 0L );

    return m_pView->printDlg();
}

/*================================================================*/
void KPresenterShell::helpAbout()
{
    KoAboutDia::about( KoAboutDia::KPresenter, "0.1.0" );
}

/*================================================================*/
bool KPresenterShell::closeDocument()
{
    if ( isModified() )
    {
        if ( !requestClose() )
            return false;
    }

    return true;
}

/*================================================================*/
bool KPresenterShell::closeAllDocuments()
{
    KPresenterShell* s;
    for( s = s_lstShells->first(); s != 0L; s = s_lstShells->next() )
    {
        if ( s->isModified() )
        {
            if ( !s->requestClose() )
                return false;
        }
    }

    return true;
}

/*================================================================*/
int KPresenterShell::documentCount()
{
    return s_lstShells->count();
}

/*================================================================*/
void KPresenterShell::releaseDocument()
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
        m_pFileMenu->setItemEnabled( m_idMenuFile_Save, false );
        m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, false );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Print, false );
        m_pFileMenu->setItemEnabled( m_idMenuFile_Close, false );
    }
    opToolBar()->setItemEnabled( TOOLBAR_PRINT, false );
    opToolBar()->setItemEnabled( TOOLBAR_SAVE, false );
}

/*================================================================*/
void KPresenterShell::slotFileNew()
{
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( false );
    if ( !newDocument() )
        ;
    // QMessageBox::critical( this, i18n( "KPresenter Error" ), i18n( "Could not create new document" ), i18n( "Ok" ) );
    // no error message. user simply selected cancel.
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( true );
    if ( m_pView )
        m_pView->getPage()->repaint( false );
}

/*================================================================*/
void KPresenterShell::slotFileOpen()
{
    if ( m_pView )
        m_pView->getPage()->setToolEditMode( TEM_MOUSE );
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( false );

    QString filter = KoFilterManager::self()->fileSelectorList( KoFilterManager::Import,
                                                                "application/x-kpresenter",
                                                                "*.kpr", "KPresenter",
                                                                TRUE );

    QString file = KFileDialog::getOpenFileName( getenv( "HOME" ), filter );
    if ( file.isNull() )
	return;

    file = KoFilterManager::self()->import( file, "application/x-kpresenter" );
    if ( file.isNull() )
	return;

    if ( !openDocument( file, "" ) )
    {
        QString tmp;
        tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
        QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    }
    if ( m_pDoc )
        m_pDoc->enableEmbeddedParts( true );
    if ( m_pView )
        m_pView->getPage()->repaint( false );
}

/*================================================================*/
void KPresenterShell::slotFileSave()
{
    assert( m_pDoc != 0L );

    m_pDoc->enableEmbeddedParts( false );

    (void) saveDocument();

    m_pDoc->enableEmbeddedParts( true );
    m_pView->getPage()->repaint( false );
}

/*================================================================*/
void KPresenterShell::slotFileSaveAs()
{
    m_pDoc->enableEmbeddedParts( false );

    QString _url = m_pDoc->url();
    m_pDoc->setURL( "" );

    if ( !saveDocument() )
      m_pDoc->setURL( _url );

    m_pDoc->enableEmbeddedParts( true );
    m_pView->getPage()->repaint( false );
}

/*================================================================*/
void KPresenterShell::slotFileClose()
{
    /*
      if ( documentCount() <= 1 )
      {
      slotFileQuit();
      return;
      }
    */

    if ( isModified() )
        if ( !requestClose() )
            return;

    releaseDocument();
    //delete this;
}

/*================================================================*/
void KPresenterShell::slotFilePrint()
{
    assert( m_pView );

    ( void )m_pView->printDlg();
}

/*================================================================*/
void KPresenterShell::slotFileQuit()
{
    cerr << "EXIT 1" << endl;

    if ( !closeAllDocuments() )
        return;

    cerr << "EXIT 2" << endl;

    delete this;
    kapp->exit();
}

/*================================================================*/
KOffice::Document_ptr KPresenterShell::document()
{
    return KOffice::Document::_duplicate( m_pDoc );
}

/*================================================================*/
KOffice::View_ptr KPresenterShell::view()
{
    return KOffice::View::_duplicate( m_pView );
}

#include "kpresenter_shell.moc"



