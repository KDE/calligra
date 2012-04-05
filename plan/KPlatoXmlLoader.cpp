/* This file is part of the KDE project
 Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#include "KPlatoXmlLoader.h"

#include "kptconfig.h"
#include "kptpackage.h"
#include "kptxmlloaderobject.h"
#include "kptproject.h"

#include "KoXmlReader.h"

#include <KMessageBox>
#include <KTimeZone>
#include <KSystemTimeZones>
#include <KDebug>
#include <kdeversion.h>

#include <QDateTime>

extern int kplatoXmlDebugArea();

namespace KPlato
{

KPlatoXmlLoader::KPlatoXmlLoader( XMLLoaderObject &loader, Project* project )
    : KPlatoXmlLoaderBase(),
    m_loader( loader ),
    m_project( project )
{
}

QString KPlatoXmlLoader::errorMessage() const
{
    return m_message;
}

Package *KPlatoXmlLoader::package() const
{
    return m_package;
}

QString KPlatoXmlLoader::timeTag() const
{
    return m_timeTag;
}

bool KPlatoXmlLoader::load( const KoXmlElement& plan )
{
    kDebug(kplatoXmlDebugArea())<<"plan";
    QString syntaxVersion = plan.attribute( "version" );
    m_loader.setVersion( syntaxVersion );
    if ( syntaxVersion.isEmpty() ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document has no syntax version.\n"
                               "Opening it in Plan may lose information." ),
                      i18n( "File-Format Error" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            m_message = "USER_CANCELED";
            return false;
        }
        // set to max version and hope for the best
        m_loader.setVersion( KPLATO_MAX_FILE_SYNTAX_VERSION );
    } else if ( syntaxVersion > KPLATO_MAX_FILE_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of KPlato than Plan can load.\n"
                               "Syntax version: %1\n"
                               "Opening it in this version of Plan may lose some information.", syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            m_message = "USER_CANCELED";
            return false;
        }
    }
    m_loader.startLoad();
    bool result = false;
    KoXmlNode n = plan.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "project" ) {
            m_loader.setProject( m_project );
            result = load( m_project, e, m_loader );
            if ( result ) {
                if ( m_project->id().isEmpty() ) {
                    m_project->setId( m_project->uniqueNodeId() );
                    m_project->registerNodeId( m_project );
                }
            } else {
                m_loader.addMsg( XMLLoaderObject::Errors, "Loading of project failed" );
                kError()<<"Loading of project failed";
                //TODO add some ui here
            }
        }
    }
    m_loader.stopLoad();
    return result;
}


bool KPlatoXmlLoader::loadWorkpackage( const KoXmlElement& plan )
{
    kDebug(kplatoXmlDebugArea());
    bool ok = false;
    if ( m_loader.workVersion() > KPLATOWORK_MAX_FILE_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                0, i18n( "This document was created with a newer version of KPlatoWork (syntax version: %1)\n"
                "Opening it in this version of PlanWork will lose some information.", m_loader.workVersion() ),
                i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            m_message = "USER_CANCELED";
            return false;
        }
    }
    m_loader.startLoad();
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
            m_loader.setProject( proj );
            ok = load( proj, e, m_loader );
            if ( ! ok ) {
                m_loader.addMsg( XMLLoaderObject::Errors, "Loading of work package failed" );
                //TODO add some ui here
            }
        } else if ( e.tagName() == "workpackage" ) {
            m_timeTag = e.attribute( "time-tag" );
            package->ownerId = e.attribute( "owner-id" );
            package->ownerName = e.attribute( "owner" );
            KoXmlElement elem;
            forEachElement( elem, e ) {
                if ( elem.tagName() != "settings" ) {
                    continue;
                }
                package->settings.usedEffort = (bool)elem.attribute( "used-effort" ).toInt();
                package->settings.progress = (bool)elem.attribute( "progress" ).toInt();
                package->settings.documents = (bool)elem.attribute( "documents" ).toInt();
            }
        }
    }
    if ( proj->numChildren() > 0 ) {
        WorkPackage &wp = static_cast<Task*>( proj->childNode( 0 ) )->workPackage();
        if ( wp.ownerId().isEmpty() ) {
            wp.setOwnerId( package->ownerId );
            wp.setOwnerName( package->ownerName );
        }
    }
    return ok;
}

} // namespace KPlato

#include "KPlatoXmlLoader.moc"
