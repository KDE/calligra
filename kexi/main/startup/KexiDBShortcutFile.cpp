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

#include <kconfig.h>
#include <kdebug.h>

#include <qstringlist.h>

#define KexiDBShortcutFile_version 1

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

bool KexiDBShortcutFile::loadConnectionData(KexiProjectData& data, QString* _groupKey)
{
	KConfig config(d->fileName, true /* readOnly */, false /* local */ );
	config.setGroup("File Information");
	int version = config.readNumEntry("version", KexiDBShortcutFile_version);

	Q_UNUSED(version);

	QString groupKey;
	if (!_groupKey || _groupKey->isEmpty()) {
		QStringList groups(config.groupList());
		for (QStringList::ConstIterator it = groups.constBegin(); it!=groups.constEnd(); ++it) {
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
		d->isDatabaseShortcut = true;
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
		<< " password=" << config.readEntry("password")
		<< " comment=" << config.readEntry("comment")
		<< endl;

	if (d->isDatabaseShortcut) {
		data.setCaption( config.readEntry("caption") );
		data.setDescription( config.readEntry("comment") );
		data.connectionData()->description = QString::null;
		data.connectionData()->connName = QString::null; /* connection name is not specified... */
		data.setDatabaseName( config.readEntry("name") );
	}
	else {
		data.setCaption( QString::null );
		data.connectionData()->connName = config.readEntry("caption");
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
	data.connectionData()->userName = config.readEntry("user");
/* @todo add "options=", eg. as string list? */
	return true;
}

bool KexiDBShortcutFile::saveConnectionData(const KexiProjectData& data, 
	bool savePassword, QString* _groupKey)
{
	KConfig config(d->fileName, false /*rw*/, false /* local */);
	config.setGroup("File Information");
	config.writeEntry("version", KexiDBShortcutFile_version);

	//use or find a nonempty group key
	QString groupKey;
	if (_groupKey && !_groupKey->isEmpty()) {
		groupKey = *_groupKey;
	}
	else {
		QString groupPrefix;
		if (data.databaseName().isEmpty())
			groupPrefix = "Connection%1"; //do not i18n!
		else
			groupPrefix = "Database%1"; //do not i18n!

		int number = 1;
		while (config.hasGroup(groupPrefix.arg(number))) //a new group key couldn't exist
			number++;
		groupKey = groupPrefix.arg(number);
		if (_groupKey) //return this one (generated)
			*_groupKey = groupKey;
	}

	config.deleteGroup(groupKey);
	config.setGroup(groupKey);
	if (!data.databaseName().isEmpty()) {//database
		config.writeEntry("type", "database");
		config.writeEntry("caption", data.caption());
		config.writeEntry("name", data.databaseName());
	}
	else {
		config.writeEntry("type", "connection");
		config.writeEntry("caption", data.constConnectionData()->connName);
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
		config.writeEntry("password", savePassword ? data.constConnectionData()->password : QString::null);
	if (!data.description().isEmpty())
		config.writeEntry("comment", data.description());
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
