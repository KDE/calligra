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

class QLayout;
class KDGanttView;
class KDGanttViewItem;
class QPoint;
class QListView;
class QListViewItem;

class KPrinter;

namespace KPlato
{

class KPTView;
class KPTProject;
class KPTResource;
class KPTNode;

class KPTResourceGroup;
class KPTResource;
class ResourceItemPrivate;
class KPTContext;


 class KPTResourceView : public QSplitter
{
    Q_OBJECT

 public:

    KPTResourceView( KPTView *view, QWidget *parent );

    //~KPTResourceView();

    void zoom(double zoom);

    void draw(KPTProject &project);
    KPTView *mainView();

    KPTResource *currentResource();

    void print(KPrinter &printer);

    KPTNode *currentNode() const { return m_currentNode; }
    
    virtual bool setContext(KPTContext &context);
    virtual void getContext(KPTContext &context) const;

public slots:
    void resSelectionChanged(QListViewItem *item);
    void popupMenuRequested(QListViewItem * item, const QPoint & pos, int);
    void appListMenuRequested(QListViewItem * item, const QPoint & pos, int);

protected slots:
    void slotItemRenamed(QListViewItem* item, int col, const QString& text);
private:
    void drawResources(QListViewItem *parent, KPTResourceGroup *group);
    void drawAppointments(KPTResource *resource);

	KPTView *m_mainview;
    int m_defaultFontSize;

    ResourceItemPrivate *m_selectedItem;
    QListView *resList;
    QListView *appList;
    
    KPTNode *m_currentNode;
    QDate m_start;
    QDate m_end;

};

}  //KPlato namespace

#endif
