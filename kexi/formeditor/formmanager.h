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

#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <qobject.h>
#include <qdom.h>
#include <qptrlist.h>
#include <qtimer.h>

class QWidget;
class QWorkspace;
class KPopupMenu;
class KexiPropertyEditor;
class KexiPropertyBuffer;
class KActionCollection;
class KAction;
class KToggleAction;
class KXMLGUIClient;
class KMainWindow;

namespace KFormDesigner {

class ObjectPropertyBuffer;
class Form;
class Container;
class WidgetLibrary;
class ObjectTreeView;
class Connection;
typedef QPtrList<KAction> Actions;

//! A class to manage (create/load/save) Forms
/** This is Form Designer's top class, which is used by external APIs to access FormDesigner. This is the class you have to use
   to integrate FormDesigner into another program.\n
   It deals with creating, saving and loading Form, as well as widget insertion and copying.\n
   It also ensures all the components (ObjectTreeView, Form and PropertyEditor) are synced, and link them.\n
   It holds the WidgetLibrary, the ObjectPropertyBuffer, links to ObjectTreeView and PropertyEditor, as well as the copied widget
   and the insert state.
 **/
class KFORMEDITOR_EXPORT FormManager : public QObject
{
	Q_OBJECT

	public:
		FormManager(QWidget *container, QObject *parent, const char *name);
		virtual ~FormManager();

		/*! Creates all the KAction related to widget insertion, and plug them into the KActionCollection \a parent.
		  These actions are automatically connected to insertWidget() slot.
		  \return a QPtrList of the created actions.
		 */
		Actions createActions(KActionCollection *parent, KMainWindow *client);

		/*! Sets the external editors used by FormDesigner (as they may be docked). This function also connects
		  appropriate signals and slots to ensure sync with the current Form.
		 */
		void setEditors(KexiPropertyEditor *editor, ObjectTreeView *treeview);

		//! \return A pointer to the WidgetLibrary owned by this Manager.
		WidgetLibrary*    lib() const { return m_lib; }
		//! \return A pointer to the ObjectPropertyBuffer owned by this Manager.
		ObjectPropertyBuffer*  buffer() const { return m_buffer; }
		/*! \return true if one of the insert buttons was pressed and the forms are ready to create a widget.
		 \return false otherwise.
		 */
		bool              inserting() const { return m_inserting; }
		/*! \return The name of the class being inserted, corresponding to the menu item or the toolbar button clicked.
		 */
		QString           insertClass() const { return m_insertClass; }

		/*! \return The popup menu to be shown when right-clicking on the form. Each container adds a widget-specific part
		  to this one before showing it. This menu contains Copy/cut/paste/remove.
		 */
		KPopupMenu*       popupMenu() const { return m_popup; }
		/*! The Container use this function to indicate the exec point of the contextual menu, which is used to position the
		  pasted widgets.
		 */
		void              setInsertPoint(const QPoint &p);

		bool              draggingConnection() { return m_drawingSlot; }
		Connection*       createdConnection() { return m_connection; }
		void              resetCreatedConnection();
		void              createSignalMenu(QWidget *w);
		void              createSlotMenu(QWidget *w);

		/*! \return The Form actually active and focused.
		 */
		Form*             activeForm() const;
		/*! \return true if \a w is a toplevel widget, ie it is the main widget of a Form (so it should have a caption ,
		 an icon ...)
		*/
		bool              isTopLevel(QWidget *w);

		//! \return A pointer to the KexiPropertyEditor we use.
		KexiPropertyEditor* editor() const { return m_editor; }

		/*! Creates a new blank Form, whose toplevel widget inherits \a classname. The Form is automatically shown. */
		QWidget *createBlankForm(const QString &classname, const char *name, QWidget *parent=0);

		/*! Adds a existing form w and changes it to a container */
		void importForm(QWidget *w, Form *form=0, bool preview=false);

		/*! Deletes the Form \a form and removes it from our list. */
		void deleteForm(Form *form);

		/*! Shows a propertybuffer in PropertyBuffer */
		virtual void showPropertyBuffer(ObjectPropertyBuffer *buff);
		/*! This function creates and displays the context menu corresponding to the widget \a w. The menu item are disabled if necessary, and
		    the widget specific part is added (menu from the factory and buddy selection). */
		void  createContextMenu(QWidget *w, Container *container, bool enableRemove);

		void  emitCreateSlot(const QString &widget, const QString &value) { emit createFormSlot(m_active, widget, value); }

