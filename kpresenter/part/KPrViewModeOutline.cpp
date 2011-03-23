/* This file is part of the KDE project
   Copyright (C) 2011 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrViewModeOutline.h"

#include "KoPAView.h"
#include "KoPACanvas.h"
#include "KoPAPageBase.h"
#include "KoPADocument.h"
#include "KPrPage.h"
#include "KoTextShapeData.h"

#include "KPrOutlineEditor.h"

#include <KDebug>

KPrViewModeOutline::KPrViewModeOutline(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode( view, canvas )
    , m_outlineEditor(new KPrOutlineEditor(this, view->parentWidget()))
{
    m_outlineEditor->hide();
}

void KPrViewModeOutline::paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect)
{
    Q_UNUSED(canvas);
    Q_UNUSED(painter);
    Q_UNUSED(paintRect);
}

void KPrViewModeOutline::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::activate(KoPAViewMode *previousViewMode)
{
    Q_UNUSED(previousViewMode);
    
    populate();
    
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    m_outlineEditor->show();
    m_outlineEditor->setFocus(Qt::ActiveWindowFocusReason);
}


void KPrViewModeOutline::deactivate()
{
    m_outlineEditor->hide();
     // Active the view as a basic but active one
    m_view->setActionEnabled(KoPAView::AllActions, true);
    m_view->doUpdateActivePage(m_view->activePage());
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->show();
    }
}

void KPrViewModeOutline::populate()
{
    int recordPosition = m_outlineEditor->textCursor().position();
    m_outlineEditor->clear();
    QTextCursor currentCursor = m_outlineEditor->textCursor();

    // For each slides
    foreach (KoPAPageBase * pageBase, m_view->kopaDocument()->pages()) {
        if (KPrPage * page = dynamic_cast<KPrPage *>(pageBase)) {
            
            // Copy relevant content of the title of the page in the frame
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                currentCursor.insertBlock();
                currentCursor.insertText(pair.second->document()->toPlainText());
                kDebug() << pair.second->document()->toPlainText();
            }
        }
    }
    currentCursor.setPosition(((recordPosition > 0) ? recordPosition : 0));
    m_outlineEditor->setTextCursor(currentCursor);
}
/*
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
*/
