/* This file is part of the KDE project
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbconnectionset.h"
#include "kexidbshortcutfile.h"

#include <KLocalizedString>

#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QHash>
#include <QStandardPaths>
#include <QDir>

//! @internal
class KexiDBConnectionSetPrivate
{
public:
    KexiDBConnectionSetPrivate()
            : maxid(-1) {
    }
    QList<KDbConnectionData*> list;
    QHash<QString, QString> filenamesForData;
    QHash<QString, KDbConnectionData*> dataForFilenames;
    int maxid;
};

KexiDBConnectionSet::KexiDBConnectionSet()
        : QObject()
        , d(new KexiDBConnectionSetPrivate())
{
}

KexiDBConnectionSet::~KexiDBConnectionSet()
{
    delete d;
}

bool KexiDBConnectionSet::addConnectionData(KDbConnectionData *data, const QString& _filename)
{
    if (!data)
        return false;
/*! @todo KEXI3
    if (data->id < 0)
        data->id = d->maxid + 1;
    @todo check for id-duplicates
    d->maxid = qMax(d->maxid, data->id);
*/
    d->maxid++;

    QString filename(_filename);
    bool generateUniqueFilename = filename.isEmpty()
        || (!filename.isEmpty() && data == d->dataForFilenames.value(filename));

    if (generateUniqueFilename) {
        QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                      + "/kexi/connections/";
        if (dir.isEmpty()) {
            m_result.setMessage(xi18n("Could not find location to save connection data file."));
            return false;
        }
        QString baseFilename(dir + (data->hostName().isEmpty() ? "localhost" : data->hostName()));
        int i = 0;
        while (QFile::exists(baseFilename + (i > 0 ? QString::number(i) : QString()) + ".kexic")) {
            i++;
        }
        if (!QDir(dir).exists()) {
            //make 'connections' dir and protect it
            //! @todo KEXI3 set permission of the created dirs to 0700
            if (!QDir().mkpath(dir)) {
                m_result.setMessage(xi18n("Could not create folder <filename>%1</filename> for connection data file.", dir));
                return false;
            }
            //! @todo change permission of every created subdir, see KStandardDirs::makeDir() create
            QFile(dir).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
        }
        filename = baseFilename + (i > 0 ? QString::number(i) : QString()) + ".kexic";
    }
    addConnectionDataInternal(data, filename);
    bool result = saveConnectionData(data, *data);
    if (!result)
        removeConnectionDataInternal(data);
    return result;
}

void KexiDBConnectionSet::addConnectionDataInternal(KDbConnectionData *data, const QString& filename)
{
    d->filenamesForData.insert(key(*data), filename);
    d->dataForFilenames.insert(filename, data);
    d->list.append(data);
}

bool KexiDBConnectionSet::saveConnectionData(KDbConnectionData *oldData,
        const KDbConnectionData &newData)
{
    if (!oldData)
        return false;
    const QString oldDataKey = key(*oldData);
    const QString filename(d->filenamesForData.value(oldDataKey));
    if (filename.isEmpty()) {
        m_result.setCode(ERR_OTHER);
        return false;
    }
    KexiDBConnShortcutFile shortcutFile(filename);
    if (!shortcutFile.saveConnectionData(newData, newData.savePassword())) {
        m_result = shortcutFile.result();
        return false;
    }
    if (oldData != &newData) {
        *oldData = newData;
    }
    const QString newDataKey = key(newData);
    if (oldDataKey != newDataKey) {
        // update file info: key changed, filename is untouched
        d->filenamesForData.remove(oldDataKey);
        d->filenamesForData.insert(newDataKey, filename);
    }
    return true;
}

void KexiDBConnectionSet::removeConnectionDataInternal(KDbConnectionData *data)
{
    const QString filename(d->filenamesForData.value(key(*data)));
    d->filenamesForData.remove(key(*data));
    d->dataForFilenames.remove(filename);
    d->list.removeAt(d->list.indexOf(data));
    delete data;
}

bool KexiDBConnectionSet::removeConnectionData(KDbConnectionData *data)
{
    if (!data)
        return false;
    const QString filename(d->filenamesForData.value(key(*data)));
    if (filename.isEmpty()) {
        m_result.setCode(ERR_OTHER);
        return false;
    }
    QFile file(filename);
    if (!file.remove()) {
        m_result.setMessage(xi18n("Could not remove connection file <filename>%1</filename>.",
                                  filename));
        return false;
    }
    removeConnectionDataInternal(data);
    return true;
}

QList<KDbConnectionData*> KexiDBConnectionSet::list() const
{
    return d->list;
}

void KexiDBConnectionSet::clear()
{
    d->list.clear();
    d->filenamesForData.clear();
    d->dataForFilenames.clear();
}

void KexiDBConnectionSet::load()
{
    clear();
    const QStringList dirs(QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                     "kexi/connections",
                                                     QStandardPaths::LocateDirectory));
    foreach(const QString &dir, dirs) {
        QDirIterator it(dir, QStringList() << "*.kexic", QDir::Files | QDir::Readable,
                        QDirIterator::FollowSymlinks);
        while (it.hasNext()) {
            KDbConnectionData *data = new KDbConnectionData();
            KexiDBConnShortcutFile shortcutFile(it.next());
            if (!shortcutFile.loadConnectionData(data)) {
                delete data;
                continue;
            }
            addConnectionDataInternal(data, it.filePath());
            //qDebug() << file << "added.";
        }
   }
}

QString KexiDBConnectionSet::fileNameForConnectionData(const KDbConnectionData &data) const
{
    return d->filenamesForData.value(key(data));
}

KDbConnectionData* KexiDBConnectionSet::connectionDataForFileName(const QString& fileName) const
{
    return d->dataForFilenames.value(fileName);
}

// static
QString KexiDBConnectionSet::key(const KDbConnectionData &data)
{
    return data.driverId().toLower() + ','
        + data.userName().toLower() + ','
        + data.hostName().toLower() + ','
        + QString::number(data.port()) + ','
        + QString::number(data.useLocalSocketFile()) + ','
        + data.localSocketFileName() + ','
        + data.databaseName().replace(',', "\\,") + ','
        + QString::number(data.savePassword() ? 1 : 0) + ','
        + data.caption().replace(',', "\\,") + ','
        + data.description().replace(',', "\\,");
}
