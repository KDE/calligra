/* This file is part of the KDE project
   Copyright (C) 2010-2011 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2010-2012 Jarosław Staniek <staniek@kde.org>

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
#include <QAbstractItemModel>

#include <kexipart.h>
#include <KexiSearchableModel.h>

class KexiProjectModelItem;
class KexiProject;

class KEXIEXTWIDGETS_EXPORT KexiProjectModel : public QAbstractItemModel, public KexiSearchableModel
{
    Q_OBJECT
public:
    KexiProjectModel(QObject* parent = 0);
    virtual ~KexiProjectModel();
    
    enum ExtraRoles {
        SearchHighlight = Qt::UserRole + 0 //!< item is highlighted when corresponding global search
                                           //!< completion is highlighted
    };
    
    KexiProject* project() const;
    void setProject(KexiProject* prj, const QString& itemsPartClass, QString* partManagerErrorMessages);
    QString itemsPartClass() const;
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    void updateItemName(KexiPart::Item& item, bool dirty);
    QModelIndex indexFromItem(KexiProjectModelItem *item) const;
    KexiProjectModelItem *modelItemFromItem(const KexiPart::Item &item) const;
    KexiProjectModelItem *modelItemFromName(const QString &name) const;
    
    void clear();

    //! @return index of first part item (looking from the top) or invalid item
    //! if there are no part items
    QModelIndex firstPartItem() const;
    
    //! Implemented for KexiSearchableModel
    virtual int searchableObjectCount() const;

    //! Implemented for KexiSearchableModel
    virtual QModelIndex sourceIndexForSearchableObject(int objectIndex) const;
    
    //! Implemented for KexiSearchableModel
    virtual QVariant searchableData(const QModelIndex &sourceIndex, int role) const;

    //! Implemented for KexiSearchableModel
    virtual QString pathFromIndex(const QModelIndex &sourceIndex) const;

    //! Implemented for KexiSearchableModel
    virtual bool highlightSearchableObject(const QModelIndex &index);

    //! Implemented for KexiSearchableModel
    virtual bool activateSearchableObject(const QModelIndex &index);

    QPersistentModelIndex itemWithSearchHighlight() const;

    bool renameItem(KexiPart::Item *item, const QString& newName);
    bool setItemCaption(KexiPart::Item *item, const QString& newCaption);

public slots:
    void slotAddItem(KexiPart::Item& item);
    void slotRemoveItem(const KexiPart::Item &item);

signals:
    void renameItem(KexiPart::Item *item, const QString& newName, bool &succes);
    void changeItemCaption(KexiPart::Item *item, const QString& newCaption, bool &succes);
    void highlightSearchedItem(const QModelIndex &index);
    void activateSearchedItem(const QModelIndex &index);

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
