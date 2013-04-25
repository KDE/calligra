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
#include "SimplePageStyleWidget.h"
#include "KWPageTool.h"
#include "frames/KWTextFrameSet.h"

//words includes
#include "KWDocument.h"
#include "KWCanvas.h"
#include "KWView.h"
#include "KWPage.h"

// words includes
#include "KWGui.h"
#include "KWFactory.h"
#include "KWStatusBar.h"
#include "KWPageManager.h"
#include "frames/KWFrame.h"
#include "frames/KWCopyShape.h"
#include "frames/KWTextFrameSet.h"
#include "dialogs/KWFrameDialog.h"
#include "dialogs/KWPageSettingsDialog.h"
#include "dialogs/KWPrintingDialog.h"
#include "dialogs/KWCreateBookmarkDialog.h"
#include "dialogs/KWSelectBookmarkDialog.h"
#include "dialogs/KWConfigureDialog.h"
#include "commands/KWFrameCreateCommand.h"
#include "commands/KWShapeCreateCommand.h"
#include "ui_KWInsertImage.h"

//Qt includes
#include <QList>
#include <QPainter>
#include <QTimer>

//calligra includes
#include <KoTextEditor.h>
#include <KoPointerEvent.h>
#include <KoViewConverter.h>
#include <KoTextDocument.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>

#include <KoShapeCreateCommand.h>
#include <calligraversion.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoProperties.h>
#include <KoCopyController.h>
#include <KoTextShapeData.h>
#include <KoCanvasResourceManager.h>
#include <KoCutController.h>
#include <KoStandardAction.h>
#include <KoTemplateCreateDia.h>
#include <KoPasteController.h>
#include <KoShape.h>
#include <KoText.h>
#include <KoFind.h>
#include <KoShapeContainer.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoToolManager.h>
#include <KoTextRangeManager.h>
#include <KoAnnotationManager.h>
#include <KoToolProxy.h>
#include <KoShapeAnchor.h>
#include <KoShapeGroupCommand.h>
#include <KoZoomController.h>
#include <KoInlineTextObjectManager.h>
#include <KoBookmark.h>
#include <KoPathShape.h> // for KoPathShapeId
#include <KoDocumentRdfBase.h>
#include <KoDocumentInfo.h>
#ifdef SHOULD_BUILD_RDF
#include <KoDocumentRdf.h>
#include <KoSemanticStylesheetsEditor.h>
#endif
// KDE + Qt includes
#include <QTimer>
#include <klocale.h>
#include <kdebug.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kxmlguifactory.h>
#include <kstatusbar.h>
#include <QMenu>

#include <KoIcon.h>
#include <limits>

