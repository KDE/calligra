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

#include <config.h>

#include "koApplication.h"
#include "koMainWindow.h"
#include "koDocument.h"
#include "koView.h"
#include "koFilterManager.h"
#include "koIcons.h"
#include "koDocumentInfo.h"
#include "koDocumentInfoDlg.h"

#include <qkeycode.h>
#include <qfile.h>
#include <qwhatsthis.h>
#include <qmime.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qsplitter.h>

#include <kaboutdialog.h>
#include <kstdaction.h>
#include <kaction.h>
#include <khelpmenu.h>
#include <kapp.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kstddirs.h>
#include <kio/netaccess.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>

#include <kparts/partmanager.h>
#include <kparts/plugin.h>
#include <kparts/event.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>

QList<KoMainWindow>* KoMainWindow::s_lstMainWindows = 0;

class KoMainWindowPrivate
{
public:
  KoMainWindowPrivate()
  {
    m_rootDoc = 0L;
    m_rootView = 0L;
    m_manager = 0L;
    bMainWindowGUIBuilt = false;
    m_activePart = 0L;
    m_activeView = 0L;
    m_splitViewActionList=0L;
    m_splitter=0L;
  }
  ~KoMainWindowPrivate()
  {
  }

  KoDocument *m_rootDoc;
  KoView *m_rootView;
  KParts::PartManager *m_manager;

  KParts::Part *m_activePart;
  KoView *m_activeView;

  QList<KAction> *m_splitViewActionList;
  QSplitter *m_splitter;

  bool bMainWindowGUIBuilt;
};

KoMainWindow::KoMainWindow( KInstance *instance, const char* name )
    : KParts::MainWindow( name )
{
    if ( !s_lstMainWindows )
	s_lstMainWindows = new QList<KoMainWindow>;
    s_lstMainWindows->append( this );

    d = new KoMainWindowPrivate;

    d->m_manager = new KParts::PartManager( this );
    d->m_manager->setSelectionPolicy( KParts::PartManager::TriState );
    d->m_manager->setAllowNestedParts( true );

    connect( d->m_manager, SIGNAL( activePartChanged( KParts::Part * ) ),
	     this, SLOT( slotActivePartChanged( KParts::Part * ) ) );

    setXMLFile( locate( "data", "koffice/koffice_shell.rc" ) );

    KStdAction::openNew( this, SLOT( slotFileNew() ), actionCollection(), "file_new" );
    KStdAction::open( this, SLOT( slotFileOpen() ), actionCollection(), "file_open" );
    m_recent = KStdAction::openRecent( this, SLOT(slotFileOpenRecent(const KURL&)), actionCollection() );
    KStdAction::save( this, SLOT( slotFileSave() ), actionCollection(), "file_save" );
    KStdAction::saveAs( this, SLOT( slotFileSaveAs() ), actionCollection(), "file_save_as" );
    KStdAction::print( this, SLOT( slotFilePrint() ), actionCollection(), "file_print" );
    KStdAction::close( this, SLOT( slotFileClose() ), actionCollection(), "file_close" );
    KStdAction::quit( this, SLOT( slotFileQuit() ), actionCollection(), "file_quit" );

    (void) new KAction( i18n( "Document Information..." ), 0,
			this, SLOT( slotDocumentInfo() ),
			actionCollection(), "file_documentinfo" );

    (void) new KAction( i18n( "Configure &Keys..." ), 0, this,
      SLOT( slotConfigureKeys() ), actionCollection(), "configurekeys" );

    (void) new KAction( i18n( "Configure Tool&bars..." ), 0, this,
      SLOT( slotConfigureToolbars() ), actionCollection(), "configuretoolbars" );

    KHelpMenu * m_helpMenu = new KHelpMenu( this );
    KStdAction::helpContents( m_helpMenu, SLOT( appHelpActivated() ), actionCollection(), "contents" );
    KStdAction::whatsThis( m_helpMenu, SLOT( contextHelpActivated() ), actionCollection(), "whats_this" );
    KStdAction::aboutApp( this, SLOT( slotHelpAbout() ), actionCollection(), "about_app" );
    KStdAction::aboutKDE( m_helpMenu, SLOT( aboutKDE() ), actionCollection(), "about_kde" );
    KStdAction::reportBug( m_helpMenu, SLOT( reportBug() ), actionCollection(), "report_bug" );

    // set up the action list for the splitter stuff
    d->m_splitViewActionList=new QList<KAction>;
    d->m_splitViewActionList->append(new KAction(i18n("Split View"), 0, this, SLOT(slotSplitView()),
						actionCollection(), "view_split"));
    d->m_splitViewActionList->append(new KAction(i18n("Remove View"), 0, this, SLOT(slotRemoveView()),
						actionCollection(), "view_rm_splitter"));
    KSelectAction *orientation=new KSelectAction(i18n("Splitter Orientation"), 0, this, SLOT(slotSetOrientation()),
						 actionCollection(), "view_splitter_orientation");
    QStringList items;
    items << i18n("Vertical")
	  << i18n("Horizontal");
    orientation->setItems(items);
    orientation->setCurrentItem(0);
    d->m_splitViewActionList->append(orientation);

    if ( instance )
      setInstance( instance );

    // Load list of recent files
    KConfig * config = instance ? instance->config() : KGlobal::config();
    m_recent->loadEntries( config );
    config->sync();

    d->m_splitter=new QSplitter(Qt::Vertical, this, "funky-splitter");
    setView( d->m_splitter );

    buildMainWindowGUI();
    //createGUI( 0L ); // NOT this ! (duplicates shell entries !)

    if (QApplication::desktop()->width() > 1100) // very big desktop ?
        resize( 900, 800 );
    if (QApplication::desktop()->width() > 850) // big desktop ?
        resize( 700, 600 );
    else // small (800x600, 640x480) desktop
        resize( 600, 400 );
}

