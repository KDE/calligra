/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KexiDBShortcutFile.h"
#include <core/kexiprojectdata.h>
#include <kexidb/connectiondata.h>

#include <kconfig.h>
#include <kdebug.h>

#include <qstringlist.h>

#define KexiDBShortcutFile_version 1

//! @internal
class KexiDBShortcutFile::Private
{
	public:
		Private()
		 : isDatabaseShortcut(true)
		{
		}
	QString fileName;
	bool isDatabaseShortcut : 1;
};

KexiDBShortcutFile::KexiDBShortcutFile( const QString& fileName )
 : d( new KexiDBShortcutFile::Private() )
{
	d->fileName = fileName;
}

KexiDBShortcutFile::~KexiDBShortcutFile()
{
	delete d;
}

bool KexiDBShortcutFile::loadProjectData(KexiProjectData& data, QString* _groupKey)
{
	KConfig config(d->fileName, true /* readOnly */, false /* local */ );
	config.setGroup("File Information");
	int version = config.readNumEntry("version", KexiDBShortcutFile_version);

	Q_UNUSED(version);

	QString groupKey;
	if (!_groupKey || _groupKey->isEmpty()) {
		QStringList groups(config.groupList());
		foreach (QStringList::ConstIterator, it, groups) {
			if ((*it).lower()!="file information") {
				groupKey = *it;
				break;
			}
		}
		if (groupKey.isEmpty()) {
			//ERR: "File %1 contains no connection information"
			return false;
		}
		if (_groupKey)
			*_groupKey = groupKey;
	}
	else {
		if (!config.hasGroup(*_groupKey))
			return false;
		groupKey = *_groupKey;
	}

	config.setGroup(groupKey);
	QString type( config.readEntry("type", "database").lower() );

	if (type=="database") {
		d->isDatabaseShortcut = true;
	} else if (type=="connection") {
		d->isDatabaseShortcut = false;
	}
	else {
		//ERR: i18n("No valid "type" field specified for section \"%1\": unknown value \"%2\".").arg(group).arg(type)
		return false;
	}

	kexidbg << "version=" << version 
		<< " using group key=" << groupKey 
		<< " type=" << type
		<< " caption=" << config.readEntry("caption")
		<< " name=" << config.readEntry("name")
		<< " engine=" << config.readEntry("engine")
		<< " server=" << config.readEntry("server")
		<< " user=" << config.readEntry("user")
		<< " password=" << QString().fill('*', config.readEntry("password").length())
		<< " comment=" << config.readEntry("comment")
		<< endl;

	if (d->isDatabaseShortcut) {
		data.setCaption( config.readEntry("caption") );
		data.setDescription( config.readEntry("comment") );
		data.connectionData()->description = QString::null;
		data.connectionData()->caption = QString::null; /* connection name is not specified... */
		data.setDatabaseName( config.readEntry("name") );
	}
	else {
		data.setCaption( QString::null );
		data.connectionData()->caption = config.readEntry("caption");
		data.setDescription( QString::null );
		data.connectionData()->description = config.readEntry("comment");
		data.setDatabaseName( QString::null ); /* db name is not specified... */
	}
	data.connectionData()->driverName = config.readEntry("engine");
	if (data.connectionData()->driverName.isEmpty()) {
		//ERR: "No valid "engine" field specified for %1 section" group
		return false;
	}
	data.connectionData()->hostName = config.readEntry("server"); //empty allowed
	data.connectionData()->port = config.readNumEntry("port", 0);
	data.connectionData()->useLocalSocketFile = config.readBoolEntry("useLocalSocketFile", false);
	data.connectionData()->localSocketFileName = config.readEntry("localSocketFile");
//UNSAFE!!!!
	data.connectionData()->password = config.readEntry("password");
	data.connectionData()->savePassword = !data.connectionData()->password.isEmpty();
	data.connectionData()->userName = config.readEntry("user");
/* @todo add "options=", eg. as string list? */
	return true;
}

