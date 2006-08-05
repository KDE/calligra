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

KexiMacroProperty::KexiMacroProperty(KoProperty::Property* parent, KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name)
	: KoProperty::CustomProperty(parent)
	, d( new Private() )
{
	d->macroitem = macroitem;
	d->name = name;
	init();
}

KexiMacroProperty::~KexiMacroProperty()
{
	delete d;
}

void KexiMacroProperty::init()
{
	Q_ASSERT( d->macroitem != 0 );
	//kdDebug() << "--------- KexiMacroProperty::set() macroitem=" << d->macroitem->name() << " name=" << d->name << endl;

	KSharedPtr<KoMacro::Action> action = d->macroitem->action();
	KSharedPtr<KoMacro::Variable> actionvariable = action->variable(d->name);
	if(! actionvariable.data()) {
		kdDebug() << "KexiMacroProperty::createProperty() Skipped cause there exists no such action=" << d->name << endl;
		return;
	}

	KSharedPtr<KoMacro::Variable> variable = d->macroitem->variable(d->name, true/*checkaction*/);
	if(! variable.data()) {
		kdDebug() << "KexiMacroProperty::createProperty() Skipped cause there exists no such variable=" << d->name << endl;
		return;
	}

	//TESTCASE!!!!!!!!!!!!!!!!!!!!!!
	//if(! variable->isEnabled()) qFatal( QString("############## VARIABLE=%1").arg(variable->name()).latin1() );

	Q_ASSERT(! d->name.isNull());
	m_property->setName( d->name.latin1() );
	m_property->setCaption( actionvariable->text() );
	m_property->setDescription( action->comment() );
	m_property->setValue( variable->variant(), true );
	m_property->setType( KEXIMACRO_PROPERTYEDITORTYPE ); // use our own propertytype
}

KoProperty::Property* KexiMacroProperty::parentProperty() const
{
	return m_property;
}

void KexiMacroProperty::setValue(const QVariant &value, bool rememberOldValue)
{
	Q_UNUSED(rememberOldValue);
	kdDebug()<<"KexiMacroProperty::setValue name="<<d->name<<" value="<<value<<" rememberOldValue="<<rememberOldValue<<endl;
	if(! d->macroitem->setVariant(d->name, value)) { // takes care of the type-conversation
		kdDebug()<<"KexiMacroProperty::setValue Update failed !!!"<<endl;
		return;
	}

	// m_property->setValue() does check if the value changed by using
	// this-value() and cause we already set it above, m_property->setValue()
	// will be aborted. Well, we don't touch the properties value and handle
	// it all via our CustomProperty class anyway. So, just ignore the property.
	//m_property->setValue(this->value(), rememberOldValue, false/*useCustomProperty*/);

	emit valueChanged();
}

QVariant KexiMacroProperty::value() const
{
	KSharedPtr<KoMacro::Variable> variable = d->macroitem->variable(d->name, true);
	Q_ASSERT( variable.data() != 0 );
	return variable.data() ? variable->variant() : QVariant();
}

bool KexiMacroProperty::handleValue() const
{
	return true; // we handle getting and setting of values and don't need KoProperty::Property for it.
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
	return d->macroitem->variable(d->name, true/*checkaction*/);
}

