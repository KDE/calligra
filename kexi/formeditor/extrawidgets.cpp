/* This file is part of the KDE project
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

#include <qlayout.h>
#include <qtoolbutton.h>
#include <qcursor.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qheader.h>

#include <kstdguiitem.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <ktoolbar.h>
#include <kfontcombo.h>
#include <kcolorcombo.h>
#include <ktoolbarradiogroup.h>
#include <klistview.h>
#include <ktabwidget.h>
#include <klistbox.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>

#include "kexipropertyeditor.h"
#include "kexipropertybuffer.h"
#include "form.h"
#include "objecttreeview.h"
#include "objecttree.h"

#include "extrawidgets.h"

namespace KFormDesigner {

//////////////////////////////////////////////////////////////////////////////////
//////////////// A simple dialog to edit rich text   ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

RichTextDialog::RichTextDialog(QWidget *parent, const QString &text)
: KDialogBase(parent, "richtext_dialog", true, i18n("Edit rich text"), Ok|Cancel, Ok, false)
{
	QFrame *frame = makeMainWidget();
	QVBoxLayout *l = new QVBoxLayout(frame);
	l->setAutoAdd(true);

	m_toolbar = new KToolBar(frame);
	m_toolbar->setFlat(true);
	m_toolbar->show();

	m_fcombo = new KFontCombo(m_toolbar);
	m_toolbar->insertWidget(101, 40, m_fcombo);
	connect(m_fcombo, SIGNAL(textChanged(const QString&)), this, SLOT(changeFont(const QString &)));

	m_toolbar->insertSeparator();

	m_colCombo = new KColorCombo(m_toolbar);
	m_toolbar->insertWidget(102, 30, m_colCombo);
	connect(m_colCombo, SIGNAL(activated(const QColor&)), this, SLOT(changeColor(const QColor&)));

	m_toolbar->insertButton("text_bold", 103, true, i18n("Bold"));
	m_toolbar->insertButton("text_italic", 104, true, i18n("Italic"));
	m_toolbar->insertButton("text_under", 105, true, i18n("Underline"));
	m_toolbar->setToggle(103, true);
	m_toolbar->setToggle(104, true);
	m_toolbar->setToggle(105, true);
	m_toolbar->insertSeparator();

	m_toolbar->insertButton("text_super", 106, true, i18n("Superscript"));
	m_toolbar->insertButton("text_sub", 107, true, i18n("Subscript"));
	m_toolbar->setToggle(106, true);
	m_toolbar->setToggle(107, true);
	m_toolbar->insertSeparator();

	KToolBarRadioGroup *group = new KToolBarRadioGroup(m_toolbar);
	m_toolbar->insertButton("text_left", 201, true, i18n("Right Align"));
	m_toolbar->setToggle(201, true);
	group->addButton(201);
	m_toolbar->insertButton("text_center", 202, true, i18n("Left Align"));
	m_toolbar->setToggle(202, true);
	group->addButton(202);
	m_toolbar->insertButton("text_right", 203, true, i18n("Centered"));
	m_toolbar->setToggle(203, true);
	group->addButton(203);
	m_toolbar->insertButton("text_block", 204, true, i18n("Justified"));
	m_toolbar->setToggle(204, true);
	group->addButton(204);

	connect(m_toolbar, SIGNAL(toggled(int)), this, SLOT(buttonToggled(int)));

	m_edit = new KTextEdit(text, QString::null, frame, "richtext_edit");
	m_edit->setTextFormat(RichText);
	m_edit->setFocus();

	connect(m_edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(cursorPositionChanged(int, int)));
	connect(m_edit, SIGNAL(clicked(int, int)), this, SLOT(cursorPositionChanged(int, int)));
	connect(m_edit, SIGNAL(currentVerticalAlignmentChanged(VerticalAlignment)), this, SLOT(slotVerticalAlignmentChanged(VerticalAlignment)));

	m_edit->moveCursor(QTextEdit::MoveEnd, false);
	cursorPositionChanged(0, 0);
	m_edit->show();
	frame->show();
}

QString
RichTextDialog::text()
{
	return m_edit->text();
}

void
RichTextDialog::changeFont(const QString &font)
{
	m_edit->setFamily(font);
}

void
RichTextDialog::changeColor(const QColor &color)
{
	m_edit->setColor(color);
}

void
RichTextDialog::buttonToggled(int id)
{
	bool isOn = m_toolbar->isButtonOn(id);

	switch(id)
	{
		case 103: m_edit->setBold(isOn); break;
		case 104: m_edit->setItalic(isOn); break;
		case 105: m_edit->setUnderline(isOn); break;
		case 106:
		{
			if(isOn && m_toolbar->isButtonOn(107))
				m_toolbar->setButton(107, false);
			m_edit->setVerticalAlignment(isOn ? QTextEdit::AlignSuperScript : QTextEdit::AlignNormal);
			break;
		}
		case 107:
		{
			if(isOn && m_toolbar->isButtonOn(106))
				m_toolbar->setButton(106, false);
			m_edit->setVerticalAlignment(isOn ? QTextEdit::AlignSubScript : QTextEdit::AlignNormal);
			break;
		}
		case 201: case 202:
		case 203: case 204:
		{
			if(!isOn)  break;
			switch(id)
			{
				case 201:  m_edit->setAlignment(Qt::AlignLeft); break;
				case 202:  m_edit->setAlignment(Qt::AlignCenter); break;
				case 203:  m_edit->setAlignment(Qt::AlignRight); break;
				case 204:  m_edit->setAlignment(Qt::AlignJustify); break;
				default: break;
			}
		}
		default: break;
	}

}

void
RichTextDialog::cursorPositionChanged(int, int)
{
//	if (m_edit->hasSelectedText())
//		return;

	m_fcombo->setCurrentFont(m_edit->currentFont().family());
	m_colCombo->setColor(m_edit->color());
	m_toolbar->setButton(103, m_edit->bold());
	m_toolbar->setButton(104, m_edit->italic());
	m_toolbar->setButton(105, m_edit->underline());

	int id = 0;
	switch(m_edit->alignment())
	{
		case Qt::AlignLeft:    id = 201; break;
		case Qt::AlignCenter:  id = 202; break;
		case Qt::AlignRight:   id = 203; break;
		case Qt::AlignJustify: id = 204; break;
		default:  id = 201; break;
	}
	m_toolbar->setButton(id, true);
}

void
RichTextDialog::slotVerticalAlignmentChanged(VerticalAlignment align)
{
	switch(align)
	{
		case QTextEdit::AlignSuperScript:
		{
			m_toolbar->setButton(106, true);
			m_toolbar->setButton(107, false);
			break;
		}
		case QTextEdit::AlignSubScript:
		{
			m_toolbar->setButton(107, true);
			m_toolbar->setButton(106, false);
			break;
		}
		default:
		{
			m_toolbar->setButton(107, false);
			m_toolbar->setButton(106, false);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
// A Dialog to edit the contents of a listview ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

EditListViewDialog::EditListViewDialog(QListView *listview, QWidget *parent)
: KDialogBase(Tabbed, 0/* WFlags */, parent, "editlistview_dialog", true, i18n("Edit listview contents"), Ok|Cancel, Ok, false)
{
	if(!listview)
	{
		kdDebug() << "EditListViewDialog ERROR: no listview " << endl;
		return;
	}

	m_column = addPage(i18n("Columns"));
	m_contents = addPage(i18n("Contents"));

	///////// Setup the "Contents" page /////////////////////////////
	QHBoxLayout *layout = new QHBoxLayout(m_contents, 0, 6);

	//// Setup the icon toolbar /////////////////
	QVBoxLayout *vlayout = new QVBoxLayout(layout, 3);
	QToolButton *newRow = new QToolButton(m_contents);
	newRow->setIconSet(BarIconSet("edit_add"));
	newRow->setTextLabel(i18n("&Add Item"), true);
	vlayout->addWidget(newRow);
	m_buttons.append(newRow);
	connect(newRow, SIGNAL(clicked()), this, SLOT(newRow()));

	QToolButton *newChild = new QToolButton(m_contents);
	newChild->setIconSet(BarIconSet("1rightarrow"));
	newChild->setTextLabel(i18n("New &Subitem"), true);
	vlayout->addWidget(newChild);
	m_buttons.append(newChild);
	connect(newChild, SIGNAL(clicked()), this, SLOT(newChildRow()));

	QToolButton *delRow = new QToolButton(m_contents);
	delRow->setIconSet(BarIconSet("edit_remove"));
	delRow->setTextLabel(i18n("&Remove Item"), true);
	vlayout->addWidget(delRow);
	m_buttons.append(delRow);
	connect(delRow, SIGNAL(clicked()), this, SLOT(removeRow()));

	QToolButton *rowUp = new QToolButton(m_contents);
	rowUp->setIconSet(BarIconSet("1uparrow"));
	rowUp->setTextLabel(i18n("Move Item &Up"), true);
	vlayout->addWidget(rowUp);
	m_buttons.append(rowUp);
	connect(rowUp, SIGNAL(clicked()), this, SLOT(MoveRowUp()));

	QToolButton *rowDown = new QToolButton(m_contents);
	rowDown->setIconSet(BarIconSet("1downarrow"));
	rowDown->setTextLabel(i18n("Move Item &Down"), true);
	vlayout->addWidget(rowDown);
	m_buttons.append(rowDown);
	connect(rowDown, SIGNAL(clicked()), this, SLOT(MoveRowDown()));
	vlayout->addStretch();

	//// The listview ///////////
	m_listview = new KListView(m_contents, "editlistview_listview");
	m_listview->setItemsRenameable(true);
	m_listview->setItemsMovable(true);
	m_listview->setDragEnabled(true);
	m_listview->setAllColumnsShowFocus(true);
	m_listview->setRootIsDecorated(true);
	m_listview->setDropVisualizer(true);
	m_listview->setAcceptDrops(true);
	m_listview->setSorting(-1);
	layout->addWidget(m_listview);
	m_listview->setFocus();
	for(int i = 0; i < listview->columns(); i++)
	{
		m_listview->addColumn(listview->columnText(i), listview->columnWidth(i));
		m_listview->header()->setClickEnabled(listview->header()->isClickEnabled(), i);
		m_listview->header()->setResizeEnabled(listview->header()->isResizeEnabled(), i);
		m_listview->header()->setStretchEnabled(listview->header()->isStretchEnabled(), i);
		m_listview->setRenameable(i, true);
	}
	QListViewItem *item = listview->firstChild();
	while(item)
	{
		loadChildNodes(m_listview, item, 0);
		item = item->nextSibling();
	}
	connect(m_listview, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(updateButtons(QListViewItem*)));
	m_listview->setSelected(m_listview->firstChild(), true);
	if(!m_listview->firstChild())
		updateButtons(0);

	/////////////////// Setup the columns page ////////////////
	QHBoxLayout *hbox = new QHBoxLayout(m_column, 0, 6);

	// The "item properties" field
	m_editor = new KexiPropertyEditor(m_column, "editcolumn_propeditor");
	m_buffer = new KexiPropertyBuffer(this, "columns");
	m_buffer->add(new KexiProperty("caption", "Caption" ,i18n("Caption")));
	m_buffer->add(new KexiProperty("width", 100, i18n("Width")));
	m_buffer->add(new KexiProperty("clickable", QVariant(true, 3), i18n("Clickable")));
	m_buffer->add(new KexiProperty("resizable", QVariant(true, 3), i18n("Resizable")));
	m_buffer->add(new KexiProperty("fullwidth", QVariant(false, 3), i18n("Full Width")));
	m_editor->setBuffer(m_buffer);
	connect(m_buffer, SIGNAL(propertyChanged(KexiPropertyBuffer&, KexiProperty&)), this, SLOT(changeProperty(KexiPropertyBuffer&, KexiProperty&)));

	// Setup the icon toolbar //////////
	QVBoxLayout *vbox = new QVBoxLayout(hbox, 3);
	QToolButton *add = new QToolButton(m_column);
	add->setIconSet(BarIconSet("edit_add"));
	add->setTextLabel(i18n("&Add Item"), true);
	vbox->addWidget(add);
	m_buttons.append(add);
	connect(add, SIGNAL(clicked()), this, SLOT(newItem()));

	QToolButton *remove = new QToolButton(m_column);
	remove->setIconSet(BarIconSet("edit_remove"));
	remove->setTextLabel(i18n("&Remove Item"), true);
	vbox->addWidget(remove);
	m_buttons.append(remove);
	connect(remove, SIGNAL(clicked()), this, SLOT(removeItem()));

	QToolButton *up = new QToolButton(m_column);
	up->setIconSet(BarIconSet("1uparrow"));
	up->setTextLabel(i18n("Move Item &Up"), true);
	vbox->addWidget(up);
	m_buttons.append(up);
	connect(up, SIGNAL(clicked()), this, SLOT(MoveItemUp()));

	QToolButton *down = new QToolButton(m_column);
	down->setIconSet(BarIconSet("1downarrow"));
	down->setTextLabel(i18n("Move Item &Down"), true);
	vbox->addWidget(down);
	m_buttons.append(down);
	connect(down, SIGNAL(clicked()), this, SLOT(MoveItemDown()));
	vbox->addStretch();

	// The listbox with columns name /////
	m_listbox = new KListBox(m_column, "editlistview_columns");
	m_listbox->setFocus();
	hbox->insertWidget(0, m_listbox);
	hbox->addWidget(m_editor);
	for(int i = 0; i < listview->columns(); i++)
		m_listbox->insertItem(listview->columnText(i));
	connect(m_listbox, SIGNAL(currentChanged(QListBoxItem*)), this, SLOT(updateItemProperties(QListBoxItem*)));
	m_listbox->setSelected(0, true);

	//// Init dialog and display it ////////////////////////
	setInitialSize(QSize(500, 300), true);
	if( exec() == QDialog::Accepted)
	{
		listview->clear();
		for(int i = 0; i < m_listview->columns(); i++)
		{
			kdDebug() << "Saving column " << listview->columnText(i) << i << endl;
			if(listview->columns() <= i)
				listview->addColumn(m_listview->columnText(i), m_listview->columnWidth(i));
			else
			{
				listview->setColumnText(i, m_listview->columnText(i));
				listview->setColumnWidth(i, m_listview->columnWidth(i));
			}
			listview->header()->setClickEnabled(m_listview->header()->isClickEnabled(i), i);
			listview->header()->setResizeEnabled(m_listview->header()->isResizeEnabled(i), i);
			listview->header()->setStretchEnabled(m_listview->header()->isStretchEnabled(i), i);
		}

		QListViewItem *item = m_listview->firstChild();
		while(item)
		{
			loadChildNodes(listview, item, 0);
			item = item->nextSibling();
		}
	}
}

