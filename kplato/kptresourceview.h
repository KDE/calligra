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

#include <KDGanttView.h>

class KPTView;
class KPTProject;
class QLayout;
class KDGanttViewItem;
class QPoint;

class KPTResourceGroup;
class KPTResource;

 class KPTResourceView : public KDGanttView
{
    Q_OBJECT

 public:

    KPTResourceView( KPTView *view, QWidget *parent );

    //~KPTResourceView();

	void zoom(double zoom);

    void draw(KPTProject &project);
    KPTView *mainView();

private:
    void drawResources(KDGanttViewItem *parent, KPTResourceGroup *group);
    void drawAppointments(KDGanttViewItem *parent, KPTResource *resource);

	KPTView *m_mainview;
    int m_defaultFontSize;

};
 #endif
