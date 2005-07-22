/* This file is part of the KDE project
  Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
  Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexidbfieldedit.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>

#include <kexidb/field.h>
#include "kexidbwidgets.h"


KexiDBFieldEdit::KexiDBFieldEdit(const QString &text, WidgetType type, LabelPosition pos, QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	init(text, type, pos);
}

KexiDBFieldEdit::KexiDBFieldEdit(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	init(i18n("Auto Field"), Auto, Left);
}

KexiDBFieldEdit::~KexiDBFieldEdit()
{}

void
KexiDBFieldEdit::init(const QString &text, WidgetType type, LabelPosition pos)
{
	m_layout = 0;
	m_editor = 0;
	m_label = new QLabel(text, this);
	QFontMetrics fm( font() );
	//m_label->setFixedWidth( fm.width("This is a test string length") );
	m_autoCaption = true;
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
			m_widgetType = widgetTypeFromFieldType();
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
		case Bool:
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
		default:
			m_editor = 0;
			m_label->setText( m_dataSource.isEmpty() ? "<datasource>" : m_dataSource );
			break;
	}

	if(m_editor) {
		m_editor->show();
		m_label->setBuddy(m_editor);
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
			if(m_widgetType == Bool)
				m_label->hide();
			else
				m_label->show();
			m_layout->addWidget(m_label);
			m_layout->addSpacing(10);
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
		iface->setValueInternal(add, removeOld);
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
KexiDBFieldEdit::setField(KexiDB::Field* field)
{
	KexiFormDataItemInterface::setField(field);
	// first, update label's text
	if(field && m_autoCaption)
		changeText(field->captionOrName());

	// change widget type depending on field type
	WidgetType type;
	if(m_widgetType_property == Auto) {
		type = widgetTypeFromFieldType();
		if(m_widgetType != type) {
			m_widgetType = type;
			createEditor();
		}
	}

	KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(m_editor);
	if(iface)
		iface->setField(field);
}

KexiDBFieldEdit::WidgetType
KexiDBFieldEdit::widgetTypeFromFieldType()
{
	if(!field())
		return Text;

	WidgetType type = Text;
	switch(field()->type()) {
		case KexiDB::Field::Integer: case KexiDB::Field::ShortInteger: case KexiDB::Field::BigInteger:
			type = Integer; break;
		case  KexiDB::Field::Boolean:
			type = Bool; break;
		case KexiDB::Field::Float: case KexiDB::Field::Double:
			type = Double; break;
		case KexiDB::Field::Date:
			type = Date; break;
		case KexiDB::Field::DateTime:
			type = DateTime; break;
		case KexiDB::Field::Time:
			type = Time; break;
		case KexiDB::Field::Text:
			type = Text; break;
		case KexiDB::Field::LongText:
			type = MultiLineText; break;
		case KexiDB::Field::Enum:
			type = Enum; break;
		case KexiDB::Field::BLOB:
		default:
			break;
	}
	return type;
}

void
KexiDBFieldEdit::changeText(const QString &text)
{
	if(m_widgetType == Bool)
		static_cast<QCheckBox*>(m_editor)->setText(text);
	else
		m_label->setText(text);
}

void
KexiDBFieldEdit::setCaption(const QString &caption)
{
	m_caption = caption;
	if(!m_autoCaption && !caption.isEmpty())
		changeText(caption);
}

void
KexiDBFieldEdit::setAutoCaption(bool autoCaption)
{
	m_autoCaption = autoCaption;
	if(!m_autoCaption && !m_caption.isEmpty())
		changeText(m_caption);
}

#include "kexidbfieldedit.moc"

