/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qprinter.h>
#include <qmsgbox.h>
#include <qhbox.h>

#include "koshell_shell.h"

#include <kfiledialog.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <klibloader.h>

#include <koQueryTypes.h>
#include <koKoolBar.h>
#include <koDocument.h>
#include <kservice.h>
#include <view.h>

QList<KoShellWindow>* KoShellWindow::s_lstShells = 0L;

KoShellWindow::KoShellWindow()
{
  m_konqueror = m_activePage = m_lstPages.end();

  if ( s_lstShells == 0L )
    s_lstShells = new QList<KoShellWindow>;

  s_lstShells->append( this );

  m_pLayout = new QHBox( this );

  m_pKoolBar = new KoKoolBar( m_pLayout );

  setView( m_pLayout );

  m_pFrame = new KoShellFrame( m_pLayout );

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

  this->resize(550, 400);

  initShell();
}

KoShellWindow::~KoShellWindow()
{
  kdebug( KDEBUG_INFO, 0, "KoShellWindow::~KoShellWindow()" );

//  cleanUp();

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
    if ( m_activePage != m_lstPages.end() &&
         (*m_activePage).m_id == _item )
      return;

    QValueList<Page>::Iterator it = m_lstPages.begin();
    while( it != m_lstPages.end() )
    {
      if ( (*it).m_id == _item )
      {
        (*m_activePage).m_pView->reparent( 0L, 0, QPoint( 0, 0 ), FALSE );
	m_activePage = it;
//        m_pFrame->detach();
//        m_pFrame->attachView( (*it).m_vView );
//	interface()->setActivePart( (*it).m_vView->id() );
        (*m_activePage).m_pView->reparent( m_pFrame, 0, QPoint( 0, 0 ), TRUE );
	m_pFrame->setView( (*m_activePage).m_pView );
	setActiveView( (*m_activePage).m_pView );
	return;
      }
      ++it;
    }
  }
}
/*
bool KoShellWindow::isModified()
{
  QValueList<Page>::Iterator it = m_lstPages.begin();
  for( ; it != m_lstPages.end(); ++it )
    if ( (*it).m_vDoc->isModified() )
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
*/
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
  KoDocument *doc = _e.createDoc();

  doc->initDoc();

  View *v;

  if ( m_activePage != m_lstPages.end() )
    (*m_activePage).m_pView->reparent( 0L, 0, QPoint( 0, 0 ), FALSE );

  v = doc->createView( m_pFrame );
/*
  view->incRef();
  view->setMode( KOffice::View::RootMode );
  view->setMainWindow( this->koInterface() );

  m_pFrame->detach();
  m_pFrame->attachView( view );
  interface()->setActivePart( view->id() );
*/
  setActiveView( v );
  v->show();
  m_pFrame->setView( v );

  // HACK (Simon)
  m_pFrame->resize( m_pFrame->width() + 1, m_pFrame->height() );
  m_pFrame->resize( m_pFrame->width() - 1, m_pFrame->height() );

  // Create a new page
  Page page;
  page.m_pDoc = doc;
  page.m_pView = v;
  page.m_id = m_pKoolBar->insertItem( m_grpDocuments, _e.icon, i18n("No name"), this,
				      SLOT( slotKoolBar( int, int ) ) );

  m_lstPages.append( page );
  m_activePage = m_lstPages.end();
  m_activePage--;
/*
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
*/
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

*/
/*
bool KoShellWindow::saveAllPages()
{
  // TODO
  return false;
}

bool KoShellWindow::printDlg()
{
  assert( m_activePage != m_lstPages.end() );

  return (*m_activePage).m_vView->printDlg();
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
  m_pFrame->detach();
  QValueList<Page>::ConstIterator it = m_lstPages.begin();
  for (; it != m_lstPages.end(); ++it )
  {
    (*it).m_vView->decRef();
    (*it).m_vDoc->decRef();
  }
  m_lstPages.clear();
}
*/
QString KoShellWindow::configFile() const
{
  return readConfigFile( locate( "data", "koshell/koshell_shell.rc" ) );
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
#if 0
void KoShellWindow::slotFileNew()
{
  /*  if ( !newDocument() )
      QMessageBox::critical( this, i18n("KImage Error"), i18n("Could not create new document"), i18n("OK") ); */
/*
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
#endif

void KoShellWindow::slotFileOpen()
{
  if ( m_konqueror != m_lstPages.end() )
  {
    if ( m_konqueror == m_activePage )
      return;

    if ( m_activePage != m_lstPages.end() )
      (*m_activePage).m_pView->reparent( 0L, 0, QPoint( 0, 0 ), FALSE );

    m_activePage = m_konqueror;
    (*m_activePage).m_pView->reparent( m_pFrame, 0, QPoint( 0, 0 ), TRUE );
    setActiveView( (*m_activePage).m_pView );
    return;
  }

  KService::Ptr service = KService::service( "Konqueror" );

  if ( !service )
    return;

  KLibFactory *factory = KLibLoader::self()->factory( service->library() );

  if ( !factory )
    return;

  if ( m_activePage != m_lstPages.end() )
    (*m_activePage).m_pView->reparent( 0L, 0, QPoint( 0, 0 ), FALSE );

  Part *part = (Part *)factory->create();

  Page p;
  p.m_pDoc = 0L;
  p.m_pView = part->createView();
  p.m_id = -1;

  m_activePage = m_konqueror = m_lstPages.append( p );

  (*m_activePage).m_pView->reparent( m_pFrame, 0, QPoint( 0, 0 ), TRUE );
  setActiveView( (*m_activePage).m_pView );
  m_pFrame->setView( p.m_pView );
}

/*
void KoShellWindow::slotFileSave()
{
}

void KoShellWindow::slotFileSaveAs()
{
}

void KoShellWindow::slotFileClose()
{*/
  /* if ( documentCount() <= 1 )
  {
    slotFileQuit();
    return;
  }

  if ( isModified() )
    if ( !requestClose() )
      return;

      delete this; */ /*
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

  return KOffice::Document::_duplicate( (*m_activePage).m_vDoc );
}

KOffice::View_ptr KoShellWindow::view()
{
  if ( m_activePage == m_lstPages.end() )
    return 0L;

  return KOffice::View::_duplicate( (*m_activePage).m_vView );
}

void KoShellWindow::slotFileOpen()
{

  if ( !CORBA::is_nil( m_vKfm ) )
  {
    m_pFrame->detach();
    m_pFrame->OPFrame::attach( m_vKfm );
    interface()->setActivePart( m_vKfm->id() );
    return;
  }

  KActivator *activator = KdedInstance::self()->kactivator();
  CORBA::Object_var obj = activator->activateService( "Konqueror", "IDL:Konqueror/Application:1.0", "App" );

  OpenParts::Application_var app = OpenParts::Application::_narrow( obj );
  assert( !CORBA::is_nil( app ) );

  m_vKfm = app->createPart();
  assert( !CORBA::is_nil( m_vKfm ) );
  m_vKfm->setMainWindow( this->koInterface() );
  m_pFrame->detach();
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
*/

KoShellFrame::KoShellFrame( QWidget *parent )
 : QWidget( parent )
{
  m_pView = 0L;
}

void KoShellFrame::setView( View *view )
{
  m_pView = view;
}

void KoShellFrame::resizeEvent( QResizeEvent * )
{
  if ( m_pView )
    m_pView->setGeometry( 0, 0, width(), height() );
}

#include "koshell_shell.moc"
