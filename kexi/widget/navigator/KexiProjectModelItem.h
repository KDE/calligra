/* This file is part of the KDE project
   Copyright (C) 2002-2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPROJECTMODELITEM_H
#define KEXIPROJECTMODELITEM_H

#include <QVariant>
#include <QString>
#include <QIcon>
#include <core/kexipartitem.h>
#include <kexi.h>

namespace KexiPart
{
class Info;
}

//! @short List view item for the navigator widget (KexiBrowser)
//! Used for creating group items as well as object items
class KEXIEXTWIDGETS_EXPORT KexiProjectModelItem
{
public:

    //! Create a generic group item
    KexiProjectModelItem(const QString& n, KexiProjectModelItem *parent = 0);

    //! Creates group item for part \a i
    KexiProjectModelItem(KexiPart::Info &i, KexiProjectModelItem *parent = 0);

    //! Creates item for object \a item defined by part \a i for \a parent
    KexiProjectModelItem(KexiPart::Info &i, KexiPart::Item &item, KexiProjectModelItem *parent = 0);

    //! Creates item for object \a item defined by part \a i, without parent
    //! (used in a case when KexiBrowser::itemsPartClass() is not empty)
    //KexiProjectModelItem(KexiPart::Info *i, KexiPart::Item *item);

    virtual ~KexiProjectModelItem();

    void clearChildren();

    //! \return part info; should not be null.
    KexiPart::Info *partInfo() const;

    //! \return part item. Can be null if the browser item is a "folder/group", i.e. a parent node.
    KexiPart::Item* partItem() const;

    KexiProjectModelItem *child(int row);
    void appendChild(KexiProjectModelItem*);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row();
    KexiProjectModelItem *parent();
    void removeChild(const KexiPart::Item &item);

    QIcon icon() const;

    //! @return the flags for the item, ie, add Editable for items, but not groups
    Qt::ItemFlags flags() const;

    void debugPrint() const;

    KexiProjectModelItem* modelItemFromItem(const KexiPart::Item &item) const;
    KexiProjectModelItem* modelItemFromName(const QString& name) const;

    void sortChildren();
    void setDirty(bool set);

private:
    class Private;
    Private * const d;
};

bool itemLessThan(const KexiProjectModelItem *a, const KexiProjectModelItem *b);

#endif // KEXIPROJECTMODELITEM_H
