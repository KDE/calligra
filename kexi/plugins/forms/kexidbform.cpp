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
#include <qpainter.h>

#include <kdebug.h>

#include <kexiproject.h>
#include <kexidialogbase.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>

#include "kexiformpart.h"
#include "kexidbform.h"

KexiDBForm::KexiDBForm(KexiFormPart *m, KexiFormPartItem &i, KexiMainWindow *win, QWidget *parent, const char *name, KexiDB::Connection *conn, bool preview)
 : KexiViewBase(win, parent, name)
{
	m_part = m;
	m_conn = conn;
	m_item = i;
//	QHBoxLayout *l = new QHBoxLayout(this);
	if(preview)
	{
		QHBoxLayout *l = new QHBoxLayout(this);
		m_preview = new QWidget(this);
		l->addWidget(m_preview);
	}
	else
		m_preview = 0;
	m_buffer = 0;
	m_id = i.item().identifier();

	kdDebug() << "KexiDBForm::KexiDBForm(): connecting" << endl;
	connect(m_part->manager(), SIGNAL(bufferSwitched(KexiPropertyBuffer *)), this, SLOT(managerPropertyChanged(KexiPropertyBuffer *)));
	connect(m_part->manager(), SIGNAL(dirty(KFormDesigner::Form *)), this, SLOT(slotDirty(KFormDesigner::Form *)));

	plugSharedAction("formpart_taborder", m_part->manager(), SLOT(editTabOrder()));
	plugSharedAction("formpart_adjust_size", m_part->manager(), SLOT(adjustWidgetSize()));
}

void
KexiDBForm::initForm()
{
	if(!m_preview)
		m_item.form()->createToplevel(this, this);
	else
	{
		m_item.form()->createToplevel(m_preview);
		m_preview->show();
	}
	loadForm();
	m_part->manager()->importForm(this, m_item.form(), m_preview);
	m_part->addForm(m_id, m_item);
}

void
KexiDBForm::loadForm()
{
	kdDebug() << "KexiDBForm::loadForm() Loading the form with id : " << m_id << endl;
	QString data;
	loadDataBlock(data);//, QString::number(m_id));
	QByteArray raw;
	raw = data.utf8();
	raw.truncate(raw.size() - 1);
	KFormDesigner::FormIO::loadFormData(m_item.form(), this, raw);
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
	// tmp !! We currently save the form in the database when switching to preview, until KexiDialogTempData is ok
	if(dirty() && mode == Kexi::DataViewMode && m_item.form()->objectTree())
		storeData();

	return true;
}

bool
KexiDBForm::afterSwitchFrom(int mode, bool &cancelled)
{
	if(mode == Kexi::DesignViewMode)
	{
		// The form may has been modified, so we must recreate the preview
		delete m_preview;
		m_preview = new QWidget(this);
		m_item.setForm(new KFormDesigner::Form(m_part->manager()));
		m_item.form()->createToplevel(m_preview);
		loadForm();
		m_part->manager()->importForm(m_preview, m_item.form(), true);
		m_preview->show();
		if (!layout())
			(void)new QHBoxLayout(this);
		layout()->add(m_preview);
	}

	return true;
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
	storeDataBlock(data);//, QString::number(m_id));

	return true;
}

//repaint all children widgets
static void repaintAll(QWidget *w)
{
	QObjectList *list = w->queryList("QWidget");
	QObjectListIt it(*list);
	for (QObject *obj; (obj=it.current()); ++it ) {
		static_cast<QWidget*>(obj)->repaint();
	}
	delete list;
}

void
KexiDBForm::drawRect(const QRect& r, int type)
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x()-2, prev_rect.y()-2), buffer, QRect(prev_rect.x()-2, prev_rect.y()-2, prev_rect.width()+4, prev_rect.height()+4));
	}
	p.setBrush(QBrush::NoBrush);
	if(type == 1) // selection rect
		p.setPen(QPen(white, 1, Qt::DotLine));
	else if(type == 2) // insert rect
		p.setPen(QPen(white, 2));
	p.setRasterOp(XorROP);
	p.drawRect(r);
	prev_rect = r;

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

void
KexiDBForm::initRect()
{
	repaintAll(this);
	buffer.resize( width(), height() );
	buffer = QPixmap::grabWindow( winId() );
	prev_rect = QRect();
}

void
KexiDBForm::clearRect()
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	//redraw entire form surface
	p.drawPixmap( QPoint(0,0), buffer, QRect(0,0,buffer.width(), buffer.height()) );

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();

	repaintAll(this);
}

KexiDBForm::~KexiDBForm()
{
	kdDebug() << "KexiDBForm::~KexiDBForm(): close" << endl;
}

#include "kexidbform.moc"

