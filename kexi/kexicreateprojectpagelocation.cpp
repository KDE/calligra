/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>

#include <kdialog.h>
#include <klocale.h>

#include "kexicreateprojectpagelocation.h"

KexiCreateProjectPageLocation::KexiCreateProjectPageLocation(KexiCreateProject *parent, QPixmap *wpic, const char *name)
 : KexiCreateProjectPage(parent, wpic, name)
{
	//page settings
	setProperty("section", QVariant("RemoteDB"));
	setProperty("caption", QVariant(i18n("Database Location")));

	//cool pic
	QLabel *lPic = new QLabel("", this);
	lPic->setPixmap(*wpic);
	lPic->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	lPic->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));

	//widgets
	m_localRBtn = new QRadioButton(i18n("Local"), this);
	m_remoteRBtn = new QRadioButton(i18n("Remote"), this);

	#warning make it global
	QCheckBox *m_customSockChk = new QCheckBox(i18n("Custom socket:"), this);
	QLineEdit *m_sock = new QLineEdit(this);
	
	QLabel *lHost = new QLabel(i18n("Host:"), this);
	QLineEdit *m_host = new QLineEdit(this);
	QCheckBox *m_customPortChk = new QCheckBox(i18n("Custom port:"), this);
	QSpinBox *m_port = new QSpinBox(0, 9999, 1, this);

	//buttongroup hinting
	QButtonGroup* selectBGrp = new QButtonGroup(this);
	selectBGrp->hide();
	selectBGrp->insert(m_localRBtn);
	selectBGrp->insert(m_remoteRBtn);
	
	// layout
	QSpacerItem *identH = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	
	QGridLayout *g = new QGridLayout(this);
	g->addMultiCellWidget(lPic,		0,	5,	0,	0);
	g->addMultiCellWidget(m_localRBtn,	0,	0,	1,	2);
	g->addWidget(m_customSockChk,		1,	2);
	g->addWidget(m_sock,			1,	3);
	g->addMultiCellWidget(m_remoteRBtn,	2,	2,	1,	2);
	g->addWidget(lHost,			3,	2);
	g->addWidget(m_host,			3,	3);
	g->addWidget(m_customPortChk,		4,	2);
	g->addWidget(m_port,			4,	3);
	
	g->addItem(identH,			5,	1);

	g->setSpacing(KDialog::spacingHint());
}

KexiCreateProjectPageLocation::~KexiCreateProjectPageLocation()
{
}

#include "kexicreateprojectpagelocation.moc"
