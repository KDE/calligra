/* This file is part of the KDE project
   Copyright (C) 2009, 2011, 2012 Dag Andersen <danders@get2net.dk>

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
   Boston, MA 02110-1301, USA.
*/

#include "taskcompletiondialog.h"
#include "workpackage.h"

#include "kptusedefforteditor.h"
#include "kptcommand.h"
#include "kptitemmodelbase.h"

#include "kptaccountsmodel.h" // FIXME hack to get at i18n'ed header text

#include <KoIcon.h>

#include <QComboBox>

#include <kdebug.h>
#include <klocale.h>

#include "debugarea.h"

using namespace KPlato;

namespace KPlatoWork
{

TaskCompletionDialog::TaskCompletionDialog(WorkPackage &p, ScheduleManager *sm, QWidget *parent)
    : KDialog(parent)
{
    setCaption( i18n("Task Progress") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new TaskCompletionPanel( p, sm, this);

    setMainWidget(m_panel);

    enableButtonOk(false);

    connect(m_panel, SIGNAL( changed( bool ) ), SLOT(slotChanged( bool )));
}

void TaskCompletionDialog::slotChanged( bool )
{
    enableButtonOk( true );
}

KUndo2Command *TaskCompletionDialog::buildCommand()
{
    //kDebug(planworkDbg());
    return m_panel->buildCommand();
}


TaskCompletionPanel::TaskCompletionPanel(WorkPackage &p, ScheduleManager *sm, QWidget *parent)
    : QWidget(parent),
      m_package( &p )
{
    //kDebug(planworkDbg());
    setupUi(this);

    addEntryBtn->setIcon(koIcon("list-add"));
    removeEntryBtn->setIcon(koIcon("list-remove"));

    CompletionEntryItemModel *m = new CompletionEntryItemModel( this );
    entryTable->setItemDelegateForColumn ( 1, new ProgressBarDelegate( this ) );
    entryTable->setItemDelegateForColumn ( 2, new DurationSpinBoxDelegate( this ) );
    entryTable->setItemDelegateForColumn ( 3, new DurationSpinBoxDelegate( this ) );
    entryTable->setCompletionModel( m );

    Task *task = qobject_cast<Task*>( p.node() );
    m_completion = task->completion();
    started->setChecked(m_completion.isStarted());
    finished->setChecked(m_completion.isFinished());
    startTime->setDateTime(m_completion.startTime());
    finishTime->setDateTime(m_completion.finishTime());
    finishTime->setMinimumDateTime( qMax( startTime->dateTime(), QDateTime(m_completion.entryDate(), QTime() ) ) );
    
    scheduledEffort = p.node()->estimate()->expectedValue();
    
    if ( m_completion.usedEffortMap().isEmpty() || task->requests().isEmpty() ) {
        foreach ( ResourceGroupRequest *g, task->requests().requests() ) {
            foreach ( ResourceRequest *r, g->resourceRequests() ) {
                m_completion.addUsedEffort( r->resource() );
            }
        }
    }

    enableWidgets();
    started->setFocus();
    
    m->setManager( sm );
    m->setTask( task );
    Resource *r = p.project()->findResource( task->workPackage().ownerId() );
    m->setSource( r, task );

    entryTable->horizontalHeader()->swapSections( CompletionEntryItemModel::Property_PlannedEffort, CompletionEntryItemModel::Property_ActualAccumulated );

    //FIXME when string freeze is lifted
    Duration pr = task->plannedEffort( r );
    Duration tr = task->plannedEffort();
    if ( pr == tr ) {
        ui_plannedFrame->hide();
    } else {
        ui_plannedLabel->setText( m->headerData( CompletionEntryItemModel::Property_PlannedEffort, Qt::Horizontal ).toString() );
        ui_labelResource->setText( r->name() );
        ui_plannedResource->setText( pr.format() );

        ui_labelTask->setText( Node::typeToString( Node::Type_Task, true ) );
        ui_plannedTask->setText( tr.format() );
    }

    if ( m->rowCount() > 0 ) {
        QModelIndex idx = m->index( m->rowCount() -1, 0 );
        entryTable->scrollTo( idx );
    }

    connect( addEntryBtn, SIGNAL( clicked() ), this, SLOT( slotAddEntry() ) );
    connect( removeEntryBtn, SIGNAL( clicked() ), entryTable, SLOT( removeEntry() ) );

    connect( entryTable, SIGNAL( rowInserted( const QDate ) ), SLOT( slotEntryAdded( const QDate ) ) );
    connect(entryTable, SIGNAL(changed() ), SLOT( slotChanged() ) );
    connect(entryTable, SIGNAL(changed() ), SLOT( slotEntryChanged() ) );
    connect(entryTable, SIGNAL(rowInserted( const QDate ) ), SLOT( slotChanged() ) );
    connect(entryTable, SIGNAL(rowInserted( const QDate ) ), SLOT( slotEntryChanged() ) );
    connect(entryTable, SIGNAL(rowRemoved( const QDate ) ), SLOT( slotEntryChanged() ) );
    connect(entryTable, SIGNAL(selectionChanged( const QItemSelection&, const QItemSelection& ) ), SLOT( slotSelectionChanged( const QItemSelection& ) ) );
    

    connect(started, SIGNAL(toggled(bool)), SLOT(slotStartedChanged(bool)));
    connect(started, SIGNAL(toggled(bool)), SLOT(slotChanged()));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotFinishedChanged(bool)));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotChanged()));

    connect(startTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotChanged()));
    connect(startTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotStartTimeChanged( const QDateTime& )));
    connect(finishTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotChanged()));
    connect(finishTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotFinishTimeChanged( const QDateTime& )));

    removeEntryBtn->setEnabled( false );
}

