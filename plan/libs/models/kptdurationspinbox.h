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
#include "kplatomodels_export.h"

#include <QSpinBox>

namespace KPlato
{

/**
 * The DurationSpinBox provides a spinbox and a line edit to display and edit durations.
 *
 * DurationSpinBox is a QDoubleSpinBox with the addition of adjustable units,
 * defined as Duration::Unit.
 * The unit can be Day, Hour, Minute, Second and Millisecond.
 * The user can select the unit the duration is displayed in by placing the cursor
 * on the unit and step up or -down.
 * Maximum- and minimum unit can be set with setMaximumUnit() and setMinimumUnit().
 * Defaults are: maximum unit Day, minimum unit Hour.
 * 
 */
class KPLATOMODELS_EXPORT DurationSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DurationSpinBox(QWidget *parent = 0);

    /// Return the current unit
    Duration::Unit unit() const { return m_unit; }
    
    /// step the value steps step. If inside unit, steps unit +/- 1 step.
    virtual void stepBy( int steps );
    /// Set maximum unit to @p unit.
    void setMaximumUnit( Duration::Unit unit );
    /// Set maximum unit to @p unit.
    void setMinimumUnit( Duration::Unit unit );
    
    double valueFromText( const QString & text ) const;
    QString textFromValue ( double value ) const;
    QValidator::State validate ( QString & input, int & pos ) const;

signals:
    void unitChanged( int );

public slots:
    /// Set the current unit.
    /// If unit is outside minimum- or maximum unit, the limit is adjusted.
    void setUnit( Duration::Unit unit);

protected slots:
    void editorTextChanged( const QString &text );

protected:
    void keyPressEvent( QKeyEvent * event );
    StepEnabled stepEnabled () const;

    void stepUnitUp();
    void stepUnitDown();

    QString extractUnit ( const QString &text ) const;
    QString extractValue ( const QString &text ) const;

    /// If unit is outside minimum- or maximum unit, the limit is used.
    void updateUnit( Duration::Unit unit);

    bool isOnUnit() const;

private:
    Duration::Unit m_unit;
    Duration::Unit m_minunit;
    Duration::Unit m_maxunit;
};

} //namespace KPlato

#endif
