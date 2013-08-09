/*
   This file is part of the KDE libraries
   Copyright (c) 2007 Thiago Macieira <thiago@kde.org>

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

#ifndef KCONFIGGROUP_P_H
#define KCONFIGGROUP_P_H

#include <QtCore/QVariant>
#include "kconfiggroup.h"

class KConfigGroup;

struct KConfigGroupGui
{
    typedef bool (*kReadEntryGui)(const QByteArray& data, const char* key, const QVariant &input,
                                  QVariant &output);
    typedef bool (*kWriteEntryGui)(KConfigGroup *, const char* key, const QVariant &input,
                                   KConfigGroup::WriteConfigFlags flags);

    kReadEntryGui readEntryGui;
    kWriteEntryGui writeEntryGui;
};

extern KCONFIGCORE_EXPORT KConfigGroupGui _kde_internal_KConfigGroupGui;

#endif
