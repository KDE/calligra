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

#ifndef KPTINTERVALEDIT_H
#define KPTINTERVALEDIT_H

#include "kptintervaleditbase.h"

#include <kdialog.h>

#include <QString>
#include <q3ptrlist.h>
#include <qpair.h>
#include <QWidget>

namespace KPlato
{

class IntervalEditImpl : public IntervalEditBase {
    Q_OBJECT
public:
    IntervalEditImpl(QWidget *parent);
    
    Q3PtrList<QPair<QTime, QTime> > intervals() const;
    void setIntervals(const Q3PtrList<QPair<QTime, QTime> > &intervals) const;
    
private slots:
    void slotClearClicked();
    void slotAddIntervalClicked();
    void slotIntervalSelectionChanged(Q3ListViewItem *item);
signals:
    void changed();
};

class IntervalEdit : public IntervalEditImpl {
    Q_OBJECT
public:
    IntervalEdit(QWidget *parent=0, const char *name=0);

};

}  //KPlato namespace

#endif // INTERVALEDIT_H
