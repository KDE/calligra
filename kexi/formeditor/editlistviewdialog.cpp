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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include <q3header.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

#include <k3listview.h>
#include <ktabwidget.h>
#include <k3listbox.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>

#include <koproperty/EditorView.h>
#include <koproperty/Set.h>
#include <koproperty/Property.h>

#include "editlistviewdialog.h"

namespace KFormDesigner
{

//////////////////////////////////////////////////////////////////////////////////
/// A Dialog to edit the contents of a listview /////////////////////
/////////////////////////////////////////////////////////////////////////////////

EditListViewDialog::EditListViewDialog(QWidget *parent)
        : KPageDialog(parent)
{
    setObjectName("editlistview_dialog");
    setModal(true);
    setFaceType(Tabbed);
    setCaption(i18n("Edit Listview Contents"));

    ///////// Setup the "Contents" page /////////////////////////////
    m_columnsPageItem = addPage(new QWidget(this), i18n("Columns"));
    Q3HBoxLayout *layout = new Q3HBoxLayout(m_contentsPageItem->widget(), 0, 6);

    //// Setup the icon toolbar /////////////////
    Q3VBoxLayout *vlayout = new Q3VBoxLayout(layout, 3);
    QToolButton *newRow = new QToolButton(m_contentsPageItem->widget());
    newRow->setIconSet(KIcon("edit_add"));
    newRow->setTextLabel(i18n("&Add Item"), true);
    vlayout->addWidget(newRow);
    m_buttons.insert(BNewRow, newRow);
    connect(newRow, SIGNAL(clicked()), this, SLOT(newRow()));

    QToolButton *newChild = new QToolButton(m_contentsPageItem->widget());
    newChild->setIconSet(KIcon("arrow-right"));
    newChild->setTextLabel(i18n("New &Subitem"), true);
    vlayout->addWidget(newChild);
    m_buttons.insert(BNewChild, newChild);
    connect(newChild, SIGNAL(clicked()), this, SLOT(newChildRow()));

    QToolButton *delRow = new QToolButton(m_contentsPageItem->widget());
    delRow->setIconSet(KIcon("edit_remove"));
    delRow->setTextLabel(i18n("&Remove Item"), true);
    vlayout->addWidget(delRow);
    m_buttons.insert(BRemRow, delRow);
    connect(delRow, SIGNAL(clicked()), this, SLOT(removeRow()));

    QToolButton *rowUp = new QToolButton(m_contentsPageItem->widget());
    rowUp->setIconSet(KIcon("arrow-up"));
    rowUp->setTextLabel(i18n("Move Item &Up"), true);
    vlayout->addWidget(rowUp);
    m_buttons.insert(BRowUp, rowUp);
    connect(rowUp, SIGNAL(clicked()), this, SLOT(MoveRowUp()));

    QToolButton *rowDown = new QToolButton(m_contentsPageItem->widget());
    rowDown->setIconSet(KIcon("arrow-down"));
    rowDown->setTextLabel(i18n("Move Item &Down"), true);
    vlayout->addWidget(rowDown);
    m_buttons.insert(BRowDown, rowDown);
    connect(rowDown, SIGNAL(clicked()), this, SLOT(MoveRowDown()));
    vlayout->addStretch();

    //// The listview ///////////
    m_listview = new K3ListView(m_contentsPageItem->widget());
    m_listview->setObjectName("editlistview_listview");
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
    connect(m_listview, SIGNAL(currentChanged(Q3ListViewItem*)), this, SLOT(updateButtons(Q3ListViewItem*)));
    connect(m_listview, SIGNAL(moved(Q3ListViewItem*, Q3ListViewItem*, Q3ListViewItem*)), this, SLOT(updateButtons(Q3ListViewItem*)));

    /////////////////// Setup the columns page ////////////////
    m_contentsPageItem = addPage(new QWidget(this), i18n("Contents"));
    Q3HBoxLayout *hbox = new Q3HBoxLayout(m_columnsPageItem->widget(), 0, 6);

    // The "item properties" field
    m_editor = new KoProperty::EditorView(m_columnsPageItem->widget());
    m_editor->setObjectName("editcolumn_propeditor");
    m_propSet = new KoProperty::Set(this, "columns");
    m_propSet->addProperty(new KoProperty::Property("caption", "Caption", i18n("Caption"), i18n("Caption")));
    m_propSet->addProperty(new KoProperty::Property("width", 100, i18n("Width"), i18n("Width")));
    m_propSet->addProperty(new KoProperty::Property("clickable", QVariant(true, 3), i18n("Clickable"), i18n("Clickable")));
    m_propSet->addProperty(new KoProperty::Property("resizable", QVariant(true, 3), i18n("Resizable"), i18n("Resizable")));
    m_propSet->addProperty(new KoProperty::Property("fullwidth", QVariant(false, 3), i18n("Full Width"), i18n("Full Width")));
    m_editor->changeSet(m_propSet);
    connect(m_propSet, SIGNAL(propertyChanged(KoProperty::Set & KoProperty::Property&)),
            this, SLOT(changeProperty(KoProperty::Set & KoProperty::Property&)));

    // Setup the icon toolbar //////////
    Q3VBoxLayout *vbox = new Q3VBoxLayout(hbox, 3);
    QToolButton *add = new QToolButton(m_columnsPageItem->widget());
    add->setIconSet(KIcon("edit_add"));
    add->setTextLabel(i18n("&Add Item"), true);
    vbox->addWidget(add);
    m_buttons.insert(BColAdd, add);
    connect(add, SIGNAL(clicked()), this, SLOT(newItem()));

    QToolButton *remove = new QToolButton(m_columnsPageItem->widget());
    remove->setIconSet(KIcon("edit_remove"));
    remove->setTextLabel(i18n("&Remove Item"), true);
    vbox->addWidget(remove);
    m_buttons.insert(BColRem, remove);
    connect(remove, SIGNAL(clicked()), this, SLOT(removeItem()));

    QToolButton *up = new QToolButton(m_columnsPageItem->widget());
    up->setIconSet(KIcon("arrow-up"));
    up->setTextLabel(i18n("Move Item &Up"), true);
    vbox->addWidget(up);
    m_buttons.insert(BColUp, up);
    connect(up, SIGNAL(clicked()), this, SLOT(MoveItemUp()));

    QToolButton *down = new QToolButton(m_columnsPageItem->widget());
    down->setIconSet(KIcon("arrow-down"));
    down->setTextLabel(i18n("Move Item &Down"), true);
    vbox->addWidget(down);
    m_buttons.insert(BColDown, down);
    connect(down, SIGNAL(clicked()), this, SLOT(MoveItemDown()));
    vbox->addStretch();

    // The listbox with columns name /////
    m_listbox = new K3ListBox(m_columnsPageItem->widget(), "editlistview_columns");
    m_listbox->setFocus();
    hbox->insertWidget(0, m_listbox);
    hbox->addWidget(m_editor);
    connect(m_listbox, SIGNAL(currentChanged(Q3ListBoxItem*)), this, SLOT(updateItemProperties(Q3ListBoxItem*)));

    //// Init dialog and display it ////////////////////////
#ifdef __GNUC__
#warning "setInitialSize() unavailable on kde 4"
#endif
// setInitialSize(QSize(500, 300), true);
}

int
EditListViewDialog::exec(Q3ListView *listview)
{
    if (!listview) {
        kDebug() << "EditListViewDialog ERROR: no listview ";
        return 0;
    }

    // We copy the contents of the listview into our listview
    for (int i = 0; i < listview->columns(); i++) {
        m_listview->addColumn(listview->columnText(i), listview->columnWidth(i));
        m_listview->header()->setClickEnabled(listview->header()->isClickEnabled(i), i);
        m_listview->header()->setResizeEnabled(listview->header()->isResizeEnabled(i), i);
        m_listview->header()->setStretchEnabled(listview->header()->isStretchEnabled(i), i);
        m_listview->setRenameable(i, true);
    }
    Q3ListViewItem *item = listview->firstChild();
    while (item)  {
        loadChildNodes(m_listview, item, 0);
        item = item->nextSibling();
    }

    m_listview->setSelected(m_listview->firstChild(), true);
    if (!m_listview->firstChild())
        updateButtons(0);

    for (int i = 0; i < listview->columns(); i++)
        m_listbox->insertItem(listview->columnText(i));
    m_listbox->setSelected(0, true);

    // and we exec the dialog
    int r =  KPageDialog::exec();
    if (r == QDialog::Accepted) {
        listview->clear();
        // We copy the contents of our listview back in the listview
        for (int i = 0; i < m_listview->columns(); i++) {
            if (listview->columns() <= i)
                listview->addColumn(m_listview->columnText(i), m_listview->columnWidth(i));
            else {
                listview->setColumnText(i, m_listview->columnText(i));
                listview->setColumnWidth(i, m_listview->columnWidth(i));
            }
            listview->header()->setClickEnabled(m_listview->header()->isClickEnabled(i), i);
            listview->header()->setResizeEnabled(m_listview->header()->isResizeEnabled(i), i);
            listview->header()->setStretchEnabled(m_listview->header()->isStretchEnabled(i), i);
        }

        Q3ListViewItem *item = m_listview->firstChild();
        while (item) {
            loadChildNodes(listview, item, 0);
            item = item->nextSibling();
        }
    }
    return r;
}

/// Columns page slots ///////
void
EditListViewDialog::changeProperty(KoProperty::Set& set, KoProperty::Property& property)
{
    if (&set != m_propSet)
        return;

    QString name = property.name();
    QVariant value = property.value();
    if (name == "caption") {
        m_propSet->blockSignals(true); // we need to block signals because changeItem will modify selection, and call updateItemProperties
        m_listbox->changeItem(value.toString(), m_listbox->currentItem());
        m_listview->setColumnText(m_listbox->currentItem(), value.toString());
        m_propSet->blockSignals(false);
    } else if (name == "width")
        m_listview->setColumnWidth(m_listbox->currentItem(), value.toInt());
    else if (name == "resizable")
        m_listview->header()->setResizeEnabled(value.toBool(), m_listbox->currentItem());
    else if (name == "clickable")
        m_listview->header()->setClickEnabled(value.toBool(), m_listbox->currentItem());
    else if (name == "fullwidth")
        m_listview->header()->setStretchEnabled(value.toBool(), m_listbox->currentItem());
}

void
EditListViewDialog::updateItemProperties(Q3ListBoxItem *item)
{
    if (!item)
        return;

    int id = m_listbox->index(item);
    if (m_propSet) {
        m_propSet->blockSignals(true); // we don't want changeProperty to be called
        (*m_propSet)["caption"].setValue(m_listview->columnText(id), false);
        (*m_propSet)["width"].setValue(m_listview->columnWidth(id), false);
        (*m_propSet)["clickable"].setValue(QVariant(m_listview->header()->isClickEnabled(id), 4), false);
        (*m_propSet)["resizable"].setValue(QVariant(m_listview->header()->isResizeEnabled(id), 4), false);
        (*m_propSet)["fullwidth"].setValue(QVariant(m_listview->header()->isStretchEnabled(id), 4), false);
        m_propSet->blockSignals(false);
        m_editor->changeSet(m_propSet);
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
    if (m_listbox->item(current + 1))
        m_listbox->setCurrentItem(current + 1);
    else
        m_listbox->setCurrentItem(current - 1);

    m_listview->removeColumn(current);
    m_listbox->removeItem(current);
    if (m_listbox->count() == 0)
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
    m_listview->setColumnText(current - 1, (*m_propSet)["caption"].value().toString());
    m_listview->setColumnWidth(current - 1, (*m_propSet)["width"].value().toBool());
    m_listview->header()->setClickEnabled((*m_propSet)["clickable"].value().toBool(), current - 1);
    m_listview->header()->setResizeEnabled((*m_propSet)["resizable"].value().toBool(), current - 1);
    m_listview->header()->setStretchEnabled((*m_propSet)["fullwidth"].value().toBool(), current - 1);

    m_listbox->blockSignals(false);
    m_listbox->setCurrentItem(current - 1);
}

void
EditListViewDialog::MoveItemDown()
{
    int current = m_listbox->currentItem();
    QString text = m_listbox->text(current);
    m_listbox->blockSignals(true);

    m_listbox->changeItem(m_listbox->text(current + 1), current);
    m_listview->setColumnText(current, m_listview->columnText(current + 1));
    m_listview->setColumnWidth(current, m_listview->columnWidth(current + 1));
    m_listview->header()->setClickEnabled(m_listview->header()->isClickEnabled(current + 1), current);
    m_listview->header()->setResizeEnabled(m_listview->header()->isResizeEnabled(current + 1), current);
    m_listview->header()->setStretchEnabled(m_listview->header()->isStretchEnabled(current + 1), current);

    m_listbox->changeItem(text, current + 1);
    m_listview->setColumnText(current + 1, (*m_propSet)["caption"].value().toString());
    m_listview->setColumnWidth(current + 1, (*m_propSet)["width"].value().toBool());
    m_listview->header()->setClickEnabled((*m_propSet)["clickable"].value().toBool(), current + 1);
    m_listview->header()->setResizeEnabled((*m_propSet)["resizable"].value().toBool(), current + 1);
    m_listview->header()->setStretchEnabled((*m_propSet)["fullwidth"].value().toBool(), current + 1);

    m_listbox->blockSignals(false);
    m_listbox->setCurrentItem(current + 1);
}


/// Contents page slots ////////
void
EditListViewDialog::updateButtons(Q3ListViewItem *item)
{
    if (!item) {
        for (int i = BNewChild; i <= BRowDown; i++)
            m_buttons[i]->setEnabled(false);
        return;
    }

    m_buttons[BNewChild]->setEnabled(true);
    m_buttons[BRemRow]->setEnabled(true);
    m_buttons[BRowUp]->setEnabled((item->itemAbove() && (item->itemAbove()->parent() == item->parent())));
    m_buttons[BRowDown]->setEnabled(item->nextSibling());
}

void
EditListViewDialog::loadChildNodes(Q3ListView *listview, Q3ListViewItem *item, Q3ListViewItem *parent)
{
    Q3ListViewItem *newItem;
    if (listview->inherits("KListView")) {
        if (parent)
            newItem = new K3ListViewItem(parent);
        else
            newItem = new K3ListViewItem(listview);
    } else {
        if (parent)
            newItem = new Q3ListViewItem(parent);
        else
            newItem = new Q3ListViewItem(listview);
    }

    // We need to move the item at the end, which is the expected behaviour (by default it is inserted at the beginning)
    Q3ListViewItem *last;
    if (parent)
        last = parent->firstChild();
    else
        last = listview->firstChild();

    while (last->nextSibling())
        last = last->nextSibling();
    newItem->moveItem(last);

    // We copy the text of all the columns
    for (int i = 0; i < listview->columns(); i++)
        newItem->setText(i, item->text(i));

    Q3ListViewItem *child = item->firstChild();
    if (child)
        newItem->setOpen(true);
    while (child)  {
        loadChildNodes(listview, child, newItem);
        child = child->nextSibling();
    }
}

void
EditListViewDialog::newRow()
{
    K3ListViewItem *parent = (K3ListViewItem*)m_listview->selectedItem();
    if (parent)
        parent = (K3ListViewItem*)parent->parent();
    K3ListViewItem *item;
    if (parent)
        item = new K3ListViewItem(parent, m_listview->selectedItem());
    else
        item = new K3ListViewItem(m_listview, m_listview->selectedItem());
    item->setText(0, i18n("New Item"));
    m_listview->setCurrentItem(item);
}

void
EditListViewDialog::newChildRow()
{
    K3ListViewItem *parent = (K3ListViewItem*)m_listview->currentItem();
    K3ListViewItem *item;
    if (parent)
        item = new K3ListViewItem(parent);
    else
        item = new K3ListViewItem(m_listview, m_listview->currentItem());
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
    Q3ListViewItem *item = m_listview->currentItem()->itemAbove();
    item->moveItem(m_listview->currentItem());
    updateButtons(m_listview->currentItem());
}

void
EditListViewDialog::MoveRowDown()
{
    Q3ListViewItem *before = m_listview->currentItem();
    before->moveItem(before->nextSibling());
    updateButtons(before);
}

}

#include "editlistviewdialog.moc"
