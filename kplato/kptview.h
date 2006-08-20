/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2002 - 2005 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_VIEW
#define KPLATO_VIEW

#include <KoView.h>
#include "kptcontext.h"
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3PopupMenu>
#include <Q3PtrList>

class Q3ListViewItem;
class Q3PopupMenu;
class Q3HBoxLayout;
class QTabWidget;
class Q3WidgetStack;

class K3ListView;
class KPrinter;
class KAction;
class KActionMenu;
class KSelectAction;
class KToggleAction;
class KStatusBarLabel;

class DCOPObject;

namespace KPlato
{

class AccountsView;
class GanttView;
class PertView;
class ResourceView;
//class ReportView;
class Part;
class Node;
class Project;
class Relation;
class Context;
class ViewAdaptor;

class View : public KoView {
    Q_OBJECT

public:
    View(Part* part, QWidget* parent=0);
    ~View();
    /**
     * Support zooming.
     */
    virtual void setZoom(double zoom);

    Part *getPart()const;

    Project& getProject() const;

    virtual void setupPrinter(KPrinter &printer);
    virtual void print(KPrinter &printer);

    Q3PopupMenu *popupMenu(const QString& name);

    void projectCalculate();

    virtual ViewAdaptor* dbusObject();

    virtual bool setContext(Context &context);
    virtual void getContext(Context &context) const;

    void setTaskActionsEnabled(QWidget *w, bool on);
    void setScheduleActionsEnabled();

public slots:
    void slotUpdate(bool calculate);
    void slotEditResource();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewGantt();
    void slotViewExpected();
    void slotViewOptimistic();
    void slotViewPessimistic();

    void slotViewGanttResources();
    void slotViewGanttTaskName();
    void slotViewGanttTaskLinks();
    void slotViewGanttProgress();
    void slotViewGanttFloat();
    void slotViewGanttCriticalTasks();
    void slotViewGanttCriticalPath();
    void slotViewGanttNoInformation();
    void slotViewTaskAppointments();
    void slotViewPert();
    void slotViewResources();
    void slotViewResourceAppointments();
    void slotViewAccounts();
    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotDefineWBS();
    void slotGenerateWBS();
    void slotConfigure();
    void slotAddRelation(Node *par, Node *child);
    void slotModifyRelation(Relation *rel);
    void slotAddRelation(Node *par, Node *child, int linkType);
    void slotModifyRelation(Relation *rel, int linkType);

    void setBaselineMode(bool on);

    void slotExportGantt(); // testing
    void setTaskActionsEnabled(bool on);

    void slotRenameNode(Node *node, const QString& name);

    void slotPopupMenu(const QString& menuname, const QPoint & pos);

protected slots:
    void slotProjectCalendar();
    void slotProjectWorktime();
    void slotProjectCalculate();
    void slotProjectCalculateExpected();
    void slotProjectCalculateOptimistic();
    void slotProjectCalculatePessimistic();
    void slotProjectAccounts();
    void slotProjectResources();
    void slotViewReportDesign();
    void slotViewReports();

    void slotOpenNode();
    void slotTaskProgress();
    void slotDeleteTask();
    void slotIndentTask();
    void slotUnindentTask();
    void slotMoveTaskUp();
    void slotMoveTaskDown();

    void slotConnectNode();
    void slotChanged(QWidget *);
    void slotChanged();

    void slotAboutToShow(QWidget *widget);

#ifndef NDEBUG
    void slotPrintDebug();
    void slotPrintSelectedDebug();
    void slotPrintCalendarDebug();
    void slotPrintTestDebug();
#else
    static void slotPrintDebug() { };
    static void slotPrintSelectedDebug() { };
    static void slotPrintCalendarDebug() { };
    static void slotPrintTestDebug() { };
#endif

protected:
    virtual void updateReadWrite(bool readwrite);
    Node *currentTask();
    void updateView(QWidget *widget);

private:
    GanttView *m_ganttview;
    Q3HBoxLayout *m_ganttlayout;
    PertView *m_pertview;
    Q3HBoxLayout *m_pertlayout;
    Q3WidgetStack *m_tab;
    ResourceView *m_resourceview;
    AccountsView *m_accountsview;
//    ReportView *m_reportview;
    Q3PtrList<QString> m_reportTemplateFiles;

    bool m_baselineMode;

    int m_viewGrp;
    int m_defaultFontSize;
    int m_currentEstimateType;

    bool m_updateGanttview;
    bool m_updateResourceview;
    bool m_updateAccountsview;

    KStatusBarLabel *m_estlabel;

    ViewAdaptor* m_dbus;

    // ------ Edit
    KAction *actionCut;
    KAction *actionCopy;
    KAction *actionPaste;

    KAction *actionIndentTask;
    KAction *actionUnindentTask;
    KAction *actionMoveTaskUp;
    KAction *actionMoveTaskDown;

    // ------ View
    KAction *actionViewGantt;
    KToggleAction *actionViewExpected;
    KToggleAction *actionViewOptimistic;
    KToggleAction *actionViewPessimistic;

    KToggleAction *actionViewGanttResources;
    KToggleAction *actionViewGanttTaskName;
    KToggleAction *actionViewGanttTaskLinks;
    KToggleAction *actionViewGanttProgress;
    KToggleAction *actionViewGanttFloat;
    KToggleAction *actionViewGanttCriticalTasks;
    KToggleAction *actionViewGanttCriticalPath;
    KToggleAction *actionViewGanttNotScheduled;
    KToggleAction *actionViewTaskAppointments;
    KAction *actionViewPert;
    KAction *actionViewResources;
    KToggleAction *actionViewResourceAppointments;
    KAction *actionViewAccounts;
    KAction *actionViewReports;

    // ------ Insert
    KAction *actionAddTask;
    KAction *actionAddSubtask;
    KAction *actionAddMilestone;

    // ------ Project
    KAction *actionEditMainProject;
    KAction *actionEditStandardWorktime;
    KAction *actionEditCalendar;
    KAction *actionEditAccounts;
    KAction *actionEditResources;
    KActionMenu *actionCalculate;
    KAction *actionCalculateExpected;
    KAction *actionCalculateOptimistic;
    KAction *actionCalculatePessimistic;
    // ------ Reports
    KAction *actionFirstpage;
    KAction *actionPriorpage;
    KAction *actionNextpage;
    KAction *actionLastpage;

    // ------ Tools
    KAction *actionDefineWBS;
    KAction *actionGenerateWBS;

    // ------ Export (testing)
    KAction *actionExportGantt;

    // ------ Settings
    KAction *actionConfigure;

    // ------ Popup
    KAction *actionOpenNode;
    KAction *actionTaskProgress;
    KAction *actionDeleteTask;
    KAction *actionEditResource;

    //Test
    KAction *actNoInformation;
};

} //Kplato namespace

#endif
