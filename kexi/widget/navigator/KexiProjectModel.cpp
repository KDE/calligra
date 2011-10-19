/* This file is part of the KDE project
   Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>
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

#include "KexiProjectModel.h"

#include <kexipart.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>
#include <kexiproject.h>
#include <kexidb/utils.h>

#include <KDebug>

#include "KexiProjectModelItem.h"

class KexiProjectModel::Private {
public:
    Private();
    ~Private();
    
    //!Part class to display
    QString itemsPartClass;
    KexiProjectModelItem *rootItem;
    QPersistentModelIndex searchHighlight;
};

KexiProjectModel::Private::Private() : rootItem(0)
{
    
}

KexiProjectModel::Private::~Private()
{
    delete rootItem;
}


KexiProjectModel::KexiProjectModel(QObject* parent): QAbstractItemModel(parent) , d(new Private())
{
    //kDebug();
    d->rootItem = new KexiProjectModelItem(QString());
}

void KexiProjectModel::setProject(KexiProject* prj, const QString& itemsPartClass, QString* partManagerErrorMessages)
{
    //kDebug() << itemsPartClass << ".";
    clear();
    d->itemsPartClass = itemsPartClass;

    d->rootItem = new KexiProjectModelItem(prj ? prj->data()->databaseName() : QString());
    
    KexiPart::PartInfoList* plist = Kexi::partManager().infoList();
    if (!plist)
        return;
    foreach(KexiPart::Info *info, *plist) {
        if (!info->isVisibleInNavigator())
            continue;
        
        if (!d->itemsPartClass.isEmpty() && info->partClass() != d->itemsPartClass)
            continue;


        //load part - we need this to have GUI merged with part's actions
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
        //kDebug() << info->partClass() << info->objectName();
// no need to load part so early:        KexiPart::Part *p = Kexi::partManager().part(info);
// no need to load part so early:        if (p) {
            KexiProjectModelItem *groupItem = 0;
            if (d->itemsPartClass.isEmpty() /*|| m_itemsPartClass == info->partClass()*/) {
                groupItem = addGroup(*info, d->rootItem);
                if (!groupItem) {
                    continue;
                } else {
                    d->rootItem->appendChild(groupItem);
                }
                
            } else {
                groupItem = d->rootItem;
            }
          
            //lookup project's objects (part items)
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
            KexiPart::ItemDict *item_dict = prj ? prj->items(info) : 0;
            if (!item_dict) {
                continue;
            }
            
            foreach(KexiPart::Item *item, *item_dict) {
                KexiProjectModelItem *itm = addItem(*item, *info, groupItem);
                if (itm && groupItem) {
                    groupItem->appendChild(itm);
                }
            }

            if (!d->itemsPartClass.isEmpty()) {
                break; //the only group added, so our work is completed
            }
            groupItem->sortChildren();
/* no need to load part so early:            
        } else {
            //add this error to the list that will be displayed later
            QString msg, details;
            KexiDB::getHTMLErrorMesage(&Kexi::partManager(), msg, details);
            if (!msg.isEmpty() && partManagerErrorMessages) {
                if (partManagerErrorMessages->isEmpty()) {
                    *partManagerErrorMessages = QString("<qt><p>")
                                                + i18n("Errors encountered during loading plugins:") + "<ul>";
                }
                partManagerErrorMessages->append(QString("<li>") + msg);
                if (!details.isEmpty())
                    partManagerErrorMessages->append(QString("<br>") + details);
                partManagerErrorMessages->append("</li>");
            }
        }*/
    }
    if (partManagerErrorMessages && !partManagerErrorMessages->isEmpty())
        partManagerErrorMessages->append("</ul></p>");

   //d->rootItem->debugPrint();
}

KexiProjectModel::~KexiProjectModel()
{
    delete d;
}

QVariant KexiProjectModel::data(const QModelIndex& index, int role) const
{
    KexiProjectModelItem *item = static_cast<KexiProjectModelItem*>(index.internalPointer());
    if (!item)
        return QVariant();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::WhatsThisRole:
        return item->data(index.column());
    case Qt::DecorationRole:
        return item->icon();
    default:;
    }

    return QVariant();
}

