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

#include <qsplitter.h>

class KPTView;
class KPTProjectList;
class KPTTimeScale;
class KPTGanttCanvas;
class KPTNodeItem;
class KPTNode;
class QLayout;
class QListViewItem;

 class KPTGanttView : public QSplitter
{
    Q_OBJECT
    
 public:
 
    KPTGanttView( KPTView *view, QWidget *parent, QLayout *layout );

    //~KPTGanttView();
    
	void zoom(double zoom);
    
    void draw();
    KPTView *mainView();
    KPTNodeItem *currentItem();
    KPTNodeItem *selectedItem();

 public slots:
    void slotOpen(QListViewItem *item);
    void slotRMBPressed(QListViewItem *item, const QPoint & point, int col);
    
private:
    void init(QLayout *layout);
    
	KPTView *m_mainview;
    KPTProjectList *m_projectlist;
    KPTTimeScale *m_timescale;
    KPTGanttCanvas *m_canvasview;
    int m_defaultFontSize;
};
 #endif
