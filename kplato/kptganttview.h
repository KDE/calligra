/* This file is part of the KDE project
  Copyright (C) 2005 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
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

#ifndef KPTGANTTVIEW_H
#define KPTGANTTVIEW_H

#include "kptviewbase.h"
#include "kptcontext.h"

#include <q3ptrlist.h>

class QPoint;
class QSplitter;


namespace KPlato
{

class MyKDGanttView;
class TaskAppointmentsView;

class Node;
class Task;
class Part;
class Project;
class Relation;

class GanttView : public ViewBase
{
    Q_OBJECT

public:

    GanttView( Part *part, QWidget *parent, bool readWrite = true );

    //~GanttView();

    virtual void setZoom( double zoom );
    void show();
    virtual void setProject( Project *project );
    virtual void draw( Project &project );
    virtual void drawChanges( Project &project );

    Node *currentNode() const;

    void clear();

    virtual bool setContext( const Context &context );
    virtual void getContext( Context &context ) const;

    void setReadWriteMode( bool on );
    bool isReadWriteMode() const { return m_readWrite; }

    bool showNoInformation() const { return m_showNoInformation; }

signals:
    void modifyRelation( Relation *rel ) ;
    void addRelation( Node *par, Node *child );
    void modifyRelation( Relation *rel, int linkType ) ;
    void addRelation( Node *par, Node *child, int linkType );
    void itemDoubleClicked();

    /**
     * Requests a specific type of popup menu.
     * Usually a KPlato::View object is connected to this signal.
     */
    void requestPopupMenu( const QString& menuname, const QPoint & pos );

public slots:
    void setShowExpected( bool on ) { m_showExpected = on; }
    void setShowOptimistic( bool on ) { m_showOptimistic = on; }
    void setShowPessimistic( bool on ) { m_showPessimistic = on; }
    void setShowResources( bool on ) { m_showResources = on; }
    void setShowTaskName( bool on ) { m_showTaskName = on; }
    void setShowTaskLinks( bool on );
    void setShowProgress( bool on ) { m_showProgress = on; }
    void setShowPositiveFloat( bool on ) { m_showPositiveFloat = on; }
    void setShowCriticalTasks( bool on ) { m_showCriticalTasks = on; }
    void setShowCriticalPath( bool on ) { m_showCriticalPath = on; }
    void setShowNoInformation( bool on ) { m_showNoInformation = on; }
    void setShowAppointments( bool on ) { m_showAppointments = on; }
    void update();

private slots:
    void slotScheduleIdChanged( long id );

private:
    bool m_readWrite;
    int m_defaultFontSize;
    QSplitter *m_splitter;
    MyKDGanttView *m_gantt;
    TaskAppointmentsView *m_taskView;
    bool m_showExpected;
    bool m_showOptimistic;
    bool m_showPessimistic;
    bool m_showResources;
    bool m_showTaskName;
    bool m_showTaskLinks;
    bool m_showProgress;
    bool m_showPositiveFloat;
    bool m_showCriticalTasks;
    bool m_showCriticalPath;
    bool m_showNoInformation;
    bool m_showAppointments;
    Project *m_project;
    long m_id;
};

}  //KPlato namespace

#endif
