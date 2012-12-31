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

#include "kexislider.h"

#include <QBoxLayout>
#include <QSpinBox>
#include <QPainter>
#include <QStyle>
#include <QSlider>
#include <QStyleOptionSlider>


class KexiSlider::Private
{
public:
    Private() {}

    Slider *slider;
    QSpinBox *spinBox;
    QBoxLayout *layout;
};


class Slider : public QSlider
{
public:
    Slider(QWidget *parent) :
        QSlider(parent)
    {

    }

    QSize sizeHint() const
    {
        if (tickPosition() == QSlider::NoTicks)
            return QSlider::sizeHint();

        // preserve space for labels
        QSize extra(0,0);

        QFontMetrics fm(font());

        int h = fm.height() + 3;
        int w = fm.width(QString::number(maximum())) + 3;

        if (orientation() == Qt::Horizontal) {
            extra.setHeight(tickPosition() == QSlider::TicksBothSides ? h * 2 : h);
        } else {
            extra.setWidth(tickPosition() == QSlider::TicksBothSides ? w * 2 : w);
        }

        return QSlider::sizeHint() + extra;
    }

protected:
    virtual void paintEvent(QPaintEvent *ev)
    {
        if (tickPosition() == QSlider::NoTicks)
            return QSlider::paintEvent(ev);

        QPainter p(this);
        QStyleOptionSlider option;
        initStyleOption(&option);

        const int& ticks( option.tickPosition );
        const int available(style()->proxy()->pixelMetric(QStyle::PM_SliderSpaceAvailable, &option, this));
        int interval = option.tickInterval;
        if( interval < 1 ) interval = option.pageStep;
        if( interval < 1 ) return;

        const QRect& r(option.rect);
        const QPalette palette(option.palette);
        const int fudge(style()->proxy()->pixelMetric(QStyle::PM_SliderLength, &option, this) / 2);
        int current(option.minimum);
        int nextLabel = current;

        QFontMetrics fm(font());
        int h = fm.height() + 3;
        int w = fm.width(QString::number(option.maximum)) + 3;

        if(available<w)
            nextLabel = -1;

        float i = available/(orientation() == Qt::Horizontal ? w : h);
        float t = option.maximum/interval;
        int valStep = t/ i + 1;

        // Since there is no subrect for tickmarks do a translation here.
        p.save();
        p.translate(r.x(), r.y());

        p.setPen(palette.color(QPalette::WindowText));
        int extra = (option.tickPosition == QSlider::TicksBothSides ? 2 : 1);
        int tickSize(option.orientation == Qt::Horizontal ? (r.height() - h*extra)/3:(r.width() - w*extra)/3);

        while(current <= option.maximum)
        {

            const int position(QStyle::sliderPositionFromValue(option.minimum, option.maximum,
                                                                 current, available, option.upsideDown) + fudge);

            // calculate positions
            if(option.orientation == Qt::Horizontal)
            {

                if (ticks & QSlider::TicksAbove) {
                    p.drawLine(position, h, position, tickSize + h);
                    if(current == nextLabel)
                        p.drawText(QRect(position - w/2, 0, w, h), Qt::AlignHCenter, QString::number(current));
                }
                if (ticks & QSlider::TicksBelow) {
                    p.drawLine( position, r.height() - h - tickSize, position, r.height() - h );
                    if(current == nextLabel)
                        p.drawText(QRect(position - w/2, r.height() - h + 3, w, h), Qt::AlignHCenter, QString::number(current));
                }
            } else {
                if (ticks & QSlider::TicksAbove) {
                    p.drawLine(w, position, tickSize + w, position);
                    if(current == nextLabel)
                        p.drawText(QRect(0, position - h/2, w - 3, h), Qt::AlignRight | Qt::AlignVCenter, QString::number(current));
                }
                if (ticks & QSlider::TicksBelow) {
                    p.drawLine(r.width() - w - tickSize, position, r.width() - w, position );
                    if(current == nextLabel)
                        p.drawText(QRect(r.width() - w + 3, position - h/2, w, h), Qt::AlignVCenter, QString::number(current));
                }
            }

            // go to next position
            if (current == nextLabel)
                nextLabel += interval*valStep;
            current += interval;

        }
        p.restore();
        option.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;

        style()->proxy()->drawComplexControl(QStyle::CC_Slider, &option, &p, this);
    }
};

KexiSlider::KexiSlider(QWidget *parent)
    : QWidget(parent)
    , d(new Private)
{
    init(Qt::Horizontal);
}

