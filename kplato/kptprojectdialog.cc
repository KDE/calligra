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

#include "kptprojectdialog.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptresourcedialog.h"

#include <kdebug.h>

#include <qlayout.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <qtextedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qstringlist.h>
#include <qpainter.h>

KPTProjectDialog::KPTProjectDialog(KPTProject &p, QWidget *q, const char *n)
    : KDialogBase(Tabbed, i18n("Project Settings"), Ok|Cancel, Ok, q, n, true, true), 
      project(p),
      m_groupItem(0),
      m_resourceItem(0)
{
    // For now the setup is pretty trivial. It's planned to be able to control
    // the children here too.
    QWidget *settings = addPage(i18n("Settings"));
    QGridLayout *layout = new QGridLayout(settings, 11, 2, marginHint(),
					 spacingHint());

    // First line: Name of the project
    layout->addWidget(new QLabel(i18n("Project name:"), settings), 0, 0);
    layout->addWidget(namefield=new KLineEdit(project.name(), settings), 0, 1);

    // Second line: Name of responsible person
    layout->addWidget(new QLabel(i18n("Project leader:"), settings), 1, 0);
    layout->addWidget(leaderfield = new KLineEdit(project.leader(), settings),
		      1, 1);


    layout->addMultiCellWidget(constraints = new QButtonGroup(i18n("Scheduling"),settings), 3, 7, 0, 1);
    QGridLayout *constraintsLayout = new QGridLayout(constraints, 5, 2, marginHint()+5, spacingHint());
    
    QRadioButton *b = new QRadioButton(i18n("ASAP"), constraints);
    constraintsLayout->addWidget(b, 1, 0);
    b = new QRadioButton(i18n("ALAP"), constraints);
    constraintsLayout->addWidget(b, 2, 0);
    b = new QRadioButton(i18n("Start Not Earlier Than"), constraints);
    constraintsLayout->addWidget(b, 3, 0);
    b = new QRadioButton(i18n("Finish Not Later Than"), constraints);
    constraintsLayout->addWidget(b, 4, 0);
    b = new QRadioButton(i18n("Must Start On"), constraints);
    constraintsLayout->addWidget(b, 5, 0);
    
    constraints->setButton(project.constraint());

    constraintsLayout->addWidget(sneTime=new QDateTimeEdit(project.startNotEarlier().dateTime(), constraints), 3, 1);
    constraintsLayout->addWidget(fnlTime=new QDateTimeEdit(project.finishNotLater().dateTime(), constraints), 4, 1);
    constraintsLayout->addWidget(msoTime=new QDateTimeEdit(project.mustStartOn().dateTime(), constraints), 5, 1);

    // Resources
    QWidget *resPage = addPage(i18n("Resources"));
    QGridLayout *resPageLayout = new QGridLayout(resPage, 1, 3, marginHint(), spacingHint());

    QWidget *gr = new QWidget(resPage);
    QWidget *res = new QWidget(resPage);
    QWidget *risc = new QWidget(resPage);
    resPageLayout->addWidget(gr, 0, 0);
    resPageLayout->addWidget(res, 0, 1);
    resPageLayout->addWidget(risc, 0, 2);

    QGridLayout *grLayout = new QGridLayout(gr, 8, 3, marginHint(), spacingHint());
    QGridLayout *resLayout = new QGridLayout(res, 8, 3, marginHint(), spacingHint());
    QGridLayout *riscLayout = new QGridLayout(risc, 8, 3, marginHint(), spacingHint());
    
    grLayout->addWidget(new QLabel(i18n("Group:"), gr), 0, 0);
    group = new QListBox(gr);
    grLayout->addMultiCellWidget(group, 1, 4, 0, 3);
    QPushButton *addGroup = new QPushButton(i18n("Add..."),gr);
    grLayout->addWidget(addGroup, 6, 0);
    QPushButton *editGroup = new QPushButton(i18n("Edit..."),gr);
    grLayout->addWidget(editGroup, 6, 1);
    QPushButton *deleteGroup = new QPushButton(i18n("Delete"),gr);
    grLayout->addWidget(deleteGroup, 7, 0);
    
    resLayout->addWidget(new QLabel(i18n("Resource:"), res), 0, 0);
    resource = new QListBox(res);
    resLayout->addMultiCellWidget(resource, 1, 4, 0, 3);
    QPushButton *addResource = new QPushButton(i18n("Add..."),res);
    resLayout->addWidget(addResource, 6, 0);
    QPushButton *editResource = new QPushButton(i18n("Edit..."),res);
    resLayout->addWidget(editResource, 6, 1);
    QPushButton *deleteResource = new QPushButton(i18n("Delete"),res);
    resLayout->addWidget(deleteResource, 7, 0);

    riscLayout->addWidget(new QLabel(i18n("Risc:"), risc), 0, 0);
    
    // Description
    QWidget *notes = addPage(i18n("Notes"));
    QGridLayout *notesLayout = new QGridLayout(notes, 6, 2, marginHint(), spacingHint());
    notesLayout->addWidget(new QLabel(i18n("Description:"), notes), 0, 0);
    descriptionfield = new QTextEdit(notes);
    descriptionfield->setText(project.description());
    notesLayout->addMultiCellWidget(descriptionfield, 1, 5, 0, 1);
    
    connect (addGroup, SIGNAL(clicked()), this, SLOT(slotAddGroup()));
    connect (editGroup, SIGNAL(clicked()), this, SLOT(slotEditGroup()));
    connect (deleteGroup, SIGNAL(clicked()), this, SLOT(slotDeleteGroup()));
    
    connect (addResource, SIGNAL(clicked()), this, SLOT(slotAddResource()));
    connect (editResource, SIGNAL(clicked()), this, SLOT(slotEditResource()));
    connect (deleteResource, SIGNAL(clicked()), this, SLOT(slotDeleteResource()));
    
    QPtrListIterator<KPTResourceGroup> it(project.resourceGroups());
    for ( ; it.current(); ++it ) {
        group->insertItem(new KPTGroupItem(it.current()));
        kdDebug()<<k_funcinfo<<" Added group: "<<it.current()->name()<<endl;
    }
    slotGroupChanged(group->item(group->topItem()));

    connect (group, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(slotGroupChanged(QListBoxItem *)));
    connect (resource, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(slotResourceChanged(QListBoxItem *)));
    
    //kdDebug()<<k_funcinfo<<" Groups="<<it.count()<<endl;
    
}


