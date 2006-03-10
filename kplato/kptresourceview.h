/* This file is part of the KDE project
   Copyright (C) 2003 - 2004 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEVIEW_H
#define KPTRESOURCEVIEW_H

#include <qsplitter.h>
#include <qdatetime.h>
#include <qvaluelist.h>

#include "kptcontext.h"

class QPoint;
class QListViewItem;

class KPrinter;

namespace KPlato
{
class ResListView;

class View;
class Project;
class Resource;
class Node;

class ResourceAppointmentsView;
class ResourceGroup;
class Resource;
class ResourceItemPrivate;


 class ResourceView : public QSplitter
{
    Q_OBJECT

 public:
    ResourceView(View *view, QWidget *parent);

    //~ResourceView();

    void zoom(double zoom);

    void draw(Project &project);
    View *mainView();

    Resource *currentResource();

    QValueList<int> listOffsets(int pageHeight) const;
    void print(KPrinter &printer);
    
    Node *currentNode() const { return m_currentNode; }
    
    virtual bool setContext(Context::Resourceview &context);
    virtual void getContext(Context::Resourceview &context) const;

    virtual QSize sizeHint() const;
    
public slots:
    void setShowAppointments(bool on) { m_showAppointments = on; }

signals:
    void itemDoubleClicked();
    
protected slots:
    void resSelectionChanged();
    void resSelectionChanged(QListViewItem *item);
    void slotItemDoubleClicked(QListViewItem*);
    void popupMenuRequested(QListViewItem * item, const QPoint & pos, int);

private:
    void drawResources(const Project &proj, QListViewItem *parent, ResourceGroup *group);

private:
	View *m_mainview;
    int m_defaultFontSize;

    ResourceItemPrivate *m_selectedItem;
    ResListView *resList;
    ResourceAppointmentsView *m_appview;
    Node *m_currentNode;
    QDate m_start;
    QDate m_end;

    bool m_showAppointments;

};

}  //KPlato namespace

#endif
