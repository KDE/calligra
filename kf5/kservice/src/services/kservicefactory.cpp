/*  This file is part of the KDE libraries
 *  Copyright (C) 1999-2006 David Faure <faure@kde.org>
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

#include "kservicefactory.h"
#include "ksycoca.h"
#include "ksycocatype.h"
#include "ksycocadict_p.h"
#include "kservice.h"
#include <QDebug>


extern int servicesDebugArea();

Q_GLOBAL_STATIC(KSycocaFactorySingleton<KServiceFactory>, kServiceFactoryInstance)

KServiceFactory::KServiceFactory()
    : KSycocaFactory( KST_KServiceFactory ),
    m_nameDict(0),
    m_relNameDict(0),
    m_menuIdDict(0)
{
    kServiceFactoryInstance()->instanceCreated(this);
    m_offerListOffset = 0;
    m_nameDictOffset = 0;
    m_relNameDictOffset = 0;
    m_menuIdDictOffset = 0;
    if (!KSycoca::self()->isBuilding()) {
        QDataStream* str = stream();
        Q_ASSERT(str);
        if (!str)
            return;
        // Read Header
        qint32 i;
        (*str) >> i;
        m_nameDictOffset = i;
        (*str) >> i;
        m_relNameDictOffset = i;
        (*str) >> i;
        m_offerListOffset = i;
        (*str) >> i;
        m_menuIdDictOffset = i;

        const int saveOffset = str->device()->pos();
        // Init index tables
        m_nameDict = new KSycocaDict(str, m_nameDictOffset);
        // Init index tables
        m_relNameDict = new KSycocaDict(str, m_relNameDictOffset);
        // Init index tables
        m_menuIdDict = new KSycocaDict(str, m_menuIdDictOffset);
        str->device()->seek(saveOffset);
    }
}

KServiceFactory::~KServiceFactory()
{
    if (kServiceFactoryInstance())
        kServiceFactoryInstance()->instanceDestroyed(this);
    delete m_nameDict;
    delete m_relNameDict;
    delete m_menuIdDict;
}

KServiceFactory * KServiceFactory::self()
{
    return kServiceFactoryInstance()->self();
}

KService::Ptr KServiceFactory::findServiceByName(const QString &_name)
{
    if (!sycocaDict()) return KService::Ptr(); // Error!

    // Warning : this assumes we're NOT building a database
    // But since findServiceByName isn't called in that case...
    // [ see KServiceTypeFactory for how to do it if needed ]

    int offset = sycocaDict()->find_string( _name );
    if (!offset) return KService::Ptr(); // Not found

    KService::Ptr newService(createEntry(offset));

    // Check whether the dictionary was right.
    if (newService && (newService->name() != _name)) {
        // No it wasn't...
        return KService::Ptr();
    }
    return newService;
}

KService::Ptr KServiceFactory::findServiceByDesktopName(const QString &_name)
{
    if (!m_nameDict) return KService::Ptr(); // Error!

    // Warning : this assumes we're NOT building a database
    // KBuildServiceFactory reimplements it for the case where we are building one

    int offset = m_nameDict->find_string( _name );
    if (!offset) return KService::Ptr(); // Not found

    KService::Ptr newService(createEntry(offset));

    // Check whether the dictionary was right.
    if (newService && (newService->desktopEntryName() != _name)) {
        // No it wasn't...
        return KService::Ptr();
    }
    return newService;
}

KService::Ptr KServiceFactory::findServiceByDesktopPath(const QString &_name)
{
    if (!m_relNameDict) return KService::Ptr(); // Error!

    // Warning : this assumes we're NOT building a database
    // KBuildServiceFactory reimplements it for the case where we are building one

    int offset = m_relNameDict->find_string( _name );
    if (!offset) {
        //qDebug() << "findServiceByDesktopPath:" << _name << "not found";
        return KService::Ptr(); // Not found
    }

    KService::Ptr newService(createEntry(offset));
    if (!newService) {
        qDebug() << "createEntry failed!";
    }
    // Check whether the dictionary was right
    // It's ok that it's wrong, for the case where we're looking up an unknown service,
    // and the hash value gave us another one.
    if (newService && (newService->entryPath() != _name)) {
        // No it wasn't...
        return KService::Ptr();
    }
    return newService;
}

KService::Ptr KServiceFactory::findServiceByMenuId(const QString &_menuId)
{
    if (!m_menuIdDict) return KService::Ptr(); // Error!

    // Warning : this assumes we're NOT building a database
    // KBuildServiceFactory reimplements it for the case where we are building one

    int offset = m_menuIdDict->find_string( _menuId );
    if (!offset) return KService::Ptr(); // Not found

    KService::Ptr newService(createEntry(offset));

    // Check whether the dictionary was right.
    if (newService && (newService->menuId() != _menuId)) {
        // No it wasn't...
        return KService::Ptr();
    }
    return newService;
}

KService* KServiceFactory::createEntry(int offset) const
{
    KService * newEntry = 0L;
    KSycocaType type;
    QDataStream *str = KSycoca::self()->findEntry(offset, type);
    switch(type) {
    case KST_KService:
        newEntry = new KService(*str, offset);
        break;
    default:
        qWarning() << "KServiceFactory: unexpected object entry in KSycoca database (type=" << int(type) << ")";
        return 0;
    }
    if (!newEntry->isValid()) {
        qWarning() << "KServiceFactory: corrupt object in KSycoca database!";
        delete newEntry;
        newEntry = 0;
    }
    return newEntry;
}

KService::List KServiceFactory::allServices()
{
    KService::List result;
    const KSycocaEntry::List list = allEntries();
    KSycocaEntry::List::const_iterator it = list.begin();
    const KSycocaEntry::List::const_iterator end = list.end();
    for( ; it != end; ++it ) {
        const KSycocaEntry::Ptr entry = *it;
        if ( entry->isType( KST_KService ) )
            result.append( KService::Ptr::staticCast( entry ) );
    }
    return result;
}

QList<KServiceOffer> KServiceFactory::offers( int serviceTypeOffset, int serviceOffersOffset )
{
    QList<KServiceOffer> list;

    // Jump to the offer list
    QDataStream* str = stream();
    str->device()->seek( m_offerListOffset + serviceOffersOffset );

    qint32 aServiceTypeOffset, aServiceOffset, initialPreference, mimeTypeInheritanceLevel;
    while (true)
    {
        (*str) >> aServiceTypeOffset;
        if ( aServiceTypeOffset ) {
            (*str) >> aServiceOffset;
            (*str) >> initialPreference;
            (*str) >> mimeTypeInheritanceLevel;
            if ( aServiceTypeOffset == serviceTypeOffset ) {
                // Save stream position !
                const int savedPos = str->device()->pos();
                // Create Service
                KService * serv = createEntry( aServiceOffset );
                if (serv) {
                    KService::Ptr servPtr( serv );
                    list.append( KServiceOffer( servPtr, initialPreference, mimeTypeInheritanceLevel, servPtr->allowAsDefault() ) );
                }
                // Restore position
                str->device()->seek( savedPos );
            } else
                break; // too far
        }
        else
            break; // 0 => end of list
    }
    return list;
}

KService::List KServiceFactory::serviceOffers( int serviceTypeOffset, int serviceOffersOffset )
{
    KService::List list;

    // Jump to the offer list
    QDataStream* str = stream();
    str->device()->seek( m_offerListOffset + serviceOffersOffset );

    qint32 aServiceTypeOffset, aServiceOffset, initialPreference, mimeTypeInheritanceLevel;
    while (true) {
        (*str) >> aServiceTypeOffset;
        if ( aServiceTypeOffset )
        {
            (*str) >> aServiceOffset;
            (*str) >> initialPreference;
            (*str) >> mimeTypeInheritanceLevel;
            if ( aServiceTypeOffset == serviceTypeOffset )
            {
                // Save stream position !
                const int savedPos = str->device()->pos();
                // Create service
                KService * serv = createEntry( aServiceOffset );
                if (serv)
                    list.append( KService::Ptr( serv ) );
                // Restore position
                str->device()->seek( savedPos );
            } else
                break; // too far
        }
        else
            break; // 0 => end of list
    }
    return list;
}

bool KServiceFactory::hasOffer( int serviceTypeOffset, int serviceOffersOffset, int testedServiceOffset )
{
    // Save stream position
    QDataStream* str = stream();
    const int savedPos = str->device()->pos();

    // Jump to the offer list
    str->device()->seek( m_offerListOffset + serviceOffersOffset );
    bool found = false;
    qint32 aServiceTypeOffset, aServiceOffset, initialPreference, mimeTypeInheritanceLevel;
    while (!found)
    {
        (*str) >> aServiceTypeOffset;
        if ( aServiceTypeOffset ) {
            (*str) >> aServiceOffset;
            (*str) >> initialPreference;
            (*str) >> mimeTypeInheritanceLevel;
            if ( aServiceTypeOffset == serviceTypeOffset )
            {
                if( aServiceOffset == testedServiceOffset )
                    found = true;
            } else
                break; // too far
        }
        else
            break; // 0 => end of list
    }
    // Restore position
    str->device()->seek( savedPos );
    return found;
}

void KServiceFactory::virtual_hook( int id, void* data )
{ KSycocaFactory::virtual_hook( id, data ); }

