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
    qDebug() << event->x() << " " << m_document->pageManager()->defaultPageStyle().pageLayout().bottomMargin;
    if(selection == 0){
        if(event->x() > m_document->pageManager()->defaultPageStyle().pageLayout().leftMargin-10
        && event->x() < m_document->pageManager()->defaultPageStyle().pageLayout().leftMargin+10 )
            selection = 1;
        else if(event->x() > m_document->pageManager()->defaultPageStyle().pageLayout().rightMargin-10
             && event->x() < m_document->pageManager()->defaultPageStyle().pageLayout().rightMargin+10 )
                selection = 3;
        else if(event->y() > m_document->pageManager()->defaultPageStyle().pageLayout().topMargin-10
             && event->y() < m_document->pageManager()->defaultPageStyle().pageLayout().topMargin+10 )
                selection = 2;
        else if(event->y() > m_document->pageManager()->defaultPageStyle().pageLayout().bottomMargin-10
             && event->y() < m_document->pageManager()->defaultPageStyle().pageLayout().bottomMargin+10 )
                selection = 4;
    }
    else{
            selection = 0;
    }
}

void KWPageTool::mouseMoveEvent(KoPointerEvent *event)
{
    qDebug() << selection;
    KoPageLayout l = m_document->pageManager()->defaultPageStyle().pageLayout();
    switch(selection){
        case 1:
            if(event->x() < l.rightMargin - 10)
                l.leftMargin = event->x();
            break;
        case 2:
            if(event->y() < l.bottomMargin - 10)
                l.topMargin = event->y();
            break;
        case 3:
            if(event->x() > l.leftMargin + 10)
                l.rightMargin = event->x();
            break;
        case 4:
            if(event->y() > l.topMargin + 10)
                l.bottomMargin = event->y();
            break;
    }
    m_document->pageManager()->defaultPageStyle().setPageLayout(l);
    m_document->relayout();
}

void KWPageTool::mouseReleaseEvent(KoPointerEvent *event)
{
    qDebug() << "RELACHE";
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


