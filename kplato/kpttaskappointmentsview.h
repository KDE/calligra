/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTTASKAPPOINTMENTSVIEW_H
#define KPTTASKAPPOINTMENTSVIEW_H

#include "kptcontext.h"
#include "kptdoublelistviewbase.h"
#include "kpteffortcostmap.h"
//Added by qt3to4:
#include <QLabel>

class QComboBox;
class Q3DateEdit;
class QPushButton;
class QSplitter;
class Q3ListViewItem;
class QLabel;
class QPushButton;

class K3ListView;
class K3ListViewItem;
class KPrinter;

namespace KPlato
{

class Project;
class Resource;
class Task;

class ResourceGroup;
class Resource;
class ResourceItemPrivate;

class TaskAppointmentsView : public DoubleListViewBase
{
    Q_OBJECT
public:

    TaskAppointmentsView(QWidget *parent);

    //~TaskAppointmentsView();

    void zoom(double zoom);

    void draw();
    void draw(Task *task);
    void print(KPrinter &printer);
    void clear();

    //virtual bool setContext(Context::TaskAppointmentsView &context);
    //virtual void getContext(Context::TaskAppointmentsView &context) const;

    virtual void createSlaveItems();

protected slots:
    void slotUpdate();

private:
    class ResourceItem : public DoubleListViewBase::MasterListItem {
    public:
        ResourceItem(Resource *r, Q3ListView *parent, bool highlight=false);
        ResourceItem(Resource *r, Q3ListViewItem *parent, bool highlight=false);
        ResourceItem(QString text, Q3ListViewItem *parent, bool highlight=false);

        Resource *resource;
        EffortCostMap effortMap;
    };

private:

    int m_defaultFontSize;
    Task *m_task;
};

}  //KPlato namespace


#endif // KPTTASKAPPOINTMENTSVIEW_H
