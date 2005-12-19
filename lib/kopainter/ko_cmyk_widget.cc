/*
 * This file is part of KOffice
 *
 * Copyright (c) 1999 Matthias Elter (me@kde.org)
 * Copyright (c) 2001-2002 Igor Jansen (rm@kde.org)
 * Copyright (c) 2005 Tim Beaulen (tbscope@gmail.com)
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

#include "ko_cmyk_widget.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcolor.h>

#include <kdebug.h>
#include <kglobal.h>

#include <koFrameButton.h>
#include <koColorSlider.h>
#include <kcolordialog.h>
#include <kdualcolorbutton.h>

KoCMYKWidget::KoCMYKWidget(QWidget *parent, const char *name) : super(parent, name)
{
    m_ColorButton = new KDualColorButton(this);
    m_ColorButton ->  setFixedSize(m_ColorButton->sizeHint());
    QGridLayout *mGrid = new QGridLayout(this, 4, 5, 5, 2);

    /* setup color sliders */
    mCSlider = new KoColorSlider(this);
    mCSlider->setMaximumHeight(20);
    mCSlider->slotSetRange(0, 255);
    mCSlider->setFocusPolicy( QWidget::ClickFocus );

    mMSlider = new KoColorSlider(this);
    mMSlider->setMaximumHeight(20);
    mMSlider->slotSetRange(0, 255);
    mMSlider->setFocusPolicy( QWidget::ClickFocus );

    mYSlider = new KoColorSlider(this);
    mYSlider->setMaximumHeight(20);
    mYSlider->slotSetRange(0, 255);
    mYSlider->setFocusPolicy( QWidget::ClickFocus );

    mKSlider = new KoColorSlider(this);
    mKSlider->setMaximumHeight(20);
    mKSlider->slotSetRange(0, 255);
    mKSlider->setFocusPolicy( QWidget::ClickFocus );

    /* setup slider labels */
    mCLabel = new QLabel("C", this);
    mCLabel->setFixedWidth(12);
    mCLabel->setFixedHeight(20);
    mMLabel = new QLabel("M", this);
    mMLabel->setFixedWidth(12);
    mMLabel->setFixedHeight(20);
    mYLabel = new QLabel("Y", this);
    mYLabel->setFixedWidth(12);
    mYLabel->setFixedHeight(20);
    mKLabel = new QLabel("K", this);
    mKLabel->setFixedWidth(12);
    mKLabel->setFixedHeight(20);

    /* setup spin box */
    mCIn = new QSpinBox(0, 255, 1, this);
    mCIn->setFixedWidth(50);
    mCIn->setFixedHeight(20);
    mCIn->setFocusPolicy( QWidget::ClickFocus );

    mMIn = new QSpinBox(0, 255, 1, this);
    mMIn->setFixedWidth(50);
    mMIn->setFixedHeight(20);
    mMIn->setFocusPolicy( QWidget::ClickFocus );

    mYIn = new QSpinBox(0, 255, 1, this);
    mYIn->setFixedWidth(50);
    mYIn->setFixedHeight(20);
    mYIn->setFocusPolicy( QWidget::ClickFocus );

    mKIn = new QSpinBox(0, 255, 1, this);
    mKIn->setFixedWidth(50);
    mKIn->setFixedHeight(20);
    mKIn->setFocusPolicy( QWidget::ClickFocus );

    mGrid->addMultiCellWidget(m_ColorButton, 0, 3, 0, 0, Qt::AlignTop);
    mGrid->addWidget(mCLabel, 0, 1);
    mGrid->addWidget(mMLabel, 1, 1);
    mGrid->addWidget(mYLabel, 2, 1);
    mGrid->addWidget(mKLabel, 3, 1);
    mGrid->addMultiCellWidget(mCSlider, 0, 0, 2, 3);
    mGrid->addMultiCellWidget(mMSlider, 1, 1, 2, 3);
    mGrid->addMultiCellWidget(mYSlider, 2, 2, 2, 3);
    mGrid->addMultiCellWidget(mKSlider, 3, 3, 2, 3);
    mGrid->addWidget(mCIn, 0, 4);
    mGrid->addWidget(mMIn, 1, 4);
    mGrid->addWidget(mYIn, 2, 4);
    mGrid->addWidget(mKIn, 3, 4);

    connect(m_ColorButton, SIGNAL(fgChanged(const QColor &)), this, SLOT(slotFGColorSelected(const QColor &)));
    connect(m_ColorButton, SIGNAL(bgChanged(const QColor &)), this, SLOT(slotBGColorSelected(const QColor &)));

    /* connect color sliders */
    connect(mCSlider, SIGNAL(valueChanged(int)), this, SLOT(slotCChanged(int)));
    connect(mMSlider, SIGNAL(valueChanged(int)), this, SLOT(slotMChanged(int)));
    connect(mYSlider, SIGNAL(valueChanged(int)), this, SLOT(slotYChanged(int)));
    connect(mKSlider, SIGNAL(valueChanged(int)), this, SLOT(slotKChanged(int)));

    /* connect spin box */
    connect(mCIn, SIGNAL(valueChanged(int)), this, SLOT(slotCChanged(int)));
    connect(mMIn, SIGNAL(valueChanged(int)), this, SLOT(slotMChanged(int)));
    connect(mYIn, SIGNAL(valueChanged(int)), this, SLOT(slotYChanged(int)));
	connect(mKIn, SIGNAL(valueChanged(int)), this, SLOT(slotKChanged(int)));
}

