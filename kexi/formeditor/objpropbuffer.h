/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
class KCommand;

namespace KFormDesigner {

class Form;
class FormManager;
class Container;
class PropertyCommand;
class GeometryPropertyCommand;
class ObjectTreeItem;

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

		/*! This function is called before changing widget. It stores all the modified properties into ObjectTreeItem so
		  they are saved later (using ObjectTreeItem::addMofProperty() ).
		 */
		void    checkModifiedProp();

		//! Adds the i18n'ed description of a property (to allow factories to use new properties).
		void addPropertyDescription(const char *property, const QString &desc);
		//! Adds the i18n'ed description of a value (to allow factories to use new properties).
		void addValueDescription(const char *value, const QString &desc);

		FormManager*    manager()  { return m_manager; }

	public slots:
		void    setSelectedWidget(QWidget *w, bool add=false);
		/*!  This function is called every time a property is modifed.  It takes care of saving set and enum
		 properties.
		*/
		void	slotChangeProperty(KexiPropertyBuffer &buff, KexiProperty &prop);
		/*! This slot is called when a property is reset using the "reload" button in PropertyEditor. */
		void    slotResetProperty(KexiPropertyBuffer &buff, KexiProperty &prop);
		//! This slot is called when the watched widget is destroyed. Resets the buffer.
		void    widgetDestroyed();
		void    storePixmapName(KexiPropertyBuffer &, KexiProperty &);
		void    slotPropertyExecuted(KexiPropertyBuffer &, KexiProperty &, const QString&); // TMP

	signals:
		/*! This signal is emitted when the name of the widget is modified. \a oldname is the name of the widget before the
		  change, \a newname is the name after.
		 */
		void	nameChanged(const QString &oldname, const QString &newname);
		/*! This signal is emitted when a property was changed ( with changeProperty() ). \a obj is the widget concerned, \a property
		  is the name of the modified property, and \a v is the new value of this property.
		 */
		void	propertyChanged(QWidget *widg, const QString &property, const QVariant &v);

	protected:
		/*! Sets the buffer's object to \a widget. The properties are created automatically, and the list view is updated.
		  checkModifiedProp() is called before doing this.
		 */
		void	setWidget(QWidget *obj);
		/*! Adds the QWidget \a w to the list of widgets managed by this buffer. The properties shown are filtered
		  to show only properties common to all the widgets.
		 */
		void    addWidget(QWidget *w);
		/*! Resets the buffer, ie empty it and reset all members. */
		void    resetBuffer();


		/*! This function filters the event of the sselected widget to automatically updates the "geometry" property
		  when the widget is moved or resized in the Form.
		 */
		bool    eventFilter(QObject *o, QEvent *ev);

		/*! This function is used to filter the properties to be shown (ie not show "caption" if the widget isn't toplevel).
		   \return true if the property should be shown. False otherwise.
		 */
		bool    showProperty(const QString &property, bool isTopLevel, const QString &classname=QString::null);
		/*! Creates the properties related to alignment (ie hAlign, vAlign and WordBreak) for the QWidget \a obj. \a meta
		  is the QMetaProperty for "alignment" property". ( called by setObject() )
		 */
		void    createAlignProperty(const QMetaProperty *meta, QWidget *obj);
		/*! Saves the properties related to alignment (ie hAlign, vAlign and WordBreak) and modifies the "alignment" property of
		  the widget. ( called by changeProperty() )
		 */
		void    saveAlignProperty(const QString &property);

		/*! Creates the "layout" property, for the Container \a container. ( called by setObject() ) */
		void    createLayoutProperty(Container *container);
		/*! Saves the "layout" property and changes the Container 's layout (using Container::setLayout() ). ( called by changeProperty() )*/
		void    saveLayoutProperty(const QString &property, const QVariant &value);

		/*! \return The i18n'ed name of the property whose name is \a name, that will be displayed in PropertyEditor. */
		QString      descFromName(const QString &name);
		/*! \return The i18n'ed name of the property's vale whose name is \a name. */
		QString      descFromValue(const QString &name);
		/*! \return The i18n'ed list of values, that will be shown by Property Editor (using descFromValue()).*/
		QStringList  descList(const QStringList &list);

		void   updateOldValue(ObjectTreeItem *tree, const char *property);

	private:
		//QGuardedPtr<QWidget> m_widget;
		QStringList	m_properties;
		QPtrList<QWidget> m_widgets;
		bool		m_multiple;
		FormManager     *m_manager;

		PropertyCommand	*m_lastcom;
		GeometryPropertyCommand *m_lastgeocom;
		bool		m_undoing;

		QColorGroup* m_origActiveColors;

		// i18n stuff
		QMap<QString, QString> propDesc;
		QMap<QString, QString> valueDesc;

		friend class PropertyCommand;
		friend class LayoutPropertyCommand;
		friend class GeometryPropertyCommand;
};

}

#endif

