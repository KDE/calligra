/* This file is part of the KDE project
   Copyright (C) 2004 - 2007, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskprogresspanel.h"
#include "kptusedefforteditor.h"

#include <KoIcon.h>

#include <QCheckBox>
#include <QDate>
#include <QTableWidget>

#include <klineedit.h>
#include <ktextedit.h>
#include <kdatetime.h>
#include <kdatetimewidget.h>
#include <knuminput.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "kpttask.h"
#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptresource.h"
#include "kptdurationspinbox.h"
#include "kptschedule.h"
#include "kptproject.h"
#include "kptdebug.h"


namespace KPlato
{


//-----------------
TaskProgressPanel::TaskProgressPanel( Task &task, ScheduleManager *sm, StandardWorktime *workTime, QWidget *parent )
    : TaskProgressPanelImpl( task, parent )
{
    kDebug(planDbg());
    started->setChecked(m_completion.isStarted());
    finished->setChecked(m_completion.isFinished());
    startTime->setDateTime(m_completion.startTime());
    finishTime->setDateTime(m_completion.finishTime());
    finishTime->setMinimumDateTime( qMax( startTime->dateTime(), QDateTime(m_completion.entryDate(), QTime() ) ) );
    
    if (workTime) {
        kDebug(planDbg())<<"daylength="<<workTime->durationDay().hours();
        m_dayLength = workTime->durationDay().hours();
        setEstimateScales(m_dayLength);
    }
    
    scheduledEffort = task.estimate()->expectedValue();
    
    setYear( QDate::currentDate().year() );
    
    if ( m_completion.usedEffortMap().isEmpty() || m_task.requests().isEmpty() ) {
        foreach ( ResourceGroupRequest *g, m_task.requests().requests() ) {
            foreach ( ResourceRequest *r, g->resourceRequests() ) {
                m_completion.addUsedEffort( r->resource() );
            }
        }
    }
    if ( m_completion.isStarted() ) {
        tabWidget->setCurrentWidget( completionTab );
    }
    enableWidgets();
    started->setFocus();
    
    connect( weekNumber, SIGNAL( currentIndexChanged( int ) ), SLOT( slotWeekNumberChanged( int ) ) );
    connect( addResource, SIGNAL( clicked() ), SLOT( slotAddResource() ) );
    connect( addEntryBtn, SIGNAL( clicked() ), entryTable, SLOT( addEntry() ) );
    connect( removeEntryBtn, SIGNAL( clicked() ), entryTable, SLOT( removeEntry() ) );

    entryTable->model()->setManager( sm );
    entryTable->model()->setTask( &task );
    entryTable->setCompletion( &m_completion );
    connect( entryTable, SIGNAL( rowInserted( const QDate ) ), SLOT( slotEntryAdded( const QDate ) ) );
    
    resourceTable->setProject( static_cast<Project*>( task.projectNode() ) );
    resourceTable->setCompletion( &m_completion );
    slotWeekNumberChanged( weekNumber->currentIndex() );
    addResource->setEnabled( resourceTable->hasFreeResources() );

    //resourceTable->resizeColumnsToContents();

    connect(started, SIGNAL(toggled(bool)), SLOT(slotStartedChanged(bool)));
    connect(started, SIGNAL(toggled(bool)), SLOT(slotChanged()));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotFinishedChanged(bool)));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotChanged()));

    connect(startTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotChanged()));
    connect(startTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotStartTimeChanged( const QDateTime& )));
    connect(finishTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotChanged()));
    connect(finishTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotFinishTimeChanged( const QDateTime& )));
}

MacroCommand *TaskProgressPanel::buildCommand()
{
    Project *project = dynamic_cast<Project*>( m_task.projectNode() );
    if ( project == 0 ) {
        return 0;
    }
    return buildCommand( *project, m_original, m_completion );
}

MacroCommand *TaskProgressPanel::buildCommand( const Project &project, Completion &org, Completion &curr )
{
    MacroCommand *cmd = 0;
    QString c = i18n("Modify task completion");
    
    if ( org.entrymode() != curr.entrymode() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionEntrymodeCmd(org, curr.entrymode() ) );
    }
    if ( org.startTime() != curr.startTime() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartTimeCmd(org, curr.startTime() ) );
    }
    if ( org.finishTime() != curr.finishTime() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishTimeCmd(org, curr.finishTime() ) );
    }
    if ( org.isStarted() != curr.isStarted() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartedCmd(org, curr.isStarted() ) );
    }
    if ( org.isFinished() != curr.isFinished() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishedCmd(org, curr.isFinished() ) );
    }
    QList<QDate> orgdates = org.entries().keys();
    QList<QDate> currdates = curr.entries().keys();
    foreach ( const QDate &d, orgdates ) {
        if ( currdates.contains( d ) ) {
            if ( curr.entry( d ) == org.entry( d ) ) {
                continue;
            }
            if ( cmd == 0 ) cmd = new MacroCommand( c );
            kDebug(planDbg())<<"modify entry "<<d;
            Completion::Entry *e = new Completion::Entry( *( curr.entry( d ) ) );
            cmd->addCommand( new ModifyCompletionEntryCmd(org, d, e ) );
        } else {
            if ( cmd == 0 ) cmd = new MacroCommand( c );
            kDebug(planDbg())<<"remove entry "<<d;
            cmd->addCommand( new RemoveCompletionEntryCmd(org, d ) );
        }
    }
    foreach ( const QDate &d, currdates ) {
        if ( ! orgdates.contains( d ) ) {
            if ( cmd == 0 ) cmd = new MacroCommand( c );
            Completion::Entry *e = new Completion::Entry( * ( curr.entry( d ) ) );
            kDebug(planDbg())<<"add entry "<<d<<e;
            cmd->addCommand( new AddCompletionEntryCmd(org, d, e ) );
        }
    }
    const Completion::ResourceUsedEffortMap &map = curr.usedEffortMap();
    foreach ( const Resource *res, map.keys() ) {
        Resource *r = project.findResource( res->id() );
        if ( r == 0 ) {
            kWarning()<<"Can't find resource:"<<res->id()<<res->name();
            continue;
        }
        Completion::UsedEffort *ue = map[ r ];
        if ( ue == 0 ) {
            continue;
        }
        if ( org.usedEffort( r ) == 0 || *ue != *(org.usedEffort( r )) ) {
            if ( cmd == 0 ) cmd = new MacroCommand( c );
            cmd->addCommand( new AddCompletionUsedEffortCmd( org, r, new Completion::UsedEffort( *ue ) ) );
        }
    }
    return cmd;
}

void TaskProgressPanel::setEstimateScales( int day )
{
    QVariantList lst;
    lst << QVariant( day );
//    remainingEffort->setScales( lst );
//     remainingEffort->setFieldScale(0, day);
//     remainingEffort->setFieldRightscale(0, day);
//     remainingEffort->setFieldLeftscale(1, day);

//    actualEffort->setScales( QVariant( lst ) );
/*    actualEffort->setFieldScale(0, day);
    actualEffort->setFieldRightscale(0, day);
    actualEffort->setFieldLeftscale(1, day);*/
}

