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

#include <koApplication.h>
#include <koMainWindow.h>
#include <koDocument.h>
#include <koView.h>
#include <koFilterManager.h>
#include <koDocumentInfo.h>
#include <koDocumentInfoDlg.h>
#include <koQueryTrader.h>
#include <koPrintPreview.h>
#include "KoMainWindowIface.h"

#include <qkeycode.h>
#include <qfile.h>
#include <qwhatsthis.h>
#include <qmime.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qsplitter.h>
#include <koprinter.h>
#include <qobjectlist.h>

#include <kstdaction.h>
#include <kaction.h>
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
#include <kprogress.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <ktempfile.h>
#include <krecentdocument.h>

#include <kparts/partmanager.h>
#include <kparts/plugin.h>
#include <kparts/event.h>

#include <unistd.h>
#include <stdlib.h>

class KoPartManager : public KParts::PartManager
{
public:
  KoPartManager( QWidget * parent, const char * name = 0L )
    : KParts::PartManager( parent, name ) {}
  KoPartManager( QWidget *topLevel, QObject *parent, const char *name = 0L )
    : KParts::PartManager( topLevel, parent, name ) {}
  virtual bool eventFilter( QObject *obj, QEvent *ev )
  {
    if ( !obj->isWidgetType() || obj->inherits( "KoFrame" ) )
      return false;
    return KParts::PartManager::eventFilter( obj, ev );
  }
};

class KoMainWindowPrivate
{
public:
  KoMainWindowPrivate()
  {
    m_rootDoc = 0L;
    m_manager = 0L;
    bMainWindowGUIBuilt = false;
    m_forQuit=false;
    m_splitted=false;
    m_activePart = 0L;
    m_activeView = 0L;
    m_splitter=0L;
    m_orientation=0L;
    m_removeView=0L;
    m_toolbarList.setAutoDelete( true );
    m_firstTime=true;
    m_progress=0L;
    m_paDocInfo = 0;
    m_paSave = 0;
    m_paSaveAs = 0;
    m_paPrint = 0;
    m_paPrintPreview = 0;
    statusBarLabel = 0L;
    m_dcopObject = 0;
  }
  ~KoMainWindowPrivate()
  {
    delete m_dcopObject;
  }

  KoDocument *m_rootDoc;
  QList<KoView> m_rootViews;
  KParts::PartManager *m_manager;

  KParts::Part *m_activePart;
  KoView *m_activeView;

  QLabel * statusBarLabel;
  KProgress *m_progress;

  QList<KAction> m_splitViewActionList;
  // This additional list is needed, because we don't plug
  // the first list, when an embedded view gets activated (Werner)
  QList<KAction> m_veryHackyActionList;
  QSplitter *m_splitter;
  KSelectAction *m_orientation;
  KAction *m_removeView;
  KoMainWindowIface *m_dcopObject;

  QList <KAction> m_toolbarList;

  bool bMainWindowGUIBuilt;
  bool m_splitted;
  bool m_forQuit;
  bool m_firstTime;

  KAction *m_paDocInfo;
  KAction *m_paSave;
  KAction *m_paSaveAs;
  KAction *m_paPrint;
  KAction *m_paPrintPreview;
};

