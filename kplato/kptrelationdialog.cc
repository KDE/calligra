/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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

#include "kptrelationdialog.h"
#include "kptrelation.h"
#include "kptnode.h"
#include "defs.h"

#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>



KPTRelationDialog::KPTRelationDialog(KPTRelation *rel, QWidget *p, const char *n)
    : KDialogBase(Plain, i18n("Edit Relationship"), Ok|Cancel|User1, Ok, p, n, true, true)
{
    m_relation = rel;
    kdDebug()<<k_funcinfo<<"rel="<<rel<<endl;
    m_parent = 0;
    m_child = 0;
    setButtonText( KDialogBase::User1, i18n("Delete") );

    init();
}

KPTRelationDialog::KPTRelationDialog(KPTNode *parentNode, KPTNode *childNode, QWidget *p, const char *n)
    : KDialogBase(Plain, i18n("New Relationship"), Ok|Cancel, Ok, p, n, true, true)
{
    m_relation = 0;
    m_parent = parentNode;
    m_child = childNode;
    init();
}

void KPTRelationDialog::init()
{
    QWidget *page = plainPage();
    QGridLayout *layout = new QGridLayout(page, 3, 2, marginHint(), spacingHint());
    QString s1 = i18n("From: "), s2 = i18n("To: ");
    if ( m_relation )
    {
        s1 += m_relation->parent()->name();
        s2 += m_relation->child()->name();
    }
    else
    {
        s1 += m_parent->name();    
        s2 += m_child->name();    
    }
    QLabel *l = new QLabel(s1,page);
    layout->addWidget(l, 0, 0);
    l = new QLabel(s2,page);
    layout->addWidget(l, 1, 0);

    //layout->addMultiCellWidget(relationType = new QButtonGroup(box), 0, 5, 0, 1);
    relationType = new QButtonGroup(0, Qt::Vertical, i18n("Relationship Type"),page);
    relationType->layout()->setSpacing(KDialog::spacingHint());
    relationType->layout()->setMargin(KDialog::marginHint());
    layout->addWidget(relationType, 2, 0);

    QGridLayout *relationTypeLayout = new QGridLayout(relationType->layout(), 6, 1);

    QRadioButton *b = new QRadioButton(QString("Finish-Start"), relationType);
    relationTypeLayout->addWidget(b, 0, 0);
    b = new QRadioButton(QString("Finish-Finish"), relationType);
    relationTypeLayout->addWidget(b, 1, 0);
    b = new QRadioButton(QString("Start-Start"), relationType);
    relationTypeLayout->addWidget(b, 2, 0);

    timingType = new QButtonGroup(0, Qt::Vertical,i18n("Timing Type"),page);
    timingType->layout()->setSpacing(KDialog::spacingHint());
    timingType->layout()->setMargin(KDialog::marginHint());
    layout->addWidget(timingType, 2, 1);

    QGridLayout *timingTypeLayout = new QGridLayout(timingType->layout(), 6, 1);
    
    b = new QRadioButton(QString("Start On Date"), timingType);
    timingTypeLayout->addWidget(b, 0, 0);
    b = new QRadioButton(QString("Finish By Date"), timingType);
    timingTypeLayout->addWidget(b, 1, 0);
    b = new QRadioButton(QString("Work Between Dates"), timingType);
    timingTypeLayout->addWidget(b, 2, 0);
    b = new QRadioButton(QString("Milestone"), timingType);
    timingTypeLayout->addWidget(b, 3, 0);
    b = new QRadioButton(QString("Percent Of Project"), timingType);
    timingTypeLayout->addWidget(b, 4, 0);
    
    if (m_relation == 0)
    {
        relationType->setButton(FINISH_START);
        timingType->setButton(START_ON_DATE);
    }
    else
    {
        relationType->setButton(m_relation->timingRelation());
        timingType->setButton(m_relation->timingType());
    }
    
}


void KPTRelationDialog::slotOk() 
{
    QButton *r = relationType->selected();
    if ( r == 0 ) {
        KMessageBox::sorry(this, i18n("You must select a relationship type"));
	    return;
    }
    QButton *t = timingType->selected();
    if ( t == 0 ) {
        KMessageBox::sorry(this, i18n("You have to select a timing type"));
	    return;
    }

    if (m_relation)
    {
        m_relation->setTimingRelation((TimingRelation)relationType->id(r));
        m_relation->setTimingType((TimingType)timingType->id(t));
    }
    else
        m_child->addDependParentNode(m_parent, (TimingType)timingType->id(t), (TimingRelation)relationType->id(r));

    accept();
}

// Delete
void KPTRelationDialog::slotUser1() 
{
    m_relation->child()->delDependParentNode(m_relation);
    m_relation->parent()->delDependChildNode(m_relation);
    delete m_relation;
    accept();
}

#include "kptrelationdialog.moc"
