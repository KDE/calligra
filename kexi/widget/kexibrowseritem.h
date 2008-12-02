/* This file is part of the KDE project
   Copyright (C) 2002-2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIBROWSERITEM_H
#define KEXIBROWSERITEM_H

#include <k3listview.h>
#include <qstring.h>

#include <core/kexipartitem.h>

namespace KexiPart
{
class Info;
}

//! @short List view item for the navigator widget (KexiBrowser)
//! Used for creating group items as well as object items
class KEXIEXTWIDGETS_EXPORT KexiBrowserItem : public K3ListViewItem
{
public:
    //! Creates group item for part \a i
    KexiBrowserItem(K3ListView *parent, KexiPart::Info *i);

    //! Creates item for object \a item defined by part \a i for \a parent
    KexiBrowserItem(K3ListViewItem *parent, KexiPart::Info *i, KexiPart::Item *item);

    //! Creates item for object \a item defined by part \a i, without parent
    //! (used in a case when KexiBrowser::itemsPartClass() is not empty)
    KexiBrowserItem(K3ListView *parent, KexiPart::Info *i, KexiPart::Item *item);

    virtual ~KexiBrowserItem();

    void clearChildren();

    //! \return part info; should not be null.
    KexiPart::Info *partInfo() const {
        return m_info;
    }

    //! \return part item. Can be null if the browser item is a "folder/group", i.e. a parent node.
    KexiPart::Item* partItem() const {
        return m_item;
    }

protected:
    void initItem();
    virtual QString key(int column, bool ascending) const;

    KexiPart::Info *m_info;
    KexiPart::Item *m_item;

    QString m_sortKey;
    bool m_fifoSorting : 1;
};

#endif
