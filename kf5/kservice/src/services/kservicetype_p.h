/* This file is part of the KDE project
Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
1999 Waldo Bastian <bastian@kde.org>

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

#ifndef __kservicetype_p_h__
#define __kservicetype_p_h__

#include "kservicetype.h"
#include <ksycocaentry_p.h>

class KServiceTypePrivate : public KSycocaEntryPrivate
{
public:
    K_SYCOCATYPE( KST_KServiceType, KSycocaEntryPrivate )

    KServiceTypePrivate(const QString &path)
        : KSycocaEntryPrivate(path),
          m_serviceOffersOffset( -1 ), m_bDerived(false), m_parentTypeLoaded(false)
    {
    }

    KServiceTypePrivate(QDataStream &_str, int offset)
        : KSycocaEntryPrivate(_str, offset),
          m_serviceOffersOffset( -1 ), m_bDerived(false), m_parentTypeLoaded(false)
    {
    }

    virtual ~KServiceTypePrivate() {}

    virtual void save( QDataStream& );

    virtual QString name() const
    {
        return m_strName;
    }

    virtual QVariant property(const QString &name) const;

    virtual QStringList propertyNames() const;

    virtual QString comment() const
    {
        return m_strComment;
    }

    virtual int serviceOffersOffset() const { return m_serviceOffersOffset; }

    void init( KDesktopFile *config );
    void load(QDataStream& _str);

    KServiceType::Ptr parentType;
    QString m_strName;
    mutable /*remove mutable when kmimetype doesn't use this anymore*/ QString m_strComment;
    int m_serviceOffersOffset;
    QMap<QString, QVariant::Type> m_mapPropDefs;
    QMap<QString,QVariant> m_mapProps;
    unsigned m_bDerived: 1;
    unsigned m_parentTypeLoaded: 1;
};

#endif // __kservicetype_p_h__

