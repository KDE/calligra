/*
   This file is part of the KDE libraries
   Copyright (c) 2006, 2007 Thomas Braxton <kde.braxton@gmail.com>
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

// Qt5 TODO: re-enable. No point in doing it before, it breaks on QString::fromUtf8(QByteArray), which exists in qt5.
#undef QT_NO_CAST_FROM_BYTEARRAY

#include "kconfig.h"
#include "kconfig_p.h"

#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include "kconfigbackend.h"
#include "kconfiggroup.h"

#include <qcoreapplication.h>
#include <qprocess.h>
#include <qstandardpaths.h>
#include <qbytearray.h>
#include <qfile.h>
#include <qlocale.h>
#include <qdir.h>
#include <QtCore/QProcess>
#include <QtCore/QSet>

bool KConfigPrivate::mappingsRegistered=false;

KConfigPrivate::KConfigPrivate(KConfig::OpenFlags flags,
                               QStandardPaths::StandardLocation resourceType)
    : openFlags(flags), resourceType(resourceType), mBackend(0),
      bDynamicBackend(true),  bDirty(false), bReadDefaults(false),
      bFileImmutable(false), bForceGlobal(false), bSuppressGlobal(false),
      configState(KConfigBase::NoAccess)
{
    sGlobalFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QLatin1String("/kdeglobals");

    static int use_etc_kderc = -1;
    if (use_etc_kderc < 0)
        use_etc_kderc = getenv("KDE_SKIP_KDERC") != 0 ? 0 : 1; // for unit tests
    if (use_etc_kderc) {

        etc_kderc =
#ifdef Q_OS_WIN
            QFile::decodeName( qgetenv("WINDIR") + "/kde4rc" );
#else
            QLatin1String("/etc/kde4rc");
#endif
        if (!QFileInfo(etc_kderc).isReadable()) {
            etc_kderc.clear();
        }
    }

//    if (!mappingsRegistered) {
//        KEntryMap tmp;
//        if (!etc_kderc.isEmpty()) {
//            KSharedPtr<KConfigBackend> backend = KConfigBackend::create(etc_kderc, QLatin1String("INI"));
//            backend->parseConfig( "en_US", tmp, KConfigBackend::ParseDefaults);
//        }
//        const QString kde4rc(QDir::home().filePath(".kde4rc"));
//        if (KStandardDirs::checkAccess(kde4rc, R_OK)) {
//            KSharedPtr<KConfigBackend> backend = KConfigBackend::create(kde4rc, QLatin1String("INI"));
//            backend->parseConfig( "en_US", tmp, KConfigBackend::ParseOptions());
//        }
//        KConfigBackend::registerMappings(tmp);
//        mappingsRegistered = true;
//    }

#if 0 // KDE4 code
    setLocale(KLocale::global()->language());
#else
    setLocale(QLocale::system().name());
#endif
}


bool KConfigPrivate::lockLocal()
{
    if (mBackend) {
        return mBackend->lock();
    }
    // anonymous object - pretend we locked it
    return true;
}

void KConfigPrivate::copyGroup(const QByteArray& source, const QByteArray& destination,
                                KConfigGroup *otherGroup, KConfigBase::WriteConfigFlags flags) const
{
    KEntryMap& otherMap = otherGroup->config()->d_ptr->entryMap;
    const int len = source.length();
    const bool sameName = (destination == source);

    // we keep this bool outside the foreach loop so that if
    // the group is empty, we don't end up marking the other config
    // as dirty erroneously
    bool dirtied = false;

    for (KEntryMap::ConstIterator entryMapIt( entryMap.constBegin() ); entryMapIt != entryMap.constEnd(); ++entryMapIt) {
        const QByteArray& group = entryMapIt.key().mGroup;

        if (!group.startsWith(source)) // nothing to do
            continue;

        // don't copy groups that start with the same prefix, but are not sub-groups
        if (group.length() > len && group[len] != '\x1d')
            continue;

        KEntryKey newKey = entryMapIt.key();

        if (flags & KConfigBase::Localized) {
            newKey.bLocal = true;
        }

        if (!sameName)
            newKey.mGroup.replace(0, len, destination);

        KEntry entry = entryMap[ entryMapIt.key() ];
        dirtied = entry.bDirty = flags & KConfigBase::Persistent;

        if (flags & KConfigBase::Global) {
            entry.bGlobal = true;
        }

        otherMap[newKey] = entry;
    }

    if (dirtied) {
        otherGroup->config()->d_ptr->bDirty = true;
    }
}

QString KConfigPrivate::expandString(const QString& value)
{
    QString aValue = value;

    // check for environment variables and make necessary translations
    int nDollarPos = aValue.indexOf( QLatin1Char('$') );
    while( nDollarPos != -1 && nDollarPos+1 < aValue.length()) {
        // there is at least one $
        if( aValue[nDollarPos+1] == QLatin1Char('(') ) {
            int nEndPos = nDollarPos+1;
            // the next character is not $
            while ( (nEndPos <= aValue.length()) && (aValue[nEndPos]!=QLatin1Char(')')) )
                nEndPos++;
            nEndPos++;
            QString cmd = aValue.mid( nDollarPos+2, nEndPos-nDollarPos-3 );

            QString result;
#if 0 // Removed in KDE Frameworks 5. No such concept anymore. Just set your PATH.
            QByteArray oldpath = qgetenv( "PATH" );
            QByteArray newpath;
            if (KComponentData::hasMainComponent()) {
                newpath = QFile::encodeName(KGlobal::dirs()->resourceDirs("exe").join(QChar::fromLatin1(KPATH_SEPARATOR)));
                if (!newpath.isEmpty() && !oldpath.isEmpty())
                    newpath += KPATH_SEPARATOR;
            }
            newpath += oldpath;
            qputenv( "PATH", newpath);
#endif

// FIXME: wince does not have pipes
#ifndef _WIN32_WCE
            FILE *fs = popen(QFile::encodeName(cmd).data(), "r");
            if (fs) {
                QTextStream ts(fs, QIODevice::ReadOnly);
                result = ts.readAll().trimmed();
                pclose(fs);
            }
#endif
#if 0 // Removed in KDE Frameworks 5, see above.
            qputenv( "PATH", oldpath);
#endif
            aValue.replace( nDollarPos, nEndPos-nDollarPos, result );
            nDollarPos += result.length();
        } else if( aValue[nDollarPos+1] != QLatin1Char('$') ) {
            int nEndPos = nDollarPos+1;
            // the next character is not $
            QString aVarName;
            if ( aValue[nEndPos] == QLatin1Char('{') ) {
                while ( (nEndPos <= aValue.length()) && (aValue[nEndPos] != QLatin1Char('}')) )
                    nEndPos++;
                nEndPos++;
                aVarName = aValue.mid( nDollarPos+2, nEndPos-nDollarPos-3 );
            } else {
                while ( nEndPos <= aValue.length() &&
                        (aValue[nEndPos].isNumber() ||
                        aValue[nEndPos].isLetter() ||
                        aValue[nEndPos] == QLatin1Char('_') ) )
                    nEndPos++;
                aVarName = aValue.mid( nDollarPos+1, nEndPos-nDollarPos-1 );
            }
            QString env;
            if (!aVarName.isEmpty()) {
#ifdef Q_OS_WIN
                if (aVarName == QLatin1String("HOME"))
                    env = QDir::homePath();
                else
#endif
                {
                    QByteArray pEnv = qgetenv(aVarName.toLatin1().constData());
                    if( !pEnv.isEmpty() )
                        env = QString::fromLocal8Bit(pEnv.constData());
                }
                aValue.replace(nDollarPos, nEndPos-nDollarPos, env);
                nDollarPos += env.length();
            } else
                aValue.remove( nDollarPos, nEndPos-nDollarPos );
        } else {
            // remove one of the dollar signs
            aValue.remove( nDollarPos, 1 );
            nDollarPos++;
        }
        nDollarPos = aValue.indexOf( QLatin1Char('$'), nDollarPos );
    }

    return aValue;
}


KConfig::KConfig(const QString& file, OpenFlags mode,
                 QStandardPaths::StandardLocation resourceType)
  : d_ptr(new KConfigPrivate(mode, resourceType))
{
    d_ptr->changeFileName(file); // set the local file name

    // read initial information off disk
    reparseConfiguration();
}

KConfig::KConfig(const QString& file, const QString& backend, QStandardPaths::StandardLocation resourceType)
    : d_ptr(new KConfigPrivate(SimpleConfig, resourceType))
{
    d_ptr->mBackend = KConfigBackend::create(file, backend);
    d_ptr->bDynamicBackend = false;
    d_ptr->changeFileName(file); // set the local file name

    // read initial information off disk
    reparseConfiguration();
}

KConfig::KConfig(KConfigPrivate &d)
    : d_ptr(&d)
{
}

KConfig::~KConfig()
{
    Q_D(KConfig);
    if (d->bDirty && d->mBackend.isUnique())
        sync();
    delete d;
}

QStringList KConfig::groupList() const
{
    Q_D(const KConfig);
    QSet<QString> groups;

    for (KEntryMap::ConstIterator entryMapIt( d->entryMap.constBegin() ); entryMapIt != d->entryMap.constEnd(); ++entryMapIt) {
        const KEntryKey& key = entryMapIt.key();
        const QByteArray group = key.mGroup;
        if (key.mKey.isNull() && !group.isEmpty() && group != "<default>" && group != "$Version") {
            const QString groupname = QString::fromUtf8(group);
            groups << groupname.left(groupname.indexOf(QLatin1Char('\x1d')));
        }
    }

    return groups.toList();
}

QStringList KConfigPrivate::groupList(const QByteArray& group) const
{
    QByteArray theGroup = group + '\x1d';
    QSet<QString> groups;

    for (KEntryMap::ConstIterator entryMapIt( entryMap.constBegin() ); entryMapIt != entryMap.constEnd(); ++entryMapIt) {
        const KEntryKey& key = entryMapIt.key();
        if (key.mKey.isNull() && key.mGroup.startsWith(theGroup)) {
            const QString groupname = QString::fromUtf8(key.mGroup.mid(theGroup.length()));
            groups << groupname.left(groupname.indexOf(QLatin1Char('\x1d')));
        }
    }

    return groups.toList();
}

// List all sub groups, including subsubgroups
QSet<QByteArray> KConfigPrivate::allSubGroups(const QByteArray& parentGroup) const
{
    QSet<QByteArray> groups;
    QByteArray theGroup = parentGroup + '\x1d';
    groups << parentGroup;

    for (KEntryMap::const_iterator entryMapIt = entryMap.begin(); entryMapIt != entryMap.end(); ++entryMapIt) {
        const KEntryKey& key = entryMapIt.key();
        if (key.mKey.isNull() && key.mGroup.startsWith(theGroup)) {
            groups << key.mGroup;
        }
    }
    return groups;
}

bool KConfigPrivate::hasNonDeletedEntries(const QByteArray& group) const
{
    const QSet<QByteArray> allGroups = allSubGroups(group);

    Q_FOREACH(const QByteArray& aGroup, allGroups) {
        // Could be optimized, let's use the slow way for now
        // Check for any non-deleted entry
        if (!keyListImpl(aGroup).isEmpty())
            return true;
    }
    return false;
}


QStringList KConfigPrivate::keyListImpl(const QByteArray& theGroup) const
{
    QStringList keys;

    const KEntryMapConstIterator theEnd = entryMap.constEnd();
    KEntryMapConstIterator it = entryMap.findEntry(theGroup);
    if (it != theEnd) {
        ++it; // advance past the special group entry marker

        QSet<QString> tmp;
        for (; it != theEnd && it.key().mGroup == theGroup; ++it) {
            const KEntryKey& key = it.key();
            if (key.mGroup == theGroup && !key.mKey.isNull() && !it->bDeleted)
                tmp << QString::fromUtf8(key.mKey);
        }
        keys = tmp.toList();
    }

    return keys;
}

QStringList KConfig::keyList(const QString& aGroup) const
{
    Q_D(const KConfig);
    const QByteArray theGroup(aGroup.isEmpty() ? "<default>" : aGroup.toUtf8());
    return d->keyListImpl(theGroup);
}

QMap<QString,QString> KConfig::entryMap(const QString& aGroup) const
{
    Q_D(const KConfig);
    QMap<QString, QString> theMap;
    const QByteArray theGroup(aGroup.isEmpty() ? "<default>" : aGroup.toUtf8());

    const KEntryMapConstIterator theEnd = d->entryMap.constEnd();
    KEntryMapConstIterator it = d->entryMap.findEntry(theGroup, 0, 0);
    if (it != theEnd) {
        ++it; // advance past the special group entry marker

        for (; it != theEnd && it.key().mGroup == theGroup; ++it) {
            // leave the default values and deleted entries out
            if (!it->bDeleted && !it.key().bDefault) {
                const QString key = QString::fromUtf8(it.key().mKey.constData());
                // the localized entry should come first, so don't overwrite it
                // with the non-localized entry
                if (!theMap.contains(key)) {
                    if (it->bExpand) {
                        theMap.insert(key,KConfigPrivate::expandString(QString::fromUtf8(it->mValue.constData())));
                    } else {
                        theMap.insert(key,QString::fromUtf8(it->mValue.constData()));
                    }
                }
            }
        }
    }

    return theMap;
}

bool KConfig::sync()
{
    Q_D(KConfig);

    if (isImmutable() || name().isEmpty()) {
        // can't write to an immutable or anonymous file.
        return false;
    }

    if (d->bDirty && d->mBackend) {
        const QByteArray utf8Locale(locale().toUtf8());

        // Create the containing dir, maybe it wasn't there
        d->mBackend->createEnclosing();

        // lock the local file
        if (d->configState == ReadWrite && !d->lockLocal()) {
            qWarning() << "couldn't lock local file";
            return false;
        }

        // Rewrite global/local config only if there is a dirty entry in it.
        bool writeGlobals = false;
        bool writeLocals = false;
        Q_FOREACH (const KEntry& e, d->entryMap) {
            if (e.bDirty) {
                if (e.bGlobal) {
                    writeGlobals = true;
                } else {
                    writeLocals = true;
                }

                if (writeGlobals && writeLocals) {
                    break;
                }
            }
        }

        d->bDirty = false; // will revert to true if a config write fails

        if (d->wantGlobals() && writeGlobals) {
            KSharedPtr<KConfigBackend> tmp = KConfigBackend::create(d->sGlobalFileName);
            if (d->configState == ReadWrite && !tmp->lock()) {
                qWarning() << "couldn't lock global file";
                d->bDirty = true;
                return false;
            }
            if (!tmp->writeConfig(utf8Locale, d->entryMap, KConfigBackend::WriteGlobal)) {
                d->bDirty = true;
            }
            if (tmp->isLocked()) {
                tmp->unlock();
            }
        }

        if (writeLocals) {
            if (!d->mBackend->writeConfig(utf8Locale, d->entryMap, KConfigBackend::WriteOptions())) {
                d->bDirty = true;
            }
        }
        if (d->mBackend->isLocked()) {
            d->mBackend->unlock();
        }
    }
    return !d->bDirty;
}

void KConfig::markAsClean()
{
    Q_D(KConfig);
    d->bDirty = false;

    // clear any dirty flags that entries might have set
    const KEntryMapIterator theEnd = d->entryMap.end();
    for (KEntryMapIterator it = d->entryMap.begin(); it != theEnd; ++it)
        it->bDirty = false;
}

bool KConfig::isDirty() const // only exists for the unittest
{
    Q_D(const KConfig);
    return d->bDirty;
}

void KConfig::checkUpdate(const QString &id, const QString &updateFile)
{
    const KConfigGroup cg(this, "$Version");
    const QString cfg_id = updateFile+QLatin1Char(':')+id;
    const QStringList ids = cg.readEntry("update_info", QStringList());
    if (!ids.contains(cfg_id)) {
#if 0
        KToolInvocation::kdeinitExecWait(QString::fromLatin1("kconf_update"), QStringList() << QString::fromLatin1("--check") << updateFile);
#else
        QProcess::execute(QString::fromLatin1("kconf_update"), QStringList() << QString::fromLatin1("--check") << updateFile);
#endif
        reparseConfiguration();
    }
}

KConfig* KConfig::copyTo(const QString &file, KConfig *config) const
{
    Q_D(const KConfig);
    if (!config)
        config = new KConfig(QString(), SimpleConfig, d->resourceType);
    config->d_func()->changeFileName(file);
    config->d_func()->entryMap = d->entryMap;
    config->d_func()->bFileImmutable = false;

    const KEntryMapIterator theEnd = config->d_func()->entryMap.end();
    for (KEntryMapIterator it = config->d_func()->entryMap.begin(); it != theEnd; ++it)
        it->bDirty = true;
    config->d_ptr->bDirty = true;

    return config;
}

QString KConfig::name() const
{
    Q_D(const KConfig);
    return d->fileName;
}

Q_GLOBAL_STATIC(QString, globalMainConfigName)

void KConfig::setMainConfigName(const QString& str)
{
    *globalMainConfigName() = str;
}

QString KConfig::mainConfigName()
{
    // --config on the command line overrides everything else
    const QStringList args = QCoreApplication::arguments();
    for (int i = 1; i < args.count() ; ++i) {
        if (args.at(i) == QLatin1String("--config") && i < args.count()-1) {
            return args.at(i+1);
        }
    }
    const QString globalName = *globalMainConfigName();
    if (!globalName.isEmpty())
        return globalName;

    QString appName = QCoreApplication::applicationName();
    return appName + QLatin1String("rc");
}

void KConfigPrivate::changeFileName(const QString& name)
{
    fileName = name;

    QString file;
    if (name.isEmpty()) {
        if (wantDefaults()) { // accessing default app-specific config "appnamerc"
            fileName = KConfig::mainConfigName();
            file = QStandardPaths::writableLocation(resourceType) + QLatin1Char('/') + fileName;
        } else if (wantGlobals()) { // accessing "kdeglobals" - XXX used anywhere?
            resourceType = QStandardPaths::ConfigLocation;
            fileName = QLatin1String("kdeglobals");
            file = sGlobalFileName;
        } else {
            // anonymous config
            openFlags = KConfig::SimpleConfig;
            return;
        }
    } else if (QDir::isAbsolutePath(fileName)) {
        fileName = QFileInfo(fileName).canonicalFilePath();
        if (fileName.isEmpty()) // file doesn't exist (yet)
            fileName = name;
        file = fileName;
    } else {
        file = QStandardPaths::writableLocation(resourceType) + QLatin1Char('/') + fileName;
    }

    Q_ASSERT(!file.isEmpty());

    bSuppressGlobal = (file == sGlobalFileName);

    if (bDynamicBackend || !mBackend) // allow dynamic changing of backend
        mBackend = KConfigBackend::create(file);
    else
        mBackend->setFilePath(file);

    configState = mBackend->accessMode();
}

void KConfig::reparseConfiguration()
{
    Q_D(KConfig);
    if (d->fileName.isEmpty()) {
        return;
    }

    // Don't lose pending changes
    if (!d->isReadOnly() && d->bDirty)
        sync();

    d->entryMap.clear();

    d->bFileImmutable = false;

    // Parse all desired files from the least to the most specific.
    if (d->wantGlobals())
        d->parseGlobalFiles();

    d->parseConfigFiles();
}


QStringList KConfigPrivate::getGlobalFiles() const
{
    QStringList globalFiles;
    Q_FOREACH (const QString& dir1, QStandardPaths::locateAll(QStandardPaths::ConfigLocation, QLatin1String("kdeglobals")))
        globalFiles.push_front(dir1);
    Q_FOREACH (const QString& dir2, QStandardPaths::locateAll(QStandardPaths::ConfigLocation, QLatin1String("system.kdeglobals")))
        globalFiles.push_front(dir2);
    if (!etc_kderc.isEmpty())
        globalFiles.push_front(etc_kderc);
    return globalFiles;
}

void KConfigPrivate::parseGlobalFiles()
{
    const QStringList globalFiles = getGlobalFiles();
//    qDebug() << "parsing global files" << globalFiles;

    // TODO: can we cache the values in etc_kderc / other global files
    //       on a per-application basis?
    const QByteArray utf8Locale = locale.toUtf8();
    Q_FOREACH(const QString& file, globalFiles) {
        KConfigBackend::ParseOptions parseOpts = KConfigBackend::ParseGlobal|KConfigBackend::ParseExpansions;
        if (file != sGlobalFileName)
            parseOpts |= KConfigBackend::ParseDefaults;

        KSharedPtr<KConfigBackend> backend = KConfigBackend::create(file);
        if ( backend->parseConfig( utf8Locale, entryMap, parseOpts) == KConfigBackend::ParseImmutable)
            break;
    }
}

void KConfigPrivate::parseConfigFiles()
{
    // can only read the file if there is a backend and a file name
    if (mBackend && !fileName.isEmpty()) {

        bFileImmutable = false;

        QList<QString> files;
        if (wantDefaults()) {
            if (bSuppressGlobal) {
                files = getGlobalFiles();
            } else {
                if (QDir::isAbsolutePath(fileName)) {
                    files << fileName;
                } else {
                    Q_FOREACH (const QString& f, QStandardPaths::locateAll(resourceType, fileName))
                        files.prepend(f);
                }
            }
        } else {
            files << mBackend->filePath();
        }
        if (!isSimple())
            files = extraFiles.toList() + files;

//        qDebug() << "parsing local files" << files;

        const QByteArray utf8Locale = locale.toUtf8();
        Q_FOREACH(const QString& file, files) {
            if (file == mBackend->filePath()) {
                switch (mBackend->parseConfig(utf8Locale, entryMap, KConfigBackend::ParseExpansions)) {
                case KConfigBackend::ParseOk:
                    break;
                case KConfigBackend::ParseImmutable:
                    bFileImmutable = true;
                    break;
                case KConfigBackend::ParseOpenError:
                    configState = KConfigBase::NoAccess;
                    break;
                }
            } else {
                KSharedPtr<KConfigBackend> backend = KConfigBackend::create(file);
                bFileImmutable = (backend->parseConfig(utf8Locale, entryMap,
                                        KConfigBackend::ParseDefaults|KConfigBackend::ParseExpansions)
                                  == KConfigBackend::ParseImmutable);
            }

            if (bFileImmutable)
                break;
        }
#pragma message("TODO: enable kiosk feature again (resource restrictions), but without KStandardDirs... Needs a class in the kconfig framework.")
#if 0
        if (componentData.dirs()->isRestrictedResource(resourceType, fileName))
            bFileImmutable = true;
#endif
    }
}

KConfig::AccessMode KConfig::accessMode() const
{
    Q_D(const KConfig);
    return d->configState;
}

void KConfig::addConfigSources(const QStringList& files)
{
    Q_D(KConfig);
    Q_FOREACH(const QString& file, files) {
        d->extraFiles.push(file);
    }

    if (!files.isEmpty()) {
        reparseConfiguration();
    }
}

QString KConfig::locale() const
{
    Q_D(const KConfig);
    return d->locale;
}

bool KConfigPrivate::setLocale(const QString& aLocale)
{
    if (aLocale != locale) {
        locale = aLocale;
        return true;
    }
    return false;
}

bool KConfig::setLocale(const QString& locale)
{
    Q_D(KConfig);
    if (d->setLocale(locale)) {
        reparseConfiguration();
        return true;
    }
    return false;
}

void KConfig::setReadDefaults(bool b)
{
    Q_D(KConfig);
    d->bReadDefaults = b;
}

bool KConfig::readDefaults() const
{
    Q_D(const KConfig);
    return d->bReadDefaults;
}

bool KConfig::isImmutable() const
{
    Q_D(const KConfig);
    return d->bFileImmutable;
}

bool KConfig::isGroupImmutableImpl(const QByteArray& aGroup) const
{
    Q_D(const KConfig);
    return isImmutable() || d->entryMap.getEntryOption(aGroup, 0, 0, KEntryMap::EntryImmutable);
}

#ifndef KDE_NO_DEPRECATED
void KConfig::setForceGlobal(bool b)
{
    Q_D(KConfig);
    d->bForceGlobal = b;
}
#endif

#ifndef KDE_NO_DEPRECATED
bool KConfig::forceGlobal() const
{
    Q_D(const KConfig);
    return d->bForceGlobal;
}
#endif

KConfigGroup KConfig::groupImpl(const QByteArray &group)
{
    return KConfigGroup(this, group.constData());
}

const KConfigGroup KConfig::groupImpl(const QByteArray &group) const
{
    return KConfigGroup(this, group.constData());
}

KEntryMap::EntryOptions convertToOptions(KConfig::WriteConfigFlags flags)
{
    KEntryMap::EntryOptions options=0;

    if (flags&KConfig::Persistent)
        options |= KEntryMap::EntryDirty;
    if (flags&KConfig::Global)
        options |= KEntryMap::EntryGlobal;
    if (flags&KConfig::Localized)
        options |= KEntryMap::EntryLocalized;
    return options;
}

void KConfig::deleteGroupImpl(const QByteArray &aGroup, WriteConfigFlags flags)
{
    Q_D(KConfig);
    KEntryMap::EntryOptions options = convertToOptions(flags)|KEntryMap::EntryDeleted;

    const QSet<QByteArray> groups = d->allSubGroups(aGroup);
    Q_FOREACH (const QByteArray& group, groups) {
        const QStringList keys = d->keyListImpl(group);
        Q_FOREACH (const QString& _key, keys) {
            const QByteArray &key = _key.toUtf8();
            if (d->canWriteEntry(group, key.constData())) {
                d->entryMap.setEntry(group, key, QByteArray(), options);
                d->bDirty = true;
            }
        }
    }
}

bool KConfig::isConfigWritable(bool warnUser)
{
    Q_D(KConfig);
    bool allWritable = (d->mBackend.isNull()? false: d->mBackend->isWritable());

    if (warnUser && !allWritable) {
        QString errorMsg;
        if (!d->mBackend.isNull()) // TODO how can be it be null? Set errorMsg appropriately
            errorMsg = d->mBackend->nonWritableErrorMessage();

        // Note: We don't ask the user if we should not ask this question again because we can't save the answer.
        errorMsg += QCoreApplication::translate("KConfig", "Please contact your system administrator.");
        QString cmdToExec = QStandardPaths::findExecutable(QString::fromLatin1("kdialog"));
        if (!cmdToExec.isEmpty())
        {
            QProcess::execute(cmdToExec, QStringList()
                              << QString::fromLatin1("--title") << QCoreApplication::applicationName()
                              << QString::fromLatin1("--msgbox") << errorMsg);
        }
    }

    d->configState = allWritable ?  ReadWrite : ReadOnly; // update the read/write status

    return allWritable;
}

bool KConfig::hasGroupImpl(const QByteArray& aGroup) const
{
    Q_D(const KConfig);

    // No need to look for the actual group entry anymore, or for subgroups:
    // a group exists if it contains any non-deleted entry.

    return d->hasNonDeletedEntries(aGroup);
}

bool KConfigPrivate::canWriteEntry(const QByteArray& group, const char* key, bool isDefault) const
{
    if (bFileImmutable ||
        entryMap.getEntryOption(group, key, KEntryMap::SearchLocalized, KEntryMap::EntryImmutable))
        return isDefault;
    return true;
}

void KConfigPrivate::putData( const QByteArray& group, const char* key,
                      const QByteArray& value, KConfigBase::WriteConfigFlags flags, bool expand)
{
    KEntryMap::EntryOptions options = convertToOptions(flags);

    if (bForceGlobal)
        options |= KEntryMap::EntryGlobal;
    if (expand)
        options |= KEntryMap::EntryExpansion;

    if (value.isNull()) // deleting entry
        options |= KEntryMap::EntryDeleted;

    bool dirtied = entryMap.setEntry(group, key, value, options);
    if (dirtied && (flags & KConfigBase::Persistent))
        bDirty = true;
}

void KConfigPrivate::revertEntry(const QByteArray& group, const char* key)
{
    bool dirtied = entryMap.revertEntry(group, key);
    if (dirtied)
        bDirty = true;
}

QByteArray KConfigPrivate::lookupData(const QByteArray& group, const char* key,
                                      KEntryMap::SearchFlags flags) const
{
    if (bReadDefaults)
        flags |= KEntryMap::SearchDefaults;
    const KEntryMapConstIterator it = entryMap.findEntry(group, key, flags);
    if (it == entryMap.constEnd())
        return QByteArray();
    return it->mValue;
}

QString KConfigPrivate::lookupData(const QByteArray& group, const char* key,
                                   KEntryMap::SearchFlags flags, bool *expand) const
{
    if (bReadDefaults)
        flags |= KEntryMap::SearchDefaults;
    return entryMap.getEntry(group, key, QString(), flags, expand);
}

QStandardPaths::StandardLocation KConfig::locationType() const
{
    Q_D(const KConfig);
    return d->resourceType;
}

void KConfig::virtual_hook(int /*id*/, void* /*data*/)
{
	/* nothing */
}
