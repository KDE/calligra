/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2005, 2006 Sven Lüppken <sven@kde.org>
   Copyright (C) 2008 Dag Andersen <kplato@kde.org>

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

#include "mainwindow.h"
#include "part.h"
#include "kptdocuments.h"

#include <QCursor>
#include <QSplitter>
#include <q3iconview.h>
#include <QLabel>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>

#include <assert.h>
#include <kicon.h>
//#include "koshellsettings.h"

#include <KoApplicationAdaptor.h>
#include <KoDocument.h>
#include <KoGlobal.h>
#include <KoQueryTrader.h>

#include <kcomponentdata.h>
#include <kmimetypetrader.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdesktopfile.h>
#include <ktemporaryfile.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kshortcutsdialog.h>
#include <kstandarddirs.h>
#include <klibloader.h>
#include <kmenu.h>
#include <kservice.h>
#include <kmessagebox.h>
#include <krecentdocument.h>
#include <kparts/partmanager.h>
#include <kparts/event.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kdeversion.h>
#include <kaboutdata.h>
#include <kxmlguifactory.h>
#include <kcomponentdata.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>
#include <kservice.h>
#include <kio/netaccess.h>

#include <KoQueryTrader.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoView.h>
#include <KoPartSelectDia.h>
#include <KoFilterManager.h>

 
KPlatoWork_MainWindow::KPlatoWork_MainWindow( const KComponentData &instance )
    : KoMainWindow( instance ),
    m_editing( false ),
    m_activePage( -1 )
{
    kDebug()<<this;
    
    m_pLayout = new QSplitter( centralWidget() );
    m_pFrame = 0;
    setupTabWidget();

    connect( this, SIGNAL( documentSaved() ), this, SLOT( slotNewDocumentName() ) );
    
    m_client = new KPlatoWork_MainGUIClient( this );
    createShellGUI();
    
}


KPlatoWork_MainWindow::~KPlatoWork_MainWindow()
{
    kDebug();
    disconnect( m_pFrame, SIGNAL( currentChanged( QWidget* ) ), this, SLOT( slotUpdatePart( QWidget* )  ) );
    // Set the active part to 0 (as we do in ~KoMainWindow, but this is too
    // late for us, it gets activePartChanged signals delivered to a dead
    // KoMainWindow object).
    partManager()->setActivePart(0);
    m_activePage = -1;
    kDebug()<<m_activePage;
    while ( ! m_lstPages.isEmpty() )
    {
        Page p = m_lstPages.takeLast();
        if ( p.isKParts ) {
            int i = m_pFrame->indexOf( p.part->widget() );
            if ( i != -1 ) {
                m_pFrame->removeTab( i );
            }
        } else {
            int i = m_pFrame->indexOf( p.m_pView );
            if ( i != -1 ) {
                p.m_pDoc->removeShell( this );
                delete p.m_pView;
            }
            if ( p.isMainDocument && p.m_pDoc->views().isEmpty() ) {
                delete p.m_pDoc;
            }
        }
    }

    // prevent our parent destructor from doing stupid things
    setDocToOpen( 0 );
    setRootDocumentDirect( 0, Q3PtrList<KoView>() );
    
    saveSettings(); // Now save our settings before exiting
}

void KPlatoWork_MainWindow::sendMail()
{
    kDebug();
}

void KPlatoWork_MainWindow::setupTabWidget()
{
    if ( m_pFrame ) {
        m_pFrame->show();
        return;
    }
    m_pFrame = new KTabWidget( m_pLayout );
    m_pFrame->hide();
    m_pFrame->setSizePolicy( QSizePolicy( QSizePolicy::Minimum,
                             QSizePolicy::Preferred ) );
    m_pFrame->setTabPosition( KTabWidget::Bottom );
    
    m_tabCloseButton = new QToolButton( m_pFrame );
    connect( m_tabCloseButton, SIGNAL( clicked() ), this, SLOT( slotFileClose() ) );
    m_tabCloseButton->setIcon( KIcon( "tab-remove" ) );
    m_tabCloseButton->adjustSize();
    m_tabCloseButton->setToolTip( i18n("Close"));
    m_pFrame->setCornerWidget( m_tabCloseButton, Qt::BottomRight );
    m_tabCloseButton->hide();

    update();
    connect( m_pFrame, SIGNAL( currentChanged( QWidget* ) ), this, SLOT( slotUpdatePart( QWidget* )  ) );
/*    connect( m_pFrame, SIGNAL( contextMenu(QWidget * ,const QPoint &)), this, SLOT( tab_contextMenu( QWidget * ,const QPoint & ) ) );*/
    
}

