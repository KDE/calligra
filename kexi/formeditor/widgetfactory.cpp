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

#include <klineedit.h>

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
	m_editor = 0;
	m_widget = 0;
	m_handles = 0;
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
	connect(m_editor, SIGNAL(destroyed()), this, SLOT(editorDeleted()));

	m_handles = new ResizeHandleSet(editor, true);

	m_editor = editor;
	m_widget = w;
	return editor;
}

bool
WidgetFactory::eventFilter(QObject *obj, QEvent *ev)
{
	if(obj != (QObject *)m_editor)
		return false;

	if(ev->type() == QEvent::FocusOut)
		resetEditor();
	else if(ev->type() == QEvent::KeyPress)
	{
		QKeyEvent *e = static_cast<QKeyEvent*>(ev);
		if((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))
			resetEditor();
	}
	else if(ev->type() == QEvent::ContextMenu)
		return true;

	return false;
}

void
WidgetFactory::resetEditor()
{
	if(!m_editor)
		return;
	changeText(m_editor->text());
	m_editor->deleteLater();
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
	KFormDesigner::ObjectPropertyBuffer *buff = container->form()->manager()->buffer();
	(*buff)[name]->setValue(value, true);
}

WidgetFactory::~WidgetFactory()
{
}
}

#include "widgetfactory.moc"
