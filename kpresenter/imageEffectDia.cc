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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qwidgetstack.h>
#include <qcheckbox.h>
#include <qimage.h>

#include <kimageeffect.h>
#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>

#include "imageEffectDia.h"

ImageEffectDia::ImageEffectDia(QWidget * parent, const char * name):
    KDialogBase(parent, name, true, i18n("Image Effect"), Ok|Cancel, Ok)
{
    m_pix = QPixmap();

    m_effect = IE_NONE;
    m_param1 = QVariant();
    m_param2 = QVariant();
    m_param3 = QVariant();

    base = new EffectBrowserBase(this, "effect_base");

    setMainWidget(base);

    connect((QWidget *)base->m_effectCombo, SIGNAL(activated(int)),
            this, SLOT(effectChanged(int)));

    //connect(this, SIGNAL(okClicked()),
    //        this, SLOT(okClicked()));
}

ImageEffectDia::~ImageEffectDia()
{
    delete base;
}

void ImageEffectDia::effectChanged(int eff)
{
    QImage _tmpImage = m_pix.convertToImage();

    //clear the (unused) params
    m_param1 = QVariant();
    m_param2 = QVariant();
    m_param3 = QVariant();

    m_effect = static_cast<ImageEffect>(eff);

    switch (m_effect) {
    case IE_CHANNEL_INTENSITY: {
        m_param1 = QVariant(base->chanInt_value->value());
        m_param2 = QVariant(base->chanInt_component->currentItem());
        KImageEffect::channelIntensity(_tmpImage, m_param1.toDouble()/100.0, static_cast<KImageEffect::RGBComponent>(m_param2.toInt()));
        break;
    }
    case IE_FADE: {
        m_param1 = QVariant(base->fade_value->value());
        m_param2 = QVariant(base->fade_color->color());
        KImageEffect::fade(_tmpImage, m_param1.toDouble(), m_param2.toColor());
        break;
    }
    case IE_FLATTEN: {
        m_param1 = QVariant(base->flatten_color1->color());
        m_param2 = QVariant(base->flatten_color2->color());
        KImageEffect::flatten(_tmpImage, m_param1.toColor(), m_param2.toColor());
        break;
    }
    case IE_INTENSITY: {
        m_param1 = QVariant(base->intensity_value->value());
        KImageEffect::intensity(_tmpImage, m_param1.toDouble()/100.0);
        break;
    }
    case IE_DESATURATE: {
        m_param1 = QVariant(base->desaturate_value->value());
        KImageEffect::desaturate(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_CONTRAST: {
        m_param1 = QVariant(base->contrast_value->value());
        KImageEffect::contrast(_tmpImage, m_param1.toInt());
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
        KImageEffect::emboss(_tmpImage);
        break;
    }
    case IE_DESPECKLE: {
        KImageEffect::despeckle(_tmpImage);
        break;
    }
    case IE_CHARCOAL: {
        m_param1 = QVariant(base->charcoal_value->value());
        KImageEffect::charcoal(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_NOISE: {
        m_param1 = QVariant(base->noise_type->currentItem());
        KImageEffect::addNoise(_tmpImage, static_cast<KImageEffect::NoiseType>(m_param1.toInt()));
        break;
    }
    case IE_BLUR: {
        m_param1 = QVariant(base->blur_value->value());
        KImageEffect::blur(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_EDGE: {
        m_param1 = QVariant(base->edge_value->value());
        KImageEffect::edge(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_IMPLODE: {
        m_param1 = QVariant(base->implode_value->value());
        KImageEffect::implode(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_OIL_PAINT: {
        m_param1 = QVariant(base->oilPaint_radius->value());
        KImageEffect::oilPaint(_tmpImage, m_param1.toInt());
        break;
    }
    case IE_SHARPEN: {
        m_param1 = QVariant(base->sharpen_value->value());
        KImageEffect::sharpen(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_SPREAD: {
        m_param1 = QVariant(base->spread_value->value());
        KImageEffect::spread(_tmpImage, m_param1.toInt());
        break;
    }
    case IE_SHADE: {
        m_param1 = QVariant(base->shade_color->isChecked());
        m_param2 = QVariant(base->shade_azimuth->value());
        m_param3 = QVariant(base->shade_elevation->value());
        KImageEffect::shade(_tmpImage, m_param1.toBool(), m_param2.toDouble(), m_param3.toDouble());
        break;
    }
    case IE_SWIRL: {
        m_param1 = QVariant(base->swirl_value->value());
        KImageEffect::swirl(_tmpImage, m_param1.toDouble());
        break;
    }
    case IE_WAVE: {
        m_param1 = QVariant(base->wave_amplitude->value());
        m_param2 = QVariant(base->wave_length->value());
        KImageEffect::wave(_tmpImage, m_param1.toDouble(), m_param2.toDouble());
        break;
    }
    case IE_NONE:
    default:
        break;
    }

    m_pix.convertFromImage(_tmpImage);

    updatePreview();
}

/*void ImageEffectDia::okClicked()
{
    accept();
}
*/

void ImageEffectDia::setPixmap(QPixmap pix)
{
    m_pix = pix;
}

void ImageEffectDia::updatePreview()
{
    base->m_previewLabel->setPixmap(m_pix);
    base->m_previewLabel->repaint(false);
}

void ImageEffectDia::setEffect(ImageEffect eff, QVariant p1, QVariant p2, QVariant p3)
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
        base->fade_color->setColor(m_param2.toColor());
        break;
    }
    case IE_FLATTEN: {
        base->flatten_color1->setColor(m_param1.toColor());
        base->flatten_color2->setColor(m_param2.toColor());
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

void ImageEffectDia::showEvent(QShowEvent * e)
{
    KDialogBase::showEvent(e);
    effectChanged((int)m_effect);
}

#include "imageEffectDia.moc"
