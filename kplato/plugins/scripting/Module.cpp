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
#include "ResourceGroup.h"
#include "Schedule.h"

// qt
#include <QPointer>
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

int Module::nodeCount()
{
    return d->project->nodeCount();
}

QObject *Module::nodeAt( int index )
{
    return d->project->node( index );
}

int Module::nodeCount( QObject *parent )
{
    Node *n = dynamic_cast<Node*>( parent );
    return n ? n->childCount() : 0;
}

QObject *Module::nodeAt( QObject *parent, int index )
{
    Node *n = dynamic_cast<Node*>( parent );
    return n ? n->childAt( index ) : 0;
}

int Module::resourceGroupCount()
{
    return d->project->resourceGroupCount();
}

QObject *Module::resourceGroupAt( int index )
{
    return d->project->resourceGroupAt( index );
}

int Module::resourceCount( QObject *parent )
{
    ResourceGroup *g = dynamic_cast<ResourceGroup*>( parent );
    return g ? g->resourceCount() : 0;
}

QObject *Module::resourceAt( QObject *parent, int index )
{
    ResourceGroup *g = dynamic_cast<ResourceGroup*>( parent );
    return g ? g->resourceAt( index ) : 0;
}

int Module::scheduleCount() const
{
    return d->project->scheduleCount();
}

QObject *Module::scheduleAt( int index )
{
    return d->project->scheduleAt( index );
}

QObject *Module::scheduleAt( QObject *parent, int index )
{
    Schedule *s = dynamic_cast<Schedule*>( parent );
    return s ? s->childAt( index ) : 0;
}


}

#include "Module.moc"