KexiSlider::KexiSlider(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , d(new Private)
{
    init(orientation);
}

void KexiSlider::init(Qt::Orientation orientation)
{
    d->layout=new QBoxLayout(QBoxLayout::LeftToRight, this);
    d->layout->setSpacing(2);
    d->layout->setMargin(0);
    d->slider = new Slider(this);
    d->spinBox = new QSpinBox(this);
    d->spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    d->layout->addWidget(d->spinBox,0, Qt::AlignVCenter);
    d->layout->addWidget(d->slider,0, Qt::AlignVCenter);

    connect(d->slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(d->slider, SIGNAL(sliderPressed()), this, SIGNAL(sliderPressed()));
    connect(d->slider, SIGNAL(sliderReleased()), this, SIGNAL(sliderReleased()));
    connect(d->slider, SIGNAL(valueChanged(int)), d->spinBox, SLOT(setValue(int)));
    connect(d->spinBox, SIGNAL(valueChanged(int)), d->slider, SLOT(setValue(int)));

    setMaximum(100);
    setOrientation(orientation);
    setTickPosition(QSlider::TicksAbove);
}

KexiSlider::~KexiSlider()
{
    delete d;
}

void KexiSlider::setMinimum(int min)
{
    d->spinBox->setMinimum(min);
    d->slider->setMinimum(min);
}

void KexiSlider::setMaximum(int max)
{
    d->spinBox->setMaximum(max);
    d->slider->setMaximum(max);
}

void KexiSlider::setValue(int val)
{
    d->slider->setValue(val);
}

int KexiSlider::minimum() const
{
    return d->slider->minimum();
}

int KexiSlider::maximum() const
{
    return d->slider->maximum();
}

int KexiSlider::value() const
{
    return d->slider->value();
}

void KexiSlider::setPageStep(int step)
{
    d->slider->setPageStep(step);
}

int KexiSlider::pageStep() const
{
    return d->slider->pageStep();
}

void KexiSlider::setSingleStep(int step)
{
    d->spinBox->setSingleStep(step);
    d->slider->setSingleStep(step);
}

int KexiSlider::singleStep() const
{
    return d->slider->singleStep();
}

void KexiSlider::setOrientation(Qt::Orientation o)
{
    d->layout->removeWidget(d->spinBox);
    d->slider->setOrientation(o);
    if(o == Qt::Horizontal)
        d->layout->insertWidget(0, d->spinBox);
    else
        d->layout->addWidget(d->spinBox);
    updateLayout();
}

Qt::Orientation KexiSlider::orientation() const
{
    return d->slider->orientation();
}

void KexiSlider::setTickInterval(int ti)
{
    d->slider->setTickInterval(ti);
}

int KexiSlider::tickInterval() const
{
    return d->slider->tickInterval();
}

void KexiSlider::setTickPosition(QSlider::TickPosition pos)
{
    d->slider->setTickPosition(pos);
    updateLayout();
}

QSlider::TickPosition KexiSlider::tickPosition() const
{
    return d->slider->tickPosition();
}

void KexiSlider::setShowEditor(bool show)
{
    d->spinBox->setVisible(show);

}

bool KexiSlider::showEditor() const
{
    return d->spinBox->isVisible();
}

void KexiSlider::updateLayout()
{
    d->layout->setDirection(orientation() == Qt::Horizontal ?
                        QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);

    if (tickPosition() == QSlider::TicksBothSides
            || tickPosition() == QSlider::NoTicks) {
        d->layout->setAlignment(d->slider, orientation() == Qt::Horizontal ?
                                   Qt::AlignVCenter :Qt::AlignHCenter);
        d->layout->setAlignment(d->spinBox, orientation() == Qt::Horizontal ?
                                   Qt::AlignVCenter :Qt::AlignHCenter);
    } else {
        if (orientation() == Qt::Horizontal) {
            d->layout->setAlignment(d->slider,
                                   tickPosition() == QSlider::TicksAbove ? Qt::AlignBottom : Qt::AlignTop);
            d->layout->setAlignment(d->spinBox,
                                   tickPosition() == QSlider::TicksAbove ? Qt::AlignBottom : Qt::AlignTop);
        } else {
            d->layout->setAlignment(d->slider,
                                   tickPosition() == QSlider::TicksLeft ? Qt::AlignRight : Qt::AlignLeft);
            d->layout->setAlignment(d->spinBox,
                                   tickPosition() == QSlider::TicksLeft ? Qt::AlignRight : Qt::AlignLeft);
        }
    }

}

