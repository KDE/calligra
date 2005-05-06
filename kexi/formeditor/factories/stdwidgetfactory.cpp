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
#include <qstyle.h>
#include <qvaluevector.h>

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

#if KDE_VERSION < KDE_MAKE_VERSION(3,1,9)
# include <qdatetimeedit.h>
# define KTimeWidget QTimeEdit
# define KDateWidget QDateEdit
# define KDateTimeWidget QDateTimeEdit
#else
# include <ktimewidget.h>
# include <kdatewidget.h>
# include <kdatetimewidget.h>
#endif

#include "spring.h"
#include "formIO.h"
#include "form.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include "stdwidgetfactory.h"
#include <kexipropertybuffer.h>

// Some widgets subclass to allow event filtering and some other things
KexiPictureLabel::KexiPictureLabel(const QPixmap &pix, QWidget *parent, const char *name)
 : QLabel(parent, name)
{
	setPixmap(pix);
	setScaledContents(false);
}

bool
KexiPictureLabel::setProperty(const char *name, const QVariant &value)
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

StdWidgetFactory::StdWidgetFactory(QObject *parent, const char *, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, "stdwidgets")
{
	KFormDesigner::WidgetInfo *wFormWidget = new KFormDesigner::WidgetInfo(this);
	wFormWidget->setPixmap("form");
	wFormWidget->setClassName("FormWidgetBase");
	wFormWidget->setName(i18n("Form"));
	wFormWidget->setNamePrefix(i18n("Form"));
	wFormWidget->setDescription(i18n("A simple form widget"));
	addClass(wFormWidget);

	KFormDesigner::WidgetInfo *wCustomWidget = new KFormDesigner::WidgetInfo(this);
	wCustomWidget->setPixmap("custom_widget");
	wCustomWidget->setClassName("CustomWidget");
	wCustomWidget->setName(i18n("Custom Widget"));
	wCustomWidget->setNamePrefix(i18n("This string will be used to name widgets of this class. It must _not_ contain white "
	"spaces and non latin1 characters", "CustomWidget"));
	wCustomWidget->setDescription(i18n("A custom or non-supported widget"));
	addClass(wCustomWidget);

	KFormDesigner::WidgetInfo *wLabel = new KFormDesigner::WidgetInfo(this);
	wLabel->setPixmap("label");
	wLabel->setClassName("QLabel");
	wLabel->setName(i18n("Text Label"));
	wLabel->setNamePrefix(i18n("Widget name (see above)", "TextLabel"));
	wLabel->setDescription(i18n("A widget to display text"));
	addClass(wLabel);

	KFormDesigner::WidgetInfo *wPixLabel = new KFormDesigner::WidgetInfo(this);
	wPixLabel->setPixmap("pixmaplabel");
	wPixLabel->setClassName("KexiPictureLabel");
	wPixLabel->setName(i18n("Picture Label"));
//! @todo Qt designer compatibility: maybe use this class when QLabel has a pixmap set...?
	//wPixLabel->addAlternateClassName("QLabel");
	wPixLabel->setSavingName("KexiPictureLabel");
	wPixLabel->setNamePrefix(i18n("Widget name (see above)", "PictureLabel"));
	wPixLabel->setDescription(i18n("A widget to display pixmaps"));
	addClass(wPixLabel);

	KFormDesigner::WidgetInfo *wLineEdit = new KFormDesigner::WidgetInfo(this);
	wLineEdit->setPixmap("lineedit");
	wLineEdit->setClassName("KLineEdit");
	wLineEdit->addAlternateClassName("QLineEdit");
	wLineEdit->setIncludeFileName("klineedit.h");
	wLineEdit->setName(i18n("Line Edit"));
	wLineEdit->setNamePrefix(i18n("Widget name (see above)", "LineEdit"));
	wLineEdit->setDescription(i18n("A widget to input text"));
	addClass(wLineEdit);

	KFormDesigner::WidgetInfo *wSpring = new KFormDesigner::WidgetInfo(this);
	wSpring->setPixmap("spring");
	wSpring->setClassName("Spring");
	wSpring->setName(i18n("Spring"));
	wSpring->setNamePrefix(i18n("Widget name (see above)", "Spring"));
	wSpring->setDescription(i18n("A spring to place between widgets"));
	addClass(wSpring);

	KFormDesigner::WidgetInfo *wPushButton = new KFormDesigner::WidgetInfo(this);
	wPushButton->setPixmap("button");
	wPushButton->setClassName("KPushButton");
	wPushButton->addAlternateClassName("QPushButton");
	wPushButton->setIncludeFileName("kpushbutton.h");
	wPushButton->setName(i18n("Push Button"));
	wPushButton->setNamePrefix(i18n("Widget name (see above)", "PushButton"));
	wPushButton->setDescription(i18n("A simple push button to execute actions"));
	addClass(wPushButton);

	KFormDesigner::WidgetInfo *wRadioButton = new KFormDesigner::WidgetInfo(this);
	wRadioButton->setPixmap("radio");
	wRadioButton->setClassName("QRadioButton");
	wRadioButton->setName(i18n("Radio"));
	wRadioButton->setNamePrefix(i18n("Widget name (see above)", "Radio"));
	wRadioButton->setDescription(i18n("A radio button with text or pixmap label"));
	addClass(wRadioButton);

	KFormDesigner::WidgetInfo *wCheckBox = new KFormDesigner::WidgetInfo(this);
	wCheckBox->setPixmap("check");
	wCheckBox->setClassName("QCheckBox");
	wCheckBox->setName(i18n("Check Box"));
	wCheckBox->setNamePrefix(i18n("Widget name (see above)", "CheckBox"));
	wCheckBox->setDescription(i18n("A check box with text or pixmap label"));
	addClass(wCheckBox);

	KFormDesigner::WidgetInfo *wSpinBox = new KFormDesigner::WidgetInfo(this);
	wSpinBox->setPixmap("spin");
	wSpinBox->setClassName("KIntSpinBox");
	wSpinBox->addAlternateClassName("QSpinBox");
	wSpinBox->setIncludeFileName("knuminput.h");
	wSpinBox->setName(i18n("Spin Box"));
	wSpinBox->setNamePrefix(i18n("Widget name (see above)", "SpinBox"));
	wSpinBox->setDescription(i18n("A spin box widget"));
	addClass(wSpinBox);

	KFormDesigner::WidgetInfo *wComboBox = new KFormDesigner::WidgetInfo(this);
	wComboBox->setPixmap("combo");
	wComboBox->setClassName("KComboBox");
	wComboBox->addAlternateClassName("QComboBox");
	wComboBox->setIncludeFileName("kcombobox.h");
	wComboBox->setName(i18n("Combo Box"));
	wComboBox->setNamePrefix(i18n("Widget name (see above)", "ComboBox"));
	wComboBox->setDescription(i18n("A combo box widget"));
	addClass(wComboBox);

	KFormDesigner::WidgetInfo *wListBox = new KFormDesigner::WidgetInfo(this);
	wListBox->setPixmap("listbox");
	wListBox->setClassName("KListBox");
	wListBox->addAlternateClassName("QListBox");
	wListBox->setIncludeFileName("klistbox.h");
	wListBox->setName(i18n("List Box"));
	wListBox->setNamePrefix(i18n("Widget name (see above)", "ListBox"));
	wListBox->setDescription(i18n("A simple list widget"));
	addClass(wListBox);

	KFormDesigner::WidgetInfo *wTextEdit = new KFormDesigner::WidgetInfo(this);
	wTextEdit->setPixmap("textedit");
	wTextEdit->setClassName("KTextEdit");
	wTextEdit->addAlternateClassName("QTextEdit");
	wTextEdit->setIncludeFileName("ktextedit.h");
	wTextEdit->setName(i18n("Text Editor"));
	wTextEdit->setNamePrefix(i18n("Widget name (see above)", "TextEditor"));
	wTextEdit->setDescription(i18n("A simple single-page rich text editor"));
	addClass(wTextEdit);

	KFormDesigner::WidgetInfo *wListView = new KFormDesigner::WidgetInfo(this);
	wListView->setPixmap("listview");
	wListView->setClassName("KListView");
	wListView->addAlternateClassName("QListView");
	wListView->setIncludeFileName("klistview.h");
	wListView->setName(i18n("List View"));
	wListView->setNamePrefix(i18n("Widget name (see above)", "ListView"));
	wListView->setDescription(i18n("A list (or tree) widget"));
	addClass(wListView);

	KFormDesigner::WidgetInfo *wSlider = new KFormDesigner::WidgetInfo(this);
	wSlider->setPixmap("slider");
	wSlider->setClassName("QSlider");
	wSlider->setName(i18n("Slider"));
	wSlider->setNamePrefix(i18n("Widget name (see above)", "Slider"));
	wSlider->setDescription(i18n("An horizontal slider"));
	addClass(wSlider);

	KFormDesigner::WidgetInfo *wProgressBar = new KFormDesigner::WidgetInfo(this);
	wProgressBar->setPixmap("progress");
	wProgressBar->setClassName("KProgress");
	wProgressBar->addAlternateClassName("QProgressBar");
	wProgressBar->setIncludeFileName("kprogress.h");
	wProgressBar->setName(i18n("Progress Bar"));
	wProgressBar->setNamePrefix(i18n("Widget name (see above)", "ProgressBar"));
	wProgressBar->setDescription(i18n("A progress indicator widget"));
	addClass(wProgressBar);

	KFormDesigner::WidgetInfo *wLine = new KFormDesigner::WidgetInfo(this);
	wLine->setPixmap("line");
	wLine->setClassName("Line");
	wLine->setName(i18n("Line"));
	wLine->setNamePrefix(i18n("Widget name (see above)", "Line"));
	wLine->setDescription(i18n("A line to be used as a separator"));
	addClass(wLine);

	KFormDesigner::WidgetInfo *wDate = new KFormDesigner::WidgetInfo(this);
	wDate->setPixmap("dateedit");
	wDate->setClassName("KDateWidget");
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
	wDate->addAlternateClassName("QDateEdit");
	wDate->setIncludeFileName("kdatewidget.h");
#endif
	wDate->setName(i18n("Date Widget"));
	wDate->setNamePrefix(i18n("Widget name (see above)", "DateWidget"));
	wDate->setDescription(i18n("A widget to input or display a date"));
	addClass(wDate);

	KFormDesigner::WidgetInfo *wTime = new KFormDesigner::WidgetInfo(this);
	wTime->setPixmap("timeedit");
	wTime->setClassName("KTimeWidget");
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
	wTime->addAlternateClassName("QTimeEdit");
	wTime->setIncludeFileName("ktimewidget.h");
#endif
	wTime->setName(i18n("Time Widget"));
	wTime->setNamePrefix(i18n("Widget name (see above)", "TimeWidget"));
	wTime->setDescription(i18n("A widget to input or display a time"));
	addClass(wTime);

	KFormDesigner::WidgetInfo *wDateTime = new KFormDesigner::WidgetInfo(this);
	wDateTime->setPixmap("datetimeedit");
	wDateTime->setClassName("KDateTimeWidget");
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
	wDateTime->addAlternateClassName("QDateTimeEdit");
	wDateTime->setIncludeFileName("kdatetimewidget.h");
#endif
	wDateTime->setName(i18n("Date/Time Widget"));
	wDateTime->setNamePrefix(i18n("Widget name (see above)", "DateTimeWidget"));
	wDateTime->setDescription(i18n("A widget to input or display a time and a date"));
	addClass(wDateTime);
}