void KPlatoWork_MainWindow::editDocument( KPlatoWork::Part *part, const KPlato::Document *doc )
{
    kDebug()<<part<<doc;
    if ( editWorkpackageDocument( doc, part ) ) {
        m_editing = true;
    }
}

bool KPlatoWork_MainWindow::editKPartsDocument(  KPlatoWork::Part *part, KService::Ptr service, const KPlato::Document *doc )
{
    kDebug()<<service->desktopEntryName()<<doc->url();
    KPlatoWork::DocumentChild *ch = part->openKPartsDocument( service, doc );
    if ( ch == 0 || ch->editor() == 0 ) {
        kDebug()<<"Failed to open KParts editor";
        return false;
    }
    Page page;
    page.isKParts = true;
    page.part = ch->editor();
    setupTabWidget();
    m_lstPages.append( page );
    
    page.part->openUrl( ch->url() );
    partManager()->addPart( page.part, false );
    
    m_pFrame->addTab( page.part->widget(), KIconLoader::global()->loadIcon( service->icon(), KIconLoader::Small ), i18n("Empty") );
    switchToPage( m_lstPages.count() - 1 );
    return true;
}

bool KPlatoWork_MainWindow::editKOfficePartDocument( KPlatoWork::Part *part, KMimeType::Ptr mimetype, const KPlato::Document *doc )
{
    kDebug()<<mimetype->name()<<doc->url();
    KPlatoWork::DocumentChild *ch = part->openKOfficeDocument( mimetype, doc );
    if ( ch == 0 ) {
        return false;
    }
    KoDocument *newdoc = dynamic_cast<KoDocument*>( ch->editor() );
    if ( newdoc == 0 ) {
        kDebug()<<"Ooops";
        return false;
    }
    connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    connect(newdoc, SIGNAL(completed()), this, SLOT(slotKSLoadCompleted()));
    connect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotKSLoadCanceled( const QString & )));
    newdoc->addShell( this ); // used by openUrl
    if ( ! newdoc->openUrl( ch->url() ) ) {
        newdoc->removeShell(this);
        removePage( newdoc );
        part->removeChildDocument( ch );
        delete newdoc;
        return false;
    }
    setMainDocument( newdoc, false );
    if ( ! ch->fileInfo().isWritable() ) {
        newdoc->setReadWrite( false );
    }
    updateCaption();
    return true;
}

bool KPlatoWork_MainWindow::editWorkpackageDocument( const KPlato::Document *doc, KPlatoWork::Part *part )
{
    kDebug()<<doc<<doc->url();
    KMimeType::Ptr mimeType = KMimeType::findByUrl( doc->url() );
    // Do we have a KOfficePart that handles this file nativly?
    QString constraint = QString( "[X-KDE-NativeMimeType] == '%1' or '%2' in [X-KDE-ExtraNativeMimeTypes]" ).arg( mimeType->name() ).arg( mimeType->name() );
    KService::List lst = KMimeTypeTrader::self()->query( mimeType->name(), "KOfficePart" , constraint );
    if ( ! lst.isEmpty() ) {
        kDebug()<<"Native KOfficePart:"<<lst.first()->desktopEntryName();
        return editKOfficePartDocument( part, mimeType, doc );
    }
    // Do we have a preffered KParts?
    KService::Ptr service = KMimeTypeTrader::self()->preferredService( mimeType->name(), "KParts/ReadWritePart");
    if ( service ) {
        kDebug()<<"Preferred KParts/ReadWritePart:"<<service->desktopEntryName();
        return editKPartsDocument( part, service, doc );
    }
    kDebug()<<"No preferred KParts service";
    // start in any suitable application
    return part->editOtherDocument( doc );
}

