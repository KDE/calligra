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
#include <opMenu.h>

#include <koAboutDia.h>

#include "kformviewer_shell.h"
#include "kformviewer_doc.h"
#include "kformviewer_view.h"

QList<KformViewerShell>* KformViewerShell::s_lstShells = 0L;

KformViewerShell::KformViewerShell()
{
  m_pDoc = 0L;
  m_pView = 0L;

  if ( s_lstShells == 0L )
    s_lstShells = new QList<KformViewerShell>;

  s_lstShells->append( this );
}

KformViewerShell::~KformViewerShell()
{
  kdebug( KDEBUG_INFO, 0, "KformViewerShell::~KformViewerShell()" );

  cleanUp();

  s_lstShells->removeRef( this );
}

bool KformViewerShell::isModified()
{
  if ( m_pDoc )
    return (bool)m_pDoc->isModified();

  return false;
}

bool KformViewerShell::requestClose()
{
  int res = QMessageBox::warning( 0L, i18n("Warning"), i18n("The document has been modified\nDo you want to save it ?" ),
				  i18n("Yes"), i18n("No"), i18n("Cancel") );

  if ( res == 0 )
    return saveDocument();

  if ( res == 1 )
    return true;

  return false;
}

void KformViewerShell::cleanUp()
{
  releaseDocument();

  KoMainWindow::cleanUp();
}

void KformViewerShell::setDocument( KformViewerDoc *_doc )
{
  if ( m_pDoc )
    releaseDocument();

  m_pDoc = _doc;
  m_pDoc->_ref();
  m_pView = _doc->createFormView( frame() );
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

bool KformViewerShell::newDocument()
{
  if ( m_pDoc )
  {
    KformViewerShell *s = new KformViewerShell();
    s->show();
    s->newDocument();
    return true;
  }

  m_pDoc = new KformViewerDoc;
  if ( !m_pDoc->initDoc() )
  {
    releaseDocument();
    kdebug( KDEBUG_FATAL, 0, "ERROR: Could not initialize document" );
    return false;
  }

  m_pView = m_pDoc->createFormView( frame() );
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

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );

  kdebug( KDEBUG_INFO, 0, "*2) VIEW void KOMBase::refcnt() = %li", m_pView->_refcnt() );

  return true;
}

bool KformViewerShell::openDocument( const char *_url, const char *_format )
{
  if ( _format == 0L || *_format == 0 )
    _format = "application/x-kformviewer";

  if ( m_pDoc && m_pDoc->isEmpty() )
    releaseDocument();
  else if ( m_pDoc && !m_pDoc->isEmpty() )
  {
    KformViewerShell *s = new KformViewerShell();
    s->show();
    return s->openDocument( _url, _format );
  }

  kdebug( KDEBUG_INFO, 0, "Creating new document" );

  m_pDoc = new KformViewerDoc;
  if ( !m_pDoc->loadFromURL( _url, _format ) )
  {
    return false;
  }
  m_pView = m_pDoc->createFormView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  m_pView->setMainWindow( interface() );

  setRootPart( m_pView );
  interface()->setActivePart( m_pView->id() );

  if ( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, true );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Print, false );
  }

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, false );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, false );

  return true;
}

bool KformViewerShell::saveDocument()
{
  ASSERT( m_pDoc != 0L );
  return KoMainWindow::saveDocument( "application/x-kformviewer", "*.kfv", "KFormViewer" );
  // choose your suffix :)  (David)
}

bool KformViewerShell::printDlg()
{
  ASSERT( m_pView != 0L );

  return m_pView->printDlg();
}

void KformViewerShell::helpAbout()
{
  // KoAboutDia::about( KoAboutDia::KformViewer, "0.0.2" );
}

bool KformViewerShell::closeDocument()
{
  if ( isModified() )
  {
    if ( !requestClose() )
      return false;
  }

  return true;
}

bool KformViewerShell::closeAllDocuments()
{
  KformViewerShell* s;
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

int KformViewerShell::documentCount()
{
  return s_lstShells->count();
}

void KformViewerShell::releaseDocument()
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

void KformViewerShell::slotFileNew()
{
  if ( !newDocument() )
    QMessageBox::critical( this, i18n("KformViewer Error"), i18n("Could not create new document"), i18n("OK") );
}

void KformViewerShell::slotFileOpen()
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

void KformViewerShell::slotFileSave()
{
  ASSERT( m_pDoc != 0L );
  (void) saveDocument();
}

void KformViewerShell::slotFileSaveAs()
{
  QString _url = m_pDoc->url();
  m_pDoc->setURL( "" );

  if ( !saveDocument() )
    m_pDoc->setURL( _url );
}

void KformViewerShell::slotFileClose()
{
  if ( isModified() )
    if ( !requestClose() )
      return;

  releaseDocument();
}

void KformViewerShell::slotFilePrint()
{
  ASSERT( m_pView );

  (void)m_pView->printDlg();
}

void KformViewerShell::slotFileQuit()
{
  kdebug( KDEBUG_INFO, 0, "EXIT 1" );

  if ( !closeAllDocuments() )
    return;

  kdebug( KDEBUG_INFO, 0, "EXIT 2" );

  delete this;
  kapp->exit();
}

KOffice::Document_ptr KformViewerShell::document()
{
  return KOffice::Document::_duplicate( m_pDoc );
}

KOffice::View_ptr KformViewerShell::view()
{
  return KOffice::View::_duplicate( m_pView );
}

#include "kformviewer_shell.moc"

