/*
   This file is part of the KDE libraries
   Copyright (c) 2006, 2007 Thomas Braxton <kde.braxton@gmail.com>
   Copyright (c) 2001 Waldo Bastian <bastian@kde.org>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997 Matthias Kalle Dalheimer <kalle@kde.org>

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

#ifndef KCONFIG_P_H
#define KCONFIG_P_H

#include "kconfigdata.h"
#include "kconfigbackend.h"
#include "kconfiggroup.h"

#include <QtCore/QStringList>
#include <QtCore/QStack>
#include <QtCore/QFile>
#include <QtCore/QDir>

class KConfigPrivate
{
    friend class KConfig;
public:
    KConfig::OpenFlags openFlags;
    QStandardPaths::StandardLocation resourceType;

    void changeFileName(const QString& fileName);

    // functions for KConfigGroup
    bool canWriteEntry(const QByteArray& group, const char* key, bool isDefault=false) const;
    QString lookupData(const QByteArray& group, const char* key, KEntryMap::SearchFlags flags,
                       bool* expand) const;
    QByteArray lookupData(const QByteArray& group, const char* key, KEntryMap::SearchFlags flags) const;

    void putData(const QByteArray& group, const char* key, const QByteArray& value,
                 KConfigBase::WriteConfigFlags flags, bool expand=false);
    void revertEntry(const QByteArray& group, const char* key);
    QStringList groupList(const QByteArray& group) const;
    // copies the entries from @p source to @p otherGroup changing all occurrences
    // of @p source with @p destination
    void copyGroup(const QByteArray& source, const QByteArray& destination,
                   KConfigGroup *otherGroup, KConfigBase::WriteConfigFlags flags) const;
    QStringList keyListImpl(const QByteArray& theGroup) const;
    QSet<QByteArray> allSubGroups(const QByteArray& parentGroup) const;
    bool hasNonDeletedEntries(const QByteArray& group) const;

    static QString expandString(const QString& value);

protected:
    KSharedPtr<KConfigBackend> mBackend;

    KConfigPrivate(KConfig::OpenFlags flags,
                   QStandardPaths::StandardLocation type);

    virtual ~KConfigPrivate()
    {
    }

    bool bDynamicBackend:1; // do we own the backend?
private:
    bool bDirty:1;
    bool bLocaleInitialized:1;
    bool bReadDefaults:1;
    bool bFileImmutable:1;
    bool bForceGlobal:1;
    bool bSuppressGlobal:1;

   QString sGlobalFileName;
   static bool mappingsRegistered;


    KEntryMap entryMap;
    QString backendType;
    QStack<QString> extraFiles;

    QString locale;
    QString fileName;
    QString etc_kderc;
    KConfigBase::AccessMode configState;

    bool wantGlobals() const { return openFlags&KConfig::IncludeGlobals && !bSuppressGlobal; }
    bool wantDefaults() const { return openFlags&KConfig::CascadeConfig; }
    bool isSimple() const { return openFlags == KConfig::SimpleConfig; }
    bool isReadOnly() const { return configState == KConfig::ReadOnly; }

    bool setLocale(const QString& aLocale);
    QStringList getGlobalFiles() const;
    void parseGlobalFiles();
    void parseConfigFiles();
    void initCustomized(KConfig*);
    bool lockLocal();
};

#endif // KCONFIG_P_H
