/* This file is part of the KDE project
   Copyright (C) 2004 - 2006 Dag Andersen <danders@get2net.dk>

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
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>
#include <kdatewidget.h>

#include <QLayout>
#include <QLabel>
#include <qdatetime.h>
#include <q3datetimeedit.h>
#include <q3groupbox.h>
#include <QPushButton>
#include <QSpinBox>

#include <kdebug.h>

namespace KPlato
{

TaskGeneralPanel::TaskGeneralPanel(Task &task, StandardWorktime *workTime, bool /*baseline*/, QWidget *p, const char *n)
    : TaskGeneralPanelImpl(p, n),
      m_task(task),
      m_dayLength(24)
{
    useTime = true;
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
        //kDebug()<<k_funcinfo<<"daylength="<<workTime->day()<<endl;
        m_dayLength = workTime->day();
        if (task.effort()->type() == Effort::Type_Effort) {
            setEstimateScales(m_dayLength);
        }
    }
    setEstimateFieldUnit(0, i18nc("days", "d"));
    setEstimateFieldUnit(1, i18nc("hours", "h"));
    setEstimateFieldUnit(2, i18nc("minutes", "m"));
    setEstimateType(task.effort()->type());

    setSchedulingType(task.constraint());
    if (task.constraintStartTime().isValid()) {
        setStartDateTime(task.constraintStartTime());
    } else {
        QDate date = QDate::currentDate();
        setStartDateTime(QDateTime(date, QTime()));
    }
    if (task.constraintEndTime().isValid()) {
        setEndDateTime(task.constraintEndTime());
    } else {
        setEndDateTime(QDateTime(startDate().addDays(1), QTime()));
    }
    //kDebug()<<k_funcinfo<<"Effort: "<<task.effort()->expected().toString()<<endl;
    setEstimate(task.effort()->expected());
    setOptimistic(task.effort()->optimisticRatio());
    setPessimistic(task.effort()->pessimisticRatio());

    namefield->setFocus();
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
    kDebug()<<k_funcinfo<<"Estimate: "<<dt.toString()<<endl;
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
    if (type == 0 /*Effort*/) {
        Duration d = estimationValue();
        setEstimateScales(m_dayLength);
        //setEstimate(d);
        estimate->setEnabled(true);
    } else {
        Duration d = estimationValue();
        setEstimateScales(24);
        //setEstimate(d);
        if (schedulingType() == 6) { /*Fixed interval*/
            estimate->setEnabled(false);
        } else {
            estimate->setEnabled(true);
        }

    }
    TaskGeneralPanelImpl::estimationTypeChanged(type);
}

void TaskGeneralPanel::scheduleTypeChanged(int value)
{
    if (value == 6 /*Fixed interval*/) {
        if (estimateType->currentIndex() == 1/*duration*/){
            setEstimateScales(24);
            estimate->setEnabled(false);
            setEstimate(DateTime(endDateTime()) - DateTime(startDateTime()));
        }
    } else {
        setEstimateScales(m_dayLength);
        estimate->setEnabled(true);
    }
    TaskGeneralPanelImpl::scheduleTypeChanged(value);
}

