/***************************************************************************
                          ktables.cpp  -  description                              
                             -------------------                                         
    begin                : Mið Júl  7 17:04:49 CEST 1999
                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include <iostream.h>

#include <qdir.h>
#include <qstrlist.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qheader.h>

#include <kiconloader.h>
#include <kfiledialog.h>
#include <kstdaccel.h>

#include <opMenu.h>
#include <opToolBar.h>
#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <komApplication.h>

#include <koPartSelectDia.h>
#include <koAboutDia.h>

#include "ktables.h"
#include "ktables_shell.h"
#include "ktables_view.h"
#include "ktables_doc.h"
#include "ktables_server.h"
#include "tableselect.h"
#include "querydialog.h"
#include "resource.h"

KtablesApp::KtablesApp()
{
  //config=kapp->getConfig();
	
  m_pDoc = 0;
	
  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
	
  //readOptions();
}

KtablesApp::~KtablesApp()
{
  cleanUp();
}

void KtablesApp::cleanUp()
{
  kdebug( KDEBUG_INFO,0,"KtablesApp::cleanUp()" );

  KoMainWindow::cleanUp();
}

void KtablesApp::createFileMenu(OPMenuBar *p_mbar)
{
  kdebug( KDEBUG_INFO, 0, "KtablesApp::createFileMenu" );

  // Do we loose control over the menubar ?
  if( p_mbar == 0L )
  {
    m_pFileMenu = 0L;
    return;
  }

  bool bInsertFileMenu = false;
  m_pFileMenu = p_mbar->fileMenu();
  if( m_pFileMenu == 0L )
  {
    bInsertFileMenu = true;
    debug("Creating File Menu in koMainWindow.cc");
    m_pFileMenu = new OPMenu;
  }
  else
    m_pFileMenu->insertSeparator();

  ///////////////////////////////////////////////////////////////////
  // menuBar entry file_menu
  KStdAccel stdAccel;

//CORBA::WString_var text;
//OpenPartsUI::Pixmap_var pix

  QString text;
  QPixmap pix;

  text = i18n("New &Window");
  pix = BarIcon( "ktablesapp.xpm" );
  m_pFileMenu->insertItem( pix, text, this, SLOT( slotFileNewWindow() ), 0 );

  m_pFileMenu->insertSeparator();

  text = i18n("&New");
  pix = BarIcon("filenew.xpm");
  m_idMenuFile_New = m_pFileMenu->insertItem( pix, text, this, SLOT( slotFileNew() ), stdAccel.openNew() );

  text = i18n("&Open...");
  pix = BarIcon("fileopen.xpm");
  m_idMenuFile_Open = m_pFileMenu->insertItem( pix, text, this, SLOT( slotFileOpen() ), stdAccel.open() );

  text = i18n("&Close");
  m_idMenuFile_Close = m_pFileMenu->insertItem( text, this, SLOT( slotFileClose() ), stdAccel.close() );

  m_pFileMenu->insertSeparator();

  text = i18n("&Save");
  pix = BarIcon("fileclose.xpm");
  m_idMenuFile_Save =  m_pFileMenu->insertItem( pix, text, this, SLOT( slotFileSave() ), stdAccel.save() );

  text = i18n("Save &As...");
  m_idMenuFile_SaveAs = m_pFileMenu->insertItem( text, this, SLOT( slotFileSaveAs() ) );

  m_pFileMenu->insertSeparator();

  text = i18n("&Print...");
  pix = BarIcon("fileprint.xpm");
  m_idMenuFile_Print = m_pFileMenu->insertItem( pix, text, this, SLOT( slotFilePrint() ), stdAccel.print() );

  m_pFileMenu->insertSeparator();

  text = i18n("E&xit");
  m_idMenuFile_Quit = m_pFileMenu->insertItem( text, this, SLOT( slotFileQuit() ), stdAccel.quit() );

  text = i18n( "&File" );
  if( bInsertFileMenu )
    p_mbar->insertItem( text, m_pFileMenu, -1, 0 );
}
	
void KtablesApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
	// TODO: add your own items you need for displaying current application status.
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG );
}

void
KtablesApp::helpAbout()
{
  KoAboutDia::about( KoAboutDia::KDE, "0.1" );
}

void
KtablesApp::setDocument( KtablesDoc *p_doc )
{
//  if ( m_pDoc )
//    releaseDocument();

	kdebug( KDEBUG_INFO,0,"KtablesApp::setDocument()" );
  m_pDoc = p_doc;
  m_pDoc->_ref();
  m_pView = p_doc->createTableView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  m_pView->setMainWindow( interface() );

  setRootPart( m_pView );
  interface()->setActivePart( m_pView->id() );

  opMenuBar()->setItemEnabled( m_idMenuFile_Save,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_SaveAs,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_Close,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_Print,true );

  opToolBar()->setItemEnabled(TOOLBAR_PRINT, true);
  opToolBar()->setItemEnabled(TOOLBAR_SAVE, true);

}

bool KtablesApp::newDocument()
{
	kdebug( KDEBUG_INFO,0,"KtablesApp::newDocument()" );
	if ( m_pDoc ) {
		kdebug( KDEBUG_INFO,0,"KtablesApp::newDocument - m_pDoc == 0" );
		KtablesApp *app = new KtablesApp;
		app->show();
		app->newDocument();
		return true;
	}

  m_pDoc = new KtablesDoc( this );
  if ( !m_pDoc->initDoc() ) {
    releaseDocument();
    kdebug( KDEBUG_FATAL, 0, "ERROR: Could not initialize document" );
    return false;
  }
  QObject::connect( m_pDoc, SIGNAL(signalMsg(const char *)),
                    this,   SLOT(slotStatusMsg(const char *)) );

  m_pView = m_pDoc->createTableView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  kdebug( KDEBUG_INFO, 0, "*1) VIEW! void KOMBase::refcnt() = %li", m_pView->_refcnt() );
  m_pView->setMainWindow( interface() );
	QObject::connect( m_pView, SIGNAL(signalMsg(const char *)),
				            this,    SLOT(slotStatusMsg(const char *)) );
	
  kdebug( KDEBUG_INFO,0,"setting root part" );
  setRootPart( m_pView );
  kdebug( KDEBUG_INFO,0,"setActivePart()" );
  interface()->setActivePart( m_pView->id() );

  kdebug( KDEBUG_INFO,0,"enableCommand(...)" );

  opMenuBar()->setItemEnabled( m_idMenuFile_Save,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_SaveAs,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_Close,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_Print,true );

  opToolBar()->setItemEnabled(TOOLBAR_PRINT, true);
  opToolBar()->setItemEnabled(TOOLBAR_SAVE, true);

  kdebug( KDEBUG_INFO, 0, "*2) VIEW! void KOMBase::refcnt() = %li", m_pView->_refcnt() );
  setCaption( m_pDoc->getTitle() );

  return true;
}

bool
KtablesApp::openDocument( const char *p_url, const char *p_format )
{
  if ( p_format == 0L || *p_format == 0 )
    p_format = "application/x-ktables";

  if ( m_pDoc && m_pDoc->isEmpty() )
    releaseDocument();
  else if ( m_pDoc && !m_pDoc->isEmpty() )
  {
    KtablesApp *app = new KtablesApp;
    app->show();
    return app->openDocument( p_url,p_format );
  }

  kdebug( KDEBUG_INFO, 0, i18n("Creating new document") );

  m_pDoc = new KtablesDoc(0,0);
  if ( !m_pDoc->loadFromURL( p_url,p_format ) )
  {           kdebug( KDEBUG_INFO,0,"KtablesView::newView()" );

    return false;
  }
  m_pView = m_pDoc->createTableView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  m_pView->setMainWindow( interface() );

  setRootPart( m_pView );
  interface()->setActivePart( m_pView->id() );

  opMenuBar()->setItemEnabled( m_idMenuFile_Save,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_SaveAs,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_Close,true );
  opMenuBar()->setItemEnabled( m_idMenuFile_Print,true );

  opToolBar()->setItemEnabled(TOOLBAR_PRINT, true);
  opToolBar()->setItemEnabled(TOOLBAR_SAVE, true);

  return true;
}

bool
KtablesApp::saveDocument(const char *p_url, const char *p_format)
{
  ASSERT( m_pDoc != 0L );
  CORBA::String_var url;
  QString file;

  if ( p_url == 0L || *p_url == 0 )
  {
    url = m_pDoc->url();
    p_url = url.in();
  }

  if ( p_url == 0L || *p_url == 0 )
  {
    file = KFileDialog::getSaveFileName( getenv( "HOME" ) );

    if ( file.isNull() )
      return false;
    p_url = file.data();
  }

  if ( p_format == 0L || *p_format == 0 )
    p_format = "application/x-ktables";

  return m_pDoc->saveToURL( p_url,p_format );
}

bool
KtablesApp::closeDocument()
{
 /* if (isModified ()) {
    if (! requestClose ())
      return false;
  } */

  return true;
}

