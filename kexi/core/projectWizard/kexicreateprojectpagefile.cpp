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
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qdatetime.h>

#include <kinstance.h>
#include <kglobal.h>
#include <koApplication.h>
#include <klocale.h>
#include <kurlrequester.h>
#include <kdialog.h>
#include <kstandarddirs.h>

#include "kexicreateprojectpagefile.h"

KexiCreateProjectPageFile::KexiCreateProjectPageFile(KexiCreateProject *parent, QPixmap *wpic, const char *name)
 : KexiCreateProjectPage(parent, wpic, name)
{
	//cool picture ;)
/*	QLabel *lPic = new QLabel("", this);
	lPic->setPixmap(*wpic);
	lPic->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	lPic->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));
*/
	//widgets
	QButtonGroup *buttonBase = new QButtonGroup(m_contents);
	buttonBase->hide();
//	QCheckBox *bpers = new QCheckBox(i18n(), this);
	m_btnEmbedd = new QRadioButton(
		i18n("Embed database into projectfile\nWARNING: You will lose transactional and\nintegrity protection.")
		, m_contents);
	m_btnEmbedd->toggle();
	m_btnExtern = new QRadioButton(i18n("Database is persistent"), m_contents);
	connect(m_btnExtern, SIGNAL(toggled(bool)), this, SLOT(slotExternToggle(bool)));
	buttonBase->insert(m_btnEmbedd);
	buttonBase->insert(m_btnExtern);

	m_externURL = new KURLRequester(m_contents);
	m_externURL->setEnabled(false);
	getFile();

	//layout
	QGridLayout *g = new QGridLayout(m_contents);
//	g->addMultiCellWidget(lPic,	0,	4,	0,	0);
//	g->addWidget(buttonBase,	0,	1);
//	g->addWidget(bpers,		0,	1);
	g->addWidget(m_btnEmbedd,	0,	0);
	g->addWidget(m_btnExtern,	1,	0);
	g->addWidget(m_externURL,	2,	0);
	g->setSpacing(KDialog::spacingHint());

	setProperty("caption", QVariant(i18n("File")));
	setProperty("section", QVariant("LocalDB"));
	setProperty("persistant", QVariant(false));
	setProperty("finish", QVariant(true));
}

void
KexiCreateProjectPageFile::getFile()
{
	QDate d = QDate::currentDate();
	QString date = d.toString(ISODate);
//	QString dir = kapp->dirs()->saveLocation("data","kexi/" + date, false);
	QString relative = "kexi/" + date;
	kapp->dirs()->saveLocation("data", "kexi/", true);
	QString dir = kapp->dirs()->saveLocation("data", relative);

	m_externURL->setURL(kapp->dirs()->saveLocation("data",relative, false));
	setProperty("ref", QVariant(dir));
}

void
KexiCreateProjectPageFile::slotExternToggle(bool checked)
{
	m_externURL->setEnabled(checked);
	setProperty("persistant", QVariant(checked));
}

void
KexiCreateProjectPageFile::urlSelected(const QString &url)
{
	setProperty("ref", QVariant(url));
}

void
KexiCreateProjectPageFile::slotEmbeddToggle(bool)
{
}

KexiCreateProjectPageFile::~KexiCreateProjectPageFile()
{
}

#include "kexicreateprojectpagefile.moc"
