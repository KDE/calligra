/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <kplato@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTRESOURCEVIEW_H
#define KPTRESOURCEVIEW_H

#include <qsplitter.h>

class KPTView;
class KPTProject;
class KPTResource;
class QLayout;
class KDGanttView;
class KDGanttViewItem;
class QPoint;
class QListView;
class QListViewItem;

class KPTResourceGroup;
class KPTResource;
class ResourceItemPrivate;

class KPrinter;

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

public slots:
    void resSelectionChanged(QListViewItem *item);
    void popupMenuRequested(QListViewItem * item, const QPoint & pos, int);

private:
    void drawResources(QListViewItem *parent, KPTResourceGroup *group);
    void drawAppointments(KPTResource *resource);

	KPTView *m_mainview;
    int m_defaultFontSize;

    ResourceItemPrivate *m_selectedItem;
    QListView *resList;
    QListView *appList;

};
 #endif
