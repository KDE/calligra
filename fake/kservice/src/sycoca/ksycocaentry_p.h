/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
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

#ifndef KSYCOCAENTRYPRIVATE_H
#define KSYCOCAENTRYPRIVATE_H

#include "ksycocaentry.h"

#define K_SYCOCATYPE( type, baseclass ) \
 virtual bool isType(KSycocaType t) const { if (t == type) return true; return baseclass::isType(t);} \
 virtual KSycocaType sycocaType() const { return type; }


class KSycocaEntryPrivate
{
public:
    KSycocaEntryPrivate(const QString &path_)
        : offset( 0 ),
          deleted( false ), path(path_)
    {}

    KSycocaEntryPrivate(QDataStream &_str, int iOffset);

    virtual ~KSycocaEntryPrivate() {}

    // Don't forget to call the parent class
    // first if you override this function.
    virtual void save(QDataStream &s);

    virtual bool isType(KSycocaType t) const
    {
        return (t == KST_KSycocaEntry);
    }

    virtual KSycocaType sycocaType() const
    {
        return KST_KSycocaEntry;
    }

    virtual bool isValid() const
    {
        return !name().isEmpty();
    }

    virtual QVariant property(const QString &name) const
    {
        Q_UNUSED(name)
        return QVariant();
    }

    virtual QStringList propertyNames() const
    {
        return QStringList();
    }

    virtual QString name() const = 0;

    virtual QString storageId() const { return name(); }

    int offset;
    bool deleted;
    QString path;
};



#endif
