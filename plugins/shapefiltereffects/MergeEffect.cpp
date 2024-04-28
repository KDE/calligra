/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MergeEffect.h"
#include "KoViewConverter.h"
#include "KoXmlReader.h"
#include "KoXmlWriter.h"
#include <KLocalizedString>
#include <QPainter>
#include <limits.h>

MergeEffect::MergeEffect()
    : KoFilterEffect(MergeEffectId, i18n("Merge"))
{
    setRequiredInputCount(2);
    setMaximalInputCount(INT_MAX);
}

QImage MergeEffect::processImage(const QImage &image, const KoFilterEffectRenderContext & /*context*/) const
{
    Q_UNUSED(image);

    return image;
}

QImage MergeEffect::processImages(const QVector<QImage> &images, const KoFilterEffectRenderContext & /*context*/) const
{
    int imageCount = images.count();
    if (!imageCount)
        return QImage();

    QImage result = images[0];
    if (imageCount == 1)
        return result;

    QPainter p(&result);

    for (int i = 1; i < imageCount; ++i) {
        p.drawImage(QPoint(), images[i]);
    }

    return result;
}

bool MergeEffect::load(const KoXmlElement &element, const KoFilterEffectLoadingContext &)
{
    if (element.tagName() != id())
        return false;

    int inputCount = inputs().count();
    int inputIndex = 0;
    for (KoXmlNode n = element.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement node = n.toElement();
        if (node.tagName() == "feMergeNode") {
            if (node.hasAttribute("in")) {
                if (inputIndex < inputCount)
                    setInput(inputIndex, node.attribute("in"));
                else
                    addInput(node.attribute("in"));
                inputIndex++;
            }
        }
    }

    return true;
}

void MergeEffect::save(KoXmlWriter &writer)
{
    writer.startElement(MergeEffectId);

    saveCommonAttributes(writer);

    foreach (const QString &input, inputs()) {
        writer.startElement("feMergeNode");
        writer.addAttribute("in", input);
        writer.endElement();
    }

    writer.endElement();
}
