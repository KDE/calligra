/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "KoCellTool.h"

#include "KoAbstractApplicationController.h"
#include "KoExternalEditorInterface.h"
#include "KoCellTool.h"
#include "FoCellEditor.h"

// Calligra
#include <sheets/Cell.h>
#include <sheets/Style.h>
#include <sheets/CellStorage.h>
#include <sheets/Sheet.h>

#include "KoColor.h"
#include "KoCanvasBase.h"
#include "KoCanvasController.h"
#include "KoViewConverter.h"
#include "KoColorSpace.h"
#include "KoColorSpaceRegistry.h"

#include <QApplication>
#include <QKeyEvent>

#define SEARCH_CONDITION(exactMatch) cell.displayText().contains(m_searchString, exactMatch) ||\
                                     cell.comment().contains(m_searchString, exactMatch) ||\
                                     cell.link().contains(m_searchString,exactMatch)

KoCellTool::KoCellTool(KoAbstractApplicationController *controller, KoCanvasBase* canvas)
    : Calligra::Tables::CellTool(canvas),
      m_editor(0),
      m_currentfindPosition(0),
      m_searchCaseSensitive(false)
{
    m_searchString.clear();
    m_matchedPosition.clear();
    m_externalEditor = controller->createExternalCellEditor(this);
    m_findArea.setCoords(0,0,0,0);
}

KoCellTool::~KoCellTool()
{
}

void KoCellTool::selectFontSize(int size){
    fontSize(size);

    //below function will automatically adjust size of row as per font size we select
    adjustRow();
}

void KoCellTool::selectFontType(const QString& fonttype){
    font(fonttype);
}

void KoCellTool::selectTextColor(const QColor &color) {
    KoColor *textcolor = new KoColor(color,KoColorSpaceRegistry::instance()->rgb16(0));
    changeTextColor(*textcolor);
}

void KoCellTool::selectTextBackgroundColor(const QColor &color) {
    KoColor *textbackgroundcolor = new KoColor(color,KoColorSpaceRegistry::instance()->rgb16(0));
    changeBackgroundColor(*textbackgroundcolor);
}

KoExternalEditorInterface* KoCellTool::externalEditor() const
{
    return m_externalEditor;
}

int KoCellTool::getFontSize() {
    const Calligra::Tables::Style style = Calligra::Tables::Cell(
        selection()->activeSheet(), selection()->marker()).style();
    return style.fontSize();
}

QString KoCellTool::getFontType() {
    const Calligra::Tables::Style style = Calligra::Tables::Cell(
        selection()->activeSheet(), selection()->marker()).style();
    return style.fontFamily();
}

bool KoCellTool::isFontBold() {
    const Calligra::Tables::Style style = Calligra::Tables::Cell(
        selection()->activeSheet(), selection()->marker()).style();
    return style.bold();
}

bool KoCellTool::isFontItalic() {
    const Calligra::Tables::Style style = Calligra::Tables::Cell(
        selection()->activeSheet(), selection()->marker()).style();
    return style.italic();
}

bool KoCellTool::isFontUnderline() {
    const Calligra::Tables::Style style = Calligra::Tables::Cell(
        selection()->activeSheet(), selection()->marker()).style();
    return style.underline();
}

Calligra::Tables::CellEditorBase* KoCellTool::editor() const
{
   return m_editor;
}

