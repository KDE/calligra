/* This file is part of the KDE project
   Copyright (C) 2002, Rob Buis(buis@kde.org)
   Copyright (C) 2004, Nicolas GOUTTE <goutte@kde.org>

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
    QValidator::State result = Acceptable;

    QRegExp regexp ("([ a-zA-Z]+)$"); // Letters or spaces at end
    const int res = regexp.search( s );
    QString number, unit;
    
    // ### TODO: are all the QString::stripWhiteSpace really necessary?
    if ( res > -1 )
    {
        number = s.left( res ).stripWhiteSpace();
        unit = regexp.cap( 1 ).stripWhiteSpace().lower();
    }
    else
    {
        number = s.stripWhiteSpace();
    }
    kdDebug(30004) << "Split:" << number << ":" << unit << ":" << endl;
    
    bool ok = false;
    const double value = m_base->toDouble( number, &ok );
    double newVal = 0.0;
    if( ok )
    {
        // ### TODO: see if KoUnit has not something similar
        if ( unit.isEmpty() )
            newVal = value; // ### TODO: verify if this is not "Intermediate" instead
        else if( unit == "mm"  )
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_MM );
        else if( unit == "cm" ) 
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_CM );
        else if( unit == "dm" ) 
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_DM );
        else if( unit == "in" )
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_INCH );
        else if( unit == "pt" )
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_PT );
        else if( unit == "dd" )
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_DD );
        else if( unit == "cc" )
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_CC );
        else if( unit == "pi" )
            newVal = KoUnit::ptFromUnit( value, KoUnit::U_PI );
#if 1
        else
        {
            // Probably the user is trying to edit the unit
            kdDebug(30004) << "Intermediate" << endl;    
            return Intermediate;
        }
#else
        else if( unit.startsWith( "m" ) || unit.startsWith( "c" ) || unit.startsWith( "d" ) || unit.startsWith( "i" ) || unit.startsWith( "p" ) )
        {
            kdDebug(30004) << "Intermediate" << endl;    
            return Intermediate;
        }
        else
        {
            kdWarning(30004) << "Unknown unit: " << unit << endl;    
            return Invalid;
        }
#endif
    }
    else
    {
        kdWarning(30004) << "Not a number: " << number << endl;
        return Invalid;
    }

    newVal = KoUnit::ptToUnit( newVal, m_base->m_unit );
    
    m_base->changeValue( newVal );
    s = m_base->getVisibleText( newVal );
    
    return result;
}


QString KoUnitDoubleBase::getVisibleText( double value ) const
{
    return QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ) ).arg( KoUnit::unitName( m_unit ) );
}

double KoUnitDoubleBase::toDouble( const QString& str, bool* ok ) const
{
    kdDebug(30004) << "toDouble:" << str << ":" << endl;
    QString str2( str );
    /* KLocale::readNumber wants the thousand separator exactly at 1000.
       But when editing, it might be anywhere. So we need to remove it. */
    const QString sep( KGlobal::locale()->thousandsSeparator() );
    if ( !sep.isEmpty() )
        str2.remove( sep );
    return KGlobal::locale()->readNumber( str2, ok );
}


KoUnitDoubleSpinBox::KoUnitDoubleSpinBox( QWidget *parent, double lower, double upper, double step, double value, KoUnit::Unit unit,       unsigned int precision, const char *name )
    : KDoubleSpinBox( lower, upper, step, value, precision, parent, name ), KoUnitDoubleBase( unit, precision ),
    m_lowerInPoints( lower ), m_upperInPoints( upper ), m_stepInPoints( step )
{
	m_validator = new KoUnitDoubleValidator( this, this );
	QSpinBox::setValidator( m_validator );
	setAcceptLocalizedNumbers( true );
    setUnit( unit );
    changeValue(  KoUnit::ptToUnit( value, unit ) );
}

void
KoUnitDoubleSpinBox::changeValue( double val )
{
	KDoubleSpinBox::setValue( val );
}