StdWidgetFactory::~StdWidgetFactory()
{
}

QWidget*
StdWidgetFactory::create(const QCString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	QWidget *w=0;
	QString text = container->form()->manager()->lib()->textForWidgetName(n, c);

	if(c == "QLabel")
		w = new QLabel(text, p, n);
	else if(c == "KexiPictureLabel")
		w = new KexiPictureLabel(DesktopIcon("image"), p, n);

	else if(c == "KLineEdit")
	{
		w = new KLineEdit(p, n);
		w->setCursor(QCursor(Qt::ArrowCursor));
	}
	else if(c == "KPushButton")
		w = new KPushButton(/*i18n("Button")*/text, p, n);

	else if(c == "QRadioButton")
		w = new QRadioButton(/*i18n("Radio Button")*/text, p, n);

	else if(c == "QCheckBox")
		w = new QCheckBox(/*i18n("Check Box")*/text, p, n);

	else if(c == "KIntSpinBox")
		w = new KIntSpinBox(p, n);

	else if(c == "KComboBox")
		w = new KComboBox(p, n);

	else if(c == "KListBox")
		w = new KListBox(p, n);

	else if(c == "KTextEdit")
		w = new KTextEdit(/*i18n("Enter your text here")*/text, QString::null, p, n);

	else if(c == "KListView")
	{
		w = new KListView(p, n);
		if(container->form()->interactiveMode())
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

	else if(c == "Spring")
		w = new Spring(p, n);

	if(w)
		return w;

	kdDebug() << "WARNING :: w == 0 "  << endl;
	return 0;
}

bool
StdWidgetFactory::previewWidget(const QCString &classname, QWidget *widget, KFormDesigner::Container *)
{
	if(classname == "Spring") {
		((Spring*)widget)->setPreviewMode();
		return true;
	}
	return false;
}

bool
StdWidgetFactory::createMenuActions(const QCString &classname, QWidget *, QPopupMenu *menu,
	KFormDesigner::Container *)
{
	if((classname == "QLabel") || (classname == "KTextEdit"))
	{
		menu->insertItem(SmallIconSet("edit"), i18n("Edit Rich Text"), this, SLOT(editText()));
		return true;
	}
	else if(classname == "KListView")
	{
		menu->insertItem(SmallIconSet("edit"), i18n("Edit Listview Contents"), this, SLOT(editListContents()));
		return true;
	}

	return false;
}

bool
StdWidgetFactory::startEditing(const QCString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "KLineEdit")
	{
		KLineEdit *lineedit = static_cast<KLineEdit*>(w);
		createEditor(classname, lineedit->text(), lineedit, container, lineedit->geometry(), lineedit->alignment(), true);
		return true;
	}
	else if(classname == "QLabel")
	{
		QLabel *label = static_cast<QLabel*>(w);
		if(label->textFormat() == RichText)
		{
			m_widget = w;
			editText();
		}
		else
			createEditor(classname, label->text(), label, container, label->geometry(), label->alignment());
		return true;
	}
	else if(classname == "KPushButton")
	{
		KPushButton *push = static_cast<KPushButton*>(w);
		QRect r = w->style().subRect(QStyle::SR_PushButtonContents, w);
		QRect editorRect = QRect(push->x() + r.x(), push->y() + r.y(), r.width(), r.height());
		//r.setX(r.x() + 5);
		//r.setY(r.y() + 5);
		//r.setWidth(r.width()-10);
		//r.setHeight(r.height() - 10);
		createEditor(classname, push->text(), push, container, editorRect, Qt::AlignCenter, false, Qt::PaletteButton);
		return true;
	}
	else if(classname == "QRadioButton")
	{
		QRadioButton *radio = static_cast<QRadioButton*>(w);
		QRect r = w->style().subRect(QStyle::SR_RadioButtonContents, w);
		QRect editorRect = QRect(radio->x() + r.x(), radio->y() + r.y(), r.width(), r.height());
		createEditor(classname, radio->text(), radio, container, editorRect, Qt::AlignAuto);
		return true;
	}
	else if(classname == "QCheckBox")
	{
		QCheckBox *check = static_cast<QCheckBox*>(w);
		//QRect r(check->geometry());
		//r.setX(r.x() + 20);
		QRect r = w->style().subRect(QStyle::SR_CheckBoxContents, w);
		QRect editorRect = QRect(check->x() + r.x(), check->y() + r.y(), r.width(), r.height());
		createEditor(classname, check->text(), check, container, editorRect, Qt::AlignAuto);
		return true;
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
		return true;
	}
	else if((classname == "KTextEdit") || (classname == "KDateTimeWidget") || (classname == "KTimeWidget") ||
		(classname == "KDateWidget") || (classname == "KIntSpinBox")) {
		disableFilter(w, container);
		return true;
	}
	return false;
}

