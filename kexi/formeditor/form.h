/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2014 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2014 Michał Poteralski <michalpoteralskikde@gmail.com>

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

#include "kformdesigner_export.h"
#include "widgetlibrary.h"
#include <kexi_global.h>

#include <QMetaProperty>
#include <QAction>

class QWidget;
class KActionCollection;

class PixmapCollection;

class KProperty;
class KPropertySet;

namespace KFormDesigner
{

class Command;
class PropertyCommandGroup;
#ifdef KFD_SIGSLOTS
class ConnectionBuffer;
#endif
class Container;
class ObjectTree;
class ObjectTreeItem;
typedef QList<ObjectTreeItem*> ObjectTreeList;
class FormPrivate;
class FormWidget;
class ResizeHandleSet;

/*!
  This class represents one form and holds the corresponding ObjectTree and Containers.
  It takes care of widget selection and pasting widgets.
 **/
//! A simple class representing a form
class KFORMEDITOR_EXPORT Form : public QObject
{
    Q_OBJECT

public:
    enum WidgetAlignment {
        AlignToGrid,
        AlignToLeft,
        AlignToRight,
        AlignToTop,
        AlignToBottom
    };

    //! Form's mode: design or data.
    enum Mode {
        DataMode,
        DesignMode
    };

    //! States like widget inserting. Used only in design mode.
    enum State {
        WidgetSelecting, //!< widget selecting
        WidgetInserting //!< widget inserting
#ifdef KFD_SIGSLOTS
        , Connecting       //!< signal/slot connecting
#endif
    };

    /*! Features used while creating Form objects. */
    enum Feature {
        NoFeatures = 0,
        EnableEvents = 1,
        EnableFileActions = 2
#ifdef KFD_SIGSLOTS
        , EnableConnections = 4
#endif
    };
    Q_DECLARE_FLAGS(Features, Feature)

    //! Types of layout
    enum LayoutType {
        NoLayout = 0,
        HBox,
        VBox,
        Grid,
        HFlow,
        VFlow,
        /* special types */
        HSplitter,
        VSplitter
    };

    /*! Creates Form object. */
    Form(WidgetLibrary* library, Mode mode, KActionCollection &col, ActionGroup& group);

    /*! Creates Form object as a child of other form. */
    explicit Form(Form *parent);

    ~Form();

    //! \return A pointer to the WidgetLibrary supporting this form.
    WidgetLibrary* library() const;

    KPropertySet& propertySet();

    void setFeatures(Features features);

    Features features() const;

    /*!
     Creates a toplevel widget out of another widget.
     \a container will become the Form toplevel widget,
     will be associated to an ObjectTree and so on.
     \code QWidget *toplevel = new QWidget(this);
     form->createToplevel(toplevel); \endcode
     */
    void createToplevel(QWidget *container, FormWidget *formWidget = 0,
                        const QByteArray &classname = "QWidget");

    /*! \return the toplevel Container or 0 if the form is in data mode or createToplevel()
       has not been called yet. */
    Container* toplevelContainer() const;

    //! \return the FormWidget that holds this Form
    FormWidget* formWidget() const;

    //! \return a pointer to this form's ObjectTree.
    ObjectTree* objectTree() const;

    //! \return the form's toplevel widget, or 0 if not in design mode.
    QWidget* widget() const;

    /*! \return A pointer to the currently active Container, ie the parent Container for a simple widget,
        and the widget's Container if it is itself a container.
     */
    Container* activeContainer();

    /*! \return A pointer to the parent Container of the currently selected widget.
     It is the same as activeContainer() for a simple widget, but unlike this function
      it will also return the parent Container if the widget itself is a Container.
     */
    Container* parentContainer(QWidget *w = 0) const;

    /*! \return The \ref Container which is a parent of all widgets in \a wlist.
     Used by \ref activeContainer(), and to find where
     to paste widgets when multiple widgets are selected. */
    ObjectTreeItem* commonParentContainer(const QWidgetList &wlist);

    //! \return the list of currently selected widgets in this form
    QWidgetList* selectedWidgets() const;

