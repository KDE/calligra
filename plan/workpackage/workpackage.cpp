/* This file is part of the KDE project
 Copyright (C) 2009, 2012 Dag Andersen <danders@get2net.dk>

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
 Boston, MA 02110-1301, USA.
*/

#include "workpackage.h"

#include "KPlatoXmlLoader.h" //NOTE: this file should probably be moved

#include "part.h"
#include "kptglobal.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kptdocuments.h"
#include "kptcommand.h"
#include "kptxmlloaderobject.h"
#include "kptconfigbase.h"
#include "kptcommonstrings.h"

#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoStoreDevice.h>

#include <QDir>
#include <QTimer>

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kmimetype.h>
#include <KDateTime>

#include <QDomDocument>

#include "debugarea.h"

using namespace KPlato;

namespace KPlatoWork
{

WorkPackage::WorkPackage( bool fromProjectStore )
    : m_project( new Project() ),
    m_fromProjectStore( fromProjectStore ),
    m_modified( false)
{
    m_config.setLocale( new KLocale( *( KGlobal::locale() ) ) );
    m_project->setConfig( &m_config );
}

WorkPackage::WorkPackage( Project *project, bool fromProjectStore )
    : m_project( project ),
    m_fromProjectStore( fromProjectStore ),
    m_modified( false)
{
    Q_ASSERT( project );
    Q_ASSERT ( project->childNode( 0 ) );

    m_config.setLocale( new KLocale( *( KGlobal::locale() ) ) );
    m_project->setConfig( &m_config );

    if ( ! project->scheduleManagers().isEmpty() ) {
        // should be only one manager
        project->setCurrentSchedule( m_project->scheduleManagers().first()->scheduleId() );
    }
    connect( project, SIGNAL( projectChanged() ), this, SLOT( projectChanged() ) );

}

WorkPackage::~WorkPackage()
{
    delete m_project;
    qDeleteAll( m_childdocs );
}

void WorkPackage::setSettings( const WorkPackageSettings &settings )
{
    if ( m_settings != settings ) {
        m_settings = settings;
        setModified( true );
    }
}

//TODO find a way to know when changes are undone
void WorkPackage::projectChanged()
{
    kDebug(planworkDbg());
    setModified( true );
}

bool WorkPackage::addChild( Part */*part*/, const Document *doc )
{
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
    if ( ! m_childdocs.contains( ch ) ) {
        m_childdocs.append( ch );
        connect( ch, SIGNAL( fileModified( bool ) ), this, SLOT( slotChildModified( bool ) ) );
    }
    return true;
}

void WorkPackage::slotChildModified( bool mod )
{
    kDebug(planworkDbg())<<mod;
    emit modified( isModified() );
    emit saveWorkPackage( this );
}

void WorkPackage::removeChild( DocumentChild *child )
{
    disconnect( child, SIGNAL( fileModified( bool ) ), this, SLOT( slotChildModified( bool ) ) );

    int i = m_childdocs.indexOf( child );
    if ( i != -1 ) {
        // TODO: process etc
        m_childdocs.removeAt( i );
        delete child;
    } else {
        kWarning()<<"Could not find document child";
    }
}

bool WorkPackage::contains( const Document *doc ) const
{
    return node() ? node()->documents().contains( doc ) : false;
}

DocumentChild *WorkPackage::findChild( const Document *doc ) const
{
    foreach ( DocumentChild *c, m_childdocs ) {
        if ( c->doc() == doc ) {
            return c;
        }
    }
    return 0;
}

bool WorkPackage::loadXML( const KoXmlElement &element, XMLLoaderObject &status )
{
    bool ok = false;
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        kDebug(planworkDbg())<<e.tagName();
        if ( e.tagName() == "project" ) {
            status.setProject( m_project );
            kDebug(planworkDbg())<<"loading new project";
            if ( ! ( ok = m_project->load( e, status ) ) ) {
                status.addMsg( XMLLoaderObject::Errors, "Loading of work package failed" );
                KMessageBox::error( 0, i18n( "Failed to load project: %1" , m_project->name() ) );
            }
        }
    }
    if ( ok ) {
        KoXmlNode n = element.firstChild();
        for ( ; ! n.isNull(); n = n.nextSibling() ) {
            if ( ! n.isElement() ) {
                continue;
            }
            KoXmlElement e = n.toElement();
            kDebug(planworkDbg())<<e.tagName();
            if ( e.tagName() == "workpackage" ) {
                Task *t = static_cast<Task*>( m_project->childNode( 0 ) );
                t->workPackage().setOwnerName( e.attribute( "owner" ) );
                t->workPackage().setOwnerId( e.attribute( "owner-id" ) );

                Resource *r = m_project->findResource( t->workPackage().ownerId() );
                if ( r == 0 ) {
                    kDebug(planworkDbg())<<"Cannot find resource id!!"<<t->workPackage().ownerId()<<t->workPackage().ownerName();
                }
                kDebug(planworkDbg())<<"is this me?"<<t->workPackage().ownerName();
                KoXmlNode ch = e.firstChild();
                for ( ; ! ch.isNull(); ch = ch.nextSibling() ) {
                    if ( ! ch.isElement() ) {
                        continue;
                    }
                    KoXmlElement el = ch.toElement();
                    kDebug(planworkDbg())<<el.tagName();
                    if ( el.tagName() == "settings" ) {
                        m_settings.loadXML( el );
                    }
                }
            }
        }
    }
    if ( ! m_project->scheduleManagers().isEmpty() ) {
        // should be only one manager
        m_project->setCurrentSchedule( m_project->scheduleManagers().first()->scheduleId() );
    }
    return ok;
}

