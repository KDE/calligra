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

#include <qkeycode.h>
#include <qfile.h>
#include <qwhatsthis.h>
#include <qmime.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qsplitter.h>
#include <qprinter.h>

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
#include <kprogress.h>
#include <kdebug.h>
#include <kmimetype.h>

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
  }
  ~KoMainWindowPrivate()
  {
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

  QList <KAction> m_toolbarList;

  bool bMainWindowGUIBuilt;
  bool m_splitted;
  bool m_forQuit;
  bool m_firstTime;

  KAction *m_paDocInfo;
  KAction *m_paSave;
  KAction *m_paSaveAs;
  KAction *m_paPrint;
};

KoMainWindow::KoMainWindow( KInstance *instance, const char* name )
    : KParts::MainWindow( name )
{
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

    setXMLFile( locate( "data", "koffice/koffice_shell.rc" ) );
    setLocalXMLFile( locateLocal( "data", "koffice/koffice_shell.rc" ) );

    KStdAction::openNew( this, SLOT( slotFileNew() ), actionCollection(), "file_new" );
    KStdAction::open( this, SLOT( slotFileOpen() ), actionCollection(), "file_open" );
    m_recent = KStdAction::openRecent( this, SLOT(slotFileOpenRecent(const KURL&)), actionCollection() );
    d->m_paSave = KStdAction::save( this, SLOT( slotFileSave() ), actionCollection(), "file_save" );
    d->m_paSaveAs = KStdAction::saveAs( this, SLOT( slotFileSaveAs() ), actionCollection(), "file_save_as" );
    d->m_paPrint = KStdAction::print( this, SLOT( slotFilePrint() ), actionCollection(), "file_print" );
    KStdAction::close( this, SLOT( slotFileClose() ), actionCollection(), "file_close" );
    KStdAction::quit( this, SLOT( slotFileQuit() ), actionCollection(), "file_quit" );

    d->m_paDocInfo = new KAction( i18n( "&Document Information..." ), "documentinfo", 0,
                        this, SLOT( slotDocumentInfo() ),
                        actionCollection(), "file_documentinfo" );

    KStdAction::keyBindings( this, SLOT( slotConfigureKeys() ), actionCollection(), "configurekeys" );
    KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection(), "configuretoolbars" );

    KHelpMenu * m_helpMenu = new KHelpMenu( this );
    KStdAction::helpContents( m_helpMenu, SLOT( appHelpActivated() ), actionCollection(), "contents" );
    KStdAction::whatsThis( m_helpMenu, SLOT( contextHelpActivated() ), actionCollection(), "whats_this" );
    KStdAction::aboutApp( this, SLOT( slotHelpAbout() ), actionCollection(), "about_app" );
    KStdAction::aboutKDE( m_helpMenu, SLOT( aboutKDE() ), actionCollection(), "about_kde" );
    KStdAction::reportBug( m_helpMenu, SLOT( reportBug() ), actionCollection(), "report_bug" );

    d->m_paDocInfo->setEnabled( false );
    d->m_paSaveAs->setEnabled( false );
    d->m_paSave->setEnabled( false );
    d->m_paPrint->setEnabled( false );

    d->m_splitter=new QSplitter(Qt::Vertical, this, "funky-splitter");
    setCentralWidget( d->m_splitter );

    // set up the action "list" for "Close all Views" (hacky :) (Werner)
    d->m_veryHackyActionList.append(new KAction(i18n("&Close All Views"), 0, this,
        SLOT(slotCloseAllViews()), actionCollection(), "view_closeallviews"));
    // set up the action list for the splitter stuff
    d->m_splitViewActionList.append(new KAction(i18n("&Split View"), 0,
        this, SLOT(slotSplitView()),
        actionCollection(), "view_split"));
    d->m_removeView=new KAction(i18n("&Remove View"), 0,
        this, SLOT(slotRemoveView()),
        actionCollection(), "view_rm_splitter");
    d->m_splitViewActionList.append(d->m_removeView);
    d->m_removeView->setEnabled(false);
    d->m_orientation=new KSelectAction(i18n("Splitter &Orientation"), 0,
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
    config->sync();

    createShellGUI();
    d->bMainWindowGUIBuilt = true;

    // Status bar
    d->statusBarLabel = new QLabel( statusBar() );
    statusBar()->addWidget( d->statusBarLabel, 1, true );

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
        kdDebug(30003) << "Destructor. No more views, deleting old doc " << d->m_rootDoc << endl;
        delete d->m_rootDoc;
    }

    // Save list of recent files
    KConfig * config = instance() ? instance()->config() : KGlobal::config();
    kdDebug() << "Saving recent files list into config. instance()=" << instance() << endl;
    m_recent->saveEntries( config );
    config->sync();

    delete d->m_manager;
    delete d->m_splitter;
    d->m_splitter=0L;
    delete d;
}

