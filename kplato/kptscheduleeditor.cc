/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen kplato@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "kptscheduleeditor.h"

#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptfactory.h"
#include "kptresourceappointmentsview.h"
#include "kptview.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptschedule.h"
#include "kptdatetime.h"
#include "kptcontext.h"

#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>


#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprinter.h>
#include <kxmlguifactory.h>
#include <kactioncollection.h>

#include <kdebug.h>

namespace KPlato
{

ScheduleItemModel::ScheduleItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent ),
    m_manager( 0 )
{
}

ScheduleItemModel::~ScheduleItemModel()
{
}

void ScheduleItemModel::slotScheduleManagerToBeInserted( const ScheduleManager *manager, int row )
{
    //kDebug()<<k_funcinfo<<manager->name()<<endl;
    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(manager);
    beginInsertRows( QModelIndex(), row, row );
}

void ScheduleItemModel::slotScheduleManagerInserted( const ScheduleManager *manager )
{
    //kDebug()<<k_funcinfo<<manager->name()<<endl;
    Q_ASSERT( manager == m_manager );
    endInsertRows();
    m_manager = 0;
}

void ScheduleItemModel::slotScheduleManagerToBeRemoved( const ScheduleManager *manager )
{
    //kDebug()<<k_funcinfo<<manager->name()<<endl;
    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(manager);
    int row = index( manager ).row();
    beginRemoveRows( QModelIndex(), row, row );
}

void ScheduleItemModel::slotScheduleManagerRemoved( const ScheduleManager *manager )
{
    //kDebug()<<k_funcinfo<<manager->name()<<endl;
    Q_ASSERT( manager == m_manager );
    endRemoveRows();
    m_manager = 0;
}

void ScheduleItemModel::slotScheduleToBeInserted( const ScheduleManager *manager, int row )
{
    //kDebug()<<k_funcinfo<<manager->name()<<" row="<<row<<endl;
    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(manager);
    beginInsertRows( index( manager ), row, row );
}

void ScheduleItemModel::slotScheduleInserted( const MainSchedule *schedule )
{
    //kDebug()<<k_funcinfo<<schedule<<"<--"<<schedule->manager()<<endl;
    Q_ASSERT( schedule->manager() == m_manager );
    endInsertRows();
    m_manager = 0;
}

void ScheduleItemModel::slotScheduleToBeRemoved( const MainSchedule *schedule )
{
    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(schedule->manager());
    int row = index( schedule ).row();
    //kDebug()<<k_funcinfo<<schedule->name()<<", "<<row<<" man="<<index( schedule->manager() ).row()<<endl;
    beginRemoveRows( index( schedule->manager() ), row, row );
}

void ScheduleItemModel::slotScheduleRemoved( const MainSchedule *schedule )
{
    //kDebug()<<k_funcinfo<<schedule->name()<<endl;
    Q_ASSERT( schedule->manager() == m_manager );
    endRemoveRows();
    m_manager = 0;
}

void ScheduleItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotManagerChanged( ScheduleManager* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleManagerToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleManagerToBeInserted( const ScheduleManager*, int) ) );
        
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleManagerAdded( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerInserted( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleManagerRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerRemoved( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleChanged( MainSchedule* ) ), this, SLOT( slotScheduleChanged( MainSchedule* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleToBeInserted( const ScheduleManager*, int ) ) );
        
        disconnect( m_project, SIGNAL( scheduleToBeRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleToBeRemoved( const MainSchedule* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleAdded( const MainSchedule* ) ), this, SLOT( slotScheduleInserted( const MainSchedule* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotManagerChanged( ScheduleManager* ) ) );
        
        connect( m_project, SIGNAL( scheduleManagerToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleManagerToBeInserted( const ScheduleManager*, int) ) );
        
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleManagerAdded( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerInserted( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleManagerRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerInserted( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleChanged( MainSchedule* ) ), this, SLOT( slotScheduleChanged( MainSchedule* ) ) );
        
        connect( m_project, SIGNAL( scheduleToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleToBeInserted( const ScheduleManager*, int ) ) );
        
        connect( m_project, SIGNAL( scheduleToBeRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleToBeRemoved( const MainSchedule* ) ) );
    
        connect( m_project, SIGNAL( scheduleAdded( const MainSchedule* ) ), this, SLOT( slotScheduleInserted( const MainSchedule* ) ) );
        
        connect( m_project, SIGNAL( scheduleRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    }
}

void ScheduleItemModel::slotManagerChanged( ScheduleManager *sch )
{
    int r = m_project->indexOf( sch );
    //kDebug()<<k_funcinfo<<sch<<": "<<r<<endl;
    emit dataChanged( createIndex( r, 0, sch ), createIndex( r, columnCount(), sch ) );
}


void ScheduleItemModel::slotScheduleChanged( MainSchedule *sch )
{
    if ( sch == 0 ) {
        return;
    }
    int r = sch->manager()->indexOf( sch );
    //kDebug()<<k_funcinfo<<sch<<": "<<r<<endl;
    emit dataChanged( createIndex( r, 0, sch ), createIndex( r, columnCount(), sch ) );
}


Qt::ItemFlags ScheduleItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() )
        return flags;
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    flags &= ~Qt::ItemIsEditable;
    ScheduleManager *sm = manager( index );
    if ( sm ) {
        switch ( index.column() ) {
            case 1: break;
            default: flags |= Qt::ItemIsEditable; break;
        }
        return flags;
    }
    flags &= ~Qt::ItemIsSelectable;
    return flags;
}


QModelIndex ScheduleItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<k_funcinfo<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;
    if ( m_project->isScheduleManager( index.internalPointer() ) ) {
        // it's manager, doesn't have a parent
        return QModelIndex();
    }
    MainSchedule *s = schedule( index );
    return createIndex( m_project->indexOf( s->manager() ), index.column(), s->manager() );
}

bool ScheduleItemModel::hasChildren( const QModelIndex &parent ) const
{
    //kDebug()<<k_funcinfo<<parent.internalPointer()<<": "<<parent.row()<<", "<<parent.column()<<endl;
    return rowCount( parent ) > 0;
}

QModelIndex ScheduleItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    //kDebug()<<k_funcinfo<<m_project<<": "<<row<<", "<<column<<endl;
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 || row >= rowCount( parent ) ) {
        //kDebug()<<k_funcinfo<<row<<", "<<column<<" out of bounce"<<endl;
        return QModelIndex();
    }
    if ( parent.isValid() ) {
        return createIndex( row, column, manager( parent )->schedules().value( row ) );
    }
    return createIndex( row, column, m_project->scheduleManagers().value( row ) );
}

QModelIndex ScheduleItemModel::index( const ScheduleManager *manager ) const
{
    if ( m_project == 0 || manager == 0 ) {
        return QModelIndex();
    }
    return createIndex( m_project->indexOf( manager ), 0, const_cast<ScheduleManager*>(manager) );

}

QModelIndex ScheduleItemModel::index( const MainSchedule *sch ) const
{
    if ( m_project == 0 || sch == 0 || sch->manager() == 0) {
        return QModelIndex();
    }
    return createIndex( sch->manager()->indexOf( sch ), 0, const_cast<MainSchedule*>(sch) );
}

int ScheduleItemModel::columnCount( const QModelIndex &parent ) const
{
    return 5;
}

int ScheduleItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( !parent.isValid() ) {
        return m_project->numScheduleManagers();
    }
    if ( manager( parent ) ) {
        return manager( parent )->numSchedules();
    }
    return 0; 
}

