/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2005-2007 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidbautofield.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qmetaobject.h>
#include <qapplication.h>

#include <kdebug.h>
#include <klocale.h>

#include "kexidbcheckbox.h"
#include "kexidbimagebox.h"
#include "kexidblabel.h"
#include "kexidblineedit.h"
#include "kexidbtextedit.h"
#include "kexidbcombobox.h"
#include "kexipushbutton.h"
#include "kexidbform.h"

#include <kexidb/queryschema.h>
#include <formeditor/utils.h>
#include <kexiutils/utils.h>

#define KexiDBAutoField_SPACING 10 //10 pixel for spacing between a label and an editor widget

//! @internal
class KexiDBAutoField::Private
{
	public:
		Private()
		{
		}

		WidgetType widgetType; //!< internal: equal to m_widgetType_property or equal to result 
		                       //!< of widgetTypeForFieldType() if widgetTypeForFieldType is Auto
		WidgetType  widgetType_property; //!< provides widget type or Auto
		LabelPosition  lblPosition;
		QBoxLayout  *layout;
		QLabel  *label;
		QString  caption;
		KexiDB::Field::Type fieldTypeInternal;
		QString fieldCaptionInternal;
		QColor baseColor; //!< needed because for unbound mode editor==0
		QColor textColor; //!< needed because for unbound mode editor==0
		bool autoCaption : 1;
		bool focusPolicyChanged : 1;
		bool designMode : 1;
};

//-------------------------------------

KexiDBAutoField::KexiDBAutoField(const QString &text, WidgetType type, LabelPosition pos, 
	QWidget *parent, const char *name, bool designMode)
 : QWidget(parent, name)
 , KexiFormDataItemInterface()
 , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
 , d( new Private() )
{
	d->designMode = designMode;
	init(text, type, pos);
}

KexiDBAutoField::KexiDBAutoField(QWidget *parent, const char *name, bool designMode, LabelPosition pos)
 : QWidget(parent, name)
 , KexiFormDataItemInterface()
 , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
 , d( new Private() )
{
	d->designMode = designMode;
	init(QString()/*i18n("Auto Field")*/, Auto, pos);
}

KexiDBAutoField::~KexiDBAutoField()
{
	setUpdatesEnabled(false);
	if (m_subwidget)
		m_subwidget->setUpdatesEnabled(false);
	delete d;
}

void
KexiDBAutoField::init(const QString &text, WidgetType type, LabelPosition pos)
{
	d->fieldTypeInternal = KexiDB::Field::InvalidType;
	d->layout = 0;
	m_subwidget = 0;
	d->label = new QLabel(text, this);
	d->label->installEventFilter( this );
	//QFontMetrics fm( font() );
	//d->label->setFixedWidth( fm.width("This is a test string length") );
	d->autoCaption = true;
	d->focusPolicyChanged = false;
	d->widgetType = Auto;
	d->widgetType_property = (type==Auto ? Text : type); //to force "differ" to be true in setWidgetType()
	setLabelPosition(pos);
	setWidgetType(type);
	d->baseColor = palette().active().base();
	d->textColor = palette().active().text();
}

void
KexiDBAutoField::setWidgetType(WidgetType type)
{
	const bool differ = (type != d->widgetType_property);
	d->widgetType_property = type;
	if(differ) {
		if(type == Auto) {// try to guess type from data source type
			if (visibleColumnInfo())
				d->widgetType = KexiDBAutoField::widgetTypeForFieldType(visibleColumnInfo()->field->type());
			else
				d->widgetType = Auto;
		}
		else
			d->widgetType = d->widgetType_property;
		createEditor();
	}
}

