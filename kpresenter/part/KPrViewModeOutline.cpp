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
                    currentCursor.block().setUserData(new SlideUserBlockData(pageNumber, pair));
                    currentCursor.insertText(pair.second->document()->toPlainText());
                }
            }
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                if (pair.first == Subtitle) {
                    currentCursor.insertBlock(blockFormat);
                    currentCursor.block().setUserData(new SlideUserBlockData(pageNumber, pair));
                    currentCursor.insertText(pair.second->document()->toPlainText());
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
}

void KPrViewModeOutline::synchronize(int position, int charsRemoved, int charsAdded)
{
    Q_UNUSED(position);
    Q_UNUSED(charsRemoved);
    Q_UNUSED(charsAdded);
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
