/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "simplecommandlineapp.h"

#include <qfileinfo.h>
#include <qtextstream.h>

#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcomponentdata.h>

#include <kexidb/connectiondata.h>
#include <kexidb/drivermanager.h>

using namespace KexiDB;

static KCmdLineOptions predefinedOptions[] =
{
	{ "drv", 0, 0 },
	{ "driver <name>", I18N_NOOP("Database driver name"), 0 },
	{ "u", 0, 0 },
	{ "user <name>", I18N_NOOP("Database user name"), 0 },
	{ "p", 0, 0 },
	{ "password", I18N_NOOP("Prompt for password"), 0 },
	{ "h", 0, 0 },
	{ "host <name>", I18N_NOOP("Host (server) name"), 0 },
	{ "port <number>", I18N_NOOP("Server's port number"), 0 },
	{ "s", 0, 0 },
	{ "local-socket <filename>", I18N_NOOP("Server's local socket filename"), 0 },
	KCmdLineLastOption
};

//-----------------------------------------

//! @internal used for SimpleCommandLineApp
class SimpleCommandLineApp::Private
{
public:
	Private()
	 : conn(0)
	{}
	~Private()
	{
		if (conn) {
			conn->disconnect();
			delete (Connection*)conn;
		}

		for (KCmdLineOptions *optionsPtr = allOptions; optionsPtr->name; optionsPtr++) {
			delete optionsPtr->name;
			delete optionsPtr->description;
			delete optionsPtr->def;
		}
		delete allOptions;
	}

	KexiDB::DriverManager manager;
	KCmdLineOptions *allOptions;
	KComponentData componentData;
	ConnectionData connData;
	QPointer<Connection> conn;
};

//-----------------------------------------

SimpleCommandLineApp::SimpleCommandLineApp(
	int argc, char** argv, KCmdLineOptions *options,
	const char *programName, const char *version, 
	const char *shortDescription, KAboutData::LicenseKey licenseType, 
	const char *copyrightStatement, const char *text, 
	const char *homePageAddress, const char *bugsEmailAddress)
 : Object()
 , d( new Private() )
{
	QFileInfo fi(argv[0]);
	Q3CString appName( fi.baseName().toLatin1() );
	KCmdLineArgs::init(argc, argv, 
		new KAboutData( appName, programName,
			version, shortDescription, licenseType, copyrightStatement, text, 
			homePageAddress, bugsEmailAddress));

	int predefinedOptionsCount = 0;
	for (KCmdLineOptions *optionsPtr = predefinedOptions; optionsPtr->name; optionsPtr++, predefinedOptionsCount++)
		;
	int userOptionsCount = 0;
	for (KCmdLineOptions *optionsPtr = options; optionsPtr->name; optionsPtr++, userOptionsCount++)
		;

	d->componentData = KComponentData(appName);

	// join the predefined options and user options
	d->allOptions = new KCmdLineOptions[predefinedOptionsCount + userOptionsCount + 1];
	KCmdLineOptions *allOptionsPtr = d->allOptions;
	for (KCmdLineOptions *optionsPtr = predefinedOptions; optionsPtr->name; optionsPtr++, allOptionsPtr++) {
		allOptionsPtr->name = qstrdup(optionsPtr->name);
		allOptionsPtr->description = qstrdup(optionsPtr->description);
		if (optionsPtr == predefinedOptions) //first row == drv
			allOptionsPtr->def = qstrdup(KexiDB::Driver::defaultFileBasedDriverName().toLatin1());
		else
			allOptionsPtr->def = qstrdup(optionsPtr->def);
	}
	for (KCmdLineOptions *optionsPtr = options; optionsPtr->name; optionsPtr++, allOptionsPtr++) {
		allOptionsPtr->name = qstrdup(optionsPtr->name);
		allOptionsPtr->description = qstrdup(optionsPtr->description);
		allOptionsPtr->def = qstrdup(optionsPtr->def);
	}
	allOptionsPtr->name = 0; //end
	allOptionsPtr->description = 0;
	allOptionsPtr->def = 0;
	KCmdLineArgs::addCmdLineOptions( d->allOptions );

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	d->connData.driverName = args->getOption("driver");
	d->connData.userName = args->getOption("user");
	d->connData.hostName = args->getOption("host");
	d->connData.localSocketFileName = args->getOption("local-socket");
	d->connData.port = args->getOption("port").toInt();
	d->connData.useLocalSocketFile = args->isSet("local-socket");

	if (args->isSet("password")) {
		QString userAtHost = d->connData.userName;
		if (!d->connData.userName.isEmpty())
			userAtHost += "@";
		userAtHost += (d->connData.hostName.isEmpty() ? "localhost" : d->connData.hostName);
		QTextStream cout(stdout,IO_WriteOnly);
		cout << i18n("Enter password for %1: ", userAtHost);
//! @todo make use of pty/tty here! (and care about portability)
		QTextStream cin(stdin,QIODevice::ReadOnly);
		cin >> d->connData.password;
		KexiDBDbg << d->connData.password << endl;
	}
}

SimpleCommandLineApp::~SimpleCommandLineApp()
{
	closeDatabase();
	delete d;
}

bool SimpleCommandLineApp::openDatabase(const QString& databaseName)
{
	if (!d->conn) {
		if (d->manager.error()) {
			setError(&d->manager);
			return false;
		}

		//get the driver
		KexiDB::Driver *driver = d->manager.driver(d->connData.driverName);
		if (!driver || d->manager.error()) {
			setError(&d->manager);
			return false;
		}

		if (driver->isFileDriver())
			d->connData.setFileName( databaseName );

		d->conn = driver->createConnection(d->connData);
		if (!d->conn || driver->error()) {
			setError(driver);
			return false;
		}
	}
	if (d->conn->isConnected()) {
		// db already opened
		if (d->conn->isDatabaseUsed() && d->conn->currentDatabase()==databaseName) //the same: do nothing
			return true;
		if (!closeDatabase()) // differs: close the first
			return false;
	}
	if (!d->conn->connect()) {
		setError(d->conn);
		delete d->conn;
		d->conn = 0;
		return false;
	}

	if (!d->conn->useDatabase( databaseName )) {
		setError(d->conn);
		delete d->conn;
		d->conn = 0;
		return false;
	}
	return true;
}

bool SimpleCommandLineApp::closeDatabase()
{
	if (!d->conn)
		return true;
	if (!d->conn->disconnect()) {
		setError(d->conn);
		return false;
	}
	return true;
}

const KComponentData &SimpleCommandLineApp::componentData() const
{
	return d->componentData;
}

KexiDB::ConnectionData* SimpleCommandLineApp::connectionData() const
{
	return &d->connData;
}

KexiDB::Connection* SimpleCommandLineApp::connection() const
{
	return d->conn;
}
