/* This file is part of the KDE project
   Copyright 1999-2006 The KSpread Team <koffice-devel@kde.org>

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

#include "Editors.h"

// KSpread
#include "ApplicationSettings.h"
#include "Cell.h"
#include "CellStorage.h"
#include "CellToolBase.h"
#include "Formula.h"
#include "Functions.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Selection.h"
#include "Sheet.h"
#include "Style.h"

// KOffice
#include <KoCanvasBase.h>
#include <KoDpi.h>
#include <KoViewConverter.h>

// KDE
#include <kdebug.h>
#include <kdialog.h>
#include <kicon.h>
#include <klistwidget.h>
#include <kmessagebox.h>
#include <ktextedit.h>
#include <klocale.h>

// Qt
#include <QAbstractButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QFocusEvent>
#include <QFrame>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QResizeEvent>
#include <QTextLayout>
#include <QTimer>
#include <QToolButton>
#include <QToolTip>
#include <QVBoxLayout>
#include <QScrollBar>

#include "commands/NamedAreaCommand.h"

using namespace KSpread;

/*****************************************************************************
 *
 * FormulaEditorHighlighter
 *
 ****************************************************************************/

namespace KSpread
{

class FormulaEditorHighlighter::Private
{
public:
    Private() {
        selection = 0;
        tokens = Tokens();
        rangeCount = 0;
        rangeChanged = false;
    }

    // source for cell reference checking
    Selection* selection;
    Tokens tokens;
    uint rangeCount;
    bool rangeChanged;
};


FormulaEditorHighlighter::FormulaEditorHighlighter(QTextEdit* textEdit, Selection* selection)
        : QSyntaxHighlighter(textEdit)
        , d(new Private)
{
    d->selection = selection;
}

FormulaEditorHighlighter::~FormulaEditorHighlighter()
{
    delete d;
}

const Tokens& FormulaEditorHighlighter::formulaTokens() const
{
    return d->tokens;
}

void FormulaEditorHighlighter::highlightBlock(const QString& text)
{
    // reset syntax highlighting
    setFormat(0, text.length(), QApplication::palette().text().color());

    // save the old ones to identify range changes
    Tokens oldTokens = d->tokens;

    // interpret the text as formula
    // we accept invalid/incomplete formulas
    Formula f;
    d->tokens = f.scan(text);

    QFont editorFont = document()->defaultFont();
    QFont font;

    uint oldRangeCount = d->rangeCount;

    d->rangeCount = 0;
    QList<QColor> colors = d->selection->colors();
    QList<QString> alreadyFoundRanges;

    Sheet *const originSheet = d->selection->originSheet();
    Map *const map = originSheet->map();

    for (int i = 0; i < d->tokens.count(); ++i) {
        Token token = d->tokens[i];
        Token::Type type = token.type();

        switch (type) {
        case Token::Cell:
        case Token::Range: {
            // don't compare, if we have already found a change
            if (!d->rangeChanged && i < oldTokens.count() && token.text() != oldTokens[i].text()) {
                d->rangeChanged = true;
            }

            const Region newRange(token.text(), map, originSheet);
            if (!newRange.isValid()) {
                continue;
            }

            int index = alreadyFoundRanges.indexOf(newRange.name());
            if (index == -1) /* not found */ {
                alreadyFoundRanges.append(newRange.name());
                index = alreadyFoundRanges.count() - 1;
            }
            const QColor color(colors[index % colors.size()]);
            setFormat(token.pos() + 1, token.text().length(), color);
            ++d->rangeCount;
        }
        break;
        case Token::Boolean:     // True, False (also i18n-ized)
            /*        font = QFont(editorFont);
                    font.setBold(true);
                    setFormat(token.pos() + 1, token.text().length(), font);*/
            break;
        case Token::Identifier:   // function name or named area*/
            /*        font = QFont(editorFont);
                    font.setBold(true);
                    setFormat(token.pos() + 1, token.text().length(), font);*/
            break;

        case Token::Unknown:
        case Token::Integer:     // 14, 3, 1977
        case Token::Float:       // 3.141592, 1e10, 5.9e-7
        case Token::String:      // "KOffice", "The quick brown fox..."
        case Token::Error:
            break;
        case Token::Operator: {  // +, *, /, -
            switch (token.asOperator()) {
            case Token::LeftPar:
            case Token::RightPar:
                //Check where this brace is in relation to the cursor and highlight it if necessary.
                handleBrace(i);
                break;
            default:
                break;
            }
        }
        break;
        }
    }

    if (oldRangeCount != d->rangeCount)
        d->rangeChanged = true;
}

void FormulaEditorHighlighter::handleBrace(uint index)
{
    const Token& token = d->tokens.at(index);

    QTextEdit* textEdit = qobject_cast<QTextEdit*>(parent());
    Q_ASSERT(textEdit);
    int cursorPos = textEdit->textCursor().position();
    int distance = cursorPos - token.pos();
    int opType = token.asOperator();
    bool highlightBrace = false;

    //Check where the cursor is in relation to this left or right parenthesis token.
    //Only one pair of braces should be highlighted at a time, and if the cursor
    //is between two braces, the inner-most pair should be highlighted.

    if (opType == Token::LeftPar) {
        //If cursor is directly to the left of this left brace, highlight it
        if (distance == 1)
            highlightBrace = true;
        else
            //Cursor is directly to the right of this left brace, highlight it unless
            //there is another left brace to the right (in which case that should be highlighted instead as it
            //is the inner-most brace)
            if (distance == 2)
                if ((index == (uint)d->tokens.count() - 1) || (d->tokens.at(index + 1).asOperator() != Token::LeftPar))
                    highlightBrace = true;

    } else {
        //If cursor is directly to the right of this right brace, highlight it
        if (distance == 2)
            highlightBrace = true;
        else
            //Cursor is directly to the left of this right brace, so highlight it unless
            //there is another right brace to the left (in which case that should be highlighted instead as it
            //is the inner-most brace)
            if (distance == 1)
                if ((index == 0) || (d->tokens.at(index - 1).asOperator() != Token::RightPar))
                    highlightBrace = true;
    }

    if (highlightBrace) {
        QFont font = QFont(document()->defaultFont());
        font.setBold(true);
        setFormat(token.pos() + 1, token.text().length(), font);

        int matching = findMatchingBrace(index);

        if (matching != -1) {
            Token matchingBrace = d->tokens.at(matching);
            setFormat(matchingBrace.pos() + 1 , matchingBrace.text().length() , font);
        }
    }
}

int FormulaEditorHighlighter::findMatchingBrace(int pos)
{
    int depth = 0;
    int step = 0;

    Tokens tokens = d->tokens;

    //If this is a left brace we need to step forwards through the text to find the matching right brace,
    //otherwise, it is a right brace so we need to step backwards through the text to find the matching left
    //brace.
    if (tokens.at(pos).asOperator() == Token::LeftPar)
        step = 1;
    else
        step = -1;

    for (int index = pos ; (index >= 0) && (index < (int) tokens.count()) ; index += step) {
        if (tokens.at(index).asOperator() == Token::LeftPar)
            depth++;
        if (tokens.at(index).asOperator() == Token::RightPar)
            depth--;

        if (depth == 0) {
            return index;
        }
    }

    return -1;
}

uint FormulaEditorHighlighter::rangeCount() const
{
    return d->rangeCount;
}

bool FormulaEditorHighlighter::rangeChanged() const
{
    return d->rangeChanged;
}

void FormulaEditorHighlighter::resetRangeChanged()
{
    d->rangeChanged = false;
}

} // namespace KSpread



