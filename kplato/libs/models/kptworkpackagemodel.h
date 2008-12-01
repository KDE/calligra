/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef WORKPACKAGEMODEL_H
#define WORKPACKAGEMODEL_H

#include "kptitemmodelbase.h"

class KoDocument;

class QMimeData;
class QModelIndex;

/// The main namespace
namespace KPlato
{

class Project;
class Task;
class Resource;
class WorkPackage;

/**
 * The WorkPackageModel class gives access to workpackage status
 * for the resources assigned to a task.
 */
class KPLATOMODELS_EXPORT WorkPackageModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit WorkPackageModel( QObject *parent = 0 )
        : ItemModelBase( parent ),
        m_project( 0 ),
        m_task( 0 )
     {}
    ~WorkPackageModel() {}
    
    void setProject( Project *project );
    Project *project() const { return m_project; }
    void setTask( Task *task );
    Task *task() const { return m_task; }
    
    virtual QModelIndex parent( const QModelIndex &index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const; 
    
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex &index = QModelIndex() ) const;
    
protected:
    WorkPackage &workPackage() const;
    Resource *resourceForIndex( const QModelIndex &index ) const;
    
    QVariant name( const Resource *r, int role ) const;
    QVariant email( const Resource *r, int role ) const;
    QVariant sendStatus( const Resource *r, int role ) const;
    QVariant sendTime( const Resource *r, int role ) const;
    QVariant responseType( const Resource *r, int role ) const;
    QVariant requiredTime( const Resource *r, int role ) const;
    QVariant responseStatus( const Resource *r, int role ) const;
    QVariant responseTime( const Resource *r, int role ) const;
    QVariant lastAction( const Resource *r, int role ) const;
    
private:
    Project *m_project;
    Task *m_task;
};


} //namespace KPlato

#endif //WORKPACKAGEMODEL_H
