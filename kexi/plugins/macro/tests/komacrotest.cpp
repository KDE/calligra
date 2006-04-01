/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2005 by Tobi Krebs (tobi.krebs@gmail.com)
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kunittest/runner.h>

static const char description[] =
	I18N_NOOP("KoMacroTester");
static const char version[] = "0.1";
static KCmdLineOptions options[] = 
{ 
	 KCmdLineLastOption 
};

int main( int argc, char** argv )
{
	try {
		KAboutData about("KoMacroTester",
						 I18N_NOOP("KoMacroTester"),
						 version,
						 description,
						 KAboutData::License_LGPL,
						"(C) 2005 Sebastian Sauer", 0, 0, "mail@dipe.org");

		KCmdLineArgs::init(argc, argv, &about);
		KCmdLineArgs::addCmdLineOptions( options );
		KApplication app;

		//create an new "Console"-runner
		KUnitTest::Runner * runner = KUnitTest::Runner::self(); 
		//start our Testsuite
		runner->runTests();
		//done
		return 0;
	}
	// mmh seems we forgot to catch an exception...
	catch(...) {
		qFatal("Unhandled Exception!");
	}
}
