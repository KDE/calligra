/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "kptview.h"
#include "kptfactory.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kptmainprojectdialog.h"
#include "kptprojectdialog.h"
#include "kpttask.h"
#include "kpttaskdialog.h"
#include "kptganttview.h"
#include "kptpertview.h"
#include "kptreportview.h"
#include "kptdatetime.h"
#include "kptcommand.h"

#include "kptresourceview.h"
#include "kptresourcedialog.h"
#include "kptresource.h"
#include "kptcalendarlistdialog.h"
#include "kptstandardworktimedialog.h"
#include "kptcanvasitem.h"

#include "KDGanttView.h"
#include "KDGanttViewTaskItem.h"
#include "KPtViewIface.h"

#include <koKoolBar.h>
#include <koRect.h>

#include <qpainter.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qcanvas.h>
#include <qscrollview.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvbox.h>
#include <qgrid.h>
#include <qsize.h>
#include <qheader.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qwidgetstack.h>
#include <qtimer.h>

#include <kiconloader.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kstdaccel.h>
#include <kaccelgen.h>
#include <kdeversion.h>
#include <kxmlguifactory.h>
#include <kprinter.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <kcommand.h>

KPTView::KPTView(KPTPart* part, QWidget* parent, const char* /*name*/)
    : KoView(part, parent, "Main View"),
    m_ganttview(0),
    m_ganttlayout(0),
    m_pertview(0),
    m_pertlayout(0),
    m_reportview(0)
{
    //kdDebug()<<k_funcinfo<<endl;
    setInstance(KPTFactory::global());
    setXMLFile("kplato.rc");
    m_dcop = 0L;
    // build the DCOP object
    dcopObject();

	m_tab = new QWidgetStack(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
	layout->add(m_tab);

	m_ganttview = new KPTGanttView( this, m_tab);
	m_tab->addWidget(m_ganttview);

	m_pertview = new KPTPertView( this, m_tab, layout );
    m_tab->addWidget(m_pertview);

    m_resourceview = new KPTResourceView( this, m_tab );
    m_tab->addWidget(m_resourceview);

    m_reportview = new KPTReportView(this, m_tab);
    m_tab->addWidget(m_reportview);

    connect(m_tab, SIGNAL(aboutToShow(QWidget *)), this, SLOT(slotAboutToShow(QWidget *)));

	// The menu items
    // ------ Edit
    KStdAction::cut( this, SLOT( slotEditCut() ), actionCollection(), "edit_cut" );
    KStdAction::copy( this, SLOT( slotEditCopy() ), actionCollection(), "edit_copy" );
    KStdAction::paste( this, SLOT( slotEditPaste() ), actionCollection(), "edit_paste" );

	new KAction(i18n("Indent Task"), "indent_task", 0, this,
		SLOT(slotIndentTask()), actionCollection(), "indent_task");
    new KAction(i18n("Unindent Task"), "unindent_task", 0, this,
		SLOT(slotUnindentTask()), actionCollection(), "unindent_task");
    new KAction(i18n("Move Up"), "move_task_up", 0, this,
		SLOT(slotMoveTaskUp()), actionCollection(), "move_task_up");
    new KAction(i18n("Move Down"), "move_task_down", 0, this,
		SLOT(slotMoveTaskDown()), actionCollection(), "move_task_down");

    // ------ View
    new KAction(i18n("Gantt"), "gantt_chart", 0, this,
		SLOT(slotViewGantt()), actionCollection(), "view_gantt");
    new KAction(i18n("PERT"), "pert_chart", 0, this,
		SLOT(slotViewPert()), actionCollection(), "view_pert");
    new KAction(i18n("Resources"), "resources", 0, this,
		SLOT(slotViewResources()), actionCollection(), "view_resources");

    // ------ Insert
    new KAction(i18n("Task..."), "add_task", 0, this,
		SLOT(slotAddTask()), actionCollection(), "add_task");
    new KAction(i18n("Sub-Task..."), "add_sub_task", 0, this,
		SLOT(slotAddSubTask()), actionCollection(), "add_sub_task");
    new KAction(i18n("Milestone..."), "add_milestone", 0, this,
		SLOT(slotAddMilestone()), actionCollection(), "add_milestone");

    // ------ Project
    new KAction(i18n("Edit Main Project..."), "project_edit", 0, this,
		SLOT(slotProjectEdit()), actionCollection(), "project_edit");
    new KAction(i18n("Edit calendar..."), "project_calendar", 0, this,
		SLOT(slotProjectCalendar()), actionCollection(), "project_calendar");
    new KAction(i18n("Edit standard worktime..."), "project_worktime", 0, this,
		SLOT(slotProjectWorktime()), actionCollection(), "project_worktime");
    new KAction(i18n("Calculate"), "project_calculate", 0, this,
		SLOT(slotProjectCalculate()), actionCollection(), "project_calculate");

    // ------ Reports
    actionReportGenerate = new KSelectAction(i18n("Generate"), 0, actionCollection(), "report_generate");
    setReportGenerateMenu();
    connect(actionReportGenerate, SIGNAL(activated(int)), SLOT(slotReportGenerate(int)));

	KStdAction::firstPage(m_reportview,SLOT(slotPrevPage()),actionCollection(),"go_firstpage");
	KStdAction::prior(m_reportview,SLOT(slotPrevPage()),actionCollection(),"go_prevpage");
	KStdAction::next(m_reportview,SLOT(slotNextPage()),actionCollection(), "go_nextpage");
	KStdAction::lastPage(m_reportview,SLOT(slotLastPage()),actionCollection(), "go_lastpage");

//     new KAction(i18n("Design..."), "report_design", 0, this,
//         SLOT(slotReportDesign()), actionCollection(), "report_design");


    // ------ Tools
    //new KAction(i18n("Resource Editor..."), "edit_resource", 0, this,
	//	SLOT(slotEditResource()), actionCollection(), "edit_resource");

    // ------ Settings
    new KAction(i18n("Configure..."), "configure", 0, this,
		SLOT(slotConfigure()), actionCollection(), "configure");

    // ------ Popup
    new KAction(i18n("Node Properties"), "node_properties", 0, this,
		SLOT(slotOpenNode()), actionCollection(), "node_properties");
    new KAction(i18n("Delete Task"), "editdelete", 0, this,
		SLOT(slotDeleteTask()), actionCollection(), "delete_task");

    new KAction(i18n("Edit resource"), "edit_resource", 0, this,
		SLOT(slotEditResource()), actionCollection(), "edit_resource");


    // ------------------- Actions with a key binding and no GUI item
#ifndef NDEBUG
    KAction* actPrintDebug = new KAction( i18n( "Print Debug" ), CTRL+SHIFT+Key_P,
                        this, SLOT( slotPrintDebug() ), actionCollection(), "print_debug" );
    KAction* actPrintCalendarDebug = new KAction( i18n( "Print Calendar Debug" ), CTRL+SHIFT+Key_C,
                        this, SLOT( slotPrintCalendarDebug() ), actionCollection(), "print_calendar_debug" );
#endif
    // Necessary for the actions that are not plugged anywhere
    // Deprecated with KDE-3.1.
    // Not entirely sure it's necessary for 3.0, please test and report.
#if KDE_VERSION < 305
    KAccel * accel = new KAccel( this );
#ifndef NDEBUG
    actPrintDebug->plugAccel( accel );
    actPrintCalendarDebug->plugAccel( accel );
#endif
#else
    // Stupid compilers ;)
#ifndef NDEBUG
    Q_UNUSED( actPrintDebug );
    Q_UNUSED( actPrintCalendarDebug );
#endif
#endif

}

KPTView::~KPTView()
{
  delete m_dcop;
}

DCOPObject * KPTView::dcopObject()
{
  if ( !m_dcop )
    m_dcop = new KPtViewIface( this );

  return m_dcop;
}


KPTProject& KPTView::getProject() const
{
	return getPart()->getProject();
}

void KPTView::setZoom(double zoom) {
    m_ganttview->zoom(zoom);
	m_pertview->zoom(zoom);
}

void KPTView::setupPrinter(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;

}

void KPTView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;
	if (m_tab->visibleWidget() == m_ganttview)
	{
        m_ganttview->print(printer);
    }
	else if (m_tab->visibleWidget() == m_pertview)
	{
        m_pertview->print(printer);
	}
	else if (m_tab->visibleWidget() == m_resourceview)
	{
        m_resourceview->print(printer);
	}
	else if (m_tab->visibleWidget() == m_reportview)
	{
        m_reportview->print(printer);
	}

}