/// Columns page slots ///////
void
EditListViewDialog::changeProperty(KexiPropertyBuffer &buffer, KexiProperty &prop)
{
	if(&buffer != m_buffer)
		return;

	QString name = prop.name();
	if(name == "caption")
	{
		m_buffer->blockSignals(true);
		m_listbox->changeItem(prop.value().toString(), m_listbox->currentItem());
		m_listview->setColumnText(m_listbox->currentItem(), prop.value().toString());
		m_buffer->blockSignals(false);
	}
	else if(name == "width")
		m_listview->setColumnWidth(m_listbox->currentItem(), prop.value().toInt());
	else if(name == "resizable")
		m_listview->header()->setResizeEnabled(prop.value().toBool(), m_listbox->currentItem());
	else if(name == "clickable")
		m_listview->header()->setClickEnabled(prop.value().toBool(), m_listbox->currentItem());
	else if(name == "fullwidth")
		m_listview->header()->setStretchEnabled(prop.value().toBool(), m_listbox->currentItem());
}

void
EditListViewDialog::updateItemProperties(QListBoxItem *item)
{
	if(!item)
		return;

	int id = m_listbox->index(item);
	if(m_buffer)
	{
		m_buffer->blockSignals(true);
		(*m_buffer)["caption"]->setValue(m_listview->columnText(id), false);
		(*m_buffer)["width"]->setValue(m_listview->columnWidth(id), false);
		(*m_buffer)["clickable"]->setValue(QVariant(m_listview->header()->isClickEnabled(id), 4), false);
		(*m_buffer)["resizable"]->setValue(QVariant(m_listview->header()->isResizeEnabled(id), 4), false);
		(*m_buffer)["fullwidth"]->setValue(QVariant(m_listview->header()->isStretchEnabled(id), 4), false);
		m_buffer->blockSignals(false);
		m_editor->setBuffer(m_buffer);
	}

	m_buttons.at(7)->setEnabled(item->prev());
	m_buttons.at(8)->setEnabled(item->next());
}

