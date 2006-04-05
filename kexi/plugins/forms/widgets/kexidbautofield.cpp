/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kdebug.h>
#include <klocale.h>

#include "kexidbcheckbox.h"
#include "kexidbdateedit.h"
#include "kexidbdatetimeedit.h"
#include "kexidbimagebox.h"
//#include "kexidbintspinbox.h"
//#include "kexidbdoublespinbox.h"
#include "kexidblabel.h"
#include "kexidblineedit.h"
#include "kexidbtextedit.h"
#include "kexidbtimeedit.h"
#include "kexipushbutton.h"
#include "kexidbform.h"

#include <kexidb/queryschema.h>
#include <formeditor/utils.h>
#include <kexiutils/utils.h>

#define KexiDBAutoField_SPACING 10 //10 pixel for spacing between a label and an editor widget

KexiDBAutoField::KexiDBAutoField(const QString &text, WidgetType type, LabelPosition pos, 
	QWidget *parent, const char *name, bool designMode)
 : QWidget(parent, name)
 , KexiFormDataItemInterface()
 , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
 , m_designMode(designMode)
{
	init(text, type, pos);
}

KexiDBAutoField::KexiDBAutoField(QWidget *parent, const char *name, bool designMode)
 : QWidget(parent, name)
 , KexiFormDataItemInterface()
 , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
 , m_designMode(designMode)
{
	init(QString::null/*i18n("Auto Field")*/, Auto, Left);
}

KexiDBAutoField::~KexiDBAutoField()
{
}

void
KexiDBAutoField::init(const QString &text, WidgetType type, LabelPosition pos)
{
	m_fieldTypeInternal = KexiDB::Field::InvalidType;
	m_layout = 0;
	m_editor = 0;
	m_label = new QLabel(text, this);
	QFontMetrics fm( font() );
	//m_label->setFixedWidth( fm.width("This is a test string length") );
	m_autoCaption = true;
	m_focusPolicyChanged = false;
	m_widgetType = Auto;
	m_widgetType_property = (type==Auto ? Text : type); //to force "differ" to be true in setWidgetType()
	setWidgetType(type);
	setLabelPosition(pos);
}

void
KexiDBAutoField::setWidgetType(WidgetType type)
{
	const bool differ = (type != m_widgetType_property);
	m_widgetType_property = type;
	if(differ) {
		if(type == Auto) {// try to guess type from data source type
			if (columnInfo())
				m_widgetType = KexiDBAutoField::widgetTypeForFieldType(columnInfo()->field->type());
			else
				m_widgetType = Auto;
		}
		else
			m_widgetType = m_widgetType_property;
		createEditor();
	}
}

void
KexiDBAutoField::createEditor()
{
	if(m_editor)
		delete m_editor;

	switch( m_widgetType ) {
		case Text:
		case Enum: //! @todo using kexitableview combo box editor when it's ready
		case Double: //! @todo setup validator
		case Integer: //! @todo setup validator
			m_editor = new KexiDBLineEdit( this );
			connect( m_editor, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotValueChanged() ) );
			break;
		case MultiLineText:
			m_editor = new KexiDBTextEdit( this );
			connect( m_editor, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotValueChanged() ) );
			break;
		case Boolean:
			m_editor = new KexiDBCheckBox(m_dataSource, this);
			connect( m_editor, SIGNAL(stateChanged()), this, SLOT(slotValueChanged()));
			break;
		//! \todo create db-aware spinboxes, date, time edit, etc
		case Date:
			m_editor = new KexiDBDateEdit(QDate::currentDate(), this);
			connect( m_editor, SIGNAL( dateChanged(const QDate&) ), this, SLOT( slotValueChanged() ) );
			break;
		case DateTime:
			m_editor = new KexiDBDateTimeEdit(QDateTime::currentDateTime(), this);
			connect( m_editor, SIGNAL(dateTimeChanged()), this, SLOT( slotValueChanged() ) );
			break;
		case Time:
			m_editor = new KexiDBTimeEdit(QTime::currentTime(), this);
			connect( m_editor, SIGNAL( valueChanged( const QTime& ) ), this, SLOT( slotValueChanged() ) );
			break;
