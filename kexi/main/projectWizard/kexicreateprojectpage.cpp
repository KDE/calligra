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

#include "kexicreateproject.h"
#include "kexicreateprojectpage.h"
#include "kexicreateprojectpage.moc"

#include <kdialog.h>
#include <qlabel.h>
#include <qlayout.h>

KexiCreateProjectPage::KexiCreateProjectPage(KexiCreateProject *parent, QPixmap *wpic, const char *name) 
	: QWidget(parent, name)
	,m_project(parent->project())
	,m_lPic( new QLabel("", this, "m_lPic") )
{
	m_loaded = false;

	m_lPic->setPixmap(*wpic);
	m_lPic->setPaletteBackgroundColor(white);
	m_lPic->setAlignment(AlignTop);
	m_lPic->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	m_lPic->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));

	m_contents = new QWidget(this, "m_contents");

	m_grid = new QGridLayout(this, 1, 2, 0, KDialog::spacingHint() );
	m_grid->addWidget(m_lPic, 0, 0);
	m_grid->addWidget(m_contents, 0, 1);

	parent->registerPage(this);
}

KexiProject *KexiCreateProjectPage::project(){return m_project;}

QVariant
KexiCreateProjectPage::data(const QString &property) const
{
	return m_data[property];
}

void
KexiCreateProjectPage::setProperty(QString property, QVariant data)
{
	m_data[property] = data;
	emit valueChanged(this, property);
}

KexiCreateProjectPage::~KexiCreateProjectPage()
{
}

