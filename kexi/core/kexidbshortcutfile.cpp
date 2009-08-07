/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbshortcutfile.h"
#include <core/kexiprojectdata.h>
#include <kexidb/connectiondata.h>
#include <kexiutils/utils.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include <qstringlist.h>
#include <qdir.h>

#include <kexi_global.h>

//! Version of the KexiDBShortcutFile format.
#define KexiDBShortcutFile_version 2
/* CHANGELOG:
 v1: initial version
 v2: "encryptedPassword" field added.
     For backward compatibility, it is not used if the connection data has been loaded from
     a file saved with version 1. In such cases unencrypted "password" field is used.
*/

//! @internal
class KexiDBShortcutFile::Private
{
public:
    Private()
            : isDatabaseShortcut(true) {
    }
    QString fileName;
    bool isDatabaseShortcut : 1;
};

KexiDBShortcutFile::KexiDBShortcutFile(const QString& fileName)
        : d(new KexiDBShortcutFile::Private())
{
    d->fileName = QDir(fileName).absolutePath();
}

KexiDBShortcutFile::~KexiDBShortcutFile()
{
    delete d;
}

bool KexiDBShortcutFile::loadProjectData(KexiProjectData& data, QString* _groupKey)
{
#ifdef __GNUC__
#warning KexiDBShortcutFile::loadProjectData: how about readOnly arg?
#else
#pragma WARNING( KexiDBShortcutFile::loadProjectData: how about readOnly arg? )
#endif
    KConfig config(d->fileName, KConfig::SimpleConfig);
    KConfigGroup cg = config.group("File Information");
    data.formatVersion = cg.readEntry("version", KexiDBShortcutFile_version);

    QString groupKey;
    if (!_groupKey || _groupKey->isEmpty()) {
        QStringList groups(config.groupList());
        foreach(QString s, groups) {
            if (s.toLower() != "file information") {
                groupKey = s;
                break;
            }
        }
        if (groupKey.isEmpty()) {
            //ERR: "File %1 contains no connection information"
            return false;
        }
        if (_groupKey)
            *_groupKey = groupKey;
    } else {
        if (!config.hasGroup(*_groupKey))
            return false;
        groupKey = *_groupKey;
    }

    cg = config.group(groupKey);
    QString type(cg.readEntry("type", "database").toLower());

    if (type == "database") {
        d->isDatabaseShortcut = true;
    } else if (type == "connection") {
        d->isDatabaseShortcut = false;
    } else {
        //ERR: i18n("No valid "type" field specified for section \"%1\": unknown value \"%2\".").arg(group).arg(type)
        return false;
    }

    /* kDebug() << "version=" << version
        << " using group key=" << groupKey
        << " type=" << type
        << " caption=" << cg.readEntry("caption")
        << " name=" << cg.readEntry("name")
        << " engine=" << cg.readEntry("engine")
        << " server=" << cg.readEntry("server")
        << " user=" << cg.readEntry("user")
        << " password=" << QString().fill('*', cg.readEntry("password").length())
        << " comment=" << cg.readEntry("comment")
;*/

    //no filename by default
    data.connectionData()->setFileName(QString());

    if (d->isDatabaseShortcut) {
        data.setCaption(cg.readEntry("caption"));
        data.setDescription(cg.readEntry("comment"));
        data.connectionData()->description.clear();
        data.connectionData()->caption.clear(); /* connection name is not specified... */
        data.setDatabaseName(cg.readEntry("name"));
    } else {
        data.setCaption(QString());
        data.connectionData()->caption = cg.readEntry("caption");
        data.setDescription(QString());
        data.connectionData()->description = cg.readEntry("comment");
        data.setDatabaseName(QString());   /* db name is not specified... */
    }
    data.connectionData()->driverName = cg.readEntry("engine");
    if (data.connectionData()->driverName.isEmpty()) {
        //ERR: "No valid "engine" field specified for %1 section" group
        return false;
    }
    data.connectionData()->hostName = cg.readEntry("server"); //empty allowed
    data.connectionData()->port = cg.readEntry("port", 0);
    data.connectionData()->useLocalSocketFile = cg.readEntry("useLocalSocketFile", false);
    data.connectionData()->localSocketFileName = cg.readEntry("localSocketFile");
    data.connectionData()->savePassword = cg.hasKey("password") || cg.hasKey("encryptedPassword");
    if (data.formatVersion >= 2) {
        kDebug() << cg.hasKey("encryptedPassword");
        data.connectionData()->password = cg.readEntry("encryptedPassword");
        KexiUtils::simpleDecrypt(data.connectionData()->password);
    }
    if (data.connectionData()->password.isEmpty()) {//no "encryptedPassword", for compatibility
        //UNSAFE
        data.connectionData()->password = cg.readEntry("password");
    }
// data.connectionData()->savePassword = !data.connectionData()->password.isEmpty();
    data.connectionData()->userName = cg.readEntry("user");
    /* @todo add "options=", eg. as string list? */
    return true;
}

