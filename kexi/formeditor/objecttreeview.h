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
		const QString	name();
		
		//void            setActive(bool active) { m_selected = selection; }
		//bool            isActive() { return m_selected; }

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
		//bool               m_selected;
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
		
		void setSelWidget(const QString &name);

	public slots:
		void slotColumnSizeChanged(int);
		void setSelWidget(QWidget *w);

	signals:
		void selectionChanged(QWidget *w);
	
	protected:
		//! Internal function to fill the list.
		void loadTree(ObjectTreeItem *item, ObjectTreeViewItem *parent);

	private:
		Form    *m_form;
		//ObjectTreeViewItem   *m_active;
		ObjectTreeViewItem   *m_topItem;
};

}

#endif

