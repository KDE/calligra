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
#include "kdiagramm_shell.h"
#include "kdiagramm_doc.h"
#include "kdiagramm_view.h"

#include <koAboutDia.h>
#include <kfiledialog.h>
#include <opMainWindowIf.h>
#include <kapp.h>
#include <qmsgbox.h>
#include <klocale.h>

QList<KDiagrammShell>* KDiagrammShell::s_lstShells = 0L;

KDiagrammShell::KDiagrammShell()
{
  m_pDoc = 0L;
  m_pView = 0L;

  if ( s_lstShells == 0L )
    s_lstShells = new QList<KDiagrammShell>;

  s_lstShells->append( this );
}

KDiagrammShell::~KDiagrammShell()
{
  cerr << "KDiagrammShell::~KDiagrammShell()" << endl;

  cleanUp();

  s_lstShells->removeRef( this );
}

bool KDiagrammShell::isModified()
{
  if ( m_pDoc )
    return (bool)m_pDoc->isModified();

  return false;
}

bool KDiagrammShell::requestClose()
{
  int res = QMessageBox::warning( 0L, i18n("Warning"), i18n("The document has been modified\nDo you want to save it ?" ),
				  i18n("Yes"), i18n("No"), i18n("Cancel") );

  if ( res == 0 )
    return saveDocument();

  if ( res == 1 )
    return true;

  return false;
}

void KDiagrammShell::cleanUp()
{
  releaseDocument();

  KoMainWindow::cleanUp();
}

void KDiagrammShell::setDocument( KDiagrammDoc *_doc )
{
  if ( m_pDoc )
    releaseDocument();

  m_pDoc = _doc;
  m_pDoc->_ref();
  m_pView = _doc->createDiagrammView( frame() );
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
    m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
  }

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );
}

bool KDiagrammShell::newDocument()
{
  if ( m_pDoc )
  {
    KDiagrammShell *s = new KDiagrammShell();
    s->show();
    s->newDocument();
    return true;
  }

  m_pDoc = new KDiagrammDoc;
  if ( !m_pDoc->initDoc() )
  {
    cerr << "ERROR: Could not initialize document" << endl;
    return false;
  }

  m_pView = m_pDoc->createDiagrammView( frame() );
  m_pView->incRef();
  m_pView->setMode( KOffice::View::RootMode );
  cerr << "*1) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;
  m_pView->setMainWindow( interface() );

  setRootPart( m_pView );
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

  cerr << "*2) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

  return true;
}

bool KDiagrammShell::openDocument( const char *_url )
{
  if ( m_pDoc && m_pDoc->isEmpty() )
    releaseDocument();
  else if ( m_pDoc && !m_pDoc->isEmpty() )
  {
    KDiagrammShell *s = new KDiagrammShell();
    s->show();
    return s->openDocument( _url );
  }

  cerr << "Creating new document" << endl;

  m_pDoc = new KDiagrammDoc;
  if ( !m_pDoc->loadFromURL( _url ) )
    return false;

  m_pView = m_pDoc->createDiagrammView( frame() );
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
    m_pFileMenu->setItemEnabled( m_idMenuFile_Quit, true );
  }

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, true );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, true );

  return true;
}

bool KDiagrammShell::saveDocument()
{
  return KoMainWindow::saveDocument( "application/x-kdiagramm", "*.kdg", "KDiagramm" );
}

bool KDiagrammShell::printDlg()
{
  assert( m_pView != 0L );

  return m_pView->printDlg();
}

void KDiagrammShell::helpAbout()
{
  // KoAboutDia::about( KoAboutDia::KDiagramm, "0.0.2" );
}

bool KDiagrammShell::closeDocument()
{
  if ( isModified() )
  {
    if ( !requestClose() )
      return false;
  }

  return true;
}

bool KDiagrammShell::closeAllDocuments()
{
  KDiagrammShell* s;
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

int KDiagrammShell::documentCount()
{
  return s_lstShells->count();
}

void KDiagrammShell::releaseDocument()
{
  int views = 0;
  if ( m_pDoc )
    views = m_pDoc->viewCount();
  cerr << "############## VIEWS=" << views << " #####################" << endl;

  cerr << "-1) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

  setRootPart( (long unsigned int)(OpenParts::Id)0 );

  cerr << "-2) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

  interface()->setActivePart( 0 );

  // cerr << "-3) VIEW void KOMBase::refcnt() = " << m_pView->_refcnt() << endl;

  if ( m_pView )
    m_pView->decRef();

  /* if ( m_pView )
    m_pView->cleanUp(); */

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

void KDiagrammShell::slotFileNew()
{
  if ( !newDocument() )
    QMessageBox::critical( this, i18n("KDiagramm Error"), i18n("Could not create new document"), i18n("OK") );
}

void KDiagrammShell::slotFileOpen()
{
  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if ( file.isNull() )
    return;

  if ( openDocument( file  ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void KDiagrammShell::slotFileSave()
{
  assert( m_pDoc != 0L );

  (void) saveDocument();
}

void KDiagrammShell::slotFileSaveAs()
{
  QString _url = m_pDoc->url();
  m_pDoc->setURL( "" );

  if ( !saveDocument() )
    m_pDoc->setURL( _url );
}

void KDiagrammShell::slotFileClose()
{
  if ( documentCount() <= 1 )
  {
    slotFileQuit();
    return;
  }

  if ( isModified() )
    if ( !requestClose() )
      return;

  delete this;
}

void KDiagrammShell::slotFilePrint()
{
  assert( m_pView );

  (void)m_pView->printDlg();
}

void KDiagrammShell::slotFileQuit()
{
  cerr << "EXIT 1" << endl;

  if ( !closeAllDocuments() )
    return;

  cerr << "EXIT 2" << endl;

  delete this;
  kapp->exit();
}

KOffice::Document_ptr KDiagrammShell::document()
{
  return KOffice::Document::_duplicate( m_pDoc );
}

KOffice::View_ptr KDiagrammShell::view()
{
  return KOffice::View::_duplicate( m_pView );
}

#include "kdiagramm_shell.moc"

