/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk

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
#include "kpttasknotespanelbase.h"
#include "kptcommand.h"

#include <qlayout.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdatepicker.h>
#include "kptdurationwidget.h"
#include <kcommand.h>

#include <qtextedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qtabwidget.h>
#include <qtable.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <kdebug.h>

KPTTaskDialog::KPTTaskDialog(KPTTask &task, QWidget *p, const char *n)
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
    m_resourcesTab->effort->setVisibleFields(
        ~(KPTDurationWidget::Seconds | KPTDurationWidget::Milliseconds));
    topLayout->addWidget(m_resourcesTab);

    page = addPage(i18n("&Notes"));
    topLayout = new QVBoxLayout(page, 0, spacingHint());
    m_notesTab = new KPTTaskNotesPanelBase(page);
    topLayout->addWidget(m_notesTab);

    // Create some shortcuts.
    m_name = m_generalTab->namefield;
    m_leader = m_generalTab->leaderfield;
    m_description = m_notesTab->descriptionfield;

    // Set the state of all the child widgets.
    enableButtonOK(false);
    m_name->setText(task.name());
    m_leader->setText(task.leader());
    m_description->setText(task.description());
    m_generalTab->setSchedulingType(task.constraint());
    if (task.constraintTime().isValid())
        m_generalTab->setDateTime(task.constraintTime());
    else
        m_generalTab->setDateTime(QDateTime::currentDateTime());

    connect(m_generalTab, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOK(bool) ));
    connect(m_resourcesTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
    m_generalTab->checkAllFieldsFilled();
    m_name->setFocus();
}


KMacroCommand *KPTTaskDialog::buildCommand() {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Task"));
    bool modified = false;

    KPTDuration dt = KPTDuration();

    if (m_task.name() != m_name->text()) {
        cmd->addCommand(new KPTNodeModifyNameCmd(m_task, m_name->text()));
        modified = true;
    }
    if (m_task.leader() != m_leader->text()) {
        cmd->addCommand(new KPTNodeModifyLeaderCmd(m_task, m_leader->text()));
        modified = true;
    }
    if (m_task.description() != m_description->text()) {
        cmd->addCommand(new KPTNodeModifyDescriptionCmd(m_task, m_description->text()));
        modified = true;
    }
    KPTNode::ConstraintType c = (KPTNode::ConstraintType)m_generalTab->schedulingType();
    if (c != m_task.constraint()) {
        cmd->addCommand(new KPTNodeModifyConstraintCmd(m_task, c));
        modified = true;
    }
    if (c == KPTNode::FinishNotLater || c == KPTNode::StartNotEarlier || c == KPTNode::MustStartOn) {
        cmd->addCommand(new KPTNodeModifyConstraintTimeCmd(m_task, m_generalTab->dateTime()));
        modified = true;
    }
    KMacroCommand *m = m_resourcesTab->buildCommand();
    if (m) {
        cmd->addCommand(m);
        modified = true;
    }
    if (modified)
        return cmd;

    delete cmd;
    return 0;
}

void KPTTaskDialog::slotOk() {
    accept();
}

#include "kpttaskdialog.moc"
