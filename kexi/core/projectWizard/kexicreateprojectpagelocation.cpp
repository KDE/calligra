/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>

#include <kdialog.h>
#include <klocale.h>
#include <klineedit.h>

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
	lPic->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	//widgets
	QRadioButton *localRBtn = new QRadioButton(i18n("Local"), this);
	QRadioButton *remoteRBtn = new QRadioButton(i18n("Remote"), this);

	QCheckBox* customSockChk = new QCheckBox(i18n("Custom socket:"), this);
	m_sock = new KLineEdit(this);
	m_sock->setEnabled(false);

	QLabel *lHost = new QLabel(i18n("Host:"), this);
	m_host = new KLineEdit(this);
//	QCheckBox *customPortChk = new QCheckBox(i18n("Custom port:"), this);
	QLabel *lPort = new QLabel(i18n("Port:"), this);
	m_port = new QSpinBox(0, 9999, 1, this);
	m_port ->setSpecialValueText(i18n("default port"));	
	m_port->setEnabled(false);

	//buttongroup hinting
	QButtonGroup* selectBGrp = new QButtonGroup(this);
	selectBGrp->hide();
	selectBGrp->insert(localRBtn);
	selectBGrp->insert(remoteRBtn);
	
	// layout
	QGridLayout *m = new QGridLayout(0);
	m->addMultiCellWidget(localRBtn,	0,	0,	0,	2);
	m->addWidget(customSockChk,		1,	1);
	m->addWidget(m_sock,			1,	2);
	m->addMultiCellWidget(remoteRBtn,	2,	2,	0,	2);
	m->addWidget(lHost,			3,	1);
	m->addWidget(m_host,			3,	2);
//	m->addWidget(customPortChk,		4,	1);
	m->addWidget(lPort,			4,	1);
	m->addWidget(m_port,			4,	2);
	m->setSpacing(KDialog::spacingHint());
	
	QGridLayout *g = new QGridLayout(this);
	g->addMultiCellWidget(lPic,		0,	1,	0,	0);
	g->addLayout(m,		0,	1);
	g->setSpacing(KDialog::spacingHint());
	
	//Connections
	connect(localRBtn, SIGNAL(toggled(bool)), this, SLOT(slotSetLocal(bool)));
	connect(customSockChk, SIGNAL(toggled(bool)), this, SLOT(slotUseCustomSock(bool)));
//	connect(customPortChk, SIGNAL(toggled(bool)), this, SLOT(slotUseCustomPort(bool)));
	connect(m_host, SIGNAL(textChanged(const QString &)), this, SLOT(slotHostChanged(const QString &)));
	connect(m_port, SIGNAL(valueChanged(const QString &)), this, SLOT(slotPortChanged(const QString &)));
	connect(m_sock, SIGNAL(textChanged(const QString &)), this, SLOT(slotSockChanged(const QString &)));

	// Default values
	localRBtn->setChecked(true);
//	customPortChk->setChecked(false);
	customSockChk->setChecked(false);
}

KexiCreateProjectPageLocation::~KexiCreateProjectPageLocation()
{
}

void
KexiCreateProjectPageLocation::slotHostChanged(const QString & t)
{
	setProperty("host", QVariant(t));
}

void
KexiCreateProjectPageLocation::slotSockChanged(const QString & t)
{
	setProperty("socket", QVariant(t));
}

void
KexiCreateProjectPageLocation::slotPortChanged(const QString & t)
{
	setProperty("port", QVariant(t));
}

void
KexiCreateProjectPageLocation::slotUseCustomSock(bool b)
{
	m_sock->setEnabled(b);
	if(b)
	{
		slotSockChanged(m_sock->text());
	}
	else
	{
		slotSockChanged("");
	}
}

void
KexiCreateProjectPageLocation::slotUseCustomPort(bool b)
{
	m_port->setEnabled(b);
	if(b)
	{
		slotPortChanged(m_port->cleanText());
	}
	else
	{
		slotPortChanged("0");
	}
}

void
KexiCreateProjectPageLocation::slotSetLocal(bool b)
{
	m_sock->setEnabled(b);

	m_host->setEnabled(!b);
	m_port->setEnabled(!b);

	if(b)
	{
		slotHostChanged("localhost");
	}
	else
	{
		slotHostChanged(m_host->text());
	}
}

#include "kexicreateprojectpagelocation.moc"
