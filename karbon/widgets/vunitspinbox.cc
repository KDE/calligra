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

#include "vunitspinbox.h"
#include <kglobal.h>
#include <klocale.h>

KoUnitDoubleValidator::KoUnitDoubleValidator( KoUnitDoubleBase *base, QObject *parent, const char *name )
: KDoubleValidator( parent, name ), m_base( base )
{
}

QValidator::State
KoUnitDoubleValidator::validate( QString &s, int &pos ) const
{
	QValidator::State result = Valid;
	bool ok = false;
	QString s2 = s;
	double value = s2.replace( ',', "." ).toDouble( &ok );
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
		s = QString( "%1%2").arg( KGlobal::locale()->formatNumber( newVal, m_base->m_precision ) ).arg( KoUnit::unitName( m_base->m_unit ) );
	}
	return result;
}

KoUnitDoubleSpinBox::KoUnitDoubleSpinBox( QWidget *parent, double lower, double upper, double step, double value, unsigned int precision, const char *name )
	: KDoubleSpinBox( lower, upper, step, value, precision, parent, name ), KoUnitDoubleBase( precision )
{
	m_validator = new KoUnitDoubleValidator( this, this );
	QSpinBox::setValidator( m_validator );
	setAcceptLocalizedNumbers( true );
}

void
KoUnitDoubleSpinBox::changeValue( double val )
{
	KDoubleSpinBox::setValue( val );
}

void
KoUnitDoubleSpinBox::setUnit( KoUnit::Unit unit )
{
	setMinValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( minValue(), m_unit ), unit ) );
	setMaxValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( maxValue(), m_unit ), unit ) );
	KDoubleSpinBox::setValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( value(), m_unit ), unit ) );
	m_unit = unit;
	setSuffix( KoUnit::unitName( unit ) );
}


KoUnitDoubleLineEdit::KoUnitDoubleLineEdit( QWidget *parent, double lower, double upper, double value, unsigned int precision, const char *name )
	: KLineEdit( parent, name ), KoUnitDoubleBase( precision ), m_value( value ), m_lower( lower ), m_upper( upper )
{
	setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	setValidator( m_validator );
	changeValue( value );
}

void
KoUnitDoubleLineEdit::changeValue( double value )
{
	setValue( value );
	setText( QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ) ).arg( KoUnit::unitName( m_unit ) ) );
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
		double value = text().replace( ',', "." ).toDouble( &ok );
		changeValue( value );
		return false;
	}
	else
		return QLineEdit::eventFilter( o, ev );
}



KoUnitDoubleComboBox::KoUnitDoubleComboBox( QWidget *parent, double lower, double upper, double value, unsigned int precision, const char *name )
	: KComboBox( true, parent, name ), KoUnitDoubleBase( precision ), m_value( value ), m_lower( lower ), m_upper( upper )
{
	lineEdit()->setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	lineEdit()->setValidator( m_validator );
	changeValue( value );
}

void
KoUnitDoubleComboBox::changeValue( double value )
{
	setValue( value );
	lineEdit()->setText( QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ) ).arg( KoUnit::unitName( m_unit ) ) );
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
		double value = lineEdit()->text().replace( ',', "." ).toDouble( &ok );
		changeValue( value );
		return false;
	}
	else
		return QComboBox::eventFilter( o, ev );
}

