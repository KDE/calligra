/* This file is part of the KDE project
 Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
 Copyright (C) 2004, 2010 Dag Andersen <danders@get2net.dk>
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
#include "plansettings.h"
#include "kpttask.h"
#include "KPlatoXmlLoader.h"
#include "kptpackage.h"

//#include "KDGanttViewTaskLink.h"

#include <KoZoomHandler.h>
#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoStoreDevice.h>
#include <KoOdfReadStore.h>
#include <KoUpdater.h>
#include <KoProgressUpdater.h>

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
#include <KTemporaryFile>
#include <KoGlobal.h>
#include <kio/global.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/copyjob.h>

namespace KPlato
{

Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ),
        m_project( 0 ), m_parentWidget( parentWidget ),
        m_context( 0 ), m_xmlLoader(),
        m_loadingTemplate( false ),
        m_viewlistModified( false )
{
    setComponentData( Factory::global(), false ); // Do not load plugins now (the view will load them)
    setTemplateType( "plan_template" );
    m_config.setReadWrite( isReadWrite() || !isEmbedded() );
    // Add library translation files
    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "planlibs" );
        locale->insertCatalog( "kdgantt" );
        locale->insertCatalog( "kabc" );
        locale->insertCatalog( "timezones4" );

        m_config.setLocale( new KLocale( *locale ) );
    }

    loadSchedulerPlugins();

    setProject( new Project( m_config ) ); // after config & plugins are loaded
    m_project->setId( m_project->uniqueNodeId() );
    m_project->registerNodeId( m_project ); // register myself

    QTimer::singleShot ( 5000, this, SLOT( checkForWorkPackages() ) );

    connect(this, SIGNAL(modified(bool)), SLOT(slotModified(bool)));
}


Part::~Part()
{
    qDeleteAll( m_schedulerPlugins );
    if ( m_project ) {
        m_project->deref(); // deletes if last user
    }
    qDeleteAll( m_mergedPackages );
    delete m_context;
}

void Part::setReadWrite( bool rw )
{
    m_config.setReadWrite( rw || !isEmbedded() ); // embedded in calligra doc
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
    // syncronize view selector
    View *view = dynamic_cast<View*>( views().value( 0 ) );
    if ( view && m_context ) {
        QDomDocument doc = m_context->save( view );
        m_context->setContent( doc.toString() );
    }
    view = new View( this, parent );
    connect( view, SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
    return view;
}

bool Part::loadOdf( KoOdfReadStore &odfStore )
{
    kWarning()<< "OpenDocument not supported, let's try native xml format";
    return loadXML( odfStore.contentDoc(), 0 ); // We have only one format, so try to load that!
}

bool Part::loadXML( const KoXmlDocument &document, KoStore* )
{
    QPointer<KoUpdater> updater;
    if (progressUpdater()) {
        updater = progressUpdater()->startSubtask(1, "Plan::Part::loadXML");
        updater->setProgress(0);
    }

    QString value;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!";
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    }
    if ( value == "application/x-vnd.kde.kplato" ) {
        if (updater) {
            updater->setProgress(5);
        }
        m_xmlLoader.setMimetype( value );
        QString message;
        Project *newProject = new Project( m_config );
        KPlatoXmlLoader loader( m_xmlLoader, newProject );
        bool ok = loader.load( plan );
        if ( ok ) {
            setProject( newProject );
            setModified( false );
        } else {
            setErrorMessage( loader.errorMessage() );
            delete newProject;
        }
        if (updater) {
            updater->setProgress(100); // the rest is only processing, not loading
        }
        emit changed();
        return ok;
    }
    if ( value != "application/x-vnd.kde.plan" ) {
        kError() << "Unknown mime type " << value;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.plan, got %1", value ) );
        return false;
    }
    QString syntaxVersion = plan.attribute( "version", PLAN_FILE_SYNTAX_VERSION );
    m_xmlLoader.setVersion( syntaxVersion );
    if ( syntaxVersion > PLAN_FILE_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of Plan (syntax version: %1)\n"
                               "Opening it in this version of Plan will lose some information.", syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }
    if (updater) updater->setProgress(5);
/*
#ifdef KOXML_USE_QDOM
    int numNodes = plan.childNodes().count();
#else
    int numNodes = plan.childNodesCount();
#endif
*/
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
            Project *newProject = new Project( m_config );
            m_xmlLoader.setProject( newProject );
            if ( newProject->load( e, m_xmlLoader ) ) {
                if ( newProject->id().isEmpty() ) {
                    newProject->setId( newProject->uniqueNodeId() );
                    newProject->registerNodeId( newProject );
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

    if (updater) updater->setProgress(100); // the rest is only processing, not loading

    setModified( false );
    emit changed();
    return true;
}

QDomDocument Part::saveXML()
{
    kDebug();
    QDomDocument document( "plan" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "plan" );
    doc.setAttribute( "editor", "Plan" );
    doc.setAttribute( "mime", "application/x-vnd.kde.plan" );
    doc.setAttribute( "version", PLAN_FILE_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    m_project->save( doc );

    return document;
}

QDomDocument Part::saveWorkPackageXML( const Node *node, long id, Resource *resource )
{
    kDebug();
    QDomDocument document( "plan" );

    document.appendChild( document.createProcessingInstruction(
                "xml",
    "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "planwork" );
    doc.setAttribute( "editor", "Plan" );
    doc.setAttribute( "mime", "application/x-vnd.kde.plan.work" );
    doc.setAttribute( "version", PLANWORK_FILE_SYNTAX_VERSION );
    doc.setAttribute( "plan-version", PLAN_FILE_SYNTAX_VERSION );
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

bool Part::saveWorkPackageToStream( QIODevice *dev, const Node *node, long id, Resource *resource )
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

    QByteArray mimeType = "application/x-vnd.kde.plan.work";
    kDebug() <<"MimeType=" << mimeType;

    KoStore *store = KoStore::createStore( file, KoStore::Write, mimeType, backend );
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

bool Part::saveWorkPackageUrl( const KUrl &_url, const Node *node, long id, Resource *resource )
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
    kDebug()<<url;
    if ( ! url.isLocalFile() ) {
        kDebug()<<"TODO: download if url not local";
        return false;
    }
    KoStore *store = KoStore::createStore( url.path(), KoStore::Read, "", KoStore::Auto );
    if ( store->bad() ) {
//        d->lastErrorMessage = i18n( "Not a valid Calligra file: %1", file );
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
    Package *package = 0;
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
        package = loadWorkPackageXML( project, store->device(), doc, url );
        ok = (bool)package;
    }
    store->close();
    if ( ok && package && package->settings.documents ) {
        ok = extractFiles( store, package );
    }
    delete store;
    if ( ! ok ) {
//        QApplication::restoreOverrideCursor();
        return false;
    }
    return true;
}

Package *Part::loadWorkPackageXML( Project &project, QIODevice *, const KoXmlDocument &document, const KUrl &url )
{
    QString value;
    bool ok = true;
    Project *proj = 0;
    Package *package = 0;
    QString timeTag;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kDebug() << "No mime type specified!";
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value == "application/x-vnd.kde.kplato.work" ) {
        m_xmlLoader.setMimetype( value );
        m_xmlLoader.setWorkVersion( plan.attribute( "version", "0.0.0" ) );
        proj = new Project( m_config );
        KPlatoXmlLoader loader( m_xmlLoader, proj );
        ok = loader.loadWorkpackage( plan );
        if ( ! ok ) {
            setErrorMessage( loader.errorMessage() );
            delete proj;
            return false;
        }
        package = loader.package();
        timeTag = loader.timeTag();
    } else if ( value != "application/x-vnd.kde.plan.work" ) {
        kDebug() << "Unknown mime type " << value;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.plan.work, got %1", value ) );
        return false;
    } else {
        QString syntaxVersion = plan.attribute( "version", "0.0.0" );
        m_xmlLoader.setWorkVersion( syntaxVersion );
        if ( syntaxVersion > PLANWORK_FILE_SYNTAX_VERSION ) {
            int ret = KMessageBox::warningContinueCancel(
                    0, i18n( "This document was created with a newer version of PlanWork (syntax version: %1)\n"
                    "Opening it in this version of PlanWork will lose some information.", syntaxVersion ),
                    i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
            if ( ret == KMessageBox::Cancel ) {
                setErrorMessage( "USER_CANCELED" );
                return false;
            }
        }
        m_xmlLoader.setVersion( plan.attribute( "plan-version", PLAN_FILE_SYNTAX_VERSION ) );
        m_xmlLoader.startLoad();
        proj = new Project();
        package = new Package();
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
        if ( proj->numChildren() > 0 ) {
            package->task = static_cast<Task*>( proj->childNode( 0 ) );
            WorkPackage &wp = package->task->workPackage();
            if ( wp.ownerId().isEmpty() ) {
                wp.setOwnerId( package->ownerId );
                wp.setOwnerName( package->ownerName );
            }
            kDebug()<<"Task set:"<<package->task->name();
        }
        m_xmlLoader.stopLoad();
    }
    if ( ok && proj->id() == project.id() && proj->childNode( 0 ) ) {
        ok = project.nodeDict().contains( proj->childNode( 0 )->id() );
        if ( ok && m_mergedPackages.contains( timeTag ) ) {
            ok = false; // already merged
        }
        if ( ok && ! timeTag.isEmpty() && ! m_mergedPackages.contains( timeTag ) ) {
            m_mergedPackages[ timeTag ] = proj; // register this for next time
        }
        if ( ok && timeTag.isEmpty() ) {
            kWarning()<<"Work package is not time tagged";
            ok = false;
        }
    }
    if ( ! ok ) {
        delete proj;
        delete package;
        return 0;
    }
    Q_ASSERT( package );
    m_workpackages.insert( package, url );
    return package;
}

bool Part::extractFiles( KoStore *store, Package *package )
{
    if ( package->task == 0 ) {
        kError()<<"No task!";
        return false;
    }
    foreach ( Document *doc, package->task->documents().documents() ) {
        if ( ! doc->isValid() || doc->type() != Document::Type_Product || doc->sendAs() != Document::SendAs_Copy ) {
            continue;
        }
        if ( ! extractFile( store, package, doc ) ) {
            return false;
        }
    }
    return true;
}

bool Part::extractFile( KoStore *store, Package *package, const Document *doc )
{
    KTemporaryFile tmpfile;
    if ( ! tmpfile.open() ) {
        kError()<<"Failed to open temporary file";
        return false;
    }
    if ( ! store->extractFile( doc->url().fileName(), tmpfile.fileName() ) ) {
        kError()<<"Failed to extract file:"<<doc->url().fileName()<<"to:"<<tmpfile.fileName();
        return false;
    }
    package->documents.insert( tmpfile.fileName(), doc->url() );
    tmpfile.setAutoRemove( false );
    return true;
}

void Part::checkForWorkPackages()
{
    if ( ! isReadWrite() || ! m_config.checkForWorkPackages() || m_config.retrieveUrl().isEmpty() || m_project == 0 || m_project->numChildren() == 0 ) {
        QTimer::singleShot ( 10000, this, SLOT( checkForWorkPackages() ) );
        return;
    }
    QDir dir( m_config.retrieveUrl().path(), "*.planwork" );
    m_infoList = dir.entryInfoList( QDir::Files | QDir::Readable, QDir::Time );
    checkForWorkPackage();
    return;
}

void Part::checkForWorkPackage()
{
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
            foreach ( const Package *p, m_workpackages.keys() ) {
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
            file.remove();
        } else if ( KPlatoSettings::saveFile() && ! KPlatoSettings::saveUrl().isEmpty() ) {
            QDir dir( KPlatoSettings::saveUrl().path() );
            if ( ! dir.exists() ) {
                if ( ! dir.mkpath( dir.path() ) ) {
                    //TODO message
                    kDebug()<<"Could not create directory:"<<dir.path();
                    return;
                }
            }
            QFileInfo from( file );
            QString name = KPlatoSettings::saveUrl().path() + '/' + from.fileName();
            if ( file.rename( name ) ) {
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
            }
            if ( ! ok ) {
                //TODO message
                kDebug()<<"mergeWorkPackage: Failed to save"<<file.fileName();
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
            cmd->addCommand( new ModifyCompletionStartTimeCmd(org, curr.startTime() ) );
        }
        if ( org.finishTime() != curr.finishTime() ) {
            cmd->addCommand( new ModifyCompletionFinishTimeCmd(org, curr.finishTime() ) );
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
    bool docsaved = false;
    if ( package->settings.documents ) {
        //TODO: handle remote files
        QMap<QString, KUrl>::const_iterator it = package->documents.constBegin();
        QMap<QString, KUrl>::const_iterator end = package->documents.constEnd();
        for ( ; it != end; ++it ) {
            KUrl src( it.key() );
            KIO::Job *job = KIO::move( src, it.value(), KIO::Overwrite );
            if ( KIO::NetAccess::synchronousRun( job, 0 ) ) {
                docsaved = true;
                //TODO: async
            }
        }
    }
    if ( ! docsaved && cmd->isEmpty() ) {
        KMessageBox::information( 0, i18n( "Nothing to save from this package" ) );
    }
    // add a copy to our tasks list of transmitted packages
    WorkPackage *wp = new WorkPackage( from->workPackage() );
    wp->setParentTask( to );
    if ( ! wp->transmitionTime().isValid() ) {
        wp->setTransmitionTime( DateTime::currentDateTime() );
    }
    wp->setTransmitionStatus( WorkPackage::TS_Receive );
    cmd->addCommand( new WorkPackageAddCmd( m_project, to, wp ) );
    addCommand( cmd );
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

void Part::openTemplate( const KUrl &url )
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
        m_project->setConstraintStartTime( KDateTime( KDateTime::currentLocalDateTime().date(), QTime( 0, 0, 0 ) ) );
        m_project->setConstraintEndTime( m_project->constraintStartTime().addYears( 2 ) );
    } else if ( isImporting() ) {
        // NOTE: I don't think this is a good idea.
        // Let the filter generate ids for non-plan files.
        // If the user wants to create a new project from an old one,
        // he should use Tools -> Insert Project File

        //m_project->generateUniqueNodeIds();
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
    // Seems like a hack, but imo the best to do
    View *view = dynamic_cast<View*>( views().value( 0 ) );
    if ( view ) {
        if ( store->open( "context.xml" ) ) {
            if ( m_context == 0 ) m_context = new Context();
            QDomDocument doc = m_context->save( view );

            KoStoreDevice dev( store );
            QByteArray s = doc.toByteArray(); // this is already Utf8!
            (void)dev.write( s.data(), s.size() );
            (void)store->close();

            m_viewlistModified = false;
            emit viewlistModified( false );
        }
    }
    return true;
}

bool Part::loadAndParse(KoStore *store, const QString &filename, KoXmlDocument &doc)
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
    Part *part = new Part();
    part->disconnect(); // part shall not handle feedback from openUrl()
    part->setAutoSave( 0 ); //disable
    part->m_insertFileInfo.url = filename;
    part->m_insertFileInfo.parent = parent;
    part->m_insertFileInfo.after = after;
    connect(part, SIGNAL(completed()), SLOT(insertFileCompleted()));
    connect(part, SIGNAL(canceled(const QString&)), SLOT(insertFileCancelled(const QString&)));
    connect(part, SIGNAL(started(KIO::Job*)), SLOT(slotStarted(KIO::Job*)));

    part->openUrl( KUrl( filename ) );
}

void Part::insertFileCompleted()
{
    Part *part = qobject_cast<Part*>( sender() );
    if ( part ) {
        Project &p = part->getProject();
        insertProject( p, part->m_insertFileInfo.parent, part->m_insertFileInfo.after );
        part->deleteLater();
    }
}

void Part::insertFileCancelled( const QString &error )
{
    if ( ! error.isEmpty() ) {
        KMessageBox::error( 0, error );
    }
    Part *part = qobject_cast<Part*>( sender() );
    if ( part ) {
        part->deleteLater();
    }
}

bool Part::insertProject( Project &project, Node *parent, Node *after )
{
    // make sure node ids in new project is unique also in old project
    QList<QString> existingIds = m_project->nodeDict().keys();
    foreach ( Node *n, project.allNodes() ) {
        QString oldid = n->id();
        n->setId( project.uniqueNodeId( existingIds ) );
        project.removeId( oldid ); // remove old id
        project.registerNodeId( n ); // register new id
    }
    MacroCommand *m = new InsertProjectCmd( project, parent==0?m_project:parent, after, i18nc( "(qtundo-format)", "Insert project" ) );
    if ( m->isEmpty() ) {
        delete m;
    } else {
        addCommand( m );
    }
    return true;
}

void Part::insertViewListItem( View *view, const ViewListItem *item, const ViewListItem *parent, int index )
{
    foreach ( KoView *v, views() ) {
        View *vv = dynamic_cast<View*>( v );
        if ( vv == 0 || vv == view ) {
            continue;
        }
        vv->addViewListItem( item, parent, index );
    }
    setModified( true );
    m_viewlistModified = true;
}

void Part::removeViewListItem( View *view, const ViewListItem *item )
{
    foreach ( KoView *v, views() ) {
        View *vv = dynamic_cast<View*>( v );
        if ( vv == 0 || vv == view ) {
            continue;
        }
        vv->removeViewListItem( item );
    }
    setModified( true );
    m_viewlistModified = true;
}

void Part::slotModified( bool mod )
{
    if ( ! mod && m_viewlistModified ) {
        setModified( true );
    }
}

void Part::viewlistModified()
{
    if ( ! m_viewlistModified ) {
        m_viewlistModified = true;
        setModified( true );
    }
}

}  //KPlato namespace

#include "kptpart.moc"