    /*! \return currently selected widget in this form,
     or 0 if there is no widget selected or more than one widget selected.
     \see selectedWidgets() */
    QWidget* selectedWidget() const;

    //! \return true if form widget itself is selected.
    /*! Used to check whether it is possible to execute copy/cut/delete actions. */
    bool isFormWidgetSelected() const;

    /*! Emits the action signals. */
    void emitActionSignals();

    /*! Emits the action signals for the undo/redo related signals. */
    void emitUndoActionSignals();

    /*! Emits again all signal related to selection (ie Form::selectionChanged()).
      Called eg when the user has the focus again. */
    void emitSelectionSignals();

    /*! Sets the Form interactivity mode. Form is not interactive when
    pasting widgets, or loading a Form.
     */
    void setInteractiveMode(bool interactive);

    /*! \return true if the Form is being updated by the user, ie the created
    widget were drawn on the Form.
        \return false if the Form is being updated by the program, ie the widget
         are created by FormIO, and so composed widgets
        should not be populated automatically (such as QTabWidget).
     */
    bool interactiveMode() const;

    /*! Sets form's mode to @a mode.
     In data mode, information related to design mode (object tree and the containers..) is removed. */
    void setMode(Mode mode);

    //! @return The actual mode of the Form.
    Mode mode() const;

    //! @return true if the form modification flag is set.
    bool isModified() const;

    //! Sets modification flag. Always emits modified() signal.
    void setModified(bool set);

    //! \return the default margin for all the layout inside this Form.
    int defaultMargin() const;

    //! \return the default spacing for all the layout inside this Form.
    int defaultSpacing() const;

    /*! This function is used by ObjectTree to emit childAdded() signal (as it is not a QObject). */
    void emitChildAdded(ObjectTreeItem *item);

    /*! This function is used by ObjectTree to emit childRemoved() signal (as it is not a QObject). */
    void emitChildRemoved(ObjectTreeItem *item);

    /*! \return The filename of the UI file this Form was saved to,
    or empty string if the Form hasn't be saved yet. */
//! @todo move this field out of this class
    QString filename() const;

    //! Sets the filename of this Form to \a filename.
//! @todo move this field out of this class
    void setFilename(const QString &file);

    /*! Clears form's undo/redo stack. */
    void clearUndoStack();

    /*! Sets undo/redo stack in a clean state, used when the document is saved. */
    void setUndoStackClean();

#ifdef KFD_SIGSLOTS
    ConnectionBuffer* connectionBuffer() const;
#endif

    PixmapCollection* pixmapCollection() const;

    void setPixmapsStoredInline(bool set);

    bool pixmapsStoredInline() const;

    //! Options for addCommand() method.
    enum AddCommandOption {
        DontExecuteCommand = 0, //!< command is not executed in addCommand()
        ExecuteCommand = 1      //!< command is executed in addCommand()
    };

    /*! Adds a command @a command in the form's undo/redo stack and returns true.
     If @a command is merged with the existing command and deleted, false is returned. */
    bool addCommand(Command *command, AddCommandOption option = ExecuteCommand);

    //! Creates a new PropertyCommand object and adds it to the undo/redo stack.
    /*! Takes care about the case when the same property of the same object is changed
     one-after-one. In this case only value of the present command on stack is changed.  */
    void addPropertyCommand(const QByteArray &wname, const QVariant &oldValue,
                            const QVariant &value, const QByteArray &propertyName,
                            AddCommandOption addOption, uint idOfPropertyCommand = 0);

    void addPropertyCommand(const QHash<QByteArray, QVariant> &oldValues,
                            const QVariant &value, const QByteArray &propertyName,
                            AddCommandOption addOption, uint idOfPropertyCommand = 0);

    //! Adds @a commandGroup to the undo/redo stack.
    /*! Assuming the @a commandGroup contains PropertyCommand objects, the method takes care
     about the case when the same properties of the same list of objects is changed
     one-after-one. In this case only values of the command in the present command group
     on the stack are changed and @a commandGroup is deleted.*/
    void addPropertyCommandGroup(PropertyCommandGroup *commandGroup,
                                 AddCommandOption addOption, uint idOfPropertyCommand = 0);

