/* This file is part of the KDE project
   Copyright (C) 2005-2010 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;; either
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

#ifndef KPTTASKPROGRESSDIALOG_H
#define KPTTASKPROGRESSDIALOG_H

#include "kplatoui_export.h"

#include <kdialog.h>


namespace KPlato
{

class TaskProgressPanel;
class Task;
class Node;
class StandardWorktime;
class ScheduleManager;
class MacroCommand;

class KPLATOUI_EXPORT TaskProgressDialog : public KDialog {
    Q_OBJECT
public:
    TaskProgressDialog(Task &task, ScheduleManager *sm, StandardWorktime *workTime, QWidget *parent=0);

    MacroCommand *buildCommand();

protected slots:
    void slotChanged();
    void slotNodeRemoved( Node *node );

private:
    Node *m_node;
    TaskProgressPanel *m_panel;

};

} //KPlato namespace

#endif // TASKPROGRESSDIALOG_H