int KexiProjectModel::columnCount(const QModelIndex& parent) const
{
     if (parent.isValid())
         return static_cast<KexiProjectModelItem*>(parent.internalPointer())->columnCount();
     else
         return d->rootItem->columnCount();
}

int KexiProjectModel::rowCount(const QModelIndex& parent) const
{
     KexiProjectModelItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = d->rootItem;
     else
         parentItem = static_cast<KexiProjectModelItem*>(parent.internalPointer());

     if (parentItem)
        return parentItem->childCount();
     else
         return 0;
}

QModelIndex KexiProjectModel::parent(const QModelIndex& index) const
{
     if (!index.isValid())
         return QModelIndex();

     KexiProjectModelItem *childItem = static_cast<KexiProjectModelItem*>(index.internalPointer());
     KexiProjectModelItem *parentItem = childItem->parent();

     if (!parentItem)
         return QModelIndex();
     
     if (parentItem == d->rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
}

QModelIndex KexiProjectModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    KexiProjectModelItem *parentItem;

    if (!parent.isValid()) {
        parentItem = d->rootItem;
    } else {
        parentItem = static_cast<KexiProjectModelItem*>(parent.internalPointer());
    }
    
    KexiProjectModelItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

bool KexiProjectModel::hasChildren(const QModelIndex& parent) const
{
    return QAbstractItemModel::hasChildren(parent);
}

bool KexiProjectModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
//    if (!(m_features & Writable))
//        return;
    if (role == Qt::EditRole) {
        KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(index.internalPointer());
        if (!it)
            return false;
        
        QString txt = value.toString().trimmed();
        bool ok = QString::compare(it->partItem()->name(), txt, Qt::CaseInsensitive); //make sure the new name is different
        if (ok) {
            emit renameItem(it->partItem(), txt, ok);
        }

        if (ok) {
            emit dataChanged(index, index);
        }
        return ok;
    }
    return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags KexiProjectModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
         return QAbstractItemModel::flags(index);

     KexiProjectModelItem *item = static_cast<KexiProjectModelItem*>(index.internalPointer());

     if (item)
         return item->flags();

     return QAbstractItemModel::flags(index);
}


void KexiProjectModel::clear()
{
    beginResetModel();
    delete(d->rootItem);
    d->rootItem = 0;
    endResetModel();
}

QString KexiProjectModel::itemsPartClass() const
{
    return d->itemsPartClass;
}

KexiProjectModelItem *KexiProjectModel::addGroup(KexiPart::Info& info, KexiProjectModelItem *p) const
{
    if (!info.isVisibleInNavigator())
        return 0;

    KexiProjectModelItem *item = new KexiProjectModelItem(info, p);
    return item;
}

void KexiProjectModel::slotAddItem(KexiPart::Item& item)
{
    //kDebug() << item.name();
    QModelIndex idx;
  
    KexiProjectModelItem *parent = modelItemFromName(item.partClass());

    if (parent) {
        kDebug() << "Got Parent" << parent->data(0);
        idx = indexFromItem(parent);
        beginInsertRows(idx, 0,0);
        KexiProjectModelItem *itm = new KexiProjectModelItem(*(parent->partInfo()), item, parent);
        if (itm) {
            kDebug() << "Appending";
            parent->appendChild(itm);
            parent->sortChildren();
        }
        endInsertRows();
    }
    else {
        kDebug() << "Unable to find parent item!";
    }
}

KexiProjectModelItem* KexiProjectModel::addItem(KexiPart::Item &item, KexiPart::Info &info, KexiProjectModelItem *p) const
{
    return new KexiProjectModelItem(info, item, p);
}

void KexiProjectModel::slotRemoveItem(const KexiPart::Item& item)
{
    QModelIndex idx;
    KexiProjectModelItem *mitm = modelItemFromItem(item);
    KexiProjectModelItem *parent =0;
    
    if (mitm) {
        kDebug() << "Got model item from item";
        parent = mitm->parent();
    } else {
        kDebug() << "Unable to get model item from item";
    }
    
    if (parent) {
        idx = indexFromItem(parent);
        beginRemoveRows(idx, 0,0);
        parent->removeChild(item);
        endRemoveRows();
    } else {
        kDebug() << "Unable to find parent item!";
    }
}

