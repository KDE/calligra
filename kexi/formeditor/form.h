/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KFORMDESIGNERFORM_H
#define KFORMDESIGNERFORM_H

#include <qobject.h>
#include <q3ptrlist.h>
#include <Q3ValueList>
#include <Q3CString>

#include "resizehandle.h"
#include "utils.h"
#include "objecttree.h"

class QWidget;
class QDomElement;
class KActionCollection;
class KCommandHistory;
class KCommand;
class PixmapCollection;

namespace KFormDesigner {

class Container;
class WidgetPropertySet;
class WidgetLibrary;
class FormManager;
class ObjectTree;
class ObjectTreeItem;
class ConnectionBuffer;

//! Base (virtual) class for all form widgets
/*! You need to inherit this class, and implement the drawing functions. This is necessary
 because you cannot inherit QWidget twice, and we want form widgets to be any widget.
 See FormWidgetBase in test/kfd_part.cpp and just copy functions there. */
class KFORMEDITOR_EXPORT FormWidget
{
	public:
		FormWidget();
		virtual ~FormWidget();

		/*! This function draws the rects in the \a list  in the Form, above of all widgets,
		 using double-buffering. \a type can be 1 (selection rect)
		 or 2 (insert rect, dotted). */

		virtual void drawRects(const Q3ValueList<QRect> &list, int type) = 0;

		virtual void drawRect(const QRect &r, int type) = 0;

		/*! This function inits the buffer used for double-buffering. Called before drawing rect. */
		virtual void initBuffer() = 0;

		/*! Clears the form, ie pastes the whole buffer to repaint the Form. */
		virtual void clearForm() = 0;

		/*! This function highlights two widgets (to is optional), which are
		sender and receiver, and draws a link between them. */
		virtual void highlightWidgets(QWidget *from, QWidget *to) = 0;
	
	protected:
		Form *m_form;

	friend class Form;
};

//! @internal
class FormPrivate
{
	public:
		FormPrivate();
		~FormPrivate();

//		FormManager  *manager;
		QPointer<Container>  toplevel;
		ObjectTree  *topTree;
		QPointer<QWidget> widget;

		WidgetList  selected;
		ResizeHandleSet::Dict resizeHandles;

		bool  dirty;
		bool  interactive;
		bool  design;
		QString  filename;

		KCommandHistory  *history;
		KActionCollection  *collection;

		ObjectTreeList  tabstops;
		bool  autoTabstops;
		ConnectionBuffer  *connBuffer;

		PixmapCollection  *pixcollection;

		//! This map is used to store cursor shapes before inserting (so we can restore them later)
		QMap<QObject*,QCursor> cursors;

		//!This string list is used to store the widgets which hasMouseTracking() == true (eg lineedits)
		QStringList *mouseTrackers;

		FormWidget  *formWidget;

		//! A set of head properties to be stored in a .ui file.
		//! This includes KFD format version.
		QMap<Q3CString,QString> headerProperties;

		//! Format version, set by FormIO or on creating a new form.
		uint formatVersion;
		//! Format version, set by FormIO's loader or on creating a new form.
		uint originalFormatVersion;
};

/*!
  This class represents one form and holds the corresponding ObjectTree and Containers.
  It takes care of widget selection and pasting widgets.
 **/
 //! A simple class representing a form
class KFORMEDITOR_EXPORT Form : public QObject
{
	Q_OBJECT

	public:
		/*! Creates a simple Form, child of the FormManager \a manager.
		 */
		Form(WidgetLibrary* library, const char *name=0, bool designMode = true);
		~Form();

		//! \return A pointer to the WidgetLibrary supporting this form.
		WidgetLibrary* library() const { return m_lib; }

		/*!
		 Creates a toplevel widget out of another widget.
		 \a container will become the Form toplevel widget,
		 will be associated to an ObjectTree and so on.
		 \code QWidget *toplevel = new QWidget(this);
		 form->createToplevel(toplevel); \endcode
		 */
		void createToplevel(QWidget *container, FormWidget *formWidget =0,
			const Q3CString &classname="QWidget");

		/*! \return the toplevel Container or 0 if this is a preview Form or createToplevel()
		   has not been called yet. */
		Container* toplevelContainer() const { return d->toplevel; }

		//! \return the FormWidget that holds this Form
		FormWidget* formWidget() const { return d->formWidget; }

		//! \return a pointer to this form's ObjectTree.
		ObjectTree* objectTree() const { return d->topTree; }