void KPTView::slotEditCut() {
    //kdDebug()<<k_funcinfo<<endl;
}

void KPTView::slotEditCopy() {
    //kdDebug()<<k_funcinfo<<endl;
}

void KPTView::slotEditPaste() {
    //kdDebug()<<k_funcinfo<<endl;
}

void KPTView::slotViewGantt() {
    //kdDebug()<<k_funcinfo<<endl;
	m_ganttview->clear();
    m_tab->raiseWidget(m_ganttview);
	m_ganttview->draw(getPart()->getProject());
}

void KPTView::slotViewPert() {
    //kdDebug()<<k_funcinfo<<endl;
    m_tab->raiseWidget(m_pertview);
    m_pertview->draw();
}

void KPTView::slotViewResources() {
    //kdDebug()<<k_funcinfo<<endl;
    m_tab->raiseWidget(m_resourceview);
	m_resourceview->draw(getPart()->getProject());
}

void KPTView::slotProjectEdit() {
    KPTMainProjectDialog *dia = new KPTMainProjectDialog(getProject());
    if (dia->exec())
	    slotUpdate(true);

    delete dia;
}

void KPTView::slotProjectCalendar() {
    KPTCalendarListDialog *dia = new KPTCalendarListDialog(getProject());
    if (dia->exec()) {
        KMacroCommand *cmd = dia->buildCommand();
        if (cmd) {
            //kdDebug()<<k_funcinfo<<"Modifying calendar(s)"<<endl;
            getPart()->addCommand(cmd); //also executes
        }
    }
    delete dia;
}