bool KPlatoWork_MainWindow::openDocumentInternal( const KUrl & url, KoDocument *newdoc )
{
    kDebug()<<url.url()<<mainDocument();
    // this is only called when opening a new file
    KoDocument *maindoc = mainDocument();
    if ( KoMainWindow::openDocumentInternal( url, newdoc ) ) {
        if ( maindoc && maindoc->isEmpty() ) {
            kDebug()<<"remove old maindoc"<<maindoc;
            removePage( maindoc );
        }
        kDebug()<<this<<newdoc<<"maindoc="<<mainDocument();
        return true;
    }
    return false;
}

// Separate from openDocument to handle async loading (remote URLs)
void KPlatoWork_MainWindow::slotKSLoadCompleted()
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

void KPlatoWork_MainWindow::slotKSLoadCanceled( const QString & errMsg )
{
    KMessageBox::error( this, errMsg );
    // ... can't delete the document, it's the one who emitted the signal...
    // ###### FIXME: This can be done in 3.0 with deleteLater, I assume (Werner)

    KoDocument* newdoc = (KoDocument *)(sender());
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    disconnect(newdoc, SIGNAL(completed()), this, SLOT(slotKSLoadCompleted()));
    disconnect(newdoc, SIGNAL(canceled( const QString & )), this, SLOT(slotKSLoadCanceled( const QString & )));
}

void KPlatoWork_MainWindow::saveAll()
{
    for ( int i = m_lstPages.count() - 1; i >= 0; --i ) {
        if ( m_lstPages[ i ].isKParts ) {
            m_lstPages[ i ].part->save();
            continue;
        }
        if ( m_lstPages[ i ].m_pDoc->isModified() ) {
            m_pFrame->setCurrentWidget( m_lstPages[ i ].m_pView );
            m_lstPages[ i ].m_pView->shell()->slotFileSave();
            if ( m_lstPages[ i ].m_pDoc->isModified() ) {
                break;
            }
        }
    }
    const Page &page = m_lstPages[ m_activePage ];
    m_pFrame->setCurrentWidget( page.isKParts ? page.part->widget() : page.m_pView );
}

// We come here for kplatowork as well as all koffice docs
void KPlatoWork_MainWindow::setRootDocument( KoDocument * doc )
{
    kDebug() <<"this="<<this<<"old="<<rootDocument()<<"new="<<doc;
    if ( rootDocument() == doc ) {
        return;
    }

    if ( doc == 0 ) {
        setRootDocumentDirect( 0, Q3PtrList<KoView>() );
        m_activePage = -1;
        KoMainWindow::updateCaption();
        return;
    }
    kDebug() <<doc;
    setupTabWidget();
    //saveDockwidgets();
    KoView *v = doc->createView(this);
    Q_ASSERT( v );
    Q3PtrList<KoView> views;
    views.append(v);
    setRootDocumentDirect( doc, views );

    Q_ASSERT( m_pFrame );
    v->setGeometry( 0, 0, m_pFrame->width(), m_pFrame->height() );
    kDebug()<<"setPartManager()"<<partManager();
    v->setPartManager( partManager() );
    
    KMimeType::Ptr mimeType = KMimeType::findByUrl( doc->url() );
    KService::Ptr service = KMimeTypeTrader::self()->preferredService( mimeType->name(),  QString::fromLatin1( "KParts/ReadWritePart" ) );
    QString icon;
    if ( service ) {
        icon = service->icon();
    }
    m_pFrame->addTab( v, KIconLoader::global()->loadIcon( icon, KIconLoader::Small ), i18n("Empty") );
    
    // Create a new page for this doc
    Page page;
    page.isMainDocument = true; // default
    page.m_pDoc = doc;
    page.m_pView = v;
    m_lstPages.append( page );
    v->show();

    switchToPage( m_lstPages.count() - 1 );
}

