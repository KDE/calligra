#include <qprinter.h>
#include <qmsgbox.h>

#include "koshell_shell.h"

#include <kfiledialog.h>
#include <klocale.h>
#include <kdebug.h>
#include <kded_instance.h>
#include <kactivator.h>

#include <opMainWindowIf.h>
#include <opMenuBarManager.h>
#include <opToolBar.h>
#include <opMenu.h>
#include <opApplication.h>

#include <koAboutDia.h>
#include <koFrame.h>
#include <koQueryTypes.h>
#include <koKoolBar.h>

QList<KoShellWindow>* KoShellWindow::s_lstShells = 0L;

KoShellWindow::KoShellWindow()
{
  m_activePage = m_lstPages.end();
  
  if ( s_lstShells == 0L )
    s_lstShells = new QList<KoShellWindow>;
  
  s_lstShells->append( this );

  QWidget *w = new QWidget( opToolBar(1) );
  QBoxLayout *ml = new QVBoxLayout(w);

  m_pKoolBar = new KoKoolBar( w );
  m_grpFile = m_pKoolBar->insertGroup("Parts");
  m_lstComponents = KoDocumentEntry::query();
  QValueList<KoDocumentEntry>::Iterator it = m_lstComponents.begin();
  for( ; it != m_lstComponents.end(); ++it )
    if ( !(*it).icon.isNull() )
    {
      int id = m_pKoolBar->insertItem( m_grpFile, (*it).icon, (*it).name,
				       this, SLOT( slotKoolBar( int, int ) ) );
      m_mapComponents[ id ] = &*it;
    }
  
  m_grpDocuments = m_pKoolBar->insertGroup("Documents");
  m_pKoolBar->insertGroup("Snippets");

  m_pKoolBar->setFixedWidth( 80 );
  m_pKoolBar->setMinimumHeight( 300 );
  
  ml->addWidget(m_pKoolBar);
  ml->activate();

  opToolBar(1)->insertWidget( 1, 80, w );
  
  opToolBar(1)->setBarPos(KToolBar::Left);

  m_pKfmFrame = m_pFrame;

  this->resize(550, 400);
}

KoShellWindow::~KoShellWindow()
{ 
  kdebug( KDEBUG_INFO, 0, "KoShellWindow::~KoShellWindow()" );
  
  cleanUp();
  
  s_lstShells->removeRef( this );
}

void KoShellWindow::slotKoolBar( int _grp, int _item )
{
  if ( _grp == m_grpFile )
  {
    newPage( *(m_mapComponents[ _item ]) );
  }
  else if ( _grp == m_grpDocuments )
  {
    list<Page>::iterator it = m_lstPages.begin();
    while( it != m_lstPages.end() )
    {
      if ( it->m_id == _item )
      {
	if ( m_pFrame )
	  m_pFrame->hide();
	m_activePage = it;
	m_pFrame = it->m_pFrame;
	setView( m_pFrame );
	m_pFrame->show();
	interface()->setActivePart( it->m_vView->id() );
	return;
      }
      ++it;
    }
  }
}

bool KoShellWindow::isModified()
{
  list<Page>::iterator it = m_lstPages.begin();
  for( ; it != m_lstPages.end(); ++it )
    if ( it->m_vDoc->isModified() )
      return true;
  
  return false;
}

bool KoShellWindow::requestClose()
{
  int res = QMessageBox::warning( 0L, i18n("Warning"), i18n("The document has been modified\nDo you want to save it ?" ),
				  i18n("Yes"), i18n("No"), i18n("Cancel") );
  
  if ( res == 0 )
    return saveAllPages();
  
  if ( res == 1 )
    return true;
  
  return false;
}

void KoShellWindow::cleanUp()
{
  releasePages();

  KoMainWindow::cleanUp();
}

void KoShellWindow::attachView( KoFrame* _frame, unsigned long _part_id )
{
  OpenParts::Part_var part;
  if ( _part_id != 0 )
  {    
    part = interface()->findPart( _part_id );
    assert( !CORBA::is_nil( part ) );
  }
  
  _frame->detach();
  
  if ( _part_id != 0 )
  {
    KOffice::View_var view = KOffice::View::_narrow( part );    
    assert( !CORBA::is_nil( view ) );
    _frame->attachView( view );
  }
}

