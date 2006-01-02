/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kpttaskgeneralpanel.h"
#include "kpttaskdialog.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptduration.h"
#include "kptdurationwidget.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptconfig.h"
#include "kptpart.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <kdatetimewidget.h>
#include <klocale.h>
#include <kcommand.h>

#include <qlayout.h>
#include <qdatetime.h> 
#include <qdatetimeedit.h> 
#include <qgroupbox.h> 
#include <kdebug.h>

namespace KPlato
{

TaskGeneralPanel::TaskGeneralPanel(Task &task, StandardWorktime *workTime, bool baseline, QWidget *p, const char *n)
    : TaskGeneralPanelBase(p, n),
      m_task(task),
      m_dayLength(24)
{
    setStartValues(task, workTime);
/*  Why is this done?  Its useless (its not actually read only, but that may be a Qt thing) and I have to
    edit these to actually be able to OK the dialog.   TZ-8-2005
    namefield->setReadOnly(baseline);
    leaderfield->setReadOnly(baseline);
    idfield->setReadOnly(baseline);
    schedulingGroup->setEnabled(!baseline);
*/
}

void TaskGeneralPanel::setStartValues(Task &task, StandardWorktime *workTime) {
    m_effort = m_duration = task.effort()->expected();
    namefield->setText(task.name());
    leaderfield->setText(task.leader());
    descriptionfield->setText(task.description());
    idfield->setText(task.id());
    wbsfield->setText(task.wbs());
    
    setEstimateFields(DurationWidget::Days|DurationWidget::Hours|DurationWidget::Minutes);
    if (workTime) {
        kdDebug()<<k_funcinfo<<"daylength="<<workTime->durationDay().hours()<<endl;
        m_dayLength = workTime->durationDay().hours();
        setEstimateScales(m_dayLength);
    }
    setEstimateFieldUnit(0, i18n("days"));
    setEstimateFieldUnit(1, i18n("hours"));
    setEstimateFieldUnit(2, i18n("minutes"));
    setEstimateType(task.effort()->type());
    
    setSchedulingType(task.constraint());
    if (task.constraintStartTime().isValid()) {
        setStartDateTime(task.constraintStartTime());
    } else {
        QDate date = QDate::currentDate();
        QTime time = workTime ? workTime->startOfDay(date.dayOfWeek()-1) : QTime::currentTime();
        setStartDateTime(QDateTime(date, time)); 
    }
    if (task.constraintEndTime().isValid()) {
        setEndDateTime(task.constraintEndTime());
    } else {
        QDate date = QDate::currentDate();
        QTime time = workTime ? workTime->endOfDay(date.dayOfWeek()-1) : QTime::currentTime();
        setEndDateTime(QDateTime(date, time)); 
    }
    kdDebug()<<k_funcinfo<<"Effort: "<<task.effort()->expected().toString()<<endl;
    setEstimate(task.effort()->expected()); 
    setOptimistic(task.effort()->optimisticRatio());
    setPessimistic(task.effort()->pessimisticRatio());
}

KMacroCommand *TaskGeneralPanel::buildCommand(Part *part) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Task"));
    bool modified = false;

    Duration dt = Duration();

    if (!namefield->isHidden() && m_task.name() != namefield->text()) {
        cmd->addCommand(new NodeModifyNameCmd(part, m_task, namefield->text()));
        modified = true;
    }
    if (!leaderfield->isHidden() && m_task.leader() != leaderfield->text()) {
        cmd->addCommand(new NodeModifyLeaderCmd(part, m_task, leaderfield->text()));
        modified = true;
    }
    if (!descriptionfield->isHidden() && 
        m_task.description() != descriptionfield->text()) {
        cmd->addCommand(new NodeModifyDescriptionCmd(part, m_task, descriptionfield->text()));
        modified = true;
    }
    Node::ConstraintType c = (Node::ConstraintType)schedulingType();
    if (c != m_task.constraint()) {
        cmd->addCommand(new NodeModifyConstraintCmd(part, m_task, c));
        modified = true;
    }
    if (startDateTime() != m_task.constraintStartTime() &&
        (c == Node::FixedInterval || c == Node::StartNotEarlier || c == Node::MustStartOn)) {
        cmd->addCommand(new NodeModifyConstraintStartTimeCmd(part, m_task, startDateTime()));
        modified = true;
    }
    if (endDateTime() != m_task.constraintEndTime() &&
        (c == Node::FinishNotLater || c == Node::FixedInterval || c == Node::MustFinishOn)) {
        cmd->addCommand(new NodeModifyConstraintEndTimeCmd(part, m_task, endDateTime()));
        modified = true;
    }
    if (!idfield->isHidden() && idfield->text() != m_task.id()) {
        
        cmd->addCommand(new NodeModifyIdCmd(part, m_task, idfield->text()));
        modified = true;
    }
    int et = estimationType();
    if (et != m_task.effort()->type()) {
        cmd->addCommand(new ModifyEffortTypeCmd(part, m_task.effort(),  m_task.effort()->type(), et));
        modified = true;
    }
    dt = estimationValue();
    kdDebug()<<k_funcinfo<<"Estimate: "<<dt.toString()<<endl;
    bool expchanged = dt != m_task.effort()->expected();
    if ( expchanged ) {
        cmd->addCommand(new ModifyEffortCmd(part, m_task.effort(), m_task.effort()->expected(), dt));
        modified = true;
    }
    int x = optimistic();
    if ( x != m_task.effort()->optimisticRatio() || expchanged) {
        cmd->addCommand(new EffortModifyOptimisticRatioCmd(part, m_task.effort(), m_task.effort()->optimisticRatio(), x));
        modified = true;
    }
    x = pessimistic();
    if ( x != m_task.effort()->pessimisticRatio() || expchanged) {
        cmd->addCommand(new EffortModifyPessimisticRatioCmd(part, m_task.effort(), m_task.effort()->pessimisticRatio(), x));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool TaskGeneralPanel::ok() {
    if (idfield->text() != m_task.id() && m_task.findNode(idfield->text())) {
        KMessageBox::sorry(this, i18n("Task id must be unique"));
        idfield->setFocus();
        return false;
    }
    return true;
}

void TaskGeneralPanel::estimationTypeChanged(int type) {
    if (scheduleType->currentItem() == 6 /* Fixed interval */) {
        if (type == 0 /*Effort*/) {
            setEstimateScales(m_dayLength);
            estimate->setEnabled(true);
            setEstimate(m_effort);
        } else {
            setEstimateScales(24);
            estimate->setEnabled(false);
            DateTime st = startDateTime();
            DateTime end = endDateTime();
            m_duration = end - st;
            estimate->setValue(m_duration);
        }
        return;
    }
    TaskGeneralPanelBase::estimationTypeChanged(type);
}

void TaskGeneralPanel::scheduleTypeChanged(int value)
{
    if (value == 6 /*Fixed interval*/) { 
        if (estimateType->currentItem() == 1/*duration*/){
            setEstimateScales(24);
            estimate->setEnabled(false);
        }
    } else {
        setEstimateScales(m_dayLength);
        estimate->setEnabled(true);
    }
    TaskGeneralPanelBase::scheduleTypeChanged(value);
}



}  //KPlato namespace

#include "kpttaskgeneralpanel.moc"
