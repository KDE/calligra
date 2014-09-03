/*
 *  Copyright (c) 2008 Boudewijn Rempt <boud@valdyas.org>
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
#include "kis_brush_based_paintop.h"
#include "kis_brush.h"
#include "kis_properties_configuration.h"
#include "kis_brush_option.h"
#include <kis_pressure_spacing_option.h>

#include <QImage>
#include <QPainter>

#ifdef HAVE_THREADED_TEXT_RENDERING_WORKAROUND

class TextBrushInitializationWorkaround
{
public:
    static TextBrushInitializationWorkaround* instance() {
        K_GLOBAL_STATIC(TextBrushInitializationWorkaround, s_instance);
        return s_instance;
    }

    void preinitialize(const KisPropertiesConfiguration *settings) {
        if (KisBrushOption::isTextBrush(settings)) {
            KisBrushOption brushOption;
            brushOption.readOptionSetting(settings);
            m_brush = brushOption.brush();
            m_settings = settings;
        }
        else {
            m_brush = 0;
            m_settings = 0;
        }
    }

    KisBrushSP tryGetBrush(const KisPropertiesConfiguration *settings) {
        return settings && settings == m_settings ? m_brush : 0;
    }

private:
    TextBrushInitializationWorkaround() : m_settings(0) {}

private:
    KisBrushSP m_brush;
    const KisPropertiesConfiguration *m_settings;
};

void KisBrushBasedPaintOp::preinitializeOpStatically(const KisPaintOpSettingsSP settings)
{
    TextBrushInitializationWorkaround::instance()->preinitialize(settings.data());
}

#endif /* HAVE_THREADED_TEXT_RENDERING_WORKAROUND */


KisBrushBasedPaintOp::KisBrushBasedPaintOp(const KisPropertiesConfiguration* settings, KisPainter* painter)
    : KisPaintOp(painter)
{
    Q_ASSERT(settings);

#ifdef HAVE_THREADED_TEXT_RENDERING_WORKAROUND
    m_brush =
        TextBrushInitializationWorkaround::instance()->tryGetBrush(settings);
#endif /* HAVE_THREADED_TEXT_RENDERING_WORKAROUND */

    if (!m_brush) {
        KisBrushOption brushOption;
        brushOption.readOptionSetting(settings);
        m_brush = brushOption.brush();
    }

    m_precisionOption.readOptionSetting(settings);
    m_dabCache = new KisDabCache(m_brush);
    m_dabCache->setPrecisionOption(&m_precisionOption);

    m_mirrorOption.readOptionSetting(settings);
    m_dabCache->setMirrorPostprocessing(&m_mirrorOption);

    m_textureProperties.fillProperties(settings);
    m_dabCache->setTexturePostprocessing(&m_textureProperties);
}

KisBrushBasedPaintOp::~KisBrushBasedPaintOp()
{
    delete m_dabCache;
}

bool KisBrushBasedPaintOp::checkSizeTooSmall(qreal scale)
{
    scale *= m_brush->scale();

    return scale * m_brush->width() < 0.01 ||
           scale * m_brush->height() < 0.01;
}

KisSpacingInformation KisBrushBasedPaintOp::effectiveSpacing(qreal scale, qreal rotation) const
{
    return effectiveSpacing(scale, rotation, 1.0, false);
}

KisSpacingInformation KisBrushBasedPaintOp::effectiveSpacing(qreal scale, qreal rotation, const KisPressureSpacingOption &spacingOption, const KisPaintInformation &pi) const
{
    qreal extraSpacingScale = 1.0;
    if (spacingOption.isChecked()) {
        extraSpacingScale = spacingOption.apply(pi);
    }

    QSizeF metric = m_brush->characteristicSize(scale, scale, rotation);
    return effectiveSpacing(metric.width(), metric.height(), extraSpacingScale, spacingOption.isotropicSpacing());
}

inline qreal calcAutoSpacing(qreal value, qreal coeff)
{
    return coeff * (value < 1.0 ? value : sqrt(value));
}

inline QPointF calcAutoSpacing(const QPointF &pt, qreal coeff)
{
    return QPointF(calcAutoSpacing(pt.x(), coeff), calcAutoSpacing(pt.y(), coeff));
}

KisSpacingInformation KisBrushBasedPaintOp::effectiveSpacing(qreal dabWidth, qreal dabHeight, qreal extraScale, bool isotropicSpacing) const
{
    QPointF spacing;

    if (!isotropicSpacing) {
        if (m_brush->autoSpacingActive()) {
            spacing = calcAutoSpacing(QPointF(dabWidth, dabHeight), m_brush->autoSpacingCoeff());
        } else {
            spacing = QPointF(dabWidth, dabHeight);
            spacing *= m_brush->spacing();
        }
    }
    else {
        qreal significantDimension = qMax(dabWidth, dabHeight);
        if (m_brush->autoSpacingActive()) {
            significantDimension = calcAutoSpacing(significantDimension, m_brush->autoSpacingCoeff());
        } else {
            significantDimension *= m_brush->spacing();
        }
        spacing = QPointF(significantDimension, significantDimension);
    }

    spacing *= extraScale;

    return spacing;
}

bool KisBrushBasedPaintOp::canPaint() const
{
    return m_brush != 0;
}
