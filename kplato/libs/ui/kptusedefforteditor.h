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


namespace KPlato
{

class Completion;
class Resource;
class Project;

class UsedEffortItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    UsedEffortItemModel( QWidget *parent );
    
    void setProject( Project *project ) { m_project = project; }
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    virtual bool setData( const QModelIndex &index, const QVariant & value, int role = Qt::EditRole );
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex &) const { return QModelIndex(); }
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    void setCompletion( Completion *completion );
    const Resource *resource(const QModelIndex &index ) const;
    Completion::UsedEffort *usedEffort(const QModelIndex &index ) const;
    void setCurrentMonday( const QDate &date );

    QModelIndex addRow();
    
signals:
    void rowInserted( const QModelIndex& );
    void changed();
    
public slots:
    bool submit();
    void revert();
    
private:
    Project *m_project;
    Completion *m_completion;
    QList<QDate> m_dates;
    QStringList m_headers;
    QList<const Resource*> m_resourcelist;
    QMap<QString, const Resource*> m_editlist;
};

class UsedEffortEditor : public QTableView
{
    Q_OBJECT
public:
    UsedEffortEditor( QWidget *parent );
    void setProject( Project *project );
    void setCompletion( Completion *completion );
    void setCurrentMonday( const QDate &date );
    void addResource();
    
signals:
    void changed();
    void resourceAdded();
    
private:
    
};

//--------------------------------------------
class CompletionEntryItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CompletionEntryItemModel( QWidget *parent );
    
    void setTask( Task *t ) { m_node = t; }
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    virtual bool setData( const QModelIndex &index, const QVariant & value, int role = Qt::EditRole );
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex &) const { return QModelIndex(); }
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    void setCompletion( Completion *completion );
    const Resource *resource(const QModelIndex &index ) const;
    Completion::UsedEffort *usedEffort(const QModelIndex &index ) const;
    void setCurrentMonday( const QDate &date );

    QModelIndex addRow();
    
    void setFlags( int col, Qt::ItemFlags flags ) { m_flags[ col ] = flags; }
    
signals:
    void rowInserted( const QDate& );
    void changed();
    
public slots:
    bool submit();
    void revert();
    void slotDataChanged();
    void setManager( ScheduleManager *sm );
    
protected:
    QVariant date ( int row, int role = Qt::DisplayRole ) const;
    QVariant percentFinished ( int row, int role ) const;
    QVariant remainingEffort ( int row, int role ) const;
    QVariant actualEffort ( int row, int role ) const;
    QVariant plannedEffort ( int row, int role ) const;

    void removeEntry( const QDate date );
    void addEntry( const QDate date );
    void refresh();
    
private:
    Task *m_node;
    ScheduleManager *m_manager;
    Completion *m_completion;
    QList<QDate> m_dates;
    QStringList m_headers;
    QList<QDate> m_datelist;
    Qt::ItemFlags m_flags[5];
};

class CompletionEntryEditor : public QTableView
{
    Q_OBJECT
public:
    CompletionEntryEditor( QWidget *parent );
    void setCompletion( Completion *completion );
    
    CompletionEntryItemModel *model() const { return static_cast<CompletionEntryItemModel*>( QTableView::model() ); }

signals:
    void changed();
    void rowInserted( const QDate );

public slots:
    void addEntry();
    void removeEntry();
    
private:
    
};


}  //KPlato namespace

#endif // TASKUSEDEFFORTEDITOR_H