void KPlatoWork_MainWindow::slotNewDocumentName()
{
    updateCaption();
}

void KPlatoWork_MainWindow::updateCaption( const QString &caption, bool modified )
{
    if ( m_activePage == -1 ) {
        return setCaption( caption, modified );
    }
    kDebug()<<m_activePage<<m_lstPages.count();
    if ( m_lstPages[ m_activePage ].isMainDocument ) {
        QString c = caption + i18nc( "Main document", " [main]" );
        if ( ! m_lstPages[ m_activePage ].m_pDoc->isReadWrite() ) {
            c += i18n( " [write protected]" );
        }
        setCaption( c , modified );
    } else {
        updateCaption();
    }
}

void KPlatoWork_MainWindow::updateCaption()
{
    kDebug();
    QString name;
    QString caption;
    bool modified = false;
    if ( m_activePage != -1 ) {
        const Page &page = m_lstPages[ m_activePage ];
        // Lepagepportunity for setting a correct name for the icon in tabtext
        if ( page.isKParts ) {
            kDebug()<<page.part;
            name = page.part->url().fileName();
        } else if ( page.m_pDoc != 0 ) {
            kDebug()<<page.m_pDoc;
            // Get name from document info (title(), in about page)
            if ( page.m_pDoc->documentInfo() ) {
                name = page.m_pDoc->documentInfo()->aboutInfo("title");
            }
            if ( name.isEmpty() ) {
                // Fall back to document URL
                name = page.m_pDoc->url().fileName();
            }
        }
        if ( !name.isEmpty() ) { // else keep Untitled
            if ( name.length() > 20 ) {
                name.truncate( 17 );
                name += "...";
            }
            m_pFrame->setTabText( m_pFrame->currentIndex(), name );
        }
        // Then the caption...
        if ( page.isKParts ) {
            caption = page.part->url().fileName();
            modified = page.part->isModified();
        } else if ( ! page.isMainDocument ) {
            // Get caption from document info (title(), in about page)
            if ( page.m_pDoc->documentInfo() )
            {
                caption = page.m_pDoc->documentInfo()->aboutInfo( "title" );
            }
            if ( caption.isEmpty() ) {
                caption = page.m_pDoc->url().fileName();
            }
            if ( ! page.m_pDoc->isReadWrite() ) {
                caption += i18n(" [write protected]");
            }
            modified = page.m_pDoc->isModified();
        } else {
            kDebug()<<"Main document";
            // calls updateCaption(caption, modified)
            return page.m_pDoc->setTitleModified();
        }
    }
    setCaption( caption, modified );
}

void KPlatoWork_MainWindow::removePage( KParts::PartBase *doc )
{
    if ( doc == 0 ) {
        return;
    }
    // Don't delete the document, the main document takes care of that
    for( int pi = 0; pi < m_lstPages.count(); ++pi )
    {
        Page p = m_lstPages[ pi ];
        if ( p.isKParts ) {
            if ( p.part != doc ) {
                continue;
            }
            int i = m_pFrame->indexOf( p.part->widget() );
            if ( i != -1 ) {
                if ( m_activePage >= pi ) {
                    switchToPage( -1 );
                }
                m_lstPages.removeAt( pi );
            }
            break;
        } else if( p.m_pDoc == doc ) {
            int i = m_pFrame->indexOf( p.m_pView );
            if ( i != -1 ) {
                kDebug()<<pi<<"curr="<<m_activePage;
                if ( m_activePage >= pi ) {
                    switchToPage( -1 );
                }
                m_lstPages.removeAt( pi );
                p.m_pDoc->removeShell( this );
                delete p.m_pView;
                break;
            }
        }
    }
    kDebug()<<"Now"<<m_lstPages.count()<<"pages";
}

void KPlatoWork_MainWindow::slotUpdatePart( QWidget* widget )
{
    kDebug()<<widget;
    for( int i = 0; i < m_lstPages.count(); ++i ) {
        if ( m_lstPages[ i ].isKParts ) {
            if ( m_lstPages[ i ].part->widget() == widget ) {
                switchToPage( i );
                break;
            }
        } else {
            if( m_lstPages[ i ].m_pView == widget ) {
                switchToPage( i );
                break;
            }
        }
    }
}

