/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
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

#include "kexiprojectset.h"
#include "kexi.h"

#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/msghandler.h>

#include <kdebug.h>

//#define ERRMSG(a1, a2)
// { if (m_msgHandler) m_msgHandler->showErrorMessage(a1, a2); }

//! @internal
class KexiProjectSetPrivate
{
public:
    KexiProjectSetPrivate() {
    }
    KexiProjectData::List list;
// KexiDB::MessageHandler* msgHandler;
};

KexiProjectSet::KexiProjectSet(KexiDB::MessageHandler* handler)
        : KexiDB::Object(handler)
        , d(new KexiProjectSetPrivate())
{
}

KexiProjectSet::KexiProjectSet(KexiDB::ConnectionData &conndata,
                               KexiDB::MessageHandler* handler)
        : KexiDB::Object(handler)
        , d(new KexiProjectSetPrivate())
{
    KexiDB::Driver *drv = Kexi::driverManager().driver(conndata.driverName);
    if (!drv) {
        setError(&Kexi::driverManager());
        return;
    }
    KexiDB::Connection *conn = drv->createConnection(conndata);
    if (!conn) {
        setError(drv);
        return;
    }
    if (!conn->connect()) {
        setError(conn);
        delete conn;
        return;
    }
    QStringList dbnames = conn->databaseNames(false/*skip system*/);
// kDebug() << dbnames.count();
    if (conn->error()) {
        setError(conn);
        delete conn;
        return;
    }
    delete conn;
    conn = 0;
    for (QStringList::ConstIterator it = dbnames.constBegin(); it != dbnames.constEnd(); ++it) {
        // project's caption is just the same as database name - nothing better is available
        KexiProjectData *pdata = new KexiProjectData(conndata, *it, *it);
        d->list.append(pdata);
    }
    clearError();
}


KexiProjectSet::~KexiProjectSet()
{
    delete d;
}

void KexiProjectSet::addProjectData(KexiProjectData *data)
{
    d->list.append(data);
}

KexiProjectData::List KexiProjectSet::list() const
{
    return d->list;
}

KexiProjectData* KexiProjectSet::findProject(const QString &dbName) const
{
    const QString _dbName(dbName.toLower());
    foreach(KexiProjectData* data, d->list) {
        if (data->databaseName().toLower() == _dbName)
            return data;
    }
    return 0;
}