void KoCMYKWidget::slotCChanged(int c)
{
	kdDebug() << "slotCChanged: " << c << endl;

    if (m_ColorButton->current() == KDualColorButton::Foreground){
        m_fgC = c / 255;
        int r = int((1 - m_fgC * (1 - m_fgK) - m_fgK) * 255);
        int g = m_fgColor.green();
        int b = m_fgColor.blue();
        m_fgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	emit sigFgColorChanged(m_fgColor);
    }
    else{
        m_bgC = c / 255;
        int r = int((1 - m_bgC * (1 - m_bgK) - m_bgK) * 255);
        int g = m_bgColor.green();
        int b = m_bgColor.blue();
        m_bgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Background);
	emit sigBgColorChanged(m_bgColor);
    }
}

void KoCMYKWidget::slotMChanged(int m)
{
    if (m_ColorButton->current() == KDualColorButton::Foreground){
        m_fgM = m / 255;
        int r = m_fgColor.red();
        int g = int((1 - m_fgM * (1 - m_fgK) - m_fgK) * 255);
        int b = m_fgColor.blue();
        m_fgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	emit sigFgColorChanged(m_fgColor);
    }
    else{
        m_bgM = m / 255;
        int r = m_bgColor.red();
        int g = int((1 - m_bgM * (1 - m_bgK) - m_bgK) * 255);
        int b = m_bgColor.blue();
        m_bgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Background);
	emit sigBgColorChanged(m_bgColor);
    }
}

void KoCMYKWidget::slotYChanged(int y)
{
    if (m_ColorButton->current() == KDualColorButton::Foreground){
        m_fgY = y / 255;
        int r = m_fgColor.red();
        int g = m_fgColor.green();
        int b = int((1 - m_fgY * (1 - m_fgK) - m_fgK) * 255);
        m_fgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	emit sigFgColorChanged(m_fgColor);
    }
    else{
        m_bgY = y / 255;
        int r = m_bgColor.red();
        int g = m_bgColor.green();
        int b = int((1 - m_bgY * (1 - m_bgK) - m_bgK) * 255);
        m_bgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Background);
	emit sigBgColorChanged(m_bgColor);
    }
}

void KoCMYKWidget::slotKChanged(int k)
{
    if (m_ColorButton->current() == KDualColorButton::Foreground){
        m_fgK = k / 255;
        int r = int((1 - m_fgC * (1 - m_fgK) - m_fgK) * 255);
        int g = int((1 - m_fgM * (1 - m_fgK) - m_fgK) * 255);
        int b = int((1 - m_fgY * (1 - m_fgK) - m_fgK) * 255);
        m_fgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Foreground);
	emit sigFgColorChanged(m_fgColor);
    }
    else{
        m_bgK = k / 255;
        int r = int((1 - m_bgC * (1 - m_bgK) - m_bgK) * 255);
        int g = int((1 - m_bgM * (1 - m_bgK) - m_bgK) * 255);
        int b = int((1 - m_bgY * (1 - m_bgK) - m_bgK) * 255);
        m_bgColor.setRgb(r, g, b);
        m_ColorButton->setCurrent(KDualColorButton::Background);
	emit sigBgColorChanged(m_bgColor);
    }
}

void KoCMYKWidget::setFgColor(const QColor & c)
{
    update(c, m_bgColor);
}

void KoCMYKWidget::setBgColor(const QColor & c)
{
    update(m_fgColor, c);
}

