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

#include "kptpart.h"
#include "kptview.h"
#include "kptfactory.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptcontext.h"
#include "kptschedulerpluginloader.h"
#include "kptschedulerplugin.h"
#include "kptbuiltinschedulerplugin.h"
#include "kptcommand.h"
#include "kplatosettings.h"
#include "kpttask.h"

//#include "KDGanttViewTaskLink.h"

#include <KoZoomHandler.h>
#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoOdfReadStore.h>

#include <QApplication>
#include <qpainter.h>
#include <QDir>
#include <QMutableMapIterator>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kparts/partmanager.h>
#include <kmimetype.h>

#include <KoGlobal.h>

namespace KPlato
{

// temporary convinience class
class Package
{
public:
    Package() {}
    Project *project;
    QString ownerId;
    QString ownerName;

    WorkPackageSettings settings;
};

Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ),
        m_project( 0 ), m_parentWidget( parentWidget ),
        m_context( 0 ), m_xmlLoader(),
        m_loadingTemplate( false )
{
    setComponentData( Factory::global() );
    setTemplateType( "kplato_template" );
    m_config.setReadWrite( isReadWrite() || !isEmbedded() );
    // Add library translation files
    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "kplatolibs" );
        locale->insertCatalog( "kdgantt" );
        locale->insertCatalog( "kabc" );
        locale->insertCatalog( "timezones4" );

        m_config.setLocale( new KLocale( *locale ) );
    }

    loadSchedulerPlugins();

    setProject( new Project( m_config ) ); // after config & plugins are loaded
    m_project->setId( m_project->uniqueNodeId() );

    qDebug()<<"setProject: initial start"<<isReadWrite()<<isEmbedded()<<singleViewMode;
    QTimer::singleShot ( 5000, this, SLOT( checkForWorkPackages() ) );

}


Part::~Part()
{
    delete m_project;
    qDeleteAll( m_mergedPackages );
}

void Part::setReadWrite( bool rw )
{
    m_config.setReadWrite( rw || !isEmbedded() ); // embedded in koffice doc
    KoDocument::setReadWrite( rw );
}

void Part::loadSchedulerPlugins()
{
    // Add built-in scheduler
    addSchedulerPlugin( "Built-in", new BuiltinSchedulerPlugin( this ) );

    // Add all real scheduler plugins
    SchedulerPluginLoader *loader = new SchedulerPluginLoader(this);
    connect(loader, SIGNAL(pluginLoaded(const QString&, SchedulerPlugin*)), this, SLOT(addSchedulerPlugin(const QString&, SchedulerPlugin*)));
    loader->loadAllPlugins();
}

void Part::addSchedulerPlugin( const QString &key, SchedulerPlugin *plugin)
{
    kDebug()<<plugin;
    m_schedulerPlugins[key] = plugin;
}

void Part::configChanged()
{
    //m_project->setConfig( m_config );
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
//        m_project->setConfig( config() );
        m_project->setSchedulerPlugins( m_schedulerPlugins );
    }
    m_aboutPage.setProject( project );
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
    kWarning()<< "OpenDocument not supported, let's try native xml format";
    return loadXML( odfStore.contentDoc(), 0 ); // We have only one format, so try to load that!
}

bool Part::loadXML( const KoXmlDocument &document, KoStore* )
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
        kError() << "No mime type specified!";
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.kplato" ) {
        kError() << "Unknown mime type " << value;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato, got %1", value ) );
        return false;
    }
    QString syntaxVersion = plan.attribute( "version", KPLATO_FILE_SYNTAX_VERSION );
    m_xmlLoader.setVersion( syntaxVersion );
    if ( syntaxVersion > KPLATO_FILE_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of KPlato (syntax version: %1)\n"
                               "Opening it in this version of KPlato will lose some information.", syntaxVersion ),
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
#if 0 
This test does not work any longer. KoXml adds a couple of elements not present in the file!!
    if ( numNodes > 2 ) {
        //TODO: Make a proper bitching about this
        kDebug() <<"*** Error ***";
        kDebug() <<"  Children count should be maximum 2, but is" << numNodes;
        return false;
    }
