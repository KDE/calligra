/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexipropertyeditor.h"

#include <qheader.h>
#include <qevent.h>
#include <qfontmetrics.h>

#include <klocale.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kiconloader.h>

#include "kexipropertybuffer.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"
#include "propertyeditorfile.h"
#include "propertyeditorfont.h"
#include "propertyeditordate.h"

KexiPropertyEditor::KexiPropertyEditor(QWidget *parent, bool autoSync, const char *name)
 : KListView(parent, name)
 , m_items(101)
{
	m_items.setAutoDelete(false);

	addColumn(i18n("Property"));//, 145);
	addColumn(i18n("Value"));//, 100);

	m_buffer = 0;
	m_topItem = 0;
	m_editItem = 0;
	m_sync = autoSync;
	slotValueChanged_enabled = true;

	connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
	connect(this, SIGNAL(expanded(QListViewItem *)), this, SLOT(slotExpanded(QListViewItem *)));
	connect(this, SIGNAL(collapsed(QListViewItem *)), this, SLOT(slotCollapsed(QListViewItem *)));
	connect(header(), SIGNAL(sizeChange(int, int, int)), this, SLOT(slotColumnSizeChanged(int, int, int)));
	connect(header(), SIGNAL(clicked(int)), this, SLOT(moveEditor()));
	connect(header(), SIGNAL(sectionHandleDoubleClicked (int)), this, SLOT(slotColumnSizeChanged(int)));

	m_defaults = new KPushButton(viewport());
	m_defaults->setPixmap(SmallIcon("reload"));
	m_defaults->hide();
	connect(m_defaults, SIGNAL(clicked()), this, SLOT(resetItem()));

	setFullWidth(true);
	setShowSortIndicator(false);
	setTooltipColumn(0);
	setSorting(0);
	setItemMargin(3);
	header()->setMovingEnabled( false );
}

KexiPropertyEditor::~KexiPropertyEditor()
{
}

void
KexiPropertyEditor::slotClicked(QListViewItem *item)
{
	if (!item)
		return;
//		int y = viewportToContents(QPoint(0, itemRect(item).y())).y();
//		kdDebug() << "KexiPropertyEditor::slotClicked() y: " << y << endl;
//		QRect g(columnWidth(0), y, columnWidth(1), item->height());
	KexiPropertyEditorItem *i = static_cast<KexiPropertyEditorItem *>(item);
	createEditor(i);//, g);
}

void
KexiPropertyEditor::slotExpanded(QListViewItem *item)
{
	if (!item)
		return;
	moveEditor();
}

void
KexiPropertyEditor::slotCollapsed(QListViewItem *item)
{
	if (!item)
		return;
	moveEditor();
}


