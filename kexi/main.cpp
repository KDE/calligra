/*
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jun  9 12:15:11 CEST 2002
    copyright            : (C) 2002 by lucijan busch, Joseph Wenninger
    email                : lucijan@gmx.at, jowenn@kde.org

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

#include <koApplication.h>
#include <kcmdlineargs.h>
#include <dcopclient.h>
#include <klocale.h>
#include "core/kexi_aboutdata.h"

static KCmdLineOptions options[] =
{
  { "open [<object_type>:]<object_name>", I18N_NOOP("Open object of type <object_type>\nand name <object_name> from specified project\non application start.\n<object_type>: is optional, if omitted - table\ntype is assumed.\nOther object types can be query, report, form,\nscript (may be more or less, depending on your\nplugins installed).\nUse \"\" chars to specify names containing spaces.\nExamples: --open MyTable,\n --open query:\"My very big query\""), 0 },
  { "+[File]", I18N_NOOP("File to open"), 0 },
  KCmdLineLastOption
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

	KCmdLineArgs::init( argc, argv, newKexiAboutData() );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KoApplication app;
	app.dcopClient()->attach();
	app.dcopClient()->registerAs( "kexi" );

	if (!app.start()) return 1;

	return app.exec();
}
