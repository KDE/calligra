/* This file is part of the KDE project
   Copyright (C) 2003 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptrequestresourcespanel.h"
#include "kpttask.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptcalendar.h"
#include "kptresourceallocationeditor.h"

#include <kdebug.h>
#include <klocale.h>
#include <kptcommand.h>

#include <QHeaderView>


namespace KPlato
{

RequestResourcesPanel::RequestResourcesPanel(QWidget *parent, Project &project, Task &task, bool)
    : QWidget(parent)
{
    QVBoxLayout *l = new QVBoxLayout( this );
    m_view = new ResourceAllocationTreeView( this );
    m_view->setViewSplitMode( false );
    m_view->masterView()->header()->moveSection( ResourceAllocationModel::RequestType, m_view->masterView()->header()->count() - 1 );
    m_view->setReadWrite( true );
    l->addWidget( m_view );
    m_view->setProject( &project );
    m_view->setTask( &task );
    m_view->masterView()->header()->resizeSections( QHeaderView::ResizeToContents );

    connect( m_view, SIGNAL( dataChanged() ), SIGNAL( changed() ) );
}

bool RequestResourcesPanel::ok()
{
    return true;
}

MacroCommand *RequestResourcesPanel::buildCommand()
{
    Task *t = m_view->task();
    if ( t == 0 ) {
        return 0;
    }
    MacroCommand *cmd = new MacroCommand( i18n( "Modify resource allocations" ) );
    Project *p = m_view->project();
    const QMap<QString, int> &rmap = m_view->resourceCache();

    // First remove all that should be removed
    for( QMap<QString, int>::const_iterator rit = rmap.constBegin(); rit != rmap.constEnd(); ++rit ) {
        Resource *r = p->findResource( rit.key() );
        if ( r == 0 ) {
            continue;
        }
        if ( rit.value() == 0 ) {
            ResourceRequest *rr = t->requests().find( r );
            if ( rr ) {
                cmd->addCommand( new RemoveResourceRequestCmd( rr->parent(), rr ) );
            }
        }
    }
    // Add/modify
    const QMap<QString, int> &gmap = m_view->groupCache();
    for( QMap<QString, int>::const_iterator git = gmap.constBegin(); git != gmap.constEnd(); ++git ) {
        ResourceGroup *g = p->findResourceGroup( git.key() );
        if ( g == 0 ) {
            continue;
        }
        ResourceGroupRequest *gr = t->requests().find( g );
        if ( gr == 0 ) {
            if ( git.value() > 0 ) {
                gr = new ResourceGroupRequest( g, git.value() );
                cmd->addCommand( new AddResourceGroupRequestCmd( *t, gr ) );
            } // else nothing
        } else {
            cmd->addCommand( new ModifyResourceGroupRequestUnitsCmd( gr, gr->units(), git.value() ) );
        }
    }
    QMap<ResourceGroup*, ResourceGroupRequest*> groups;
    for( QMap<QString, int>::const_iterator rit = rmap.constBegin(); rit != rmap.constEnd(); ++rit ) {
        Resource *r = p->findResource( rit.key() );
        if ( r == 0 ) {
            continue;
        }
        if ( rit.value() > 0 ) {
            ResourceRequest *rr = t->requests().find( r );
            if ( rr == 0 ) {
                ResourceGroupRequest *gr = t->requests().find( r->parentGroup() );
                if ( gr == 0 ) {
                    if ( groups.contains( r->parentGroup() ) ) {
                        gr = groups[ r->parentGroup() ];
                    } else {
                        gr = new ResourceGroupRequest( r->parentGroup(), 0 );
                        groups[ r->parentGroup() ] = gr;
                        cmd->addCommand( new AddResourceGroupRequestCmd( *t, gr ) );
                    }
                }
                ResourceRequest *rr = new ResourceRequest( r, rit.value() );
                cmd->addCommand( new AddResourceRequestCmd( gr, rr ) );
            } else if ( rit.value() != rr->units() ) {
                cmd->addCommand( new ModifyResourceRequestUnitsCmd( rr, rr->units(), rit.value() ) );
            }
        }
    }
    if ( cmd->isEmpty() ) {
        delete cmd;
        return 0;
    }
    return cmd;
}

}  //KPlato namespace

#include "kptrequestresourcespanel.moc"
