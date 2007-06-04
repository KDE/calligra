/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "importwizard.h"
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>

/*
This is in no way meant to compile let alone work
This is very preliminary and is meant for example only
 
This will be an example program to demonstrate how to import an existing db into
a new kexi based db
*/

using namespace KexiMigration;

int main(int argc, char *argv[])
{
	KAboutData aboutData( "keximigratetest", "Kexi Migrate Test", "2.0" );
	KCmdLineArgs::init(argc, argv, &aboutData);
	KApplication app;

	ImportWizard* iw = new ImportWizard();
	iw->setGeometry(300,300,300,250);
	iw->show();
	
	return app.exec();
}
