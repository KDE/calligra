/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>

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

#ifndef KPROPERTY_FACTORY_H
#define KPROPERTY_FACTORY_H

#include "koproperty_global.h"
#include <kstaticdeleter.h> 
#include <qobject.h> 

template<class U> class QValueList;

namespace KoProperty {

class Widget;
class CustomProperty;
class Property;
class FactoryPrivate;

///*! A pointer to factory function which creates and returns widget for a given property type.*/
//typedef Widget *(*createWidget) (Property*);
//typedef CustomProperty *(*createCustomProperty) (Property*);

class KOPROPERTY_EXPORT CustomPropertyFactory : public QObject
{
	public:
		CustomPropertyFactory(QObject *parent);
		virtual ~CustomPropertyFactory();

		virtual CustomProperty* createCustomProperty(Property *parent) = 0;
		virtual Widget* createCustomWidget(Property *prop) = 0;
};

//! Factory class which loads custom editors and properties
/*! This class is static, you don't need to create an instance of it. It's used to enable the
     custom property/editors system.
     You may want to create your own property types and/or editors to:
     <ul> <li>Create your own editors for some special kind of properties, not included in
     KProperty basic editors; </li>
               <li>Create composed properties, which contain more than one value. Child
     items will then be created in the Editor (that's how rect, size properties are created). </li></ul>

     \section custom_prop Using Custom Properties
     To create a custom property, create a subclass of \ref CustomProperty class. You need to implement
     some virtuals functions, to customise the behaviour of your property (see \ref CustomProperty api doc).
     If you create a composed property, both parent and children properties must have custom (different) types. \n
     Then, you need to register the new created type, using \ref registerCustomProperty(). The second
     parameter is a function which returns a CustomProperty subclass.\n
     To create a property of this type, just use the normal constructor, overriding the type parameter with
     the type you registered.

     \section custom_editor Using Custom Editors
     First, create a subclass of Widget, and implements all the virtuals you need to tweak
     the editor. You can find examples of editors in the src/editors/ dir.\n
     Then, register it using \ref registerEditor() , as for properties (see test/ dir for an example of custom editor).
     You can also register a new editor for a basic type, if the basic editor doesn't fit your needs
     (if you have created a better editor, send us the code, and it may get included in KProperty lib).\n
     Every time a property of this type is created, the function you registered will be called to create the editor.

   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
 */
class KOPROPERTY_EXPORT Factory : public QObject
{
	public:
		/*! Registers a custom property factory \a creator for handling property editor for \a type.
		This custom factory will be used before defaults when widgetForProperty() is called. 
		\a creator is not owned by this Factory object, but it's good idea 
		to instantiate CustomPropertyFactory object itself as a child of Factory parent. For example:
		\code
			MyCustomPropertyFactory *f = new MyCustomPropertyFactory(KoProperty::Factory::self());
			KoProperty::Factory::self()->registerEditor( MyCustomType, f );
		\endcode */
		void registerEditor(int type, CustomPropertyFactory *creator);

		/*! Registers custom property factory \a creator for handling property editor for \a types.
		 @see registerEditor(). */
		void registerEditors(const QValueList<int> &types, CustomPropertyFactory *creator);

		/*! Creates and returns the editor for given property type.
		Warning: editor and viewer widgets won't have parent widget. Property editor
		cares about reparenting and deletion of returned widgets in machines.
		If \a createWidget is false, just create child properties, not widget.*/
		Widget* widgetForProperty(Property *property);

		/*! Registers a custom property factory that handles a CustomProperty of a type \a type.
		 This function will be called every time a property of \a type is created. */
		void registerCustomProperty(int type, CustomPropertyFactory *creator);

		/*! Registers a custom property factory that handles a CustomProperty for \a types.
		 @see registerCustomProperty() */
		void registerCustomProperties(const QValueList<int> &types, CustomPropertyFactory *creator);

		/*! This function is called in Property::Property() to create (optional)
		  custom property. It creates the custom property for built-in types, or
		  calls one of createCustomProperty function previously registered for other types. */
		CustomProperty* customPropertyForProperty(Property *parent);

		/*! \return a pointer to a property factory instance.*/
		static Factory* self();

	private:
		Factory();
		~Factory();

		FactoryPrivate *d;
		friend class KStaticDeleter;
};

}

#endif

