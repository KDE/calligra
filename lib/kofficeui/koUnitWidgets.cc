/* This file is part of the KDE project
   Copyright (C) 2002, Rob Buis(buis@kde.org)

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

#include "koUnitWidgets.moc"
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qlayout.h>

KoUnitDoubleValidator::KoUnitDoubleValidator( KoUnitDoubleBase *base, QObject *parent, const char *name )
: KDoubleValidator( parent, name ), m_base( base )
{
}

QValidator::State
KoUnitDoubleValidator::validate( QString &s, int &pos ) const
{

        kdDebug(30004) << "KoUnitDoubleValidator::validate : " << s << " at " << pos << endl;
	QValidator::State result = Valid;
	bool ok = false;
	double value = m_base->toDouble( s, &ok );
	double newVal = -1;
	if( !ok )
	{
		if( s.endsWith( "mm" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_MM ), m_base->m_unit );
		else if( s.endsWith( "cm" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_CM ), m_base->m_unit );
		else if( s.endsWith( "dm" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_DM ), m_base->m_unit );
		else if( s.endsWith( "in" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_INCH ), m_base->m_unit );
		else if( s.endsWith( "pt" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_PT ), m_base->m_unit );
		else if( s.endsWith( "dd" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_DD ), m_base->m_unit );
		else if( s.endsWith( "cc" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_CC ), m_base->m_unit );
		else if( s.endsWith( "pi" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_PI ), m_base->m_unit );
		else if(
			s.at( pos - 2 ).isDigit() &&
			(
				s.endsWith( "m" ) ||
				s.endsWith( "c" ) ||
				s.endsWith( "d" ) ||
				s.endsWith( "i" ) ||
				s.endsWith( "p" ) ) )
		{
			result = Intermediate;
		}
		else
			return KDoubleValidator::validate( s, pos );
	}
	if( newVal >= 0.0 )
	{
		m_base->changeValue( newVal );
		s = m_base->getVisibleText( newVal );
	}
	return result;
}


QString KoUnitDoubleBase::getVisibleText( double value ) const
{
    return QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ) ).arg( KoUnit::unitName( m_unit ) );
}

double KoUnitDoubleBase::toDouble( const QString& str, bool* ok ) const
{
    QString str2 ( str );
    // ### TODO: 1000 separator, correct decimal separator, negative sign...
    return str2.replace( ',', "." ).toDouble( ok );
}


KoUnitDoubleSpinBox::KoUnitDoubleSpinBox( QWidget *parent, double lower, double upper, double step, double value, KoUnit::Unit unit, unsigned int precision, const char *name )
	: KDoubleSpinBox( lower, upper, step, value, precision, parent, name ), KoUnitDoubleBase( unit, precision )
{
	m_validator = new KoUnitDoubleValidator( this, this );
	QSpinBox::setValidator( m_validator );
	setAcceptLocalizedNumbers( true );
	setUnit( unit );
}

void
KoUnitDoubleSpinBox::changeValue( double val )
{
	KDoubleSpinBox::setValue( val );
}

void
KoUnitDoubleSpinBox::setUnit( KoUnit::Unit unit )
{
	double oldvalue = KoUnit::ptFromUnit( value(), m_unit );
	setMinValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( minValue(), m_unit ), unit ) );
	setMaxValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( maxValue(), m_unit ), unit ) );
	KDoubleSpinBox::setValue( KoUnit::ptToUnit( oldvalue, unit ) );
	m_unit = unit;
	setSuffix( KoUnit::unitName( unit ).prepend( ' ' ) );
}


KoUnitDoubleLineEdit::KoUnitDoubleLineEdit( QWidget *parent, double lower, double upper, double value, KoUnit::Unit unit, unsigned int precision, const char *name )
	: KLineEdit( parent, name ), KoUnitDoubleBase( unit, precision ), m_value( value ), m_lower( lower ), m_upper( upper )
{
	setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	setValidator( m_validator );
	changeValue( value );
}

void
KoUnitDoubleLineEdit::changeValue( double value )
{
	m_value = value < m_lower ? m_lower : ( value > m_upper ? m_upper : value );
	setText( getVisibleText( m_value ) );
}

void
KoUnitDoubleLineEdit::setUnit( KoUnit::Unit unit )
{
	KoUnit::Unit old = m_unit;
	m_unit = unit;
	m_lower = KoUnit::ptToUnit( KoUnit::ptFromUnit( m_lower, old ), unit );
	m_upper = KoUnit::ptToUnit( KoUnit::ptFromUnit( m_upper, old ), unit );
	changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( m_value, old ), unit ) );
}

bool
KoUnitDoubleLineEdit::eventFilter( QObject* o, QEvent* ev )
{
	if( ev->type() == QEvent::FocusOut || ev->type() == QEvent::Leave || ev->type() == QEvent::Hide )
	{
		bool ok;
		double value = toDouble( text(), &ok );
		changeValue( value );
		return false;
	}
	else
		return QLineEdit::eventFilter( o, ev );
}



KoUnitDoubleComboBox::KoUnitDoubleComboBox( QWidget *parent, double lower, double upper, double value, KoUnit::Unit unit, unsigned int precision, const char *name )
	: KComboBox( true, parent, name ), KoUnitDoubleBase( unit, precision ), m_value( value ), m_lower( lower ), m_upper( upper )
{
	lineEdit()->setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	lineEdit()->setValidator( m_validator );
	changeValue( value );
	connect( this, SIGNAL( activated( int ) ), this, SLOT( slotActivated( int ) ) );
}

void
KoUnitDoubleComboBox::changeValue( double value )
{
	QString oldLabel = lineEdit()->text();
	updateValue( value );
	if( lineEdit()->text() != oldLabel )
		emit valueChanged( m_value );
}

void
KoUnitDoubleComboBox::updateValue( double value )
{
	m_value = value < m_lower ? m_lower : ( value > m_upper ? m_upper : value );
	lineEdit()->setText( getVisibleText( m_value ) );
}

void
KoUnitDoubleComboBox::insertItem( double value, int index )
{
	KComboBox::insertItem( getVisibleText( value ), index );
}

void
KoUnitDoubleComboBox::slotActivated( int index )
{
	double oldvalue = m_value;
	bool ok;
	double value = toDouble( text( index ), &ok );
	m_value = value < m_lower ? m_lower : ( value > m_upper ? m_upper : value );
	if( m_value != oldvalue )
		emit valueChanged( m_value );
}

void
KoUnitDoubleComboBox::setUnit( KoUnit::Unit unit )
{
	KoUnit::Unit old = m_unit;
	m_unit = unit;
	m_lower = KoUnit::ptToUnit( KoUnit::ptFromUnit( m_lower, old ), unit );
	m_upper = KoUnit::ptToUnit( KoUnit::ptFromUnit( m_upper, old ), unit );
	changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( m_value, old ), unit ) );
}

bool
KoUnitDoubleComboBox::eventFilter( QObject* o, QEvent* ev )
{
	if( ev->type() == QEvent::FocusOut || ev->type() == QEvent::Leave || ev->type() == QEvent::Hide )
	{
		bool ok;
		double value = toDouble( lineEdit()->text(), &ok );
		changeValue( value );
		return false;
	}
	else
		return QComboBox::eventFilter( o, ev );
}

KoUnitDoubleSpinComboBox::KoUnitDoubleSpinComboBox( QWidget *parent, double lower, double upper, double step, double value, KoUnit::Unit unit, unsigned int precision, const char *name )
	: QWidget( parent ), m_step( step )
{
	QGridLayout *layout = new QGridLayout( this, 2, 3 );
	//layout->setMargin( 2 );
	QPushButton *up = new QPushButton( "+", this );
	//up->setFlat( true );
	up->setMaximumHeight( 15 );
	up->setMaximumWidth( 15 );
	layout->addWidget( up, 0, 0 );
	connect( up, SIGNAL( clicked() ), this, SLOT( slotUpClicked() ) );

	QPushButton *down = new QPushButton( "-", this );
	down->setMaximumHeight( 15 );
	down->setMaximumWidth( 15 );
	layout->addWidget( down, 1, 0 );
	connect( down, SIGNAL( clicked() ), this, SLOT( slotDownClicked() ) );

	m_combo = new KoUnitDoubleComboBox( this, lower, upper, value, unit, precision, name );
	connect( m_combo, SIGNAL( valueChanged( double ) ), this, SIGNAL( valueChanged( double ) ) );
	layout->addMultiCellWidget( m_combo, 0, 1, 2, 2 );
}

void
KoUnitDoubleSpinComboBox::slotUpClicked()
{
	m_combo->changeValue( m_combo->value() + m_step );
}

void
KoUnitDoubleSpinComboBox::slotDownClicked()
{
	m_combo->changeValue( m_combo->value() - m_step );
}

void
KoUnitDoubleSpinComboBox::insertItem( double value, int index )
{
	m_combo->insertItem( value, index );
}

void
KoUnitDoubleSpinComboBox::updateValue( double value )
{
	m_combo->updateValue( value );
}

double
KoUnitDoubleSpinComboBox::value() const
{
	return m_combo->value();
}

