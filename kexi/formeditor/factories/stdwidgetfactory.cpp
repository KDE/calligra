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
class KFORMEDITOR_EXPORT MyTextEdit : public KTextEdit
{
	public:
		MyTextEdit(const QString &text, QWidget *parent, const char *name, QObject *container)
		 : KTextEdit(text, QString::null, parent, name)
		{
			m_container = container;
			//setReadOnly(true);
			setCursor(QCursor(Qt::ArrowCursor));
		}
		~MyTextEdit() {;}

		void setContainer(QObject *container)
		{
			m_container = container;
		}
		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			if(!m_container)
				return KTextEdit::eventFilter(o, ev);
			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return KTextEdit::eventFilter(o, ev);
			return true;
		}

	private:
		QGuardedPtr<QObject>  m_container;
};

class KFORMEDITOR_EXPORT MySpinBox : public KIntSpinBox
{
	public:
		MySpinBox(QWidget *parent, const char *name, QObject *container)
		 : KIntSpinBox(parent, name)
		{
			m_container = container;
			setCursor(QCursor(Qt::ArrowCursor));
			editor()->setCursor(QCursor(Qt::ArrowCursor));

			QObjectList *list = new QObjectList(*children());
			for(QObject *obj = list->first(); obj; obj = list->next())
				obj->installEventFilter(this);
			delete list;
		}
		~MySpinBox() {;}

		void setContainer(QObject *container)
		{
			m_container = container;
			editor()->setCursor(QCursor(Qt::ArrowCursor));
		}
		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			if(!m_container)
				return KIntSpinBox::eventFilter(o, ev);
			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return KIntSpinBox::eventFilter(o, ev);
			return true;
		}

	private:
		QGuardedPtr<QObject>  m_container;
};

class KFORMEDITOR_EXPORT MyTimeWidget : public KTimeWidget
{
	public:
		MyTimeWidget(const QTime &time, QWidget *parent, const char *name, QObject *container)
		 : KTimeWidget(time, parent, name)
		{
			m_container = container;
			setCursor(QCursor(Qt::ArrowCursor));

			QObjectList *list = new QObjectList(*children());
			for(QObject *obj = list->first(); obj; obj = list->next())
				KFormDesigner::installRecursiveEventFilter(obj, this);

			delete list;
		}
		~MyTimeWidget() {;}

		void setContainer(QObject *container)
		{
			m_container = container;
		}
		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			if(!m_container)
				return KTimeWidget::eventFilter(o, ev);
			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return KTimeWidget::eventFilter(o, ev);
			return true;
		}

	private:
		QGuardedPtr<QObject>   m_container;
};

class KFORMEDITOR_EXPORT MyDateWidget : public KDateWidget
{
	public:
		MyDateWidget(const QDate &date, QWidget *parent, const char *name, QObject *container)
		 : KDateWidget(date, parent, name)
		{
			m_container = container;
			setCursor(QCursor(Qt::ArrowCursor));

			QObjectList *list = new QObjectList(*children());
			for(QObject *obj = list->first(); obj; obj = list->next())
				KFormDesigner::installRecursiveEventFilter(obj, this);

			delete list;
		}
		~MyDateWidget() {;}

		void setContainer(QObject *container)
		{
			m_container = container;
		}
		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			if(!m_container)
				return KDateWidget::eventFilter(o, ev);
			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return KDateWidget::eventFilter(o, ev);
			return true;
		}

	private:
		QGuardedPtr<QObject>   m_container;
};

class KFORMEDITOR_EXPORT MyDateTimeWidget : public KDateTimeWidget
{
	public:
		MyDateTimeWidget(const QDateTime &datetime, QWidget *parent, const char *name, QObject *container)
		 : KDateTimeWidget(datetime, parent, name)
		{
			m_container = container;
			setCursor(QCursor(Qt::ArrowCursor));

			QObjectList *list = new QObjectList(*children());
			for(QObject *obj = list->first(); obj; obj = list->next())
				KFormDesigner::installRecursiveEventFilter(obj, this);

			delete list;
		}
		~MyDateTimeWidget() {;}

