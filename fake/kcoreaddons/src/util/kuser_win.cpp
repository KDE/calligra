/*
 *  KUser - represent a user/account (Windows)
 *  Copyright (C) 2007 Bernhard Loos <nhuh.put@web.de>
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

#include <QtCore/QMutableStringListIterator>
#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <windows.h>
#include <lm.h>
#include <sddl.h>

class KUser::Private : public QSharedData
{
    public:
        PUSER_INFO_11 userInfo;
        PSID sid;

        Private() : userInfo(0), sid(0) {}

        Private(PUSER_INFO_11 userInfo_, PSID sid_ = 0) : userInfo(userInfo_) {}

        Private(const QString &name, PSID sid_ = 0) : userInfo(0), sid(NULL)
        {
            LPBYTE servername;
            NET_API_STATUS status = NetGetAnyDCName(0, 0, &servername);
            if (status != NERR_Success)
            {
                servername = NULL;
            }

            if (NetUserGetInfo((LPCWSTR) servername, (LPCWSTR) name.utf16(), 11, (LPBYTE *) &userInfo) != NERR_Success) {
                goto error;
            }
            if (servername)
            {
                NetApiBufferFree(servername);
                servername = 0;
            }

            if (!sid_) {
                DWORD size = 0;
                SID_NAME_USE nameuse;
                DWORD cchReferencedDomainName = 0;
                WCHAR* referencedDomainName = NULL;

                // the following line definitely fails:
                // both the sizes for sid and for referencedDomainName are Null
                // the needed sizes are set in size and cchReferencedDomainName
                LookupAccountNameW(NULL, (LPCWSTR) name.utf16(), sid, &size, referencedDomainName, &cchReferencedDomainName, &nameuse);
                sid = (PSID) new SID[size + 1];
                referencedDomainName = new WCHAR[cchReferencedDomainName + 1];
                if (!LookupAccountNameW(NULL, (LPCWSTR) name.utf16(), sid, &size, referencedDomainName, &cchReferencedDomainName, &nameuse)) {
                    delete[] referencedDomainName;
                    goto error;
                }

                // if you want to see both the DomainName and the sid of the user
                // uncomment the following lines
/*                LPWSTR sidstring;
                ConvertSidToStringSidW(sid, &sidstring);
                qDebug() << QString("\\\\") + QString::fromUtf16(reinterpret_cast<ushort*>(referencedDomainName)) + \
                            "\\" + name + "(" + QString::fromUtf16(reinterpret_cast<ushort*>(sidstring)) + ")";

                LocalFree(sidstring);*/
                delete[] referencedDomainName;
            }
            else {
                if (!IsValidSid(sid_))
                    goto error;

                DWORD sidlength = GetLengthSid(sid_);
                sid = (PSID) new BYTE[sidlength];
                if (!CopySid(sidlength, sid, sid_))
                    goto error;
            }

            return;

          error:
            delete[] sid;
            sid = 0;
            if (userInfo) {
                NetApiBufferFree(userInfo);
                userInfo = 0;
            }
            if (servername)
            {
                NetApiBufferFree(servername);
                servername = 0;
            }
        }

        ~Private()
        {
            if (userInfo)
                NetApiBufferFree(userInfo);

            delete[] sid;
        }
};

KUser::KUser(UIDMode mode)
        : d(0)
{
    Q_UNUSED(mode)

    DWORD bufferLen = UNLEN + 1;
    ushort buffer[UNLEN + 1];

    if (GetUserNameW((LPWSTR) buffer, &bufferLen))
        d = new Private(QString::fromUtf16(buffer));
}

KUser::KUser(K_UID uid)
    : d(0)
{
    DWORD bufferLen = UNLEN + 1;
    ushort buffer[UNLEN + 1];
    SID_NAME_USE eUse;

    if (LookupAccountSidW(NULL, uid, (LPWSTR) buffer, &bufferLen, NULL, NULL, &eUse))
        d = new Private(QString::fromUtf16(buffer), uid);
}

