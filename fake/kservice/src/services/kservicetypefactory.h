/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __k_service_type_factory_h__
#define __k_service_type_factory_h__

#include <assert.h>

#include <QtCore/QStringList>

#include "ksycocafactory.h"
#include "kservicetype.h"

class KSycoca;

class KServiceType;

/**
 * @internal
 * A sycoca factory for service types
 * It loads the service types from parsing directories (e.g. servicetypes/)
 * but can also create service types from data streams or single config files
 * @see KServiceType
 *
 * Exported for kbuildsycoca, but not installed.
 */
class KSERVICE_EXPORT KServiceTypeFactory : public KSycocaFactory
{
  K_SYCOCAFACTORY( KST_KServiceTypeFactory )
public:
  /**
   * Create factory
   */
  KServiceTypeFactory();

  virtual ~KServiceTypeFactory();

  /**
   * Not meant to be called at this level
   */
  virtual KSycocaEntry *createEntry(const QString &) const
    { assert(0); return 0; }

  /**
   * Find a service type in the database file (allocates it)
   * Overloaded by KBuildServiceTypeFactory to return a memory one.
   */
  virtual KServiceType::Ptr findServiceTypeByName(const QString &_name);

  /**
   * Find a the property type of a named property.
   */
  QVariant::Type findPropertyTypeByName(const QString &_name);

  /**
   * @return all servicetypes
   * Slow and memory consuming, avoid using
   */
  KServiceType::List allServiceTypes();

  /**
   * @return the unique servicetype factory, creating it if necessary
   */
  static KServiceTypeFactory * self();

protected:
  virtual KServiceType *createEntry(int offset) const;

  // protected for KBuildServiceTypeFactory
  QMap<QString,int> m_propertyTypeDict;

protected:
  virtual void virtual_hook( int id, void* data );
private:
  class KServiceTypeFactoryPrivate* d;
};

#endif