void KPlatoWork_MainWindow::switchToPage( int index )
{
    if ( m_activePage != -1 ) {
        kDebug()<<"Deactivate current part"<<m_activePage;
        partManager()->setActivePart( 0, 0 );
        setRootDocumentDirect( 0, Q3PtrList<KoView>() );
    }
    // Select new active page (view)
    m_activePage = index;
    if ( index == -1 ) {
        kDebug()<<"No new page:"<<m_activePage;
        enableHelp( false );
        return;
    }
    const Page &page = m_lstPages[ index ];
    if ( page.isKParts ) {
        kDebug() <<"KParts document: setting active kpart to" << page.part;
        // set new active part
        m_pFrame->setCurrentWidget( page.part->widget() );
        updateCaption();
        page.part->widget()->setFocus();
        partManager()->setActivePart( page.part, page.part->widget() );
    } else {
        // Koffice document
        KoView *v = page.m_pView;
        kDebug() <<" setting active part to" << page.m_pDoc;
        // Make it active (GUI etc.)
        partManager()->setActivePart( page.m_pDoc, v );
        // Change current document
        Q3PtrList<KoView> views;
        views.append(v);
        setRootDocumentDirect( page.m_pDoc, views );
        // Raise the new page
        m_pFrame->setCurrentWidget( v );
        updateCaption();
        v->setFocus();
    }
    enableHelp( true );
}

void KPlatoWork_MainWindow::enableHelp( bool enable )
{
    if ( ! enable || m_activePage == -1 ) {
        partSpecificHelpAction->setEnabled(false);
        partSpecificHelpAction->setText( i18n( "Part Handbook" ) );
        return;
    }
    const Page &page = m_lstPages[ m_activePage ];
    if ( page.isKParts ) {
        return; //TODO
    } else if ( ! enable || page.m_pDoc->componentData().aboutData()->appName() == "kplatowork" ) {
        partSpecificHelpAction->setEnabled(false);
        partSpecificHelpAction->setText( i18n( "Part Handbook" ) );
        return;
    }
    partSpecificHelpAction->setEnabled(true);
    partSpecificHelpAction->setText(i18n("%1 Handbook",page.m_pDoc->componentData().aboutData()->programName()));
}
// Helper method for slotFileNew and slotFileClose
void KPlatoWork_MainWindow::chooseNewDocument( InitDocFlags initDocFlags )
{
    KoDocument* doc = rootDocument();
    KoDocument *newdoc = createDoc();

    if ( !newdoc )
        return;

    //FIXME: This needs to be handled differently
    connect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));
    disconnect(newdoc, SIGNAL(sigProgress(int)), this, SLOT(slotProgress(int)));

    if ( ( !doc  && ( initDocFlags == InitDocFileNew ) ) // ? (maybe if newdoc is not same type as doc, so reread componentData)
         || ( doc ) // we have a noneemty doc, we must keep it
       ) {
        // open newdoc in a new main window
        KPlatoWork_MainWindow *s = new KPlatoWork_MainWindow( newdoc->componentData() );
        s->show();
        newdoc->addShell( s );
        newdoc->showStartUpWidget( s, true /*Always show widget*/ );
        return;
    }
    // Ok to reuse this main window
    if( doc ) {
        // we get here if doc is empty
        // Close our views on the doc, and delete it if nobody else is viewing it
        setRootDocument( 0 );
    }
    // open newdoc in this mainwindow
    newdoc->addShell( this );
    newdoc->showStartUpWidget( this, true /*Always show widget*/ );
}
 
void KPlatoWork_MainWindow::slotFileNew()
{
    kDebug();
    chooseNewDocument( InitDocFileNew );
}

