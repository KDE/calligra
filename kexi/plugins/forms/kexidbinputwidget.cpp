/* This file is part of the KDE project
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

#include <qlabel.h>
#include <qlayout.h>

#include <kdatewidget.h>
#include <kdatetimewidget.h>
#include <klineedit.h>
#include <knuminput.h>
#include <ktextedit.h>
#include <ktimewidget.h>

#include <kdebug.h>

#include "kexidbinputwidget.h"

#define INTMIN -2147483648
#define INTMAX 2147483647
#define PRECISION 2
#define MAXLENGTH 32767

double KexiDBInputWidget::getDoubleRangeFromPrecision( int precision ) {
	QString base = "99999999";
	base.truncate( 8 - precision );
	return base.toDouble();
}

KexiDBInputWidget::KexiDBInputWidget( WidgetType type, QWidget *parent, const char *name )
	: QWidget( parent, name ), p_widget( 0 ) {

	p_intMin = INTMIN;
	p_intMax = INTMAX;
	p_precision = PRECISION;
	p_doubleMax = getDoubleRangeFromPrecision( PRECISION );
	p_doubleMin = -p_doubleMax;
	p_maxStringLength = MAXLENGTH;
	p_readOnly = false;
	
	(new QVBoxLayout( this ))->setAutoAdd( true );
	
	setWidgetType( type );
}

KexiDBInputWidget::KexiDBInputWidget( QWidget *parent, const char *name )
	: QWidget( parent, name ), p_widget( 0 ) {

	p_intMin = INTMIN;
	p_intMax = INTMAX;
	p_precision = PRECISION;
	p_doubleMax = getDoubleRangeFromPrecision( PRECISION );
	p_doubleMin = -p_doubleMax;
	p_maxStringLength = MAXLENGTH;
	p_readOnly = false;
	
	(new QVBoxLayout( this ))->setAutoAdd( true );
	
	setWidgetType( Undefined );
}

KexiDBInputWidget::~KexiDBInputWidget() {
}

void KexiDBInputWidget::setReadOnly( bool state ) {
	p_readOnly = state;
	
	if ( dynamic_cast<QLineEdit*>( p_widget ) != 0L ) {
		dynamic_cast<QLineEdit*>( p_widget )->setReadOnly( state );
		return;
	}
	if ( dynamic_cast<QTextEdit*>( p_widget ) != 0L ) {
		dynamic_cast<QTextEdit*>( p_widget )->setReadOnly( state );
		return;
	}
	
	/*
	Every widget missing a readonly-mode will be disabled.
	*/
	p_widget->setDisabled( state );
}

void KexiDBInputWidget::setValueInternal( const QVariant& value ) {
	if ( dynamic_cast<KDateWidget*>( p_widget ) != 0L ) {
		dynamic_cast<KDateWidget*>( p_widget )->setDate( value.toDate() );
		return;
	}
	if ( dynamic_cast<KDateTimeWidget*>( p_widget ) != 0L ) {
		dynamic_cast<KDateTimeWidget*>( p_widget )->setDateTime( value.toDateTime() );
		return;
	}
	if ( dynamic_cast<KTimeWidget*>( p_widget ) != 0L ) {
		dynamic_cast<KTimeWidget*>( p_widget )->setTime( value.toTime() );
		return;
	}
	if ( dynamic_cast<QLabel*>( p_widget ) != 0L ) {
		dynamic_cast<QLabel*>( p_widget )->setText( value.toString() );
		return;
	}
	if ( dynamic_cast<QLineEdit*>( p_widget ) != 0L ) {
		dynamic_cast<QLineEdit*>( p_widget )->setText( value.toString() );
		return;
	}
	if ( dynamic_cast<QTextEdit*>( p_widget ) != 0L ) {
		dynamic_cast<QTextEdit*>( p_widget )->setText( value.toString() );
		return;
	}
	if ( dynamic_cast<KDoubleSpinBox*>( p_widget ) != 0L ) {
		dynamic_cast<KDoubleSpinBox*>( p_widget )->setValue( value.toDouble() );
		return;
	}
	if ( dynamic_cast<QSpinBox*>( p_widget ) != 0L ) {
		dynamic_cast<QSpinBox*>( p_widget )->setValue( value.toInt() );
		return;
	}
}