void
EditListViewDialog::newItem()
{
	m_listbox->insertItem(i18n("New Column"));
	m_listview->addColumn(i18n("New Column"));
	m_listview->setRenameable(m_listview->columns() - 1, true);
	m_listbox->setCurrentItem(m_listbox->count() - 1);
	m_buttons.at(6)->setEnabled(true);
}

void
EditListViewDialog::removeItem()
{
	int current = m_listbox->currentItem();
	if(m_listbox->item(current + 1))
		m_listbox->setCurrentItem(current +1);
	else
		m_listbox->setCurrentItem(current - 1);

	m_listview->removeColumn(current);
	m_listbox->removeItem(current);
	if(m_listbox->count() == 0)
		m_buttons.at(6)->setEnabled(false);
}

void
EditListViewDialog::MoveItemUp()
{
	int current = m_listbox->currentItem();
	QString text = m_listbox->text(current);
	m_listbox->blockSignals(true);

	m_listbox->changeItem(m_listbox->text(current - 1), current);
	m_listview->setColumnText(current, m_listview->columnText(current - 1));
	m_listview->setColumnWidth(current, m_listview->columnWidth(current - 1));
	m_listview->header()->setClickEnabled(m_listview->header()->isClickEnabled(current - 1), current);
	m_listview->header()->setResizeEnabled(m_listview->header()->isResizeEnabled(current - 1), current);
	m_listview->header()->setStretchEnabled(m_listview->header()->isStretchEnabled(current - 1), current);

	m_listbox->changeItem(text, current - 1);
	m_listview->setColumnText(current - 1, (*m_buffer)["caption"]->value().toString());
	m_listview->setColumnWidth(current - 1,(*m_buffer)["width"]->value().toBool());
	m_listview->header()->setClickEnabled((*m_buffer)["clickable"]->value().toBool(), current - 1);
	m_listview->header()->setResizeEnabled((*m_buffer)["resizable"]->value().toBool(), current - 1);
	m_listview->header()->setStretchEnabled((*m_buffer)["fullwidth"]->value().toBool(), current - 1);

	m_listbox->blockSignals(false);
	m_listbox->setCurrentItem(current - 1);
}