/*
void KoShellWindow::setDocument( KImageDoc *_doc )
{
  if ( m_pDoc )
    releaseDocument();
  
  m_pDoc = _doc;
  m_pDoc->_ref();
  m_pView = _doc->createImageView();
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  m_pView->setMainWindow( interface() );
  
  setRootPart( m_pView->id() );
  interface()->setActivePart( m_pView->id() );

  if( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
  }
  
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
}
*/
bool KoShellWindow::newPage( KoDocumentEntry& _e )
{
  // Create document
  KOffice::DocumentFactory_var factory = KOffice::DocumentFactory::_narrow( _e.reference );
  if( CORBA::is_nil( factory ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Server %s is not document factory" ), _e.name.data() );
    QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), tmp, i18n( "Ok" ) );
    return false;
  }
  
  KOffice::Document_var doc = factory->create();
  if( CORBA::is_nil( doc ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Server %s did not create a document" ), _e.name.data() );
    QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), tmp, i18n( "Ok" ) );
    return false;
  }

  doc->initDoc();

  // Create a view
  OpenParts::View_var v = doc->createView();
  if ( CORBA::is_nil( v ) )
  {
    QMessageBox::critical( 0L, i18n("KOffice Error"), i18n("Could not create a view") );
    return false;
  }
  
  KOffice::View_var view;
  view = KOffice::View::_narrow( v );
  if ( CORBA::is_nil( view ) )
  {
    QMessageBox::critical( 0L, i18n("KOffice Error"), i18n("Could not create a koffice compliant view") );
    return false;
  }
  view->setMode( KOffice::View::RootMode );
  view->setMainWindow( this->koInterface() );

  // Create a new frame
  KoFrame* frame = new KoFrame( this );
  if ( m_pFrame )
    m_pFrame->hide();
  m_pFrame = frame;
  setView( m_pFrame );
  m_pFrame->show();
  
  attachView( frame, view->id() );
  interface()->setActivePart( view->id() );
  
  // Create a new page
  Page page;
  page.m_vDoc = KOffice::Document::_duplicate( doc );
  page.m_vView = KOffice::View::_duplicate( view );
  page.m_pFrame = frame;
  page.m_id = m_pKoolBar->insertItem( m_grpDocuments, _e.icon, i18n("No name"), this,
				      SLOT( slotKoolBar( int, int ) ) );
  
  m_lstPages.push_back( page );
  m_activePage = m_lstPages.end();
  m_activePage--;
  
  if( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
  }
  
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
  opToolBar()->setFullWidth(false);

  return true;
}
/*
bool KoShellWindow::openDocument( const char *_url, const char *_format )
{
  if ( _format == 0L || *_format == 0 )
    _format = "application/x-kimage";

  if ( m_pDoc && m_pDoc->isEmpty() )
    releaseDocument();
  else if ( m_pDoc && !m_pDoc->isEmpty() )
  {
    KoShellWindow *s = new KoShellWindow();
    s->show();
    return s->openDocument( _url, _format );
  }
  
  cerr << "Creating new document" << endl;
  
  m_pDoc = new KImageDoc;
  if ( !m_pDoc->loadFromURL( _url, _format ) )
    return false;
  
  m_pView = m_pDoc->createImageView();
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  m_pView->setMainWindow( interface() );
  
  setRootPart( m_pView->id() );
  interface()->setActivePart( m_pView->id() );
  
  if ( m_pFileMenu )
  {    
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
  }
  
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
  
  return true;
}

bool KoShellWindow::saveDocument( const char *_url, const char *_format )
{
  assert( m_pDoc != 0L );

  CORBA::String_var url;
  if ( _url == 0L || *_url == 0 )
  {
    url = m_pDoc->url();
    _url = url.in();
  }
  
  QString file;
  if ( _url == 0L || *_url == 0 )
  {
    file = KFileDialog::getSaveFileName( getenv( "HOME" ) );

    if ( file.isNull() )
      return false;
    _url = file.data();
  }
  
  if ( _format == 0L || *_format == 0 )
    _format = "application/x-kimage";
  
  return m_pDoc->saveToURL( _url, _format );
}
*/

bool KoShellWindow::saveAllPages()
{
  // TODO
  return false;
}

bool KoShellWindow::printDlg()
{
  assert( m_activePage != m_lstPages.end() );

  return m_activePage->m_vView->printDlg();
}

void KoShellWindow::helpAbout()
{
  // KoAboutDia::about( KoAboutDia::KImage, "0.0.2" );
}

