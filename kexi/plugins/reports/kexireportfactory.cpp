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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include <qpopupmenu.h>
#include <qvaluevector.h>

#include <kgenericfactory.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klineedit.h>

#include <container.h>
#include <form.h>
#include <formmanager.h>
#include <widgetlibrary.h>

#include "reportwidgets.h"
#include "kexireportfactory.h"

KexiReportFactory::KexiReportFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::WidgetInfo *wView = new KFormDesigner::WidgetInfo(this);
	wView->setPixmap("report");
	wView->setClassName("KexiReportForm");
	wView->setName(i18n("Report"));
	wView->setNamePrefix(i18n("This string will be used to name widgets of this class. It must _not_ contain white "
	"spaces and non latin1 characters", "Report"));
	wView->setDescription(i18n("A report"));
	m_classes.append(wView);

	KFormDesigner::WidgetInfo *wLabel = new KFormDesigner::WidgetInfo(this);
	wLabel->setPixmap("label");
	wLabel->setClassName("Label");
	wLabel->setName(i18n("Label"));
	wLabel->setNamePrefix(i18n("Widget name (see above)", "Label"));
	wLabel->setDescription(i18n("A label to display text"));
	m_classes.append(wLabel);

	KFormDesigner::WidgetInfo *wPicLabel = new KFormDesigner::WidgetInfo(this);
	wPicLabel->setPixmap("pixmaplabel");
	wPicLabel->setClassName("PicLabel");
	wPicLabel->setName(i18n("Picture Label"));
	wPicLabel->setNamePrefix(i18n("Widget name (see above)", "PicLabel"));
	wPicLabel->setDescription(i18n("A label to display images or icons"));
	m_classes.append(wPicLabel);

	KFormDesigner::WidgetInfo *wLine = new KFormDesigner::WidgetInfo(this);
	wLine->setPixmap("line");
	wLine->setClassName("ReportLine");
	wLine->setName(i18n("Line"));
	wLine->setNamePrefix(i18n("Widget name (see above)", "Line"));
	wLine->setDescription(i18n("A simple line"));
	m_classes.append(wLine);

	KFormDesigner::WidgetInfo *wSubReport = new KFormDesigner::WidgetInfo(this);
	wSubReport->setPixmap("report");
	wSubReport->setClassName("KexiSubReport");
	wSubReport->setName(i18n("Sub Report"));
	wSubReport->setNamePrefix(i18n("Widget name (see above)", "SubReport"));
	wSubReport->setDescription(i18n("A report embedded in another report"));
	m_classes.append(wSubReport);
}

QString
KexiReportFactory::name()
{
	return("kexireportwidgets");
}

QWidget*
KexiReportFactory::create(const QCString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kexipluginsdbg << "KexiReportFactory::create() " << this << endl;

	QString text = container->form()->manager()->lib()->textForWidgetName(n, c);

	if(c == "Label")
		return new Label(text, p, n);
	else if(c == "PicLabel")
		return new PicLabel(DesktopIcon("image"), p, n);
	else if(c == "ReportLine")
		return new ReportLine(p, n);
	else if(c == "KexiSubReport")
		return new KexiSubReport(container->form()->manager(), p, n);

	return 0;
}

bool
KexiReportFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu,
		   KFormDesigner::Container *container, QValueVector<int> *menuIds)
{
	m_widget = w;
	m_container = container;

	if(classname == "Label") {
		int id = menu->insertItem(SmallIconSet("edit"), i18n("Edit Rich Text"), this, SLOT(editText()));
		menuIds->append(id);
		return true;
	}

	return false;
}

void
KexiReportFactory::startEditing(const QString &c, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;

	if(c == "Label") {
		QLabel *label = static_cast<QLabel*>(w);
		if(label->textFormat() == RichText) {
			m_widget = w;
			editText();
		}
		else
			createEditor(label->text(), label, container, label->geometry(), label->alignment());
		return;
	}
	return;
}

bool
KexiReportFactory::showProperty(const QString &classname, QWidget *, const QString &property, bool multiple)
{
	if(classname == "Label") {
		if(property == "pixmap")
			return false;
	}
	else if(classname == "PicLabel") {
		if((property == "text") || (property == "indent") || (property == "textFormat") || (property == "font") || (property == "alignment"))
			return false;
	}

	return !multiple;
}

QStringList
KexiReportFactory::autoSaveProperties(const QString &classname)
{
	QStringList l;

	if(classname == "Label")
		l << "text";
	else if(classname == "PicLabel")
		l << "pixmap";

	return l;
}

void
KexiReportFactory::changeText(const QString &text)
{
	//QString n = WidgetFactory::m_widget->className();
	QWidget *w = WidgetFactory::m_widget;
	changeProperty("text", text, m_container);

	int width = w->sizeHint().width();

	if(w->width() < width)
		w->resize(width, w->height() );
}

void
KexiReportFactory::resizeEditor(QWidget *widget, const QString &)
{
	QSize s = widget->size();
	QPoint p = widget->pos();
	QRect r;

	m_editor->resize(s);
	m_editor->move(p);
}

void
KexiReportFactory::editText()
{
	QString classname = m_widget->className();
	QString text;

	if(classname == "Label")
		text = ((QLabel*)m_widget)->text();

	if(editRichText(m_widget, text)) {
		changeProperty("textFormat", "RichText", m_container);
		changeProperty("text", text, m_container);
	}

	if(classname == "Label")
		m_widget->resize(m_widget->sizeHint());
}

KexiReportFactory::~KexiReportFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(kexireportwidgets, KGenericFactory<KexiReportFactory>("kexireportwidgets"))

#include "kexireportfactory.moc"

