/*
 *  Widget for use in light table docker
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "lt_slider.h"

LTSlider::LTSlider(QObject* parent)
{
    m_layout = new QVBoxLayout(this);
    this->setLayout(m_layout);
    
    m_check = new QCheckBox(this);
    m_slider = new QSlider(this);
    m_slider->setRange(0, 1000);
    
    m_layout->addWidget(m_check);
    m_layout->addWidget(m_slider);
    
    connect(m_check, SIGNAL(toggled(bool)), this, SLOT(visibilityEmitter(bool)));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(opacityEmitter(int)));
    
    m_number = 0;
}

LTSlider::~LTSlider()
{

}

void LTSlider::setNumbner(int n)
{
    m_number = n;
}

void LTSlider::opacityEmitter(int value)
{
    emit opacityChanged(m_number, value);
}

void LTSlider::visibilityEmitter(bool value)
{
    emit visibilityChanged(m_number, value);
}

void LTSlider::setOpacity(double n)
{
    m_slider->setValue(n*1000);
}

void LTSlider::setVisibility(bool o)
{
    m_check->setChecked(o);;
}