void KPTView::slotProjectWorktime() {
    KPTStandardWorktimeDialog *dia = new KPTStandardWorktimeDialog(getProject());
    if (dia->exec()) {
        KMacroCommand *cmd = dia->buildCommand();
        if (cmd) {
            //kdDebug()<<k_funcinfo<<"Modifying calendar(s)"<<endl;
            getPart()->addCommand(cmd); //also executes
        }
    }
    delete dia;
}

void KPTView::slotProjectCalculate() {
    slotUpdate(true);
}

void KPTView::projectCalculate() {
    getPart()->getProject().calculate();
//    KPTDateTime *t = getPart()->getProject().getStartTime();
//    delete t;
}

void KPTView::slotReportDesign() {
    //kdDebug()<<k_funcinfo<<endl;
}

void KPTView::slotReportGenerate(int idx) {
    //kdDebug()<<k_funcinfo<<endl;
    m_tab->raiseWidget(m_reportview);
    QString *file = m_reportTemplateFiles.at(idx);
    if (file)
        m_reportview->draw(*file);

    actionReportGenerate->setCurrentItem(-1); //remove checkmark
}

void KPTView::slotAddSubTask() {
	// If we are positionend on the root project, then what we really want to
	// do is to add a first project. We will silently accept the challenge
	// and will not complain.
    KPTTask* node = new KPTTask(currentTask());
    KPTTaskDialog *dia = new KPTTaskDialog(*node);
    if (dia->exec()) {
		KPTNode *currNode = currentTask();
		if (currNode)
        {
            KMacroCommand *m = dia->buildCommand();
            m->execute(); // do changes to task
            delete m;
            KPTSubtaskAddCmd *cmd = new KPTSubtaskAddCmd(&(getProject()), node, currNode, i18n("Add subtask"));
            getPart()->addCommand(cmd); // add task to project
    		slotUpdate(true);
			return;
	    }
		else
		    kdDebug()<<k_funcinfo<<"Cannot insert new project. Hmm, no current node!?"<<endl;
	}
    delete node;
    delete dia;
}


void KPTView::slotAddTask() {
    KPTTask *node = new KPTTask(currentTask());
    KPTTaskDialog *dia = new KPTTaskDialog(*node);
    if (dia->exec()) {
		KPTNode* currNode = currentTask();
		if (currNode)
        {
            KMacroCommand *m = dia->buildCommand();
            m->execute(); // do changes to task
            delete m;
            KPTTaskAddCmd *cmd = new KPTTaskAddCmd(&(getProject()), node, currNode, i18n("Add task"));
            getPart()->addCommand(cmd); // add task to project
    		slotUpdate(true);
			return;
	    }
		else
		    kdDebug()<<k_funcinfo<<"Cannot insert new task. Hmm, no current node!?"<<endl;
	}
    delete node;
    delete dia;
}

void KPTView::slotAddMilestone() {
	KPTDuration zeroDuration(0);
    KPTTask* node = new KPTTask(currentTask());
	node->effort()->set( zeroDuration );

    //KPTMilestone *node = new KPTMilestone(currentTask());
    node->setName(i18n("Milestone"));

    KPTTaskDialog *dia = new KPTTaskDialog(*node);
    if (dia->exec()) {
		KPTNode *currNode = currentTask();
		if (currNode)
        {
            KMacroCommand *m = dia->buildCommand();
            m->execute(); // do changes to task
            delete m;
            KPTTaskAddCmd *cmd = new KPTTaskAddCmd(&(getProject()), node, currNode, i18n("Add milestone"));
            getPart()->addCommand(cmd); // add task to project
    		slotUpdate(true);
			return;
	    }
		else
		    kdDebug()<<k_funcinfo<<"Cannot insert new milestone. Hmm, no current node!?"<<endl;
	}
    delete node;
    delete dia;
}

 void KPTView::slotConfigure() {

}

