/* This file is part of the KDE project
   Copyright 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

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
#include "FoCellEditor.h"

#include <sheets/ui/Selection.h>
#include <sheets/ui/CellToolBase.h>
#include <sheets/Cell.h>
#include <KoViewConverter.h>
#include <KoDpi.h>
#include <KoUnit.h>
#include <sheets/ui/CellEditorBase.h>

#include <QGtkStyle>
#include <QScrollBar>
#include <QFocusEvent>

FoCellEditor::FoCellEditor(Calligra::Tables::CellToolBase *cellTool, QWidget *parent)
    :QTextEdit(parent),
    m_cellTool(cellTool)
{
    this->setStyleSheet("* { color:black; } ");
    setInputMethodHints(Qt::ImhNoAutoUppercase);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);
    document()->setDocumentMargin(0);
    const Calligra::Tables::Cell cell(m_cellTool->selection()->activeSheet(), m_cellTool->selection()->marker());
    const bool wrapText = cell.style().wrapText();
    Q_UNUSED(wrapText);
    setWordWrapMode(QTextOption::WordWrap);
    setReadOnly(false);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

FoCellEditor::~FoCellEditor()
{

}

void FoCellEditor::selectionChanged()
{

}

void FoCellEditor::setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter)
{
    const qreal scaleY = POINT_TO_INCH(static_cast<qreal>((KoDpi::dpiY())));
    setFont(QFont(font.family(), viewConverter->documentToViewY(font.pointSizeF()) / scaleY));

    if (updateSize) {
        QFontMetrics fontMetrics(this->font());
        int width = fontMetrics.width(toPlainText()) + fontMetrics.averageCharWidth();
        // don't make it smaller: then we would have to repaint the obscured cells
        if (width < this->width())
            width = this->width();
        int height = fontMetrics.height();
        if (height < this->height())
            height = this->height();
        setGeometry(x(), y(), width, height);
        update();
    }
}

void FoCellEditor::setText(const QString& text, int cursorPos)
{
    if (text == toPlainText()) {
        return;
    }

    setPlainText(text);

    if (cursorPos >= 0) {
        if (cursorPos > text.length()) {
            cursorPos = text.length();
        }
        setCursorPosition(cursorPos);
    }
}

QString FoCellEditor::toPlainText() const
{
    return QTextEdit::toPlainText();
}

void FoCellEditor::setCursorPosition(int pos)
{
    QTextCursor textCursor(this->textCursor());
    textCursor.setPosition(pos);
    setTextCursor(textCursor);
}

int FoCellEditor::cursorPosition() const
{
    return textCursor().position();
}

void FoCellEditor::setActiveSubRegion(int /*index*/)
{
    /*index = qBound(0, index, (int)d->highlighter->rangeCount());
    int counter = 0;
    bool subRegion = false;
    const Tokens tokens = d->highlighter->formulaTokens();
    for (int i = 0; i < tokens.count(); ++i) {
        const Token token = tokens[i];
        switch (token.type()) {
        case Token::Cell:
        case Token::Range:
            if (!subRegion) {
                d->currentToken = i;
                subRegion = true;
            }
            if (counter == index) {
                setCursorPosition(token.pos() + token.text().length() + 1);
                return;
            }
            ++counter;
            continue;
        case Token::Operator:
            if (token.asOperator() == Token::Semicolon) {
                if (subRegion) {
                    continue;
                }
            }
            subRegion = false;
            continue;
        default:
            subRegion = false;
            continue;
        }
    }*/
}


void FoCellEditor::slotTextChanged()
{
    // Fix the position.
    verticalScrollBar()->setValue(1);

    const QString text = toPlainText();

    const QFontMetricsF fontMetrics(font());
    const QRectF rect = fontMetrics.boundingRect(this->rect(), 0, text);
    const int requiredWidth = rect.width();
    const int requiredHeight = rect.height() - 1; // -1 to fit into a default cell
    if (text.isRightToLeft()) {
        setGeometry(x() - requiredWidth + width(), y(), requiredWidth, requiredHeight);
    } else {
        setGeometry(x(), y(), requiredWidth, requiredHeight);
    }

#if 0 // CALLIGRA_TABLES_WIP_EDITOR_OVERHAUL
    const Cell cell(d->selection->activeSheet(), d->selection->marker());
    if ((cell.style().formatType()) == Format::Percentage) {
        if ((text.length() == 1) && text[0].isDigit()) {
            setPlainText(text + " %");
            setCursorPosition(1);
            return;
        }
    }
#endif // CALLIGRA_TABLES_WIP_EDITOR_OVERHAUL

    // update the external editor, but only if we have focus
    if (hasFocus()) {
        emit textChanged(text);
    }
#if 0
    // Enable/disable the reference selection.
   if (!text.isEmpty() && text[0] == '=') {
        m_cellTool->selection()->startReferenceSelection();
    } else {
        m_cellTool->selection()->endReferenceSelection();
        return;
    }

    //Code to be uncommented when using formula.
     if (d->highlighter->rangeChanged()) {
        // Reset the flag, that indicates range changes after text changes.
        d->highlighter->resetRangeChanged();
        // Rebuild the reference selection by using the formula tokens.
        d->rebuildSelection();
    }
#endif
}


void FoCellEditor::slotCursorPositionChanged()
{

}

void FoCellEditor::focusInEvent(QFocusEvent *event)
{
    if (event->reason() != Qt::OtherFocusReason) {
        kDebug() << "induced by user";
        m_cellTool->setLastEditorWithFocus(Calligra::Tables::CellToolBase::EmbeddedEditor);
    }
    QTextEdit::focusInEvent(event);
}

void FoCellEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        // Forward left/right arrows to parent, so that pressing left/right
        // in this editor leaves editing mode, unless this editor has been
        // set to capture arrow key events.
        event->ignore(); // pass to parent
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        // Shift + Return: manual line wrap
        if (event->modifiers() & Qt::ShiftModifier) {
            break; // pass to KTextEdit
        }
        event->ignore(); // pass to parent
        return;
    }
    QTextEdit::keyPressEvent(event);
}

//#include "FoCellEditor.moc"