//-----------------------------
TaskGeneralPanelImpl::TaskGeneralPanelImpl(QWidget *p, const char *n)
    : TaskGeneralPanelBase(p, n) {

    connect(idfield, SIGNAL(textChanged(const QString &)), SLOT(checkAllFieldsFilled()));
    connect(namefield, SIGNAL(textChanged(const QString &)), SLOT(checkAllFieldsFilled()));
    connect(leaderfield, SIGNAL(textChanged(const QString &)), SLOT(checkAllFieldsFilled()));
    connect(chooseLeader, SIGNAL(clicked()), SLOT(changeLeader()));
    connect(estimateType, SIGNAL(activated(int)), SLOT(estimationTypeChanged(int)));
    connect(scheduleType, SIGNAL(activated(int)), SLOT(scheduleTypeChanged(int)));
    connect(scheduleStartDate, SIGNAL(changed(QDate)), SLOT(startDateChanged()));
    connect(scheduleStartTime, SIGNAL(valueChanged(const QTime&)), SLOT(startTimeChanged(const QTime&)));
    connect(scheduleEndDate, SIGNAL(changed(QDate)), SLOT(endDateChanged()));
    connect(scheduleEndTime, SIGNAL(valueChanged(const QTime&)), SLOT(endTimeChanged(const QTime&)));
    connect(estimate, SIGNAL(valueChanged()), SLOT(checkAllFieldsFilled()));
    connect(optimisticValue, SIGNAL(valueChanged(int)), SLOT(checkAllFieldsFilled()));
    connect(pessimisticValue, SIGNAL(valueChanged(int)), SLOT(checkAllFieldsFilled()));
    connect(descriptionfield, SIGNAL(textChanged()), SLOT(checkAllFieldsFilled()));
}

void TaskGeneralPanelImpl::setSchedulingType(int type)
{
    enableDateTime(type);
    scheduleType->setCurrentIndex(type);
    emit schedulingTypeChanged(type);
}

int TaskGeneralPanelImpl::schedulingType() const
{
    return scheduleType->currentIndex();
}

void TaskGeneralPanelImpl::changeLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        leaderfield->setText(a.fullEmail());
    }
}

void TaskGeneralPanelImpl::setEstimationType( int type )
{
    estimateType->setCurrentIndex(type);
}

int TaskGeneralPanelImpl::estimationType() const
{
    return estimateType->currentIndex();
}

void TaskGeneralPanelImpl::setOptimistic( int value )
{
    optimisticValue->setValue(value);
}

void TaskGeneralPanelImpl::setPessimistic( int value )
{
    pessimisticValue->setValue(value);
}

int TaskGeneralPanelImpl::optimistic() const
{
    return optimisticValue->value();
}

int TaskGeneralPanelImpl::pessimistic()
{
    return pessimisticValue->value();
}

void TaskGeneralPanelImpl::enableDateTime( int scheduleType )
{
    scheduleStartTime->setEnabled(false);
    scheduleEndTime->setEnabled(false);
    scheduleStartDate->setEnabled(false);
    scheduleEndDate->setEnabled(false);
    switch (scheduleType)
    {
    case 0: //ASAP
    case 1: //ALAP
        break;
    case 2: //Must start on
    case 4: // Start not earlier
        if (useTime) {
            scheduleStartTime->setEnabled(true);
            scheduleEndTime->setEnabled(false);
        }
        scheduleStartDate->setEnabled(true);
        scheduleEndDate->setEnabled(false);
        break;
    case 3: //Must finish on
    case 5: // Finish not later
        if (useTime) {
            scheduleStartTime->setEnabled(false);
            scheduleEndTime->setEnabled(true);
        }
        scheduleStartDate->setEnabled(false);
        scheduleEndDate->setEnabled(true);
        break;
    case 6: //Fixed interval
        if (useTime) {
            scheduleStartTime->setEnabled(true);
            scheduleEndTime->setEnabled(true);
        }
        scheduleStartDate->setEnabled(true);
        scheduleEndDate->setEnabled(true);
        break;
    default:
        break;
    }
}


void TaskGeneralPanelImpl::estimationTypeChanged( int /*type*/ )
{
    checkAllFieldsFilled();
}



void TaskGeneralPanelImpl::setEstimate( const Duration & duration)
{
    estimate->setValue( duration );
}


void TaskGeneralPanelImpl::setEstimateType( int type)
{
    estimateType->setCurrentIndex(type);
}


void TaskGeneralPanelImpl::checkAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty());
}


Duration TaskGeneralPanelImpl::estimationValue()
{
    return estimate->value();
}