    /*! @return Command that being executed through addCommand() or 0 if addCommand()
     is not currently being executed.
     Currently used by WidgetTreeWidget to check if tab page adding was
     the original reason for adding widget. */
    const Command* executingCommand() const;

    /*! \return tabstops list. It contains all the widgets that can have focus
     (i.e. no labels, etc.) in the order of the tabs.*/
    ObjectTreeList* tabStops();

    /*! Called (e.g. by KexiDBForm) when certain widgets can have updated focusPolicy properties
     these having no TabFocus flags set are removed from tabStops() list. */
    void updateTabStopsOrder();

    /*! Adds the widget at the end of tabstops list. Called on widget creation. */
    void addWidgetToTabStops(ObjectTreeItem *it);

    /*! \return True if the Form automatically handles tab stops. */
    bool autoTabStops() const;

    /*! If \a autoTab is true (the default), then the Form will automatically handle tab stops,
       and the "Edit Tab Order" dialog will be disabled.
       The tab widget will be set from the top-left to the bottom-right corner.\n
        If \a autoTab is false, it's up to the user to change tab stops.*/
    void setAutoTabStops(bool autoTab);

    /*! Tells the Form to reassign the tab stops because the widget layout has changed
     (called for example before saving or displaying the tab order dialog).
     Automatically sorts widget from the top-left to bottom-right corner.
     Widget can be grouped with containers. In particular, for tab widgets,
     child widgets should ordered by parent tab's order. */
    void autoAssignTabStops();

    //! Specifies target of context menu. Used in createContextMenu().
    enum ContextMenuTarget
    {
        FormContextMenuTarget, //!< Context menu should be displayed on the form
                               //!< with offset specified by menuPos argument.
        WidgetTreeContextMenuTarget //!< Context menu should be displayed on the widget tree
                                    //!< area with offset specified by menuPos argument.
    };

    /*! This function creates and displays the context menu corresponding to the widget \a w.
        The menu item are disabled if necessary, and
        the widget specific part is added (menu from the factory and buddy selection). */
    void createContextMenu(QWidget *w, Container *container, const QPoint& menuPos, ContextMenuTarget target);

    //! @return true if snapping to grid is enabled. The defalt value is false.
    bool isSnapToGridEnabled() const;

    //! Sets state of snapping to grid to @a enabled.
    void setSnapToGridEnabled(bool enabled);

    //! @return size of grid. Positive values make sense here. The defalt value is 0.
    //! @todo make gridSize configurable at global level
    int gridSize() const;

    //! Sets size of grid to @a gridSize.
    void setGridSize(int gridSize);

#ifdef KEXI_DEBUG_GUI
    //! For debugging purposes
    QString m_recentlyLoadedUICode;
#endif

    ResizeHandleSet* resizeHandlesForWidget(QWidget* w);

    /*! A set of value/key pairs provided to be stored as attributes in
     <kfd:customHeader/> XML element (saved as a first child of \<UI> element). */
    QHash<QByteArray, QString>* headerProperties();

    //! \return format version number for this form.
    //! For new forms it is equal to KFormDesigner::version().
    uint formatVersion() const;
    void setFormatVersion(uint ver);

    //! \return original format version number for this form (as loaded from .ui XML string)
    //! For new forms it is equal to KFormDesigner::version().
    uint originalFormatVersion() const;
    void setOriginalFormatVersion(uint ver);

#ifdef KFD_SIGSLOTS
    /*! Related to signal/slots connections.
     Resets recently selected signal/slot connection (when the user clicks
     outside of signals/slots menu). We stay in "connection creation" mode,
     but user can only start a new connection. */
    void resetSelectedConnection();
#endif

    //! @return state ofthe Form, i.e. the current operation like inserting a widget or selecting.
    State state() const;

