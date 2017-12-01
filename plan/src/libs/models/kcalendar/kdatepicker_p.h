/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 1998-2001 Mirko Boehm (john@layt.net)
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

#ifndef KDATEPICKER_P_H
#define KDATEPICKER_P_H

#include <QDate>
#include <QLineEdit>
#include <QValidator>

namespace KPlato
{

/** Year selection widget.
* @internal
* @author Tim Gilman, Mirko Boehm, John Layt
*/

class KDatePickerPrivateYearSelector : public QLineEdit
{
    Q_OBJECT

public:
    KDatePickerPrivateYearSelector(const QDate &currentDate, QWidget *parent = 0);
    int year();
    void setYear(int year);

public Q_SLOTS:
    void yearEnteredSlot();

Q_SIGNALS:
    void closeMe(int);

protected:
    QIntValidator *val;
    int result;

private:
    QDate oldDate;

    Q_DISABLE_COPY(KDatePickerPrivateYearSelector)
};

} //namespace KPlato

#endif // KDATEPICKER_P_H