bool KexiDBShortcutFile::saveProjectData(const KexiProjectData& data, 
	bool savePassword, QString* _groupKey, bool overwriteFirstGroup)
{
	KConfig config(d->fileName, false /*rw*/, false /* local */);
	config.setGroup("File Information");
	config.writeEntry("version", KexiDBShortcutFile_version);

	const bool thisIsConnectionData = data.databaseName().isEmpty();

	//use or find a nonempty group key
	QString groupKey;
	if (_groupKey && !_groupKey->isEmpty()) {
		groupKey = *_groupKey;
	}
	else {
		QString groupPrefix;
		const QStringList groups(config.groupList());
		if (overwriteFirstGroup && !groups.isEmpty()) {
//			groupKey = groups.first(); //found
			foreach (QStringList::ConstIterator, it, groups) {
				if ((*it).lower()!="file information") {
					groupKey = *it;
					break;
				}
			}
		}

		if (groupKey.isEmpty()) {
			//find a new unique name
			if (thisIsConnectionData)
				groupPrefix = "Connection%1"; //do not i18n!
			else
				groupPrefix = "Database%1"; //do not i18n!

			int number = 1;
			while (config.hasGroup(groupPrefix.arg(number))) //a new group key couldn't exist
				number++;
			groupKey = groupPrefix.arg(number);
		}
		if (_groupKey) //return this one (generated or found)
			*_groupKey = groupKey;
	}

	config.deleteGroup(groupKey);
	config.setGroup(groupKey);
	if (thisIsConnectionData) {
		config.writeEntry("type", "connection");
		config.writeEntry("caption", data.constConnectionData()->caption);
		if (!data.constConnectionData()->description.isEmpty())
			config.writeEntry("comment", data.constConnectionData()->description);
	}
	else {//database
		config.writeEntry("type", "database");
		config.writeEntry("caption", data.caption());
		config.writeEntry("name", data.databaseName());
		if (!data.description().isEmpty())
			config.writeEntry("comment", data.description());
	}

	config.writeEntry("engine", data.constConnectionData()->driverName);
	if (!data.constConnectionData()->hostName.isEmpty())
		config.writeEntry("server", data.constConnectionData()->hostName);

	if (data.constConnectionData()->port!=0)
		config.writeEntry("port", data.constConnectionData()->port);
	config.writeEntry("useLocalSocketFile", data.constConnectionData()->useLocalSocketFile);
	if (!data.constConnectionData()->localSocketFileName.isEmpty())
		config.writeEntry("localSocketFile", data.constConnectionData()->localSocketFileName);

//UNSAFE!!!! @todo check if user wants to store her password!
	if (!data.constConnectionData()->password.isEmpty())
		config.writeEntry("password", (savePassword || data.constConnectionData()->savePassword) ? data.constConnectionData()->password : QString::null);
	if (!data.constConnectionData()->userName.isEmpty())
		config.writeEntry("user", data.constConnectionData()->userName);
/* @todo add "options=", eg. as string list? */
	config.sync();
	return true;
}

QString KexiDBShortcutFile::fileName() const
{
	return d->fileName;
}

//---------------------------------------------

KexiDBConnShortcutFile::KexiDBConnShortcutFile( const QString& fileName )
 : KexiDBShortcutFile( fileName )
{
}

KexiDBConnShortcutFile::~KexiDBConnShortcutFile()
{
}

bool KexiDBConnShortcutFile::loadConnectionData(KexiDB::ConnectionData& data, QString* _groupKey)
{
	KexiProjectData pdata(data);
	if (!loadProjectData(pdata, _groupKey))
		return false;
	data = *pdata.connectionData();
	return true;
}

bool KexiDBConnShortcutFile::saveConnectionData(const KexiDB::ConnectionData& data, 
	bool savePassword, QString* groupKey, bool overwriteFirstGroup)
{
	KexiProjectData pdata(data);
	return saveProjectData(pdata, savePassword, groupKey, overwriteFirstGroup);
}

//---------------------------------------------

#if 0
/*! Loads connection data into \a data. */
bool KexiDBConnSetShortcutFiles::loadConnectionDataSet(KexiDBConnectionSet& set)
{
	set.clear();
//	QStringList dirs( KGlobal::dirs()->findDirs("data", "kexi/connections") );
//	kexidbg << dirs << endl;
	QStringList files( KGlobal::dirs()->findAllResources("data", "kexi/connections/*.kexic") );
//	//also try for capital file extension
//	files += KGlobal::dirs()->findAllResources("data", "kexi/connections/*.KEXIC");
	kexidbg << files << endl;

	foreach(QStringList::ConstIterator, it, files) {
		KexiDB::ConnectionData *data = new KexiDB::ConnectionData();
		KexiDBConnShortcutFile shortcutFile( *it );
		if (!shortcutFile.loadConnectionData(*data)) {
			delete data;
			continue;
		}
		set.addConnectionData(data);
	}
}


/*! Saves a set of connection data \a set to a shortcut files. 
 Existing files are overwritten with a new data. */
bool KexiDBConnSetShortcutFiles::saveConnectionDataSet(const KexiDBConnectionSet& set)
{
}

#endif