KoProperty::Property* KexiMacroProperty::createProperty(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name)
{
	KoProperty::Property* property = new KoProperty::Property();
	KexiMacroProperty* customproperty = new KexiMacroProperty(property, macroitem, name);
	if(! customproperty->variable().data()) {
		kdWarning() << "KexiMacroProperty::createProperty() No such variable" << endl;
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
	kdDebug()<<"KexiMacroPropertyFactory::createCustomProperty parent="<<parent->name()<<endl;

	KoProperty::CustomProperty* customproperty = parent->customProperty();
	KexiMacroProperty* parentcustomproperty = dynamic_cast<KexiMacroProperty*>(customproperty);
	if(! parentcustomproperty) {
		kdWarning() << "KexiMacroPropertyFactory::createCustomProperty() parent=" << parent->name() << " has an invalid customproperty." << endl;
		return 0;
	}

	KSharedPtr<KoMacro::MacroItem> macroitem = parentcustomproperty->macroItem();
	Q_ASSERT( macroitem.data() != 0 );
	const QString name = parentcustomproperty->name();
	Q_ASSERT(! name.isEmpty());

	KexiMacroProperty* macroproperty = new KexiMacroProperty(parent, macroitem, name);
	if(! macroproperty->variable().data()) {
		delete macroproperty; macroproperty = 0;
		return 0;
	}

	return macroproperty;
}

KoProperty::Widget* KexiMacroPropertyFactory::createCustomWidget(KoProperty::Property* property)
{
	kdDebug()<<"KexiMacroPropertyFactory::createCustomWidget property="<<property->name()<<endl;
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
* combobox used within @a KexiMacroPropertyWidget to handle variables
* within a @a ListBox instance.
*/
class ListBoxItem : public QListBoxText
{
	public:
		ListBoxItem(QListBox* listbox)
			: QListBoxText(listbox), m_enabled(true) {}
		ListBoxItem(QListBox* listbox, const QString& text, QListBoxItem* after)
			: QListBoxText(listbox, text, after), m_enabled(true) {}
		virtual ~ListBoxItem() {}
		void setEnabled(bool enabled) { m_enabled = enabled; }
		virtual int width(const QListBox* lb) const {
			Q_ASSERT( dynamic_cast<KComboBox*>( lb->parent() ) );
			return static_cast<KComboBox*>( lb->parent() )->lineEdit()->width() + 2;
		}
		virtual int height(const QListBox* lb) const {
			Q_ASSERT( dynamic_cast<KComboBox*>( lb->parent() ) );
			return m_enabled ? static_cast<KComboBox*>( lb->parent() )->height() + 2 : 0;
		}
	private:
		bool m_enabled;
};

/**
* @internal implementation of a @a ListBoxItem to provide an editable
* @a KoProperty::Widget as QListBoxItem in a @a ListBox instance.
*/
class EditListBoxItem : public ListBoxItem
{
	public:

		EditListBoxItem(QListBox* listbox, KexiMacroProperty* macroproperty)
			: ListBoxItem(listbox)
			, m_macroproperty(macroproperty)
			, m_prop(0)
			, m_widget(0)
		{
			init();
		}

		virtual ~EditListBoxItem() {
			delete m_widget;
			delete m_prop;
		}

		virtual QString text() const {
			KSharedPtr<KoMacro::Variable> variable = m_macroproperty->variable();
			Q_ASSERT( variable.data() );
			//kdDebug()<<"EditListBoxItem::text() text="<<variable->toString()<<endl;
			Q_ASSERT( variable->toString() != QString::null );
			return variable->toString();
		}

		KoProperty::Widget* widget() const { return m_widget; }
		KSharedPtr<KoMacro::MacroItem> macroItem() const { return m_macroproperty->macroItem(); }
		KSharedPtr<KoMacro::Variable> variable() const { return m_macroproperty->variable(); }
		KSharedPtr<KoMacro::Action> action() const { return m_macroproperty->macroItem()->action(); }

	protected:
		virtual void paint(QPainter* p) {
			if(! m_widget) return;
			Q_ASSERT( dynamic_cast<KComboBox*>( listBox()->parent() ) );
			const int w = width(listBox());
			const int h = height(listBox());
			m_widget->setFixedSize(w - 2, h - 2);
			p->drawPixmap(0, 0, QPixmap::grabWidget(m_widget), 1, 1, w - 1, h - 1);
		}

	private:
		void init() {
			KSharedPtr<KoMacro::MacroItem> macroitem = m_macroproperty->macroItem();
			Q_ASSERT( macroitem.data() );
			KSharedPtr<KoMacro::Action> action = m_macroproperty->macroItem()->action();
			if(! action.data()) {
				kdWarning() << "EditListBoxItem::EditListBoxItem() Skipped cause there exists no action for macroproperty=" << m_macroproperty->name() << endl;
				return;
			}
			KoProperty::Property* parentproperty = m_macroproperty->parentProperty();
			if(! parentproperty) {
				kdWarning() << "EditListBoxItem::EditListBoxItem() No parentproperty defined" << endl;
				return;
			}
			KSharedPtr<KoMacro::Variable> variable = m_macroproperty->variable();
			if(! variable.data()) {
				kdWarning() << "EditListBoxItem::EditListBoxItem() No variable defined for property=" << parentproperty->name() << endl;
				return;
			}

			QVariant variant = variable->variant();

			KSharedPtr<KoMacro::Variable> actionvariable = action->variable(m_macroproperty->name());
			if(actionvariable.data()) {
				QVariant actionvariant = actionvariable->variant();
				Q_ASSERT( ! actionvariant.isNull() );
				Q_ASSERT( variant.canCast(actionvariant.type()) );
				variant.cast( actionvariant.type() ); //preserve type.
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
					kdWarning() << "EditListBoxItem::EditListBoxItem() name=" << variable->name() << " type=" << QVariant::typeToName(variant.type()) << endl;
				} break;
			}

			QString name = variable->name();
			Q_ASSERT(! name.isNull());
			//if(name.isNull()) name = "aaaaaaaaaaaaaaaaa";//TESTCASE
			m_prop = new KoProperty::Property(
				name.latin1(), // name
				variant, // value
				variable->text(), // caption
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

	private:
		KexiMacroProperty* m_macroproperty;
		KoProperty::Property* m_prop;
		KoProperty::Widget* m_widget;
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
			, m_macroproperty(macroproperty)
			, m_edititem(0)
		{
			viewport()->setBackgroundMode(PaletteBackground);
			setVariableHeight(true);
			update();
		}

		virtual ~ListBox() {}

		void update() {
			m_items.clear();
			delete m_edititem;
			m_edititem = 0;
			clear();

			m_edititem = new EditListBoxItem(this, m_macroproperty);
			Q_ASSERT( m_edititem->widget() != 0 );

			const QString name = m_macroproperty->name();
			KoMacro::Variable::List children;
			{
				KoMacro::Variable::List actionchildren;

				KSharedPtr<KoMacro::Variable> itemvar = m_macroproperty->macroItem()->variable(name,false);
				//kdDebug() << "KexiMacroProperty::ListBox::update() itemvar="<<(itemvar.data() ? "name:"+itemvar->name()+" value:"+itemvar->toString() : "NULL")<<endl;
				if(itemvar.data())
					actionchildren = itemvar->children();

				KSharedPtr<KoMacro::Action> action = m_edititem->action();
				KSharedPtr<KoMacro::Variable> actionvar = action.data() ? action->variable(name) : KSharedPtr<KoMacro::Variable>();
				//kdDebug() << "KexiMacroProperty::ListBox::update() actionvar="<<(actionvar.data() ? "name:"+actionvar->name()+" value:"+actionvar->toString() : "NULL")<<endl;
				if(actionvar.data())
					actionchildren += actionvar->children();

				KoMacro::Variable::List::ConstIterator it(actionchildren.constBegin()), end(actionchildren.constEnd());
				for(; it != end; ++it) {
					if(name == (*it)->name()) {
						KoMacro::Variable::List list = (*it)->children();
						KoMacro::Variable::List::ConstIterator listit(list.constBegin()), listend(list.constEnd());
						for(; listit != listend; ++listit)
							children.append( *listit );
					}
				}

				if(children.count() <= 0)
					children = actionchildren;
			}

			/*
			kdDebug() << "KexiMacroProperty::ListBox::update() name="<<name<<" childcount="<<children.count()<<endl;
			KoMacro::Variable::List::ConstIterator listit(children.constBegin()), listend(children.constEnd());
			for(; listit != listend; ++listit) {
				kdDebug()<<"    child name="<<(*listit)->name()<<" value="<<(*listit)->toString()<<" childcount="<<(*listit)->children().count()<<endl;
			}
			*/

			if(children.count() > 0) {
				KoMacro::Variable::List::Iterator childit(children.begin()), childend(children.end());
				for(; childit != childend; ++childit) {
					const QString n = (*childit)->name();
					//if(! n.startsWith("@")) continue;
					const QVariant v = (*childit)->variant();

					//kdDebug() << "  child name=" << n << " value=" << v << endl;
					switch( v.type() ) {
						/* case QVariant::Map: {
							const QMap<QString,QVariant> map = v.toMap();
							for(QMap<QString,QVariant>::ConstIterator it = map.constBegin(); it != map.constEnd(); ++it)
								m_items.append(it.key());
						} break; */
						case QVariant::List: {
							const QValueList<QVariant> list = v.toList();
							for(QValueList<QVariant>::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it) {
								const QString s = (*it).toString().stripWhiteSpace();
								if(! s.isEmpty())
									m_items.append(s);
							}
						} break;
						case QVariant::StringList: {
							const QStringList list = v.toStringList();
							for(QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
								if(! (*it).isEmpty())
									m_items.append(*it);
						} break;
						default: {
							const QString s = v.toString().stripWhiteSpace();
							if(! s.isEmpty())
								m_items.append(s);
						} break;
					}
				}
			}

			QListBoxItem* item = m_edititem;
			const uint count = m_items.count();
			for(uint i = 0; i < count; i++)
				item = new ListBoxItem(this, m_items[i], item);
		}

		EditListBoxItem* editItem() const { return m_edititem; }
		QStringList items() const { return m_items; }

		virtual void hide () {
			QListBox::hide();
			for(uint i = 0; i < count(); i++)
				static_cast<ListBoxItem*>( item(i) )->setEnabled(false);
		}
		virtual void show() {
			update();
			adjustSize();
			QListBox::show();
		}

	private:
		KexiMacroProperty* m_macroproperty;
		EditListBoxItem* m_edititem;
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
	d->combobox->setEditable(true);
	d->combobox->setListBox(d->listbox);
	d->combobox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	d->combobox->setMinimumHeight(5);
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
		d->listbox->editItem()->widget()->setValue( d->macroproperty->value(), true );
		//d->combobox->setCurrentItem(0);
	}
	kdDebug() << ">>> KexiMacroPropertyWidget::KexiMacroPropertyWidget() CurrentItem=" << d->combobox->currentItem() << endl;

	d->combobox->setFocusProxy( d->listbox->editItem()->widget() );
	setFocusWidget( d->combobox->lineEdit() );

	connect(d->combobox, SIGNAL(textChanged(const QString&)), 
	        this, SLOT(slotComboBoxChanged()));
	connect(d->combobox, SIGNAL(activated(int)),
	        this, SLOT(slotComboBoxActivated()));
	connect(d->listbox->editItem()->widget(), SIGNAL(valueChanged(Widget*)),
	        this, SLOT(slotWidgetValueChanged()));
	connect(d->macroproperty, SIGNAL(valueChanged()),
	        this, SLOT(slotPropertyValueChanged()));
}

KexiMacroPropertyWidget::~KexiMacroPropertyWidget()
{
	kdDebug() << "KexiMacroPropertyWidget::~KexiMacroPropertyWidget() Dtor" << endl;
	delete d;
}

QVariant KexiMacroPropertyWidget::value() const
{
	kdDebug()<<"KexiMacroPropertyWidget::value() value="<<d->macroproperty->value()<<endl;
	return d->macroproperty->value();
	/* QVariant value = d->combobox->currentText();
	value.cast( d->macroproperty->value().type() );
	return value; */
}

void KexiMacroPropertyWidget::setValue(const QVariant& value, bool emitChange)
{
	kdDebug()<<"KexiMacroPropertyWidget::setValue() value="<<value<<" emitChange="<<emitChange<<endl;
	
	if(! emitChange)
		d->combobox->blockSignals(true);

	const QString s = value.toString();
	d->combobox->setCurrentText( s.isNull() ? "" : s );

	if(emitChange)
		emit valueChanged(this);
	else
		d->combobox->blockSignals(false);
}

void KexiMacroPropertyWidget::setReadOnlyInternal(bool readOnly)
{
	Q_UNUSED(readOnly);
	//kdDebug()<<"KexiMacroPropertyWidget::setReadOnlyInternal() readOnly="<<readOnly<<endl;
}

void KexiMacroPropertyWidget::slotComboBoxChanged()
{
	kdDebug()<<"KexiMacroPropertyWidget::slotComboBoxChanged()"<<endl;
	const QVariant v = d->combobox->currentText();
	d->macroproperty->setValue(v, true);
	//emit valueChanged(this);
}

void KexiMacroPropertyWidget::slotComboBoxActivated()
{
	Q_ASSERT( d->listbox->editItem()->widget() );
	const int index = d->combobox->currentItem();
	QString text = (index == 0)
		? d->listbox->editItem()->widget()->value().toString()
		: d->combobox->text(index);
	kdDebug()<<"KexiMacroPropertyWidget::slotComboBoxActivated() index="<<index<<" text="<<text<<endl;
	d->combobox->setCurrentText(text);
	slotWidgetValueChanged();
}

void KexiMacroPropertyWidget::slotWidgetValueChanged()
{
	d->macroproperty->emitPropertyChanged();
}

void KexiMacroPropertyWidget::slotPropertyValueChanged()
{
	Q_ASSERT( d->listbox->editItem()->widget() );
	const QVariant v = d->macroproperty->value();
	kdDebug()<<"KexiMacroPropertyWidget::slotPropertyValueChanged() value="<<v<<endl;
	d->listbox->editItem()->widget()->setValue(v, true);
}

#include "keximacroproperty.moc"