KUser::KUser(const QString &name)
    : d(new Private(name))
{
}

KUser::KUser(const char *name)
    :d(new Private(QString::fromLocal8Bit(name)))
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
    return EqualSid(d->sid, user.d->sid);
}

bool KUser::operator !=(const KUser &user) const
{
    return !operator==(user);
}

bool KUser::isValid() const
{
    return d->userInfo != 0 && d->sid != 0;
}

bool KUser::isSuperUser() const
{
    return d->userInfo && d->userInfo->usri11_priv == USER_PRIV_ADMIN;
}

QString KUser::loginName() const
{
    return (d->userInfo ? QString::fromUtf16((ushort *) d->userInfo->usri11_name) : QString());
}

#ifndef KDE_NO_DEPRECATED
QString KUser::fullName() const
{
    return (d->userInfo ? QString::fromUtf16((ushort *) d->userInfo->usri11_full_name) : QString());
}
#endif

QString KUser::homeDir() const
{
	return QDir::fromNativeSeparators(QString::fromLocal8Bit(qgetenv("USERPROFILE")));
}

QString KUser::faceIconPath() const
{
    // FIXME: this needs to be adapted to windows systems (BC changes)
    return QString();
}

QString KUser::shell() const
{
    return QString::fromLatin1("cmd.exe");
}

QList<KUserGroup> KUser::groups() const
{
    QList<KUserGroup> result;

    Q_FOREACH (const QString &name, groupNames()) {
        result.append(KUserGroup(name));
    }

    return result;
}

QStringList KUser::groupNames() const
{
    QStringList result;

    if (!d->userInfo) {
        return result;
    }

    PGROUP_USERS_INFO_0 pGroups = NULL;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus;

    nStatus = NetUserGetGroups(NULL, d->userInfo->usri11_name, 0, (LPBYTE *) &pGroups, MAX_PREFERRED_LENGTH, &dwEntriesRead, &dwTotalEntries);

    if (nStatus == NERR_Success) {
        for (DWORD i = 0; i < dwEntriesRead; ++i) {
            result.append(QString::fromUtf16((ushort *) pGroups[i].grui0_name));
        }
    }

    if (pGroups) {
        NetApiBufferFree(pGroups);
    }

    return result;
}

K_UID KUser::uid() const
{
    return d->sid;
}

QVariant KUser::property(UserProperty which) const
{
    if (which == FullName)
        return QVariant(d->userInfo ? QString::fromUtf16((ushort *) d->userInfo->usri11_full_name) : QString());

    return QVariant();
}

QList<KUser> KUser::allUsers()
{
    QList<KUser> result;

    NET_API_STATUS nStatus;
    PUSER_INFO_11 pUser = NULL;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;

    KUser tmp;

    do {
        nStatus = NetUserEnum(NULL, 11, 0, (LPBYTE*) &pUser, 1, &dwEntriesRead, &dwTotalEntries, &dwResumeHandle);

        if ((nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) && dwEntriesRead > 0) {
            tmp.d = new Private(pUser);
            result.append(tmp);
        }
    } while (nStatus == ERROR_MORE_DATA);

    return result;
}

QStringList KUser::allUserNames()
{
    QStringList result;

    NET_API_STATUS nStatus;
    PUSER_INFO_0 pUsers = NULL;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;

    nStatus = NetUserEnum(NULL, 0, 0, (LPBYTE*) &pUsers, MAX_PREFERRED_LENGTH, &dwEntriesRead, &dwTotalEntries, NULL);

    if (nStatus == NERR_Success) {
        for (DWORD i = 0; i < dwEntriesRead; ++i) {
            result.append(QString::fromUtf16((ushort *) pUsers[i].usri0_name));
        }
    }

    if (pUsers) {
        NetApiBufferFree(pUsers);
    }

    return result;
}

KUser::~KUser()
{
}

class KUserGroup::Private : public QSharedData
{
    public:
        PGROUP_INFO_0 groupInfo;

