/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#include "KexiPluginMetaData.h"
#include "KexiJsonTrader.h"

#include <QStringList>
#include <QDebug>
#include <QJsonArray>

class KexiPluginMetaData::Private
{
public:
    Private(KexiPluginMetaData *info, const QPluginLoader &loader)
     : rootObject(KexiJsonTrader::rootObjectForPluginLoader(loader))
    {
        QStringList v(info->version().split('.'));
        bool ok = v.count() >= 2;
        if (ok) {
            majorVersion = v[0].toInt(&ok);
        }
        if (ok) {
            minorVersion = v[1].toInt(&ok);
        }
        if (!ok) {
            majorVersion = 0;
            minorVersion = 0;
        }
    }

    QJsonObject rootObject;
    QString errorMessage;
    int majorVersion;
    int minorVersion;
};

//------------------------------

KexiPluginMetaData::KexiPluginMetaData(const QPluginLoader &loader)
    : KPluginMetaData(loader), d(new Private(this, loader))
{
}

KexiPluginMetaData::~KexiPluginMetaData()
{
    delete d;
}

QString KexiPluginMetaData::id() const
{
    return pluginId();
}

void KexiPluginMetaData::setErrorMessage(const QString& errorMessage)
{
    d->errorMessage = errorMessage;
}

QString KexiPluginMetaData::errorMessage() const
{
    return d->errorMessage;
}

int KexiPluginMetaData::majorVersion() const
{
    return d->majorVersion;
}

int KexiPluginMetaData::minorVersion() const
{
    return d->minorVersion;
}

QJsonObject KexiPluginMetaData::rootObject() const
{
    return d->rootObject;
}
