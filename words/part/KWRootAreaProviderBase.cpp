/* This file is part of the KDE project
 * Copyright (C) 2010-2015 C. Boemann <cbo@boemann.dk>
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

#include "KWRootAreaProviderBase.h"
#include "KWPageManager.h"
#include "KWDocument.h"
#include "KWView.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWCopyShape.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeFactoryBase.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutObstruction.h>
#include <KoShapeAnchor.h>


KWRootAreaProviderBase::KWRootAreaProviderBase(KWTextFrameSet *textFrameSet)
    : KoTextLayoutRootAreaProvider()
    , m_textFrameSet(textFrameSet)
{
}

void KWRootAreaProviderBase::doPostLayout(KoTextLayoutRootArea *rootArea, bool /*isNewRootArea*/)
{
    KoShape *shape = rootArea->associatedShape();
    if (!shape) {
        return;
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    QRectF updateRect = shape->outlineRect();

    QSizeF newSize = shape->size()
                    - QSizeF(data->leftPadding() + data->rightPadding(),
                             data->topPadding() + data->bottomPadding());

    KoBorder *border = shape->border();

    if (border) {
        newSize -= QSizeF(border->borderWidth(KoBorder::LeftBorder) + border->borderWidth(KoBorder::RightBorder), border->borderWidth(KoBorder::TopBorder) + border->borderWidth(KoBorder::BottomBorder));
    }

    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {

        newSize.setHeight(rootArea->bottom() - rootArea->top());

        // adjust size to have at least the defined minimum height
        Q_ASSERT(frameSet()->shapeCount() > 0);
        KoShape *firstShape = frameSet()->shapes().first();
        if (firstShape->minimumHeight() > newSize.height())
            newSize.setHeight(firstShape->minimumHeight());
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

void KWRootAreaProviderBase::updateAll()
{
    foreach (KoShape *shape, frameSet()->shapes()) {
        shape->update();
    }
}

QRectF KWRootAreaProviderBase::suggestRect(KoTextLayoutRootArea *rootArea)
{
    KoShape *shape = rootArea->associatedShape();
    if (!shape) { // no shape => nothing to draw => no space needed
        return QRectF(0., 0., 0.,0.);
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    QRectF rect(QPointF(), shape->size());
    rect.adjust(data->leftPadding(), data->topPadding(), -data->rightPadding(), - data->bottomPadding());

    KoBorder *border = shape->border();
    if (border) {
        rect.adjust(border->borderWidth(KoBorder::LeftBorder),  border->borderWidth(KoBorder::TopBorder),
              -border->borderWidth(KoBorder::RightBorder), - border->borderWidth(KoBorder::BottomBorder));
    }

    rect.setWidth(qMax(rect.width(), qreal(1.0)));
    rect.setHeight(qMax(rect.height(), qreal(1.0)));
    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight || data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {
        rect.setHeight(1E6);
    }

    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        || data->resizeMethod() == KoTextShapeData::AutoGrowWidth) {
        // By setting this we make sure the textlayout librar does 2 internal runs. Once to
        // figure out how much width is needed, second to do a normal layout using that calculated width
        rootArea->setNoWrap(1E6);
    }

    return rect;
}

QList<KoTextLayoutObstruction *> KWRootAreaProviderBase::relevantObstructions(KoTextLayoutRootArea *rootArea)
{
    QList<KoTextLayoutObstruction*> obstructions;
    Q_ASSERT(rootArea);

    KoShape *currentShape = rootArea->associatedShape();

    if(!currentShape) {
        return obstructions;
    }

    // let's convert into canvas/KWDocument coords
    QRectF rect = currentShape->boundingRect();

    //TODO would probably be faster if we could use the RTree of the shape manager
    foreach (KWFrameSet *fs, frameSet()->wordsDocument()->frameSets()) {
        if (fs  == frameSet()) {
            continue; // we don't collide with ourselves
        }

        if (KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs)) {
            if (tfs->textFrameSetType() != Words::OtherTextFrameSet) {
                continue; // we don't collide with headers, footers and main-text.
            }
        }

        foreach (KoShape *shape, fs->shapes()) {
            if (shape == currentShape) {
                continue;
            }
            if (! shape->isVisible(true)) {
                continue;
            }
            if (shape->anchor() && shape->anchor()->anchorType() != KoShapeAnchor::AnchorPage) {
                continue;
            }
            if (shape->textRunAroundSide() == KoShape::RunThrough) {
                continue;
            }
            if (shape->zIndex() <= currentShape->zIndex()) {
                continue;
            }
            if (! rect.intersects(shape->boundingRect())) {
                continue;
            }
            bool isChild = false;
            KoShape *parent = shape->parent();
            while (parent && !isChild) {
                if (parent == currentShape) {
                    isChild = true;
                }
                parent = parent->parent();
            }
            if (isChild) {
                continue;
            }
            QTransform matrix = shape->absoluteTransformation(0);
            matrix = matrix * currentShape->absoluteTransformation(0).inverted();
            matrix.translate(0, rootArea->top());
            KoTextLayoutObstruction *obstruction = new KoTextLayoutObstruction(shape, matrix);
            obstructions.append(obstruction);
        }
    }

    return obstructions;
}