QVariant ScheduleItemModel::name( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    MainSchedule *sch = 0;
    if ( sm == 0 ) {
        sch = schedule( index );
        if ( sch == 0 )
            return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return sm ? sm->name() : sch->typeToString();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setName( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand(new ModifyScheduleManagerNameCmd( m_part, *sm, value.toString(), "Modify Schedule Name" ) );
            return true;
    }
    return false;
}

QVariant ScheduleItemModel::state( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    MainSchedule *sch = 0;
    if ( sm == 0 ) {
        sch = schedule( index );
        if ( sch == 0 )
            return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole: 
        {
            QStringList l = sm ? sm->state() : sch->state();
            if ( l.isEmpty() ) {
                return "";
            }
            return l.first();
        }
        case Qt::ToolTipRole:
            return ( sm ? sm->state() : sch->state() ).join(", ");
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setState( const QModelIndex &index, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            return false;
    }
    return false;
}

QVariant ScheduleItemModel::allowOverbooking( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->allowOverbooking();
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return sm->allowOverbooking() ? i18n( "Allow" ) : i18n( "Avoid" );
        case Role::EnumList:
            return QStringList() << i18n( "Avoid" ) << i18n( "Allow" );
        case Role::EnumListValue:
            return sm->allowOverbooking() ? 1 : 0;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setAllowOverbooking( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand(new ModifyScheduleManagerAllowOverbookingCmd( m_part, *sm, value.toBool(), "Modify Schedule Allow Overbooking" ) );
            return true;
    }
    return false;
}


QVariant ScheduleItemModel::usePert( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->usePert();
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return sm->usePert() ? i18n( "PERT" ) : i18n( "None" );
        case Role::EnumList:
            return QStringList() << i18n( "None" ) << i18n( "PERT" );
        case Role::EnumListValue:
            return sm->usePert() ? 1 : 0;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setUsePert( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand(new ModifyScheduleManagerDistributionCmd( m_part, *sm, value.toBool(), "Modify Schedule Distribution" ) );
            emit slotManagerChanged( static_cast<ScheduleManager*>( sm ) );
            return true;
    }
    return false;
}

QVariant ScheduleItemModel::calculateAll( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->calculateAll();
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return sm->calculateAll() ? i18n( "All" ) : i18n( "Expected only" );
        case Role::EnumList:
            return QStringList() << i18n( "Expected only" ) << i18n( "All" );
        case Role::EnumListValue:
            return sm->calculateAll() ? 1 : 0;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setCalculateAll( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand(new ModifyScheduleManagerCalculateAllCmd( m_part, *sm, value.toBool(), "Modify Schedule Calculate" ) );
            return true;
    }
    return false;
}

QVariant ScheduleItemModel::data( const QModelIndex &index, int role ) const
{
    //kDebug()<<k_funcinfo<<index.row()<<", "<<index.column()<<endl;
    QVariant result;
    switch ( index.column() ) {
        case 0: result = name( index, role ); break;
        case 1: result = state( index, role ); break;
        case 2: result = allowOverbooking( index, role ); break;
        case 3: result = usePert( index, role ); break;
        case 4: result = calculateAll( index, role ); break;
        default:
            kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
            return QVariant();
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    return QVariant();
}

bool ScheduleItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( !index.isValid() || ( flags( index ) & Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    switch (index.column()) {
        case 0: return setName( index, value, role );
        case 1: return setState( index, value, role );
        case 2: return setAllowOverbooking( index, value, role );
        case 3: return setUsePert( index, value, role );
        case 4: return setCalculateAll( index, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            break;
    }
    return false;
}

QVariant ScheduleItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "State" );
                case 2: return i18n( "Overbooking" );
                case 3: return i18n( "Distribution" );
                case 4: return i18n( "Calculate" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                default: return Qt::AlignCenter;
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case 0: return ToolTip::ScheduleName;
            case 1: return ToolTip::ScheduleState;
            case 2: return ToolTip::ScheduleOverbooking;
            case 3: return ToolTip::ScheduleDistribution;
            case 4: return ToolTip::ScheduleCalculate;
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QItemDelegate *ScheduleItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case 2: return new EnumDelegate( parent );
        case 3: return new EnumDelegate( parent );
        case 4: return new EnumDelegate( parent );
    }
    return 0;
}

void ScheduleItemModel::sort( int column, Qt::SortOrder order )
{
}

QMimeData * ScheduleItemModel::mimeData( const QModelIndexList &indexes ) const
{
    return 0;
}

QStringList ScheduleItemModel::mimeTypes () const
{
    return QStringList();
}

MainSchedule *ScheduleItemModel::schedule( const QModelIndex &index ) const
{
    if ( !index.isValid() || manager( index ) != 0 ) {
        return 0;
    }
    return static_cast<MainSchedule*>( index.internalPointer() );
}

ScheduleManager *ScheduleItemModel::manager( const QModelIndex &index ) const
{
    ScheduleManager *o = 0;
    if ( index.isValid() && m_project != 0 && index.internalPointer() != 0 && m_project->isScheduleManager( index.internalPointer() ) ) {
        o = static_cast<ScheduleManager*>( index.internalPointer() );
        Q_ASSERT( o );
    }
    return o;
}


//--------------------
ScheduleTreeView::ScheduleTreeView( Part *part, QWidget *parent )
    : TreeViewBase( parent ),
    m_part( part )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new ScheduleItemModel( part ) );
    setSelectionModel( new QItemSelectionModel( model() ) );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    
    for ( int c = 0; c < itemModel()->columnCount(); ++c ) {
        QItemDelegate *delegate = itemModel()->createDelegate( c, this );
        if ( delegate ) {
            setItemDelegateForColumn( c, delegate );
        }
    }

    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );

}

void ScheduleTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<k_funcinfo<<index.column()<<endl;
}

void ScheduleTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<k_funcinfo<<header()->logicalIndexAt(pos)<<" at "<<pos<<endl;
}

void ScheduleTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<k_funcinfo<<sel.indexes().count()<<endl;
    foreach( QModelIndex i, selectionModel()->selectedIndexes() ) {
        //kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl;
    }
    QTreeView::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void ScheduleTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    //kDebug()<<k_funcinfo<<current.row()<<", "<<current.column()<<endl;
    QTreeView::currentChanged( current, previous );
    emit currentChanged( current );
    selectionModel()->select( current, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
}

ScheduleManager *ScheduleTreeView::currentManager() const
{
    return itemModel()->manager( currentIndex() );
}

//-----------------------------------
ScheduleEditor::ScheduleEditor( Part *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_editor = new ScheduleTreeView( part, this );
    l->addWidget( m_editor );
    m_editor->setEditTriggers( m_editor->editTriggers() | QAbstractItemView::EditKeyPressed );

    connect( m_editor, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentChanged( QModelIndex ) ) );

    connect( m_editor, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_editor, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
}

void ScheduleEditor::draw( Project &project )
{
    m_editor->setProject( &project );
}

void ScheduleEditor::draw()
{
}

void ScheduleEditor::setGuiActive( bool activate )
{
    //kDebug()<<k_funcinfo<<activate<<endl;
    ViewBase::setGuiActive( activate );
    if ( activate && !m_editor->currentIndex().isValid() ) {
        m_editor->selectionModel()->setCurrentIndex(m_editor->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void ScheduleEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    kDebug()<<k_funcinfo<<index.row()<<", "<<index.column()<<": "<<pos<<endl;
/*    QString name;
    if ( index.isValid() ) {
        QObject *obj = m_editor->itemModel()->object( index );
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            name = "resourceeditor_group_popup";
        } else {
            Resource *r = qobject_cast<Resource*>( obj );
            if ( r ) {
                name = "resourceeditor_resource_popup";
            }
        }
    }
    kDebug()<<k_funcinfo<<name<<endl;
    emit requestPopupMenu( name, pos );*/
}

void ScheduleEditor::slotCurrentChanged(  const QModelIndex &curr )
{
    //kDebug()<<k_funcinfo<<curr.row()<<", "<<curr.column()<<endl;
}

void ScheduleEditor::slotSelectionChanged( const QModelIndexList list)
{
    //kDebug()<<k_funcinfo<<list.count()<<endl;
    // The list has one entry per column, and we only select one row at a time, so...
    ScheduleManager *sm = 0;
    if ( ! list.isEmpty() ) {
        sm = m_editor->itemModel()->manager( list.first() );
    }
    slotEnableActions( sm );
}

void ScheduleEditor::slotEnableActions( const ScheduleManager *sm )
{
    actionDeleteSelection->setEnabled( sm != 0 );
    actionCalculateSchedule->setEnabled( sm != 0 );
}

void ScheduleEditor::setupGui()
{
    KActionCollection *coll = actionCollection();
    QString name = "scheduleeditor_edit_list";
    
    actionCalculateSchedule  = new KAction(KIcon( "project_calculate" ), i18n("Calculate Schedule..."), this);
    actionCollection()->addAction("calculate_schedule", actionCalculateSchedule );
    connect( actionCalculateSchedule, SIGNAL( triggered( bool ) ), SLOT( slotCalculateSchedule() ) );
    addAction( name, actionCalculateSchedule );
    
    actionAddSchedule  = new KAction(KIcon( "document-new" ), i18n("Add Schedule..."), this);
    actionCollection()->addAction("add_schedule", actionAddSchedule );
    connect( actionAddSchedule, SIGNAL( triggered( bool ) ), SLOT( slotAddSchedule() ) );
    addAction( name, actionAddSchedule );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Schedules"), this);
    actionCollection()->addAction("schedule_delete_selection", actionDeleteSelection );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );

}

void ScheduleEditor::slotCalculateSchedule()
{
    //kDebug()<<k_funcinfo<<endl;
    ScheduleManager *sm = m_editor->currentManager();
    if ( sm ) {
        emit calculateSchedule( m_editor->project(), sm );
	//emit slotUpdate();
    }
}

void ScheduleEditor::slotAddSchedule()
{
    //kDebug()<<k_funcinfo<<endl;
    emit addScheduleManager( m_editor->project() );
}

void ScheduleEditor::slotDeleteSelection()
{
    //kDebug()<<k_funcinfo<<endl;
    ScheduleManager *sm = m_editor->currentManager();
    if ( sm ) {
        emit deleteScheduleManager( m_editor->project(), sm );
    }
}


} // namespace KPlato

#include "kptscheduleeditor.moc"