/*****************************************************************************
 *
 * FunctionCompletion
 *
 ****************************************************************************/

class FunctionCompletion::Private
{
public:
    CellEditor* editor;
    QFrame *completionPopup;
    KListWidget *completionListBox;
    QLabel* hintLabel;
};

FunctionCompletion::FunctionCompletion(CellEditor* editor)
        : QObject(editor)
        , d(new Private)
{
    d->editor = editor;
    d->hintLabel = 0;

    d->completionPopup = new QFrame(editor->topLevelWidget(), Qt::Popup);
    d->completionPopup->setFrameStyle(QFrame::Box | QFrame::Plain);
    d->completionPopup->setLineWidth(1);
    d->completionPopup->installEventFilter(this);
    d->completionPopup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QVBoxLayout *layout = new QVBoxLayout(d->completionPopup);
    layout->setMargin(0);
    layout->setSpacing(0);

    d->completionListBox = new KListWidget(d->completionPopup);
    d->completionPopup->setFocusProxy(d->completionListBox);
    d->completionListBox->setFrameStyle(QFrame::NoFrame);
//   d->completionListBox->setVariableWidth( true );
    d->completionListBox->installEventFilter(this);
    connect(d->completionListBox, SIGNAL(currentRowChanged(int)), SLOT(itemSelected()));
    // When items are activated on single click, also change the help page on mouse-over, otherwise there is no (easy) way to get
    // the help (with the mouse) without inserting the function
    if (d->completionListBox->style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick, 0, d->completionListBox)) {
        connect(d->completionListBox, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
        d->completionListBox->setMouseTracking(true);
    }

    connect(d->completionListBox, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(doneCompletion()));
    layout->addWidget(d->completionListBox);

    d->hintLabel = new QLabel(0, Qt::FramelessWindowHint | Qt::Tool |  Qt::X11BypassWindowManagerHint);
    d->hintLabel->setFrameStyle(QFrame::Plain | QFrame::Box);
    d->hintLabel->setPalette(QToolTip::palette());
    d->hintLabel->setWordWrap(true);
    d->hintLabel->hide();
}