KWPageTool::KWPageTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
{
    m_selection = NONE;
    m_canvas = dynamic_cast<KWCanvas*>(canvas);

    if (m_canvas) {
        m_document = m_canvas->document();
    }
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

    KWPageStyle style = pageUnderMouse().pageStyle();
    KoPageLayout layout = style.pageLayout();

    int xMouse = xMouseInPage();
    int yMouse = yMouseInPage();

    m_numberPageClicked = pageUnderMouse().pageNumber();

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
    else if (yMouse < layout.height /2
         && pageUnderMouse().pageStyle().headerPolicy() == Words::HFTypeNone){
        enableHeader();
    }
    else if(yMouse > layout.height / 2
         && pageUnderMouse().pageStyle().footerPolicy() == Words::HFTypeNone){
        enableFooter();
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
        KWPageStyle style = pageUnderMouse().pageStyle();
        KoPageLayout layout = style.pageLayout();

        int xMouse = xMouseInPage();
        int yMouse = yMouseInPage();

        //Refresh the cursor icon
        if (m_styleFomWiget != "") {
            useCursor(Qt::UpArrowCursor);
        }
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

void KWPageTool::applyStyle(int page, QString style) {
    if(page != -1) {
        applyStyle(page, m_document->pageManager()->pageStyle(style));
    }
}

void KWPageTool::applyStyle(int page, KWPageStyle style) {
    if(page != -1) {
        m_document->pageManager()->page(page).setPageStyle(style);
        refreshCanvas();
    }
}

void KWPageTool::mouseReleaseEvent(KoPointerEvent *event)
{
    KWPageStyle style = pageUnderMouse().pageStyle();;
    KoPageLayout layout = style.pageLayout();
    int yMouse = yMouseInPage();
    if (m_styleFomWiget != "") {
        applyStyle(pageUnderMouse().pageNumber(),m_document->pageManager()->pageStyle(m_styleFomWiget));
    }
    m_styleFomWiget = "";
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
        changeLayoutInStyle(layout, style);
    }
    refreshCanvas();
    m_resizeTimer->stop();
}

int KWPageTool::marginInPx(Selection selection)
{
    KWPageStyle style = pageUnderMouse().pageStyle();
    KoPageLayout layout = style.pageLayout();

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
    KWPageStyle style = pageUnderMouse().pageStyle();
    KoPageLayout layout = style.pageLayout();

    int leftMargin   = marginInPx(MLEFT);
    int rightMargin  = marginInPx(MRIGHT);
    int topMargin    = marginInPx(MTOP);
    int bottomMargin = marginInPx(MBOTTOM);

    int xMouse = xMouseInPage();
    int yMouse = yMouseInPage();

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

    changeLayoutInStyle(layout, style);
}


void KWPageTool::resizePage()
{
    KWPageStyle style = m_document->pageManager()->page(m_numberPageClicked).pageStyle();
    KoPageLayout layout = style.pageLayout();
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

    int numberPageModified = 0;
    for(int i = 1; i <= m_numberPageClicked; i++) {
        if(m_document->pageManager()->page(i).pageStyle().name() == style.name()) {
            numberPageModified++;
        }
    }
    float ratio = ((heightNew / heightOld) * numberPageModified
                + (m_numberPageClicked - numberPageModified)) / m_numberPageClicked;
    qDebug() << "RATIO " << ratio;
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

    changeLayoutInStyle(layout, style);
}

int KWPageTool::xMouseInPage()
{
    int xMouseInPage = m_canvas->mapFromGlobal(QCursor::pos()).x() + m_canvas->documentOffset().x();
    return int(m_canvas->viewConverter()->viewToDocumentX(qreal(xMouseInPage)));
}

int KWPageTool::yMouseInPage()
{
    int yMouseInPage = int(yMouseInDocument()) - distanceOverPage(pageUnderMouse().pageNumber());
    return yMouseInPage;
}

qreal KWPageTool::yMouseInDocument()
{
    qreal posYInCanvas = m_canvas->mapFromGlobal(QCursor::pos()).y();
    return m_canvas->viewConverter()->viewToDocumentY(posYInCanvas + m_canvas->documentOffset().y());
}


KWPage KWPageTool::pageUnderMouse()
{
    int yPosition = 0;
    int yMouse = int(yMouseInDocument());
    for(int i = 1; i <= m_document->pageCount(); i++) {
        if(yPosition > yMouse) {
            return m_document->pageManager()->page(i-1);
        }
        yPosition += m_document->pageManager()->page(i).pageStyle().pageLayout().height + 21;
    }
    return m_document->pageManager()->page(m_document->pageCount());
    //return m_document->pageManager()->page(yMouseInDocument());
    //Sometimes page(qreal y) return a page after the one that's really under the mouse
}

int KWPageTool::distanceOverPage(int pageNumber) {
    int distance = 0;
    for(int i = 1; i < pageNumber; i++) {
        distance += m_document->pageManager()->page(i).pageStyle().pageLayout().height + 21;
    }
    return distance;
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

void KWPageTool::enableHeader()
{
    KWPage m_currentPage = pageUnderMouse();
    if (!m_currentPage.isValid())
        return;
    Q_ASSERT(m_currentPage.pageStyle().isValid());
    m_currentPage.pageStyle().setHeaderPolicy(Words::HFTypeUniform);
    m_canvas->view()->actionCollection()->action("insert_header")->setEnabled(false);
    refreshCanvas();
}


void KWPageTool::enableFooter()
{
    KWPage m_currentPage = pageUnderMouse();
    if (!m_currentPage.isValid())
        return;
    Q_ASSERT(m_currentPage.pageStyle().isValid());
    m_currentPage.pageStyle().setFooterPolicy(Words::HFTypeUniform);
    m_canvas->view()->actionCollection()->action("insert_footer")->setEnabled(false);
    refreshCanvas();
}

void KWPageTool::changeLayoutInStyle(KoPageLayout layout, KWPageStyle style)
{
    m_document->pageManager()->pageStyle(style.name()).setPageLayout(layout);
    refreshCanvas();
}


QList<QWidget *> KWPageTool::createOptionWidgets()
{
    QList<QWidget*> widgets;

    SimpleSetupWidget *ssw = new SimpleSetupWidget(m_canvas->view());
    ssw->setWindowTitle(i18n("Page Setup"));
    widgets.append(ssw);

    SimplePageStyleWidget *spsw = new SimplePageStyleWidget(m_canvas->view(),this);
    spsw->setWindowTitle(i18n("Pages Style"));
    widgets.append(spsw);

    SimpleHeaderFooterWidget *shfw = new SimpleHeaderFooterWidget(m_canvas->view(),this);
    shfw->setWindowTitle(i18n("Header & Footer"));
    widgets.append(shfw);

    return widgets;
}

void KWPageTool::setStyleFromWidget(QString style) {
    m_styleFomWiget = style;
}

void KWPageTool::refreshCanvas()
{
    QPoint scrollTmp = m_canvas->canvasController()->scrollBarValue();
    m_document->relayout();
    m_canvas->canvasController()->setScrollBarValue(scrollTmp);
    m_canvas->repaint();
}

#include "KWPageTool.moc"