KPTNode *KPTView::currentTask()
{
	KPTNode* task = 0;
	if (m_tab->visibleWidget() == m_ganttview) {
	    task = m_ganttview->currentNode();
	}
	else if (m_tab->visibleWidget() == m_pertview) {
		task = m_pertview->currentNode();
	}
	if ( 0 != task ) {
		return task;
	}
	return &(getProject());
}

void KPTView::slotOpenNode() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTNode *node = 0;
	if (m_tab->visibleWidget() == m_ganttview) {
        node = m_ganttview->currentNode();
	} else if (m_tab->visibleWidget() == m_pertview) {
	    node = m_pertview->currentNode();
	}
    if (!node)
        return;

    switch (node->type()) {
        case KPTNode::Type_Project: {
            KPTProject *project = dynamic_cast<KPTProject *>(node);
            KPTProjectDialog *dia = new KPTProjectDialog(*project);
            if (dia->exec())
                slotUpdate(true);
            delete dia;
            break;
        }
        case KPTNode::Type_Subproject:
            //TODO
            break;
        case KPTNode::Type_Task: {
            KPTTask *task = dynamic_cast<KPTTask *>(node);
            KPTTaskDialog *dia = new KPTTaskDialog(*task);
            if (dia->exec()) {
                KMacroCommand *m = dia->buildCommand();
                if (m) {
                    getPart()->addCommand(m);
                    slotUpdate(true);
                }
            }
            delete dia;
            break;
        }
        case KPTNode::Type_Milestone: {
            // Use the normal task dialog for now.
            // Maybe milestone should have it's own dialog, but we need to be able to
            // enter a duration in case we accidentally set a tasks duration to zero
            // and hence, create a milestone
            KPTTask *task = dynamic_cast<KPTTask *>(node);
            KPTTaskDialog *dia = new KPTTaskDialog(*task);
            if (dia->exec())
                slotUpdate(true);
            delete dia;
            break;
        }
        case KPTNode::Type_Summarytask: {
            // TODO
            break;
        }
        default:
            break; // avoid warnings
    }
}

void KPTView::slotDeleteTask()
{
    //kdDebug()<<k_funcinfo<<endl;

	KPTNode* task = currentTask();

    KPTNodeDeleteCmd *cmd = new KPTNodeDeleteCmd(task, i18n("Delete task"));
    getPart()->addCommand(cmd);

	// display the changes
	slotUpdate(true);
}

void KPTView::slotIndentTask()
{
    //kdDebug()<<k_funcinfo<<endl;

	KPTNode* task = currentTask();

	// tell the model to do the work for us
	getProject().indentTask( task );

	// display the changes
	slotUpdate(true);
}

void KPTView::slotUnindentTask()
{
    //kdDebug()<<k_funcinfo<<endl;

	KPTNode* task = currentTask();

	// tell the model to do the work for us
	getProject().unindentTask( task );

	// display the changes
	slotUpdate(true);
}

void KPTView::slotMoveTaskUp()
{
    //kdDebug()<<k_funcinfo<<endl;

	KPTNode* task = currentTask();
	if ( 0 == task ) {
		// is always != 0. At least we would get the KPTProject, but you never know who might change that
		// so better be careful
		return;
	}

	if ( KPTNode::Type_Project == task->type() ) {
		kdDebug()<<k_funcinfo<<"The root node cannot be moved up"<<endl;
		return;
	}
	// tell the model to do the work for us
	getProject().moveTaskUp( task );

	// display the changes
	slotUpdate(true);
}

void KPTView::slotMoveTaskDown()
{
    //kdDebug()<<k_funcinfo<<endl;

	KPTNode* task = currentTask();
	if ( 0 == task ) {
		// is always != 0. At least we would get the KPTProject, but you never know who might change that
		// so better be careful
		return;
	}

	if ( KPTNode::Type_Project == task->type() ) {
		kdDebug()<<k_funcinfo<<"The root node cannot be moved down"<<endl;
		return;
	}
	// tell the model to do the work for us
	getProject().moveTaskDown( task );

	// display the changes
	slotUpdate(true);
}

