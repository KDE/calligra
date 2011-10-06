/* This file is part of the KDE project
   Copyright (C) 2010-2011 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPROJECTMODEL_H
#define KEXIPROJECTMODEL_H

#include <QModelIndex>
#include <kexipart.h>
#include <QAbstractItemModel>

class KexiProjectModelItem;
class KexiProject;

class KEXIEXTWIDGETS_EXPORT KexiProjectModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    KexiProjectModel(QObject* parent = 0);
    virtual ~KexiProjectModel();
    
    void setProject(KexiProject* prj, const QString& itemsPartClass, QString* partManagerErrorMessages);
    QString itemsPartClass() const;
    
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    void updateItemName(KexiPart::Item& item, bool dirty);
    QModelIndex indexFromItem(KexiProjectModelItem *item) const;
    KexiProjectModelItem *modelItemFromItem(const KexiPart::Item &item) const;
    KexiProjectModelItem *modelItemFromName(const QString &name) const;
    
    void clear();

    //! @return index of first part item (looking from the top) or invalid item
    //! if there are no part items
    QModelIndex firstPartItem() const;

public slots:
    void slotAddItem(KexiPart::Item& item);
    void slotRemoveItem(const KexiPart::Item &item);

signals:
    void renameItem(KexiPart::Item *item, const QString& _newName, bool &succes);

private:
    KexiProjectModelItem* addGroup(KexiPart::Info& info, KexiProjectModelItem*) const;
    KexiProjectModelItem* addItem(KexiPart::Item& item, KexiPart::Info& info,
                                  KexiProjectModelItem*) const;

    //! @return index of first part item within children of parentIndex (recursively)
    QModelIndex firstChildPartItem(const QModelIndex &parentIndex) const;

    class Private;  
    Private * const d;
};

#endif // KEXIPROJECTMODEL_H
