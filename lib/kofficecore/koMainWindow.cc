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
#include <qaction.h>
#include <qwhatsthis.h>
#include <qmime.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#ifdef USE_QFD
#include <qfiledialog.h>
#endif

#include <kaboutdialog.h>
#include <kaction.h>
#include <kapp.h>
#ifndef USE_QFD
#include <kfiledialog.h>
#endif
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kstdaccel.h>
#include <kstddirs.h>

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

    /*KAction* fnew =*/ new KAction( i18n("New"), KofficeBarIcon( "filenew" ), KStdAccel::key(KStdAccel::New), this, SLOT( slotFileNew() ),
			  actionCollection(), "filenew" );
    /*KAction* open =*/ new KAction( i18n("Open ..."), KofficeBarIcon( "fileopen" ), KStdAccel::key(KStdAccel::Open), this, SLOT( slotFileOpen() ),
			  actionCollection(), "fileopen" );
    /*KAction* save =*/ new KAction( i18n("Save"), KofficeBarIcon( "filefloppy" ), KStdAccel::key(KStdAccel::Save), this, SLOT( slotFileSave() ),
			  actionCollection(), "filesave" );
    /*KAction* saveAs =*/ new KAction( i18n("Save as..."), 0, this, SLOT( slotFileSaveAs() ),
			    actionCollection(), "filesaveas" );
    KAction* print = new KAction( i18n("Print..."), KofficeBarIcon( "fileprint" ), KStdAccel::key(KStdAccel::Print), this, SLOT( slotFilePrint() ),
			  actionCollection(), "fileprint" );
    /*KAction* close =*/ new KAction( i18n("Close"), KofficeBarIcon( "close" ), KStdAccel::key(KStdAccel::Close), this, SLOT( slotFileClose() ),
			  actionCollection(), "fileclose" );
    /*KAction* quit =*/ new KAction( i18n("Quit"), KofficeBarIcon( "exit" ), KStdAccel::key(KStdAccel::Quit), this, SLOT( slotFileQuit() ),
			  actionCollection(), "quit" );
    /*KAction* helpAbout =*/ new KAction( i18n("About..."), 0, this, SLOT( slotHelpAbout() ),
			  actionCollection(), "about" );

    if ( instance )
      setInstance( instance );
}

KoMainWindow::~KoMainWindow()
{
    if ( s_lstMainWindows )
	s_lstMainWindows->removeRef( this );

    delete d->m_manager;
    
    delete d;
}

void KoMainWindow::setRootDocument( KoDocument *doc )
{
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
    KoDocument* doc = rootDocument();
	
    KoDocument* newdoc = createDoc();
    if ( !newdoc->loadFromURL( url ) )
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
        KoMainWindow *s = newdoc->createShell();
        s->show();
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

    KURL url = pDoc->url();
    QCString _native_format ( KoDocument::nativeFormatMimeType() );
    QCString outputMimeType ( _native_format );

#ifndef USE_QFD
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
#endif

    if ( !url.hasPath() || saveas )
    {
#ifdef USE_QFD
        QString filter = KoFilterManager::self()->fileSelectorList( KoFilterManager::Export,
								    _native_format, nativeFormatPattern(),
                                                                    nativeFormatName(), TRUE );
#else
        KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Export,
                                              _native_format, nativeFormatPattern(),
                                              nativeFormatName(), true);
#endif
	QString file;

        //kdebug(KDEBUG_INFO, 31000, "koMainWindow, nach prepareDialog, vor exec");

	bool bOk = true;
	do {
#ifdef USE_QFD
	    file = QFileDialog::getSaveFileName( QString::null, filter );
#else
            if(dialog->exec()==QDialog::Accepted)
                file=dialog->selectedFile();
            else
                return false;

            kdebug(KDEBUG_INFO, 31000, "koMainWindow, nach exec");

            KoFilterManager::self()->cleanUp();
            delete dialog;
#endif
	    if ( file.isNull() )
		return false;

	    if ( QFileInfo( file ).extension().isEmpty() ) {
		// assume a that the the native patterns ends with .extension
		QString s( nativeFormatPattern() );
		QString extension = s.mid( s.find( "." ) );
		file += extension;
	    }

	    if ( QFile::exists( file ) ) { // this file exists => ask for confirmation
		bOk = KMessageBox::questionYesNo( this,
						  i18n("A document with this name already exists\n"\
						       "Do you want to overwrite it ?"),
						  i18n("Warning") ) == KMessageBox::Yes;
	    }
	} while ( !bOk );
	url = file;
	pDoc->setURL( url );
	KMimeType::Ptr t = KMimeType::findByURL( url, 0, TRUE );
	outputMimeType = t->mimeType();
    }

    if ( !url.isLocalFile() ) return false; // only local files

    QApplication::setOverrideCursor( waitCursor );

    if ( QFile::exists( url.path() ) ) { // this file exists => backup
	// TODO : make this configurable ?
        QString cmd = QString( "rm -rf %1~" ).arg( url.path() );
	system( cmd.local8Bit() );
	cmd = QString("cp %1 %2~").arg( url.path() ).arg( url.path() );
	system( cmd.local8Bit() );
    }

    // Not native format : save using export filter
    if ( outputMimeType != _native_format ) {
	QString nativeFile=KoFilterManager::self()->prepareExport(url.path(), _native_format);
	bool ret;
	ret = pDoc->saveToURL( nativeFile, _native_format ) && KoFilterManager::self()->export_();
	QApplication::restoreOverrideCursor();
	return ret;
    }

    bool ret = true;
    // Native format => normal save
    if ( !pDoc->saveToURL( url, _native_format ) ) {
	KMessageBox::error( this, i18n( "Could not save\n%1" ).arg(url.url()) );
	ret = false;
    }
    QApplication::restoreOverrideCursor();
    return ret;
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
	return;
    }

    setRootDocument( newdoc );
    return;
}

