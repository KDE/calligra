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

#ifndef KPROPERTY_CUSTOMPROPERTY_H
#define KPROPERTY_CUSTOMPROPERTY_H

#include <koffice_export.h>

class QVariant;

namespace KoProperty {

class Property;

//!  Base class for custom properties
/*! You will need to subclass CustomProperty to override the behaviour of a property type.\n
  In the constructor, you should create the child properties (if needed).
  Then, you need to implement the functions concerning values.\n

  Examples of custom properties implementation can be found in src/customproperty.cpp.

   \author Cedric Pasteur <cedric.pasteur@free.fr>
*/
class KOPROPERTY_EXPORT CustomProperty
{
	public:
		CustomProperty(Property *parent);
		virtual ~CustomProperty() {}

		/*! This function is called by \ref Property::setValue() when
		a custom property is set.
		You don't have to modify the property value, it is done by Property class.
		You just have to update child or parent properties value, using \ref Property::setValue().
		Please notice that, when calling Property::setValue, you <b>need</b> to set
		useCustomProperty (3rd parameter) to false, or there will be infinite recursion. */
		virtual void setValue(const QVariant &value, bool rememberOldValue) = 0;

		/*! This function is called by \ref Property::value() when
		a custom property is set and \ref handleValue() is true.
		You should return property's value, taken from parent's value.*/
		virtual QVariant value() const = 0;

		/*! Tells whether CustomProperty should be use to get the property's value.
		You should return true for child properties, and false for others. */
		virtual bool handleValue() const { return false; }

	protected:
		Property  *m_property;
};

class KOPROPERTY_EXPORT SizeCustomProperty : public CustomProperty
{
	public:
		SizeCustomProperty(Property *parent);
		~SizeCustomProperty();

		void setValue(const QVariant &value, bool rememberOldValue);
		QVariant value() const;
		bool handleValue() const;
};

class KOPROPERTY_EXPORT PointCustomProperty : public CustomProperty
{
	public:
		PointCustomProperty(Property *parent);
		~PointCustomProperty();

		void setValue(const QVariant &value, bool rememberOldValue);
		QVariant value() const;
		bool handleValue() const;
};

class KOPROPERTY_EXPORT RectCustomProperty : public CustomProperty
{
	public:
		RectCustomProperty(Property *parent);
		~RectCustomProperty();

		void setValue(const QVariant &value, bool rememberOldValue);
		QVariant value() const;
		bool handleValue() const;
};

class KOPROPERTY_EXPORT SizePolicyCustomProperty : public CustomProperty
{
	public:
		SizePolicyCustomProperty(Property *parent);
		~SizePolicyCustomProperty();

		void setValue(const QVariant &value, bool rememberOldValue);
		QVariant value() const;
		bool handleValue() const;
};

}

#endif
