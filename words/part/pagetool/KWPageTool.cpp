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
    margin = NONE;
    m_canvas = dynamic_cast<KWCanvas*>(canvas);
    if (m_canvas) {
        m_document = m_canvas->document();
    }
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
        int leftMargin = marginInPx(LEFT);
        int rightMargin = marginInPx(RIGHT);
        int topMargin = marginInPx(TOP);
        int bottomMargin = marginInPx(BOTTOM);
        int header = marginInPx(HEADER);
        int bottom = marginInPx(FOOTER);

        KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
        qreal mmToPxHeight = m_canvas->height() / layout.height;
        qDebug() << "SOUROS Y : " << event->y()/mmToPxHeight;
        qDebug() << "MARGE TOP: " << layout.topMargin;
        qDebug() << "HEADER D : " << m_document->pageManager()->defaultPageStyle().headerDistance();
        qDebug() << "TOP + HEA: " << layout.topMargin + m_document->pageManager()->defaultPageStyle().headerDistance();

        if(event->x() > leftMargin - 10
        && event->x() < leftMargin + 10 )
            margin = LEFT;
        else
        if(event->x() > rightMargin - 10
        && event->x() < rightMargin + 10 )
            margin = RIGHT;
        else
        if(event->y() > topMargin - 10
        && event->y() < topMargin + 10 )
            margin = TOP;
        else
        if(event->y() > bottomMargin - 10
        && event->y() < bottomMargin + 10 )
            margin = BOTTOM;
        else
        if(event->y() > header - 10
        && event->y() < header + 10 )
            margin = HEADER;
}

void KWPageTool::mouseMoveEvent(KoPointerEvent *event)
{

    if(margin != NONE){
        setMarginInPx(margin,event->x(),event->y());
    }
}

void KWPageTool::mouseReleaseEvent(KoPointerEvent *event)
{
    if(margin != 0){
        margin = NONE;
    }
}

int KWPageTool::marginInPx(Margin p_selection){

    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
    qreal mmToPxWidth = m_canvas->width() / layout.width;
    qreal mmToPxHeight = m_canvas->height() / layout.height;

    switch(p_selection){
        case LEFT:
            return layout.leftMargin*mmToPxWidth;
        case RIGHT:
            return (layout.width - layout.rightMargin)*mmToPxWidth;
        case TOP:
            return layout.topMargin*mmToPxHeight;
        case BOTTOM:
            return (layout.height - layout.bottomMargin)*mmToPxHeight;
        case HEADER:
            return layout.topMargin + m_document->pageManager()->defaultPageStyle().headerDistance();
    }
    return -1;
}

void KWPageTool::setMarginInPx(Margin p_selection, int p_positionX, int p_positionY){

    KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
    qreal mmToPxWidth = m_canvas->width() / layout.width;
    qreal mmToPxHeight = m_canvas->height() / layout.height;
    qreal xMouseInMM = p_positionX/mmToPxWidth;
    qreal yMouseInMM = p_positionY/mmToPxHeight;

    switch(p_selection){
        case LEFT:
            if(xMouseInMM > 1 && xMouseInMM < marginInPx(RIGHT)/mmToPxWidth - 10)
                layout.leftMargin = xMouseInMM;
            break;
        case RIGHT:
            if(xMouseInMM < layout.width - 1 && xMouseInMM > layout.leftMargin + 10)
                layout.rightMargin = layout.width - xMouseInMM;
            break;
        case TOP:
            if(yMouseInMM > 1 && yMouseInMM < marginInPx(BOTTOM)/mmToPxHeight - 10)
                layout.topMargin = yMouseInMM;
            break;
        case BOTTOM:
            if(yMouseInMM < layout.height - 1 && yMouseInMM > layout.topMargin + 10)
                layout.bottomMargin = layout.height - yMouseInMM;
            break;
        case HEADER:
            break;
        case FOOTER:
            break;
    }
    m_document->pageManager()->defaultPageStyle().setPageLayout(layout);
    m_document->relayout();
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


