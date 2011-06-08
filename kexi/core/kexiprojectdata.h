/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPROJECTDATA_H
#define KEXIPROJECTDATA_H

#include <kexi_export.h>
#include <kexidb/connectiondata.h>
#include <kexidb/schemadata.h>

#include <QDateTime>
#include <QList>
#include <QtDebug>

class KexiProjectDataPrivate;

/** @short Kexi project core data member

 Contains:
 - project name
 - database name
 - connection data
 - date and time of last opening

 @todo make it value-based class
*/
class KEXICORE_EXPORT KexiProjectData : public QObject, public KexiDB::SchemaData
{
public:
    typedef QList<KexiProjectData*> List;
    typedef QHash<QByteArray, QString> ObjectInfo;

    //! A list of autoopen objects allowing deep copies
    class KEXICORE_EXPORT AutoOpenObjects : public QList<ObjectInfo*>
    {
    public:
        AutoOpenObjects();

        ~AutoOpenObjects();

        //! Construct autoopen objects list as a deep copy of \a other
        AutoOpenObjects(const AutoOpenObjects& other);

        //! Assigns a deep copy of \a other
        AutoOpenObjects& operator=(const AutoOpenObjects& other);
    };

    KexiProjectData();

    KexiProjectData(const KexiDB::ConnectionData &cdata,
                    const QString& dbname = QString(), const QString& caption = QString());

    /*! Constructs a copy of \a pdata */
    KexiProjectData(const KexiProjectData& pdata);

    ~KexiProjectData();

    KexiProjectData& operator=(const KexiProjectData& pdata);

    /*! \return true if there is the User Mode set in internal
     project settings. */
    bool userMode() const;

    KexiDB::ConnectionData* connectionData();

    const KexiDB::ConnectionData* constConnectionData() const;

    /*! \return database name.
     In fact, this is the same as KexiDB::SchemaData::name() */
    QString databaseName() const;
    void setDatabaseName(const QString& dbName);

    /*! \return user-visible string better describing the project than just databaseName().
     For server-based projects returns i18n'd string:
     "<project name>" (connection: user\@server:port).
     For file-based projects returns project's filename.
     If \a nobr is true, \<nobr\> tags are added around '(connection: user\@server:port)'
     (useful for displaying in message boxes). */
    QString infoString(bool nobr = true) const;

    QDateTime lastOpened() const;

    void setLastOpened(const QDateTime& lastOpened);

    QString description() const;
    void setDescription(const QString& desc);

    /*! If \a set is true, sets readonly flag for this data, so any connection opened for the project will
     be readonly. Change this flag before using this data in KexiProject instance,
     otherwise you will need to reopen the project. */
    void setReadOnly(bool set);

    /*! \return readonly flag. False by default.
     @see setReadOnly() */
    bool isReadOnly() const;

    /*! objects to open on startup (come from command line "-open" option)
     It's public for convenience */
    AutoOpenObjects autoopenObjects;

    /*! @internal
     Format version used when saving the data to a shortcut file.
     This is set to 0 by default what means KexiDBShortcutFile_version should be used on saving.
     If KexiDBShortcutFile was used to create this KexiProjectData object,
     the version information is retrieved from the file. */
    uint formatVersion;

private:
    KexiProjectDataPrivate * const d;
};

KEXICORE_EXPORT QDebug operator<<(QDebug dbg, const KexiProjectData& d);

#endif
