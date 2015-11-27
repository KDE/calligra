/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * Copyright (C) 2007-2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWOdfSharedLoadingData.h"
#include "KWOdfLoader.h"
#include "KWDocument.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWCopyShape.h"

#include <KoTextShapeData.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>
#include <KoUnit.h>

#include <QTextCursor>

KWOdfSharedLoadingData::KWOdfSharedLoadingData(KWOdfLoader *loader)
        : KoTextSharedLoadingData()
        , m_loader(loader)
{
    KoShapeLoadingContext::addAdditionalAttributeData(
        KoShapeLoadingContext::AdditionalAttributeData(
            KoXmlNS::text, "anchor-type", "text:anchor-type"));
    KoShapeLoadingContext::addAdditionalAttributeData(
        KoShapeLoadingContext::AdditionalAttributeData(
            KoXmlNS::text, "anchor-page-number", "text:anchor-page-number"));
}

void KWOdfSharedLoadingData::shapeInserted(KoShape *shape, const KoXmlElement &element, KoShapeLoadingContext &context)
{
    shape->removeAdditionalAttribute("text:anchor-type");
    const KoXmlElement *style = 0;
    if (element.hasAttributeNS(KoXmlNS::draw, "style-name")) {
        style = context.odfLoadingContext().stylesReader().findStyle(
                    element.attributeNS(KoXmlNS::draw, "style-name"), "graphic",
                    context.odfLoadingContext().useStylesAutoStyles());
    }

    if (shape->shapeId() == "TextShape_SHAPEID") {
        KoXmlElement textBox(KoXml::namedItemNS(element, KoXmlNS::draw, "text-box"));
        if (!textBox.isNull()) {
            QString nextName = textBox.attributeNS(KoXmlNS::draw, "chain-next-name");
            m_nextShapeNames.insert(shape, nextName);
            m_shapesToProcess.append(shape);

            if (textBox.hasAttributeNS(KoXmlNS::fo, "min-height")) {
                shape->setMinimumHeight(KoUnit::parseValue(textBox.attributeNS(KoXmlNS::fo, "min-height")));
                QSizeF newSize = shape->size();
                if (newSize.height() < shape->minimumHeight()) {
                    newSize.setHeight(shape->minimumHeight());
                    shape->setSize(newSize);
                }
            }
            KWTextFrameSet *fs = new KWTextFrameSet(m_loader->document());
            fs->setName(m_loader->document()->uniqueFrameSetName(shape->name()));
            KWFrame *frame = new KWFrame(shape, fs);
            if (style)
                fillFrameProperties(frame, *style);
        }
    } else {
        KWFrameSet *fs = new KWFrameSet();
        fs->setName(m_loader->document()->uniqueFrameSetName(shape->name()));
        KWFrame *frame = new KWFrame(shape, fs);
        if (style)
            fillFrameProperties(frame, *style);
        m_loader->document()->addFrameSet(fs);
    }
}

bool KWOdfSharedLoadingData::fillFrameProperties(KWFrame *frame, const KoXmlElement &style)
{
    KoXmlElement properties(KoXml::namedItemNS(style, KoXmlNS::style, "graphic-properties"));
    if (properties.isNull())
        return frame;

    return true;
}

void KWOdfSharedLoadingData::connectFlowingTextShapes()
{
    while (!m_shapesToProcess.isEmpty()) {
        KoShape *shape = m_shapesToProcess.takeFirst();
        KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *>(KWFrameSet::from(shape));
        m_loader->document()->addFrameSet(fs);

        while (shape) {
            QString nextName = m_nextShapeNames[shape];
            shape = 0;

            foreach (KoShape *s, m_shapesToProcess) {
                if (s->name() == nextName) {
                    shape = s;
                    m_shapesToProcess.removeAll(shape);
                    new KWFrame(shape, fs);
                    break;
                }
            }
        }
    }
}