void KPlatoWork_MainWindow::slotFileOpen()
{
    kDebug();

    KFileDialog *dialog = new KFileDialog(KUrl("kfiledialog:///OpenDialog"), QString(), this);
    dialog->setObjectName( "file dialog" );
    dialog->setMode(KFile::File);
        
    dialog->setCaption( i18n("Open Document") );

    QStringList mimeFilter; mimeFilter <<"application/x-vnd.kde.kplato.work";
   
    dialog->setMimeFilter( mimeFilter );
    if(dialog->exec()!=QDialog::Accepted) {
        delete dialog;
        return;
    }
    KUrl url( dialog->selectedUrl() );
    delete dialog;

    if ( url.isEmpty() ) {
        return;
    }

    (void) openDocument( url );
}

bool KPlatoWork_MainWindow::openDocument( const KUrl &url )
{
    if ( !KIO::NetAccess::exists( url, KIO::NetAccess::SourceSide, 0 ) ) {
        // Trick to remove url from private recent list
        return KoMainWindow::openDocument( url );
    }
    if ( m_lstPages.isEmpty() ) {
        kDebug()<<"First document, load";
        if ( openDocumentInternal( url ) ) {
            (*m_lstPages.begin()).isMainDocument;
            return true;
        }
        return false;
    }
    if ( mainDocument() && ! mainDocument()->isEmpty() ) {
        kDebug()<<"We have a main document, open newdoc in a new main window";
        KPlatoWork_MainWindow *s = new KPlatoWork_MainWindow( mainDocument()->componentData() );
        if ( ! s->openDocument( url ) ) {
            delete s;
            return false;
        }
        s->show();
        return true;
    }
    kDebug()<<"We have an empty main document, load";
    if ( openDocumentInternal( url ) ) {
        (*m_lstPages.begin()).isMainDocument;
        return true;
    }
    return false;
}

void KPlatoWork_MainWindow::slotFileClose()
{
    kDebug();
    // No docs at all ?
    if ( m_lstPages.count() == 0 )
        close(); // close window
    else
        closeDocument(); // close only doc

    if ( m_pFrame->count() == 0 )
        m_tabCloseButton->hide();
}

void KPlatoWork_MainWindow::closeDocument()
{
    kDebug();
    // Set the root document to the current one - so that queryClose acts on it
    Page page = m_lstPages[ m_activePage ];
    int index = m_activePage;
    
    Q_ASSERT( m_activePage != -1 );
    if ( page.isKParts ) {
        return; //TODO
    }
    assert( rootDocument() == page.m_pDoc );
    
    // First do the standard queryClose
    kDebug() <<"MainWindow::closeDocument calling standard queryClose";
    if ( KPlatoWork_MainWindow::queryClose() ) {
        kDebug() <<"Ok for closing document";
        page.m_pDoc->removeShell(this);
        m_lstPages.removeAt( index );
        m_activePage = -1; // no active page right now

        kDebug() <<"m_lstPages has" << m_lstPages.count() <<" documents";
        if ( m_lstPages.count() > 0 ) {
            kDebug() <<"Activate the document behind";
            switchToPage( qMin( index, m_lstPages.count() - 1 ) );
        } else {
            kDebug() <<"Revert to initial state (no docs)";
            setRootDocument( 0 );
            partManager()->setActivePart( 0, 0 );
            enableHelp( false );
        }
        // Now delete the old view
        // Don't do it before, because setActivePart will call slotActivePartChanged,
        // which needs the old view (to unplug it and its plugins)
        delete page.m_pView; // NOTE: don't delete the doc
    }
    kDebug() <<"m_lstPages has" << m_lstPages.count() <<" documents";
}

