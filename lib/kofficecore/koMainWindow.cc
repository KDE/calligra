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

#include <koMainWindow.h>
#include <koDocument.h>
#include <koView.h>
#include <koFilterManager.h>
#include <koDocumentInfo.h>
#include <koDocumentInfoDlg.h>
#include <koQueryTrader.h>
#include "KoMainWindowIface.h"

#include <kprinter.h>
#include <qobjectlist.h>

#include <kstdaction.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kfilefiltercombo.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <kprogress.h>
#include <kdebug.h>
#include <ktempfile.h>
#include <krecentdocument.h>

#include <kparts/partmanager.h>
#include <kparts/plugin.h>
#include <kparts/event.h>

#include <unistd.h>
#include <stdlib.h>
#include <klocale.h>
#include <kstatusbar.h>

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

// Extension to KFileDialog in order to add "save as koffice-1.1" and "save as dir"
// Used only when saving!
class KoFileDialog : public KFileDialog
{
public:
    KoFileDialog(const QString& startDir, const QString& filter,
                 QWidget *parent, const char *name,
                 bool modal)
        : KFileDialog( startDir, filter, parent, name, modal ) { }

    void setSpecialMimeFilter( QStringList& mimeFilter, const QString& nativeFormat )
    {
        Q_ASSERT( !mimeFilter.isEmpty() );
        Q_ASSERT( mimeFilter[0] == nativeFormat );
        // Insert two entries with native mimetypes, for the special entries.
        QStringList::Iterator mimeFilterIt = mimeFilter.at( 1 );
        mimeFilter.insert( mimeFilterIt /* before 1 -> after 0 */, 2, nativeFormat );
        // Fill in filter combo
        setMimeFilter( mimeFilter, nativeFormat );
        // To get a different description in the combo, we need to change its entries afterwards
        KMimeType::Ptr type = KMimeType::mimeType( nativeFormat );
        filterWidget->changeItem( i18n("%1 (KOffice-1.1 format)").arg( type->comment() ), KoDocument::SaveAsKOffice1dot1 );
        filterWidget->changeItem( i18n("%1 (Uncompressed XML files)").arg( type->comment() ), KoDocument::SaveAsDirectoryStore );
        // KFileFilterCombo selected the _last_ "native mimetype" entry, select 0 again.
        filterWidget->setCurrentItem( 0 );
    }
    int specialEntrySelected()
    {
        int i = filterWidget->currentItem();
        // This enum is the position of the special items in the filter combo.
        if ( i == KoDocument::SaveAsKOffice1dot1 || i == KoDocument::SaveAsDirectoryStore )
            return i;
        return 0;
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
    m_sendfile = 0;
  }
  ~KoMainWindowPrivate()
  {
    delete m_dcopObject;
  }

  KoDocument *m_rootDoc;
  QPtrList<KoView> m_rootViews;
  KParts::PartManager *m_manager;

  KParts::Part *m_activePart;
  KoView *m_activeView;

  QLabel * statusBarLabel;
  KProgress *m_progress;

  QPtrList<KAction> m_splitViewActionList;
  // This additional list is needed, because we don't plug
  // the first list, when an embedded view gets activated (Werner)
  QPtrList<KAction> m_veryHackyActionList;
  QSplitter *m_splitter;
  KSelectAction *m_orientation;
  KAction *m_removeView;
  KoMainWindowIface *m_dcopObject;

  QPtrList <KAction> m_toolbarList;

  bool bMainWindowGUIBuilt;
  bool m_splitted;
  bool m_forQuit;
  bool m_firstTime;

  KAction *m_paDocInfo;
  KAction *m_paSave;
  KAction *m_paSaveAs;
  KAction *m_paPrint;
  KAction *m_paPrintPreview;
  KAction *m_sendfile;
};

