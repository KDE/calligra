/* This file is part of the KDE project
 * Copyright (c) 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOCOLORPOPUPBUTTON_H_
#define KOCOLORPOPUPBUTTON_H_

#include <QToolButton>

#include "kowidgets_export.h"

/**
 * @short A widget for qreal values with a popup slider
 *
 * KoColorPopupButton combines a numerical input and a dropdown slider in a way that takes up as
 * little screen space as possible.
 *
 * It allows the user to either enter a floating point value or quickly set the value using a slider
 *
 * One signal is emitted when the value changes. The signal is even emitted when the slider
 * is moving. The second argument of the signal however tells you if the value is final or not. A
 * final value is produced by entering a value numerically or by releasing the slider.
 *
 * The input of the numerical line edit is constrained to numbers and decimal signs.
 */
class KOWIDGETS_EXPORT KoColorPopupButton: public QToolButton
{
    Q_OBJECT

public:
    /**
     * Constructor for the widget, where value is set to 0
     *
     * @param parent parent QWidget
     */
    explicit KoColorPopupButton(QWidget *parent=0);

    /**
     * Destructor
     */
    virtual ~KoColorPopupButton();

protected:
    virtual void resizeEvent(QResizeEvent *); ///< reimplemented from QToolButton

private:
    QSize m_iconSize;
};

#endif
