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
#include "Account.h"
#include "Calendar.h"
#include "Node.h"
#include "Resource.h"
#include "ResourceGroup.h"
#include "Schedule.h"
#include "ScriptingWidgets.h"

// qt
#include <QPointer>
#include <QWidget>
#include <QMap>
// kde
#include <kdebug.h>
// kplato
#include "kptpart.h"
#include "kptview.h"
#include "kptproject.h"
#include "kptnode.h"
#include "kptcommand.h"

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
            QMap<QString, Module*> modules;
    };

Module::Module(QObject* parent)
    : KoScriptingModule(parent, "Plan")
    , d( new Private() )
{
    d->doc = 0;
    d->project = 0;

    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "plan" );
        locale->insertCatalog( "planlibs" );
        locale->insertCatalog( "timezones4" );
        locale->insertCatalog( "krossmoduleplan" );
    }
}

Module::~Module()
{
    qDeleteAll( d->modules );
    delete d->project;
    delete d;
}

KPlato::Part* Module::part()
{
    if(! d->doc) {
        if( KPlato::View* v = dynamic_cast< KPlato::View* >(view()) ) {
            d->doc = v->getPart();
        }
        if( ! d->doc ) {
            d->doc = new KPlato::Part(0, this);
        }
    }
    return d->doc;
}

KoDocument* Module::doc()
{
    return part();
}

QObject *Module::openDocument( const QString tag, const QString &url )
{
    Module *m = d->modules[ tag ];
    if ( m == 0 ) {
        m = new Module();
        d->modules[ tag ] = m;
    }
    m->part()->openUrl( url );
    return m;
}


QObject *Module::project()
{
    if ( d->project != 0 && d->project->kplatoProject() != &( part()->getProject() ) ) {
        // need to replace the project, happens when new document is loaded
        delete d->project;
        d->project = 0;
    }
    if ( d->project == 0 ) {
        d->project = new Project( this, &(part()->getProject()) );
    }
    return d->project;
}


QWidget *Module::createScheduleListView( QWidget *parent )
{
    ScriptingScheduleListView *v = new ScriptingScheduleListView( this, parent );
    if ( parent && parent->layout() ) {
        parent->layout()->addWidget( v );
    }
    return v;
}

QWidget *Module::createDataQueryView( QWidget *parent )
{
    ScriptingDataQueryView *v = new ScriptingDataQueryView( this, parent );
    if ( parent && parent->layout() ) {
        parent->layout()->addWidget( v );
    }
    return v;
}


QVariant Module::data( QObject *object, const QString &property )
{
    return data( object, property, "DisplayRole", -1 );
}

QVariant Module::data( QObject *object, const QString &property, const QString &role, qlonglong scheduleId )
{
    Project *p = qobject_cast<Project*>( project() );
    if ( object == 0 || p == 0) {
        return QVariant();
    }
    Node *n = qobject_cast<Node*>( object );
    if ( n ) {
        return p->nodeData( n->kplatoNode(), property, role, scheduleId );
    }
    Resource *r = qobject_cast<Resource*>( object );
    if ( r ) {
        return p->resourceData( r->kplatoResource(), property, role, scheduleId );
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( object );
    if ( g ) {
        return p->resourceGroupData( g->kplatoResourceGroup(), property, role );
    }
    Account *a = qobject_cast<Account*>( object );
    if ( a ) {
        return p->accountData( a->kplatoAccount(), property, role );
    }
    Calendar *c = qobject_cast<Calendar*>( object );
    if ( c ) {
        return p->calendarData( c->kplatoCalendar(), property, role );
    }
    // TODO Schedule (if needed)
    return QVariant();
}

bool Module::setData( QObject *object, const QString &property, const QVariant &data, const QString &role )
{
    Project *p = qobject_cast<Project*>( project() );
    if ( object == 0 || p == 0) {
        return false;
    }
    Node *n = qobject_cast<Node*>( object );
    if ( n ) {
        return p->setNodeData( n->kplatoNode(), property, data, role );
    }
    Resource *r = qobject_cast<Resource*>( object );
    if ( r ) {
        return p->setResourceData( r->kplatoResource(), property, data, role );
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( object );
    if ( g ) {
        return p->setResourceGroupData( g->kplatoResourceGroup(), property, data, role );
    }
    Account *a = qobject_cast<Account*>( object );
    if ( a ) {
        return p->setAccountData( a->kplatoAccount(), property, data, role );
    }
    Calendar *c = qobject_cast<Calendar*>( object );
    if ( c ) {
        return p->setCalendarData( c->kplatoCalendar(), property, data, role );
    }
    return false;
}

QVariant Module::headerData( int objectType, const QString &property )
{
    Project *p = qobject_cast<Project*>( project() );
    if ( p == 0 ) {
        return QVariant();
    }
    switch ( objectType ) {
        case 0: return p->nodeHeaderData( property );
        case 1: return p->resourceHeaderData( property );
        case 2: return p->accountHeaderData( property );
        default: break;
    }
    return QVariant();
}

void Module::addCommand( KUndo2Command *cmd )
{
    KPlato::MacroCommand *m = new MacroCommand( cmd->text().isEmpty()
            ? i18nc( "(qtundo-format)", "Scripting command" )
            : cmd->text() );
    doc()->addCommand( m );
    m->addCommand( cmd );
}

} //namespace Scripting

#include "Module.moc"
