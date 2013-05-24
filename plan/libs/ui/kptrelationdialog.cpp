/* This file is part of the KDE project
   Copyright (C) 2003 - 2010 Dag Andersen <danders@get2net.dk>

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

#include "kptrelationdialog.h"
#include "kptrelation.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kptcommand.h"


#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

RelationPanel::RelationPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    lagLabel->setText( i18nc( "@label:spinbox Time lag", "Lag:" ) );
    QString tt = i18nc( "@info:tooltip", "<emphasis>Lag</emphasis> is the time the dependent task is delayed" );
    lagLabel->setToolTip( tt );
    lag->setToolTip( tt );
}
    
AddRelationDialog::AddRelationDialog(Project &project, Relation *rel, QWidget *p, const QString& caption, ButtonCodes buttons)
    : KDialog(p),
    m_project( project ),
    m_relation( rel ),
    m_deleterelation( true )
{
    setCaption( caption );
    setButtons( buttons );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    if ( caption.isEmpty() ) {
        setCaption( i18nc( "@title:window", "Add Dependency" ) );
    }
    m_relation = rel;
    m_panel = new RelationPanel(this);
    setMainWidget(m_panel);
    m_panel->activateWindow();

    m_panel->fromName->setText(rel->parent()->name());
    m_panel->toName->setText(rel->child()->name());
    if (rel->type() == Relation::FinishStart) {
        m_panel->bFinishStart->setChecked(true);
    } else if (rel->type() == Relation::FinishFinish) {
        m_panel->bFinishFinish->setChecked(true);
    } else if (rel->type() == Relation::StartStart) {
        m_panel->bStartStart->setChecked(true);
    }

    m_panel->lag->setUnit( Duration::Unit_h );
    m_panel->lag->setValue(rel->lag().toDouble( Duration::Unit_h ) ); //FIXME store user input

    m_panel->relationType->setFocus();
    enableButtonOk(true);
    //connect(m_panel->relationType, SIGNAL(clicked(int)), SLOT(typeClicked(int)));
    connect(m_panel->bFinishStart, SIGNAL(toggled(bool)), SLOT(slotFinishStartToggled(bool)));
    connect(m_panel->bFinishFinish, SIGNAL(toggled(bool)), SLOT(slotFinishFinishToggled(bool)));
    connect(m_panel->bStartStart, SIGNAL(toggled(bool)), SLOT(slotStartStartToggled(bool)));
    connect(m_panel->lag, SIGNAL(valueChanged(double)), SLOT(lagChanged()));
    
    connect(&project, SIGNAL(nodeRemoved(Node*)), SLOT(slotNodeRemoved(Node*)));
}

AddRelationDialog::~AddRelationDialog()
{
    if ( m_deleterelation ) {
        delete m_relation; //in case of cancel
    }
}

void AddRelationDialog::slotNodeRemoved( Node *node )
{
    if ( m_relation->parent() == node || m_relation->child() == node ) {
        reject();
    }
}

MacroCommand *AddRelationDialog::buildCommand() {
    MacroCommand *c = new MacroCommand( i18nc("(qtundo-format)", "Add task dependency") );
    c->addCommand( new AddRelationCmd(m_project, m_relation ) );
    m_deleterelation = false; // don't delete
    return c;
}

void AddRelationDialog::slotOk() {
    accept();
}
void AddRelationDialog::slotFinishStartToggled(bool ch) {
    //kDebug(planDbg())<<ch;
    if (ch && m_relation->type() != Relation::FinishStart)
        enableButtonOk(true);
}
void AddRelationDialog::slotFinishFinishToggled(bool ch) {
    //kDebug(planDbg())<<ch;
    if (ch && m_relation->type() != Relation::FinishFinish)
        enableButtonOk(true);
}
void AddRelationDialog::slotStartStartToggled(bool ch) {
    //kDebug(planDbg())<<ch;
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

ModifyRelationDialog::ModifyRelationDialog(Project &project, Relation *rel, QWidget *p)
    : AddRelationDialog(project, rel, p, i18nc( "@title:window", "Edit Dependency"), Ok|Cancel|User1)
{
    m_deleterelation = false;

    setButtonText( KDialog::User1, i18nc( "@action:button", "Delete") );
    m_deleted = false;
    enableButtonOk(false);
    
    connect(this, SIGNAL(user1Clicked()), SLOT(slotUser1()));
    
    connect(&project, SIGNAL(relationRemoved(Relation*)), SLOT(slotRelationRemoved(Relation*)));
}

void ModifyRelationDialog::slotRelationRemoved( Relation *relation )
{
    if ( m_relation == relation ) {
        reject();
    }
}

// Delete
void ModifyRelationDialog::slotUser1() {
    m_deleted = true;
    accept();
}

MacroCommand *ModifyRelationDialog::buildCommand() {
    MacroCommand *cmd=0;
    if ( m_deleted ) {
        cmd = new MacroCommand( i18nc( "(qtundo-format)", "Delete task dependency" ) );
        cmd ->addCommand( new DeleteRelationCmd( m_project, m_relation ) );
        return cmd;
    }
    QString s = i18nc( "(qtundo-format)", "Modify task dependency" );
    if (selectedRelationType() != m_relation->type()) {
        if (cmd == 0)
            cmd = new MacroCommand( s );
        cmd->addCommand(new ModifyRelationTypeCmd(m_relation, (Relation::Type)(selectedRelationType())));
        
        //kDebug(planDbg())<<m_panel->relationType->selectedId();
    }
    Duration d(m_panel->lag->value(), m_panel->lag->unit());
    if (m_relation->lag() != d) {
        if (cmd == 0)
            cmd = new MacroCommand( s );
        cmd->addCommand(new ModifyRelationLagCmd(m_relation, d));
    }
    return cmd;
}

}  //KPlato namespace

#include "kptrelationdialog.moc"
