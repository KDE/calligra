/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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

KoUnitDoubleValidator::KoUnitDoubleValidator( VUnitDoubleBase *base, QObject *parent, const char *name )
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
		else if( s.endsWith( "in" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_INCH ), m_base->m_unit );
		else if( s.endsWith( "pt" ) )
			newVal = KoUnit::ptToUnit( KoUnit::ptFromUnit( value, KoUnit::U_PT ), m_base->m_unit );
		else if( s.at( pos - 2 ).isDigit() && ( s.endsWith( "m" ) || s.endsWith( "c" ) || s.endsWith( "i" ) || s.endsWith( "p" ) ) )
			result = Intermediate;
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

VUnitDoubleSpinBox::VUnitDoubleSpinBox( QWidget *parent, double lower, double upper, double step, double value, unsigned int precision, const char *name )
	: KDoubleSpinBox( lower, upper, step, value, precision, parent, name ), VUnitDoubleBase( precision )
{
	m_validator = new KoUnitDoubleValidator( this, this );
	setValidator( m_validator );
	setAcceptLocalizedNumbers( true );
}

void
VUnitDoubleSpinBox::setValidator( const QValidator *v )
{
	QSpinBox::setValidator( v );
}

void
VUnitDoubleSpinBox::changeValue( double val )
{
	setValue( val );
}

void
VUnitDoubleSpinBox::setUnit( KoUnit::Unit unit )
{
	setValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( value(), m_unit ), unit ) );
	m_unit = unit;
	setSuffix( KoUnit::unitName( unit ) );
}


VUnitDoubleLineEdit::VUnitDoubleLineEdit( QWidget *parent, double lower, double upper, double value, unsigned int precision, const char *name )
	: QLineEdit( parent, name ), VUnitDoubleBase( precision ), m_value( value )
{
	setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	setValidator( m_validator );
	changeValue( value );
}

void
VUnitDoubleLineEdit::setValidator( const QValidator *v )
{
	QLineEdit::setValidator( v );
}

void
VUnitDoubleLineEdit::changeValue( double value )
{
	setValue( value );
	setText( QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ) ).arg( KoUnit::unitName( m_unit ) ) );
}

void
VUnitDoubleLineEdit::setUnit( KoUnit::Unit unit )
{
	KoUnit::Unit old = m_unit;
	m_unit = unit;
	changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( m_value, old ), unit ) );
}


VUnitDoubleComboBox::VUnitDoubleComboBox( QWidget *parent, double lower, double upper, double value, unsigned int precision, const char *name )
	: QComboBox( true, parent, name ), VUnitDoubleBase( precision ), m_value( value )
{
	lineEdit()->setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	lineEdit()->setValidator( m_validator );
	changeValue( value );
}

void
VUnitDoubleComboBox::changeValue( double value )
{
	setValue( value );
	lineEdit()->setText( QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ) ).arg( KoUnit::unitName( m_unit ) ) );
}

void
VUnitDoubleComboBox::setUnit( KoUnit::Unit unit )
{
	KoUnit::Unit old = m_unit;
	m_unit = unit;
	changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( m_value, old ), unit ) );
}

bool
VUnitDoubleComboBox::eventFilter( QObject* o, QEvent* ev )
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

