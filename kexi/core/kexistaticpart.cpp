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

#include "kexistaticpart.h"
#include "kexipartinfo_p.h"
#include "kexipartitem.h"
#include "kexi.h"

using namespace KexiPart;

//------------------------------

StaticInfo::StaticInfo(const QString& partClass, const QString& itemIcon,
                       const QString& objectName)
        : Info()
{
    d->partClass = partClass;
    d->itemIcon = itemIcon;
    d->objectName = objectName;
}

StaticInfo::~StaticInfo()
{
}

//------------------------------

StaticPart::StaticPart(const QString& partClass, const QString& itemIcon,
                       const QString& objectName)
        : Part(&Kexi::partManager(), new StaticInfo(partClass, itemIcon, objectName))
{
    Kexi::partManager().insertStaticPart(this);
}

StaticPart::~StaticPart()
{
}

KexiView* StaticPart::createView(QWidget *parent, KexiWindow* window,
                                 KexiPart::Item &item, Kexi::ViewMode viewMode)
{
    Q_UNUSED(parent);
    Q_UNUSED(window);
    Q_UNUSED(item);
    Q_UNUSED(viewMode);
    //unused
    return 0;
}