bool KexiDBShortcutFile::saveProjectData(const KexiProjectData& data,
        bool savePassword, QString* _groupKey, bool overwriteFirstGroup)
{
#ifdef __GNUC__
#warning KexiDBShortcutFile::saveProjectData: how about readOnly arg?
#else
#pragma WARNING( KexiDBShortcutFile::saveProjectData: how about readOnly arg? )
#endif
    KConfig config(d->fileName, KConfig::SimpleConfig);
    KConfigGroup cg = config.group("File Information");

    uint realFormatVersion = data.formatVersion;
    if (realFormatVersion == 0) /* 0 means "default version"*/
        realFormatVersion = KexiDBShortcutFile_version;
    cg.writeEntry("version", realFormatVersion);

    const bool thisIsConnectionData = data.databaseName().isEmpty();

    //use or find a nonempty group key
    QString groupKey;
    if (_groupKey && !_groupKey->isEmpty()) {
        groupKey = *_groupKey;
    } else {
        QString groupPrefix;
        const QStringList groups(config.groupList());
        if (overwriteFirstGroup && !groups.isEmpty()) {
//   groupKey = groups.first(); //found
            foreach(QString s, groups) {
                if (s.toLower() != "file information") {
                    groupKey = s;
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

    config.group(groupKey).deleteGroup();
    cg = config.group(groupKey);
    if (thisIsConnectionData) {
        cg.writeEntry("type", "connection");
        cg.writeEntry("caption", data.constConnectionData()->caption);
        if (!data.constConnectionData()->description.isEmpty())
            cg.writeEntry("comment", data.constConnectionData()->description);
    } else {//database
        cg.writeEntry("type", "database");
        cg.writeEntry("caption", data.caption());
        cg.writeEntry("name", data.databaseName());
        if (!data.description().isEmpty())
            cg.writeEntry("comment", data.description());
    }

    cg.writeEntry("engine", data.constConnectionData()->driverName);
    if (!data.constConnectionData()->hostName.isEmpty())
        cg.writeEntry("server", data.constConnectionData()->hostName);

    if (data.constConnectionData()->port != 0)
        cg.writeEntry("port", int(data.constConnectionData()->port));
    cg.writeEntry("useLocalSocketFile", data.constConnectionData()->useLocalSocketFile);
    if (!data.constConnectionData()->localSocketFileName.isEmpty())
        cg.writeEntry("localSocketFile", data.constConnectionData()->localSocketFileName);

    if (savePassword || data.constConnectionData()->savePassword) {
        if (realFormatVersion < 2) {
            cg.writeEntry("password", data.constConnectionData()->password);
        } else {
            QString encryptedPassword = data.constConnectionData()->password;
            KexiUtils::simpleCrypt(encryptedPassword);
            cg.writeEntry("encryptedPassword", encryptedPassword);
            encryptedPassword.fill(' '); //for security
        }
    }

    if (!data.constConnectionData()->userName.isEmpty())
        cg.writeEntry("user", data.constConnectionData()->userName);
    /* @todo add "options=", eg. as string list? */
    cg.sync();
    return true;
}

QString KexiDBShortcutFile::fileName() const
{
    return d->fileName;
}

//---------------------------------------------

KexiDBConnShortcutFile::KexiDBConnShortcutFile(const QString& fileName)
        : KexiDBShortcutFile(fileName)
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
// QStringList dirs( KGlobal::dirs()->findDirs("data", "kexi/connections") );
// kDebug() << dirs;
    QStringList files(KGlobal::dirs()->findAllResources("data", "kexi/connections/*.kexic"));
// //also try for capital file extension
// files += KGlobal::dirs()->findAllResources("data", "kexi/connections/*.KEXIC");
    kDebug() << files;

    foreach(QStringList::ConstIterator, it, files) {
        KexiDB::ConnectionData *data = new KexiDB::ConnectionData();
        KexiDBConnShortcutFile shortcutFile(*it);
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
