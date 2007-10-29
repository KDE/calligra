/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTWORKPACKAGECONTROLPANEL_H
#define KPTWORKPACKAGECONTROLPANEL_H

#include "kplatoui_export.h"

#include "ui_kptworkpackagecontrolpanel.h"

#include <QWidget>


namespace KPlato
{

class Project;
class Task;

class WorkPackageControlPanel : public QWidget, public Ui_WorkPackageControlPanel
{
    Q_OBJECT
public:
    explicit WorkPackageControlPanel( Project &project, Task &task, QWidget *parent=0 );

protected slots:
    void slotSelectionChanged();
    void slotSendWPClicked();
    void slotLoadWPClicked();
    void slotViewWPClicked();
    void slotMailToClicked();

private:
    Project &m_project;
    Task &m_task;
};

} //KPlato namespace

#endif // KPTWORKPACKAGECONTROLPANEL_H
