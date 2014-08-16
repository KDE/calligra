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

#ifndef KEXI_MIGRATION_MNGR_H
#define KEXI_MIGRATION_MNGR_H

#include <QObject>
#include <QMap>

#include <kservice.h>

#include <core/KexiMigrateManagerInterface.h>

#include "keximigrate.h"

namespace KexiMigration
{

class MigrateManagerInternal;

//! @short Migration library management, for finding and loading migration drivers.
class KEXIMIGR_EXPORT MigrateManager : public QObject, public KexiDB::Object,
                                       public KexiMigrateManagerInterface
{
public:
    MigrateManager();
    virtual ~MigrateManager();

    /*! Tries to load db driver with named name \a name.
      The name is case insensitive.
      \return db driver, or 0 if error (then error message is also set) */
    KexiMigrate* driver(const QString& name);

    /*! returns list of available drivers names.
      That drivers can be loaded by first use of driver() method. */
    const QStringList driverNames();

    /*! Looks up a drivers list based on @a fileName.
     Only file-based database drivers are checked.
     First it finds by file content (using KMimeType::findByFileContent()).
     If no matching driver is found, it attempts to find driver by mimetype for URL (using KMimeType::findByUrl()).
     @return matching driver name or empty string if no driver has been found. */
    QString findDriverByFileContentOrName(const QString &fileName);

    /*! Looks up a drivers list based on MIME type.
     Only file-based database drivers are checked.
     The lookup is case insensitive.
     @return driver name for matching mimetype or empty string if no driver has been found. */
    QString findDriverByMimeType(const QString &mimeType);

    /*! Looks up a drivers list by MIME type @a knownMimeType of database file @a fileName.
     Only file-based database drivers are checked.
     The mimetype lookup is case insensitive.
     If the @a mimeType is not supported by any driver, attempts to find mimetype by file content
     (using KMimeType::findByFileContent()).
     If no matching mimetype is found or the mime type is generic "application/octet-stream",
     "text/plain" or "application/zip", attempts to find mimetype by URL
     (using KMimeType::findByUrl()).
     @return matching mimetype name or empty string if no mimetype has been found.
    */
    virtual QString findSupportedMimeType(const QString &knownMimeType, const QString &fileName);

    //! server error is set if there is error at KService level (useful for debugging)
    virtual QString serverErrorMsg();
    virtual int serverResult();
    virtual QString serverResultName();

//! @todo copied from KexiDB::DriverManager, merge it.
    /*! HTML information about possible problems encountered.
     It's displayed in 'details' section, if an error encountered.
     Currently it contains a list of incompatible migration drivers. */
    QString possibleProblemsInfoMsg() const;

    //! @return the list of file MIME types that are supported by migration drivers.
    //! Implements MigrateManagerInterface
    virtual QStringList supportedFileMimeTypes();

protected:
    virtual void drv_clearServerResult();

private:
    MigrateManagerInternal *d_int;
};

} //namespace KexiMigrate

#endif
