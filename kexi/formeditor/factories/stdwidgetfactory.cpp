/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch <lucijan@kde.org>                 *
 *   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qlabel.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qobjectlist.h>
#include <qstring.h>
#include <qvariant.h>
#include <qheader.h>
#include <qdom.h>

#include <klineedit.h>
#include <kpushbutton.h>
#include <knuminput.h>
#include <kcombobox.h>
#include <klistbox.h>
#include <ktextedit.h>
#include <klistview.h>
#include <kprogress.h>
#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdeversion.h>

#if !KDE_IS_VERSION(3,1,9) //TMP
# include <qdatetimeedit.h>
# define KTimeWidget QTimeEdit
# define KDateWidget QDateEdit
# define KDateTimeWidget QDateTimeEdit
#else
# include <ktimewidget.h>
# include <kdatewidget.h>
# include <kdatetimewidget.h>
#endif

#include "spacer.h"
#include "formIO.h"
#include "stdwidgetfactory.h"

// Some widgets subclass to allow event filtering and some other things
MyPicLabel::MyPicLabel(const QPixmap *pix, QWidget *parent, const char *name)
 : QLabel(parent, name)
{
	setPixmap(*pix);
	setScaledContents(false);
}

bool
MyPicLabel::setProperty(const char *name, const QVariant &value)
{
	if(QString(name) == "pixmap")
		resize(value.toPixmap().height(), value.toPixmap().width());
	return QLabel::setProperty(name, value);
}

Line::Line(Qt::Orientation orient, QWidget *parent, const char *name)
 : QFrame(parent, name)
{
	setFrameShadow(Sunken);
	if(orient == Horizontal)
		setFrameShape(HLine);
	else
		setFrameShape(VLine);
}

void
Line::setOrientation(Qt::Orientation orient)
{
	if(orient == Horizontal)
		setFrameShape(HLine);
	else
		setFrameShape(VLine);
}

Qt::Orientation
Line::orientation() const
{
	if(frameShape() == HLine)
		return Horizontal;
	else
		return Vertical;
}

// The factory itself

