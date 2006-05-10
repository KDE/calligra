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

#ifndef KPTINTERVALEDIT_H
#define KPTINTERVALEDIT_H

#include "kptintervaleditbase.h"

#include <kdialogbase.h>

#include <QString>
#include <q3ptrlist.h>
#include <qpair.h>

namespace KPlato
{

class IntervalEditImpl : public IntervalEditBase {
    Q_OBJECT
public:
    IntervalEditImpl(const Q3PtrList<QPair<QTime, QTime> > &intervals, QWidget *parent);
    
    Q3PtrList<QPair<QTime, QTime> > intervals() const;
    
private slots:
    void slotCheckAllFieldsFilled();
    void slotEnableButtonOk(bool on);
    
    void slotClearClicked();
    void slotAddIntervalClicked();
    void slotIntervalSelectionChanged(Q3ListViewItem *item);
signals:
    void obligatedFieldsFilled(bool yes);
    void enableButtonOk(bool);
    
};

class IntervalEdit : public KDialogBase {
    Q_OBJECT
public:
    IntervalEdit(const Q3PtrList<QPair<QTime, QTime> > &intervals, QWidget *parent=0, const char *name=0);
    
    Q3PtrList<QPair<QTime, QTime> > intervals() const;
    
private:
    IntervalEditImpl *dia;
};

}  //KPlato namespace

#endif // INTERVALEDIT_H