void KPTView::slotEditResource() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTResource *r = m_resourceview->currentResource();
    if (!r)
        return;
    KPTResourceDialog *dia = new KPTResourceDialog(getProject(), *r);
    if (dia->exec())
        slotUpdate(true); //FIXME: just refresh the view
    delete dia;
}

void KPTView::updateReadWrite(bool /*readwrite*/) {
}

KPTPart *KPTView::getPart()const {
    return (KPTPart *)koDocument();
}

void KPTView::slotConnectNode() {
    //kdDebug()<<k_funcinfo<<endl;
/*    KPTNodeItem *curr = m_ganttview->currentItem();
    if (curr) {
        kdDebug()<<k_funcinfo<<"node="<<curr->getNode().name()<<endl;
    }*/
}

QPopupMenu * KPTView::popupMenu( const QString& name )
{
    //kdDebug()<<k_funcinfo<<endl;
    Q_ASSERT(factory());
    if ( factory() )
        return ((QPopupMenu*)factory()->container( name, this ));
    return 0L;
}

void KPTView::slotChanged(QWidget *)
{
    //kdDebug()<<k_funcinfo<<endl;
    slotUpdate(false);
}

void KPTView::slotChanged()
{
    //kdDebug()<<k_funcinfo<<endl;
    slotUpdate(false);
}

void KPTView::slotUpdate(bool calculate)
{
    //kdDebug()<<k_funcinfo<<"calculate="<<calculate<<endl;
    if (calculate)
	    projectCalculate();

	if (m_tab->visibleWidget() == m_ganttview)
	{
	    //m_ganttview->hide();
    	m_ganttview->draw(getPart()->getProject());
	    m_ganttview->show();
	}
	else if (m_tab->visibleWidget() == m_pertview)
	{
    	m_pertview->draw();
	    m_pertview->show();
	}
	else if (m_tab->visibleWidget() == m_resourceview)
	{
    	m_resourceview->draw(getPart()->getProject());
	    m_resourceview->show();
	}
	else if (m_tab->visibleWidget() == m_reportview)
	{
	}
}

//FIXME: We need a solution that takes care project specific reports.
void KPTView::setReportGenerateMenu() {
    kdDebug()<<k_funcinfo<<endl;
    QStringList list;
    m_reportTemplateFiles.clear();
    KStandardDirs std;
    QStringList reportDesktopFiles = std.findAllResources("data", "kplato/reports/*.desktop", true, true);
    for (QStringList::iterator it = reportDesktopFiles.begin(); it != reportDesktopFiles.end(); ++it) {
        KDesktopFile file((*it), true);
        QString name = file.readName();
        if (!name.isNull()) {
            list.append(name);
            kdDebug()<<" file: "<<*it<<" name="<<name<<endl;
            QString *url = new QString(file.readURL());
            if (url->isNull()) {
                delete url;
            } else {
                if (url[0] != "/" || url->left(6) != "file:/") {
                    QString path = (*it).left((*it).findRev('/', -1)+1); // include '/'
                    *url = path + *url;
                }

                m_reportTemplateFiles.append(url);
            }
        }
    }
    actionReportGenerate->setItems(list);
}

void KPTView::slotAboutToShow(QWidget *widget) {
	if (widget == m_ganttview)
	{
        //kdDebug()<<k_funcinfo<<"draw gantt"<<endl;
	    //m_ganttview->hide();
    	m_ganttview->draw(getPart()->getProject());
	    m_ganttview->show();
	}
	else if (widget == m_pertview)
	{
        //kdDebug()<<k_funcinfo<<"draw pertview"<<endl;
    	m_pertview->draw();
	    m_pertview->show();
	}
	else if (widget == m_resourceview)
	{
        //kdDebug()<<k_funcinfo<<"draw resourceview"<<endl;
    	m_resourceview->draw(getPart()->getProject());
	    m_resourceview->show();
	}
	else if (widget == m_reportview)
	{
        //kdDebug()<<k_funcinfo<<"draw reportview"<<endl;
	}

}

#ifndef NDEBUG
void KPTView::slotPrintDebug() {
    kdDebug()<<"-------- Debug printout: Node list" <<endl;
/*    KPTNode *curr = m_ganttview->currentNode();
    if (curr) {
        curr->printDebug(true,"");
    } else*/
        getPart()->getProject().printDebug(true, "");
}
void KPTView::slotPrintCalendarDebug() {
    kdDebug()<<"-------- Debug printout: Node list" <<endl;
/*    KPTNode *curr = m_ganttview->currentNode();
    if (curr) {
        curr->printDebug(true,"");
    } else*/
        getPart()->getProject().printCalendarDebug("");
}
#endif
#include "kptview.moc"
