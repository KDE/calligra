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

KoShellWindow::KoShellWindow()
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
    kdDebug() << "Inserting into koolbar : " << (*it).name << endl;
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
//  cleanUp();
}

QString KoShellWindow::nativeFormatName() const
{
  return i18n("KOffice Documents");
}

QString KoShellWindow::nativeFormatPattern() const
{
  // TODO get this from the desktop files
  return "*.kwd *.ksp *.kpr";
}

KoDocument * KoShellWindow::createDoc()
{
  if ( m_mimeType.isEmpty() )
    ;// TODO use part chooser dialog
  else
  {
    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( m_mimeType.latin1() );
    m_mimeType = QString::null; // reset
    return entry.createDoc();
  }
  return 0L;
}

bool KoShellWindow::openDocument( const KURL & url )
{
  KMimeType::Ptr mimeType = KMimeType::findByURL( url );
  m_mimeType = mimeType->name();
  // This one will call createDoc, that's why we saved the mimetype first
  return KoMainWindow::openDocument( url ); 
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
        KoView * v = (*m_activePage).m_pView;
        v->reparent( 0L, 0, QPoint( 0, 0 ), FALSE );
	m_activePage = it;
        v->reparent( m_pFrame, 0, QPoint( 0, 0 ), TRUE );
	m_pFrame->setView( v );
	partManager()->setActivePart( v->koDocument(), v );
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

bool KoShellWindow::newPage( KoDocumentEntry& _e )
{
  // Create document
  KoDocument *doc = _e.createDoc();

  doc->initDoc();

  KoView *v;

  if ( m_activePage != m_lstPages.end() )
    (*m_activePage).m_pView->reparent( 0L, 0, QPoint( 0, 0 ), FALSE );

  v = doc->createView( m_pFrame );
  v->setGeometry( 0, 0, width(), height() );
  partManager()->setActivePart( doc, v );
  v->show();
  m_pFrame->setView( v );

  // Create a new page
  Page page;
  page.m_pDoc = doc;
  page.m_pView = v;
  page.m_id = m_pKoolBar->insertItem( m_grpDocuments, _e.icon, i18n("No name"), this,
				      SLOT( slotKoolBar( int, int ) ) );

  m_lstPages.append( page );
  m_activePage = m_lstPages.end();
  m_activePage--;
  return true;
}


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
  //return readConfigFile( locate( "data", "koshell/koshell_shell.rc" ) );
  return QString::null; // use UI standards only for now
}

/*
void KoShellWindow::slotFileSave()
{
}

void KoShellWindow::slotFileSaveAs()
{
}

void KoShellWindow::slotFilePrint()
{
  printDlg();
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
