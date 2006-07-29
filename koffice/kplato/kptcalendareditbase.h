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

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QComboBox;
class QButtonGroup;
class QPushButton;
class QListView;
class QListViewItem;
class QTimeEdit;

namespace KPlato
{

class CalendarPanel;

class CalendarEditBase : public QWidget
{
    Q_OBJECT

public:
    CalendarEditBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CalendarEditBase();

    QGroupBox* groupBox2;
    CalendarPanel* calendarPanel;
    QButtonGroup* day;
    QComboBox* state;
    QPushButton* bApply;
    QGroupBox* groupBox4;
    QListView* intervalList;
    QTimeEdit* startTime;
    QTimeEdit* endTime;
    QPushButton* bClear;
    QPushButton* bAddInterval;

protected:
    QHBoxLayout* CalendarEditBaseLayout;
    QGridLayout* groupBox2Layout;
    QHBoxLayout* layout10;
    QVBoxLayout* dayLayout;
    QHBoxLayout* layout8;
    QVBoxLayout* groupBox4Layout;
    QHBoxLayout* layout6;
    QHBoxLayout* layout5;

protected slots:
    virtual void languageChange();

};

}  //KPlato namespace

#endif // CALENDAREDITBASE_H