bool WorkPackage::loadKPlatoXML( const KoXmlElement &element, XMLLoaderObject &status )
{
    bool ok = false;
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        kDebug(planworkDbg())<<e.tagName();
        if ( e.tagName() == "project" ) {
            status.setProject( m_project );
            KPlatoXmlLoader loader( status, m_project );
            kDebug(planworkDbg())<<"loading new project";
            if ( ! ( ok = loader.load( m_project, e, status ) ) ) {
                status.addMsg( XMLLoaderObject::Errors, "Loading of work package failed" );
                KMessageBox::error( 0, i18n( "Failed to load project: %1" , m_project->name() ) );
            }
        }
    }
    if ( ok ) {
        KoXmlNode n = element.firstChild();
        for ( ; ! n.isNull(); n = n.nextSibling() ) {
            if ( ! n.isElement() ) {
                continue;
            }
            KoXmlElement e = n.toElement();
            kDebug(planworkDbg())<<e.tagName();
            if ( e.tagName() == "workpackage" ) {
                Task *t = static_cast<Task*>( m_project->childNode( 0 ) );
                t->workPackage().setOwnerName( e.attribute( "owner" ) );
                t->workPackage().setOwnerId( e.attribute( "owner-id" ) );

                Resource *r = m_project->findResource( t->workPackage().ownerId() );
                if ( r == 0 ) {
                    kDebug(planworkDbg())<<"Cannot find resource id!!"<<t->workPackage().ownerId()<<t->workPackage().ownerName();
                }
                kDebug(planworkDbg())<<"is this me?"<<t->workPackage().ownerName();
                KoXmlNode ch = e.firstChild();
                for ( ; ! ch.isNull(); ch = ch.nextSibling() ) {
                    if ( ! ch.isElement() ) {
                        continue;
                    }
                    KoXmlElement el = ch.toElement();
                    kDebug(planworkDbg())<<el.tagName();
                    if ( el.tagName() == "settings" ) {
                        m_settings.loadXML( el );
                    }
                }
            }
        }
    }
    if ( ! m_project->scheduleManagers().isEmpty() ) {
        // should be only one manager
        m_project->setCurrentSchedule( m_project->scheduleManagers().first()->scheduleId() );
    }
    return ok;
}

bool WorkPackage::saveToStream( QIODevice * dev )
{
    QDomDocument doc = saveXML();
    // Save to buffer
    QByteArray s = doc.toByteArray(); // utf8 already
    dev->open(QIODevice::WriteOnly);
    int nwritten = dev->write(s.data(), s.size());
    if (nwritten != (int)s.size())
        kWarning(30003) << "wrote " << nwritten << "- expected" <<  s.size();
    return nwritten == (int)s.size();
}

