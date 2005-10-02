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

#include "kexidbfieldedit.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>

#include "kexidbwidgets.h"
#include <kexidb/queryschema.h>
#include <formeditor/utils.h>

#define KexiDBFieldEdit_SPACING 10 //10 pixel for spacing between a label and an editor widget

KexiDBFieldEdit::KexiDBFieldEdit(const QString &text, WidgetType type, LabelPosition pos, 
	QWidget *parent, const char *name, bool designMode)
 : QWidget(parent, name)
 , KexiFormDataItemInterface()
 , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
 , m_designMode(designMode)
{
	init(text, type, pos);
}

KexiDBFieldEdit::KexiDBFieldEdit(QWidget *parent, const char *name, bool designMode)
 : QWidget(parent, name)
 , KexiFormDataItemInterface()
 , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
 , m_designMode(designMode)
{
	init(QString::null/*i18n("Auto Field")*/, Auto, Left);
}

KexiDBFieldEdit::~KexiDBFieldEdit()
{
}

void
KexiDBFieldEdit::init(const QString &text, WidgetType type, LabelPosition pos)
{
	m_fieldTypeInternal = KexiDB::Field::InvalidType;
	m_layout = 0;
	m_editor = 0;
	m_label = new QLabel(text, this);
	QFontMetrics fm( font() );
	//m_label->setFixedWidth( fm.width("This is a test string length") );
	m_autoCaption = true;
	m_focusPolicyChanged = false;
	m_widgetType_property = Auto;
	m_widgetType = Auto;
	setWidgetType(type);
	setLabelPosition(pos);
}

void
KexiDBFieldEdit::setWidgetType(WidgetType type)
{
	const bool differ = (type != m_widgetType_property);
	m_widgetType_property = type;
	if(differ) {
		if(type == Auto) // try to guess type from data source type
			m_widgetType = widgetTypeForFieldType(
				columnInfo() ? columnInfo()->field->type() : KexiDB::Field::Text);
		else
			m_widgetType = m_widgetType_property;
		createEditor();
	}
}

void
KexiDBFieldEdit::createEditor()
{
	if(m_editor)
		delete m_editor;

	switch( m_widgetType ) {
		case Text: case Enum: //! \todo using kexitableview combo box editor when it's ready
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
		case Double:
			m_editor = new KexiDBDoubleSpinBox(this);
			connect( m_editor, SIGNAL( valueChanged(double) ), this, SLOT( slotValueChanged() ) );
			break;
		case Integer:
			m_editor = new KexiDBIntSpinBox(this);
			connect( m_editor, SIGNAL(valueChanged(int)), this, SLOT( slotValueChanged() ) );
			break;
		case Image:
			m_editor = new KexiImageBox(m_designMode, this);
			connect( m_editor, SIGNAL(valueChanged()), this, SLOT( slotValueChanged() ) );
			break;
		default:
			m_editor = 0;
			m_label->setText( m_dataSource.isEmpty() ? "<datasource>" : m_dataSource );
			break;
	}

	if(m_editor) {
		m_editor->setName( QCString("KexiDBFieldEdit_")+m_editor->className() );
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
//		KFormDesigner::installRecursiveEventFilter(m_editor, this);
	}

	setLabelPosition(labelPosition());
}

void
KexiDBFieldEdit::setLabelPosition(LabelPosition position)
{
	m_lblPosition = position;
	if(m_layout) {
		delete m_layout;
		m_layout = 0;
	}

	if(m_editor)
		m_editor->show();
	//! \todo support right-to-left layout where positions are inverted
	switch(position) {
		case Top: case Left:
			if(position == Top)
				m_layout = (QBoxLayout*) new QVBoxLayout(this);
			else
				m_layout = (QBoxLayout*) new QHBoxLayout(this);
			if(m_widgetType == Boolean)
				m_label->hide();
			else
				m_label->show();
			m_layout->addWidget(m_label);
			m_layout->addSpacing(KexiDBFieldEdit_SPACING);
			m_layout->addWidget(m_editor);
			break;

		default:
			m_layout = (QBoxLayout*) new QHBoxLayout(this);
			m_label->hide();
			m_layout->addWidget(m_editor);
	}
}

void
KexiDBFieldEdit::setInvalidState( const QString &text )
{
	// Widget with an invalid dataSource is just a QLabel
	m_widgetType = Auto;
	createEditor();
	setFocusPolicy(QWidget::NoFocus);
	m_editor->setFocusPolicy(QWidget::NoFocus);
//! @todo or set this to editor's text?
	m_label->setText( text );
}

bool
KexiDBFieldEdit::isReadOnly() const
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->isReadOnly();
	else
		return false;
}
/*
void
KexiDBFieldEdit::setReadOnly(bool state)
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->setReadOnly(state);
}*/