void
KexiDBAutoField::createEditor()
{
	if(m_subwidget) {
		delete (QWidget *)m_subwidget;
	}

	QWidget *newSubwidget;
	switch( d->widgetType ) {
		case Text:
		case Double: //! @todo setup validator
		case Integer: //! @todo setup validator
		case Date:
		case Time:
		case DateTime:
			newSubwidget = new KexiDBLineEdit( this, QCString("KexiDBAutoField_KexiDBLineEdit:")+name() );
			break;
		case MultiLineText:
			newSubwidget = new KexiDBTextEdit( this, QCString("KexiDBAutoField_KexiDBTextEdit:")+name() );
			break;
		case Boolean:
			newSubwidget = new KexiDBCheckBox(dataSource(), this, QCString("KexiDBAutoField_KexiDBCheckBox:")+name());
			break;
		case Image:
			newSubwidget = new KexiDBImageBox(d->designMode, this, QCString("KexiDBAutoField_KexiDBImageBox:")+name());
			break;
		case ComboBox:
			newSubwidget = new KexiDBComboBox(this, Q3CString("KexiDBAutoField_KexiDBComboBox:")+name(), d->designMode);
			break;
		default:
			newSubwidget = 0;
			changeText(d->caption);
			//d->label->setText( d->dataSource.isEmpty() ? "<datasource>" : d->dataSource );
			break;
	}

	setSubwidget( newSubwidget ); //this will also allow to declare subproperties, see KFormDesigner::WidgetWithSubpropertiesInterface
	if(newSubwidget) {
		newSubwidget->setName( QCString("KexiDBAutoField_") + newSubwidget->className() );
		dynamic_cast<KexiDataItemInterface*>(newSubwidget)->setParentDataItemInterface(this);
		dynamic_cast<KexiFormDataItemInterface*>(newSubwidget)
			->setColumnInfo(columnInfo()); //needed at least by KexiDBImageBox
		dynamic_cast<KexiFormDataItemInterface*>(newSubwidget)
			->setVisibleColumnInfo(visibleColumnInfo()); //needed at least by KexiDBComboBox
		newSubwidget->setProperty("dataSource", dataSource()); //needed at least by KexiDBImageBox
		KFormDesigner::DesignTimeDynamicChildWidgetHandler::childWidgetAdded(this);
		newSubwidget->show();
		d->label->setBuddy(newSubwidget);
		if (d->focusPolicyChanged) {//if focusPolicy is changed at top level, editor inherits it
			newSubwidget->setFocusPolicy(focusPolicy());
		}
		else {//if focusPolicy is not changed at top level, inherit it from editor
			QWidget::setFocusPolicy(newSubwidget->focusPolicy());
		}
		setFocusProxy(newSubwidget); //ok?
		if (parentWidget())
			newSubwidget->setPalette( qApp->palette() );
		copyPropertiesToEditor();
//		KFormDesigner::installRecursiveEventFilter(newSubwidget, this);
	}

	setLabelPosition(labelPosition());
}

void KexiDBAutoField::copyPropertiesToEditor()
{
	if (m_subwidget) {
//		kDebug() << "KexiDBAutoField::copyPropertiesToEditor(): base col: " <<  d->baseColor.name() << 
//			"; text col: " << d->textColor.name() << endl;
		QPalette p( m_subwidget->palette() );
		p.setColor( QPalette::Active, QColorGroup::Base, d->baseColor );
		if(d->widgetType == Boolean)
			p.setColor( QPalette::Active, QColorGroup::Foreground, d->textColor );
		else
			p.setColor( QPalette::Active, QColorGroup::Text, d->textColor );
		m_subwidget->setPalette(p);
		//m_subwidget->setPaletteBackgroundColor( d->baseColor );
	}
}

