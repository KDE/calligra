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

#include <qsplitter.h>
#include <assert.h>

#include "koshell_shell.h"
#include "koshellsettings.h"

#include <ktempfile.h>
#include <kfiledialog.h>
//#include <kio/netaccess.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <klibloader.h>
#include <kservice.h>
#include <kmessagebox.h>
#include <krecentdocument.h>
#include <kparts/partmanager.h>
#include <kaction.h>
#include <kdeversion.h>

#include <koQueryTrader.h>
#include <koKoolBar.h>
#include <koDocumentInfo.h>
#include <koDocument.h>
#include <koView.h>
#include <koPartSelectDia.h>
#include <koFilterManager.h>
#include <kiconloader.h>

KoShellWindow::KoShellWindow()
 : KoMainWindow( KGlobal::instance() )
{
  m_activePage = m_lstPages.end();

  m_pLayout = new QSplitter( centralWidget() );
  m_pKoolBar = new KoKoolBar( m_pLayout );

  m_pFrame = new KoShellFrame( m_pLayout );

  m_grpFile = m_pKoolBar->insertGroup(i18n("Components"));
  QValueList<KoDocumentEntry> lstComponents = KoDocumentEntry::query(false,QString());
  QValueList<KoDocumentEntry>::Iterator it = lstComponents.begin();
  for( ; it != lstComponents.end(); ++it )
  {
      //kdDebug() << "Inserting into koolbar : " << (*it).name() << endl;
      int id;
      if (!(*it).service()->genericName().isEmpty()) //skip the unavailable part
          id = m_pKoolBar->insertItem( m_grpFile,
                                       DesktopIcon((*it).service()->icon()),
                                       (*it).name(),
                                       this, SLOT( slotKoolBar( int, int ) ) );
      else
          continue;

      m_mapComponents[ id ] = *it;

      // Build list of patterns for all supported KOffice documents...
      QString nativeMimeType = (*it).service()->property( "X-KDE-NativeMimeType" ).toString();
      //kdDebug() << nativeMimeType << endl;
      if ( !nativeMimeType.isEmpty() )
      {
        KMimeType::Ptr mime = KMimeType::mimeType( nativeMimeType );
        if (mime)
        {
          if ( !m_filter.isEmpty() )
            m_filter += " ";
          m_filter += mime->patterns().join(" ");
        }
      }
  }

  m_grpDocuments = m_pKoolBar->insertGroup(i18n("Documents"));
  //m_pKoolBar->insertGroup("Snippets"); //?

  QValueList<int> list;
  list.append( KoShellSettings::sidebarWidth() );
  list.append( this->width() - KoShellSettings::sidebarWidth() );
  m_pLayout->setSizes( list );
  
  connect( this, SIGNAL( documentSaved() ),
           this, SLOT( slotNewDocumentName() ) );

  // Not implemented yet
  actionCollection()->action("view_split")->setEnabled(false);
  actionCollection()->action("view_splitter_orientation")->setEnabled(false);

  m_client = new KoShellGUIClient( this );
  createShellGUI();
}

KoShellWindow::~KoShellWindow()
{
  //kdDebug() << "KoShellWindow::~KoShellWindow()" << endl;

  // Set the active part to 0 (as we do in ~KoMainWindow, but this is too
  // late for KoShell, it gets activePartChanged signals delivered to a dead
  // KoShellWindow object).
  partManager()->setActivePart(0);

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

  setRootDocumentDirect( 0L, QPtrList<KoView>() ); // prevent our parent destructor from doing stupid things
  saveSettings(); // Now save our settings before exiting
}

