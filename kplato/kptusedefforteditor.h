/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef TASKUSEDEFFORTEDITOR_H
#define TASKUSEDEFFORTEDITOR_H

#include <QWidget>
#include <QTableView>

#include "kpttask.h"

class K3Command;

namespace KPlato
{

class Completion;
class Duration;
class Resource;

class UsedEffortItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    UsedEffortItemModel( QWidget *parent );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    virtual bool setData( const QModelIndex &index, const QVariant & value, int role = Qt::EditRole );
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex &child) const { return QModelIndex(); }
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    void setCompletion( Completion *completion );
    const Resource *resource(const QModelIndex &index ) const;
    Completion::UsedEffort *usedEffort(const QModelIndex &index ) const;
    void setCurrentMonday( const QDate &date );

private:
    Completion *m_completion;
    QList<QDate> m_dates;
    QStringList m_headers;
};

class UsedEffortEditor : public QTableView
{
    Q_OBJECT
public:
    UsedEffortEditor( QWidget *parent );
    void setCompletion( Completion *completion );
    void setCurrentMonday( const QDate &date );
    
signals:
    void dataChanged( const QModelIndex&, const QModelIndex& );
    
private:
    
};

}  //KPlato namespace

#endif // TASKUSEDEFFORTEDITOR_H
