/*
 * This file is part of KSpread
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
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

#include "ScriptingPart.h"
#include "ScriptingModule.h"

#include <QApplication>
#include <QFileInfo>

#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kcmdlineargs.h>
#include <kurl.h>

#include <Doc.h>
#include <View.h>

#define KROSS_MAIN_EXPORT KDE_EXPORT
#include <kross/core/manager.h>
#include <kross/core/guiclient.h>
//#include <main/wdgscriptsmanager.h>

typedef KGenericFactory< ScriptingPart > KSpreadScriptingFactory;
K_EXPORT_COMPONENT_FACTORY( krossmodulekspread, KSpreadScriptingFactory( "krossmodulekspread" ) )

/// \internal d-pointer class.
class ScriptingPart::Private
{
	public:
		Kross::GUIClient* guiclient;
		ScriptingModule* module;

		Private() : module(0) {}
		~Private() {}
};

ScriptingPart::ScriptingPart(QObject* parent, const QStringList&)
    : KParts::Plugin(parent)
    , d(new Private())
{
	setComponentData(ScriptingPart::componentData());
	setXMLFile(KStandardDirs::locate("data","kspread/kpartplugins/scripting.rc"), true);

	kDebug() << "Scripting plugin. Class: " << metaObject()->className() << ", Parent: " << parent->metaObject()->className() << endl;

	KSpread::View* view = dynamic_cast< KSpread::View* >(parent);
	Q_ASSERT(view);

	// Create the Kross GUIClient which is the higher level to let
	// Kross deal with scripting code.
	d->guiclient = new Kross::GUIClient(this, this);
	//d->guiclient ->setXMLFile(locate("data","kspreadplugins/scripting.rc"), true);

	// Setup the actions Kross provides and KSpread likes to have.
    KAction* execaction  = new KAction(i18n("Execute Script File..."), this);
    actionCollection()->addAction("executescriptfile", execaction );
	connect(execaction, SIGNAL(triggered(bool)), d->guiclient, SLOT(executeFile()));

    KAction* manageraction  = new KAction(i18n("Script Manager..."), this);
    actionCollection()->addAction("configurescripts", manageraction );
	connect(manageraction, SIGNAL(triggered(bool)), d->guiclient, SLOT(showManager()));

	QAction* scriptmenuaction = d->guiclient->action("scripts");
	actionCollection()->addAction("scripts", scriptmenuaction);

	// Publish the ScriptingModule which offers access to KSpread internals.
	ScriptingModule* module = Kross::Manager::self().hasObject("KSpread")
		? dynamic_cast< ScriptingModule* >( Kross::Manager::self().object("KSpread") )
		: 0;
	if( ! module ) {
		module = new ScriptingModule();
		Kross::Manager::self().addObject(module, "KSpread");
	}
	module->setView(view);

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	foreach(QByteArray ba, args->getOptionList("scriptfile")) {
		KUrl url(ba);
		QFileInfo fi(url.path());
		const QString file = fi.absoluteFilePath();
		if( ! fi.exists() ) {
			kWarning() << QString("ScriptingPart: scriptfile \"%1\" does not exist.").arg(file) << endl;
			continue;
		}
		if( ! fi.isExecutable() ) {
			kWarning() << QString("ScriptingPart: scriptfile \"%1\" is not executable. Please set the executable-attribute on that file.").arg(file) << endl;
			continue;
		}
		{ // check whether file is not in some temporary directory.
			QStringList tmpDirs = KGlobal::dirs()->resourceDirs("tmp");
			tmpDirs += KGlobal::dirs()->resourceDirs("cache");
			tmpDirs.append("/tmp/");
			tmpDirs.append("/var/tmp/");
			bool inTemp = false;
			foreach(QString tmpDir, tmpDirs)
				if( file.startsWith(tmpDir) ) {
					inTemp = true;
					break;
				}
			if( inTemp ) {
				kWarning() << QString("ScriptingPart: scriptfile \"%1\" is in a temporary directory. Execution denied.").arg(file) << endl;
				continue;
			}
		}
		if( ! d->guiclient->executeFile(url) )
			kWarning() << QString("ScriptingPart: Failed to execute scriptfile \"%1\"").arg(file) << endl;
	}
}

ScriptingPart::~ScriptingPart()
{
    kDebug() << "ScriptingPart::~ScriptingPart()" << endl;
    delete d;
}

#include "ScriptingPart.moc"