void TaskGeneralPanelImpl::setEstimateFields( int mask )
{
    estimate->setVisibleFields(mask);
}


void TaskGeneralPanelImpl::setEstimateScales( double day )
{
    estimate->setFieldScale(0, day);
    estimate->setFieldRightscale(0, day);

    estimate->setFieldLeftscale(1, day);
}


void TaskGeneralPanelImpl::setEstimateFieldUnit( int field, QString unit )
{
    estimate->setFieldUnit(field, unit);
}

void TaskGeneralPanelImpl::startDateChanged()
{
    if (!scheduleStartDate->isEnabled()) {
        return;
    }
    QDate date = startDate();
    if (startDateTime() > endDateTime())
    {
        scheduleEndTime->blockSignals(true);
        scheduleEndDate->blockSignals(true);
        setEndDate(date);
        setEndTime(startTime());
        scheduleEndTime->blockSignals(false);
        scheduleEndDate->blockSignals(false);
    }
    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}

void TaskGeneralPanelImpl::startTimeChanged( const QTime &time )
{
    if (!scheduleStartTime->isEnabled()) {
        return;
    }
    if (startDateTime() > endDateTime())
    {
        scheduleEndTime->blockSignals(true);
        setEndTime(time);
        scheduleEndTime->blockSignals(false);
    }
    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}


void TaskGeneralPanelImpl::endDateChanged()
{
    if (!scheduleEndDate->isEnabled()) {
        return;
    }
    QDate date = endDate();
    if (endDateTime() < startDateTime())
    {
        scheduleStartTime->blockSignals(true);
        scheduleStartDate->blockSignals(true);
        setStartDate(date);
        setStartTime(endTime());
        scheduleStartTime->blockSignals(false);
        scheduleStartDate->blockSignals(false);
    }

    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}

void TaskGeneralPanelImpl::endTimeChanged( const QTime &time )
{
    if (!scheduleEndTime->isEnabled()) {
        return;
    }
    if (endDateTime() < startDateTime())
    {
        scheduleStartTime->blockSignals(true);
        setStartTime(time);
        scheduleStartTime->blockSignals(false);
    }

    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}

void TaskGeneralPanelImpl::scheduleTypeChanged( int value )
{
     estimationTypeChanged(estimateType->currentIndex());
     enableDateTime(value);
     checkAllFieldsFilled();
}


QDateTime TaskGeneralPanelImpl::startDateTime()
{
    return QDateTime(startDate(), startTime());
}


QDateTime TaskGeneralPanelImpl::endDateTime()
{
    return QDateTime(endDate(), endTime());
}

void TaskGeneralPanelImpl::setStartTime( const QTime &time )
{
    scheduleStartTime->setTime(time);
}

void TaskGeneralPanelImpl::setEndTime( const QTime &time )
{
    scheduleEndTime->setTime(time);
}

QTime TaskGeneralPanelImpl::startTime() const
{
    return scheduleStartTime->time();
}

QTime TaskGeneralPanelImpl::endTime()
{
    return scheduleEndTime->time();
}

QDate TaskGeneralPanelImpl::startDate()
{
    return scheduleStartDate->date();
}


QDate TaskGeneralPanelImpl::endDate()
{
    return scheduleEndDate->date();
}

void TaskGeneralPanelImpl::setStartDateTime( const QDateTime &dt )
{
    setStartDate(dt.date());
    setStartTime(dt.time());
}


void TaskGeneralPanelImpl::setEndDateTime( const QDateTime &dt )
{
    setEndDate(dt.date());
    setEndTime(dt.time());
}

void TaskGeneralPanelImpl::setStartDate( const QDate &date )
{
    scheduleStartDate->setDate(date);
}


void TaskGeneralPanelImpl::setEndDate( const QDate &date )
{
    scheduleEndDate->setDate(date);
}



}  //KPlato namespace

#include "kpttaskgeneralpanel.moc"