    //! selection flags used in methods like selectWidget()
    enum WidgetSelectionFlag {
        AddToPreviousSelection = 0,   //!< add to the previous selection, for clarity,
                                      //!< do not use with ReplacePreviousSelection
        ReplacePreviousSelection = 1, //!< replace the actually selected widget(s)
        MoreWillBeSelected = 0,       //!< indicates that more selections will be added
                                      //!< do not use with LastSelection
                                      //!< so the property editor can be updated (used without ReplacePreviousSelection)
        LastSelection = 2,            //!< indicates that this selection is the last one
                                      //!< so the property editor can be updated (used without ReplacePreviousSelection)
        Raise = 0,                    //!< raise the widget(s) on selection
                                      //!< do not use with DontRaise
        DontRaise = 4,                //!< do not raise the widget(s) on selection
        DefaultWidgetSelectionFlags = ReplacePreviousSelection | LastSelection | Raise
    };
    Q_DECLARE_FLAGS(WidgetSelectionFlags, WidgetSelectionFlag)

    /*! \return The name of the class being inserted, corresponding
     to the menu item or the toolbar button clicked. */
    QByteArray selectedClass() const;

    /*! @return widgets list for names @a names. Form widget, if present is omitted. */
    QList<QWidget*> widgetsForNames(const QList<QByteArray>& names) const;

    //! @return action from related action collection
    QAction* action(const QString& name);

    void createPropertyCommandsInDesignMode(QWidget* widget,
                                            const QHash<QByteArray, QVariant> &propValues,
                                            Command *parentCommand, bool addToActiveForm = true);

    //! @return class name of currently edited widget's
    QByteArray editedWidgetClass() const;

    //! @return true if we're within redo(). Useful to check if command is being executed
    //! in response of a redo or it is caused by some other event.
    bool isRedoing() const;

public Q_SLOTS:
    /*! Called when the user presses a widget item of the toolbox.
      The form enters into "widget inserting" state.
      Prepares all form's widgets for creation of a new widget
      (i.e. temporarily changes their cursor). */
    void enterWidgetInsertingState(const QByteArray &classname);

    //! Called when the user presses 'Pointer' icon. Switches to Default mode.
    void enterWidgetSelectingState();

    /*! Aborts the current widget inserting operation (i.e. unsets the cursor ...). */
    void abortWidgetInserting();

#ifdef KFD_SIGSLOTS
     //! Enters the Connection creation state.
     void enterConnectingState();

    //! Leave the Connection creation mode.
    void abortCreatingConnection();
#endif

    /*! Called when the name of a widget was changed in the Property Editor.
     Renames the ObjectTreeItem associated to this widget.
     */
    void changeName(const QByteArray &oldname, const QByteArray &newname);

    /*! Sets @a selected to be the selected widget of this Form.
     The form widget is always selected alone. */
    void selectWidget(QWidget *selected, WidgetSelectionFlags flags = DefaultWidgetSelectionFlags);

    /*! Sets all widgets @a widgets to be the selected for this Form.
     Form widget, if present is omitted. */
    void selectWidgets(const QList<QWidget*>& widgets, WidgetSelectionFlags flags);

    /*! Sets all widgets with @a names to be the selected for this Form.
     Form widget, if present is omitted. */
    void selectWidgets(const QList<QByteArray>& names, WidgetSelectionFlags flags);

    /*! Removes selection for widget \a w.
     The widget is removed from the Container's list
     and its resize handle is removed as well. */
    void deselectWidget(QWidget *w);

    /*! Sets the form widget selected. Deselects any previously selected widgets. */
    void selectFormWidget();

    /*! Clears the current selection. */
    void clearSelection();

    /*! Sets the point where the subsequently pasted widget should be moved to. */
    void setInsertionPoint(const QPoint &p);

    void undo();
    void redo();

    /*! Changes undoing state of the list. Used by Undo command to
     prevent recursion. */
    void setUndoing(bool isUndoing);

    bool isUndoing() const;

    bool isTopLevelWidget(QWidget *w) const;

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

    /*! Displays a dialog where the user can modify the tab order of the active Form,
     by drag-n-drop or using up/down buttons. */
    void editTabOrder();

