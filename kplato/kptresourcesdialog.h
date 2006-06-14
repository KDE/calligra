/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTRESOURCESDIALOG_H
#define KPTRESOURCESDIALOG_H

#include <kdialog.h>

class KCommand;

namespace KPlato
{

class Project;
class ResourcesPanel;
class Part;


class ResourcesDialog : public KDialog {
    Q_OBJECT
public:
    ResourcesDialog(Project &project, QWidget *parent=0, const char *name=0);

    KCommand *buildCommand(Part *part);
    
protected slots:
    void slotChanged();
    void slotOk();

private:
    Project &project;
    ResourcesPanel *panel;
};

}  //KPlato namespace

#endif // RESOURCESDIALOG_H
