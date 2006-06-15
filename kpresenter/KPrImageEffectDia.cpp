// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Lukas Tinkl <lukas@kde.org>

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

#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <q3widgetstack.h>
#include <QCheckBox>
#include <qimage.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QPixmap>

#include <kimageeffect.h>
#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>

#include "KPrImageEffectDia.h"

KPrImageEffectDia::KPrImageEffectDia(QWidget * parent, const char * name):
    KDialog(parent)
{
    setCaption(i18n("Image Effect"));
    setButtons(Ok|Cancel);
    m_pix = QPixmap();

    m_effect = IE_NONE;
    m_param1 = QVariant();
    m_param2 = QVariant();
    m_param3 = QVariant();

    base = new EffectBrowserBase(this, "effect_base");

    setMainWidget(base);

    setupSignals();

    connect((QWidget *)base->m_effectCombo, SIGNAL(activated(int)),
            this, SLOT(effectChanged(int)));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(okClicked()));
}

KPrImageEffectDia::~KPrImageEffectDia()
{
    delete base;
}

void KPrImageEffectDia::effectChanged(int eff)
{
    QImage _tmpImage = m_origpix.convertToImage();

    //clear the (unused) params
    m_param1 = QVariant();
    m_param2 = QVariant();
    m_param3 = QVariant();

    m_effect = static_cast<ImageEffect>(eff);

    switch (m_effect) {
    case IE_CHANNEL_INTENSITY: {
        m_param1 = QVariant(base->chanInt_value->value());
        m_param2 = QVariant(base->chanInt_component->currentItem());
        _tmpImage = KImageEffect::channelIntensity(_tmpImage, m_param1.toDouble()/100.0,
                                                   static_cast<KImageEffect::RGBComponent>(m_param2.toInt()));
        break;
    }
    case IE_FADE: {
        m_param1 = QVariant(base->fade_value->value());
        m_param2 = QVariant(base->fade_color->color());
        _tmpImage = KImageEffect::fade(_tmpImage, m_param1.toDouble(), m_param2.value<QColor>());
        break;
    }
    case IE_FLATTEN: {
        m_param1 = QVariant(base->flatten_color1->color());
        m_param2 = QVariant(base->flatten_color2->color());
        _tmpImage = KImageEffect::flatten(_tmpImage, m_param1.value<QColor>(), m_param2.value<QColor>());
        break;
    }
    case IE_INTENSITY: {
        m_param1 = QVariant(base->intensity_value->value());
        _tmpImage = KImageEffect::intensity(_tmpImage, m_param1.toDouble()/100.0);
        break;
    }
    case IE_DESATURATE: {
        m_param1 = QVariant(base->desaturate_value->value());
        _tmpImage = KImageEffect::desaturate(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_CONTRAST: {
        m_param1 = QVariant(base->contrast_value->value());
        _tmpImage = KImageEffect::contrast(_tmpImage, m_param1.toInt());
        break;
    }
    case IE_NORMALIZE: {
        KImageEffect::normalize(_tmpImage);
        break;
    }
    case IE_EQUALIZE: {
        KImageEffect::equalize(_tmpImage);
        break;
    }
    case IE_THRESHOLD: {
        m_param1 = QVariant(base->threshold_value->value());
        KImageEffect::threshold(_tmpImage, m_param1.toInt());
        break;
    }
    case IE_SOLARIZE: {
        m_param1 = QVariant(base->solarize_value->value());
        KImageEffect::solarize(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_EMBOSS: {
        _tmpImage = KImageEffect::emboss(_tmpImage);
        break;
    }
    case IE_DESPECKLE: {
        _tmpImage = KImageEffect::despeckle(_tmpImage);
        break;
    }
    case IE_CHARCOAL: {
        m_param1 = QVariant(base->charcoal_value->value());
        _tmpImage = KImageEffect::charcoal(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_NOISE: {
        m_param1 = QVariant(base->noise_type->currentItem());
        _tmpImage = KImageEffect::addNoise(_tmpImage, static_cast<KImageEffect::NoiseType>(m_param1.toInt()));
        break;
    }
    case IE_BLUR: {
        m_param1 = QVariant(base->blur_value->value());
        _tmpImage = KImageEffect::blur(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_EDGE: {
        m_param1 = QVariant(base->edge_value->value());
        _tmpImage = KImageEffect::edge(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_IMPLODE: {
        m_param1 = QVariant(base->implode_value->value());
        _tmpImage = KImageEffect::implode(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_OIL_PAINT: {
        m_param1 = QVariant(base->oilPaint_radius->value());
        _tmpImage = KImageEffect::oilPaint(_tmpImage, m_param1.toInt());
        break;
    }
    case IE_SHARPEN: {
        m_param1 = QVariant(base->sharpen_value->value());
        _tmpImage = KImageEffect::sharpen(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_SPREAD: {
        m_param1 = QVariant(base->spread_value->value());
        _tmpImage = KImageEffect::spread(_tmpImage, m_param1.toInt());
        break;
    }
    case IE_SHADE: {
        m_param1 = QVariant(base->shade_color->isChecked());
        m_param2 = QVariant(base->shade_azimuth->value());
        m_param3 = QVariant(base->shade_elevation->value());
        _tmpImage = KImageEffect::shade(_tmpImage, m_param1.toBool(), m_param2.toDouble(), m_param3.toDouble());
        break;
    }
    case IE_SWIRL: {
        m_param1 = QVariant(base->swirl_value->value());
        _tmpImage = KImageEffect::swirl(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_WAVE: {
        m_param1 = QVariant(base->wave_amplitude->value());
        m_param2 = QVariant(base->wave_length->value());
        _tmpImage = KImageEffect::wave(_tmpImage, m_param1.toDouble(), m_param2.toDouble());
        break;
    }
    case IE_NONE:
    default:
        break;
    }

    m_pix.convertFromImage(_tmpImage);

    updatePreview();
}

void KPrImageEffectDia::okClicked()
{
    accept();
}


void KPrImageEffectDia::setPixmap(QPixmap pix)
{
    m_pix = pix;
    m_origpix = pix;
    m_pix.detach();
    m_origpix.detach();
}

void KPrImageEffectDia::updatePreview()
{
    base->m_previewLabel->setPixmap(m_pix);
    base->m_previewLabel->repaint(false);
}

void KPrImageEffectDia::setEffect(ImageEffect eff, QVariant p1, QVariant p2, QVariant p3)
{
    m_effect = eff;
    m_param1 = p1;
    m_param2 = p2;
    m_param3 = p3;

    switch (m_effect) {
    case IE_CHANNEL_INTENSITY: {
        base->chanInt_value->setValue(m_param1.toInt());
        base->chanInt_component->setCurrentItem(m_param2.toInt());
        break;
    }
    case IE_FADE: {
        base->fade_value->setValue(m_param1.toDouble());
        base->fade_color->setColor(m_param2.value<QColor>());
        break;
    }
    case IE_FLATTEN: {
        base->flatten_color1->setColor(m_param1.value<QColor>());
        base->flatten_color2->setColor(m_param2.value<QColor>());
        break;
    }
    case IE_INTENSITY: {
        base->intensity_value->setValue(m_param1.toInt());
        break;
    }
    case IE_DESATURATE: {
        base->desaturate_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_CONTRAST: {
        base->contrast_value->setValue(m_param1.toInt());
        break;
    }
    case IE_THRESHOLD: {
        base->threshold_value->setValue(m_param1.toInt());
        break;
    }
    case IE_SOLARIZE: {
        base->solarize_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_CHARCOAL: {
        base->charcoal_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_NOISE: {
        base->noise_type->setCurrentItem(m_param1.toInt());
        break;
    }
    case IE_BLUR: {
        base->blur_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_EDGE: {
        base->edge_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_IMPLODE: {
        base->implode_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_OIL_PAINT: {
        base->oilPaint_radius->setValue(m_param1.toInt());
        break;
    }
    case IE_SHARPEN: {
        base->sharpen_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_SPREAD: {
        base->spread_value->setValue(m_param1.toInt());
        break;
    }
    case IE_SHADE: {
        base->shade_color->setChecked(m_param1.toBool());
        base->shade_azimuth->setValue(m_param2.toDouble());
        base->shade_elevation->setValue(m_param3.toDouble());
        break;
    }
    case IE_SWIRL: {
        base->swirl_value->setValue(m_param1.toDouble());
        break;
    }
    case IE_WAVE: {
        base->wave_amplitude->setValue(m_param1.toDouble());
        base->wave_length->setValue(m_param2.toDouble());
        break;
    }

        //no params
    case IE_NORMALIZE:
    case IE_EQUALIZE:
    case IE_EMBOSS:
    case IE_DESPECKLE:

    case IE_NONE:
    default:
        break;
    }

    base->m_effectCombo->setCurrentItem(static_cast<int>(m_effect));
    base->m_widgetStack->raiseWidget(static_cast<int>(m_effect)); //bug in Qt? the above doesn't emit this :(
}

void KPrImageEffectDia::showEvent(QShowEvent * e)
{
    KDialog::showEvent(e);
    effectChanged((int)m_effect);
}

void KPrImageEffectDia::setupSignals()
{
    connect((QWidget *)base->chanInt_value, SIGNAL(valueChanged(int)),
            this, SLOT(effectParamChanged()));
    connect((QWidget *)base->chanInt_component, SIGNAL(activated(int)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->fade_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));
    connect((QWidget *)base->fade_color, SIGNAL(changed(const QColor&)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->flatten_color1, SIGNAL(changed(const QColor&)),
            this, SLOT(effectParamChanged()));
    connect((QWidget *)base->flatten_color2, SIGNAL(changed(const QColor&)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->intensity_value, SIGNAL(valueChanged(int)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->desaturate_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->contrast_value, SIGNAL(valueChanged(int)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->threshold_value, SIGNAL(valueChanged(int)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->solarize_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->charcoal_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->noise_type, SIGNAL(activated(int)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->blur_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->edge_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->implode_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->oilPaint_radius, SIGNAL(valueChanged(int)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->sharpen_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->spread_value, SIGNAL(valueChanged(int)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->shade_color, SIGNAL(toggled(bool)),
            this, SLOT(effectParamChanged()));
    connect((QWidget *)base->shade_elevation, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));
    connect((QWidget *)base->shade_azimuth, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->swirl_value, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));

    connect((QWidget *)base->wave_amplitude, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));
    connect((QWidget *)base->wave_length, SIGNAL(valueChanged(double)),
            this, SLOT(effectParamChanged()));
}

void KPrImageEffectDia::effectParamChanged()
{
    effectChanged(base->m_effectCombo->currentItem());
}

#include "KPrImageEffectDia.moc"
