/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <qdom.h>
#include <qtimer.h>
#include <qpointer.h>
#include <qstringlist.h>
#include <QActionGroup>

#include "form.h"
#include <kexi_export.h>
#include <koproperty/EditorView.h>

class QWidget;
class QWorkspace;
class KMenu;
class KActionCollection;
class KAction;
class KToggleAction;
class KPageDialog;
class KTextEdit;
class KXMLGUIClient;

namespace KoProperty
{
class Editor;
class Set;
class Property;
class Widget;
}

namespace KFormDesigner
{

class WidgetPropertySet;
class Container;
class WidgetLibrary;
class ObjectTreeView;
class Connection;
class FormManager;
typedef QList<KAction*> ActionList;

//! A class to manage (create/load/save) Forms
/** This is Form Designer's main class, which is used by external APIs to access FormDesigner.
   This is the class you have to use to integrate FormDesigner into another program.
   It deals with creating, saving and loading Form, as well as widget insertion and copying.
   It also ensures all the components (ObjectTreeView, Form and PropertyEditor) are synced,
   and link them.
   It holds the WidgetLibrary, the WidgetPropertySet, links to ObjectTreeView and PropertyEditor,
   as well as the copied widget and the insert state.
 **/
class KFORMEDITOR_EXPORT FormManager : public QObject
{
    Q_OBJECT

public:
    /*! Constructs FormManager object.
     Using \a options you can control manager's behaviour, see Options. */
    FormManager(QObject *parent = 0, int options = 0, const char *name = 0);

    virtual ~FormManager();

    //! Creates widget library for supportedFactoryGroups
    //! and initializes FormManager singleton. \a m should be always the same for every call.
    static WidgetLibrary* createWidgetLibrary(FormManager* m,
            const QStringList& supportedFactoryGroups);

    //! Access to FormManager singleton
    static FormManager* self();

    /*! Options for creating FormManager objects.
    *   These are really bit-flags and may be or-ed together.
    */
    enum Options { HideEventsInPopupMenu = 1, SkipFileActions = 2,
                   HideSignalSlotConnections = 4
                 }; //todo

    /*! Creates all the KActions related to widget insertion, and plug them
      into the \a collection. \a client XML GUI client is used to call
      lib->addCustomWidgetActions(client).
      These actions are automatically connected to \ref insertWidget() slot.
      \return a QPtrList of the created actions.
     */
    ActionList createActions(WidgetLibrary *lib, KActionCollection* collection,
                             KXMLGUIClient *client);

    /*! Enables or disables actions \a name.
     KFD uses KPart's, action collection here.
     Kexi implements this to get (shared) actions defined elsewhere. */
    virtual void enableAction(const char* name, bool enable) = 0;

    /*! \return action for \a name. @see enableAction() */
    virtual QAction* action(const char* name) = 0;

    bool isPasteEnabled();

    //! \return A pointer to the WidgetPropertySet owned by this Manager.
    WidgetPropertySet* propertySet() const {
        return m_propSet;
    }

    /*! \return true if one of the insert buttons was pressed and the forms
     are ready to create a widget. */
    bool isInserting() const {
        return m_inserting;
    }

    /*! \return The name of the class being inserted, corresponding
     to the menu item or the toolbar button clicked. */
    QByteArray selectedClass() const {
        return m_selectedClass;
    }

    /*! Sets the point where the pasted widget should be moved to. */
    void setInsertPoint(const QPoint &p);

    //! \return If we are creating a Connection by drag-and-drop or not.
    bool isCreatingConnection() {
        return m_drawingSlot;
    }

    //! \return the Connection being created.
    Connection* createdConnection() {
        return m_connection;
    }

    /*! Resets the Connection being created. We stay in Connection creation mode,
      but we start a new connection (when the user clicks
     outside of signals/slots menu). */
    void resetCreatedConnection();

    //! Creates and display a menu with all the signals of widget \a w.
    void createSignalMenu(QWidget *w);

    //! Creates and display a menu with all the slots of widget \a w.
    void createSlotMenu(QWidget *w);

    //! Emits the signal \ref createFormSlot(). Used by WidgetPropertySet.
    void emitCreateSlot(const QString &widget, const QString &value) {
        emit createFormSlot(m_active, widget, value);
    }

