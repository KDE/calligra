/*
 *  kis_tool_brush.cc - part of Krita
 *
 *  Copyright (c) 2003-2004 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_tool_brush.h"

#include <QCheckBox>

#include <klocale.h>

#include "kis_cursor.h"
#include "kis_slider_spin_box.h"

#define MAXIMUM_SMOOTHNESS_QUALITY 100 // 0..100
#define MAXIMUM_SMOOTHNESS_FACTOR 1000.0 // 0..1000.0 == weight in gui
#define MAXIMUM_MAGNETISM 1000


KisToolBrush::KisToolBrush(KoCanvasBase * canvas)
    : KisToolFreehand(canvas,
                      KisCursor::load("tool_freehand_cursor.png", 5, 5),
                      i18nc("(qtundo-format)", "Brush"))
{
    setObjectName("tool_brush");
}

KisToolBrush::~KisToolBrush()
{
}

void KisToolBrush::slotSetSmoothnessQuality(int quality)
{
    m_smoothnessQuality = quality;
}

void KisToolBrush::slotSetSmoothnessFactor(qreal factor)
{
    m_smoothnessFactor = factor;
}

void KisToolBrush::slotSetMagnetism(int magnetism)
{
    m_magnetism = expf(magnetism / (double)MAXIMUM_MAGNETISM) / expf(1.0);
}

QWidget * KisToolBrush::createOptionWidget()
{
    QWidget * optionWidget = KisToolFreehand::createOptionWidget();
    optionWidget->setObjectName(toolId() + "option widget");

    // Line smoothing configuration
    m_chkSmooth = new QCheckBox(i18nc("smooth out the curves while drawing", "Smoothness:"), optionWidget);
    m_chkSmooth->setObjectName("chkSmooth");
    m_chkSmooth->setChecked(m_smooth);
    connect(m_chkSmooth, SIGNAL(toggled(bool)), this, SLOT(setSmooth(bool)));

    addOptionWidgetOption(m_chkSmooth);

    m_sliderSmoothnessQuality = new KisSliderSpinBox(optionWidget);
    m_sliderSmoothnessQuality->setRange(1, MAXIMUM_SMOOTHNESS_QUALITY);
    m_sliderSmoothnessQuality->setEnabled(true);
    connect(m_chkSmooth, SIGNAL(toggled(bool)), m_sliderSmoothnessQuality, SLOT(setEnabled(bool)));
    connect(m_sliderSmoothnessQuality, SIGNAL(valueChanged(int)), SLOT(slotSetSmoothnessQuality(int)));
    m_sliderSmoothnessQuality->setValue(m_smoothnessQuality);
    addOptionWidgetOption(m_sliderSmoothnessQuality, new QLabel(i18n("Quality:")));

    m_sliderSmoothnessFactor = new KisDoubleSliderSpinBox(optionWidget);
    m_sliderSmoothnessFactor->setRange(0.0, MAXIMUM_SMOOTHNESS_FACTOR, 1);
    m_sliderSmoothnessFactor->setEnabled(true);
    connect(m_chkSmooth, SIGNAL(toggled(bool)), m_sliderSmoothnessFactor, SLOT(setEnabled(bool)));
    connect(m_sliderSmoothnessFactor, SIGNAL(valueChanged(qreal)), SLOT(slotSetSmoothnessFactor(qreal)));
    m_sliderSmoothnessFactor->setValue(m_smoothnessFactor);

    addOptionWidgetOption(m_sliderSmoothnessFactor, new QLabel(i18n("Weight:")));

    // Drawing assistant configuration
    m_chkAssistant = new QCheckBox(i18n("Assistant:"), optionWidget);
    m_chkAssistant->setToolTip(i18n("You need to add Ruler Assistants before this tool will work."));
    connect(m_chkAssistant, SIGNAL(toggled(bool)), this, SLOT(setAssistant(bool)));
    m_sliderMagnetism = new KisSliderSpinBox(optionWidget);
    m_sliderMagnetism->setToolTip(i18n("Assistant Magnetism"));
    m_sliderMagnetism->setRange(0, MAXIMUM_MAGNETISM);
    m_sliderMagnetism->setEnabled(false);
    connect(m_chkAssistant, SIGNAL(toggled(bool)), m_sliderMagnetism, SLOT(setEnabled(bool)));
    m_sliderMagnetism->setValue(m_magnetism * MAXIMUM_MAGNETISM);
    connect(m_sliderMagnetism, SIGNAL(valueChanged(int)), SLOT(slotSetMagnetism(int)));

    addOptionWidgetOption(m_sliderMagnetism, m_chkAssistant);

    return optionWidget;
}

#include "kis_tool_brush.moc"