bool KoCellTool::createEditor(bool clear, bool /*focus*/)
{
    bool status=false;
    const Calligra::Tables::Cell cell(selection()->activeSheet(), selection()->marker());

    if (selection()->activeSheet()->isProtected() && !cell.style().notProtected())
        return false;

    if(!editor()) {
#warning TODO create abstract interface for cell editor
        m_editor = new FoCellEditor(this, canvas()->canvasWidget());
        m_editor->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
        if(m_editor) {
            status=true;
        }
    }

    if(status) {

        double w = cell.width();
        double h = cell.height();
        double min_w = cell.width();
        double min_h = cell.height();


        double xpos = selection()->activeSheet()->columnPosition(selection()->marker().x());
        xpos += canvas()->viewConverter()->viewToDocumentX(canvas()->canvasController()->canvasOffsetX());

        Qt::LayoutDirection sheetDir = selection()->activeSheet()->layoutDirection();
        bool rtlText = cell.displayText().isRightToLeft();

        // if sheet and cell direction don't match, then the editor's location
        // needs to be shifted backwards so that it's right above the cell's text
        if (w > 0 && ((sheetDir == Qt::RightToLeft && !rtlText) ||
                      (sheetDir == Qt::LeftToRight && rtlText)))
            xpos -= w - min_w;

        // paint editor above correct cell if sheet direction is RTL
        if (sheetDir == Qt::RightToLeft) {
            double dwidth = canvas()->viewConverter()->viewToDocumentX(canvas()->canvasWidget()->width());
            double w2 = qMax(w, min_w);
            xpos = dwidth - w2 - xpos;
        }

        double ypos = selection()->activeSheet()->rowPosition(selection()->marker().y());
        ypos += canvas()->viewConverter()->viewToDocumentY(canvas()->canvasController()->canvasOffsetY());

        // Setup the editor's palette.
        const Calligra::Tables::Style style = cell.effectiveStyle();
        QPalette editorPalette(m_editor->palette());
        QColor color = style.fontColor();
        if (!color.isValid())
            color = canvas()->canvasWidget()->palette().text().color();
        editorPalette.setColor(QPalette::Text, color);
        color = style.backgroundColor();
        if (!color.isValid())
            color = editorPalette.base().color();
        editorPalette.setColor(QPalette::Background, color);
        m_editor->setPalette(editorPalette);

        // apply (table shape) offset
        xpos += offset().x();
        ypos += offset().y();

        const QRectF rect(xpos + 0.5, ypos + 0.5, w - 0.5, h - 0.5); //needed to circumvent rounding issue with height/width
        const QRectF zoomedRect = canvas()->viewConverter()->documentToView(rect);
        m_editor->setGeometry(zoomedRect.toRect().adjusted(1, 1, -1, -1));
        m_editor->setMinimumSize(QSize((int)canvas()->viewConverter()->documentToViewX(min_w) - 1,
                                       (int)canvas()->viewConverter()->documentToViewY(min_h) - 1));
        m_editor->show();

        // Laurent 2001-12-05
        // Don't add focus when we create a new editor and
        // we select text in edit widget otherwise we don't delete
        // selected text.
        /*if (focus)
        m_editor->setFocus();*/
        selection()->update();
        if (!clear && !cell.isNull()) {
            m_editor->setText(cell.userInput());

        }
        if (m_externalEditor) {
            if (clear) {
                m_externalEditor->clear();
            }
            else {
                m_externalEditor->setPlainText(editor()->toPlainText());
                m_externalEditor->setCursorPosition(m_externalEditor->toPlainText().length());
                connect(((FoCellEditor*)m_editor), SIGNAL(textChanged(const QString &)),
                        this, SLOT(setExternalText(const QString &)));
                if (dynamic_cast<QObject*>(m_externalEditor)) {
                    connect(dynamic_cast<QObject*>(m_externalEditor), SIGNAL(textChanged(const QString &)),
                            this, SLOT(setText(const QString &)));
                }
            }
        }
        m_editor->setFocus();
        m_editor->setCursorPosition(m_editor->toPlainText().length());
    }
    return status;
}

void KoCellTool::setText(const QString& text)
{
    if (sender() == m_editor) {
        if (m_externalEditor) {
            m_externalEditor->setText(text);
        }
    }
    else if (sender() == dynamic_cast<QObject*>(m_externalEditor)) {
        m_editor->setText(text);
    }
}

void KoCellTool::deleteEditor(bool saveChanges, bool expandMatrix)
{
    if(!m_editor) {
        return;
    }
    m_editorContents=m_editor->toPlainText();

    if (saveChanges) {
        Calligra::Tables::CellToolBase::applyUserInput(m_editorContents, expandMatrix);
    } else {
        selection()->update();
    }
    //canvas()->canvasWidget()->setFocus();
    delete m_editor;
    m_editor =0;
}




