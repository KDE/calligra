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

#include "kptintervaledit.h"
#include "intervalitem.h"
#include "kptcommand.h"

#include <QPushButton>
#include <QTreeWidget>
#include <QList>

#include <kdialog.h>
#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

IntervalEdit::IntervalEdit(QWidget *parent)
    : IntervalEditImpl(parent)
{
    //kDebug();
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
    intervalList->sortByColumn( 0, Qt::Ascending );
    
    connect(bClear, SIGNAL(clicked()), SLOT(slotClearClicked()));
    connect(bAddInterval, SIGNAL(clicked()), SLOT(slotAddIntervalClicked()));
    connect(intervalList, SIGNAL(itemSelectionChanged()), SLOT(slotIntervalSelectionChanged()));

}

void IntervalEditImpl::slotClearClicked() {
    bool c = intervalList->topLevelItemCount() > 0;
    intervalList->clear();
    if (c)
        emit changed();
}

void IntervalEditImpl::slotAddIntervalClicked() {
    new IntervalItem(intervalList, startTime->time(), (int)length->value() * 1000 * 60 *60 );
    emit changed();
}

void IntervalEditImpl::slotIntervalSelectionChanged() {
    QList<QTreeWidgetItem*> lst = intervalList->selectedItems();
    if (lst.count() == 0)
        return;
    
    IntervalItem *ii = static_cast<IntervalItem *>(lst[0]);
    startTime->setTime(ii->interval().first);
    length->setValue((double)ii->interval().second / (1000*60*60));
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

void IntervalEditImpl::setIntervals(const QList<TimeInterval*> &intervals) const {
    intervalList->clear();
    foreach (TimeInterval *i, intervals) {
        kDebug()<<i->first<<i->second;
        new IntervalItem(intervalList, i->first, i->second);
    }
}

//-------------------------------------------------------------
IntervalEditDialog::IntervalEditDialog( CalendarDay *day, QWidget *parent)
    : KDialog( parent ),
    m_day( day )
{
    //kDebug();
    setCaption( i18n("Edit Work Intervals") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug()<<&p;
    m_panel = new IntervalEdit( this );
    if ( day ) {
        m_panel->setIntervals( day->workingIntervals() );
    }
    setMainWidget( m_panel );
    enableButtonOk( false );

    connect( m_panel, SIGNAL( changed() ), SLOT( slotChanged() ) );
}

void IntervalEditDialog::slotChanged()
{
    enableButtonOk( true );
}

MacroCommand *IntervalEditDialog::buildCommand( Calendar *calendar, CalendarDay *day )
{
    kDebug();
    const QList<TimeInterval*> lst = m_panel->intervals();
    const QList<TimeInterval*> org = day->workingIntervals();
    if ( lst == org ) {
        return 0;
    }
    MacroCommand *cmd = 0;
    foreach ( TimeInterval *i, org ) {
        CalendarRemoveTimeIntervalCmd *c = new CalendarRemoveTimeIntervalCmd( calendar, day, i );
        if (cmd == 0) cmd = new MacroCommand("");
        cmd->addCommand(c);
    }
    foreach ( TimeInterval *i, lst ) {
        CalendarAddTimeIntervalCmd *c = new CalendarAddTimeIntervalCmd( calendar, day, i );
        if (cmd == 0) cmd = new MacroCommand("");
        cmd->addCommand(c);
    }
    if (cmd) {
        cmd->setText( i18n( "Modify Work Interval" ) );
    }
    return cmd;
}

}  //KPlato namespace

#include "kptintervaledit.moc"
