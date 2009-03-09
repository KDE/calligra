/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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
#include "frames/KWFrame.h"

#include <KoShape.h>
#include <KoTextAnchor.h>
#include <KoTextDocumentLayout.h>
#include <KoInlineTextObjectManager.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

#include <QTextCursor>
#include <kdebug.h>

KWOdfSharedLoadingData::KWOdfSharedLoadingData(KWOdfLoader* loader)
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

void KWOdfSharedLoadingData::shapeInserted(KoShape* shape)
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

    kDebug(32001) << "text:anchor-type =" << shape->additionalAttribute("text:anchor-type") << shape->additionalAttribute( "text:anchor-page-number" ) << pageNumber;
    shape->removeAdditionalAttribute("text:anchor-type");

    KWFrameSet* fs = new KWFrameSet();
    fs->setName(m_loader->document()->uniqueFrameSetName(shape->name()));
    new KWFrame(shape, fs, pageNumber);
    m_loader->document()->addFrameSet(fs);
}
