/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qlistview.h>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

IntervalEdit::IntervalEdit(const QPtrList<QPair<QTime, QTime> > &intervals, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Edit Interval"), Ok|Cancel, Ok, parent, name, true, true)
{
    //kdDebug()<<k_funcinfo<<endl;
    dia = new IntervalEditImpl(intervals, this);

    setMainWidget(dia);
    enableButtonOK(false);

    connect(dia, SIGNAL(obligatedFieldsFilled(bool) ), SLOT(enableButtonOK(bool)));
    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOK(bool)));
}

QPtrList<QPair<QTime, QTime> > IntervalEdit::intervals() const {
    return dia->intervals();
}


IntervalEditImpl::IntervalEditImpl(const QPtrList<QPair<QTime, QTime> > &intervals, QWidget *parent)
    : IntervalEditBase(parent) {

    intervalList->setSortColumn(0);
    QPtrListIterator<QPair<QTime, QTime> > it = intervals;
    for (; it.current(); ++it) {
        new IntervalItem(intervalList, it.current()->first, it.current()->second);
    }

    connect(bClear, SIGNAL(clicked()), SLOT(slotClearClicked()));
    connect(bAddInterval, SIGNAL(clicked()), SLOT(slotAddIntervalClicked()));
    connect(intervalList, SIGNAL(selectionChanged(QListViewItem*)), SLOT(slotIntervalSelectionChanged(QListViewItem*)));

}


void IntervalEditImpl::slotEnableButtonOk(bool on) {
    emit enableButtonOk(on);
}

void IntervalEditImpl::slotCheckAllFieldsFilled() {
    emit obligatedFieldsFilled(true); //FIXME
}

void IntervalEditImpl::slotClearClicked() {
    intervalList->clear();
}

void IntervalEditImpl::slotAddIntervalClicked() {
    new IntervalItem(intervalList, startTime->time(), endTime->time());
    slotEnableButtonOk(true);
}

void IntervalEditImpl::slotIntervalSelectionChanged(QListViewItem *item) {
    IntervalItem *ii = dynamic_cast<IntervalItem *>(item);
    if (!ii)
        return;
    startTime->setTime(ii->interval().first);
    endTime->setTime(ii->interval().second);
}

QPtrList<QPair<QTime, QTime> > IntervalEditImpl::intervals() const {
    QPtrList<QPair<QTime, QTime> > l;
    QListViewItem *i = intervalList->firstChild();
    for (; i; i = i->nextSibling()) {
        IntervalItem *item = dynamic_cast<IntervalItem*>(i);
        if (i)
            l.append(new QPair<QTime, QTime>(item->interval().first, item->interval().second));
    }
    return l;
}

}  //KPlato namespace

#include "kptintervaledit.moc"
