/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#include <qcheckbox.h>
#include <qlistbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>

#include <kfiledialog.h>

#include "kexiproject.h"
#include "kexistartupdlg.h"

KexiStartupDlg::KexiStartupDlg(QWidget *parent)
 : KexiStartupDlgUI(parent)
{
	connect(alternativeDS, SIGNAL(clicked()), this, SLOT(checkNew()));
	connect(recentFiles, SIGNAL(clicked(QListBoxItem *)), this, SLOT(checkRecent()));

	connect(openRecent, SIGNAL(clicked()), this, SLOT(openRecentClicked()));
	connect(createProject, SIGNAL(clicked()), this, SLOT(otherClicked()));
	connect(openProject, SIGNAL(clicked()), this, SLOT(otherClicked()));
}

void
KexiStartupDlg::checkRecent()
{
	openRecent->setChecked(true);
	btnOk->setEnabled(true);
}

void
KexiStartupDlg::checkNew()
{
	if(alternativeDS->isChecked())
		createProject->setChecked(true);

	btnOk->setEnabled(true);
}

void
KexiStartupDlg::openRecentClicked()
{
	if(recentFiles->currentItem() == -1)
		btnOk->setEnabled(false);
}

void
KexiStartupDlg::otherClicked()
{
	if(!createProject->isChecked())
		alternativeDS->setChecked(false);

	btnOk->setEnabled(true);
}

void
KexiStartupDlg::accept()
{
	if(createProject->isChecked() && alternativeDS->isChecked())
	{
		done(CreateNewAlternative);
	}
	else if(createProject->isChecked())
	{
		done(CreateNew);
	}
	else if(openProject->isChecked())
	{
		m_file = KFileDialog::getOpenFileName(QString::null, "*.kexi");
		done(OpenExisting);
	}
	else
	{
		done(OpenRecent);
	}
}

void
KexiStartupDlg::reject()
{
	done(Cancel);
}

KexiStartupDlg::~KexiStartupDlg()
{
}

#include "kexistartupdlg.moc"

