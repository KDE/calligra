/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

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


#ifndef KEXIFIELDLISTMODELITEM_H
#define KEXIFIELDLISTMODELITEM_H

#include <kexi_export.h>
#include <QString>
#include <QPixmap>

class KexiFieldListModelItem
{

public:
    KexiFieldListModelItem(const QString &fname, const QString &dtype, bool pkey);
    virtual ~KexiFieldListModelItem();
    QVariant data(int column);
    QPixmap icon();
    QString caption();
    void setCaption(const QString &caption);
    Qt::ItemFlags flags();
    
private:
    QString m_fieldName;
    QString m_dateType;
    QPixmap m_icon;
    QString m_caption;
};

#endif // KEXIFIELDLISTMODELITEM_H
