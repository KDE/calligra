/*
 * This file is part of KPlato
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <danders@get2net>
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

#include "ScriptingPart.h"

#include "Module.h"
#include "ScriptingDebug.h"

#include <QStandardPaths>
// KF5
#include <kpluginfactory.h>
// kross
#include <kross/core/manager.h>
#include <kross/core/interpreter.h>
#include <kross/core/action.h>
#include <kross/ui/model.h>
// calligra
#include <KoDockRegistry.h>
#include <KoMainWindow.h>
#include <KoDocument.h>

#include <kptview.h>


K_PLUGIN_FACTORY_WITH_JSON(KPlatoScriptingFactory, "planscripting.json",
                           registerPlugin<KPlatoScriptingPart>();)


KPlatoScriptingPart::KPlatoScriptingPart(QObject *parent, const QVariantList &args)
    : KoScriptingPart(new Scripting::Module(parent))
{
    Q_UNUSED(args);
    //setComponentData(ScriptingPart::componentData());
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligraplan/viewplugins/scripting.rc"), true);
    debugPlanScripting <<"PlanScripting plugin. Class:" << metaObject()->className() <<", Parent:" <<(parent?parent->metaObject()->className():"0");

}

KPlatoScriptingPart::~KPlatoScriptingPart()
{
}

#include "ScriptingPart.moc"