void
EditListViewDialog::MoveItemDown()
{
	int current = m_listbox->currentItem();
	QString text = m_listbox->text(current);
	m_listbox->blockSignals(true);

	m_listbox->changeItem(m_listbox->text(current+1), current);
	m_listview->setColumnText(current, m_listview->columnText(current + 1));
	m_listview->setColumnWidth(current, m_listview->columnWidth(current + 1));
	m_listview->header()->setClickEnabled(m_listview->header()->isClickEnabled(current + 1), current);
	m_listview->header()->setResizeEnabled(m_listview->header()->isResizeEnabled(current + 1), current);
	m_listview->header()->setStretchEnabled(m_listview->header()->isStretchEnabled(current + 1), current);

	m_listbox->changeItem(text, current+1);
	m_listview->setColumnText(current + 1, (*m_buffer)["caption"]->value().toString());
	m_listview->setColumnWidth(current + 1,(*m_buffer)["width"]->value().toBool());
	m_listview->header()->setClickEnabled((*m_buffer)["clickable"]->value().toBool(), current + 1);
	m_listview->header()->setResizeEnabled((*m_buffer)["resizable"]->value().toBool(), current + 1);
	m_listview->header()->setStretchEnabled((*m_buffer)["fullwidth"]->value().toBool(), current + 1);

	m_listbox->blockSignals(false);
	m_listbox->setCurrentItem(current + 1);
}


