/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qstrlist.h>
#include <qmetaobject.h>
#include <qlineedit.h>
#include <qheader.h>

#include <klocale.h>
#include <kdebug.h>

#include "propertyeditor.h"
#include "propertyeditoritem.h"
#include "propertybuffer.h"

#include "propertyeditoreditor.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"
#include "eventeditoreditor.h"

PropertyEditor::PropertyEditor(QWidget *parent, bool returnToAccept, const char *name)
 : KListView(parent, name)
{
	addColumn(i18n("Property"));
	addColumn(i18n("Value"));

	m_currentEditor = 0;
	m_returnToAccept = returnToAccept;

	connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
	connect(header(), SIGNAL(sizeChange(int, int, int)), this, SLOT(slotColumnSizeChanged(int, int, int)));

	m_buffer = 0;
}

void
PropertyEditor::setObject(QObject *object)
{
	reset();

	QStrList pList = object->metaObject()->propertyNames(true);

	int count = 0;
	char *name;
	QStrListIterator it(pList);
	for(; (name = it.current()) != 0; ++it)
	{
		const QMetaProperty *meta = object->metaObject()->property(count, true);
		if(meta->designable())
		{
//			KListViewItem *i = new KListViewItem(this, *it, meta->type());
			PropertyEditorItem *i = new PropertyEditorItem(this, *it, object->property(*it).type(), object->property(*it), object);
		}
		count++;
	}
}

void
PropertyEditor::slotClicked(QListViewItem *item)
{
	if(item)
	{
//		QRect g(QPoint(itemRect(item).x() + columnWidth(0), viewportToContents(QPoint(0, itemRect(item).y()))),
//		 QPoint(columnWidth(1), item->height()));
		int y = viewportToContents(QPoint(0, itemRect(item).y())).y();
		kdDebug() << "PropertyEditor::slotClicked() y: " << y << endl;
		QRect g(columnWidth(0), y, columnWidth(1), item->height());
		PropertyEditorItem *i = static_cast<PropertyEditorItem *>(item);
		createEditor(i, g);
	}
}

void
PropertyEditor::createEditor(PropertyEditorItem *i, const QRect &geometry)
{
	kdDebug() << "PropertyEditor::createEditor: Create editor for type " << i->type() << endl;
	if(m_currentEditor)
	{
		m_editItem->setValue(m_currentEditor->getValue(), true);
		delete m_currentEditor;
	}

	PropertyEditorEditor *editor;
	switch(i->type())
	{
		case QVariant::Bool:
			editor = new PropertyEditorBool(viewport(), i->type(), i->value());
			break;

		case QVariant::String:
		case QVariant::CString:
			editor = new PropertyEditorInput(viewport(), i->type(), i->value());
			break;

		case QVariant::Int:
			editor = new PropertyEditorSpin(viewport(), i->type(), i->value());
			break;

		case QVariant::StringList:
			editor = new PropertyEditorList(viewport(), i->type(), i->value(), i->list());
			kdDebug() << "PropertyEditor::createEditor: ComboBox created!" << endl;
			break;

		case QVariant::BitArray:
			editor = new EventEditorEditor(viewport(), i);
			break;

		default:
			m_currentEditor = 0;
			kdDebug() << "PropertyEditor::createEditor: No editor created!" << endl;
			return;
	}

	connect(editor, SIGNAL(reject(PropertyEditorEditor *)), this,
		SLOT(slotEditorReject(PropertyEditorEditor *)));

	if(m_returnToAccept)
	{
		connect(editor, SIGNAL(accept(PropertyEditorEditor *)), this,
			SLOT(slotEditorAccept(PropertyEditorEditor *)));
	}

	connect(editor, SIGNAL(changed(PropertyEditorEditor *)), this,
		SLOT(slotValueChanged(PropertyEditorEditor *)));
	editor->setGeometry(geometry);
	editor->resize(geometry.width(), geometry.height());
	editor->show();
	addChild(editor);
	moveChild(editor, geometry.x(), geometry.y());

	m_currentEditor = editor;
	m_editItem = i;
}

void
PropertyEditor::slotValueChanged(PropertyEditorEditor *editor)
{
	if(m_currentEditor) {
		m_editItem->setValue(m_currentEditor->getValue(), true);
		if(m_buffer)
		{
			m_buffer->changeProperty(m_editItem->object(), m_editItem->name().latin1(), editor->getValue());
		}
	}

	if(!m_returnToAccept)
	{
		emit itemRenamed(m_editItem);
	}
}

void
PropertyEditor::slotEditorAccept(PropertyEditorEditor *editor)
{
	emit itemRenamed(m_editItem);
	editor->hide();
}

void
PropertyEditor::slotEditorReject(PropertyEditorEditor *editor)
{
	editor->hide();
}

void
PropertyEditor::slotColumnSizeChanged(int section, int, int newS)
{
	if(m_currentEditor)
	{
		if(section == 0)
		{
			m_currentEditor->move(newS, m_currentEditor->y());
		}
		else
		{
			m_currentEditor->resize(newS, m_currentEditor->height());
		}
	}
}

void
PropertyEditor::reset(bool editorOnly)
{
	if(m_currentEditor)
	{
		delete m_currentEditor;
		m_currentEditor = 0;
	}

	if(!editorOnly)
	{
		clear();
	}
}

PropertyEditor::~PropertyEditor()
{
}

#include "propertyeditor.moc"
