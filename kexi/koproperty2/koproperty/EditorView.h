/* This file is part of the KDE project
   Copyright (C) 2008-2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_EDITORVIEW_H
#define KPROPERTY_EDITORVIEW_H

#include "koproperty_export.h"

#include <QtGui/QItemEditorFactory>
#include <QtGui/QTreeView>
#include <QtGui/QLabel>

#include <KLineEdit>

namespace KoProperty
{

class Property;
class Set;

//! @brief A widget for editing properties
//! @todo review this .............
/*! Editor widgets use property options using Property::option(const char *)
    to override default behaviour of editor items.
    Currently supported options are:
    <ul><li> min: integer setting for minimum value for integer and double types. 
             The default is 0.
             Set it to -1, if you want this special value to be allowed.</li>
    <li> minValueText: i18n'd QString used for integer type to set "specialValueText"
         widget's property</li>
    <li> max: integer setting for minimum value the property of integer type. Default is 0xffff.</li>
    <li> precision:  The number of decimals after the decimal point (for double types).</li>
    <li> step: the size of the step that is taken when the user hits the up
    or down buttons (for double types).</li>
    <li> 3State: boolean value used for boolean types; if true, the editor becomes a combobox 
         (instead of checkable button) and accepta the third "null" state.</li>
    <li> yesName: i18n'd QString used for boolean types (both 2- and 3-state)
         to display the "true" value. If not present, i18n("Yes") is used.</li>
    <li> noName: i18n'd QString used for boolean types (both 2- and 3-state)
         to display the "false" value. If not present, i18n("No") is used.</li>
    <li> 3rdStateName: i18n'd QString used for boolean types (both 2- and 3-state)
         to display the 3rd state "null" value. If not present, i18n("N") is used.</li>
    <li> nullName: i18n'd QString used for boolean types to display the "null" value if
         and only if the property accepts two states (i.e. when "3State" option is not true).
         If this option is not present, null values are displayed as false.</li>
    <li> extraValueAllowed: Allow the user to manually enter a value into a combobox
         that is not in the list. The entered text will be returned as opposed to a matching key.
    </li>
    </ul>
 */
class KOPROPERTY_EXPORT EditorView : public QTreeView
{
    Q_OBJECT
public:
    /*! Creates an empty property editor with @a parent as parent widget. */
    EditorView(QWidget *parent = 0);

    ~EditorView();

    //! Options for changeSet().
    enum SetOption {
        NoOptions = 0,
        PreservePreviousSelection = 1, //!< If used, previously selected editor item
                                       //!< will be kept selected.
        AlphabeticalOrder = 2,         //!< Alphabetical order of properties (the default is insert-order)
        ExpandChildItems = 4           //!< Child property items are expanded (the default is "collapsed")
    };
    Q_DECLARE_FLAGS(SetOptions, SetOption)

    //! @return grid line color, defaultGridLineColor() by default
    QColor gridLineColor() const;

    //! @return default grid line color - Qt::gray
    static QColor defaultGridLineColor() { return Qt::gray; }

public slots:
    /*! Populates the editor view with items for each property from the @ set set.
     Child items for composed properties are also created.
     See SetOption documentation for description of @a options options.
     If @a preservePreviousSelection is true, previously selected editor
     item will be kept selected, if present. */
    void changeSet(Set *set, SetOptions options = NoOptions);

    /*! Populates the editor view with items for each property from the @ set set.
     Child items for composed properties are also created.
     If @a propertyToSelect is provided, item for this property name
     will be selected, if present. */
    void changeSet(Set *set, const QByteArray& propertyToSelect, SetOptions options = NoOptions);

    /*! If @a enable is true (the default), property values are automatically synced as
    soon as editor contents change (e.g. every time the user types a character)
    and the values are written back to the assigned property set. 
    If @a enable is false, property set is updated only when selection within
    the property editor or user presses Enter/Return key.
    Each property can overwrite this setting by changing its own autoSync flag.
    */
    void setAutoSync(bool enable);

    /*! @return value of autoSync flag. */
    bool isAutoSync() const;

    /*! Accepts the changes made to the current editor item (if any)
     (as if the user had pressed Enter key). */
    void acceptInput();

    //! Sets grid line color
    void setGridLineColor(const QColor& color);

signals:
    /*! Emitted when current property set has been changed. May be 0. */
    void propertySetChanged(KoProperty::Set *set);

protected:
    virtual bool viewportEvent( QEvent * event );

protected slots:
    virtual void currentChanged( const QModelIndex & current, const QModelIndex & previous );
    virtual void commitData( QWidget * editor );

    /*! Called when current property set is about to be destroyed. */
    void slotSetWillBeDeleted();

    /*! Updates editor widget in the editor.*/
    void slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property);

    void slotPropertyReset(KoProperty::Set& set, KoProperty::Property& property);

private:
    /*! Used by changeSet(). */
    void changeSetInternal(Set *set, SetOptions options, const QByteArray& propertyToSelect);
    virtual bool edit( const QModelIndex & index, EditTrigger trigger, QEvent * event );
    virtual void drawBranches( QPainter * painter, const QRect & rect, const QModelIndex & index ) const;
    virtual void mousePressEvent( QMouseEvent * event );

    //! @return true if @a x is within the area of the revert button for @a index index.
    bool withinRevertButtonArea( int x, const QModelIndex& index ) const;

    //! @return area of revert button, if it is displayed for @a index index.
    //! Otherwise invalid QRect is returned.
    QRect revertButtonArea( const QModelIndex& index ) const;

    /*! Undoes the last change in the property editor.*/
    void undo();

    class Private;
    Private * const d;
};

#if 0
class EditorPrivate;
class Property;
class Set;
class Widget;
class EditorItem;