void KoMainWindow::slotFileOpen()
{
#ifdef USE_QFD
    QString filter = KoFilterManager::self()->fileSelectorList( KoFilterManager::Import,
								KoDocument::nativeFormatMimeType(), nativeFormatPattern(),
								nativeFormatName(), TRUE );
#else
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
    KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Import,
                                           KoDocument::nativeFormatMimeType(), nativeFormatPattern(),
                                           nativeFormatName(), true);
#endif

    KURL file;
#ifdef USE_QFD
    file = QFileDialog::getOpenFileName( QString::null, filter );
#else
    //file = KFileDialog::getOpenFileName( QString::null, filter );
    if(dialog->exec()==QDialog::Accepted)
        file=dialog->selectedURL();
    else
        return;

    KoFilterManager::self()->cleanUp();
    delete dialog;
#endif
    if ( file.url().isEmpty() )
	return;

    (void) openDocument(file);
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
    if ( closeAllDocuments() )
	kapp->exit();
}

void KoMainWindow::slotHelpAbout()
{
    KAboutDialog *dia = new KAboutDialog( KAboutDialog::AbtProduct | KAboutDialog::AbtTitle | KAboutDialog::AbtImageOnly,
					  kapp->caption(),
					  KDialogBase::Ok, KDialogBase::Ok, this, 0, TRUE );
    dia->setTitle( kapp->caption() );
    dia->setProduct( "", "pre-Beta1", "the KOffice Team", "1998-1999" );
    dia->setImage( locate( "data", "koffice/pics/koffice-logo.png" ) );
    dia->exec();
    delete dia;
}

void KoMainWindow::slotActivePartChanged( KParts::Part *newPart )
{
  qDebug( "void KoMainWindow::slotActivePartChanged( KParts::Part *newPart )" );

  if ( d->m_activePart && d->m_activePart == newPart )
  {
    qDebug( "no need to change the GUI" );
    return;
  }

  KXMLGUIFactory *factory = guiFactory();

  QValueList<KXMLGUIServant *> plugins;
  QValueList<KXMLGUIServant *>::ConstIterator pIt, pBegin, pEnd;

  setUpdatesEnabled( false );

  if ( d->m_activeView )
  {
    //TODO: event stuff

    plugins = KParts::Plugin::pluginServants( d->m_activeView );
    pIt = plugins.fromLast();
    pBegin = plugins.begin();

    for (; pIt != pBegin; --pIt )
      factory->removeServant( *pIt );

    if ( pIt != plugins.end() )
      factory->removeServant( *pIt );

    factory->removeServant( (KXMLGUIServant *)d->m_activeView );
  }

  if ( !d->bMainWindowGUIBuilt )
  {
    KParts::GUIActivateEvent ev( true );
    QApplication::sendEvent( this, &ev );

    factory->addServant( this );

    plugins = KParts::Plugin::pluginServants( this );
    pIt = plugins.begin();
    pEnd = plugins.end();
    for (; pIt != pEnd; ++pIt )
      factory->addServant( *pIt );

    d->bMainWindowGUIBuilt = true;
  }

  if ( newPart && d->m_manager->activeWidget() && d->m_manager->activeWidget()->inherits( "KoView" ) )
  {
    d->m_activeView = (KoView *)d->m_manager->activeWidget();
    d->m_activePart = newPart;

    factory->addServant( (KXMLGUIServant *)d->m_activeView );

    plugins = KParts::Plugin::pluginServants( d->m_activeView );
    pIt = plugins.begin();
    pEnd = plugins.end();
    for (; pIt != pEnd; ++pIt )
      factory->addServant( *pIt );
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
