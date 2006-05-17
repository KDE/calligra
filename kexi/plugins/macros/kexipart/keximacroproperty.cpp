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

#include "keximacroproperty.h"

#include <qlayout.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kdebug.h>

#define KEXIMACRO_PROPERTYEDITORTYPE 5682

/*
1. Used selected "specify value" item
   PropertyName: [specify value] <- combobox
              [line editor]   <- another line in property editor
2. User selected "X" variable
   PropertyName: [X] <- combobox
              [disabled line editor]   <- another line in property editor
Advantage: user can see his options all the time, without
blinking magical checkboxes of disappearing properties
*/

/*************************************************************
 * KexiMacroProperty
 */

KexiMacroProperty::KexiMacroProperty(KoProperty::Property* parent)
	: KoProperty::CustomProperty(parent)
{
}

KexiMacroProperty::~KexiMacroProperty()
{
}

void KexiMacroProperty::setValue(const QVariant &value, bool rememberOldValue)
{
	//Q_UNUSED(value);
	Q_UNUSED(rememberOldValue);
	kdDebug()<<"KexiMacroProperty::setValue value="<<value.toString()<<endl;
	//TODO
}

QVariant KexiMacroProperty::value() const
{
	//return m_property->parent()->value();
	kdDebug()<<"KexiMacroProperty::value"<<endl;
	return QVariant();
}

bool KexiMacroProperty::handleValue() const
{
	return false;
}

/*************************************************************
 * KexiMacroPropertyFactory
 */

KexiMacroPropertyFactory::KexiMacroPropertyFactory(QObject* parent)
	: KoProperty::CustomPropertyFactory(parent)
{
}

KexiMacroPropertyFactory::~KexiMacroPropertyFactory()
{
}

KoProperty::CustomProperty* KexiMacroPropertyFactory::createCustomProperty(KoProperty::Property* parent)
{
	kdDebug()<<"KexiMacroPropertyFactory::createCustomProperty parent="<<parent->name()<<endl;
	return new KexiMacroProperty(parent);
}

KoProperty::Widget* KexiMacroPropertyFactory::createCustomWidget(KoProperty::Property* property)
{
	kdDebug()<<"KexiMacroPropertyFactory::createCustomWidget property?"<<property->name()<<endl;
	return new KexiMacroPropertyWidget(property);
}

void KexiMacroPropertyFactory::initFactory()
{
	CustomPropertyFactory* factory = KoProperty::FactoryManager::self()->factoryForEditorType(KEXIMACRO_PROPERTYEDITORTYPE);
	if(! factory) {
		factory = new KexiMacroPropertyFactory( KoProperty::FactoryManager::self() );
		KoProperty::FactoryManager::self()->registerFactoryForEditor(KEXIMACRO_PROPERTYEDITORTYPE, factory);
	}
}

/*************************************************************
 * KexiMacroPropertyWidget
 */

/**
* @internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroPropertyWidget::Private
{
	public:

		KComboBox* combobox;
		//KoProperty::Widget* propertywidget;
};

KexiMacroPropertyWidget::KexiMacroPropertyWidget(KoProperty::Property* property, QWidget* parent)
	: KoProperty::Widget(property, parent)
	, d( new Private() )
{
	/*
	KexiMacroPropertyFactory* propertyfactory = KoProperty::FactoryManager::self()->factoryForEditorType(KEXIMACRO_PROPERTYEDITORTYPE);
	if(! propertyfactory) {
		propertyfactory = KexiMacroPropertyFactory( KoProperty::Factory::self() );
		KoProperty::FactoryManager::self()->registerFactoryForEditor(KEXIMACRO_PROPERTYEDITORTYPE, propertyfactory);
	}
	*/



	QHBoxLayout* layout = new QHBoxLayout(this, 0, 0);

	d->combobox = new KComboBox(this);
	d->combobox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	d->combobox->setMinimumHeight(5);
	layout->addWidget(d->combobox);

	d->combobox->setEditable(false);
	d->combobox->setInsertionPolicy(QComboBox::NoInsertion);
	d->combobox->setMinimumSize(10, 0); // to allow the combo to be resized to a small size
	d->combobox->setAutoCompletion(true);
	d->combobox->setContextMenuEnabled(false);

	d->combobox->insertItem(i18n("Custom..."), 0);

	setFocusWidget(d->combobox);
	connect(d->combobox, SIGNAL(activated(int)), this, SLOT(slotComboboxChanged(int)));
}

KexiMacroPropertyWidget::~KexiMacroPropertyWidget()
{
}

QVariant KexiMacroPropertyWidget::value() const
{
	//TODO
	return QVariant();
}

void KexiMacroPropertyWidget::setValue(const QVariant& value, bool emitChange)
{
	Q_UNUSED(value);
	Q_UNUSED(emitChange);
	//TODO
}

void KexiMacroPropertyWidget::setReadOnlyInternal(bool readOnly)
{
	Q_UNUSED(readOnly);
	//TODO
}

void KexiMacroPropertyWidget::slotComboboxChanged(int index)
{
	kdDebug()<<"KexiMacroPropertyWidget::slotComboboxChanged index="<<index<<endl;
	switch(index) {
		case 0: { // "Custom..."
			//d->propertywidget->setEnabled(true);
		} break;
		default: { // a variable got selected
			//d->propertywidget->setEnabled(false);
		} break;
	}
}

#include "keximacroproperty.moc"
