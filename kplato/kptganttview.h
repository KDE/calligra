/* This file is part of the KDE project
   Copyright (C) 2002 Dag Andersen <danders@get2net.dk>

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
class KPTNode;
class KPTTask;
class KPTProject;
class QLayout;
class QListViewItem;
class QPoint;
class QListView;
class QLineEdit;
class QSpinBox;

class KDGanttView;
class KDGanttViewSummaryItem;
class KDGanttViewItem;

class KPrinter;

class KPTTaskAppointmentsView : QWidget
{
    Q_OBJECT

public:
    KPTTaskAppointmentsView(QWidget *parent, const char* name = 0 );

    void clear();
    void draw(KPTTask *task);

private:
    QListView *appList;
    QLineEdit *m_taskName;
    QLineEdit *m_responsible;
    QLineEdit *m_costToDate;
    QLineEdit *m_totalCost;
    QLineEdit *m_workToDate;
    QLineEdit *m_totalWork;
    QSpinBox *m_completion;
    QLineEdit *m_deviationCost;
    QLineEdit *m_deviationWork;

};

 class KPTGanttView : public QSplitter
{
    Q_OBJECT

 public:

    KPTGanttView( KPTView *view, QWidget *parent, const char* name = 0  );

    //~KPTGanttView();

	void zoom(double zoom);

    void draw(KPTProject &project);
    KPTView *mainView();

	KPTNode *currentNode();

	void clear();
    void print(KPrinter &printer);

public slots:
    void popupMenuRequested(KDGanttViewItem * item, const QPoint & pos, int);

private slots:
    void currentItemChanged(KDGanttViewItem *);
	void slotItemDoubleClicked(KDGanttViewItem*);

private:
    void drawChildren(KDGanttViewSummaryItem *item, KPTNode &node);
    void drawProject(KDGanttViewSummaryItem *parentItem, KPTNode *node);
    void drawSubProject(KDGanttViewSummaryItem *parentItem, KPTNode *node);
    void drawSummaryTask(KDGanttViewSummaryItem *parentItem, KPTTask *task);
    void drawTask(KDGanttViewSummaryItem *parentItem, KPTTask *task);
	void drawMilestone(KDGanttViewSummaryItem *parentItem, KPTTask *task);

    void drawRelations();
    void drawRelations(KDGanttViewItem *item);
    void drawChildRelations(KDGanttViewItem *item);

private:
	KPTView *m_mainview;
    int m_defaultFontSize;
	KDGanttViewItem *m_currentItem;
    KDGanttView *m_gantt;
    KPTTaskAppointmentsView *m_taskView;
};
 #endif
