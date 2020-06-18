/* This file is part of the KDE project
   Copyright 1999-2006 The KSpread Team <calligra-devel@kde.org>

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

#include "CellEditor.h"

// Sheets
#include "ApplicationSettings.h"
#include "CellToolBase.h"
#include "Formula.h"
#include "FormulaEditorHighlighter.h"
#include "Function.h"
#include "FunctionRepository.h"
#include "FunctionCompletion.h"
#include "Selection.h"
#include "Sheet.h"
#include "Style.h"
#include "Cell.h"
#include "ValueConverter.h"
#include "RectStorage.h"
#include "Map.h"
#include "Value.h"
#include "CellStorage.h"
#include "SheetsDebug.h"

// Calligra
#include <KoDpi.h>
#include <KoUnit.h>
#include <KoViewConverter.h>
// KF5
#include <ktextedit.h>

// Qt
#include <QFocusEvent>
#include <QKeyEvent>
#include <QCompleter>
#include <QAbstractItemView>
#include <QStringListModel>
#include <QScrollBar>
#include <QList>

using namespace Calligra::Sheets;

class CellEditor::Private
{
public:
    CellToolBase*             cellTool;
    Selection*                selection;
    KTextEdit*                textEdit;
    FormulaEditorHighlighter* highlighter;
    FunctionCompletion*       functionCompletion;
    QTimer*                   functionCompletionTimer;
    QHash<int, QString>       *wordCollection;
    QPoint globalCursorPos;
    QCompleter                *complete;
    bool captureAllKeyEvents : 1;
    bool selectionChangedLocked     : 1;
    int currentToken;

public:
    void updateActiveSubRegion(const Tokens &tokens);
    void rebuildSelection();
};

void CellEditor::Private::updateActiveSubRegion(const Tokens &tokens)
{
    // Index of the token, at which the text cursor is positioned.
    // For sub-regions it is the start range.
    currentToken = 0;

    if (tokens.isEmpty()) {
        selection->setActiveSubRegion(0, 0); // also set the active element
        return;
    }

    const int cursorPosition = textEdit->textCursor().position() - 1; // without '='
    debugSheets << "cursorPosition:" << cursorPosition << "textLength:" << textEdit->toPlainText().length() - 1;

    uint rangeCounter = 0; // counts the ranges in the sub-region
    uint currentRange = 0; // range index denoting the current range
    int regionStart = 0; // range index denoting the sub-region start
    uint regionEnd = 0; // range index denoting the sub-region end
    enum { Anywhere, InRegion, BeyondCursor } state = Anywhere;

    Token token;
    Token::Type type;
    // Search the current range the text cursor is positioned to.
    // Determine the subregion start and end, in which the range is located.
    for (int i = 0; i < tokens.count(); ++i) {
        token = tokens[i];
        type = token.type();

        // If not in a subregion, we may already quit the loop here.
        if (state == Anywhere) {
            // Already beyond the cursor position?
            if (token.pos() > cursorPosition) {
                state = BeyondCursor;
                break; // for loop
            }
        } else if (state == InRegion) {
            // Loop to the end of the subregion.
            if (type == Token::Cell || type == Token::Range) {
                regionEnd = rangeCounter++;
                continue; // keep going until the referenced region ends
            }
            if (type == Token::Operator) {
                if (tokens[i].asOperator() == Token::Semicolon) {
                    continue; // keep going until the referenced region ends
                }
            }
            state = Anywhere;
            continue;
        }

        // Can the token be replaced by a reference?
        switch (type) {
        case Token::Cell:
        case Token::Range:
            if (state == Anywhere) {
                currentToken = i;
                regionStart = rangeCounter;
                state = InRegion;
            }
            regionEnd = rangeCounter; // length = 1
            currentRange = ++rangeCounter; // point behind the last
            continue;
        case Token::Unknown:
        case Token::Boolean:
        case Token::Integer:
        case Token::Float:
        case Token::String:
        case Token::Error:
            // Set the active sub-region start to the next range but
            // with a length of 0, which results in inserting a new range
            // to the selection on calling Selection::initialize() or
            // Selection::update().
            currentToken = i;
            regionStart = rangeCounter; // position of the next range
            regionEnd = rangeCounter - 1; // length = 0
            currentRange = rangeCounter;
            continue;
        case Token::Operator:
        case Token::Identifier:
            continue;
        }
    }

    // Cursor not reached? I.e. the cursor is placed at the last token's end.
    if (state == Anywhere) {
        token = tokens.last();
        type = token.type();
        // Check the last token.
        // It was processed, but maybe a reference can be placed behind it.
        // Check, if the token can be replaced by a reference.
        switch (type) {
        case Token::Operator:
            // Possible to place a reference behind the operator?
            switch (token.asOperator()) {
            case Token::Plus:
            case Token::Minus:
            case Token::Asterisk:
            case Token::Slash:
            case Token::Caret:
            case Token::LeftPar:
            case Token::Semicolon:
            case Token::Equal:
            case Token::NotEqual:
            case Token::Less:
            case Token::Greater:
            case Token::LessEqual:
            case Token::GreaterEqual:
            case Token::Intersect:
            case Token::Union:
                // Append new references by pointing behind the last.
                currentToken = tokens.count();
                regionStart = rangeCounter;
                regionEnd = rangeCounter - 1; // length = 0
                currentRange = rangeCounter;
                break;
            case Token::InvalidOp:
            case Token::RightPar:
            case Token::Comma:
            case Token::Ampersand:
            case Token::Percent:
            case Token::CurlyBra:
            case Token::CurlyKet:
            case Token::Pipe:
                // reference cannot be placed behind
                break;
            }
            break;
        case Token::Unknown:
        case Token::Boolean:
        case Token::Integer:
        case Token::Float:
        case Token::String:
        case Token::Identifier:
        case Token::Error:
            // currentToken = tokens.count() - 1; // already set
            // Set the active sub-region start to the end of the selection
            // with a length of 0, which results in appending a new range
            // to the selection on calling Selection::initialize() or
            // Selection::update().
            regionStart = rangeCounter;
            regionEnd = rangeCounter - 1; // length = 0
            currentRange = rangeCounter;
            break;
        case Token::Cell:
        case Token::Range:
            // currentToken = tokens.count() - 1; // already set
            // Set the last range as active one. It is not a sub-region,
            // otherwise the state would have been InRegion.
            regionStart = rangeCounter - 1;
            regionEnd = rangeCounter - 1; // length = 1
            currentRange = rangeCounter; // point behind the last
            break;
        }
    }

    const int regionLength = regionEnd - regionStart + 1;
    debugSheets << "currentRange:" << currentRange << "regionStart:" << regionStart
    << "regionEnd:" << regionEnd << "regionLength:" << regionLength;

    selection->setActiveSubRegion(regionStart, regionLength, currentRange);
}


CellEditor::CellEditor(CellToolBase *cellTool,QHash<int,QString> &wordList, QWidget* parent)
        : KTextEdit(parent)
        , d(new Private)
{
    d->cellTool = cellTool;
    d->selection = cellTool->selection();
    d->textEdit = this;
    d->globalCursorPos = QPoint();
    d->captureAllKeyEvents = false;
    d->selectionChangedLocked = false;
    d->currentToken = 0;
    d->wordCollection = &wordList;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);
    document()->setDocumentMargin(0);
    // setMinimumHeight(fontMetrics().height());

    d->highlighter = new FormulaEditorHighlighter(this, d->selection);

    d->functionCompletion = new FunctionCompletion(this);
    d->functionCompletionTimer = new QTimer(this);

    const Cell cell(d->selection->activeSheet(), d->selection->marker());
    const bool wrapText = cell.style().wrapText();
    d->textEdit->setWordWrapMode(wrapText ? QTextOption::WordWrap : QTextOption::NoWrap);

#if 0 // FIXME Implement a completion aware KTextEdit.
    setCompletionMode(selection()->view()->doc()->completionMode());
    setCompletionObject(&selection()->view()->doc()->map()->stringCompletion(), true);
#endif
    
    //populateWordCollection();
    
    //AutoCompletion Code
    d->complete = new QCompleter(this);
    d->complete->setModel(model());
    //completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    d->complete->setCaseSensitivity(Qt::CaseInsensitive);
    d->complete->setWrapAround(false);
    this->setCompleter(d->complete);
    
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

CellEditor::~CellEditor()
{
    if (selection()){
        selection()->endReferenceSelection();
    }
    delete d;
}

Selection* CellEditor::selection() const
{
    return d->selection;
}

QPoint CellEditor::globalCursorPosition() const
{
    return d->globalCursorPos;
}

bool CellEditor::captureArrowKeys() const
{
    return d->captureAllKeyEvents;
}

void CellEditor::setCaptureArrowKeys(bool capture)
{
    d->captureAllKeyEvents = capture;
}

//AutoCompletion Functions

QAbstractItemModel *CellEditor::model()
{
  //ValueConverter *conv;
  QList<QString> words;
  QList<QString> wordlist;
  const Cell cell(d->selection->activeSheet(), d->selection->marker());
  int col = cell.column();
  words = d->wordCollection->values(col);
  for (int i = 0; i < 3 && (!words.isEmpty()) ; i++) {
    wordlist.push_back(words.front());
    words.pop_front();
  }
    return new QStringListModel(wordlist, d->complete);
}

void CellEditor::setCompleter(QCompleter *completer)
{
     if (d->complete)
         QObject::disconnect(d->complete, 0, this, 0);

     d->complete = completer;

     if (!d->complete)
         return;

     d->complete->setWidget(this);
     d->complete->setCompletionMode(QCompleter::PopupCompletion);
     d->complete->setCaseSensitivity(Qt::CaseInsensitive);
     QObject::connect(d->complete, SIGNAL(activated(QString)),
                      this, SLOT(insertCompletion(QString)));
}

QCompleter *CellEditor::completer() const
{
     return d->complete;
}

 void CellEditor::insertCompletion(const QString& completion)
{
     if (d->complete->widget() != this)
         return;
     QTextCursor tc = textCursor();
     int extra = completion.length() - d->complete->completionPrefix().length();
     tc.movePosition(QTextCursor::Left);
     tc.movePosition(QTextCursor::EndOfWord);
     tc.insertText(completion.right(extra));
     setTextCursor(tc);
}

QString CellEditor::textUnderCursor() const
{
     QTextCursor tc = textCursor();
     tc.select(QTextCursor::WordUnderCursor);
     return tc.selectedText();
}

void CellEditor::slotCursorPositionChanged()
{
    // Nothing to do, if not in reference selection mode.
    if (!selection()->referenceSelection()) {
        return;
    }
    // NOTE On text changes TextEdit::cursorPositionChanged() is triggered
    // before TextEdit::textChanged(). The text is already up-to-date.

    // Save the global position for the function auto-completion popup.
    d->globalCursorPos = mapToGlobal(cursorRect().bottomLeft());

    // Needs up-to-date tokens; QSyntaxHighlighter::rehighlight() gets called
    // automatically on text changes, which does the update.
    d->updateActiveSubRegion(d->highlighter->formulaTokens());
}

void CellEditor::Private::rebuildSelection()
{
    // Do not react on selection changes, that update the formula's expression,
    // because the selection gets already build based on the current formula.
    selectionChangedLocked = true;

    Sheet *const originSheet = selection->originSheet();
    Map *const map = originSheet->map();

    // Rebuild the reference selection by using the formula tokens.
    Tokens tokens = highlighter->formulaTokens();
    selection->update(); // set the old cursor dirty; updates the editors
    selection->clear();

    //A list of regions which have already been highlighted on the spreadsheet.
    //This is so that we don't end up highlighting the same region twice in two different
    //colors.
    QSet<QString> alreadyUsedRegions;

    int counter = 0;
    for (int i = 0; i < tokens.count(); ++i) {
        const Token token = tokens[i];
        const Token::Type type = token.type();

        if (type == Token::Cell || type == Token::Range) {
            const Region region(token.text(), map, originSheet);

            if (!region.isValid() || region.isEmpty()) {
                continue;
            }
            if (alreadyUsedRegions.contains(region.name())) {
                continue;
            }
            alreadyUsedRegions.insert(region.name());

            const QRect range = region.firstRange();
            Sheet *const sheet = region.firstSheet();

            selection->initialize(range, sheet);
            // Always append the next range by pointing behind the last item.
            selection->setActiveSubRegion(++counter, 0);
        }
    }

    // Set the active sub-region.
    // Needs up-to-date tokens; QSyntaxHighlighter::rehighlight() gets called
    // automatically on text changes, which does the update.
    updateActiveSubRegion(highlighter->formulaTokens());

    selectionChangedLocked = false;
}

void CellEditor::setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter)
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
    }
}

void CellEditor::slotCompletionModeChanged(KCompletion::CompletionMode _completion)
{
    selection()->activeSheet()->map()->settings()->setCompletionMode(_completion);
}

void CellEditor::slotTextChanged()
{
    // NOTE On text changes TextEdit::cursorPositionChanged() is triggered
    // before TextEdit::textChanged().

    // Fix the position.
    verticalScrollBar()->setValue(1);

    const QString text = toPlainText();

    const QFontMetricsF fontMetrics(font());
    // TODO Adjust size depending on which cells can be obscured (see CellView)?
    // The following line would result in an unchanged width for cells with
    // enabled word wrapping, but after the user input got applied it may
    // obscure cells horizontally.
    // Passing no flags will only change the height, if a manual line break is
    // entered (Shift + Return).
    // const int flags = wordWrapMode() == QTextOption::NoWrap ? 0 : Qt::TextWordWrap;
    const QRectF rect = fontMetrics.boundingRect(this->rect(), 0, text);
    const int requiredWidth = rect.width();
    const int requiredHeight = rect.height() - 1; // -1 to fit into a default cell
    if (text.isRightToLeft()) {
        setGeometry(x() - requiredWidth + width(), y(), requiredWidth, requiredHeight);
    } else {
        setGeometry(x(), y(), requiredWidth, requiredHeight);
    }

    // FIXME Stefan: Is this really wanted? The percent char does not get
    // removed on applying the user input. If the style changes afterwards,
    // the user input is still indicating a percent value. If the digit gets
    // deleted while editing the percent char also stays. Disabling for now.
#if 0 // CALLIGRA_SHEETS_WIP_EDITOR_OVERHAUL
    const Cell cell(d->selection->activeSheet(), d->selection->marker());
    if ((cell.style().formatType()) == Format::Percentage) {
        if ((text.length() == 1) && text[0].isDigit()) {
            setPlainText(text + " %");
            setCursorPosition(1);
            return;
        }
    }
#endif // CALLIGRA_SHEETS_WIP_EDITOR_OVERHAUL

    // update the external editor, but only if we have focus
    if (hasFocus()) {
        emit textChanged(text);
    }

    // Enable/disable the reference selection.
    if (!text.isEmpty() && text[0] == '=') {
        selection()->startReferenceSelection();
    } else {
        selection()->endReferenceSelection();
        return;
    }

    // The expression highlighting got updated automatically.
    // If a reference has changed since the last update ...
    if (d->highlighter->rangeChanged()) {
        // Reset the flag, that indicates range changes after text changes.
        d->highlighter->resetRangeChanged();
        // Rebuild the reference selection by using the formula tokens.
        d->rebuildSelection();
    }
}

// Called on selection (and sheet) changes.
void CellEditor::selectionChanged()
{
    if (d->selectionChangedLocked) {
        return;
    }

    Selection* choice = selection();

    if (choice->isEmpty())
        return;

    const QString text = toPlainText();
    const int textLength = text.length();

    // Find the start text cursor position for the active sub-region within
    // the formula's expression and determine the length of the sub-region.
    Tokens tokens = d->highlighter->formulaTokens();
    uint start = 1;
    uint length = 0;
    if (!tokens.empty()) {
        if (d->currentToken < tokens.count()) {
            Token token = tokens[d->currentToken];
            Token::Type type = token.type();
            if (type == Token::Cell || type == Token::Range) {
                start = token.pos() + 1; // don't forget the '='!
                length = token.text().length();
                // Iterate to the end of the sub-region.
                for (int i = d->currentToken + 1; i < tokens.count(); ++i) {
                    token = tokens[i];
                    type = token.type();
                    switch (type) {
                    case Token::Cell:
                    case Token::Range:
                        length += token.text().length();
                        continue;
                    case Token::Operator:
                        if (token.asOperator() == Token::Semicolon) {
                            ++length;
                            continue;
                        }
                    default:
                        break;
                    }
                    break;
                }
            } else {
                start = token.pos() + 1; // don't forget the '='!
                length = token.text().length();
            }
        } else {
            // sanitize
            d->currentToken = tokens.count();
            start = textLength;
        }
    }

    // Replace the formula's active sub-region with the selection's one.
    const QString address = choice->activeSubRegionName();
    const QString newExpression = QString(text).replace(start, length, address);
    // The expression highlighting gets updated automatically by the next call,
    // even though signals are blocked (must be connected to QTextDocument).
    blockSignals(true);
    setText(newExpression, start + address.length());
    blockSignals(false);

    // Ranges have changed.
    // Reset the flag, that indicates range changes after text changes.
    d->highlighter->resetRangeChanged();
    // Mirror the behaviour of slotCursorPositionChanged(), but here the tokens
    // are already up-to-date.
    d->globalCursorPos = mapToGlobal(cursorRect().bottomLeft());
    // Set the active sub-region.
    // Needs up-to-date tokens; QSyntaxHighlighter::rehighlight() gets called
    // automatically on text changes, which does the update.
    d->updateActiveSubRegion(d->highlighter->formulaTokens());

    // Always emit, because this editor may be hidden or does not have focus,
    // but the external one needs an update.
    emit textChanged(toPlainText());
}

void CellEditor::keyPressEvent(QKeyEvent *event)
 { 
    const Cell cell_temp(d->selection->activeSheet(), d->selection->marker()); 
   
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
        // Forward left/right arrows to parent, so that pressing left/right
        // in this editor leaves editing mode, unless this editor has been
        // set to capture arrow key events.
        if (d->captureAllKeyEvents) {
            break; // pass to TextEdit
        }
        event->ignore(); // pass to parent
        return;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        // Always forward tab/backtab to parent, so that pressing them leaves
        // editing mode. To insert literal tabs you can always use the external
        // editor.

        if (!textUnderCursor().isEmpty() && !d->wordCollection->values(cell_temp.column()).contains(textUnderCursor())) {
	  d->wordCollection->insertMulti(cell_temp.column(), textUnderCursor());
	}
	event->ignore();
	return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        // Shift + Return: manual line wrap
        if (event->modifiers() & Qt::ShiftModifier) {
            break; // pass to TextEdit
        }
        if (!textUnderCursor().isEmpty() && !d->wordCollection->values(cell_temp.column()).contains(textUnderCursor())) {
	  d->wordCollection->insertMulti(cell_temp.column(), textUnderCursor());
	}
	event->ignore(); // pass to parent
        return;
    }
    
    if (d->complete && d->complete->popup()->isVisible()) {
         // The following keys are forwarded by the completer to the widget
        switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
             event->ignore();
             return; // let the completer do default behavior
        default:
            break;
        }
     }


     bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E); // CTRL+E
     if (!d->complete || !isShortcut) // do not process the shortcut when we have a completer
         QTextEdit::keyPressEvent(event);

     const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
     if (!d->complete || (ctrlOrShift && event->text().isEmpty()))
         return;

     static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
     bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor();

     if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 3||eow.contains(event->text().right(1)))) {
         d->complete->popup()->hide();
         return;
     }
     if (completionPrefix != d->complete->completionPrefix()) {
         d->complete->setCompletionPrefix(completionPrefix);
         d->complete->popup()->setCurrentIndex(d->complete->completionModel()->index(0, 0));
    }
     QRect cr = cursorRect();
     cr.setWidth(d->complete->popup()->sizeHintForColumn(0) + d->complete->popup()->verticalScrollBar()->sizeHint().width());
     d->complete->complete(); // pop it up! 
    
}

void CellEditor::focusInEvent(QFocusEvent *event)
{
    if (d->complete)
         d->complete->setWidget(this);
     KTextEdit::focusInEvent(event);
    // If the focussing is user induced.
    if (event->reason() != Qt::OtherFocusReason) {
        debugSheets << "induced by user";
        d->cellTool->setLastEditorWithFocus(CellToolBase::EmbeddedEditor);
    }
    KTextEdit::focusInEvent(event);
}

void CellEditor::keyReleaseEvent(QKeyEvent *event)
{
    // need to eat event or else it is delivered to others and weird things might happen
    event->accept();
}

void CellEditor::focusOutEvent(QFocusEvent *event)
{
    KTextEdit::focusOutEvent(event);
}

void CellEditor::setText(const QString& text, int cursorPos)
{
    if (text == toPlainText()) return;

    setPlainText(text);

    // Only update the cursor position, if a non-negative value was set.
    // The default parameter value is -1, i.e. the cursor does not get touched.
    if (cursorPos >= 0) {
        if (cursorPos > text.length()) {
            // Usability: It is usually more convenient, if the cursor is
            // positioned at the end of the text so it can be quickly deleted
            // using the backspace key.
            cursorPos = text.length();
        }
        setCursorPosition(cursorPos);
    }
}

// helper for CellEditor::permuteFixation()
QString permuteLocationFixation(const QString &regionName, int &i,
                                bool columnFixed, bool rowFixed)
{
    QString result;
    if (columnFixed) {
        result += '$';
    }
    // copy the column letter(s)
    while (i < regionName.count()) {
        if (!regionName[i].isLetter()) {
            if (regionName[i] == '$') {
                // swallow the old fixation
                ++i;
                continue;
            }
            // stop, if not a column letter
            break;
        }
        result += regionName[i++];
    }
    if (rowFixed) {
        result += '$';
    }
    // copy the row number(s)
    while (i < regionName.count()) {
        if (!regionName[i].isNumber()) {
            if (regionName[i] == '$') {
                // swallow the old fixation
                ++i;
                continue;
            }
            // stop, if not a row number
            break;
        }
        result += regionName[i++];
    }
    return result;
}

void CellEditor::permuteFixation()
{
    // Nothing to do, if not in reference selection mode.
    if (!d->selection->referenceSelection()) {
        return;
    }

    // Search for the last range before or the range at the cursor.
    int index = -1;
    const int cursorPosition = textCursor().position() - 1; // - '='
    const Tokens tokens = d->highlighter->formulaTokens();
    for (int i = 0; i < tokens.count(); ++i) {
        const Token token = tokens[i];
        if (token.pos() > cursorPosition) {
            break; // for loop
        }
        if (token.type() == Token::Cell || token.type() == Token::Range) {
            index = i;
        }
    }
    // Quit, if no range was found.
    if (index == -1) {
        return;
    }

    const Token token = tokens[index];
    Map *const map = d->selection->activeSheet()->map();
    QString regionName = token.text();
    // Filter sheet; truncates regionName; range without sheet name resides.
    Sheet *const sheet = Region(QString(), map).filterSheetName(regionName);
    const Region region(regionName, map, 0);
    // TODO Stefan: Skip named areas.
    if (!region.isValid()) {
        return;
    }
    // FIXME Stefan: need access to fixation, thus to Region::Range; must use iterator
    Region::Element *range = (*region.constBegin());
    QString result(sheet ? (sheet->sheetName() + '!') : QString());
    // Permute fixation.
    if (region.isSingular()) {
        char fixation = 0x00;
        if (range->isRowFixed()) {
            fixation += 0x01;
        }
        if (range->isColumnFixed()) {
            fixation += 0x02;
        }
        fixation += 0x01;

        int i = 0;
        result += permuteLocationFixation(regionName, i, fixation & 0x02, fixation & 0x01);
    } else {
        char fixation = 0x00;
        if (range->isBottomFixed()) {
            fixation += 0x01;
        }
        if (range->isRightFixed()) {
            fixation += 0x02;
        }
        if (range->isTopFixed()) {
            fixation += 0x04;
        }
        if (range->isLeftFixed()) {
            fixation += 0x08;
        }
        fixation += 0x01;

        int i = 0;
        result += permuteLocationFixation(regionName, i, fixation & 0x08, fixation & 0x04);
        Q_ASSERT(regionName[i] == ':');
        ++i;
        result += ':';
        result += permuteLocationFixation(regionName, i, fixation & 0x02, fixation & 0x01);
    }
    // Replace the range in the formula's expression.
    QString text = toPlainText();
    const int start = token.pos() + 1; // + '='
    const int length = token.text().length();
    setPlainText(text.replace(start, length, result));
    // Set the cursor to the end of the range.
    QTextCursor textCursor = this->textCursor();
    textCursor.setPosition(start + result.length());
    setTextCursor(textCursor);
}

int CellEditor::cursorPosition() const
{
    return textCursor().position();
}

void CellEditor::setCursorPosition(int pos)
{
    QTextCursor textCursor(this->textCursor());
    textCursor.setPosition(pos);
    setTextCursor(textCursor);
}

// Called by the cell tool when setting the active element with a cell location.
void CellEditor::setActiveSubRegion(int index)
{
    index = qBound(0, index, (int)d->highlighter->rangeCount());
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
    }
}
