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

#include <kdebug.h>
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

#define CURRENT_SYNTAX_VERSION XML_FILE_SYNTAX_VERSION

using namespace KPlato;

namespace KPlatoWork
{


DocumentChild::DocumentChild( Part *parent)
    : KoDocumentChild( parent ),
    m_doc( 0 ),
    m_copy( false ),
    m_process( 0 )
{
}

Part *DocumentChild::part() const
{
    return static_cast<Part*>( parentDocument() );
}

void DocumentChild::setFileInfo( const KUrl &url )
{
    m_fileinfo.setFile( url.path() );
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
        url = part()->extractFile( doc );
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
        KMessageBox::error( 0, i18n( "File does not exist: %1" ).arg( fileName() ) );
        return false;
    }
    KUrl filename( filePath() );
    KMimeType::Ptr mimetype = KMimeType::findByUrl( filename, 0, true );
    KService::Ptr service = KMimeTypeTrader::self()->preferredService( mimetype->name() );
    QStringList args;
    if ( service ) {
        args = KRun::processDesktopExec( *service, KUrl::List()<< filename );
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
        args << filename.url();
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
    return isOpen() || ( m_fileinfo.created() != m_fileinfo.lastModified() );
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

//------------------------------------
Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ),
        m_project( 0 ), m_xmlLoader()
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
    
    connect( KDirWatch::self(), SIGNAL( dirty( const QString & ) ), SLOT( slotDirty( const QString &) ) );
}

Part::~Part()
{
//    m_config.save();
    delete m_project;
}

void Part::slotDirty( const QString & )
{
    setModified( true );
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
    return true;
}

KUrl Part::extractFile( const Document *doc )
{
    KoStore *store = KoStore::createStore( localFilePath(), KoStore::Read, "", KoStore::Zip );
    if ( store->bad() )
    {
        KMessageBox::error( 0, i18n( "Could not create workpackage store:\n %1", localFilePath() ) );
        delete store;
        return KUrl();
    }
    //FIXME: should use a special tmp dir
    QString tmp = KStandardDirs::locateLocal( "tmp", QString(), false );
    KUrl url( tmp + doc->url().fileName() );
    kDebug()<<"Extract: "<<doc->url().fileName()<<" -> "<<url.pathOrUrl();
    if ( ! store->extractFile( doc->url().fileName(), url.path() ) ) {
        delete store;
        return KUrl();
    }
    delete store;
    return url;
}

bool Part::editDocument( const Document *doc )
{
    Q_ASSERT( doc != 0 );
    kDebug()<<doc->url();
    DocumentChild *ch = 0;
    foreach ( DocumentChild *c, m_childdocs ) {
        if ( c->doc() == doc ) {
            ch = c;
            break;
        }
    }
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
    return true;
}

void Part::addChild( DocumentChild *child )
{
    m_childdocs.append( child );
    KDirWatch::self()->addFile( child->filePath() );
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

bool Part::completeSaving( KoStore *store )
{
    if ( node() == 0 ) {
        return false;
    }
    KoStore *oldstore = KoStore::createStore( localFilePath(), KoStore::Read, "", KoStore::Zip );
    if ( oldstore->bad() ) {
        KMessageBox::error( 0, i18n( "Failed to open store:\n %1", localFilePath() ) );
        return false;
    }
    // First get all open documents
    kDebug()<<m_childdocs.count();
    foreach ( DocumentChild *cd, m_childdocs ) {
        if ( cd->isOpen() ) {
            if ( KMessageBox::warningContinueCancel( 0, i18n( "Document is open for editing: %1", cd->fileName() ) ) == KMessageBox::Cancel ) {
                setErrorMessage( "USER_CANCELED" );
                return false;
            }
        }
        kDebug()<<"Save"<<cd->fileName();
        store->addLocalFile( cd->filePath(), cd->fileName() );
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

}  //KPlatoWork namespace

#include "part.moc"
