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

#include <qobjectlist.h>
#include <qlayout.h>

#include <kdebug.h>

#include <kexiproject.h>
#include <kexidialogbase.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>

#include "kexiformpart.h"
#include "kexidbform.h"

KexiDBForm::KexiDBForm(KexiFormPart *m, KexiFormPartItem &i, KexiMainWindow *win, QWidget *parent, const char *name, KexiDB::Connection *conn)
 : KexiViewBase(win, parent, name)
{
	m_part = m;
	m_conn = conn;
	m_item = i;
	m_preview = 0;
	m_buffer = 0;
	m_id = i.item().identifier();

	kdDebug() << "KexiDBForm::KexiDBForm(): connecting" << endl;
	connect(m->manager(), SIGNAL(bufferSwitched(KexiPropertyBuffer *)), this, SLOT(managerPropertyChanged(KexiPropertyBuffer *)));
	connect(m->manager(), SIGNAL(dirty(KFormDesigner::Form *)), this, SLOT(slotDirty(KFormDesigner::Form *)));
}

void
KexiDBForm::initForm()
{
	QString data;
	loadDataBlock(data, QString::number(m_id));
	QByteArray raw;
	raw = data.utf8();
	raw.truncate(raw.size() - 1);
	KFormDesigner::FormIO::loadFormData(m_item.form(), this, raw);
	m_part->addForm(m_id, m_item);
}

void
KexiDBForm::managerPropertyChanged(KexiPropertyBuffer *b)
{
	kdDebug() << "KexiDBForm::managerPropertyChanged(): KUKU!!!!" << endl;
	m_buffer = b;
	propertyBufferSwitched();
}

bool
KexiDBForm::beforeSwitchTo(int mode, bool &cancelled, bool &dontStore)
{
	kdDebug() << "KexiDBForm::beforeSwitchTo(): " << mode << " using " << m_item.form() <<  endl;
	if (m_item.form()->objectTree())
		m_item.form()->objectTree()->debug();
	QByteArray data;
	KFormDesigner::FormIO::saveForm(m_item.form(), data);
	kdDebug() << "KexiDBForm::beforeSwitchTo(): data follows:\n" << QString(data) << endl;

/*
	if(mode == Kexi::DataViewMode) //save and prepare preview
	{
		QByteArray fd;
		KFormDesigner::FormIO::saveForm(m_part->manager()->activeForm(), fd);
		kdDebug() << "KexiDBForm::beforeSwitchTo(): data: \n" << QString(fd) << endl;

		m_part->saveForm(m_conn, m_item.item(), fd);
	}
*/

	return true;
}

bool
KexiDBForm::afterSwitchFrom(int mode, bool &cancelled)
{
	if(mode == Kexi::DesignViewMode)
	{
		preview();
	}

	return true;
}

void
KexiDBForm::preview()
{
	delete m_preview;
	m_preview = new QWidget(this);
//		QHBoxLayout *l = new QHBoxLayout(this);
//		l->addWidget(m_preview);
	m_part->manager()->previewForm(m_item.form(), m_preview);
	if (m_item.form()->objectTree())
		m_item.form()->objectTree()->debug();

	kdDebug() << "KexiDBForm::afterSwitchFrom(): preview!: using " << m_item.form() << endl;

	m_preview->show();

	QLayout *l = layout();
	if(!l)
		l = new QHBoxLayout(this);

	l->add(m_preview);
}

void
KexiDBForm::slotDirty(KFormDesigner::Form *form)
{
	if(form == m_item.form())
		KexiViewBase::setDirty(true);
}

KexiDB::SchemaData*
KexiDBForm::storeNewData(const KexiDB::SchemaData& sdata)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata);
	kdDebug() << "KexiDBForm::storeNewData(): new id:" << s->id() << endl;

	m_id = s->id();

	parentDialog()->setId(s->id());
	storeData();
	return s;
}

bool
KexiDBForm::storeData()
{
	kdDebug(44000) << "KexiDBForm::storeData(): " << parentDialog()->partItem()->name() << " [" << m_id << "]" << endl;
	QByteArray data;
	KFormDesigner::FormIO::saveForm(m_item.form(), data);
	storeDataBlock(data, QString::number(m_id));

	return true;
}

KexiDBForm::~KexiDBForm()
{
	kdDebug() << "KexiDBForm::~KexiDBForm(): close" << endl;
}

#include "kexidbform.moc"

