/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
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

#ifndef FORMEDITORCONTAINER_H
#define FORMEDITORCONTAINER_H

#include <qobject.h>
#include <qptrlist.h>

class QEvent;
class QWidget;
class QLayout;
typedef QPtrList<QWidget> WidgetList;

namespace KFormDesigner {

class Container;
class WidgetLibrary;
class ObjectTreeItem;
class Form;

/**
 * This class makes a container out of any QWidget. You can then create child widgets, and the background is dotted.
 */
//! A class to make a container from any widget
class KFORMEDITOR_EXPORT Container : public QObject
{
	Q_OBJECT

	public:
		enum LayoutType { NoLayout=0, HBox, VBox, Grid };

		/**
		 * Creates a Container from the widget \a container, which have \a toplevel as parent Container.
		 */

		Container(Container *toplevel, QWidget *container, QObject *parent=0, const char *name=0);
		~Container();

		//! \return a pointer to the toplevel Container.
		Container	*toplevel();

		/**
		 * Sets the ObjectTree of this Container.\n
		 * NOTE: this is needed only if we are toplevel.
		 */
		void		setObjectTree(ObjectTreeItem *t) { m_tree = t; }

		/**
		 * \return The ObjectTreeItem assosiated with this Container's widget.
		 */
		ObjectTreeItem	*tree() const { return m_tree; }

		//! \return a pointer to the QLayout of this Container, or 0 if there is not.
		QLayout*        layout() const { return m_layout; }
		//! \return the type of the layout associated to this Container's widget (see LayoutType enum).
		LayoutType      layoutType() const { return m_layType; }
		/*! Sets this Container to use \a type of layout. The widget are inserted automatically in the layout
		  following their positions.
		 */
		void            setLayout(LayoutType type);

	public slots:
		//! \return The watched widget.
		QWidget		*widget() { return m_container; }

		//! Sets the Form which this Container belongs to.
		void		setForm(Form *form) { m_form = form; }

		//! \return The form this Container belongs to.
		Form		*form() const { return m_form; }

		/*! Deletes the selected child item of this Container, and remove it from ObjectTree. */
		void		deleteItem();

		/*! Sets \a selected to be the selected widget of this container (and so of the Form). If \a add is true, the formerly selected widget
		  is still selected, and the new one is just added. If false, \a selected replace the actually selected widget.
		 */
		void		setSelectedWidget(QWidget *selected, bool add);

		/*! Unselects the widget \a w. Te widget is removed from the Cntainer 's list and its resizeHandles are removed. */
		void		unSelectWidget(QWidget *w);

	protected slots:
		/*! This slot is called when the watched widget is deleted. Deletes the Container too. */
		void		widgetDeleted();

	protected:
		/*! This is the main function of Container, which filters the event sent to the watched widget.\n
		   It takes care of drawing the background and the insert rect, of creating the new child widgets, of moving the widgets and
		   pop up a menu when right-clicking.
		  */
		virtual bool	eventFilter(QObject *o, QEvent *e);
		/*! Internal function to create a HBoxLayout or VBoxLayout for this container. \a list is a subclass of QObjectList that can sort widgets
		   following their position (such as HorWidgetList or VerWidgetList).
		  */
		void		createBoxLayout(WidgetList *list);

	private:
		// the watched container and it's toplevel one...
		QWidget		*m_container;
		Container 	*m_toplevel;
		QLayout		*m_layout;
		LayoutType	m_layType;

		// selection
		//QWidget		*m_selected;
		QPtrList<QWidget> m_selected;

		// moving etc.
		QPoint		m_grab;
		QWidget		*m_moving;
		bool		m_move;

		//inserting
		QPoint		m_insertBegin;
		QRect		m_insertRect;
		ObjectTreeItem	*m_tree;

		Form		*m_form;
};

}
#endif
