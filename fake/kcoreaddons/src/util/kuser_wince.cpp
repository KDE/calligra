/*
 *  KUser - represent a user/account (Windows)
 *  Copyright (C) 2010 Andreas Holzammer <andreas.holzammer@kdab.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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
 */

#include "kuser.h"

#include <QStringList>


class KUser::Private : public QSharedData
{
};

KUser::KUser(UIDMode mode)
        : d(0)
{
    Q_UNUSED(mode)
}

KUser::KUser(K_UID uid)
    : d(0)
{
    Q_UNUSED(uid)
}

KUser::KUser(const QString &name)
    : d(0)
{
}

KUser::KUser(const char *name)
    :d(0)
{
}

KUser::KUser(const KUser &user)
    : d(user.d)
{
}

KUser &KUser::operator=(const KUser &user)
{
    d = user.d;
    return *this;
}

bool KUser::operator==(const KUser &user) const
{
    if (!isValid() || !user.isValid())
        return false;
    return true;
}

bool KUser::operator !=(const KUser &user) const
{
    return !operator==(user);
}

bool KUser::isValid() const
{
    return true;
}

bool KUser::isSuperUser() const
{
    return true;
}

QString KUser::loginName() const
{
    return QString("default");
}

#ifndef KDE_NO_DEPRECATED
QString KUser::fullName() const
{
    return QString("default");
}
#endif

QString KUser::homeDir() const
{
	return QString("\\Documents and Settings\\default");
}

QString KUser::faceIconPath() const
{
    return QString();
}

QString KUser::shell() const
{
    return QString::fromLatin1("cmd.exe");
}

QList<KUserGroup> KUser::groups() const
{
    return QList<KUserGroup>();
}

QStringList KUser::groupNames() const
{
    return QStringList();
}

K_UID KUser::uid() const
{
    return (K_UID)100;
}

QVariant KUser::property(UserProperty which) const
{
    return QVariant();
}

QList<KUser> KUser::allUsers()
{
    QList<KUser> result;

    result.append(KUser());

    return result;
}

QStringList KUser::allUserNames()
{
    QStringList result;

    result.append("wince");

    return result;
}

KUser::~KUser()
{
}

class KUserGroup::Private : public QSharedData
{
};

KUserGroup::KUserGroup(const QString &_name)
    : d(0)
{
}

KUserGroup::KUserGroup(const char *_name)
    : d(0)
{
}

KUserGroup::KUserGroup(const KUserGroup &group)
    : d(0)
{
}

KUserGroup& KUserGroup::operator =(const KUserGroup &group)
{
    d = group.d;
    return *this;
}

bool KUserGroup::operator==(const KUserGroup &group) const
{

    return true;
}

bool KUserGroup::operator!=(const KUserGroup &group) const
{
    return !operator==(group);
}

bool KUserGroup::isValid() const
{
    return true;
}

QString KUserGroup::name() const
{
    return QString("wince");
}

QList<KUser> KUserGroup::users() const
{
    QList<KUser> Result;

    Result.append(KUser());

    return Result;
}

QStringList KUserGroup::userNames() const
{
    QStringList result;

    result.append("default");

    return result;
}

QList<KUserGroup> KUserGroup::allGroups()
{
    QList<KUserGroup> result;

    result.append(KUserGroup(""));

    return result;
}

QStringList KUserGroup::allGroupNames()
{
    QStringList result;

    result.append("wince");

    return result;
}

KUserGroup::~KUserGroup()
{
}
