/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include <qpainter.h>
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <Q3Frame>
#include <QPaintEvent>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <kexidb/utils.h>
#include <kexidb/connection.h>
#include <kexipart.h>

#include "kexireportview.h"
#include "reportwidgets.h"

Label::Label(const QString &text, QWidget *parent, const char *name)
: QLabel(text, parent, name)
{
	setPaletteBackgroundColor(Qt::white);
}

////////////////////////////////////////////////////////////////////

ReportLine::ReportLine(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_lineStyle = (ReportLineStyle)Qt::SolidLine;
	m_lineWidth = 1;
	m_capStyle = (CapStyle)Qt::FlatCap;
	m_color = paletteForegroundColor();
	setPaletteBackgroundColor(Qt::white);
}

ReportLine::ReportLineStyle
ReportLine::lineStyle() const
{
	return m_lineStyle;
}

void
ReportLine::setLineStyle(ReportLineStyle style)
{
	m_lineStyle = style;
	update();
}

int
ReportLine::lineWidth() const
{
	return m_lineWidth;
}

void
ReportLine::setLineWidth(int width)
{
	m_lineWidth = width;
	update();
}

QColor
ReportLine::color() const
{
	return m_color;
}

void
ReportLine::setColor(const QColor &color)
{
	m_color = color;
	update();
}

ReportLine::CapStyle
ReportLine::capStyle() const
{
	return m_capStyle;
}

void
ReportLine::setCapStyle(CapStyle capStyle)
{
	m_capStyle = capStyle;
	update();
}

void
ReportLine::paintEvent (QPaintEvent *ev)
{
	QPainter p(this);
	if(!ev->erased())
		p.eraseRect(0, 0, width(), height());
	QPen pen(m_color, m_lineWidth, (Qt::PenStyle)m_lineStyle);
	pen.setCapStyle((Qt::PenCapStyle)m_capStyle);
	p.setPen(pen);
	p.drawLine(0, 0, width() -1, height() - 1);
}

////////////////////////////////////////////////////////////////////


PicLabel::PicLabel(const QPixmap &pix, QWidget *parent, const char *name)
 : QLabel(parent, name)
{
	setPixmap(pix);
	setScaledContents(false);
	setPaletteBackgroundColor(Qt::white);
}

bool
PicLabel::setProperty(const char *name, const QVariant &value)
{
	if(QString(name) == "pixmap")
		resize(value.toPixmap().height(), value.toPixmap().width());
	return QLabel::setProperty(name, value);
}

////////////////////////////////////////////////////////////////////

KexiSubReport::KexiSubReport(QWidget *parent, const char *name)
: Q3ScrollView(parent, name), m_form(0), m_widget(0)
{
	setFrameStyle(Q3Frame::Plain | Q3Frame::Box);
	viewport()->setPaletteBackgroundColor(Qt::white);
}

void
KexiSubReport::setReportName(const QString &name)
{
	if(name.isEmpty())
		return;

	// we need a KexiReportView*
	QWidget *w = parentWidget();
	while(w && !w->isA("KexiReportView"))
		w = w->parentWidget();
	KexiReportView *view = (KexiReportView*)w;
	if(!view)
		return;

	// we check if there is a form with this name
	int id = KexiDB::idForObjectName(*(view->connection()), name, KexiPart::ReportObjectType);
	if((id == 0) || (id == view->window()->id())) // == our form
		return; // because of recursion when loading

	// we create the container widget
	delete m_widget;
	m_widget = new QWidget(viewport(), "kexisubreport_widget");
	m_widget->show();
	addChild(m_widget);
	m_form = new Form(KexiReportPart::library(), this->name());
	m_form->createToplevel(m_widget);

	// and load the sub form
	QString data;
	tristate res = view->connection()->loadDataBlock(id, data , QString());
	if(res != true)
		return;

	KFormDesigner::FormIO::loadFormFromString(m_form, m_widget, data);
	m_form->setDesignMode(false);

	m_reportName = name;
}

#include "reportwidgets.moc"