FunctionCompletion::~FunctionCompletion()
{
    delete d->hintLabel;
    delete d;
}

void FunctionCompletion::itemSelected(QListWidgetItem* listItem)
{
    QString item;
    if (listItem) {
        item = listItem->text();
    } else {
        listItem = d->completionListBox->currentItem();
        if (listItem) {
            item = listItem->text();
        }
    }

    KSpread::FunctionDescription* desc;
    desc = KSpread::FunctionRepository::self()->functionInfo(item);
    if (!desc) {
        d->hintLabel->hide();
        return;
    }

    const QStringList helpTexts = desc->helpText();
    QString helpText = helpTexts.isEmpty() ? QString() : helpTexts.first();
    if (helpText.isEmpty()) {
        d->hintLabel->hide();
        return;
    }

    helpText.append("</qt>").prepend("<qt>");
    d->hintLabel->setText(helpText);
    d->hintLabel->adjustSize();

    // reposition nicely
    QPoint pos = d->editor->mapToGlobal(QPoint(d->editor->width(), 0));
    pos.setY(pos.y() - d->hintLabel->height() - 1);
    d->hintLabel->move(pos);
    d->hintLabel->show();
    d->hintLabel->raise();

    // do not show it forever
    //QTimer::singleShot( 5000, d->hintLabel, SLOT( hide()) );
}

bool FunctionCompletion::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != d->completionPopup && obj != d->completionListBox)
        return false;

    if (ev->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(ev);
        switch (ke->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            doneCompletion();
            return true;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            return false;
        default:
            d->hintLabel->hide();
            d->completionPopup->close();
            d->editor->setFocus();
            QApplication::sendEvent(d->editor, ev);
            return true;
        }
    }

    if (ev->type() == QEvent::Close) {
        d->hintLabel->hide();
    }

    if (ev->type() == QEvent::MouseButtonDblClick) {
        doneCompletion();
        return true;
    }
    return false;
}

void FunctionCompletion::doneCompletion()
{
    d->hintLabel->hide();
    d->completionPopup->close();
    d->editor->setFocus();
    emit selectedCompletion(d->completionListBox->currentItem()->text());
}

void FunctionCompletion::showCompletion(const QStringList &choices)
{
    if (!choices.count()) return;

    d->completionListBox->clear();
    d->completionListBox->addItems(choices);
    d->completionListBox->setCurrentItem(0);

    // size of the pop-up
    d->completionPopup->setMaximumHeight(100);
    d->completionPopup->resize(d->completionListBox->sizeHint() +
                               QSize(d->completionListBox->verticalScrollBar()->width() + 4,
                                     d->completionListBox->horizontalScrollBar()->height() + 4));
    int h = d->completionListBox->height();
    int w = d->completionListBox->width();

    QPoint pos = d->editor->globalCursorPosition();

    // if popup is partially invisible, move to other position
    // FIXME check it if it works in Xinerama multihead
    int screen_num = QApplication::desktop()->screenNumber(d->completionPopup);
    QRect screen = QApplication::desktop()->screenGeometry(screen_num);
    if (pos.y() + h > screen.y() + screen.height())
        pos.setY(pos.y() - h - d->editor->height());
    if (pos.x() + w > screen.x() + screen.width())
        pos.setX(screen.x() + screen.width() - w);

    d->completionPopup->move(pos);
    d->completionListBox->setFocus();
    d->completionPopup->show();
}



/****************************************************************************
 *
 * CellEditor
 *
 ****************************************************************************/

class CellEditor::Private
{
public:
    CellToolBase*             cellTool;
    Selection*                selection;
    KTextEdit*                textEdit;
    FormulaEditorHighlighter* highlighter;
    FunctionCompletion*       functionCompletion;
    QTimer*                   functionCompletionTimer;

    QPoint globalCursorPos;

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
    kDebug() << "cursorPosition:" << cursorPosition << "textLength:" << textEdit->toPlainText().length() - 1;

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
    kDebug() << "currentRange:" << currentRange << "regionStart:" << regionStart
             << "regionEnd:" << regionEnd << "regionLength:" << regionLength;

    selection->setActiveSubRegion(regionStart, regionLength, currentRange);
}