bool WorkPackage::saveNativeFormat( Part */*part*/, const QString &path )
{
    if ( path.isEmpty() ) {
        KMessageBox::error( 0, i18n("Cannot save to empty filename") );
        return false;
    }
    kDebug(planworkDbg())<<node()->name()<<path;
    KoStore* store = KoStore::createStore(path, KoStore::Write, "application/x-vnd.kde.plan.work", KoStore::Auto );
    if (store->bad()) {
        KMessageBox::error( 0, i18n("Could not create the file for saving") );
        delete store;
        return false;
    }
    if (store->open("root")) {
        KoStoreDevice dev(store);
        if ( ! saveToStream(&dev) || ! store->close() ) {
            kDebug(planworkDbg()) << "saveToStream failed";
            delete store;
            return false;
        }
    } else {
        KMessageBox::error( 0, i18n("Not able to write '%1'. Partition full?", QString("maindoc.xml") ) );
        delete store;
        return false;
    }

    if (!completeSaving(store)) {
        delete store;
        return false;
    }
    if (!store->finalize()) {
        delete store;
        return false;
    }
    // Success
    delete store;
    m_modified = false;
    return true;
}

bool WorkPackage::completeSaving( KoStore *store )
{
    kDebug(planworkDbg());
    KoStore *oldstore = KoStore::createStore( filePath(), KoStore::Read, "", KoStore::Zip );
    if ( oldstore->bad() ) {
        KMessageBox::error( 0, i18n( "Failed to open store:\n %1", filePath() ) );
        return false;
    }
    if (oldstore->hasFile( "documentinfo.xml" ) ) {
        copyFile( oldstore, store, "documentinfo.xml" );
    }
    if (oldstore->hasFile( "preview.png" ) ) {
        copyFile( oldstore, store, "preview.png" );
    }

    // First get all open documents
    kDebug(planworkDbg())<<m_childdocs.count();
    foreach ( DocumentChild *cd, m_childdocs ) {
        if ( ! cd->saveToStore( store ) ) {
        }
    }
    // Then get new files
    foreach ( const Document *doc,  node()->documents().documents() ) {
        if ( m_newdocs.contains( doc ) ) {
            store->addLocalFile( m_newdocs[ doc ].path(), doc->url().fileName() );
            m_newdocs.remove( doc );
            // TODO remove temp file ??
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

QString WorkPackage::fileName( const Part *part ) const
{
    if ( m_project == 0 ) {
        kWarning()<<"No project in this package";
        return QString();
    }
    Node *n = node();
    if ( n == 0 ) {
        kWarning()<<"No node in this project";
        return QString();
    }
    KStandardDirs *sd = part->componentData().dirs();
    QString projectName = m_project->name().remove( ' ' );
    QString path = sd->saveLocation( "projects", projectName + '/' );
    QString wpName = QString( n->name().remove( ' ' ) + '_' + n->id() + ".planwork" );
    return path + wpName;
}

void WorkPackage::removeFile()
{
    QFile file( m_filePath );
    if ( ! file.exists() ) {
        kWarning()<<"No project in this package";
        return;
    }
    file.remove();
}

void WorkPackage::saveToProjects( Part *part )
{
    kDebug(planworkDbg());
    QString path = fileName( part );
    kDebug(planworkDbg())<<node()->name();
    if ( saveNativeFormat( part, path ) ) {
        m_fromProjectStore = true;
        m_filePath = path;
    } else {
        KMessageBox::error( 0, i18n( "Cannot save to projects store:\n%1" , path ) );
    }
    return;
}

bool WorkPackage::isModified() const
{
    if ( m_modified ) {
        return true;
    }
    foreach ( DocumentChild *ch, m_childdocs ) {
        if ( ch->isModified() || ch->isFileModified() ) {
            return true;
        }
    }
    return false;
}

Node *WorkPackage::node() const
{
    return m_project == 0 ? 0 : m_project->childNode( 0 );
}

Task *WorkPackage::task() const
{
    Task *task = qobject_cast<Task*>( node() );
    Q_ASSERT( task );
    return task;
}

bool WorkPackage::removeDocument( Part *part, Document *doc )
{
    Node *n = node();
    if ( n == 0 ) {
        return false;
    }
    part->addCommand( new DocumentRemoveCmd( n->documents(), doc, UndoText::removeDocument() ) );
    return true;
}

bool WorkPackage::copyFile( KoStore *from, KoStore *to, const QString &filename )
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
    kDebug(planworkDbg())<<"Copied file:"<<filename;
    return true;
}

QDomDocument WorkPackage::saveXML()
{
    kDebug(planworkDbg());
    QDomDocument document( "plan-workpackage" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "planwork" );
    doc.setAttribute( "editor", "PlanWork" );
    doc.setAttribute( "mime", "application/x-vnd.kde.plan.work" );
    doc.setAttribute( "version", PLANWORK_FILE_SYNTAX_VERSION );
    doc.setAttribute( "plan-version", PLAN_FILE_SYNTAX_VERSION );
    document.appendChild( doc );

    // Work package info
    QDomElement wp = document.createElement( "workpackage" );
    wp.setAttribute( "time-tag", KDateTime::currentLocalDateTime().toString( KDateTime::ISODate ) );
    m_settings.saveXML( wp );
    Task *t = qobject_cast<Task*>( node() );
    if ( t ) {
        wp.setAttribute( "owner", t->workPackage().ownerName() );
        wp.setAttribute( "owner-id", t->workPackage().ownerId() );
    }
    doc.appendChild( wp );
    m_project->save( doc );
    return document;
}

void WorkPackage::merge( Part *part, const WorkPackage *wp, KoStore *store )
{
    kDebug(planworkDbg());
    const Node *from = wp->node();
    Node *to = node();

    MacroCommand *m = new MacroCommand( i18nc( "(qtundo-format)", "Merge data" ) );
    if ( to->name() != from->name() ) {
        m->addCommand( new NodeModifyNameCmd( *to, from->name() ) );
    }
    if ( to->description() != from->description() ) {
        m->addCommand( new NodeModifyDescriptionCmd( *to, from->description() ) );
    }
    if ( to->startTime() != from->startTime() && from->startTime().isValid() ) {
        m->addCommand( new NodeModifyStartTimeCmd( *to, from->startTime() ) );
    }
    if ( to->endTime() != from->endTime() && from->endTime().isValid() ) {
        m->addCommand( new NodeModifyEndTimeCmd( *to, from->endTime() ) );
    }
    if ( to->leader() != from->leader() ) {
        m->addCommand( new NodeModifyLeaderCmd( *to, from->leader() ) );
    }

    if ( from->type() == Node::Type_Task && from->type() == Node::Type_Task ) {
        if ( static_cast<Task*>( to )->workPackage().ownerId() != static_cast<const Task*>( from )->workPackage().ownerId() ) {
            kDebug(planworkDbg())<<"merge:"<<"different owners"<<static_cast<const Task*>( from )->workPackage().ownerName()<<static_cast<Task*>( to )->workPackage().ownerName();
            if ( static_cast<Task*>( to )->workPackage().ownerId().isEmpty() ) {
                //TODO cmd
                static_cast<Task*>( to )->workPackage().setOwnerId( static_cast<const Task*>( from )->workPackage().ownerId() );
                static_cast<Task*>( to )->workPackage().setOwnerName( static_cast<const Task*>( from )->workPackage().ownerName() );
            }
        }
        foreach ( Document *doc,  from->documents().documents() ) {
            Document *org = to->documents().findDocument( doc->url() );
            if ( org ) {
                // TODO: also handle modified type, sendas
                // update ? what if open, modified ...
                if ( doc->type() == Document::Type_Product ) {
                    //### FIXME. user feedback
                    kWarning()<<"We do not update existing deliverables (except name change)";
                    if ( doc->name() != org->name() ) {
                        m->addCommand( new DocumentModifyNameCmd( org, doc->name() ) );
                    }
                } else {
                    if ( doc->name() != org->name() ) {
                        m->addCommand( new DocumentModifyNameCmd( org, doc->name() ) );
                    }
                    if ( doc->sendAs() != org->sendAs() ) {
                        m->addCommand( new DocumentModifySendAsCmd( org, doc->sendAs() ) );
                    }
                    if ( doc->sendAs() == Document::SendAs_Copy ) {
                        kDebug(planworkDbg())<<"Update existing doc:"<<org->url();
                        openNewDocument( org, store );
                    }
                }
            } else {
                kDebug(planworkDbg())<<"new document:"<<doc->typeToString(doc->type())<<doc->url();
                Document *newdoc = new Document( *doc );
                m->addCommand( new DocumentAddCmd( to->documents(), newdoc ) );
                if ( doc->sendAs() == Document::SendAs_Copy ) {
                    kDebug(planworkDbg())<<"Copy file";
                    openNewDocument( newdoc, store );
                }
            }
        }
    }
    const Project *fromProject = wp->project();
    Project *toProject = m_project;
    const ScheduleManager *fromSm = fromProject->scheduleManagers().value( 0 );
    Q_ASSERT( fromSm );
    ScheduleManager *toSm = toProject->scheduleManagers().value( 0 );
    Q_ASSERT( toSm );
    if ( fromSm->managerId() != toSm->managerId() || fromSm->scheduleId() != toSm->scheduleId() ) {
        // rescheduled, update schedules
        m->addCommand( new CopySchedulesCmd( *fromProject, *toProject ) );
    }
    if ( m->isEmpty() ) {
        delete m;
    } else {
        part->addCommand( m );
    }
}

void WorkPackage::openNewDocument( const Document *doc, KoStore *store )
{
    KUrl url = extractFile( doc, store );
    if ( url.url().isEmpty() ) {
        KMessageBox::error( 0, i18n( "Could not extract document from storage:<br>%1", doc->url().pathOrUrl() ) );
        return;
    }
    if ( ! url.isValid() ) {
        KMessageBox::error( 0, i18n( "Invalid URL:<br>%1", url.pathOrUrl() ) );
        return;
    }
    m_newdocs.insert( doc, url );
}

int WorkPackage::queryClose( Part *part )
{
    kDebug(planworkDbg())<<isModified();
    QString name = node()->name();
    QStringList lst;
    if ( ! m_childdocs.isEmpty() ) {
        foreach ( DocumentChild *ch, m_childdocs ) {
            if ( ch->isOpen() && ch->doc()->sendAs() == Document::SendAs_Copy ) {
                lst << ch->doc()->url().fileName();
            }
        }
    }
    if ( ! lst.isEmpty() ) {
        int result = KMessageBox::warningContinueCancelList( 0,
                    i18np(
                        "<p>The work package <b>'%2'</b> has an open document.</p><p>Data may be lost if you continue.</p>",
                        "<p>The work package <b>'%2'</b> has open documents.</p><p>Data may be lost if you continue.</p>",
                        lst.count(),
                        name ),
                    lst );

        switch (result) {
            case KMessageBox::Continue: {
                kDebug(planworkDbg())<<"Continue";
                break;
            }
            default: // case KMessageBox::Cancel :
                kDebug(planworkDbg())<<"Cancel";
                return KMessageBox::Cancel;
                break;
        }
    }
    if ( ! isModified() ) {
        return KMessageBox::Yes;
    }
    int res = KMessageBox::warningYesNoCancel( 0,
                i18n("<p>The work package <b>'%1'</b> has been modified.</p><p>Do you want to save it?</p>", name),
                QString(),
                KStandardGuiItem::save(),
                KStandardGuiItem::discard());

    switch (res) {
        case KMessageBox::Yes: {
            kDebug(planworkDbg())<<"Yes";
            saveToProjects( part );
            break;
        }
        case KMessageBox::No:
            kDebug(planworkDbg())<<"No";
            break;
        default: // case KMessageBox::Cancel :
            kDebug(planworkDbg())<<"Cancel";
            break;
    }
    return res;
}

KUrl WorkPackage::extractFile( const Document *doc )
{
    KoStore *store = KoStore::createStore( m_filePath, KoStore::Read, "", KoStore::Zip );
    if ( store->bad() )
    {
        KMessageBox::error( 0, i18n( "<p>Work package <b>'%1'</b></p><p>Could not open store:</p><p>%2</p>", node()->name(), m_filePath ) );
        delete store;
        return KUrl();
    }
    KUrl url = extractFile( doc, store );
    delete store;
    return url;
}

KUrl WorkPackage::extractFile( const Document *doc, KoStore *store )
{
    //FIXME: should use a special tmp dir
    QString tmp = KStandardDirs::locateLocal( "tmp", QString(), false );
    KUrl url( tmp + doc->url().fileName() );
    kDebug(planworkDbg())<<"Extract: "<<doc->url().fileName()<<" -> "<<url.pathOrUrl();
    if ( ! store->extractFile( doc->url().fileName(), url.path() ) ) {
        KMessageBox::error( 0, i18n( "<p>Work package <b>'%1'</b></p><p>Could not extract file:</p><p>%2</p>", node()->name(), doc->url().fileName() ) );
        return KUrl();
    }
    return url;
}

QString WorkPackage::id() const
{
    QString id;
    if ( node() ) {
        id = m_project->id() + node()->id();
    }
    return id;
}

//--------------------------------
PackageRemoveCmd::PackageRemoveCmd( Part *part, WorkPackage *value, const QString& name )
    : NamedCommand( name ),
    m_part( part ),
    m_value( value ),
    m_mine( false )
{
}
PackageRemoveCmd::~PackageRemoveCmd()
{
    if ( m_mine ) {
        m_value->removeFile();
        delete m_value;
    }
}
void PackageRemoveCmd::execute()
{
    m_part->removeWorkPackage( m_value );
    m_mine = true;
}
void PackageRemoveCmd::unexecute()
{
    m_part->addWorkPackage( m_value );
    m_mine = false;
}

//---------------------
ModifyPackageSettingsCmd::ModifyPackageSettingsCmd( WorkPackage *wp, WorkPackageSettings &value, const QString& name )
    : NamedCommand( name ),
    m_wp( wp ),
    m_value( value ),
    m_oldvalue( wp->settings() )
{
}
void ModifyPackageSettingsCmd::execute()
{
    m_wp->setSettings( m_value );
}
void ModifyPackageSettingsCmd::unexecute()
{
    m_wp->setSettings( m_oldvalue );
}

//---------------------
CopySchedulesCmd::CopySchedulesCmd( const Project &fromProject, Project &toProject, const QString &name )
    : NamedCommand( name ),
      m_project( toProject )
{
    QDomDocument olddoc;
    QDomElement e = olddoc.createElement( "old" );
    olddoc.appendChild( e );
    toProject.save( e );
    m_olddoc = olddoc.toString();

    QDomDocument newdoc;
    e = newdoc.createElement( "new" );
    newdoc.appendChild( e );
    fromProject.save( e );
    m_newdoc = newdoc.toString();
}
void CopySchedulesCmd::execute()
{
    load( m_newdoc );
}
void CopySchedulesCmd::unexecute()
{
    load( m_olddoc );
}

void CopySchedulesCmd::load( const QString &doc )
{
    clearSchedules();

    KoXmlDocument d;
    d.setContent( doc );
    KoXmlElement proj = d.documentElement().namedItem( "project").toElement();
    Q_ASSERT( ! proj.isNull() );
    KoXmlElement task = proj.namedItem( "task").toElement();
    Q_ASSERT( ! task.isNull() );
    KoXmlElement ts = task.namedItem( "schedules").namedItem( "schedule").toElement();
    Q_ASSERT( ! ts.isNull() );
    KoXmlElement ps = proj.namedItem( "schedules").namedItem( "plan" ).toElement();
    Q_ASSERT( ! ps.isNull() );

    XMLLoaderObject status;
    status.setProject( &m_project );
    status.setVersion( PLAN_FILE_SYNTAX_VERSION );
    // task first
    NodeSchedule *ns = new NodeSchedule();
    if ( ns->loadXML( ts, status ) ) {
        kDebug(planworkDbg())<<ns->name()<<ns->type()<<ns->id();
        ns->setNode( m_project.childNode( 0 ) );
        m_project.childNode( 0 )->addSchedule( ns );
    } else {
        Q_ASSERT( false );
        delete ns;
    }
    // schedule manager next (includes main schedule and resource schedule)
    ScheduleManager *sm = new ScheduleManager( m_project );
    if ( sm->loadXML( ps, status ) ) {
        m_project.addScheduleManager( sm );
    } else {
        Q_ASSERT( false );
        delete sm;
    }
    if ( sm ) {
        m_project.setCurrentSchedule( sm->scheduleId() );
    }
    m_project.childNode( 0 )->changed();
}

void CopySchedulesCmd::clearSchedules()
{
    foreach ( Schedule *s, m_project.schedules() ) {
        m_project.takeSchedule( s );
    }
    foreach ( Schedule *s, m_project.childNode( 0 )->schedules() ) {
        foreach ( Appointment *a, s->appointments() ) {
            if ( a->resource() && a->resource()->resource() ) {
                a->resource()->resource()->takeSchedule( a->resource() );
            }
        }
        m_project.childNode( 0 )->takeSchedule( s );
    }
    foreach ( ScheduleManager *sm,  m_project.scheduleManagers() ) {
        m_project.takeScheduleManager( sm );
        delete sm;
    }
}


}  //KPlatoWork namespace

#include "workpackage.moc"
