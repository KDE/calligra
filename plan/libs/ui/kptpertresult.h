/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>
  Copyright (C) 2007 Dag Andersen <danders@get2net>

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

#ifndef KPTPERTRESULT_H
#define KPTPERTRESULT_H

#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptpertcpmmodel.h"

#include "ui_kptpertresult.h"
#include "ui_kptcpmwidget.h"

#include <QList>

#include <kdebug.h>


class KoDocument;

class KAction;

/// The main namespace
namespace KPlato
{

class DateTime;
class Duration;
class Estimate;
class Node;
class Project;
class ScheduleManager;
class Task;
class View;

class KPLATOUI_EXPORT PertResult : public ViewBase
{
    Q_OBJECT
public:
    explicit PertResult(KoPart *part, KoDocument *doc, QWidget *parent = 0);
    
    void setupGui();
    void setProject( Project *project );
    Project *project() const { return m_project; }
    void draw( Project &project );
    void draw();

    PertResultItemModel *model() const { return static_cast<PertResultItemModel*>( widget.treeWidgetTaskResult->model() ); }

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &/*context*/ ) const;

    KoPrintJob *createPrintJob();
    
    Node *currentNode() const;
            
public slots:
    void slotScheduleSelectionChanged( ScheduleManager *sm );
    
protected slots:
    void slotProjectCalculated( ScheduleManager *sm );
    void slotScheduleManagerToBeRemoved( const ScheduleManager *sm );
    void slotScheduleManagerChanged( ScheduleManager *sm );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    void slotContextMenuRequested( const QModelIndex& index, const QPoint& pos );
    
    void slotSplitView();
    virtual void slotOptions();
    
private:
    Node * m_node;
    Project * m_project;
    ScheduleManager *current_schedule;
    Ui::PertResult widget;
    
private slots:
    void slotUpdate();

};

//--------------------
class KPLATOUI_EXPORT PertCpmView : public ViewBase
{
    Q_OBJECT
public:
    explicit PertCpmView(KoPart *part, KoDocument *doc, QWidget *parent = 0);
    
    void setupGui();
    void setProject( Project *project );
    Project *project() const { return m_project; }
    void draw( Project &project );
    void draw();
    
    CriticalPathItemModel *model() const { return static_cast<CriticalPathItemModel*>( widget.cpmTable->model() ); }

    double probability( double z ) const;
    double valueZ( double p ) const;
    
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &/*context*/ ) const;

    KoPrintJob *createPrintJob();
    
    Node *currentNode() const;
    
public slots:
    void slotScheduleSelectionChanged( ScheduleManager *sm );
    
protected slots:
    void slotProjectCalculated( ScheduleManager *sm );
    void slotScheduleManagerToBeRemoved( const ScheduleManager *sm );
    void slotScheduleManagerChanged( ScheduleManager *sm );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    void slotContextMenuRequested( const QModelIndex& index, const QPoint& pos );
    
    void slotSplitView();
    virtual void slotOptions();
    
    void slotFinishTimeChanged( const QDateTime &dt );
    void slotProbabilityChanged( int value );

private slots:
    void slotUpdate();

private:
    Project * m_project;
    QList<Node *> m_criticalPath;
    ScheduleManager *current_schedule;
    Ui::CpmWidget widget;
    
    bool block;
};

}  //KPlato namespace

#endif
