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

#ifndef OBJPROPBUFFER_H
#define OBJPROPBUFFER_H

#include "kexipropertybuffer.h"

class KexiPropertyEditor;

namespace KFormDesigner {

class Form;
class FormManager;

//! A buffer which holds the property of the selected widget
/*! This class inherits KexiPropertyBuffer and holds the properties of the selected widget, which are shown in
  KexiPropertyEditor. It takes care of reading and writing the properties of the widget, and you just need to call 
   setObject() and properties are created.\n
   It includes property filtering (eg "caption" only for a toplevel widget), and is synced with widget resizig or moving.
 */
class KFORMEDITOR_EXPORT ObjectPropertyBuffer : public KexiPropertyBuffer
{
	Q_OBJECT

	public:
		//! Creates a ObjectPropertyBuffer linked to the FormManager \a manager.
		ObjectPropertyBuffer(FormManager *manager, QObject *parent, const char *name=0);
		~ObjectPropertyBuffer();

		/*! Reimplemented from KexiPropertyBuffer to save properties. This function is called by
		 KexiPropertyEditor every time a property is modifed in the list. It takes care of saving set and enum 
		 properties.
		*/
		void	changeProperty(const QString &property, const QVariant &value);

		/*! This function filters the event of the sselected widget to automatically updates the "geometry" property
		  when the widget is moved or resized in the Form.
		 */
		bool    eventFilter(QObject *o, QEvent *ev);
		/*! This function is called before changing widget. It stores all the modified properties into ObjectTreeItem so
		  they are saved later (using ObjectTreeItem::addMofProperty() ).
		 */
		void    checkModifiedProp();
	
	public slots:
		/*! Sets the buffer's object to \a widget. The properties are created automatically, and the list view is updated.
		  checkModifiedProp() is called before doing this.
		 */
		void	setObject(QWidget *obj);

	signals:
		/*! This signal is emitted when the name of the widget is modified. \a oldname is the name of the widget before the 
		  change, \a newname is the name after.
		 */
		void	nameChanged(const QString &oldname, const QString &newname);
		/*! This signal is emitted when a property was changed ( with changeProperty() ). \a obj is the widget concerned, \a property
		  is the name of the modified property, and \a v is the new value of this property.
		 */
		void	propertyChanged(QObject *obj, const QString &property, const QVariant &v);

	protected:
		/*! This function is used to filter the properties to be shown (ie not show "caption" if the widget isn't toplevel).
		   \return true if the property should be shown. False otherwise.
		 */
		bool    showProperty(QObject *obj, const QString &property);
		void    createAlignProperty(const QMetaProperty *meta, QObject *obj);
		void    saveAlignProperty();
	
	private:
		QObject		*m_object;
		FormManager     *m_manager;
};

}

#endif

