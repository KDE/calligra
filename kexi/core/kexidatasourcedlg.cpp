/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kdialog.h>

#include "kexiproject.h"
#include "kexidatasourcedlg.h"
#include "kexiprojecthandler.h"
#include "kexidatasourcecombobox.h"

KexiDataSourceDlg::KexiDataSourceDlg(KexiProject *p, QWidget *parent, const char *name)
 : QDialog(parent, name)
{
	setCaption(i18n("New Form"));
	QLabel *lDs = new QLabel(i18n("Data source:"), this);
	QLabel *lName = new QLabel(i18n("Name:"), this);

	m_ds = new KexiDataSourceComboBox(this,"kdscb",p);
	m_name = new QLineEdit(this);


	QPushButton *btnOk = new QPushButton(i18n("&OK"), this);
	connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
	QPushButton *btnCancel = new QPushButton(i18n("&Cancel"), this);
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *g = new QGridLayout(this);
	g->setSpacing(KDialog::spacingHint());
	g->addWidget(lDs,		0, 0);
	g->addWidget(lName,		1, 0);
	g->addMultiCellWidget(m_ds,	0, 0, 1, 2);
	g->addMultiCellWidget(m_name,	1, 1, 1, 2);
	g->addWidget(btnOk,		2, 1);
	g->addWidget(btnCancel,		2, 2);
}

QString
KexiDataSourceDlg::source() const
{
	return m_ds->globalIdentifier();
}

QString
KexiDataSourceDlg::name() const
{
	return m_name->text();
}

KexiDataSourceDlg::~KexiDataSourceDlg()
{
}

#include "kexidatasourcedlg.moc"

