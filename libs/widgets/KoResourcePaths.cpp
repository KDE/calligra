/*
 * Copyright (c) 2015 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "KoResourcePaths.h"

#include <QGlobalStatic>
#include <QStringList>
#include <QHash>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QSet>


#ifdef Q_OS_WIN
static const Qt::CaseSensitivity cs = Qt::CaseInsensitive;
#else
static const Qt::CaseSensitivity cs = Qt::CaseSensitive;
#endif

class KoResourcePathsImpl
{
public:
    static QStandardPaths::StandardLocation mapTypeToQStandardPaths(const QString &type)
    {
        return
            type == QLatin1String("data") ?    QStandardPaths::GenericDataLocation :
            type == QLatin1String("config") ?  QStandardPaths::GenericConfigLocation :
            type == QLatin1String("cache") ?   QStandardPaths::CacheLocation :
            type == QLatin1String("tmp") ?     QStandardPaths::TempLocation :
            type == QLatin1String("appdata") ? QStandardPaths::DataLocation :
            type == QLatin1String("locale") ?  QStandardPaths::GenericDataLocation :
            /* default */                      QStandardPaths::GenericDataLocation;
    }

    KoResourcePathsImpl() = default;
    ~KoResourcePathsImpl() = default;

    void addResourceTypeInternal(const QString &type, const QString &basetype,
                                 const QString &relativeName, bool priority);

    void addResourceDirInternal(const QString &type, const QString &absdir, bool priority);

    QString findResourceInternal(const QString &type, const QString &fileName);

    QStringList findDirsInternal(const QString &type, const QString &relDir);

    QStringList findAllResourcesInternal(const QString &type,
                                         const QString &filter = QString(),
                                         KoResourcePaths::SearchOptions options = KoResourcePaths::NoSearchOptions) const;

    QStringList resourceDirsInternal(const QString &type);

    QString saveLocationInternal(const QString &type, const QString &suffix = QString(), bool create = true);

    QString locateLocalInternal(const QString &type, const QString &filename, bool createDir = false);

private:
    QHash<QString, QStringList> m_absolutes; // For each resource type, the list of absolute paths, from most local (most priority) to most global
    QHash<QString, QStringList> m_relatives; // Same with relative paths
};

void KoResourcePathsImpl::addResourceTypeInternal(const QString &type, const QString &basetype,
                                                  const QString &relativename,
                                                  bool priority)
{
    if (relativename.isEmpty()) return;

    QString copy = relativename;

    Q_ASSERT(basetype == "data");

    if (!copy.endsWith(QLatin1Char('/'))) {
        copy += QLatin1Char('/');
    }

    QStringList &rels = m_relatives[type]; // find or insert

    if (!rels.contains(copy, cs)) {
        if (priority) {
            rels.prepend(copy);
        } else {
            rels.append(copy);
        }
    }

    //qDebug() << "addResourceType: type" << type << "basetype" << basetype << "relativename" << relativename << "priority" << priority << m_relatives[type];
}

void KoResourcePathsImpl::addResourceDirInternal(const QString &type, const QString &absdir, bool priority)
{
    if (absdir.isEmpty() || type.isEmpty()) return;

    // find or insert entry in the map
    QString copy = absdir;
    if (!copy.endsWith(QLatin1Char('/'))) {
        copy += QLatin1Char('/');
    }

    QStringList &paths = m_absolutes[type];
    if (!paths.contains(copy, cs)) {
        if (priority) {
            paths.prepend(copy);
        } else {
            paths.append(copy);
        }
    }

    //qDebug() << "addResourceDir: type" << type << "absdir" << absdir << "priority" << priority << m_absolutes[type];
}

