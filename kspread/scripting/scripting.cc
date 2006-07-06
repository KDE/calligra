/*
 * This file is part of the KSpread project
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
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

#define KROSS_MAIN_EXPORT KDE_EXPORT
#include <main/manager.h>
#include <main/scriptguiclient.h>
#include <main/wdgscriptsmanager.h>

#include <kspread_doc.h>
#include <kspread_view.h>

typedef KGenericFactory<Scripting> KSpreadScriptingFactory;
K_EXPORT_COMPONENT_FACTORY( kspreadscripting, KSpreadScriptingFactory( "kspread" ) )

Scripting::Scripting(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{
    setInstance(KSpreadScriptingFactory::instance());

    kDebug() << "Scripting plugin. Class: "
          << metaObject()->className()
          << ", Parent: "
          << parent->metaObject()->className()
          << "\n";
    if ( parent->inherits("KSpread::View") )
    {
        setInstance(Scripting::instance());
        m_view = (KSpread::View*) parent;
        m_scriptguiclient = new Kross::Api::ScriptGUIClient( m_view, m_view );
//         m_scriptguiclient ->setXMLFile(locate("data","kspreadplugins/scripting.rc"), true);
        kDebug() << "Setup actions for scripting !" << endl;
        //BEGIN TODO: understand why the ScriptGUIClient doesn't "link" its actions to the menu
        setXMLFile(KStandardDirs::locate("data","kspread/kpartplugins/scripting.rc"), true);
        new KAction(i18n("Execute Script File..."), 0, 0, m_scriptguiclient, SLOT(executeScriptFile()), actionCollection(), "executescriptfile");
        new KAction(i18n("Script Manager..."), 0, 0, m_scriptguiclient, SLOT(showScriptManager()), actionCollection(), "configurescripts");
        //END TODO

        connect(m_scriptguiclient, SIGNAL(executionFinished( const Kross::Api::ScriptAction* )), this, SLOT(executionFinished(const Kross::Api::ScriptAction*)));
	Kross::Api::Manager::scriptManager()->addQObject(m_view->doc(), "KSpreadDocument");
        Kross::Api::Manager::scriptManager()->addQObject(m_view, "KSpreadView");
    }

}

Scripting::~Scripting()
{
}

void Scripting::executionFinished(const Kross::Api::ScriptAction*)
{
    QApplication::restoreOverrideCursor();
}

#include "scripting.moc"