KoMainWindow::~KoMainWindow()
{
    // The doc and view might still exist (this is the case when closing the window)
    if (d->m_rootDoc)
        d->m_rootDoc->removeShell(this);
    delete d->m_rootView;
    if ( d->m_rootDoc && d->m_rootDoc->viewCount() == 0 )
    {
        kdDebug(30003) << "Destructor. No more views, deleting old doc " << d->m_rootDoc << endl;
        delete d->m_rootDoc;
    }

    // Save list of recent files
    KConfig * config = instance() ? instance()->config() : KGlobal::config();
    kdDebug() << "Saving recent files list into config. instance()=" << instance() << endl;
    m_recent->saveEntries( config );
    config->sync();

    if ( s_lstMainWindows )
	s_lstMainWindows->removeRef( this );

    delete d->m_manager;
    delete d->m_splitViewActionList;
    delete d->m_splitter;
    d->m_splitter=0L;

    delete d;
}

void KoMainWindow::setRootDocument( KoDocument *doc )
{
  kdDebug(30003) <<  "KoMainWindow::setRootDocument this = " << this << " doc = " << doc << endl;
  KoView *oldRootView = d->m_rootView;
  KoDocument *oldRootDoc = d->m_rootDoc;

  if ( oldRootDoc )
    oldRootDoc->removeShell( this );

  d->m_rootDoc = doc;

  if ( doc )
  {
    doc->setSelectable( false );
    d->m_rootView = doc->createView( d->m_splitter );
    d->m_rootView->setPartManager( d->m_manager );

    d->m_rootView->show();
    d->m_rootDoc->addShell( this );
  }
  else
    d->m_rootView = 0L;

  updateCaption();

  d->m_manager->setActivePart( d->m_rootDoc, d->m_rootView );

  if ( oldRootView )
    delete oldRootView;
  if ( oldRootDoc && oldRootDoc->viewCount() == 0 )
  {
    kdDebug(30003) << "No more views, deleting old doc " << oldRootDoc << endl;
    delete oldRootDoc;
  }
}