		//! \return the form's toplevel widget, or 0 if designMode() == false.
		QWidget* widget() const;

//		//! \return the FormManager parent of this form.
//		FormManager* manager() const { return d->manager; }

		/*! \return A pointer to the currently active Container, ie the parent Container for a simple widget,
		    and the widget's Container if it is itself a container.
		 */
		Container* activeContainer();

		/*! \return A pointer to the parent Container of the currently selected widget.
		 It is the same as activeContainer() for a simple widget, but unlike this function
		  it will also return the parent Container if the widget itself is a Container.
		 */
		Container* parentContainer(QWidget *w=0);

		/*! \return The \ref Container which is a parent of all widgets in \a wlist.
		 Used by \ref activeContainer(), and to find where
		 to paste widgets when multiple widgets are selected. */
		ObjectTreeItem* commonParentContainer(WidgetList *wlist);

		//! \return the list of currently selected widgets in this form
		WidgetList* selectedWidgets() const {return &(d->selected);}

		/*! \return currently selected widget in this form,
		 or 0 if there is no widget selected or more than one widget selected.
		 \see selectedWidgets() */
		QWidget* selectedWidget() const { return d->selected.count()==1 ? d->selected.first() : 0; }

		/*! Emits the action signals, and optionaly the undo/redo related signals
		 if \a withUndoAction == true. See \a FormManager for signals description. */
		void emitActionSignals(bool withUndoAction=true);

		/*! Emits again all signal related to selection (ie Form::selectionChanged()).
		  Called eg when the user has the focus again. */
		void  emitSelectionSignals();

		/*! Sets the Form interactivity mode. Form is not interactive when
		pasting widgets, or loading a Form.
		 */
		void setInteractiveMode(bool interactive) { d->interactive = interactive; }

		/*! \return true if the Form is being updated by the user, ie the created
		widget were drawn on the Form.
		    \return false if the Form is being updated by the program, ie the widget
		     are created by FormIO, and so composed widgets
		    should not be populated automatically (such as QTabWidget).
		 */
		bool interactiveMode() const { return d->interactive; }

		/*! If \a design is true, the Form is in Design Mode (by default).
		If \a design is false, then the Form is in Preview Mode, so
		  the ObjectTree and the Containers are removed. */
		void setDesignMode(bool design);

		//! \return The actual mode of the Form.
		bool designMode() const { return d->design; }

		bool isModified() { return d->dirty; }

		//! \return the distance between two dots in the form background.
//! @todo make gridSize configurable at global level
		int gridSize() { return 10; }

		//! \return the default margin for all the layout inside this Form.
		int defaultMargin() { return 11;}

		//! \return the default spacing for all the layout inside this Form.
		int defaultSpacing() { return 6;}

		/*! This function is used by ObjectTree to emit childAdded() signal (as it is not a QObject). */
		void emitChildAdded(ObjectTreeItem *item);

		/*! This function is used by ObjectTree to emit childRemoved() signal (as it is not a QObject). */
		void emitChildRemoved(ObjectTreeItem *item);

		/*! \return The filename of the UI file this Form was saved to,
		or QString::null if the Form hasn't be saved yet. */
		QString filename() const { return d->filename; }

		//! Sets the filename of this Form to \a filename.
		void setFilename(const QString &file) { d->filename = file; }

		KCommandHistory* commandHistory() const { return d->history; }
		ConnectionBuffer* connectionBuffer() const { return d->connBuffer; }
		PixmapCollection* pixmapCollection() const { return d->pixcollection; }

		/*! Adds a widget in the form's command history. Please use it instead
		of calling directly actionCollection()->addCommand(). */
		void addCommand(KCommand *command, bool execute);

		/*! Clears form's command history. */
		void clearCommandHistory();

		/*! \return A pointer to this Form tabstops list : it contains all the widget
		 that can have focus ( ie no labels, etc)
		 in the order of the tabs.*/
		ObjectTreeList* tabStops() const { return &(d->tabstops); }

		inline ObjectTreeListIterator tabStopsIterator() const { return ObjectTreeListIterator(d->tabstops); }

		/*! Called (e.g. by KexiDBForm) when certain widgets can have updated focusPolicy properties
		 these having no TabFocus flags set are removed from tabStops() list. */
		void updateTabStopsOrder();

		/*! Adds the widget at the end of tabstops list. Called on widget creation. */
		void addWidgetToTabStops(ObjectTreeItem *c);

