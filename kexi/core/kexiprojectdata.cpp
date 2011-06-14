/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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

#include "kexiprojectdata.h"

#include <sys/types.h>
#include <unistd.h>

#include <qdom.h>
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kurl.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <kexidb/drivermanager.h>

//! @internal
class KexiProjectDataPrivate
{
public:
    KexiProjectDataPrivate()
            : userMode(false)
            , readOnly(false) {}

    KexiDB::ConnectionData connData;
    QDateTime lastOpened;
    bool userMode;
    bool readOnly;
};

//---------------------------------------

KexiProjectData::AutoOpenObjects::AutoOpenObjects()
        : QList<ObjectInfo*>()
{
}

KexiProjectData::AutoOpenObjects::AutoOpenObjects(const KexiProjectData::AutoOpenObjects& other)
        : QList<ObjectInfo*>()
{
    *this = other;
}

KexiProjectData::AutoOpenObjects::~AutoOpenObjects()
{
    qDeleteAll(*this);
}

KexiProjectData::AutoOpenObjects& KexiProjectData::AutoOpenObjects::operator=(
    const KexiProjectData::AutoOpenObjects & other)
{
    clear();
    for (QListIterator<ObjectInfo*> it(other);it.hasNext();) //deep copy
        append(new ObjectInfo(*it.next()));
    return *this;
}

//---------------------------------------

KexiProjectData::KexiProjectData()
        : QObject(0)
        , KexiDB::SchemaData()
        , formatVersion(0)
        , d(new KexiProjectDataPrivate())
{
    setObjectName("KexiProjectData");
}

KexiProjectData::KexiProjectData(
    const KexiDB::ConnectionData &cdata, const QString& dbname, const QString& caption)
        : QObject(0)
        , KexiDB::SchemaData()
        , formatVersion(0)
        , d(new KexiProjectDataPrivate())
{
    setObjectName("KexiProjectData");
    d->connData = cdata;
    setDatabaseName(dbname);
    setCaption(caption);
}

KexiProjectData::KexiProjectData(const KexiProjectData& pdata)
        : QObject(0)
        , KexiDB::SchemaData()
        , d(new KexiProjectDataPrivate())
{
    setObjectName("KexiProjectData");
    *this = pdata;
    autoopenObjects = pdata.autoopenObjects;
    /*
      d->connData = *pdata.connectionData();
      setDatabaseName(pdata.databaseName());
      setCaption(pdata.caption());*/
}

KexiProjectData::~KexiProjectData()
{
    delete d;
}

KexiProjectData& KexiProjectData::operator=(const KexiProjectData & pdata)
{
//    delete d; //this is old
    static_cast<KexiDB::SchemaData&>(*this) = static_cast<const KexiDB::SchemaData&>(pdata);
    //deep copy
    autoopenObjects = pdata.autoopenObjects;
    formatVersion = pdata.formatVersion;
    *d = *pdata.d;
// d->connData = *pdata.constConnectionData();
// setDatabaseName(pdata.databaseName());
// setCaption(pdata.caption());
// setDescription(pdata.description());
    return *this;
}

KexiDB::ConnectionData* KexiProjectData::connectionData()
{
    return &d->connData;
}

const KexiDB::ConnectionData* KexiProjectData::constConnectionData() const
{
    return &d->connData;
}

QString KexiProjectData::databaseName() const
{
    return KexiDB::SchemaData::name();
}

void KexiProjectData::setDatabaseName(const QString& dbName)
{
    kDebug() << dbName;
    kDebug() << *this;
    KexiDB::SchemaData::setName(dbName);
}

bool KexiProjectData::userMode() const
{
    return d->userMode;
}

QDateTime KexiProjectData::lastOpened() const
{
    return d->lastOpened;
}

void KexiProjectData::setLastOpened(const QDateTime& lastOpened)
{
    d->lastOpened = lastOpened;

}
QString KexiProjectData::description() const
{
    return KexiDB::SchemaData::description();
}

void KexiProjectData::setDescription(const QString& desc)
{
    return KexiDB::SchemaData::setDescription(desc);
}