    /*! \return The Form actually active and focused.
     */
    Form* activeForm() const;

    /*! \return the Form whose toplevel widget is \a w, or 0
     if there is not or the Form is in preview mode. */
    Form* formForWidget(QWidget *w);

    /*! \return true if \a w is a toplevel widget,
     ie. it is the main widget of a Form (so it should have a caption ,
     an icon ...) */
    bool isTopLevel(QWidget *w);

    //! \return A pointer to the KoProperty::EditorView we use.
    //unused KoProperty::EditorView* propertyEditor() const { return m_editor; }

    /*! Shows a property set \a set in a Property Editor.
     If \a buff is 0, Property Editor will be cleared.
     If \a forceReload is true, the set will be reloaded even
     if it's the same as previous one.
     If \a propertyToSelect is not empty, an item for this name will be selected
     (usable when previously there was no set visible). */
    virtual void showPropertySet(WidgetPropertySet *set, bool forceReload = false,
                                 const QByteArray& propertyToSelect = QByteArray());

    void blockPropertyEditorUpdating(void *blockingObject);

    void unblockPropertyEditorUpdating(void *blockingObject, WidgetPropertySet *set);

    /*! Sets the external property editor pane used by FormDesigner (it may be docked).*/
    void setEditor(KoProperty::EditorView *editor);

    /*! Sets the external object tree view used by FormDesigner (it may be docked).
     This function also connects appropriate signals and slots to ensure
     sync with the current Form. */
    void setObjectTreeView(ObjectTreeView *treeview);

    /*! Previews the Form \a form using the widget \a w as toplevel container for this Form. */
    void previewForm(Form *form, QWidget *w, Form *toForm = 0);

    /*! Adds a existing form w and changes it to a container */
    void importForm(Form *form = 0, bool preview = false);

    /*! Deletes the Form \a form and removes it from our list. */
    void deleteForm(Form *form);

    /*! This function creates and displays the context menu corresponding to the widget \a w.
        The menu item are disabled if necessary, and
        the widget specific part is added (menu from the factory and buddy selection). */
    void  createContextMenu(QWidget *w, Container *container, bool popupAtCursor = true);

    //! \return If we align widgets to grid or not.
    bool  snapWidgetsToGrid();

    //! @internal used by Container
    int contextMenuKey() const {
        return m_contextMenuKey;
    }

    //! @internal
    void emitWidgetSelected(KFormDesigner::Form* form, bool multiple);
    //! @internal
    void emitFormWidgetSelected(KFormDesigner::Form* form);
    //! @internal
    void emitNoFormSelected();

    /*! @internal
     \return true is redo action is being executed.
     Used in WidgetPropertySet::slotPropertyChanged() */
    bool isRedoing() const {
        return m_isRedoing;
    }

public slots:
    /*! Deletes the selected widget in active Form and all of its children. */
    void deleteWidget();

    /*! Copies the slected widget and all its children of the active Form using an XML representation. */
    void copyWidget();

    /*! Cuts (ie Copies and deletes) the selected widget and all its children of
     the active Form using an XML representation. */
    void cutWidget();

    /*! Pastes the XML representation of the copied or cut widget. The widget is
      pasted when the user clicks the Form to
      indicate the new position of the widget, or at the position of the contextual menu if there is one. */
    void pasteWidget();

    /*! Selects all toplevel widgets in trhe current form. */
    void selectAll();

    /*! Clears the contents of the selected widget(s) (eg for a line edit or a listview). */
    void clearWidgetContent();

    void undo();
    void redo();

    /*! Displays a dialog where the user can modify the tab order of the active Form,
     by drag-n-drop or using up/down buttons. */
    void editTabOrder();

    /*! Adjusts the size of the selected widget, ie resize it to its size hint. */
    void adjustWidgetSize();

    /*! Creates a dialog to edit the \ref activeForm() PixmapCollection. */
    void editFormPixmapCollection();

    /*! Creates a dialog to edit the Connection of \ref activeForm(). */
    void editConnections();

