/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskdialog.h"
#include "kpttask.h"
#include "kpttaskgeneralpanelbase.h"
#include "kptrequestresourcespanel.h"
#include "kptcommand.h"
#include "kptdurationwidget.h"
#include "kptduration.h"
#include "kptcalendar.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdatepicker.h>
#include <kcommand.h>
#include <kmessagebox.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <knuminput.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qtabwidget.h>
#include <qtable.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <kdebug.h>

namespace KPlato
{

KPTTaskDialog::KPTTaskDialog(KPTTask &task, KPTStandardWorktime *workTime, QWidget *p, const char *n)
    : KDialogBase(Tabbed, i18n("Task Settings"), Ok|Cancel, Ok, p, n, true, true),
      m_task(task)
{
    QFrame *page;
    QVBoxLayout *topLayout;

    // Create all the tabs.
    page = addPage(i18n("&General"));
    topLayout = new QVBoxLayout(page, 0, spacingHint());
    m_generalTab = new KPTTaskGeneralPanelBase(page);
    topLayout->addWidget(m_generalTab);

    page = addPage(i18n("&Resources"));
    topLayout = new QVBoxLayout(page, 0, spacingHint());
    m_resourcesTab = new KPTRequestResourcesPanel(page, task);
    topLayout->addWidget(m_resourcesTab);

    // Create some shortcuts.
    m_name = m_generalTab->namefield;
    m_leader = m_generalTab->leaderfield;
    m_description = m_generalTab->descriptionfield;
    m_id = m_generalTab->idfield;

    // Set the state of all the child widgets.
    enableButtonOK(false);
    m_name->setText(task.name());
    m_leader->setText(task.leader());
    m_description->setText(task.description());
    m_id->setText(task.id());
    m_generalTab->setSchedulingType(task.constraint());
    if (task.constraintStartTime().isValid()) {
        kdDebug()<<k_funcinfo<<task.constraintStartTime().toString()<<endl;
        m_generalTab->setStartTime(task.constraintStartTime());
    } else {
        QDate date = QDate::currentDate();
        QTime time = workTime ? workTime->startOfDay(date.dayOfWeek()-1) : QTime::currentTime();
        m_generalTab->setStartTime(QDateTime(date, time)); 
    }
    if (task.constraintEndTime().isValid()) {
        kdDebug()<<k_funcinfo<<task.constraintEndTime().toString()<<endl;
        m_generalTab->setEndTime(task.constraintEndTime());
    } else {
        QDate date = QDate::currentDate();
        QTime time = workTime ? workTime->endOfDay(date.dayOfWeek()-1) : QTime::currentTime();
        m_generalTab->setEndTime(QDateTime(date, time)); 
    }    
    m_generalTab->setEstimateType(task.effort()->type());
    m_generalTab->setEstimateFields(KPTDurationWidget::Days|KPTDurationWidget::Hours|KPTDurationWidget::Minutes);
    if (workTime) {
        m_generalTab->setEstimateScales(workTime->durationDay().hours());
    }
    m_generalTab->setEstimateFieldUnit(0, i18n("days"));
    m_generalTab->setEstimateFieldUnit(1, i18n("hours"));
    m_generalTab->setEstimateFieldUnit(2, i18n("minutes"));
    
    m_generalTab->setEstimate(task.effort()->expected()); 
    m_generalTab->setOptimistic(task.effort()->optimisticRatio());
    m_generalTab->setPessimistic(task.effort()->pessimisticRatio());
    
    connect(m_generalTab, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOK(bool) ));
    connect(m_resourcesTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
    m_name->setFocus();
    
}


KMacroCommand *KPTTaskDialog::buildCommand(KPTPart *part) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Task"));
    bool modified = false;

    KPTDuration dt = KPTDuration();

    if (m_task.name() != m_name->text()) {
        cmd->addCommand(new KPTNodeModifyNameCmd(part, m_task, m_name->text()));
        modified = true;
    }
    if (m_task.leader() != m_leader->text()) {
        cmd->addCommand(new KPTNodeModifyLeaderCmd(part, m_task, m_leader->text()));
        modified = true;
    }
    if (m_task.description() != m_description->text()) {
        cmd->addCommand(new KPTNodeModifyDescriptionCmd(part, m_task, m_description->text()));
        modified = true;
    }
    KPTNode::ConstraintType c = (KPTNode::ConstraintType)m_generalTab->schedulingType();
    if (c != m_task.constraint()) {
        cmd->addCommand(new KPTNodeModifyConstraintCmd(part, m_task, c));
        modified = true;
    }
    if (m_generalTab->startTime() != m_task.constraintStartTime() &&
        (c == KPTNode::FixedInterval || c == KPTNode::StartNotEarlier || c == KPTNode::MustStartOn)) {
        cmd->addCommand(new KPTNodeModifyConstraintStartTimeCmd(part, m_task, m_generalTab->startTime()));
        modified = true;
    }
    if (m_generalTab->endTime() != m_task.constraintEndTime() &&
        (c == KPTNode::FinishNotLater || c == KPTNode::FixedInterval || c == KPTNode::MustFinishOn)) {
        cmd->addCommand(new KPTNodeModifyConstraintEndTimeCmd(part, m_task, m_generalTab->endTime()));
        modified = true;
    }
    if (m_id->text() != m_task.id()) {
        
        cmd->addCommand(new KPTNodeModifyIdCmd(part, m_task, m_id->text()));
        modified = true;
    }
    int et = m_generalTab->estimationType();
    if (et != m_task.effort()->type()) {
        cmd->addCommand(new KPTModifyEffortTypeCmd(part, m_task.effort(),  m_task.effort()->type(), et));
        modified = true;
    }
    dt = m_generalTab->estimationValue();
    bool expchanged = dt != m_task.effort()->expected();
    if ( expchanged ) {
        cmd->addCommand(new KPTModifyEffortCmd(part, m_task.effort(), m_task.effort()->expected(), dt));
        modified = true;
    }
    int x = m_generalTab->optimistic();
    if ( x != m_task.effort()->optimisticRatio() || expchanged) {
        cmd->addCommand(new KPTEffortModifyOptimisticRatioCmd(part, m_task.effort(), m_task.effort()->optimisticRatio(), x));
        modified = true;
    }
    x = m_generalTab->pessimistic();
    if ( x != m_task.effort()->pessimisticRatio() || expchanged) {
        cmd->addCommand(new KPTEffortModifyPessimisticRatioCmd(part, m_task.effort(), m_task.effort()->pessimisticRatio(), x));
        modified = true;
    }
    KCommand *m = m_resourcesTab->buildCommand(part);
    if (m) {
        //kdDebug()<<k_funcinfo<<"ResourceTab modified"<<endl;
        cmd->addCommand(m);
        modified = true;
    }
    if (modified)
        return cmd;

    delete cmd;
    return 0;
}

void KPTTaskDialog::slotOk() {
    if (m_id->text() != m_task.id() && KPTNode::find(m_id->text())) {
        KMessageBox::sorry(this, "Task id must be unique");
        m_id->setFocus();
        return;
    }
    m_resourcesTab->slotOk();
    accept();
}


}  //KPlato namespace

#include "kpttaskdialog.moc"