/// Contents page slots ////////
void
EditListViewDialog::updateButtons(QListViewItem *item)
{
	if(!item)
	{
		for(int i = 1; i < 5; i++)
			m_buttons.at(i)->setEnabled(false);
		return;
	}

	m_buttons.at(1)->setEnabled(true);
	m_buttons.at(2)->setEnabled(true);
	m_buttons.at(3)->setEnabled( (item->itemAbove() && (item->itemAbove()->parent() == item->parent())) );
	m_buttons.at(4)->setEnabled(item->nextSibling());
}

void
EditListViewDialog::loadChildNodes(QListView *listview, QListViewItem *item, QListViewItem *parent)
{
	QListViewItem *newItem;
	if(listview->inherits("KListView"))
	{
		if(parent)
			newItem = new KListViewItem(parent);
		else
			newItem = new KListViewItem(listview);
	}
	else
	{
		if(parent)
			newItem = new QListViewItem(parent);
		else
			newItem = new QListViewItem(listview);
	}
	if(parent)
	{
		QListViewItem *last = parent->firstChild();
		while(last->nextSibling())
			last = last->nextSibling();
		newItem->moveItem(last);
	}
	else
		newItem->moveItem(listview->lastItem());

	for(int i = 0; i < listview->columns(); i++)
		newItem->setText(i, item->text(i));

	QListViewItem *child = item->firstChild();
	while(child)
	{
		loadChildNodes(listview, child, newItem);
		child = child->nextSibling();
	}
}

