/* This file is part of the KDE project
   Copyright (C) 2004 - 2006 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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
#include <q3header.h>
#include <QLabel>
#include <QLineEdit>
#include <q3datetimeedit.h>
#include <qpair.h>
#include <qdatetime.h>
#include <q3listview.h>
//Added by qt3to4:
#include <Q3PtrList>

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
  
    intervalList->setSortColumn(0);

    connect(bClear, SIGNAL(clicked()), SLOT(slotClearClicked()));
    connect(bAddInterval, SIGNAL(clicked()), SLOT(slotAddIntervalClicked()));
    connect(intervalList, SIGNAL(selectionChanged(Q3ListViewItem*)), SLOT(slotIntervalSelectionChanged(Q3ListViewItem*)));

}

void IntervalEditImpl::slotClearClicked() {
    bool c = intervalList->firstChild() != 0;
    intervalList->clear();
    if (c)
        emit changed();
}

void IntervalEditImpl::slotAddIntervalClicked() {
    new IntervalItem(intervalList, startTime->time(), endTime->time());
    emit changed();
}

void IntervalEditImpl::slotIntervalSelectionChanged(Q3ListViewItem *item) {
    IntervalItem *ii = dynamic_cast<IntervalItem *>(item);
    if (!ii)
        return;
    startTime->setTime(ii->interval().first);
    endTime->setTime(ii->interval().second);
}

Q3PtrList<QPair<QTime, QTime> > IntervalEditImpl::intervals() const {
    Q3PtrList<QPair<QTime, QTime> > l;
    Q3ListViewItem *i = intervalList->firstChild();
    for (; i; i = i->nextSibling()) {
        IntervalItem *item = dynamic_cast<IntervalItem*>(i);
        if (i)
            l.append(new QPair<QTime, QTime>(item->interval().first, item->interval().second));
    }
    return l;
}

void IntervalEditImpl::setIntervals(const Q3PtrList<QPair<QTime, QTime> > &intervals) const {
    intervalList->clear();
    Q3PtrListIterator<QPair<QTime, QTime> > it =intervals;
    for (; it.current(); ++it) {
        new IntervalItem(intervalList, it.current()->first, it.current()->second);
    }
}

}  //KPlato namespace

#include "kptintervaledit.moc"
