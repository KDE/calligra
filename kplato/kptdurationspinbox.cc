/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/


#include <kptdurationspinbox.h>

#include "kptnode.h"

#include <qevent.h>
#include <qlineedit.h>
#include <qlocale.h>
#include <qvalidator.h>

#include <kdebug.h>

#include <math.h>
#include <limits.h>

namespace KPlato
{

DurationSpinBox::DurationSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{
    setAlignment( Qt::AlignRight );
    setUnit( Duration::Unit_h );
    setMaximum(140737488355328.0); // NOTE: 2**47. Less than qint64, but let's see if it gets us problems (4886718,35 days)

    msToFromSec = 1000.0;
    secToFromMin = 60.0;
    minToFromHour = 60.0;
    hourToFromDay = 24.0;
}

void DurationSpinBox::setValue( const Duration &value )
{
    //kDebug()<<k_funcinfo<<value.milliseconds()<<endl;
    QDoubleSpinBox::setValue( durationToDouble( value, m_unit ) );
}

void DurationSpinBox::setValue( const qint64 value )
{
    //kDebug()<<k_funcinfo<<value<<endl;
    Duration d = value;
    setValue( d );
}

qint64 DurationSpinBox::value() const
{
    return durationValue().milliseconds();
}

Duration DurationSpinBox::durationValue() const
{
    return durationFromDouble( QDoubleSpinBox::value(), m_unit );
}

void DurationSpinBox::setUnit( Duration::Unit unit )
{
    Duration v = durationValue();
    m_unit = unit;
    setValue( v );
}

void DurationSpinBox::stepUnitUp()
{
    //kDebug()<<k_funcinfo<<m_unit<<" > "<<Duration::Unit_d<<endl;
    if ( m_unit > Duration::Unit_d ) {
        setUnit( static_cast<Duration::Unit>(m_unit - 1) );
        // line may change lenght, make sure cursor stays within unit
        lineEdit()->setCursorPosition( lineEdit()->displayText().length() );
    }
}

void DurationSpinBox::stepUnitDown()
{
    //kDebug()<<k_funcinfo<<m_unit<<" < "<<Duration::Unit_ms<<endl;
    if ( m_unit < Duration::Unit_ms ) {
        setUnit( static_cast<Duration::Unit>(m_unit + 1) );
        // line may change lenght, make sure cursor stays within unit
        lineEdit()->setCursorPosition( lineEdit()->displayText().length() );
    }
}

void DurationSpinBox::stepBy( int steps )
{
    //kDebug()<<k_funcinfo<<steps<<endl;
    if ( lineEdit()->cursorPosition() > text().size() - Duration::unitToString( m_unit, true ).size() ) {
        // we are in unit
        if ( steps > 0 ) {
            stepUnitUp();
        } else if ( steps < 0 ) {
            stepUnitDown();
        }
        return;
    }
    QDoubleSpinBox::stepBy( steps );
}

// prepare the text to be displayed
QString DurationSpinBox::textFromValue(double value) const
{
    QString s = QDoubleSpinBox::textFromValue( value );
    s += Duration::unitToString( m_unit, true );
    //kDebug()<<k_funcinfo<<value<<" -> "<<s<<endl;
    return s;
}

// extract the value from the text
double DurationSpinBox::valueFromText(const QString &text) const
{
    QString t = text;
    t = t.remove( Duration::unitToString( m_unit, true ) );
    double s = QDoubleSpinBox::valueFromText( t );
    //kDebug()<<k_funcinfo<<text<<", "<<t<<" -> "<<s<<endl;
    return s;
}

// validate, need to handle my unit
QValidator::State DurationSpinBox::validate(QString &txt, int &pos) const
{
    QString unit = Duration::unitToString( m_unit, true );
    QString t = txt;
    QValidator::State s;
    if ( !unit.isEmpty() ) {
        if ( txt.indexOf( unit ) < txt.length() - unit.length() ) {
            // no unit or unit in the middle of the value
            //kDebug()<<k_funcinfo<<t<<", "<<pos<<"; "<<s<<endl;
            return QValidator::Invalid;
        } else if ( pos > txt.length() - unit.length() ) {
            // inside unit
            if ( txt.indexOf( unit ) == -1 ) {
                //kDebug()<<k_funcinfo<<t<<", "<<pos<<"; "<<s<<endl;
                return QValidator::Invalid;
            }
        }
        t.remove( unit );
    }
    s = QDoubleSpinBox::validate( t, pos );
    //kDebug()<<k_funcinfo<<t<<", "<<pos<<"; "<<s<<endl;
    return s;
}


QAbstractSpinBox::StepEnabled DurationSpinBox::stepEnabled () const
{
    if ( QDoubleSpinBox::value() <= minimum() && m_unit == Duration::Unit_ms ) {
        return QAbstractSpinBox::StepUpEnabled;
    }
    if ( QDoubleSpinBox::value() >= maximum() && m_unit == Duration::Unit_d ) {
        return QAbstractSpinBox::StepDownEnabled;
    }
    return StepUpEnabled| StepDownEnabled;
}

void DurationSpinBox::keyPressEvent( QKeyEvent * event )
{
    //kDebug()<<k_funcinfo<<lineEdit()->cursorPosition()<<", "<<(text().size() - Duration::unitToString( m_unit, true ).size())<<" "<<event->text().isEmpty()<<endl;
    
    if ( lineEdit()->cursorPosition() > text().size() - Duration::unitToString( m_unit, true ).size() ) {
        // we are in unit
        switch (event->key()) {
        case Qt::Key_Up:
            event->accept();
            stepBy( 1 );
            return;
        case Qt::Key_Down:
            event->accept();
            stepBy( -1 );
            return;
        default:
            break;
        }
    }
    QDoubleSpinBox::keyPressEvent(event);
}

void DurationSpinBox::setScales( const QVariant &scales )
{
    QVariantList lst = scales.toList();
    switch ( lst.count() ) {
        case 4: msToFromSec = lst[3].toDouble();
        case 3: secToFromMin = lst[2].toDouble();
        case 2: minToFromHour = lst[1].toDouble();
        case 1: hourToFromDay = lst[0].toDouble();
    }
}

double DurationSpinBox::durationToDouble( const Duration &value, Duration::Unit unit) const
{
    QList<double> lst;
    lst << hourToFromDay << minToFromHour << secToFromMin << msToFromSec;
    return Effort::scale( value, unit, lst );
}

Duration DurationSpinBox::durationFromDouble( double value, Duration::Unit unit) const
{
    QList<double> lst;
    lst << hourToFromDay << minToFromHour << secToFromMin << msToFromSec;
    return Effort::scale( value, unit, lst );
}

} //namespace KPlato

#include "kptdurationspinbox.moc"