		void setContainer(QObject *container)
		{
			m_container = container;
		}
		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			if(!m_container)
				return KDateTimeWidget::eventFilter(o, ev);
			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return KDateTimeWidget::eventFilter(o, ev);
			return true;
		}

	private:
		QGuardedPtr<QObject>   m_container;
};

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
	wSpinBox->setName(i18n("Spin Box"));
	wSpinBox->setDescription(i18n("A spin box widget"));
	m_classes.append(wSpinBox);

	KFormDesigner::Widget *wComboBox = new KFormDesigner::Widget(this);
	wComboBox->setPixmap("combo");
	wComboBox->setClassName("KComboBox");
	wComboBox->setName(i18n("Combo Box"));
	wComboBox->setDescription(i18n("A combo box widget"));
	m_classes.append(wComboBox);

	KFormDesigner::Widget *wListBox = new KFormDesigner::Widget(this);
	wListBox->setPixmap("listbox");
	wListBox->setClassName("KListBox");
	wListBox->setName(i18n("List Box"));
	wListBox->setDescription(i18n("A simple list widget"));
	m_classes.append(wListBox);

	KFormDesigner::Widget *wTextEdit = new KFormDesigner::Widget(this);
	wTextEdit->setPixmap("lineedit");
	wTextEdit->setClassName("KTextEdit");
	wTextEdit->setName(i18n("Text Editor"));
	wTextEdit->setDescription(i18n("A simple single-page rich text editor"));
	m_classes.append(wTextEdit);

	KFormDesigner::Widget *wListView = new KFormDesigner::Widget(this);
	wListView->setPixmap("listview");
	wListView->setClassName("KListView");
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
	wDate->setName(i18n("Date Widget"));
	wDate->setDescription(i18n("A widget to input or display a date"));
	m_classes.append(wDate);

	KFormDesigner::Widget *wTime = new KFormDesigner::Widget(this);
	wTime->setPixmap("lineedit");
	wTime->setClassName("KTimeWidget");
	wTime->setName(i18n("Time Widget"));
	wTime->setDescription(i18n("A widget to input or display a time"));
	m_classes.append(wTime);

	KFormDesigner::Widget *wDateTime = new KFormDesigner::Widget(this);
	wDateTime->setPixmap("lineedit");
	wDateTime->setClassName("KDateTimeWidget");
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
StdWidgetFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
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
	{
		w = new MySpinBox(p, n, container);
	}
	else if(c == "KComboBox")
		w = new KComboBox(p, n);

	else if(c == "KListBox")
		w = new KListBox(p, n);

	else if(c == "KTextEdit")
		w = new MyTextEdit(i18n("Enter your text here"), p, n, container);

	else if(c == "KListView")
	{
		w = new KListView(p, n);
		((KListView*)w)->addColumn(i18n("Column 1"));
	}
	else if(c == "QSlider")
		w = new QSlider(Qt::Horizontal, p, n);

	else if(c == "KProgress")
		w = new KProgress(p, n);

	else if(c == "KDateWidget")
		w = new MyDateWidget(QDate::currentDate(), p, n, container);

	else if(c == "KTimeWidget")
		w = new MyTimeWidget(QTime::currentTime(), p, n, container);

	else if(c == "KDateTimeWidget")
		w = new MyDateTimeWidget(QDateTime::currentDateTime(), p, n, container);

	else if(c == "Line")
		w= new Line(Line::Horizontal, p, n);

	else if(c == "Spacer")
		w = new KFormDesigner::Spacer(p, n);

	if(w)
	{
		w->installEventFilter(container);
		return w;
	}
	else
	{
		kdDebug() << "WARNING :: w == 0 "  << endl;
		return 0;
	}
}


