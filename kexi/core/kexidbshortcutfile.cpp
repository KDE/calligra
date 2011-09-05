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

#include "kexidbshortcutfile.h"
#include <core/kexiprojectdata.h>
#include <kexidb/connectiondata.h>
#include <kexiutils/utils.h>
#include <kexi_global.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include <QStringList>
#include <QDir>

//! Version of the KexiDBShortcutFile format.
#define KexiDBShortcutFile_version 3
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

//! @internal
class KexiDBShortcutFile::Private
{
public:
    Private()
            : isDatabaseShortcut(true) {
    }
    QString fileName;
    bool isDatabaseShortcut;
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
    if (!pdata.load(fileName(), _groupKey))
        return false;
    data = *pdata.connectionData();
    return true;
}

bool KexiDBConnShortcutFile::saveConnectionData(const KexiDB::ConnectionData& data,
        bool savePassword, QString* groupKey, bool overwriteFirstGroup)
{
    KexiProjectData pdata(data);
    return pdata.save(fileName(), savePassword, groupKey, overwriteFirstGroup);
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
