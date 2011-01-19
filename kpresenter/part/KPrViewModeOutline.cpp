/* This file is part of the KDE project
* Copyright (C) 2010 Ludovic Delfau <ludovicdelfau@gmail.com>
* Copyright (C) 2010 Julien Desgats <julien.desgats@gmail.com>
* Copyright (C) 2010-2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
* Copyright (C) 2010-2011 Benjamin Port <port.benjamin@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (  at your option ) any later version.
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

#include "KPrViewModeOutline.h"
#include "KPresenter.h"
#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAView.h>
#include <KoTextShapeData.h>
#include <KoToolBase.h>
#include <KPrPage.h>
#include <tools/KPrPlaceholderTool.h>
#include <pagelayout/KPrPageLayout.h>
#include <pagelayout/KPrPageLayouts.h>
#include <QTextDocumentFragment>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextList>
#include <QTextListFormat>
#include <QKeyEvent>
#include <QShortcut>
#include <QKeySequence>

#include <KDebug>

#include "KPrOutlineEditor.h"

KPrViewModeOutline::KPrViewModeOutline( KoPAView * view, KoPACanvas * canvas )
: KoPAViewMode(view, canvas)
, m_editor(new KPrOutlineEditor(this, view->parentWidget()))
, m_link()
{
    m_editor->hide();
    connect(m_editor, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    //m_editor->document()->setUndoRedoEnabled(false);


    // sets text format
    m_titleCharFormat.setFontWeight(QFont::Bold);
    m_titleCharFormat.setFontPointSize(14.0);
    m_titleFrameFormat.setTopMargin(12.0);
    m_titleFrameFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    m_titleFrameFormat.setBorder(1);

    m_defaultFrameFormat.setLeftMargin(15.0);
    m_defaultFrameFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    m_defaultFrameFormat.setBorder(1);
}

KPrViewModeOutline::~KPrViewModeOutline()
{
}

void KPrViewModeOutline::paint(KoPACanvasBase* /*canvas*/, QPainter& /*painter*/, const QRectF &/*paintRect*/)
{
}

void KPrViewModeOutline::paintEvent( KoPACanvas * canvas, QPaintEvent* event )
{
    Q_ASSERT( m_canvas == canvas );
    Q_UNUSED(event);
    Q_UNUSED(canvas);
}

void KPrViewModeOutline::tabletEvent( QTabletEvent *event, const QPointF &point )
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mousePressEvent( QMouseEvent *event, const QPointF &point )
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point )
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseMoveEvent( QMouseEvent *event, const QPointF &point )
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseReleaseEvent( QMouseEvent *event, const QPointF &point )
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::keyPressEvent( QKeyEvent *event )
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::keyReleaseEvent( QKeyEvent *event )
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::wheelEvent( QWheelEvent * event, const QPointF &point )
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::activate(KoPAViewMode * previousViewMode)
{
    Q_UNUSED(previousViewMode);

    populate();
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    m_editor->show();
    m_editor->setFocus(Qt::ActiveWindowFocusReason);
}

void KPrViewModeOutline::updateActivePage( KoPAPageBase * page )
{
    if ( m_view->activePage() != page ) {
        m_view->setActivePage( page );
    }
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    setCursorTo(view->kopaDocument()->pageIndex(page), Title);
    m_editor->setFocus(Qt::ActiveWindowFocusReason);
}

void KPrViewModeOutline::slotSelectionChanged()
{
    KoPAPageBase * page = m_view->kopaDocument()->pageByIndex(m_link.value(currentFrame()).numSlide, false);
    if ( m_view->activePage() != page ) {
        m_view->setActivePage(page);
    }
}

void KPrViewModeOutline::enableSync()
{
    connect(m_editor->document(), SIGNAL(contentsChange(int, int, int)), this, SLOT(synchronize(int,int,int)));
}

void KPrViewModeOutline::disableSync()
{
    disconnect(m_editor->document(), SIGNAL(contentsChange(int, int, int)), this, SLOT(synchronize(int,int,int)));
}