//! \brief A listview to edit properties
/*! Editor widgets use property options using Property::option(const char *)
    to override default behaviour of editor items.
    Currently supported options are:
    <ul><li> min: integer setting for minimum value of IntEdit and DoubleEdit item. Default is 0.
    Set "min" to -1, if you want this special value to be allowed.</li>
    <li> minValueText: i18n'd QString used in IntEdit to set "specialValueText"
            widget's property</li>
    <li> max: integer setting for minimum value of IntEdit item. Default is 0xffff.</li>
    <li> precision:  The number of decimals after the decimal point. (for DoubleEdit)</li>
    <li> step : the size of the step that is taken when the user hits the up
    or down buttons (for DoubleEdit) </li>
    <li> 3rdState: i18n'd QString used in BoolEdit. If not empty, the the editor's button
     accept third "null" state with name equal to this string. When this value is selected,
     Widget::value() returns null QVariant. This option is used for example in the "defaultValue"
     property for a field of type boolean (in Kexi Table Designer). Third, "null" value
     of the property means there is no "defaultValue" specified. </li>
    <li>
     extraValueAllowed: Allow the user to manually enter a value into a combobox
     that isnt in the list.  The entered text will be returned as opposed to a matching key.
    </li>
    </ul>
   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
   \author Jarosław Staniek <staniek@kde.org>
 */
class KOPROPERTY_EXPORT Editor : public K3ListView
{
    Q_OBJECT

public:
//PORTED
    /*! Creates an empty Editor with \a parent as parent widget.
    If \a autoSync == true, properties values are automatically synced as
    soon as editor contents change (eg the user types text, etc.)
    and the values are written in the property set. Otherwise, property set
    is updated only when selected item changes or user presses Enter key.
    Each property can overwrite this if its autoSync() == 0 or 1.
    */
    Editor(QWidget *parent = 0, bool autoSync = true, const char *name = 0);

    virtual ~Editor();

    virtual QSize sizeHint() const;
    virtual void setFocus();
    virtual void setSorting(int column, bool ascending = true);

public slots:
//PORTED
    /*! Populates the editor with an item for each property in the List.
      Also creates child items for composed properties.
     If \a preservePrevSelection is true, previously selected editor
     item will be kept selected, if present. */
    void changeSet(Set *set, bool preservePrevSelection = false);

//PORTED
    /*! Populates the editor with an item for each property in the List.
      Also creates child items for composed properties.
     If \a propertyToSelect is not empty, editor item for this property name
     will be selected, if present. */
    void changeSet(Set *set, const QByteArray& propertyToSelect);

//UNUSED
    /*! Clears all items in the list.
       if \a editorOnly is true, then only the current editor will be cleared,
      not the whole list.
    */
    void clear(bool editorOnly = false);

//PORTED
    /*! Accept the changes mae to the current editor (as if the user had pressed Enter key) */
    void acceptInput();

signals:
//PORTED
    /*! Emitted when current property set has been changed. May be 0. */
    void propertySetChanged(KoProperty::Set *set);

protected slots:
    /*! Updates property widget in the editor.*/
    void slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property);

    void slotPropertyReset(KoProperty::Set& set, KoProperty::Property& property);

    /*! Updates property in the list when new value is selected in the editor.*/
    void slotWidgetValueChanged(Widget* widget);

    /*! Called when the user presses Enter to accet the input
       (only applies when autoSync() == false).*/
    void slotWidgetAcceptInput(Widget *widget);

    /*! Called when the user presses Esc. Calls undo(). */
    void slotWidgetRejectInput(Widget *widget);

    /*! Called when current property set is about to be cleared. */
    void slotSetWillBeCleared();

    /*! Called when current property set is about to be destroyed. */
    void slotSetWillBeDeleted();

    /*! This slot is called when the user clicks the list view.
       It takes care of deleting current editor and
       creating a new editor for the newly selected item. */
    void slotClicked(Q3ListViewItem *item);

//ported
    /*! Undoes the last change in property editor.*/
    void undo();

    void updateEditorGeometry(bool forceUndoButtonSettings = false, bool undoButtonVisible = false);
    void updateEditorGeometry(EditorItem *item, Widget* widget, bool forceUndoButtonSettings = false, bool undoButtonVisible = false);
    void updateGroupLabelsPosition();

    void hideEditor();

    void slotCollapsed(Q3ListViewItem *item);
    void slotExpanded(Q3ListViewItem *item);
    void slotColumnSizeChanged(int section);
    void slotColumnSizeChanged(int section, int oldSize, int newSize);
    void slotCurrentChanged(Q3ListViewItem *item);
    void changeSetLater();
    void selectItemLater();
protected:
    /*! \return \ref Widget for given property.
    Uses cache to store created widgets.
    Cache will be cleared only with clearWidgetCache().*/
    Widget *createWidgetForProperty(Property *property, bool changeWidgetProperty = true);

    /*! Deletes cached machines.*/
    void clearWidgetCache();

    void fill();
    void addItem(const QByteArray &name, EditorItem *parent);

    void showUndoButton(bool show);

    virtual void resizeEvent(QResizeEvent *ev);
    virtual bool eventFilter(QObject * watched, QEvent * e);
    bool handleKeyPress(QKeyEvent* ev);

    virtual bool event(QEvent * e);
    void updateFont();

    virtual void contentsMousePressEvent(QMouseEvent * e);
//PORTED
    /*! Used for changeSet(). */
    void changeSetInternal(Set *set, bool preservePrevSelection,
                           const QByteArray& propertyToSelect);

private:
    EditorPrivate * const d;

    friend class EditorItem;
    friend class Widget;
};
#endif

}
#endif
