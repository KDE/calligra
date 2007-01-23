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

#ifndef KPLATO_DURATIONSPINBOX_H
#define KPLATO_DURATIONSPINBOX_H

#include "kptduration.h"

#include <QtGui/qspinbox.h>
#include <QVariantList>

namespace KPlato
{

/**
 * The DurationSpinBox provides a spinbox and a line edit to display and edit durations.
 *
 * DurationSpinBox is a QDoubleSpinBox with the addition of units and scales.
 * The unit can be Day, Hour, Minute, Second and Millisecond.
 * The user can select the unit the duration is displayed in by placing the cursor
 * on the unit and step up or -down. The value is scaled accordingly.
 *
 * The scales to use for conversion between the different units can be set with setScales().
 */
class DurationSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DurationSpinBox(QWidget *parent = 0);

    /// Return value in milliseconds
    qint64 value() const;
    /// Return the current unit
    Duration::Unit unit() const { return m_unit; }
    
    virtual QValidator::State validate(QString &input, int &pos) const;
    virtual double valueFromText(const QString &text) const;
    virtual QString textFromValue(double val) const;

    /// step the value steps step. If inside unit, steps unit +/- 1 step.
    virtual void stepBy( int steps );
    
public slots:
    /// Set spinbox to value, scaled by current unit
    void setValue( const Duration &value );
    /// Set spinbox to value, scaled by current unit.
    void setValue( const qint64 value );
    /// Set the current unit. The displayed value is rescaled.
    void setUnit( Duration::Unit unit);
    /**
     * Set the scales used for conversion between different units.
     * lst is a QVariant<QVariantList> where entry
     * 0 is number of hours in a day,
     * 1 is number of minutes in an hour,
     * 2 is number of seconds in a minute,
     * 3 is number of milliseconds in a second.
     * If the list is shorter, default values are used for the missing values.
     */
    void setScales( const QVariant &lst );

protected:
    void keyPressEvent( QKeyEvent * event );
    StepEnabled stepEnabled () const;

    void stepUnitUp();
    void stepUnitDown();

    /// Return value as a Duration (in milliseconds)
    Duration durationValue() const;
    /// Convert a Duration into a double, scaled to unit
    double durationToDouble( const Duration &value, Duration::Unit unit) const;
    /// Convert a double in unit into a Duration
    Duration durationFromDouble( double value, Duration::Unit unit) const;

private:
    Duration::Unit m_unit;
    double msToFromSec; // 1000
    double secToFromMin; // 60
    double minToFromHour; // 60
    double hourToFromDay; // 24
};

} //namespace KPlato

#endif