void KPTProjectDialog::slotOk() {
    if (namefield->text() == "" || leaderfield->text() == "") {
    	KMessageBox::sorry(this, i18n("You have to set a name and leader for "
		    	   "the project"));
	    return;
    }
    KPTDuration dt = KPTDuration();
    QButton *b = constraints->selected();
    if ( b == 0 ) {
        KMessageBox::sorry(this, i18n("You have to select a constraint"));
	    return;
    }
    
    project.setConstraint((KPTNode::ConstraintType)constraints->id(b));
    dt.set(msoTime->dateTime());
    project.setStartTime(dt);
    project.setMustStartOn(dt);
    
        
    project.setName(namefield->text());
    project.setLeader(leaderfield->text());
    project.setDescription(descriptionfield->text());

    accept();
}

void KPTProjectDialog::slotAddGroup()
{
    kdDebug()<<k_funcinfo<<endl;
    KPTGroupDialog *dia = new KPTGroupDialog();
    if (dia->exec() == QDialog::Accepted)
    {
        KPTResourceGroup *r = new KPTResourceGroup();
        r->setName(dia->name());
        project.addResourceGroup(r);
        m_groupItem = new KPTGroupItem(r);
        group->insertItem(m_groupItem);
        group->setCurrentItem(m_groupItem);
    }
    delete dia;
}

void KPTProjectDialog::slotEditGroup()
{
    kdDebug()<<k_funcinfo<<endl;
}

void KPTProjectDialog::slotDeleteGroup()
{
    kdDebug()<<k_funcinfo<<endl;
}

void KPTProjectDialog::slotAddResource()
{
    kdDebug()<<k_funcinfo<<endl;
    if (!m_groupItem)
    {
        KMessageBox::sorry(this, i18n("You have to select a resource group"));
    }
    else
    {
        KPTResourceDialog *dia = new KPTResourceDialog();
        if (dia->exec() == QDialog::Accepted)
        {
            KPTResource *res = new KPTResource();
            res->setName(dia->name());
            //res->addWorkingHour();
            m_groupItem->m_group->addResource(res, 0);
            m_resourceItem = new KPTResourceItem(res);
            resource->insertItem(m_resourceItem);
            resource->setCurrentItem(m_resourceItem);
        }
        delete dia;
    }
}

void KPTProjectDialog::slotEditResource()
{
    kdDebug()<<k_funcinfo<<endl;
}

void KPTProjectDialog::slotDeleteResource()
{
    kdDebug()<<k_funcinfo<<endl;
}

void KPTProjectDialog::slotGroupChanged( QListBoxItem *item)
{
    kdDebug()<<k_funcinfo<<endl;
    if (!item)
        return;
        
    m_groupItem = (KPTGroupItem *)item;
    resource->clear();
    QPtrListIterator<KPTResource> it(m_groupItem->m_group->resources());
    for ( ; it.current(); ++it ) {
        resource->insertItem(new KPTResourceItem(it.current()));
    }
    //kdDebug()<<k_funcinfo<<" Groups now: "<<it.count()<<endl;
}

void KPTProjectDialog::slotResourceChanged( QListBoxItem *item)
{
    kdDebug()<<k_funcinfo<<endl;
    m_resourceItem = (KPTResourceItem *)item;
}


//////////////////////////  KPTGroupDialog //////////////////////////////////

KPTGroupDialog::KPTGroupDialog(QWidget *p, const char *n)
    : KDialogBase(Plain, i18n("New Resource Group"), Ok|Cancel, Ok, p, n, true, true)
{
    QWidget *page = plainPage();
    QGridLayout *layout = new QGridLayout(page, 1, 2, marginHint(), spacingHint());
    layout->addWidget(new QLabel(i18n("Group name:"), page), 0, 0);
    layout->addWidget(m_name = new KLineEdit(page), 0, 1);
}

QString KPTGroupDialog::name()
{
    return m_name->text(); 
}

void KPTGroupDialog::slotOk()
{
    if (m_name->text().isEmpty())
    {
    	KMessageBox::sorry(this, i18n("You have to enter a name for the resource group"));
	    return;
    }
    accept();
}

#include "kptprojectdialog.moc"
