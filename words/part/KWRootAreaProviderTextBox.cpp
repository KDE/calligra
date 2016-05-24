/* This file is part of the KDE project
 * Copyright (C) 2010-2012 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2006,2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2006-2007, 2010 Thomas Zander <zander@kde.org>
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

#include "KWRootAreaProviderTextBox.h"
#include "KWRootAreaProvider.h"
#include "KWPageManager.h"
#include "KWDocument.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWCopyShape.h"
#include "frames/KWFrameLayout.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>

#include <WordsDebug.h>

KWRootAreaProviderTextBox::KWRootAreaProviderTextBox(KWTextFrameSet *textFrameSet)
    : KWRootAreaProviderBase(textFrameSet)
{
}

KWRootAreaProviderTextBox::~KWRootAreaProviderTextBox()
{
    //FIXME : this code crashes so far, why ?
    /*foreach (QList<KoTextLayoutRootArea*> areaList, m_rootAreaCache.values())
        qDeleteAll(areaList);*/
    m_rootAreaCache.clear();
}

void KWRootAreaProviderTextBox::clearPages(int /*pageNumber*/)
{
    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(frameSet()->document()->documentLayout());
    Q_ASSERT(lay);
    foreach(KoTextLayoutRootArea *area, m_rootAreaCache) {
        releaseAllAfter(area);
        lay->removeRootArea(area);
    }
}

void KWRootAreaProviderTextBox::setPageDirty(int /*pageNumber*/)
{
    foreach(KoTextLayoutRootArea *rootArea, m_rootAreaCache) {
        rootArea->setDirty(); // be sure the root-areas are relayouted
    }
}


KoTextLayoutRootArea *KWRootAreaProviderTextBox::provide(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &, int requestedPosition, bool *isNewArea)
{
    KWPageManager *pageManager = frameSet()->wordsDocument()->pageManager();
    Q_ASSERT(pageManager);
    if (pageManager->pageCount() == 0) { // not ready yet (may happen e.g. on loading a document)
        return 0;
    }

    if (m_rootAreaCache.size() > requestedPosition)
    {
        KoTextLayoutRootArea *rootArea = m_rootAreaCache[requestedPosition];
        Q_ASSERT(rootArea);

        // No constraints except for the main frame set
        *isNewArea = false;
        return rootArea;
    }

    if (!documentLayout->referencedLayout()) {
        KoTextDocumentLayout *reflay = dynamic_cast<KoTextDocumentLayout*>(frameSet()->wordsDocument()->frameLayout()->mainFrameSet()->document()->documentLayout());
        documentLayout->setReferencedLayout(reflay);
    }

    if (frameSet()->shapeCount() > requestedPosition) {
        KoShape *shape = frameSet()->shapes()[requestedPosition];
        Q_ASSERT(shape);

        KoTextLayoutRootArea *area = new KoTextLayoutRootArea(documentLayout);
        m_rootAreaCache.append(area);
        Q_ASSERT(m_rootAreaCache.size() > requestedPosition);

        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
        if (data) {
            data->setRootArea(area);
            area->setAssociatedShape(shape);
        } else {
            warnWords << "shape has no KoTextShapeData";
        }

        if ((!shape->anchor()) || shape->anchor()->anchorType() == KoShapeAnchor::AnchorPage) {
            KWPage *page = new KWPage(pageManager->page(shape));
            area->setPage(page);
        }


        area->setLayoutEnvironmentResctictions(true, true);

        *isNewArea = true;
        return area;
    }

    *isNewArea = true;

    return 0;
}

// afterThis==nullptr means delete everything
void KWRootAreaProviderTextBox::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    while (m_rootAreaCache.count() != 0 && m_rootAreaCache.back() != afterThis){
        m_rootAreaCache.removeLast();
    }
}

void KWRootAreaProviderTextBox::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    KWDocument *kwdoc = const_cast<KWDocument*>(frameSet()->wordsDocument());
    KWPageManager *pageManager = kwdoc->pageManager();
    Q_ASSERT(pageManager);

    KoShape *shape = rootArea->associatedShape();
    if (!shape) {
        return;
    }

    KWPage page = pageManager->page(shape);
    Q_ASSERT(page.isValid());

    if (false){//FIXME m_pages.count() > pageManager->pageCount()) {
        // we need to wait for the mainFrameSet to finish till we are able to continue
        KWRootAreaProvider *provider = (KWRootAreaProvider *)kwdoc->frameLayout()->mainFrameSet()->rootAreaProvider();
        provider->addDependentProvider(this, page.pageNumber());
    }


    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    debugWords << "pageNumber=" << page.pageNumber() << "frameSetType=" << "isNewRootArea=" << isNewRootArea << "rootArea=" << rootArea << "isDirty=" << rootArea->isDirty();

    QRectF updateRect = shape->outlineRect();

    QSizeF newSize = shape->size()
                    - QSizeF(data->leftPadding() + data->rightPadding(),
                             data->topPadding() + data->bottomPadding());

    KoBorder *border = shape->border();

    if (border) {
        newSize -= QSizeF(border->borderWidth(KoBorder::LeftBorder) + border->borderWidth(KoBorder::RightBorder), border->borderWidth(KoBorder::TopBorder) + border->borderWidth(KoBorder::BottomBorder));
    }

    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidth) {
        newSize.setWidth(rootArea->right() - rootArea->left());
    }

    newSize += QSizeF(data->leftPadding() + data->rightPadding(),
                      data->topPadding() + data->bottomPadding());
    if (border) {
        newSize += QSizeF(border->borderWidth(KoBorder::LeftBorder) + border->borderWidth(KoBorder::RightBorder), border->borderWidth(KoBorder::TopBorder) + border->borderWidth(KoBorder::BottomBorder));
    }

    if (newSize != rootArea->associatedShape()->size()) {
        rootArea->associatedShape()->setSize(newSize);

        // transfer the new size to the copy-shapes
        foreach(KWCopyShape *cs, frameSet()->copyShapes()) {
            cs->setSize(newSize);
        }
    }

    updateRect |= rootArea->associatedShape()->outlineRect();
    rootArea->associatedShape()->update(updateRect);
}

QRectF KWRootAreaProviderTextBox::suggestRect(KoTextLayoutRootArea *rootArea)
{
    KoShape *shape = rootArea->associatedShape();
    if (!shape) { // no shape => nothing to draw => no space needed
        return QRectF(0., 0., 0.,0.);
    }

    QRectF rect = KWRootAreaProviderBase::suggestRect(rootArea);

    if (frameSet()->shapes().back() == shape) {
        rect.setHeight(1E6);
    }

    return rect;
}