QString KoResourcePathsImpl::findResourceInternal(const QString &type, const QString &fileName)
{
    const QStandardPaths::StandardLocation location = mapTypeToQStandardPaths(type);
    QString resource = QStandardPaths::locate(location, fileName, QStandardPaths::LocateFile);
    if (resource.isEmpty()) {
        foreach(const QString &relative, m_relatives.value(type)) {
            resource = QStandardPaths::locate(location, relative + fileName, QStandardPaths::LocateFile);
            if (!resource.isEmpty()) {
                break;
            }
        }
    }
    if (resource.isEmpty()) {
        foreach(const QString &absolute, m_absolutes.value(type)) {
            const QString filePath = absolute + fileName;
            if (QFileInfo::exists(filePath)) {
                resource = filePath;
                break;
            }
        }
    }
    //Q_ASSERT(!resource.isEmpty());
    //qDebug() << "findResource: type" << type << "filename" << fileName << "resource" << resource;
    return resource;
}

QStringList KoResourcePathsImpl::findDirsInternal(const QString &type, const QString &relDir)
{
    const QStandardPaths::StandardLocation location = mapTypeToQStandardPaths(type);

    QStringList dirs = QStandardPaths::locateAll(location, relDir, QStandardPaths::LocateDirectory);

    foreach(const QString &relative, m_relatives.value(type)) {
        dirs << QStandardPaths::locateAll(location, relative + relDir, QStandardPaths::LocateDirectory);
    }

    foreach(const QString &absolute, m_absolutes.value(type)) {
        const QString dirPath = absolute + relDir;
        if (QDir(dirPath).exists()) {
            dirs << dirPath;
        }
    }

    //Q_ASSERT(!dirs.isEmpty());
    //qDebug() << "findDirs: type" << type << "relDir" << relDir<< "resource" << dirs;
    return dirs;
}


