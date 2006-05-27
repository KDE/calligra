/* This file is part of the KDE project
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIMACROPROPERTY_H
#define KEXIMACROPROPERTY_H

#include <ksharedptr.h>
#include <koproperty/property.h>
#include <koproperty/factory.h>
#include <koproperty/customproperty.h>
#include <koproperty/widget.h>

namespace KoMacro {
	class Variable;
	class MacroItem;
}

class KexiMacroPropertyWidget;

/**
* Implementation of a @a KoProperty::CustomProperty to have
* more control about the handling of our macro-properties.
*/
class KexiMacroProperty
	: public QObject
	, public KoProperty::CustomProperty
{
		Q_OBJECT

		friend class KexiMacroPropertyWidget;

	public:

		/** Constructor. */
		explicit KexiMacroProperty(KoProperty::Property* parent, KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name);
		/** Destructor. */
		virtual ~KexiMacroProperty();

		/** @return the parent @a KoProperty::Property instance. */
		KoProperty::Property* parentProperty() const;

		/** This function is called by @ref KoProperty::Property::setValue() 
		when a custom property is set.
		You don't have to modify the property value, it is done by Property class.
		You just have to update child or parent properties value (m_property->parent()->setValue()).
		Note that, when calling Property::setValue, you <b>need</b> to set
		useCustomProperty (3rd parameter) to false, or there will be infinite recursion. */
		virtual void setValue(const QVariant &value, bool rememberOldValue);

		/** This function is called by @ref KoProperty::Property::value()
		when a custom property is set and @ref handleValue() is true.
		You should return property's value, taken from parent's value.*/
		virtual QVariant value() const;

		/** Tells whether CustomProperty should be used to get the property's value.
		You should return true for child properties, and false for others. */
		virtual bool handleValue() const;

		/** \return the \a KoMacro::MacroItem this custom property has or
		NULL if there was no item provided. */
		KSharedPtr<KoMacro::MacroItem> macroItem() const;

		/** \return the name the property has in the \a KoMacro::MacroItem
		above. Is QString::null if there was no item provided. */
		QString name() const;

		/** \return the \a KoMacro::Variable which has the name @a name()
		in the item @a macroItem() . If such a variable doesn't exists NULL
		is returned. */
		KSharedPtr<KoMacro::Variable> variable() const;

		/** Factory function to create a new @a KoProperty::Property instance
		that will use a @a KexiMacroProperty as container. */
		static KoProperty::Property* createProperty(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name);

	signals:

		/** Emitted if @a setValue was called and the value changed. */
		void valueChanged();

	private:
		/** \internal d-pointer class. */
		class Private;
		/** \internal d-pointer instance. */
		Private* const d;

		inline void init();
};

/**
* Implementation of a @a KoProperty::CustomPropertyFactory to handle
* creation of @a KexiMacroProperty and @a KexiMacroPropertyWidget
* instances for our macro-properties.
*/
class KexiMacroPropertyFactory : public KoProperty::CustomPropertyFactory
{
	public:
		/** Constructor. */
		explicit KexiMacroPropertyFactory(QObject* parent);
		/** Destructor. */
		virtual ~KexiMacroPropertyFactory();

		/** @return a new instance of custom property for @p parent.
		 Implement this for property types you want to support.
		 Use parent->type() to get type of the property. */
		virtual KoProperty::CustomProperty* createCustomProperty(KoProperty::Property* parent);

		/** @return a new instance of custom property for @p property.
		 Implement this for property editor types you want to support.
		 Use parent->type() to get type of the property. */
		virtual KoProperty::Widget* createCustomWidget(KoProperty::Property* property);

		/** Initializes this factory. The factory may register itself at
		the @a KoProperty::FactoryManager if not alreadydone before. This
		function should be called from within the @a KexiMacroDesignView
		before the functionality provided with @a KexiMacroProperty and
		@a KexiMacroPropertyWidget got used. */
		static void initFactory();
};

/**
 * Implementation of a @a KoProperty::Widget used to display and
 * edit a @a KexiMacroProperty .
 */
class KexiMacroPropertyWidget : public KoProperty::Widget
{
		Q_OBJECT

	public:
		/** Constructor. */
		explicit KexiMacroPropertyWidget(KoProperty::Property* property, QWidget* parent = 0);
		/** Destructor. */
		virtual ~KexiMacroPropertyWidget();

		/** @return the value this widget has. */
		virtual QVariant value() const;

		/** Set the value @p value this widget has. If @p emitChange is true,
		the @p KoProperty::Widget::valueChanged signal will be emitted. */
		virtual void setValue(const QVariant& value, bool emitChange=true);

		//virtual void drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value);

	protected:

		/** Called if the value should be read only. */
		virtual void setReadOnlyInternal(bool readOnly);

	private slots:

		/** Called if the text in the KComboBox changed. */
		void slotComboBoxChanged();

		/** Called if an item in the QListBox of the KComboBox got activated. */
		void slotComboBoxActivated();

		/** Called if the @a KoProperty::Widget of the EditListBoxItem got changed. */
		void slotWidgetValueChanged();

		/** Called if the value of a @a KexiMacroProperty changed to update
		the widget and the displayed content. */
		void slotPropertyValueChanged();

	private:
		/** \internal d-pointer class. */
		class Private;
		/** \internal d-pointer instance. */
		Private* const d;
};

#endif

