/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qstrlist.h>
#include <qlineedit.h>
#include <qheader.h>

#include <klocale.h>
#include <kdebug.h>

#include "kexipropertyeditoritem.h"
#include "kexipropertybuffer.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"
#include "propertyeditorfile.h"
#include "propertyeditorfont.h"

#include "kexipropertyeditor.h"


KexiPropertyEditor::KexiPropertyEditor(QWidget *parent, bool returnToAccept, const char *name)
 : KListView(parent, name)
{
	addColumn(i18n("Property"), 145);
	addColumn(i18n("Value"), 100);

	m_currentEditor = 0;
	m_returnToAccept = returnToAccept;

	connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
	connect(header(), SIGNAL(sizeChange(int, int, int)), this, SLOT(slotColumnSizeChanged(int, int, int)));

	m_buffer = 0;
	
	QColorGroup cg = this->colorGroup();
//	setAlternateBackground(cg.highlight().light(120));
	setFullWidth(true);
	setRootIsDecorated(true);
}


void
KexiPropertyEditor::slotClicked(QListViewItem *item)
{
	if(item)
	{
		int y = viewportToContents(QPoint(0, itemRect(item).y())).y();
		kdDebug() << "KexiPropertyEditor::slotClicked() y: " << y << endl;
		QRect g(columnWidth(0), y, columnWidth(1), item->height());
		KexiPropertyEditorItem *i = static_cast<KexiPropertyEditorItem *>(item);
		createEditor(i, g);
	}
}

void
KexiPropertyEditor::createEditor(KexiPropertyEditorItem *i, const QRect &geometry)
{
	kdDebug() << "KexiPropertyEditor::createEditor: Create editor for type " << i->type() << endl;
	if(m_currentEditor)
	{
		m_editItem->setValue(m_currentEditor->getValue());
		delete m_currentEditor;
	}

	KexiPropertySubEditor *editor=0;
	switch(i->type())
	{
		case QVariant::Bool:
			editor = new PropertyEditorBool(viewport(), i->property());
			break;

		case QVariant::String:
		case QVariant::CString:
			editor = new PropertyEditorInput(viewport(), i->property());
			break;
   
		case QVariant::Int:
			editor = new PropertyEditorSpin(viewport(), i->property());
			break;
			
		case QVariant::Double:
			editor = new PropertyEditorDblSpin(viewport(), i->property());
			break;

		case QVariant::StringList:
			editor = new PropertyEditorList(viewport(), i->property());
			break;

		case QVariant::BitArray:
//			editor = new EventEditorEditor(viewport(), i);
			break;
			
		case QVariant::Font:
			editor = new PropertyEditorFont(viewport(), i->property());
			break;
			
		case QVariant::Pixmap:
			editor = new PropertyEditorPixmap(viewport(), i->property());
			break;
			
		case QVariant::Color:
			editor = new PropertyEditorColor(viewport(), i->property());
			break;

		default:
			m_currentEditor = 0;
			kdDebug() << "PropertyEditor::createEditor: No editor created!" << endl;
			return;
	}

	connect(editor, SIGNAL(reject(KexiPropertySubEditor *)), this,
		SLOT(slotEditorReject(KexiPropertySubEditor *)));

	if(m_returnToAccept)
	{
		connect(editor, SIGNAL(accept(KexiPropertySubEditor *)), this,
			SLOT(slotEditorAccept(KexiPropertySubEditor *)));
	}

	connect(editor, SIGNAL(changed(KexiPropertySubEditor *)), this,
		SLOT(slotValueChanged(KexiPropertySubEditor *)));
	editor->setGeometry(geometry);
	editor->resize(geometry.width(), geometry.height());
	editor->show();
	addChild(editor);
	moveChild(editor, geometry.x(), geometry.y());

	m_currentEditor = editor;
	m_editItem = i;
}

void
KexiPropertyEditor::slotValueChanged(KexiPropertySubEditor *editor)
{
	if(m_currentEditor) {
		m_editItem->setValue(m_currentEditor->getValue());
		if(m_buffer)
		{
			m_buffer->changeProperty( m_editItem->name().latin1(), editor->getValue());
		}
	}

	if(!m_returnToAccept)
	{
		emit itemRenamed(m_editItem);
	}
}

void
KexiPropertyEditor::slotEditorAccept(KexiPropertySubEditor *editor)
{
	emit itemRenamed(m_editItem);
	if(m_currentEditor)
	{
		m_currentEditor->hide();
	}
}

void
KexiPropertyEditor::slotEditorReject(KexiPropertySubEditor *editor)
{
	editor->hide();
}

void
KexiPropertyEditor::slotColumnSizeChanged(int section, int, int newS)
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
KexiPropertyEditor::reset(bool editorOnly)
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

QSize KexiPropertyEditor::sizeHint() const
{
	if (firstChild())
		 return QSize(KListView::sizeHint().width(), firstChild()->height()*childCount());
	return KListView::sizeHint();
}


void
KexiPropertyEditor::fill()
{
	reset(false);

	KexiPropertyBuffer::Iterator it;
	
	for(it = m_buffer->begin(); it != m_buffer->end(); ++it)
	{
		new KexiPropertyEditorItem(this, &(it.data()) );
	}
}



KexiPropertyEditor::~KexiPropertyEditor()
{
}


#include "kexipropertyeditor.moc"