void KoMainWindow::setRootDocumentDirect( KoDocument *doc )
{
  d->m_rootDoc = doc;
  // maybe we want to add the KoView as parameter and set it into
  // d->m_rootView but it doesn't seem used at all ?!? (David)
}

void KoMainWindow::updateCaption()
{
  kdDebug(30003) << "KoMainWindow::updateCaption()" << endl;
  if ( !d->m_rootDoc )
    setCaption(QString::null);
  else
  {
      QString caption;
      // Get caption from document info (title(), in about page)
      if ( d->m_rootDoc->documentInfo() )
      {
          KoDocumentInfoPage * page = d->m_rootDoc->documentInfo()->page( QString::fromLatin1("about"));
          if (page)
              caption = static_cast<KoDocumentInfoAbout *>(page)->title();
      }
      if ( caption.isEmpty() )
          // Fall back to document URL
          caption = d->m_rootDoc->url().prettyURL();

      // KTMW hides some of the functionality of kapp->makeStdCaption !
      QWidget::setCaption( kapp->makeStdCaption( caption, true, d->m_rootDoc->isModified() ) );
  }
}

KoDocument *KoMainWindow::rootDocument() const
{
  return d->m_rootDoc;
}

KoView *KoMainWindow::rootView() const
{
  return d->m_rootView;
}

KParts::PartManager *KoMainWindow::partManager()
{
  return d->m_manager;
}

KoMainWindow* KoMainWindow::firstMainWindow()
{
    if ( !s_lstMainWindows )
	return 0;

    return s_lstMainWindows->first();
}

KoMainWindow* KoMainWindow::nextMainWindow()
{
    if ( !s_lstMainWindows )
	return 0;

    return s_lstMainWindows->next();
}

bool KoMainWindow::openDocument( const KURL & url )
{
    m_recent->addURL( url );
    KoDocument* doc = rootDocument();
	
    KoDocument* newdoc = createDoc();
    if ( !newdoc || !newdoc->openURL( url ) )
    {
	delete newdoc;
	return FALSE;
    }

    if ( doc && doc->isEmpty() )
    {
        // Replace current empty document
	setRootDocument( newdoc );
    }
    else if ( doc && !doc->isEmpty() )
    {
        // Open in a new shell
        // (Note : could create the shell first and the doc next for this
        // particular case, that would give a better user feedback...)
        KoMainWindow *s = newdoc->createShell();
        s->show();
        s->setRootDocument( newdoc );
    }
    else
    {
        // We had no document, set the new one
        setRootDocument( newdoc );
    }
    return TRUE;
}

bool KoMainWindow::saveDocument( bool saveas )
{
    KoDocument* pDoc = rootDocument();

    QCString _native_format = pDoc->nativeFormatMimeType();

    if ( pDoc->url().isEmpty() || saveas )
    {
        KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
        dialog->setCaption( i18n("Save document as") );
        KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Export,
                                              _native_format, nativeFormatPattern(),
                                              nativeFormatName(), true);
	KURL newURL;

        bool bOk = true;
	do {
            if(dialog->exec()==QDialog::Accepted)
                newURL=dialog->selectedURL();
            else
            {
                bOk = false;
                break;
            }

	    if ( newURL.isEmpty() )
            {
                bOk = false;
                break;
            }

            // HACK - should we add extension() to KURL ?
	    if ( QFileInfo( newURL.path() ).extension().isEmpty() ) {
		// assume that the pattern ends with .extension
		QString s( dialog->currentFilter() );
		QString extension = s.mid( s.find( "." ) );
		newURL.setPath( newURL.path() + extension );
	    }

	    if ( KIO::NetAccess::exists( newURL ) ) { // this file exists => ask for confirmation
		bOk = KMessageBox::questionYesNo( this,
						  i18n("A document with this name already exists\n"\
						       "Do you want to overwrite it ?"),
						  i18n("Warning") ) == KMessageBox::Yes;
	    }
        } while ( !bOk );
        KoFilterManager::self()->cleanUp();
        delete dialog;
        if (bOk)
        {
            m_recent->addURL( newURL );
            bool ret = pDoc->saveAs( newURL );
            pDoc->setTitleModified();
            return ret;
        }
        return false;
    }
    else
      return pDoc->save();
}

