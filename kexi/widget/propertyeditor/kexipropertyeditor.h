/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIPROPERTYEDITOR_H
#define KEXIPROPERTYEDITOR_H

#include <qvariant.h>
#include <qguardedptr.h>
#include <qdict.h>

#include <klistview.h>

#include "kexipropertyeditoritem.h"

class KexiProperty;
class KexiPropertyBuffer;
class KexiPropertySubEditor;
class KPushButton;

/** This class is a KListView which shows an item for each property in its KexiPropertyBuffer.
    When an item gets the focus, an editor (eg. a line edit, a spin box, etc.) is shown, which allows to modify property value.
    Pressing Enter key allow to validate the input (ie save it in the buffer), whereas Esc key undo previous input.
    Properties can be reset using "Revert to defaults" push button next to editor.
    Modified items names are written in bold.\n
    \n
    Example of usage of KexiPropertyEditor :
    \code
    // Create a Property Buffer
    m_buffer = new KexiPropertyBuffer(this);
    // Add properties into the buffer
    m_buffer->add( new KexiProperty("Name", "Name") );
    m_buffer->add( new KexiProperty("Int", 43) );
    
    // for a string list property
    QStringList list;
    list.append("MyItem");
    list.append("OtherItem");
    list.append("Item");
    m_buffer->add( new KexiProperty("list","Item" , list) );
    [...]
    
    KexiPropertyEditor *edit = new KexiPropertyEditor(this,false);
    edit->setBuffer(m_buffer);
    \endcode
    KexiPropertyEditor receives propery value changes from the assigned buffer,
    and automatically updates visually, if needed.
*/
//! A list view to edit any type of properties
class KEXIPROPERTYEDITOR_EXPORT KexiPropertyEditor : public KListView
{
	Q_OBJECT

	public:
		/*! Creates an empty KexiPropertyEditor with \a parent as parent widget. If \a autoSync == true,
		 properties values are automatically synced as soon as editor contents change 
		 (eg the user types text, etc.) and the values are written in the buffer. 
		 Otherwise, the buffer is updated only when another item is selected
		 or user presses Enter key. Each property can overwrite this if its autoSync() == 0 or 1.
		*/
		KexiPropertyEditor(QWidget *parent=0, bool autoSync=true, const char *name=0);
		virtual ~KexiPropertyEditor();

		/*! Reset the list, ie clears all items in the list.
		   if \a editorOnly is true, then only the current editor will be cleared, not the whole list.
		*/
		void reset(bool editorOnly = false);

		/*! Sets \a b as Property Editor's buffer. 
		 The list will be automatically filled with a list item for each KexiProperty
		 in the buffer. If \a preservePrevSelection is true and there was a buffer 
		 set before call, previously selected item will be preselected (if found).
		*/
		void setBuffer(KexiPropertyBuffer *b, bool preservePrevSelection = false);

		virtual QSize sizeHint() const;

		//! \return editor item named with \a name or null if such item not found
		KexiPropertyEditorItem* item(const QString& name) const { return m_items[name.ascii()]; }

		/*! @internal used by KexiPropertySubEditor and KexiPropertyEditor. */
		bool handleKeyPress( QKeyEvent* ev );

	signals:
		/*! This signal is emitted when a property value has changed, ie when the user presses Enter or when another item
		    gets the focus. \a propname is the name of the property and \a value is the new value of this property.
		*/
		void valueChanged(const QString &propname, QVariant value);

	public slots:
		/*! On focus:
		 - previously focused editor is activated
		 - first visible item is activated if no item was active
		*/
		virtual void setFocus();

	protected slots:
		/*! This slot resets the value of an item, using KexiProperty::oldValue().
		   It is called when pressing the "Revert to defaults" button
		*/
		void resetItem();

		/*! This slot updates the positions of current editor and revert button.
		   It is called when double-clicking list's header. 
		*/
		void moveEditor();

		/*! Fills the list with an item for each property in the buffer.
		   You shouldn't need to call this, as it is automatically called in setBuffer().
		*/
		void fill();

		/*! This slot is called when the user presses Enter key and when the selected item changes.
		    It takes care of saving editor value into buffer.
		*/
		void slotEditorAccept(KexiPropertySubEditor *editor);
		/*! This slot is called when the user press Esc key.
		    It undoes last input, setting item value as it was on editor's creation.
		    If autoSync == true, it sets item value to KexiProperty::oldValue().
		*/
		void slotEditorReject(KexiPropertySubEditor *editor);
		/*! This slot is called every time the editor contents change.
		   If AutoSync is true, the buffer is updated. Otherwise, does nothing.
		   This slot also takes care of syncing composed items (eg. QRect item with x, y, width and height chilren).
		*/
		void slotValueChanged(KexiPropertySubEditor *editor);

		/*! This slot updates editor and revert buttons position and size when the columns are resized. */
		void slotColumnSizeChanged(int section, int oldS, int newS);
		void slotColumnSizeChanged(int section);

		/*! This slot is called when the user clicks the list view. It takes care of deleting current editor and
		   creating a new editor for the newly selected item.
		*/
		void slotClicked(QListViewItem *item);

		/*! used to fix selection when unselectable item was activated.
		 This can occur when */
		void slotCurrentChanged(QListViewItem *);

		void slotExpanded(QListViewItem *item);
		void slotCollapsed(QListViewItem *item);

		/*! Receives signals on \a prop property change from buffer \a buf. */
		void slotPropertyChanged(KexiPropertyBuffer &buf,KexiProperty &prop);

		/*! Called when current buffer is about to be destroyed. */
		void slotBufferDestroying();

		void slotPropertyReset(KexiPropertyBuffer &buf,KexiProperty &prop);
		void setBufferLater();

	protected:
		/*! Creates an editor for the list item \a i in the rect \a geometry, and displays revert button 
		    if property is modified (ie KexiPropertyEditorItem::modified() == true).
		    The editor type depends on KexiProperty::type() of the item's property.
		*/
		void createEditor(KexiPropertyEditorItem *i);//, const QRect &geometry);
		/*! Reimplemented from KListView to update editor and revert button position. */
		void resizeEvent(QResizeEvent *ev);

		void showDefaultsButton( bool show );

		int baseRowHeight() const { return m_baseRowHeight; }

		QGuardedPtr<KexiPropertySubEditor> m_currentEditor;
		KexiPropertyEditorItem *m_editItem;
		KexiPropertyEditorItem *m_topItem; //The top item is used to control the drawing of every branches.
		QGuardedPtr<KexiPropertyBuffer> m_buffer;
		KPushButton *m_defaults; // "Revert to defaults" button
		KexiPropertyEditorItem::Dict m_items;
		int m_baseRowHeight;
		bool m_sync : 1;
		bool slotValueChanged_enabled : 1;
		bool insideSlotValueChanged : 1;
		//! Helpers for setBufferLater()
		bool setBufferLater_set : 1;
		bool preservePrevSelection_preservePrevSelection : 1;
		bool doNotSetFocusOnSelection : 1;
		//! Helper for setBuffer()
		KexiPropertyBuffer* setBufferLater_buffer;

	friend class KexiPropertyEditorItem;
	friend class KexiPropertySubEditor;
};

#endif
