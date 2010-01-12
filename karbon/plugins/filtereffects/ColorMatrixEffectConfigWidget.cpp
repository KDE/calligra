/* This file is part of the KDE project
 * Copyright (c) 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ColorMatrixEffectConfigWidget.h"
#include "ColorMatrixEffect.h"
#include "KoFilterEffect.h"

#include <KNumInput>
#include <KComboBox>
#include <KLocale>

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QStackedWidget>

ColorMatrixEffectConfigWidget::ColorMatrixEffectConfigWidget(QWidget *parent)
        : KoFilterEffectConfigWidgetBase(parent), m_effect(0)
{
    QGridLayout * g = new QGridLayout(this);

    m_type = new KComboBox(this);
    m_type->addItem(i18n("Apply color matrix"));
    m_type->addItem(i18n("Saturate colors"));
    m_type->addItem(i18n("Rotate hue"));
    m_type->addItem(i18n("Luminance to alpha"));
    g->addWidget(m_type, 0, 0);

    m_stack = new QStackedWidget(this);
    m_stack->setContentsMargins(0, 0, 0, 0);
    g->addWidget(m_stack, 1, 0);

    QWidget * matrixWidget = new QWidget(m_stack);
    QGridLayout * matrixLayout = new QGridLayout(matrixWidget);
    for (int i = 0; i < ColorMatrixElements; ++i) {
        m_matrix.append(new KDoubleNumInput(matrixWidget));
        m_matrix[i]->setMinimumWidth(40);
        m_matrix[i]->setValue(i / 5 == i % 5 ? 1.0 : 0.0);
        matrixLayout->addWidget(m_matrix[i], i / 5, i % 5);
        connect(m_matrix[i], SIGNAL(valueChanged(double)), this, SLOT(matrixChanged()));
    }
    matrixWidget->setLayout(matrixLayout);
    matrixLayout->setSpacing(2);
    matrixLayout->setContentsMargins(0, 0, 0, 0);
    m_stack->addWidget(matrixWidget);

    QWidget * saturateWidget = new QWidget(m_stack);
    QGridLayout * saturateLayout = new QGridLayout(saturateWidget);
    saturateLayout->addWidget(new QLabel(i18n("Saturate value"), saturateWidget), 0, 0);
    m_saturate = new KDoubleNumInput(saturateWidget);
    m_saturate->setRange(0.0, 1.0, 0.05, false);
    saturateLayout->addWidget(m_saturate, 0, 1);
    saturateLayout->addItem(new QSpacerItem(0, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 1, 0);
    saturateWidget->setLayout(saturateLayout);
    m_stack->addWidget(saturateWidget);

    QWidget * hueRotateWidget = new QWidget(m_stack);
    QGridLayout * hueRotateLayout = new QGridLayout(hueRotateWidget);
    hueRotateLayout->addWidget(new QLabel(i18n("Angle"), hueRotateWidget), 0, 0);
    m_hueRotate = new KDoubleNumInput(hueRotateWidget);
    m_hueRotate->setRange(0.0, 360.0, 1.0, false);
    hueRotateLayout->addWidget(m_hueRotate, 0, 1);
    hueRotateLayout->addItem(new QSpacerItem(0, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 1, 0);
    hueRotateWidget->setLayout(hueRotateLayout);
    m_stack->addWidget(hueRotateWidget);

    QWidget * luminanceWidget = new QWidget(m_stack);
    m_stack->addWidget(luminanceWidget);

    setLayout(g);

    connect(m_type, SIGNAL(currentIndexChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
    connect(m_type, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
    connect(m_saturate, SIGNAL(valueChanged(double)), this, SLOT(saturateChanged(double)));
    connect(m_hueRotate, SIGNAL(valueChanged(double)), this, SLOT(hueRotateChanged(double)));
}

bool ColorMatrixEffectConfigWidget::editFilterEffect(KoFilterEffect * filterEffect)
{
    m_effect = dynamic_cast<ColorMatrixEffect*>(filterEffect);
    if (!m_effect)
        return false;

    m_type->blockSignals(true);

    switch (m_effect->type()) {
    case ColorMatrixEffect::Matrix:
        m_type->setCurrentIndex(0);
        for (int i = 0; i < ColorMatrixElements; ++i) {
            m_matrix[i]->blockSignals(true);
            m_matrix[i]->setValue(m_effect->colorMatrix()[i]);
            m_matrix[i]->blockSignals(false);
        }
        break;
    case ColorMatrixEffect::Saturate:
        m_type->setCurrentIndex(1);
        m_saturate->blockSignals(true);
        m_saturate->setValue(m_effect->saturate());
        m_saturate->blockSignals(false);
        break;
    case ColorMatrixEffect::HueRotate:
        m_type->setCurrentIndex(2);
        m_hueRotate->blockSignals(true);
        m_hueRotate->setValue(m_effect->hueRotate());
        m_hueRotate->blockSignals(false);
        break;
    case ColorMatrixEffect::LuminanceAlpha:
        m_type->setCurrentIndex(3);
        break;
    }

    m_type->blockSignals(false);
    m_stack->setCurrentIndex(m_type->currentIndex());

    return true;
}

void ColorMatrixEffectConfigWidget::matrixChanged()
{
    if (!m_effect)
        return;

    qreal m[ColorMatrixElements];
    memcpy(m, m_effect->colorMatrix(), ColorMatrixElements*sizeof(qreal));

    for (int i = 0; i < ColorMatrixElements; ++i) {
        m[i] = m_matrix[i]->value();
    }
    m_effect->setColorMatrix(m);
    emit filterChanged();
}

void ColorMatrixEffectConfigWidget::saturateChanged(double saturate)
{
    if (!m_effect)
        return;

    m_effect->setSaturate(saturate);
    emit filterChanged();
}

void ColorMatrixEffectConfigWidget::hueRotateChanged(double angle)
{
    if (!m_effect)
        return;

    m_effect->setHueRotate(angle);
    emit filterChanged();
}

void ColorMatrixEffectConfigWidget::typeChanged(int index)
{
    if (!m_effect)
        return;

    if (index == ColorMatrixEffect::Matrix) {
        qreal m[ColorMatrixElements];
        for (int i = 0; i < ColorMatrixElements; ++i) {
            m[i] = m_matrix[i]->value();
        }
        m_effect->setColorMatrix(m);
    } else if (index == ColorMatrixEffect::Saturate) {
        m_effect->setSaturate(m_saturate->value());
    } else if (index == ColorMatrixEffect::HueRotate) {
        m_effect->setHueRotate(m_hueRotate->value());
    } else {
        m_effect->setLuminanceAlpha();
    }
    emit filterChanged();
}

#include "ColorMatrixEffectConfigWidget.moc"
