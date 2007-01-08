/***************************************************************************
 *   Copyright (C) 2004 by Adam Pigg                                       *
 *   adam@piggz.co.uk                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.             *
 ***************************************************************************/

#include <migration/importwizard.h>
#include <kapplication.h>

/*
This is in no way meant to compile let alone work
This is very preliminary and is meant for example only
 
This will be an example program to demonstrate how to import an existing db into
a new kexi based db
*/

using namespace KexiMigration;

int main(int argc, char *argv[])
{
	KApplication app(argc, argv, "Kexi Migrate Test");

	ImportWizard* iw = new ImportWizard();
	iw->setGeometry(300,300,300,250);
	app.setMainWidget(iw);
	iw->show();
	
	return app.exec();
}