bool KoMainWindow::queryClose()
{
  if ( rootDocument() == 0 )
    return TRUE;
  kdDebug(30003) << "KoMainWindow::queryClose() viewcount=" << rootDocument()->viewCount() << endl;
  if ( rootDocument()->viewCount() > 1 ) // last view ?
    return TRUE; // no, so no problem for closing

  if ( rootDocument()->isModified() )
  {
      int res = KMessageBox::warningYesNoCancel( 0L,
                    i18n( "The document has been modified\nDo you want to save it ?" ));

      switch(res) {
          case KMessageBox::Yes :
              if (! saveDocument() )
                  return false;
          case KMessageBox::No :
              break;
          default : // case KMessageBox::Cancel :
              return FALSE;
      }
  }

  return TRUE;
}

bool KoMainWindow::closeAllDocuments()
{
    KoMainWindow* win = firstMainWindow();
    for( ; win; win = nextMainWindow() )
    {
	if ( !win->queryClose() )
	    return FALSE;
        else
            win->setRootDocument( 0L );
    }

    return TRUE;
}

void KoMainWindow::slotFileNew()
{
    KoDocument* doc = rootDocument();

    KoDocument* newdoc = createDoc();
    if ( !newdoc || !newdoc->initDoc() )
    {
	delete newdoc;
	return;
    }

    if ( doc && doc->isEmpty() )
    {
	setRootDocument( newdoc );
	return;
    }
    else if ( doc && !doc->isEmpty() )
    {
        KoMainWindow *s = newdoc->createShell();
        s->show();
        s->setRootDocument( newdoc );
	return;
    }

    setRootDocument( newdoc );
    return;
}

void KoMainWindow::slotFileOpen()
{
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
    dialog->setCaption( i18n("Open document") );
    KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Import,
                                           KoDocument::readNativeFormatMimeType(), nativeFormatPattern(),
                                           nativeFormatName(), true);
    KURL url;
    if(dialog->exec()==QDialog::Accepted)
        url=dialog->selectedURL();
    else
        return;

    KoFilterManager::self()->cleanUp();
    delete dialog;
    if ( url.isEmpty() )
	return;

    (void) openDocument(url);
}

void KoMainWindow::slotFileOpenRecent( const KURL & url )
{
  (void) openDocument( url );
}

void KoMainWindow::slotFileSave()
{
    saveDocument();
}

void KoMainWindow::slotFileSaveAs()
{
    saveDocument( TRUE );
}

void KoMainWindow::slotDocumentInfo()
{
  if ( !d->m_rootDoc )
    return;

  KoDocumentInfo *docInfo = d->m_rootDoc->documentInfo();

  if ( !docInfo )
    return;

  KoDocumentInfoDlg *dlg = new KoDocumentInfoDlg( docInfo, this, "documentInfoDlg" );
  if ( dlg->exec() )
  {
    dlg->save();
    d->m_rootDoc->setModified( true );
    d->m_rootDoc->setTitleModified();
  }

  delete dlg;
}

void KoMainWindow::slotFileClose()
{
    if ( queryClose() )
    {
        setRootDocument( 0L );
	close();
    }
}

void KoMainWindow::slotFilePrint()
{
}

void KoMainWindow::slotFileQuit()
{
    // The style guide says: 'quit' closes the window.
    // (which it calls "Application", but this does not mean kapp->quit())
    close();
}

void KoMainWindow::slotConfigureKeys()
{
  KKeyDialog::configureKeys(actionCollection(), xmlFile());
}

void KoMainWindow::slotConfigureToolbars()
{
  KEditToolbar edit(factory());
  edit.exec();
}

