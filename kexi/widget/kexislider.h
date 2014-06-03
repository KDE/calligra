/* This file is part of the KDE project
   Copyright (C) 2012 Oleg Kukharchuk <oleg.kuh@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISLIDER_H
#define KEXISLIDER_H

#include "kexi_export.h"

#include <QWidget>
#include <QSlider>

class Slider;
class QSpinBox;
class QBoxLayout;


/**
 * @brief KexiSlider widget provides slider with spinbox and labels for tickmarks
 *
 */
class KEXIEXTWIDGETS_EXPORT KexiSlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(int tickInterval READ tickInterval WRITE setTickInterval)
    Q_PROPERTY(QSlider::TickPosition tickPosition READ tickPosition WRITE setTickPosition)
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int pageStep READ pageStep WRITE setPageStep)
    Q_PROPERTY(bool showEditor READ showEditor WRITE setShowEditor)

public:
    explicit KexiSlider(QWidget *parent = 0);
    explicit KexiSlider(Qt::Orientation orientation, QWidget *parent = 0);
    ~KexiSlider();

    void setTickInterval(int ti);
    void setTickPosition(QSlider::TickPosition pos);
    void setMinimum(int min);
    void setMaximum(int max);
    void setSingleStep(int step);
    void setPageStep(int step);
    void setShowEditor(bool show);

    int value() const;
    int maximum() const;
    int minimum() const;
    Qt::Orientation orientation() const;
    int singleStep() const;
    int pageStep() const;
    QSlider::TickPosition tickPosition() const;
    int tickInterval() const;
    bool showEditor() const;

signals:
    void valueChanged(int);
    void sliderPressed();
    void sliderReleased();
public slots:
    void setValue(int value);
    void setOrientation(Qt::Orientation o);
private:
    void updateLayout();
    void init(Qt::Orientation);

    class Private;
    Private * const d;
};
#endif // KEXISLIDER_H