KoMainWindow::KoMainWindow( KInstance *instance, const char* name )
    : KParts::MainWindow( name )
{
    ASSERT(instance);
    d = new KoMainWindowPrivate;

    d->m_manager = new KoPartManager( this );
    d->m_manager->setSelectionPolicy( KParts::PartManager::TriState );
    d->m_manager->setAllowNestedParts( true );
    d->m_manager->setIgnoreScrollBars( true );

    connect( d->m_manager, SIGNAL( activePartChanged( KParts::Part * ) ),
             this, SLOT( slotActivePartChanged( KParts::Part * ) ) );

    if ( instance )
        setInstance( instance, false ); // don't load plugins! we don't want
    // the part's plugins with this shell, even though we are using the
    // part's instance! (Simon)

    QString doc;
    QStringList allFiles = KGlobal::dirs()->findAllResources( "data", "koffice/koffice_shell.rc" );
    setXMLFile( findMostRecentXMLFile( allFiles, doc ) );
    setLocalXMLFile( locateLocal( "data", "koffice/koffice_shell.rc" ) );

    KStdAction::openNew( this, SLOT( slotFileNew() ), actionCollection(), "file_new" );
    KStdAction::open( this, SLOT( slotFileOpen() ), actionCollection(), "file_open" );
    m_recent = KStdAction::openRecent( this, SLOT(slotFileOpenRecent(const KURL&)), actionCollection() );
    d->m_paSave = KStdAction::save( this, SLOT( slotFileSave() ), actionCollection(), "file_save" );
    d->m_paSaveAs = KStdAction::saveAs( this, SLOT( slotFileSaveAs() ), actionCollection(), "file_save_as" );
    d->m_paPrint = KStdAction::print( this, SLOT( slotFilePrint() ), actionCollection(), "file_print" );
    d->m_paPrintPreview = KStdAction::printPreview( this, SLOT( slotFilePrintPreview() ), actionCollection(), "file_print_preview" );
    KStdAction::close( this, SLOT( slotFileClose() ), actionCollection(), "file_close" );
    KStdAction::quit( this, SLOT( slotFileQuit() ), actionCollection(), "file_quit" );

    d->m_paDocInfo = new KAction( i18n( "&Document Information..." ), "documentinfo", 0,
                        this, SLOT( slotDocumentInfo() ),
                        actionCollection(), "file_documentinfo" );

    KStdAction::keyBindings( this, SLOT( slotConfigureKeys() ), actionCollection(), "configurekeys" );
    KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection(), "configuretoolbars" );

    d->m_paDocInfo->setEnabled( false );
    d->m_paSaveAs->setEnabled( false );
    d->m_paSave->setEnabled( false );
    d->m_paPrint->setEnabled( false );
    d->m_paPrintPreview->setEnabled( false );

    d->m_splitter=new QSplitter(Qt::Vertical, this, "funky-splitter");
    setCentralWidget( d->m_splitter );

    // set up the action "list" for "Close all Views" (hacky :) (Werner)
    d->m_veryHackyActionList.append(
        new KAction(i18n("&Close All Views"), "fileclose",
                    0, this, SLOT(slotCloseAllViews()),
                    actionCollection(), "view_closeallviews") );

    // set up the action list for the splitter stuff
    d->m_splitViewActionList.append(new KAction(i18n("&Split View"), "view_split", 0,
        this, SLOT(slotSplitView()),
        actionCollection(), "view_split"));
    d->m_removeView=new KAction(i18n("&Remove View"), "view_remove", 0,
        this, SLOT(slotRemoveView()),
        actionCollection(), "view_rm_splitter");
    d->m_splitViewActionList.append(d->m_removeView);
    d->m_removeView->setEnabled(false);
    d->m_orientation=new KSelectAction(i18n("Splitter &Orientation"), "view_orientation", 0,
        this, SLOT(slotSetOrientation()),
        actionCollection(), "view_splitter_orientation");
    QStringList items;
    items << i18n("&Vertical")
          << i18n("&Horizontal");
    d->m_orientation->setItems(items);
    d->m_orientation->setCurrentItem(static_cast<int>(d->m_splitter->orientation()));
    d->m_splitViewActionList.append(d->m_orientation);
    d->m_splitViewActionList.append(new KActionSeparator(this));

    // Load list of recent files
    KConfig * config = instance ? instance->config() : KGlobal::config();
    m_recent->loadEntries( config );

    createShellGUI();
    d->bMainWindowGUIBuilt = true;

    if ( !initialGeometrySet() )
    {
        // Default size
        if (QApplication::desktop()->width() > 1100) // very big desktop ?
            resize( 900, 800 );
        if (QApplication::desktop()->width() > 850) // big desktop ?
            resize( 700, 600 );
        else // small (800x600, 640x480) desktop
            resize( 600, 400 );
    }

    // Saved size
    config->setGroup( "MainWindow" );
    //kdDebug(30003) << "KoMainWindow::restoreWindowSize" << endl;
    restoreWindowSize( config );
}

KoMainWindow::~KoMainWindow()
{
    // The doc and view might still exist (this is the case when closing the window)
    if (d->m_rootDoc)
        d->m_rootDoc->removeShell(this);

    // safety first ;)
    d->m_manager->setActivePart(0);

    if(d->m_rootViews.findRef(d->m_activeView)==-1) {
        delete d->m_activeView;
        d->m_activeView=0L;
    }
    d->m_rootViews.setAutoDelete( true );
    d->m_rootViews.clear();

    // We have to check if this was a root document.
    // -> We aren't allowed to delete the (embedded) document!
    // This has to be checked from queryClose, too :)
    if ( d->m_rootDoc && d->m_rootDoc->viewCount() == 0 &&
         !d->m_rootDoc->isEmbedded())
    {
        //kdDebug(30003) << "Destructor. No more views, deleting old doc " << d->m_rootDoc << endl;
        delete d->m_rootDoc;
    }

    // Save list of recent files
    KConfig * config = instance() ? instance()->config() : KGlobal::config();
    //kdDebug() << "Saving recent files list into config. instance()=" << instance() << endl;
    m_recent->saveEntries( config );
    config->sync();

    delete d->m_manager;
    delete d;
}

