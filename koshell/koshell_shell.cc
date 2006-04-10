/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2005, 2006 Sven Lüppken <sven@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qcursor.h>
#include <qsplitter.h>
#include <q3iconview.h>
#include <qlabel.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>
#include <Q3PtrList>

#include <assert.h>

#include "koshell_shell.h"
#include "koshellsettings.h"

#include <kapplication.h>
#include <ktempfile.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <kstandarddirs.h>
#include <klibloader.h>
#include <kmenu.h>
#include <kservice.h>
#include <kmessagebox.h>
#include <krecentdocument.h>
#include <kparts/partmanager.h>
#include <kaction.h>
#include <kdeversion.h>
#include <kaboutdata.h>

#include <KoQueryTrader.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoView.h>
#include <KoPartSelectDia.h>
#include <KoFilterManager.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>

KoShellWindow::KoShellWindow()
 : KoMainWindow( KGlobal::instance() )
{
  m_activePage = m_lstPages.end();

  m_pLayout = new QSplitter( centralWidget() );

  // Setup the sidebar
  m_pSidebar = new IconSidePane( m_pLayout );
  m_pSidebar->setSizePolicy( QSizePolicy( QSizePolicy::Maximum,
                             QSizePolicy::Preferred ) );
  m_pSidebar->setActionCollection( actionCollection() );
  m_grpFile = m_pSidebar->insertGroup(i18n("Components"), false, this, SLOT( slotSidebar_Part(int )));
  m_grpDocuments = m_pSidebar->insertGroup(i18n("Documents"), true, this, SLOT(slotSidebar_Document(int)));
  m_pLayout->setResizeMode(m_pSidebar,QSplitter::FollowSizeHint);

  // Setup the tabbar
  m_pFrame = new KTabWidget( m_pLayout );
  m_pFrame->setSizePolicy( QSizePolicy( QSizePolicy::Minimum,
                            QSizePolicy::Preferred ) );
  m_pFrame->setTabPosition( KTabWidget::Bottom );

  m_tabCloseButton = new QToolButton( m_pFrame );
  connect( m_tabCloseButton, SIGNAL( clicked() ),
           this, SLOT( slotFileClose() ) );
  m_tabCloseButton->setIconSet( SmallIconSet( "tab_remove" ) );
  m_tabCloseButton->adjustSize();
  m_tabCloseButton->setToolTip( i18n("Close"));
  m_pFrame->setCornerWidget( m_tabCloseButton, BottomRight );
  m_tabCloseButton->hide();

  Q3ValueList<KoDocumentEntry> lstComponents = KoDocumentEntry::query(false,QString());
  Q3ValueList<KoDocumentEntry>::Iterator it = lstComponents.begin();
  int id = 0;
  // Get all available components
  for( ; it != lstComponents.end(); ++it )
  {
      KService* service = (*it).service();
      if ( !service->genericName().isEmpty() )
      {
          id = m_pSidebar->insertItem(m_grpFile, service->icon(), service->genericName());
      }
      else
      {
          continue;
      }

      m_mapComponents[ id++ ] = *it;
  }

  Q3ValueList<int> list;
  list.append( KoShellSettings::sidebarWidth() );
  list.append( this->width() - KoShellSettings::sidebarWidth() );
  m_pLayout->setSizes( list );

  connect( this, SIGNAL( documentSaved() ),
           this, SLOT( slotNewDocumentName() ) );

  connect( m_pFrame, SIGNAL( currentChanged( QWidget* ) ),
           this, SLOT( slotUpdatePart( QWidget* ) ) );
  connect( m_pFrame, SIGNAL( contextMenu(QWidget * ,const QPoint &)), this, SLOT( tab_contextMenu(QWidget * ,const QPoint &)) );

  m_client = new KoShellGUIClient( this );
  createShellGUI();
}

