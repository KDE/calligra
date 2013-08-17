/*
   This file is part of the KDE libraries
   Copyright (c) 2006, 2007 Thomas Braxton <kde.braxton@gmail.com>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Portions copyright (c) 1997 Matthias Kalle Dalheimer <kalle@kde.org>

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

#ifndef KCONFIGINI_P_H
#define KCONFIGINI_P_H

#include <kconfigcore_export.h>
#include <kconfigbackend.h>

class QLockFile;
class QIODevice;

class KConfigIniBackend : public KConfigBackend
{
private:
    class BufferFragment;

    QLockFile *lockFile;
public:

    KConfigIniBackend();
    ~KConfigIniBackend();

    ParseInfo parseConfig(const QByteArray& locale,
                          KEntryMap& entryMap,
                          ParseOptions options);
    ParseInfo parseConfig(const QByteArray& locale,
                          KEntryMap& entryMap,
                          ParseOptions options,
                          bool merging);
    bool writeConfig(const QByteArray& locale, KEntryMap& entryMap,
                     WriteOptions options);

    bool isWritable() const;
    QString nonWritableErrorMessage() const;
    KConfigBase::AccessMode accessMode() const;
    void createEnclosing();
    void setFilePath(const QString& path);
    bool lock();
    void unlock();
    bool isLocked() const;

protected:

    enum StringType {
        GroupString = 0,
        KeyString = 1,
        ValueString = 2
    };
    // Warning: this modifies data in-place. Other BufferFragment objects referencing the same buffer
    // fragment will get their data modified too.
    static void printableToString(BufferFragment* aString, const QFile& file, int line);
    static QByteArray stringToPrintable(const QByteArray& aString, StringType type);
    static char charFromHex(const char *str, const QFile& file, int line);
    static QString warningProlog(const QFile& file, int line);

    void writeEntries(const QByteArray& locale, QIODevice& file, const KEntryMap& map);
    void writeEntries(const QByteArray& locale, QIODevice& file, const KEntryMap& map,
                      bool defaultGroup, bool &firstEntry);
};

#endif // KCONFIGINI_P_H