void KoMainWindow::setRootDocument( KoDocument *doc )
{
  if ( d->m_rootDoc == doc )
    return;

  //kdDebug(30003) <<  "KoMainWindow::setRootDocument this = " << this << " doc = " << doc << endl;
  QList<KoView> oldRootViews = d->m_rootViews;
  d->m_rootViews.clear();
  KoDocument *oldRootDoc = d->m_rootDoc;

  if ( oldRootDoc )
    oldRootDoc->removeShell( this );

  d->m_rootDoc = doc;

  if ( doc )
  {
    doc->setSelectable( false );
    d->m_manager->addPart( doc, false );
    d->m_rootViews.append( doc->createView( d->m_splitter ) );
    d->m_rootViews.current()->setPartManager( d->m_manager );

    d->m_rootViews.current()->show();
    d->m_rootDoc->addShell( this );
    d->m_removeView->setEnabled(false);
    d->m_orientation->setEnabled(false);
    // Add entry to recent documents list (doing here makes it work with cmd line too etc.)
    if ( !doc->url().isEmpty() )
      m_recent->addURL( doc->url() );
  }

  bool enable = d->m_rootDoc != 0 ? true : false;
  d->m_paDocInfo->setEnabled( enable );
  d->m_paSave->setEnabled( enable );
  d->m_paSaveAs->setEnabled( enable );
  d->m_paPrint->setEnabled( enable );
  d->m_paPrintPreview->setEnabled( enable );

  updateCaption();

  d->m_manager->setActivePart( d->m_rootDoc, d->m_rootViews.current() );

  oldRootViews.setAutoDelete( true );
  oldRootViews.clear();

  if ( oldRootDoc && oldRootDoc->viewCount() == 0 )
  {
    //kdDebug(30003) << "No more views, deleting old doc " << oldRootDoc << endl;
    delete oldRootDoc;
  }
}

void KoMainWindow::setRootDocumentDirect( KoDocument *doc, const QList<KoView> & views )
{
  d->m_rootDoc = doc;
  d->m_rootViews = views;
  bool enable = d->m_rootDoc != 0 ? true : false;
  d->m_paDocInfo->setEnabled( enable );
  d->m_paSave->setEnabled( enable );
  d->m_paSaveAs->setEnabled( enable );
  d->m_paPrint->setEnabled( enable );
  d->m_paPrintPreview->setEnabled( enable );
}

KoDocument* KoMainWindow::createDoc() const
{
    QCString mimetype=KoDocument::readNativeFormatMimeType();
    KoDocumentEntry entry=KoDocumentEntry::queryByMimeType(mimetype);
    return entry.createDoc();
}

void KoMainWindow::updateCaption()
{
  //kdDebug(30003) << "KoMainWindow::updateCaption()" << endl;
  if ( !d->m_rootDoc )
    setCaption(QString::null);
  else
  {
      QString caption;
      // Get caption from document info (title(), in about page)
      if ( rootDocument()->documentInfo() )
      {
          KoDocumentInfoPage * page = rootDocument()->documentInfo()->page( QString::fromLatin1("about") );
          if (page)
              caption = static_cast<KoDocumentInfoAbout *>(page)->title();
      }
      if ( caption.isEmpty() )
          // Fall back to document URL
          caption = rootDocument()->url().prettyURL();

      setCaption( caption, rootDocument()->isModified() );
  }
}

KoDocument *KoMainWindow::rootDocument() const
{
    return d->m_rootDoc;
}

KoView *KoMainWindow::rootView() const
{
  if(d->m_rootViews.find(d->m_activeView)!=-1)
    return d->m_activeView;
  return d->m_rootViews.first();
}

KParts::PartManager *KoMainWindow::partManager()
{
  return d->m_manager;
}

bool KoMainWindow::openDocument( const KURL & url )
{
    return openDocumentInternal( 0L, url );
}

bool KoMainWindow::openDocument( KoDocument *newdoc, const KURL & url )
{
    return openDocumentInternal( 0L, url, newdoc );
}

bool KoMainWindow::openDocumentInternal( KoFilterManager * filterManager, const KURL & url, KoDocument *newdoc )
{
    //kdDebug(30003) << "KoMainWindow::openDocument " << url.url() << endl;

    if ( !newdoc )
        newdoc = createDoc();

    // Pass the filterManager to the document (who will own it from now on)
    if ( filterManager )
        newdoc->setFilterManager( filterManager );

    d->m_firstTime=true;
    connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    connect(newdoc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
    connect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotLoadCanceled( const QString & )));
    if(!newdoc || !newdoc->openURL(url))
    {
        delete newdoc;
        disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
        disconnect(newdoc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
        disconnect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotLoadCanceled( const QString & )));
        return false;
    }
    return true;
}

