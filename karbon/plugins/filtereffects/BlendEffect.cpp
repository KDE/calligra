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

#include "BlendEffect.h"
#include "KoFilterEffectRenderContext.h"
#include "KoXmlWriter.h"
#include "KoXmlReader.h"
#include <KLocale>
#include <QtCore/QRect>
#include <math.h>

BlendEffect::BlendEffect()
        : KoFilterEffect(BlendEffectId, i18n("Blend"))
        , m_blendMode(Normal)
{
    setRequiredInputCount(2);
    setMaximalInputCount(2);
}

BlendEffect::BlendMode BlendEffect::blendMode() const
{
    return m_blendMode;
}

void BlendEffect::setBlendMode(BlendMode blendMode)
{
    m_blendMode = blendMode;
}

QImage BlendEffect::processImage(const QImage &image, const KoFilterEffectRenderContext &context) const
{
    Q_UNUSED(context);
    return image;
}

QImage BlendEffect::processImages(const QList<QImage> &images, const KoFilterEffectRenderContext &context) const
{
    int imageCount = images.count();
    if (!imageCount)
        return QImage();

    QImage result = images[0];
    if (images.count() != 2) {
        return result;
    }

    QRgb *src = (QRgb*)images[1].bits();
    QRgb *dst = (QRgb*)result.bits();
    int w = result.width();

    qreal sa, sr, sg, sb;
    qreal da, dr, dg, db;
    int pixel = 0;

    QRect roi = context.filterRegion().toRect();
    for (int row = roi.top(); row < roi.bottom(); ++row) {
        for (int col = roi.left(); col < roi.right(); ++col) {
            pixel = row * w + col;
            const QRgb &s = src[pixel];
            QRgb &d = dst[pixel];

            sa = qAlpha(s) / 255.0;
            sr = qRed(s) / 255.0;
            sb = qBlue(s) / 255.0;
            sg = qGreen(s) / 255.0;

            da = qAlpha(d) / 255.0;
            dr = qRed(d) / 255.0;
            db = qBlue(d) / 255.0;
            dg = qGreen(d) / 255.0;

            switch (m_blendMode) {
            case Normal:
                dr = (1.0 - da) * sr + dr;
                dg = (1.0 - da) * sg + dg;
                db = (1.0 - da) * sb + db;
                break;
            case Multiply:
                dr = (1.0 - da) * sr + (1.0 - sa) * dr + dr * sr;
                dg = (1.0 - da) * sg + (1.0 - sa) * dg + dg * sg;
                db = (1.0 - da) * sb + (1.0 - sa) * db + db * sb;
                break;
            case Screen:
                dr = sr + dr - dr * sr;
                dg = sg + dg - dg * sg;
                db = sb + db - db * sb;
                break;
            case Darken:
                dr = qMin((1.0 - da) * sr + dr, (1.0 - sa) * dr + sr);
                dg = qMin((1.0 - da) * sg + dg, (1.0 - sa) * dg + sg);
                db = qMin((1.0 - da) * sb + db, (1.0 - sa) * db + sb);
                break;
            case Lighten:
                dr = qMax((1.0 - da) * sr + dr, (1.0 - sa) * dr + sr);
                dg = qMax((1.0 - da) * sg + dg, (1.0 - sa) * dg + sg);
                db = qMax((1.0 - da) * sb + db, (1.0 - sa) * db + sb);
                break;
            }
            da = 1.0 - (1.0 - da) * (1.0 - sa);

            d = qRgba(static_cast<quint8>(qBound(0.0, dr * 255.0, 255.0)),
                      static_cast<quint8>(qBound(0.0, dg * 255.0, 255.0)),
                      static_cast<quint8>(qBound(0.0, db * 255.0, 255.0)),
                      static_cast<quint8>(qBound(0.0, da * 255.0, 255.0)));
        }
    }

    return result;
}

bool BlendEffect::load(const KoXmlElement &element, const QMatrix &matrix)
{
    Q_UNUSED(matrix);

    if (element.tagName() != id())
        return false;

    m_blendMode = Normal; // default blend mode

    QString modeStr = element.attribute("mode");
    if (!modeStr.isEmpty()) {
        if (modeStr == "multiply")
            m_blendMode = Multiply;
        else if (modeStr == "screen")
            m_blendMode = Screen;
        else if (modeStr == "darken")
            m_blendMode = Darken;
        else if (modeStr == "lighten")
            m_blendMode = Lighten;
    }

    if (element.hasAttribute("in2")) {
        if (inputs().count() == 2)
            setInput(1, element.attribute("in2"));
        else
            addInput(element.attribute("in2"));
    }

    return true;
}

void BlendEffect::save(KoXmlWriter &writer)
{
    writer.startElement(BlendEffectId);

    saveCommonAttributes(writer);

    switch (m_blendMode) {
    case Normal:
        writer.addAttribute("mode", "normal");
        break;
    case Multiply:
        writer.addAttribute("mode", "multiply");
        break;
    case Screen:
        writer.addAttribute("mode", "screen");
        break;
    case Darken:
        writer.addAttribute("mode", "darken");
        break;
    case Lighten:
        writer.addAttribute("mode", "lighten");
        break;
    }

    writer.addAttribute("in2", inputs().at(1));

    writer.endElement();
}