#endif
    m_xmlLoader.startLoad();
    KoXmlNode n = plan.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "project" ) {
            Project * newProject = new Project( m_config );
            m_xmlLoader.setProject( newProject );
            if ( newProject->load( e, m_xmlLoader ) ) {
                if ( newProject->id().isEmpty() ) {
                    newProject->setId( newProject->uniqueNodeId() );
                }
                // The load went fine. Throw out the old project
                setProject( newProject );
            } else {
                delete newProject;
                m_xmlLoader.addMsg( XMLLoaderObject::Errors, "Loading of project failed" );
                //TODO add some ui here
            }
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

QDomDocument Part::saveXML()
{
    kDebug();
    QDomDocument document( "kplato" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "kplato" );
    doc.setAttribute( "editor", "KPlato" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato" );
    doc.setAttribute( "version", KPLATO_FILE_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    m_project->save( doc );
    
    return document;
}

QDomDocument Part::saveWorkPackageXML( const Node *node, long id, Resource *resource )
{
    kDebug();
    QDomDocument document( "kplato" );

    document.appendChild( document.createProcessingInstruction(
                "xml",
    "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "kplatowork" );
    doc.setAttribute( "editor", "KPlato" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato.work" );
    doc.setAttribute( "version", KPLATOWORK_FILE_SYNTAX_VERSION );
    doc.setAttribute( "kplato-version", KPLATO_FILE_SYNTAX_VERSION );
    document.appendChild( doc );

    // Work package info
    QDomElement wp = document.createElement( "workpackage" );
    if ( resource ) {
        wp.setAttribute( "owner", resource->name() );
        wp.setAttribute( "owner-id", resource->id() );
    }
    wp.setAttribute( "time-tag", KDateTime::currentLocalDateTime().toString( KDateTime::ISODate ) );
    doc.appendChild( wp );

    // Save the project
    m_project->saveWorkPackageXML( doc, node, id );
    
    return document;
}

bool Part::saveWorkPackageToStream( QIODevice * dev, const Node *node, long id, Resource *resource )
{
    QDomDocument doc = saveWorkPackageXML( node, id, resource );
    // Save to buffer
    QByteArray s = doc.toByteArray(); // utf8 already
    dev->open( QIODevice::WriteOnly );
    int nwritten = dev->write( s.data(), s.size() );
    if ( nwritten != (int)s.size() ) {
        kWarning()<<"wrote:"<<nwritten<<"- expected:"<< s.size();
    }
    return nwritten == (int)s.size();
}

bool Part::saveWorkPackageFormat( const QString &file, const Node *node, long id, Resource *resource  )
{
    kDebug() <<"Saving to store";

    KoStore::Backend backend = KoStore::Zip;
#ifdef QCA2
/*    if ( d->m_specialOutputFlag == SaveEncrypted ) {
        backend = KoStore::Encrypted;
        kDebug() <<"Saving using encrypted backend.";
    }*/
#endif

    QByteArray mimeType = "application/x-vnd.kde.kplato.work";
    kDebug() <<"MimeType=" << mimeType;
    
    KoStore* store = KoStore::createStore( file, KoStore::Write, mimeType, backend );
/*    if ( d->m_specialOutputFlag == SaveEncrypted && !d->m_password.isNull( ) ) {
        store->setPassword( d->m_password );
    }*/
    if ( store->bad() ) {
        setErrorMessage( i18n( "Could not create the workpackage file for saving: %1", file ) ); // more details needed?
        delete store;
        return false;
    }
    // Tell KoStore not to touch the file names
    store->disallowNameExpansion();

    if ( ! store->open( "root" ) ) {
        setErrorMessage( i18n( "Not able to write '%1'. Partition full?", QString( "maindoc.xml") ) );
        delete store;
        return false;
    }
    KoStoreDevice dev( store );
    if ( !saveWorkPackageToStream( &dev, node, id, resource ) || !store->close() ) {
        kDebug() <<"saveToStream failed";
        delete store;
        return false;
    }
    node->documents().saveToStore( store );
    
    kDebug() <<"Saving done of url:" << file;
    if ( !store->finalize() ) {
        delete store;
        return false;
    }
    // Success
    delete store;

    return true;
}

bool Part::saveWorkPackageUrl( const KUrl & _url, const Node *node, long id, Resource *resource )
{
    //kDebug()<<_url;
    QApplication::setOverrideCursor( Qt::WaitCursor );
    emit statusBarMessage( i18n("Saving...") );
    bool ret = false;
    ret = saveWorkPackageFormat( _url.path(), node, id, resource ); // kzip don't handle file://
    QApplication::restoreOverrideCursor();
    emit clearStatusBarMessage();
    return ret;
}

bool Part::loadWorkPackage( Project &project, const KUrl &url )
{
    //qDebug()<<"loadWorkPackage:"<<url;
    if ( ! url.isLocalFile() ) {
        kDebug()<<"TODO: download if url not local";
        return false;
    }
    KoStore * store = KoStore::createStore( url.path(), KoStore::Read, "", KoStore::Auto );
    if ( store->bad() ) {
//        d->lastErrorMessage = i18n( "Not a valid KOffice file: %1", file );
        kDebug()<<"bad store"<<url.prettyUrl();
        delete store;
//        QApplication::restoreOverrideCursor();
        return false;
    }
    if ( ! store->open( "root" ) ) { // "old" file format (maindoc.xml)
        // i18n( "File does not have a maindoc.xml: %1", file );
        kDebug()<<"No root"<<url.prettyUrl();
        delete store;
//        QApplication::restoreOverrideCursor();
        return false;
    }
    KoXmlDocument doc;
    QString errorMsg; // Error variables for QDomDocument::setContent
    int errorLine, errorColumn;
    bool ok = doc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn );
    if ( ! ok ) {
        kError() << "Parsing error in " << url.url() << "! Aborting!" << endl
                << " In line: " << errorLine << ", column: " << errorColumn << endl
                << " Error message: " << errorMsg;
        //d->lastErrorMessage = i18n( "Parsing error in %1 at line %2, column %3\nError message: %4",filename  ,errorLine, errorColumn , QCoreApplication::translate("QXml", errorMsg.toUtf8(), 0, QCoreApplication::UnicodeUTF8));
    } else {
        Project *p = loadWorkPackageXML( project, store->device(), doc, url );
        if ( p == 0 ) {
            ok = false;
        }
    }
    store->close();
    delete store;
    if ( ! ok ) {
//        QApplication::restoreOverrideCursor();
        return false;
    }
    return true;
}

Project *Part::loadWorkPackageXML( Project &project, QIODevice *, const KoXmlDocument &document, const KUrl &url )
{
    QString value;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        qDebug() << "No mime type specified!";
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.kplato.work" ) {
        qDebug() << "Unknown mime type " << value;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato.work, got %1", value ) );
        return false;
    }
    QString syntaxVersion = plan.attribute( "version", KPLATOWORK_FILE_SYNTAX_VERSION );
    m_xmlLoader.setWorkVersion( syntaxVersion );
    if ( syntaxVersion > KPLATOWORK_FILE_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                0, i18n( "This document was created with a newer version of KPlatoWork (syntax version: %1)\n"
                "Opening it in this version of KPlatoWork will lose some information.", syntaxVersion ),
                i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }
    m_xmlLoader.setVersion( plan.attribute( "kplato-version", KPLATO_FILE_SYNTAX_VERSION ) );

#ifdef KOXML_USE_QDOM
    int numNodes = plan.childNodes().count();
#else
    int numNodes = plan.childNodesCount();
#endif

    bool ok = true;
    QString timeTag;
    m_xmlLoader.startLoad();
    Project *proj = new Project();
    Package *package = new Package();
    package->project = proj;
    KoXmlNode n = plan.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "project" ) {
            m_xmlLoader.setProject( proj );
            ok = proj->load( e, m_xmlLoader );
            if ( ! ok ) {
                m_xmlLoader.addMsg( XMLLoaderObject::Errors, "Loading of work package failed" );
                //TODO add some ui here
            }
        } else if ( e.tagName() == "workpackage" ) {
            timeTag = e.attribute( "time-tag" );
            package->ownerId = e.attribute( "owner-id" );
            package->ownerName = e.attribute( "owner" );
            KoXmlElement elem;
            forEachElement( elem, e ) {
                if ( elem.tagName() != "settings" ) {
                    continue;
                }
                package->settings.usedEffort = (bool)elem.attribute( "used-effort" ).toInt();
                package->settings.progress = (bool)elem.attribute( "progress" ).toInt();
                package->settings.remainingEffort = (bool)elem.attribute( "remaining-effort" ).toInt();
                package->settings.documents = (bool)elem.attribute( "documents" ).toInt();
            }
        }
    }
    WorkPackage &wp = static_cast<Task*>( proj->childNode( 0 ) )->workPackage();
    if ( wp.ownerId().isEmpty() ) {
        wp.setOwnerId( package->ownerId );
        wp.setOwnerName( package->ownerName );
    }

    m_xmlLoader.stopLoad();

    if ( ok && proj->id() == project.id() && proj->childNode( 0 ) ) {
        ok = project.nodeDict().contains( proj->childNode( 0 )->id() );
        if ( ok && m_mergedPackages.contains( timeTag ) ) {
            qDebug()<<"loadWorkPackageXML:"<<"Already merged:"<<timeTag<<proj->name()<<proj->childNode( 0 )->name();
            ok = false; // already merged
        }
        if ( ok && ! timeTag.isEmpty() && ! m_mergedPackages.contains( timeTag ) ) {
            m_mergedPackages[ timeTag ] = proj; // register this for next time
            qDebug()<<"loadWorkPackageXML:"<<"New package:"<<timeTag<<proj->name()<<proj->childNode( 0 )->name();
        }
        if ( ok && timeTag.isEmpty() ) {
            kWarning()<<"Work package is not time tagged";
            qDebug()<<"loadWorkPackageXML:"<<"Old package format:"<<timeTag<<proj->name()<<proj->childNode( 0 )->name();
            ok = false;
        }
    }
    if ( ! ok ) {
        delete proj;
        delete package;
        return 0;
    }
    m_workpackages.insert( package, url );
    return proj;
}

void Part::checkForWorkPackages()
{
    //qDebug()<<"checkForWorkPackages:";
    if ( ! isReadWrite() || ! m_config.checkForWorkPackages() || m_config.retrieveUrl().isEmpty() || m_project == 0 || m_project->numChildren() == 0 ) {
        //qDebug()<<"checkForWorkPackages: idle:"<<isReadWrite()<<m_config.checkForWorkPackages()<<m_config.retrieveUrl()<<m_project <<(m_project?m_project->numChildren():0);
        QTimer::singleShot ( 10000, this, SLOT( checkForWorkPackages() ) );
        return;
    }
    QDir dir( m_config.retrieveUrl().path(), "*.kplatowork" );
    m_infoList = dir.entryInfoList( QDir::Files | QDir::Readable, QDir::Time );
    checkForWorkPackage();
    return;
}

void Part::checkForWorkPackage()
{
    qDebug()<<"checkForWorkPackage: files ="<<m_infoList.count();
    if ( ! m_infoList.isEmpty() ) {
        loadWorkPackage( *m_project, KUrl( m_infoList.takeLast().absoluteFilePath() ) );
        if ( ! m_infoList.isEmpty() ) {
            QTimer::singleShot ( 0, this, SLOT( checkForWorkPackage() ) );
            return;
        }
        // all files read
        // remove other projects
        QMutableMapIterator<Package*, KUrl> it( m_workpackages );
        while ( it.hasNext() ) {
            it.next();
            if ( it.key()->project->id() != m_project->id() ) {
                delete it.key()->project;
                delete it.key();
                it.remove();
            }
        }
        // Merge our workpackages
        if ( ! m_workpackages.isEmpty() ) {
            QStringList lst;
            foreach ( Package *p, m_workpackages.keys() ) {
                lst << QString( "%1: %2" ).arg( static_cast<Task*>( p->project->childNode( 0 ) )->workPackage().ownerName() ).arg( p->project->childNode( 0 )->name() );
            }
            int r = KMessageBox::questionYesNoList( 0, "New work packages detected. Merge data with existing tasks?", lst );
            if ( r == KMessageBox::Yes ) {
                mergeWorkPackages();
            }
            qDeleteAll( m_workpackages.keys() );
            m_workpackages.clear();
        }
    }
    qDebug()<<"checkForWorkPackage: start again:";
    QTimer::singleShot ( 10000, this, SLOT( checkForWorkPackages() ) );
}

void Part::mergeWorkPackages()
{
    foreach ( const Package *p, m_workpackages.keys() ) {
        mergeWorkPackage( p );
    }
}

void Part::mergeWorkPackage( const Package *package )
{
    const Project &proj = *(package->project);
    if ( proj.id() == m_project->id() && proj.childNode( 0 ) ) {
        const Task *from = qobject_cast<const Task*>( proj.childNode( 0 ) );
        Task *to = qobject_cast<Task*>( m_project->findNode( from->id() ) );
        if ( to && from ) {
            mergeWorkPackage( to, from, package );
        }
        if ( KPlatoSettings::leaveFile() ) {
            return;
        }
        QFile file( m_workpackages[ const_cast<Package*>(package) ].path() );
        if ( ! file.exists() ) {
            return;
        }
        if ( KPlatoSettings::deleteFile() ) {
            qDebug()<<"mergeWorkPackage: remove file"<<file.fileName();
            file.remove();
        } else if ( KPlatoSettings::saveFile() && ! KPlatoSettings::saveUrl(). isEmpty() ) {
            QDir dir( KPlatoSettings::saveUrl().path() );
            if ( ! dir.exists() ) {
                if ( ! dir.mkpath( dir.path() ) ) {
                    //TODO message
                    qDebug()<<"mergeWorkPackage: could not create directory:"<<dir.path();
                    return;
                }
            }
            QFileInfo from( file );
            QString name = KPlatoSettings::saveUrl().path() + '/' + from.fileName();
            qDebug()<<"mergeWorkPackage: rename file"<<file.fileName();
            if ( file.rename( name ) ) {
                qDebug()<<"mergeWorkPackage: Saved"<<file.fileName()<<"to"<<name;
                return;
            }
            name = KPlatoSettings::saveUrl().path() + '/';
            name += from.completeBaseName() + "-%1";
            if ( ! from.suffix().isEmpty() ) {
                name += '.' + from.suffix();
            }
            int i = 0;
            bool ok = false;
            while ( ! ok && i < 1000 ) {
                ++i;
                ok = QFile::rename( file.fileName(), name.arg( i ) );
                //qDebug()<<"mergeWorkPackage: tried to save"<<file.fileName()<<"to"<<name.arg(i);
            }
            if ( ! ok ) {
                //TODO message
                qDebug()<<"mergeWorkPackage: Failed to save"<<file.fileName();
            } else {
                qDebug()<<"mergeWorkPackage: Saved"<<file.fileName()<<"to"<<name.arg(i);
            }
        }
    }
}

void Part::mergeWorkPackage( Task *to, const Task *from, const Package *package )
{
    MacroCommand *cmd = new MacroCommand( "Merge workpackage" );
    Completion &org = to->completion();
    const Completion &curr = from->completion();
/*    if ( org.entrymode() != curr.entrymode() ) {
        cmd->addCommand( new ModifyCompletionEntrymodeCmd(org, curr.entrymode() ) );
    }*/
    if ( package->settings.progress ) {
        if ( org.isStarted() != curr.isStarted() ) {
            cmd->addCommand( new ModifyCompletionStartedCmd(org, curr.isStarted() ) );
        }
        if ( org.isFinished() != curr.isFinished() ) {
            cmd->addCommand( new ModifyCompletionFinishedCmd(org, curr.isFinished() ) );
        }
        if ( org.startTime() != curr.startTime() ) {
            cmd->addCommand( new ModifyCompletionStartTimeCmd(org, curr.startTime().dateTime() ) );
        }
        if ( org.finishTime() != curr.finishTime() ) {
            cmd->addCommand( new ModifyCompletionFinishTimeCmd(org, curr.finishTime().dateTime() ) );
        }
    }
    QList<QDate> orgdates = org.entries().keys();
    QList<QDate> currdates = curr.entries().keys();
    foreach ( const QDate &d, orgdates ) {
        if ( currdates.contains( d ) ) {
            if ( curr.entry( d ) == org.entry( d ) ) {
                continue;
            }
            kDebug()<<"modify entry "<<d;
            Completion::Entry *e = new Completion::Entry( *( curr.entry( d ) ) );
            cmd->addCommand( new ModifyCompletionEntryCmd(org, d, e ) );
        } else {
            kDebug()<<"remove entry "<<d;
            cmd->addCommand( new RemoveCompletionEntryCmd(org, d ) );
        }
    }
    foreach ( const QDate &d, currdates ) {
        if ( ! orgdates.contains( d ) ) {
            Completion::Entry *e = new Completion::Entry( * ( curr.entry( d ) ) );
            kDebug()<<"add entry "<<d<<e;
            cmd->addCommand( new AddCompletionEntryCmd(org, d, e ) );
        }
    }
    if ( package->settings.usedEffort ) {
        Resource *r = m_project->findResource( package->ownerId );
        if ( r == 0 ) {
            KMessageBox::error( 0, i18n( "The package owner '%1' is not a resource in this project. You must handle this manually.", package->ownerName ) );
            delete cmd;
            return;
        }
        const Completion::ResourceUsedEffortMap &map = curr.usedEffortMap();
        foreach ( const Resource *res, map.keys() ) {
            if ( r->id() != res->id() ) {
                continue;
            }
            Completion::UsedEffort *ue = map[ r ];
            if ( ue == 0 ) {
                break;
            }
            if ( org.usedEffort( r ) == 0 || *ue != *(org.usedEffort( r )) ) {
                cmd->addCommand( new AddCompletionUsedEffortCmd( org, r, new Completion::UsedEffort( *ue ) ) );
            }
        }
    }
    if ( package->settings.documents ) {
        //TODO
    }
    if ( cmd->isEmpty() ) {
        KMessageBox::information( 0, i18n( "Nothing to save from this package" ) );
    }
    // add a copy to our tasks list of transmitted packages
    WorkPackage *wp = new WorkPackage( from->workPackage() );
    wp->setParentTask( to );
    if ( ! wp->transmitionTime().isValid() ) {
        wp->setTransmitionTime( DateTime::currentLocalDateTime() );
    }
    wp->setTransmitionStatus( WorkPackage::TS_Receive );
    cmd->addCommand( new WorkPackageAddCmd( m_project, to, wp ) );
    qDebug()<<"mergeWorkPackage:"<<from->name()<<to->name()<<cmd->isEmpty();
    if ( cmd->isEmpty() ) {
        KMessageBox::information( 0, i18n( "Nothing to save from this package" ) );
        delete cmd;
    } else {
        addCommand( cmd );
    }
}

void Part::paintContent( QPainter &, const QRect &)
{
    // Don't embed this app!!!
}

void Part::slotViewDestroyed()
{
}

void Part::activate( QWidget *w )
{
    if ( manager() )
        manager()->setActivePart( this, w );
}

void Part::openTemplate( const KUrl& url )
{
    //kDebug()<<url;
    // HACK because we can't really reimplemt openTemplate() (private methods)
    m_loadingTemplate = true;
    KoDocument::openTemplate( url );
    m_loadingTemplate = false;
}

bool Part::completeLoading( KoStore *store )
{
    // If we get here the new project is loaded and set
    if ( m_loadingTemplate ) {
        //kDebug()<<"Loading template, generate unique ids";
        m_project->generateUniqueIds();
    } else if ( isImporting() ) {
        //kDebug()<<"Importing, generate unique node ids";
        m_project->generateUniqueNodeIds();
    }
    if ( store == 0 ) {
        // can happen if loading a template
        kDebug()<<"No store";
        return true; // continue anyway
    }
    delete m_context;
    m_context = new Context();
    KoXmlDocument doc;
    if ( loadAndParse( store, "context.xml", doc ) ) {
        store->close();
        m_context->load( doc );
    } else kWarning()<<"No context";
    return true;
}

bool Part::completeSaving( KoStore *store )
{
    delete m_context;
    m_context = 0;
    // Seems like a hack, but imo the best to do
    View *view = dynamic_cast<View*>( views().value( 0 ) );
    if ( view ) {
        m_context = new Context();
        m_context->save( view );
        if ( store->open( "context.xml" ) ) {
            QDomDocument doc = m_context->save( view );
            KoStoreDevice dev( store );

            QByteArray s = doc.toByteArray(); // this is already Utf8!
            (void)dev.write( s.data(), s.size() );
            (void)store->close();
        }
    }
    return true;
}

bool Part::loadAndParse(KoStore* store, const QString& filename, KoXmlDocument& doc)
{
    //kDebug() << "oldLoadAndParse: Trying to open " << filename;

    if (!store->open(filename))
    {
        kWarning() << "Entry " << filename << " not found!";
//        d->lastErrorMessage = i18n( "Could not find %1",filename );
        return false;
    }
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    bool ok = doc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn );
    if ( !ok )
    {
        kError() << "Parsing error in " << filename << "! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg;
/*        d->lastErrorMessage = i18n( "Parsing error in %1 at line %2, column %3\nError message: %4"
                              ,filename  ,errorLine, errorColumn ,
                              QCoreApplication::translate("QXml", errorMsg.toUtf8(), 0,
                                  QCoreApplication::UnicodeUTF8));*/
        store->close();
        return false;
    }
    kDebug() << "File " << filename << " loaded and parsed";
    return true;
}

void Part::insertFile( const QString &filename, Node *parent, Node *after )
{
    qDebug()<<"Part::insertFile:"<<filename<<parent->name()<<(after?after->name():"nil");
    Part part;
    if ( part.openUrl( KUrl( filename ) ) ) {
        part.setReadWrite( false );
        Project &p = part.getProject();
        qDebug()<<"Part::insertFile:"<<p.name();
        insertProject( p, parent, after );
    }
}

bool Part::insertProject( Project &project, Node *parent, Node *after )
{
    qDebug()<<"Part::insertProject:";
    // make sure node ids in new project is unique also in old project
    QList<QString> existingIds = m_project->nodeDict().keys();
    foreach ( Node *n, project.allNodes() ) {
        if ( ! n->id().isEmpty() && ! m_project->findNode( n->id() ) ) {
            continue; // id is ok to use in m_project
        }
        bool res = n->setId( project.uniqueNodeId( existingIds ) );
        Q_ASSERT( res );
    }
    qDebug()<<"Part::insertProject:"<<project.childNodeIterator();
    MacroCommand *m = new InsertProjectCmd( project, parent==0?m_project:parent, after, i18n( "Insert project nodes" ) );
    if ( m->isEmpty() ) {
        delete m;
    } else {
        addCommand( m );
    }
    return true;
}


}  //KPlato namespace

#include "kptpart.moc"
