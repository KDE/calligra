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

	//widgets
	QRadioButton *localRBtn = new QRadioButton(i18n("Local"), m_contents);
	QRadioButton *remoteRBtn = new QRadioButton(i18n("Remote"), m_contents);

	m_customSockChk = new QCheckBox(i18n("Custom socket:"), m_contents);
	m_sock = new KLineEdit(m_contents);
	m_sock->setEnabled(false);

	m_lHost = new QLabel(i18n("Host:"), m_contents);
	m_host = new KLineEdit(m_contents);
	m_lPort = new QLabel(i18n("Port:"), m_contents);
	m_port = new QSpinBox(0, 65535, 1, m_contents);
	m_port ->setSpecialValueText(i18n("default port"));	
	m_port->setEnabled(false);

	//buttongroup hinting
	QButtonGroup* selectBGrp = new QButtonGroup(m_contents);
	selectBGrp->hide();
	selectBGrp->insert(localRBtn);
	selectBGrp->insert(remoteRBtn);
	
	// layout
	QGridLayout *m = new QGridLayout(m_contents);
	m->addMultiCellWidget(localRBtn,	0,	0,	0,	2);
	m->addWidget(m_customSockChk,		1,	1);
	m->addWidget(m_sock,			1,	2);
	m->addMultiCellWidget(remoteRBtn,	2,	2,	0,	2);
	m->addWidget(m_lHost,			3,	1);
	m->addWidget(m_host,			3,	2);
	m->addWidget(m_lPort,			4,	1);
	m->addWidget(m_port,			4,	2);
	m->setSpacing(KDialog::spacingHint());
	m->setRowStretch(5, 1);
	m->setColStretch(3, 1);

	//Connections
	connect(localRBtn, SIGNAL(toggled(bool)), this, SLOT(slotSetLocal(bool)));
	connect(m_customSockChk, SIGNAL(toggled(bool)), this, SLOT(slotUseCustomSock(bool)));
//	connect(m_customPortChk, SIGNAL(toggled(bool)), this, SLOT(slotUseCustomPort(bool)));
	connect(m_host, SIGNAL(textChanged(const QString &)), this, SLOT(slotHostChanged(const QString &)));
	connect(m_port, SIGNAL(valueChanged(const QString &)), this, SLOT(slotPortChanged(const QString &)));
	connect(m_sock, SIGNAL(textChanged(const QString &)), this, SLOT(slotSockChanged(const QString &)));

	// Default values
	localRBtn->setChecked(true);
//	customPortChk->setChecked(false);
	slotUseCustomSock(false);
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

/*void
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
}*/

void
KexiCreateProjectPageLocation::slotSetLocal(bool b)
{
	m_sock->setEnabled(b);
	m_customSockChk->setEnabled(b);
	if (b)
		slotUseCustomSock(m_customSockChk->isOn());

	m_host->setEnabled(!b);
	m_port->setEnabled(!b);
	m_lHost->setEnabled(!b);
	m_lPort->setEnabled(!b);

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
