/* This file is part of the KDE project
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

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

//#include <klibloader.h>
#include <kservice.h>

#include "keximigrate.h"

namespace KexiMigration
{

class MigrateManagerInternal;

//! @short Migration library management, for finding and loading mogration drivers.
class KEXIMIGR_EXPORT MigrateManager : public QObject, public KexiDB::Object
{
public:
    typedef QMap<QString, KService::Ptr> ServicesMap;

    MigrateManager();
    virtual ~MigrateManager();

    /*! Tries to load db driver with named name \a name.
      The name is case insensitive.
      \return db driver, or 0 if error (then error message is also set) */
    KexiMigrate* driver(const QString& name);

    /*! returns list of available drivers names.
      That drivers can be loaded by first use of driver() method. */
    const QStringList driverNames();

    /*! Looks up a drivers list by MIME type of database file.
     Only file-based database drivers are checked.
     The lookup is case insensitive.
     \return driver name or null string if no driver found.
    */
    QString driverForMimeType(const QString &mimeType);

    //! server error is set if there is error at KService level (useful for debugging)
    virtual QString serverErrorMsg();
    virtual int serverResult();
    virtual QString serverResultName();

//! @todo copied from KexiDB::DriverManager, merge it.
    /*! HTML information about possible problems encountered.
     It's displayed in 'details' section, if an error encountered.
     Currently it contains a list of incompatible migration drivers. */
    QString possibleProblemsInfoMsg() const;

    //!Return the list of mime types that are supported by the drivers
    QList<QString> supportedMimeTypes() const;

protected:
    virtual void drv_clearServerResult();

private:
    MigrateManagerInternal *d_int;
};

} //namespace KexiMigrate

#endif