bool
StdWidgetFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	if(classname == "QLabel")
	{
		menu->insertItem(i18n("Change text"), this, SLOT(chText()) );
		return true;
	}
	else if(classname == "KLineEdit")
	{
		menu->insertItem(i18n("Change text"), this, SLOT(chText()) );
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
	else if(classname == "KIntSpinBox")
	{
		/*KIntSpinBox *spin = static_cast<KIntSpinBox*>(w);
		QRect r(spin->geometry());
		r.setWidth(r.width() - spin->upRect().width());
		createEditor(spin->text(), spin, r, Qt::AlignRight, true, Qt::PaletteBase);
		m_editor->setValidator(spin->validator());*/
		((MySpinBox*)w)->setContainer(this);
		disableFilter(w, container);
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
	else if(classname == "KTextEdit")
	{
		((MyTextEdit*)w)->setContainer(this);
		disableFilter(w, container);
	}
	else if(classname == "KTimeWidget")
	{
		((MyTimeWidget*)w)->setContainer(this);
		disableFilter(w, container);
	}
	else if(classname == "KDateWidget")
	{
		((MyDateWidget*)w)->setContainer(this);
		disableFilter(w, container);
	}
	else if(classname == "KDateTimeWidget")
	{
		((MyDateTimeWidget*)w)->setContainer(this);
		disableFilter(w, container);
	}

	return;
}

void
StdWidgetFactory::resetEditor()
{
	if(m_widget && !m_editor)
	{
		QString classname = m_widget->className();
		QWidget *w = m_widget;
		if(classname == "KTextEdit")
			((MyTextEdit*)w)->setContainer(m_container);
		else if(classname == "KIntSpinBox")
			((MySpinBox*)w)->setContainer(m_container);
		else if(classname == "KTimeWidget")
			((MyTimeWidget*)w)->setContainer(m_container);
		else if(classname == "KDateWidget")
			((MyDateWidget*)w)->setContainer(m_container);
		else if(classname == "KDateTimeWidget")
			((MyDateTimeWidget*)w)->setContainer(m_container);
	}

	WidgetFactory::resetEditor();
}

void
StdWidgetFactory::changeText(const QString &text)
{
	QString n = m_widget->className();
	QWidget *w = m_widget;
	if(n == "KIntSpinBox")
		((KIntSpinBox*)w)->setValue(text.toInt());
	else
		changeProperty("text", text, m_container);
}

void
StdWidgetFactory::saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &domDoc)
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

	if(name == "list_items" && classname == "KListBox")
	{
		KListBox *listbox = (KListBox*)w;
		for(uint i=0; i < listbox->count(); i++)
		{
			QDomElement item = domDoc.createElement("item");
			KFormDesigner::FormIO::saveProperty(item, domDoc, "property", "text", listbox->text(i));
			parentNode.appendChild(item);
		}
	}

	return;
}

void
StdWidgetFactory::readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item)
{
	QString tag = node.tagName();
	QString name = node.attribute("name");

	if(tag == "item" && classname == "KComboBox")
	{
		KComboBox *combo = (KComboBox*)w;
		QVariant val = KFormDesigner::FormIO::readProp(node.firstChild().firstChild(), w, name);
		if(val.canCast(QVariant::Pixmap))
			combo->insertItem(val.toPixmap());
		else
			combo->insertItem(val.toString());
	}

	if(tag == "item" && classname == "KListBox")
	{
		KListBox *listbox = (KListBox*)w;
		QVariant val = KFormDesigner::FormIO::readProp(node.firstChild().firstChild(), w, name);
		if(val.canCast(QVariant::Pixmap))
			listbox->insertItem(val.toPixmap());
		else
			listbox->insertItem(val.toString());
	}
}

bool
StdWidgetFactory::showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple)
{
	if(classname == "Spacer")
	{
		return KFormDesigner::Spacer::showProperty(property);
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
	if(classname == "QLabel")
		return QStringList("text");
	else if(classname == "MyPicLabel")
		return QStringList("pixmap");
	else if(classname == "KComboBox")
		return QStringList("list_items");
	else if(classname == "KListBox")
		return QStringList("list_items");
	else if(classname == "Line")
		return QStringList("orientation");
	else if(classname == "KTimeWidget")
		return QStringList("time");
	else if(classname == "KDateWidget")
		return QStringList("date");
	else if(classname == "KDateTimeWidget")
		return QStringList("dateTime");

	return QStringList();
}

StdWidgetFactory::~StdWidgetFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(stdwidgets, KGenericFactory<StdWidgetFactory>)

#include "stdwidgetfactory.moc"

