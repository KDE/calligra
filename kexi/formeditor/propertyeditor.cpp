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

#include <klocale.h>
#include <kdebug.h>

#include "propertyeditor.h"
#include "propertyeditoritem.h"

#include "propertyeditoreditor.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"

PropertyEditor::PropertyEditor(QWidget *parent, const char *name)
 : KListView(parent, name)
{
	addColumn(i18n("Property"));
	addColumn(i18n("Value"));

	m_currentEditor = 0;

	connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
}

void
PropertyEditor::setObject(QObject *object)
{
	clear();

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
	kdDebug() << "PropertyEditor::createEditor()" << endl;
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
			editor = new PropertyEditorInput(viewport(), i->type(), i->value());
			break;

		case QVariant::Int:
			editor = new PropertyEditorSpin(viewport(), i->type(), i->value());
			break;

		default:
			m_currentEditor = 0;
			return;
	}
	
	connect(editor, SIGNAL(reject(PropertyEditorEditor *)), this,
	 SLOT(slotEditorReject(PropertyEditorEditor *)));
	editor->setGeometry(geometry);
	editor->resize(geometry.width(), geometry.height());
	editor->show();
	addChild(editor);
	moveChild(editor, geometry.x(), geometry.y());

	m_currentEditor = editor;
	m_editItem = i;
}

void
PropertyEditor::slotEditorAccept(PropertyEditorEditor *editor)
{
}

void
PropertyEditor::slotEditorReject(PropertyEditorEditor *editor)
{
	editor->hide();
}

PropertyEditor::~PropertyEditor()
{
}

#include "propertyeditor.moc"
