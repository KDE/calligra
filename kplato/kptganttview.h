/* This file is part of the KDE project
   Copyright (C) 2002 The Koffice Team <koffice@kde.org>

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
 
#ifndef KPTGANTTVIEW_H
#define KPTGANTTVIEW_H

#include <KDGanttView.h>

class KPTView;
class KPTNode;
class QLayout;
class QListViewItem;
class QPoint;

class KDGanttViewSummaryItem;
class KDGanttViewItem;

 class KPTGanttView : public KDGanttView
{
    Q_OBJECT
    
 public:
 
    KPTGanttView( KPTView *view, QWidget *parent );

    //~KPTGanttView();
    
	void zoom(double zoom);
    
    void draw(KPTNode &node);
    KPTView *mainView();

	KPTNode *currentNode();

public slots:
    void popupMenuRequested(KDGanttViewItem * item, const QPoint & pos, int);
	
private slots:
    void currentItemChanged(KDGanttViewItem *);
	
private:
    void drawChildren(KDGanttViewSummaryItem *item, KPTNode &node);
    void drawProject(KDGanttViewSummaryItem *parentItem, KPTNode &node);
    void drawTask(KDGanttViewSummaryItem *parentItem, KPTNode &node);
	void drawMilestone(KDGanttViewSummaryItem *parentItem, KPTNode &node);

private:    
	KPTView *m_mainview;
    int m_defaultFontSize;
	KDGanttViewItem *m_currentItem;
};
 #endif