QString KexiProjectData::infoString(bool nobr) const
{
    if (d->connData.fileName().isEmpty()) {
        //server-based
        return QString(nobr ? "<nobr>" : "") + QString("\"%1\"").arg(databaseName()) + (nobr ? "</nobr>" : "")
               + (nobr ? " <nobr>" : " ") + i18nc("database connection", "(connection %1)",
                                                  d->connData.serverInfoString()) + (nobr ? "</nobr>" : "");
    }
    //file-based
    return QString(nobr ? "<nobr>" : "")
           + QString("\"%1\"").arg(d->connData.fileName()) + (nobr ? "</nobr>" : "");
}

void KexiProjectData::setReadOnly(bool set)
{
    d->readOnly = set;
}

bool KexiProjectData::isReadOnly() const
{
    return d->readOnly;
}

/* This file is part of the KDE project
   Copyright (C) 2005-2011 Jarosław Staniek <staniek@kde.org>

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

#include "kexiprojectdata.h"
#include <kexidb/connectiondata.h>
#include <kexiutils/utils.h>
#include <kexi_global.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include <QStringList>
#include <QDir>

//! Version of the KexiProjectData format.
#define KEXIPROJECTDATA_FORMAT 3
/* CHANGELOG:
 v1: initial version
 v2: "encryptedPassword" field added.
     For backward compatibility, it is not used if the connection data has been loaded from
     a file saved with version 1. In such cases unencrypted "password" field is used.
 v3: "name" for shortcuts to file-based databases is a full file path.
     If the file is within the user's home directory, the dir is replaced with $HOME,
     e.g. name=$HOME/mydb.kexi. Not compatible with earlier versions but in these
     versions only filename was stored so the file was generally inaccessible anyway.
     "lastOpened" field added of type date/time (ISO format).
*/

bool KexiProjectData::load(const QString& fileName, QString* _groupKey)
{
#ifdef __GNUC__
#warning KexiProjectData::load: how about readOnly arg?
#else
#pragma WARNING( KexiProjectData::load: how about readOnly arg? )
#endif
    KConfig config(fileName, KConfig::SimpleConfig);
    KConfigGroup cg = config.group("File Information");
    uint _formatVersion = cg.readEntry("version", KEXIPROJECTDATA_FORMAT);

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

    bool isDatabaseShortcut;
    if (type == "database") {
        isDatabaseShortcut = true;
    } else if (type == "connection") {
        isDatabaseShortcut = false;
    } else {
        //ERR: i18n("No valid "type" field specified for section \"%1\": unknown value \"%2\".").arg(group).arg(type)
        return false;
    }

    QString driverName = cg.readEntry("engine").toLower();
    if (driverName.isEmpty()) {
        //ERR: "No valid "engine" field specified for %1 section" group
        return false;
    }
    
    // verification OK, now applying the values:
    d->connData.driverName = driverName;
    formatVersion = _formatVersion;
    d->connData.hostName = cg.readEntry("server"); //empty allowed
    if (isDatabaseShortcut) {
        const bool fileBased = d->connData.hostName.isEmpty();
        setCaption(cg.readEntry("caption"));
        setDescription(cg.readEntry("comment"));
        d->connData.description.clear();
        d->connData.caption.clear(); /* connection name is not specified... */
        if (fileBased) {
            QString fn(cg.readEntry("name"));
            if (!fn.isEmpty()) {
                const QString homeVar("$HOME");
                if (fn.startsWith(homeVar)) {
                    QString home(QDir::homePath());
                    if (home.endsWith('/')) {
                        home.chop(1);
                    }
                    fn = home + fn.mid(homeVar.length());
                }
                d->connData.setFileName(fn);
                setDatabaseName(fn);
            }
        }
        else {
            setDatabaseName(cg.readEntry("name"));
        }
    } else { // connection
        d->connData.setFileName(QString());
        setCaption(QString());
        d->connData.caption = cg.readEntry("caption");
        setDescription(QString());
        d->connData.description = cg.readEntry("comment");
        setDatabaseName(QString());   /* db name is not specified... */
    }
    d->connData.port = cg.readEntry("port", 0);
    d->connData.useLocalSocketFile = cg.readEntry("useLocalSocketFile", true);
    d->connData.localSocketFileName = cg.readEntry("localSocketFile");
    d->connData.savePassword = cg.hasKey("password") || cg.hasKey("encryptedPassword");
    if (formatVersion >= 2) {
        kDebug() << cg.hasKey("encryptedPassword");
        d->connData.password = cg.readEntry("encryptedPassword");
        KexiUtils::simpleDecrypt(d->connData.password);
    }
    if (d->connData.password.isEmpty()) {//no "encryptedPassword", for compatibility
        //UNSAFE
        d->connData.password = cg.readEntry("password");
    }
// data.connectionData()->savePassword = !data.connectionData()->password.isEmpty();
    d->connData.userName = cg.readEntry("user");
    QString lastOpenedStr(cg.readEntry("lastOpened"));
    if (!lastOpenedStr.isEmpty()) {
        QDateTime lastOpened(QDateTime::fromString(lastOpenedStr, Qt::ISODate));
        if (lastOpened.isValid()) {
            setLastOpened(lastOpened);
        }
    }
    /* @todo add "options=", eg. as string list? */
    return true;
}

