/*  This file is part of the KDE project
    Copyright (C) 2007 Bernhard Loos <nhuh.put@web.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kpluginloader.h"

#if 0 // TEMP_KF5_REENABLE
#include <klocalizedstring.h>
#else
QString i18n(QString a) { return a; }
QString i18n(QString a, QString) { return a; }
QString i18n(QString a, QString, QString) { return a; }
#endif
#include "kpluginfactory.h"
#include <kservice.h>
#include "klibrary.h"

#include <QtCore/QLibrary>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QDebug>
#include <QCoreApplication>

class KPluginLoaderPrivate
{
    Q_DECLARE_PUBLIC(KPluginLoader)
protected:
    KPluginLoaderPrivate(const QString &libname)
        : name(libname), pluginVersion(~0U), verificationData(0), lib(0)
    {}
    ~KPluginLoaderPrivate()
    {
        delete lib;
    }

    KPluginLoader *q_ptr;
    const QString name;
    quint32 pluginVersion;
    KDEPluginVerificationData *verificationData;
    QString errorString;

    KLibrary *lib;
};

inline QString makeLibName( const QString &libname )
{
#if defined(Q_OS_WIN) || defined(Q_OS_CYGWIN)
    if (!libname.endsWith(QLatin1String(".dll")))
        return libname + QLatin1String(".dll");
    return libname;
#else
    int pos = libname.lastIndexOf(QLatin1Char('/'));
    if (pos < 0)
      pos = 0;
    if (libname.indexOf(QLatin1Char('.'), pos) < 0) {
        const char* const extList[] = { ".so", ".dylib", ".bundle", ".sl" };
        for (uint i = 0; i < sizeof(extList) / sizeof(*extList); ++i) {
            const QString lib = libname + QString::fromLatin1(extList[i]);
            if (QLibrary::isLibrary(lib))
                return lib;
        }
    }
    return libname;
#endif
}

#ifdef Q_OS_WIN
extern QString fixLibPrefix(const QString& libname);
#endif

QString findLibraryInternal(const QString &name)
{
    // Convert name to a valid platform libname
    QString libname = makeLibName(name);
    QFileInfo fileinfo(name);
    bool hasPrefix = fileinfo.fileName().startsWith(QLatin1String("lib"));
    bool kdeinit = fileinfo.fileName().startsWith(QLatin1String("libkdeinit5_"));

    if (hasPrefix && !kdeinit)
        qDebug() << "plugins should not have a 'lib' prefix:" << libname;
#ifdef Q_CC_MSVC
    // first remove the 'lib' prefix in front of windows plugins
    libname = fixLibPrefix(libname);
#endif

    // If it is a absolute path just return it
    if (!QDir::isRelativePath(libname))
        return libname;

#if 0
    // TEMPORARY HACK
    Q_FOREACH(const QString &path, QFile::decodeName(qgetenv("LD_LIBRARY_PATH")).split(QLatin1Char(':'), QString::SkipEmptyParts)) {
        QString libfile = path + QLatin1String("/kde5/") + libname;
        if (QFile::exists(libfile)) {
            //qDebug() << "Looking at" << libfile << ": FOUND!";
            return libfile;
        }
        //qDebug() << "Looking at" << libfile << ": doesn't exist";
        libfile = path + QLatin1String("/") + libname;
        if (QFile::exists(libfile)) {
            if (!kdeinit) {
                qDebug() << "library" << libname << "not found under 'module' but under 'lib'";
            }
            return libfile;
        }
    }
#endif

    // Ask Qt for the list of based paths containing plugins
    Q_FOREACH(const QString &path, QCoreApplication::libraryPaths()) {
        // Check for kde modules/plugins?
        QString libfile = path + QLatin1String("/kf5/") + libname;
        if (QFile::exists(libfile)) {
            //qDebug() << "Looking at" << libfile << ": FOUND!";
            return libfile;
        }
        //qDebug() << "Looking at" << libfile << ": doesn't exist";

#if 0 // old code, not sure how to port
    // Now look where they don't belong but sometimes are
#ifndef Q_CC_MSVC
    if (!hasPrefix)
        libname = fileinfo.path() + QLatin1String("/lib") + fileinfo.fileName();
#endif
#endif

        libfile = path + QLatin1String("/") + libname;
        if (QFile::exists(libfile)) {
            if (!kdeinit) {
                qDebug() << "library" << libname << "not found under 'module' but under 'lib'";
            }
            return libfile;
        }
    }

    // Nothing found
    return QString();
}

bool KPluginLoader::isLoaded() const
{
    return QPluginLoader::isLoaded() || d_ptr->lib;
}

KPluginLoader::KPluginLoader(const QString &plugin, QObject *parent)
    : QPluginLoader(findLibraryInternal(plugin), parent), d_ptr(new KPluginLoaderPrivate(plugin))
{
    d_ptr->q_ptr = this;
    Q_D(KPluginLoader);

    // No lib, no fun.
    if (fileName().isEmpty()) {
        d->errorString = i18n(
                "Could not find plugin '%1' for application '%2'",
                plugin,
                QCoreApplication::instance()->applicationName());
        return;
    }
}


KPluginLoader::KPluginLoader(const KService &service, QObject *parent)
: QPluginLoader(findLibraryInternal(service.library()), parent), d_ptr(new KPluginLoaderPrivate(service.library()))
{
    d_ptr->q_ptr = this;
    Q_D(KPluginLoader);

    // It's probably to late to check this because service.library() is used
    // above.
    if (!service.isValid()) {
        d->errorString = i18n("The provided service is not valid");
        return;
    }

    // service.library() is used to find the lib. So first check if it is empty.
    if (service.library().isEmpty()) {
        d->errorString = i18n("The service '%1' provides no library or the Library key is missing", service.entryPath());
        return;
    }

    // No lib, no fun. service.library() was set but we were still unable to
    // find the lib.
    if (fileName().isEmpty()) {
        d->errorString = i18n(
                "Could not find plugin '%1' for application '%2'",
                service.name(),
                QCoreApplication::instance()->applicationName());
        return;
    }
}

KPluginLoader::~KPluginLoader()
{
    delete d_ptr;
}

KPluginFactory *KPluginLoader::factory()
{
    Q_D(KPluginLoader);

    if (!load())
        return 0;

#ifndef KDE_NO_DEPRECATED
    if (d->lib) {
        // Calling a deprecated method, but this is the only way to
        // support both new and old-style factories for now.
        // KDE5: remove the whole if().
        return d->lib->factory(d->name.toUtf8());
    }
#endif

    QObject *obj = instance();

    if (!obj)
        return 0;

    KPluginFactory *factory = qobject_cast<KPluginFactory *>(obj);

    if (factory == 0) {
        qDebug() << "Expected a KPluginFactory, got a" << obj->metaObject()->className();
        delete obj;
        d->errorString = i18n("The library %1 does not offer a KDE 4 compatible factory." , d->name);
    }

    return factory;
}

bool KPluginLoader::load()
{
    Q_D(KPluginLoader);

    if (isLoaded())
        return true;

    if (!QPluginLoader::load()) {
        d->lib = new KLibrary(d->name);
        if (d->lib->load())
            return true;

        return false;
    }

    Q_ASSERT(!fileName().isEmpty());
    QLibrary lib(fileName());
    Q_ASSERT(lib.isLoaded()); // already loaded by QPluginLoader::load()

    d->verificationData = (KDEPluginVerificationData *) lib.resolve("kde_plugin_verification_data");
    if (d->verificationData) {
        if (d->verificationData->dataVersion < KDEPluginVerificationData::PluginVerificationDataVersion
            || (d->verificationData->KDEVersion > KSERVICE_VERSION)
            || (KSERVICE_VERSION_MAJOR << 16 != (d->verificationData->KDEVersion & 0xFF0000)))
        {
            d->errorString = i18n("The plugin '%1' uses an incompatible KDE library (%2).", d->name, QString::fromLatin1(d->verificationData->KDEVersionString));
            unload();
            return false;
        }
    } else {
        qDebug() << "The plugin" << d->name << "doesn't contain a kde_plugin_verification_data structure";
    }

    quint32 *version = (quint32 *) lib.resolve("kde_plugin_version");
    if (version)
        d->pluginVersion = *version;
    else
        d->pluginVersion = ~0U;

    return true;
}

QString KPluginLoader::errorString() const
{
    Q_D(const KPluginLoader);

    if (!d->errorString.isEmpty())
        return d->errorString;

    return QPluginLoader::errorString();
}

quint32 KPluginLoader::pluginVersion() const
{
    Q_D(const KPluginLoader);
    const_cast<KPluginLoader*>(this)->load();
    return d->pluginVersion;
}

QString KPluginLoader::pluginName() const
{
    Q_D(const KPluginLoader);
    const_cast<KPluginLoader*>(this)->load();
    return d->name;
}

#include "kpluginloader.moc"
