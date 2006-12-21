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

#ifndef OBJECTTREEVIEW_H
#define OBJECTTREEVIEW_H

#include <k3listview.h>
#include <Q3CString>

#include <kexi_export.h>

namespace KFormDesigner {

class ObjectTreeItem;
class Form;

//! @short An item in ObjectTreeView associated with an ObjectTreeItem.
class KFORMEDITOR_EXPORT ObjectTreeViewItem : public K3ListViewItem
{
	public:
		ObjectTreeViewItem(ObjectTreeViewItem *parent, ObjectTreeItem *item);
		ObjectTreeViewItem(K3ListView *list, ObjectTreeItem *item=0);
		virtual ~ObjectTreeViewItem();

		//! \return the item name, ie the ObjectTreeItem name
		QString name() const;

		//! \return the ObjectTreeItem associated to this item.
		ObjectTreeItem* objectTree() const { return m_item; }

		virtual void setOpen( bool o );

	protected:
		//! Reimplemented to draw custom contents (copied from Property Editor)
		virtual void paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align);

		//! Reimplemented to draw custom contents (copied from Property Editor)
		virtual void paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h);

		//! Reimplemented to draw custom contents (copied from Property Editor)
		virtual void setup();

	private:
		ObjectTreeItem *m_item;

	friend class ObjectTreeView;
};

/*! @short A graphical view of Form's ObjectTree.
 This is a K3ListView which represents an item for each widget in the form. 
 The actually selected widget is written bold
 and selected. Clicking on a list item selects the corresponding widget in the Form.
 */
class KFORMEDITOR_EXPORT ObjectTreeView : public K3ListView
{
	Q_OBJECT

	public:
		ObjectTreeView(QWidget *parent=0, const char *name=0, bool tabStop = false);
		virtual ~ObjectTreeView();

		virtual QSize sizeHint() const;

		/*! Sets \a form as the current Form in the list. The list will automatically 
		 be filled with an item for each widget in the Form, and selection will be synced. 
		 Nothing happens if \a form is already the current Form.
		 */
		void setForm(Form *form);

		//! \return the pixmap name for a given class, to be shown next to the widget name.
		QString iconNameForClass(const Q3CString &classname);

	public slots:
		/*! Sets the widget \a w as selected item, so it will be written bold. 
		 It is added to current selection if \a add is true. */
		void setSelectedWidget(QWidget *w, bool add=false);

		/*! Adds the ObjectTreeItem \a item in the list, with the appropriate parent. */
		void addItem(ObjectTreeItem *item);

		/*! Removess the ObjectTreeItem \a item from the list. */
		void removeItem(ObjectTreeItem *item);

		/*! Just renames the list item from \a oldname to \a newname. */
		void renameItem(const Q3CString &oldname, const Q3CString &newname);

	protected slots:
		/*! This slot is called when the user right-click a list item. 
		 The widget context menu is shown, as inisde the Form. */
		void displayContextMenu(K3ListView *list, Q3ListViewItem *item, const QPoint &p);

		void slotColumnSizeChanged(int);

		/*! The selected list item has changed, so we emit a signal to update the Form. */
		void slotSelectionChanged();

		/*! Called before Form object is destroyed. */
		void slotBeforeFormDestroyed();

	protected:
		//! Internal function to fill the list.
		ObjectTreeViewItem* loadTree(ObjectTreeItem *item, ObjectTreeViewItem *parent);

		//! \return The item whose name is \a name.
		ObjectTreeViewItem* findItem(const QString &name);

	private:
		Form *m_form;
		ObjectTreeViewItem *m_topItem;

	friend class TabStopDialog;
};

}

#endif
