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

#include <qkeycode.h>
#include <qfile.h>
#include <qwhatsthis.h>
#include <qmime.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#ifdef USE_QFD
#include <qfiledialog.h>
#endif

#include <kaboutdialog.h>
#include <kstdaction.h>
#include <kaction.h>
#include <khelpmenu.h>
#include <kapp.h>
#ifndef USE_QFD
#include <kfiledialog.h>
#endif
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kstddirs.h>
#include <kio/netaccess.h>

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
  }
  ~KoMainWindowPrivate()
  {
  }

  KoDocument *m_rootDoc;
  KoView *m_rootView;
  KParts::PartManager *m_manager;

  KParts::Part *m_activePart;
  KoView *m_activeView;

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

    KHelpMenu * m_helpMenu = new KHelpMenu( this );
    KStdAction::helpContents( m_helpMenu, SLOT( appHelpActivated() ), actionCollection(), "contents" );
    KStdAction::whatsThis( m_helpMenu, SLOT( contextHelpActivated() ), actionCollection(), "whats_this" );
    KStdAction::aboutApp( this, SLOT( slotHelpAbout() ), actionCollection(), "about_app" );
    KStdAction::aboutKDE( m_helpMenu, SLOT( aboutKDE() ), actionCollection(), "about_kde" );
    KStdAction::reportBug( m_helpMenu, SLOT( reportBug() ), actionCollection(), "report_bug" );

    if ( instance )
      setInstance( instance );

    // Load list of recent files
    KConfig * config = instance ? instance->config() : KGlobal::config();
    m_recent->loadEntries( config );
    config->sync();

    // we need this for a proper layout of the toolbars upon startup,
    // when no real view has been loaded, yet
    m_dummyWidget = new QWidget( this );
    setView( m_dummyWidget );

    buildMainWindowGUI();
    //createGUI( 0L ); // NOT this ! (duplicates shell entries !)
}

KoMainWindow::~KoMainWindow()
{
    // Save list of recent files
    KConfig * config = instance() ? instance()->config() : KGlobal::config();
    m_recent->saveEntries( config );
    config->sync();

    if ( s_lstMainWindows )
	s_lstMainWindows->removeRef( this );

    delete d->m_manager;

    delete d;
}

void KoMainWindow::setRootDocument( KoDocument *doc )
{
  kdDebug(31000) <<  "KoMainWindow::setRootDocument this = " << this << " doc = " << doc << endl;
  KoView *oldRootView = d->m_rootView;

  if ( d->m_rootDoc )
    d->m_rootDoc->removeShell( this );

  d->m_rootDoc = doc;

  if ( doc )
  {
    doc->setSelectable( false );
    d->m_rootView = doc->createView( this );
    d->m_rootView->setPartManager( d->m_manager );

    setView( d->m_rootView );
    if ( m_dummyWidget )
    {
      delete m_dummyWidget;
      m_dummyWidget = 0L;
    }
    d->m_rootView->show();
    d->m_rootDoc->addShell( this );
  }
  else
    d->m_rootView = 0L;

  d->m_manager->setActivePart( d->m_rootDoc, d->m_rootView );

  if ( oldRootView )
    delete oldRootView;
}

KoDocument *KoMainWindow::rootDocument() const
{
  return d->m_rootDoc;
}

KoView *KoMainWindow::rootView() const
{
  return (KoView *)view();
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
    if ( !newdoc->openURL( url ) )
    {
	delete newdoc;
	return FALSE;
    }

    if ( doc && doc->isEmpty() )
    {
        // Replace current empty document
	setRootDocument( newdoc );
	delete doc;
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
#ifdef USE_QFD
        QString filter = KoFilterManager::self()->fileSelectorList( KoFilterManager::Export,
								    _native_format, nativeFormatPattern(),
                                                                    nativeFormatName(), TRUE );
#else
        KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
        KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Export,
                                              _native_format, nativeFormatPattern(),
                                              nativeFormatName(), true);
#endif
	KURL newURL;

        bool bOk = true;
	do {
#ifdef USE_QFD
	    QString file = QFileDialog::getSaveFileName( QString::null, filter );
            if ( file.isNull() )
                return false;
            KURL::encode(file);
            newURL = file;
#else
            if(dialog->exec()==QDialog::Accepted)
                newURL=dialog->selectedURL();
            else
                return false;

            KoFilterManager::self()->cleanUp();
            delete dialog;
#endif
	    if ( newURL.isEmpty() )
		return false;

            // HACK - should we add extension() to KURL ?
	    if ( QFileInfo( newURL.path() ).extension().isEmpty() ) {
		// assume a that the the native patterns ends with .extension
		QString s( nativeFormatPattern() );
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
        m_recent->addURL( newURL );
        return pDoc->saveAs( newURL );
    }
    else
      return pDoc->save();
}

bool KoMainWindow::queryClose()
{
  return closeDocument();
}

bool KoMainWindow::closeDocument()
{
    if ( rootDocument() == 0 )
	return TRUE;

    if ( rootDocument()->isModified() )
    {
	int res = KMessageBox::warningYesNoCancel( 0L,
           i18n( "The document has been modified\nDo you want to save it ?" ));

        switch(res) {
        case KMessageBox::Yes :
	    return saveDocument();
        case KMessageBox::No :
          {
            KoDocument* doc = rootDocument();
    	    setRootDocument( 0 );
            delete doc;
            return TRUE;
          }
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
	if ( !win->closeDocument() )
	    return FALSE;
    }

    return TRUE;
}

void KoMainWindow::slotFileNew()
{
    KoDocument* doc = rootDocument();

    KoDocument* newdoc = createDoc();
    if ( !newdoc->initDoc() )
    {
	delete newdoc;
	return;
    }

    if ( doc && doc->isEmpty() )
    {
	setRootDocument( newdoc );
	delete doc;
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
#ifdef USE_QFD
    QString filter = KoFilterManager::self()->fileSelectorList( KoFilterManager::Import,
								KoDocument::readNativeFormatMimeType(), nativeFormatPattern(),
								nativeFormatName(), TRUE );
#else
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
    KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Import,
                                           KoDocument::readNativeFormatMimeType(), nativeFormatPattern(),
                                           nativeFormatName(), true);
#endif

    KURL url;
#ifdef USE_QFD
    QString file = QFileDialog::getOpenFileName( QString::null, filter );
    if (file.isEmpty()) return;
    KURL::encode(file);
    url = file;
#else
    if(dialog->exec()==QDialog::Accepted)
        url=dialog->selectedURL();
    else
        return;

    KoFilterManager::self()->cleanUp();
    delete dialog;
#endif
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

void KoMainWindow::slotFileClose()
{
    if ( closeDocument() )
	close();
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
  kdDebug(31000) <<  "KoMainWindow::slotActivePartChanged( Part * newPart) newPart = " <<
    newPart << endl;
  kdDebug(31000) <<  "active part is " << d->m_activePart << endl;

  if ( d->m_activePart && d->m_activePart == newPart )
  {
    kdDebug(31000) << "no need to change the GUI" << endl;
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

    factory->addClient( (KXMLGUIClient *)d->m_activeView );

    plugins = KParts::Plugin::pluginClients( d->m_activeView );
    pIt = plugins.begin();
    pEnd = plugins.end();
    for (; pIt != pEnd; ++pIt )
      factory->addClient( *pIt );
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
