/* This file is part of the KDE project
 * Copyright (C) 2010-2011 Casper Boemann <cbo@boemann.dk>
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

#include "KWRootAreaProvider.h"
#include "KWPageManager.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "KWView.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrameLayout.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutObstruction.h>
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>

#include <QTimer>
#include <kdebug.h>

class KWTextLayoutRootArea : public KoTextLayoutRootArea
{
    public:
        KWTextLayoutRootArea(KoTextDocumentLayout *documentLayout, KWTextFrameSet *frameSet, const KWPage &page) : KoTextLayoutRootArea(documentLayout), m_frameSet(frameSet), m_page(page) {}
        virtual ~KWTextLayoutRootArea() {}
        virtual bool layout(FrameIterator *cursor) {
            kDebug() << "pageNumber=" << m_page.pageNumber() << "frameSetType=" << KWord::frameSetTypeName(m_frameSet->textFrameSetType()) << "isDirty=" << isDirty();
            return KoTextLayoutRootArea::layout(cursor);
        }
        KWTextFrameSet *m_frameSet;
        KWPage m_page; //FIXME hack to prevent the KWPage from going out of scope
};

KWRootAreaProvider::KWRootAreaProvider(KWTextFrameSet *textFrameSet)
    : KoTextLayoutRootAreaProvider()
    , m_textFrameSet(textFrameSet)
{
}

KWRootAreaProvider::~KWRootAreaProvider()
{
}

void KWRootAreaProvider::addDependentProvider(KWRootAreaProvider *provider)
{
    if (!m_dependentProviders.contains(provider)) {
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(provider->m_textFrameSet->document()->documentLayout());
        Q_ASSERT(lay);
        lay->setContinuousLayout(false); // to abort the current layout-loop
        lay->setBlockLayout(true); // to prevent a new layout-loop from being started
        m_dependentProviders.append(provider);
    }
}

void KWRootAreaProvider::handleDependentProviders(int pageNumber)
{
    for(int i = m_dependentProviders.count() - 1; i >= 0; --i) {
        KWRootAreaProvider *provider = m_dependentProviders[i];
        // only handle providers which would continue layouting at the page we just processed
        if (provider->m_pages.count() >= pageNumber) {
            continue;
        }

        m_dependentProviders.removeAt(i);
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(provider->m_textFrameSet->document()->documentLayout());
        Q_ASSERT(lay);
        lay->setContinuousLayout(true); // to not abort the current layout-loop any longer
        lay->setBlockLayout(false); // to allow layouting again
        lay->layout(); // continue layouting but don't schedule so we are sure it's done instantly
    }
}

KoTextLayoutRootArea *KWRootAreaProvider::provide(KoTextDocumentLayout *documentLayout)
{
    KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
    Q_ASSERT(pageManager);
    if (pageManager->pageCount() == 0) // not ready yet (may happen e.g. on loading a document)
        return 0;

    // The page is created in KWTextFrameSet::setupFrame and the TextShape in KWFrameLayout::createTextShape
#if 0
    const KWPageStyle pageStyle(pageManager->pageCount() > 0 ? pageManager->page(pageManager->pageCount() - 1).pageStyle() : pageManager->defaultPageStyle());
    KWPage page = pageManager->appendPage(pageStyle);
    Q_ASSERT(page.isValid());
    KWPage page = m_textFrameSet->kwordDocument()->appendPage();
    Q_ASSERT(page.isValid());
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
    Q_ASSERT(factory);
    KoResourceManager *rm = m_textFrameSet->kwordDocument()->resourceManager();
    KoShape *shape = factory->createDefaultShape(rm);
    Q_ASSERT(shape);
#endif

    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    Q_ASSERT(kwdoc);

    int pageNumber = m_pages.count() + 1;

    kDebug() << "pageNumber=" << pageNumber << "frameSetType=" << KWord::frameSetTypeName(m_textFrameSet->textFrameSetType());

    if (m_textFrameSet->textFrameSetType() == KWord::MainTextFrameSet) {
        // Create missing KWPage's (they will also create a KWFrame and TextShape per page)
        for(int i = pageManager->pageCount(); i <= m_pages.count(); ++i) {
            KWPage page = kwdoc->appendPage();
            Q_ASSERT(page.isValid());
        }
    } else {
        Q_ASSERT_X(!kwdoc->frameLayout()->mainFrameSet()->rootAreaProvider()->m_dependentProviders.contains(this), __FUNCTION__, "Seems we are still attached to the mainFrame but got still asked to layout what should never have happened");
    }

    Q_ASSERT(pageNumber <= pageManager->pageCount());

    handleDependentProviders(pageNumber);

    KWPage page = pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());

    KWFrame *frame = kwdoc->frameLayout()->frameOn(m_textFrameSet, pageNumber);

    if (m_textFrameSet->textFrameSetType() != KWord::OtherTextFrameSet) {
        KWTextFrameSet* tfs = kwdoc->frameLayout()->getFrameSet(m_textFrameSet->textFrameSetType(), m_textFrameSet->pageStyle());
        Q_ASSERT_X(tfs == m_textFrameSet, __FUNCTION__, QString("frameLayout vs rootAreaProvider error, frameSetType=%1 pageNumber=%2 frameCount=%3 pageCount=%4").arg(KWord::frameSetTypeName(m_textFrameSet->textFrameSetType())).arg(pageNumber).arg(m_textFrameSet->frameCount()).arg(pageManager->pageCount()).toLocal8Bit());
    }

    KWTextLayoutRootArea *area = new KWTextLayoutRootArea(documentLayout, m_textFrameSet, page);
    area->setAcceptsPageBreak(true);

    if (frame) {
        KoShape *shape = frame->shape();
        Q_ASSERT(shape);
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
//Q_ASSERT(data);
        if(data) {
            area->setAssociatedShape(shape);
            data->setRootArea(area);
        }
        area->setPage(&area->m_page);
    }

    m_pages.append(area);
    return area;
}

// afterThis==NULL means delete everything
void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    int afterPageNumber = -1;
    if (afterThis) {
        KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
        Q_ASSERT(pageManager);
        KoShape *shape = afterThis->associatedShape();
        Q_ASSERT(shape);
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
        Q_ASSERT(data);
        KWPage page = pageManager->page(shape);
        Q_ASSERT(page.isValid());
        afterPageNumber = page.pageNumber();
    }

    //kDebug() << "afterPageNumber=" << afterPageNumber;

    if (afterPageNumber >= 1) {
        for(int i = m_pages.count(); i > afterPageNumber; --i) {
            KoTextLayoutRootArea *area = m_pages.takeLast();
            //delete area;
        }
    } else {
        //qDeleteAll(m_pages);
        m_pages.clear();
    }
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
    Q_ASSERT(pageManager);

    if (m_textFrameSet->textFrameSetType() != KWord::MainTextFrameSet) {
        if (m_pages.count() >= pageManager->pageCount()) {
            // we need to wait for the mainFrameSet to finish till we are able to continue
            KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
            Q_ASSERT(kwdoc);
            kwdoc->frameLayout()->mainFrameSet()->rootAreaProvider()->addDependentProvider(this);
        }
    }

    KoShape *shape = rootArea->associatedShape();
    if (!shape)
        return;

    KWPage page = pageManager->page(shape);
    Q_ASSERT(page.isValid());
    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);
    bool isHeaderFooter = KWord::isHeaderFooter(m_textFrameSet);

    //kDebug() << "pageNumber=" << page.pageNumber() << "frameSetType=" << KWord::frameSetTypeName(m_textFrameSet->textFrameSetType()) << "isNewRootArea=" << isNewRootArea << "rootArea=" << rootArea << "size=" << rootArea->associatedShape()->size();
    kDebug() << "pageNumber=" << page.pageNumber() << "frameSetType=" << KWord::frameSetTypeName(m_textFrameSet->textFrameSetType()) << m_pages.count() << pageManager->pageCount() << rootArea->isDirty();

    QRectF updateRect = rootArea->associatedShape()->outlineRect();
    //rootArea->associatedShape()->update(updateRect);

    QSizeF newSize = rootArea->associatedShape()->size();
    if (isHeaderFooter
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {
        newSize.setHeight(rootArea->bottom() - rootArea->top());
    }
    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidth) {
        newSize.setWidth(rootArea->right() - rootArea->left());
    }
    if (newSize != rootArea->associatedShape()->size()) {
        //QPointF centerpos = rootArea->associatedShape()->absolutePosition();
        rootArea->associatedShape()->setSize(newSize);
        //rootArea->associatedShape()->setAbsolutePosition(centerpos);

        if (isHeaderFooter) {
            // adjust the minimum frame height for headers and footer
            Q_ASSERT(m_textFrameSet->frameCount() > 0);
            KWFrame *frame = static_cast<KWFrame*>(m_textFrameSet->frames().first());
            if (frame->minimumFrameHeight() != newSize.height()) {
                frame->setMinimumFrameHeight(newSize.height());
                // cause the header/footer's height changed we have to relayout the whole page
                //m_textFrameSet->kwordDocument()->frameLayout()->layoutFramesOnPage(page.pageNumber());
            }
        }
    }

#if 0
    qreal newBottom = rootArea->top() + rootArea->associatedShape()->size().height();
    if (data->verticalAlignment() & Qt::AlignBottom) {
        if (true /*FIXME test no page based shapes interfering*/) {
            rootArea->setVerticalAlignOffset(newBottom - rootArea->bottom());
        }
    }
    if (data->verticalAlignment() & Qt::AlignVCenter) {
        if (true /*FIXME test no page based shapes interfering*/) {
            rootArea->setVerticalAlignOffset((newBottom - rootArea->bottom()) / 2);
        }
    }
