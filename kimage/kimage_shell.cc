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

#include <qprinter.h>
#include <qmsgbox.h>

#include <qtimer.h>
#include <qstring.h>

#include <klocale.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <klocale.h>

#include <opMainWindowIf.h>
#include <koAboutDia.h>

#include "kimage_shell.h"
#include "kimage_doc.h"
#include "kimage_view.h"

QList<KImageShell>* KImageShell::s_lstShells = 0L;

KImageShell::KImageShell()
{
  m_pDoc = 0L;
  m_pView = 0L;

  if ( s_lstShells == 0L )
    s_lstShells = new QList<KImageShell>;

  s_lstShells->append( this );
}

KImageShell::~KImageShell()
{
  debug( "KImageShell::~KImageShell()" );

  cleanUp();

  s_lstShells->removeRef( this );
}

bool KImageShell::isModified()
{
  if ( m_pDoc )
    return (bool)m_pDoc->isModified();

  return false;
}

bool KImageShell::requestClose()
{
  int res = QMessageBox::warning( 0L, i18n("Warning"), i18n("The document has been modified\nDo you want to save it ?" ),
				  i18n("Yes"), i18n("No"), i18n("Cancel") );

  if ( res == 0 )
    return saveDocument( "", "" );

  if ( res == 1 )
    return true;

  return false;
}

void KImageShell::cleanUp()
{
  releaseDocument();

  KoMainWindow::cleanUp();
}

void KImageShell::setDocument( KImageDoc *_doc )
{
  if ( m_pDoc )
    releaseDocument();

  m_pDoc = _doc;
  m_pDoc->_ref();
  m_pView = _doc->createImageView( getFrame() );
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

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
}

bool KImageShell::newDocument()
{
  if ( m_pDoc )
  {
    KImageShell *s = new KImageShell();
    s->show();
    s->newDocument();
    return true;
  }

  m_pDoc = new KImageDoc;
  if ( !m_pDoc->init() )
  {
    releaseDocument();
    debug( "ERROR: Could not initialize document" );
    return false;
  }

  m_pView = m_pDoc->createImageView( getFrame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  debug( "*1) VIEW void KOMBase::refcnt() = %li", m_pView->_refcnt() );
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

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );

  debug( "*2) VIEW void KOMBase::refcnt() = %li", m_pView->_refcnt() );

  return true;
}

bool KImageShell::openDocument( const char *_url, const char *_format )
{
  if ( _format == 0L || *_format == 0 )
    _format = "application/x-kimage";

  if ( m_pDoc && m_pDoc->isEmpty() )
    releaseDocument();
  else if ( m_pDoc && !m_pDoc->isEmpty() )
  {
    KImageShell *s = new KImageShell();
    s->show();
    return s->openDocument( _url, _format );
  }

  debug( "Creating new document" );

  m_pDoc = new KImageDoc;
  if ( !m_pDoc->loadFromURL( _url, _format ) )
    return false;

  m_pView = m_pDoc->createImageView( getFrame() );
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

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );

  return true;
}

bool KImageShell::saveDocument( const char *_url, const char *_format )
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

bool KImageShell::printDlg()
{
  assert( m_pView != 0L );

  return m_pView->printDlg();
}

void KImageShell::helpAbout()
{
  // KoAboutDia::about( KoAboutDia::KImage, "0.0.2" );
}

bool KImageShell::closeDocument()
{
  if ( isModified() )
  {
    if ( !requestClose() )
      return false;
  }

  return true;
}

bool KImageShell::closeAllDocuments()
{
  KImageShell* s;
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

int KImageShell::documentCount()
{
  return s_lstShells->count();
}

void KImageShell::releaseDocument()
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

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, false );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, false );
}

void KImageShell::slotFileNew()
{
  if ( !newDocument() )
    QMessageBox::critical( this, i18n("KImage Error"), i18n("Could not create new document"), i18n("OK") );
}

void KImageShell::slotFileOpen()
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

void KImageShell::slotFileSave()
{
  assert( m_pDoc != 0L );

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

void KImageShell::slotFileSaveAs()
{
  if ( !saveDocument( "", "" ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not save file" ) );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void KImageShell::slotFileClose()
{
  if ( isModified() )
    if ( !requestClose() )
      return;

  releaseDocument();
}

void KImageShell::slotFilePrint()
{
  assert( m_pView );

  (void)m_pView->printDlg();
}

void KImageShell::slotFileQuit()
{
  debug( "EXIT 1" );

  if ( !closeAllDocuments() )
    return;

  debug( "EXIT 2" );

  delete this;
  kapp->exit();
}

KOffice::Document_ptr KImageShell::document()
{
  return KOffice::Document::_duplicate( m_pDoc );
}

KOffice::View_ptr KImageShell::view()
{
  return KOffice::View::_duplicate( m_pView );
}

#include "kimage_shell.moc"

