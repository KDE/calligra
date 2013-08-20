/* Copyright (C) 2008 Michael Jansen <kde@michael-jansen.biz>

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

#include "kglobalshortcutinfo.h"
#include "kglobalshortcutinfo_p.h"

KGlobalShortcutInfo::KGlobalShortcutInfo()
        :   d(new KGlobalShortcutInfoPrivate)
    {}


KGlobalShortcutInfo::KGlobalShortcutInfo(const KGlobalShortcutInfo &rhs)
    :   QObject()
        ,d(new KGlobalShortcutInfoPrivate)
    {
    d->contextUniqueName     = rhs.d->contextUniqueName;
    d->contextFriendlyName   = rhs.d->contextFriendlyName;
    d->componentFriendlyName = rhs.d->componentFriendlyName;
    d->componentUniqueName   = rhs.d->componentUniqueName;
    d->friendlyName          = rhs.d->friendlyName;
    d->uniqueName            = rhs.d->uniqueName;
    d->keys                  = rhs.d->keys;
    d->defaultKeys           = rhs.d->defaultKeys;
    }


KGlobalShortcutInfo::~KGlobalShortcutInfo()
    {
    delete d;
    }


KGlobalShortcutInfo &KGlobalShortcutInfo::operator=(const KGlobalShortcutInfo &rhs)
    {
    KGlobalShortcutInfo tmp(rhs);
    KGlobalShortcutInfoPrivate *swap;
    swap = d;
    d = tmp.d;
    tmp.d = swap;
    return *this;
    }


QString KGlobalShortcutInfo::contextFriendlyName() const
    {
    return d->contextFriendlyName.isEmpty()
        ? d->contextUniqueName
        : d->contextFriendlyName;
    }


QString KGlobalShortcutInfo::contextUniqueName() const
    {
    return d->contextUniqueName;
    }


QString KGlobalShortcutInfo::componentFriendlyName() const
    {
    return d->componentFriendlyName.isEmpty()
        ? d->componentUniqueName
        : d->componentFriendlyName;
    }


QString KGlobalShortcutInfo::componentUniqueName() const
    {
    return d->componentUniqueName;
    }


QList<QKeySequence> KGlobalShortcutInfo::defaultKeys() const
    {
    return d->defaultKeys;
    }


QString KGlobalShortcutInfo::friendlyName() const
    {
    return d->friendlyName;
    }


QList<QKeySequence> KGlobalShortcutInfo::keys() const
    {
    return d->keys;
    }

QString KGlobalShortcutInfo::uniqueName() const
    {
    return d->uniqueName;
    }


