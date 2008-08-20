/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPROJECTPARTITEM_P_H
#define KEXIPROJECTPARTITEM_P_H

#include "kexipartinfo.h"
#include <kservice.h>

namespace KexiPart
{
//! @internal
class Info::Private
{
public:
    Private(const KService::Ptr& aPtr);

    //! used in StaticItem class
    Private();

    KService::Ptr ptr;
    QString errorMessage;
    QString groupName;
    QString mimeType;
    QString itemIcon;
    QString objectName;
    int projectPartID;
bool broken : 1;
bool isVisibleInNavigator : 1;
bool idStoredInPartDatabase : 1;
};
}

#endif