QModelIndex KexiProjectModel::indexFromItem(KexiProjectModelItem* item) const
{
    //kDebug();
    if (item /*&& item->parent()*/) {
        int row = item->parent() ? item->row() : 0;
        //kDebug() << row;
        return createIndex(row, 0, (void*)item);
    }
    return QModelIndex();
}

KexiProjectModelItem* KexiProjectModel::modelItemFromItem(const KexiPart::Item& item) const
{
    return d->rootItem->modelItemFromItem(item);
}

KexiProjectModelItem* KexiProjectModel::modelItemFromName(const QString& name) const
{
    //kDebug() << name;
    return d->rootItem->modelItemFromName(name);
}

void KexiProjectModel::updateItemName(KexiPart::Item& item, bool dirty)
{
    //kDebug();
    KexiProjectModelItem *bitem = modelItemFromItem(item);
    if (!bitem)
        return;

    QModelIndex idx = indexFromItem(bitem);
    bitem->setDirty(dirty);
    emit dataChanged(idx, idx);
}

QModelIndex KexiProjectModel::firstChildPartItem(const QModelIndex &parentIndex) const
{
    int count = rowCount(parentIndex);
    kDebug() << "parent:" << data(parentIndex) << parentIndex.isValid() << count;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(parentIndex.internalPointer());
    if (it) {
        if (it->partItem()) {
            return parentIndex;
        }
    }
    for (int i = 0; i < count; i++) {
        QModelIndex index = parentIndex.child(i, 0);
        kDebug() << data(index);
        index = firstChildPartItem(index);
        if (index.isValid()) {
            return index;
        }
    }
    return QModelIndex();
}

QModelIndex KexiProjectModel::firstPartItem() const
{
    return firstChildPartItem(indexFromItem(d->rootItem));
}

// Implemented for KexiSearchableModel:

int KexiProjectModel::searchableObjectCount() const
{
    const QModelIndex rootIndex = indexFromItem(d->rootItem);
    const int topLevelCount = rowCount(rootIndex);
    int result = 0;
    for (int i = 0; i < topLevelCount; i++) {
        QModelIndex index = this->index(i, 0, rootIndex);
        result += rowCount(index);
    }
    return result;
}

QModelIndex KexiProjectModel::sourceIndexForSearchableObject(int objectIndex) const
{
    const QModelIndex rootIndex = indexFromItem(d->rootItem);
    const int topLevelCount = rowCount(rootIndex);
    int j = objectIndex;
    for (int i = 0; i < topLevelCount; i++) {
        QModelIndex index = this->index(i, 0, rootIndex);
        const int childCount = rowCount(index);
        if (j < childCount) {
            return this->index(j, 0, index);
        }
        j -= childCount;
    }
    return QModelIndex();
}

QVariant KexiProjectModel::searchableData(const QModelIndex &sourceIndex, int role) const
{
    return data(sourceIndex, role);
}

QString KexiProjectModel::pathFromIndex(const QModelIndex &sourceIndex) const
{
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(sourceIndex.internalPointer());
    return it->partItem()->name();
}

QPersistentModelIndex KexiProjectModel::itemWithSearchHighlight() const
{
    return d->searchHighlight;
}

bool KexiProjectModel::highlightSearchableObject(const QModelIndex &index)
{
    if (d->searchHighlight.isValid() && index != d->searchHighlight) {
        setData(d->searchHighlight, false, SearchHighlight);
    }
    setData(index, true, SearchHighlight);
    //setData(index, true, SearchHighlight);
    emit highlightSearchedItem(index);
    d->searchHighlight = QPersistentModelIndex(index);
    return true;
}

bool KexiProjectModel::activateSearchableObject(const QModelIndex &index)
{
    if (d->searchHighlight.isValid() && index != d->searchHighlight) {
        setData(d->searchHighlight, false, SearchHighlight);
    }
    emit activateSearchedItem(index);
    return true;
}