void
KexiPropertyEditor::createEditor(KexiPropertyEditorItem *i)//, const QRect &geometry)
{
//	kdDebug() << "KexiPropertyEditor::createEditor: Create editor for type " << i->type() << endl;
	int y = viewportToContents(QPoint(0, itemRect(i).y())).y();
	QRect geometry(columnWidth(0), y, columnWidth(1), i->height());

	if(m_currentEditor)
	{
		slotEditorAccept(m_currentEditor);
		delete m_currentEditor;
	}

//	m_defaults->hide();

	m_editItem = i;

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
			if(i->property()->value().type() == QVariant::StringList)
				editor = new PropertyEditorMultiList(viewport(), i->property());
			else
				editor = new PropertyEditorList(viewport(), i->property());
			break;

		case QVariant::BitArray:
//			editor = new EventEditorEditor(viewport(), i);
			break;

		case QVariant::Font:
			editor = new PropertyEditorFont(viewport(), i->property());
			break;

		case QVariant::Pixmap:
		case QVariant::Invalid:
			editor = new PropertyEditorPixmap(viewport(), i->property());
			break;

		case QVariant::Color:
			editor = new PropertyEditorColor(viewport(), i->property());
			break;

		case QVariant::Date:
			editor = new PropertyEditorDate(viewport(), i->property());
			break;

		case QVariant::Time:
			editor = new PropertyEditorTime(viewport(), i->property());
			break;

		case QVariant::DateTime:
			editor = new PropertyEditorDateTime(viewport(), i->property());
			break;

		case QVariant::Cursor:
			editor = new PropertyEditorCursor(viewport(), i->property());
			break;

		default:
//			m_currentEditor = 0;
			setFocus();
			kdDebug() << "PropertyEditor::createEditor: No editor created!" << endl;
//			return;
	}

	if (editor) {
		connect(editor, SIGNAL(reject(KexiPropertySubEditor *)), this,
			SLOT(slotEditorReject(KexiPropertySubEditor *)));

		connect(editor, SIGNAL(accept(KexiPropertySubEditor *)), this,
			SLOT(slotEditorAccept(KexiPropertySubEditor *)));

		connect(editor, SIGNAL(changed(KexiPropertySubEditor *)), this,
			SLOT(slotValueChanged(KexiPropertySubEditor *)));

		addChild(editor);
		moveChild(editor, geometry.x(), geometry.y());
		editor->show();

		editor->setFocus();
	}
	m_currentEditor = editor;
//	m_editItem = i;
	showDefaultsButton( i->property()->changed() );
}

void
KexiPropertyEditor::showDefaultsButton( bool show )
{
	int y = viewportToContents(QPoint(0, itemRect(m_editItem).y())).y();
	QRect geometry(columnWidth(0), y, columnWidth(1), m_editItem->height());
	m_defaults->resize(geometry.height(), geometry.height());

	if (!show) {
		if (m_currentEditor) {
			if (m_currentEditor->leavesTheSpaceForRevertButton()) {
				geometry.setWidth(geometry.width()-m_defaults->width());
			}
			m_currentEditor->resize(geometry.width(), geometry.height());
		}
		m_defaults->hide();
		return;
	}

	QPoint p = contentsToViewport(QPoint(0, geometry.y()));
	m_defaults->move(geometry.x() + geometry.width() - m_defaults->width(), p.y());
	if (m_currentEditor) {
		m_currentEditor->move(m_currentEditor->x(), p.y());
		m_currentEditor->resize(geometry.width()-m_defaults->width(), geometry.height());
	}
	m_defaults->show();
}

void
KexiPropertyEditor::slotValueChanged(KexiPropertySubEditor *editor)
{
	if (!slotValueChanged_enabled)
		return;
	if(m_currentEditor)
	{
		QVariant value = m_currentEditor->value();
//js: not needed		m_editItem->setValue(value);
		bool sync = (m_editItem->property()->autoSync() != 0 && m_editItem->property()->autoSync() != 1) ?
		         m_sync : (bool)m_editItem->property()->autoSync();
		if(m_buffer && sync)
		{
			if (m_editItem->property()->parent()) {
				m_editItem->property()->setValue( value );
			}
			else {
				m_buffer->changeProperty(m_editItem->name(), value);//getComposedValue());
			}

/*			if(m_editItem->depth()==1) {
				m_buffer->changeProperty(m_editItem->name().latin1(), value);
			}
			else if(m_editItem->depth()==2)
			{
				KexiPropertyEditorItem *parent = static_cast<KexiPropertyEditorItem*>(m_editItem->parent());
//js: TODO				m_buffer->changeProperty(parent->name().latin1(), parent->getComposedValue());
			}*/
		}
		else
		{
			if(m_editItem->depth()==2)
			{
				KexiPropertyEditorItem *parent = static_cast<KexiPropertyEditorItem*>(m_editItem->parent());
//js: TODO				parent->getComposedValue();
			}
		}
		m_editItem->updateValue();
		showDefaultsButton( m_editItem->property()->changed() );
		emit valueChanged(m_editItem->name(), value);
	}
}