QStringList filesInDir(const QString &startdir, const QString & filter, bool noduplicates, bool recursive)
{
    //qDebug() << "filesInDir: startdir" << startdir << "filter" << filter << "noduplicates" << noduplicates << "recursive" << recursive;
    QStringList result;

    // First the entries in this path
    QStringList nameFilters;
    nameFilters << filter;
    const QStringList fileNames = QDir(startdir).entryList(nameFilters, QDir::Files | QDir::CaseSensitive, QDir::Name);
    //qDebug() << "\tFound:" << fileNames.size() << ":" << fileNames;
    Q_FOREACH (const QString &fileName, fileNames) {
        QString file = startdir + '/' + fileName;
        result << file;
    }

    // And then everything underneath, if recursive is specified
    if (recursive) {
        const QStringList entries = QDir(startdir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        Q_FOREACH (const QString &subdir, entries) {
            //qDebug() << "\tGoing to look in subdir" << subdir << "of" << startdir;
            result << filesInDir(startdir + '/' + subdir, filter, noduplicates, recursive);
        }
    }
    return result;
}

QStringList KoResourcePathsImpl::findAllResourcesInternal(const QString &type,
                                                          const QString &_filter,
                                                          KoResourcePaths::SearchOptions options) const
{
    //qDebug() << "=====================================================";

    bool noDuplicates = options & KoResourcePaths::NoDuplicates;
    bool recursive = options & KoResourcePaths::Recursive;

    //qDebug() << "findAllResources: type" << type << "filter" << _filter << "no dups" << noDuplicates << "recursive" << recursive;

    const QStandardPaths::StandardLocation location = mapTypeToQStandardPaths(type);

    const QStringList relatives = m_relatives.value(type);
    QString filter = _filter;
    QString prefix;

    // In cases where the filter  is like "color-schemes/*.colors" instead of "*.kpp", used with unregistgered resource types
    if (filter.indexOf('*') > 0) {
        prefix = filter.split('*').first();
        filter = '*' + filter.split('*')[1];
        //qDebug() << "Split up alias" << relatives << "filter" << filter;
    }

    QStringList resources;
    if (relatives.isEmpty()) {
        resources << QStandardPaths::locateAll(location, prefix + filter, QStandardPaths::LocateFile);
    }

    ////qDebug() << "\tresources from qstandardpaths:" << resources.size();


    foreach(const QString &relative, relatives) {
        //qDebug() << "\t\relative:" << relative;
        const QStringList dirs = QStandardPaths::locateAll(location, relative + prefix, QStandardPaths::LocateDirectory);
        QSet<QString> s = QSet<QString>::fromList(dirs);

        //qDebug() << "\t\tdirs:" << dirs;
        Q_FOREACH (const QString &dir, s) {
            resources << filesInDir(dir, filter, noDuplicates, recursive);
        }
    }

    foreach(const QString &absolute, m_absolutes.value(type)) {
        const QString dir = absolute + prefix;
        if (QDir(dir).exists()) {
            resources << filesInDir(dir, filter, noDuplicates, recursive);
        }
    }

    if (noDuplicates) {
        QSet<QString> s = QSet<QString>::fromList(resources);
        resources = s.toList();
    }

    //qDebug() << "\tresources also from aliases:" << resources.size();
    //qDebug() << "=====================================================";

    //Q_ASSERT(!resources.isEmpty());

    return resources;
}

QStringList KoResourcePathsImpl::resourceDirsInternal(const QString &type)
{
    //return KGlobal::dirs()->resourceDirs(type.toLatin1());
    QStringList resourceDirs;

    const QStandardPaths::StandardLocation location = mapTypeToQStandardPaths(type);
    foreach(const QString &relative, m_relatives.value(type)) {
        resourceDirs << QStandardPaths::locateAll(location, relative, QStandardPaths::LocateDirectory);
    }
    foreach(const QString &absolute, m_absolutes.value(type)) {
        if (QDir(absolute).exists()) {
            resourceDirs << absolute;
        }
    }
    //qDebug() << "resourceDirs: type" << type << resourceDirs;

    return resourceDirs;
}

QString KoResourcePathsImpl::saveLocationInternal(const QString &type, const QString &suffix, bool create)
{
    QString path = QStandardPaths::writableLocation(mapTypeToQStandardPaths(type)) + '/' + suffix;
    QDir d(path);
    if (!d.exists() && create) {
        d.mkpath(path);
    }
    //qDebug() << "saveLocation: type" << type << "suffix" << suffix << "create" << create << "path" << path;

    return path;
}

QString KoResourcePathsImpl::locateLocalInternal(const QString &type, const QString &filename, bool createDir)
{
    QString path = saveLocationInternal(type, "", createDir);
    //qDebug() << "locateLocal: type" << type << "filename" << filename << "CreateDir" << createDir << "path" << path;
    return path + '/' + filename;
}

Q_GLOBAL_STATIC(KoResourcePathsImpl, s_instance);


void KoResourcePaths::addResourceType(const char *type, const char *basetype,
                                      const QString &relativeName, bool priority)
{
    s_instance->addResourceTypeInternal(QString::fromLatin1(type), QString::fromLatin1(basetype), relativeName, priority);
}

void KoResourcePaths::addResourceDir(const char *type, const QString &dir, bool priority)
{
    s_instance->addResourceDirInternal(QString::fromLatin1(type), dir, priority);
}

QString KoResourcePaths::findResource(const char *type, const QString &fileName)
{
    return s_instance->findResourceInternal(QString::fromLatin1(type), fileName);
}

QStringList KoResourcePaths::findDirs(const char *type, const QString &reldir)
{
    return s_instance->findDirsInternal(QString::fromLatin1(type), reldir);
}

QStringList KoResourcePaths::findAllResources(const char *type,
                                              const QString &filter,
                                              SearchOptions options)
{
    return s_instance->findAllResourcesInternal(QString::fromLatin1(type), filter, options);
}

QStringList KoResourcePaths::resourceDirs(const char *type)
{
    return s_instance->resourceDirsInternal(QString::fromLatin1(type));
}

QString KoResourcePaths::saveLocation(const char *type, const QString &suffix, bool create)
{
    return s_instance->saveLocationInternal(QString::fromLatin1(type), suffix, create);
}

QString KoResourcePaths::locate(const char *type, const QString &filename)
{
    return s_instance->findResourceInternal(QString::fromLatin1(type), filename);
}

QString KoResourcePaths::locateLocal(const char *type, const QString &filename, bool createDir)
{
    return s_instance->locateLocalInternal(QString::fromLatin1(type), filename, createDir);
}
