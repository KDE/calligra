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
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qframe.h>

#include <klocale.h>
#include <kurlrequester.h>
#include <kdialog.h>

#include "kexicreateprojectpagefile.h"

KexiCreateProjectPageFile::KexiCreateProjectPageFile(KexiCreateProject *parent, QPixmap *wpic, const char *name)
 : KexiCreateProjectPage(parent, wpic, name)
{
	//cool picture ;)
	QLabel *lPic = new QLabel("", this);
	lPic->setPixmap(*wpic);
	lPic->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	lPic->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));

	//widgets
	QButtonGroup *buttonBase = new QButtonGroup(this);
	buttonBase->hide();
	m_btnEmbedd = new QRadioButton(i18n("Embed new file into project"), this);
	m_btnEmbedd->toggle();
	m_btnExtern = new QRadioButton(i18n("Use external file reference"), this);
	connect(m_btnExtern, SIGNAL(toggled(bool)), this, SLOT(slotExternToggle(bool)));
	buttonBase->insert(m_btnEmbedd);
	buttonBase->insert(m_btnExtern);

	m_externURL = new KURLRequester(this);

	//layout
	QGridLayout *g = new QGridLayout(this);
	g->addMultiCellWidget(lPic,	0,	4,	0,	0);
//	g->addWidget(buttonBase,	0,	1);
	g->addWidget(m_btnEmbedd,	1,	1);
	g->addWidget(m_btnExtern,	2,	1);
	g->addWidget(m_externURL,	3,	1);
	g->setSpacing(KDialog::spacingHint());

	setProperty("caption", QVariant(i18n("File")));
	setProperty("section", QVariant("LocalDB"));
	setProperty("finish", QVariant(true));
}

void
KexiCreateProjectPageFile::slotExternToggle(bool checked)
{
//	if(checked)
//	{
		m_externURL->setEnabled(checked);
//	}
}

void
KexiCreateProjectPageFile::slotEmbeddToggle(bool)
{
}

KexiCreateProjectPageFile::~KexiCreateProjectPageFile()
{
}

#include "kexicreateprojectpagefile.moc"
