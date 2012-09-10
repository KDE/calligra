/* This file is part of the KDE project
   Copyright (C) 2004 - 2010 Dag Andersen <danders@get2net.dk>

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

#include "kptintervaledit.h"
#include "intervalitem.h"
#include "kptcommand.h"
#include "kptproject.h"

#include <KoIcon.h>

#include <QPushButton>
#include <QTreeWidget>
#include <QList>

#include <kdialog.h>
#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

IntervalEdit::IntervalEdit( CalendarDay *day, QWidget *parent)
    : IntervalEditImpl(parent)
{
    //kDebug(planDbg());
    if ( day ) {
        const QList<TimeInterval*> &intervals = day->timeIntervals();
        setIntervals( intervals );
        if ( ! intervals.isEmpty() ) {
            startTime->setTime( intervals.last()->endTime() );
            qreal l = ( intervals.last()->endTime().msecsTo( QTime().addMSecs( -1 ) ) + 1 )  / (1000.0*60.0*60.0);
            length->setValue( qMin( l, (qreal) 8.0 ) );
        }
    }
    enableButtons();
    startTime->setFocus();
}


//--------------------------------------------
IntervalEditImpl::IntervalEditImpl(QWidget *parent)
    : IntervalEditBase(parent) 
{
    intervalList->setColumnCount( 2 );
    QStringList lst;
    lst << i18nc( "Interval start time", "Start" )
        << i18nc( "Interval length", "Length" );
    intervalList->setHeaderLabels( lst );

    intervalList->setRootIsDecorated( false );
    intervalList->setSortingEnabled( true );
    intervalList->sortByColumn( 0, Qt::AscendingOrder );

    bAddInterval->setIcon(koIcon("list-add"));
    bRemoveInterval->setIcon(koIcon("list-remove"));
    bClear->setIcon(koIcon("edit-clear-list"));

    connect(bClear, SIGNAL(clicked()), SLOT(slotClearClicked()));
    connect(bAddInterval, SIGNAL(clicked()), SLOT(slotAddIntervalClicked()));
    connect(bRemoveInterval, SIGNAL(clicked()), SLOT(slotRemoveIntervalClicked()));
    connect(intervalList, SIGNAL(itemSelectionChanged()), SLOT(slotIntervalSelectionChanged()));
    
    connect( startTime, SIGNAL( timeChanged( const QTime& ) ), SLOT( enableButtons() ) );
    connect( length, SIGNAL( valueChanged( double ) ), SLOT( enableButtons() ) );
    
}

void IntervalEditImpl::slotClearClicked() {
    bool c = intervalList->topLevelItemCount() > 0;
    intervalList->clear();
    enableButtons();
    if (c)
        emit changed();
}

void IntervalEditImpl::slotAddIntervalClicked() {
    new IntervalItem(intervalList, startTime->time(), (int)(length->value() * 1000. * 60. *60.) );
    enableButtons();
    emit changed();
}

void IntervalEditImpl::slotRemoveIntervalClicked() {
    IntervalItem *item = static_cast<IntervalItem*>( intervalList->currentItem() );
    if ( item == 0) {
        return;
    }
    intervalList->takeTopLevelItem( intervalList->indexOfTopLevelItem( item ) );
    delete item;
    enableButtons();
    emit changed();
}


void IntervalEditImpl::slotIntervalSelectionChanged() {
    QList<QTreeWidgetItem*> lst = intervalList->selectedItems();
    if (lst.count() == 0)
        return;
    
    IntervalItem *ii = static_cast<IntervalItem *>(lst[0]);
    startTime->setTime(ii->interval().first);
    length->setValue((double)(ii->interval().second) / (1000.*60.*60.));
    
    enableButtons();
}

QList<TimeInterval*> IntervalEditImpl::intervals() const {
    QList<TimeInterval*> l;
    int cnt = intervalList->topLevelItemCount();
    for (int i=0; i < cnt; ++i) {
        IntervalItem *item = static_cast<IntervalItem*>(intervalList->topLevelItem(i));
        l.append(new TimeInterval(item->interval().first, item->interval().second));
    }
    return l;
}

void IntervalEditImpl::setIntervals(const QList<TimeInterval*> &intervals) {
    intervalList->clear();
    foreach (TimeInterval *i, intervals) {
        new IntervalItem(intervalList, i->first, i->second);
    }
    enableButtons();
}

void IntervalEditImpl::enableButtons() {
    bClear->setEnabled( ! intervals().isEmpty() );
    
    bRemoveInterval->setEnabled( intervalList->currentItem() );
    
    if ( length->value() == 0.0 ) {
        bAddInterval->setEnabled( false );
        return;
    }
    if ( QTime( 0, 0, 0 ).secsTo( startTime->time() ) + (int)(length->value() * 60. * 60.) > 24 * 60 * 60 ) {
        bAddInterval->setEnabled( false );
        return;
    }
    TimeInterval ti( startTime->time(),  (int)(length->value() * 1000. * 60. *60.) );
    foreach (TimeInterval *i, intervals()) {
        if ( i->intersects( ti ) ) {
            bAddInterval->setEnabled( false );
            return;
        }
    }
    bAddInterval->setEnabled( true );
}

//-------------------------------------------------------------
IntervalEditDialog::IntervalEditDialog( Calendar *calendar, const QList<CalendarDay*> &days, QWidget *parent)
    : KDialog( parent ),
    m_calendar( calendar ),
    m_days( days )
{
    //kDebug(planDbg());
    setCaption( i18n("Edit Work Intervals") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug(planDbg())<<&p;
    m_panel = new IntervalEdit( days.value( 0 ), this );
    setMainWidget( m_panel );
    enableButtonOk( false );

    connect( m_panel, SIGNAL( changed() ), SLOT( slotChanged() ) );
    connect( calendar->project(), SIGNAL( calendarRemoved( const Calendar* ) ), SLOT( slotCalendarRemoved( const Calendar* ) ) );
}

IntervalEditDialog::IntervalEditDialog( Calendar *calendar, const QList<QDate> &dates, QWidget *parent)
    : KDialog( parent ),
    m_calendar( calendar ),
    m_dates( dates )
{
    //kDebug(planDbg());
    setCaption( i18n("Edit Work Intervals") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug(planDbg())<<&p;
    foreach ( const QDate &d, dates ) {
        CalendarDay *day = calendar->findDay( d );
        if ( day ) {
            m_days << day;
        }
    }
    m_panel = new IntervalEdit( m_days.value( 0 ), this );
    setMainWidget( m_panel );
    enableButtonOk( false );

    connect( m_panel, SIGNAL( changed() ), SLOT( slotChanged() ) );
    connect( calendar->project(), SIGNAL( calendarRemoved( const Calendar* ) ), SLOT( slotCalendarRemoved( const Calendar* ) ) );
}

void IntervalEditDialog::slotCalendarRemoved( const Calendar *cal )
{
    if ( m_calendar == cal ) {
        reject();
    }
}

void IntervalEditDialog::slotChanged()
{
    enableButtonOk( true );
}

MacroCommand *IntervalEditDialog::buildCommand()
{
    MacroCommand *cmd = new MacroCommand( i18nc( "(qtundo-format)", "Modify Work Interval" ) );
    foreach ( const QDate &d, m_dates ) {
        // these are dates, weekdays don't have date
        CalendarDay *day = m_calendar->findDay( d );
        if ( day == 0 ) {
            // create a new day
            day = new CalendarDay( d );
            cmd->addCommand( new CalendarAddDayCmd( m_calendar, day ) );
        }
        MacroCommand *c = buildCommand( m_calendar, day );
        if ( c ) {
            cmd->addCommand( c );
        }
    }
    if ( m_dates.isEmpty() ) {
        // weekdays
        foreach ( CalendarDay *day, m_days ) {
            MacroCommand *c = buildCommand( m_calendar, day );
            if ( c ) {
                cmd->addCommand( c );
            }
        }
    }
    if ( cmd->isEmpty() ) {
        delete cmd;
        return 0;
    }
    return cmd;
}

MacroCommand *IntervalEditDialog::buildCommand( Calendar *calendar, CalendarDay *day )
{
    //kDebug(planDbg());
    const QList<TimeInterval*> lst = m_panel->intervals();
    if ( lst == day->timeIntervals() ) {
        return 0;
    }
    MacroCommand *cmd = 0;
    // Set to Undefined. This will also clear any intervals
    CalendarModifyStateCmd *c = new CalendarModifyStateCmd( calendar, day, CalendarDay::Undefined );
    if (cmd == 0) cmd = new MacroCommand("");
    cmd->addCommand(c);
    //kDebug(planDbg())<<"Set Undefined";

    foreach ( TimeInterval *i, lst ) {
        CalendarAddTimeIntervalCmd *c = new CalendarAddTimeIntervalCmd( calendar, day, i );
        if (cmd == 0) cmd = new MacroCommand("");
        cmd->addCommand(c);
    }
    if ( ! lst.isEmpty() ) {
        CalendarModifyStateCmd *c = new CalendarModifyStateCmd( calendar, day, CalendarDay::Working );
        if (cmd == 0) cmd = new MacroCommand("");
        cmd->addCommand(c);
    }
    if (cmd) {
        cmd->setText( i18nc( "(qtundo-format)", "Modify Work Interval" ) );
    }
    return cmd;
}

}  //KPlato namespace

#include "kptintervaledit.moc"
