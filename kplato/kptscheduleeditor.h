/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <kplato@kde.org>

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

#ifndef KPTSCHEDULEEDITOR_H
#define KPTSCHEDULEEDITOR_H

#include <kptviewbase.h>
#include <kptitemmodelbase.h>

#include <QTreeWidget>

#include "kptcontext.h"

class QPoint;
class QTreeWidgetItem;
class QSplitter;

class KToggleAction;
class KPrinter;

namespace KPlato
{

class View;
class Project;
class ScheduleManager;
class MainSchedule;
class Schedule;

class ScheduleItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    ScheduleItemModel( Part *part, QObject *parent = 0 );
    ~ScheduleItemModel();

    virtual void setProject( Project *project );

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    QItemDelegate *createDelegate( int column, QWidget *parent ) const;
    
    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;

    ScheduleManager *manager( const QModelIndex &index ) const;
    MainSchedule *schedule( const QModelIndex &index ) const;
    
protected slots:
    void slotManagerChanged( ScheduleManager *sch );
    void slotScheduleChanged( MainSchedule *sch );

protected:
    int row( const Schedule *sch ) const;
    
    QVariant name( const QModelIndex &index, int role ) const;
    bool setName( const QModelIndex &index, const QVariant &value, int role );
    
    QVariant state( const QModelIndex &index, int role ) const;
    bool setState( const QModelIndex &index, const QVariant &value, int role );

    QVariant usePert( const QModelIndex &index, int role ) const;
    bool setUsePert( const QModelIndex &index, const QVariant &value, int role );

    QVariant calculateAll( const QModelIndex &index, int role ) const;
    bool setCalculateAll( const QModelIndex &index, const QVariant &value, int role );

};

class ScheduleTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    ScheduleTreeView( Part *part, QWidget *parent );

    ScheduleItemModel *itemModel() const { return static_cast<ScheduleItemModel*>( model() ); }

    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    Project *project() const { return itemModel()->project(); }
    void setProject( Project *project ) { itemModel()->setProject( project ); }

    ScheduleManager *currentManager() const;

    Part *part() const { return m_part; }
    
signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

    void contextMenuRequested( QModelIndex, const QPoint& );
    
protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    
protected:

private:
    bool m_arrowKeyNavigation;
    Part *m_part;
};

class ScheduleEditor : public ViewBase
{
    Q_OBJECT
public:
    ScheduleEditor( Part *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual void updateReadWrite( bool /*readwrite*/ ) {};

signals:
    void requestPopupMenu( const QString&, const QPoint& );
    void calculateSchedule( Project*, ScheduleManager* );
    void addScheduleManager( Project* );
    void deleteScheduleManager( Project*, ScheduleManager* );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( const ScheduleManager *sm );

    void slotCalculateSchedule();
    void slotAddSchedule();
    void slotDeleteSelection();
    
private:
    ScheduleTreeView *m_editor;

    KAction *actionCalculateSchedule;
    KAction *actionAddSchedule;
    KAction *actionDeleteSelection;
};

}  //KPlato namespace

#endif