bool
StdWidgetFactory::clearWidgetContent(const QCString &classname, QWidget *w)
{
	if(classname == "KLineEdit")
		((KLineEdit*)w)->clear();
	else if(classname == "KListBox")
		((KListBox*)w)->clear();
	else if(classname == "KListView")
		((KListView*)w)->clear();
	else if(classname == "KComboBox")
		((KComboBox*)w)->clear();
	else if(classname == "KTextEdit")
		((KTextEdit*)w)->clear();
	else
		return false;
	return true;
}

bool
StdWidgetFactory::changeText(const QString &text)
{
	QCString n = WidgetFactory::m_widget->className();
	QWidget *w = WidgetFactory::m_widget;
	if(n == "KIntSpinBox")
		((KIntSpinBox*)w)->setValue(text.toInt());
	else
		changeProperty("text", text, m_container);

	/* By-hand method not needed as sizeHint() can do that for us
	QFontMetrics fm = w->fontMetrics();
	QSize s(fm.width( text ), fm.height());
	int width;
	if(n == "QLabel") // labels are resized to fit the text
	{
		w->resize(w->sizeHint());
		WidgetFactory::m_editor->resize(w->size());
		return;
	}
	// and other widgets are just enlarged if needed
	else if(n == "KPushButton")
		width = w->style().sizeFromContents( QStyle::CT_PushButton, w, s).width();
	else if(n == "QCheckBox")
		width = w->style().sizeFromContents( QStyle::CT_CheckBox, w, s).width();
	else if(n == "QRadioButton")
		width = w->style().sizeFromContents( QStyle::CT_RadioButton, w, s).width();
	else
		return;*/
	int width = w->sizeHint().width();

	if(w->width() < width)
	{
		w->resize(width, w->height() );
		//WidgetFactory::m_editor->resize(w->size());
	}
	return true;
}

