/*
 *  kimageshop_shell.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch    <koch@kde.org>
 *                1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qprinter.h>
#include <qmsgbox.h>
#include <qstring.h>

#include <klocale.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <klocale.h>

#include <opMainWindowIf.h>
#include <koAboutDia.h>

#include "kimageshop_shell.h"
#include "kimageshop_doc.h"
#include "kimageshop_view.h"
#include "newdialog.h"

QList<KImageShopShell>* KImageShopShell::s_lstShells = 0L;

KImageShopShell::KImageShopShell()
{
  setCaption("KImageShop");
  m_pDoc = 0L;
  m_pView = 0L;

  if (s_lstShells == 0L)
    s_lstShells = new QList<KImageShopShell>;

  s_lstShells->append(this);
  resize(700,500);
}

KImageShopShell::~KImageShopShell()
{
  kdebug(KDEBUG_INFO, 0, "KImageShopShell::~KImageShopShell()");

  cleanUp();
  s_lstShells->removeRef(this);
}

bool KImageShopShell::isModified()
{
  if (m_pDoc)
    return (bool)m_pDoc->isModified();

  return false;
}

bool KImageShopShell::requestClose()
{
  int res = QMessageBox::warning( 0L, i18n("Warning"), i18n("The document has been modified\nDo you want to save it ?" ),
				  i18n("Yes"), i18n("No"), i18n("Cancel") );

  if (res == 0)
    return saveDocument("", "");

  if (res == 1)
    return true;
  return false;
}

void KImageShopShell::cleanUp()
{
  releaseDocument();
  KoMainWindow::cleanUp();
}

void KImageShopShell::setDocument( KImageShopDoc *_doc )
{
  if ( m_pDoc )
    releaseDocument();

  m_pDoc = _doc;
  m_pDoc->_ref();
  m_pView = _doc->createImageView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  m_pView->setMainWindow( interface() );

  setRootPart( m_pView );
  interface()->setActivePart( m_pView->id() );

  if( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Print, true );
  }

  opToolBar()->setFullWidth(false);
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
}

bool KImageShopShell::newDocument()
{
  if ( m_pDoc )
  {
    KImageShopShell *s = new KImageShopShell();
    s->show();
    s->newDocument();
    return true;
  }

  NewDialog newdialog( this );
  if ( ! newdialog.exec() ) return false;
  
  m_pDoc = new KImageShopDoc( newdialog.newwidth(), newdialog.newheight() );
  if ( !m_pDoc->initDoc() )
  {
    releaseDocument();
    kdebug( KDEBUG_FATAL, 0, "ERROR: Could not initialize document" );
    return false;
  }

  m_pView = m_pDoc->createImageView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  kdebug( KDEBUG_INFO, 0, "*1) VIEW void KOMBase::refcnt() = %li", m_pView->_refcnt() );
  m_pView->setMainWindow( interface() );

  setRootPart( m_pView );
  interface()->setActivePart( m_pView->id() );

  if( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Print, true );
  }

  opToolBar()->setFullWidth(false);
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );

  kdebug( KDEBUG_INFO, 0, "*2) VIEW void KOMBase::refcnt() = %li", m_pView->_refcnt() );

  return true;
}

bool KImageShopShell::openDocument( const char *_url, const char *_format )
{
  if ( _format == 0L || *_format == 0 )
    _format = "application/x-kimageshop";

  if ( m_pDoc && m_pDoc->isEmpty() )
    releaseDocument();
  else if ( m_pDoc && !m_pDoc->isEmpty() )
  {
    KImageShopShell *s = new KImageShopShell();
    s->show();
    return s->openDocument( _url, _format );
  }

  kdebug( KDEBUG_INFO, 0, "Creating new document" );

  m_pDoc = new KImageShopDoc(510, 510);
  if ( !m_pDoc->loadFromURL( _url, _format ) )
    return false;

  m_pView = m_pDoc->createImageView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  m_pView->setMainWindow( interface() );

  setRootPart( m_pView );
  interface()->setActivePart( m_pView->id() );

  if ( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Print, true );
  }

  opToolBar()->setFullWidth(false);
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );

  return true;
}

bool KImageShopShell::saveDocument( const char *_url, const char *_format )
{
  ASSERT( m_pDoc != 0L );

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
    _format = "application/x-kimageshop";

  return m_pDoc->saveToURL( _url, _format );
}

bool KImageShopShell::printDlg()
{
  ASSERT( m_pView != 0L );

  return m_pView->printDlg();
}

void KImageShopShell::helpAbout()
{
  // KoAboutDia::about( KoAboutDia::KImageShop, "0.0.2" );
}

bool KImageShopShell::closeDocument()
{
  if ( isModified() )
  {
    if ( !requestClose() )
      return false;
  }

  return true;
}

bool KImageShopShell::closeAllDocuments()
{
  KImageShopShell* s;
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

int KImageShopShell::documentCount()
{
  return s_lstShells->count();
}

void KImageShopShell::releaseDocument()
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

  if ( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Print, false );
  }

  opToolBar()->setFullWidth(false);
  opToolBar()->setItemEnabled( TOOLBAR_PRINT, false );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, false );
}

void KImageShopShell::slotFileNew()
{
  if ( !newDocument() )
    QMessageBox::critical( this, i18n("KImageShop Error"), i18n("Could not create new document"), i18n("OK") );
}

void KImageShopShell::slotFileOpen()
{
  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if ( file.isNull() )
    return;

  if ( !openDocument( file, "" ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void KImageShopShell::slotFileSave()
{
  ASSERT( m_pDoc != 0L );

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
  }
}

void KImageShopShell::slotFileSaveAs()
{
  if ( !saveDocument( "", "" ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not save file" ) );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void KImageShopShell::slotFileClose()
{
  if ( isModified() )
    if ( !requestClose() )
      return;

  releaseDocument();
}

void KImageShopShell::slotFilePrint()
{
  ASSERT( m_pView );

  (void)m_pView->printDlg();
}

void KImageShopShell::slotFileQuit()
{
  kdebug( KDEBUG_INFO, 0, "EXIT 1" );

  if ( !closeAllDocuments() )
    return;

  kdebug( KDEBUG_INFO, 0, "EXIT 2" );

  delete this;
  kapp->exit();
}

KOffice::Document_ptr KImageShopShell::document()
{
  return KOffice::Document::_duplicate( m_pDoc );
}

KOffice::View_ptr KImageShopShell::view()
{
  return KOffice::View::_duplicate( m_pView );
}

#include "kimageshop_shell.moc"

