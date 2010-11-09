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
#include <kexidb/utils.h>
#include <klocalizedstring.h>

KexiFieldListModelItem::KexiFieldListModelItem(const QString &fname, const QString &dtype, bool pkey) : m_fieldName(fname), m_dateType(dtype)
{
    if (pkey) {
        m_icon = SmallIcon("key");
    } else {
        m_icon = KexiUtils::emptyIcon(KIconLoader::Small);
    }
    
}

KexiFieldListModelItem::~KexiFieldListModelItem()
{

}

QVariant KexiFieldListModelItem::data(int column) const
{
    if (column == 0) {
        return m_fieldName == "*" ? i18n("* (All Columns)") : m_fieldName;
    } else {
        return m_dateType;
    }
}

QPixmap KexiFieldListModelItem::icon() const
{
    return m_icon;
}

QString KexiFieldListModelItem::caption() const
{
    return m_caption;
}

void KexiFieldListModelItem::setCaption(const QString& caption)
{
    m_caption = caption;
}

Qt::ItemFlags KexiFieldListModelItem::flags() const
{
    if (m_fieldName.isEmpty() || m_fieldName == "*"){
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsDragEnabled;
}