void TaskProgressPanel::slotWeekNumberChanged( int index )
{
    kDebug(planDbg())<<index<<","<<m_weekOffset;
    QDate date = QDate( m_year, 1, 1 ).addDays( Qt::Monday - QDate( m_year, 1, 1 ).dayOfWeek() );
    date = date.addDays( index * 7 );
    resourceTable->setCurrentMonday( date );
}

void TaskProgressPanel::slotAddResource()
{
    kDebug(planDbg());
    resourceTable->addResource();
    addResource->setEnabled( resourceTable->hasFreeResources() );
}

void TaskProgressPanel::slotEntryAdded( const QDate date )
{
    kDebug(planDbg())<<date;
}

//-------------------------------------

TaskProgressPanelImpl::TaskProgressPanelImpl( Task &task, QWidget *parent )
    : QWidget(parent),
      m_task(task),
      m_original( task.completion() ),
      m_completion( m_original ),
      m_dayLength(24),
      m_firstIsPrevYear( false ),
      m_lastIsNextYear( false )
{
    setupUi(this);

    addEntryBtn->setIcon(koIcon("list-add"));
    removeEntryBtn->setIcon(koIcon("list-remove"));

    connect(entryTable, SIGNAL(selectionChanged( const QItemSelection&, const QItemSelection& ) ), SLOT( slotSelectionChanged( const QItemSelection& ) ) );
    removeEntryBtn->setEnabled( false );

    editmode->setCurrentIndex( m_original.entrymode() - 1 );
    connect( editmode, SIGNAL( currentIndexChanged( int ) ), SLOT( slotEditmodeChanged( int ) ) );
    connect( editmode, SIGNAL( activated( int ) ), SLOT( slotChanged() ) );
    
    connect(resourceTable, SIGNAL(changed() ), SLOT( slotChanged() ) );
    connect(resourceTable, SIGNAL(resourceAdded() ), SLOT( slotChanged() ) );
    
    connect(entryTable, SIGNAL(changed() ), SLOT( slotChanged() ) );
    connect(entryTable, SIGNAL(rowInserted( const QDate ) ), SLOT( slotChanged() ) );

    connect(entryTable, SIGNAL(changed() ), SLOT( slotEntryChanged() ) );
    connect(entryTable, SIGNAL(rowInserted( const QDate ) ), SLOT( slotEntryChanged() ) );
    connect(entryTable, SIGNAL(rowRemoved( const QDate ) ), SLOT( slotEntryChanged() ) );

    connect( prevWeekBtn, SIGNAL( clicked( bool ) ), SLOT( slotPrevWeekBtnClicked() ) );
    connect( nextWeekBtn, SIGNAL( clicked( bool ) ), SLOT( slotNextWeekBtnClicked() ) );
    
    connect ( ui_year, SIGNAL( valueChanged( int ) ), SLOT( slotFillWeekNumbers( int ) ) );

    int y = 0;
    int wn = QDate::currentDate().weekNumber( &y );
    setYear( y );
    weekNumber->setCurrentIndex( wn - m_weekOffset );

}

