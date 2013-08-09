/* This file is part of the KDE project
   Copyright (C) 2000 Waldo Bastian <bastian@kde.org>

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

#ifndef KSERVICEGROUPFACTORY_H
#define KSERVICEGROUPFACTORY_H

#include <QtCore/QStringList>

#include "kservicegroup.h"
#include "ksycocafactory.h"
#include <assert.h>

class KSycoca;
class KSycocaDict;

/**
 * @internal
 * A sycoca factory for service groups (e.g. list of applications)
 * It loads the services from parsing directories (e.g. share/applications/)
 *
 * Exported for kbuildsycoca, but not installed.
 */
class KSERVICE_EXPORT KServiceGroupFactory : public KSycocaFactory
{
  K_SYCOCAFACTORY( KST_KServiceGroupFactory )
public:
  /**
   * Create factory
   */
  KServiceGroupFactory();
  virtual ~KServiceGroupFactory();

  /**
   * Construct a KServiceGroup from a config file.
   */
   virtual KSycocaEntry *createEntry(const QString &) const
    { assert(0); return 0; }

  /**
   * Find a group ( by desktop path, e.g. "Applications/Editors")
   */
  virtual KServiceGroup::Ptr findGroupByDesktopPath( const QString &_name, bool deep = true );

  /**
   * Find a base group by name, e.g. "settings"
   */
  KServiceGroup::Ptr findBaseGroup( const QString &_baseGroupName, bool deep = true );

  /**
   * @return the unique service group factory, creating it if necessary
   */
  static KServiceGroupFactory * self();
protected:
  KServiceGroup* createGroup(int offset, bool deep) const;
  KServiceGroup* createEntry(int offset) const;
  KSycocaDict *m_baseGroupDict;
  int m_baseGroupDictOffset;

protected:
  virtual void virtual_hook( int id, void* data );
private:
  class KServiceGroupFactoryPrivate* d;
};

#endif
