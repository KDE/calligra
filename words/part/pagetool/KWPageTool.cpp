/*
 *  Copyright (C) 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

//pagetool includes
#include "SimpleHeaderFooterWidget.h"
#include "SimpleSetupWidget.h"
#include "KWPageTool.h"
#include "frames/KWTextFrameSet.h"

//words includes
#include "KWDocument.h"
#include "KWCanvas.h"
#include "KWView.h"

//Qt includes
#include <QList>
#include <QPainter>
#include <QTimer>

//calligra includes
#include <KoTextEditor.h>
#include <KoTextEditor.h>
#include <KoPointerEvent.h>
#include <KoViewConverter.h>
#include <KoTextDocument.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>

KWPageTool::KWPageTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
{
    selection = NONE;
    m_canvas = dynamic_cast<KWCanvas*>(canvas);
    if (m_canvas) {
        m_document = m_canvas->document();
    }
    header = footer = false;
    m_resizeTimer = new QTimer(this);
    connect( m_resizeTimer, SIGNAL(timeout()), this, SLOT(resizePage()));
    m_resizeTimer->stop();
}

bool KWPageTool::wantsAutoScroll() const
{
    return false;
}

void KWPageTool::resizePage()
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

    int widthResize = m_canvas->viewConverter()->viewToDocumentX(m_mousePosTmp->x() - QCursor::pos().x());
    int heightResize = m_canvas->viewConverter()->viewToDocumentY(m_mousePosTmp->y() - QCursor::pos().y());

    float heightTmp = (m_canvas->viewConverter()->viewToDocumentY(layout.height) + 21) * m_document->pageCount();

    switch(selection) {
    case BLEFT:
        layout.width += widthResize*2;
        break;
    case BRIGHT:
        layout.width -= widthResize*2;
        break;
    case BTOP:
        layout.height += heightResize*2;
        break;
    case BBOTTOM:
        layout.height -= heightResize*2;
        break;
    default:
        qDebug() << "Error!";
    }
    qDebug() << m_canvas->canvasController()->scrollBarValue().y()*(layout.height*m_document->pageCount())/heightTmp;
    qDebug() << (layout.height*m_document->pageCount())/heightTmp;

    float heightNew = (m_canvas->viewConverter()->viewToDocumentY(layout.height) + 21) * m_document->pageCount();
    QPoint scrollNewPos(0,m_canvas->canvasController()->scrollBarValue().y()*(heightNew/heightTmp));
    m_canvas->canvasController()->setScrollBarValue(scrollNewPos);
    m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
    layout.width = std::max(layout.width,qreal(200));
    layout.height = std::max(layout.height,qreal(200));
    m_document->pageManager()->defaultPageStyle().setPageLayout(layout);
    m_document->relayout();
    m_canvas->repaint();
}

KWPageTool::~KWPageTool()
{
}

void KWPageTool::paint( QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void KWPageTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    useCursor(Qt::ArrowCursor);
    repaintDecorations();
    Q_UNUSED(toolActivation);
    Q_UNUSED(shapes);
}

void KWPageTool::mousePressEvent(KoPointerEvent *event)
{    
    int leftMargin   = marginInPx(MLEFT);
    int rightMargin  = marginInPx(MRIGHT);
    int topMargin    = marginInPx(MTOP);
    int bottomMargin = marginInPx(MBOTTOM);
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

    int xMouse = xMouseInPage(event->x());
    int yMouse = yMouseInPage(event->y());

    if (xMouse > leftMargin - 10 && xMouse < leftMargin + 10) {
        selection = MLEFT;
    }
    else if (xMouse > rightMargin - 10 && xMouse < rightMargin + 10) {
        selection = MRIGHT;
    }
    else if (yMouse > topMargin - 10 && yMouse < topMargin + 10) {
        selection = MTOP;
    }
    else if (yMouse > bottomMargin - 10 && yMouse < bottomMargin + 10) {
        selection = MBOTTOM;
    }
    else if (xMouse < 10 ) {
        selection = BLEFT;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(10);
    }
    else if (xMouse > layout.width - 10 ) {
        selection = BRIGHT;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(10);
    }
    else if (yMouse < 10 ) {
        selection = BTOP;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(10);
    }
    else if (yMouse > layout.height - 10 && yMouse < layout.height) {
        selection = BBOTTOM;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(10);
    }
    else if (yMouse < layout.height /2 && !header){
        selection = HEADER;
        createHeader();
        header = true;
    }
    else if(yMouse > layout.height / 2 && !footer){
        selection = FOOTER;
        createFooter();
        footer = true;
    }
}

void KWPageTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (selection != NONE) {
        setMarginInPx(selection, event->x(), event->y());
    }
    else {
        int leftMargin   = marginInPx(MLEFT);
        int rightMargin  = marginInPx(MRIGHT);
        int topMargin    = marginInPx(MTOP);
        int bottomMargin = marginInPx(MBOTTOM);
        KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

        int xMouse = xMouseInPage(event->x());
        int yMouse = yMouseInPage(event->y());

        if ((xMouse > leftMargin - 10 && xMouse < leftMargin + 10)
         || (xMouse > rightMargin - 10 && xMouse < rightMargin + 10)
         || (xMouse < 10 || xMouse > layout.width - 10)) {
            useCursor(Qt::SplitHCursor);
        }
        else if ((yMouse > topMargin - 10 && yMouse < topMargin + 10)
              || (yMouse > bottomMargin - 10 && yMouse < bottomMargin + 10)
              || (yMouse < 10 || (yMouse > layout.height - 10 && yMouse < layout.height))) {
            useCursor(Qt::SplitVCursor);
        }
        else{
            useCursor(Qt::ArrowCursor);
        }
    }
}

void KWPageTool::mouseReleaseEvent(KoPointerEvent *event)
{
    if (selection != 0){
        selection = NONE;
    }
    m_document->relayout();
    m_canvas->repaint();
    m_resizeTimer->stop();
}

int KWPageTool::marginInPx(Selection p_selection)
{

    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

    int leftMargin   = layout.leftMargin;
    int rightMargin  = layout.width - layout.rightMargin;
    int topMargin    = layout.topMargin;
    int bottomMargin = layout.height - layout.bottomMargin;

    switch (p_selection) {
    case MLEFT:
        return leftMargin;
    case MRIGHT:
        return rightMargin;
    case MTOP:
        return topMargin;
    case MBOTTOM:
        return bottomMargin;
    }
    return -1;
}

void KWPageTool::setMarginInPx(Selection p_selection, int p_positionX, int p_positionY)
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

    int leftMargin   = marginInPx(MLEFT);
    int rightMargin  = marginInPx(MRIGHT);
    int topMargin    = marginInPx(MTOP);
    int bottomMargin = marginInPx(MBOTTOM);

    int xMouse = xMouseInPage(p_positionX);
    int yMouse = yMouseInPage(p_positionY);

    switch (p_selection) {
    case MLEFT:
        if (xMouse > 1 && xMouse < rightMargin - 10) {
            layout.leftMargin = xMouse;
        }
        break;
    case MRIGHT:
        if (xMouse < layout.width - 1 && xMouse > leftMargin + 10) {
            layout.rightMargin = layout.width - xMouse;
        }
        break;
    case MTOP:
        if (yMouse > 1 && yMouse < bottomMargin - 10) {
            layout.topMargin = yMouse;
        }
        break;
    case MBOTTOM:
        if (yMouse < layout.height - 1 && yMouse > topMargin + 10) {
            layout.bottomMargin = layout.height - yMouse;
        }
        break;
    }

    m_document->pageManager()->defaultPageStyle().setPageLayout(layout);
    m_document->relayout();
    m_canvas->repaint();
}

int KWPageTool::xMouseInPage(int p_positionX)
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
    int xMouseInPage = int(m_canvas->viewConverter()->viewToDocumentX(p_positionX
              + m_canvas->documentOffset().x()));
    if(xMouseInPage > 0) {
        xMouseInPage %= int(layout.width);
    }
    return xMouseInPage;
}

int KWPageTool::yMouseInPage(int p_positionY)
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
    int yMouseInPage = int(m_canvas->viewConverter()->viewToDocumentY(p_positionY
              + m_canvas->documentOffset().y()));
    if (yMouseInPage > 0) {
         yMouseInPage %= (int(layout.height) + 21);
    }
    return yMouseInPage;
}

void KWPageTool::insertPageBreak()
{
    if (m_document->mainFrameSet()) { 
        // lets just add one to the main text frameset
        KoTextEditor *editor =  KoTextDocument(m_document->mainFrameSet()->document()).textEditor();
        if (editor == KoTextEditor::getTextEditorFromCanvas(m_canvas)) {
            editor->insertFrameBreak();
        }
    }
}


void KWPageTool::createHeader(){
    m_canvas->view()->enableHeader();
    m_document->relayout();
    m_canvas->repaint();
}

void KWPageTool::createFooter(){
    m_canvas->view()->enableFooter();
    m_document->relayout();
    m_canvas->repaint();
}

QList<QWidget *> KWPageTool::createOptionWidgets()
{
    QList<QWidget*> widgets;

    SimpleSetupWidget *ssw = new SimpleSetupWidget(m_canvas->view());
    ssw->setWindowTitle(i18n("Page Setup"));
    widgets.append(ssw);

    SimpleHeaderFooterWidget *shfw = new SimpleHeaderFooterWidget(m_canvas->view());
    shfw->setWindowTitle(i18n("Header & Footer"));
    widgets.append(shfw);

    return widgets;
}

#include "KWPageTool.moc"