void
EditListViewDialog::newRow()
{
	KListViewItem *parent = (KListViewItem*)m_listview->selectedItem();
	if(parent)
		parent = (KListViewItem*)parent->parent();
	KListViewItem *item;
	if(parent)
		item = new KListViewItem(parent, m_listview->selectedItem());
	else
		item = new KListViewItem(m_listview, m_listview->selectedItem());
	item->setText(0, i18n("New Item"));
	m_listview->setCurrentItem(item);
}

void
EditListViewDialog::newChildRow()
{
	KListViewItem *parent = (KListViewItem*)m_listview->currentItem();
	KListViewItem *item;
	if(parent)
		item = new KListViewItem(parent);
	else
		item = new KListViewItem(m_listview, m_listview->currentItem());
	item->setText(0, i18n("Sub Item"));

	m_listview->setCurrentItem(item);
	parent->setOpen(true);
}

void
EditListViewDialog::removeRow()
{
	delete m_listview->currentItem();
}

void
EditListViewDialog::MoveRowUp()
{
	QListViewItem *item = m_listview->currentItem()->itemAbove();
	item->moveItem(m_listview->currentItem());
	updateButtons(m_listview->currentItem());
}

void
EditListViewDialog::MoveRowDown()
{
	QListViewItem *before = m_listview->currentItem();
	before->moveItem(before->nextSibling());
	updateButtons(before);
}


//////////////////////////////////////////////////////////////////////////////////
//////////  The Tab Stop Dialog to edit tab order  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

TabStopDialog::TabStopDialog(Form *form, QWidget *parent)
: KDialogBase(parent, "tabstop_dialog", true, i18n("Edit tab order"), Ok|Cancel, Ok, false)
{
	QFrame *frame = makeMainWidget();
	QHBoxLayout *l = new QHBoxLayout(frame, 0, 6);

	m_treeview = new ObjectTreeView(frame, "tabstops_treeview", true);
	m_treeview->m_form = form;
	m_treeview->setItemsMovable(true);
	m_treeview->setDragEnabled(true);
	m_treeview->setDropVisualizer(true);
	m_treeview->setAcceptDrops(true);
	m_treeview->setFocus();
	l->addWidget(m_treeview);

	ObjectTreeViewItem *topItem = new ObjectTreeViewItem(m_treeview);
	topItem->setOpen(true);
	for(ObjectTreeItem *it = form->tabStops()->last(); it; it = form->tabStops()->prev())
		ObjectTreeViewItem *item = new ObjectTreeViewItem(topItem, it);

	connect(m_treeview, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(updateButtons(QListViewItem*)));

	QVBoxLayout *vbox = new QVBoxLayout(l);
	QToolButton *up = new QToolButton(frame);
	up->setIconSet(BarIconSet("1uparrow"));
	up->setTextLabel(i18n("Move Widget &Up"), true);
	m_buttons.append(up);
	vbox->addWidget(up);
	connect(up, SIGNAL(clicked()), this, SLOT(MoveItemUp()));

	QToolButton *down = new QToolButton(frame);
	down->setIconSet(BarIconSet("1downarrow"));
	down->setTextLabel(i18n("Move Widget &Down"), true);
	vbox->addWidget(down);
	m_buttons.append(down);
	connect(down, SIGNAL(clicked()), this, SLOT(MoveItemDown()));
	vbox->addStretch();

	setInitialSize(QSize(400, 250), true);
	if( exec() == QDialog::Accepted)
	{
		form->tabStops()->clear();
		ObjectTreeViewItem *item = (ObjectTreeViewItem*)topItem->itemBelow();
		while(item)
		{
			ObjectTreeItem *tree = item->objectTree();
			if(tree)
				form->tabStops()->append(tree);
			item = (ObjectTreeViewItem*)item->itemBelow();
		}
	}
}

void
TabStopDialog::MoveItemUp()
{
	QListViewItem *before = m_treeview->selectedItem()->itemAbove();
	before->moveItem(m_treeview->selectedItem());
	updateButtons(m_treeview->selectedItem());
}

void
TabStopDialog::MoveItemDown()
{
	QListViewItem *item = m_treeview->selectedItem();
	item->moveItem( item->nextSibling());
	updateButtons(item);
}

void
TabStopDialog::updateButtons(QListViewItem *item)
{
	if(!item)
	{
		m_buttons.at(0)->setEnabled(false);
		m_buttons.at(1)->setEnabled(false);
		return;
	}

	m_buttons.at(0)->setEnabled( (item->itemAbove() && (item->itemAbove()->parent() == item->parent())) );
	m_buttons.at(1)->setEnabled(item->nextSibling());
}

}

#include "extrawidgets.moc"
