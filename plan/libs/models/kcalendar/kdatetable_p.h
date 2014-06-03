/*  -*- C++ -*-
This file is part of the KDE libraries
Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
(C) 1998-2001 Mirko Boehm (mirko@kde.org)
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
#ifndef KPTDATETABLE_P_H
#define KPTDATETABLE_P_H

#include <QLineEdit>

namespace KPlato
{

/** Year selection widget.
* @internal
* @author Tim Gilman, Mirko Boehm
*/
class KDateInternalYearSelector : public QLineEdit
{
  Q_OBJECT
protected:
  QIntValidator *val;
  int result;
  
  virtual void focusOutEvent(QFocusEvent*);
    
public Q_SLOTS:
  void yearEnteredSlot();
Q_SIGNALS:
  void closeMe(int);
public:
  KDateInternalYearSelector( QWidget* parent=0);
  int getYear();
  void setYear(int year);

private:
  Q_DISABLE_COPY(KDateInternalYearSelector)
};


} //namespace KPlato

#endif // KDATETABLE_P_H