void KoMainWindow::setRootDocument( KoDocument *doc )
{
  if ( d->m_rootDoc == doc )
    return;

  kdDebug(30003) <<  "KoMainWindow::setRootDocument this = " << this << " doc = " << doc << endl;
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
  }

  bool enable = d->m_rootDoc != 0 ? true : false;
  d->m_paDocInfo->setEnabled( enable );
  d->m_paSave->setEnabled( enable );
  d->m_paSaveAs->setEnabled( enable );
  d->m_paPrint->setEnabled( enable );

  updateCaption();

  d->m_manager->setActivePart( d->m_rootDoc, d->m_rootViews.current() );

  oldRootViews.setAutoDelete( true );
  oldRootViews.clear();

  if ( oldRootDoc && oldRootDoc->viewCount() == 0 )
  {
    kdDebug(30003) << "No more views, deleting old doc " << oldRootDoc << endl;
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
}

KoDocument* KoMainWindow::createDoc() const
{
    QCString mimetype=KoDocument::readNativeFormatMimeType();
    KoDocumentEntry entry=KoDocumentEntry::queryByMimeType(mimetype);
    return entry.createDoc();
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
      if ( rootDocument()->documentInfo() )
      {
          KoDocumentInfoPage * page = rootDocument()->documentInfo()->page( QString::fromLatin1("about"));
          if (page)
              caption = static_cast<KoDocumentInfoAbout *>(page)->title();
      }
      if ( caption.isEmpty() )
          // Fall back to document URL
          caption = rootDocument()->url().prettyURL();

      // KTMW hides some of the functionality of kapp->makeStdCaption !
      QWidget::setCaption( kapp->makeStdCaption( caption, true, rootDocument()->isModified() ) );
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
    m_recent->addURL( url );
    KoDocument* doc = rootDocument();
    KoDocument *newdoc=createDoc();
    d->m_firstTime=true;
    connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    if ( !newdoc || !newdoc->openURL( url ) )
    {
        newdoc->delayedDestruction();
        return false;
    }
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));

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
//       KoMainWindow *s = newdoc->createShell();
        KoMainWindow *s = new KoMainWindow( newdoc->instance() );
       s->show();
       s->setRootDocument( newdoc );
    }
    else
    {
        // We had no document, set the new one
       setRootDocument( newdoc );
    }
    return true;
}