QSize TaskCompletionPanel::sizeHint() const
{
    return QWidget::sizeHint().expandedTo( QSize( 610, 0 ) );
}

KUndo2Command *TaskCompletionPanel::buildCommand()
{
    MacroCommand *cmd = new MacroCommand( i18nc("(qtundo-format)", "Modify task completion") );
    Completion &org = m_package->task()->completion();
    if ( org.entrymode() != m_completion.entrymode() ) {
        cmd->addCommand( new ModifyCompletionEntrymodeCmd(org, m_completion.entrymode() ) );
    }
    if ( org.isStarted() != m_completion.isStarted() ) {
        cmd->addCommand( new ModifyCompletionStartedCmd(org, m_completion.isStarted() ) );
    }
    if ( org.isFinished() != m_completion.isFinished() ) {
        cmd->addCommand( new ModifyCompletionFinishedCmd(org, m_completion.isFinished() ) );
    }
    if ( org.startTime() != m_completion.startTime() ) {
        cmd->addCommand( new ModifyCompletionStartTimeCmd(org, m_completion.startTime() ) );
    }
    if ( org.finishTime() != m_completion.finishTime() ) {
        cmd->addCommand( new ModifyCompletionFinishTimeCmd(org, m_completion.finishTime() ) );
    }
    QList<QDate> orgdates = org.entries().keys();
    QList<QDate> m_completiondates = m_completion.entries().keys();
    foreach ( const QDate &d, orgdates ) {
        if ( m_completiondates.contains( d ) ) {
            if ( m_completion.entry( d ) == org.entry( d ) ) {
                continue;
            }
            Completion::Entry *e = new Completion::Entry( *( m_completion.entry( d ) ) );
            cmd->addCommand( new ModifyCompletionEntryCmd(org, d, e ) );
        } else {
            cmd->addCommand( new RemoveCompletionEntryCmd(org, d ) );
        }
    }
    foreach ( const QDate &d, m_completiondates ) {
        if ( ! orgdates.contains( d ) ) {
            Completion::Entry *e = new Completion::Entry( * ( m_completion.entry( d ) ) );
            cmd->addCommand( new AddCompletionEntryCmd(org, d, e ) );
        }
    }

    if ( cmd->isEmpty() ) {
        delete cmd;
        return 0;
    }
    return cmd;
}

void TaskCompletionPanel::slotChanged()
{
    emit changed( true ); //FIXME
}

void TaskCompletionPanel::slotStartedChanged(bool state) {
    m_completion.setStarted( state );
    if (state) {
        m_completion.setStartTime( KDateTime::currentLocalDateTime() );
        startTime->setDateTime( m_completion.startTime() );
        slotCalculateEffort();
    }
    enableWidgets();
}

void TaskCompletionPanel::setFinished() {
    finishTime->setDateTime( QDateTime::currentDateTime() );
    slotFinishTimeChanged( finishTime->dateTime() );
}