void KoCellTool::keyPressEvent(QKeyEvent *event)
{
    Calligra::Tables::CellTool::keyPressEvent(event);
    if(event->key() == Qt::Key_Up ||
       event->key() == Qt::Key_Down ||
       event->key() == Qt::Key_Left ||
       event->key() == Qt::Key_Right ||
       event->key() == Qt::Key_Enter ||
       event->key() == Qt::Key_Return) {
        Calligra::Tables::Cell cell(selection()->activeSheet(), selection()->cursor());
        m_externalEditor->setPlainText(cell.displayText());
//        cancelCurrentStrategy();
//        createEditor(false,false);
    }
}

void KoCellTool::mousePressEvent(KoPointerEvent* event)
{
    Calligra::Tables::CellTool::mousePressEvent(event);
    if (selection()->isSingular()) {
        Calligra::Tables::Cell cell(selection()->activeSheet(), selection()->cursor());
        m_externalEditor->setPlainText(cell.displayText());
    } else {
        m_externalEditor->setPlainText("");
    }
    /* cancelCurrentStrategy();
    scrollToCell(selection()->cursor());
    createEditor(false);*/
}

void KoCellTool::slotSearchTextChanged(const QString &text)
{
    if(text.isEmpty() || text.isNull()) {
        return;
    }

    m_searchString=text;
    initializeFind();
}

void KoCellTool::initializeFind()
{
    Calligra::Tables::Sheet * currentSheet = selection()->activeSheet();
    QRect filledRect = currentSheet->usedArea(true);
    m_matchedPosition.clear();
    m_currentfindPosition=-1;
    if(filledRect!=m_findArea) {
        m_findArea.setCoords(filledRect.left(),filledRect.top(),
                             filledRect.right(),filledRect.bottom());
    }
    find();
}

int KoCellTool::find()
{
    //search row wise i.e reading order
    int row=m_findArea.left();
    Calligra::Tables::Sheet * currentSheet = selection()->activeSheet();
    while(row<=m_findArea.bottom()) {
        Calligra::Tables::Cell cell = currentSheet->cellStorage()->firstInRow(row);
        if(cell.isNull()) {
            row++;
            continue;
        }
        int col=cell.column();
        while (!cell.isNull() && col <= m_findArea.right()) {
            if(m_searchCaseSensitive) {
                if (SEARCH_CONDITION(Qt::CaseSensitive)) {
                    m_matchedPosition.append(cell.cellPosition());
                }
            } else {
                if (SEARCH_CONDITION(Qt::CaseInsensitive)) {
                    m_matchedPosition.append(cell.cellPosition());
                }
            }
            cell = currentSheet->cellStorage()->nextInRow(col, row);
            if(cell.isNull())
                continue;
            col = cell.column();
        }
        row++;
    }
    return 0;
}

void KoCellTool::slotHighlight(QPoint goToCell)
{
    //here we select the cell where the string was found.
    selection()->initialize(goToCell);
    Calligra::Tables::CellToolBase::scrollToCell(goToCell);
}

void KoCellTool::findNext()
{
    if(m_currentfindPosition<(m_matchedPosition.length()-1) && m_currentfindPosition>=0) {
        m_currentfindPosition++;
    } else {
        m_currentfindPosition=0;
    }

    slotHighlight(m_matchedPosition.value(m_currentfindPosition,QPoint(1,1)));
}

void KoCellTool::findPrevious()
{
    if(m_currentfindPosition<(m_matchedPosition.length()-1) && m_currentfindPosition>=0) {
        m_currentfindPosition--;
    } else {
        m_currentfindPosition=m_matchedPosition.length()-1;
    }

    slotHighlight(m_matchedPosition.value(m_currentfindPosition,QPoint(1,1)));
}

QPair<int,int> KoCellTool::currentSearchStatistics()
{
    if(m_currentfindPosition<0 || m_currentfindPosition>=m_matchedPosition.length()) {
        return qMakePair(0,0);
    }
    return qMakePair(m_currentfindPosition+1,m_matchedPosition.length());
}


void KoCellTool::setCaseSensitive(bool isSensitive)
{
    m_searchCaseSensitive=isSensitive;
}

//#include "KoCellTool.moc"