void
KexiDBAutoField::setLabelPosition(LabelPosition position)
{
	d->lblPosition = position;
	if(d->layout) {
		QBoxLayout *lyr = d->layout;
		d->layout = 0;
		delete lyr;
	}

	if(m_subwidget)
		m_subwidget->show();
	//! \todo support right-to-left layout where positions are inverted
	if (position==Top || position==Left) {
		int align = d->label->alignment();
		if(position == Top) {
			d->layout = (QBoxLayout*) new QVBoxLayout(this);
			align |= AlignVertical_Mask;
			align ^= AlignVertical_Mask;
			align |= AlignTop;
		}
		else {
			d->layout = (QBoxLayout*) new QHBoxLayout(this);
			align |= AlignVertical_Mask;
			align ^= AlignVertical_Mask;
			align |= AlignVCenter;
		}
		d->label->setAlignment(align);
		if(d->widgetType == Boolean 
			|| (d->widgetType == Auto && fieldTypeInternal() == KexiDB::Field::InvalidType && !d->designMode))
		{
			d->label->hide();
		}
		else {
			d->label->show();
		}
		d->layout->addWidget(d->label, 0, position == Top ? int(Qt::AlignLeft) : 0);
		if(position == Left && d->widgetType != Boolean)
			d->layout->addSpacing(KexiDBAutoField_SPACING);
		d->layout->addWidget(m_subwidget, 1);
		KexiSubwidgetInterface *subwidgetInterface = dynamic_cast<KexiSubwidgetInterface*>((QWidget*)m_subwidget);
		if (subwidgetInterface) {
			if (subwidgetInterface->appendStretchRequired(this))
				d->layout->addStretch(0);
			if (subwidgetInterface->subwidgetStretchRequired(this)) {
				QSizePolicy sizePolicy( m_subwidget->sizePolicy() );
				if(position == Left) {
					sizePolicy.setHorData( QSizePolicy::Minimum );
					d->label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
				}
				else {
					sizePolicy.setVerData( QSizePolicy::Minimum );
					d->label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
				}
				m_subwidget->setSizePolicy(sizePolicy);
			}
		}
//		if(m_subwidget)
	//		m_subwidget->setSizePolicy(...);
	}
	else {
		d->layout = (QBoxLayout*) new QHBoxLayout(this);
		d->label->hide();
		d->layout->addWidget(m_subwidget);
	}
	//a hack to force layout to be refreshed (any better idea for this?)
	resize(size()+QSize(1,0));
	resize(size()-QSize(1,0));
	if (dynamic_cast<KexiDBAutoField*>((QWidget*)m_subwidget)) {
		//needed for KexiDBComboBox
		dynamic_cast<KexiDBAutoField*>((QWidget*)m_subwidget)->setLabelPosition(position);
	}
}

void
KexiDBAutoField::setInvalidState( const QString &text )
{
	// Widget with an invalid dataSource is just a QLabel
	if (d->designMode)
		return;
	d->widgetType = Auto;
	createEditor();
	setFocusPolicy(QWidget::NoFocus);
	if (m_subwidget)
		m_subwidget->setFocusPolicy(QWidget::NoFocus);
//! @todo or set this to editor's text?
	d->label->setText( text );
}

bool
KexiDBAutoField::isReadOnly() const
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		return iface->isReadOnly();
	else
		return false;
}

void
KexiDBAutoField::setReadOnly( bool readOnly )
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		iface->setReadOnly(readOnly);
}

void
KexiDBAutoField::setValueInternal(const QVariant& add, bool removeOld)
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		iface->setValue(m_origValue, add, removeOld);
//		iface->setValueInternal(add, removeOld);
}

QVariant
KexiDBAutoField::value()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		return iface->value();
	return QVariant();
}

bool
KexiDBAutoField::valueIsNull()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		return iface->valueIsNull();
	return true;
}

bool
KexiDBAutoField::valueIsEmpty()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		return iface->valueIsEmpty();
	return true;
}

bool
KexiDBAutoField::valueIsValid()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		return iface->valueIsValid();
	return true;
}

bool
KexiDBAutoField::valueChanged()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	kexipluginsdbg << m_origValue  << endl;
	if(iface)
		return iface->valueChanged();
	return false;
}

void
KexiDBAutoField::installListener(KexiDataItemChangesListener* listener)
{
	KexiFormDataItemInterface::installListener(listener);
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		iface->installListener(listener);
}

KexiDBAutoField::WidgetType KexiDBAutoField::widgetType() const
{
	return d->widgetType_property;
}

KexiDBAutoField::LabelPosition KexiDBAutoField::labelPosition() const
{
	return d->lblPosition;
}

QString KexiDBAutoField::caption() const
{
	return d->caption;
}

bool KexiDBAutoField::hasAutoCaption() const
{
	return d->autoCaption;
}

QWidget* KexiDBAutoField::editor() const
{
	return m_subwidget;
}

QLabel* KexiDBAutoField::label() const
{
	return d->label;
}

int KexiDBAutoField::fieldTypeInternal() const
{
	return d->fieldTypeInternal;
}

