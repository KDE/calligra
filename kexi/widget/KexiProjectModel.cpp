/*
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

KexiProjectModel::KexiProjectModel(QObject* parent): QAbstractItemModel(parent)
{
    kDebug();
    m_rootItem = new KexiProjectModelItem(QString());
}

void KexiProjectModel::setProject(KexiProject* prj, const QString& itemsPartClass, QString* partManagerErrorMessages)
{
    kDebug() << itemsPartClass << ".";
    clear();
    m_itemsPartClass = itemsPartClass;

    m_rootItem = new KexiProjectModelItem(prj->data()->databaseName());
    
    KexiPart::PartInfoList* plist = Kexi::partManager().partInfoList();
    
    foreach(KexiPart::Info *info, *plist) {
        if (!info->isVisibleInNavigator())
            continue;
        
        if (!m_itemsPartClass.isEmpty() && info->partClass() != m_itemsPartClass)
            continue;


        //load part - we need this to have GUI merged with part's actions
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
        kDebug() << info->partClass() << info->objectName();
        KexiPart::Part *p = Kexi::partManager().part(info);
        if (p) {
            KexiProjectModelItem *groupItem = 0;
            if (m_itemsPartClass.isEmpty()) {
                groupItem = addGroup(*info, m_rootItem);
                if (!groupItem) {
                    continue;
                } else {
                    m_rootItem->appendChild(groupItem);
                }
                
            }
            
            //lookup project's objects (part items)
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
            KexiPart::ItemDict *item_dict = prj->items(info);
            if (!item_dict) {
                continue;
            }
            
            foreach(KexiPart::Item *item, *item_dict) {
                KexiProjectModelItem *itm = addItem(*item, *info, groupItem);
                if (itm) {
                    groupItem->appendChild(itm);
                }
            }

            if (!m_itemsPartClass.isEmpty()) {
                break; //the only group added, so our work is completed
            }
            groupItem->sortChildren();
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
        }
    }
    if (partManagerErrorMessages && !partManagerErrorMessages->isEmpty())
        partManagerErrorMessages->append("</ul></p>");

    m_rootItem->debugPrint();
}

KexiProjectModel::~KexiProjectModel()
{
    if (m_rootItem) {
        delete m_rootItem;
    }
}

QVariant KexiProjectModel::data(const QModelIndex& index, int role) const
{
    KexiProjectModelItem *item = static_cast<KexiProjectModelItem*>(index.internalPointer());
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
         return m_rootItem->columnCount();
}

int KexiProjectModel::rowCount(const QModelIndex& parent) const
{
     KexiProjectModelItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = m_rootItem;
     else
         parentItem = static_cast<KexiProjectModelItem*>(parent.internalPointer());

     return parentItem->childCount();
}

QModelIndex KexiProjectModel::parent(const QModelIndex& index) const
{
     if (!index.isValid())
         return QModelIndex();

     KexiProjectModelItem *childItem = static_cast<KexiProjectModelItem*>(index.internalPointer());
     KexiProjectModelItem *parentItem = childItem->parent();

     if (!parentItem)
         return QModelIndex();
     
     if (parentItem == m_rootItem)
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
        parentItem = m_rootItem;
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

}

QString KexiProjectModel::itemsPartClass() const
{
    return m_itemsPartClass;
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
    kDebug() << item.name();
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
    return new KexiProjectModelItem(info, item, p);;
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
    kDebug();
    if (item && item->parent()) {
        int row = item->row();
        kDebug() << row;
        return createIndex(row, 0, (void*)item);
    } 
    return QModelIndex();
}

KexiProjectModelItem* KexiProjectModel::modelItemFromItem(const KexiPart::Item& item) const
{
    return m_rootItem->modelItemFromItem(item);
}

KexiProjectModelItem* KexiProjectModel::modelItemFromName(const QString& name) const
{
    kDebug() << name;
    return m_rootItem->modelItemFromName(name);
}

void KexiProjectModel::updateItemName(KexiPart::Item& item, bool dirty)
{
    kDebug();
    KexiProjectModelItem *bitem = modelItemFromItem(item);
    if (!bitem)
        return;

    QModelIndex idx = indexFromItem(bitem);
    bitem->setDirty(dirty);
    emit dataChanged(idx, idx);
}


