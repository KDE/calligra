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
#include "KWDocument.h"
#include "KWCopyShape.h"

#include <KoTextShapeData.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextDocumentLayout.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>

#include <changetracker/KoChangeTracker.h>

#include <klocale.h>
#include <kdebug.h>
#include <QTextDocument>
#include <QTextBlock>

KWTextFrameSet::KWTextFrameSet(KWDocument *wordsDocument, Words::TextFrameSetType type)
    : KWFrameSet(Words::TextFrameSet)
    , m_document(new QTextDocument())
    , m_textFrameSetType(type)
    , m_pageManager(wordsDocument->pageManager())
    , m_wordsDocument(wordsDocument)
    , m_rootAreaProvider(new KWRootAreaProvider(this))
{
    Q_ASSERT(m_wordsDocument);
    setName(Words::frameSetTypeName(m_textFrameSetType));
    setupDocument();

    kDebug(32001) << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType());
}

KWTextFrameSet::~KWTextFrameSet()
{
    kDebug(32001) << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType());
    delete m_rootAreaProvider;
    delete m_document;
}

void KWTextFrameSet::setupFrame(KWFrame *frame)
{
    Q_ASSERT(frame->shape());
    Q_ASSERT(frame->frameSet() == this);
    Q_ASSERT(frames().contains(frame));

    KWPage page = m_pageManager->page(frame->shape());
    if (!page.isValid()) {
        // This can happen if the KWFrame was created by someone else at a time where we didn't start
        // layouting yet and therefore things are not proper setup yet. That's okay and they will be
        // proper setup at a later time once we start layouting.
    } else {
        if (page.pageNumber() <= m_rootAreaProvider->pages().count()) {
            // The just added KWFrame needs to invalidate the layouter so the layouter picks up the new
            // KWFrame on the next layout-run.
            m_rootAreaProvider->clearPages(page.pageNumber());
        }
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
    if (!data) {
        // copy-frames don't need to be setup cause they only point to the referenced KWFrame which
        // contains everything needed and which was or will be proper setup.
        Q_ASSERT(dynamic_cast<KWCopyShape*>(frame->shape()));
        return;
    }

    kDebug(32001) << "frameSet=" << frame->frameSet() << "frame=" << frame << "pageNumber=" << page.pageNumber();

    // Handle the special case that the KoTextShapeData already defines a QTextDocument that we need
    // to take over. This is the case for example with OtherTextFrameSet's where the KWTextFrameSet
    // and the KWFrame are created after the TextShape was created and it's loadOdf was called what
    // means that the QTextDocument of the KoTextShapeData already has content we like to take over.
    // The mainTextFrame's are created on demand and need to be ignored.
    if (textFrameSetType() != Words::MainTextFrameSet && frameCount() == 1 && data->document() && m_document->isEmpty()) {
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

void KWTextFrameSet::setupDocument()
{
    m_document->setUseDesignMetrics(true);

    KoTextDocument doc(m_document);
    doc.setInlineTextObjectManager(m_wordsDocument->inlineTextObjectManager());
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
    m_document->setDocumentLayout(lay);
    QObject::connect(lay, SIGNAL(layoutIsDirty()), lay, SLOT(scheduleLayout()));
}

void KWTextFrameSet::setPageStyle(const KWPageStyle &style)
{
    kDebug(32001) << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType()) << "pageStyleName=" << style.name() << "pageStyleIsValid=" << style.isValid();
    m_pageStyle = style;
    if (style.isValid()) {
        foreach(KWFrame* frame, frames()) {
            if (frame->shape()) {
                frame->shape()->setBackground(style.background());
            }
        }
    }
}

const KWPageStyle& KWTextFrameSet::pageStyle() const
{
    return m_pageStyle;
}
