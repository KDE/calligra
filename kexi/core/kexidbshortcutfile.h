/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDBSHORTCUTFILE_H
#define KEXIDBSHORTCUTFILE_H

#include <qstring.h>

#include <kexi_export.h>

class KexiProjectData;
namespace KexiDB
{
class ConnectionData;
}

/*! Loads and saves information for a "shortcut to a connection" file containing
 connection information with database name (i.e. ProjectData).
 This is implementation for handling .KEXIS files.
 See http://www.kexi-project.org/wiki/wikiview/index.php?KexiMimeTypes_DataSaving_Loading
*/
class KEXICORE_EXPORT KexiDBShortcutFile
{
public:
    /*! Creates a new object for \a fileName. */
    KexiDBShortcutFile(const QString& fileName);

    ~KexiDBShortcutFile();

    /*! Loads project data (with connection data) into \a data.
     Database name and caption can be set there but these are optional.
     \a groupKey, if provided will be set to a group key,
     so you can later use it in saveConnectionData().
     \return true on success. */
    bool loadProjectData(KexiProjectData& data, QString* groupKey = 0);

    /*! Saves project data \a data (with connection data) to a shortcut file.
     If \a storePassword is true, password will be saved in the file,
     even if data.connectionData()->savePassword is false.
     Existing data is merged with new data. \a groupKey is reused, if specified.
     If \a overwriteFirstGroup is true (the default) first found group will be overwritten
     instead of creating of a new unique group. This mode is usable for updating .kexic files
     containing single connection data, what's used for storing connections repository.
     \return true on success. */
    bool saveProjectData(const KexiProjectData& data, bool savePassword,
                         QString* groupKey = 0, bool overwriteFirstGroup = true);

    //! \return filename provided on this object's construction. */
    QString fileName() const;

protected:
    class Private;
    Private *d;
};

/*! Loads and saves information for a "shortcut" file containing
 connection information (i.e. KexiDB::ConnectionData).
 This is implementation for handling .KEXIC files.
 See http://www.kexi-project.org/wiki/wikiview/index.php?KexiMimeTypes_DataSaving_Loading
*/
class KEXICORE_EXPORT KexiDBConnShortcutFile : protected KexiDBShortcutFile
{
public:
    /*! Creates a new object for \a fileName. */
    KexiDBConnShortcutFile(const QString& fileName);

    ~KexiDBConnShortcutFile();

    /*! Loads connection data into \a data.
     \a groupKey, if provided will be set to a group key,
     so you can later use it in saveConnectionData().
     \return true on success. */
    bool loadConnectionData(KexiDB::ConnectionData& data, QString* groupKey = 0);

    /*! Saves connection data \a data to a shortcut file.
     If \a storePassword is true, password will be saved in the file,
     even if data.savePassword is false.
     Existing data is merged with new data. \a groupKey is reused, if specified.
     If \a overwriteFirstGroup is true (the default) first found group will be overwritten
     instead of creating of a new unique group. This mode is usable for updating .kexic files
     containing single connection data, what's used for storing connections repository.
     \return true on success. */
    bool saveConnectionData(const KexiDB::ConnectionData& data,
                            bool savePassword, QString* groupKey = 0, bool overwriteFirstGroup = true);

    //! \return filename provided on this object's construction. */
    QString fileName() const {
        return KexiDBShortcutFile::fileName();
    }

protected:
};

#if 0
//! Loads and saves information for a sef of "shortcut to a connection" file containing
//! connection information (i.e. KexiDBConnectionSet).
//! This is implementation for handling .KEXIC files.
//! The set is loaded from files found using
//! KGlobal::dirs()->findAllResources("data", "kexi/connections/*.kexic").
class KexiDBConnSetShortcutFiles
{
public:
    KexiDBConnSetShortcutFiles();

    /*! Loads connection data into \a set. The set is cleared before loading.
     \retuirn true on successful loading. */
    static bool loadConnectionDataSet(KexiDBConnectionSet& set);

    /*! Saves a set of connection data \a set to a shortcut files.
     Existing files are overwritten with a new data.
     \retuirn true on successful saving. */
    static bool saveConnectionDataSet(const KexiDBConnectionSet& set);
}
#endif
#endif
