/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXISEARCHABLEMODEL_H
#define KEXISEARCHABLEMODEL_H

#include <kexi_global.h>

class QModelIndex;
class QVariant;

class KEXICORE_EXPORT KexiSearchableModel
{
public:
    KexiSearchableModel();
    virtual ~KexiSearchableModel();
    virtual int searchableObjectCount() const = 0;
    virtual QModelIndex sourceIndexForSearchableObject(int objectIndex) const = 0;
    virtual QVariant searchableData(const QModelIndex &sourceIndex, int role) const = 0;
    virtual QString pathFromIndex(const QModelIndex &sourceIndex) const = 0;
    virtual bool highlightSearchableObject(const QModelIndex &index) = 0;
    virtual bool activateSearchableObject(const QModelIndex &index) = 0;
};

#endif
