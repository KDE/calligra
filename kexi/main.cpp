/* This file is part of the KDE project
    
    begin                : Sun Jun  9 12:15:11 CEST 2002
    copyright            : (C) 2003 by lucijan busch, Joseph Wenninger
    email                : lucijan@gmx.at, jowenn@kde.org
   
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kmimetype.h>

#include "core/kexiproject.h"
#include "core/kexidialogbase.h"
#include "core/kexi.h"
#include "main/keximainwindowimpl.h"
#include "main/kexiaboutdata.h"
#include "main/startup/KexiStartup.h"

#include <qfileinfo.h>

static KCmdLineOptions options[] =
{
  { "open [<object_type>:]<object_name>", I18N_NOOP(
	"Open object of type <object_type>\n"
	"and name <object_name> from specified project\n"
	"on application start.\n"
	"<object_type>: is optional, if omitted - table\n"
	"type is assumed.\n"
	"Other object types can be query, report, form,\n"
	"script (may be more or less, depending on your\n"
	"plugins installed).\n"
	"Use \"\" chars to specify names containing spaces.\n"
	"Examples: --open MyTable,\n"
	" --open query:\"My very big query\""), 0 },
  { "design [<object_type>:]<object_name>", I18N_NOOP(
	"Like --open, but the object will\n"
	"be opened in Design Mode, if one is available"), 0 },
  { "edittext [<object_type>:]<object_name>", I18N_NOOP(
	"Like --open, but the object will\n"
	"be opened in Text Mode, if one is available"), 0 },
  { "new <object_type>", I18N_NOOP(
	"Start new object design of type <object_type>"), 0 },
  { "final-mode", I18N_NOOP(
	"Start project in the Final Mode, regardless \n"
	"of the project settings"), 0 },
  { "design-mode", I18N_NOOP(
	"Start project in the Design Mode, regardless \n"
	"of the project settings"), 0 },
  { "+[file]", I18N_NOOP(
	"Database project file (or shortcut file) to open"), 0 },
  // INSERT YOUR COMMANDLINE OPTIONS HERE
  KCmdLineLastOption
};

#ifdef Q_WS_WIN //temp. workaround
# define kdemain main
#endif

extern "C" int kdemain(int argc, char *argv[])
{
	KCmdLineArgs::init( argc, argv, newKexiAboutData() );
	KCmdLineArgs::addCmdLineOptions( options );

	KApplication app(true, true);
//TODO: switch GUIenabled off when needed
	
	if (!Kexi::startupHandler().init())
		return 1;
	
	kdDebug() << "startupActions OK" <<endl;

	KexiMainWindowImpl *win = new KexiMainWindowImpl();
	app.setMainWidget(win);
	win->show();
	app.processEvents();//allow refresh our app

	if (!win->startup())
		return 1;

	return app.exec();
}

