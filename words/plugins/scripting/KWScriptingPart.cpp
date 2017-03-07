/*
 * This file is part of Words
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

