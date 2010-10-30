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


#ifndef KEXIFIELDLISTMODEL_H
#define KEXIFIELDLISTMODEL_H

#include <QModelIndex>
#include <QPixmap>

namespace KexiDB
{
    class TableOrQuerySchema;
}

class KexiFieldListModelItem;

class KexiFieldListModel : public QAbstractTableModel
{
Q_OBJECT
public:
    
    //! Flags used to alter models behaviour and appearance
    enum Options {
        ShowDataTypes = 1, //!< if set, 'data type' column is added
        ShowAsterisk = 2, //!< if set, asterisk ('*') item is prepended to the list
        AllowMultiSelection = 4, //!< if set, multiple selection is allowed
        ShowEmptyItem = 8 //!< if set, an empty item is prepended to the list
    };
    
    KexiFieldListModel(QObject* parent = 0, int options = ShowDataTypes | AllowMultiSelection);
    virtual ~KexiFieldListModel();
    
    /*! Sets table or query schema \a schema.
    The schema object will be owned by the KexiFieldListView object. */
    void setSchema(KexiDB::TableOrQuerySchema* schema);
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
private:
    KexiDB::TableOrQuerySchema* m_schema;
    int m_options;
    KexiFieldListModelItem *m_allColumnsItem;
    QList<KexiFieldListModelItem*> m_items;
    
};

#endif // KEXIFIELDLISTMODEL_H
