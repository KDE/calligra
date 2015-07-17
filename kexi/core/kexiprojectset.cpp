/* This file is part of the KDE project
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#include <KDbDriver>
#include <KDbConnection>
#include <KDbMessageHandler>
#include <KDbDriverManager>

//! @internal
class KexiProjectSetPrivate
{
public:
    KexiProjectSetPrivate()
    {
    }
    ~KexiProjectSetPrivate()
    {
        qDeleteAll(list);
    }
    KexiProjectData::List list;
};

KexiProjectSet::KexiProjectSet(KDbMessageHandler* handler)
        : KDbResultable()
        , d(new KexiProjectSetPrivate())
{
    setMessageHandler(handler);
}

KexiProjectSet::~KexiProjectSet()
{
    delete d;
}

bool KexiProjectSet::setConnectionData(KDbConnectionData* conndata)
{
    Q_ASSERT(conndata);
    clearResult();
    qDeleteAll(d->list);
    d->list.clear();

    KDbMessageGuard mg(this);
    KDbDriver *drv = Kexi::driverManager().driver(conndata->driverId());
    if (!drv) {
        m_result = Kexi::driverManager().result();
        return false;
    }
    QStringList dbnames;
    QScopedPointer<KDbConnection> conn(drv->createConnection(*conndata));
    {
        if (!conn) {
            m_result = drv->result();
            return false;
        }
        if (!conn->connect()) {
            m_result = conn->result();
            return false;
        }
        dbnames = conn->databaseNames(false/*skip system*/);
        if (conn->result().isError()) {
            m_result = conn->result();
            return false;
        }
    }
    for (QStringList::ConstIterator it = dbnames.constBegin(); it != dbnames.constEnd(); ++it) {
        // project's caption is just the same as database name - nothing better is available
        KexiProjectData *pdata = new KexiProjectData(*conndata, *it, *it);
        addProjectData(pdata);
    }
    return true;
}

void KexiProjectSet::addProjectData(KexiProjectData *data)
{
    d->list.append(data);
}

KexiProjectData* KexiProjectSet::takeProjectData(KexiProjectData *data)
{
    return d->list.removeOne(data) ? data : 0;
}

KexiProjectData::List KexiProjectSet::list() const
{
    return d->list;
}

KexiProjectData* KexiProjectSet::findProject(const QString &dbName) const
{
    foreach(KexiProjectData* data, d->list) {
        if (0 == QString::compare(data->databaseName(), dbName, Qt::CaseInsensitive)) {
            return data;
        }
    }
    return 0;
}