// Separate from openDocument to handle async loading (remote URLs)
void KoMainWindow::slotLoadCompleted()
{
    kdDebug(30003) << "KoMainWindow::slotLoadCompleted" << endl;
    KoDocument* doc = rootDocument();
    KoDocument* newdoc = (KoDocument *)(sender());
    if ( doc && doc->isEmpty() && !doc->isEmbedded() )
    {
        // Replace current empty document
        setRootDocument( newdoc );
    }
    else if ( doc && !doc->isEmpty() )
    {
        // Open in a new shell
        // (Note : could create the shell first and the doc next for this
        // particular case, that would give a better user feedback...)
        KoMainWindow *s = new KoMainWindow( newdoc->instance() );
        s->show();
        s->setRootDocument( newdoc );
    }
    else
    {
        // We had no document, set the new one
       setRootDocument( newdoc );
    }
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    disconnect(newdoc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
    disconnect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotLoadCanceled( const QString & )));
}

void KoMainWindow::slotLoadCanceled( const QString & errMsg )
{
    kdDebug() << "KoMainWindow::slotLoadCanceled" << endl;
    KMessageBox::error( this, errMsg );
    // ... can't delete the document, it's the one who emitted the signal...

    KoDocument* newdoc = (KoDocument *)(sender());
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    disconnect(newdoc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
    disconnect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotLoadCanceled( const QString & )));
}

bool KoMainWindow::saveDocument( bool saveas )
{
    KoDocument* pDoc = rootDocument();
    if(!pDoc)
        return true;
    connect(pDoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));

    QCString _native_format = pDoc->nativeFormatMimeType();
    bool wasModified = pDoc->isModified();

    bool ret = false;

    if ( pDoc->url().isEmpty() || saveas )
    {
        KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
        dialog->setCaption( i18n("Save document as") );
        dialog->setKeepLocation( true );
        KoFilterManager * filterManager = new KoFilterManager();
        filterManager->prepareDialog(dialog, KoFilterManager::Export,
                                     _native_format, nativeFormatPattern(),
                                     nativeFormatName(), true);
        KURL newURL;
        QCString outputFormat = _native_format;
        kdDebug() << "KoMainWindow::saveDocument outputFormat = " << outputFormat << endl;
        bool bOk;
        do {
            bOk=true;
            if(dialog->exec()==QDialog::Accepted) {
                newURL=dialog->selectedURL();
#if KDE_VERSION >= 220 // only in kdelibs > 2.1
                outputFormat=dialog->currentMimeFilter().latin1();
#endif
            }
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

// ###### To be made configurable !
            if ( QFileInfo( newURL.path() ).extension().isEmpty() ) {
#if KDE_VERSION < 220
                // assume that the pattern ends with .extension
                QString s( dialog->currentFilter() );
                QString extension = s.mid( s.find( "." ) );
                extension = extension.left( extension.find( " " ) );

                newURL.setPath( newURL.path() + extension );
#else
                // No more extensions in filters. We need to get it from the mimetype.
                KMimeType::Ptr mime = KMimeType::mimeType( outputFormat );
                QString extension = mime->property( "X-KDE-NativeExtension" ).toString();
                kdDebug() << "KoMainWindow::saveDocument outputFormat=" << outputFormat << " extension=" << extension << endl;
                newURL.setPath( newURL.path() + extension );
#endif
            }

            if ( KIO::NetAccess::exists( newURL ) ) { // this file exists => ask for confirmation
                bOk = KMessageBox::questionYesNo( this,
                                                  i18n("A document with this name already exists.\n"\
                                                       "Do you want to overwrite it ?"),
                                                  i18n("Warning") ) == KMessageBox::Yes;
            }
        } while ( !bOk );
        // Get the options from the dialog
        filterManager->cleanUp();
        // Pass the filterManager to the document (who will own it from now on)
        pDoc->setFilterManager( filterManager );
        delete dialog;
        if (bOk) {
            m_recent->addURL( newURL );
            if ( newURL.isLocalFile() )
                KRecentDocument::add(newURL.path(-1));
            else
                KRecentDocument::add(newURL.url(-1), true);

#if KDE_VERSION < 220
            KMimeType::Ptr t = KMimeType::findByURL( newURL, 0, TRUE );
            outputFormat = t->name().latin1();
#endif

            pDoc->setOutputMimeType( outputFormat );
            if ( outputFormat != _native_format )
            {
                // Warn the user
                KMimeType::Ptr mime = KMimeType::mimeType( outputFormat );
                QString comment = ( mime->name() == KMimeType::defaultMimeType() ) ? i18n( "Unknown file type %1" ).arg( outputFormat )
                                  : mime->comment();
                int res = KMessageBox::warningContinueCancel(
                    0, i18n( "<qt>You are about to save the document using the format %1"
                             "This might lose parts of the formatting of the document. Proceed?</qt>" )
                    .arg( QString( "<b>%1</b><p>" ).arg( comment ) ), // in case we want to remove the bold later
                    i18n( "File Export: Confirmation Required" ),
                    i18n( "Continue" ),
#if KDE_VERSION >= 220
                    "FileExportConfirmation",
#endif
                    true );
                if (res == KMessageBox::Cancel )
                    return false;
            }


            ret = pDoc->saveAs( newURL );

            pDoc->setTitleModified();
        }
        else
            ret = false;
    }
    else {
        ret = pDoc->save();
    }

    // When exporting to a non-native format, we don't reset modified.
    // This way the user will be reminded to save it again in the native format,
    // if he/she doesn't want to lose formatting.
    if ( wasModified && pDoc->outputMimeType() != _native_format )
        pDoc->setModified( true );

    disconnect(pDoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    return ret;
}

