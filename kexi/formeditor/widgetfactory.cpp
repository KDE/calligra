/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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
#include <qlayout.h>
#include <qdialog.h>
#include <qcursor.h>
#include <qobjectlist.h>

#include <keditlistbox.h>
#include <kstdguiitem.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kdebug.h>

#include "resizehandle.h"
#include "objpropbuffer.h"
#include "formmanager.h"
#include "form.h"
#include "container.h"
#include "widgetfactory.h"


namespace KFormDesigner {
WidgetFactory::WidgetFactory(QObject *parent, const char *name)
 : QObject(parent, name)
{
}

KLineEdit*
WidgetFactory::createEditor(const QString &text, QWidget *w, QRect geometry, int align,  bool useFrame, BackgroundMode background)
{
	KLineEdit *editor = new KLineEdit(text, w->parentWidget());
	editor->setAlignment(align);
	editor->setPalette(w->palette());
	editor->setGeometry(geometry);
	if(background == Qt::NoBackground)
		editor->setBackgroundMode(w->backgroundMode());
	else
		editor->setBackgroundMode(background);
	editor->installEventFilter(this);
	editor->setFrame(useFrame);
	editor->show();
	editor->setFocus();
	connect(editor, SIGNAL(textChanged(const QString&)), this, SLOT(changeText(const QString&)));
	connect(w, SIGNAL(destroyed()), this, SLOT(resetEditor()));
	connect(editor, SIGNAL(destroyed()), this, SLOT(editorDeleted()));

	m_handles = new ResizeHandleSet(w, true);

	m_editor = editor;
	m_widget = w;
	return editor;
}

void
WidgetFactory::disableFilter(QWidget *w, Container *container)
{
	w->removeEventFilter(container);
	w->installEventFilter(this);
	w->setFocus();
	m_handles = new ResizeHandleSet(w, true);
	m_widget = w;
	m_container = container;
	m_editor = 0;

	connect(w, SIGNAL(destroyed()), this, SLOT(resetEditor()));
}

bool
WidgetFactory::editList(QWidget *w, QStringList &list)
{
	QDialog* dialog = new QDialog(w->topLevelWidget(), "stringlist_dialog", true);
	QVBoxLayout *vbox = new QVBoxLayout(dialog, 2);

	KEditListBox *edit = new KEditListBox(dialog, "editlist");
	vbox->addWidget(edit);

	QHBoxLayout *hbox = new QHBoxLayout(vbox, 6);
	KPushButton *pbOk = new KPushButton(KStdGuiItem::ok(), dialog);
	KPushButton *pbCancel = new KPushButton(KStdGuiItem::cancel(), dialog);
	QSpacerItem *spacer = new QSpacerItem(30, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

	connect(pbOk, SIGNAL(clicked()), dialog, SLOT(accept()));
	connect(pbCancel, SIGNAL(clicked()), dialog, SLOT(reject()));

	hbox->addItem(spacer);
	hbox->addWidget(pbOk);
	hbox->addWidget(pbCancel);

	edit->insertStringList(list);

	if(dialog->exec() == QDialog::Accepted)
	{
		list = edit->items();
		return true;
	}
	else
		return false;
}

bool
WidgetFactory::eventFilter(QObject *obj, QEvent *ev)
{
	QWidget *w = m_editor ? m_editor : (QWidget *)m_widget;

	if(obj != (QObject *)w)
		return false;

	if(ev->type() == QEvent::FocusOut)
	{
		QWidget *focus = w->topLevelWidget()->focusWidget();
		if(w != focus && !w->child(focus->name(), focus->className()))
			resetEditor();
	}
	else if(ev->type() == QEvent::KeyPress)
	{
		QKeyEvent *e = static_cast<QKeyEvent*>(ev);
		if(((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)) && (e->state() != ControlButton))
			resetEditor();
	}
	else if(ev->type() == QEvent::ContextMenu)
		return true;

	return false;
}

void
WidgetFactory::resetEditor()
{
	if(!m_editor && m_widget)
	{
		m_widget->removeEventFilter(this);
		m_widget->installEventFilter(m_container);
	}
	else if(m_editor)
	{
		changeText(m_editor->text());
		disconnect(m_editor, 0, this, 0);
		m_editor->deleteLater();
	}

	if(m_widget)
		disconnect(m_widget, 0, this, 0);
	delete m_handles;
	m_editor = 0;
	m_widget = 0;
	m_handles = 0;
}

void
WidgetFactory::editorDeleted()
{
	delete m_handles;
	m_editor = 0;
	m_handles = 0;
}

void
WidgetFactory::changeProperty(const char *name, const QVariant &value, Container *container)
{
	if (!container->form()->manager())
		return;
	KFormDesigner::ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
	(*buff)[name]->setValue(value, true);
}

void
WidgetFactory::addPropertyDescription(Container *container, const char *prop, const QString &desc)
{
	ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
	buff->addPropertyDescription(prop, desc);
}

void
WidgetFactory::addValueDescription(Container *container, const char *value, const QString &desc)
{
	ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
	buff->addValueDescription(value, desc);
}

void installRecursiveEventFilter(QObject *object, QObject *container)
{
	object->installEventFilter(container);
	if(!object->isWidgetType())
		return;
	((QWidget*)object)->setCursor(QCursor(Qt::ArrowCursor));

	if(!object->children())
		return;

	QObjectList list = *(object->children());
	for(QObject *obj = list.first(); obj; obj = list.next())
		installRecursiveEventFilter(obj, container);
}

WidgetFactory::~WidgetFactory()
{
}
}

#include "widgetfactory.moc"