void
KexiDBFieldEdit::setValueInternal(const QVariant& add, bool removeOld)
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->setValue(m_origValue, add, removeOld);
}

QVariant
KexiDBFieldEdit::value()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->value();
	return QVariant();
}

void
KexiDBFieldEdit::slotValueChanged()
{
	signalValueChanged();
}

bool
KexiDBFieldEdit::valueIsNull()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->valueIsNull();
	return true;
}

bool
KexiDBFieldEdit::valueIsEmpty()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->valueIsEmpty();
	return true;
}

bool
KexiDBFieldEdit::valueChanged()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	kexipluginsdbg << m_origValue  << endl;
	if(iface)
		return iface->valueChanged();
	return false;
}

void
KexiDBFieldEdit::installListener(KexiDataItemChangesListener* listener)
{
	KexiFormDataItemInterface::installListener(listener);
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->installListener(listener);
}

bool
KexiDBFieldEdit::cursorAtStart()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->cursorAtStart();
	return false;
}

bool
KexiDBFieldEdit::cursorAtEnd()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		return iface->cursorAtEnd();
	return false;
}

void
KexiDBFieldEdit::clear()
{
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->clear();
}

void
KexiDBFieldEdit::setFieldTypeInternal(int kexiDBFieldType)
{
	m_fieldTypeInternal = (KexiDB::Field::Type)kexiDBFieldType;
	WidgetType type = KexiDBFieldEdit::widgetTypeForFieldType(
		m_fieldTypeInternal==KexiDB::Field::InvalidType ? KexiDB::Field::Text : m_fieldTypeInternal);

	if(m_widgetType != type) {
		m_widgetType = type;
		createEditor();
	}
}

void
KexiDBFieldEdit::setFieldCaptionInternal(const QString& text)
{
	m_fieldCaptionInternal = text;
	//change text only if autocaption is set and no columnInfo is available
	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if((!iface || !iface->columnInfo()) && m_autoCaption) {
		changeText(m_fieldCaptionInternal);
	}
}

void
KexiDBFieldEdit::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	// first, update label's text
	if(cinfo && m_autoCaption)
		changeText(cinfo->captionOrAliasOrName());

	// change widget type depending on field type
	WidgetType type;
	if(m_widgetType_property == Auto) {
		type = KexiDBFieldEdit::widgetTypeForFieldType(cinfo ? cinfo->field->type() : KexiDB::Field::Text);
		if(m_widgetType != type) {
			m_widgetType = type;
			createEditor();
		}
	}

	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->setColumnInfo(cinfo);
}

//static
KexiDBFieldEdit::WidgetType
KexiDBFieldEdit::widgetTypeForFieldType(KexiDB::Field::Type type)
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
		case KexiDB::Field::BLOB:
		default:
			break;
	}
	return Text;
}

void
KexiDBFieldEdit::changeText(const QString &text, bool beautify)
{
	QString realText;
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

	if(m_widgetType == Boolean)
		static_cast<QCheckBox*>(m_editor)->setText(realText);
	else
		m_label->setText(realText);
}

void
KexiDBFieldEdit::setCaption(const QString &caption)
{
	m_caption = caption;
	if(!m_autoCaption && !caption.isEmpty())
		changeText(m_caption);
}

/*void
KexiDBFieldEdit::setCaptionInternal(const QString& text)
{
	if(!m_autoCaption && !caption.isEmpty())
}*/

void
KexiDBFieldEdit::setAutoCaption(bool autoCaption)
{
	m_autoCaption = autoCaption;
	if(m_autoCaption) {
		m_caption = QString::null;
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
KexiDBFieldEdit::setDataSource( const QString &ds ) {
	KexiFormDataItemInterface::setDataSource(ds);
}

QSize
KexiDBFieldEdit::sizeHint() const
{
	if (m_lblPosition == NoLabel)
		return m_editor ? m_editor->sizeHint() : QWidget::sizeHint();

	QSize s1(0,0);
	if (m_editor)
		s1 = m_editor->sizeHint();
	QSize s2(m_label->sizeHint());
	if (m_lblPosition == Top)
		return QSize(QMAX(s1.width(), s2.width()), s1.height()+KexiDBFieldEdit_SPACING+s2.height());

//	if (m_lblPosition == Left) 
	//left
	return QSize(s1.width()+KexiDBFieldEdit_SPACING+s2.width(), QMAX(s1.height(), s2.height()));
}

void
KexiDBFieldEdit::setFocusPolicy( FocusPolicy policy )
{
	m_focusPolicyChanged = true;
	QWidget::setFocusPolicy(policy);
	m_label->setFocusPolicy(policy);
	if (m_editor)
		m_editor->setFocusPolicy(policy);
}

#include "kexidbfieldedit.moc"