void TaskCompletionPanel::slotFinishedChanged(bool state) {
    kDebug(planworkDbg())<<state;
    m_completion.setFinished( state );
    if (state) {
        kDebug(planworkDbg())<<state;
        setFinished();
        Completion::Entry *e = m_completion.entry( m_completion.finishTime().date() );
        if ( e == 0 ) {
            kDebug(planworkDbg())<<"no entry on this date, just add one:"<<m_completion.finishTime().date();
            e = new Completion::Entry( 100, Duration::zeroDuration, m_package->node()->plannedEffort() );
            m_completion.addEntry( m_completion.finishTime().date(), e );
            entryTable->setCompletion( &m_completion );
            kDebug(planworkDbg())<<"Entry added:"<<m_completion.finishTime().date()<<m_completion.entry( m_completion.finishTime().date() );
        } else {
            // row exists, use model to update to respect calculation mode
            int row = entryTable->model()->rowCount() - 1;
            QModelIndex idx = entryTable->model()->index( row, CompletionEntryItemModel::Property_Completion );
            entryTable->model()->setData( idx, 100 );
        }
    }   
    enableWidgets();
}

void TaskCompletionPanel::slotFinishTimeChanged( const QDateTime &dt )
{
    m_completion.setFinishTime( KDateTime( dt, KDateTime::Spec(KDateTime::LocalZone) ) );
}

void TaskCompletionPanel::slotStartTimeChanged( const QDateTime &dt )
{
    m_completion.setStartTime( KDateTime( dt, KDateTime::Spec(KDateTime::LocalZone) ) );
    finishTime->setMinimumDateTime( qMax( startTime->dateTime(), QDateTime(m_completion.entryDate(), QTime() ) ) );
    
}

void TaskCompletionPanel::slotAddEntry()
{
    CompletionEntryItemModel *m = static_cast<CompletionEntryItemModel*>( entryTable->model() );
    int col = KPlato::CompletionEntryItemModel::Property_UsedEffort;

    entryTable->addEntry();

    m_completion.setEntrymode( Completion::EnterEffortPerTask );
    m->setFlags( col, Qt::ItemIsEditable );
}

void TaskCompletionPanel::slotEntryChanged()
{
    finishTime->setMinimumDateTime( qMax( startTime->dateTime(), QDateTime(m_completion.entryDate(), QTime() ) ) );
    if ( ! finished->isChecked() && ! m_completion.isFinished() && m_completion.percentFinished() == 100 ) {
        finished->setChecked( true );
    }
}

void TaskCompletionPanel::enableWidgets() {
    started->setEnabled(!finished->isChecked());
    finished->setEnabled(started->isChecked());
    finishTime->setEnabled(finished->isChecked());
    startTime->setEnabled(started->isChecked() && !finished->isChecked());
}


void TaskCompletionPanel::slotPercentFinishedChanged( int ) {
    slotCalculateEffort();
}

void TaskCompletionPanel::slotCalculateEffort()
{
}

void TaskCompletionPanel::slotEntryAdded( const QDate date )
{
    kDebug(planworkDbg())<<date;
}

void TaskCompletionPanel::slotSelectionChanged( const QItemSelection &sel )
{
    removeEntryBtn->setEnabled( !sel.isEmpty() );
}

void TaskCompletionPanel::slotEditmodeChanged( int index )
{
}

//-------------------
CompletionEntryItemModel::CompletionEntryItemModel( QObject *parent )
    : KPlato::CompletionEntryItemModel( parent ),
    m_calculate( false ),
    m_resource( 0 ),
    m_task( 0 )
{
    // FIXME after string freeze is lifted
    CostBreakdownItemModel m;
    m_headers << m.headerData( 2, Qt::Horizontal ).toString();
}

void CompletionEntryItemModel::setSource( Resource *resource, Task *task )
{
    m_resource = resource;
    m_task = task;
    setCompletion( &(task->completion()) );
}

int CompletionEntryItemModel::columnCount( const QModelIndex& ) const
{
    return 6;
}

