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
#include "KWPageTool.h"
#include "SimpleHeaderFooterWidget.h"
#include "SimpleSetupWidget.h"
#include "frames/KWTextFrameSet.h"

//words includes
#include "KWDocument.h"
#include "KWCanvas.h"

//Qt includes
#include <QList>

//calligra includes
#include <KoTextEditor.h>
#include <KoPointerEvent.h>
#include <KoTextDocument.h>
#include <KoCanvasBase.h>

KWPageTool::KWPageTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
{
    selection = 0;
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

        KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
        qreal mmToPxWidth = m_canvas->width() / layout.width;
        qreal mmToPxHeight = m_canvas->height() / layout.height;

        qDebug() << "mmToPxWidth : " << mmToPxWidth;
        qDebug() << "Souris X    : " << event->x();
        qDebug() << "Width in MM : " << layout.width;
        qDebug() << "Width in PX : " << m_canvas->width();
        qDebug() << "Conversion  : " << m_canvas->width()/mmToPxWidth;
        qDebug() << "Conversion X: " << event->x()/mmToPxWidth;
        qDebug() << "Margin Left : " << layout.leftMargin;

        if(event->x() > (layout.leftMargin*mmToPxWidth) -10
        && event->x() < (layout.leftMargin*mmToPxWidth) +10 )
            selection = 1;
        else
        if(event->x() > ((layout.width - layout.rightMargin)*mmToPxWidth) -10
        && event->x() < ((layout.width - layout.rightMargin)*mmToPxWidth) +10 )
            selection = 3;
        else
        if(event->y() > (layout.topMargin*mmToPxHeight) -10
        && event->y() < (layout.topMargin*mmToPxHeight) +10 )
            selection = 2;
        else
        if(event->y() > ((layout.height - layout.bottomMargin)*mmToPxHeight) -10
        && event->y() < ((layout.height - layout.bottomMargin)*mmToPxHeight) +10 )
            selection = 4;

        qDebug() << selection;
}

void KWPageTool::mouseMoveEvent(KoPointerEvent *event)
{
    if(selection != 0){
        KoPageLayout layout = m_document->pageManager()->defaultPageStyle().pageLayout();
        qreal mmToPxWidth = m_canvas->width() / layout.width;
        qreal mmToPxHeight = m_canvas->height() / layout.height;

        switch(selection){
            case 1:
            if(event->x() < ((layout.width - layout.rightMargin)*mmToPxWidth - 10)
            && event->x() > 1)
                    layout.leftMargin = event->x()/mmToPxWidth;
                break;
            case 2:
            if(event->y() < ((layout.height - layout.bottomMargin)*mmToPxHeight - 10)
            && event->y() > 1)
                    layout.topMargin = event->y()/mmToPxHeight;
                break;
            case 3:
            if(event->x() > (layout.leftMargin)*mmToPxWidth + 10
            && event->x() < m_canvas->width() - 1)
                    layout.rightMargin = layout.width - event->x()/mmToPxWidth;
                break;
            case 4:
            if(event->y() > (layout.topMargin)*mmToPxHeight + 10
            && event->y() < m_canvas->height() - 1)
                    layout.bottomMargin = layout.height - event->y()/mmToPxHeight;
                break;
        }

        m_document->pageManager()->defaultPageStyle().setPageLayout(layout);
        m_document->relayout();
    }
}

void KWPageTool::mouseReleaseEvent(KoPointerEvent *event)
{
    qDebug() << "RELACHE";
    selection = 0;
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


