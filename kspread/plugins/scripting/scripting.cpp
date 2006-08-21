/*
 * This file is part of the KSpread project
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "scripting.h"

#include <QApplication>

#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>

#include <Doc.h>
#include <View.h>

#define KROSS_MAIN_EXPORT KDE_EXPORT
#include <core/manager.h>
#include <core/guiclient.h>
//#include <main/wdgscriptsmanager.h>

typedef KGenericFactory<Scripting> KSpreadScriptingFactory;
K_EXPORT_COMPONENT_FACTORY( kspreadscripting, KSpreadScriptingFactory( "kspreadscripting" ) )

class Scripting::Private
{
    public:
        KSpread::View * view;
        Kross::GUIClient* scriptguiclient;
};

Scripting::Scripting(QObject *parent, const QStringList &)
    : KParts::Plugin(parent)
    , d(new Private())
{
    setInstance(KSpreadScriptingFactory::instance());
    kDebug() << "Scripting plugin. Class: " << metaObject()->className() << ", Parent: " << parent->metaObject()->className() << endl;

    if ( parent->inherits("KSpread::View") ) {
        setInstance(Scripting::instance());
        //d->scriptguiclient ->setXMLFile(locate("data","kspreadplugins/scripting.rc"), true);
        setXMLFile(KStandardDirs::locate("data","kspread/kpartplugins/scripting.rc"), true);

        d->view = static_cast< KSpread::View* >( parent );
        d->scriptguiclient = new Kross::GUIClient( d->view, d->view );

        KAction* execaction = new KAction(i18n("Execute Script File..."), actionCollection(), "executescriptfile");
        connect(execaction, SIGNAL(triggered(bool)), d->scriptguiclient, SLOT(executeFile()));

        KAction* manageraction = new KAction(i18n("Script Manager..."), actionCollection(), "configurescripts");
        connect(manageraction, SIGNAL(triggered(bool)), d->scriptguiclient, SLOT(showManager()));

        KAction* scriptmenuaction = d->scriptguiclient->action("scripts");
        actionCollection()->insert(scriptmenuaction);

        //connect(d->scriptguiclient, SIGNAL(executionFinished( const Kross::Api::ScriptAction* )), this, SLOT(executionFinished()));
        Kross::Manager::self().addObject(d->view->doc(), "KSpreadDocument");
        Kross::Manager::self().addObject(d->view, "KSpreadView");
    }
    else {
        kWarning() << "Scripting::Scripting Invalid parent QObject!" << endl;
    }
}

Scripting::~Scripting()
{
    delete d;
}

void Scripting::executionFinished()
{
    kDebug() << "Scripting::executionFinished" << endl;
    QApplication::restoreOverrideCursor();
}

#include "scripting.moc"