	public slots:
		/*! Creates a new blank Form with default class top widget (ie QWidget). The new Form is shown and becomes
		   the active Form.
		  */
		void createBlankForm();
		/*! Loads a Form from a UI file. A "Open File" dialog is shown to select the file. The loaded Form is shown and becomes
		   the active Form.
		  */
		void loadForm(bool preview=false, const QString &filename=QString::null);
		/*! Save the active Form into a UI file. A "Save File" dialog is shown to choose a name for the file, but the former name
		  is used if there is one (using Form::filename()).
		 */
		void saveForm();
		/*! Save the active Form into a UI file. A "Save File" dialog is shown to choose a name for the file, even if the Form has
		    already been saved.
		 */
		void saveFormAs();
		/*! Previews the Form \a form using the widget \a w as toplevel container for this Form. */
		void previewForm(Form *form, QWidget *w, Form *toForm=0);
		/*! Deletes the selected widget in active Form and all of its children. */
		void deleteWidget();
		/*! Copies the slected widget and all its children of the active Form using an XML representation. */
		void copyWidget();
		/*! Cuts (ie Copies and deletes) the selected widget and all its children of the active Form using an XML representation. */
		void cutWidget();
		/*! Pastes the XML representation of the copied or cut widget. The widget is pasted when the user clicks the Form to
		  indicate the new position of the widget, or at the position of the contextual menu if there is one.
		 */
		void pasteWidget();

		/*! Displays a dialog where the user can modify the tab order of the active Form, by drag-n-drop or using up/down buttons. */
		void editTabOrder();
		/*! Adjusts the size of the selected widget, ie resize it to its size hint. */
		void adjustWidgetSize();
		/*! Creates a dialog to edit the current Form's PixmapCollection. */
		void editFormPixmapCollection();
		void editConnections();
		/*! This slot is called when the user presses a "Widget" toolbar button or a "Widget" menu item. Prepares all Forms for
		  creation of a new widget (ie changes cursor ...).
		 */
		void insertWidget(const QString &classname);
		/*! Stopts the current widget insertion (ie unset the cursor ...). */
		void stopInsert();
		void slotPointerClicked();

		void startDraggingConnection();
		void stopDraggingConnection();

		/*! Print to the command line the ObjectTree of the active Form (ie a line for each widget, with parent and name). */
		void debugTree();
		/*! Sets \a w as the selected widget in the active Form. (called by ObjectTreeView) */
		void setSelWidget(QWidget *w);

		/*! Calls this slot when the window activated changes (eg connect to QWorkspace::windowActivated(QWidget*)). You <b>need</b> to connect
		   to this slot, it will crash otherwise.
		  */
		void windowChanged(QWidget *w);

		//! Used to delayed widgets' deletion (in Container::deleteItem())
		void deleteWidgetLater( QWidget *w );

	protected slots:
		void deleteWidgetLaterTimeout();
		/*! Slot called when a buddy is choosed in the buddy list. Sets the label buddy. */
		void buddyChoosed(int id);
		void menuSignalChoosed(int id);
		void slotStyle();

		void layoutHBox();
		void layoutVBox();
		void layoutGrid();

		void slotConnectionCreated(Form*, Connection&);

	protected:
		void initForm(Form *form);
		/*! Slot called by the "Lay out in..." menu items. It creates a layout from the currently selected widgets (that must have the same parent). */
		void createLayout(int layoutType);

	signals:
		/*! this signal is emmited as the property buffer switched */
		void bufferSwitched(KexiPropertyBuffer *buff);
		/*! This signal is emitted when any change is made to the Form \a form, so it will need to be saved. */
		void dirty(KFormDesigner::Form *form);

		void createFormSlot(Form *form, const QString &widget, const QString &signal);
		void connectionCreated(Form *form, Connection &connection);
		void connectionAborted(Form *form);

	private:
		// Enum for menu items indexes
		enum { MenuCopy = 201, MenuCut, MenuPaste, MenuDelete, MenuHBox = 301, MenuVBox, MenuGrid, MenuNoBuddy = 501 };

		ObjectPropertyBuffer	*m_buffer;
		WidgetLibrary		*m_lib;
		QGuardedPtr<KexiPropertyEditor>  m_editor;
		QGuardedPtr<ObjectTreeView>  m_treeview;
		// Forms
		QPtrList<Form>		m_forms;
		QPtrList<Form>		m_preview;
		int			m_count;
		QGuardedPtr<Form>	m_active;
		QWidget			*m_parent;

		// Copy/Paste
		QDomDocument		m_domDoc;
		KPopupMenu		*m_popup;
		QPoint			m_insertPoint;
		QGuardedPtr<QWidget>	m_menuWidget;

		// Insertion
		bool			m_inserting;
		QString			m_insertClass;

		// Connection stuff
		bool			m_drawingSlot;
		Connection		*m_connection;
		KPopupMenu		*m_sigSlotMenu;

		// Actions
		KActionCollection	*m_collection;
		KMainWindow 		*m_client;
		KToggleAction		*m_pointer, *m_dragConnection;

		//! Used to delayed widgets deletion
		QTimer m_deleteWidgetLater_timer;
		QPtrList<QWidget> m_deleteWidgetLater_list;

		friend class PropertyCommand;
		friend class GeometryPropertyCommand;
		friend class CutWidgetCommand;
		friend class Form;
};

}

#endif

