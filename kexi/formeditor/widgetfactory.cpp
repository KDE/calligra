/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
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
#include <kdebug.h>
#include <klocale.h>
#include <klineedit.h>
#include <kdialogbase.h>
#include <keditlistbox.h>

#include "extrawidgets.h"
#include "resizehandle.h"
#include "objpropbuffer.h"
#include "formmanager.h"
#include "form.h"
#include "container.h"
#include "objecttree.h"
#include "widgetfactory.h"


namespace KFormDesigner {

///// Widget Factory //////////////////////////

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
	editor->setFont(w->font());
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
	ObjectTreeItem *tree = container->form()->objectTree()->lookup(w->name());
	if(!tree)
		return;
	tree->eventEater()->setContainer(this);

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
	KDialogBase dialog(w->topLevelWidget(), "stringlist_dialog", true, i18n("Edit List of Items"),
	    KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, false);

	KEditListBox *edit = new KEditListBox(i18n("%1 Contents").arg(w->name()), &dialog, "editlist");
	dialog.setMainWidget(edit);
	edit->insertStringList(list);
//	edit->show();

	if(dialog.exec() == QDialog::Accepted)
	{
		list = edit->items();
		return true;
	}
	return false;
}

bool
WidgetFactory::editRichText(QWidget *w, QString &text)
{
	RichTextDialog dlg(w, text);
	if(dlg.exec()== QDialog::Accepted)
	{
		text = dlg.text();
		return true;
	}
	return false;
}

void
WidgetFactory::editListView(QListView *listview)
{
	EditListViewDialog dlg(listview, ((QWidget*)listview)->topLevelWidget());
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
		ObjectTreeItem *tree = m_container->form()->objectTree()->lookup(m_widget->name());
		if(!tree)
		{
			kdDebug() << "WidgetFactory::resetEditor() : error cannot found a tree item " << endl;
			return;
		}
		tree->eventEater()->setContainer(m_container);
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
	if(container->form()->selectedWidgets()->count() > 1)
	{ // If eg multiple labels are selected, we only want to change the text of one of them (the one the user cliked on)
		container->form()->selectedWidgets()->first()->setProperty(name, value);
	}
	else
	{
		KFormDesigner::ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
		if((*buff)[name])
			(*buff)[name]->setValue(value, true);
	}
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

WidgetFactory::~WidgetFactory()
{
}

}

#include "widgetfactory.moc"
