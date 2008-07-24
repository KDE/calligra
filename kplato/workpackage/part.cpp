/* This file is part of the KDE project
 Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
 Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
 Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "part.h"
#include "view.h"
#include "factory.h"
#include "mainwindow.h"

#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdocuments.h"

#include <KoZoomHandler.h>
#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoOdfReadStore.h>

#include <qpainter.h>
#include <qfileinfo.h>
#include <QDir>
#include <QTimer>

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kparts/partmanager.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kopenwithdialog.h>
#include <kmimetype.h>
#include <kmimetypetrader.h>
//#include <kserviceoffer.h>
#include <krun.h>
#include <kprocess.h>
#include <kdirwatch.h>

#include <KoGlobal.h>
#include <KoMainWindow.h>

#define CURRENT_SYNTAX_VERSION XML_FILE_SYNTAX_VERSION

using namespace KPlato;

namespace KPlatoWork
{

DocumentChild::DocumentChild( Part *parent)
    : KoDocumentChild( parent ),
    m_doc( 0 ),
    m_type( Type_Unknown ),
    m_copy( false ),
    m_process( 0 ),
    m_editor( 0 ),
    m_editormodified( false ),
    m_filemodified( false )
{
}

DocumentChild::DocumentChild( KParts::ReadWritePart *editor, const KUrl &url, const Document *doc, Part *parent)
    : KoDocumentChild( parent ),
    m_doc( doc ),
    m_type( Type_Unknown ),
    m_copy( true ),
    m_process( 0 ),
    m_editor( editor ),
    m_editormodified( false ),
    m_filemodified( false )
{
    setFileInfo( url );
    if ( dynamic_cast<KoDocument*>( editor ) ) {
        kDebug()<<"Creating KOffice doc";
        m_type = Type_KOffice;
        connect( static_cast<KoDocument*>( editor ), SIGNAL( modified( bool ) ), this, SLOT( setModified( bool ) ) );
    } else {
        kDebug()<<"Creating KParts doc";
        m_type = Type_KParts;
        slotUpdateModified();
    }
}

DocumentChild::~DocumentChild()
{
    kDebug();
    disconnect( KDirWatch::self(), SIGNAL( dirty( const QString & ) ), this, SLOT( slotDirty( const QString &) ) );
    KDirWatch::self()->removeFile( filePath() );
    
    if ( m_type == Type_KOffice || m_type == Type_KParts ) {
        delete m_editor;
    }
}

Part *DocumentChild::parentPart() const
{
    return static_cast<Part*>( parentDocument() );
}

void DocumentChild::setFileInfo( const KUrl &url )
{
    m_fileinfo.setFile( url.path() );
    connect( KDirWatch::self(), SIGNAL( dirty( const QString & ) ), this, SLOT( slotDirty( const QString &) ) );
    KDirWatch::self()->addFile( filePath() );
}

void DocumentChild::setModified( bool mod )
{
    kDebug()<<mod<<filePath();
    if ( m_editormodified != mod ) {
        m_editormodified = mod;
        emit modified( mod );
    }
}

void DocumentChild::slotDirty( const QString &file )
{
    kDebug()<<filePath()<<file<<m_filemodified;
    if ( file == filePath() && ! m_filemodified ) {
        kDebug()<<file<<"is modified";
        m_filemodified = true;
        emit fileModified( true );
    }
}

void DocumentChild::slotUpdateModified()
{
    if ( m_type == Type_KParts && m_editor && ( m_editor->isModified() != m_editormodified ) ) {
        setModified( m_editor->isModified() );
    }
    QTimer::singleShot( 500, this, SLOT( slotUpdateModified() ) );
}

bool DocumentChild::setDoc( const Document *doc )
{
    Q_ASSERT ( m_doc == 0 );
    if ( isOpen() ) {
        KMessageBox::error( 0, i18n( "Document is already open: %1", doc->url().pathOrUrl() ) );
        return false;
    }
    m_doc = doc;
    KUrl url;
    if ( doc->sendAs() == Document::SendAs_Copy ) {
        url = parentPart()->extractFile( doc );
        if ( url.url().isEmpty() ) {
            KMessageBox::error( 0, i18n( "Could not extract file: %1", doc->url().pathOrUrl() ) );
            return false;
        }
        m_copy = true;
    } else {
        url = doc->url();
    }
    if ( ! url.isValid() ) {
        KMessageBox::error( 0, i18n( "Invalid url: %1", url.pathOrUrl() ) );
        return false;
    }
    setFileInfo( url.path() );
    return true;
}

bool DocumentChild::editDoc()
{
    Q_ASSERT( m_doc != 0 );
    kDebug()<<"file:"<<filePath();
    if ( isOpen() ) {
        KMessageBox::error( 0, i18n( "Document is already open:\n %1", m_doc->url().pathOrUrl() ) );
        return false;
    }
    if ( ! m_fileinfo.exists() ) {
        KMessageBox::error( 0, i18n( "File does not exist: %1", fileName() )  );
        return false;
    }
    KUrl filename( filePath() );
    KMimeType::Ptr mimetype = KMimeType::findByUrl( filename, 0, true );
    KService::Ptr service = KMimeTypeTrader::self()->preferredService( mimetype->name() );
    return startProcess( service, filename );
}

bool DocumentChild::startProcess( KService::Ptr service, const KUrl &url )
{
    QStringList args;
    KUrl::List files;
    if ( url.isValid() ) {
        files << url;
    }
    if ( service ) {
        args = KRun::processDesktopExec( *service, files );
    } else {
        KUrl::List list;
        KOpenWithDialog dlg( list, i18n("Edit with:"), QString::null, 0 );
        if ( dlg.exec() == QDialog::Accepted ){
            args << dlg.text();
        }
        if ( args.isEmpty() ) {
            kDebug()<<"No executable selected";
            return false;
        }
        args << url.url();
    }
    kDebug()<<args;
    m_process = new KProcess();
    m_process->setProgram( args );
    connect( m_process, SIGNAL( finished( int,  QProcess::ExitStatus ) ), SLOT( slotEditFinished( int,  QProcess::ExitStatus ) ) );
    connect( m_process, SIGNAL( error( QProcess::ProcessError ) ), SLOT( slotEditError( QProcess::ProcessError ) ) );
    m_process->start();
    return true;
}

bool DocumentChild::isModified() const
{
    return m_editormodified;
}

bool DocumentChild::isFileModified() const
{
    return m_filemodified;
}

void DocumentChild::slotEditFinished( int,  QProcess::ExitStatus )
{
    kDebug()<<filePath();
    delete m_process;
    m_process = 0;
}

void DocumentChild::slotEditError( QProcess::ProcessError status )
{
    kDebug()<<status;
    if ( status == QProcess::FailedToStart || status == QProcess::Crashed ) {
        m_process->deleteLater();
        m_process = 0;
    } else kDebug()<<"Error="<<status<<" what to do?";
}

bool DocumentChild::saveToStore( KoStore *store )
{
    kDebug()<<filePath();
    KDirWatch::self()->removeFile( filePath() );
    bool ok = false;
    bool wasmod = m_filemodified;
    if ( m_type == Type_KOffice || m_type == Type_KParts ) {
        if ( m_editor->isModified() ) {
            ok = m_editor->save(); // hmmmm
        } else {
            ok = true;
        }
    } else if ( m_type == Type_Other ) {
        if ( isOpen() ) {
            kWarning()<<"External editor open";
        }
        ok = true;
    } else {
        kError()<<"Unknown document type";
    }
    if ( ok ) {
        kDebug()<<"Add to store:"<<fileName();
        store->addLocalFile( filePath(), fileName() );
        m_filemodified = false;
        if ( wasmod != m_filemodified ) {
            emit fileModified( m_filemodified );
        }
    }
    KDirWatch::self()->addFile( filePath() );
    return ok;
}


//------------------------------------
Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ),
        m_project( 0 ),
        m_xmlLoader(),
        m_modified( false )
{
    setComponentData( Factory::global() );
    setTemplateType( "kplatowork_template" );
    
    // Add library translation files
    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "kplatokernel" );
        locale->insertCatalog( "kplatomodels" );
        locale->insertCatalog( "kplatoui" );
    }

//     m_config.setReadWrite( isReadWrite() || !isEmbedded() );
//     m_config.load();

    setProject( new Project() ); // after config is loaded
    
}

Part::~Part()
{
    kDebug();
//    m_config.save();
    delete m_project;
    while ( ! m_childdocs.isEmpty() ) {
        delete m_childdocs.takeFirst();
    }
}

void Part::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( changed() ), this, SIGNAL( changed() ) );
        delete m_project;
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( changed() ), this, SIGNAL( changed() ) );
    }
    emit changed();
}

KoView *Part::createViewInstance( QWidget *parent )
{
    View *view = new View( this, parent );
    connect( view, SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
    return view;
}

bool Part::loadOdf( KoOdfReadStore & odfStore )
{
    kDebug();
    return loadXML( 0, odfStore.contentDoc() ); // We have only one format, so try to load that!
}

bool Part::loadXML( QIODevice *, const KoXmlDocument &document )
{
    kDebug();
    QTime dt;
    dt.start();
    emit sigProgress( 0 );

    QString value;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.kplato.work" ) {
        kError() << "Unknown mime type " << value << endl;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato.work, got %1", value ) );
        return false;
    }
    QString m_syntaxVersion = plan.attribute( "version", CURRENT_SYNTAX_VERSION );
    m_xmlLoader.setVersion( m_syntaxVersion );
    if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of KPlatoWork (syntax version: %1)\n"
                               "Opening it in this version of KPlatoWork will lose some information.", m_syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }
    emit sigProgress( 5 );

#ifdef KOXML_USE_QDOM
    int numNodes = plan.childNodes().count();
#else
    int numNodes = plan.childNodesCount();
#endif
    if ( numNodes > 2 ) {
        //TODO: Make a proper bitching about this
        kDebug() <<"*** Error ***";
        kDebug() <<"  Children count should be maximum 2, but is" << numNodes;
        return false;
    }
    emit sigProgress( 100 ); // the rest is only processing, not loading

    kDebug() <<"Loading took" << ( float ) ( dt.elapsed() ) / 1000 <<" seconds";

    m_xmlLoader.startLoad();
    KoXmlNode n = plan.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "project" ) {
            Project * newProject = new Project();
            m_xmlLoader.setProject( newProject );
            if ( newProject->load( e, m_xmlLoader ) ) {
                // The load went fine. Throw out the old project
                setProject( newProject );
                // Load documents
                Task *t = dynamic_cast<Task*>( node() );
                if ( t && t->type() == Node::Type_Task ) {
                    foreach ( Document *doc, t->documents().documents() ) {
                        if ( doc->type() == Document::Type_Product ) {
                            kDebug()<<"load "<<doc->url();
                            //loadObject( doc );
                        }
                    }
                }
            } else {
                delete newProject;
                m_xmlLoader.addMsg( XMLLoaderObject::Errors, "Loading of work package failed" );
                //TODO add some ui here
            }
        } else if ( e.tagName() == "objects" ) {
            kDebug()<<"loadObjects";
            //loadObjects( e );
        }
    }
    m_xmlLoader.stopLoad();
    emit sigProgress( 100 ); // the rest is only processing, not loading

    kDebug() <<"Loading took" << ( float ) ( dt.elapsed() ) / 1000 <<" seconds";

    // do some sanity checking on document.
    emit sigProgress( -1 );

    setModified( false );
    emit changed();
    return true;
}

bool Part::completeLoading( KoStore * )
{
    QTimer::singleShot( 10, this, SLOT( saveToProjects() ) );
    setTitleModified();
    return true;
}

void Part::saveToProjects()
{
    kDebug();
    Node *n = node();
    if ( n == 0 ) {
        kWarning()<<"No node in this project";
        return;
    }
    KStandardDirs *sd = componentData().dirs();

    QString projectName = m_project->name().remove( ' ' );
    QString path = sd->saveLocation( "projects", projectName + '/' );
    kDebug()<<"path="<<path;
    QString wpName = QString( n->name().remove( ' ' ) + '_' + n->id() + ".kplatowork" );
    QString filePath = path + wpName;
    if ( filePath == localFilePath() ) {
        // opened from projects store
        setTitleModified();
        return;
    }
    if ( QFileInfo( filePath ).exists() ) {
        if ( KMessageBox::warningYesNo( 0, i18n( "This workpackage already exists in your project store:\n%1\nDo you want to overwrite it?", wpName ) ) == KMessageBox::No ) {
            return;
        }
    } else {
        if ( KMessageBox::warningYesNo( 0, i18n( "This workpackage is not in your project store:\n%1\nDo you want to save it?", wpName ) ) == KMessageBox::No ) {
            return;
        }
    }
    m_oldFile = localFilePath();
    saveAs( KUrl( filePath ) );
    m_oldFile = QString();
    if ( ! isSingleViewMode() ) {
        QFileInfo fi( filePath );
        setReadWrite( fi.isWritable() );
    }
    setTitleModified();
    return;
}

KUrl Part::extractFile( const Document *doc )
{
    KoStore *store = KoStore::createStore( localFilePath(), KoStore::Read, "", KoStore::Zip );
    if ( store->bad() )
    {
        KMessageBox::error( 0, i18n( "Could not open workpackage store:\n %1", localFilePath() ) );
        delete store;
        return KUrl();
    }
    //FIXME: should use a special tmp dir
    QString tmp = KStandardDirs::locateLocal( "tmp", QString(), false );
    KUrl url( tmp + doc->url().fileName() );
    kDebug()<<"Extract: "<<doc->url().fileName()<<" -> "<<url.pathOrUrl();
    if ( ! store->extractFile( doc->url().fileName(), url.path() ) ) {
        delete store;
        KMessageBox::error( 0, i18n( "Could not extract file:\n %1", doc->url().fileName() ) );
        return KUrl();
    }
    delete store;
    return url;
}

int Part::docType( const Document *doc ) const
{
    DocumentChild *ch = findChild( doc );
    if ( ch == 0 ) {
        return DocumentChild::Type_Unknown;
    }
    return ch->type();
}

DocumentChild *Part::findChild( const Document *doc ) const
{
    foreach ( DocumentChild *c, m_childdocs ) {
        if ( c->doc() == doc ) {
            return c;
        }
    }
    return 0;
}

DocumentChild *Part::openKOfficeDocument( KMimeType::Ptr mimetype, const Document *doc )
{
    Q_ASSERT( doc != 0 );
    kDebug()<<mimetype->name()<<doc->url();
    DocumentChild *ch = findChild( doc );
    if ( ch ) {
        KMessageBox::error( 0, i18n( "Document is already open" ) );
        return 0;
    }
    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( mimetype->name().toLatin1() );
    if ( entry.isEmpty() ) {
        kDebug()<<"Non-koffice document";
        return 0;
    }
    KUrl url = extractFile( doc );
    if ( url.isEmpty() ) {
        KMessageBox::error( 0, i18n( "Could not extract file:\n%1" ) );
        return 0;
    }
    KoDocument* newdoc = entry.createDoc();
    if ( !newdoc ) {
        KMessageBox::error( 0, i18n( "Failed to create KOffice document" ) );
        return 0;
    }
    ch = new DocumentChild( newdoc, url, doc, this );
    addChild( ch );
    return ch;
}

DocumentChild *Part::openKPartsDocument( KService::Ptr service, const Document *doc )
{
    Q_ASSERT( doc != 0 );
    kDebug()<<service->desktopEntryName()<<doc->url();
    DocumentChild *ch = findChild( doc );
    if ( ch ) {
        KMessageBox::error( 0, i18n( "Document is already open" ) );
        return 0;
    }
    KUrl url = extractFile( doc );
    if ( url.isEmpty() ) {
        kDebug()<<"Failed to extract file";
        return 0;
    }
    KParts::ReadWritePart *part = service->createInstance<KParts::ReadWritePart>();
    if ( part == 0 ) {
        kDebug()<<"Failed to create part";
        return 0;
    }
    ch = new DocumentChild( part, url, doc, this );
    addChild( ch );
    return ch;
}

bool Part::editOtherDocument( const Document *doc )
{
    Q_ASSERT( doc != 0 );
    kDebug()<<doc->url();
    DocumentChild *ch = findChild( doc );
    if ( ch ) {
        if ( ch->isOpen() ) {
            KMessageBox::error( 0, i18n( "Document is already open" ) );
            return false;
        }
    } else {
        ch = new DocumentChild( this );
        if ( ! ch->setDoc( doc ) ) {
            delete ch;
            return false;
        }
    }
    if ( ! ch->editDoc() ) {
        delete ch;
        return false;
    }
    addChild( ch );
    setModified( true );
    return true;
}

void Part::removeChildDocument( DocumentChild *child )
{
    disconnect( child, SIGNAL( modified( bool ) ), this, SLOT( slotUpdateModified( bool ) ) );
    disconnect( child, SIGNAL( fileModified( bool ) ), this, SLOT( slotChildModified( bool ) ) );
    int i = m_childdocs.indexOf( child );
    if ( i != -1 ) {
        // TODO: process etc
        m_childdocs.removeAt( i );
        delete child;
    }
}

void Part::addChild( DocumentChild *child )
{
    m_childdocs.append( child );
    connect( child, SIGNAL( modified( bool ) ), this, SLOT( slotChildModified( bool ) ) );
    connect( child, SIGNAL( fileModified( bool ) ), this, SLOT( slotChildModified( bool ) ) );
    kDebug()<<child<<m_childdocs;
}

void Part::slotChildModified( bool mod )
{
    kDebug()<<mod;
    if ( mod ) {
        setModified( true );
    } else {
        bool m = false;
        foreach( DocumentChild *ch, m_childdocs ) {
            if ( ch->isModified() || ch->isFileModified() ) {
                m = true;
                break;
            }
        }
        setModified( m || m_modified );
    }
    setTitleModified();
}

bool Part::copyFile( KoStore *from, KoStore *to, const QString &filename )
{
    QByteArray data;
    if ( ! from->extractFile( filename , data ) ) {
        KMessageBox::error( 0, i18n( "Failed read file:\n %1", filename ) );
        return false;
    }
    if ( ! to->addDataToFile( data, filename ) ) {
        KMessageBox::error( 0, i18n( "Failed write file:\n %1", filename ) );
        return false;
    }
    kDebug()<<"Copied file:"<<filename;
    return true;
}

void Part::setDocumentClean( bool clean )
{
    m_modified = !clean;
    
    bool mod = false;
    foreach( DocumentChild *ch, m_childdocs ) {
        if ( ch->isModified() || ch->isFileModified() ) {
            mod = true;
            break;
        }
    }
    // if no child is modified follow clean, else stay modified
    if ( ! mod ) {
        setModified ( ! clean );
    }
}

void Part::setTitleModified()
{
    kDebug()<<" url:"<<url().url();
    KoDocument *doc = dynamic_cast<KoDocument *>( parent() );
    QString pname = m_project->name();
    QString tname = node() == 0 ? i18n( "none" ) : node()->name();
    QString caption = pname + '/' + tname;
    if ( doc ) {
        doc->setTitleModified( caption, isModified() );
    } else {
        kDebug()<<caption;
        setTitleModified( caption, isModified() );
    }
}

bool Part::saveAs( const KUrl &url )
{
    m_oldFile = localFilePath();
    bool res = KoDocument::saveAs( url );
    m_oldFile = localFilePath();
    kDebug()<<m_oldFile;
//    setTitleModified();
    if ( res ) {
        QFileInfo fi( localFilePath() );
        setReadWrite( fi.isWritable() );
    }
    return res;
}

bool Part::completeSaving( KoStore *store )
{
    kDebug();
    if ( node() == 0 ) {
        return false;
    }
    KoStore *oldstore = KoStore::createStore( oldFileName(), KoStore::Read, "", KoStore::Zip );
    if ( oldstore->bad() ) {
        KMessageBox::error( 0, i18n( "Failed to open store:\n %1", oldFileName() ) );
        return false;
    }
    // First get all open documents
    kDebug()<<m_childdocs.count();
    foreach ( DocumentChild *cd, m_childdocs ) {
        if ( ! cd->saveToStore( store ) ) {
        }
    }
    // Then get files from the old store copied to the new store
    foreach ( Document *doc,  node()->documents().documents() ) {
        if ( doc->sendAs() != Document::SendAs_Copy ) {
            continue;
        }
        if ( ! store->hasFile( doc->url().fileName() ) ) {
            copyFile( oldstore, store, doc->url().fileName() );
        }
    }
    return true;
}

QDomDocument Part::saveXML()
{
    kDebug();
    QDomDocument document( "kplato-workpackage" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "kplatowork" );
    doc.setAttribute( "editor", "KPlatoWork" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato.work" );
    doc.setAttribute( "version", CURRENT_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    m_project->save( doc );

    return document;
}

void Part::paintContent( QPainter &, const QRect &)
{
    // Don't embed this app!!!
}

void Part::slotViewDestroyed()
{
}

Node *Part::node() const
{
    if ( m_project == 0 ) {
        return 0;
    }
    return m_project->childNode( 0 );
}

void Part::showStartUpWidget( KoMainWindow* parent, bool alwaysShow )
{
    KoDocument::showStartUpWidget( parent, alwaysShow );
    KPlatoWork_MainWindow *mw = dynamic_cast<KPlatoWork_MainWindow*>( parent );
    if ( mw ) {
        mw->setDocToOpen( 0 ); // Not used in KPlatoWork_MainWindow
    }
}

}  //KPlatoWork namespace

#include "part.moc"
