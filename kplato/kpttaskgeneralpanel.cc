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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpttaskgeneralpanel.h"
#include "kpttaskdialog.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptduration.h"
#include "kptdurationwidget.h"
#include "kptcalendar.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <klocale.h>
#include <kcommand.h>

#include <qlayout.h>
#include <qdatetime.h> 
#include <kdebug.h>

namespace KPlato
{

KPTTaskGeneralPanel::KPTTaskGeneralPanel(KPTTask &task, KPTStandardWorktime *workTime, QWidget *p, const char *n)
    : KPTTaskGeneralPanelBase(p, n),
      m_task(task)
{
    setStartValues(task, workTime);
    namefield->setFocus();
}

void KPTTaskGeneralPanel::setStartValues(KPTTask &task, KPTStandardWorktime *workTime) {
    namefield->setText(task.name());
    leaderfield->setText(task.leader());
    descriptionfield->setText(task.description());
    idfield->setText(task.id());
    setSchedulingType(task.constraint());
    if (task.constraintStartTime().isValid()) {
        setStartTime(task.constraintStartTime());
    } else {
        QDate date = QDate::currentDate();
        QTime time = workTime ? workTime->startOfDay(date.dayOfWeek()-1) : QTime::currentTime();
        setStartTime(QDateTime(date, time)); 
    }
    if (task.constraintEndTime().isValid()) {
        setEndTime(task.constraintEndTime());
    } else {
        QDate date = QDate::currentDate();
        QTime time = workTime ? workTime->endOfDay(date.dayOfWeek()-1) : QTime::currentTime();
        setEndTime(QDateTime(date, time)); 
    }    
    setEstimateType(task.effort()->type());
    setEstimateFields(KPTDurationWidget::Days|KPTDurationWidget::Hours|KPTDurationWidget::Minutes);
    if (workTime) {
        setEstimateScales(workTime->durationDay().hours());
    }
    setEstimateFieldUnit(0, i18n("days"));
    setEstimateFieldUnit(1, i18n("hours"));
    setEstimateFieldUnit(2, i18n("minutes"));
    
    setEstimate(task.effort()->expected()); 
    setOptimistic(task.effort()->optimisticRatio());
    setPessimistic(task.effort()->pessimisticRatio());
}

KMacroCommand *KPTTaskGeneralPanel::buildCommand(KPTPart *part) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Task"));
    bool modified = false;

    KPTDuration dt = KPTDuration();

    if (!namefield->isHidden() && m_task.name() != namefield->text()) {
        cmd->addCommand(new KPTNodeModifyNameCmd(part, m_task, namefield->text()));
        modified = true;
    }
    if (!leaderfield->isHidden() && m_task.leader() != leaderfield->text()) {
        cmd->addCommand(new KPTNodeModifyLeaderCmd(part, m_task, leaderfield->text()));
        modified = true;
    }
    if (!descriptionfield->isHidden() && 
        m_task.description() != descriptionfield->text()) {
        cmd->addCommand(new KPTNodeModifyDescriptionCmd(part, m_task, descriptionfield->text()));
        modified = true;
    }
    KPTNode::ConstraintType c = (KPTNode::ConstraintType)schedulingType();
    if (c != m_task.constraint()) {
        cmd->addCommand(new KPTNodeModifyConstraintCmd(part, m_task, c));
        modified = true;
    }
    if (startTime() != m_task.constraintStartTime() &&
        (c == KPTNode::FixedInterval || c == KPTNode::StartNotEarlier || c == KPTNode::MustStartOn)) {
        cmd->addCommand(new KPTNodeModifyConstraintStartTimeCmd(part, m_task, startTime()));
        modified = true;
    }
    if (endTime() != m_task.constraintEndTime() &&
        (c == KPTNode::FinishNotLater || c == KPTNode::FixedInterval || c == KPTNode::MustFinishOn)) {
        cmd->addCommand(new KPTNodeModifyConstraintEndTimeCmd(part, m_task, endTime()));
        modified = true;
    }
    if (!idfield->isHidden() && idfield->text() != m_task.id()) {
        
        cmd->addCommand(new KPTNodeModifyIdCmd(part, m_task, idfield->text()));
        modified = true;
    }
    int et = estimationType();
    if (et != m_task.effort()->type()) {
        cmd->addCommand(new KPTModifyEffortTypeCmd(part, m_task.effort(),  m_task.effort()->type(), et));
        modified = true;
    }
    dt = estimationValue();
    bool expchanged = dt != m_task.effort()->expected();
    if ( expchanged ) {
        cmd->addCommand(new KPTModifyEffortCmd(part, m_task.effort(), m_task.effort()->expected(), dt));
        modified = true;
    }
    int x = optimistic();
    if ( x != m_task.effort()->optimisticRatio() || expchanged) {
        cmd->addCommand(new KPTEffortModifyOptimisticRatioCmd(part, m_task.effort(), m_task.effort()->optimisticRatio(), x));
        modified = true;
    }
    x = pessimistic();
    if ( x != m_task.effort()->pessimisticRatio() || expchanged) {
        cmd->addCommand(new KPTEffortModifyPessimisticRatioCmd(part, m_task.effort(), m_task.effort()->pessimisticRatio(), x));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool KPTTaskGeneralPanel::ok() {
    if (idfield->text() != m_task.id() && KPTNode::find(idfield->text())) {
        KMessageBox::sorry(this, i18n("Task id must be unique"));
        idfield->setFocus();
        return false;
    }
    return true;
}


}  //KPlato namespace

#include "kpttaskgeneralpanel.moc"
