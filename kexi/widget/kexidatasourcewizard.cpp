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

#include <kstandarddirs.h>
#include <qpushbutton.h>
#include <qpixmap.h>

#include "kexidswelcome.h"
#include "kexidssource.h"
#include "kexidatasourcewizard.h"

class KexiDataSourceWizard_Pixmap
{
	public:
	KexiDataSourceWizard_Pixmap() { pic=0; }
	~KexiDataSourceWizard_Pixmap() { delete pic; }
	QPixmap *pic; 
};

static KexiDataSourceWizard_Pixmap KexiDataSourceWizard_pic;

KexiDataSourceWizard::KexiDataSourceWizard(KexiMainWindow *win, QWidget *parent, const char *name)
 : KWizard(parent, name)
{
	m_win = win;
	m_finishNext = false;
	m_fields = 0;
	m_used = true;

	(void)KexiDataSourceWizard::pixmap();

	addPage(new KexiDSWelcome(this), "Form Wizard");
	QWidget *wds = new KexiDSSource(win, this);
	addPage(wds, "Form Wizard");
	setFinishEnabled(wds, true);

	connect(nextButton(), SIGNAL(clicked()), this, SLOT(next()));
}

QPixmap &
KexiDataSourceWizard::pixmap()
{
	if (!KexiDataSourceWizard_pic.pic)
		KexiDataSourceWizard_pic.pic = new QPixmap(locate("data","kexi/pics/cp-wiz.png"));
	return *KexiDataSourceWizard_pic.pic;
}

void
KexiDataSourceWizard::next()
{
	if(m_finishNext)
		accept();

	KWizard::next();
}

void
KexiDataSourceWizard::setFieldList(KexiDB::FieldList *fl)
{
	delete m_fields;
	m_fields = fl;
}

void
KexiDataSourceWizard::setDataSource(const DSInfo &ds)
{
	m_ds = DSInfo(ds);
}

KexiDataSourceWizard::~KexiDataSourceWizard()
{
}

// DATASOURCE WIZARD PIXMAP

KexiDSPixmap::KexiDSPixmap(QWidget *parent)
 : QLabel("", parent)
{
	setPixmap(KexiDataSourceWizard::pixmap());
	setPaletteBackgroundColor(white);
	setAlignment(AlignTop);
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));
}

KexiDSPixmap::~KexiDSPixmap()
{
}

#include "kexidatasourcewizard.moc"