bool KoShellWindow::closeApplication()
{
  KoShellWindow* s;
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

int KoShellWindow::documentCount()
{
  return s_lstShells->count();
}

void KoShellWindow::releasePages()
{
  // TODO
}

/*
void KoShellWindow::releaseDocument()
{
  int views = 0;
  if ( m_pDoc )
    views = m_pDoc->viewCount();
  cerr << "############## VIEWS=" << views << " #####################" << endl;
  
  cerr << "-1) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

  setRootPart( 0 );

  cerr << "-2) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

  interface()->setActivePart( 0 );

  // cerr << "-3) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
  
  if ( m_pView )
    m_pView->decRef();
  
  // cerr << "-4) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
  if ( m_pDoc && views <= 1 )
    m_pDoc->cleanUp();
  // cerr << "-5) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
  // if ( m_pView )
  // CORBA::release( m_pView );
  // cerr << "-6) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
  if ( m_pDoc )
    CORBA::release( m_pDoc );
  // cerr << "-7) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
  m_pView = 0L;
  m_pDoc = 0L;
}
*/

void KoShellWindow::slotFileNew()
{
  /*  if ( !newDocument() )    
      QMessageBox::critical( this, i18n("KImage Error"), i18n("Could not create new document"), i18n("OK") ); */
}

//void KoShellWindow::slotFileOpen()
//{
  /* QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if ( file.isNull() )
    return;
  
  if ( !openDocument( file, "" ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    } */
//}

void KoShellWindow::slotFileSave()
{
  /* assert( m_pDoc != 0L );
  
  CORBA::String_var url = m_pDoc->url();
  if ( strlen( url.in() ) == 0 )
  {
    slotFileSaveAs();
    return;
  }
  
  if ( !saveDocument( url.in(), "" ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not save\n%s" ), url.in() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    } */
}

void KoShellWindow::slotFileSaveAs()
{
  /* if ( !saveDocument( "", "" ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not save file" ) );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
    } */
}

void KoShellWindow::slotFileClose()
{
  /* if ( documentCount() <= 1 )
  {
    slotFileQuit();
    return;
  }
  
  if ( isModified() )
    if ( !requestClose() )
      return;
  
      delete this; */
}

void KoShellWindow::slotFilePrint()
{
  printDlg();
}

void KoShellWindow::slotFileQuit()
{
  cerr << "EXIT 1" << endl;

  if ( !closeApplication() )
    return;

  cerr << "EXIT 2" << endl;
  
  delete this;
  kapp->exit();
}

KOffice::Document_ptr KoShellWindow::document()
{
  if ( m_activePage == m_lstPages.end() )
    return 0L;
  
  return KOffice::Document::_duplicate( m_activePage->m_vDoc );
}

KOffice::View_ptr KoShellWindow::view()
{
  if ( m_activePage == m_lstPages.end() )
    return 0L;

  return KOffice::View::_duplicate( m_activePage->m_vView );
}

void KoShellWindow::slotFileOpen()
{

  if ( !CORBA::is_nil( m_vKfm ) )
  {
    if ( m_pFrame && m_pFrame != m_pKfmFrame )
      m_pFrame->hide();
    m_pFrame = m_pKfmFrame;
    m_pFrame->raise();
    setView( m_pFrame );
    m_pFrame->show();
    interface()->setActivePart( m_vKfm->id() );
    return;
  }
  
  KActivator *activator = KdedInstance::self()->kactivator();
  CORBA::Object_var obj = activator->activateService( "Konqueror", "IDL:Konqueror/Application:1.0", "App" );

  OpenParts::Application_var app = OpenParts::Application::_narrow( obj );
  assert( !CORBA::is_nil( app ) );

  if ( m_pFrame && m_pFrame != m_pKfmFrame )
    m_pFrame->hide();
  m_pFrame = m_pKfmFrame;
  m_pFrame->raise();
  setView( m_pFrame );
  m_pFrame->show();

  m_vKfm = app->createPart();
  assert( !CORBA::is_nil( m_vKfm ) );
  m_vKfm->setMainWindow( this->koInterface() );
  m_pFrame->OPFrame::attach( m_vKfm );
  interface()->setActivePart( m_vKfm->id() );
  
  if( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
  }
  
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
  opToolBar()->setFullWidth(false);
}

#include "koshell_shell.moc"