bool KPlatoWork_MainWindow::queryClose()
{
    // This one is called by slotFileQuit and by the X button.
    kDebug()<<"Save documents, index="<<m_activePage;
    if ( m_lstPages.isEmpty() ) {
        return true; //hmmm
    }
    KoDocument *doc = mainDocument();
    Q_ASSERT( doc );
    if ( ! doc->isModified() ) {
        kDebug()<<doc<<doc->isModified();
        return true;
    }
    KoDocument *root = rootDocument();
    if ( root != doc ) {
        // KoMainWindow uses rootDocument()
        setRootDocumentDirect( doc, Q3PtrList<KoView>() );
    }
    bool ok = KoMainWindow::queryClose();
    if ( root == 0 ) {
        // there was no root document, so current doc was a KParts
        setRootDocumentDirect( 0, Q3PtrList<KoView>() );
    } else if ( root != doc ) {
        Q3PtrList<KoView> lst; foreach( KoView *v, root->views() ) { lst.append( v ); }
        setRootDocumentDirect( root, lst );
    } // else root == doc so we haven't touched anything
    return ok;
}

/*
// Should this be an additional action in the File menu ?
bool KPlatoWork_MainWindow::saveAllPages()
{
  // TODO
  return false;
}
*/

void KPlatoWork_MainWindow::saveSettings()
{
/*  MainSettings::setSidebarWidth( m_pLayout->sizes().first() );
  MainSettings::self()->writeConfig();*/
}

QString KPlatoWork_MainWindow::configFile() const
{
  //return readConfigFile( KStandardDirs::locate( "data", "koshell/koshell_shell.rc" ) );
  return QString(); // use UI standards only for now
}

KPlatoWork_MainWindow::Page KPlatoWork_MainWindow::findPage( const KParts::ReadWritePart *doc ) const
{
    for ( int i = 0; i < m_lstPages.count(); ++i ) {
        if ( m_lstPages[ i ].isKParts ) {
            if ( m_lstPages[ i ].part == doc ) {
                return m_lstPages[ i ];
            }
        } else if ( m_lstPages[ i ].m_pDoc == doc ) {
            return m_lstPages[ i ];
        }
    }
    return Page();
}

void KPlatoWork_MainWindow::setMainDocument( const KoDocument *doc, bool main )
{
    QList<Page>::Iterator it = m_lstPages.begin();
    for ( int i = 0; i < m_lstPages.count(); ++i ) {
        if ( m_lstPages[ i ].m_pDoc == doc ) {
            m_lstPages[ i ].isMainDocument = main;
            return;
        }
    }
}

KoDocument *KPlatoWork_MainWindow::mainDocument() const
{
    QList<Page>::ConstIterator it = m_lstPages.begin();
    for ( int i = 0; i < m_lstPages.count(); ++i ) {
        if ( m_lstPages[ i ].isMainDocument ) {
            return m_lstPages[ i ].m_pDoc;
        }
    }
    return 0;
}

bool KPlatoWork_MainWindow::isMainDocument( const KoDocument *doc ) const
{
    return findPage( doc ).isMainDocument;
}

void KPlatoWork_MainWindow::slotFileSave()
{
    kDebug();
    KoMainWindow::slotFileSave();
}

void KPlatoWork_MainWindow::slotFileSaveAs()
{
    kDebug();
    KoMainWindow::slotFileSaveAs();
}

// called from slotFileSave(), slotFileSaveAs(), queryClose(), slotEmailFile()
bool KPlatoWork_MainWindow::saveDocument( bool saveas, bool silent )
{
    kDebug()<<saveas<<silent;
    if ( m_lstPages[ m_activePage ].isKParts ) {
    }
    KoDocument *doc = rootDocument();
    if ( doc == 0 ) {
        return true;
    }
    if ( saveas ) {
        if ( isMainDocument( doc ) ) {
            kDebug()<<"save workpackage:"<<doc;
        } else {
            kDebug()<<"save embedded document:"<<doc;
        }
    }
    return KoMainWindow::saveDocument( saveas, silent );
}