void KoMainWindow::closeEvent(QCloseEvent *e) {
    if(queryClose()) {
        if (settingsDirty() && rootDocument())
        {
            // Save window size into the config file of our instance
            instance()->config()->setGroup( "MainWindow" );
            //kdDebug(30003) << "KoMainWindow::closeEvent -> saveWindowSize" << endl;
            saveWindowSize( instance()->config() );
            // Save toolbar position into the config file of the app, under the doc's instance name
            //kdDebug() << "KoMainWindow::closeEvent -> saveMainWindowSettings rootdoc's instance=" << rootDocument()->instance()->instanceName() << endl;
            saveMainWindowSettings( KGlobal::config(), rootDocument()->instance()->instanceName() );
            KGlobal::config()->sync();
            resetAutoSaveSettings(); // Don't let KMainWindow override the good stuff we wrote down
        }
        setRootDocument(0L);
        KParts::MainWindow::closeEvent(e);
    }
}

void KoMainWindow::resizeEvent( QResizeEvent * e )
{
    setSettingsDirty();
    KParts::MainWindow::resizeEvent( e );
}

bool KoMainWindow::queryClose()
{
  if ( rootDocument() == 0 )
    return true;
  //kdDebug(30003) << "KoMainWindow::queryClose() viewcount=" << rootDocument()->viewCount()
  //               << " shellcount=" << rootDocument()->shellCount() << endl;
  if ( !d->m_forQuit && rootDocument()->shellCount() > 1 )
    // there are more open, and we are closing just one, so no problem for closing
    return true;

  // see DTOR for a descr. for the 2nd test
  if ( d->m_rootDoc->isModified() &&
       !d->m_rootDoc->isEmbedded())
  {
      int res = KMessageBox::warningYesNoCancel( 0L,
                    i18n( "The document has been modified.\nDo you want to save it ?" ));

      switch(res) {
          case KMessageBox::Yes : {
              bool isNative = ( d->m_rootDoc->outputMimeType() == d->m_rootDoc->nativeFormatMimeType() );
              if (! saveDocument( !isNative ) )
                  return false;
          }
          case KMessageBox::No :
              break;
          default : // case KMessageBox::Cancel :
              return false;
      }
  }
  return true;
}

void KoMainWindow::slotFileNew()
{
    KoDocument* doc = rootDocument();
    KoDocument *newdoc=createDoc();
    connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    if(!newdoc || !newdoc->initDoc())
    {
        delete newdoc;
        return;
    }
    if ( doc && doc->isEmpty() && !doc->isEmbedded() )
    {
        setRootDocument( newdoc );
        return;
    }
    else if ( doc && !doc->isEmpty() )
    {
        KoMainWindow *s = new KoMainWindow( newdoc->instance() );
        s->show();
        s->setRootDocument( newdoc );
        return;
    }
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    setRootDocument( newdoc );
    return;
}

void KoMainWindow::slotFileOpen()
{
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
    dialog->setCaption( i18n("Open document") );
    KoFilterManager * filterManager = new KoFilterManager;
    filterManager->prepareDialog(dialog, KoFilterManager::Import,
                                 KoDocument::readNativeFormatMimeType(),
                                 nativeFormatPattern(), nativeFormatName(), true);
    KURL url;
    if(dialog->exec()==QDialog::Accepted) {
        url=dialog->selectedURL();
        m_recent->addURL( url );
        if ( url.isLocalFile() )
            KRecentDocument::add(url.path(-1));
        else
            KRecentDocument::add(url.url(-1), true);
    }
    else
    {
        delete filterManager;
        return;
    }

    filterManager->cleanUp();
    delete dialog;
    if ( url.isEmpty() )
    {
        delete filterManager;
        return;
    }

    (void) openDocumentInternal( filterManager, url, 0L );
}

void KoMainWindow::slotFileOpenRecent( const KURL & url )
{
    (void) openDocument( url );
}

void KoMainWindow::slotFileSave()
{
    if ( saveDocument() )
        emit documentSaved();
}

void KoMainWindow::slotFileSaveAs()
{
    if ( saveDocument( true ) )
        emit documentSaved();
}

