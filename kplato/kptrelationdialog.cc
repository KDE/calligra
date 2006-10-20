/* This file is part of the KDE project
   Copyright (C) 2003, 2004 Dag Andersen <danders@get2net.dk>

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

#include "kptrelationdialog.h"
#include "kptrelation.h"
#include "kptnode.h"
#include "kptpart.h"
#include "kptcommand.h"
#include "kptdurationwidget.h"
#include "relationpanel.h"

#include <QLayout>
#include <QLabel>

#include <kmessagebox.h>
#include <klocale.h>
#include <kcommand.h>
#include <kdebug.h>

namespace KPlato
{

RelationPanel::RelationPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    lag = new DurationWidget(durationHolder);
    QLayout *l = durationHolder->layout();
    if (l)
        l->addWidget(lag);
}
    
AddRelationDialog::AddRelationDialog(Relation *rel, QWidget *p, QString caption, ButtonCodes buttons, const char *n)
    : KDialog(p)
{
    setCaption( caption );
    setButtons( buttons );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    if (caption.isEmpty())
        setCaption(i18n("Add Relationship"));
    m_relation = rel;
    m_panel = new RelationPanel(this);
    setMainWidget(m_panel);
    m_panel->setActiveWindow();

    m_panel->fromName->setText(rel->parent()->name());
    m_panel->toName->setText(rel->child()->name());
    if (rel->type() == Relation::FinishStart) {
        m_panel->bFinishStart->setChecked(true);
    } else if (rel->type() == Relation::FinishFinish) {
        m_panel->bFinishFinish->setChecked(true);
    } else if (rel->type() == Relation::StartStart) {
        m_panel->bStartStart->setChecked(true);
    }

    m_panel->lag->setVisibleFields(DurationWidget::Days|DurationWidget::Hours|DurationWidget::Minutes);
    m_panel->lag->setFieldUnit(0, i18nc("days", "d"));
    m_panel->lag->setFieldUnit(1, i18nc("hours", "h"));
    m_panel->lag->setFieldUnit(2, i18nc("minutes", "m"));
    m_panel->lag->setValue(rel->lag());

    m_panel->relationType->setFocus();
    enableButtonOk(true);
    //connect(m_panel->relationType, SIGNAL(clicked(int)), SLOT(typeClicked(int)));
    connect(m_panel->bFinishStart, SIGNAL(toggled(bool)), SLOT(slotFinishStartToggled(bool)));
    connect(m_panel->bFinishFinish, SIGNAL(toggled(bool)), SLOT(slotFinishFinishToggled(bool)));
    connect(m_panel->bStartStart, SIGNAL(toggled(bool)), SLOT(slotStartStartToggled(bool)));
    connect(m_panel->lag, SIGNAL(valueChanged()), SLOT(lagChanged()));
}

KCommand *AddRelationDialog::buildCommand(Part *part) {
    return new AddRelationCmd(part, m_relation, i18n("Add Relation"));
}

void AddRelationDialog::slotOk() {
    accept();
}
void AddRelationDialog::slotFinishStartToggled(bool ch) {
    //kDebug()<<k_funcinfo<<ch<<endl;
    if (ch && m_relation->type() != Relation::FinishStart)
        enableButtonOk(true);
}
void AddRelationDialog::slotFinishFinishToggled(bool ch) {
    //kDebug()<<k_funcinfo<<ch<<endl;
    if (ch && m_relation->type() != Relation::FinishFinish)
        enableButtonOk(true);
}
void AddRelationDialog::slotStartStartToggled(bool ch) {
    //kDebug()<<k_funcinfo<<ch<<endl;
    if (ch && m_relation->type() != Relation::StartStart)
        enableButtonOk(true);
}

void AddRelationDialog::lagChanged() {
    enableButtonOk(true);
}

void AddRelationDialog::typeClicked(int id) {
    if (id != m_relation->type())
        enableButtonOk(true);
}

int AddRelationDialog::selectedRelationType() const {
    if (m_panel->bStartStart->isChecked())
        return Relation::StartStart;
    else if (m_panel->bFinishFinish->isChecked())
        return Relation::FinishFinish;
    
    return Relation::FinishStart;
}

//////////////////

ModifyRelationDialog::ModifyRelationDialog(Relation *rel, QWidget *p, const char *n)
    : AddRelationDialog(rel, p, i18n("Edit Relationship"), Ok|Cancel|User1, n)
{
    setButtonText( KDialog::User1, i18n("Delete") );
    m_deleted = false;
    enableButtonOk(false);
}

// Delete
void ModifyRelationDialog::slotUser1() {
    m_deleted = true;
    accept();
}

KCommand *ModifyRelationDialog::buildCommand(Part *part) {
    KMacroCommand *cmd=0;
    if (selectedRelationType() != m_relation->type()) {
        if (cmd == 0)
            cmd = new KMacroCommand(i18n("Modify Relation"));
        cmd->addCommand(new ModifyRelationTypeCmd(part, m_relation, (Relation::Type)(selectedRelationType())));
        
        //kDebug()<<k_funcinfo<<m_panel->relationType->selectedId()<<endl;
    }
    if (m_relation->lag() != m_panel->lag->value()) {
        if (cmd == 0)
            cmd = new KMacroCommand(i18n("Modify Relation"));
        cmd->addCommand(new ModifyRelationLagCmd(part, m_relation, m_panel->lag->value()));
    }
    return cmd;
}

}  //KPlato namespace

#include "kptrelationdialog.moc"
