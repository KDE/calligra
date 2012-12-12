/*
    This file is part of the KDE project
    Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "KexiFieldListModelItem.h"
#include <db/utils.h>
#include <kexiutils/utils.h>
#include <KoIcon.h>
#include <klocalizedstring.h>

class KexiFieldListModelItem::Private
{
public:
    Private(const QString &fname, const QString &dtype)
      : fieldName(fname)
      , dateType(dtype)
    {
    }

    QString dateType;
    QString fieldName;
    QIcon icon;
    QString caption;
};

KexiFieldListModelItem::KexiFieldListModelItem(const QString &fname,
					       const QString &dtype, bool pkey)
        : d(new Private(fname, dtype))
{
    if (pkey) {
        d->icon = koIcon("key");
    } else {
        d->icon = KexiUtils::emptyIcon(KIconLoader::Small);
    }
}

KexiFieldListModelItem::~KexiFieldListModelItem()
{
    delete d;
}

QVariant KexiFieldListModelItem::data(int column) const
{
    if (column == 0) {
        return d->fieldName == "*" ? i18n("* (All Columns)") : d->fieldName;
    } else {
        return d->dateType;
    }
}

QIcon KexiFieldListModelItem::icon() const
{
    return d->icon;
}

QString KexiFieldListModelItem::caption() const
{
    return d->caption;
}

void KexiFieldListModelItem::setCaption(const QString& caption)
{
    d->caption = caption;
}

Qt::ItemFlags KexiFieldListModelItem::flags() const
{
    if (d->fieldName.isEmpty() || d->fieldName == "*"){
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsDragEnabled;
}
