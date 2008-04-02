/*
 * This file is part of KPlato
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "Module.h"
#include "Project.h"
#include "Node.h"
#include "Resource.h"
#include "ResourceGroup.h"
#include "Schedule.h"
#include "ScriptingWidgets.h"

// qt
#include <QPointer>
#include <QWidget>
// kde
#include <kdebug.h>
// kplato
#include <kptpart.h>
#include <kptview.h>
#include <kptproject.h>
#include <kptnode.h>

extern "C"
{
    KDE_EXPORT QObject* krossmodule()
    {
        return new Scripting::Module();
    }
}

namespace Scripting {

    /// \internal d-pointer class.
    class Module::Private
    {
        public:
            QPointer<KPlato::Part> doc;
            Project *project;
            Node *node;
    };

Module::Module(QObject* parent)
    : KoScriptingModule(parent, "KPlato")
    , d( new Private() )
{
    d->doc = 0;
    d->project = 0;
}

Module::~Module()
{
    delete d;
}

KPlato::Part* Module::part()
{
    if(! d->doc) {
        if( KPlato::View* v = dynamic_cast< KPlato::View* >(view()) )
            d->doc = v->getPart();
        if( ! d->doc )
            d->doc = new KPlato::Part(0, this);
    }
    return d->doc;
}

KoDocument* Module::doc()
{
    return part();
}

QObject *Module::project()
{
    if ( d->project == 0 ) {
        d->project = new Project( this, &(part()->getProject()) );
    }
    return d->project;
}

QWidget *Module::createScheduleListView( QWidget *parent )
{
    return new ScriptingScheduleListView( this, parent );
}

QVariant Module::data( QObject *object, const QString &property ) const
{
    return data( object, property, "DisplayRole", -1 );
}

QVariant Module::data( QObject *object, const QString &property, const QString &role, qlonglong scheduleId ) const
{
    if ( object == 0 ) {
        return QVariant();
    }
    Node *n = qobject_cast<Node*>( object );
    if ( n ) {
        return d->project->nodeData( n->kplatoNode(), property, role, scheduleId );
    }
    Resource *r = qobject_cast<Resource*>( object );
    if ( r ) {
        return d->project->resourceData( r->kplatoResource(), property, role, scheduleId );
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( object );
    if ( g ) {
        return d->project->resourceGroupData( g->kplatoResourceGroup(), property, role );
    }
    // TODO Schedule (if needed)
    return QVariant();
}

} //namespace Scripting

#include "Module.moc"
