/*
 * Copyright (c) 1999 Matthias Elter (me@kde.org)
 * Copyright (c) 2001-2002 Igor Jansen (rm@kde.org)
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "ko_hsv_widget.h"
#include "ko_color_wheel.h"

#include <kselect.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <koFrameButton.h>
#include <koColorSlider.h>
#include <kcolordialog.h>
#include <kdualcolorbutton.h>
#include <koColor.h>
#include <kdebug.h>

KoHSVWidget::KoHSVWidget(QWidget *parent, const char *name) : super(parent, name)
{
    m_ColorButton = new KDualColorButton(this);
    m_ColorButton ->  setFixedSize(m_ColorButton->sizeHint());

    QGridLayout *mGrid = new QGridLayout(this, 5, 7, 5, 2);
    m_colorwheel = new KoColorWheel(this);
    m_colorwheel->setFixedSize( 120, 120);
    m_VSelector = new KValueSelector(Qt::Vertical, this);
    m_VSelector-> setFixedSize( 30, 120);

    /* setup slider labels */
    mHLabel = new QLabel("H", this);
    mHLabel->setFixedSize(12, 20);
    mSLabel = new QLabel("S", this);
    mSLabel->setFixedSize(12, 20);
    mVLabel = new QLabel("V", this);
    mVLabel->setFixedSize(12, 20);

    /* setup spin box */
    mHIn = new QSpinBox(0, 359, 1, this);
    mHIn->setFixedSize(50, 20);
    mHIn->setFocusPolicy( QWidget::ClickFocus );

    mSIn = new QSpinBox(0, 255, 1, this);
    mSIn->setFixedSize(50, 20);
    mSIn->setFocusPolicy( QWidget::ClickFocus );

    mVIn = new QSpinBox(0, 255, 1, this);
    mVIn->setFixedSize(50, 20);
    mVIn->setFocusPolicy( QWidget::ClickFocus );

    mGrid->addMultiCellWidget(m_ColorButton, 0, 0, 0, 1, Qt::AlignTop);

    mGrid->addWidget(mHLabel, 1, 0);
    mGrid->addWidget(mSLabel, 2, 0);
    mGrid->addWidget(mVLabel, 3, 0);

    mGrid->addMultiCellWidget(m_colorwheel, 0, 3, 2, 4);

    mGrid->addWidget(mHIn, 1, 1);
    mGrid->addWidget(mSIn, 2, 1);
    mGrid->addWidget(mVIn, 3, 1);

    mGrid->addMultiCellWidget(m_VSelector, 0, 3, 5, 5);


    connect(m_ColorButton, SIGNAL(fgChanged(const QColor &)), this, SLOT(slotFGColorSelected(const QColor &)));
    connect(m_ColorButton, SIGNAL(bgChanged(const QColor &)), this, SLOT(slotBGColorSelected(const QColor &)));

    connect(m_VSelector, SIGNAL(valueChanged(int)), this, SLOT(slotVChanged(int)));
    connect(m_colorwheel, SIGNAL(valueChanged(const KoColor&)), this, SLOT(slotWheelChanged(const KoColor&)));

    /* connect spin box */
    connect(mHIn, SIGNAL(valueChanged(int)), this, SLOT(slotHChanged(int)));
    connect(mSIn, SIGNAL(valueChanged(int)), this, SLOT(slotSChanged(int)));
    connect(mVIn, SIGNAL(valueChanged(int)), this, SLOT(slotVChanged(int)));

    //setFixedSize(mGrid -> minimumSize());
    locked = false;
    autovalue = true;
}

void KoHSVWidget::slotHChanged(int h)
{
    if (m_ColorButton->current() == KDualColorButton::Foreground){
        m_fgColor.setHSV(h, m_fgColor.S(), m_fgColor.V());
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	QColor c = m_fgColor.color();
	emit sigFgColorChanged(c);
    }
    else{
        m_bgColor.setHSV(h, m_bgColor.S(), m_bgColor.V());
        m_ColorButton->setCurrent(KDualColorButton::Background);
	QColor c = m_bgColor.color();
	emit sigBgColorChanged(c);
    }
}