bool
KtablesApp::closeAllDocuments()
{
  return true;
}

void
KtablesApp::releaseDocument()
{
  int views = 0;
  if ( m_pDoc )
    views = m_pDoc->viewCount();

  setRootPart( (long unsigned int)(OpenParts::Id)0 );

  interface()->setActivePart( 0 );

  if ( m_pView )
    m_pView->decRef();

  /* if ( m_pView )
    m_pView->cleanUp(); */

  if ( m_pDoc && views <= 1 )
    m_pDoc->cleanUp();
  // if ( m_pView )
  // CORBA::release( m_pView );
  if ( m_pDoc )
    CORBA::release( m_pDoc );
  m_pView = 0L;
  m_pDoc = 0L;

  opMenuBar()->setItemEnabled( m_idMenuFile_Save,false );
  opMenuBar()->setItemEnabled( m_idMenuFile_SaveAs,false );
  opMenuBar()->setItemEnabled( m_idMenuFile_Close,false );
  opMenuBar()->setItemEnabled( m_idMenuFile_Print,false );

  opToolBar()->setItemEnabled( TOOLBAR_PRINT,false );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE,false  );
}

void
KtablesApp::setCaption(const QString& p_str)
{
	KTMainWindow::setCaption(QString("%1: %2").arg(kapp->name()).arg(p_str));
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KtablesApp::slotFileNewWindow()
{
  slotStatusMsg(i18n("Opening a new Application window..."));
	
	KtablesApp* new_window= new KtablesApp();
	new_window->show();
	
  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileNew()
{
  slotStatusMsg(i18n("Creating new document..."));
	
	newDocument();		

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));
	

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileOpenRecent(int id_)
{
  slotStatusMsg(i18n("Opening file..."));
	

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."));
	
	m_pDoc->saveDocument(m_pDoc->getPathName()+m_pDoc->getTitle());

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file under new filename..."));


  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileClose()
{
  slotStatusMsg(i18n("Closing file..."));
	
	close();
}

void KtablesApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));

  QPrinter printer;
  if (printer.setup(this)){
//			m_pView->print(&printer);
	}

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileQuit()
{
	// close the first window, the list makes the next one the first again.
	// This ensures that queryClose() is called on each window to ask for closing
	KTMainWindow* w;
	if(memberList){
		for(w=memberList->first(); w; w=memberList->first()){
			// only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
			// the window and the application stay open.
			if(!w->close())
				break;
		}
	}	
}

void KtablesApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting selection..."));

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to Clipboard..."));

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotEditPaste()
{
  slotStatusMsg(i18n("Inserting Clipboard contents..."));

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotViewToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
	if( m_pViewMenu->isItemChecked(ID_VIEW_TOOLBAR))
	  m_pViewMenu->setItemChecked(ID_VIEW_TOOLBAR, false);
	else
		m_pViewMenu->setItemChecked(ID_VIEW_TOOLBAR, true);
		
  enableToolBar();

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotViewStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
	if( m_pViewMenu->isItemChecked(ID_VIEW_STATUSBAR))
	  m_pViewMenu->setItemChecked(ID_VIEW_STATUSBAR, false);
	else
		m_pViewMenu->setItemChecked(ID_VIEW_STATUSBAR, true);

  enableStatusBar();

  slotStatusMsg(i18n("Ready."));
}


void KtablesApp::slotStatusMsg(const char *text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG );
}


void KtablesApp::slotStatusHelpMsg(const char *text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->message(text, 2000);
}

KOffice::Document_ptr
KtablesApp::document()
{
  return KOffice::Document::_duplicate( m_pDoc );
}

KOffice::View_ptr
KtablesApp::view()
{
  return KOffice::View::_duplicate( m_pView );
}

#include "ktables_shell.moc"