    /*! Adjusts the size of the selected widget, ie resize it to its size hint. */
    void adjustWidgetSize();

    /*! Creates a dialog to edit the \ref activeForm() PixmapCollection. */
    void editFormPixmapCollection();

    /*! Creates a dialog to edit the Connection of \ref activeForm(). */
    void editConnections();

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

    /*! Executes font dialog and changes it for currently selected widget(s). */
    void changeFont();

    //! Sets slotPropertyChangedEnabled() enabled or disabled.
    void setSlotPropertyChangedEnabled(bool set);

    /*! @internal. This slot is called when the editor has lost focus or the user pressed Enter.
    It destroys the editor or installs again the event filter on the widget. */
    void resetInlineEditor();

    /*! This function provides a simple editing mode: it just disables event filtering
     for the widget, and it install it again when
     the widget loose focus or Enter is pressed.
    */
    void disableFilter(QWidget *w, Container *container);

protected Q_SLOTS:
    /*! This slot is called when the toplevel widget of this Form is deleted
    (ie the window closed) so that the Form gets deleted at the same time.
     */
    void formDeleted();

    void emitUndoEnabled();
    void emitRedoEnabled();

    /*! This slot is called when form is restored, ie when the user has undone
      all actions. The form modified flag is updated, and
    \ref modified() is called. */
    void slotFormRestored();

    /*!  This function is called every time a property is modifed.  It also takes
     care of saving set and enum properties. */
    void slotPropertyChanged(KPropertySet& set, KProperty& property);

    /*! This slot is called when a property is reset using the "reload" button in PropertyEditor. */
    void slotPropertyReset(KPropertySet& set, KProperty& property);

    /*! Default implementation changes "text" property.
    You have to reimplement this function for editing inside the Form to work if your widget's
    property you want to change isn't named "text".
    This slot is called when the line edit text changes, and you have to make
    it really change the good property of the widget using changeProperty() (text, or title, etc.).
    */
    void changeInlineTextInternal(const QString& text);

    void slotInlineTextChanged();

    /*! This slot is called when the editor is destroyed.*/
    void inlineEditorDeleted();

    void widgetDestroyed();

Q_SIGNALS:
    /*! This signal is emitted by selectWidget() when user selects a new widget,
     to update both the Property Editor and the Object Tree View.
     \a w is the newly selected widget.
      */
    void selectionChanged(QWidget *w, KFormDesigner::Form::WidgetSelectionFlags flags);

    /*! This signal is emitted when a new widget is created, to update ObjectTreeView.
     \a it is the ObjectTreeItem representing this new widget.
     */
    void childAdded(KFormDesigner::ObjectTreeItem *it);

    /*! This signal is emitted when a widget is deleted, to update ObjectTreeView.
     \a it is the ObjectTreeItem representing this deleted widget.
     */
    void childRemoved(KFormDesigner::ObjectTreeItem *it);

    //! This signal emitted when Form is about to be destroyed
    void destroying();

    /*! This signal is emitted when the property set has been switched. */
    void propertySetSwitched();

    /*! This signal is emitted when any change is made to the 'modified' flag of the form. */
    void modified(bool m);

    /*! Signal emitted when a normal widget is selected inside of the form
     (not the form's widget). If \a multiple is true, then more than one widget is selected.
     Use this to update actions state. */
    void widgetSelected(bool multiple);

    /*! Signal emitted when the form widget is selected inside of the form.
     Use this to update actions state. */
    void formWidgetSelected();

    /*! Signal emitted when no form (or a preview form) is selected.
     Use this to update actions state. */
    void noFormSelected();

#ifdef KFD_SIGSLOTS
    /*! Signal emitted when the Connection creation by drag-and-drop ends.
     \a connection is the created Connection. You should copy it,
      because it is deleted just after the signal is emitted. */
    void connectionCreated(KFormDesigner::Form *form, KFormDesigner::Connection &connection);

    /*! Signal emitted when the Connection creation by drag-and-drop is aborted by user. */
    void connectionAborted(KFormDesigner::Form *form);
#endif

