/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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
class KWTextFrame;
class KWPageManager;
class KWDocument;

/**
 * A frameset with a TextDocument backing it.
 */
class KWORD_EXPORT KWTextFrameSet : public KWFrameSet {
    Q_OBJECT
public:
    /// normal constructor, for user text
    explicit KWTextFrameSet(const KWDocument *document);
    /**
     * Constructor with a type of text specified
     * @param document the document this frameset belongs to.
     * @param type the type of frameSet; this can indicate headers, footers etc.
     */
    KWTextFrameSet(const KWDocument *document, KWord::TextFrameSetType type);
    ~KWTextFrameSet();

    /// return the type of frameSet this is
    KWord::TextFrameSetType textFrameSetType() { return m_textFrameSetType; }

    /// return the document with the text that belongs to this frameset.
    QTextDocument *document() const { return m_document; }

    /// return true if the content of this frameset should not be allowed to be altered
    bool protectContent() const { return m_protectContent; }
    /// set wheater the content of this frameset should not be allowed to be altered
    void setProtectContent(bool protect) { m_protectContent = protect; }

    /**
     * Sets the flag if this frameset is allowed to automaticall do layout of the textdata.
     * A text will do layouting of the text when the content changes, but also when frames
     * are moved.
     * When lots of changes are made it may be faster to disable layouts for a little while.
     * @param allow if false; text will no longer be layouted until enabled again.  If true,
     *  schedule a layout.
     */
    void setAllowLayout(bool allow);
    bool allowLayout() const;
    /**
     * Set the page manager used by this frameset.
     * If we can't get rid of the dependency on KWDocument, we should remove this variable.
     */
    void setPageManager(const KWPageManager *pageMager) { m_pageManager = pageMager; }
    /// return the pageManager for this frameSet
    const KWPageManager* pageManager() const { return m_pageManager; }
    /// return the document for this frameset
    const KWDocument *kwordDocument() const { return m_kwordDocument; }

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
    /**
     * Call this to make it know that the text we want to layout needs more space to be shown fully.
     * This will resize the frame, or emit a moreFramesNeeded signal based on the settings.
     */
    void requestMoreFrames(double textHeight);
    /// called by the KWTextDocumentLayout to mark that the frame is bigger then the text in it.
    void spaceLeft(double excessHeight);
    /// called by the KWTextDocumentLayout to mark that there are frames not in use because the text is too short.
    void framesEmpty(int framesInUse);
    /**
     * Schedules a followup schedule run.
     * This method is used to 'chunk' layout runs. It will followup where the last stopped.
     * Calling this multiple times will make sure the relayout() is only called ones.
     */
    void scheduleLayout();

private slots:
    void updateTextLayout();

private:
    QTextDocument *m_document;
    bool m_protectContent, m_layoutTriggered, m_allowLayoutRequests;
    KWord::TextFrameSetType m_textFrameSetType;
    const KWPageManager *m_pageManager;
    const KWDocument *m_kwordDocument;

    static bool sortTextFrames(const KWFrame *frame1, const KWFrame *frame2);
};

#endif
