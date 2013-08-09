/*  This file is part of the KDE libraries
 *  Copyright (C) 2000 Waldo Bastian <bastian@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include "kservicegroupfactory.h"
#include "ksycoca.h"
#include "ksycocatype.h"
#include "ksycocadict_p.h"
#include "kservice.h"

#include <QDebug>

Q_GLOBAL_STATIC(KSycocaFactorySingleton<KServiceGroupFactory>, kServiceGroupFactoryInstance)

KServiceGroupFactory::KServiceGroupFactory()
    : KSycocaFactory( KST_KServiceGroupFactory )
{
    kServiceGroupFactoryInstance()->instanceCreated(this);
    m_baseGroupDictOffset = 0;
    if (!KSycoca::self()->isBuilding()) {
        QDataStream* str = stream();
        // Read Header
        qint32 i;
        (*str) >> i;
        m_baseGroupDictOffset = i;

        const int saveOffset = str->device()->pos();
        // Init index tables
        m_baseGroupDict = new KSycocaDict(str, m_baseGroupDictOffset);
        str->device()->seek(saveOffset);
    }
}

KServiceGroupFactory::~KServiceGroupFactory()
{
    delete m_baseGroupDict;
    if (kServiceGroupFactoryInstance())
        kServiceGroupFactoryInstance()->instanceDestroyed(this);
}

KServiceGroupFactory * KServiceGroupFactory::self()
{
    return kServiceGroupFactoryInstance()->self();
}

KServiceGroup::Ptr KServiceGroupFactory::findGroupByDesktopPath(const QString &_name, bool deep)
{
   if (!sycocaDict()) return KServiceGroup::Ptr(); // Error!
   int offset = sycocaDict()->find_string( _name );
   if (!offset) return KServiceGroup::Ptr(); // Not found

   KServiceGroup::Ptr newGroup(createGroup(offset, deep));

   // Check whether the dictionary was right.
   if (newGroup && (newGroup->relPath() != _name))
   {
      // No it wasn't...
      newGroup = 0; // Not found
   }
   return newGroup;
}

KServiceGroup::Ptr KServiceGroupFactory::findBaseGroup(const QString &_baseGroupName, bool deep)
{
   if (!m_baseGroupDict) return KServiceGroup::Ptr(); // Error!

   // Warning : this assumes we're NOT building a database
   // But since findBaseGroup isn't called in that case...
   // [ see KServiceTypeFactory for how to do it if needed ]

   int offset = m_baseGroupDict->find_string( _baseGroupName );
   if (!offset) return KServiceGroup::Ptr(); // Not found

   KServiceGroup::Ptr newGroup(createGroup(offset, deep));

   // Check whether the dictionary was right.
   if (newGroup && (newGroup->baseGroupName() != _baseGroupName))
   {
      // No it wasn't...
      newGroup = 0; // Not found
   }
   return newGroup;
}

KServiceGroup* KServiceGroupFactory::createGroup(int offset, bool deep) const
{
   KServiceGroup * newEntry = 0L;
   KSycocaType type;
   QDataStream *str = KSycoca::self()->findEntry(offset, type);
   switch(type)
   {
     case KST_KServiceGroup:
        newEntry = new KServiceGroup(*str, offset, deep);
        break;

     default:
        qWarning() << "KServiceGroupFactory: unexpected object entry in KSycoca database (type = " << int(type) << ")";
        return 0;
   }
   if (!newEntry->isValid())
   {
      qWarning() << "KServiceGroupFactory: corrupt object in KSycoca database!";
      delete newEntry;
      newEntry = 0;
   }
   return newEntry;
}

KServiceGroup* KServiceGroupFactory::createEntry(int offset) const
{
   return createGroup(offset, true);
}

void KServiceGroupFactory::virtual_hook( int id, void* data )
{ KSycocaFactory::virtual_hook( id, data ); }

