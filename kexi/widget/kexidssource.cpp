/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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
#include <qcheckbox.h>
#include <qgroupbox.h>

#include <klocale.h>
#include <kdebug.h>

#include <keximainwindow.h>
#include "kexidatasourcecombo.h"
#include "kexidatasourcefields.h"
#include "kexidatasourcewizard.h"
#include "kexidssource.h"

KexiDSSource::KexiDSSource(KexiMainWindow *win, KexiDataSourceWizard *parent)
 : QWidget(parent)
{
	m_wiz = parent;
	KexiDSPixmap *pic = new KexiDSPixmap(this);

	QLabel *lText = new QLabel(i18n("Datasource:"), this);
	KexiDataSourceCombo *source = new KexiDataSourceCombo(win, this, 0);

	QGroupBox *btn = new QGroupBox(1, Horizontal, i18n("Fields"), this);
	DSInfo initial = source->selectedSource();

	m_fields = new KexiDataSourceFields(win->project(), btn);
	m_fields->setDataSource(initial.first, initial.second);
	connect(m_fields, SIGNAL(listChanged()), this, SLOT(fieldsChanged()));

	connect(source, SIGNAL(dataSourceChanged(KexiPart::DataSource *, const KexiPart::Item &)), m_fields,
			SLOT(setDataSource(KexiPart::DataSource *, const KexiPart::Item &)));

	connect(source, SIGNAL(dataSourceChanged(KexiPart::DataSource *, const KexiPart::Item &)), m_wiz,
			SLOT(setDataSource(KexiPart::DataSource *, const KexiPart::Item &)));

	QGridLayout *g = new QGridLayout(this);
	g->setSpacing(3);

	g->addMultiCellWidget(pic, 0, 1, 0, 0);
	g->addWidget(lText, 0, 1);
	g->addWidget(source, 0, 2);
	g->addMultiCellWidget(btn, 1, 1, 1, 2);

}

void
KexiDSSource::fieldsChanged()
{
	m_wiz->setFieldList(m_fields->usedFields());
}

KexiDSSource::~KexiDSSource()
{
}

#include "kexidssource.moc"