void KoHSVWidget::slotSChanged(int s)
{
    if (m_ColorButton->current() == KDualColorButton::Foreground){
        m_fgColor.setHSV(m_fgColor.H(), s, m_fgColor.V());
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	QColor c = m_fgColor.color();
	emit sigFgColorChanged(c);
    }
    else{
        m_bgColor.setHSV(m_bgColor.H(), s, m_bgColor.V());
        m_ColorButton->setCurrent(KDualColorButton::Background);
	QColor c = m_bgColor.color();
	emit sigBgColorChanged(c);
    }
}

void KoHSVWidget::slotVChanged(int v)
{
    autovalue = false;
    locked = true;
    if (m_ColorButton->current() == KDualColorButton::Foreground){
        m_fgColor.setHSV(m_fgColor.H(), m_fgColor.S(), v);
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	QColor c = m_fgColor.color();
	emit sigFgColorChanged(c);
    }
    else{
        m_bgColor.setHSV(m_bgColor.H(), m_bgColor.S(), v);
        m_ColorButton->setCurrent(KDualColorButton::Background);
	QColor c = m_bgColor.color();
	emit sigBgColorChanged(c);
    }
    locked = false;
}

void KoHSVWidget::slotWheelChanged(const KoColor& c)
{
    locked = true;
    if (m_ColorButton->current() == KDualColorButton::Foreground){
        if(autovalue)
            m_fgColor.setHSV(c.H(), c.S(), 255);
        else
            m_fgColor.setHSV(c.H(), c.S(), m_fgColor.V());
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	QColor c = m_fgColor.color();
	emit sigFgColorChanged(c);
    }
    else{
        if(autovalue)
            m_bgColor.setHSV(c.H(), c.S(), 255);
        else
            m_bgColor.setHSV(c.H(), c.S(), m_fgColor.V());
        m_ColorButton->setCurrent(KDualColorButton::Background);
	QColor c = m_bgColor.color();
	emit sigBgColorChanged(c);
    }
    locked = false;
}


void KoHSVWidget::setFgColor(const QColor & c)
{
    update(KoColor(c), m_bgColor);
}

void KoHSVWidget::setBgColor(const QColor & c)
{
    update(m_fgColor, KoColor(c));
}

void KoHSVWidget::update(const KoColor & fgColor, const KoColor & bgColor)
{
    if( !locked )
    {
        m_fgColor = fgColor;
        m_bgColor = bgColor;
    }

    KoColor color = (m_ColorButton->current() == KDualColorButton::Foreground)? m_fgColor : m_bgColor;

    int h = color.H();
    int s = color.S();
    int v = color.V();

    m_ColorButton->blockSignals(true);
    m_ColorButton->setForeground( m_fgColor.color() );
    m_ColorButton->setBackground( m_bgColor.color() );
    m_ColorButton->blockSignals(false);

    mHIn->blockSignals(true);
    mSIn->blockSignals(true);
    mVIn->blockSignals(true);
    mHIn->setValue(h);
    mSIn->setValue(s);
    mVIn->setValue(v);
    mHIn->blockSignals(false);
    mSIn->blockSignals(false);
    mVIn->blockSignals(false);

    m_VSelector->blockSignals(true);
    m_VSelector->setHue(h);
    m_VSelector->setSaturation(s);
    m_VSelector->setValue(v);
    m_VSelector->updateContents();
    m_VSelector->blockSignals(false);
    m_VSelector->repaint(false);

    m_colorwheel->blockSignals(true);
    m_colorwheel->slotSetValue(color);
    m_colorwheel->blockSignals(false);
}

void KoHSVWidget::slotFGColorSelected(const QColor& c)
{
    m_fgColor = KoColor(c);
    emit sigFgColorChanged(c);
}

void KoHSVWidget::slotBGColorSelected(const QColor& c)
{
    m_bgColor = KoColor(c);
    emit sigBgColorChanged(c);
}

#include "ko_hsv_widget.moc"
