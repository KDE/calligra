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

#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kcommand.h>
#include <kdebug.h>

namespace KPlato
{

KPTAddRelationDialog::KPTAddRelationDialog(KPTRelation *rel, QWidget *p, QString caption, int buttons, const char *n)
    : KDialogBase(Swallow, caption, buttons, Ok, p, n, true, true)
{
    if (caption.isEmpty())
        setCaption(i18n("Add Relationship"));
    m_relation = rel;
    m_panel = new RelationPanel(this);
    setMainWidget(m_panel);
    m_panel->setActiveWindow();
    
    m_panel->fromName->setText(rel->parent()->name());
    m_panel->toName->setText(rel->child()->name());
    m_panel->relationType->setButton(rel->type());
    
    m_panel->lag->setVisibleFields(KPTDurationWidget::Days|KPTDurationWidget::Hours|KPTDurationWidget::Minutes);
    m_panel->lag->setFieldUnit(0, i18n("days"));
    m_panel->lag->setFieldUnit(1, i18n("hours"));
    m_panel->lag->setFieldUnit(2, i18n("minutes"));
    m_panel->lag->setValue(rel->lag());
    
    m_panel->relationType->setFocus();
    enableButtonOK(true);
    connect(m_panel->relationType, SIGNAL(clicked(int)), SLOT(typeClicked(int)));
    connect(m_panel->lag, SIGNAL(valueChanged()), SLOT(lagChanged()));
}

KCommand *KPTAddRelationDialog::buildCommand(KPTPart *part) {
    return new KPTAddRelationCmd(part, m_relation, i18n("Add Relation"));
}

void KPTAddRelationDialog::slotOk() {
    if ( m_panel->relationType->selected() == 0 ) {
        KMessageBox::sorry(this, i18n("You must select a relationship type"));
        return;
    }
    accept();
}

void KPTAddRelationDialog::lagChanged() {
    enableButtonOK(true);
}

void KPTAddRelationDialog::typeClicked(int id) {
    if (id != m_relation->type())
        enableButtonOK(true);
}

//////////////////

KPTModifyRelationDialog::KPTModifyRelationDialog(KPTRelation *rel, QWidget *p, const char *n)
    : KPTAddRelationDialog(rel, p, i18n("Edit Relationship"), Ok|Cancel|User1, n)
{
    setButtonText( KDialogBase::User1, i18n("Delete") );
    m_deleted = false;
    enableButtonOK(false);
}

// Delete
void KPTModifyRelationDialog::slotUser1() {
    m_deleted = true;
    accept();
}

KCommand *KPTModifyRelationDialog::buildCommand(KPTPart *part) {
    KMacroCommand *cmd=0;
    if (m_panel->relationType->selectedId() != m_relation->type()) {
        if (cmd == 0)
            cmd = new KMacroCommand(i18n("Modify Relation"));
        cmd->addCommand(new KPTModifyRelationTypeCmd(part, m_relation, (KPTRelation::Type)m_panel->relationType->selectedId()));
    }
    if (m_relation->lag() != m_panel->lag->value()) {
        if (cmd == 0)
            cmd = new KMacroCommand(i18n("Modify Relation"));
        cmd->addCommand(new KPTModifyRelationLagCmd(part, m_relation, m_panel->lag->value()));
    }
    return cmd;
}

}  //KPlato namespace

#include "kptrelationdialog.moc"
