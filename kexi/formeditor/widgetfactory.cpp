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
#include <qcursor.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>
#include <klineedit.h>
#include <kdialogbase.h>
#include <keditlistbox.h>

#include "richtextdialog.h"
#include "editlistviewdialog.h"
#include "resizehandle.h"
#include "objpropbuffer.h"
#include "formmanager.h"
#include "form.h"
#include "container.h"
#include "objecttree.h"
#include "widgetfactory.h"


namespace KFormDesigner {

static void setRecursiveCursor(QWidget *w, Form *form)
{
	ObjectTreeItem *tree = form->objectTree()->lookup(w->name());
	if(tree && ((tree->modifiedProperties()->contains("cursor")) || !tree->children()->isEmpty()) )
			return; // if the user has set a cursor for this widget or this is a container, don't change it

	if(w->ownCursor())
		w->setCursor(Qt::ArrowCursor);

	QObjectList *l = w->queryList( "QWidget" );
	for(QObject *o = l->first(); o; o = l->next())
		((QWidget*)o)->setCursor(Qt::ArrowCursor);
	delete l;
}

///// Widget Factory //////////////////////////

WidgetFactory::WidgetFactory(QObject *parent, const char *name)
 : QObject(parent, name)
{
}

KLineEdit*
WidgetFactory::createEditor(const QString &text, QWidget *w, Container *container, QRect geometry, int align,  bool useFrame, BackgroundMode background)
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
	editor->setMargin(2); //to move away from resize handle
	editor->show();
	editor->setFocus();
	editor->selectAll();
	connect(editor, SIGNAL(textChanged(const QString&)), this, SLOT(changeText(const QString&)));
	connect(w, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
	connect(editor, SIGNAL(destroyed()), this, SLOT(editorDeleted()));

	m_handles = new ResizeHandleSet(w, container->form(), true);

	ObjectTreeItem *tree = container->form()->objectTree()->lookup(w->name());
	if(!tree)
		return 0;
	tree->eventEater()->setContainer(this);

	m_editor = editor;
	m_widget = w;
	m_firstText = text;
	m_container = container;

	changeText(text); // to update size of the widget
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
	m_handles = new ResizeHandleSet(w, container->form(), true);
	m_widget = w;
	m_container = container;
	m_editor = 0;

	connect(w, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
}

bool
WidgetFactory::editList(QWidget *w, QStringList &list)
{
	KDialogBase dialog(w->topLevelWidget(), "stringlist_dialog", true, i18n("Edit List of Items"),
	    KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, false);

	KEditListBox *edit = new KEditListBox(i18n("Contents of %1").arg(w->name()), &dialog, "editlist");
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
	EditListViewDialog dlg(((QWidget*)listview)->topLevelWidget());
	dlg.exec(listview);
}

bool
WidgetFactory::eventFilter(QObject *obj, QEvent *ev)
{
	// widget resize using resize handles
	if( ((ev->type() == QEvent::Resize) || (ev->type() == QEvent::Move) ) && m_editor && (obj == m_widget))
		resizeEditor(m_widget, m_widget->className());
	// paint event for container edited (eg button group)
	else if((ev->type() == QEvent::Paint) && m_editor && (obj == m_widget))
		return m_container->eventFilter(obj, ev);

	QWidget *w = m_editor ? m_editor : (QWidget *)m_widget;

	if(obj != (QObject *)w)
		return false;

	else if(ev->type() == QEvent::FocusOut)
	{
		QWidget *focus = w->topLevelWidget()->focusWidget();
		if(w != focus && !w->child(focus->name(), focus->className()))
			resetEditor();
	}
	else if(ev->type() == QEvent::KeyPress)
	{
		QKeyEvent *e = static_cast<QKeyEvent*>(ev);
		if(((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)) && (e->state() != AltButton))
			resetEditor();
		if(e->key() == Qt::Key_Escape)
		{
			m_editor->setText(m_firstText);
			//changeText(m_firstText);
			resetEditor();
		}
	}
	else if(ev->type() == QEvent::ContextMenu)
		return true;

	if(obj == m_widget)
		return m_container->eventFilter(obj, ev);
	else
		return false;
}

void
WidgetFactory::resetEditor()
{
	m_container->stopInlineEditing();

	if(m_widget)
	{
		ObjectTreeItem *tree = m_container->form()->objectTree()->lookup(m_widget->name());
		if(!tree)
		{
			kdDebug() << "WidgetFactory::resetEditor() : error cannot found a tree item " << endl;
			return;
		}
		tree->eventEater()->setContainer(m_container);
		if(!m_editor && m_widget)
			setRecursiveCursor(m_widget, m_container->form());
	}
	if(m_editor)
	{
		changeText(m_editor->text());
		disconnect(m_editor, 0, this, 0);
		m_editor->deleteLater();
	}

	if(m_widget)
	{
		disconnect(m_widget, 0, this, 0);
		m_widget->repaint();
	}

	delete m_handles;
	m_editor = 0;
	m_widget = 0;
	m_handles = 0;
	m_container = 0;
}

void
WidgetFactory::widgetDestroyed()
{
	if(m_editor)
	{
		m_editor->deleteLater();
		m_editor = 0;
	}

	delete m_handles;
	m_widget = 0;
	m_handles = 0;
	m_container = 0;
}

void
WidgetFactory::editorDeleted()
{
	delete m_handles;
	m_widget = 0;
	m_handles = 0;
	m_container = 0;
	m_editor= 0;
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
			(*buff)[name] = value;
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

bool
WidgetFactory::showProperty(const QString&, QWidget*, const QString&, bool multiple)
{
	return !multiple;
}

void
WidgetFactory::resizeEditor(QWidget *, const QString&)
{}

void
WidgetFactory::changeText(const QString&)
{}

void
WidgetFactory::readSpecialProperty(const QString &, QDomElement &, QWidget *, ObjectTreeItem *)
{}

void
WidgetFactory::saveSpecialProperty(const QString &, const QString &, const QVariant&, QWidget *, QDomElement &,  QDomDocument &)
{}

WidgetFactory::~WidgetFactory()
{
}

}

#include "widgetfactory.moc"
