/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000 David Faure <faure@kde.org>

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

#include <qhbox.h>

#include "koshell_shell.h"

#include <kfiledialog.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <klibloader.h>
#include <kservice.h>

#include <koQueryTypes.h>
#include <koKoolBar.h>
#include <koDocument.h>
#include <koView.h>
#include <koPartSelectDia.h>

KoShellWindow::KoShellWindow()
  : m_documentEntry( 0L )
{
  m_activePage = m_lstPages.end();

  m_pLayout = new QHBox( this );

  m_pKoolBar = new KoKoolBar( m_pLayout );

  setView( m_pLayout );

  m_pFrame = new KoShellFrame( m_pLayout );

  m_grpFile = m_pKoolBar->insertGroup(i18n("Parts"));
  m_lstComponents = KoDocumentEntry::query();
  QValueList<KoDocumentEntry>::Iterator it = m_lstComponents.begin();
  for( ; it != m_lstComponents.end(); ++it )
  {
    //kdDebug() << "Inserting into koolbar : " << (*it).name << endl;
    if ( !(*it).icon.isNull() )
    {
      int id = m_pKoolBar->insertItem( m_grpFile, DesktopIcon((*it).icon),
                                       (*it).name,
				       this, SLOT( slotKoolBar( int, int ) ) );
      m_mapComponents[ id ] = &*it;
    }
  }

  m_grpDocuments = m_pKoolBar->insertGroup(i18n("Documents"));
  //m_pKoolBar->insertGroup("Snippets"); //?

  m_pKoolBar->setFixedWidth( 80 );
  m_pKoolBar->setMinimumHeight( 300 );

  this->resize(550, 400);
}

KoShellWindow::~KoShellWindow()
{
  //kdDebug() << "KoShellWindow::~KoShellWindow()" << endl;

  // Destroy all documents - queryClose has made sure we saved them first
  QValueList<Page>::ConstIterator it = m_lstPages.begin();
  for (; it != m_lstPages.end(); ++it )
  {
    (*it).m_pDoc->removeShell( this );
    delete (*it).m_pView;
    if ( (*it).m_pDoc->viewCount() == 0 )
      delete (*it).m_pDoc;
  }
  m_lstPages.clear();

  setRootDocumentDirect( 0L ); // prevent our parent destructor from doing stupid things
}

QString KoShellWindow::nativeFormatName() const
{
  return i18n("KOffice Documents");
}

QString KoShellWindow::nativeFormatPattern() const
{
  // TODO get this from the desktop files
  // KoDocumentEntry::query(), then get hold of the service pointers
  return "*.kwd *.ksp *.kpr";
}

KoDocument * KoShellWindow::createDoc()
{
  if ( !m_documentEntry )
    m_documentEntry = new KoDocumentEntry( KoPartSelectDia::selectPart( this ) );

  KoDocument * doc = m_documentEntry->createDoc();
  return doc;
}

bool KoShellWindow::openDocument( const KURL & url )
{
  KMimeType::Ptr mimeType = KMimeType::findByURL( url );
  m_documentEntry = new KoDocumentEntry(
    KoDocumentEntry::queryByMimeType( mimeType->name().latin1() ) );

  // This one will call createDoc, that's why we saved the document entry first
  bool ret = KoMainWindow::openDocument( url );

  delete m_documentEntry;
  m_documentEntry = 0L;
  return ret;
}

void KoShellWindow::setRootDocument( KoDocument * doc )
{
  kdDebug() << "KoShellWindow::setRootDocument this=" << this << " doc=" << doc << endl;
  // We do things quite differently from KoMainWindow::setRootDocument
  // This one is called with doc != 0 when a new doc is created
  // and with 0L after they have all been removed.
  // We use setRootDocumentDirect to switch the 'root doc' known by KoMainWindow.

  setRootDocumentDirect( doc );
  updateCaption();

  if ( doc )
  {
    doc->addShell( this );
    KoView *v = doc->createView( m_pFrame );
    v->show();
    v->setGeometry( 0, 0, m_pFrame->width(), m_pFrame->height() );

    // Create a new page for this doc
    Page page;
    page.m_pDoc = doc;
    page.m_pView = v;
    assert( m_documentEntry ); // we need it right below
    page.m_id = m_pKoolBar->insertItem( m_grpDocuments, DesktopIcon( m_documentEntry->icon ),
                                        i18n("No name"), // TODO
                                        this, SLOT( slotKoolBar( int, int ) ) );
    kdDebug() << " New page has id " << page.m_id << " doc is " << doc << endl;

    m_lstPages.append( page );

    switchToPage( m_lstPages.fromLast() );
  } else
    m_activePage = m_lstPages.end();
}

