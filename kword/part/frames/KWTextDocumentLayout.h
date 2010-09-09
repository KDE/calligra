/* This file is part of the KDE project
 * Copyright (C) 2006-2007, 2009 Thomas Zander <zander@kde.org>
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

#ifndef KWTEXTDOCUMENTLAYOUT_H
#define KWTEXTDOCUMENTLAYOUT_H

#include "kword_export.h"

#include <KoTextDocumentLayout.h>

class KWTextFrameSet;
class KWAnchorStrategy;
class Outline;

#define MIN_WIDTH   0.01f
/**
 * KWords text layouter that allows text to flow in multiple frames and around
 * other KWord objects.
 */
class KWORD_TEST_EXPORT KWTextDocumentLayout : public KoTextDocumentLayout
{
public:
    /// constructor
    explicit KWTextDocumentLayout(KWTextFrameSet *frameSet);
    ~KWTextDocumentLayout();

    /// reimplemented from KoTextDocumentLayout::layout()
    void layout();

    /// reimplemented from KoTextDocumentLayout::shapes()
    QList<KoShape*> shapes() const;

protected:
    friend class KWTextFrameSet;

private:
    /// reimplemented from KoTextDocumentLayout::relayout()
    virtual void relayout();

    virtual void positionInlineObject(QTextInlineObject item, int position, const QTextFormat &format);

    void cleanupAnchors();

    KWTextFrameSet *m_frameSet;
    class DummyShape;
    DummyShape * const m_dummyShape;

    QList<KWAnchorStrategy*> m_activeAnchors;
    QList<KWAnchorStrategy*> m_newAnchors;

    int m_lastKnownFrameCount;
};

#endif