CellEditor::CellEditor(CellToolBase *cellTool, QWidget* parent)
        : KTextEdit(parent)
        , d(new Private)
{
    d->cellTool = cellTool;
    d->selection = cellTool->selection();
    d->textEdit = this;
    d->globalCursorPos = QPoint();
    d->captureAllKeyEvents = d->selection->activeSheet()->map()->settings()->captureAllArrowKeys();
  d->selectionChangedLocked = false;
    d->currentToken = 0;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);
    document()->setDocumentMargin(0);
    // setMinimumHeight(fontMetrics().height());

    d->highlighter = new FormulaEditorHighlighter(this, d->selection);

    d->functionCompletion = new FunctionCompletion(this);
    d->functionCompletionTimer = new QTimer(this);
    connect(d->functionCompletion, SIGNAL(selectedCompletion(const QString&)),
            SLOT(functionAutoComplete(const QString&)));
  connect( this, SIGNAL( textChanged() ), SLOT( checkFunctionAutoComplete() ) );
    connect(d->functionCompletionTimer, SIGNAL(timeout()),
            SLOT(triggerFunctionAutoComplete()));

    const Cell cell(d->selection->activeSheet(), d->selection->marker());
    const bool wrapText = cell.style().wrapText();
    d->textEdit->setWordWrapMode(wrapText ? QTextOption::WordWrap: QTextOption::NoWrap);

#if 0 // FIXME Implement a completion aware KTextEdit.
  setCompletionMode(selection()->view()->doc()->completionMode());
  setCompletionObject(&selection()->view()->doc()->map()->stringCompletion(), true);
#endif

  connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT (slotCursorPositionChanged()));
  connect( this, SIGNAL( textChanged() ), this, SLOT( slotTextChanged() ) );
// connect( this, SIGNAL(completionModeChanged( KGlobalSettings::Completion )),this,SLOT (slotCompletionModeChanged(KGlobalSettings::Completion)));
}