    //! Lay out selected widgets using HBox layout (calls \ref CreateLayoutCommand).
    void layoutHBox();
    //! Lay out selected widgets using VBox layout.
    void layoutVBox();
    //! Lay out selected widgets using Grid layout.
    void layoutGrid();
    //! Lay out selected widgets in an horizontal splitter
    void  layoutHSplitter();
    //! Lay out selected widgets in a verticak splitter
    void  layoutVSplitter();
    //! Lay out selected widgets using HFlow layout
    void layoutHFlow();
    //! Lay out selected widgets using VFlow layout.
    void layoutVFlow();

    //! Breaks selected layout(calls \ref BreakLayoutCommand).
    void breakLayout();

    void alignWidgetsToLeft();
    void alignWidgetsToRight();
    void alignWidgetsToTop();
    void alignWidgetsToBottom();
    void alignWidgetsToGrid();

    void adjustSizeToGrid();

    //! Resize all selected widgets to the width of the narrowest widget.
    void adjustWidthToSmall();

    //! Resize all selected widgets to the width of the widest widget.
    void adjustWidthToBig();

    //! Resize all selected widgets to the height of the shortest widget.
    void adjustHeightToSmall();

    //! Resize all selected widgets to the height of the tallest widget.
    void adjustHeightToBig();

    void bringWidgetToFront();
    void sendWidgetToBack();

    /*! This slot is called when the user presses a "Widget" toolbar button
      or a "Widget" menu item. Prepares all Forms for
      creation of a new widget (ie changes cursor ...).
     */
    void insertWidget(const QByteArray &classname);

    /*! Stops the current widget insertion (ie unset the cursor ...). */
    void stopInsert();

    //! Slot called when the user presses 'Pointer' icon. Switch to Default mode.
    void slotPointerClicked();

    //! Enter the Connection creation mode.
    void startCreatingConnection();

    //! Leave the Connection creation mode.
    void stopCreatingConnection();

    /*! Calls this slot when the window activated changes (eg connect
     to QWorkspace::windowActivated(QWidget*)). You <b>need</b> to connect
     to this slot, it will crash otherwise.
    */
    void windowChanged(QWidget *w);

    //! Used to delayed widgets' deletion (in Container::deleteItem())
    void deleteWidgetLater(QWidget *w);

    /*! For debugging purposes only:
     shows a text window containing contents of .ui XML definition of the current form. */
    void showFormUICode();

    /*! Executes font dialog and changes it for currently selected widget(s). */
    void changeFont();

signals:
    /*! This signal is emitted as the property set switched.
     If \a forceReload is true, the set needs to be reloaded even
     if it's the same as previous one. */
    void propertySetSwitched(KoProperty::Set *set, bool forceReload = false,
                             const QByteArray& propertyToSelect = QByteArray());

    /*! This signal is emitted when any change is made to the Form \a form,
     so it will need to be saved. */
    void dirty(KFormDesigner::Form *form, bool isDirty = true);

    /*! Signal emitted when a normal widget is selected inside \a form
     (ie not form widget). If \a multiple is true,
     then more than one widget is selected. Use this to update actions state. */
    void widgetSelected(KFormDesigner::Form *form, bool multiple);

    /*! Signal emitted when the form widget is selected inside \a form.
     Use this to update actions state. */
    void formWidgetSelected(KFormDesigner::Form *form);

    /*! Signal emitted when no form (or a preview form) is selected.
     Use this to update actions state. */
    void noFormSelected();

    /*! Signal emitted when undo action activation changes.
     \a text is the full text of the action (including command name). */
    void undoEnabled(bool enabled, const QString &text = QString());

    /*! Signal emitted when redo action activation changes.
     \a text is the full text of the action (including command name). */
    void redoEnabled(bool enabled, const QString &text = QString());

    /*! Signal emitted when the user choose a signal in 'Events' menu
     in context menu, or in 'Events' in property editor.
     The code editor should then create the slot connected to this signal. */
    void createFormSlot(KFormDesigner::Form *form, const QString &widget, const QString &signal);

    /*! Signal emitted when the Connection creation by drag-and-drop ends.
     \a connection is the created Connection. You should copy it,
      because it is deleted just after the signal is emitted. */
    void connectionCreated(KFormDesigner::Form *form, KFormDesigner::Connection &connection);

