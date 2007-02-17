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

#include <QPushButton>
#include <QComboBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QList>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

IntervalEdit::IntervalEdit(QWidget *parent, const char *name)
    : IntervalEditImpl(parent)
{
    //kDebug()<<k_funcinfo<<endl;
}


//--------------------------------------------
IntervalEditImpl::IntervalEditImpl(QWidget *parent)
    : IntervalEditBase(parent) {
  
    intervalList->setSortingEnabled(true);

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
    new IntervalItem(intervalList, startTime->time(), endTime->time());
    emit changed();
}

void IntervalEditImpl::slotIntervalSelectionChanged() {
    QList<QTreeWidgetItem*> lst = intervalList->selectedItems();
    if (lst.count() == 0)
        return;
    
    IntervalItem *ii = static_cast<IntervalItem *>(lst[0]);
    startTime->setTime(ii->interval().first);
    endTime->setTime(ii->interval().second);
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
        new IntervalItem(intervalList, i->first, i->second);
    }
}

}  //KPlato namespace

#include "kptintervaledit.moc"
