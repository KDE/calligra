/* This file is part of the KDE project
    
    begin                : Sun Jun  9 12:15:11 CEST 2002
    copyright            : (C) 2003 by lucijan busch, Joseph Wenninger
    email                : lucijan@gmx.at, jowenn@kde.org
   
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kapplication.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include <core/kexiproject.h>
#include <core/kexidialogbase.h>
#include <core/kexi.h>
#include <main/keximainwindowimpl.h>
#include <main/startup/KexiStartup.h>
#include <kexidb/utils.h>

extern "C" int kdemain(int argc, char *argv[])
{
	Kexi::initCmdLineArgs( argc, argv );

	bool GUIenabled = true;
	QWidget *dummyWidget = 0; //needed to have icon for dialogs before KexiMainWindowImpl is created
//! @todo switch GUIenabled off when needed
	KApplication app(true, GUIenabled);
#ifdef KEXI_STANDALONE
	KGlobal::locale()->removeCatalog("kexi");
	KGlobal::locale()->insertCatalog("standalone_kexi");
#endif
	KGlobal::locale()->insertCatalog("koffice");
	KGlobal::locale()->insertCatalog("koproperty");

	if (GUIenabled) {
		dummyWidget = new QWidget();
		dummyWidget->setIcon( DesktopIcon( "kexi" ) );
		app.setMainWidget(dummyWidget);
	}

#ifdef KEXI_DEBUG_GUI
	QWidget* debugWindow = 0;
	app.config()->setGroup("General");
	if (app.config()->readBoolEntry("showKexiDBDebugger", false))
		debugWindow = KexiDB::createDebugWindow(0);
#endif

	tristate res = Kexi::startupHandler().init(argc, argv);
	if (!res)
		return 1;
	if (~res)
		return 0;
	
	kDebug() << "startupActions OK" <<endl;

	/* Exit requested, e.g. after database removing. */
	if (Kexi::startupHandler().action() == KexiStartupData::Exit)
		return 0;

#ifdef CUSTOM_VERSION
# include "custom_exec.h"
#endif

	KexiMainWindowImpl *win = new KexiMainWindowImpl();
	app.setMainWidget(win);
	delete dummyWidget;

	if (true != win->startup()) {
		delete win;
		return 1;
	}

	win->show();
	app.processEvents();//allow refresh our app

	int r = app.exec();

#ifdef KEXI_DEBUG_GUI
	delete debugWindow;
#endif
	return r;
}