void KoMainWindow::slotDocumentInfo()
{
  if ( !rootDocument() )
    return;

  KoDocumentInfo *docInfo = rootDocument()->documentInfo();

  if ( !docInfo )
    return;

  KoDocumentInfoDlg *dlg = new KoDocumentInfoDlg( docInfo, this, "documentInfoDlg" );
  if ( dlg->exec() )
  {
    dlg->save();
    rootDocument()->setModified( true );
    rootDocument()->setTitleModified();
  }

  delete dlg;
}

void KoMainWindow::slotFileClose()
{
    if (queryClose())
    {
        setRootDocument( 0L ); // don't delete this shell when deleting the document
        delete d->m_rootDoc;
        d->m_rootDoc = 0L;
        slotFileNew();
    }
}

void KoMainWindow::slotFileQuit()
{
    if ( queryClose() )
    {
        setRootDocument( 0L );
        close();
    }
}

void KoMainWindow::slotFilePrint()
{
    if ( !rootView() )
    {
        kdDebug() << "KoMainWindow::slotFilePrint : No root view!" << endl;
        return;
    }

    KPrinter printer;

    // ### TODO: apply global koffice settings here

    rootView()->setupPrinter( printer );

    if ( printer.setup( this ) )
        rootView()->print( printer );
}

void KoMainWindow::slotFilePrintPreview()
{
    if ( !rootView() )
    {
        kdWarning() << "KoMainWindow::slotFilePrint : No root view!" << endl;
        return;
    }
    KPrinter printer;
    KTempFile tmpFile;
    // The temp file is deleted by KoPrintPreview

    // This line has to be before setupPrinter to let the apps decide what to
    // print and what not (if they want to :)
    printer.setFromTo( printer.minPage(), printer.maxPage() );
    rootView()->setupPrinter( printer );

    QString oldFileName = printer.outputFileName();
    printer.setOutputFileName( tmpFile.name() );
    int oldNumCopies = printer.numCopies();
    printer.setNumCopies( 1 );

    rootView()->print(printer);
    KoPrintPreview::preview(this, "KoPrintPreviewDialog", tmpFile.name());

    // Restore previous values
    printer.setOutputFileName( oldFileName );
    printer.setNumCopies( oldNumCopies );
}

void KoMainWindow::slotConfigureKeys()
{
    // We need to merge the shell, the doc, and the view's action collections
    KActionCollection coll( *actionCollection() );
    KoDocument *doc = rootDocument();
    if ( doc )
        coll += *doc->actionCollection();
    KoView *view = rootView();
    if ( view )
        coll += *view->actionCollection();
    KKeyDialog::configureKeys(&coll, xmlFile());
}

void KoMainWindow::slotConfigureToolbars()
{
    if (rootDocument())
        saveMainWindowSettings( KGlobal::config(), rootDocument()->instance()->instanceName() );
    KEditToolbar edit(factory());
    connect(&edit,SIGNAL(newToolbarConfig()),this,SLOT(slotNewToolbarConfig()));
    (void) edit.exec();
}

void KoMainWindow::slotNewToolbarConfig()
{
  if (rootDocument())
    applyMainWindowSettings( KGlobal::config(), rootDocument()->instance()->instanceName() );
  KXMLGUIFactory *factory = guiFactory();
  // This gets plugged in even for embedded views
  factory->plugActionList(d->m_activeView, "view_closeallviews",
			  d->m_veryHackyActionList);
  // This one only for root views
  if(d->m_rootViews.findRef(d->m_activeView)!=-1)
    factory->plugActionList(d->m_activeView, "view_split",
			    d->m_splitViewActionList );
  plugActionList( "toolbarlist", d->m_toolbarList );
}

void KoMainWindow::slotToolbarToggled( bool toggle )
{
  //kdDebug(30003) << "KoMainWindow::slotToolbarToggled " << sender()->name() << " toggle=" << true << endl;
  // The action (sender) and the toolbar have the same name
  KToolBar * bar = toolBar( sender()->name() );
  if (bar)
  {
    if (toggle)
      bar->show();
    else
      bar->hide();

    if (rootDocument())
        saveMainWindowSettings( KGlobal::config(), rootDocument()->instance()->instanceName() );
  }
  else
    kdWarning(30003) << "slotToolbarToggled : Toolbar " << sender()->name() << " not found!" << endl;
}

void KoMainWindow::showToolbar( const char * tbName, bool shown )
{
    QWidget * tb = toolBar( tbName );
    if ( !tb )
    {
        kdWarning(30003) << "KoMainWindow: toolbar " << tbName << " not found." << endl;
        return;
    }
    if ( shown )
        tb->show();
    else
        tb->hide();

    // Update the action appropriately
    QListIterator<KAction> it( d->m_toolbarList );
    for ( ; it.current() ; ++it )
        if ( !strcmp( it.current()->name(), tbName ) )
        {
            //kdDebug(30003) << "KoMainWindow::showToolbar setChecked " << shown << endl;
            static_cast<KToggleAction *>(it.current())->setChecked( shown );
            break;
        }
}