CellEditor::~CellEditor()
{
    if (selection())
        selection()->endReferenceSelection();

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

void CellEditor::checkFunctionAutoComplete()
{
    // Nothing to do, if no focus or not in reference selection mode.
    if (!hasFocus() || !selection()->referenceSelection()) {
        return;
    }

    d->functionCompletionTimer->stop();
    d->functionCompletionTimer->setSingleShot(true);
    d->functionCompletionTimer->start(2000);
}

void CellEditor::triggerFunctionAutoComplete()
{
    // tokenize the expression (don't worry, this is very fast)
  int curPos = textCursor().position();
  QString subtext = toPlainText().left( curPos );

    KSpread::Formula f;
    KSpread::Tokens tokens = f.scan(subtext);
    if (!tokens.valid()) return;
    if (tokens.count() < 1) return;

    KSpread::Token lastToken = tokens[ tokens.count()-1 ];

    // last token must be an identifier
    if (!lastToken.isIdentifier()) return;
    QString id = lastToken.text();
    if (id.length() < 1) return;

    // find matches in function names
    QStringList fnames = KSpread::FunctionRepository::self()->functionNames();
    QStringList choices;
    for (int i = 0; i < fnames.count(); i++)
        if (fnames[i].startsWith(id, Qt::CaseInsensitive))
            choices.append(fnames[i]);
    choices.sort();

    // no match, don't bother with completion
    if (!choices.count()) return;

    // single perfect match, no need to give choices
    if (choices.count() == 1)
        if (choices[0].toLower() == id.toLower())
            return;

    // present the user with completion choices
    d->functionCompletion->showCompletion(choices);
}

void CellEditor::functionAutoComplete(const QString& item)
{
    if (item.isEmpty()) return;

  QTextCursor textCursor = this->textCursor();
    int curPos = textCursor.position();
  QString subtext = toPlainText().left( curPos );

    KSpread::Formula f;
    KSpread::Tokens tokens = f.scan(subtext);
    if (!tokens.valid()) return;
    if (tokens.count() < 1) return;

    KSpread::Token lastToken = tokens[ tokens.count()-1 ];
    if (!lastToken.isIdentifier()) return;

  blockSignals( true );
    // Select the incomplete function name in order to replace it.
    textCursor.setPosition(lastToken.pos() + 1);
    textCursor.setPosition(lastToken.pos() + lastToken.text().length() + 1,
                           QTextCursor::KeepAnchor);
  setTextCursor( textCursor );
  blockSignals( false );
    // Replace the incomplete function name with the selected one.
    insertPlainText(item);
}

void CellEditor::slotCursorPositionChanged()
{
    // Nothing to do, if not in reference selection mode.
    if (!selection()->referenceSelection()) {
        return;
    }
    // NOTE On text changes KTextEdit::cursorPositionChanged() is triggered
    // before KTextEdit::textChanged(). The text is already up-to-date.

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

void CellEditor::slotCompletionModeChanged(KGlobalSettings::Completion _completion)
{
    selection()->activeSheet()->map()->settings()->setCompletionMode(_completion);
}

void CellEditor::slotTextChanged()
{
    // NOTE On text changes KTextEdit::cursorPositionChanged() is triggered
    // before KTextEdit::textChanged().

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
#if 0 // KSPREAD_WIP_EDITOR_OVERHAUL
    const Cell cell(d->selection->activeSheet(), d->selection->marker());
    if ((cell.style().formatType()) == Format::Percentage) {
        if ((text.length() == 1) && text[0].isDigit()) {
            setPlainText(text + " %");
            setCursorPosition(1);
            return;
        }
    }
#endif // KSPREAD_WIP_EDITOR_OVERHAUL

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
            if (d->captureAllKeyEvents) {
                break; // pass to KTextEdit
            }
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
    KTextEdit::keyPressEvent(event);
}

void CellEditor::focusInEvent(QFocusEvent *event)
{
    // If the focussing is user induced.
    if (event->reason() != Qt::OtherFocusReason) {
        kDebug() << "induced by user";
        d->cellTool->setLastEditorWithFocus(CellToolBase::EmbeddedEditor);
    }
    KTextEdit::focusInEvent(event);
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
    QTextCursor textCursor( this->textCursor() );
    textCursor.setPosition(pos);
    setTextCursor( textCursor );
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


/*****************************************************************************
 *
 * LocationComboBox
 *
 ****************************************************************************/

LocationComboBox::LocationComboBox(CellToolBase *cellTool, QWidget *_parent)
    : KComboBox(true, _parent)
    , m_cellTool(cellTool)
{
    setCompletionObject(&completionList, true);
    setCompletionMode(KGlobalSettings::CompletionAuto);

    insertItem(0, QString());
    updateAddress();
    Map *const map = cellTool->selection()->activeSheet()->map();
    const QList<QString> areaNames = map->namedAreaManager()->areaNames();
    for (int i = 0; i < areaNames.count(); ++i)
        slotAddAreaName(areaNames[i]);

    connect(this, SIGNAL(activated(const QString &)),
            this, SLOT(slotActivateItem()));
}

void LocationComboBox::updateAddress()
{
    QString address;
    Selection *const selection = m_cellTool->selection();
    const QList< QPair<QRectF, QString> > names = selection->activeSheet()->cellStorage()->namedAreas(*selection);
    {
        QRect range;
        if (selection->isSingular()) range = QRect(selection->marker(), QSize(1, 1));
        else range = selection->lastRange();
        for (int i = 0; i < names.size(); i++) {
            if (names[i].first.toRect() == range) {
                address = names[i].second;
            }
        }
    }
    if (selection->activeSheet()->getLcMode()) {
        if (selection->isSingular()) {
            address = 'L' + QString::number(selection->marker().y()) +
                      'C' + QString::number(selection->marker().x());
        } else {
            const QRect lastRange = selection->lastRange();
            address = QString::number(lastRange.height()) + "Lx";
            address += QString::number(lastRange.width()) + 'C';
        }
    } else {
        address = selection->name();
    }
    setItemText(0, address);
    setCurrentItem(0);
    lineEdit()->setText(address);
}

void LocationComboBox::slotAddAreaName(const QString &_name)
{
    insertItem(count(), _name);
    addCompletionItem(_name);
}

void LocationComboBox::slotRemoveAreaName(const QString &_name)
{
    for (int i = 0; i < count(); i++) {
        if (itemText(i) == _name) {
            removeItem(i);
            break;
        }
    }
    removeCompletionItem(_name);
}

void LocationComboBox::addCompletionItem( const QString &_item )
{
    if (completionList.items().contains(_item) == 0) {
        completionList.addItem(_item);
        kDebug(36005) << _item;
    }
}

void LocationComboBox::removeCompletionItem( const QString &_item )
{
    completionList.removeItem(_item);
}

void LocationComboBox::slotActivateItem()
{
    if (activateItem()) {
        m_cellTool->scrollToCell(m_cellTool->selection()->cursor());
    }
}

bool LocationComboBox::activateItem()
{
    Selection *const selection = m_cellTool->selection();

    // Set the focus back on the canvas.
    parentWidget()->setFocus();

    const QString text = lineEdit()->text();
    // check whether an already existing named area was entered
    Region region = selection->activeSheet()->map()->namedAreaManager()->namedArea(text);
    if (region.isValid()) {
        // TODO Stefan: Merge the sheet change into Selection.
        if (region.firstSheet() != selection->activeSheet()) {
            selection->emitVisibleSheetRequested(region.firstSheet());
        }
        selection->initialize(region);
        return true;
    }

    // check whether a valid cell region was entered
    region = Region(text, selection->activeSheet()->map(), selection->activeSheet());
    if (region.isValid()) {
        // TODO Stefan: Merge the sheet change into Selection.
        if (region.firstSheet() != selection->activeSheet()) {
            selection->emitVisibleSheetRequested(region.firstSheet());
        }
        selection->initialize(region);
        return true;
    }

    // A name for an area entered?
    // FIXME Stefan: allow all characters
    bool validName = true;
    for (int i = 0; i < text.length(); ++i) {
        if (!text[i].isLetter()) {
            validName = false;
            break;
        }
    }
    if (validName) {
        NamedAreaCommand* command = new NamedAreaCommand();
        command->setSheet(selection->activeSheet());
        command->setAreaName(text);
        command->add(Region(selection->lastRange(), selection->activeSheet()));
        if (command->execute())
            return true;
        else
            delete command;
    }
    return false;
}


void LocationComboBox::keyPressEvent( QKeyEvent * _ev )
{
    Selection *const selection = m_cellTool->selection();

    // Do not handle special keys and accelerators. This is
    // done by KComboBox.
    if (_ev->modifiers() & (Qt::AltModifier | Qt::ControlModifier)) {
        KComboBox::keyPressEvent(_ev);
        // Never allow that keys are passed on to the parent.
        _ev->accept(); // QKeyEvent

        return;
    }

    // Handle some special keys here. Eve
    switch (_ev->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter: {
        if (activateItem()) {
            m_cellTool->scrollToCell(selection->cursor());
            return;
        }
        _ev->accept(); // QKeyEvent
    }
    break;
    // Escape pressed, restore original value
    case Qt::Key_Escape:
        updateAddress();
        parentWidget()->setFocus();
        _ev->accept(); // QKeyEvent
        break;
    default:
        KComboBox::keyPressEvent(_ev);
        // Never allow that keys are passed on to the parent.
        _ev->accept(); // QKeyEvent
    }
}


/****************************************************************
 *
 * ExternalEditor
 * The editor that appears in the tool option widget and allows
 * to edit the cell's content.
 *
 ****************************************************************/

class ExternalEditor::Private
{
public:
    CellToolBase* cellTool;
    FormulaEditorHighlighter* highlighter;
    bool isArray;
};

ExternalEditor::ExternalEditor(QWidget *parent)
        : KTextEdit(parent)
        , d(new Private)
{
    d->cellTool = 0;
    d->highlighter = 0;
    d->isArray = false;

    setCurrentFont(KGlobalSettings::generalFont());

    // Try to immitate KLineEdit regarding the margins and size.
    document()->setDocumentMargin(1);
    setMinimumHeight(fontMetrics().height() + 2 * frameWidth() + 1);

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(slotCursorPositionChanged()));
}

ExternalEditor::~ExternalEditor()
{
    delete d->highlighter;
    delete d;
}

QSize ExternalEditor::sizeHint() const
{
    return KTextEdit::sizeHint(); // document()->size().toSize();
}

void ExternalEditor::setCellTool(CellToolBase* cellTool)
{
    d->cellTool = cellTool;
    d->highlighter = new FormulaEditorHighlighter(this, cellTool->selection());
}

void ExternalEditor::applyChanges()
{
    Q_ASSERT(d->cellTool);
    d->cellTool->deleteEditor(true, d->isArray); // save changes
    d->isArray = false;
}

void ExternalEditor::discardChanges()
{
    Q_ASSERT(d->cellTool);
    clear();
    d->cellTool->deleteEditor(false); // discard changes
    d->cellTool->selection()->update();
}

void ExternalEditor::setText(const QString &text)
{
    Q_ASSERT(d->cellTool);
    if (toPlainText() == text) {
        return;
    }
    // This method is called from the embedded editor. Do not send signals back.
    blockSignals(true);
    KTextEdit::setPlainText(text);
    QTextCursor textCursor = this->textCursor();
    textCursor.setPosition(d->cellTool->editor()->cursorPosition());
    setTextCursor(textCursor);
    blockSignals(false);
}

void ExternalEditor::keyPressEvent(QKeyEvent *event)
{
    Q_ASSERT(d->cellTool);
    if (!d->cellTool->selection()->activeSheet()->map()->isReadWrite()) {
        return;
    }

    // Create the embedded editor, if necessary.
    if (!d->cellTool->editor()) {
        d->cellTool->createEditor(false /* keep content */, false /* no focus */);
    }

    // the Enter and Esc key are handled by the embedded editor
    if ((event->key() == Qt::Key_Return) || (event->key() == Qt::Key_Enter) ||
            (event->key() == Qt::Key_Escape)) {
        d->cellTool->editor()->setFocus();
        QApplication::sendEvent(d->cellTool->editor(), event);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_F2) {
        // Switch the focus back to the embedded editor.
        d->cellTool->editor()->setFocus();
    }
    // call inherited handler
    KTextEdit::keyPressEvent(event);
}

void ExternalEditor::focusInEvent(QFocusEvent* event)
{
    Q_ASSERT(d->cellTool);
    // If the focussing is user induced.
    if (event->reason() != Qt::OtherFocusReason) {
        kDebug() << "induced by user";
        d->cellTool->setLastEditorWithFocus(CellToolBase::ExternalEditor);
    }
    // when the external editor gets focus, create also the internal editor
    // this in turn means that ranges will be instantly highlighted right
    if (!d->cellTool->editor())
        d->cellTool->createEditor(false /* keep content */, false /* no focus */);
    KTextEdit::focusInEvent(event);
}

void ExternalEditor::focusOutEvent(QFocusEvent* event)
{
    Q_ASSERT(d->cellTool);
    KTextEdit::focusOutEvent(event);
}

void ExternalEditor::slotTextChanged()
{
    if (!hasFocus()) return;  // only report change if we have focus
    emit textChanged(toPlainText());
    // Update the cursor position again, because this slot is invoked after
    // slotCursorPositionChanged().
    if (d->cellTool->editor()) {
        d->cellTool->editor()->setCursorPosition(textCursor().position());
    }
}

void ExternalEditor::slotCursorPositionChanged()
{
    if (!hasFocus() || !d->cellTool->editor()) {
        return;
    }
    // Suppress updates, if this slot got invoked by a text change. It is done
    // later by slotTextChanged().
    if (d->cellTool->editor()->toPlainText() == toPlainText()) {
        d->cellTool->editor()->setCursorPosition(textCursor().position());
    }
}

#if 0 // KSPREAD_DISCARD_FORMULA_BAR
/****************************************************************
 *
 * EditWidget
 * The line-editor that appears above the sheet and allows to
 * edit the cells content.
 *
 ****************************************************************/

EditWidget::EditWidget(QWidget *_parent, Canvas *_canvas,
                       QAbstractButton *cancelButton, QAbstractButton *okButton)
        : KLineEdit(_parent)
{
    m_pCanvas = _canvas;
    Q_ASSERT(m_pCanvas != 0);
    // Those buttons are created by the caller, so that they are inserted
    // properly in the layout - but they are then managed here.
    m_pCancelButton = cancelButton;
    m_pOkButton = okButton;
    m_isArray = false;

    installEventFilter(m_pCanvas);

    if (!m_pCanvas->doc()->isReadWrite() || !m_pCanvas->activeSheet())
        setEnabled(false);

    QObject::connect(m_pCancelButton, SIGNAL(clicked()),
                     this, SLOT(slotAbortEdit()));
    QObject::connect(m_pOkButton, SIGNAL(clicked()),
                     this, SLOT(slotDoneEdit()));

    setEditMode(false);   // disable buttons

    setCompletionMode(m_pCanvas->doc()->completionMode());
    setCompletionObject(&m_pCanvas->doc()->map()->stringCompletion(), true);
}

void EditWidget::showEditWidget(bool _show)
{
    if (_show) {
        m_pCancelButton->show();
        m_pOkButton->show();
        show();
    } else {
        m_pCancelButton->hide();
        m_pOkButton->hide();
        hide();
    }
}

void EditWidget::slotAbortEdit()
{
    m_pCanvas->deleteEditor(false /*discard changes*/);
    // will take care of the buttons
}

void EditWidget::slotDoneEdit()
{
    m_pCanvas->deleteEditor(true /*keep changes*/, m_isArray);
    m_isArray = false;
    // will take care of the buttons
}

void EditWidget::keyPressEvent(QKeyEvent* _ev)
{
    // Don't handle special keys and accelerators, except Enter ones
    if (((_ev->modifiers() & (Qt::AltModifier | Qt::ControlModifier))
            || (_ev->modifiers() & Qt::ShiftModifier)
            || (_ev->key() == Qt::Key_Shift)
            || (_ev->key() == Qt::Key_Control))
            && (_ev->key() != Qt::Key_Return) && (_ev->key() != Qt::Key_Enter)) {
        KLineEdit::keyPressEvent(_ev);
        _ev->accept();
        return;
    }

    if (!m_pCanvas->doc()->isReadWrite())
        return;

    if (!m_pCanvas->editor()) {
        // Start editing the current cell
        m_pCanvas->createEditor(true /* clear content */, false /* no focus */);
    }
    CellEditor * cellEditor = (CellEditor*) m_pCanvas->editor();

    switch (_ev->key()) {
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        cellEditor->setText(text());
        // Don't allow to start a chooser when pressing the arrow keys
        // in this widget, since only up and down would work anyway.
        // This is why we call slotDoneEdit now, instead of sending
        // to the canvas.
        //QApplication::sendEvent( m_pCanvas, _ev );
        m_isArray = (_ev->modifiers() & Qt::AltModifier) &&
                    (_ev->modifiers() & Qt::ControlModifier);
        slotDoneEdit();
        _ev->accept();
        break;
    case Qt::Key_F2:
        cellEditor->setFocus();
        cellEditor->setText(text());
        cellEditor->setCursorPosition(cursorPosition());
        break;
    default:

        KLineEdit::keyPressEvent(_ev);

        setFocus();
        cellEditor->setCheckChoice(false);
        cellEditor->setText(text());
        cellEditor->setCheckChoice(true);
        cellEditor->setCursorPosition(cursorPosition());
    }
}

void EditWidget::setEditMode(bool mode)
{
    m_pCancelButton->setEnabled(mode);
    m_pOkButton->setEnabled(mode);
}

void EditWidget::focusOutEvent(QFocusEvent* ev)
{
    //kDebug(36001) <<"EditWidget lost focus";
    // See comment about setLastEditorWithFocus
    m_pCanvas->setLastEditorWithFocus(Canvas::EditWidget);

    KLineEdit::focusOutEvent(ev);
}

void EditWidget::setText(const QString& t)
{
    if (t == text())   // Why this? (David)
        return;

    KLineEdit::setText(t);
}
#endif



/*****************************************************************************
 *
 * RegionSelector
 *
 ****************************************************************************/

class RegionSelector::Private
{
public:
    Selection* selection;
    QDialog* parentDialog;
    KDialog* dialog;
    KTextEdit* textEdit;
    QToolButton* button;
    FormulaEditorHighlighter* highlighter;
    DisplayMode displayMode;
    SelectionMode selectionMode;
    static RegionSelector* s_focussedSelector;
};

RegionSelector* RegionSelector::Private::s_focussedSelector = 0;

RegionSelector::RegionSelector(QWidget* parent)
        : QWidget(parent),
        d(new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    d->displayMode = Widget;
    d->parentDialog = 0;
    d->selection = 0;
    d->dialog = 0;
    d->button = new QToolButton(this);
    d->button->setCheckable(true);
    d->button->setIcon(KIcon("selection"));
    d->highlighter = 0;
    d->textEdit = new KTextEdit(this);
    d->textEdit->setLineWrapMode(QTextEdit::NoWrap);
    d->textEdit->setWordWrapMode(QTextOption::NoWrap);
    d->textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    d->textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->textEdit->setFixedHeight(d->button->height() - 2*d->textEdit->frameWidth());   // FIXME
    d->textEdit->setTabChangesFocus(true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(2);
    layout->addWidget(d->textEdit);
    layout->addWidget(d->button);

    d->button->installEventFilter(this);
    d->textEdit->installEventFilter(this);
    connect(d->button, SIGNAL(toggled(bool)),
            this, SLOT(switchDisplayMode(bool)));
}

RegionSelector::~RegionSelector()
{
    d->selection->endReferenceSelection();
  d->selection->setSelectionMode(Selection::MultipleCells);
    delete d;
}

void RegionSelector::setSelectionMode(SelectionMode mode)
{
    d->selectionMode = mode;
    // TODO adjust selection
}

void RegionSelector::setSelection(Selection* selection)
{
    d->selection = selection;
    d->highlighter = new FormulaEditorHighlighter(d->textEdit, d->selection);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(choiceChanged()));
}

void RegionSelector::setDialog(QDialog* dialog)
{
    d->parentDialog = dialog;
}

KTextEdit* RegionSelector::textEdit() const
{
    return d->textEdit;
}

bool RegionSelector::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Close) {
        if (object == d->dialog  && d->button->isChecked()) {
            // TODO Stefan: handle as button click
//       d->button->toggle();
            event->ignore();
            return true; // eat it
        }
    } else if (event->type() == QEvent::FocusIn) {
        Private::s_focussedSelector = this;
        d->selection->startReferenceSelection();
        if (d->selectionMode == SingleCell) {
            d->selection->setSelectionMode(Selection::SingleCell);
        } else {
            d->selection->setSelectionMode(Selection::MultipleCells);
        }
        // TODO Stefan: initialize choice
    }
    return QObject::eventFilter(object, event);
}