void KexiDBInputWidget::reinit() {
	if ( p_widget != 0L ) {
		delete p_widget;
	}
	
	switch( p_widgetType ) {
		case Date:
			p_widget = new KDateWidget( this );
			connect( p_widget, SIGNAL( valueChanged( const QDate& ) ), this, SLOT( slotValueChanged() ) );
			break;
		case DateTime:
			p_widget = new KDateTimeWidget( this );
			connect( p_widget, SIGNAL( valueChanged( const QDateTime& ) ), this, SLOT( slotValueChanged() ) );
			break;
		case Decimal:
			p_widget = new KDoubleSpinBox( p_doubleMin, p_doubleMax, 1.0, 0.0, 2, this );
			connect( p_widget, SIGNAL( valueChanged( double ) ), this, SLOT( slotValueChanged() ) );
			break;
		case Integer:
			p_widget = new KIntSpinBox( p_intMin, p_intMax, 1, 0, 10, this );
			connect( p_widget, SIGNAL( valueChanged( int ) ), this, SLOT( slotValueChanged() ) );
			break;
		case Text:
			p_widget = new KLineEdit( this );
			((KLineEdit*)p_widget)->setMaxLength( p_maxStringLength );
			connect( p_widget, SIGNAL( valueChanged( const QString& ) ), this, SLOT( slotValueChanged( const QString& ) ) );
			break;
		case Time:
			p_widget = new KTimeWidget( this );
			connect( p_widget, SIGNAL( valueChanged( const QTime& ) ), this, SLOT( slotValueChanged() ) );
			break;
		case MultiLineText:
			p_widget = new KTextEdit( this );
			connect( p_widget, SIGNAL( valueChanged( const QString& ) ), this, SLOT( slotValueChanged( const QString& ) ) );
			break;
		default:
			p_widget = new QLabel( this );
			((QLabel*)p_widget)->setFrameShape( QFrame::Box );
			((QLabel*)p_widget)->setText( m_dataSource.isEmpty() ? "<datasource>" : m_dataSource );
			break;
	}
	p_widget->show();
	setMinimumSize( p_widget->minimumSize() );
	if ( size().width() < minimumSize().width()
		|| size().height() < minimumSize().height() ) {
		resize( minimumSize() );
	}
}

void KexiDBInputWidget::setInvalidState( const QString& text ) {
	/*
	Widgets with an invalid dataSource are always a QLabel
	Setting this explicitly until the widgettype is set automatically
	*/
	setWidgetType( Undefined );
	QLabel* lbl = dynamic_cast<QLabel*>( p_widget );
	if ( lbl != 0L ) {
		lbl->setText( text );
	}
}

QVariant KexiDBInputWidget::value() {
	if ( dynamic_cast<KDateWidget*>( p_widget ) != 0L ) {
		return dynamic_cast<KDateWidget*>( p_widget )->date();
	}
	if ( dynamic_cast<KDateTimeWidget*>( p_widget ) != 0L ) {
		return dynamic_cast<KDateTimeWidget*>( p_widget )->dateTime();
	}
	if ( dynamic_cast<KTimeWidget*>( p_widget ) != 0L ) {
		return dynamic_cast<KTimeWidget*>( p_widget )->time();
	}
	if ( dynamic_cast<QLabel*>( p_widget ) != 0L ) {
		return dynamic_cast<QLabel*>( p_widget )->text();
	}
	if ( dynamic_cast<QLineEdit*>( p_widget ) != 0L ) {
		return dynamic_cast<QLineEdit*>( p_widget )->text();
	}
	if ( dynamic_cast<QTextEdit*>( p_widget ) != 0L ) {
		return dynamic_cast<QTextEdit*>( p_widget )->text();
	}
	if ( dynamic_cast<KDoubleSpinBox*>( p_widget ) != 0L ) {
		return dynamic_cast<KDoubleSpinBox*>( p_widget )->value();
	}
	if ( dynamic_cast<QSpinBox*>( p_widget ) != 0L ) {
		return dynamic_cast<QSpinBox*>( p_widget )->value();
	}
	return QVariant();
}

void KexiDBInputWidget::slotValueChanged( const QString& ) {
	valueChanged();
}

void KexiDBInputWidget::slotValueChanged() {
	valueChanged();
}

void KexiDBInputWidget::setMinValue( int value ) {
	QSpinBox* box = dynamic_cast<QSpinBox*>( p_widget );
	if ( box != 0L ) {
		box->setMinValue( value );
	}
	p_intMin = value;
}

void KexiDBInputWidget::setMaxValue( int value ) {
	QSpinBox* box = dynamic_cast<QSpinBox*>( p_widget );
	if ( box != 0L ) {
		box->setMaxValue( value );
	}
	p_intMax = value;
}

void KexiDBInputWidget::setMinValue( double value ) {
	KDoubleSpinBox* box = dynamic_cast<KDoubleSpinBox*>( p_widget );
	if ( box != 0L ) {
		box->setMinValue( value );
	}
	p_doubleMin = value;
}

void KexiDBInputWidget::setMaxValue( double value ) {
	KDoubleSpinBox* box = dynamic_cast<KDoubleSpinBox*>( p_widget );
	if ( box != 0L ) {
		box->setMaxValue( value );
	}
	p_doubleMax = value;
}

void KexiDBInputWidget::setMaxLength( int value ) {
	QLineEdit* edit = dynamic_cast<QLineEdit*>( p_widget );
	if ( edit != 0L ) {
		edit->setMaxLength( value );
	}
	p_maxStringLength = value;
}

void KexiDBInputWidget::setPrecision( int precision ) {
	KDoubleSpinBox* box = dynamic_cast<KDoubleSpinBox*>( p_widget );
	if ( box != 0L ) {
		box->setPrecision( precision );
	}
	p_precision = precision;
}

#include "kexidbinputwidget.moc"