KoMainWindow::KoMainWindow( KInstance *instance, const char* name )
    : KParts::MainWindow( name )
{
    Q_ASSERT(instance);
    d = new KoMainWindowPrivate;

    d->m_manager = new KoPartManager( this );
    d->m_manager->setSelectionPolicy( KParts::PartManager::TriState );
    d->m_manager->setAllowNestedParts( true );
    d->m_manager->setIgnoreScrollBars( true );
#if KDE_VERSION > 305
    d->m_manager->setActivationButtonMask( Qt::LeftButton | Qt::MidButton );
#endif

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

    d->m_sendfile = new KAction( i18n( "Send File..."), "mail_send", 0,
                    this, SLOT( slotEmailFile() ),
                    actionCollection(), "file_send_file");
    d->m_paDocInfo = new KAction( i18n( "&Document Information..." ), "documentinfo", 0,
                        this, SLOT( slotDocumentInfo() ),
                        actionCollection(), "file_documentinfo" );

    KStdAction::keyBindings( this, SLOT( slotConfigureKeys() ), actionCollection() );
    KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection() );

    d->m_paDocInfo->setEnabled( false );
    d->m_paSaveAs->setEnabled( false );
    d->m_paSave->setEnabled( false );
    d->m_paPrint->setEnabled( false );
    d->m_paPrintPreview->setEnabled( false );
    d->m_sendfile->setEnabled( false);

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
            resize( 1000, 800 );
        if (QApplication::desktop()->width() > 850) // big desktop ?
            resize( 800, 600 );
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

    delete d->m_manager;
    delete d;
}

void KoMainWindow::setRootDocument( KoDocument *doc )
{
  if ( d->m_rootDoc == doc )
    return;

  //kdDebug(30003) <<  "KoMainWindow::setRootDocument this = " << this << " doc = " << doc << endl;
  QPtrList<KoView> oldRootViews = d->m_rootViews;
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
  }

  bool enable = d->m_rootDoc != 0 ? true : false;
  d->m_paDocInfo->setEnabled( enable );
  d->m_paSave->setEnabled( enable );
  d->m_paSaveAs->setEnabled( enable );
  d->m_paPrint->setEnabled( enable );
  d->m_paPrintPreview->setEnabled( enable );
  d->m_sendfile->setEnabled( enable);
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

void KoMainWindow::setRootDocumentDirect( KoDocument *doc, const QPtrList<KoView> & views )
{
  d->m_rootDoc = doc;
  d->m_rootViews = views;
  bool enable = d->m_rootDoc != 0 ? true : false;
  d->m_paDocInfo->setEnabled( enable );
  d->m_paSave->setEnabled( enable );
  d->m_paSaveAs->setEnabled( enable );
  d->m_paPrint->setEnabled( enable );
  d->m_paPrintPreview->setEnabled( enable );
  d->m_sendfile->setEnabled( enable);
}

void KoMainWindow::addRecentURL( const KURL& url )
{
    kdDebug(30003) << "KoMainWindow::addRecentURL url=" << url.prettyURL() << endl;
    // Add entry to recent documents list
    // (call coming from KoDocument because it must work with cmd line, template dlg, file/open, etc.)
    if ( !url.isEmpty() )
    {
        bool ok = true;
        if ( url.isLocalFile() )
        {
            QString path = url.path( -1 );
            QStringList tmpDirs = KGlobal::dirs()->resourceDirs( "tmp" );
            for ( QStringList::Iterator it = tmpDirs.begin() ; ok && it != tmpDirs.end() ; ++it )
                if ( path.contains( *it ) )
                    ok = false; // it's in the tmp resource
            if ( ok )
                KRecentDocument::add(path);
        }
        else
            KRecentDocument::add(url.url(-1), true);

        if ( ok )
            m_recent->addURL( url );
        saveRecentFiles();
    }
}

void KoMainWindow::saveRecentFiles()
{
    // Save list of recent files
    KConfig * config = instance() ? instance()->config() : KGlobal::config();
    kdDebug(30003) << this << " Saving recent files list into config. instance()=" << instance() << endl;
    m_recent->saveEntries( config );
    config->sync();
    if (KMainWindow::memberList)
    {
        // Tell all windows to reload their list, after saving
        // Doesn't work multi-process, but it's a start
        KMainWindow *window = KMainWindow::memberList->first();
        for (; window; window = KMainWindow::memberList->next())
            static_cast<KoMainWindow *>(window)->reloadRecentFileList();
    }
}