void KoShellWindow::slotKoolBar( int _grp, int _item )
{
  kdDebug() << "KoShellWindow::slotKoolBar "  << _grp << " " << _item << endl;
  if ( _grp == m_grpFile )
  {
    // Create new document from a KoDocumentEntry
    if ( m_documentEntry )
      delete m_documentEntry;
    m_documentEntry = m_mapComponents[ _item ];
    KoDocument *doc = m_documentEntry->createDoc();
    if (doc)
    {
        if ( doc->initDoc() )
            setRootDocument( doc );
        else
            delete doc;
    }
    m_documentEntry = 0L;
  }
  else if ( _grp == m_grpDocuments )
  {
    // Switch to an existing document
    if ( m_activePage != m_lstPages.end() &&
         (*m_activePage).m_id == _item )
      return;

    QValueList<Page>::Iterator it = m_lstPages.begin();
    while( it != m_lstPages.end() )
    {
      if ( (*it).m_id == _item )
      {
        switchToPage( it );
	return;
      }
      ++it;
    }
  }
}

void KoShellWindow::switchToPage( QValueList<Page>::Iterator it )
{
  // Move away current page (view)
  if ( m_activePage != m_lstPages.end() )
    (*m_activePage).m_pView->reparent( 0L, 0, QPoint( 0, 0 ), FALSE );
  // Select new active page (view)
  m_activePage = it;
  KoView *v = (*m_activePage).m_pView;
  // Show it here
  v->reparent( m_pFrame, 0, QPoint( 0, 0 ), TRUE );
  m_pFrame->setView( v );
  kdDebug() << " setting active part to " << (*m_activePage).m_pDoc << endl;
  // Make it active (GUI etc.)
  partManager()->setActivePart( (*m_activePage).m_pDoc, v );
  // Fix caption
  setRootDocumentDirect( (*m_activePage).m_pDoc );
  updateCaption();
}

void KoShellWindow::slotFileNew()
{
    // Ensure we get the part chooser dialog in createDoc
    delete m_documentEntry;
    m_documentEntry = 0L;
    KoMainWindow::slotFileNew();
}

void KoShellWindow::slotFileClose()
{
  // reimplemented to avoid closing the window when we have docs opened

  // No docs at all ?
  if ( m_lstPages.count() == 0 )
    close(); // close window
  else
    closeDocument(); // close only doc
}

void KoShellWindow::closeDocument()
{
  // Set the root document to the current one - so that queryClose acts on it
  assert( m_activePage != m_lstPages.end() );
  setRootDocumentDirect( (*m_activePage).m_pDoc );
  kdDebug() << "KoShellWindow::closeDocument() : rootDocument is " << rootDocument() << endl;

  // First do the standard queryClose
  if ( KoMainWindow::queryClose() )
  {
    kdDebug() << "Ok for closing document" << endl;
    m_pFrame->setView( 0L ); // safety measure
    m_pKoolBar->removeItem( m_grpDocuments, (*m_activePage).m_id );
    (*m_activePage).m_pDoc->removeShell(this);
    delete (*m_activePage).m_pView;
    if ( (*m_activePage).m_pDoc->viewCount() <= 1 )
      delete (*m_activePage).m_pDoc;
    m_lstPages.remove( m_activePage );
    m_activePage = m_lstPages.end(); // no active page right now

    kdDebug() << "m_lstPages has " << m_lstPages.count() << " documents" << endl;
    if ( m_lstPages.count() > 0 )
    {
      kdDebug() << "Activate the document behind" << endl;
      slotKoolBar( m_grpDocuments, m_lstPages.last().m_id );
    }
    else
    {
      kdDebug() << "Revert to initial state (no docs)" << endl;
      setRootDocument( 0L );
      partManager()->setActivePart( 0L, 0L );
    }
  }
  kdDebug() << "m_lstPages has " << m_lstPages.count() << " documents" << endl;
}

bool KoShellWindow::queryClose()
{
  // This one is called by slotFileQuit and by the X button.
  // We have to check for unsaved docs...
  bool ok = true;
  QValueList<Page>::Iterator it = m_lstPages.begin();
  for( ; it != m_lstPages.end(); ++it )
  {
    setRootDocumentDirect( (*it).m_pDoc );
    // Test if we can close this doc
    if ( !KoMainWindow::queryClose() )
    {
      ok = false; // No
      break; // abort
    }
  }
  return ok;
}

/*
// Should this be an additionnal action in the File menu ?
bool KoShellWindow::saveAllPages()
{
  // TODO
  return false;
}
*/

QString KoShellWindow::configFile() const
{
  //return readConfigFile( locate( "data", "koshell/koshell_shell.rc" ) );
  return QString::null; // use UI standards only for now
}

/*
void KoShellWindow::slotFilePrint()
{
  assert( m_activePage != m_lstPages.end() );

  return (*m_activePage).m_vView->printDlg();
}
*/

///////

KoShellFrame::KoShellFrame( QWidget *parent )
 : QWidget( parent )
{
  m_pView = 0L;
}

void KoShellFrame::setView( KoView *view )
{
  m_pView = view;
}

void KoShellFrame::resizeEvent( QResizeEvent * )
{
  if ( m_pView )
    m_pView->setGeometry( 0, 0, width(), height() );
}

#include "koshell_shell.moc"