void TaskProgressPanelImpl::slotChanged() {
    emit changed();
}

void TaskProgressPanelImpl::slotEditmodeChanged( int idx )
{
    m_completion.setEntrymode( static_cast<Completion::Entrymode>( idx + 1 ) );
    entryTable->model()->slotDataChanged();
    enableWidgets();
}

void TaskProgressPanelImpl::slotStartedChanged(bool state) {
    m_completion.setStarted( state );
    if (state) {
        QTime t = QTime::currentTime();
        t.setHMS( t.hour(), t.minute(), 0 );
        m_completion.setStartTime( KDateTime( QDateTime( QDate::currentDate(), t ) ) );
        startTime->setDateTime( m_completion.startTime() );
        slotCalculateEffort();
    }
    enableWidgets();
}

void TaskProgressPanelImpl::setFinished() {
    QTime t = QTime::currentTime();
    t.setHMS( t.hour(), t.minute(), 0 );
    finishTime->setDateTime( QDateTime( QDate::currentDate(), t ) );
    slotFinishTimeChanged( finishTime->dateTime() );
}

void TaskProgressPanelImpl::slotFinishedChanged(bool state) {
    kDebug(planDbg())<<state;
    m_completion.setFinished( state );
    if (state) {
        kDebug(planDbg())<<state;
        setFinished();
        kDebug(planDbg())<<finishTime->dateTime();
        slotCalculateEffort();
    }   
    enableWidgets();
}

void TaskProgressPanelImpl::slotFinishTimeChanged( const QDateTime &dt )
{
    if ( ! m_completion.isFinished() ) {
        return;
    }
    m_completion.setFinishTime( KDateTime( dt, KDateTime::Spec(KDateTime::LocalZone) ) );
    if ( m_completion.percentFinished() < 100 ) {
        m_completion.setPercentFinished( dt.date(), 100 );
    }
    entryTable->setCompletion( &m_completion ); // for refresh
}

void TaskProgressPanelImpl::slotStartTimeChanged( const QDateTime &dt )
{
    m_completion.setStartTime( KDateTime( dt, KDateTime::Spec(KDateTime::LocalZone) ) );
    finishTime->setMinimumDateTime( qMax( startTime->dateTime(), QDateTime(m_completion.entryDate(), QTime() ) ) );
    
}