void
StdWidgetFactory::resizeEditor(QWidget *widget, const QCString &classname)
{
	QSize s = widget->size();
	QPoint p = widget->pos();
	QRect r;
	//if((classname == "QLabel") || (classname == "KPushButton") || (classname == "KLineEdit") || (classname == "QLabel"))

	if(classname == "QRadioButton")
	{
		r = widget->style().subRect(QStyle::SR_RadioButtonContents, widget);
		p += r.topLeft();
		s.setWidth(r.width());
	}
	else if(classname == "QCheckBox")
	{
		r = widget->style().subRect(QStyle::SR_CheckBoxContents, widget);
		p += r.topLeft();
		s.setWidth(r.width());
	}
	else if(classname == "KPushButton")
	{
		r = widget->style().subRect(QStyle::SR_PushButtonContents, widget);
		p += r.topLeft();
		s = r.size();
	}

	m_editor->resize(s);
	m_editor->move(p);
}

bool
StdWidgetFactory::saveSpecialProperty(const QCString &classname, const QString &name, const QVariant &, QWidget *w, QDomElement &parentNode, QDomDocument &domDoc)
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
		return true;
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
		return true;
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
		return true;
	}

	return false;
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

bool
StdWidgetFactory::readSpecialProperty(const QCString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *)
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
		return true;
	}

	if((tag == "item") && (classname == "KListBox"))
	{
		KListBox *listbox = (KListBox*)w;
		QVariant val = KFormDesigner::FormIO::readProp(node.firstChild().firstChild(), w, name);
		if(val.canCast(QVariant::Pixmap))
			listbox->insertItem(val.toPixmap());
		else
			listbox->insertItem(val.toString());
		return true;
	}

	if((tag == "column") && (classname == "KListView"))
	{
		KListView *listview = (KListView*)w;
		int id=0;
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
		return true;
	}
	else if((tag == "item") && (classname == "KListView"))
	{
		KListView *listview = (KListView*)w;
		readListItem(node, 0, listview);
		return true;
	}

	return false;
}

