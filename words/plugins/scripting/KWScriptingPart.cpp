// This file is part of Words
// SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KWScriptingPart.h"
#include "Module.h"

#include <kpluginfactory.h>
#include <kdebug.h>
#include <kstandarddirs.h>

K_PLUGIN_FACTORY(WordsScriptingFactory, registerPlugin<KWScriptingPart>();)
K_EXPORT_PLUGIN(WordsScriptingFactory("krossmodulewords"))

KWScriptingPart::KWScriptingPart(QObject* parent, const QVariantList& args)
        : KoScriptingPart(new Scripting::Module(parent))
{
    Q_UNUSED(args);
    setComponentData(KWScriptingPart::componentData());
    setXMLFile(KStandardDirs::locate("data", "calligrawords/kpartplugins/scripting.rc"), true);
    kDebug(32010) << "Parent:" << parent->metaObject()->className();

    /*
    // Add variables
    Kross::ActionCollection* actioncollection = Kross::Manager::self().actionCollection();
    if( actioncollection && (actioncollection = actioncollection->collection("variables")) ) {
        foreach(Kross::Action* action, actioncollection->actions()) {
            Q_ASSERT(action);
            Scripting::VariableFactory* factory = Scripting::VariableFactory::create(action);
            if( ! factory ) continue;
            kDebug(32010) <<"Adding scripting variable with id=" << factory->id();
        }
    }
    */
}

KWScriptingPart::~KWScriptingPart()
{
}

