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
#include <QtGui/qtextformat.h>
#include <../libs/kotext/KoTextShapeData.h>

KPrViewModeOutline::KPrViewModeOutline(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode( view, canvas )
    , m_outlineEditor(new KPrOutlineEditor(this, view->parentWidget()))
{
    m_outlineEditor->hide();
    connect(m_outlineEditor, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
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
    
    // Enable synchronization
    connect(m_outlineEditor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(synchronize(int, int, int)));
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    m_outlineEditor->show();
    m_outlineEditor->setFocus(Qt::ActiveWindowFocusReason);
}


void KPrViewModeOutline::deactivate()
{
    disconnect(m_outlineEditor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(synchronize(int, int, int)));
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
    disconnect(m_outlineEditor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(synchronize(int, int, int)));
    
    m_outlineEditor->clear();
    QTextCursor currentCursor = m_outlineEditor->textCursor();
    // For each slides
    foreach (KoPAPageBase * pageBase, m_view->kopaDocument()->pages()) {
        if (KPrPage * page = dynamic_cast<KPrPage *>(pageBase)) {
            int pageNumber = m_view->kopaDocument()->pages().indexOf(pageBase);
            QTextBlockFormat blockFormat;
            blockFormat.setBackground((pageNumber%2)?QBrush(Qt::gray):QBrush(Qt::white));
            // Copy relevant content of the "view" block of the page in the "outline" block
            // Do three iterations in order to kept the correct order i.e. Title, Subtitle, Outline
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                if (pair.first == Title) {
                    currentCursor.insertBlock(blockFormat);
                    int start = currentCursor.blockNumber();
                    currentCursor.insertText(pair.second->document()->toPlainText());
                    for(start; start <= currentCursor.blockNumber(); start++){
                        m_outlineEditor->document()->findBlockByNumber(start).setUserData(new SlideUserBlockData(pageNumber, pair));
                    }
                }
            }
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                if (pair.first == Subtitle) {
                    currentCursor.insertBlock(blockFormat);
                    int start = currentCursor.blockNumber();
                    currentCursor.insertText(pair.second->document()->toPlainText());
                    for(start; start <= currentCursor.blockNumber(); start++){
                        m_outlineEditor->document()->findBlockByNumber(start).setUserData(new SlideUserBlockData(pageNumber, pair));
                    }
                }
            }
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                if (pair.first == Outline) {
                    currentCursor.insertBlock(blockFormat);
                    int start = currentCursor.blockNumber();
                    currentCursor.insertText(pair.second->document()->toPlainText());
                    for(start; start <= currentCursor.blockNumber(); start++){
                        m_outlineEditor->document()->findBlockByNumber(start).setUserData(new SlideUserBlockData(pageNumber, pair));
                    }
                }
            }
        }
    }
    // Delete the first empty line
    currentCursor.setPosition(0);
    currentCursor.deleteChar();

    m_outlineEditor->setTextCursor(currentCursor);

    connect(m_outlineEditor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(synchronize(int, int, int)));
}

void KPrViewModeOutline::synchronize(int position, int charsRemoved, int charsAdded)
{

    QTextCursor cursor = m_outlineEditor->textCursor();
    cursor.setPosition(position);
    int blockBegin = cursor.block().position();

    // Trying to find in which block we are (after the last user data)
    SlideUserBlockData *userData;
    while (!(userData = dynamic_cast<SlideUserBlockData*>( cursor.block().userData() )))
    {
        if (cursor.blockNumber() > 0) {
            cursor.movePosition(QTextCursor::PreviousBlock);
        }
    }
    
    if (userData = dynamic_cast<SlideUserBlockData*>( cursor.block().userData() )) {
        KoTextShapeData *viewData =  userData->outlinePair().second;
        QTextCursor viewCursor = QTextCursor(viewData->document());
        
        // synchronize real target
        if (charsRemoved > 0) {
            viewCursor.setPosition(position - blockBegin);
            viewCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsRemoved);
            viewCursor.deleteChar();
        }
        if (charsAdded > 0) {
            cursor.setPosition(position);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);

            viewCursor.setPosition(position - blockBegin);
            viewCursor.insertText(cursor.selectedText());
        }
    } else {
        kDebug(33001) << "No user data anymore in the outline::synchronize";
    }
}

void KPrViewModeOutline::slotSelectionChanged()
{
    QTextCursor cursor = m_outlineEditor->textCursor();
    SlideUserBlockData *userData = dynamic_cast<SlideUserBlockData*>( cursor.block().userData() );
    if(userData){
        KoPAPageBase * page = m_view->kopaDocument()->pageByIndex(userData->pageNumber(), false);
        if ( m_view->activePage() != page ) {
            m_view->setActivePage(page);
        }
    }
}

void KPrViewModeOutline::updateActivePage( KoPAPageBase * page )
{
    if ( m_view->activePage() != page ) {
        m_view->setActivePage( page );
    }
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    setCursorTo(view->kopaDocument()->pageIndex(page));
    m_outlineEditor->setFocus(Qt::ActiveWindowFocusReason);
}

void KPrViewModeOutline::setCursorTo(int slide)
{
    int position = 0;
    for(int i=0; i < m_outlineEditor->document()->blockCount(); i++){
        QTextBlock block = m_outlineEditor->document()->findBlockByNumber(i);
        SlideUserBlockData *userData = dynamic_cast<SlideUserBlockData*>( block.userData() );
        if (userData && userData->pageNumber() == slide) {
            position = block.position();
            break;
        }
    }
    QTextCursor cursor = m_outlineEditor->textCursor();
    cursor.setPosition(position);
    m_outlineEditor->setTextCursor(cursor);
}