void
KoUnitDoubleSpinBox::setUnit( KoUnit::Unit unit )
{
    double oldvalue = KoUnit::ptFromUnit( KDoubleSpinBox::value(), m_unit );
    setMinValue( KoUnit::ptToUnit( m_lowerInPoints, unit ) );
    setMaxValue( KoUnit::ptToUnit( m_upperInPoints, unit ) );
    setLineStep( KoUnit::ptToUnit( m_stepInPoints, unit ) );
    KDoubleSpinBox::setValue( KoUnit::ptToUnit( oldvalue, unit ) );
    m_unit = unit;
    setSuffix( KoUnit::unitName( unit ).prepend( ' ' ) );
}

double KoUnitDoubleSpinBox::value( void ) const
{
    return KoUnit::ptFromUnit( KDoubleSpinBox::value(), m_unit );
}


KoUnitDoubleLineEdit::KoUnitDoubleLineEdit( QWidget *parent, double lower, double upper, double value, KoUnit::Unit unit,
    unsigned int precision, const char *name )
    : KLineEdit( parent, name ), KoUnitDoubleBase( unit, precision ), m_value( value ), m_lower( lower ), m_upper( upper ),
    m_lowerInPoints( lower ), m_upperInPoints( upper )
{
	setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	setValidator( m_validator );
    setUnit( unit );
    changeValue(  KoUnit::ptToUnit( value, unit ) );
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
    m_lower = KoUnit::ptToUnit( m_lowerInPoints, unit );
    m_upper = KoUnit::ptToUnit( m_upperInPoints, unit );
	changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( m_value, old ), unit ) );
}

bool
KoUnitDoubleLineEdit::eventFilter( QObject* o, QEvent* ev )
{
#if 0
	if( ev->type() == QEvent::FocusOut || ev->type() == QEvent::Leave || ev->type() == QEvent::Hide )
	{
		bool ok;
		double value = toDouble( text(), &ok );
		changeValue( value );
		return false;
	}
	else
#endif
		return QLineEdit::eventFilter( o, ev );
}

double KoUnitDoubleLineEdit::value( void ) const
{
    return KoUnit::ptFromUnit( m_value, m_unit );
}


KoUnitDoubleComboBox::KoUnitDoubleComboBox( QWidget *parent, double lower, double upper, double value, KoUnit::Unit unit,
     unsigned int precision, const char *name )
     : KComboBox( true, parent, name ), KoUnitDoubleBase( unit, precision ), m_value( value ), m_lower( lower ), m_upper( upper ),
     m_lowerInPoints( lower ), m_upperInPoints( upper )
{
	lineEdit()->setAlignment( Qt::AlignRight );
	m_validator = new KoUnitDoubleValidator( this, this );
	lineEdit()->setValidator( m_validator );
    setUnit( unit );
    changeValue(  KoUnit::ptToUnit( value, unit ) );
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
    m_lower = KoUnit::ptToUnit( m_lowerInPoints, unit );
    m_upper = KoUnit::ptToUnit( m_upperInPoints, unit );
	changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( m_value, old ), unit ) );
}

bool
KoUnitDoubleComboBox::eventFilter( QObject* o, QEvent* ev )
{
#if 0
	if( ev->type() == QEvent::FocusOut || ev->type() == QEvent::Leave || ev->type() == QEvent::Hide )
	{
		bool ok;
		double value = toDouble( lineEdit()->text(), &ok );
		changeValue( value );
		return false;
	}
	else
#endif
		return QComboBox::eventFilter( o, ev );
}

double KoUnitDoubleComboBox::value( void ) const
{
    return KoUnit::ptFromUnit( m_value, m_unit );
}


KoUnitDoubleSpinComboBox::KoUnitDoubleSpinComboBox( QWidget *parent, double lower, double upper, double step, double value,         
    KoUnit::Unit unit, unsigned int precision, const char *name )
    : QWidget( parent ), m_step( step )//, m_lowerInPoints( lower ), m_upperInPoints( upper )
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

	m_combo = new KoUnitDoubleComboBox( this, KoUnit::ptToUnit( lower, unit ), KoUnit::ptToUnit( upper, unit ), value, unit, precision, name );
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