void KPrViewModeOutline::populate()
{
    // No synchronization needed while the populate
    disableSync();

    // Record the cursor position
    QTextCursor cur = m_editor->textCursor();
    int recordCursorPosition = m_editor->textCursor().position();

    m_editor->clear();
    m_link.clear();
    QTextCursor cursor = m_editor->document()->rootFrame()->lastCursorPosition();
    int frameRank = 0;
    QTextFrameFormat slideFrameFormat;
    int numSlide = 0;

    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        // For each slides
        foreach (KoPAPageBase * pageBase, view->kopaDocument()->pages()) {
            if (KPrPage * page = dynamic_cast<KPrPage *>(pageBase)) {
                // Set background color alternatively white and grey
                slideFrameFormat.setBackground(QBrush( (frameRank++%2) ? QColor(240,240,240) : QColor(255,255,255) ));
                QTextFrame* slideFrame = cursor.insertFrame(slideFrameFormat);

                // Copy relevant content of the title of the page in the frame
                foreach (OutlinePair pair, page->placeholders().outlineData()) {
                    if (pair.first == Title) {
                        QTextFrame *frame = cursor.insertFrame(m_titleFrameFormat);
                        if (frame != 0 && pair.second != 0) {
                            FrameData frameData = {pair.second->document(), numSlide, pair.first};
                            m_link.insert(frame, frameData); // Create frame and save the link
                            cursor.setCharFormat(m_titleCharFormat);
                            // insert text (create lists where needed)
                            insertText(pair.second->document(), frame, &m_titleCharFormat);
                            kDebug() << "title:" << pair.second->document()->toPlainText();
                        }
                        cursor.setPosition(slideFrame->lastPosition());
                    }
                }

                // Copy relevant content of the text part of the page in the frame
                foreach (OutlinePair pair, page->placeholders().outlineData()) {
                    if (pair.first == Subtitle || pair.first == Outline) {
                        QTextFrame *frame = cursor.insertFrame(m_defaultFrameFormat);
                        if (frame != 0 && pair.second != 0) {
                            FrameData frameData = {pair.second->document(), numSlide, pair.first};
                            m_link.insert(frame, frameData); // Create frame and save the link
                            cursor.setCharFormat(m_defaultCharFormat);
                            // insert text (create lists where needed)
                            insertText(pair.second->document(), frame, &m_defaultCharFormat);
                        }
                        cursor.setPosition(slideFrame->lastPosition());
                    }
                }
            }
            cursor.movePosition(QTextCursor::End);
            numSlide++;
        }
    }

    setCursorTo(m_view->kopaDocument()->pageIndex(m_view->activePage()), Title);
    // Do not forget to reactive the synchronize
    enableSync();

    cur.setPosition((recordCursorPosition > 0) ? recordCursorPosition : 0);
    m_editor->setTextCursor(cur);

}

void KPrViewModeOutline::insertText(QTextDocument* sourceShape, QTextFrame* destinationFrame, QTextCharFormat* charFormat)
{

    kDebug() << "title:" << sourceShape->toPlainText();
    // we  start by insert raw blocks
    QTextCursor destinationFrameCursor = destinationFrame->firstCursorPosition();
    for (QTextBlock currentBlock = sourceShape->begin(); currentBlock.isValid(); currentBlock = currentBlock.next()) {
        destinationFrameCursor.setCharFormat(*charFormat);
        destinationFrameCursor.insertText(currentBlock.text());
        destinationFrameCursor.insertBlock();
    }
    destinationFrameCursor.deletePreviousChar();

    // then we format lists
    QTextList *currentList = 0;
    int currentIndentation = -1;
    for (QTextBlock srcBlock = sourceShape->begin(), destinationBlock = destinationFrame->firstCursorPosition().block();
            srcBlock.isValid() && destinationBlock.isValid();
                srcBlock = srcBlock.next(), destinationBlock = destinationBlock.next()) {
        if (srcBlock.textList()) {
            QTextCursor destinationCursor(destinationBlock);
            if (currentList) {
                currentList->add(destinationBlock);
            }
            else {
                currentList = destinationCursor.createList(QTextListFormat::ListDisc);
            }
            QTextBlockFormat format = destinationBlock.blockFormat();
            currentIndentation = srcBlock.blockFormat().leftMargin();
            format.setLeftMargin(currentIndentation);
            destinationCursor.setBlockFormat(format);
        }
    }
}

