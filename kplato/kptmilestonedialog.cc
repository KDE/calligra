/* This file is part of the KDE project
   Copyright (C) 2002 The koffice team koffice@kde.org

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

#include "kptmilestonedialog.h"
#include "kptmilestone.h"

#include <qlayout.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <qtextedit.h>


KPTMilestoneDialog::KPTMilestoneDialog(KPTMilestone &ms, QWidget *p, const char *n)
    : KDialogBase(Tabbed, i18n("Milestone Settings"), Ok|Cancel, Ok, p,
		  n, true, true), m_ms(ms)
{
    // For now the setup is pretty trivial. It's planned to be able to control
    // the children here too.
    QWidget *panel = addPage(i18n("Settings"));
    QGridLayout *layout = new QGridLayout(panel, 4, 2, marginHint(),
					 spacingHint());

    // Name of the milestone
    layout->addWidget(new QLabel(i18n("Milestone name:"), panel), 0, 0);
    layout->addWidget(m_namefield=new KLineEdit(m_ms.name(), panel), 0, 1);

    // The description field
    layout->addMultiCellWidget(new QLabel(i18n("Description:"), panel), 2, 2,
			       0, 1);
    m_descriptionfield = new QTextEdit(panel);
    m_descriptionfield->setText(m_ms.description());
    layout->addMultiCellWidget(m_descriptionfield, 3, 3, 0, 1);
}


void KPTMilestoneDialog::slotOk() {
    if (m_namefield->text() == "" ) {
	KMessageBox::sorry(this, i18n("You have to set a name for this milestone"));
	return;
    }

    m_ms.setName(m_namefield->text());
    m_ms.setDescription(m_descriptionfield->text());

    accept();
}


#include "kptmilestonedialog.moc"
