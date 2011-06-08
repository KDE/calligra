/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#include "KexiRecentProjects.h"

#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/msghandler.h>
#include <core/kexidbshortcutfile.h>

#include <KStandardDirs>
#include <KDebug>

#include <QDir>

//! @internal
class KexiRecentProjects::Private
{
public:
    Private()
        : loaded(false)
    {
    }
    void load();
    bool loaded;
    QString path;
};

void KexiRecentProjects::Private::load()
{
    if (loaded)
        return;
    loaded = true;
    
    path = KStandardDirs::locateLocal("data", "kexi/recent_projects/",
                                              true /*create dir*/);
    QDir dir(path);
    if (!dir.exists() || !dir.isReadable()) {
        return;
    }
    QStringList shortcutPaths = dir.entryList(
        QStringList() << QLatin1String("*.kexic"),
        QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::CaseSensitive);
    foreach(const QString& shortcutPath, shortcutPaths) {
        kDebug() << shortcutPath;
    }
}

KexiRecentProjects::KexiRecentProjects(KexiDB::MessageHandler* handler)
    : KexiProjectSet(handler)
    , d(new Private)
{
}

KexiRecentProjects::~KexiRecentProjects()
{
    delete d;
}

void KexiRecentProjects::addProjectData(KexiProjectData *data)
{
    d->load();
    KexiProjectSet::addProjectData(data);
    kDebug() << d->path;
    QString shortcutPath = d->path + data->databaseName();
    QFileInfo fi(shortcutPath);
    if (!fi.suffix().isEmpty()) {
        shortcutPath.chop(fi.suffix().length() + 1);
    }
    shortcutPath += ".kexis";
    KexiDBShortcutFile shortcut(shortcutPath);
    bool result = shortcut.saveProjectData(*data, false // !savePassword
                                          );
    kDebug() << "result:" << result;
}

KexiProjectData::List KexiRecentProjects::list() const
{
    d->load();
    return KexiProjectSet::list();
}