bool KoShellWindow::openDocumentInternal( const KURL &url, KoDocument* )
{
  // Here we have to distinguish two cases: The passed URL has a native
  // KOffice mimetype. Then we query the trader and create the document.
  // The file is loaded and everyone's happy.
  // The second case is a non-native file. Here we have to create a
  // filter manager, ask it to convert the file to the "closest" available
  // KOffice part and open the temporary file.
  
  /*if (!KIO::NetAccess::exists(url,true,0) )
  {
    KMessageBox::error(0L, i18n("The file %1 doesn't exist.").arg(url.url()) );
    recentAction()->removeURL(url); //remove the file from the recent-opened-file-list
    saveRecentFiles();
    return false;
  }*/
  
  KMimeType::Ptr mimeType = KMimeType::findByURL( url );
  m_documentEntry = KoDocumentEntry::queryByMimeType( mimeType->name().latin1() );

  KTempFile* tmpFile = 0;
  KURL tmpUrl( url );  // we might have to load a converted temp. file

  if ( m_documentEntry.isEmpty() ) { // non-native
    tmpFile = new KTempFile;

    KoFilterManager *manager = new KoFilterManager( url.path() );
    QCString mimetype;                                               // an empty mimetype means, that the "nearest"
    KoFilter::ConversionStatus status = manager->exp0rt( tmpFile->name(), mimetype ); // KOffice part will be chosen
    delete manager;

    if ( status != KoFilter::OK || mimetype.isEmpty() ) {
      tmpFile->unlink();
      delete tmpFile;
      return false;
    }

    // If the conversion was successful we get the mimetype of the
    // chosen KOffice part back.
    m_documentEntry = KoDocumentEntry::queryByMimeType( mimetype );
    if ( m_documentEntry.isEmpty() ) {
      tmpFile->unlink();
      delete tmpFile;
      return false;
    }

    // Open the temporary file
    tmpUrl.setPath( tmpFile->name() );
  }

  recentAction()->addURL( url );

  KoDocument* newdoc = m_documentEntry.createDoc();

  connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
  connect(newdoc, SIGNAL(completed()), this, SLOT(slotKSLoadCompleted()));
  connect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotKSLoadCanceled( const QString & )));
  newdoc->addShell( this ); // used by openURL
  bool openRet = (!isImporting ()) ? newdoc->openURL(tmpUrl) : newdoc->import(tmpUrl);
  if ( !newdoc || !openRet )
  {
      delete newdoc;
      if ( tmpFile ) {
        tmpFile->unlink();
        delete tmpFile;
      }
      return false;
  }

  if ( tmpFile ) {
    //if the laoded file has been a temporary file
    //we need to correct a few document settings
    //see description of bug #77574 for additional information

    //correct (output) mime type: we need to set it to the non-native format
    //to make sure the user knows about saving to a non-native mime type
    //setConfirmNonNativeSave is set to true below
    newdoc->setMimeType( mimeType->name().latin1() );
    newdoc->setOutputMimeType( mimeType->name().latin1() );

    //the next time the user saves the document he should be warned
    //because of mime type settings done above;
    newdoc->setConfirmNonNativeSave(true,true); //exporting,warn_on
    newdoc->setConfirmNonNativeSave(false,true); //save/save as,warn_on

    //correct document file (should point to URL)
    newdoc->setFile( url.path() );

    //correct document URL
    newdoc->setURL( url );

    //update caption to represent the correct URL in the window titlebar
    updateCaption();

    tmpFile->unlink();
    delete tmpFile;
  }
  return true;
}

// Separate from openDocument to handle async loading (remote URLs)
void KoShellWindow::slotKSLoadCompleted()
{
    KoDocument* newdoc = (KoDocument *)(sender());

    // KoDocument::import() calls resetURL() too late...
    // ...setRootDocument will show the URL...
    // So let's stop this from happening and the user will never know :)
    if (isImporting()) newdoc->resetURL ();

    partManager()->addPart( newdoc, false );
    setRootDocument( newdoc );
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    disconnect(newdoc, SIGNAL(completed()), this, SLOT(slotKSLoadCompleted()));
    disconnect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotKSLoadCanceled( const QString & )));
}

void KoShellWindow::slotKSLoadCanceled( const QString & errMsg )
{
    KMessageBox::error( this, errMsg );
    // ... can't delete the document, it's the one who emitted the signal...
    // ###### FIXME: This can be done in 3.0 with deleteLater, I assume (Werner)

    KoDocument* newdoc = (KoDocument *)(sender());
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    disconnect(newdoc, SIGNAL(completed()), this, SLOT(slotKSLoadCompleted()));
    disconnect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotKSLoadCanceled( const QString & )));
}

