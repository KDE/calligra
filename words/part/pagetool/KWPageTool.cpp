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
    m_selection = NONE;
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

    //For the orientation modification
    m_mousePosTmp = new QPoint(xMouse,yMouse);

    //For the page resizing
    if (xMouse < SELECT_SPACE ) {
        m_selection = BLEFT;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(3);
    }
    else if (xMouse > layout.width - SELECT_SPACE ) {
        m_selection = BRIGHT;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(3);
    }
    else if (yMouse < SELECT_SPACE ) {
        m_selection = BTOP;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(3);
    }
    else if (yMouse > layout.height - SELECT_SPACE && yMouse < layout.height + SELECT_SPACE) {
        m_selection = BBOTTOM;
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        m_resizeTimer->start(3);
    }
    //For the margin resizing
    else if (xMouse > leftMargin - SELECT_SPACE && xMouse < leftMargin + SELECT_SPACE) {
        m_selection = MLEFT;
    }
    else if (xMouse > rightMargin - SELECT_SPACE && xMouse < rightMargin + SELECT_SPACE) {
        m_selection = MRIGHT;
    }
    else if (yMouse > topMargin - SELECT_SPACE && yMouse < topMargin + SELECT_SPACE) {
        m_selection = MTOP;
    }
    else if (yMouse > bottomMargin - SELECT_SPACE && yMouse < bottomMargin + SELECT_SPACE) {
        m_selection = MBOTTOM;
    }
    //For the creation of header
    else if (yMouse < layout.height /2 && !header){
        m_selection = HEADER;
        createHeader();
        header = true;
    }
    else if(yMouse > layout.height / 2 && !footer){
        m_selection = FOOTER;
        createFooter();
        footer = true;
    }
}

void KWPageTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (m_selection == MLEFT
     || m_selection == MRIGHT
     || m_selection == MBOTTOM
     || m_selection == MTOP) {
        setMarginInPx(m_selection, event->x(), event->y());
    }
    else {
        int leftMargin   = marginInPx(MLEFT);
        int rightMargin  = marginInPx(MRIGHT);
        int topMargin    = marginInPx(MTOP);
        int bottomMargin = marginInPx(MBOTTOM);
        KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

        int xMouse = xMouseInPage(event->x());
        int yMouse = yMouseInPage(event->y());

        //Refresh the cursor icon
        if ((xMouse > leftMargin - SELECT_SPACE && xMouse < leftMargin + SELECT_SPACE)
         || (xMouse > rightMargin - SELECT_SPACE && xMouse < rightMargin + SELECT_SPACE)
         || (xMouse < SELECT_SPACE || xMouse > layout.width - SELECT_SPACE)) {
            useCursor(Qt::SplitHCursor);
        }
        else if ((yMouse > topMargin - SELECT_SPACE && yMouse < topMargin + SELECT_SPACE)
             || (yMouse > bottomMargin - SELECT_SPACE && yMouse < bottomMargin + SELECT_SPACE)
             || (yMouse < SELECT_SPACE
             || (yMouse > layout.height - SELECT_SPACE && yMouse < layout.height + SELECT_SPACE))) {
            useCursor(Qt::SplitVCursor);
        }
        else{
            useCursor(Qt::ArrowCursor);
        }
    }
}

void KWPageTool::mouseReleaseEvent(KoPointerEvent *event)
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
    int yMouse = yMouseInPage(event->y());
    if (m_selection != NONE){
        m_selection = NONE;
    }
    else {
        if ((m_mousePosTmp->y() > layout.height / 2 && yMouse < layout.height / 2)
        ||  (m_mousePosTmp->y() < layout.height / 2 && yMouse > layout.height / 2)) {
            qreal widthTmp = layout.width;
            layout.width = layout.height;
            layout.height = widthTmp;
            if (layout.width > layout.height) {
                layout.orientation = KoPageFormat::Landscape;
            }
            else {
                layout.orientation = KoPageFormat::Portrait;
            }
        }
        m_document->pageManager()->defaultPageStyle().setPageLayout(layout);
    }
    m_document->relayout();
    m_canvas->repaint();
    m_resizeTimer->stop();
}