void
StdWidgetFactory::readListItem(QDomElement &node, QListViewItem *parent, KListView *listview)
{
	QListViewItem *item;
	if(parent)
		item = new KListViewItem(parent);
	else
		item = new KListViewItem(listview);

	// We need to move the item at the end of the list
	QListViewItem *last;
	if(parent)
		last = parent->firstChild();
	else
		last = listview->firstChild();

	while(last->nextSibling())
		last = last->nextSibling();
	item->moveItem(last);

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
StdWidgetFactory::isPropertyVisibleInternal(const QCString &classname, QWidget *, const QCString &property)
{
	if(classname == "FormWidgetBase")
	{
		if(property == "iconText")
			return false;
	}
	else if (classname == "CustomWidget")
	{
	}
	else if(classname == "Spring")
	{
		return Spring::isPropertyVisible(property);
	}
	else if(classname == "KexiPictureLabel")
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
	else if(classname=="QCheckBox")
	{
		return m_showAdvancedProperties || (property != "autoRepeat");
	}
	else if(classname=="QRadioButton")
	{
		return m_showAdvancedProperties || (property != "autoRepeat");
	}
	else if(classname=="KPushButton")
	{
//! @todo reenable autoDefault / default if the top level window is dialog...
		return m_showAdvancedProperties || (property != "autoDefault" && property != "default");
	}
	return true;
}

QValueList<QCString>
StdWidgetFactory::autoSaveProperties(const QCString &classname)
{
	QValueList<QCString> l;

	if(classname == "QLabel")
		l << "text";
	if(classname == "KPushButton")
		l << "text";
	else if(classname == "KexiPictureLabel")
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
	else if(classname == "Spring")
		l << "sizeType" << "orientation";
	else if(classname == "KTextEdit")
		l << "textFormat" << "text";

	return l;
}

void
StdWidgetFactory::editText()
{
	QCString classname = m_widget->className();
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

	if(classname == "QLabel")
		m_widget->resize(m_widget->sizeHint());
}

void
StdWidgetFactory::editListContents()
{
	if(m_widget->inherits("QListView"))
		editListView((QListView*)m_widget);
}


void
StdWidgetFactory::setPropertyOptions( KexiPropertyBuffer& buf, const KFormDesigner::WidgetInfo& info, QWidget *w )
{
	if (buf.hasProperty("indent")) {
		buf["indent"].setOption("min", -1);
/*		buf["indent"].setOption("minValueText", i18n("default\n(HINT: default indent value)", "default"));*/
	}
}

K_EXPORT_COMPONENT_FACTORY(stdwidgets, KGenericFactory<StdWidgetFactory>("stdwidgets"))

#include "stdwidgetfactory.moc"