    /*! Signal emitted when the Connection creation by drag-and-drop is aborted by user. */
    void connectionAborted(KFormDesigner::Form *form);

    /*! Signal emitted when "autoTabStops" is changed. */
    void autoTabStopsSet(KFormDesigner::Form *form, bool set);

    /*! Signal emitted before the form gets finally deleted. \a form is still a valid pointer,
     but the widgets inside the form are in unknown state. */
    void aboutToDeleteForm(KFormDesigner::Form *form);

    /*! Signal emitted when new form gets created.  */
    void formCreated(KFormDesigner::Form *form);

protected slots:
    void deleteWidgetLaterTimeout();

    /*! Slot called when a buddy is chosen in the buddy list. Sets the label buddy. */
    void buddyChosen(QAction* action);

    /*! Slot called when the user chooses an item in signal (or slot) menu.
     The \ref createdConnection() is updated, and the connection created
     (for the signal menu). */
    void menuSignalChosen(QAction* action);

    /*! Slot called when the user changes current style using combbox in toolbar or menu. */
    void slotStyle();

    void slotConnectionCreated(KFormDesigner::Form*, KFormDesigner::Connection&);

    void slotSettingsChanged(int category);

protected:
    /*! Inits the Form, adds it to m_forms, and conects slots. */
    void initForm(Form *form);

    /*! \return action group containing "insert widget" actions for each widget. */
    QActionGroup* widgetActionGroup() const {
        return m_widgetActionGroup;
    }

    /*! Function called by the "Lay out in..." menu items. It creates a layout from the
      currently selected widgets (that must have the same parent).
      Calls \ref CreateLayoutCommand. */
    void createLayout(int layoutType);

    /*! Function called by all other AlignWidgets*() function. Calls \ref AlignWidgetsCommand. */
    void alignWidgets(int type);

    void enableFormActions();
    void disableWidgetActions();
    void emitUndoEnabled(bool enabled, const QString &text);
    void emitRedoEnabled(bool enabled, const QString &text);

    /*! True if emitSelectionSignals() updates property set so showPropertySet() will
     not be needed in windowChanged(). False by default. Set to true in KexiFormManager. */
    bool m_emitSelectionSignalsUpdatesPropertySet : 1;

private:
    static FormManager* _self;

    //! Enum for menu items indexes
    enum { MenuTitle = 200, MenuCopy, MenuCut, MenuPaste, MenuDelete, MenuHBox = 301,
           MenuVBox, MenuGrid, MenuHSplitter, MenuVSplitter, MenuNoBuddy = 501
         };

    WidgetPropertySet *m_propSet;

    QPointer<KoProperty::EditorView>  m_editor;

    QPointer<ObjectTreeView>  m_treeview;
    // Forms
    QList<Form*> m_forms;
    QList<Form*> m_preview;
    QPointer<Form> m_active;

    // Copy/Paste
    QDomDocument m_domDoc;
    KMenu *m_popup;
    QPoint m_insertPoint;
    QPointer<QWidget> m_menuWidget;

    // Insertion
    bool m_inserting;
    QByteArray m_selectedClass;

    // Connection stuff
    bool m_drawingSlot;
    Connection *m_connection;
    KMenu *m_sigSlotMenu;

    // Actions
    KActionCollection *m_collection;
    QActionGroup* m_widgetActionGroup;
    KToggleAction *m_pointer, *m_dragConnection, *m_snapToGrid;
    QPointer<QAction> m_menuNoBuddy;

    //! Used to delayed widgets deletion
    QTimer m_deleteWidgetLater_timer;
    QWidgetList m_deleteWidgetLater_list;

#ifdef KEXI_DEBUG_GUI
    KPageDialog *m_uiCodeDialog;
    KTextEdit *m_currentUICodeDialogEditor;
    KTextEdit *m_originalUICodeDialogEditor;
#endif

    int m_options; //!< @see Options enum
    int m_contextMenuKey; //!< Id of context menu key (cached)

    void *m_objectBlockingPropertyEditorUpdating;
    QStyle *m_style;
    bool m_isRedoing : 1;

    friend class PropertyCommand;
    friend class GeometryPropertyCommand;
    friend class CutWidgetCommand;
    friend class LibActionWidget;
    friend class Form;
};

}

#endif