        Private() : groupInfo(NULL) {}
        Private(PGROUP_INFO_0 groupInfo_) : groupInfo(groupInfo_) {}
        Private(const QString &Name) : groupInfo(NULL)
        {
            NetGroupGetInfo(NULL, (PCWSTR) Name.utf16(), 0, (PBYTE *) &groupInfo);
        }

        ~Private()
        {
            if (groupInfo) {
                NetApiBufferFree(groupInfo);
            }
        }
};

KUserGroup::KUserGroup(const QString &_name)
    : d(new Private(_name))
{
}

KUserGroup::KUserGroup(const char *_name)
    : d(new Private(QLatin1String(_name)))
{
}

KUserGroup::KUserGroup(const KUserGroup &group)
    : d(group.d)
{
}

KUserGroup& KUserGroup::operator =(const KUserGroup &group)
{
    d = group.d;
    return *this;
}

bool KUserGroup::operator==(const KUserGroup &group) const
{
    if (d->groupInfo == NULL || group.d->groupInfo == NULL) {
        return false;
    }
    return wcscmp(d->groupInfo->grpi0_name, group.d->groupInfo->grpi0_name) == 0;
}

bool KUserGroup::operator!=(const KUserGroup &group) const
{
    return !operator==(group);
}

bool KUserGroup::isValid() const
{
    return d->groupInfo != NULL;
}

QString KUserGroup::name() const
{
    if(d && d->groupInfo)
        return QString::fromUtf16((ushort *) d->groupInfo->grpi0_name);
    return QString();
}

QList<KUser> KUserGroup::users() const
{
    QList<KUser> Result;

    Q_FOREACH(const QString &user, userNames()) {
        Result.append(KUser(user));
    }

    return Result;
}

QStringList KUserGroup::userNames() const
{
    QStringList result;

    if (!d->groupInfo) {
        return result;
    }

    PGROUP_USERS_INFO_0 pUsers = NULL;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus;

    nStatus = NetGroupGetUsers(NULL, d->groupInfo->grpi0_name, 0, (LPBYTE *) &pUsers, MAX_PREFERRED_LENGTH, &dwEntriesRead, &dwTotalEntries, NULL);

    if (nStatus == NERR_Success) {
        for (DWORD i = 0; i < dwEntriesRead; ++i) {
            result.append(QString::fromUtf16((ushort *) pUsers[i].grui0_name));
        }
    }

    if (pUsers) {
        NetApiBufferFree(pUsers);
    }

    return result;
}

QList<KUserGroup> KUserGroup::allGroups()
{
    QList<KUserGroup> result;

    NET_API_STATUS nStatus;
    PGROUP_INFO_0 pGroup=NULL;
    DWORD dwEntriesRead=0;
    DWORD dwTotalEntries=0;
    DWORD dwResumeHandle=0;

    KUserGroup tmp("");

    do {
        nStatus = NetGroupEnum(NULL, 0, (LPBYTE*) &pGroup, 1, &dwEntriesRead, &dwTotalEntries, (PDWORD_PTR)&dwResumeHandle);

        if ((nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) && dwEntriesRead > 0) {
            tmp.d = new Private(pGroup);
            result.append(tmp);
        }
    } while (nStatus == ERROR_MORE_DATA);

    return result;
}

QStringList KUserGroup::allGroupNames()
{
    QStringList result;

    NET_API_STATUS nStatus;
    PGROUP_INFO_0 pGroups=NULL;
    DWORD dwEntriesRead=0;
    DWORD dwTotalEntries=0;

    nStatus = NetGroupEnum(NULL, 0, (LPBYTE*) &pGroups, MAX_PREFERRED_LENGTH, &dwEntriesRead, &dwTotalEntries, NULL);

    if (nStatus == NERR_Success) {
        for (DWORD i = 0; i < dwEntriesRead; ++i) {
            result.append(QString::fromUtf16((ushort *) pGroups[i].grpi0_name));
        }
    }

    if (pGroups) {
        NetApiBufferFree(pGroups);
    }

    return result;
}

KUserGroup::~KUserGroup()
{
}
