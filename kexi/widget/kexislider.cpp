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
{
    init(Qt::Horizontal);
}

KexiSlider::KexiSlider(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
    init(orientation);
}

void KexiSlider::init(Qt::Orientation orientation)
{
    m_layout=new QBoxLayout(QBoxLayout::LeftToRight, this);
    m_layout->setSpacing(2);
    m_layout->setMargin(0);
    m_slider = new Slider(this);
    m_spinBox = new QSpinBox(this);
    m_spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_layout->addWidget(m_spinBox,0, Qt::AlignVCenter);
    m_layout->addWidget(m_slider,0, Qt::AlignVCenter);

    connect(m_slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(m_slider, SIGNAL(sliderPressed()), this, SIGNAL(sliderPressed()));
    connect(m_slider, SIGNAL(sliderReleased()), this, SIGNAL(sliderReleased()));
    connect(m_slider, SIGNAL(valueChanged(int)), m_spinBox, SLOT(setValue(int)));
    connect(m_spinBox, SIGNAL(valueChanged(int)), m_slider, SLOT(setValue(int)));

    setMaximum(100);
    setOrientation(orientation);
    setTickPosition(QSlider::TicksAbove);
}

KexiSlider::~KexiSlider()
{

}

void KexiSlider::setMinimum(int min)
{
    m_spinBox->setMinimum(min);
    m_slider->setMinimum(min);
}

void KexiSlider::setMaximum(int max)
{
    m_spinBox->setMaximum(max);
    m_slider->setMaximum(max);
}

void KexiSlider::setValue(int val)
{
    m_slider->setValue(val);
}

int KexiSlider::minimum() const
{
    return m_slider->minimum();
}

int KexiSlider::maximum() const
{
    return m_slider->maximum();
}

int KexiSlider::value() const
{
    return m_slider->value();
}

void KexiSlider::setPageStep(int step)
{
    m_slider->setPageStep(step);
}

int KexiSlider::pageStep() const
{
    return m_slider->pageStep();
}

void KexiSlider::setSingleStep(int step)
{
    m_spinBox->setSingleStep(step);
    m_slider->setSingleStep(step);
}

int KexiSlider::singleStep() const
{
    return m_slider->singleStep();
}

void KexiSlider::setOrientation(Qt::Orientation o)
{
    m_layout->removeWidget(m_spinBox);
    m_slider->setOrientation(o);
    if(o == Qt::Horizontal)
        m_layout->insertWidget(0, m_spinBox);
    else
        m_layout->addWidget(m_spinBox);
    updateLayout();
}

Qt::Orientation KexiSlider::orientation() const
{
    return m_slider->orientation();
}

void KexiSlider::setTickInterval(int ti)
{
    m_slider->setTickInterval(ti);
}

int KexiSlider::tickInterval() const
{
    return m_slider->tickInterval();
}

void KexiSlider::setTickPosition(QSlider::TickPosition pos)
{
    m_slider->setTickPosition(pos);
    updateLayout();
}

QSlider::TickPosition KexiSlider::tickPosition() const
{
    return m_slider->tickPosition();
}

void KexiSlider::setShowEditor(bool show)
{
    m_spinBox->setVisible(show);

}

bool KexiSlider::showEditor() const
{
    return m_spinBox->isVisible();
}

void KexiSlider::updateLayout()
{
    m_layout->setDirection(orientation() == Qt::Horizontal ?
                        QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);

    if (tickPosition() == QSlider::TicksBothSides
            || tickPosition() == QSlider::NoTicks) {
        m_layout->setAlignment(m_slider, orientation() == Qt::Horizontal ?
                                   Qt::AlignVCenter :Qt::AlignHCenter);
        m_layout->setAlignment(m_spinBox, orientation() == Qt::Horizontal ?
                                   Qt::AlignVCenter :Qt::AlignHCenter);
    } else {
        if (orientation() == Qt::Horizontal) {
            m_layout->setAlignment(m_slider,
                                   tickPosition() == QSlider::TicksAbove ? Qt::AlignBottom : Qt::AlignTop);
            m_layout->setAlignment(m_spinBox,
                                   tickPosition() == QSlider::TicksAbove ? Qt::AlignBottom : Qt::AlignTop);
        } else {
            m_layout->setAlignment(m_slider,
                                   tickPosition() == QSlider::TicksLeft ? Qt::AlignRight : Qt::AlignLeft);
            m_layout->setAlignment(m_spinBox,
                                   tickPosition() == QSlider::TicksLeft ? Qt::AlignRight : Qt::AlignLeft);
        }
    }

}

