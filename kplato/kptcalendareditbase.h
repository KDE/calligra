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

#ifndef KPTCALENDAREDITBASE_H
#define KPTCALENDAREDITBASE_H

#include <QVariant>
#include <QWidget>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <QLabel>
#include <Q3GridLayout>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class Q3GroupBox;
class QLabel;
class QComboBox;
class Q3ButtonGroup;
class QPushButton;
class Q3ListView;
class Q3ListViewItem;
class Q3TimeEdit;

namespace KPlato
{

class CalendarPanel;

class CalendarEditBase : public QWidget
{
    Q_OBJECT

public:
    CalendarEditBase( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    ~CalendarEditBase();

    Q3GroupBox* groupBox2;
    CalendarPanel* calendarPanel;
    Q3ButtonGroup* day;
    QComboBox* state;
    QPushButton* bApply;
    Q3GroupBox* groupBox4;
    Q3ListView* intervalList;
    Q3TimeEdit* startTime;
    Q3TimeEdit* endTime;
    QPushButton* bClear;
    QPushButton* bAddInterval;

protected:
    Q3HBoxLayout* CalendarEditBaseLayout;
    Q3GridLayout* groupBox2Layout;
    Q3HBoxLayout* layout10;
    Q3VBoxLayout* dayLayout;
    Q3HBoxLayout* layout8;
    Q3VBoxLayout* groupBox4Layout;
    Q3HBoxLayout* layout6;
    Q3HBoxLayout* layout5;

protected slots:
    virtual void languageChange();

};

}  //KPlato namespace

#endif // CALENDAREDITBASE_H
