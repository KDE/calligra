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

#include <qlayout.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <qtextedit.h>


KPTProjectDialog::KPTProjectDialog(KPTProject &p, QWidget *q, const char *n)
    : KDialogBase(Tabbed, i18n("Project Settings"), Ok|Cancel, Ok, q,
		  n, true, true), project(p)
{
    // For now the setup is pretty trivial. It's planned to be able to control
    // the children here too.
    QWidget *panel = addPage(i18n("Settings"));
    QGridLayout *layout = new QGridLayout(panel, 4, 2, marginHint(),
					 spacingHint());

    // First line: Name of the project
    layout->addWidget(new QLabel(i18n("Project name:"), panel), 0, 0);
    layout->addWidget(namefield=new KLineEdit(project.name(), panel), 0, 1);

    // Second line: Name of responsible person
    layout->addWidget(new QLabel(i18n("Project leader:"), panel), 1, 0);
    layout->addWidget(leaderfield = new KLineEdit(project.leader(), panel),
		      1, 1);

    // Last: The description field
    layout->addMultiCellWidget(new QLabel(i18n("Description:"), panel), 2, 2, 0, 1);
    descriptionfield = new QTextEdit(panel);
    descriptionfield->setText(project.description());
    layout->addMultiCellWidget(descriptionfield, 3, 3, 0, 1);
}


void KPTProjectDialog::slotOk() {
    if (namefield->text() == "" || leaderfield->text() == "") {
	KMessageBox::sorry(this, i18n("You have to set a name and leader for "
			   "the project"));
	return;
    }

    project.setName(namefield->text());
    project.setLeader(leaderfield->text());
    project.setDescription(descriptionfield->text());

    accept();
}


#include "kptprojectdialog.moc"