void KoMainWindow::reloadRecentFileList()
{
    KConfig * config = instance() ? instance()->config() : KGlobal::config();
    m_recent->loadEntries( config );
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
    return openDocumentInternal( url );
}

bool KoMainWindow::openDocument( KoDocument *newdoc, const KURL & url )
{
    return openDocumentInternal( url, newdoc );
}

bool KoMainWindow::openDocumentInternal( const KURL & url, KoDocument *newdoc )
{
    //kdDebug(30003) << "KoMainWindow::openDocument " << url.url() << endl;

    if ( !newdoc )
        newdoc = createDoc();

    d->m_firstTime=true;
    connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    connect(newdoc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
    connect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotLoadCanceled( const QString & )));
    if(!newdoc || !newdoc->openURL(url))
    {
        delete newdoc;
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
    kdDebug(30003) << "KoMainWindow::slotLoadCanceled" << endl;
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
        KoFileDialog *dialog=new KoFileDialog(QString::null, QString::null, 0L, "file dialog", true);
        dialog->setCaption( i18n("Save Document As") );
        dialog->setKeepLocation( true );
        dialog->setOperationMode( KFileDialog::Saving );
        QStringList mimeFilter = KoFilterManager::mimeFilter( _native_format, KoFilterManager::Export );
        dialog->setSpecialMimeFilter( mimeFilter, _native_format );
        KURL newURL;
        QCString outputFormat = _native_format;
        int specialOutputFlag = 0;
        kdDebug(30003) << "KoMainWindow::saveDocument outputFormat = " << outputFormat << endl;
        bool bOk;
        do {
            bOk=true;
            if(dialog->exec()==QDialog::Accepted) {
                newURL=dialog->selectedURL();
                outputFormat=dialog->currentMimeFilter().latin1();
                specialOutputFlag = dialog->specialEntrySelected();
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
                // No more extensions in filters. We need to get it from the mimetype.
                KMimeType::Ptr mime = KMimeType::mimeType( outputFormat );
                QString extension = mime->property( "X-KDE-NativeExtension" ).toString();
                kdDebug(30003) << "KoMainWindow::saveDocument outputFormat=" << outputFormat << " extension=" << extension << endl;
                newURL.setPath( newURL.path() + extension );
            }

            if ( KIO::NetAccess::exists( newURL ) ) { // this file exists => ask for confirmation
                bOk = KMessageBox::questionYesNo( this,
                                                  i18n("A document with this name already exists.\n"\
                                                       "Do you want to overwrite it?"),
                                                  i18n("Warning") ) == KMessageBox::Yes;
            }
        } while ( !bOk );

        delete dialog;
        if (bOk) {
            addRecentURL( newURL );

            pDoc->setOutputMimeType( outputFormat, specialOutputFlag );
            if ( outputFormat != _native_format )
            {
                Q_ASSERT( specialOutputFlag == 0 ); // this is for the native mimetype only!
                // Warn the user
                KMimeType::Ptr mime = KMimeType::mimeType( outputFormat );
                QString comment = ( mime->name() == KMimeType::defaultMimeType() ) ? i18n( "Unknown file type %1" ).arg( outputFormat )
                                  : mime->comment();
                int res = KMessageBox::warningContinueCancel(
                    0, i18n( "<qt>You are about to save the document using the format %1.<p>"
                             "This might lose parts of the formatting of the document. Proceed?</qt>" )
                    .arg( QString( "<b>%1</b>" ).arg( comment ) ), // in case we want to remove the bold later
                    i18n( "File Export: Confirmation Required" ),
                    i18n( "Continue" ),
                    "FileExportConfirmation",
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
            //kdDebug(30003) << "KoMainWindow::closeEvent -> saveMainWindowSettings rootdoc's instance=" << rootDocument()->instance()->instanceName() << endl;
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
      QString name;
      if ( rootDocument()->documentInfo() )
      {
         name = rootDocument()->documentInfo()->title();
      }
      if ( name.isEmpty() )
          name = rootDocument()->url().fileName();

      if ( name.isEmpty() )
          name = i18n( "Untitled" );

      int res = KMessageBox::warningYesNoCancel( 0L,
                    i18n( "<p>The document <b>'%1'</b> has been modified.</p><p>Do you want to save it?</p>" ).arg(name));

      switch(res) {
          case KMessageBox::Yes : {
              bool isNative = ( d->m_rootDoc->outputMimeType() == d->m_rootDoc->nativeFormatMimeType() );
              if (! saveDocument( !isNative ) )
                  return false;
          }
          case KMessageBox::No :
              rootDocument()->removeAutoSaveFiles();
              rootDocument()->setModified( false ); // Now when queryClose() is called by closeEvent it won't do anything.
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
    dialog->setCaption( i18n("Open Document") );
    dialog->setMimeFilter( KoFilterManager::mimeFilter( KoDocument::readNativeFormatMimeType(),
                                                        KoFilterManager::Import ) );
    if(dialog->exec()!=QDialog::Accepted) {
        delete dialog;
        return;
    }
    KURL url( dialog->selectedURL() );
    delete dialog;

    if ( url.isEmpty() )
        return;

    (void) openDocumentInternal( url, 0L );
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
    if (queryClose())
        close(); // queryClose will also be called in this method but won't do anything because isModified==false.
}

void KoMainWindow::print(bool quick) {
    if ( !rootView() )
    {
        kdDebug(30003) << "KoMainWindow::slotFilePrint : No root view!" << endl;
        return;
    }

    KPrinter printer( true /*, QPrinter::HighResolution*/ );
    QString title = rootView()->koDocument()->documentInfo()->title();
    QString fileName = rootView()->koDocument()->url().fileName();

    // strip off the native extension (I don't want foobar.kwd.ps when printing into a file)
    KMimeType::Ptr mime = KMimeType::mimeType( rootView()->koDocument()->outputMimeType() );
    if ( mime ) {
        QString extension = mime->property( "X-KDE-NativeExtension" ).toString();

        if ( fileName.endsWith( extension ) )
            fileName.truncate( fileName.length() - extension.length() );
    }

    if ( title.isNull() )
        title = fileName;
    printer.setDocName( title );
    printer.setDocFileName( fileName );

    // ### TODO: apply global koffice settings here

    rootView()->setupPrinter( printer );

    if ( quick ||  printer.setup( this ) )
        rootView()->print( printer );
}


void KoMainWindow::slotFilePrint()
{
	print(false);
}

void KoMainWindow::slotFilePrintPreview()
{
    if ( !rootView() )
    {
        kdWarning() << "KoMainWindow::slotFilePrint : No root view!" << endl;
        return;
    }
    KPrinter printer( false );
    KTempFile tmpFile;
    // The temp file is deleted by KoPrintPreview

    // This line has to be before setupPrinter to let the apps decide what to
    // print and what not (if they want to :)
    printer.setFromTo( printer.minPage(), printer.maxPage() );
    rootView()->setupPrinter( printer );

    QString oldFileName = printer.outputFileName();
    printer.setOutputFileName( tmpFile.name() );
    printer.setPreviewOnly( true );
    int oldNumCopies = printer.numCopies();
    printer.setNumCopies( 1 );
    // Disable kdeprint's own preview, we'd get two. This shows that KPrinter needs
    // a "don't use the previous settings" mode. The current way is really too much of a hack.
    QString oldKDEPreview = printer.option( "kde-preview" );
    printer.setOption( "kde-preview", "0" );

    rootView()->print(printer);
    //KoPrintPreview::preview(this, "KoPrintPreviewDialog", tmpFile.name());

    // Restore previous values
    printer.setOutputFileName( oldFileName );
    printer.setNumCopies( oldNumCopies );
    printer.setOption( "kde-preview", oldKDEPreview );
}

void KoMainWindow::slotConfigureKeys()
{
    KoView *view = rootView();
    // We _need_ a view. We use the view's xmlFile() (e.g. kword.rc)
    Q_ASSERT( view );
    if ( !view )
        return;

    KKeyDialog dlg;
    dlg.insert( actionCollection() );
    dlg.insert( view->actionCollection() );
    if ( rootDocument() )
        dlg.insert( rootDocument()->actionCollection() );
    dlg.configure();
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

bool KoMainWindow::toolbarIsVisible(const char *tbName)
{
    QWidget *tb = toolBar( tbName);
    return !tb->isHidden();
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
    QPtrListIterator<KAction> it( d->m_toolbarList );
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
            QPtrListIterator<KoMainWindow> it(d->m_rootDoc->shells());
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
    d->m_progress->setProgress(value);
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

  setUpdatesEnabled( false );

  if ( d->m_activeView )
  {
    KParts::GUIActivateEvent ev( false );
    QApplication::sendEvent( d->m_activePart, &ev );
    QApplication::sendEvent( d->m_activeView, &ev );

    // As of KDE-3.1, the plugins are child XMLGUI objects of the part
#if KDE_VERSION < 305
    QPtrList<KParts::Plugin> plugins = KParts::Plugin::pluginObjects( d->m_activeView );
    KParts::Plugin *plugin = plugins.last();
    while ( plugin )
    {
      factory->removeClient( plugin );
      plugin = plugins.prev();
    }
#endif

    factory->removeClient( d->m_activeView );

    unplugActionList( "toolbarlist" );
    d->m_toolbarList.clear(); // deletes the actions
  }

  if ( !d->bMainWindowGUIBuilt )
  {
#if KDE_VERSION >= 305
    // Load mainwindow plugins
    KParts::Plugin::loadPlugins( this, this, instance(), true );
#endif
    createShellGUI();
  }

  if ( newPart && d->m_manager->activeWidget() && d->m_manager->activeWidget()->inherits( "KoView" ) )
  {
    d->m_activeView = (KoView *)d->m_manager->activeWidget();
    d->m_activePart = newPart;
    //kdDebug(30003) <<  "new active part is " << d->m_activePart << endl;

    factory->addClient( d->m_activeView );

    // As of KDE-3.1, the plugins are child XMLGUI objects of the part
#if KDE_VERSION < 305
    QPtrList<KParts::Plugin> plugins = KParts::Plugin::pluginObjects( d->m_activeView );
    QPtrListIterator<KParts::Plugin> pIt( plugins );
    for (; pIt.current(); ++pIt )
      factory->addClient( pIt.current() );
#endif

    // This gets plugged in even for embedded views
    factory->plugActionList(d->m_activeView, "view_closeallviews",
                            d->m_veryHackyActionList);
    // This one only for root views
    if(d->m_rootViews.findRef(d->m_activeView)!=-1)
        factory->plugActionList(d->m_activeView, "view_split", d->m_splitViewActionList );

    // Position and show toolbars according to user's preference
    setAutoSaveSettings( newPart->instance()->instanceName(), false );

    // Create and plug toolbar list for Settings menu
    //QPtrListIterator<KToolBar> it = toolBarIterator();
    QPtrList<QWidget> toolBarList = factory->containers( "ToolBar" );
    QPtrListIterator<QWidget> it( toolBarList );
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

void KoMainWindow::slotEmailFile()
{
   saveDocument();
   // Subject = Document file name
   // Attachment = The current file
   // Message Body = The current document in HTML export? <-- This may be an option.
   QString fileURL = d->m_rootDoc->url().url();
   QString theSubject = d->m_rootDoc->url().fileName(false);
   kdDebug(30003) << "(" << fileURL <<")" << endl;
   QStringList urls;
   urls.append( fileURL );
   if (!fileURL.isEmpty())
       kapp->invokeMailer(QString::null, QString::null, QString::null, theSubject,
                          QString::null, //body
                          QString::null,
                          urls); // attachments

   /*kapp->invokeMailer("mailto:?subject=" + theSubject +
     "&attach=" + fileURL);*/
   else
       KMessageBox::detailedSorry (0, i18n("ERROR: File not found."),
                                   i18n("To send a file you must first have saved the file to the filesystem."),
                                   i18n("Error: File Not Found!"));
}

#include <koMainWindow.moc>