void KPlatoWork_MainWindow::tab_contextMenu(QWidget * w,const QPoint &p)
{
    KMenu menu;
    KIconLoader il;
    QAction *mnuSave = menu.addAction( il.loadIconSet( "document-save", KIconLoader::Small ), i18n("Save") );
    QAction *mnuClose = menu.addAction( il.loadIcon( "window-close", KIconLoader::Small ), i18n("Close") );
    
    int tabnr = m_pFrame->indexOf( w );
    Page page = m_lstPages[tabnr]; //hmmm
    // disable save if there's nothing to save
    if ( page.isKParts ) {
        return; //TODO
    }
    if ( !page.m_pDoc->isModified() ) {
        mnuSave->setEnabled( false );
    }
    // show menu
    QAction *choice = menu.exec(p);
    
    if( choice == mnuClose ) {
        const int index = m_pFrame->currentIndex();
        m_pFrame->setCurrentIndex( tabnr );
        slotFileClose();
        if ( index > m_pFrame->currentIndex() ) {
            m_pFrame->setCurrentIndex(index-1);
        } else {
            m_pFrame->setCurrentIndex(index);
        }
    } else if ( choice == mnuSave ) {
        kDebug()<<"mnuSave";
    }
}

void KPlatoWork_MainWindow::slotConfigureKeys()
{
    KoView *view = rootView();
    KShortcutsDialog dlg(KShortcutsEditor::AllActions,KShortcutsEditor::LetterShortcutsAllowed, this );
    
    dlg.addCollection( actionCollection() );
    if ( view ) {
        dlg.addCollection( view->actionCollection() );
    }
    if ( rootDocument() ) {
        dlg.addCollection( rootDocument()->actionCollection() );
    }
    dlg.configure();
}

void KPlatoWork_MainWindow::createShellGUI( bool  )
{
    guiFactory()->addClient( m_client );
}

void KPlatoWork_MainWindow::showPartSpecificHelp()
{
    if ( m_activePage == -1 ) {
        return;
    }
    const Page &page = m_lstPages[ m_activePage ];
    if ( page.isKParts ) {
        return; //TODO
    }
    if ( page.m_pDoc == 0 ) {
        return;
    }
    KToolInvocation::invokeHelp("", page.m_pDoc->componentData().aboutData()->appName(), "");
}

void KPlatoWork_MainWindow::slotActivePartChanged( KParts::Part *newPart )
{
    kDebug()<<newPart<<m_activePage;
    if ( m_activePage != -1 ) {
        const Page &page = m_lstPages[ m_activePage ];
        if ( page.isKParts && guiFactory() ) {
            kDebug()<<"deactivate"<<page.part<<page.part->widget();
            KParts::GUIActivateEvent ev( false );
            QApplication::sendEvent( page.part, &ev );
            KXMLGUIClient *client = dynamic_cast<KXMLGUIClient*>( page.part->widget() );
            kDebug()<<client;
            if ( client && guiFactory() ) {
                kDebug()<<"remove client:"<<client<<page.part->widget();
                QApplication::sendEvent( page.part->widget(), &ev );
                guiFactory()->removeClient( client );
            }
        }
    }
    kDebug()<<newPart;
    if ( newPart == 0 || dynamic_cast<KoDocument*>( newPart ) || dynamic_cast<KoView*>( newPart  ) ) {
        KoMainWindow::slotActivePartChanged( newPart );
    } else {
        kDebug()<<"KParts:"<<newPart;
        KParts::GUIActivateEvent ev( true );
        QApplication::sendEvent( newPart, &ev );
        KXMLGUIClient *client = dynamic_cast<KXMLGUIClient*>( newPart->widget() );
        kDebug()<<newPart<<newPart->widget()<<client;
        if ( client && guiFactory() ) {
            guiFactory()->addClient( client );
            QApplication::sendEvent( newPart->widget(), &ev );
        }
    }
}


///////////////////
KPlatoWork_MainGUIClient::KPlatoWork_MainGUIClient( KPlatoWork_MainWindow *window ) : KXMLGUIClient()
{
    setXMLFile( "kplatowork_mainwindow.rc", true, true );
    window->partSpecificHelpAction = new KAction(KIcon("help-contents"), i18n("Part Handbook"), window );
    actionCollection()->addAction( "partSpecificHelp", window->partSpecificHelpAction );
    QObject::connect(window->partSpecificHelpAction, SIGNAL(triggered(bool) ), window, SLOT(showPartSpecificHelp()));
    window->partSpecificHelpAction->setEnabled(false);
}

#include "mainwindow.moc"