StdWidgetFactory::StdWidgetFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	m_classes.setAutoDelete(true);

	KFormDesigner::Widget *wLabel = new KFormDesigner::Widget(this);
	wLabel->setPixmap("label");
	wLabel->setClassName("QLabel");
	wLabel->setName(i18n("Text Label"));
	wLabel->setDescription(i18n("A widget to display text"));
	m_classes.append(wLabel);

	KFormDesigner::Widget *wPixLabel = new KFormDesigner::Widget(this);
	wPixLabel->setPixmap("label");
	wPixLabel->setClassName("MyPicLabel");
	wPixLabel->setName(i18n("Picture Label"));
	wPixLabel->setDescription(i18n("A widget to display pixmaps"));
	m_classes.append(wPixLabel);

	KFormDesigner::Widget *wLineEdit = new KFormDesigner::Widget(this);
	wLineEdit->setPixmap("lineedit");
	wLineEdit->setClassName("KLineEdit");
	wLineEdit->setAlternateClassName("QLineEdit");
	wLineEdit->setInclude("klineedit.h");
	wLineEdit->setName(i18n("Line Edit"));
	wLineEdit->setDescription(i18n("A widget to input text"));
	m_classes.append(wLineEdit);

	KFormDesigner::Widget *wSpacer = new KFormDesigner::Widget(this);
	wSpacer->setPixmap("kexi");
	wSpacer->setClassName("Spacer");
	wSpacer->setName(i18n("Spacer"));
	wSpacer->setDescription(i18n("A spacer widget to fill in blanks"));
	m_classes.append(wSpacer);

	KFormDesigner::Widget *wPushButton = new KFormDesigner::Widget(this);
	wPushButton->setPixmap("button");
	wPushButton->setClassName("KPushButton");
	wPushButton->setAlternateClassName("QPushButton");
	wPushButton->setInclude("kpushbutton.h");
	wPushButton->setName(i18n("Push Button"));
	wPushButton->setDescription(i18n("A simple push button to execute a command"));
	m_classes.append(wPushButton);

	KFormDesigner::Widget *wRadioButton = new KFormDesigner::Widget(this);
	wRadioButton->setPixmap("radio");
	wRadioButton->setClassName("QRadioButton");
	wRadioButton->setName(i18n("Radio"));
	wRadioButton->setDescription(i18n("A radio button with text or pixmap label"));
	m_classes.append(wRadioButton);

	KFormDesigner::Widget *wCheckBox = new KFormDesigner::Widget(this);
	wCheckBox->setPixmap("check");
	wCheckBox->setClassName("QCheckBox");
	wCheckBox->setName(i18n("Check Box"));
	wCheckBox->setDescription(i18n("A check box with text or pixmap label"));
	m_classes.append(wCheckBox);

	KFormDesigner::Widget *wSpinBox = new KFormDesigner::Widget(this);
	wSpinBox->setPixmap("spin");
	wSpinBox->setClassName("KIntSpinBox");
	wSpinBox->setAlternateClassName("QSpinBox");
	wSpinBox->setInclude("knuminput.h");
	wSpinBox->setName(i18n("Spin Box"));
	wSpinBox->setDescription(i18n("A spin box widget"));
	m_classes.append(wSpinBox);

	KFormDesigner::Widget *wComboBox = new KFormDesigner::Widget(this);
	wComboBox->setPixmap("combo");
	wComboBox->setClassName("KComboBox");
	wComboBox->setAlternateClassName("QComboBox");
	wComboBox->setInclude("kcombobox.h");
	wComboBox->setName(i18n("Combo Box"));
	wComboBox->setDescription(i18n("A combo box widget"));
	m_classes.append(wComboBox);

	KFormDesigner::Widget *wListBox = new KFormDesigner::Widget(this);
	wListBox->setPixmap("listbox");
	wListBox->setClassName("KListBox");
	wListBox->setAlternateClassName("QListBox");
	wListBox->setInclude("klistbox.h");
	wListBox->setName(i18n("List Box"));
	wListBox->setDescription(i18n("A simple list widget"));
	m_classes.append(wListBox);

	KFormDesigner::Widget *wTextEdit = new KFormDesigner::Widget(this);
	wTextEdit->setPixmap("lineedit");
	wTextEdit->setClassName("KTextEdit");
	wTextEdit->setAlternateClassName("QTextEdit");
	wTextEdit->setInclude("ktextedit.h");
	wTextEdit->setName(i18n("Text Editor"));
	wTextEdit->setDescription(i18n("A simple single-page rich text editor"));
	m_classes.append(wTextEdit);

	KFormDesigner::Widget *wListView = new KFormDesigner::Widget(this);
	wListView->setPixmap("listview");
	wListView->setClassName("KListView");
	wListView->setAlternateClassName("QListView");
	wListView->setInclude("klistview.h");
	wListView->setName(i18n("List View"));
	wListView->setDescription(i18n("A list (or tree) widget"));
	m_classes.append(wListView);

	KFormDesigner::Widget *wSlider = new KFormDesigner::Widget(this);
	wSlider->setPixmap("slider");
	wSlider->setClassName("QSlider");
	wSlider->setName(i18n("Slider"));
	wSlider->setDescription(i18n("An horizontal slider"));
	m_classes.append(wSlider);

	KFormDesigner::Widget *wProgressBar = new KFormDesigner::Widget(this);
	wProgressBar->setPixmap("progress");
	wProgressBar->setClassName("KProgress");
	wProgressBar->setAlternateClassName("QProgressBar");
	wProgressBar->setInclude("kprogress.h");
	wProgressBar->setName(i18n("Progress Bar"));
	wProgressBar->setDescription(i18n("A progress indicator widget"));
	m_classes.append(wProgressBar);

	KFormDesigner::Widget *wLine = new KFormDesigner::Widget(this);
	wLine->setPixmap("line");
	wLine->setClassName("Line");
	wLine->setName(i18n("Line"));
	wLine->setDescription(i18n("A line to be used as a separator"));
	m_classes.append(wLine);

	KFormDesigner::Widget *wDate = new KFormDesigner::Widget(this);
	wDate->setPixmap("lineedit");
	wDate->setClassName("KDateWidget");
	#if KDE_IS_VERSION(3,1,9) //TMP
	wDate->setAlternateClassName("QDateEdit");
	wDate->setInclude("kdatewidget.h");
	#endif
	wDate->setName(i18n("Date Widget"));
	wDate->setDescription(i18n("A widget to input or display a date"));
	m_classes.append(wDate);

	KFormDesigner::Widget *wTime = new KFormDesigner::Widget(this);
	wTime->setPixmap("lineedit");
	wTime->setClassName("KTimeWidget");
	#if KDE_IS_VERSION(3,1,9) //TMP
	wTime->setAlternateClassName("QTimeEdit");
	wTime->setInclude("ktimewidget.h");
	#endif
	wTime->setName(i18n("Time Widget"));
	wTime->setDescription(i18n("A widget to input or display a time"));
	m_classes.append(wTime);

	KFormDesigner::Widget *wDateTime = new KFormDesigner::Widget(this);
	wDateTime->setPixmap("lineedit");
	wDateTime->setClassName("KDateTimeWidget");
	#if KDE_IS_VERSION(3,1,9) //TMP
	wDateTime->setAlternateClassName("QDateTimeEdit");
	wDateTime->setInclude("kdatetimewidget.h");
	#endif
	wDateTime->setName(i18n("Date/Time Widget"));
	wDateTime->setDescription(i18n("A widget to input or display a time and a date"));
	m_classes.append(wDateTime);
}

