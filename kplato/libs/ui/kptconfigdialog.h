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

#ifndef KPTCONFIGDIALOG_H
#define KPTCONFIGDIALOG_H

#include <kpagedialog.h>

class QWidget;

namespace KPlato
{

class Project;
class TaskDefaultPanel;
//class ConfigBehaviorPanel;
class Config;

class ConfigDialog : public KPageDialog {
    Q_OBJECT
public:
    ConfigDialog(Config &config, Project &project, QWidget *parent=0, const char *name=0);

protected slots:
    void slotApply();
    void slotOk();
    void slotDefault();
    void slotChanged();
    
private:
    Config &m_config;
    TaskDefaultPanel *m_taskDefaultPage;
    //ConfigBehaviorPanel *m_behaviorPage;

};

} //KPlato namespace

#endif // CONFIGDIALOG_H