void TaskProgressPanelImpl::slotEntryChanged()
{
    finishTime->setMinimumDateTime( qMax( startTime->dateTime(), QDateTime(m_completion.entryDate(), QTime() ) ) );
}

void TaskProgressPanelImpl::enableWidgets() {
    editmode->setEnabled( !finished->isChecked() );

    started->setEnabled(!finished->isChecked());
    finished->setEnabled(started->isChecked());
    finishTime->setEnabled(finished->isChecked());
    startTime->setEnabled(started->isChecked() && !finished->isChecked());

    addEntryBtn->setEnabled( started->isChecked() && !finished->isChecked() );
    removeEntryBtn->setEnabled( ! entryTable->selectionModel()->selectedIndexes().isEmpty() && started->isChecked() && ! finished->isChecked() );

    if ( finished->isChecked() ) {
        for ( int i = 0; i < entryTable->model()->columnCount(); ++i ) {
            entryTable->model()->setFlags( i, Qt::NoItemFlags );
        }
    }

    resourceTable->model()->setReadOnly( ( ! started->isChecked() ) || finished->isChecked() || m_completion.entrymode() != Completion::EnterEffortPerResource );
}


void TaskProgressPanelImpl::slotPercentFinishedChanged( int ) {
    slotCalculateEffort();
}

void TaskProgressPanelImpl::slotCalculateEffort()
{
}

void TaskProgressPanelImpl::slotPrevWeekBtnClicked()
{
    kDebug(planDbg());
    int i = weekNumber->currentIndex();
    if ( i == 0 ) {
        kDebug(planDbg())<<i;
        int decr = m_firstIsPrevYear ? 2 : 1;
        setYear( ui_year->value() - 1 );
        if ( m_lastIsNextYear ) {
            decr = 2;
        }
        weekNumber->setCurrentIndex( weekNumber->count() - decr );
    } else  {
        weekNumber->setCurrentIndex( i - 1 );
    }
}

void TaskProgressPanelImpl::slotNextWeekBtnClicked()
{
    int i = weekNumber->currentIndex();
    kDebug(planDbg())<<i<<weekNumber->count();
    if ( i == weekNumber->count() - 1 ) {
        kDebug(planDbg())<<i;
        int index = m_lastIsNextYear ? 1 : 0;
        setYear( ui_year->value() + 1 );
        if ( m_firstIsPrevYear ) {
            index = 1;
        }
        weekNumber->setCurrentIndex( index );
    } else {
        weekNumber->setCurrentIndex( i + 1 );
    }
}

void TaskProgressPanelImpl::setYear( int year )
{
    kDebug(planDbg());
    ui_year->setValue( year );
}

void TaskProgressPanelImpl::slotFillWeekNumbers( int year )
{
    kDebug(planDbg());
    weekNumber->clear();
    m_year = year;
    m_weekOffset = 1;
    int y = 0;
    QDate date( year, 1, 1 );
    int wn = date.weekNumber( &y );
    m_firstIsPrevYear = false;
    kDebug(planDbg())<<date<<wn<<y<<year;
    if ( y < year ) {
        weekNumber->addItem( i18nc( "Week number (year)", "Week %1 (%2)", wn, y ) );
        m_weekOffset = 0;
        m_firstIsPrevYear = true;
        kDebug(planDbg())<<"Added last week of prev year";
    }
    for ( int i=1; i <= 52; ++i ) {
        weekNumber->addItem( i18nc( "Week number", "Week %1", i ) );
    }
    date = QDate( year, 12, 31 );
    wn = date.weekNumber( &y );
    kDebug(planDbg())<<date<<wn<<y<<year;
    m_lastIsNextYear = false;
    if ( wn == 53 ) {
        weekNumber->addItem( i18nc( "Week number", "Week %1", wn ) );
    } else if ( wn == 1 ) {
        weekNumber->addItem( i18nc( "Week number (year)", "Week %1 (%2)", wn, y ) );
        m_lastIsNextYear = true;
    }
}

void TaskProgressPanelImpl::slotSelectionChanged( const QItemSelection &sel )
{
    removeEntryBtn->setEnabled( ! sel.isEmpty() && started->isChecked() && ! finished->isChecked() );
}

}  //KPlato namespace

#include "kpttaskprogresspanel.moc"