void KoCMYKWidget::update(const QColor fgColor, const QColor bgColor)
{
    kdDebug() << "update" << endl;
    m_fgColor = fgColor;
    m_bgColor = bgColor;

    QColor color = (m_ColorButton->current() == KDualColorButton::Foreground)? m_fgColor : m_bgColor;

    int r = color.red();
    int g = color.green();
    int b = color.blue();

    float ac = (255 - r) / 255;
    float am = (255 - g) / 255;
    float ay = (255 - b) / 255;

    float c = 0;
    float m = 0;
    float y = 0;
    float k = 0;

    if ((r == 0) && (g == 0) && (b == 0))
    {
        k = 1; 
    }
    else
    {
        KMIN(ac,am) ? k = KMIN(ac,ay) : k = KMIN(am,ay);
        c = (ac - k) / (1 - k);
        m = (am - k) / (1 - k);
        y = (ay - k) / (1 - k);
    }

	kdDebug() << "c = " << c << endl << "m = " << m << endl << "y = " << y << endl << "k = " << k << endl;

    disconnect(m_ColorButton, SIGNAL(fgChanged(const QColor &)), this, SLOT(slotFGColorSelected(const QColor &)));
    disconnect(m_ColorButton, SIGNAL(bgChanged(const QColor &)), this, SLOT(slotBGColorSelected(const QColor &)));

    m_ColorButton->setForeground( m_fgColor );
    m_ColorButton->setBackground( m_bgColor );

    connect(m_ColorButton, SIGNAL(fgChanged(const QColor &)), this, SLOT(slotFGColorSelected(const QColor &)));
    connect(m_ColorButton, SIGNAL(bgChanged(const QColor &)), this, SLOT(slotBGColorSelected(const QColor &)));

    mCSlider->blockSignals(true);
    mCIn->blockSignals(true);
    mMSlider->blockSignals(true);
    mMIn->blockSignals(true);
    mYSlider->blockSignals(true);
    mYIn->blockSignals(true);
    mKSlider->blockSignals(true);
    mKIn->blockSignals(true);

    QColor minC;
    QColor maxC;

    int minCr = int((1 - k) * 255);
    int minCg = g;
    int minCb = b;
    minC.setRgb(minCr, minCg, minCb);

    int maxCr = 0;
    int maxCg = g;
    int maxCb = b;
    maxC.setRgb(maxCr, maxCg, maxCb);

    mCSlider->slotSetColor1(minC);
    mCSlider->slotSetColor2(maxC);
    mCSlider->slotSetValue(int(c * 255));
    mCIn->setValue(int(c * 255));

    QColor minM;
    QColor maxM;

    int minMr = r;
    int minMg = int((1 - k) * 255);
    int minMb = b;
    minM.setRgb(minMr, minMg, minMb);

    int maxMr = r;
    int maxMg = 0;
    int maxMb = b;
    maxM.setRgb(maxMr, maxMg, maxMb);

    mMSlider->slotSetColor1(minM);
    mMSlider->slotSetColor2(maxM);
    mMSlider->slotSetValue(int(m * 255));
    mMIn->setValue(int(m * 255));

    QColor minY;
    QColor maxY;

    int minYr = r;
    int minYg = g;
    int minYb = int((1 - k) * 255);
    minY.setRgb(minYr, minYg, minYb);

    int maxYr = r;
    int maxYg = g;
    int maxYb = 0;
    maxY.setRgb(maxYr, maxYg, maxYb);

    mYSlider->slotSetColor1(minY);
    mYSlider->slotSetColor2(maxY);
    mYSlider->slotSetValue(int(y * 255));
    mYIn->setValue(int(y * 255));

    QColor minK;
    QColor maxK;

    int minKr = 255;
    int minKg = 255;
    int minKb = 255;
    minK.setRgb(minKr, minKg, minKb);

    int maxKr = 0;
    int maxKg = 0;
    int maxKb = 0;
    maxK.setRgb(maxKr, maxKg, maxKb);

    mKSlider->slotSetColor1(minK);
    mKSlider->slotSetColor2(maxK);
    mKSlider->slotSetValue(int(k * 255));
    mKIn->setValue(int(k * 255));

    mCSlider->blockSignals(false);
    mCIn->blockSignals(false);
    mMSlider->blockSignals(false);
    mMIn->blockSignals(false);
    mYSlider->blockSignals(false);
    mYIn->blockSignals(false);
    mKSlider->blockSignals(false);
    mKIn->blockSignals(false);
}

void KoCMYKWidget::slotFGColorSelected(const QColor& c)
{
    m_fgColor = QColor(c);
    emit sigFgColorChanged(m_fgColor);
}

void KoCMYKWidget::slotBGColorSelected(const QColor& c)
{
    m_bgColor = QColor(c);
    emit sigBgColorChanged(m_bgColor);
}

#include "ko_cmyk_widget.moc"
