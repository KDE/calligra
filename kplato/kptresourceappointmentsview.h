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

#ifndef KPTRESOURCEAPPOINTMENTSVIEW_H
#define KPTRESOURCEAPPOINTMENTSVIEW_H

#include "kptcontext.h"
#include "kptdoublelistviewbase.h"
#include "kpteffortcostmap.h"

#include <QLabel>

class QComboBox;
class Q3DateEdit;
class QPushButton;
class QSplitter;
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
class QPushButton;

class K3ListView;
class K3ListViewItem;
class KPrinter;

namespace KPlato
{

class View;
class Project;
class Resource;
class Node;

class ResourceGroup;
class Resource;

class ResourceAppointmentsView : public DoubleListViewBase
{
    Q_OBJECT
public:

    ResourceAppointmentsView( QWidget *parent );

    //~ResourceAppointmentsView();

    void zoom(double zoom);

    void draw(Resource *resource, const QDate &start, const QDate &end);
    void draw();
    void print(KPrinter &printer);
    void clear();
    
    //virtual bool setContext(Context::ResourceAppointmentsView &context);
    //virtual void getContext(Context::ResourceAppointmentsView &context) const;

    virtual void createSlaveItems();
    
protected slots:
    void slotUpdate();
    
private:
    class NodeItem : public DoubleListViewBase::MasterListItem {
    public:
        NodeItem(Node *n, QTreeWidget *parent, bool highlight=false);
        NodeItem(Node *n, QTreeWidgetItem *parent, bool highlight=false);
        NodeItem(const QString& text, QTreeWidget *parent, bool highlight=false);
        NodeItem(const QString& text, QTreeWidgetItem *parent, bool highlight=false);
        
        Node *node;
        EffortCostMap effortMap;
    };
    
private:
    int m_defaultFontSize;
    Resource *m_resource;
    QDate m_start;
    QDate m_end;
    NodeItem *m_availItem;
    NodeItem *m_totalItem;
};

}  //KPlato namespace


#endif // KPTTASKAPPOINTMENTSVIEW_H
