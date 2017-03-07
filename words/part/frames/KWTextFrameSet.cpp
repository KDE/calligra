/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#include "KWTextFrameSet.h"
#include "KWFrame.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWRootAreaProvider.h"
#include "KWRootAreaProviderTextBox.h"
#include "KWDocument.h"
#include "KWCopyShape.h"

#include <KoTextShapeData.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoShapeFactoryBase.h>
#include <KoAnnotationLayoutManager.h>

#include <changetracker/KoChangeTracker.h>

#include <WordsDebug.h>
#include <QTextDocument>
#include <QTextBlock>

KWTextFrameSet::KWTextFrameSet(KWDocument *wordsDocument, Words::TextFrameSetType type)
    : KWFrameSet(Words::TextFrameSet)
    , m_document(new QTextDocument())
    , m_textFrameSetType(type)
    , m_pageManager(wordsDocument->pageManager())
    , m_wordsDocument(wordsDocument)
{
    if(type == Words::OtherTextFrameSet) {
        m_rootAreaProvider = new KWRootAreaProviderTextBox(this);
    } else {
        m_rootAreaProvider = new KWRootAreaProvider(this);
    }
    Q_ASSERT(m_wordsDocument);
    setName(Words::frameSetTypeName(m_textFrameSetType));
    setupDocument();

    debugWords << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType());
}

KWTextFrameSet::~KWTextFrameSet()
{
    debugWords << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType());
    // delete the root area provider first and set to 0 as we don't want relayouting on deletion
    delete m_rootAreaProvider;
    m_rootAreaProvider = 0;
    delete m_document;
}

void KWTextFrameSet::setupShape(KoShape *shape)
{
    Q_ASSERT(shapes().contains(shape));

    KWPage page = m_pageManager->page(shape);
    if (!page.isValid()) {
        // This can happen if the KWFrame was created by someone else at a time where we didn't start
        // layouting yet and therefore things are not proper setup yet. That's okay and they will be
        // proper setup at a later time once we start layouting.
    } else {
        m_rootAreaProvider->clearPages(page.pageNumber());
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    if (!data) {
        // copy-frames don't need to be setup cause they only point to the referenced KWFrame which
        // contains everything needed and which was or will be proper setup.
        Q_ASSERT(dynamic_cast<KWCopyShape*>(shape));
        return;
    }

    debugWords << "frameSet=" << this << "shape=" << shape << "pageNumber=" << page.pageNumber();

    if (Words::isHeaderFooter(this)) {
        // header and footer are always auto-grow-height independent of whatever
        // was defined for them in the document.
        data->setResizeMethod(KoTextShapeDataBase::AutoGrowHeight);
    }

    if(textFrameSetType() != Words::OtherTextFrameSet) {
        shape->setGeometryProtected(true);
    }
 
    // We need to keep collision detection on or we will not relayout when page anchored shapes are
    // moved. For page anchored shapes (which are different from anchored shapes which are usually
    // children of the shape they are anchored too and therefore the ShapeManager filters collision
    // events for them out) the KoTextRootAreaProvider::relevantObstructions method is used to produce
    // obstructions whereas for anchored shapes the KoTextDocumentLayout::registerAnchoredObstruction
    // is used to explicit register the obstructions.
    //shape->setCollisionDetection(false);

    // Handle the special case that the KoTextShapeData already defines a QTextDocument that we need
    // to take over. This is the case with OtherTextFrameSet's where the KWTextFrameSet
    // and the KWFrame are created after the TextShape was created and it's loadOdf was called which
    // means that the QTextDocument of the KoTextShapeData already has content we like to take over.
    if (textFrameSetType() == Words::OtherTextFrameSet && shapeCount() == 1 && data->document() && m_document->isEmpty() && !data->document()->isEmpty()) {
        // FIXME probably better to test if rangemanager has anything rather than tesing if shape
        // is not empty
        Q_ASSERT(m_document != data->document());
        delete m_document;
        m_document = data->document();
        setupDocument();
    }

    // The QTexDocument is shared between the shapes and we are the owner.
    data->setDocument(m_document, false);

#ifndef QT_NO_DEBUG // these checks are just sanity checks in development mode
    KoTextDocument doc(m_document);
    KoStyleManager *styleManager = m_wordsDocument->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(doc.styleManager() == styleManager);
    KoChangeTracker *changeTracker = m_wordsDocument->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    Q_ASSERT(doc.changeTracker() == changeTracker);
    Q_ASSERT(doc.inlineTextObjectManager() == m_wordsDocument->inlineTextObjectManager());
    Q_ASSERT(doc.undoStack() == m_wordsDocument->resourceManager()->undoStack());
#endif
}

void KWTextFrameSet::cleanupShape(KoShape *shape) {
    // it is no longer set when document is destroyed
    if (rootAreaProvider()) {
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(document()->documentLayout());
        Q_ASSERT(lay);
        QList<KoTextLayoutRootArea *> layoutRootAreas = lay->rootAreas();
        for(int i = 0; i < layoutRootAreas.count(); ++i) {
            KoTextLayoutRootArea *rootArea = layoutRootAreas[i];
            if (rootArea->associatedShape() == shape) {
                KoTextLayoutRootArea *prevRootArea = i >= 1 ? layoutRootAreas[i - 1] : 0;
                rootAreaProvider()->releaseAllAfter(prevRootArea);
                lay->removeRootArea(prevRootArea);
                rootArea->setAssociatedShape(0);
                break;
            }
        }
    }
}

void KWTextFrameSet::setupDocument()
{
    m_document->setUseDesignMetrics(true);

    KoTextDocument doc(m_document);
    doc.setInlineTextObjectManager(m_wordsDocument->inlineTextObjectManager());
    doc.setTextRangeManager(m_wordsDocument->textRangeManager());
    KoStyleManager *styleManager = m_wordsDocument->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    doc.setStyleManager(styleManager);
    KoChangeTracker *changeTracker = m_wordsDocument->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    doc.setChangeTracker(changeTracker);
    doc.setUndoStack(m_wordsDocument->resourceManager()->undoStack());
    doc.setShapeController(m_wordsDocument->shapeController());

    doc.setRelativeTabs(true);
    doc.setParaTableSpacingAtStart(true);

    // the KoTextDocumentLayout needs to be setup after the actions above are done to prepare the document
    KoTextDocumentLayout *lay = new KoTextDocumentLayout(m_document, m_rootAreaProvider);
    lay->setWordprocessingMode();

    QObject::connect(lay, SIGNAL(foundAnnotation(KoShape*,QPointF)),
                     m_wordsDocument->annotationLayoutManager(), SLOT(registerAnnotationRefPosition(KoShape*,QPointF)));

    m_document->setDocumentLayout(lay);
    QObject::connect(lay, SIGNAL(layoutIsDirty()), lay, SLOT(scheduleLayout()));
}

void KWTextFrameSet::setPageStyle(const KWPageStyle &style)
{
    debugWords << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType()) << "pageStyleName=" << style.name() << "pageStyleIsValid=" << style.isValid();
    m_pageStyle = style;
    // TODO: check if this is really needed here, when KWFrameLayout::layoutFramesOnPage() also
    // ensures the background is set. Especially as the separator data is only set there to the text background shape
    if (style.isValid()) {
        foreach(KoShape *shape, shapes()) {
            shape->setBackground(style.background());
        }
    }
}

const KWPageStyle& KWTextFrameSet::pageStyle() const
{
    return m_pageStyle;
}