void KoShellWindow::setRootDocument( KoDocument * doc )
{
  kdDebug() << "KoShellWindow::setRootDocument this=" << this << " doc=" << doc << endl;
  // We do things quite differently from KoMainWindow::setRootDocument
  // This one is called with doc != 0 when a new doc is created
  // and with 0L after they have all been removed.
  // We use setRootDocumentDirect to switch the 'root doc' known by KoMainWindow.

  if ( doc )
  {
    if ( !doc->shells().contains( this ) )
        doc->addShell( this );
    KoView *v = doc->createView( m_pFrame );
    QPtrList<KoView> views;
    views.append(v);
    setRootDocumentDirect( doc, views );

    v->show();
    v->setGeometry( 0, 0, m_pFrame->width(), m_pFrame->height() );
    v->setPartManager( partManager() );

    // Create a new page for this doc
    Page page;
    page.m_pDoc = doc;
    page.m_pView = v;
    page.m_id = m_pKoolBar->insertItem( m_grpDocuments,
                                        DesktopIcon( m_documentEntry.service()->icon() ),
                                        i18n("Untitled"),
                                        this, SLOT( slotKoolBar( int, int ) ) );
    kdDebug() << " New page has id " << page.m_id << " doc is " << doc << endl;

    m_lstPages.append( page );

    switchToPage( m_lstPages.fromLast() );
  } else
  {
    setRootDocumentDirect( 0L, QPtrList<KoView>() );
    m_activePage = m_lstPages.end();
    KoMainWindow::updateCaption();
  }
}

void KoShellWindow::slotNewDocumentName()
{
	updateCaption();
}

void KoShellWindow::updateCaption()
{
    kdDebug() << "KoShellWindow::updateCaption() rootDoc=" << rootDocument() << endl;
    KoMainWindow::updateCaption();
    // Let's take this opportunity for setting a correct name for the icon
    // in koolbar
    QValueList<Page>::Iterator it = m_lstPages.begin();
    for( ; it != m_lstPages.end() ; ++it )
    {
      if ( (*it).m_pDoc == rootDocument() )
      {
        kdDebug() << "updateCaption called for " << rootDocument() << endl;
        // Get caption from document info (title(), in about page)
        QString name;
        if ( rootDocument()->documentInfo() )
        {
            name = rootDocument()->documentInfo()->title();
        }
        if ( name.isEmpty() )
            // Fall back to document URL
            name = rootDocument()->url().fileName();

        if ( !name.isEmpty() ) // else keep Untitled
        {
	    if ( name.length() > 20 )
	    {
	        name.truncate( 17 );
	        name += "...";
	    }
            m_pKoolBar->renameItem( m_grpDocuments, (*it).m_id, name );
        }

        return;
      }
    }
}

