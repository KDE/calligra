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

#ifndef KFORMDESIGNERFORM_H
#define KFORMDESIGNERFORM_H


#include <qobject.h>
#include <qptrlist.h>

#include "resizehandle.h"

class QWidget;
class QDomElement;
class KActionCollection;
class KCommandHistory;
class KCommand;

namespace KFormDesigner {

class Container;
class ObjectPropertyBuffer;
class FormManager;
//class ResizeHandleSet;
class ObjectTree;
class ObjectTreeItem;

/*!
  This class represents one form and holds the corresponding ObjectTree and Containers.
  It takes care of widget selection and pasting widgets.
 **/
class KFORMEDITOR_EXPORT Form : public QObject
{
	Q_OBJECT

	public:
		/*! Creates a simple Form, child of the FormManager \a manager.
		 */
		Form(FormManager *manager, const char *name=0);
		~Form();

		/*!
		 * Creates a toplevel widget out of another widget. \a container will become the Form toplevel widget,
		   will be associated to an ObjectTree and so on.
		 * /code QWidget *toplevel = new QWidget(this);
		 *  form->createToplevel(toplevel); /endcode
		 */
		void	createToplevel(QWidget *container, const QString &classname="QWidget");

		/*!
		 * \return the toplevel Container or 0 if there isn't any.
		 */
		Container			*toplevelContainer() const { return m_toplevel; }

		//! \return a pointer to this form's ObjectTree.
		ObjectTree		*objectTree() const { return m_topTree; }
		//! \return the FormManager parent of this form.
		FormManager*		manager() const { return m_manager; }
		//! \return the widget currently selected in this form, or 0 if there is not.
		QPtrList<QWidget>* selectedWidgets() {return &m_selected;}
		/*! \return A pointer to the currently active Container, ie the parent Container for a simple widget,
		    and the widget's Container if it is itself a container.
		 */
		Container*		activeContainer();
		/*! \return A pointer to the parent Container of the currently selected widget. It is the same as activeContainer() for
		    a simple widget, but unlike this function it will also return the parent Container if the widget itself is a Container.
		 */
		Container*		parentContainer(QWidget *w=0);
		/*! This function is used by child Container to tell Form which widget is selected. The ResizeHandleSet is changed accordingly,
		   and the PropertyBuffer and ObjectTreeView are updated.
		 */
		void			setCurrentWidget(QWidget *w);
		/*! Adds the widget \a w to the Form 's list of selected widgets. The widget gets ResizeHandles, and the ObjectPropertyBuffer is
		  modified. */
		void			addSelectedWidget(QWidget *w);
		/*! Unselects the widget \a w. Te widget is removed from the Cntainer 's list and its resizeHandle is removed. */
		void			unSelectWidget(QWidget *w);
		/*! Sets the Form interactivity mode. If is used when pasting widgets, or loading a Form.
		 */
		void			setInteractiveMode(bool interactive) { m_inter = interactive; }
		/*! \return true if the Form is being updated by the user, ie the created widget were drawn on the Form.
		    \return false if the Form is being updated by the program, ie the widget are created by FormIO, and so composed widgets
		    should not be populated automatically (such as QTabWidget).
		 */
		bool			interactiveMode() const { return m_inter; }

		/*! If \a design is true, the Form is in Design Mode (by default). If \a design is false, then the Form is in Preview Mode, so
		  the ObjectTree and the Containers are removed. */
		void			setDesignMode(bool design);
		//! \return The actual mode of the Form.
		bool			designMode() { return m_design; }

		//! \return the x distance between two dots in the background.
		static int		gridX() { return 10;}
		//! \return the y distance between two dots in the background.
		static int		gridY() { return 10;}

		/*! Pastes the widget represented by the QDomElement \a widg in the Form. \a widg is created by FormManager::saveWidget().\n
		    If \a pos is null or not given, then the widget will be pasted in the actual Container, at the same position as
		    in its former parent widget. Otherwise, it is pasted at \a pos in the active Container (when the user used the context menu).
		    If \a cont is 0, the Form::activeContainer() is used, otherwise the widgets are pasted in the \a cont Container.
		 */
		void			pasteWidget(QDomElement &widg, Container *cont=0, QPoint pos=QPoint());

		/*! This function is used by ObjectTree to emit childAdded() signal (as it is not a QObject). */
		void			emitChildAdded(ObjectTreeItem *item);
		/*! This function is used by ObjectTree to emit childRemoved() signal (as it is not a QObject). */
		void			emitChildRemoved(ObjectTreeItem *item);

		//! \return The filename of the UI file this Form was saved to, or QString::null if the Form hasn't be saved yet.
		QString			filename() const { return m_filename; }
		//! Sets the filename of this Form to \a filename.
		void			setFilename(const QString &file) { m_filename = file; }

		KCommandHistory*	commandHistory() { return m_history; }
		KActionCollection*	actionCollection() { return m_collection; }

		void addCommand(KCommand *command, bool execute);

	public slots:
		/*! This slot is called when the name of a widget was changed in Property Editor. It renames the ObjectTreeItem
		  associated to this widget.
		 */
		void			changeName(const QString &oldname, const QString &newname);
		/*! Sets \a w to be the selected widget in this Form.
		 */
		void			setSelWidget(QWidget *w);

	protected slots:
		/*! This slot is called when the toplevel widget of this Form is deleted (ie the window closed) so that the Form gets deleted
		  at the same time.
		 */
		void			formDeleted();

	signals:
		/*! This signal is emitted when user selects a new widget, to update both Property Editor and ObjectTreeView.
		   \a w is the newly selected widget.
		  */
		void			selectionChanged(QWidget *w);
		/*! This signal is emitted when the widget \a w is added to the list of selected widgets. */
		void			addedSelectedWidget(QWidget *w);
		/*! This signal is emitted when a new widget is created, to update ObjectTreeView.
		 \a it is the ObjectTreeItem representing this new widget.
		 */
		void			childAdded(ObjectTreeItem *it);
		/*! This signal is emitted when a widget is deleted, to update ObjectTreeView.
		 \a it is the ObjectTreeItem representing this deleted widget.
		 */
		void			childRemoved(ObjectTreeItem *it);

	protected:
		/*! Internal function used to fix the coordinates of a widget before pasting it (to paste it at the position of the
		   contextual menu). It modifies the "geometry" property of the QDomElement representing the widget.
		   \return the modified QDomElement.
		 */
		QDomElement  fixPos(QDomElement el, QPoint newpos);
		/*! Internal function used to fix the coordinates of a widget before pasting it (to avoid to have two widgets at the same position).
		   It moves the widget by (10, 10) increment (several times if there are already asted widgets at this position).
		   \return the modified QDomElement.
		 */
		QDomElement  fixPos(QDomElement el);
		/*! Internal function used to fix the names of the widgets before pasting them. It prevents from pasting a widget with
		  the same name as an actual widget. The child widgets are also fixed recursively.\n
		  If the name of the widget ends with a number (eg "QLineEdit1"), the new name is just incremented by one (eg becomes "QLineEdit2").
		  Otherwise, a "2" is just appended at the end of the name (eg "myWidget" becomes "myWidget2").
		 */
		void  fixNames(QDomElement el);

	private:
		FormManager		*m_manager;
		Container		*m_toplevel;
		ObjectTree		*m_topTree;

		QPtrList<QWidget>	m_selected;
		ResizeHandleSet::Dict m_resizeHandles;

		bool			m_inter;
		bool			m_design;
		QString			m_filename;

		KCommandHistory		*m_history;
		KActionCollection	*m_collection;
};

}
#endif