#endif

    /* already done in KWDocument::addFrame on shapeManager()->addShape

    // emits KWDocument::pageSetupChanged which calls KWViewMode::updatePageCache
    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    Q_ASSERT(kwdoc);
    kwdoc->firePageSetupChanged();
    */

    updateRect |= rootArea->associatedShape()->outlineRect();
    rootArea->associatedShape()->update(updateRect);
}

bool KWRootAreaProvider::suggestPageBreak(KoTextLayoutRootArea *beforeThis)
{
    if (m_textFrameSet->textFrameSetType() != KWord::MainTextFrameSet)
        return false;
    int index = m_pages.indexOf(beforeThis);
    Q_ASSERT_X(index >= 0, __FUNCTION__, QString("Unknown area on page=%1").arg(m_textFrameSet->kwordDocument()->pageManager()->page(beforeThis->associatedShape()).pageNumber()).toLocal8Bit());
    if (index == 0)
        return false;
    KoTextLayoutRootArea *areaBefore = m_pages[index - 1];
    KoTextPage *pageCurrent = beforeThis->page();
    Q_ASSERT(pageCurrent);
    KoTextPage *pageBefore = areaBefore->page();
    Q_ASSERT(pageBefore);
    return pageCurrent->masterPageName() != pageBefore->masterPageName();
}