void RegionSelector::switchDisplayMode(bool state)
{
    Q_UNUSED(state)
    kDebug() ;

    if (d->displayMode == Widget) {
        d->displayMode = Dialog;

        d->dialog = new KDialog(d->parentDialog->parentWidget(), Qt::Tool);
        d->dialog->resize(d->parentDialog->width(), 20);
        d->dialog->move(d->parentDialog->pos());
        d->dialog->setButtons(0);
        d->dialog->setModal(false);

        if (d->selectionMode == SingleCell) {
            d->dialog->setCaption(i18n("Select Single Cell"));
        } else { // if ( d->selectionMode == MultipleCells )
            d->dialog->setCaption(i18n("Select Multiple Cells"));
        }

        QWidget* widget = new QWidget(d->dialog);
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(d->textEdit);
        layout->addWidget(d->button);

        d->dialog->setMainWidget(widget);
        d->dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        d->dialog->installEventFilter(this);
        d->dialog->show();
        d->parentDialog->hide();
    } else {
        d->displayMode = Widget;

        layout()->addWidget(d->textEdit);
        layout()->addWidget(d->button);

        d->parentDialog->move(d->dialog->pos());
        d->parentDialog->show();
        delete d->dialog;
        d->dialog = 0;
    }
}

void RegionSelector::choiceChanged()
{
    if (Private::s_focussedSelector != this)
        return;

    if (d->selection->isValid()) {
        QString area = d->selection->name();
        d->textEdit->setPlainText(area);
    }
}

#include "Editors.moc"