int KWPageTool::marginInPx(Selection selection)
{

    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

    int leftMargin   = layout.leftMargin;
    int rightMargin  = layout.width - layout.rightMargin;
    int topMargin    = layout.topMargin;
    int bottomMargin = layout.height - layout.bottomMargin;

    switch (selection) {
    case MLEFT:
        return leftMargin;
        break;
    case MRIGHT:
        return rightMargin;
        break;
    case MTOP:
        return topMargin;
        break;
    case MBOTTOM:
        return bottomMargin;
        break;
    default:
        qDebug() << "Unexcepted case PageTool::marginPx";
        break;
    }
    return -1;
}

void KWPageTool::setMarginInPx(Selection selection, int positionX, int positionY)
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

    int leftMargin   = marginInPx(MLEFT);
    int rightMargin  = marginInPx(MRIGHT);
    int topMargin    = marginInPx(MTOP);
    int bottomMargin = marginInPx(MBOTTOM);

    int xMouse = xMouseInPage(positionX);
    int yMouse = yMouseInPage(positionY);

    switch (selection) {
    case MLEFT:
        if (xMouse > SELECT_SPACE && xMouse < rightMargin - SELECT_SPACE) {
            layout.leftMargin = xMouse;
        }
        break;
    case MRIGHT:
        if (xMouse < layout.width - SELECT_SPACE && xMouse > leftMargin + SELECT_SPACE) {
            layout.rightMargin = layout.width - xMouse;
        }
        break;
    case MTOP:
        if (yMouse > SELECT_SPACE && yMouse < bottomMargin - SELECT_SPACE) {
            layout.topMargin = yMouse;
        }
        break;
    case MBOTTOM:
        if (yMouse < layout.height - SELECT_SPACE && yMouse > topMargin + SELECT_SPACE) {
            layout.bottomMargin = layout.height - yMouse;
        }
        break;
    default:
        qDebug() << "Unexcepted case PageTool::setMarginPx";
        break;
    }
    m_document->pageManager()->defaultPageStyle().setPageLayout(layout);
    m_document->relayout();
    m_canvas->repaint();
}


void KWPageTool::resizePage()
{
        KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();

        //Get the size of the resizing
        int widthResize =
                m_canvas->viewConverter()->viewToDocumentX(m_mousePosTmp->x() - QCursor::pos().x());
        int heightResize =
                m_canvas->viewConverter()->viewToDocumentY(m_mousePosTmp->y() - QCursor::pos().y());
        m_mousePosTmp = new QPoint(QCursor::pos().x(),QCursor::pos().y());
        //In order to refresh the scroll position in order to follow the page that we resize
        float heightOld = layout.height;

        //Apply the resize
        switch(m_selection) {
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
            qDebug() << "Unexcepted case PageTool::ResizePage";
        }
        layout.width =
               std::max(layout.width
               , qreal(marginInPx(MLEFT) + (layout.width - marginInPx(MRIGHT)) + SELECT_SPACE));
        layout.height =
               std::max(layout.height
               , qreal(marginInPx(MTOP) + (layout.height - marginInPx(MBOTTOM)) + SELECT_SPACE));
        //We follow the page
        float heightNew = layout.height;
        float ratio = heightNew / heightOld;
        m_canvas->canvasController()->setPreferredCenter(
                    QPointF(m_canvas->canvasController()->preferredCenter().x()
                           ,m_canvas->canvasController()->preferredCenter().y()*ratio));
        //Changethe orientation of the page if needed + change size format to custom
        layout.format = KoPageFormat::CustomSize;
        if (layout.width > layout.height) {
            layout.orientation = KoPageFormat::Landscape;
        }
        else {
            layout.orientation = KoPageFormat::Portrait;
        }

        m_document->pageManager()->defaultPageStyle().setPageLayout(layout);
        m_document->relayout();
        m_canvas->repaint();
}

int KWPageTool::xMouseInPage(int positionX)
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
    int xMouseInPage = int(m_canvas->viewConverter()->viewToDocumentX(positionX
              + m_canvas->documentOffset().x()));
    if(xMouseInPage > 0) {
        xMouseInPage %= int(layout.width);
    }
    return xMouseInPage;
}

int KWPageTool::yMouseInPage(int positionY)
{
    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
    int yMouseInPage = int(m_canvas->viewConverter()->viewToDocumentY(positionY
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