/*		case Double:
			m_editor = new KexiDBDoubleSpinBox(this);
			connect( m_editor, SIGNAL( valueChanged(double) ), this, SLOT( slotValueChanged() ) );
			break;
		case Integer:
			m_editor = new KexiDBIntSpinBox(this);
			connect( m_editor, SIGNAL(valueChanged(int)), this, SLOT( slotValueChanged() ) );
			break;*/
		case Image:
			m_editor = new KexiDBImageBox(m_designMode, this);
			connect( m_editor, SIGNAL(valueChanged()), this, SLOT( slotValueChanged() ) );
			break;
		default:
			m_editor = 0;
			changeText(m_caption);
			//m_label->setText( m_dataSource.isEmpty() ? "<datasource>" : m_dataSource );
			break;
	}

	if(m_editor) {
		m_editor->setName( QCString("KexiDBAutoField_")+m_editor->className() );
		dynamic_cast<KexiDataItemInterface*>(m_editor)->setParentDataItemInterface(this);
		KFormDesigner::DesignTimeDynamicChildWidgetHandler::childWidgetAdded(this);
		m_editor->show();
		m_label->setBuddy(m_editor);
		if (m_focusPolicyChanged) {//if focusPolicy is changed at top level, editor inherits it
			m_editor->setFocusPolicy(focusPolicy());
		}
		else {//if focusPolicy is not changed at top level, inherit it from editor
			QWidget::setFocusPolicy(m_editor->focusPolicy());
		}
		setFocusProxy(m_editor); //ok?
//		KFormDesigner::installRecursiveEventFilter(m_editor, this);
	}

	setLabelPosition(labelPosition());
}

void
KexiDBAutoField::setLabelPosition(LabelPosition position)
{
	m_lblPosition = position;
	if(m_layout) {
		delete m_layout;
		m_layout = 0;
	}

	if(m_editor)
		m_editor->show();
	//! \todo support right-to-left layout where positions are inverted
	if (position==Top || position==Left) {
		int align = m_label->alignment();
		if(position == Top) {
			m_layout = (QBoxLayout*) new QVBoxLayout(this);
			align |= AlignVertical_Mask;
			align ^= AlignVertical_Mask;
			align |= AlignTop;
		}
		else {
			m_layout = (QBoxLayout*) new QHBoxLayout(this);
			align |= AlignVertical_Mask;
			align ^= AlignVertical_Mask;
			align |= AlignVCenter;
		}
		m_label->setAlignment(align);
		if(m_widgetType == Boolean)
			m_label->hide();
		else
			m_label->show();
		m_layout->addWidget(m_label);
		m_layout->addSpacing(KexiDBAutoField_SPACING);
		m_layout->addWidget(m_editor);
//		if(m_editor)
	//		m_editor->setSizePolicy(...);
	}
	else {
		m_layout = (QBoxLayout*) new QHBoxLayout(this);
		m_label->hide();
		m_layout->addWidget(m_editor);
	}
	//a hack to force layout to be refreshed (any better idea for this?)
	resize(size()+QSize(1,0));
	resize(size()-QSize(1,0));
}

void
KexiDBAutoField::setInvalidState( const QString &text )
{
	// Widget with an invalid dataSource is just a QLabel
	if (m_designMode)
		return;
	m_widgetType = Auto;
	createEditor();
	setFocusPolicy(QWidget::NoFocus);
	m_editor->setFocusPolicy(QWidget::NoFocus);
//! @todo or set this to editor's text?
	m_label->setText( text );
}

bool
KexiDBAutoField::isReadOnly() const
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->isReadOnly();
	else
		return false;
}
/*
void
KexiDBAutoField::setReadOnly(bool state)
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->setReadOnly(state);
}*/

void
KexiDBAutoField::setValueInternal(const QVariant& add, bool removeOld)
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->setValue(m_origValue, add, removeOld);
}

QVariant
KexiDBAutoField::value()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->value();
	return QVariant();
}

void
KexiDBAutoField::slotValueChanged()
{
	signalValueChanged();
}

bool
KexiDBAutoField::valueIsNull()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->valueIsNull();
	return true;
}

bool
KexiDBAutoField::valueIsEmpty()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->valueIsEmpty();
	return true;
}

bool
KexiDBAutoField::valueChanged()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	kexipluginsdbg << m_origValue  << endl;
	if(iface)
		return iface->valueChanged();
	return false;
}

void
KexiDBAutoField::installListener(KexiDataItemChangesListener* listener)
{
	KexiFormDataItemInterface::installListener(listener);
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->installListener(listener);
}

bool
KexiDBAutoField::cursorAtStart()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->cursorAtStart();
	return false;
}

bool
KexiDBAutoField::cursorAtEnd()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->cursorAtEnd();
	return false;
}

void
KexiDBAutoField::clear()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->clear();
}

void
KexiDBAutoField::setFieldTypeInternal(int kexiDBFieldType)
{
	m_fieldTypeInternal = (KexiDB::Field::Type)kexiDBFieldType;
	WidgetType type = KexiDBAutoField::widgetTypeForFieldType(
		m_fieldTypeInternal==KexiDB::Field::InvalidType ? KexiDB::Field::Text : m_fieldTypeInternal);

	if(m_widgetType != type) {
		m_widgetType = type;
		createEditor();
	}
	setFieldCaptionInternal(m_fieldCaptionInternal);
}

void
KexiDBAutoField::setFieldCaptionInternal(const QString& text)
{
	m_fieldCaptionInternal = text;
	//change text only if autocaption is set and no columnInfo is available
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if((!iface || !iface->columnInfo()) && m_autoCaption) {
		changeText(m_fieldCaptionInternal);
	}
}

