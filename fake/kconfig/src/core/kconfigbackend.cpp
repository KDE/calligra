/*
   This file is part of the KDE libraries
   Copyright (c) 2006 Thomas Braxton <brax108@cox.net>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997-1999 Matthias Kalle Dalheimer <kalle@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "kconfigbackend.h"

#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QDebug>

#include "kconfig.h"
#include "kconfigini_p.h"
#include "kconfigdata.h"
#include "kconfigbackend.moc"

typedef KSharedPtr<KConfigBackend> BackendPtr;

class KConfigBackend::Private
{
public:
    qint64 size;
    QDateTime lastModified;
    QString localFileName;

    static QString whatSystem(const QString& /*fileName*/)
    {
        return QLatin1String("INI");
    }
};


void KConfigBackend::registerMappings(const KEntryMap& /*entryMap*/)
{
}

BackendPtr KConfigBackend::create(const QString& file, const QString& sys)
{
    //qDebug() << "creating a backend for file" << file << "with system" << sys;
    const QString system = (sys.isEmpty() ? Private::whatSystem(file) : sys);
    KConfigBackend* backend = 0;

#if 0 // TODO port to Qt5 plugin loading
    if (system.compare(QLatin1String("INI"), Qt::CaseInsensitive) != 0) {
        const QString constraint = QString::fromLatin1("[X-KDE-PluginInfo-Name] ~~ '%1'").arg(system);
        KService::List offers = KServiceTypeTrader::self()->query(QLatin1String("KConfigBackend"), constraint);

        //qDebug() << "found" << offers.count() << "offers for KConfigBackend plugins with name" << system;
        foreach (const KService::Ptr& offer, offers) {
            backend = offer->createInstance<KConfigBackend>(0);
            if (backend) {
                //qDebug() << "successfully created a backend for" << system;
                backend->setFilePath(file);
                return BackendPtr(backend);
            }
        } // foreach offers
    }
#endif

    //qDebug() << "default creation of the Ini backend";
    backend = new KConfigIniBackend;
    backend->setFilePath(file);
    return BackendPtr(backend);
}

KConfigBackend::KConfigBackend()
 : d(new Private)
{
}

KConfigBackend::~KConfigBackend()
{
    delete d;
}

QDateTime KConfigBackend::lastModified() const
{
    return d->lastModified;
}

void KConfigBackend::setLastModified(const QDateTime& dt)
{
    d->lastModified = dt;
}

qint64 KConfigBackend::size() const
{
    return d->size;
}

void KConfigBackend::setSize(qint64 sz)
{
    d->size = sz;
}

QString KConfigBackend::filePath() const
{
    return d->localFileName;
}

void KConfigBackend::setLocalFilePath(const QString& file)
{
    d->localFileName = file;
}
