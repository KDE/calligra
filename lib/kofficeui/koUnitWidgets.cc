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
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <koUnit.h>

// Taken from KoUnit::unit of KOffice CVS HEAD
static KoUnit::Unit getUnit( const QString &_unitName, bool* ok )
{
    if ( ok )
        *ok = true;
    if ( _unitName == QString::fromLatin1( "mm" ) ) return KoUnit::U_MM;
    if ( _unitName == QString::fromLatin1( "cm" ) ) return KoUnit::U_CM;
    if ( _unitName == QString::fromLatin1( "dm" ) ) return KoUnit::U_DM;
    if ( _unitName == QString::fromLatin1( "in" )
         || _unitName == QString::fromLatin1("inch") /*compat*/ ) return KoUnit::U_INCH;
    if ( _unitName == QString::fromLatin1( "pi" ) ) return KoUnit::U_PI;
    if ( _unitName == QString::fromLatin1( "dd" ) ) return KoUnit::U_DD;
    if ( _unitName == QString::fromLatin1( "cc" ) ) return KoUnit::U_CC;
    if ( _unitName == QString::fromLatin1( "pt" ) ) return KoUnit::U_PT;
    if ( ok )
        *ok = false;
    return KoUnit::U_PT;
}

// Taken from KoUnit::fromUserValue of KOffice CVS HEAD
static double getUserValue( double value, KoUnit::Unit unit )
{
    switch ( unit ) {
    case KoUnit::U_MM:
        return MM_TO_POINT( value );
    case KoUnit::U_CM:
        return CM_TO_POINT( value );
    case KoUnit::U_DM:
        return DM_TO_POINT( value );
    case KoUnit::U_INCH:
        return INCH_TO_POINT( value );
    case KoUnit::U_PI:
        return PI_TO_POINT( value );
    case KoUnit::U_DD:
        return DD_TO_POINT( value );
    case KoUnit::U_CC:
        return CC_TO_POINT( value );
    case KoUnit::U_PT:
    default:
        return value;
    }
}


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

    if ( res == -1 )
    {
        // Nothing like an unit? The user is probably editing the unit
        kdDebug(30004) << "Intermediate (no unit)" << endl;
        return Intermediate;
    }

    // ### TODO: are all the QString::stripWhiteSpace really necessary?
    const QString number ( s.left( res ).stripWhiteSpace() );
    const QString unitName ( regexp.cap( 1 ).stripWhiteSpace().lower() );

    kdDebug(30004) << "Split:" << number << ":" << unitName << ":" << endl;

    bool ok = false;
    const double value = KoUnitDoubleBase::toDouble( number, &ok );
    double newVal = 0.0;
    if( ok )
    {
        KoUnit::Unit unit = getUnit( unitName, &ok );
        if ( ok )
            newVal = getUserValue( value, unit );
        else
        {
            // Probably the user is trying to edit the unit
            kdDebug(30004) << "Intermediate (unknown unit)" << endl;
            return Intermediate;
        }
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
#if QT_VERSION > 0x030102
    const QString num ( QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ), KoUnit::unitName( m_unit ) ) );
#else
    const QString num ( QString( "%1%2").arg( KGlobal::locale()->formatNumber( value, m_precision ) ).arg( KoUnit::unitName( m_unit ) ) );
#endif

    kdDebug(30004) << "getVisibleText: " << QString::number( value, 'f', 12 ) << " => " << num << endl;
    return num;
}

double KoUnitDoubleBase::toDouble( const QString& str, bool* ok )
{
    QString str2( str );
    /* KLocale::readNumber wants the thousand separator exactly at 1000.
       But when editing, it might be anywhere. So we need to remove it. */
    const QString sep( KGlobal::locale()->thousandsSeparator() );
    if ( !sep.isEmpty() )
        str2.remove( sep );
    const double dbl = KGlobal::locale()->readNumber( str2, ok );
    if ( ok )
        kdDebug(30004) << "toDouble:" << str << ": => :" << str2 << ": => " << QString::number( dbl, 'f', 12 ) << endl;
    else
        kdWarning(30004) << "toDouble error:" << str << ": => :" << str2 << ":" << endl;
    return dbl;
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
	setSuffix( KoUnit::unitName( unit ) );
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
                const double value = KoUnitDoubleBase::toDouble( text(), &ok );
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
        const double value = KoUnitDoubleBase::toDouble( text( index ), &ok );
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
	changeValue( KoUnit::ptToUnit( getUserValue( m_value, old ), unit ) );
}

bool
KoUnitDoubleComboBox::eventFilter( QObject* o, QEvent* ev )
{
	if( ev->type() == QEvent::FocusOut || ev->type() == QEvent::Leave || ev->type() == QEvent::Hide )
	{
		bool ok;
                const double value = KoUnitDoubleBase::toDouble( lineEdit()->text(), &ok );
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

