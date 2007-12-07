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
    : QDoubleSpinBox(parent),
    m_unit( Duration::Unit_d ),
    m_minunit( Duration::Unit_h ),
    m_maxunit( Duration::Unit_d )
{
    setUnit( Duration::Unit_h );
    setMaximum(140737488355328.0); //Hmmmm
}

void DurationSpinBox::setUnit( Duration::Unit unit )
{
    if ( unit < m_maxunit ) {
        m_maxunit = unit;
    } else if ( unit > m_minunit ) {
        m_minunit = unit;
    }
    m_unit = unit;
    setSuffix( Duration::unitToString( m_unit, true ) );
}

void DurationSpinBox::setMaximumUnit( Duration::Unit unit )
{
    //NOTE Day = 0, Milliseconds = 3 !!!
    m_maxunit = unit;
    if ( m_minunit < unit ) {
        m_minunit = unit;
    }
    if ( m_unit < unit ) {
        setUnit( unit );
    }
}

void DurationSpinBox::setMinimumUnit( Duration::Unit unit )
{
    //NOTE Day = 0, Milliseconds = 3 !!!
    m_minunit = unit;
    if ( m_maxunit > unit ) {
        m_maxunit = unit;
    }
    if ( m_unit > unit ) {
        setUnit( unit );
    }
}

void DurationSpinBox::stepUnitUp()
{
    //kDebug()<<m_unit<<">"<<m_maxunit;
    if ( m_unit > m_maxunit ) {
        setUnit( static_cast<Duration::Unit>(m_unit - 1) );
        // line may change length, make sure cursor stays within unit
        lineEdit()->setCursorPosition( lineEdit()->displayText().length() );
    }
}

void DurationSpinBox::stepUnitDown()
{
    //kDebug()<<m_unit<<"<"<<m_minunit;
    if ( m_unit < m_minunit ) {
        setUnit( static_cast<Duration::Unit>(m_unit + 1) );
        // line may change length, make sure cursor stays within unit
        lineEdit()->setCursorPosition( lineEdit()->displayText().length() );
    }
}

void DurationSpinBox::stepBy( int steps )
{
    //kDebug()<<steps;
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

QAbstractSpinBox::StepEnabled DurationSpinBox::stepEnabled () const
{
    if ( lineEdit()->cursorPosition() > text().size() - suffix().size() ) {
        if ( m_unit >= m_minunit ) {
            //kDebug()<<"inside unit, up"<<m_unit<<m_minunit<<m_maxunit;
            return QAbstractSpinBox::StepUpEnabled;
        }
        if ( m_unit <= m_maxunit ) {
            //kDebug()<<"inside unit, down"<<m_unit<<m_minunit<<m_maxunit;
            return QAbstractSpinBox::StepDownEnabled;
        }
        //kDebug()<<"inside unit, up|down"<<m_unit<<m_minunit<<m_maxunit;
        return QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled;
    }
    return QDoubleSpinBox::stepEnabled();
}

void DurationSpinBox::keyPressEvent( QKeyEvent * event )
{
    //kDebug()<<lineEdit()->cursorPosition()<<","<<(text().size() - Duration::unitToString( m_unit, true ).size())<<""<<event->text().isEmpty();
    
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


} //namespace KPlato

#include "kptdurationspinbox.moc"

