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

#ifndef OBJECTTREEVIEW_H
#define OBJECTTREEVIEW_H

#include <klistview.h>

namespace KFormDesigner {

class ObjectTreeItem;
class Form;

//! An item in ObjectTreeView associated to an ObjectTreeItem.
class KFORMEDITOR_EXPORT ObjectTreeViewItem : public KListViewItem
{
	public:
		ObjectTreeViewItem(ObjectTreeViewItem *parent, ObjectTreeItem *item);
		ObjectTreeViewItem(KListView *list);
		~ObjectTreeViewItem() {;}

		//! \return the item name, ie the ObjectTreeItem name
		const QString	name() const;
		//! \return the ObjectTreeItem associated to this item.
		ObjectTreeItem* objectTree() const { return m_item; }

	protected:
		//! Reimplemented to draw custom contents (copied from Property Editor)
		virtual void paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align);
		//! Reimplemented to draw custom contents (copied from Property Editor)
		virtual void paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h);
		//! Reimplemented to draw custom contents (copied from Property Editor)
		virtual void setup();

		/*! \return The pixmap rerpresenting the widget with \a classname, to be shown in the list.
		   Not implemented yet.
		 */
		static QPixmap iconByName(const QString &classname);

	private:
		ObjectTreeItem     *m_item;

	friend class ObjectTreeView;
};

//! A graphical view of the ObjectTree of a Form.
/*! This is a KListView which represents an item for each widget in the form. The actually selected widget is written bold
    and selected. Clicking on a list item selects the corresponding widget in the Form.
 */
class KFORMEDITOR_EXPORT ObjectTreeView : public KListView
{
	Q_OBJECT

	public:
		ObjectTreeView(QWidget *parent=0, const char *name=0);
		~ObjectTreeView(){;}

		virtual QSize sizeHint() const;

		/*! Sets \a form as the current Form in the list. The list will automatically be filled with an item for each widget
		    in the Form, and selection will be synced. Nothing happens if \a form is already the current Form.
		 */
		void setForm(Form *form);
		QString  pixmapForClass(const QString &classname);

	public slots:
		void slotColumnSizeChanged(int);
		/*! Sets the widget \a w as selected item, so it will be written bold. */
		void setSelWidget(QWidget *w);
		/*! Adds the ObjectTreeItem \a item in the list, with the appropriate parent. */
		void addItem(ObjectTreeItem *item);
		/*! Removess the ObjectTreeItem \a item from the list. */
		void removeItem(ObjectTreeItem *item);
		/*! The selected list item has changed, so we emit a signal to update the Form. */
		void emitSelChanged(QListViewItem *item);
		/*! Just renames the list item from \a oldname to \a newname. */
		void renameItem(const QString &oldname, const QString &newname);
		void displayContextMenu(KListView *list, QListViewItem *item, const QPoint &p);

	signals:
		/*! This signal is emitted when the user changes the list item selected, so that the Form and the Property
		  Editor gets updated.
		 */
		void selectionChanged(QWidget *w);

	protected:
		//! Internal function to fill the list.
		ObjectTreeViewItem* loadTree(ObjectTreeItem *item, ObjectTreeViewItem *parent);
		//! \return The item whose name is \a name.
		ObjectTreeViewItem* findItem(const QString &name);

	private:
		Form    *m_form;
		ObjectTreeViewItem   *m_topItem;
};

}

#endif