QString KexiDBAutoField::fieldCaptionInternal() const
{
	return d->fieldCaptionInternal;
}

bool
KexiDBAutoField::cursorAtStart()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		return iface->cursorAtStart();
	return false;
}

bool
KexiDBAutoField::cursorAtEnd()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		return iface->cursorAtEnd();
	return false;
}

void
KexiDBAutoField::clear()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		iface->clear();
}

void
KexiDBAutoField::setFieldTypeInternal(int kexiDBFieldType)
{
	d->fieldTypeInternal = (KexiDB::Field::Type)kexiDBFieldType;
	KexiDB::Field::Type fieldType;
	//find real fied type to use
	if (d->fieldTypeInternal==KexiDB::Field::InvalidType) {
		if (visibleColumnInfo())
			fieldType = KexiDB::Field::Text;
		else
			fieldType = KexiDB::Field::InvalidType;
	}
	else
		fieldType = d->fieldTypeInternal;

	const WidgetType newWidgetType = KexiDBAutoField::widgetTypeForFieldType( fieldType );

	if(d->widgetType != newWidgetType) {
		d->widgetType = newWidgetType;
		createEditor();
	}
	setFieldCaptionInternal(d->fieldCaptionInternal);
}

void
KexiDBAutoField::setFieldCaptionInternal(const QString& text)
{
	d->fieldCaptionInternal = text;
	//change text only if autocaption is set and no columnInfo is available
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if((!iface || !iface->columnInfo()) && d->autoCaption) {
		changeText(d->fieldCaptionInternal);
	}
}

void
KexiDBAutoField::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	setColumnInfoInternal(cinfo, cinfo);
}

void
KexiDBAutoField::setColumnInfoInternal(KexiDB::QueryColumnInfo* cinfo, KexiDB::QueryColumnInfo* visibleColumnInfo)
{
	// change widget type depending on field type
	if(d->widgetType_property == Auto) {
		WidgetType newWidgetType = Auto;
		KexiDB::Field::Type fieldType;
		if (cinfo)
			fieldType = visibleColumnInfo->field->type();
		else if (dataSource().isEmpty())
			fieldType = KexiDB::Field::InvalidType;
		else
			fieldType = KexiDB::Field::Text;

		if (fieldType != KexiDB::Field::InvalidType) {
			newWidgetType = KexiDBAutoField::widgetTypeForFieldType( fieldType );
		}
		if(d->widgetType != newWidgetType || newWidgetType==Auto) {
			d->widgetType = newWidgetType;
			createEditor();
		}
	}
	// update label's text
	changeText((cinfo && d->autoCaption) ? cinfo->captionOrAliasOrName() : d->caption);

	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if(iface)
		iface->setColumnInfo(visibleColumnInfo);
}

//static
KexiDBAutoField::WidgetType
KexiDBAutoField::widgetTypeForFieldType(KexiDB::Field::Type type)
{
	switch(type) {
		case KexiDB::Field::Integer:
		case KexiDB::Field::ShortInteger:
		case KexiDB::Field::BigInteger:
			return Integer;
		case  KexiDB::Field::Boolean:
			return Boolean;
		case KexiDB::Field::Float:
		case KexiDB::Field::Double:
			return Double;
		case KexiDB::Field::Date:
			return Date;
		case KexiDB::Field::DateTime:
			return DateTime;
		case KexiDB::Field::Time:
			return Time;
		case KexiDB::Field::Text:
			return Text;
		case KexiDB::Field::LongText:
			return MultiLineText;
		case KexiDB::Field::Enum:
			return ComboBox;
		case KexiDB::Field::InvalidType:
			return Auto;
		case KexiDB::Field::BLOB:
			return Image;
		default:
			break;
	}
	return Text;
}

