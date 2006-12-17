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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef FORMEDITORCONTAINER_H
#define FORMEDITORCONTAINER_H

#include <qobject.h>
#include <qpointer.h>
#include <q3ptrlist.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>

#include "utils.h"

class QEvent;
class QWidget;
class QLayout;

namespace KFormDesigner {

class Container;
class WidgetLibrary;
class ObjectTreeItem;
class Form;

/**
 * This class is used to filter the events from any widget (and all its subwidgets)
 * and direct it to the Container.
 */
//! A class for redirecting events
class KFORMEDITOR_EXPORT EventEater : public QObject
{
	Q_OBJECT

	public:
		/*! Constructs eater object. All events for \a widget and it's subwidgets
		will be redirected to \a container. \a container will be also parent of eater object,
		so you don't need to care about deleting it. */
		EventEater(QWidget *widget, QObject *container);
		~EventEater();

		//! Sets the object which will receive the events
		void setContainer(QObject *container)  { m_container = container; }
		bool eventFilter(QObject *o, QEvent *ev);

	private:
		QPointer<QWidget>  m_widget;
		QPointer<QObject>  m_container;
};

/**
 * This class makes a container out of any QWidget. You can then create child widgets, and
 the background is dotted.
 */
//! A class to make a container from any widget
class KFORMEDITOR_EXPORT Container : public QObject
{
	Q_OBJECT

	public:
		enum LayoutType { NoLayout=0, HBox, VBox, Grid, HFlow, VFlow, /* special types */ HSplitter, VSplitter };

		/**
		 * Creates a Container from the widget \a container, which have
		 \a toplevel as parent Container. */
		Container(Container *toplevel, QWidget *container, QObject *parent=0, const char *name=0);
		virtual ~Container();

		//! \return a pointer to the toplevel Container, or 0 if this Container is toplevel
		Container* toplevel();

		//! \return The form this Container belongs to.
		Form* form() const { return m_form; }

		//! \return The watched widget.
		QWidget* widget() const { return m_container; }

		//! \return The ObjectTreeItem associated with this Container's widget.
		ObjectTreeItem* objectTree() const { return m_tree; }

		//! Sets the Form which this Container belongs to.
		void setForm(Form *form);

		/*! Sets the ObjectTree of this Container.\n
		 * NOTE: this is needed only if we are toplevel. */
		void setObjectTree(ObjectTreeItem *t) { m_tree = t; }

		//! \return a pointer to the QLayout of this Container, or 0 if there is not.
		QLayout* layout() const { return m_layout; }

		//! \return the type of the layout associated to this Container's widget (see LayoutType enum).
		LayoutType layoutType() const { return m_layType; }

		//! \return the margin of this Container.
		int layoutMargin() { return m_margin; }

		//! \return the spacing of this Container.
		int layoutSpacing() { return m_spacing; }

		/*! Sets this Container to use \a type of layout. The widget are inserted
		 automatically in the layout following their positions.
		  \sa createBoxLayout(), createGridLayout() */
		void setLayout(LayoutType type);

		//! Sets the spacing of this Container.
		void setLayoutSpacing(int spacing) { m_spacing = spacing;}

		//! Sets the margin of this Container.
		void setLayoutMargin(int margin) { m_margin = margin;}

		//! \return the string representing the layoutType \a type.
		static QString layoutTypeToString(int type);

		//! \return the LayoutType (an int) for a given layout name.
		static LayoutType stringToLayoutType(const QString &name);

		/*! Stops the inline editing of the current widget (as when you click
		 on another widget or press Esc). */
		void stopInlineEditing() { m_state = DoingNothing; }

		/*! This is the main function of Container, which filters the event sent
		   to the watched widget.\n It takes care of drawing the background and
		   the insert rect, of creating the new child widgets, of moving the widgets
		    and pop up a menu when right-clicking. */
		virtual bool eventFilter(QObject *o, QEvent *e);

	public slots:
		/*! Sets \a selected to be the selected widget of this container
		  (and so of the Form). If \a add is true, the formerly selected widget
		  is still selected, and the new one is just added. If false, \a selected
		   replace the actually selected widget. If \a dontRaise is true, then
		  the widget \a selected (and its parent) won't be raised (eg when you
		   select widget in ObjectTreeView).
		  \sa Form::setSelectedWidget() */
		void setSelectedWidget(QWidget *selected, bool add, bool dontRaise=false,
			bool moreWillBeSelected = false);

		/*! Unselects the widget \a w. The widget is removed from the Form's list
		 and its resizeHandles are removed. */
		void unSelectWidget(QWidget *w);

		/*! Deletes the widget \a w. Removes it from ObjectTree, and sets selection
		 to Container's widget. */
		void deleteWidget(QWidget *w);

		/*! Recreates the Container layout. Calls this when a widget has been moved
		 or added to update the layout. */
		void reloadLayout();

	protected slots:
		/*! This slot is called when the watched widget is deleted. Deletes the Container too. */
		void widgetDeleted();

	protected:
		/*! Internal function to create a HBoxLayout or VBoxLayout for this container.
		 \a list is a subclass of QObjectList that can sort widgets
		   following their position (such as HorWidgetList or VerWidgetList). */
		void createBoxLayout(WidgetList *list);

		/*! Internal function to create a KexiFlowLayout. */
		void createFlowLayout();

		/*! Internal function to create a GridLayout. if \a testOnly is true, the layout
		  is simulated, and only the widget's grid info aris filled. */
		void createGridLayout(bool testOnly=false);

		void drawConnection(QMouseEvent *mev);
		void drawSelectionRect(QMouseEvent *mev);
		void drawInsertRect(QMouseEvent *mev, QObject *s);
		void drawCopiedWidgetRect(QMouseEvent *mev);

		void moveSelectedWidgetsBy(int realdx, int realdy, QMouseEvent *mev=0);

	private:
		bool handleMouseReleaseEvent(QObject *s, QMouseEvent *mev);

		// the watched container and it's toplevel one...
		QPointer<QWidget> m_container;
		QPointer<Container> m_toplevel;

		int m_state;
		enum { DoingNothing = 100, DrawingSelectionRect, CopyingWidget,
			MovingWidget, InlineEditing };

		// Layout
		QLayout *m_layout;
		LayoutType m_layType;
		int m_margin, m_spacing;

		// moving etc.
		QPoint m_grab;
		//QPoint		m_initialPos;
		QPointer<QWidget> m_moving;
		//QRect		m_copyRect;

		//inserting
		QPoint m_insertBegin;
		QRect m_insertRect;
		ObjectTreeItem *m_tree;

		QPointer<Form> m_form;
		bool m_mousePressEventReceived;
		QMouseEvent m_mouseReleaseEvent;
		QPointer<QObject> m_objectForMouseReleaseEvent;

		friend class InsertWidgetCommand;
		friend class PasteWidgetCommand;
		friend class DeleteWidgetCommand;
		friend class FormIO;
};

//! Interface for adding dynamically created (at design time) widget to event eater.
/*! This is currently used by KexiDBFieldEdit from Kexi forms. */
class KFORMEDITOR_EXPORT DesignTimeDynamicChildWidgetHandler
{
	public:
		DesignTimeDynamicChildWidgetHandler();
		~DesignTimeDynamicChildWidgetHandler();

	protected:
		void childWidgetAdded(QWidget* w);
		void assignItem(ObjectTreeItem* item) { m_item = item; }

	private:
		ObjectTreeItem* m_item;
		friend class InsertWidgetCommand;
		friend class FormIO;
};

}

#endif
