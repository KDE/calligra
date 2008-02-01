/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATOWORK_VIEW
#define KPLATOWORK_VIEW

#include <KoView.h>

#include <QMenu>

#include <kprocess.h>

class QProgressBar;
class QTabWidget;
class QPrinter;
class QPrintDialog;

class KAction;
class KToggleAction;
class QLabel;

class KUrl;

class KoView;

class KPlatoWork_MainWindow;

namespace KPlato
{

class ViewBase;
class AccountsView;
class Document;
class GanttView;
class PertEditor;
class ResourceView;
class AccountsEditor;
class TaskEditor;
class CalendarEditor;
class ScheduleEditor;
class ScheduleManager;

class Node;
class Project;
class MainSchedule;
class Schedule;
class Resource;
class ResourceGroup;
class Relation;

}
using namespace KPlato;

/// Then namespace for the KPlato work package handler
namespace KPlatoWork
{

class Part;
class View;


//-------------
class View : public KoView
{
    Q_OBJECT

public:
    explicit View( Part* part, QWidget* parent = 0 );
    ~View();
    /**
     * Support zooming.
     */
    virtual void setZoom( double zoom );

    Part *getPart() const;

    Project& getProject() const;

    virtual void setupPrinter( QPrinter &printer, QPrintDialog &printDialog );
    virtual void print( QPrinter &printer, QPrintDialog &printDialog );

    QMenu *popupMenu( const QString& name );

//    virtual ViewAdaptor* dbusObject();

    virtual bool loadContext();
    virtual void saveContext( QDomElement &context ) const;

    QWidget *canvas() const;

    //virtual QDockWidget *createToolBox();

    KoDocument *hitTest( const QPoint &viewPos );

    ScheduleManager *currentScheduleManager() const;
    long currentScheduleId() const;
    
    ViewBase *createTaskInfoView();
    ViewBase *createDocumentsView();
    
    bool viewDocument( const KUrl &filename );
    
    KPlatoWork_MainWindow *kplatoWorkMainWindow() const;
    
signals:
    void currentScheduleManagerChanged( ScheduleManager *sm );
    void openInternalDocument( KoStore * );
    void sigUpdateReadWrite( bool );

public slots:
    void slotUpdate();
    
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();

    void slotConfigure();

    void slotPopupMenu( const QString& menuname, const QPoint &pos );

protected slots:
    void slotGuiActivated( ViewBase *view, bool );
    void slotPlugScheduleActions();
    void slotViewSchedule( QAction *act );
    void slotScheduleChanged( MainSchedule* );
    void slotScheduleAdded( const MainSchedule * );
    void slotScheduleRemoved( const MainSchedule * );

    void slotAddScheduleManager( Project *project );
    void slotDeleteScheduleManager( Project *project, ScheduleManager *sm );
    
    void slotProgressChanged( int value );

    void slotCurrentChanged( int );

    void slotEditDocument( Document *doc );
    void slotViewDocument( Document *doc );
    
    void slotTaskProgress();
    
protected:
    virtual void guiActivateEvent( KParts::GUIActivateEvent *event );
    virtual void updateReadWrite( bool readwrite );

    QAction *addScheduleAction( Schedule *sch );
    void setLabel();
    void updateView( QWidget *widget );

private slots:
    void slotActionDestroyed( QObject *o );

private:
    void createViews();
    void addPart( KParts::Part* part, const QString &name );
    
private:
    QTabWidget *m_tab;
    QWidget *m_currentWidget;
    QMap<QWidget*, KParts::Part*> m_partsMap;
    int m_viewGrp;
    int m_defaultFontSize;

    QLabel *m_estlabel;
    QProgressBar *m_progress;
    
    QActionGroup *m_scheduleActionGroup;
    QMap<QAction*, Schedule*> m_scheduleActions;
    ScheduleManager *m_manager;
    
    bool m_readWrite;
    
    // ------ Edit
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;

    // ------ Settings
    KAction *actionConfigure;

    KAction *actionTaskProgress;
};

} //KplatoWork namespace

#endif