void
KexiDBAutoField::changeText(const QString &text, bool beautify)
{
	QString realText;
	bool unbound = false;
	if (d->autoCaption && (d->widgetType==Auto || dataSource().isEmpty())) {
		if (d->designMode)
			realText = QString::fromLatin1(name())+" "+i18n("Unbound Auto Field", "(unbound)");
		else
			realText.clear();
		unbound = true;
	}
	else {
		if (beautify) {
	/*! @todo look at appendColonToAutoLabels setting [bool]
		@todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
		(see doc/dev/settings.txt) */
			if (!text.isEmpty()) {
				realText = text[0].upper() + text.mid(1);
				if (d->widgetType!=Boolean) {
//! @todo ":" suffix looks weird for checkbox; remove this condition when [x] is displayed _after_ label
//! @todo support right-to-left layout where position of ":" is inverted
					realText += ": ";
				}
			}
		}
		else
			realText = text;
	}

	if (unbound)
		d->label->setAlignment( Qt::AlignCenter | Qt::WordBreak );
	else
		d->label->setAlignment( Qt::AlignCenter );
//	QWidget* widgetToAlterForegroundColor;
	if(d->widgetType == Boolean) {
		static_cast<QCheckBox*>((QWidget*)m_subwidget)->setText(realText);
//		widgetToAlterForegroundColor = m_subwidget;
	}
	else {
		d->label->setText(realText);
//		widgetToAlterForegroundColor = d->label;
	}
/*
	if (unbound)
		widgetToAlterForegroundColor->setPaletteForegroundColor( 
			KexiUtils::blendedColors(
				widgetToAlterForegroundColor->paletteForegroundColor(), 
				widgetToAlterForegroundColor->paletteBackgroundColor(), 2, 1));
	else
		widgetToAlterForegroundColor->setPaletteForegroundColor( paletteForegroundColor() );*/
}

void
KexiDBAutoField::setCaption(const QString &caption)
{
	d->caption = caption;
	if(!d->autoCaption && !caption.isEmpty())
		changeText(d->caption);
}

void
KexiDBAutoField::setAutoCaption(bool autoCaption)
{
	d->autoCaption = autoCaption;
	if(d->autoCaption) {
		//d->caption.clear();
		if(columnInfo()) {
			changeText(columnInfo()->captionOrAliasOrName());
		}
		else {
			changeText(d->fieldCaptionInternal);
		}
	}
	else
		changeText(d->caption);
}

void
KexiDBAutoField::setDataSource( const QString &ds ) {
	KexiFormDataItemInterface::setDataSource(ds);
	if (ds.isEmpty()) {
		setColumnInfo(0);
	}
}

QSize
KexiDBAutoField::sizeHint() const
{
	if (d->lblPosition == NoLabel)
		return m_subwidget ? m_subwidget->sizeHint() : QWidget::sizeHint();

	QSize s1(0,0);
	if (m_subwidget)
		s1 = m_subwidget->sizeHint();
	QSize s2(d->label->sizeHint());
	if (d->lblPosition == Top)
		return QSize(qMax(s1.width(), s2.width()), s1.height()+KexiDBAutoField_SPACING+s2.height());

	//left
	return QSize(s1.width()+KexiDBAutoField_SPACING+s2.width(), qMax(s1.height(), s2.height()));
}

void
KexiDBAutoField::setFocusPolicy( FocusPolicy policy )
{
	d->focusPolicyChanged = true;
	QWidget::setFocusPolicy(policy);
	d->label->setFocusPolicy(policy);
	if (m_subwidget)
		m_subwidget->setFocusPolicy(policy);
}

void
KexiDBAutoField::updateInformationAboutUnboundField()
{
	if ( (d->autoCaption && (dataSource().isEmpty() || dataSourceMimeType().isEmpty()))
		|| (!d->autoCaption && d->caption.isEmpty()) )
	{
		d->label->setText( QString::fromLatin1(name())+" "+i18n("Unbound Auto Field", " (unbound)") );
	}
//	else
//		d->label->setText( QString::fromLatin1(name())+" "+i18n(" (unbound)") );
}

/*void
KexiDBAutoField::paintEvent( QPaintEvent* pe )
{
	QWidget::paintEvent( pe );

	if (   (d->autoCaption && (dataSource().isEmpty() || dataSourceMimeType().isEmpty()))
		|| (!d->autoCaption && d->caption.isEmpty()) )
	{
		QPainter p(this);
		p.setPen( d->label->paletteForegroundColor() );
		p.setClipRect(pe->rect());
		p.setFont(d->label->font());
		p.drawText(rect(), Qt::AlignLeft | Qt::TextWordWrap, 
			QString::fromLatin1(name())+" "+i18n(" (unbound)"));
	}
}*/