QString
StdWidgetFactory::name()
{
	return("stdwidgets");
}

KFormDesigner::WidgetList
StdWidgetFactory::classes()
{
	return m_classes;
}

QWidget*
StdWidgetFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *)
{
	kdDebug() << "StdWidgetFactory::create() " << this << endl;

	QWidget *w=0;

	if(c == "QLabel")
		w = new QLabel(i18n("Label"), p, n);
	if(c == "MyPicLabel")
		w = new MyPicLabel(p->topLevelWidget()->icon(), p, n);

	else if(c == "KLineEdit")
	{
		w = new KLineEdit(p, n);
		w->setCursor(QCursor(Qt::ArrowCursor));
	}
	else if(c == "KPushButton")
		w = new KPushButton(i18n("Button"), p, n);

	else if(c == "QRadioButton")
		w = new QRadioButton(i18n("Radio Button"), p, n);

	else if(c == "QCheckBox")
		w = new QCheckBox(i18n("Check Box"), p, n);

	else if(c == "KIntSpinBox")
		w = new KIntSpinBox(p, n);

	else if(c == "KComboBox")
		w = new KComboBox(p, n);

	else if(c == "KListBox")
		w = new KListBox(p, n);

	else if(c == "KTextEdit")
		w = new KTextEdit(i18n("Enter your text here"), QString::null, p, n);

	else if(c == "KListView")
	{
		w = new KListView(p, n);
		((KListView*)w)->addColumn(i18n("Column 1"));
		((KListView*)w)->setRootIsDecorated(true);
	}
	else if(c == "QSlider")
		w = new QSlider(Qt::Horizontal, p, n);

	else if(c == "KProgress")
		w = new KProgress(p, n);

	else if(c == "KDateWidget")
		w = new KDateWidget(QDate::currentDate(), p, n);

	else if(c == "KTimeWidget")
		w = new KTimeWidget(QTime::currentTime(), p, n);

	else if(c == "KDateTimeWidget")
		w = new KDateTimeWidget(QDateTime::currentDateTime(), p, n);

	else if(c == "Line")
		w= new Line(Line::Horizontal, p, n);

	else if(c == "Spacer")
		w = new Spacer(p, n);

	if(w)
		return w;
	else
	{
		kdDebug() << "WARNING :: w == 0 "  << endl;
		return 0;
	}
}

void
StdWidgetFactory::previewWidget(const QString &classname, QWidget *widget, KFormDesigner::Container *)
{
	if(classname == "Spacer")
		((Spacer*)widget)->setPreviewMode();
}

bool
StdWidgetFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	m_widget = w;
	m_container = container;

	if((classname == "QLabel") || (classname == "KTextEdit"))
	{
		menu->insertItem(SmallIconSet("edit"), i18n("Edit rich text"), this, SLOT(editText()));
		return true;
	}
	else if(classname == "KListView")
	{
		menu->insertItem(SmallIconSet("edit"), i18n("Edit listview contents"), this, SLOT(editListContents()));
		return true;
	}

	return false;
}

