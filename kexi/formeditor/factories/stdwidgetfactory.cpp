/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch                                   *
 *   lucijan@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qlabel.h>
#include <klineedit.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <knuminput.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qobjectlist.h>
#include <qstring.h>

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>

#include "spacer.h"
#include "stdwidgetfactory.h"

// Some widgets subclass to allow event filtering and some other things
class KFORMEDITOR_EXPORT MyTextEdit : public QTextEdit
{
	public:
		MyTextEdit(const QString &text, QWidget *parent, const char *name, QObject *container)
		 : QTextEdit(text, QString::null, parent, name)
		{
			m_container = container;
			setReadOnly(true);
			setCursor(QCursor(Qt::ArrowCursor));
		}
		~MyTextEdit() {;}

		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return QTextEdit::eventFilter(o, ev);
			return true;
		}

	private:
		QObject   *m_container;
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

		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return KIntSpinBox::eventFilter(o, ev);
			return true;
		}

	private:
		QObject   *m_container;
};

// THe factory itself

StdWidgetFactory::StdWidgetFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wLabel = new KFormDesigner::Widget(this);
	wLabel->setPixmap("label");
	wLabel->setClassName("QLabel");
	wLabel->setName(i18n("Text Label"));
	wLabel->setDescription(i18n("A widget to display text or pixmaps"));
	m_classes.append(wLabel);

	KFormDesigner::Widget *wLineEdit = new KFormDesigner::Widget(this);
	wLineEdit->setPixmap("lineedit");
	wLineEdit->setClassName("QLineEdit");
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
	wPushButton->setPixmap("pushbutton");
	wPushButton->setClassName("QPushButton");
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
	wComboBox->setClassName("QComboBox");
	wComboBox->setName(i18n("Combo Box"));
	wComboBox->setDescription(i18n("A combo box widget"));
	m_classes.append(wComboBox);

	KFormDesigner::Widget *wListBox = new KFormDesigner::Widget(this);
	wListBox->setPixmap("listbox");
	wListBox->setClassName("QListBox");
	wListBox->setName(i18n("List Box"));
	wListBox->setDescription(i18n("A simple list widget"));
	m_classes.append(wListBox);

	KFormDesigner::Widget *wTextEdit = new KFormDesigner::Widget(this);
	wTextEdit->setPixmap("textedit");
	wTextEdit->setClassName("QTextEdit");
	wTextEdit->setName(i18n("Text Editor"));
	wTextEdit->setDescription(i18n("A simple single-page rich text editor"));
	m_classes.append(wTextEdit);
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

	else if(c == "QLineEdit")
	{
		w = new QLineEdit(p, n);
		w->setCursor(QCursor(Qt::ArrowCursor));
	}
	else if(c == "QPushButton")
		w = new QPushButton(i18n("Button"), p, n);

	else if(c == "QRadioButton")
		w = new QRadioButton(i18n("Radio Button"), p, n);

	else if(c == "QCheckBox")
		w = new QCheckBox(i18n("Check Box"), p, n);

	else if(c == "KIntSpinBox")
	{
		w = new MySpinBox(p, n, container);
	}
	else if(c == "QComboBox")
		w = new QComboBox(p, n);

	else if(c == "QListBox")
		w = new QListBox(p, n);

	else if(c == "QTextEdit")
		w = new MyTextEdit(i18n("Enter your text here"), p, n, container);

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


void
StdWidgetFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	if(classname == "QLabel")
	{
		menu->insertItem(i18n("Change text"), this, SLOT(chText()) );
		return;
	}
	else if(classname == "QLineEdit")
	{
		menu->insertItem(i18n("Change text"), this, SLOT(chText()) );
		return;
	}
	else if(classname == "QPushButton")
	{

	}
	else if(classname == "QRadioButton")
	{

	}
	else if(classname == "QCheckBox")
	{

	}
	else if(classname == "KIntSpinBox")
	{

	}
	else if(classname == "QComboBox")
	{
	}
	else if(classname == "QListBox")
	{
	}
	else if(classname == "QTextEdit")
	{
	}

	return;
}

void
StdWidgetFactory::startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "QLineEdit")
	{
		QLineEdit *lineedit = static_cast<QLineEdit*>(w);
		createEditor(lineedit->text(), lineedit, lineedit->geometry(), lineedit->alignment(), true);
		return;
	}
	else if(classname == "QLabel")
	{
		QLabel *label = static_cast<QLabel*>(w);
		createEditor(label->text(), label, label->geometry(), label->alignment());
		return;
	}
	else if(classname == "QPushButton")
	{
		QPushButton *push = static_cast<QPushButton*>(w);
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
		QSpinBox *spin = static_cast<QSpinBox*>(w);
		QRect r(spin->geometry());
		r.setWidth(r.width() - spin->upRect().width());
		createEditor(spin->text(), spin, r, Qt::AlignRight, true, Qt::PaletteBase);
		m_editor->setValidator(spin->validator());
	}
	else if(classname == "QComboBox")
	{
	}
	else if(classname == "QListBox")
	{
	}
	else if(classname == "QTextEdit")
	{
	}

	return;
}

void
StdWidgetFactory::changeText(const QString &text)
{
	QString n = m_widget->className();
	if(n == "KIntSpinBox")
		((QSpinBox*)m_widget)->setValue(text.toInt());
	else
		changeProperty("text", text, m_container);
}

bool
StdWidgetFactory::showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple)
{
	if(classname == "Spacer")
	{
		return KFormDesigner::Spacer::showProperty(property);
	}
	return !multiple;
}

StdWidgetFactory::~StdWidgetFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(stdwidgets, KGenericFactory<StdWidgetFactory>)

#include "stdwidgetfactory.moc"