void KoMainWindow::slotSplitView() {
    d->m_splitted=true;
    d->m_rootViews.append(d->m_rootDoc->createView(d->m_splitter));
    d->m_rootViews.current()->show();
    d->m_rootViews.current()->setPartManager( d->m_manager );
    d->m_manager->setActivePart( d->m_rootDoc, d->m_rootViews.current() );
    d->m_removeView->setEnabled(true);
    d->m_orientation->setEnabled(true);
}

void KoMainWindow::slotCloseAllViews() {

    // Attention: Very touchy code... you know what you're doing? Goooood :)
    d->m_forQuit=true;
    if(queryClose()) {
        hide();
        d->m_rootDoc->removeShell(this);
        // In case the document is embedded we close all open "extra-shells"
        if(d->m_rootDoc && d->m_rootDoc->isEmbedded()) {
            QListIterator<KoMainWindow> it(d->m_rootDoc->shells());
            while (it.current()) {
                it.current()->hide();
                delete it.current(); // this updates the lists' current pointer and thus
                                     // the iterator (the shell dtor calls removeShell)
            }
        }
        // not embedded -> destroy the document and all shells/views ;)
        else
            delete d->m_rootDoc;
        d->m_rootDoc=0;
        close();  // close this window (and quit the app if necessary)
    }
    d->m_forQuit=false;
}

void KoMainWindow::slotRemoveView() {
    KoView *view;
    if(d->m_rootViews.findRef(d->m_activeView)!=-1)
        view=d->m_rootViews.current();
    else
        view=d->m_rootViews.first();
    view->hide();
    if ( !d->m_rootViews.removeRef(view) )
        kdWarning() << "view not found in d->m_rootViews!" << endl;

    if(d->m_rootViews.count()==1)
    {
        d->m_removeView->setEnabled(false);
        d->m_orientation->setEnabled(false);
    }
    // Prevent the view's destroyed() signal from triggering GUI rebuilding (too early)
    d->m_manager->setActivePart( 0, 0 );

    delete view;
    view=0L;

    d->m_rootViews.first()->setPartManager( d->m_manager );
    d->m_manager->setActivePart( d->m_rootDoc, d->m_rootViews.first() );

    if(d->m_rootViews.count()==1)
        d->m_splitted=false;
}

void KoMainWindow::slotSetOrientation() {
    d->m_splitter->setOrientation(static_cast<Qt::Orientation>
                                  (d->m_orientation->currentItem()));
}

void KoMainWindow::slotProgress(int value) {
    //kdDebug(30003) << "KoMainWindow::slotProgress " << value << endl;
    if(value==-1) {
        if ( d->m_progress )
        {
            statusBar()->removeWidget(d->m_progress);
            delete d->m_progress;
            d->m_progress=0L;
        }
        d->m_firstTime=true;
        return;
    }
    if(d->m_firstTime)
    {
        // The statusbar might not even be created yet.
        // So check for that first, and create it if necessary
        QObjectList *l = queryList( "QStatusBar" );
        if ( !l || !l->first() ) {
            statusBar()->show();
            QApplication::sendPostedEvents( this, QEvent::ChildInserted );
            setUpLayout();
        }
        delete l;

        if ( d->m_progress )
        {
            statusBar()->removeWidget(d->m_progress);
            delete d->m_progress;
            d->m_progress=0L;
        }
        statusBar()->setMaximumHeight(statusBar()->height());
        d->m_progress=new KProgress(statusBar());
        //d->m_progress->setMaximumHeight(statusBar()->height());
        statusBar()->addWidget( d->m_progress, 0, true );
        d->m_progress->show();
        d->m_firstTime=false;
    }
    d->m_progress->setValue(value);
    kapp->processEvents();
}

