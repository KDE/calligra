/*
   This file is part of the KDE libraries
   Copyright (c) 2006, 2007 Thomas Braxton <kde.braxton@gmail.com>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997-1999 Matthias Kalle Dalheimer <kalle@kde.org>

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

#include "kconfigbase.h"

#include "kconfiggroup.h"

#include <QtCore/QString>

bool KConfigBase::hasGroup(const QString &group) const
{
    return hasGroupImpl(group.toUtf8());
}

bool KConfigBase::hasGroup(const char *group) const
{
    return hasGroupImpl(QByteArray(group));
}

bool KConfigBase::hasGroup(const QByteArray &group) const
{
    return hasGroupImpl(group);
}

KConfigGroup KConfigBase::group( const QByteArray &b)
{
    return groupImpl(b);
}

KConfigGroup KConfigBase::group( const QString &str)
{
    return groupImpl(str.toUtf8());
}

KConfigGroup KConfigBase::group( const char *str)
{
    return groupImpl(QByteArray(str));
}

const KConfigGroup KConfigBase::group( const QByteArray &b ) const
{
    return groupImpl(b);
}

const KConfigGroup KConfigBase::group( const QString &s ) const
{
    return groupImpl(s.toUtf8());
}

const KConfigGroup KConfigBase::group( const char *s ) const
{
    return groupImpl(QByteArray(s));
}

void KConfigBase::deleteGroup(const QByteArray &group, WriteConfigFlags flags)
{
    deleteGroupImpl(group, flags);
}

void KConfigBase::deleteGroup(const QString &group, WriteConfigFlags flags)
{
    deleteGroupImpl(group.toUtf8(), flags);
}

void KConfigBase::deleteGroup(const char *group, WriteConfigFlags flags)
{
    deleteGroupImpl(QByteArray(group), flags);
}

bool KConfigBase::isGroupImmutable(const QByteArray& aGroup) const
{
    return isGroupImmutableImpl(aGroup);
}

bool KConfigBase::isGroupImmutable(const QString& aGroup) const
{
    return isGroupImmutableImpl(aGroup.toUtf8());
}


bool KConfigBase::isGroupImmutable(const char *aGroup) const
{
    return isGroupImmutableImpl(QByteArray(aGroup));
}

KConfigBase::~KConfigBase()
{}

KConfigBase::KConfigBase()
{}

void KConfigBase::virtual_hook(int , void *)
{}
