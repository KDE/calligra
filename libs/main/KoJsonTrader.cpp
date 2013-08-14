/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright     2007       David Faure <faure@kde.org>

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

#include "KoJsonTrader.h"

#include <kdebug.h>

#include <QList>
#include <QPluginLoader>
#include <QJsonObject>
#include <QDirIterator>

KoJsonTrader::KoJsonTrader()
{
}

KoJsonTrader* KoJsonTrader::self()
{
    static KoJsonTrader *s_instance = 0;
    if (!s_instance)
        s_instance = new KoJsonTrader();
    return s_instance;
}

QList<QPluginLoader *> KoJsonTrader::query(const QString &servicetype, const QString &constraint) const
{
    Q_UNUSED(servicetype);
    Q_UNUSED(constraint);
    QList<QPluginLoader *>list;
    QDirIterator dirIter(QCoreApplication::applicationDirPath()+"/../lib/", QDirIterator::Subdirectories);
    while (dirIter.hasNext()) {
        dirIter.next();
        QPluginLoader *loader = new QPluginLoader(dirIter.filePath());
        QJsonObject json = loader->metaData();
        if (!json.isEmpty()) {
            list.append(loader);
            qDebug() << "plugin: " << dirIter.fileName();
        }

    }

    return list;
}