void
StdWidgetFactory::startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "KLineEdit")
	{
		KLineEdit *lineedit = static_cast<KLineEdit*>(w);
		createEditor(lineedit->text(), lineedit, lineedit->geometry(), lineedit->alignment(), true);
		return;
	}
	else if(classname == "QLabel")
	{
		QLabel *label = static_cast<QLabel*>(w);
		createEditor(label->text(), label, label->geometry(), label->alignment());
		return;
	}
	else if(classname == "KPushButton")
	{
		KPushButton *push = static_cast<KPushButton*>(w);
		QRect r(push->geometry());
		r.setX(r.x() + 10);
		r.setY(r.y() + 10);
		r.setWidth(r.width()-10);
		r.setHeight(r.height() - 10);
		createEditor(push->text(), push, r, Qt::AlignCenter);
	}
	else if(classname == "QRadioButton")
	{
		QRadioButton *radio = static_cast<QRadioButton*>(w);
		QRect r(radio->geometry());
		r.setX(r.x() + 20);
		createEditor(radio->text(), radio, r, Qt::AlignAuto);
		return;
	}
	else if(classname == "QCheckBox")
	{
		QCheckBox *check = static_cast<QCheckBox*>(w);
		QRect r(check->geometry());
		r.setX(r.x() + 20);
		createEditor(check->text(), check, r, Qt::AlignAuto);
		return;
	}
	else if((classname == "KComboBox") || (classname == "KListBox"))
	{
		QStringList list;
		if(classname == "KListBox")
		{
			KListBox *listbox = (KListBox*)w;
			for(uint i=0; i < listbox->count(); i++)
				list.append(listbox->text(i));
		}
		else if(classname == "KComboBox")
		{
			KComboBox *combo = (KComboBox*)w;
			for(int i=0; i < combo->count(); i++)
				list.append(combo->text(i));
		}

		if(editList(w, list))
		{
			if(classname == "KListBox")
			{
				((KListBox*)w)->clear();
				((KListBox*)w)->insertStringList(list);
			}
			else if(classname == "KComboBox")
			{
				((KComboBox*)w)->clear();
				((KComboBox*)w)->insertStringList(list);
			}
		}
	}
	else if((classname == "KTextEdit") || (classname == "KDateTimeWidget") || (classname == "KTimeWidget") ||
		(classname == "KDateWidget") || (classname == "KIntSpinBox"))
		disableFilter(w, container);

	return;
}

void
StdWidgetFactory::changeText(const QString &text)
{
	QString n = WidgetFactory::m_widget->className();
	QWidget *w = WidgetFactory::m_widget;
	if(n == "KIntSpinBox")
		((KIntSpinBox*)w)->setValue(text.toInt());
	else
		changeProperty("text", text, m_container);
}

void
StdWidgetFactory::saveSpecialProperty(const QString &classname, const QString &name, const QVariant &, QWidget *w, QDomElement &parentNode, QDomDocument &domDoc)
{
	if(name == "list_items" && classname == "KComboBox")
	{
		KComboBox *combo = (KComboBox*)w;
		for(int i=0; i < combo->count(); i++)
		{
			QDomElement item = domDoc.createElement("item");
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "text", combo->text(i));
			parentNode.appendChild(item);
		}
	}

	else if(name == "list_items" && classname == "KListBox")
	{
		KListBox *listbox = (KListBox*)w;
		for(uint i=0; i < listbox->count(); i++)
		{
			QDomElement item = domDoc.createElement("item");
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "text", listbox->text(i));
			parentNode.appendChild(item);
		}
	}

	else if(name == "list_contents" && classname == "KListView")
	{
		KListView *listview = (KListView*)w;
		// First we save the columns
		for(int i = 0; i < listview->columns(); i++)
		{
			QDomElement item = domDoc.createElement("column");
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "text", listview->columnText(i));
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "width", listview->columnWidth(i));
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "resizable", listview->header()->isResizeEnabled(i));
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "clickable", listview->header()->isClickEnabled(i));
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "fullwidth", listview->header()->isStretchEnabled(i));
			parentNode.appendChild(item);
		}

		// Then we save the list view items
		QListViewItem *item = listview->firstChild();
		while(item)
		{
			saveListItem(item, parentNode, domDoc);
			item = item->nextSibling();
		}
	}

	return;
}

void
StdWidgetFactory::saveListItem(QListViewItem *item, QDomNode &parentNode, QDomDocument &domDoc)
{
	QDomElement element = domDoc.createElement("item");
	parentNode.appendChild(element);

	// We save the text of each column
	for(int i = 0; i < item->listView()->columns(); i++)
		KFormDesigner::FormIO::saveProperty(element, domDoc, "property", "text", item->text(i));

	// Then we save every sub items
	QListViewItem *child = item->firstChild();
	while(child)
	{
		saveListItem(child, element, domDoc);
		child = child->nextSibling();
	}
}

