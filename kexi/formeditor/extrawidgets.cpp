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
#include <qcursor.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qmetaobject.h>
#include <qstrlist.h>
#include <qregexp.h>

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
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#include "kexitableview.h"
#include "kexitableviewdata.h"
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
: KDialogBase(parent, "richtext_dialog", true, i18n("Edit Rich Text"), Ok|Cancel, Ok, false)
{
	QFrame *frame = makeMainWidget();
	QVBoxLayout *l = new QVBoxLayout(frame);
	l->setAutoAdd(true);

	m_toolbar = new KToolBar(frame);
	m_toolbar->setFlat(true);
	m_toolbar->show();

	m_fcombo = new KFontCombo(m_toolbar);
	m_toolbar->insertWidget(TBFont, 40, m_fcombo);
	connect(m_fcombo, SIGNAL(textChanged(const QString&)), this, SLOT(changeFont(const QString &)));

	m_toolbar->insertSeparator();

	m_colCombo = new KColorCombo(m_toolbar);
	m_toolbar->insertWidget(TBColor, 30, m_colCombo);
	connect(m_colCombo, SIGNAL(activated(const QColor&)), this, SLOT(changeColor(const QColor&)));

	m_toolbar->insertButton("text_bold", TBBold, true, i18n("Bold"));
	m_toolbar->insertButton("text_italic", TBItalic, true, i18n("Italic"));
	m_toolbar->insertButton("text_under", TBUnder, true, i18n("Underline"));
	m_toolbar->setToggle(TBBold, true);
	m_toolbar->setToggle(TBItalic, true);
	m_toolbar->setToggle(TBUnder, true);
	m_toolbar->insertSeparator();

	m_toolbar->insertButton("text_super", TBSuper, true, i18n("Superscript"));
	m_toolbar->insertButton("text_sub", TBSub, true, i18n("Subscript"));
	m_toolbar->setToggle(TBSuper, true);
	m_toolbar->setToggle(TBSub, true);
	m_toolbar->insertSeparator();

	KToolBarRadioGroup *group = new KToolBarRadioGroup(m_toolbar);
	m_toolbar->insertButton("text_left", TBLeft, true, i18n("Right Align"));
	m_toolbar->setToggle(TBLeft, true);
	group->addButton(TBLeft);
	m_toolbar->insertButton("text_center", TBCenter, true, i18n("Left Align"));
	m_toolbar->setToggle(TBCenter, true);
	group->addButton(TBCenter);
	m_toolbar->insertButton("text_right", TBRight, true, i18n("Centered"));
	m_toolbar->setToggle(TBRight, true);
	group->addButton(TBRight);
	m_toolbar->insertButton("text_block", TBJustify, true, i18n("Justified"));
	m_toolbar->setToggle(TBJustify, true);
	group->addButton(TBJustify);

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
		case TBBold: m_edit->setBold(isOn); break;
		case TBItalic: m_edit->setItalic(isOn); break;
		case TBUnder: m_edit->setUnderline(isOn); break;
		case TBSuper:
		{
			if(isOn && m_toolbar->isButtonOn(TBSub))
				m_toolbar->setButton(TBSub, false);
			m_edit->setVerticalAlignment(isOn ? QTextEdit::AlignSuperScript : QTextEdit::AlignNormal);
			break;
		}
		case TBSub:
		{
			if(isOn && m_toolbar->isButtonOn(TBSuper))
				m_toolbar->setButton(TBSuper, false);
			m_edit->setVerticalAlignment(isOn ? QTextEdit::AlignSubScript : QTextEdit::AlignNormal);
			break;
		}
		case TBLeft: case TBCenter:
		case TBRight: case TBJustify:
		{
			if(!isOn)  break;
			switch(id)
			{
				case TBLeft:  m_edit->setAlignment(Qt::AlignLeft); break;
				case TBCenter:  m_edit->setAlignment(Qt::AlignCenter); break;
				case TBRight:  m_edit->setAlignment(Qt::AlignRight); break;
				case TBJustify:  m_edit->setAlignment(Qt::AlignJustify); break;
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
	m_toolbar->setButton(TBBold, m_edit->bold());
	m_toolbar->setButton(TBItalic, m_edit->italic());
	m_toolbar->setButton(TBUnder, m_edit->underline());

	int id = 0;
	switch(m_edit->alignment())
	{
		case Qt::AlignLeft:    id = TBLeft; break;
		case Qt::AlignCenter:  id = TBCenter; break;
		case Qt::AlignRight:   id = TBRight; break;
		case Qt::AlignJustify: id = TBJustify; break;
		default:  id = TBLeft; break;
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
			m_toolbar->setButton(TBSuper, true);
			m_toolbar->setButton(TBSub, false);
			break;
		}
		case QTextEdit::AlignSubScript:
		{
			m_toolbar->setButton(TBSub, true);
			m_toolbar->setButton(TBSuper, false);
			break;
		}
		default:
		{
			m_toolbar->setButton(TBSuper, false);
			m_toolbar->setButton(TBSub, false);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
// A Dialog to edit the contents of a listview ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

EditListViewDialog::EditListViewDialog(QWidget *parent)
//js(kde3.2 dependent) : KDialogBase(Tabbed, 0/* WFlags */, parent, "editlistview_dialog", true, i18n("Edit listview contents"), Ok|Cancel, Ok, false)
: KDialogBase(Tabbed, i18n("Edit listview contents"), Ok|Cancel, Ok, parent, "editlistview_dialog", true /* modal */, false)
{
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
	m_buttons.insert(BNewRow, newRow);
	connect(newRow, SIGNAL(clicked()), this, SLOT(newRow()));

	QToolButton *newChild = new QToolButton(m_contents);
	newChild->setIconSet(BarIconSet("1rightarrow"));
	newChild->setTextLabel(i18n("New &Subitem"), true);
	vlayout->addWidget(newChild);
	m_buttons.insert(BNewChild, newChild);
	connect(newChild, SIGNAL(clicked()), this, SLOT(newChildRow()));

	QToolButton *delRow = new QToolButton(m_contents);
	delRow->setIconSet(BarIconSet("edit_remove"));
	delRow->setTextLabel(i18n("&Remove Item"), true);
	vlayout->addWidget(delRow);
	m_buttons.insert(BRemRow, delRow);
	connect(delRow, SIGNAL(clicked()), this, SLOT(removeRow()));

	QToolButton *rowUp = new QToolButton(m_contents);
	rowUp->setIconSet(BarIconSet("1uparrow"));
	rowUp->setTextLabel(i18n("Move Item &Up"), true);
	vlayout->addWidget(rowUp);
	m_buttons.insert(BRowUp, rowUp);
	connect(rowUp, SIGNAL(clicked()), this, SLOT(MoveRowUp()));

	QToolButton *rowDown = new QToolButton(m_contents);
	rowDown->setIconSet(BarIconSet("1downarrow"));
	rowDown->setTextLabel(i18n("Move Item &Down"), true);
	vlayout->addWidget(rowDown);
	m_buttons.insert(BRowDown, rowDown);
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
	connect(m_listview, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(updateButtons(QListViewItem*)));
	connect(m_listview, SIGNAL(moved(QListViewItem*, QListViewItem*, QListViewItem*)), this, SLOT(updateButtons(QListViewItem*)));

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
	m_buttons.insert(BColAdd, add);
	connect(add, SIGNAL(clicked()), this, SLOT(newItem()));

	QToolButton *remove = new QToolButton(m_column);
	remove->setIconSet(BarIconSet("edit_remove"));
	remove->setTextLabel(i18n("&Remove Item"), true);
	vbox->addWidget(remove);
	m_buttons.insert(BColRem, remove);
	connect(remove, SIGNAL(clicked()), this, SLOT(removeItem()));

	QToolButton *up = new QToolButton(m_column);
	up->setIconSet(BarIconSet("1uparrow"));
	up->setTextLabel(i18n("Move Item &Up"), true);
	vbox->addWidget(up);
	m_buttons.insert(BColUp, up);
	connect(up, SIGNAL(clicked()), this, SLOT(MoveItemUp()));

	QToolButton *down = new QToolButton(m_column);
	down->setIconSet(BarIconSet("1downarrow"));
	down->setTextLabel(i18n("Move Item &Down"), true);
	vbox->addWidget(down);
	m_buttons.insert(BColDown, down);
	connect(down, SIGNAL(clicked()), this, SLOT(MoveItemDown()));
	vbox->addStretch();

	// The listbox with columns name /////
	m_listbox = new KListBox(m_column, "editlistview_columns");
	m_listbox->setFocus();
	hbox->insertWidget(0, m_listbox);
	hbox->addWidget(m_editor);
	connect(m_listbox, SIGNAL(currentChanged(QListBoxItem*)), this, SLOT(updateItemProperties(QListBoxItem*)));

	//// Init dialog and display it ////////////////////////
	setInitialSize(QSize(500, 300), true);

}

int
EditListViewDialog::exec(QListView *listview)
{
	if(!listview)
	{
		kdDebug() << "EditListViewDialog ERROR: no listview " << endl;
		return 0;
	}

	// We copy the contents of the listview into our listview
	for(int i = 0; i < listview->columns(); i++)
	{
		m_listview->addColumn(listview->columnText(i), listview->columnWidth(i));
		m_listview->header()->setClickEnabled(listview->header()->isClickEnabled(i), i);
		m_listview->header()->setResizeEnabled(listview->header()->isResizeEnabled(i), i);
		m_listview->header()->setStretchEnabled(listview->header()->isStretchEnabled(i), i);
		m_listview->setRenameable(i, true);
	}
	QListViewItem *item = listview->firstChild();
	while(item)
	{
		loadChildNodes(m_listview, item, 0);
		item = item->nextSibling();
	}

	m_listview->setSelected(m_listview->firstChild(), true);
	if(!m_listview->firstChild())
		updateButtons(0);

	for(int i = 0; i < listview->columns(); i++)
		m_listbox->insertItem(listview->columnText(i));
	m_listbox->setSelected(0, true);

	// and we exec the dialog
	int r =  KDialogBase::exec();
	if(r == QDialog::Accepted)
	{
		listview->clear();
		// We copy the contents of our listview back in the listview
		for(int i = 0; i < m_listview->columns(); i++)
		{
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
	return r;
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
		m_buffer->blockSignals(true); // we need to block signals because changeItem will modify selection, and call updateItemProperties
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
		m_buffer->blockSignals(true); // we don't want changeProperty to be called
		(*m_buffer)["caption"]->setValue(m_listview->columnText(id), false);
		(*m_buffer)["width"]->setValue(m_listview->columnWidth(id), false);
		(*m_buffer)["clickable"]->setValue(QVariant(m_listview->header()->isClickEnabled(id), 4), false);
		(*m_buffer)["resizable"]->setValue(QVariant(m_listview->header()->isResizeEnabled(id), 4), false);
		(*m_buffer)["fullwidth"]->setValue(QVariant(m_listview->header()->isStretchEnabled(id), 4), false);
		m_buffer->blockSignals(false);
		m_editor->setBuffer(m_buffer);
	}

	m_buttons[BColUp]->setEnabled(item->prev());
	m_buttons[BColDown]->setEnabled(item->next());
}

void
EditListViewDialog::newItem()
{
	m_listbox->insertItem(i18n("New Column"));
	m_listview->addColumn(i18n("New Column"));
	m_listview->setRenameable(m_listview->columns() - 1, true);
	m_listbox->setCurrentItem(m_listbox->count() - 1);
	m_buttons[BColRem]->setEnabled(true);
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
		m_buttons[BColRem]->setEnabled(false);
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
		for(int i = BNewChild; i <= BRowDown; i++)
			m_buttons[i]->setEnabled(false);
		return;
	}

	m_buttons[BNewChild]->setEnabled(true);
	m_buttons[BRemRow]->setEnabled(true);
	m_buttons[BRowUp]->setEnabled( (item->itemAbove() && (item->itemAbove()->parent() == item->parent())) );
	m_buttons[BRowDown]->setEnabled(item->nextSibling());
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

	// We need to move the item at the end, which is the expected behaviour (by default it is inserted at the beginning)
	QListViewItem *last;
	if(parent)
		last = parent->firstChild();
	else
		last = listview->firstChild();

	while(last->nextSibling())
		last = last->nextSibling();
	newItem->moveItem(last);

	// We copy the text of all the columns
	for(int i = 0; i < listview->columns(); i++)
		newItem->setText(i, item->text(i));

	QListViewItem *child = item->firstChild();
	if(child)
		newItem->setOpen(true);
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

TabStopDialog::TabStopDialog(QWidget *parent)
: KDialogBase(parent, "tabstop_dialog", true, i18n("Edit Tab Order"), Ok|Cancel, Ok, false)
{
	QFrame *frame = makeMainWidget();
	QGridLayout *l = new QGridLayout(frame, 2, 2, 0, 6);
	m_treeview = new ObjectTreeView(frame, "tabstops_treeview", true);
	m_treeview->setItemsMovable(true);
	m_treeview->setDragEnabled(true);
	m_treeview->setDropVisualizer(true);
	m_treeview->setAcceptDrops(true);
	m_treeview->setFocus();
	l->addWidget(m_treeview, 0, 0);

	m_treeview->m_form = 0;
	connect(m_treeview, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(updateButtons(QListViewItem*)));
	connect(m_treeview, SIGNAL(moved(QListViewItem*, QListViewItem*, QListViewItem*)), this, SLOT(updateButtons(QListViewItem*)));

	QVBoxLayout *vbox = new QVBoxLayout();
	l->addLayout(vbox, 0, 1);
	QToolButton *up = new QToolButton(frame);
	up->setIconSet(BarIconSet("1uparrow"));
	up->setTextLabel(i18n("Move Widget &Up"), true);
	m_buttons.insert(BUp, up);
	vbox->addWidget(up);
	connect(up, SIGNAL(clicked()), this, SLOT(MoveItemUp()));

	QToolButton *down = new QToolButton(frame);
	down->setIconSet(BarIconSet("1downarrow"));
	down->setTextLabel(i18n("Move Widget &Down"), true);
	vbox->addWidget(down);
	m_buttons.insert(BDown, down);
	connect(down, SIGNAL(clicked()), this, SLOT(MoveItemDown()));
	vbox->addStretch();

	m_check = new QCheckBox(i18n("Handle tab stops automatically"), frame, "tabstops_check");
	connect(m_check, SIGNAL(toggled(bool)), this, SLOT(slotRadioClicked(bool)));
	l->addMultiCellWidget(m_check, 1, 1, 0, 1);

	setInitialSize(QSize(400, 250), true);
}

int TabStopDialog::exec(Form *form)
{
	m_treeview->clear();
	m_treeview->m_form = form;

	//ObjectTreeViewItem *topItem = new ObjectTreeViewItem(m_treeview);
	//topItem->setOpen(true);
	if(form->autoTabStops())
		form->autoAssignTabStops();
	for(ObjectTreeItem *it = form->tabStops()->last(); it; it = form->tabStops()->prev())
		new ObjectTreeViewItem(m_treeview, it);

	m_check->setChecked(form->autoTabStops());

	int r = KDialogBase::exec();
	if( r == QDialog::Accepted)
	{
		form->setAutoTabStops(m_check->isChecked());
		if(form->autoTabStops())
		{
			form->autoAssignTabStops();
			return r;
		}

		form->tabStops()->clear();
		//ObjectTreeViewItem *item = (ObjectTreeViewItem*)topItem->itemBelow();
		ObjectTreeViewItem *item = (ObjectTreeViewItem*)m_treeview->firstChild();
		while(item)
		{
			ObjectTreeItem *tree = item->objectTree();
			if(tree)
				form->tabStops()->append(tree);
			item = (ObjectTreeViewItem*)item->nextSibling();
		}
	}
	return r;
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
		m_buttons[BUp]->setEnabled(false);
		m_buttons[BDown]->setEnabled(false);
		return;
	}

	m_buttons[BUp]->setEnabled( (item->itemAbove() /*&& (item->itemAbove()->parent() == item->parent()))*/ ));
	m_buttons[BDown]->setEnabled(item->nextSibling());
}

void
TabStopDialog::slotRadioClicked(bool isOn)
{
	m_treeview->setEnabled(!isOn);
	m_buttons[BUp]->setEnabled(!isOn);
	m_buttons[BDown]->setEnabled(!isOn);
}


/////////////////////////////////////////////////////////////////////////////////
///////////// The dialog to edit or add/remove connections //////////////////////
/////////////////////////////////////////////////////////////////////////////////
ConnectionDialog::ConnectionDialog(QWidget *parent)
: KDialogBase(parent, "connections_dialog", true, i18n("Edit Form connections"), Ok|Cancel|Details, Ok, false)
{
	QFrame *frame = makeMainWidget();
	QHBoxLayout *layout = new QHBoxLayout(frame, 0, 6);

	// And the KexiTableView ////////
	m_data = new KexiTableViewData();
	m_table = new KexiTableView(m_data, frame, "connections_tableview");
	m_table->setSpreadSheetMode();
	initTable();
	m_table->setData(m_data, false);
	layout->addWidget(m_table);

	//// Setup the icon toolbar /////////////////
	QVBoxLayout *vlayout = new QVBoxLayout(layout, 3);
	QToolButton *newItem = new QToolButton(frame);
	newItem->setIconSet(BarIconSet("edit_add"));
	newItem->setTextLabel(i18n("&Add Connection"), true);
	vlayout->addWidget(newItem);
	m_buttons.insert(BAdd, newItem);
	connect(newItem, SIGNAL(clicked()), this, SLOT(newItem()));

	QToolButton *newItemDrag = new QToolButton(frame);
	newItemDrag->setIconSet(BarIconSet("edit_add"));
	newItemDrag->setTextLabel(i18n("Create a Connection by &Drag and drop"), true);
	vlayout->addWidget(newItemDrag);
	m_buttons.insert(BAddDrag, newItemDrag);
	connect(newItemDrag, SIGNAL(clicked()), this, SLOT(newItemByDragnDrop()));

	QToolButton *delItem = new QToolButton(frame);
	delItem->setIconSet(BarIconSet("edit_remove"));
	delItem->setTextLabel(i18n("&Remove Connection"), true);
	vlayout->addWidget(delItem);
	m_buttons.insert(BRemove, delItem);
	connect(delItem, SIGNAL(clicked()), this, SLOT(removeItem()));
	vlayout->addStretch();

	// Setup the details widget /////////
	QHBox *details = new QHBox(frame);
	setDetailsWidget(details);
	setDetails(true);

	m_pixmapLabel = new QLabel(details);
	m_pixmapLabel->setFixedWidth( int(IconSize(KIcon::Desktop) * 1.5) );
	m_pixmapLabel->setAlignment(AlignHCenter | AlignTop);

	m_textLabel = new QLabel(details);
	m_textLabel->setAlignment(AlignLeft | AlignTop);
	setStatusOk();

	setInitialSize(QSize(600, 300));
}

void
ConnectionDialog::initTable()
{
	QValueList<QVariant> empty_list;
	KexiTableViewColumn *col1 = new KexiTableViewColumn(i18n("Sender"), KexiDB::Field::Enum);
	m_widgetsColumnData = new KexiTableViewData( empty_list, empty_list,
		KexiDB::Field::Text, KexiDB::Field::Text);
	col1->setRelatedData( m_widgetsColumnData );
	m_data->addColumn(col1);

	KexiTableViewColumn *col2 = new KexiTableViewColumn(i18n("Signal"), KexiDB::Field::Enum);
	m_signalsColumnData = new KexiTableViewData( empty_list, empty_list,
		KexiDB::Field::Text, KexiDB::Field::Text);
	col2->setRelatedData( m_signalsColumnData );
	m_data->addColumn(col2);

	KexiTableViewColumn *col3 = new KexiTableViewColumn(i18n("Receiver"), KexiDB::Field::Enum);
	col3->setRelatedData( m_widgetsColumnData );
	m_data->addColumn(col3);

	KexiTableViewColumn *col4 = new KexiTableViewColumn(i18n("Slot"), KexiDB::Field::Enum);
	m_slotsColumnData = new KexiTableViewData( empty_list, empty_list,
		KexiDB::Field::Text, KexiDB::Field::Text);
	col4->setRelatedData( m_slotsColumnData );
	m_data->addColumn(col4);

	QValueList<int> c;
	c << 1 << 3;
	m_table->maximizeColumnsWidth(c);

	connect(m_data, SIGNAL(aboutToChangeCell(KexiTableItem*, int, QVariant, KexiDB::ResultInfo*)),
	      this,SLOT(slotCellChanged(KexiTableItem*, int, QVariant, KexiDB::ResultInfo*)));
	connect(m_data, SIGNAL(rowUpdated(KexiTableItem*)), this, SLOT(checkConnection(KexiTableItem *)));
	connect(m_table, SIGNAL(itemSelected(KexiTableItem *)), this, SLOT(checkConnection(KexiTableItem *)));
}

int
ConnectionDialog::exec(Form *form)
{
	m_form = form;
	updateTableData();

	int r = KDialogBase::exec();
	if( r == QDialog::Accepted)
	{
	}
	return r;
}

void
ConnectionDialog::updateTableData()
{
	// First we update the columns data
	TreeDict dict = *(m_form->objectTree()->dict());
	TreeDictIterator it(dict);
	for(; it.current(); ++it)
	{
		KexiTableItem *item = new KexiTableItem(2);
		(*item)[0] = it.current()->name();
		(*item)[1] = (*item)[0];
		m_widgetsColumnData->append(item);
	}

	/*for (int i=0; i<4; i++) {
		KexiTableItem *item = new KexiTableItem(4);
		m_data->append(item);
	}*/
}

void
ConnectionDialog::setStatusOk()
{
	m_pixmapLabel->setPixmap( DesktopIcon("button_ok") );
	m_textLabel->setText("<qt><h2>The connection is OK.</h2></qt>");
}

void
ConnectionDialog::setStatusError(const QString &msg)
{
	m_pixmapLabel->setPixmap( DesktopIcon("button_cancel") );
	m_textLabel->setText("<qt><h2>The connection is wrong.</h2></qt>" + msg);
}

void
ConnectionDialog::slotCellChanged(KexiTableItem *item, int col, QVariant value, KexiDB::ResultInfo*)
{
	switch(col)
	{
		// sender changed, we update the signals list
		case 0:
		{
			ObjectTreeItem *tree = m_form->objectTree()->lookup(value.toString());
			if(!tree || !tree->widget())
				return;

			m_signalsColumnData->clear();
			QStrList signalList = tree->widget()->metaObject()->signalNames(true);
			QStrListIterator it(signalList);
			for(; it.current() != 0; ++it)
			{
				KexiTableItem *item = new KexiTableItem(2);
				(*item)[0] = QString(*it);
				(*item)[1] = (*item)[0];
				m_signalsColumnData->append(item);
			}
			// and we reset the signal value
			(*item)[1] = QString("");
			break;
		}
		// the signal was changed, update slot list
		case 1:
		{
			updateSlotList(item, value.toString(), (*item)[2].toString());
			break;
		}
		// receiver changed, we update the slots list
		case 2:
		{
			updateSlotList(item, (*item)[1].toString(), value.toString());
			break;
		}
		default:
			break;
	}
}

void
ConnectionDialog::updateSlotList(KexiTableItem *item, const QString &signal, const QString &widget)
{
	m_slotsColumnData->clear();

	if(widget.isEmpty())// || signal.isEmpty())
		return;
	ObjectTreeItem *tree = m_form->objectTree()->lookup(widget);
	if(!tree || !tree->widget())
		return;

	QString signalArg(signal);
	signalArg = signalArg.remove( QRegExp(".*[(]|[)]") );

	QStrList slotList = tree->widget()->metaObject()->slotNames(true);
	QStrListIterator it(slotList);
	for(; it.current() != 0; ++it)
	{
		// we add the slot only if it is compatible with the signal
		QString slotArg(*it);
		slotArg = slotArg.remove( QRegExp(".*[(]|[)]") );
		kdDebug() << "Comparing arguments for signal : " << signalArg << " and slot " << slotArg << endl;
		if(!signalArg.startsWith(slotArg, true) && (!signal.isEmpty())) // args not compatible
			continue;

		KexiTableItem *item = new KexiTableItem(2);
		(*item)[0] = QString(*it);
		(*item)[1] = (*item)[0];
		m_slotsColumnData->append(item);
	}
	// and we reset the slot value
	(*item)[3] = QString("");
}

void
ConnectionDialog::checkConnection(KexiTableItem *item)
{
	// First we check if one column is empty
	for(int i = 0; i < 4; i++)
	{
		if( (*item)[i].toString().isEmpty())
		{
			setStatusError( i18n("<qt>You have not selected a <b>%1</b>.</qt>").arg(m_data->column(i)->nameOrCaption()) );
			return;
		}
	}

	// Then we check if signal/slot args are compatible
	QString signal = (*item)[1].toString();
	signal = signal.remove( QRegExp(".*[(]|[)]") ); // just keep the args list
	QString slot = (*item)[3].toString();
	slot = slot.remove( QRegExp(".*[(]|[)]") );
	kdDebug() << "Comparing arguments for signal : " << signal << " and slot " << slot << endl;
	if(!signal.startsWith(slot, true))
	{
		setStatusError( i18n("The signal/slot arguments are not compatible."));
		return;
	}

	setStatusOk();
}

void
ConnectionDialog::newItem()
{
	m_table->insertEmptyRow(m_table->rows() + 1);
}

void
ConnectionDialog::newItemByDragnDrop()
{
	// TODO
}

void
ConnectionDialog::removeItem()
{
	int confirm = KMessageBox::questionYesNo(parentWidget(), i18n("<qt>Do you really want to remove this connection ?</qt>"));
	if(confirm == KMessageBox::No)
		return;

	m_table->deleteItem(m_table->selectedItem());
}

}

#include "extrawidgets.moc"
