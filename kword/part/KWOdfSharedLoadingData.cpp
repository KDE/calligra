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
#include "frames/KWTextFrame.h"
#include "frames/KWCopyShape.h"

#include <KoTextShapeData.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

#include <QTextCursor>
#include <kdebug.h>

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
    int pageNumber = -1;
    if (shape->hasAdditionalAttribute("text:anchor-type")) {
        QString anchorType = shape->additionalAttribute("text:anchor-type");
        if (anchorType == "page" && shape->hasAdditionalAttribute("text:anchor-page-number")) {
            pageNumber = shape->additionalAttribute("text:anchor-page-number").toInt();
            if (pageNumber <= 0) {
                pageNumber = -1;
            }
        }
    }

    //kDebug(32001) << "text:anchor-type =" << shape->additionalAttribute("text:anchor-type") << shape->additionalAttribute("text:anchor-page-number") << pageNumber;
    shape->removeAdditionalAttribute("text:anchor-type");
    const KoXmlElement *style = 0;
    if (element.hasAttributeNS(KoXmlNS::draw, "style-name")) {
        style = context.odfLoadingContext().stylesReader().findStyle(
                    element.attributeNS(KoXmlNS::draw, "style-name"), "graphic",
                    context.odfLoadingContext().useStylesAutoStyles());
    }

    KoTextShapeData *text = qobject_cast<KoTextShapeData*>(shape->userData());
    if (text) {
        KWTextFrameSet *fs = 0;
        KWFrame *previous = m_nextFrames.value(shape->name());
        if (previous)
            fs = dynamic_cast<KWTextFrameSet*>(previous->frameSet());
        if (fs == 0) {
            fs = new KWTextFrameSet(m_loader->document());
            fs->setAllowLayout(false);
            fs->setName(m_loader->document()->uniqueFrameSetName(shape->name()));
            m_loader->document()->addFrameSet(fs);
        }

        KWTextFrame *frame = new KWTextFrame(shape, fs, pageNumber);
        if (style) {
            if (! fillFrameProperties(frame, *style))
                return; // done
        }

        KoXmlElement textBox(KoXml::namedItemNS(element, KoXmlNS::draw, "text-box"));
        if (frame && !textBox.isNull()) {
            QString nextFrame = textBox.attributeNS(KoXmlNS::draw, "chain-next-name");
            if (! nextFrame.isEmpty()) {
#ifndef NDEBUG
                if (m_nextFrames.contains(nextFrame))
                    kWarning(32001) << "Document has two frames with the same 'chain-next-name' value, strange things may happen";
#endif
                m_nextFrames.insert(nextFrame, frame);
            }

            if (textBox.hasAttributeNS(KoXmlNS::fo, "min-height")) {
                frame->setMinimumFrameHeight(KoUnit::parseValue(textBox.attributeNS(KoXmlNS::fo, "min-height")));
                KoShape *shape = frame->shape();
                QSizeF newSize = shape->size();
                if (newSize.height() < frame->minimumFrameHeight()) {
                    newSize.setHeight(frame->minimumFrameHeight());
                    shape->setSize(newSize);
                }
            }
        }
    } else {
        KWFrameSet *fs = new KWFrameSet();
        fs->setName(m_loader->document()->uniqueFrameSetName(shape->name()));
        KWFrame *frame = new KWFrame(shape, fs, pageNumber);
        if (style)
            fillFrameProperties(frame, *style);
        m_loader->document()->addFrameSet(fs);
    }
}

bool KWOdfSharedLoadingData::fillFrameProperties(KWFrame *frame, const KoXmlElement &style)
{
    frame->setFrameBehavior(KWord::IgnoreContentFrameBehavior);
    KoXmlElement properties(KoXml::namedItemNS(style, KoXmlNS::style, "graphic-properties"));
    if (properties.isNull())
        return frame;

    QString copy = properties.attributeNS(KoXmlNS::draw, "copy-of");
    if (! copy.isEmpty()) {
        // untested... No app saves this currently..
        foreach (KWFrame *f, frame->frameSet()->frames()) {
            if (f->shape()->name() == copy) {
                KWCopyShape *shape = new KWCopyShape(f->shape());
                new KWFrame(shape, frame->frameSet(), frame->loadingPageNumber());
                delete frame;
                return false;
            }
        }
    }

    QString overflow = properties.attributeNS(KoXmlNS::style, "overflow-behavior", QString());
    if (overflow == "clip")
        frame->setFrameBehavior(KWord::IgnoreContentFrameBehavior);
    else if (overflow == "auto-create-new-frame")
        frame->setFrameBehavior(KWord::AutoCreateNewFrameBehavior);
    else
        frame->setFrameBehavior(KWord::AutoExtendFrameBehavior);
    QString newFrameBehavior = properties.attributeNS(KoXmlNS::koffice, "frame-behavior-on-new-page", QString());
    if (newFrameBehavior == "followup")
        frame->setNewFrameBehavior(KWord::ReconnectNewFrame);
    else if (newFrameBehavior == "copy")
        frame->setNewFrameBehavior(KWord::CopyNewFrame);
    else
        frame->setNewFrameBehavior(KWord::NoFollowupFrame);

    QString margin = properties.attributeNS(KoXmlNS::fo, "margin");
    if (margin.isEmpty())
        margin = properties.attributeNS(KoXmlNS::fo, "margin-left");
    if (margin.isEmpty())
        margin = properties.attributeNS(KoXmlNS::fo, "margin-top");
    if (margin.isEmpty())
        margin = properties.attributeNS(KoXmlNS::fo, "margin-bottom");
    if (margin.isEmpty())
        margin = properties.attributeNS(KoXmlNS::fo, "margin-right");
    frame->setRunAroundDistance(KoUnit::parseValue(margin));

    QString wrap;
    if (properties.hasAttributeNS(KoXmlNS::style, "wrap")) {
        wrap = properties.attributeNS(KoXmlNS::style, "wrap");
    } else {
        // no value given in the file, and for compatibility reasons we do some suggestion on
        // what to use.
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
        if (tfs == 0)
            wrap = "none";
        else
            wrap = "biggest";
    }
    if (wrap == "none") {
        frame->setTextRunAround(KWord::NoRunAround);
    } else if (wrap == "run-through") {
        frame->setTextRunAround(KWord::RunThrough);
    } else {
        frame->setTextRunAround(KWord::RunAround);
        if (wrap == "biggest")
            frame->setRunAroundSide(KWord::BiggestRunAroundSide);
        else if (wrap == "left")
            frame->setRunAroundSide(KWord::LeftRunAroundSide);
        else if (wrap == "right")
            frame->setRunAroundSide(KWord::RightRunAroundSide);
        else if (wrap == "dynamic")
            frame->setRunAroundSide(KWord::AutoRunAroundSide);
        else if (wrap == "parallel")
            frame->setRunAroundSide(KWord::BothRunAroundSide);
    }
    frame->setFrameOnBothSheets(properties.attributeNS(KoXmlNS::koffice,
                "frame-copy-to-facing-pages default").compare("true", Qt::CaseInsensitive));
    return true;
}