void KoMainWindow::slotActivePartChanged( KParts::Part *newPart )
{

  // This looks very much like KParts::MainWindow::createGUI, but we have
  // to reimplement it because it works with an active part, whereas we work
  // with an active view _and_ an active part, depending for what.
  // Both are KXMLGUIClients, but e.g. the plugin query needs a QObject.
  //kdDebug(30003) <<  "KoMainWindow::slotActivePartChanged( Part * newPart) newPart = " << newPart << endl;
  //kdDebug(30003) <<  "current active part is " << d->m_activePart << endl;

  if ( d->m_activePart && d->m_activePart == newPart && !d->m_splitted )
  {
    //kdDebug(30003) << "no need to change the GUI" << endl;
    return;
  }

  KXMLGUIFactory *factory = guiFactory();

  QList<KParts::Plugin> plugins;

  setUpdatesEnabled( false );

  if ( d->m_activeView )
  {
    KParts::GUIActivateEvent ev( false );
    QApplication::sendEvent( d->m_activePart, &ev );
    QApplication::sendEvent( d->m_activeView, &ev );

    plugins = KParts::Plugin::pluginObjects( d->m_activeView );
    KParts::Plugin *plugin = plugins.last();
    while ( plugin )
    {
      factory->removeClient( plugin );
      plugin = plugins.prev();
    }

    factory->removeClient( d->m_activeView );

    unplugActionList( "toolbarlist" );
    d->m_toolbarList.clear(); // deletes the actions
  }

  if ( !d->bMainWindowGUIBuilt )
    createShellGUI();

  if ( newPart && d->m_manager->activeWidget() && d->m_manager->activeWidget()->inherits( "KoView" ) )
  {
    d->m_activeView = (KoView *)d->m_manager->activeWidget();
    d->m_activePart = newPart;
    //kdDebug(30003) <<  "new active part is " << d->m_activePart << endl;

    factory->addClient( d->m_activeView );

    plugins = KParts::Plugin::pluginObjects( d->m_activeView );
    QListIterator<KParts::Plugin> pIt( plugins );
    for (; pIt.current(); ++pIt )
      factory->addClient( pIt.current() );

    // This gets plugged in even for embedded views
    factory->plugActionList(d->m_activeView, "view_closeallviews",
                            d->m_veryHackyActionList);
    // This one only for root views
    if(d->m_rootViews.findRef(d->m_activeView)!=-1)
        factory->plugActionList(d->m_activeView, "view_split", d->m_splitViewActionList );

    // Position and show toolbars according to user's preference
    setAutoSaveSettings( newPart->instance()->instanceName(), false );

    // Create and plug toolbar list for Settings menu
    //QListIterator<KToolBar> it = toolBarIterator();
    QList<QWidget> toolBarList = factory->containers( "ToolBar" );
    QListIterator<QWidget> it( toolBarList );
    for ( ; it.current() ; ++it )
    {
      if ( it.current()->inherits("KToolBar") )
      {
          KToolBar * tb = static_cast<KToolBar *>(it.current());
          KToggleAction * act = new KToggleAction( i18n("Show %1 Toolbar").arg( tb->text() ), 0,
                                               actionCollection(), tb->name() );
          connect( act, SIGNAL( toggled( bool ) ), this, SLOT( slotToolbarToggled( bool ) ) );
          act->setChecked ( !tb->isHidden() );
          d->m_toolbarList.append( act );
      }
      else
          kdWarning(30003) << "Toolbar list contains a " << it.current()->className() << " which is not a toolbar!" << endl;
    }
    plugActionList( "toolbarlist", d->m_toolbarList );

    // Send the GUIActivateEvent only now, since it might show/hide toolbars too
    // (and this has priority over applyMainWindowSettings)
    KParts::GUIActivateEvent ev( true );
    QApplication::sendEvent( d->m_activePart, &ev );
    QApplication::sendEvent( d->m_activeView, &ev );
  }
  else
  {
    d->m_activeView = 0L;
    d->m_activePart = 0L;
  }
  setUpdatesEnabled( true );
}

QLabel * KoMainWindow::statusBarLabel()
{
  if ( !d->statusBarLabel )
  {
    d->statusBarLabel = new QLabel( statusBar() );
    statusBar()->addWidget( d->statusBarLabel, 1, true );
  }
  return d->statusBarLabel;
}

QString KoMainWindow::nativeFormatName()
{
    QString serviceType;

    if ( rootDocument() )
        serviceType = QString::fromLatin1( rootDocument()->nativeFormatMimeType() );
    else
        serviceType = QString::fromLatin1( KoDocument::readNativeFormatMimeType() );

    if ( serviceType.isEmpty() )
        return QString::null;

    KMimeType::Ptr mimeType = KMimeType::mimeType( serviceType );

    if ( !mimeType )
        return QString::null;

    return mimeType->comment();
}

QString KoMainWindow::nativeFormatPattern()
{
    QString serviceType;

    if ( rootDocument() )
        serviceType = QString::fromLatin1( rootDocument()->nativeFormatMimeType() );
    else
        serviceType = QString::fromLatin1( KoDocument::readNativeFormatMimeType() );

    if ( serviceType.isEmpty() )
        return QString::null;

    KMimeType::Ptr mimeType = KMimeType::mimeType( serviceType );

    if ( !mimeType )
        return QString::null;

    return *mimeType->patterns().begin();
}

void KoMainWindow::setMaxRecentItems(uint _number)
{
        m_recent->setMaxItems( _number );
}

DCOPObject * KoMainWindow::dcopObject()
{
    if ( !d->m_dcopObject )
    {
        d->m_dcopObject = new KoMainWindowIface( this );
    }

    return d->m_dcopObject;
}

#include <koMainWindow.moc>