void KoMainWindow::slotHelpAbout()
{
    KAboutDialog *dia = new KAboutDialog( KAboutDialog::AbtProduct | KAboutDialog::AbtTitle | KAboutDialog::AbtImageOnly,
					  kapp->caption(),
					  KDialogBase::Ok, KDialogBase::Ok, this, 0, TRUE );
    dia->setTitle( kapp->caption() );
    dia->setProduct( "", "pre-Beta1", "the KOffice Team", "1998-2000" );
    dia->setImage( locate( "data", "koffice/pics/koffice-logo.png" ) );
    dia->exec();
    delete dia;
}

void KoMainWindow::slotSplitView() {
    kdDebug(30003) << "KoMainWindow::slotSplitView() called" << endl;
}

void KoMainWindow::slotRemoveView() {
    kdDebug(30003) << "KoMainWindow::slotRemoveView() called" << endl;
}

void KoMainWindow::slotSetOrientation() {
    kdDebug(30003) << "KoMainWindow::slotSetOrientation() called" << endl;
    // access the state via d->m_splitViewActionList->last()->currentItem()
}


void KoMainWindow::buildMainWindowGUI()
{
  KXMLGUIFactory *factory = guiFactory();
  QValueList<KXMLGUIClient *> plugins;
  QValueList<KXMLGUIClient *>::ConstIterator pIt, pEnd;
  if ( !d->bMainWindowGUIBuilt )
  {
    KParts::GUIActivateEvent ev( true );
    QApplication::sendEvent( this, &ev );

    factory->addClient( this );

    plugins = KParts::Plugin::pluginClients( this );
    pIt = plugins.begin();
    pEnd = plugins.end();
    for (; pIt != pEnd; ++pIt )
      factory->addClient( *pIt );

    d->bMainWindowGUIBuilt = true;
  }
}

void KoMainWindow::slotActivePartChanged( KParts::Part *newPart )
{
  kdDebug(30003) <<  "KoMainWindow::slotActivePartChanged( Part * newPart) newPart = " <<
    newPart << endl;
  kdDebug(30003) <<  "current active part is " << d->m_activePart << endl;

  if ( d->m_activePart && d->m_activePart == newPart )
  {
    kdDebug(30003) << "no need to change the GUI" << endl;
    return;
  }

  KXMLGUIFactory *factory = guiFactory();

  QValueList<KXMLGUIClient *> plugins;
  QValueList<KXMLGUIClient *>::ConstIterator pIt, pBegin, pEnd;

  setUpdatesEnabled( false );

  if ( d->m_activeView )
  {
    //TODO: event stuff

    plugins = KParts::Plugin::pluginClients( d->m_activeView );
    pIt = plugins.fromLast();
    pBegin = plugins.begin();

    for (; pIt != pBegin; --pIt )
      factory->removeClient( *pIt );

    if ( pIt != plugins.end() )
      factory->removeClient( *pIt );

    factory->removeClient( (KXMLGUIClient *)d->m_activeView );
  }

  buildMainWindowGUI();

  if ( newPart && d->m_manager->activeWidget() && d->m_manager->activeWidget()->inherits( "KoView" ) )
  {
    d->m_activeView = (KoView *)d->m_manager->activeWidget();
    d->m_activePart = newPart;
    kdDebug(30003) <<  "new active part is " << d->m_activePart << endl;

    factory->addClient( (KXMLGUIClient *)d->m_activeView );

    plugins = KParts::Plugin::pluginClients( d->m_activeView );
    pIt = plugins.begin();
    pEnd = plugins.end();
    for (; pIt != pEnd; ++pIt )
      factory->addClient( *pIt );

    if(d->m_activeView==d->m_rootView)
	factory->plugActionList((KXMLGUIClient*)d->m_activeView, "view_split", *d->m_splitViewActionList );
  }
  else
  {
    d->m_activeView = 0L;
    d->m_activePart = 0L;
  }

  setUpdatesEnabled( true );
  updateRects();
}

#include "koMainWindow.moc"