bool KoMainWindow::saveDocument( bool saveas )
{
    KoDocument* pDoc = rootDocument();
    if(!pDoc)
        return true;
    connect(pDoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));

    QCString _native_format = pDoc->nativeFormatMimeType();

    if ( pDoc->url().isEmpty() || saveas )
    {
        KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
        dialog->setCaption( i18n("Save document as") );
        KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Export,
                                               _native_format, nativeFormatPattern(),
                                               nativeFormatName(), true);
        KURL newURL;

        bool bOk;
        do {
            bOk=true;
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
                extension = extension.left( extension.find( " " ) );

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
        if (bOk) {
            m_recent->addURL( newURL );
            bool ret = pDoc->saveAs( newURL );
            pDoc->setTitleModified();
            disconnect(pDoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
            return ret;
        }
        disconnect(pDoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
        return false;
    }
    else {
        bool ret=pDoc->save();
        disconnect(pDoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
        return ret;
    }
}

void KoMainWindow::closeEvent(QCloseEvent *e) {
    if(queryClose()) {
        setRootDocument(0L);
        KParts::MainWindow::closeEvent(e);
    }
}

bool KoMainWindow::queryClose()
{
  if ( rootDocument() == 0 )
    return true;
  kdDebug(30003) << "KoMainWindow::queryClose() viewcount=" << rootDocument()->viewCount()
                 << " shellcount=" << rootDocument()->shellCount() << endl;
  if ( !d->m_forQuit && rootDocument()->shellCount() > 1 )
    // there are more open, and we are closing just one, so no problem for closing
    return true;

  // see DTOR for a descr. for the 2nd test
  if ( rootDocument()->isModified() &&
       !rootDocument()->isEmbedded())
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
    if ( !newdoc || !newdoc->initDoc() )
    {
        newdoc->delayedDestruction();
        return;
    }
    if ( doc && doc->isEmpty() && !doc->isEmbedded() )
    {
        setRootDocument( newdoc );
        return;
    }
    else if ( doc && !doc->isEmpty() )
    {
//        KoMainWindow *s = newdoc->createShell();
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
    KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Import,
                                           KoDocument::readNativeFormatMimeType(),
                                           nativeFormatPattern(), nativeFormatName(), true);
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
    saveDocument( true );
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

    QPrinter printer;

    // ### TODO: apply global koffice settings here

    rootView()->setupPrinter( printer );

    if ( printer.setup( this ) )
        rootView()->print( printer );
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

void KoMainWindow::slotToolbarToggled( bool toggle )
{
  // The action (sender) and the toolbar have the same name
  KToolBar * bar = toolBar( sender()->name() );
  if (bar)
  {
    if (toggle)
      bar->show();
    else
      bar->hide();
  }
  else
    kdWarning(30003) << "slotToolbarToggled : Toolbar " << sender()->name() << " not found!" << endl;
}

void KoMainWindow::slotHelpAbout()
{
    KAboutDialog *dia = new KAboutDialog( KAboutDialog::AbtProduct | KAboutDialog::AbtTitle | KAboutDialog::AbtImageOnly,
                                          kapp->caption(),
                                          KDialogBase::Ok, KDialogBase::Ok, this, 0, true );
    dia->setTitle( kapp->caption() );
    dia->setProduct( "", "1.0", "the KOffice Team", "1998-2000" );
    dia->setImage( locate( "data", "koffice/pics/koffice-logo.png" ) );
    dia->exec();
    delete dia;
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

  d->m_forQuit=true;
  if(queryClose()) {
    KoDocument *doc=d->m_rootDoc;
    // get the tompost parent document (save because of inherits())
    while(doc && doc->isEmbedded())
      doc = static_cast<KoDocument*>(doc->parent());
    doc->delayedDestruction();
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
    d->m_rootViews.removeRef(view);

    if(d->m_rootViews.count()==1)
    {
        d->m_removeView->setEnabled(false);
        d->m_orientation->setEnabled(false);
    }
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

    if(value==-1) {
        statusBar()->removeWidget(d->m_progress);
        delete d->m_progress;
        d->m_progress=0L;
        d->m_firstTime=true;
        return;
    }
    if(d->m_firstTime) {
        statusBar()->removeWidget(d->m_progress);
        delete d->m_progress;
        d->m_progress=0L;
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
  kdDebug(30003) <<  "KoMainWindow::slotActivePartChanged( Part * newPart) newPart = " <<
    newPart << endl;
  kdDebug(30003) <<  "current active part is " << d->m_activePart << endl;

  if ( d->m_activePart && d->m_activePart == newPart && !d->m_splitted )
  {
    kdDebug(30003) << "no need to change the GUI" << endl;
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
    kdDebug(30003) <<  "new active part is " << d->m_activePart << endl;

    factory->addClient( d->m_activeView );

    KParts::GUIActivateEvent ev( true );
    QApplication::sendEvent( d->m_activePart, &ev );
    QApplication::sendEvent( d->m_activeView, &ev );

    plugins = KParts::Plugin::pluginObjects( d->m_activeView );
    QListIterator<KParts::Plugin> pIt( plugins );
    for (; pIt.current(); ++pIt )
      factory->addClient( pIt.current() );

    // This gets plugged in even for embedded views!
    factory->plugActionList(d->m_activeView, "view_closeallviews",
                            d->m_veryHackyActionList);
    // This one only for root views
    if(d->m_rootViews.findRef(d->m_activeView)!=-1)
        factory->plugActionList(d->m_activeView, "view_split", d->m_splitViewActionList );

    // Create and plug toolbar list for Settings menu
    QListIterator<KToolBar> it = toolBarIterator();
    for ( ; it.current() ; ++it )
    {
      KToggleAction * act = new KToggleAction( i18n("Show %1 Toolbar").arg( (*it)->text() ), 0,
                                               actionCollection(), (*it)->name() );
      connect( act, SIGNAL( toggled( bool ) ), this, SLOT( slotToolbarToggled( bool ) ) );
      act->setChecked ( true );
      d->m_toolbarList.append( act );
    }
    plugActionList( "toolbarlist", d->m_toolbarList );

  }
  else
  {
    d->m_activeView = 0L;
    d->m_activePart = 0L;
  }
  setUpdatesEnabled( true );
}

QLabel * KoMainWindow::statusBarLabel() const
{
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

#include <koMainWindow.moc>
