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



QDBusArgument &operator<< (QDBusArgument &argument, const KGlobalShortcutInfo &shortcut)
    {
    argument.beginStructure();
    argument << shortcut.uniqueName()
             << shortcut.friendlyName()
             << shortcut.componentUniqueName()
             << shortcut.componentFriendlyName()
             << shortcut.contextUniqueName()
             << shortcut.contextFriendlyName();
    argument.beginArray(qMetaTypeId<int>());
    Q_FOREACH(const QKeySequence &key, shortcut.keys())
        {
        argument << key[0];
        }
    argument.endArray();
    argument.beginArray(qMetaTypeId<int>());
    Q_FOREACH(const QKeySequence &key, shortcut.defaultKeys())
        {
        argument << key[0];
        }
    argument.endArray();
    argument.endStructure();
    return argument;
    }


const QDBusArgument &operator>> (const QDBusArgument &argument, KGlobalShortcutInfo &shortcut)
    {
    argument.beginStructure();
    argument >> shortcut.d->uniqueName
             >> shortcut.d->friendlyName
             >> shortcut.d->componentUniqueName
             >> shortcut.d->componentFriendlyName
             >> shortcut.d->contextUniqueName
             >> shortcut.d->contextFriendlyName;
    argument.beginArray();
    while (!argument.atEnd())
        {
        int key;
        argument >> key;
        shortcut.d->keys.append(QKeySequence(key));
        }
    argument.endArray();
    argument.beginArray();
    while (!argument.atEnd())
        {
        int key;
        argument >> key;
        shortcut.d->defaultKeys.append(QKeySequence(key));
        }
    argument.endArray();
    argument.endStructure();
    return argument;
    }

