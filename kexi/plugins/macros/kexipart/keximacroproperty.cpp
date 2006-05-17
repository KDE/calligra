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
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpainter.h>

#include <kcombobox.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "../lib/variable.h"
#include "../lib/macroitem.h"

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

/**
* @internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroProperty::Private
{
	public:
		/** The @a KoMacro::MacroItem the custom property uses
		internal. Together with the name we are able to identify
		the used variable at runtime. */
		KSharedPtr<KoMacro::MacroItem> macroitem;
		/** The name the variable @a KoMacro::Variable is known
		as in the @a KoMacro::MacroItem defined above. */
		QString name;
};

KexiMacroProperty::KexiMacroProperty(KoProperty::Property* parent)
	: KoProperty::CustomProperty(parent)
	, d( new Private() )
{
	//new KoProperty::Property("width", "test", i18n("Width"), i18n("Width"), KoProperty::String, parent);
}

KexiMacroProperty::~KexiMacroProperty()
{
	delete d;
}

KoProperty::Property* KexiMacroProperty::parentProperty() const
{
	return m_property;
}

void KexiMacroProperty::setValue(const QVariant &value, bool rememberOldValue)
{
	Q_UNUSED(rememberOldValue);
	kdDebug()<<"KexiMacroProperty::setValue name="<<d->name<<" value="<<value<<endl;
	d->macroitem->setVariable(d->name, value);
	emit valueChanged();
}

QVariant KexiMacroProperty::value() const
{
	KoMacro::Variable::Ptr variable = d->macroitem->variable(d->name, true);
	Q_ASSERT( variable.data() != 0 );
	return variable.data() ? variable->variant() : QVariant();
}

bool KexiMacroProperty::handleValue() const
{
	return true;
}

KSharedPtr<KoMacro::MacroItem> KexiMacroProperty::macroItem() const
{
	return d->macroitem;
}


QString KexiMacroProperty::name() const
{
	return d->name;
}

KSharedPtr<KoMacro::Variable> KexiMacroProperty::variable() const
{
	return d->macroitem->variable(d->name, true);
}

bool KexiMacroProperty::set(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name)
{
	Q_ASSERT( macroitem != 0 );
	kdDebug() << "KexiMacroProperty::set() macroitem=" << macroitem->name() << " name=" << name << endl;

	d->name = QString::null;
	d->macroitem = KSharedPtr<KoMacro::MacroItem>(0);

	KoMacro::Action::Ptr action = macroitem->action();
	KoMacro::Variable::Ptr actionvariable = action->variable(name);
	if(! actionvariable.data()) {
		kdDebug() << "KexiMacroProperty::createProperty() Skipped cause there exists no such action=" << name << endl;
		return false;
	}

	KoMacro::Variable::Ptr variable = macroitem->variable(name, true);
	if(! variable.data()) {
		kdDebug() << "KexiMacroProperty::createProperty() Skipped cause there exists no such variable=" << name << endl;
		return false;
	}

	d->name = name;
	d->macroitem = macroitem;

	Q_ASSERT(! name.isNull());
	m_property->setName( name.latin1() );
	m_property->setCaption( actionvariable->text() );
	m_property->setDescription( action->comment() );
	m_property->setValue( variable->variant() );
	m_property->setType( KEXIMACRO_PROPERTYEDITORTYPE ); // use our own propertytype

	return true;
}

KoProperty::Property* KexiMacroProperty::createProperty(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name)
{
	KoProperty::Property* property = new KoProperty::Property();
	KexiMacroProperty* customproperty = new KexiMacroProperty(property);
	if(! customproperty->set(macroitem, name)) {
		delete customproperty; customproperty = 0;
		delete property; property = 0;
		return 0;
	}
	property->setCustomProperty(customproperty);
	return property;
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
	kdDebug()<<"====================> KexiMacroPropertyFactory::createCustomProperty parent="<<parent->name()<<endl;

	KoProperty::CustomProperty* customproperty = parent->customProperty();
	KexiMacroProperty* parentcustomproperty = dynamic_cast<KexiMacroProperty*>(customproperty);
	if(! parentcustomproperty) {
		kdWarning() << "KexiMacroPropertyFactory::createCustomProperty() parent=" << parent->name() << " has an invalid customproperty." << endl;
		return 0;
	}

	KoMacro::MacroItem::Ptr macroitem = parentcustomproperty->macroItem();
	Q_ASSERT( macroitem.data() != 0 );
	const QString name = parentcustomproperty->name();
	Q_ASSERT(! name.isEmpty());

	KexiMacroProperty* macroproperty = new KexiMacroProperty(parent);
	if(! macroproperty->set(macroitem, name)) {
		delete macroproperty; macroproperty = 0;
		return 0;
	}

	return macroproperty;
}

