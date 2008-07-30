/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_MIGRATE_MNGR_P_H
#define KEXI_MIGRATE_MNGR_P_H

#include <qobject.h>
#include <q3asciidict.h>

namespace KexiMigration {

/*! Internal class of driver manager.
*/
class MigrateManagerInternal : public QObject, public KexiDB::Object
{
  Q_OBJECT
  public:
    ~MigrateManagerInternal();

    /*! Tries to load db driver \a name.
      \return db driver, or 0 if error (then error message is also set) */
    KexiMigrate* driver(const QString& name);

    static MigrateManagerInternal *self();

    /*! increments the refcount for the manager */
    void incRefCount();

    /*! decrements the refcount for the manager
      if the refcount reaches a value less than 1 the manager is freed */
    void decRefCount();

  protected slots:
    /*! Used to destroy all drivers on QApplication quit, so even if there are 
     DriverManager's static instances that are destroyed on program 
     "static destruction", drivers are not kept after QApplication death.
    */
    void slotAppQuits();

  protected:
    /*! Used by self() */
    MigrateManagerInternal();

    bool lookupDrivers();

    static MigrateManagerInternal* s_self;

    MigrateManager::ServicesMap m_services; //! services map
    MigrateManager::ServicesMap m_services_lcase; //! as above but service names in lowercase
    MigrateManager::ServicesMap m_services_by_mimetype;
    
    Q3AsciiDict<KexiMigrate> m_drivers;
    ulong m_refCount;

    QString m_serverErrMsg;
    int m_serverResultNum;
    QString m_serverResultName;
    //! result names for KParts::ComponentFactory::ComponentLoadingError
    QMap<int,QString> m_componentLoadingErrors;

    bool lookupDriversNeeded : 1;

    QStringList possibleProblems;

  friend class MigrateManager;
};
}

#endif
