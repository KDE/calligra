/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
#include "FoCellTool.h"

#include "FoExternalEditor.h"
#include "FoCellTool.h"
#include "FoCellEditor.h"

// KOffice
#include "Cell.h"
#include "Style.h"

#include "KoColor.h"
#include <ui/CellEditor.h>
#include "KoCanvasBase.h"
#include "KoCanvasController.h"
#include "KoViewConverter.h"
#include "Cell.h"
#include "Sheet.h"
#include "Style.h"
#include "KoColorSpace.h"
#include "KoColorSpaceRegistry.h"

using KSpread::CellEditorBase;
using KSpread::Sheet;
using KSpread::Cell;
using KSpread::Style;

FoCellTool::FoCellTool(KoCanvasBase* canvas)
        : KSpread::CellTool(canvas),
        m_editor(0)
{
    m_externalEditor=new FoExternalEditor();
    m_externalEditor->setCellTool(this);
}

FoCellTool::~FoCellTool()
{
}

void FoCellTool::selectFontSize(int size){
    fontSize(size);

    //below function will automatically adjust size of row as per font size we select
    adjustRow();
}

void FoCellTool::selectFontType(const QString& fonttype){
    font(fonttype);
}

void FoCellTool::selectTextColor(const QColor &color) {
    KoColor *textcolor = new KoColor(color,KoColorSpaceRegistry::instance()->rgb16(0));
    changeTextColor(*textcolor);
}

void FoCellTool::selectTextBackgroundColor(const QColor &color) {
    KoColor *textbackgroundcolor = new KoColor(color,KoColorSpaceRegistry::instance()->rgb16(0));
    changeBackgroundColor(*textbackgroundcolor);
}

FoExternalEditor *FoCellTool::externalEditor()
{
    return m_externalEditor;
}

int FoCellTool::getFontSize() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.fontSize();
}

QString FoCellTool::getFontType() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.fontFamily();
}

bool FoCellTool::isFontBold() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.bold();
}

bool FoCellTool::isFontItalic() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.italic();
}

bool FoCellTool::isFontUnderline() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.underline();
}

KSpread::CellEditorBase* FoCellTool::editor() const
{
   return m_editor;
}

bool FoCellTool::createEditor(bool clear, bool focus)
{
    bool status=false;
    const Cell cell(selection()->activeSheet(), selection()->marker());

    if (selection()->activeSheet()->isProtected() && !cell.style().notProtected())
        return false;

    if(!editor()) {
        m_editor=new FoCellEditor(this,canvas()->canvasWidget());
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
        const Style style = cell.effectiveStyle();
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
        if(clear) {
            m_externalEditor->clear();
        } else {
            m_externalEditor->setPlainText(editor()->toPlainText());
            m_externalEditor->setCursorPosition(m_externalEditor->toPlainText().length());
            connect(((FoCellEditor*)m_editor), SIGNAL(textChanged(const QString &)),
                    m_externalEditor, SLOT(setText(const QString &)));
            connect(m_externalEditor, SIGNAL(textChanged(const QString &)),
                    ((FoCellEditor*)m_editor), SLOT(setText(const QString &)));
        }
    }
    return status;
}

void FoCellTool::deleteEditor(bool saveChanges, bool expandMatrix)
{
    if(!m_editor) {
        return;
    }
    m_editorContents=m_editor->toPlainText();

    if (saveChanges) {
        CellToolBase::applyUserInput(m_editorContents, expandMatrix);
    } else {
        selection()->update();
    }
    //canvas()->canvasWidget()->setFocus();
    delete m_editor;
    m_editor =0;
}




void FoCellTool::keyPressEvent(QKeyEvent *event)
{
    KSpread::CellTool::keyPressEvent(event);
    /*if(event->key() == Qt::Key_Up ||
       event->key() == Qt::Key_Down ||
       event->key() == Qt::Key_Left ||
       event->key() == Qt::Key_Right ||
       event->key() == Qt::Key_Enter ||
       event->key() == Qt::Key_Return) {
        cancelCurrentStrategy();
        createEditor(false,false);
    }*/
}

void FoCellTool::mousePressEvent(KoPointerEvent* event)
{
    KSpread::CellTool::mousePressEvent(event);
    /* cancelCurrentStrategy();
    scrollToCell(selection()->cursor());
    createEditor(false);*/
}