QVariant CompletionEntryItemModel::actualEffort ( int row, int role ) const
{
    Completion::Entry *e = m_completion->entry( date( row ).toDate() );
    if ( e == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Duration v = e->totalPerformed;
            if ( row > 0 ) {
                v -= m_completion->entry( date( row - 1 ).toDate() )->totalPerformed;
            }
            //kDebug(planworkDbg())<<m_node->name()<<": "<<v<<" "<<unit<<" : "<<scales<<endl;
            return v.format();
        }
        case Qt::EditRole: {
            Duration v = e->totalPerformed;
            if ( row > 0 ) {
                v -= m_completion->entry( date( row - 1 ).toDate() )->totalPerformed;
            }
            //kDebug(planworkDbg())<<m_node->name()<<": "<<v<<" "<<unit<<" : "<<scales<<endl;
            return v.toDouble( Duration::Unit_h );
        }
        case Role::DurationScales: {
            QVariantList lst;
            lst << 24 << 60 << 60 << 1000;
            return lst;
        }
        case Role::DurationUnit:
            return static_cast<int>( Duration::Unit_h );
        case Role::Minimum:
            return static_cast<int>( Duration::Unit_h );
        case Role::Maximum:
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        default:
            break;
    }
    return QVariant();
}

QVariant CompletionEntryItemModel::data( const QModelIndex &idx, int role ) const
{
    if ( idx.column() == Property_PlannedEffort && m_resource ) {
        switch ( role ) {
            case Qt::DisplayRole: {
                    Duration v = m_task->plannedEffortTo( m_resource, date( idx.row() ).toDate() );
                    return v.format();
                }
            default:
                return QVariant();
        }
    } else if ( idx.column() == Property_ActualAccumulated ) {
        switch ( role ) {
            case Qt::DisplayRole: {
                    Duration v;
                    Completion::Entry *e = m_completion->entry( date( idx.row() ).toDate() );
                    if ( e ) {
                        v = e->totalPerformed;
                    }
                    return v.format();
                }
            default:
                return QVariant();
        }
    }
    return KPlato::CompletionEntryItemModel::data( idx, role );
}

bool CompletionEntryItemModel::setData( const QModelIndex &idx, const QVariant &value, int role )
{
    //kDebug(planworkDbg());
    switch ( role ) {
        case Qt::EditRole: {
            if ( idx.column() == Property_Date ) {
                QDate od = date( idx.row() ).toDate();
                removeEntry( od );
                addEntry( value.toDate() );
                // emit dataChanged( idx, idx );
                m_calculate = true;
                return true;
            }
            if ( idx.column() == Property_Completion ) {
                Completion::Entry *e = m_completion->entry( date( idx.row() ).toDate() );
                if ( e == 0 ) {
                    return false;
                }
                e->percentFinished = value.toInt();
                if ( m_calculate && m_node && idx.row() == rowCount() - 1 ) {
                    // calculate used/remaining
                    Duration est = m_node->plannedEffort( id(), ECCT_EffortWork );
                    e->totalPerformed = est * e->percentFinished / 100;
                    e->remainingEffort = est - e->totalPerformed;
                } else if ( e->percentFinished == 100 && e->remainingEffort != 0 ) {
                    e->remainingEffort = Duration::zeroDuration;
                }
                emit dataChanged( idx, createIndex( idx.row(), 3 ) );
                return true;
            }
            if ( idx.column() == Property_ActualEffort ) {
                Completion::Entry *e = m_completion->entry( date( idx.row() ).toDate() );
                if ( e == 0 ) {
                    return false;
                }
                m_calculate = false;
                Duration prev;
                if ( idx.row() > 0 ) {
                    Completion::Entry *pe = m_completion->entry( date( idx.row() - 1 ).toDate() );
                    if ( pe ) {
                        prev = pe->totalPerformed;
                    }
                }
                double v( value.toList()[0].toDouble() );
                Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
                Duration d = Estimate::scale( v, unit, scales() );
                if ( d + prev == e->totalPerformed ) {
                    return false;
                }
                e->totalPerformed = d + prev;
                emit dataChanged( idx, idx );
                return true;
            }
            if ( idx.column() == Property_RemainigEffort ) {
                Completion::Entry *e = m_completion->entry( date( idx.row() ).toDate() );
                if ( e == 0 ) {
                    return false;
                }
                m_calculate = false;
                double v( value.toList()[0].toDouble() );
                Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
                Duration d = Estimate::scale( v, unit, scales() );
                if ( d == e->remainingEffort ) {
                    return false;
                }
                e->remainingEffort = d;
                kDebug(planworkDbg())<<value<<d.format()<<e->remainingEffort.format();
                emit dataChanged( idx, idx );
                return true;
            }
            break;
        }
        default: break;
    }
    return false;
}

}  //KPlatoWork namespace

#include "taskcompletiondialog.moc"
