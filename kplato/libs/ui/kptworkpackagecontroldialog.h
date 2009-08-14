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

#ifndef KPTWORKPACKAGECONTROLDIALOG_H
#define KPTWORKPACKAGECONTROLDIALOG_H

#include "kplatoui_export.h"

#include <kpagedialog.h>


namespace KPlato
{

class DocumentsPanel;
class WorkPackageSendPanel;
class Project;
class Task;
class Node;

// class KPLATOUI_EXPORT WorkPackageControlDialog : public KPageDialog
// {
//     Q_OBJECT
// public:
//     explicit WorkPackageControlDialog( Project &project, Task &task, QWidget *parent=0);
// 
// private:
//     WorkPackageControlPanel *m_wp;
//     DocumentsPanel *m_docs;
// };

class KPLATOUI_EXPORT WorkPackageSendDialog : public KDialog
{
    Q_OBJECT
public:
    explicit WorkPackageSendDialog( const QList<Node*> &tasks, QWidget *parent=0);

    WorkPackageSendPanel *panel() const { return m_wp; }

private:
    WorkPackageSendPanel *m_wp;
};

} //KPlato namespace

#endif // KPTWORKPACKAGECONTROLDIALOG_H
