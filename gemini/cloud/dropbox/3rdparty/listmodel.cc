/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QDebug>

#include "listmodel.h"
#include "folderitem.h"
#include "filetransferitem.h"

ListModel::ListModel(ListItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ListItem::ModifiedRole] = "modified";
    roles[ListItem::PathRole] = "path";
    roles[ListItem::Is_dirRole] = "is_dir";
    roles[ListItem::SizeRole] = "size";
    roles[ListItem::Mime_typeRole] = "mime_type";
    roles[FileTransferItem::FileNameRole] = "filename";
    roles[FileTransferItem::Is_downloadRole] = "is_download";
    roles[FileTransferItem::Is_finishedRole] = "is_finished";
    roles[FileTransferItem::ProgressingRole] = "progressing";
    roles[FileTransferItem::CompletedRole] = "completed";
    roles[FileTransferItem::Dropbox_pathRole] = "dropbox_path";
    roles[FileTransferItem::In_queueRole] = "in_queue";
    roles[FileTransferItem::Is_CancelledRolse] = "is_cancelled";
    roles[FileTransferItem::DateRole] = "date";
    return roles;
}

int ListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.count();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

ListModel::~ListModel() {
  delete m_prototype;
  qDeleteAll(m_list);
}

void ListModel::appendRow(ListItem *item)
{
  appendRows(QList<ListItem*>() << item);
}

void ListModel::appendRows(const QList<ListItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(ListItem *item, items) {
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void ListModel::insertRow(int row, ListItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void ListModel::handleItemChange()
{
  ListItem* item = static_cast<ListItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

ListItem * ListModel::find(const QString &id) const
{
  foreach(ListItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex ListModel::indexFromItem(const ListItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void ListModel::clear()
{
  beginResetModel();
  qDeleteAll(m_list);
  m_list.clear();
  //reset();
  endResetModel();
}

bool ListModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  //reset();
  return true;
}

bool ListModel::removeRows(int row, int count, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || (row+count) >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row+count-1);
  for(int i=0; i<count; ++i) {
    delete m_list.takeAt(row);
  }
  endRemoveRows();
  return true;
}

ListItem * ListModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  ListItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

ListItem * ListModel::getRow(int row){
   ListItem* item = m_list.at(row);
   return item;
}

QVariantMap ListModel::get(int row) const
{
   /* ListItem * item = m_list.at(row);
    QMap<QString, QVariant> itemData;
    QHashIterator<int, QByteArray> hashItr(item->roleNames());
    while(hashItr.hasNext()){
        hashItr.next();
        itemData.insert(hashItr.value(),item->data(hashItr.key()).toString());
    }
    return QVariantMap(itemData);*/
    QVariantMap elem;
    if (row < 0 || row > m_list.count())
        return elem;

    FolderItem *item = (FolderItem*)m_list.at(row);
    elem["section"] = item->xsection();
    return elem;
}

int ListModel::count() const {
    return m_list.count();
}