void
KexiPropertyEditor::slotEditorAccept(KexiPropertySubEditor *editor)
{
//	kdDebug() << "KexiPropertyEditor::slotEditorAccept" << endl;

	if(m_currentEditor)
	{
		QVariant value = m_currentEditor->value();
//js: not needed		m_editItem->setValue(value);
		if(m_buffer)
		{
			m_buffer->debug();

			if (m_editItem->property()->parent()) {
				m_editItem->property()->setValue( value );
			}
			else {
				m_buffer->changeProperty(m_editItem->name(), value);
			}

/*			if(m_editItem->depth()==1) {
				m_buffer->changeProperty(m_editItem->name().latin1(), value);
			}
			else if(m_editItem->depth()==2)
			{
				KexiPropertyEditorItem *parent = static_cast<KexiPropertyEditorItem*>(m_editItem->parent());
				m_buffer->changeProperty(parent->name(), parent->value());//getComposedValue());
			}*/
		}
//		m_currentEditor->hide();
//		m_currentEditor->clearFocus();
		m_editItem->updateValue();
		emit valueChanged(m_editItem->name(), value); //todo: only when changed
	}
}

void
KexiPropertyEditor::slotEditorReject(KexiPropertySubEditor *editor)
{
	if(m_currentEditor)
	{
		bool sync = (m_editItem->property()->autoSync() != 0 && m_editItem->property()->autoSync() != 1) ?
		         m_sync : (bool)m_editItem->property()->autoSync();
		if(!sync)
		{
			//js: not needed m_editItem->setValue(m_editItem->property()->value());
			m_currentEditor->setValue(m_editItem->property()->value());
		}
		else
		{
			//js: not needed m_editItem->setValue(m_editItem->oldValue());
			m_currentEditor->setValue(m_editItem->property()->oldValue());
		}
		m_editItem->updateValue();
	}
//	editor->hide();
//	editor->setFocusPolicy(QWidget::NoFocus);
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
			if(m_defaults->isVisible())
				m_currentEditor->resize(newS - m_defaults->width(), m_currentEditor->height());
			else
				m_currentEditor->resize(
					newS-(m_currentEditor->leavesTheSpaceForRevertButton()?m_defaults->width():0),
					m_currentEditor->height());
		}
	}
}

void
KexiPropertyEditor::slotColumnSizeChanged(int section)
{
	setColumnWidth(1, viewport()->width() - columnWidth(0));
	slotColumnSizeChanged(section, 0, header()->sectionSize(section));
	if(m_currentEditor)
	{
		if(m_defaults->isVisible())
			m_currentEditor->resize(columnWidth(1) - m_defaults->width(), m_currentEditor->height());
		else
			m_currentEditor->resize(
				columnWidth(1)-(m_currentEditor->leavesTheSpaceForRevertButton()?m_defaults->width():0),
				m_currentEditor->height());
	}
}

void
KexiPropertyEditor::reset(bool editorOnly)
{
	delete m_currentEditor;
	m_currentEditor = 0;
	if(m_defaults->isVisible())
		m_defaults->hide();

	if(!editorOnly)
	{
		clear();
		m_topItem = 0;
	}
}

QSize KexiPropertyEditor::sizeHint() const
{
	return QSize( QFontMetrics(font()).width(columnText(0)+columnText(1)+"   "),
		KListView::sizeHint().height());
//	if (firstChild())
//		 return QSize(KListView::sizeHint().width(), firstChild()->height()*childCount());
//	return KListView::sizeHint();
}

