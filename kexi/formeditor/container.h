/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <qwidget.h>
#include <qpixmap.h>

#include "resizehandle.h"

class QPixmap;
class QPoint;

namespace KFormDesigner {
/**
 *
 * Lucijan Busch
 **/
class Container;
//class ResizeHandleSet;
class WidgetLibrary;
class ObjectTreeItem;
class Form;

/**
 * this class makes a container out
 * of any QWidget
 */

class KFORMEDITOR_EXPORT Container : public QObject
{
	Q_OBJECT

	public:
		/**
		 * simply add a widget which you want to have as container
		 * this factory will care about the rest...
		 * (e.g. inserting widgets, painting the grid, ...)
		 */

		Container(Container *toplevel, QWidget *container, QObject *parent=0, const char *name=0, bool attach=false);
		~Container();

		/**
		 * inserts a constructed widget
		 */
		void		addWidget(QWidget *w, QRect r = QRect());

		/**
		 * clears the dots around selected widgets and emits the new selection.
		 * you won't need to call that in most cases.<br>
		 * note: use this for a toplevel container only!
		 */
		void		setSelectionChanged(QWidget *selected);

		/**
		 * @returns a pointer to the toplevel
		 */
		Container	*toplevel();

		/**
		 * sets the object tree
		 * NOTE: this is needed if we are toplevel
		 */
		void		setObjectTree(ObjectTreeItem *t) { m_tree = t; }

		/**
		 * @returns the treenode assosiated with current container
		 */
		ObjectTreeItem	*tree();

		void		emitPrepareInsert( const QString &);
//		void		emitPreparePaste();
//		void		emitPreparePaste(QWidget *w, bool cut);

		/**
		 * registers a sub-container and adds it to the widget tree
		 */
		void		registerChild(Container *t);

	signals:
		/**
		 * this siganl gets emmited when the mode changes<br>
		 * if e = true one can edit the form<br>
		 * if e = false one can use the form
		 */
		void		modeChanged(bool e);

		/**
		 * this singal is emmited as the userselection changes,
		 * you can probabbly ingnore the first prarameter
		 */
		void		selectionChanged(QWidget *selected);

	public slots:
		/**
		 * use this function to toggle between editing and viewing mode.<br>
		 * if e = true one can edit the form<br>
		 * if e = false one can use the form
		 */
		void		setEditingMode(bool e);

		/**
		 * prepares it self to for insertation (listen to click and mousemove eventFilter
		 * and resend a signal if we are toplevel.
		 */
		void		slotPrepareInsert( const QString &);
		
//		void		slotPreparePaste(QWidget *w, bool cut);

		/**
		 * resets the ui canges, e.g. showing a position-pointer made for widgetinserting
		 * (it is called e.g. after a widget gets created or the user abroats)
		 */
		void		stopInsert();

		/**
		 * @returns the watched widget
		 */
		QWidget		*widget() { return m_container; }

		void		setForm(Form *form);

		/**
		 * @returns the form this container belongs to
		 */
		Form		*form();

		void		deleteItem();
		void		copyWidget();
		void		cutWidget();
		void		pasteWidget();

	protected slots:
		/**
		 * this slot uselets widgets
		 */
		void		slotSelectionChanged(QWidget *selected);

		void		widgetDeleted();

	signals:
		void		prepareInsert( const QString &);
//		void		preparePaste(QWidget *,bool);
		void		insertStop();

	protected:
		virtual bool	eventFilter(QObject *o, QEvent *e);

		/**
		 * updates the dotted background (e.g. after a ResizeEvent)
		 */
		void		updateBackground();

	private:
		// the watched container and it's toplevel one...
		QWidget		*m_container;
		Container 	*m_toplevel;

		// selection
		ResizeHandleSet	*m_resizeHandles;
		QWidget		*m_selected;

		// moving etc.
		QPoint		m_grab;
		QWidget		*m_moving;

		//background grid:
		QPixmap		m_dotBg;
		int		m_gridX;
		int		m_gridY;

		//inserting
		bool		m_prepare;
		QPoint		m_insertBegin;
		QRect		m_insertRect;
		WidgetLibrary	*m_lib;
		QString		m_insertClass;
		ObjectTreeItem	*m_tree;
		
		//Copy/Paste
	//	QWidget		*m_copiedw;
	//	bool		m_cut;
		
		Form		*m_form;
};

}
#endif
