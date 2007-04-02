/* This file is part of the KDE project
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>

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

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <klineedit.h>
#include <ktextedit.h>
#include <kdatetimewidget.h>
#include <knuminput.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <k3command.h>

#include <kdebug.h>

#include "kpttask.h"
#include "kptcommand.h"
#include "kptdurationwidget.h"
#include "kptcalendar.h"
#include "kptresource.h"

namespace KPlato
{


//-----------------
TaskProgressPanel::TaskProgressPanel(Task &task, StandardWorktime *workTime, QWidget *parent, const char *name)
    : TaskProgressPanelImpl(parent, name),
      m_task(task),
      m_original( task.completion() ),
      m_completion( m_original ),
      m_dayLength(24)
{
    kDebug()<<k_funcinfo<<endl;
    started->setChecked(m_completion.isStarted());
    finished->setChecked(m_completion.isFinished());
    startTime->setDateTime(m_completion.startTime().dateTime());
    finishTime->setDateTime(m_completion.finishTime().dateTime());
    
    if ( m_completion.entryDate() < QDate::currentDate() ) {
        dateEdit->setDate( QDate::currentDate() );
    } else {
        dateEdit->setDate( m_completion.entryDate() );
    } 
    percentFinished->setValue(m_completion.percentFinished());
    
    if (workTime) {
        kDebug()<<k_funcinfo<<"daylength="<<workTime->durationDay().hours()<<endl;
        m_dayLength = workTime->durationDay().hours();
        setEstimateScales(m_dayLength);
    }
    remainingEffort->setValue(m_completion.remainingEffort());
    remainingEffort->setVisibleFields(DurationWidget::Days | DurationWidget::Hours | DurationWidget::Minutes);
    remainingEffort->setFieldUnit(0, i18nc("day", "d"));
    remainingEffort->setFieldUnit(1, i18nc("hour", "h"));
    remainingEffort->setFieldUnit(2, i18nc("minute", "m"));

    actualEffort->setValue(m_completion.actualEffort());
    actualEffort->setVisibleFields(DurationWidget::Days | DurationWidget::Hours | DurationWidget::Minutes);
    actualEffort->setFieldUnit(0, i18nc("day", "d"));
    actualEffort->setFieldUnit(1, i18nc("hour", "h"));
    actualEffort->setFieldUnit(2, i18nc("minute", "m"));
    
    scheduledEffort = task.effort()->expected();
    
    m_year = dateEdit->date().year();
    m_weekOffset = 1;
    int year = 0;
    QDate date( m_year, 1, 1 );
    int wn = date.weekNumber( &year );
    if ( year < date.year() ) {
        weekNumber->addItem( i18nc( "Week number (year)", "Week %1 (%2)", wn, year ) );
        m_weekOffset = 0;
    }
    for ( int i=1; i <= 52; ++i ) {
        weekNumber->addItem( i18nc( "Week number", "Week %1", i ) );
    }
    date = QDate( m_year, 12, 31 );
    wn = date.weekNumber( &year );
    kDebug()<<k_funcinfo<<date<<", "<<wn<<", "<<year<<endl;
    if ( wn == 53 ) {
        weekNumber->addItem( i18nc( "Week number", "Week %1", wn ) );
    } else if ( wn == 1 ) {
        weekNumber->addItem( i18nc( "Week number (year)", "Week %1 (%2)", wn, year ) );
    }
    date = dateEdit->date();
    wn = date.weekNumber( &year );
    if ( wn == 53 && year < m_year ) {
        weekNumber->setCurrentIndex( 0 );
    } else if ( wn == 1 && year > m_year ) {
        weekNumber->setCurrentIndex( weekNumber->count() - 1 );
    } else {
        weekNumber->setCurrentIndex( wn - m_weekOffset );
    }
    if ( m_completion.usedEffortMap().isEmpty() && m_task.requests() ) {
        foreach ( ResourceGroupRequest *g, m_task.requests()->requests() ) {
            foreach ( ResourceRequest *r, g->resourceRequests() ) {
                m_completion.addUsedEffort( r->resource() );
            }
        }
    }
    
    enableWidgets();
    started->setFocus();
    
    connect( weekNumber, SIGNAL( currentIndexChanged( int ) ), SLOT( slotWeekNumberChanged( int ) ) );
    connect( addResource, SIGNAL( clicked() ), SLOT( slotAddResource() ) );

    resourceTable->setCompletion( &m_completion );
    slotWeekNumberChanged( weekNumber->currentIndex() );
    
}


bool TaskProgressPanel::ok() {
    return true;
}

K3Command *TaskProgressPanel::buildCommand(Part *part) {
    K3MacroCommand *cmd = 0;
    QString c = i18n("Modify task completion");
    
    if ( m_original.isStarted() != started->isChecked() ) {
        if ( cmd == 0 ) cmd = new K3MacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartedCmd(part, m_original, started->isChecked() ) );
    }
    if ( m_original.isFinished() != finished->isChecked() ) {
        if ( cmd == 0 ) cmd = new K3MacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishedCmd(part, m_original, finished->isChecked() ) );
    }
    if ( m_original.startTime().dateTime() != startTime->dateTime() ) {
        if ( cmd == 0 ) cmd = new K3MacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartTimeCmd(part, m_original, startTime->dateTime() ) );
    }
    if ( m_original.finishTime().dateTime() != finishTime->dateTime() ) {
        if ( cmd == 0 ) cmd = new K3MacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishTimeCmd(part, m_original, finishTime->dateTime() ) );
    }
    if ( m_original.entryDate() != dateEdit->date() ) {
        if ( cmd == 0 ) cmd = new K3MacroCommand( c );
        Completion::Entry *e = new Completion::Entry( percentFinished->value(), remainingEffort->value(), actualEffort->value() );
        cmd->addCommand( new AddCompletionEntryCmd(part, m_original, dateEdit->date(), e ) );
    } else {
        if ( ( m_original.percentFinished() != percentFinished->value() ) ||
             ( m_original.remainingEffort()  != remainingEffort->value() ) ||
             ( m_original.actualEffort() != actualEffort->value() ) ) {
            if ( cmd == 0 ) cmd = new K3MacroCommand( c );
            Completion::Entry *e = new Completion::Entry( percentFinished->value(), remainingEffort->value(), actualEffort->value() );
            cmd->addCommand( new AddCompletionEntryCmd(part, m_original, dateEdit->date(), e ) );
        }
    }
    const Completion::ResourceUsedEffortMap &map = m_completion.usedEffortMap();
    foreach ( const Resource *r, map.keys() ) {
        Completion::UsedEffort *ue = map[ r ];
        if ( ue == 0 ) {
            continue;
        }
        if ( m_original.usedEffort( r ) == 0 || *ue != *(m_original.usedEffort( r )) ) {
            if ( cmd == 0 ) cmd = new K3MacroCommand( c );
            cmd->addCommand( new AddCompletionUsedEffortCmd( part, m_original, r, new Completion::UsedEffort( *ue ) ) );
        }
    }
    return cmd;
}

void TaskProgressPanel::setEstimateScales( int day )
{
    remainingEffort->setFieldScale(0, day);
    remainingEffort->setFieldRightscale(0, day);
    remainingEffort->setFieldLeftscale(1, day);

    actualEffort->setFieldScale(0, day);
    actualEffort->setFieldRightscale(0, day);
    actualEffort->setFieldLeftscale(1, day);
}

void TaskProgressPanel::slotWeekNumberChanged( int index )
{
    kDebug()<<k_funcinfo<<index<<", "<<m_weekOffset<<endl;
    QDate date = QDate( m_year, 1, 1 ).addDays( Qt::Monday - QDate( m_year, 1, 1 ).dayOfWeek() );
    date = date.addDays( index * 7 );
    resourceTable->setCurrentMonday( date );
}

void TaskProgressPanel::slotAddResource()
{
    kDebug()<<k_funcinfo<<endl;
}

//-------------------------------------

TaskProgressPanelImpl::TaskProgressPanelImpl(QWidget *parent, const char *name)
    : QWidget(parent) {
    
    setObjectName(name);
    setupUi(this);
    actualEffort = new DurationWidget(actualEffortHolder);
    if (actualEffortHolder->layout()) {
        actualEffortHolder->layout()->addWidget(actualEffort);
    }
    remainingEffort = new DurationWidget(remainingEffortHolder);
    if (remainingEffortHolder->layout()) {
        remainingEffortHolder->layout()->addWidget(remainingEffort);
    }
    connect(started, SIGNAL(toggled(bool)), SLOT(slotStartedChanged(bool)));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotFinishedChanged(bool)));

    connect(percentFinished, SIGNAL(valueChanged(int)), SLOT(slotPercentFinishedChanged(int)));
    connect(percentFinished, SIGNAL(valueChanged(int)), SLOT(slotChanged()));
    
    connect(startTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotChanged()));
    connect(finishTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotChanged()));
    
    connect(remainingEffort, SIGNAL(valueChanged()), SLOT(slotChanged()));
    connect(actualEffort, SIGNAL(valueChanged()), SLOT(slotChanged()));

    connect(resourceTable, SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( slotChanged() ) );
    
    connect( prevWeekBtn, SIGNAL( clicked( bool ) ), SLOT( slotPrevWeekBtnClicked() ) );
    connect( nextWeekBtn, SIGNAL( clicked( bool ) ), SLOT( slotNextWeekBtnClicked() ) );
}

void TaskProgressPanelImpl::slotChanged() {
    emit changed();
}

void TaskProgressPanelImpl::slotStartedChanged(bool state) {
    if (state) {
        startTime->setDateTime(QDateTime::currentDateTime());
        percentFinished->setValue(0);
        slotCalculateEffort();
    }
    enableWidgets();
}


void TaskProgressPanelImpl::slotFinishedChanged(bool state) {
    if (state) {
        percentFinished->setValue(100);
        if (!finishTime->dateTime().isValid()) {
            finishTime->setDateTime(QDateTime::currentDateTime());
        }
        slotCalculateEffort();
    }   
    enableWidgets();
}


void TaskProgressPanelImpl::enableWidgets() {
    started->setEnabled(!finished->isChecked());
    finished->setEnabled(started->isChecked());
    finishTime->setEnabled(started->isChecked());
    startTime->setEnabled(started->isChecked() && !finished->isChecked());
    performedGroup->setEnabled(started->isChecked() && !finished->isChecked());
}


void TaskProgressPanelImpl::slotPercentFinishedChanged( int value ) {
    slotCalculateEffort();
}

void TaskProgressPanelImpl::slotCalculateEffort()
{
    remainingEffort->setValue(scheduledEffort * ( 100.0 - (double)percentFinished->value() ) / 100.0 );
    actualEffort->setValue( scheduledEffort - remainingEffort->value() );
}

void TaskProgressPanelImpl::slotPrevWeekBtnClicked()
{
    kDebug()<<k_funcinfo<<endl;
    int i = weekNumber->currentIndex();
    if ( i > 0 && i < weekNumber->count() - 1 ) {
        weekNumber->setCurrentIndex( i - 1 );
    }
}

void TaskProgressPanelImpl::slotNextWeekBtnClicked()
{
    kDebug()<<k_funcinfo<<endl;
    int i = weekNumber->currentIndex();
    if ( i > 0 && i < weekNumber->count() - 1 ) {
        weekNumber->setCurrentIndex( i + 1 );
    }
}


}  //KPlato namespace

#include "kpttaskprogresspanel.moc"
