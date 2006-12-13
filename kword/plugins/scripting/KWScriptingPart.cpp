/*
 * This file is part of KWord
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
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

#include "KWScriptingPart.h"
#include "Module.h"

//#include <QApplication>
//#include <QFileInfo>

#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
//#include <kcmdlineargs.h>
//#include <kurl.h>

#include <KWView.h>

//#define KROSS_MAIN_EXPORT KDE_EXPORT
#include <kross/core/manager.h>
#include <kross/core/guiclient.h>

typedef KGenericFactory< KWScriptingPart > KWordScriptingFactory;
K_EXPORT_COMPONENT_FACTORY( krossmodulekword, KWordScriptingFactory( "krossmodulekword" ) )

/// \internal d-pointer class.
class KWScriptingPart::Private
{
    public:
        Kross::GUIClient* guiclient;
        Scripting::Module* module;

        Private() : module(0) {}
        ~Private() {}
};

KWScriptingPart::KWScriptingPart(QObject* parent, const QStringList&)
    : KParts::Plugin(parent)
    , d(new Private())
{
    setInstance(KWScriptingPart::instance());
    setXMLFile(KStandardDirs::locate("data","kword/kpartplugins/scripting.rc"), true);

    kDebug() << "KWScripting plugin. Class: " << metaObject()->className() << ", Parent: " << parent->metaObject()->className() << endl;

    KWView* view = dynamic_cast< KWView* >(parent);
    Q_ASSERT(view);

    // Create the Kross GUIClient which is the higher level to let
    // Kross deal with scripting code.
    d->guiclient = new Kross::GUIClient(this, this);
    //d->guiclient ->setXMLFile(locate("data","kspreadplugins/scripting.rc"), true);

    // Setup the actions Kross provides and KSpread likes to have.
    KAction* execaction = new KAction(i18n("Execute Script File..."), actionCollection(), "executescriptfile");
    connect(execaction, SIGNAL(triggered(bool)), d->guiclient, SLOT(executeFile()));

    KAction* manageraction = new KAction(i18n("Script Manager..."), actionCollection(), "configurescripts");
    connect(manageraction, SIGNAL(triggered(bool)), d->guiclient, SLOT(showManager()));

    QAction* scriptmenuaction = d->guiclient->action("scripts");
    actionCollection()->insert(scriptmenuaction);

    // Publish the KWScriptingModule which offers access to KSpread internals.
    Scripting::Module* module = Kross::Manager::self().hasObject("KWord")
        ? dynamic_cast< Scripting::Module* >( Kross::Manager::self().object("KWord") )
        : 0;
    if( ! module ) {
        module = new Scripting::Module(this);
        Kross::Manager::self().addObject(module, "KWord");
    }
    module->setView(view);
}

KWScriptingPart::~KWScriptingPart()
{
    kDebug() << "KWScriptingPart::~KWScriptingPart()" << endl;
    delete d;
}

#include "KWScriptingPart.moc"
