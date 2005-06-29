/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004  Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KPROPERTY_PROPERTYEDITOR_H
#define KPROPERTY_PROPERTYEDITOR_H

#include <qguardedptr.h>
#include <koffice_export.h>

#ifdef QT_ONLY
#include <qlistview.h>
#else
#include <klistview.h>
#define QListView KListView
#endif

class QSize;

namespace KOProperty {

class EditorPrivate;
class Property;
class Set;
class Widget;
class EditorItem;

//! A listview to edit properties
/*! Editor uses property options using Property::option(const char *)
    to override default behaviour of editor items.
    Currently supported options are:
    <ul><li> min: integer setting for minimum value of IntEdit and DoubleEdit item. Default is 0.
    Set "min" to -1, if you want this special value to be allowed.</li>
    <li> minValueText: i18n'd QString used in IntEdit to set "specialValueText"
            widget's property</li>
    <li> max: integer setting for minimum value of IntEdit item. Default is 0xffff.</li>
    <li> precision:  The number of decimals after the decimal point. (for DoubleEdit)</li>
    <li> step : the size of the step that is taken when the user hits the up
    or down buttons (for DoubleEdit) </li></ul>
   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
 */
class KOPROPERTY_EXPORT Editor : public QListView
{
    Q_OBJECT

    public:
        /*! Creates an empty Editor with \a parent as parent widget.
           If \a autoSync == true, properties values are automatically synced as
            soon as editor contents change (eg the user types text, etc.)
            and the values are written in the buffer. Otherwise, buffer
             is updated only when selected item changes or user presses Enter key.
             Each property can overwrite this if its autoSync() == 0 or 1.
        */
        Editor(QWidget *parent=0, bool autoSync=true, const char *name=0);
        ~Editor();

        virtual QSize sizeHint() const;
        virtual void setFocus();

    public slots:
        /*! Populates the editor with an item for each property in the List.
          Also creates child items for composed properties.
        */
        void  changeSet(Set *l, bool preservePrevSelection=false);
        /*! Clears all items in the list.
           if \a editorOnly is true, then only the current editor will be cleared,
            not the whole list.
        */
        void clear(bool editorOnly = false);
	/*! Accept the changes mae to the current editor (as if the user had pressed Enter key) */
	void  acceptInput();

    signals:
        void  propertySetChanged(KOProperty::Set *list);

    protected slots:
        /*! Updates property widget in the editor.*/
        void  slotPropertyChanged(KOProperty::Property* property, KOProperty::Set *list);
        void  slotPropertyReset(KOProperty::Property* property, KOProperty::Set *list);
        /*! Updates property in the list when new value is selected in the editor.*/
        void slotWidgetValueChanged(Widget *widget);
        /*! Called when the user presses Enter to accet the input
           (only applies when autoSync() == false).*/
        void slotWidgetAcceptInput(Widget *widget);
        /*! Called when the user presses Esc. Calls undo(). */
        void slotWidgetRejectInput(Widget *widget);

        /*! Called when current buffer is about to be destroyed. */
        void slotSetDeleted();
        /*! This slot is called when the user clicks the list view.
           It takes care of deleting current editor and
           creating a new editor for the newly selected item. */
        void  slotClicked(QListViewItem *item);
        /*! Undoes the last change in property editor.*/
        void  undo();

        void  updateEditorGeometry(bool forceUndoButtonSettings = false, bool undoButtonVisible = false);
        void  hideEditor();

        void  slotCollapsed(QListViewItem *item);
        void  slotExpanded(QListViewItem *item);
        void  slotColumnSizeChanged(int section);
        void  slotColumnSizeChanged(int section, int, int newS);
        void  slotCurrentChanged(QListViewItem *item);

    protected:
        /*! \return \ref Widget for given property.
        Uses cache to store created widgets.
        Cache will be cleared only with clearWidgetCache().*/
        Widget *createWidgetForProperty(Property *property, bool changeWidgetProperty=true);
        /*! Deletes cached machines.*/
        void clearWidgetCache();

        void  fill();
        void  changeSetLater();
        void addItem(const QCString &name, EditorItem *parent);

        void showUndoButton( bool show );

        virtual void resizeEvent(QResizeEvent *ev);
        bool handleKeyPress(QKeyEvent* ev);

    private:
        EditorPrivate *d;

    friend class EditorItem;
};

}

#endif
