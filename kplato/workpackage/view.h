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
class QLabel;

class KAction;
class KToggleAction;
class KActionCollection;

class KUrl;

class KoStore;
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
class TaskWorkPackageView;

//-------------
class View : public QWidget
{
    Q_OBJECT

public:
    explicit View( Part* part, QWidget *parent, KActionCollection *collection );
    ~View();

    Part *part() const;

    virtual void setupPrinter( QPrinter &printer, QPrintDialog &printDialog );
    virtual void print( QPrinter &printer, QPrintDialog &printDialog );

    QMenu *popupMenu( const QString& name, const QPoint &pos );

//    virtual ViewAdaptor* dbusObject();

    virtual bool loadContext();
    virtual void saveContext( QDomElement &context ) const;

    ScheduleManager *currentScheduleManager() const;
    long currentScheduleId() const;
    
    TaskWorkPackageView *createTaskWorkPackageView();
//     ViewBase *createTaskInfoView();
//     ViewBase *createDocumentsView();
//     ViewBase *createTaskView();

    KPlatoWork_MainWindow *kplatoWorkMainWindow() const;
    
    Node *currentNode() const;
    Document *currentDocument() const;

signals:
    void currentScheduleManagerChanged( ScheduleManager *sm );
    void openInternalDocument( KoStore * );
    void sigUpdateReadWrite( bool );

    void viewDocument( Document *doc );

public slots:
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();

    void slotConfigure();

    void slotPopupMenu( const QString& menuname, const QPoint &pos );

    void slotTaskProgress();
    void slotTaskCompletion();

protected slots:
    void slotProgressChanged( int value );

    void slotEditDocument();
    void slotEditDocument( Document *doc );
    void slotViewDocument();
    
    void slotSendPackage();
    void slotPackageSettings();
    void slotTaskDescription();
    void slotRemoveCurrentPackage();
    void slotRemoveSelectedPackages();
    void slotSelectionChanged();

protected:
    virtual void updateReadWrite( bool readwrite );

    QAction *addScheduleAction( Schedule *sch );
    void setLabel();
    TaskWorkPackageView *currentView() const;

private:
    void createViews();
    
private:
    Part *m_part;

    QActionGroup *m_scheduleActionGroup;
    QMap<QAction*, Schedule*> m_scheduleActions;
    ScheduleManager *m_manager;
    
    bool m_readWrite;
    
    // ------ Edit
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    KAction *actionRemoveSelectedPackages;

    // ------ Settings
    KAction *actionConfigure;

    KAction *actionViewDocument;
    KAction *actionEditDocument;

    KAction *actionSendPackage;
    KAction *actionPackageSettings;
    KAction *actionTaskCompletion;
    KAction *actionViewDescription;
    KAction *actionRemoveCurrentPackage;
};

} //KplatoWork namespace

#endif