QSizeF KWRootAreaProvider::suggestSize(KoTextLayoutRootArea *rootArea)
{
    KoShape *shape = rootArea->associatedShape();
    if (!shape) { // no shape => nothing to draw => no space needed
        return QSizeF(0.,0.);
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight || data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {
        QSizeF size = shape->size();
        size.setHeight(1E6);
        return size;
    }

    return shape->size();
}

QList<KoTextLayoutObstruction *> KWRootAreaProvider::relevantObstructions(const QRectF &textRect, const QList<KoTextLayoutObstruction *> &excludingThese)
{
    QList<KoTextLayoutObstruction*> obstructions;

    KoTextLayoutRootArea *rootArea = 0;
    foreach (KoTextLayoutRootArea *area, m_pages) {
        if (area->boundingRect().intersects(textRect)) {
            rootArea = area;
            break;
        }
    }
    if (!rootArea) {
        return obstructions;
    }

    KoShape *currentShape = rootArea->associatedShape();

    // let's convert into canvas/KWDocument coords
    QRectF rect = textRect;
    rect.moveTop(rect.top() - rootArea->top());
    QTransform transform = currentShape->absoluteTransformation(0);
    rect = transform.mapRect(rect);

    //TODO would probably be faster if we could use the RTree of the shape manager
    foreach (KWFrameSet *fs, m_textFrameSet->kwordDocument()->frameSets()) {
        if (fs  == m_textFrameSet)
            continue;
        foreach (KWFrame *frame, fs->frames()) {
            KoShape *shape = frame->shape();
            if (shape == currentShape)
                continue;
            if (! shape->isVisible(true))
                continue;
            if (shape->textRunAroundSide() == KoShape::RunThrough)
                continue;
            if (shape->zIndex() <= currentShape->zIndex())
                continue;
            if (! rect.intersects(shape->boundingRect()))
                continue;
            bool isChild = false;
            KoShape *parent = shape->parent();
            while (parent && !isChild) {
                if (parent == currentShape)
                    isChild = true;
                parent = parent->parent();
            }
            if (isChild)
                continue;
            QTransform matrix = shape->absoluteTransformation(0);
            matrix = matrix * currentShape->absoluteTransformation(0).inverted();
            matrix.translate(0, rootArea->top());
            KoTextLayoutObstruction *obstruction = new KoTextLayoutObstruction(shape, matrix);
            obstructions.append(obstruction);
        }
    }

    return obstructions;
}