void KoShellWindow::slotKoolBar( int _grp, int _item )
{
  kdDebug() << "KoShellWindow::slotKoolBar "  << _grp << " " << _item << endl;
  if ( _grp == m_grpFile )
  {
    // Create new document from a KoDocumentEntry
    m_documentEntry = m_mapComponents[ _item ];
    kdDebug() << m_documentEntry.service() << endl;
    kdDebug() << m_documentEntry.name() << endl;
    KoDocument *doc = m_documentEntry.createDoc();
    if (doc)
    {
        // koshell isn't starting, but this is like starting a new app:
        // offer both "open existing file" and "open new file".
        if ( doc->initDoc( KoDocument::InitDocAppStarting) )
        {
            partManager()->addPart( doc, false );
            setRootDocument( doc );
        }
        else
            delete doc;
    }
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

void KoShellWindow::slotShowSidebar()
{
	if( m_pKoolBar->isShown() )
		m_pKoolBar->hide();
	else
		m_pKoolBar->show();
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
  // Change current document
  QPtrList<KoView> views;
  views.append(v);
  setRootDocumentDirect( (*m_activePage).m_pDoc, views );
  // Fix caption and set focus to the new view
  updateCaption();
  v->setFocus();
}

void KoShellWindow::slotFileNew()
{
    m_documentEntry = KoPartSelectDia::selectPart( this );
    if ( m_documentEntry.isEmpty() )
      return;
    KoDocument* newdoc = m_documentEntry.createDoc();
    if ( !newdoc )
        return;
    if ( !newdoc->initDoc(KoDocument::InitDocFileNew) )
    {
      delete newdoc;
      return;
    }

    partManager()->addPart( newdoc, false );
    setRootDocument( newdoc );
}

void KoShellWindow::slotFileOpen()
{
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
    if (!isImporting())
        dialog->setCaption( i18n("Open Document") );
    else
        dialog->setCaption( i18n("Import Document") );
    dialog->setMimeFilter( KoFilterManager::mimeFilter() );

    KURL url;
    if(dialog->exec()==QDialog::Accepted) {
        url=dialog->selectedURL();
        recentAction()->addURL( url );
        if ( url.isLocalFile() )
            KRecentDocument::add(url.path(-1));
        else
            KRecentDocument::add(url.url(-1), true);
    }
    else
        return;

    delete dialog;
    if ( url.isEmpty() )
        return;

    (void) openDocumentInternal( url );
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
  assert( rootDocument() == (*m_activePage).m_pDoc );

  // First do the standard queryClose
  kdDebug() << "KoShellWindow::closeDocument calling standard queryClose" << endl;
  if ( KoMainWindow::queryClose() )
  {
    kdDebug() << "Ok for closing document" << endl;
    m_pFrame->setView( 0L ); // safety measure
    m_pKoolBar->removeItem( m_grpDocuments, (*m_activePage).m_id );
    (*m_activePage).m_pDoc->removeShell(this);
    Page oldPage = (*m_activePage); // make a copy of the struct
    m_lstPages.remove( m_activePage );
    m_activePage = m_lstPages.end(); // no active page right now

    kdDebug() << "m_lstPages has " << m_lstPages.count() << " documents" << endl;
    if ( m_lstPages.count() > 0 )
    {
      kdDebug() << "Activate the document behind" << endl;
      switchToPage( m_lstPages.fromLast() );
    }
    else
    {
      kdDebug() << "Revert to initial state (no docs)" << endl;
      setRootDocument( 0L );
      partManager()->setActivePart( 0L, 0L );
    }

    // Now delete the old view and page
    // Don't do it before, because setActivePart will call slotActivePartChanged,
    // which needs the old view (to unplug it and its plugins)
    delete oldPage.m_pView;
    if ( oldPage.m_pDoc->viewCount() <= 1 )
      delete oldPage.m_pDoc;

  }
  kdDebug() << "m_lstPages has " << m_lstPages.count() << " documents" << endl;
}

bool KoShellWindow::queryClose()
{
  // Save current doc and views
  QPtrList<KoView> currentViews;
  KoDocument * currentDoc = 0L;
  bool ok = true;
  if (m_activePage != m_lstPages.end())
  {
      currentDoc = (*m_activePage).m_pDoc;
      currentViews.append((*m_activePage).m_pView);

      // This one is called by slotFileQuit and by the X button.
      // We have to check for unsaved docs...

      QValueList<Page>::Iterator it = m_lstPages.begin();
      for( ; it != m_lstPages.end(); ++it )
      {
          // This is quite a HACK
          // We should ask ourselves, to get a better dialog box
          setRootDocumentDirect( (*it).m_pDoc, QPtrList<KoView>() );
          // Test if we can close this doc
          if ( !KoMainWindow::queryClose() )
          {
              ok = false; // No
              break; // abort
          }
      }

  // Restore current doc and views
  setRootDocumentDirect( currentDoc, currentViews );
  }
  return ok;
}

/*
// Should this be an additional action in the File menu ?
bool KoShellWindow::saveAllPages()
{
  // TODO
  return false;
}
*/

void KoShellWindow::saveSettings()
{
  KoShellSettings::setSidebarWidth( m_pKoolBar->width() );	
  KoShellSettings::writeConfig();
}

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

void KoShellWindow::createShellGUI()
{
	guiFactory()->addClient( m_client );
}

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

KoShellGUIClient::KoShellGUIClient( KoShellWindow *window ) : KXMLGUIClient()
{
	setXMLFile( "koshellui.rc", true, false );

	sidebar = new KToggleAction(i18n("Show Sidebar"), "view_choose", 0, window,
			SLOT( slotShowSidebar() ), actionCollection(), "show_sidebar");
#if KDE_IS_VERSION(3,2,90)
        sidebar->setCheckedState(i18n("Hide Sidebar"));
#endif
	sidebar->setChecked( true );
}

#include "koshell_shell.moc"