void
KexiPropertyEditor::setBuffer(KexiPropertyBuffer *b)
{
	if (m_buffer) {
		m_buffer->disconnect(this);
	}
	m_buffer = b;
	if (m_buffer) {
		//receive property changes
		connect(m_buffer,SIGNAL(propertyChanged(KexiPropertyBuffer&,KexiProperty&)),
			this,SLOT(slotPropertyChanged(KexiPropertyBuffer&,KexiProperty&)));
		connect(m_buffer,SIGNAL(propertyReset(KexiPropertyBuffer&,KexiProperty&)),
			this, SLOT(slotPropertyReset(KexiPropertyBuffer&,KexiProperty&)));
		connect(m_buffer,SIGNAL(destroying()), this, SLOT(slotBufferDestroying()));
	}
	fill();
}

void KexiPropertyEditor::slotPropertyReset(KexiPropertyBuffer &buf,KexiProperty &prop)
{
	if (m_currentEditor) {
		slotValueChanged_enabled = false;
		m_currentEditor->setValue(m_editItem->property()->value());
		slotValueChanged_enabled = true;
	}
	else {
		m_editItem->updateValue();
	}
	//update children
	m_editItem->updateChildrenValue();

	showDefaultsButton( false );
}

void KexiPropertyEditor::slotBufferDestroying()
{
	m_buffer = 0;
	fill();
}

void
KexiPropertyEditor::fill()
{
	reset(false);

	if (!m_buffer)
		return;

	KexiProperty::ListIterator it(*m_buffer->list());

	if(!m_topItem)
	{
		m_topItem = new KexiPropertyEditorItem(this,"Top Item");
	}

	m_items.clear();

	KexiPropertyEditorItem *item=0;
	for(;it.current(); ++it)
	{
		if (it.current()->isVisible()) {
			item = new KexiPropertyEditorItem(m_topItem, it.current(), item);
			m_items.insert(it.current()->name(), item);
		}
	}
}

void
KexiPropertyEditor::resetItem()
{
	if(m_editItem)
	{
//		if (m_currentEditor) {
//			m_currentEditor->setValue(m_editItem->property()->oldValue());
//		}

//		m_editItem->property()->setValue( m_editItem->property()->oldValue(), false );
		m_editItem->property()->resetValue();
/*
		if (!m_currentEditor) {
			m_editItem->updateValue();
		}
		//update children
		m_editItem->updateChildrenValue();

		showDefaultsButton( false );
*/
//js: not needed		else
//js: not needed			m_editItem->setValue(m_editItem->property()->oldValue());
	}
}

void
KexiPropertyEditor::moveEditor()
{
//	if (m_editItem) {
	if (m_currentEditor) {
		QPoint p = contentsToViewport(QPoint(0, itemPos(m_editItem)));
//		if(m_currentEditor)
		m_currentEditor->move(m_currentEditor->x(), p.y());
		if(m_defaults->isVisible())
			m_defaults->move(m_defaults->x(), p.y());
	}
}

void
KexiPropertyEditor::resizeEvent(QResizeEvent *ev)
{
	KListView::resizeEvent(ev);
	if(m_defaults->isVisible())
	{
		QRect r = itemRect(m_editItem);
		if(r.y()) // r.y() == 0 if the item is not visible on the screen
			m_defaults->move(r.x() + r.width() - m_defaults->width(), r.y());
//		if(m_currentEditor)
//			m_currentEditor->resize(columnWidth(1) - m_defaults->width(), m_currentEditor->height());
	}

	if(m_currentEditor) {
		m_currentEditor->resize(
			columnWidth(1)-((m_currentEditor->leavesTheSpaceForRevertButton()||m_defaults->isVisible()) ? m_defaults->width() : 0),
			m_currentEditor->height());
	}
}

void
KexiPropertyEditor::slotPropertyChanged(KexiPropertyBuffer &buf,KexiProperty &prop)
{
	if (static_cast<KexiPropertyBuffer*>(m_buffer)!=&buf)
		return;
	KexiPropertyEditorItem* item = m_items[prop.name()];
	if (!item) //this property is not visible here
		return;
	if(item == m_editItem)
		m_currentEditor->setValue(prop.value());
	item->updateValue();
	item->updateChildrenValue();
}


#include "kexipropertyeditor.moc"