    /*! Emitted when the name of the widget is modified.
     @a oldname is the name of the widget before the
     change, @a newname is the name after renaming. */
    void widgetNameChanged(const QByteArray &oldname, const QByteArray &newname);

protected:
    void emitSelectionChanged(QWidget *w, WidgetSelectionFlags flags);

    void updatePropertiesForSelection(QWidget *w, WidgetSelectionFlags flags);

#ifdef KFD_SIGSLOTS
    //! Sets connection buffer to @a b, which will be owned by the form.
    //! The previous buffer will be deleted, if there is any.
    void setConnectionBuffer(ConnectionBuffer *b);
#endif

    void setFormWidget(FormWidget* w);

    void emitFormWidgetSelected();

    void emitWidgetSelected(bool multiple);

    //! @internal
    void emitNoFormSelected();

    void enableFormActions();

    void disableWidgetActions();

    /*! Checks if the name entered by user is valid, ie that it is
     a valid identifier, and that there is no name conflict.  */
    bool isNameValid(const QString &name) const;

    void addWidget(QWidget *w);

    /*! Clears the current property set and fills it with properties related to the widget @a w.
     Also updates the newly created properties with previously set values. */
    void createPropertiesForWidget(QWidget *w);

    /*! Changes \a property old value and changed state, using the value
    stored in \a tree. Optional \a meta can be specified if you need to handle enum values. */
    void updatePropertyValue(ObjectTreeItem *tree, const char *property,
                             const QMetaProperty &meta = QMetaProperty());

    /*! Function called by all other alignWidgets*() function. */
    void alignWidgets(WidgetAlignment alignment);

    /*! This function is used to filter the properties to be shown
       (ie not show "caption" if the widget isn't toplevel).
       \return true if the property should be shown. False otherwise.*/
    bool isPropertyVisible(const QByteArray &property, bool isTopLevel,
                           const QByteArray &classname = QByteArray()) const;

    // Following methods are used to create special types of properties, different
    // from Q_PROPERTY

    /*! Creates the properties related to alignment (ie hAlign, vAlign and WordBreak) for
     the QWidget \a widget. \a subwidget is the same as \a widget if the widget itself handles
     the property and it's a child widget if the child handles the property.
     For example, the second case is true for KexiDBAutoField.
     \a meta  is the QMetaProperty for "alignment" property" of subwidget.  */
    void createAlignProperty(const QMetaProperty &meta, QWidget *widget, QWidget *subwidget);

    /*! Saves the properties related to alignment (ie hAlign, vAlign and WordBreak)
     and modifies the "alignment" property of  the widget.*/
    void saveAlignProperty(const QString &property);

    KActionCollection  *actionCollection() const;

    //! @todo rm when the 2 libs are merged
    KFormDesigner::ActionGroup* widgetActionGroup() const;

    /*! Called when a property has been changed.
      @a w is the widget concerned, @a property
      is the name of the modified property, and @a value is the new value of this property. */
    void handleWidgetPropertyChanged(QWidget *w, const QByteArray &name, const QVariant &value);

    /*! This function creates a QLineEdit to input some text and edit a widget's contents. */
    void createInlineEditor(const KFormDesigner::WidgetFactory::InlineEditorCreationArguments& args);

    QString inlineEditorText() const;

    void setInlineEditorText(const QString& text);

    /*! This function destroys the editor when it loses focus or Enter is pressed. */
    virtual bool eventFilter(QObject *obj, QEvent *ev);

private:
    void init(Mode mode, KActionCollection &col, KFormDesigner::ActionGroup &group);

    void selectWidgetInternal(QWidget *w, WidgetSelectionFlags flags);

    FormPrivate * const d;

    friend class FormWidget;
    friend class WidgetLibrary;
#ifdef KFD_SIGSLOTS
    friend class ConnectionDialog;
#endif
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Form::Features)
Q_DECLARE_OPERATORS_FOR_FLAGS(Form::WidgetSelectionFlags)

}

#endif
