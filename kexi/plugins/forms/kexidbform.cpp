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

#include <kexidialogbase.h>
#include <kexidatasourcewizard.h>
#include <kexidb/fieldlist.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>

#include "kexidbform.h"

//#define NO_DSWIZARD

KexiDBForm::KexiDBForm(/*KexiFormPartItem &i,*/ KexiMainWindow *win, QWidget *parent, const char *name, KexiDB::Connection *conn, bool preview)
 : KexiViewBase(win, parent, name)
{
	m_conn = conn;
	if(preview)
	{
		QHBoxLayout *l = new QHBoxLayout(this);
		m_preview = new QWidget(this);
		l->addWidget(m_preview);
	}
	else
		m_preview = 0;
	m_buffer = 0;

	kdDebug() << "KexiDBForm::KexiDBForm(): connecting" << endl;
	if(!preview)
	{
		connect(formPart()->manager(), SIGNAL(bufferSwitched(KexiPropertyBuffer *)), this, SLOT(managerPropertyChanged(KexiPropertyBuffer *)));
		connect(formPart()->manager(), SIGNAL(dirty(KFormDesigner::Form *)), this, SLOT(slotDirty(KFormDesigner::Form *)));
	}

	plugSharedAction("formpart_taborder", formPart()->manager(), SLOT(editTabOrder()));
	plugSharedAction("formpart_adjust_size", formPart()->manager(), SLOT(adjustWidgetSize()));

	initForm();
}

KFormDesigner::Form*
KexiDBForm::form() const
{
	if(m_preview)
		return tempData()->previewForm;
	else
		return tempData()->form;
}

void
KexiDBForm::setForm(KFormDesigner::Form *f)
{
	if(m_preview)
		tempData()->previewForm = f;
	else
		tempData()->form = f;
}

void
KexiDBForm::initForm()
{
	setForm( new KFormDesigner::Form(formPart()->manager()) );
	if(!m_preview)
		form()->createToplevel(this, this);
	else
	{
		form()->createToplevel(m_preview);
		m_preview->show();
	}

	// Show the form wizard if this is a new Form
	KexiDB::FieldList *fields = 0;
	if(parentDialog()->id() < 0)
	{
#ifndef NO_DSWIZARD
		KexiDataSourceWizard *w = new KexiDataSourceWizard(mainWin(), (QWidget*)mainWin(), "datasource_wizard");
		if(!w->exec())
			fields = 0;
		else
			fields = w->fields();
		delete w;
#endif
	}
	else
		fields = 0;

	if(fields)
	{
		QDomDocument dom;
		formPart()->generateForm(fields, dom);
		KFormDesigner::FormIO::loadFormFromDom(form(), this, dom);
	}
	else
		loadForm();

	formPart()->manager()->importForm(this, form(), m_preview);
}

void
KexiDBForm::loadForm()
{
	kdDebug() << "KexiDBForm::loadForm() Loading the form with id : " << parentDialog()->id() << endl;
	// If we are previewing the Form, use the tempData instead of the form stored in the db
	if(m_preview && (tempData()->tempForm.size() != 0) )
	{
		KFormDesigner::FormIO::loadFormData(form(), this, tempData()->tempForm);
		return;
	}

	// normal load
	QString data;
	loadDataBlock(data);//, QString::number(m_id));
	QByteArray raw;
	raw = data.utf8();
	raw.truncate(raw.size() - 1);
	KFormDesigner::FormIO::loadFormData(form(), this, raw);
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
	// we don't store on db, but in our TempData
	dontStore = true;
	if(dirty() && (mode == Kexi::DataViewMode) && form()->objectTree())
	{
		KFormDesigner::FormIO::saveForm(form(), tempData()->tempForm);
		tempData()->tempForm.truncate(tempData()->tempForm.size() - 1);
	}

	return true;
}