void
StdWidgetFactory::readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *)
{
	QString tag = node.tagName();
	QString name = node.attribute("name");

	if((tag == "item") && (classname == "KComboBox"))
	{
		KComboBox *combo = (KComboBox*)w;
		QVariant val = KFormDesigner::FormIO::readProp(node.firstChild().firstChild(), w, name);
		if(val.canCast(QVariant::Pixmap))
			combo->insertItem(val.toPixmap());
		else
			combo->insertItem(val.toString());
	}

	else if((tag == "item") && (classname == "KListBox"))
	{
		KListBox *listbox = (KListBox*)w;
		QVariant val = KFormDesigner::FormIO::readProp(node.firstChild().firstChild(), w, name);
		if(val.canCast(QVariant::Pixmap))
			listbox->insertItem(val.toPixmap());
		else
			listbox->insertItem(val.toString());
	}

	else if((tag == "column") && (classname == "KListView"))
	{
		KListView *listview = (KListView*)w;
		int id;
		for(QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling())
		{
			QString prop = n.toElement().attribute("name");
			QVariant val = KFormDesigner::FormIO::readProp(n.firstChild(), w, name);
			if(prop == "text")
				id = listview->addColumn(val.toString());
			else if(prop == "width")
				listview->setColumnWidth(id, val.toInt());
			else if(prop == "resizable")
				listview->header()->setResizeEnabled(val.toBool(), id);
			else if(prop == "clickable")
				listview->header()->setClickEnabled(val.toBool(), id);
			else if(prop == "fullwidth")
				listview->header()->setStretchEnabled(val.toBool(), id);
		}
	}
	else if((tag == "item") && (classname == "KListView"))
	{
		KListView *listview = (KListView*)w;
		readListItem(node, 0, listview);
	}
}

void
StdWidgetFactory::readListItem(QDomElement &node, QListViewItem *parent, KListView *listview)
{
	QListViewItem *item;
	if(parent)
		item = new KListViewItem(parent);
	else
		item = new KListViewItem(listview);

	int i = 0;
	for(QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement childEl = n.toElement();
		QString prop = childEl.attribute("name");
		QString tag = childEl.tagName();

		// We read sub items
		if(tag == "item")
		{
			item->setOpen(true);
			readListItem(childEl, item, listview);
		}
		// and column texts
		else if((tag == "property") && (prop == "text"))
		{
			QVariant val = KFormDesigner::FormIO::readProp(n.firstChild(), listview, "item");
			item->setText(i, val.toString());
			i++;
		}
	}
}

bool
StdWidgetFactory::showProperty(const QString &classname, QWidget *, const QString &property, bool multiple)
{
	if(classname == "Spacer")
	{
		return Spacer::showProperty(property);
	}
	else if(classname == "MyPicLabel")
	{
		if((property == "text") || (property == "indent") || (property == "textFormat") || (property == "font") || (property == "alignment"))
			return false;
	}
	else if(classname == "QLabel")
	{
		if(property == "pixmap")
			return false;
	}
	else if(classname == "Line")
	{
		if((property == "frameShape") || (property == "font") || (property == "margin"))
			return false;
	}
	return !multiple;
}

QStringList
StdWidgetFactory::autoSaveProperties(const QString &classname)
{
	QStringList l;

	if(classname == "QLabel")
		l << "text";
	if(classname == "KPushButton")
		l << "text";
	else if(classname == "MyPicLabel")
		l << "pixmap";
	else if(classname == "KComboBox")
		l << "list_items";
	else if(classname == "KListBox")
		l << "list_items";
	else if(classname == "KListView")
		l << "list_contents";
	else if(classname == "Line")
		l << "orientation";
	else if(classname == "KTimeWidget")
		l << "time";
	else if(classname == "KDateWidget")
		l << "date";
	else if(classname == "KDateTimeWidget")
		l << "dateTime";
	else if(classname == "Spacer")
		l << "sizeType" << "orientation";
	else if(classname == "KTextEdit")
		l << "textFormat" << "text";

	return l;
}

void
StdWidgetFactory::editText()
{
	QString classname = m_widget->className();
	QString text;
	if(classname == "KTextEdit")
		text = ((KTextEdit*)m_widget)->text();
	else if(classname == "QLabel")
		text = ((QLabel*)m_widget)->text();

	if(editRichText(m_widget, text))
	{
		changeProperty("textFormat", "RichText", m_container);
		changeProperty("text", text, m_container);
	}
}

void
StdWidgetFactory::editListContents()
{
	if(m_widget->inherits("QListView"))
		editListView((QListView*)m_widget);
}

StdWidgetFactory::~StdWidgetFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(stdwidgets, KGenericFactory<StdWidgetFactory>)

#include "stdwidgetfactory.moc"