void
KexiDBAutoField::paletteChange( const QPalette& oldPal )
{
	Q_UNUSED(oldPal);
	d->label->setPalette( palette() );
}

void KexiDBAutoField::unsetPalette()
{
	QWidget::unsetPalette();

}

// ===== methods below are just proxies for the internal editor or label =====

const QColor & KexiDBAutoField::paletteForegroundColor() const
{
	return d->textColor;
}

void KexiDBAutoField::setPaletteForegroundColor( const QColor & color )
{
	d->textColor = color;
	copyPropertiesToEditor();
}

const QColor & KexiDBAutoField::paletteBackgroundColor() const
{
	return d->baseColor;
}

void KexiDBAutoField::setPaletteBackgroundColor( const QColor & color )
{
	d->baseColor = color;
	copyPropertiesToEditor();
}

const QColor & KexiDBAutoField::foregroundLabelColor() const
{
	if(d->widgetType == Boolean)
		return paletteForegroundColor();

	return d->label->paletteForegroundColor();
}

void KexiDBAutoField::setForegroundLabelColor( const QColor & color )
{
	if(d->widgetType == Boolean)
		setPaletteForegroundColor(color);
	else {
		d->label->setPaletteForegroundColor(color);
		QWidget::setPaletteForegroundColor(color);
	}
}

const QColor & KexiDBAutoField::backgroundLabelColor() const
{
	if(d->widgetType == Boolean)
		return paletteBackgroundColor();

	return d->label->paletteBackgroundColor();
}

void KexiDBAutoField::setBackgroundLabelColor( const QColor & color )
{
	if(d->widgetType == Boolean)
		setPaletteBackgroundColor(color);
	else {
		d->label->setPaletteBackgroundColor(color);
		QWidget::setPaletteBackgroundColor(color);
	}

//	if (m_subwidget)
//		m_subwidget->setPalette( qApp->palette() );
}

QVariant KexiDBAutoField::property( const char * name ) const
{
	bool ok;
	QVariant val = KFormDesigner::WidgetWithSubpropertiesInterface::subproperty(name, ok);
	if (ok)
		return val;
	return QWidget::property(name);
}

bool KexiDBAutoField::setProperty( const char * name, const QVariant & value )
{
	bool ok = KFormDesigner::WidgetWithSubpropertiesInterface::setSubproperty(name, value);
	if (ok)
		return true;
	return QWidget::setProperty(name, value);
}

bool KexiDBAutoField::eventFilter( QObject *o, QEvent *e )
{
	if (o==d->label && d->label->buddy() && e->type()==QEvent::MouseButtonRelease) {
		//focus label's buddy when user clicked the label
		d->label->buddy()->setFocus();
	}
	return QWidget::eventFilter(o, e);
}

void KexiDBAutoField::setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue)
{
	KexiFormDataItemInterface::setDisplayDefaultValue(widget, displayDefaultValue);
	if (dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget))
		dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget)->setDisplayDefaultValue(m_subwidget, displayDefaultValue);
}

void KexiDBAutoField::moveCursorToEnd()
{
	KexiDataItemInterface *iface = dynamic_cast<KexiDataItemInterface*>((QWidget*)m_subwidget);
	if (iface)
		iface->moveCursorToEnd();
}

void KexiDBAutoField::moveCursorToStart()
{
	KexiDataItemInterface *iface = dynamic_cast<KexiDataItemInterface*>((QWidget*)m_subwidget);
	if (iface)
		iface->moveCursorToStart();
}

void KexiDBAutoField::selectAll()
{
	KexiDataItemInterface *iface = dynamic_cast<KexiDataItemInterface*>((QWidget*)m_subwidget);
	if (iface)
		iface->selectAll();
}

bool KexiDBAutoField::keyPressed(QKeyEvent *ke)
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)m_subwidget);
	if (iface && iface->keyPressed(ke))
		return true;
	return false;
}

#include "kexidbautofield.moc"
