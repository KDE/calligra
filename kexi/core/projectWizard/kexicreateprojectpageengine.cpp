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

#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>

#include <klocale.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <ktextbrowser.h>
#include <kdialog.h>

#include "kexiDB/kexidbdriver.h"
#include "kexiDB/kexidbinterfacemanager.h"
#include "kexicreateprojectpageengine.h"
#include "kexiproject.h"

KexiCreateProjectPageEngine::KexiCreateProjectPageEngine(KexiCreateProject *parent, QPixmap *wpic, const char *name)
 : KexiCreateProjectPage(parent, wpic, name)
{
	QLabel *lPic = new QLabel("", this);
	lPic->setPixmap(*wpic);
	lPic->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	lPic->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));

	QLabel *lEngine = new QLabel(i18n("Driver: "), this);

	m_engine = new KComboBox(this);
	connect(m_engine, SIGNAL(activated(const QString &)), this, SLOT(slotActivated(const QString &)));

	m_summery = new KTextBrowser(this);

	QGridLayout *g = new QGridLayout(this);
	g->addMultiCellWidget(lPic,	0,	1,	0,	0);
	g->addWidget(lEngine,		0,	1);
	g->addWidget(m_engine,		0,	2);
	g->addMultiCellWidget(m_summery,1,	1,	1,	2);
	g->setSpacing(KDialog::spacingHint());

	fill();

	setProperty("section", QVariant("Both"));
	setProperty("caption", QVariant(i18n("Engine")));
}

void
KexiCreateProjectPageEngine::fill()
{
	QStringList drivers = project()->manager()->drivers();
	for(QStringList::Iterator it = drivers.begin(); it != drivers.end(); ++it)
	{
		m_engine->insertItem(*it);
	}

	if(!m_engine->currentText().isEmpty())
	{
		setProperty("engine", QVariant(m_engine->currentText()));
		setProperty("continue", QVariant(true));
	}

	fillSummery();
}

void
KexiCreateProjectPageEngine::fillSummery()
{
	QString engineSummery = project()->manager()->driverInfo(m_engine->currentText())->service()->comment();
	QString userSummery = QString("<b>" + m_engine->currentText() + "</b><br><hr><br>" + engineSummery);


	if(!m_engine->currentText().isEmpty())
	{
		m_summery->setText(userSummery);
	}

	QVariant location = project()->manager()->driverInfo(m_engine->currentText())->service()->property("X-Kexi-Location");
	setProperty("location", location);
}

void
KexiCreateProjectPageEngine::slotActivated(const QString &engine)
{
	setProperty("engine", QVariant(engine));
	setProperty("continue", QVariant(true));
	fillSummery();
}

KexiCreateProjectPageEngine::~KexiCreateProjectPageEngine()
{
}

#include "kexicreateprojectpageengine.moc"