KoShellWindow::~KoShellWindow()
{
  //kDebug() << "KoShellWindow::~KoShellWindow()" << endl;

  // Set the active part to 0 (as we do in ~KoMainWindow, but this is too
  // late for KoShell, it gets activePartChanged signals delivered to a dead
  // KoShellWindow object).
  partManager()->setActivePart(0);

  // Destroy all documents - queryClose has made sure we saved them first
  Q3ValueList<Page>::ConstIterator it = m_lstPages.begin();
  for (; it != m_lstPages.end(); ++it )
  {
    (*it).m_pDoc->removeShell( this );
    delete (*it).m_pView;
    if ( (*it).m_pDoc->viewCount() == 0 )
      delete (*it).m_pDoc;
  }
  m_lstPages.clear();

  setRootDocumentDirect( 0L, Q3PtrList<KoView>() ); // prevent our parent destructor from doing stupid things
  saveSettings(); // Now save our settings before exiting
}

bool KoShellWindow::openDocumentInternal( const KUrl &url, KoDocument* )
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
    recentAction()->removeUrl(url); //remove the file from the recent-opened-file-list
    saveRecentFiles();
    return false;
  }*/
  
  KMimeType::Ptr mimeType = KMimeType::findByURL( url );
  m_documentEntry = KoDocumentEntry::queryByMimeType( mimeType->name().latin1() );

  KTempFile* tmpFile = 0;
  KUrl tmpUrl( url );  // we might have to load a converted temp. file

  if ( m_documentEntry.isEmpty() ) { // non-native
    tmpFile = new KTempFile;

    KoFilterManager *manager = new KoFilterManager( url.path() );
    Q3CString mimetype;                                               // an empty mimetype means, that the "nearest"
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

  recentAction()->addUrl( url );

  KoDocument* newdoc = m_documentEntry.createDoc();
  if ( !newdoc ) {
      if ( tmpFile ) {
        tmpFile->unlink();
        delete tmpFile;
      }
      return false;
  }

  connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
  connect(newdoc, SIGNAL(completed()), this, SLOT(slotKSLoadCompleted()));
  connect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotKSLoadCanceled( const QString & )));
  newdoc->addShell( this ); // used by openURL
  bool openRet = (!isImporting ()) ? newdoc->openURL(tmpUrl) : newdoc->import(tmpUrl);
  if ( !openRet )
  {
      newdoc->removeShell(this);
      delete newdoc;
      if ( tmpFile ) {
        tmpFile->unlink();
        delete tmpFile;
      }
      return false;
  }

  if ( tmpFile ) {
    //if the loaded file has been a temporary file
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

void KoShellWindow::slotSidebarItemClicked( Q3IconViewItem *item )
{
  //kDebug() << "slotSidebarItemClicked called!" << endl;
  if( item != 0 )
  {
    int index = item->index();
  
    // Create new document from a KoDocumentEntry
    m_documentEntry = m_mapComponents[ index ];
    KoDocument *doc = m_documentEntry.createDoc();
    if (doc)
    {
        // koshell isn't starting, but this is like starting a new app:
        // offer both "open existing file" and "open new file".
        if ( doc->showEmbedInitDialog( this ) )
        {
            partManager()->addPart( doc, false );
            setRootDocument( doc );
        }
        else
            delete doc;
    }
  }
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

void KoShellWindow::saveAll()
{
  KoView *currentView = (*m_activePage).m_pView;
  for (Q3ValueList<Page>::iterator it=m_lstPages.begin(); it != m_lstPages.end(); ++it)
  {
    if ( (*it).m_pDoc->isModified() )
    {
      m_pFrame->showPage( (*it).m_pView );
      (*it).m_pView->shell()->slotFileSave();
      if ( (*it).m_pDoc->isModified() )
        break;
    }
  }
  m_pFrame->showPage( currentView );
}

void KoShellWindow::setRootDocument( KoDocument * doc )
{
  kDebug() << "KoShellWindow::setRootDocument this=" << this << " doc=" << doc << endl;
  // We do things quite differently from KoMainWindow::setRootDocument
  // This one is called with doc != 0 when a new doc is created
  // and with 0L after they have all been removed.
  // We use setRootDocumentDirect to switch the 'root doc' known by KoMainWindow.

  if ( doc )
  {
    if ( !doc->shells().contains( this ) )
        doc->addShell( this );

    KoView *v = doc->createView(this);
    Q3PtrList<KoView> views;
    views.append(v);
    setRootDocumentDirect( doc, views );
    
    v->setGeometry( 0, 0, m_pFrame->width(), m_pFrame->height() );
    v->setPartManager( partManager() );
    m_pFrame->addTab( v, KGlobal::iconLoader()->loadIcon( m_documentEntry.service()->icon(), K3Icon::Small ), i18n("Untitled") );
    
    // Create a new page for this doc
    Page page;
    page.m_pDoc = doc;
    page.m_pView = v;
    // insert the new document in the sidebar
    page.m_id = m_pSidebar->insertItem( m_grpDocuments,
                                       m_documentEntry.service()->icon(),
                                       i18n("Untitled"));
    m_lstPages.append( page );
    v->show();

    switchToPage( m_lstPages.fromLast() );
    mnuSaveAll->setEnabled(true);
  } else
  {
    setRootDocumentDirect( 0L, Q3PtrList<KoView>() );
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
    //kDebug() << "KoShellWindow::updateCaption() rootDoc=" << rootDocument() << endl;
    KoMainWindow::updateCaption();
    // Let's take this opportunity for setting a correct name for the icon
    // in koolbar
    Q3ValueList<Page>::Iterator it = m_lstPages.begin();
    for( ; it != m_lstPages.end() ; ++it )
    {
      if ( (*it).m_pDoc == rootDocument() )
      {
        //kDebug() << "updateCaption called for " << rootDocument() << endl;
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
          m_pFrame->changeTab( m_pFrame->currentPage(), name );
          m_pSidebar->renameItem(m_grpDocuments, (*m_activePage).m_id, name); //remove the document from the sidebar
        }

        return;
      }
    }
}


void KoShellWindow::slotSidebar_Part(int _item)
{
  //kDebug() << "Component part choosed:" << _item << endl;
  kapp->setOverrideCursor( QCursor(Qt::WaitCursor) );
  m_documentEntry = m_mapComponents[ _item ];
  kDebug() << m_documentEntry.service() << endl;
  kDebug() << m_documentEntry.name() << endl;
  KoDocument *doc = m_documentEntry.createDoc();
  kapp->restoreOverrideCursor();
  if (doc)
  {
    if ( doc->showEmbedInitDialog( this ) )
    {
      partManager()->addPart( doc, false );
      setRootDocument( doc );
      m_tabCloseButton->show();
    }
    else
      delete doc;
  }
}

void KoShellWindow::slotSidebar_Document(int _item)
{
    // Switch to an existing document
  if ( m_activePage != m_lstPages.end() &&
       (*m_activePage).m_id == _item )
    return;
    
  Q3ValueList<Page>::Iterator it = m_lstPages.begin();
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

void KoShellWindow::slotShowSidebar()
{
  if( m_pSidebar->isShown() )
  {
    m_pSidebar->hide();
    m_pComponentsLabel->hide();
  }
  else
  {
    m_pSidebar->show();
    m_pComponentsLabel->show();
  }
}

void KoShellWindow::slotUpdatePart( QWidget* widget )
{
  KoView* v = dynamic_cast<KoView*>(widget);
  if ( v != 0 ) 
  {
    Q3ValueList<Page>::Iterator it = m_lstPages.begin();
    for( ; it != m_lstPages.end(); ++it )
    {
      if( (*it).m_pView == v )
        switchToPage(it);
    }
  }
}

void KoShellWindow::switchToPage( Q3ValueList<Page>::Iterator it )
{
  // Select new active page (view)
  m_activePage = it;
  KoView *v = (*m_activePage).m_pView;

  kDebug() << " setting active part to " << (*m_activePage).m_pDoc << endl;
  // Make it active (GUI etc.)
  partManager()->setActivePart( (*m_activePage).m_pDoc, v );
  // Change current document
  Q3PtrList<KoView> views;
  views.append(v);
  setRootDocumentDirect( (*m_activePage).m_pDoc, views );
  // Select the item in the sidebar
  m_pSidebar->group(m_grpDocuments)->setSelected((*m_activePage).m_id,true);
  // Raise the new page
  m_pFrame->showPage( v );
  // Fix caption and set focus to the new view
  updateCaption();
  v->setFocus();

  partSpecificHelpAction->setEnabled(true);
  partSpecificHelpAction->setText(i18n("%1 Handbook").arg((*m_activePage).m_pDoc->instance()->aboutData()->programName()));
}

void KoShellWindow::slotFileNew()
{
    m_documentEntry = KoPartSelectDia::selectPart( this );
    if ( m_documentEntry.isEmpty() )
      return;
    KoDocument* newdoc = m_documentEntry.createDoc();
    if ( !newdoc )
        return;
    if ( !newdoc->showEmbedInitDialog( this ) )
    {
      delete newdoc;
      return;
    }

    partManager()->addPart( newdoc, false );
    setRootDocument( newdoc );
    m_tabCloseButton->show();
}

void KoShellWindow::slotFileOpen()
{
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
    if (!isImporting())
        dialog->setCaption( i18n("Open Document") );
    else
        dialog->setCaption( i18n("Import Document") );
    dialog->setMimeFilter( KoFilterManager::mimeFilter() );

    KUrl url;
    if(dialog->exec()==QDialog::Accepted) {
        url=dialog->selectedURL();
        recentAction()->addUrl( url );
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
    m_tabCloseButton->show();
}

void KoShellWindow::slotFileClose()
{
  // reimplemented to avoid closing the window when we have docs opened

  // No docs at all ?
  if ( m_lstPages.count() == 0 )
    close(); // close window
  else
    closeDocument(); // close only doc

  if ( m_pFrame->count() == 0 )
    m_tabCloseButton->hide();
}

void KoShellWindow::closeDocument()
{
  // Set the root document to the current one - so that queryClose acts on it
  assert( m_activePage != m_lstPages.end() );
  assert( rootDocument() == (*m_activePage).m_pDoc );

  // First do the standard queryClose
  kDebug() << "KoShellWindow::closeDocument calling standard queryClose" << endl;
  if ( KoMainWindow::queryClose() )
  {
    kDebug() << "Ok for closing document" << endl;
    m_pSidebar->removeItem(m_grpDocuments, (*m_activePage).m_id ); //remove the document from the sidebar
    (*m_activePage).m_pDoc->removeShell(this);
    Page oldPage = (*m_activePage); // make a copy of the struct
    m_lstPages.remove( m_activePage );
    m_activePage = m_lstPages.end(); // no active page right now
    m_pSidebar->group(m_grpDocuments)->setSelected((*m_activePage).m_id, true); //select the new document in the sidebar

    kDebug() << "m_lstPages has " << m_lstPages.count() << " documents" << endl;
    if ( m_lstPages.count() > 0 )
    {
      kDebug() << "Activate the document behind" << endl;
      switchToPage( m_lstPages.fromLast() );
    }
    else
    {
      kDebug() << "Revert to initial state (no docs)" << endl;
      setRootDocument( 0L );
      partManager()->setActivePart( 0L, 0L );
      mnuSaveAll->setEnabled(false);
      partSpecificHelpAction->setEnabled(false);
      partSpecificHelpAction->setText(i18n("Part Handbook"));
    }

    // Now delete the old view and page
    // Don't do it before, because setActivePart will call slotActivePartChanged,
    // which needs the old view (to unplug it and its plugins)
    delete oldPage.m_pView;
    if ( oldPage.m_pDoc->viewCount() <= 1 )
      delete oldPage.m_pDoc;

  }
  kDebug() << "m_lstPages has " << m_lstPages.count() << " documents" << endl;
}

bool KoShellWindow::queryClose()
{
  // Save current doc and views
  Q3PtrList<KoView> currentViews;
  KoDocument * currentDoc = 0L;
  bool ok = true;
  if (m_activePage != m_lstPages.end())
  {
      currentDoc = (*m_activePage).m_pDoc;
      currentViews.append((*m_activePage).m_pView);

      // This one is called by slotFileQuit and by the X button.
      // We have to check for unsaved docs...
      Q3ValueList<Page>::Iterator it = m_lstPages.begin();
      for( ; it != m_lstPages.end(); ++it )
      {
          // This is quite a HACK
          // We should ask ourselves, to get a better dialog box
          setRootDocumentDirect( (*it).m_pDoc, Q3PtrList<KoView>() );
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
  KoShellSettings::setSidebarWidth( m_pLayout->sizes().first() );
  KoShellSettings::writeConfig();
}

QString KoShellWindow::configFile() const
{
  //return readConfigFile( locate( "data", "koshell/koshell_shell.rc" ) );
  return QString::null; // use UI standards only for now
}

void KoShellWindow::tab_contextMenu(QWidget * w,const QPoint &p)
{
  KMenu menu;
  KIconLoader il;
  int const mnuSave = menu.insertItem( il.loadIconSet( "filesave", K3Icon::Small ), i18n("Save") );
  int const mnuClose = menu.insertItem( il.loadIcon( "fileclose", K3Icon::Small ), i18n("Close") );
  
  int tabnr = m_pFrame->indexOf( w );
  Page page = m_lstPages[tabnr];
  // disable save if there's nothing to save
  if ( !page.m_pDoc->isModified() )
    menu.setItemEnabled( mnuSave, false );
  
  // show menu
  int const choice = menu.exec(p);

  if( choice == mnuClose )
  {
    const int index = m_pFrame->currentPageIndex();
    m_pFrame->setCurrentPage( tabnr );
    slotFileClose();
    if ( index > m_pFrame->currentPageIndex() )
      m_pFrame->setCurrentPage(index-1);
    else
      m_pFrame->setCurrentPage(index);
  }
  else if ( choice == mnuSave )
  {
      page.m_pView->shell()->slotFileSave();
  }
}

void KoShellWindow::slotConfigureKeys()
{
  KoView *view = rootView();
  KKeyDialog dlg( this );
  dlg.insert( actionCollection() );
  if ( view )
     dlg.insert( view->actionCollection() );
  if ( rootDocument() )
    dlg.insert( rootDocument()->actionCollection() );
  dlg.configure();
}

void KoShellWindow::createShellGUI( bool  )
{
	guiFactory()->addClient( m_client );
}

void KoShellWindow::showPartSpecificHelp()
{
  if((m_activePage == m_lstPages.end()) || ((*m_activePage).m_pDoc == 0))
    return;

  KToolInvocation::invokeHelp("", (*m_activePage).m_pDoc->instance()->aboutData()->appName(), "");
}


///////////////////
KoShellGUIClient::KoShellGUIClient( KoShellWindow *window ) : KXMLGUIClient()
{
  setXMLFile( "koshellui.rc", true, true );
  window->mnuSaveAll = new KAction( i18n("Save All"), 0, window, SLOT( saveAll() ), actionCollection(), "save_all" );
  window->mnuSaveAll->setEnabled(false);
  window->partSpecificHelpAction = new KAction(i18n("Part Handbook"), "contents", 0, window, SLOT(showPartSpecificHelp()),
                                               actionCollection(), "partSpecificHelp");
  window->partSpecificHelpAction->setEnabled(false);
}

#include "koshell_shell.moc"