bool KexiProjectData::save(const QString& fileName, bool savePassword,
                           QString* _groupKey, bool overwriteFirstGroup)
{
#ifdef __GNUC__
#warning KexiProjectData::save: how about readOnly arg?
#else
#pragma WARNING( KexiProjectData::save: how about readOnly arg? )
#endif
    KConfig config(fileName, KConfig::SimpleConfig);
    KConfigGroup cg = config.group("File Information");

    uint realFormatVersion = formatVersion;
    if (realFormatVersion == 0) /* 0 means "default version"*/
        realFormatVersion = KEXIPROJECTDATA_FORMAT;
    cg.writeEntry("version", realFormatVersion);

    const bool thisIsConnectionData = databaseName().isEmpty();

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
    const bool fileBased = !d->connData.fileName().isEmpty();
    if (thisIsConnectionData) {
        cg.writeEntry("type", "connection");
        if (!d->connData.caption.isEmpty())
            cg.writeEntry("caption", d->connData.caption);
        if (!d->connData.description.isEmpty())
            cg.writeEntry("comment", d->connData.description);
    } else { //database
        cg.writeEntry("type", "database");
        if (!caption().isEmpty())
            cg.writeEntry("caption", caption());
        if (fileBased) {
            QString fn(d->connData.fileName());
            if (!QDir::homePath().isEmpty() && fn.startsWith(QDir::homePath())) {
                // replace prefix if == $HOME
                fn = fn.mid(QDir::homePath().length());
                if (!fn.startsWith('/'))
                    fn.prepend('/');
                fn.prepend(QLatin1String("$HOME"));
            }
            cg.writeEntry("name", fn);
        }
        else { // server-based
            cg.writeEntry("name", databaseName());
        }
        if (!description().isEmpty())
            cg.writeEntry("comment", description());
    }

    cg.writeEntry("engine", d->connData.driverName.toLower());
    if (!fileBased) {
        if (!d->connData.hostName.isEmpty())
            cg.writeEntry("server", d->connData.hostName);

        if (d->connData.port != 0)
            cg.writeEntry("port", int(d->connData.port));
        cg.writeEntry("useLocalSocketFile", d->connData.useLocalSocketFile);
        if (!d->connData.localSocketFileName.isEmpty())
            cg.writeEntry("localSocketFile", d->connData.localSocketFileName);
        if (!d->connData.userName.isEmpty())
            cg.writeEntry("user", d->connData.userName);
    }

    if (savePassword || d->connData.savePassword) {
        if (realFormatVersion < 2) {
            cg.writeEntry("password", d->connData.password);
        } else {
            QString encryptedPassword = d->connData.password;
            KexiUtils::simpleCrypt(encryptedPassword);
            cg.writeEntry("encryptedPassword", encryptedPassword);
            encryptedPassword.fill(' '); //for security
        }
    }

    if (lastOpened().isValid()) {
        cg.writeEntry("lastOpened", lastOpened().toString(Qt::ISODate));
    }

    /* @todo add "options=", eg. as string list? */
    cg.sync();
    return true;
}

KEXICORE_EXPORT QDebug operator<<(QDebug dbg, const KexiProjectData& d)
{
    dbg.space() << "KexiProjectData" << "databaseName=" << d.databaseName()
        << "lastOpened=" << d.lastOpened() << "description=" << d.description()
        << "driverName=" << d.constConnectionData()->driverName
        << "filename=" << d.constConnectionData()->fileName();
    return dbg.space();
}