KoProperty::Widget* KexiMacroPropertyFactory::createCustomWidget(KoProperty::Property* property)
{
	kdDebug()<<"====================> KexiMacroPropertyFactory::createCustomWidget property="<<property->name()<<endl;
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
* @internal implementation of a QListBoxItem to display the items of the
* combobox used within @a KexiMacroPropertyWidget to handle different
* variable-states at the @a ListBox listbox.
*/
class ListBoxItem : public QListBoxItem
{
	public:
		ListBoxItem(QListBox* listbox, KexiMacroProperty* macroproperty)
			: QListBoxItem(listbox)
			, m_macroproperty(macroproperty)
			, m_prop(0)
			, m_widget(0)
		{
			init();
		}

		virtual ~ListBoxItem() {
			//m_widget->hide();
			delete m_widget;
			delete m_prop;
		}

		virtual QString text() const {
			Q_ASSERT(m_variable);
			Q_ASSERT(m_variable->toString() != QString::null);
			return m_variable->toString();
		}

		KoProperty::Widget* widget() const { return m_widget; }
		KoMacro::Variable::Ptr variable() const { return m_variable; }
		KoMacro::Action::Ptr action() const { return m_action; }

	private:
		void init() {
			KoMacro::MacroItem::Ptr macroitem = m_macroproperty->macroItem();
			if(! macroitem.data()) {
				kdWarning() << "ListBoxItem::ListBoxItem() Skipped cause there exists no macroitem!" << endl;
				return;
			}
			m_action = m_macroproperty->macroItem()->action();
			if(! m_action.data()) {
				kdWarning() << "ListBoxItem::ListBoxItem() Skipped cause there exists no action for macroproperty=" << m_macroproperty->name() << endl;
				return;
			}
			KoProperty::Property* parentproperty = m_macroproperty->parentProperty();
			if(! parentproperty) {
				kdWarning() << "ListBoxItem::ListBoxItem() No parentproperty defined" << endl;
				return;
			}
			m_variable = m_macroproperty->variable();
			if(! m_variable.data()) {
				kdWarning() << "ListBoxItem::ListBoxItem() No variable defined for property=" << parentproperty->name() << endl;
				return;
			}

			QVariant variant = m_variable->variant();

			KoMacro::Variable::Ptr actionvariable = m_action->variable(m_macroproperty->name());
			if(actionvariable.data()) {
				QVariant actionvariant = actionvariable->variant();
				Q_ASSERT( ! actionvariant.isNull() );
				Q_ASSERT( variant.canCast( actionvariant.type()) );
				variant.cast( actionvariant.type() ); //preserve type. FIXME: move to lib!
			}

			int type = KoProperty::Auto;
			switch(variant.type()) {
				case QVariant::UInt:
				case QVariant::Int: {
					type = KoProperty::Integer;
				} break;
				case QVariant::CString:
				case QVariant::String: {
					type = KoProperty::String;
				} break;
				default: {
					kdWarning() << "ListBoxItem::ListBoxItem() name=" << m_variable->name() << " type=" << QVariant::typeToName(variant.type()) << endl;
				} break;
			}

			Q_ASSERT(! m_variable->name().isNull());
			m_prop = new KoProperty::Property(
				m_variable->name().latin1(), // name
				variant, // value
				m_variable->text(), // caption
				QString::null, // description
				type, // type
				0 //parentproperty // parent
			);

			m_widget = KoProperty::FactoryManager::self()->createWidgetForProperty(m_prop);
			Q_ASSERT( m_widget != 0 );
			//m_widget->reparent(listBox()->viewport(), 0, QPoint(0,0));
			m_widget->reparent(listBox(), 0, QPoint(1,1));
			//layout->addWidget(m_widget, 1);
			m_widget->setMinimumHeight(5);
			m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		}

	protected:
		virtual int width(const QListBox* lb) const {
			return lb->viewport()->width();
		}
		virtual int height(const QListBox* lb) const {
			return static_cast<QWidget*>(lb->parent())->height();
		}
		virtual void paint(QPainter* p) {
			if(! m_widget) return;
			Q_ASSERT( dynamic_cast<KComboBox*>( listBox()->parent() ) );
			int w = static_cast<KComboBox*>( listBox()->parent() )->lineEdit()->width();
			int h = static_cast<QWidget*>(listBox()->parent())->height();
			m_widget->setFixedSize(w - 1, h - 1);
			p->drawPixmap(1, 1, QPixmap::grabWidget(m_widget), 0, 0, w, h);
		}

	private:
		KexiMacroProperty* m_macroproperty;
		KoProperty::Property* m_prop;
		KoProperty::Widget* m_widget;
		KoMacro::Variable::Ptr m_variable;
		KoMacro::Action::Ptr m_action;
};

/**
* @internal implementation of a @a QListBox for the combobox used within
* @a KexiMacroPropertyWidget to handle different variable-states.
*/
class ListBox : public QListBox
{
	public:
		ListBox(KComboBox* parent, KexiMacroProperty* macroproperty)
			: QListBox(parent)
		{
			viewport()->setBackgroundMode(PaletteBackground);
			setVariableHeight(true);

			m_edititem = new ListBoxItem(this, macroproperty);
			Q_ASSERT( m_edititem->widget() != 0 );

			KoMacro::Variable::Ptr variable = m_edititem->variable();
			if(variable.data()) {
				KoMacro::Variable::List children = variable->children();
				if(children.count() <= 0) {
					KoMacro::Action::Ptr action = m_edititem->action();
					if(action.data()) {
						KoMacro::Variable::Ptr v = action->variable( macroproperty->name() );
						if(v.data())
							children = v->children();
					}
				}
				if(children.count() > 0) {
					KoMacro::Variable::List::Iterator childit(children.begin()), childend(children.end());
					for(; childit != childend; ++childit) {
						const QString s = (*childit)->variant().toString().stripWhiteSpace();
						if(! s.isEmpty())
							m_items.append(s);
					}
				}
			}

			QListBoxItem* item = m_edititem;
			const uint count = m_items.count();
			for(uint i = 0; i < count; i++)
				item = new QListBoxText(this, m_items[i], item);
		}

		virtual ~ListBox() {}

		ListBoxItem* editItem() const { return m_edititem; }
		QStringList items() const { return m_items; }

	private:
		ListBoxItem* m_edititem;
		QStringList m_items;
};

/**
* @internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroPropertyWidget::Private
{
	public:
		KexiMacroProperty* macroproperty;
		KComboBox* combobox;
		ListBox* listbox;
};

KexiMacroPropertyWidget::KexiMacroPropertyWidget(KoProperty::Property* property, QWidget* parent)
	: KoProperty::Widget(property, parent)
	, d( new Private() )
{
	kdDebug() << "KexiMacroPropertyWidget::KexiMacroPropertyWidget() Ctor" << endl;

	QHBoxLayout* layout = new QHBoxLayout(this, 0, 0);

	d->macroproperty = dynamic_cast<KexiMacroProperty*>( property->customProperty() );
	if(! d->macroproperty) {
		kdWarning() << "KexiMacroPropertyWidget::KexiMacroPropertyWidget() Missing macroproperty for property=" << property->name() << endl;
		return;
	}

	d->combobox = new KComboBox(this);
	layout->addWidget(d->combobox);
	d->listbox = new ListBox(d->combobox, d->macroproperty);
	d->combobox->setListBox(d->listbox);
	d->combobox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	d->combobox->setMinimumHeight(5);
	d->combobox->setEditable(true);
	d->combobox->setInsertionPolicy(QComboBox::NoInsertion);
	d->combobox->setMinimumSize(10, 0); // to allow the combo to be resized to a small size
	d->combobox->setAutoCompletion(false);
	d->combobox->setContextMenuEnabled(false);

	QVariant value = d->macroproperty->value();
	int index = d->listbox->items().findIndex( value.toString() );
	if(index >= 0) {
		d->combobox->setCurrentItem(index + 1);
		d->listbox->setCurrentItem(index + 1);
	}
	else {
		Q_ASSERT( d->listbox->editItem()->widget() != 0 );
		d->listbox->editItem()->widget()->setValue( d->macroproperty->value() );
		//d->combobox->setCurrentItem(0);
	}
	kdDebug() << ">>> KexiMacroPropertyWidget::KexiMacroPropertyWidget() CurrentItem=" << d->combobox->currentItem() << endl;

	d->combobox->setFocusProxy( d->listbox->editItem()->widget() );
	setFocusWidget(d->combobox);
	connect(d->combobox, SIGNAL(activated(int)), this, SLOT(propertyValueChanged()));
	connect(d->macroproperty, SIGNAL(valueChanged()), this, SLOT(propertyValueChanged()));
}

KexiMacroPropertyWidget::~KexiMacroPropertyWidget()
{
	kdDebug() << "KexiMacroPropertyWidget::~KexiMacroPropertyWidget() Dtor" << endl;
	delete d;
}

QVariant KexiMacroPropertyWidget::value() const
{
	QVariant value = d->combobox->currentText();
	Q_ASSERT( value.canCast( d->macroproperty->value().type() ) );
	value.cast( d->macroproperty->value().type() );
	kdDebug()<<"KexiMacroPropertyWidget::value() value="<<value<<endl;
	return value;
}

void KexiMacroPropertyWidget::setValue(const QVariant& value, bool emitChange)
{
	kdDebug()<<"KexiMacroPropertyWidget::setValue() value="<<value<<endl;
	d->combobox->setCurrentText( value.toString() );
	if(emitChange)
		emit valueChanged(this);
}

void KexiMacroPropertyWidget::setReadOnlyInternal(bool readOnly)
{
	Q_UNUSED(readOnly);
	//kdDebug()<<"KexiMacroPropertyWidget::setReadOnlyInternal() readOnly="<<readOnly<<endl;
}

void KexiMacroPropertyWidget::propertyValueChanged()
{
	const int index = d->combobox->currentItem();

	QString text;
	if(index == 0) {
		Q_ASSERT( d->listbox->editItem()->widget() );
		text = d->listbox->editItem()->widget()->value().toString();
	}
	else {
		text = d->combobox->text(index);
	}

	kdDebug()<<"KexiMacroPropertyWidget::propertyValueChanged() index="<<index<<" text="<<text<<endl;
	d->combobox->setCurrentText(text);
	emit valueChanged(this);
}

#include "keximacroproperty.moc"