void
KexiDBAutoField::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);

	// change widget type depending on field type
	if(m_widgetType_property == Auto) {
		WidgetType newWidgetType = Auto;
		KexiDB::Field::Type fieldType;
		if (cinfo)
			fieldType = cinfo->field->type();
		else if (dataSource().isEmpty())
			fieldType = KexiDB::Field::InvalidType;
		else
			fieldType = KexiDB::Field::Text;

		if (fieldType != KexiDB::Field::InvalidType) {
			newWidgetType = KexiDBAutoField::widgetTypeForFieldType( fieldType );
		}
		if(m_widgetType != newWidgetType || newWidgetType==Auto) {
			m_widgetType = newWidgetType;
			createEditor();
		}
	}
	// update label's text
	changeText((cinfo && m_autoCaption) ? cinfo->captionOrAliasOrName() : QString::null);

	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->setColumnInfo(cinfo);
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
			return Enum;
		case KexiDB::Field::InvalidType:
			return Auto;
		case KexiDB::Field::BLOB:
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
	if (m_autoCaption && (m_widgetType==Auto || dataSource().isEmpty())) {
		realText = QString::fromLatin1(name())+" "+i18n("Unbound Auto Field", " (unbound)");
		unbound = true;
	}
	else {
		if (beautify) {
	/*! @todo look at appendColonToAutoLabels setting [bool]
		@todo look at makeFirstCharacterUpperCaseInAutoLabels setting [bool]
		(see doc/dev/settings.txt) */
			if (!text.isEmpty()) {
				realText = text[0].upper();
				realText += (text.mid(1) + ": ");
			}
		}
		else
			realText = text;
	}

	QWidget* widgetToAlterForegroundColor;
	if(m_widgetType == Boolean) {
		static_cast<QCheckBox*>(m_editor)->setText(realText);
		widgetToAlterForegroundColor = m_editor;
	}
	else {
		m_label->setText(realText);
		widgetToAlterForegroundColor = m_label;
	}

/*	if (unbound)
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
	m_caption = caption;
	if(!m_autoCaption && !caption.isEmpty())
		changeText(m_caption);
}

/*void
KexiDBAutoField::setCaptionInternal(const QString& text)
{
	if(!m_autoCaption && !caption.isEmpty())
}*/

void
KexiDBAutoField::setAutoCaption(bool autoCaption)
{
	m_autoCaption = autoCaption;
	if(m_autoCaption) {
		//m_caption = QString::null;
		if(columnInfo()) {
			changeText(columnInfo()->captionOrAliasOrName());
		}
		else {
			changeText(m_fieldCaptionInternal);
		}
	}
	else
		changeText(m_caption);

//	if(!m_autoCaption && !m_caption.isEmpty())
//		changeText(m_caption);
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
	if (m_lblPosition == NoLabel)
		return m_editor ? m_editor->sizeHint() : QWidget::sizeHint();

	QSize s1(0,0);
	if (m_editor)
		s1 = m_editor->sizeHint();
	QSize s2(m_label->sizeHint());
	if (m_lblPosition == Top)
		return QSize(QMAX(s1.width(), s2.width()), s1.height()+KexiDBAutoField_SPACING+s2.height());

//	if (m_lblPosition == Left) 
	//left
	return QSize(s1.width()+KexiDBAutoField_SPACING+s2.width(), QMAX(s1.height(), s2.height()));
}

void
KexiDBAutoField::setFocusPolicy( FocusPolicy policy )
{
	m_focusPolicyChanged = true;
	QWidget::setFocusPolicy(policy);
	m_label->setFocusPolicy(policy);
	if (m_editor)
		m_editor->setFocusPolicy(policy);
}

void
KexiDBAutoField::updateInformationAboutUnboundField()
{
	if (   (m_autoCaption && (dataSource().isEmpty() || dataSourceMimeType().isEmpty()))
		|| (!m_autoCaption && m_caption.isEmpty()) )
	{
		m_label->setText( QString::fromLatin1(name())+" "+i18n("Unbound Auto Field", " (unbound)") );
	}
//	else
//		m_label->setText( QString::fromLatin1(name())+" "+i18n(" (unbound)") );
}

/*void
KexiDBAutoField::paintEvent( QPaintEvent* pe )
{
	QWidget::paintEvent( pe );

	if (   (m_autoCaption && (dataSource().isEmpty() || dataSourceMimeType().isEmpty()))
		|| (!m_autoCaption && m_caption.isEmpty()) )
	{
		QPainter p(this);
		p.setPen( m_label->paletteForegroundColor() );
		p.setClipRect(pe->rect());
		p.setFont(m_label->font());
		p.drawText(rect(), Qt::AlignLeft | Qt::WordBreak, 
			QString::fromLatin1(name())+" "+i18n(" (unbound)"));
	}
}*/

void
KexiDBAutoField::paletteChange( const QPalette& oldPal )
{
	Q_UNUSED(oldPal);
	m_label->setPalette( palette() );
}


#include "kexidbautofield.moc"