bool
KexiDBForm::afterSwitchFrom(int mode, bool &cancelled)
{
	if((mode == Kexi::DesignViewMode) && m_preview) //aka !preview
	{
		// The form may has been modified, so we must recreate the preview
		delete m_preview;
		m_preview = new QWidget(this);
		KFormDesigner::Form *prevForm = form();
		setForm( new KFormDesigner::Form(formPart()->manager()) );
		form()->createToplevel(m_preview);
		loadForm();
		formPart()->manager()->importForm(m_preview, form(), true);
		m_preview->show();
		if (!layout())
			(void)new QHBoxLayout(this);
		layout()->add(m_preview);
		formPart()->manager()->deleteForm( prevForm );
		delete prevForm;
	}

	return true;
}

void
KexiDBForm::slotDirty(KFormDesigner::Form *dirtyForm)
{
	if(dirtyForm == form())
		KexiViewBase::setDirty(true);
}

KexiDB::SchemaData*
KexiDBForm::storeNewData(const KexiDB::SchemaData& sdata)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata);
	kdDebug() << "KexiDBForm::storeNewData(): new id:" << s->id() << endl;

	storeData();
	return s;
}

bool
KexiDBForm::storeData()
{
	kdDebug(44000) << "KexiDBForm::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;
	QByteArray data;
	KFormDesigner::FormIO::saveForm(form(), data);
	storeDataBlock(data);//, QString::number(m_id));
	tempData()->tempForm = QByteArray();

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

void
KexiDBForm::highlightWidgets(QWidget *from, QWidget *to)//, const QPoint &point)
{
	QPoint fromPoint, toPoint;
	fromPoint = mapFrom(from->parentWidget(), from->pos());
	if(to)
		toPoint = mapFrom(to->parentWidget(), to->pos());

	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x(), prev_rect.y()), buffer, QRect(prev_rect.x(), prev_rect.y(), prev_rect.width(), prev_rect.height()));
	}

	p.setPen( QPen(Qt::red, 2) );

	if(to)
	{
		QPixmap pix1 = QPixmap::grabWidget(from);
		QPixmap pix2 = QPixmap::grabWidget(to);

		/*if(from == this)
			p.drawLine( point, mapFrom(to->parentWidget(), to->geometry().center()) );
		else if(to == this)
			p.drawLine( mapFrom(from->parentWidget(), from->geometry().center()), point);
		else*/
		p.drawLine( mapFrom(from->parentWidget(), from->geometry().center()), mapFrom(to->parentWidget(), to->geometry().center()) );

		p.drawPixmap(fromPoint.x(), fromPoint.y(), pix1);
		p.drawPixmap(toPoint.x(), toPoint.y(), pix2);

		if(to == this)
			p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
		else
			p.drawRoundRect(toPoint.x(), toPoint.y(), to->width(), to->height(), 5, 5);
	}

	if(from == this)
		p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
	else
		p.drawRoundRect(fromPoint.x(),  fromPoint.y(), from->width(), from->height(), 5, 5);

	if((to == this) || (from == this))
		prev_rect = QRect(0, 0, buffer.width(), buffer.height());
	else if(to)
	{
		prev_rect.setX( (fromPoint.x() < toPoint.x()) ? (fromPoint.x() - 5) : (toPoint.x() - 5) );
		prev_rect.setY( (fromPoint.y() < toPoint.y()) ? (fromPoint.y() - 5) : (toPoint.y() - 5) );
		prev_rect.setRight( (fromPoint.x() < toPoint.x()) ? (toPoint.x() + to->width() + 5) : (fromPoint.x() + from->width() + 5) );
		prev_rect.setBottom( (fromPoint.y() < toPoint.y()) ? (toPoint.y() + to->height() + 5) : (fromPoint.y() + from->height() + 5) ) ;
	}
	else
		prev_rect = QRect(fromPoint.x()- 5,  fromPoint.y() -5, from->width() + 10, from->height() + 10);

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

KexiDBForm::~KexiDBForm()
{
	kdDebug() << "KexiDBForm::~KexiDBForm(): close" << endl;
}

#include "kexidbform.moc"

