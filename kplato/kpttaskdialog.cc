/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@suse.de

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

#include <qlayout.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <qtextedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qtable.h>
#include <kdebug.h>

KPTTaskDialog::KPTTaskDialog(KPTTask &t, QPtrList<KPTResourceGroup> &resourceGroups, QWidget *p, const char *n)
    : KDialogBase(Tabbed, i18n("Task Settings"), Ok|Cancel, Ok, p,
		  n, true, true), task(t), m_resourceGroups(resourceGroups)
{
    // For now the setup is pretty trivial. It's planned to be able to control
    // the children here too.
    QWidget *settings = addPage(i18n("Settings"));
    QGridLayout *layout = new QGridLayout(settings, 11, 2, marginHint(),
					 spacingHint());

    // First line: Name of the task
    layout->addWidget(new QLabel(i18n("Task name:"), settings), 0, 0);
    layout->addWidget(namefield=new KLineEdit(task.name(), settings), 0, 1);

    // Second line: Name of responsible person
    layout->addWidget(new QLabel(i18n("Responsible:"), settings), 1, 0);
    layout->addWidget(leaderfield = new KLineEdit(task.leader(), settings), 1, 1);

    layout->addWidget(new QLabel(i18n("Effort (hours):"), settings), 2, 0);
    effort=new QSpinBox(1,365*24,1,settings);
    effort->setValue(task.effort()->expected().duration()/3600);
    layout->addWidget(effort, 2, 1);


    layout->addMultiCellWidget(constraints = new QButtonGroup(0, Qt::Vertical, i18n("Scheduling"), settings), 3, 7, 0, 1);
    constraints->layout()->setSpacing(KDialog::spacingHint());
    constraints->layout()->setMargin(KDialog::marginHint());
    QGridLayout *constraintsLayout = new QGridLayout(constraints->layout(), 5, 2);

    QRadioButton *b = new QRadioButton(i18n("ASAP"), constraints);
    constraintsLayout->addWidget(b, 1, 0);
    b = new QRadioButton(i18n("ALAP"), constraints);
    constraintsLayout->addWidget(b, 2, 0);
    b = new QRadioButton(i18n("Start not earlier than"), constraints);
    constraintsLayout->addWidget(b, 3, 0);
    b = new QRadioButton(i18n("Finish not later than"), constraints);
    constraintsLayout->addWidget(b, 4, 0);
    b = new QRadioButton(i18n("Must start on"), constraints);
    constraintsLayout->addWidget(b, 5, 0);

    constraints->setButton(task.constraint());

    constraintsLayout->addWidget(sneTime=new QDateTimeEdit(task.startNotEarlier().dateTime(), constraints), 3, 1);
    constraintsLayout->addWidget(fnlTime=new QDateTimeEdit(task.finishNotLater().dateTime(), constraints), 4, 1);
    constraintsLayout->addWidget(msoTime=new QDateTimeEdit(task.mustStartOn().dateTime(), constraints), 5, 1);


    // Resources
    QWidget *res = addPage(i18n("Resources"));
    QGridLayout *resLayout = new QGridLayout(res, 6, 2, marginHint(), spacingHint());

    uint noRes = m_resourceGroups.count();
    table = new QTable(noRes,3,res);
    resLayout->addWidget(table, 0, 0);
    //table->verticalHeader()->hide();
    table->horizontalHeader()->setLabel(0,"Resource");
    table->horizontalHeader()->setLabel(1,"Use");
    table->horizontalHeader()->setLabel(2,"Limit");

    for (uint i=0; i<noRes; ++i) {
        KPTResourceItem *item = new KPTResourceItem(m_resourceGroups.at(i), table, QTableItem::Never);
        table->setItem(i, 0, item);

        QCheckTableItem *c = new QCheckTableItem(table,"");
        QTableItem *ti = new QTableItem(table,QTableItem::OnTyping,"1");
        QPtrListIterator<KPTResourceRequest> it(task.resourceRequests());
        for (; it.current(); ++it) {
            kdDebug()<<k_funcinfo<<" group="<<item->m_resourceGroup<<"  request group="<<it.current()->group()<<endl;
            if (it.current()->group() == item->m_resourceGroup) {
                c->setChecked(true);
                kdDebug()<<k_funcinfo<<"numResources: "<<it.current()->numResources()<<" -> "<<QString("%1").arg(it.current()->numResources())<<endl;
                ti->setText(QString("%1").arg(it.current()->numResources()));
            }
        }
        table->setItem(i, 1, c);
        table->setItem(i, 2, ti);
    }

    // Description
    QWidget *notes = addPage(i18n("Notes"));
    QGridLayout *notesLayout = new QGridLayout(notes, 6, 2, marginHint(), spacingHint());
    notesLayout->addWidget(new QLabel(i18n("Description:"), notes), 0, 0);
    descriptionfield = new QTextEdit(notes);
    descriptionfield->setText(task.description());
    notesLayout->addMultiCellWidget(descriptionfield, 1, 5, 0, 1);
    

}


void KPTTaskDialog::slotOk() {
    if (namefield->text() == "" || leaderfield->text() == "") {
	KMessageBox::sorry(this, i18n("You have to set a name and responsible for "
			   "the task"));
	return;
    }
    KPTDuration dt = KPTDuration();
    QButton *b = constraints->selected();
    if ( b == 0 ) {
        KMessageBox::sorry(this, i18n("You have to select a constraint"));
	    return;
    }
    
    task.setConstraint((KPTNode::ConstraintType)constraints->id(b));
    dt.set(sneTime->dateTime());
    task.setStartNotEarlier(dt);
    dt.set(fnlTime->dateTime());
    task.setFinishNotLater(dt);
    dt.set(msoTime->dateTime());
    task.setMustStartOn(dt);

    task.setName(namefield->text());
    task.setLeader(leaderfield->text());
    task.setDescription(descriptionfield->text());

    task.effort()->set( (effort->value()*3600) );

    //resources
    task.clearResourceRequests();
    for (int i = 0; i < table->numRows(); ++i) {
        QCheckTableItem *cti = dynamic_cast<QCheckTableItem *>(table->item(i, 1));
        if (cti->isChecked()) {
            KPTResourceItem *item = dynamic_cast<KPTResourceItem *>(table->item(i, 0));
            bool ok = false;
            int num = table->item(i,2)->text().toInt(&ok);
            if (!ok)
                num = 0;
            task.addResourceRequest(item->m_resourceGroup, num);
            kdDebug()<<k_funcinfo<<"Checked: '"<<item->m_resourceGroup->name()<<"' numResources="<<num<<endl;
        }
    }
                            
    accept();
}


#include "kpttaskdialog.moc"