		/*! \return True if the Form automatically handles tab stops. */
		bool autoTabStops() const { return d->autoTabstops; }

		/*! If \a autoTab is true, then the Form will automatically handle tab stops,
		   and the "Edit Tab Order" dialog will be disabled.
		   The tab widget will be set from the top-left to the bottom-right corner.\n
		    If \ autoTab is false, then it's up to the user to change tab stops
		    (which are by default in order of creation).*/
		void setAutoTabStops(bool autoTab) { d->autoTabstops = autoTab;}

		/*! Tells the Form to reassign the tab stops because the widget layout has changed
		(called for example before saving or displaying the tab order dialog) */
		void autoAssignTabStops();

#ifdef KEXI_DEBUG_GUI
		//! For debugging purposes
		QString m_recentlyLoadedUICode;
#endif

		/*! Internal: called by ResizeHandle when mouse move event causes first
		 resize handle's dragging. As a result, current widget's editing (if any)
		 is finished - see WidgetFactory::resetEditor(). */
//		void resizeHandleDraggingStarted(QWidget *draggedWidget);

		ResizeHandleSet* resizeHandlesForWidget(QWidget* w);

		/*! A set of value/key pairs provided to be stored as attributes in 
		 <kfd:customHeader/> XML element (saved as a first child of \<UI> element). */
		QMap<Q3CString,QString>* headerProperties() const { return &d->headerProperties; }

		//! \return format version number for this form. 
		//! For new forms it is equal to KFormDesigner::version().
		uint formatVersion() const;
		void setFormatVersion(uint ver);

		//! \return original format version number for this form (as loaded from .ui XML string)
		//! For new forms it is equal to KFormDesigner::version().
		uint originalFormatVersion() const;
		void setOriginalFormatVersion(uint ver);

	public slots:
		/*! This slot is called when the name of a widget was changed in Property Editor.
		It renames the ObjectTreeItem associated to this widget.
		 */
		void changeName(const Q3CString &oldname, const Q3CString &newname);

		/*! Sets \a selected to be the selected widget of this Form. 
		 If \a add is true, the formerly selected widget is still selected, 
		 and the new one is just added. If false, \a selected replace the actually selected widget.
		 The form widget is always selected alone.
		 \a moreWillBeSelected indicates whether more widgets will be selected soon
		 (so for multiselection we should not update the property pane before the last widget is selected) */
		void setSelectedWidget(QWidget *selected, bool add=false, bool dontRaise=false,
			bool moreWillBeSelected = false);

		/*! Unselects the widget \a w. Te widget is removed from the Cntainer 's list
		and its resizeHandle is removed. */
		void unSelectWidget(QWidget *w);

		/*! Sets the form widget (it will be uniquely selected widget). */
		void selectFormWidget();

		void clearSelection();

	protected slots:
		/*! This slot is called when the toplevel widget of this Form is deleted
		(ie the window closed) so that the Form gets deleted at the same time.
		 */
		void formDeleted();

		void emitUndoEnabled();
		void emitRedoEnabled();

		/*! This slot is called when a command is executed. The undo/redo signals
		  are emitted to update actions. */
		void slotCommandExecuted();

		/*! This slot is called when form is restored, ie when the user has undone
		  all actions. The form modified flag is updated, and
		\ref FormManager::dirty() is called. */
		void slotFormRestored();

	signals:
		/*! This signal is emitted by setSelectedWidget() when user selects a new widget, 
		 to update both Property Editor and ObjectTreeView.
		 \a w is the newly selected widget.
		  */
		void selectionChanged(QWidget *w, bool add, bool moreWillBeSelected = false);

		/*! This signal is emitted when a new widget is created, to update ObjectTreeView.
		 \a it is the ObjectTreeItem representing this new widget.
		 */
		void childAdded(ObjectTreeItem *it);

		/*! This signal is emitted when a widget is deleted, to update ObjectTreeView.
		 \a it is the ObjectTreeItem representing this deleted widget.
		 */
		void childRemoved(ObjectTreeItem *it);

		//! This signal emitted when Form is about to be destroyed
		void destroying();

	protected:
		void setConnectionBuffer(ConnectionBuffer *b) { d->connBuffer = b; }

		void setFormWidget(FormWidget* w);
	private:
		WidgetLibrary *m_lib;
		FormPrivate *d;

		friend class FormManager;
		friend class FormWidget;
		friend class ConnectionDialog;
};

}

#endif
