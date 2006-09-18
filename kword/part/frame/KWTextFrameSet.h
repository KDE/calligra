/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#ifndef KWTEXTFRAMESET_H
#define KWTEXTFRAMESET_H

#include "KWFrameSet.h"
#include "kword_export.h"

class QTextDocument;

/**
 * A frameset with a TextDocument backing it.
 */
class KWORD_TEST_EXPORT KWTextFrameSet : public KWFrameSet {
    Q_OBJECT
public:
    /// normal constructor, for user text
    KWTextFrameSet();
    /**
     * Constructor with a type of text specified
     * @param type the type of frameSet; this can indicate headers, footers etc.
     */
    KWTextFrameSet(KWord::TextFrameSetType type);
    ~KWTextFrameSet();

    /// return the type of frameSet this is
    KWord::TextFrameSetType textFrameSetType() { return m_textFrameSetType; }

    /// return the document with the text that belongs to this frameset.
    QTextDocument *document() const { return m_document; }

    /// return true if the content of this frameset should not be allowed to be altered
    bool protectContent() const { return m_protectContent; }
    /// set wheater the content of this frameset should not be allowed to be altered
    void setProtectContent(bool protect) { m_protectContent = protect; }

#ifndef NDEBUG
    void printDebug();
    void printDebug(KWFrame *frame);
#endif

signals:
    /**
     * Emitted when the frameset finished layout and found that there is more
     * text then will fit in the frameset.
     * Signal will be emitted only when the policy of the last frame allows it.
     */
    void moreFramesNeeded(KWTextFrameSet *fs);

protected:
    friend class KWTextDocumentLayout;
    void setupFrame(KWFrame *frame);
    void requestMoreFrames();
    void framesEmpty(int framesInUse);

protected slots:
    void relayout();
    void requestLayout();

private:
    QTextDocument *m_document;
    bool m_protectContent, m_layoutTriggered;
    KWord::TextFrameSetType m_textFrameSetType;
};

#endif