void KPrViewModeOutline::deactivate()
{
    m_editor->hide();
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->show();
    }
}

bool KPrViewModeOutline::indent(bool indentation)
{
    QTextCursor cursor = m_editor->textCursor();
    int selectionStart = qMin(cursor.anchor(), cursor.position());
    int selectionEnd = qMax(cursor.anchor(), cursor.position());
    // Ensures the block containing the cursor is selected in that case
    bool noSelection = (selectionStart == selectionEnd);

    // if cursor is not at line start and there is no selection, no ident has to be done
    cursor.movePosition(QTextCursor::StartOfLine);
    int startOfLine = cursor.position();
    cursor.movePosition(QTextCursor::EndOfLine);
    int endOfLine = cursor.position();
    if (selectionStart > startOfLine && selectionEnd <= endOfLine && selectionEnd != startOfLine) {
        kDebug() << "Not indentation needed";
        return false;
    }

    // if selection is through several frames, no indentation should be done, but normal tab key
    // should not be handled either
    cursor.setPosition(selectionStart);
    QTextDocument *targetShape = m_link.value(cursor.currentFrame()).textDocument;
    cursor.setPosition(selectionEnd);
    QTextDocument *selectionEndShape = m_link.value(cursor.currentFrame()).textDocument;

    if (targetShape == 0 || targetShape != selectionEndShape) {
        kDebug() << "Incorrect indentation";
        return true;
    }

    disableSync();
    int frameOffset = cursor.currentFrame()->firstPosition();

    cursor.setPosition(selectionStart);
    cursor.beginEditBlock();

    QTextCursor targetCursor(targetShape);
    targetCursor.beginEditBlock();

    kDebug() << (indentation ? "Indent" : "Unindent") << " from "<<selectionStart << " to " << selectionEnd;

    QTextBlock block = cursor.block();
    for (; block.isValid() && block.textList() && ((block.position() < selectionEnd) || noSelection); block = block.next()) {
        QTextBlockFormat format = block.blockFormat();
        if (indentation || format.leftMargin() > 0) {
            int newMargin = format.leftMargin() + (indentation ? 10 : -10);
            kDebug() << "Indent "<<block.text() << " old indent: " << format.leftMargin() << "; new indent: " << newMargin;
            format.setLeftMargin(newMargin);
            cursor.setPosition(block.position());
            cursor.setBlockFormat(format);

            targetCursor.setPosition(block.position() - frameOffset);
            QTextBlockFormat targetFormat = targetCursor.blockFormat();
            targetFormat.setLeftMargin(newMargin);
            targetCursor.setBlockFormat(targetFormat);
        } else {
            kDebug() << "New slide!";
        }
        noSelection = false;
    }
    cursor.endEditBlock();
    targetCursor.endEditBlock();

    // if the selection is not fully a list, we undo the changes
    if (!block.previous().contains(selectionEnd)) {
        kDebug() << "undo last indent";
        cursor.document()->undo();
        targetCursor.document()->undo();
    }
    enableSync();

    return true;
}

void KPrViewModeOutline::placeholderSwitch()
{
    QTextCursor cur = m_editor->textCursor();
    QTextFrame* currentFrame = cur.currentFrame();
    const FrameData &currentFrameData = m_link.value(currentFrame);

    if (!currentFrameData.textDocument || currentFrameData.type == Title) {
        return;
    }

    // we search the next known frame
    while((cur.currentFrame() == currentFrame ||
        !(m_link.value(cur.currentFrame()).textDocument)) &&
        !cur.atEnd()) {
        cur.movePosition(QTextCursor::NextCharacter);
    }

    if(!cur.atEnd() && m_link.value(cur.currentFrame()).numSlide == currentFrameData.numSlide) {
        m_editor->setTextCursor(cur);
    }
}


void KPrViewModeOutline::addSlide()
{
    int numSlide = m_link.value(currentFrame()).numSlide;
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        // Active the current page and insert page
        view->setActivePage(view->kopaDocument()->pageByIndex(numSlide, false));
        view->insertPage();

        // Search layouts
        KPrPageLayouts * layouts = view->kopaDocument()->resourceManager()->resource(KPresenter::PageLayouts).value<KPrPageLayouts*>();
        Q_ASSERT( layouts );
        const QList<KPrPageLayout *> layoutMap = layouts->layouts();
        // Add the layout 1
        //TODO Find constant for 1
        KPrPage * page = static_cast<KPrPage *>(view->kopaDocument()->pages()[numSlide + 1]);
        page->setLayout(layoutMap[1], view->kopaDocument());
        // Create shapes
        KPrPlaceholderTool tool(static_cast<KoCanvasBase *>(m_canvas));
        foreach (KoShape * shape, page->placeholders().placeholderShape()) {
            tool.activate(KoToolBase::DefaultActivation, QSet<KoShape *>() << shape);
        }
        // Reload the editor
        populate();
        setCursorTo(numSlide + 1, Title);
    }
}

void KPrViewModeOutline::deleteSlide()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        if(view->kopaDocument()->pageCount() != 1){
            int numSlide = m_link.value(currentFrame()).numSlide;
            KoPAPageBase * page = view->kopaDocument()->pageByIndex(numSlide, false);
            view->setActivePage(page);
            view->kopaDocument()->removePage(page);

            populate();
            setCursorTo(numSlide-1, Title, false);
        }
    }
}

void KPrViewModeOutline::setCursorTo(int slide, PlaceholderType type, bool atBegin)
{
    foreach (QTextFrame* frame, m_link.keys()) {
        if (m_link.value(frame).numSlide == slide && m_link.value(frame).type == type) {
            m_editor->setTextCursor(atBegin ? frame->firstCursorPosition() : frame->lastCursorPosition());
            return;
        }
    }
}

void KPrViewModeOutline::synchronize(int position, int charsRemoved, int charsAdded)
{
    QTextCursor cur(m_editor->document());
    cur.setPosition(position);
    QTextFrame* frame = cur.currentFrame();
    int frameBegin = frame->firstPosition();

    kDebug() << "Event on pos " << position << ", frame " << frame << "(" << frameBegin << " => " << frame->lastPosition() << ") "<< (m_link.contains(frame)?"(known)":"(unknown)");
    kDebug() << charsAdded << " chars added; " << charsRemoved << " chars removed";

    QTextDocument* targetDocument = m_link.value(frame).textDocument;
    if(!targetDocument) {  // event on an unknown frame (parasite blank line ?)
        kDebug() << "Incorrect action";
        populate();  // we can't just undo last action because frame stucture may be changed
        return;
    }

    QTextCursor target(targetDocument);
    // synchronize real target
    if (charsRemoved > 0) {
        target.setPosition(position - frameBegin);
        target.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsRemoved);
        target.deleteChar();
    }
    if (charsAdded > 0) {
        cur.setPosition(position);
        kDebug() << cur.currentFrame();
        cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        kDebug() << cur.currentFrame();
        kDebug() << charsAdded << " chars added (" << cur.selectedText() << ")";
        target.setPosition(position - frameBegin);
        target.insertText(cur.selectedText());

        populate();
    }
}

QTextFrame * KPrViewModeOutline::currentFrame()
{
    return m_editor->textCursor().currentFrame();
}

QMap<QTextFrame*, KPrViewModeOutline::FrameData> KPrViewModeOutline::link()
{
    return m_link;
}
