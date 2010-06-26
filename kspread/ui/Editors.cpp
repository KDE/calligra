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

            Region newRange(token.text());

            if (!alreadyFoundRanges.contains(newRange.name())) {
                alreadyFoundRanges.append(newRange.name());
                d->rangeCount++;
            }
            setFormat(token.pos() + 1, token.text().length(), colors[ alreadyFoundRanges.indexOf(newRange.name()) % colors.size()]);
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

    connect(d->completionListBox, SIGNAL(activated(QModelIndex)), SLOT(doneCompletion()));
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

    QString helpText = desc->helpText().isEmpty() ? QString() : desc->helpText()[0];
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
    Cell                      cell;
    Selection*                selection;
    KTextEdit*                textEdit;
    FormulaEditorHighlighter* highlighter;
    FunctionCompletion*       functionCompletion;
    QTimer*                   functionCompletionTimer;

    QPoint globalCursorPos;

bool captureAllKeyEvents : 1;
bool checkChoice         : 1;
bool updateChoice        : 1;
bool updatingChoice      : 1;

    int length;
    uint fontLength;
    uint length_namecell;
    uint length_text;
    int currentToken;
    uint rangeCount;
};


CellEditor::CellEditor(QWidget* parent, Selection* selection, bool captureAllKeyEvents)
        : QWidget(parent)
        , d(new Private)
{
    d->cell = Cell(selection->activeSheet(), selection->marker());
    d->selection = selection;
    d->textEdit = new KTextEdit(this);
    d->globalCursorPos = QPoint();
    d->captureAllKeyEvents = captureAllKeyEvents;
    d->checkChoice = true;
    d->updateChoice = true;
    d->updatingChoice = false;
    d->length = 0;
    d->fontLength = 0;
    d->length_namecell = 0;
    d->length_text = 0;
    d->currentToken = 0;
    d->rangeCount = 0;

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(d->textEdit);

    d->textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->textEdit->setFrameStyle(QFrame::NoFrame);
    d->textEdit->setLineWidth(0);
    d->textEdit->installEventFilter(this);

    d->highlighter = new FormulaEditorHighlighter(d->textEdit, selection);

    d->functionCompletion = new FunctionCompletion(this);
    d->functionCompletionTimer = new QTimer(this);
    connect(d->functionCompletion, SIGNAL(selectedCompletion(const QString&)),
            SLOT(functionAutoComplete(const QString&)));
    connect(d->textEdit, SIGNAL(textChanged()), SLOT(checkFunctionAutoComplete()));
    connect(d->functionCompletionTimer, SIGNAL(timeout()),
            SLOT(triggerFunctionAutoComplete()));

    if (!d->cell.style().wrapText())
        d->textEdit->setWordWrapMode(QTextOption::NoWrap);
    else
        d->textEdit->setWordWrapMode(QTextOption::WordWrap);

#if 0 // FIXME Implement a completion aware KTextEdit.
    d->textEdit->setCompletionMode(selection()->view()->doc()->completionMode());
    d->textEdit->setCompletionObject(&selection()->view()->doc()->map()->stringCompletion(), true);
#endif

    setFocusProxy(d->textEdit);

    connect(d->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
    connect(d->textEdit, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
    connect(d->textEdit->document(), SIGNAL(modificationChanged(bool)),
            this, SIGNAL(modificationChanged(bool)));
// connect( d->textEdit, SIGNAL(completionModeChanged( KGlobalSettings::Completion )),this,SLOT (slotCompletionModeChanged(KGlobalSettings::Completion)));

    // A choose should always start at the edited cell
//  selection()->setChooseMarkerRow( selection()->d->selection->marker().y() );
//  selection()->setChooseMarkerColumn( selection()->d->selection->marker().x() );

    connect(d->selection, SIGNAL(destroyed()), SLOT(slotSelectionDestroyed()));
}

CellEditor::~CellEditor()
{
    if (selection())
        selection()->endReferenceSelection();

    delete d->highlighter;
    delete d->functionCompletion;
    delete d->functionCompletionTimer;
    delete d;
}

const Cell& CellEditor::cell() const
{
    return d->cell;
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
    d->functionCompletionTimer->stop();
    d->functionCompletionTimer->setSingleShot(true);
    d->functionCompletionTimer->start(2000);
}

void CellEditor::triggerFunctionAutoComplete()
{
    // tokenize the expression (don't worry, this is very fast)
    int curPos = d->textEdit->textCursor().position();
    QString subtext = d->textEdit->toPlainText().left(curPos);

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

    QTextCursor textCursor = d->textEdit->textCursor();
    int curPos = textCursor.position();
    QString subtext = text().left(curPos);

    KSpread::Formula f;
    KSpread::Tokens tokens = f.scan(subtext);
    if (!tokens.valid()) return;
    if (tokens.count() < 1) return;

    KSpread::Token lastToken = tokens[ tokens.count()-1 ];
    if (!lastToken.isIdentifier()) return;

    d->textEdit->blockSignals(true);
    textCursor.setPosition(lastToken.pos() + 1);
    textCursor.setPosition(lastToken.pos() + lastToken.text().length() + 1,
                           QTextCursor::KeepAnchor);
    d->textEdit->setTextCursor(textCursor);
    d->textEdit->insertPlainText(item);
    d->textEdit->blockSignals(false);
    // call slotTextChanged to sync the text between the inline editor and the editor in the docker
    slotTextChanged();
}

void CellEditor::slotCursorPositionChanged()
{
    // kDebug() <<"position:" << cursorPosition()<<endl;;

    // TODO Stefan: optimize this function!

    // turn choose mode on/off
    if (!checkChoice())
        return;

    d->globalCursorPos = d->textEdit->mapToGlobal(d->textEdit->cursorRect().topLeft());

    d->highlighter->rehighlight();

    Tokens tokens = d->highlighter->formulaTokens();
    uint rangeCounter = 0;
    uint currentRange = 0;
    int regionStart = -1;
    uint regionEnd = 0;
    bool lastWasASemicolon = false;
    d->currentToken = 0;
    uint rangeCount = d->highlighter->rangeCount();
    d->rangeCount = rangeCount;

    Token token;
    Token::Type type;
    // search the current token
    // determine the subregion number, btw
    for (int i = 0; i < tokens.count(); ++i) {
        if (tokens[i].pos() >= d->textEdit->textCursor().position() - 1) { // without '='
            /*      kDebug() <<"token.pos >= cursor.pos";*/
            type = tokens[i].type();
            if (type == Token::Cell || type == Token::Range) {
                if (lastWasASemicolon) {
                    regionEnd = rangeCounter++;
                    lastWasASemicolon = false;
                    continue;
                }
            }
            if (type == Token::Operator && tokens[i].asOperator() == Token::Semicolon) {
                lastWasASemicolon = true;
                continue;
            }
            lastWasASemicolon = false;
            break;
        }
        token = tokens[i];
        d->currentToken = i;

        type = token.type();
        if (type == Token::Cell || type == Token::Range) {
            if (!lastWasASemicolon) {
                regionStart = rangeCounter;
            }
            regionEnd = rangeCounter;
            currentRange = rangeCounter++;
        }
        // semicolons are use as deliminiters in regions
        if (type == Token::Operator) {
            if (token.asOperator() == Token::Semicolon) {
                lastWasASemicolon = true;
            } else {
                lastWasASemicolon = false;
                // set the region start to the next element
                regionStart = currentRange + 1;
                regionEnd = regionStart - 1; // len = 0
            }
        }
    }

//   kDebug() <<"regionStart =" << regionStart/* << endl*/
//             << ", regionEnd = " << regionEnd/* << endl*/
//             << ", currentRange = " << currentRange << endl;

    // only change the active sub region, if we have found one.
    if (regionStart != -1) {
        selection()->setActiveElement(currentRange);
        selection()->setActiveSubRegion(regionStart, regionEnd - regionStart + 1);
    }

    // triggered by keyboard action?
    if (!d->updatingChoice) {
        if (d->highlighter->rangeChanged()) {
            d->highlighter->resetRangeChanged();

            //selection()->blockSignals(true);
            setUpdateChoice(false);

            Tokens tokens = d->highlighter->formulaTokens();
            selection()->update(); // set the old one dirty
            selection()->clear();
            Region tmpRegion;
            Region::ConstIterator it;

            //A list of regions which have already been highlighted on the spreadsheet.
            //This is so that we don't end up highlighting the same region twice in two different
            //colors.
            QLinkedList<Region> alreadyUsedRegions;

            for (int i = 0; i < tokens.count(); ++i) {
                Token token = tokens[i];
                Token::Type type = token.type();
                if (type == Token::Cell || type == Token::Range) {
                    Region region(token.text(), selection()->activeSheet()->map(), selection()->activeSheet());
                    it = region.constBegin();

                    if (it != region.constEnd() && !alreadyUsedRegions.contains(region)) {
                        QRect r = (*it)->rect();

                        if (selection()->isEmpty())
                            selection()->initialize((*it)->rect(), (*it)->sheet());
                        else
                            selection()->extend((*it)->rect(), (*it)->sheet());

                        alreadyUsedRegions.append(region);
                    }
                }
            }
            setUpdateChoice(true);
            //selection()->blockSignals(false);
        }
    }
}

void CellEditor::cut()
{
    d->textEdit->cut();
}

void CellEditor::paste()
{
    d->textEdit->paste();
}

void CellEditor::copy()
{
    d->textEdit->copy();
}

void CellEditor::setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter)
{
    const qreal scaleY = POINT_TO_INCH(static_cast<qreal>((KoDpi::dpiY())));
    d->textEdit->setFont(QFont(font.family(), viewConverter->documentToViewY(font.pointSizeF()) / scaleY));

    if (updateSize) {
        QFontMetrics fontMetrics(d->textEdit->font());
        d->fontLength = 0.0; // fontMetrics.width('x');
        int width = fontMetrics.width(d->textEdit->toPlainText()) + d->fontLength;
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
    // Fix the position.
    d->textEdit->verticalScrollBar()->setValue(4);

    //FIXME - text() may return richtext?
    QString t = text();

    if (t.length() > d->length) {
        d->length = t.length();

        QFontMetrics fm(d->textEdit->font());
        // - requiredWidth = width of text plus some spacer characters
        int requiredWidth = fm.width(t) + (2 * fm.width('x'));

        //For normal single-row cells, the text editor must be expanded horizontally to
        //allow the text to fit if the new text is too wide
        //For multi-row (word-wrap enabled) cells, the text editor must expand vertically to
        //allow for new rows of text & the width of the text editor is not affected
        if (d->textEdit->wordWrapMode() == QTextOption::NoWrap) {
            if (requiredWidth > width()) {
                if (t.isRightToLeft()) {
                    setGeometry(x() - requiredWidth + width(), y(), requiredWidth, height());
                } else {
                    setGeometry(x(), y(), requiredWidth, height());
                }
            }
        } else {
            int requiredHeight = d->textEdit->heightForWidth(width());

            if (requiredHeight > height()) {
                setGeometry(x(), y(), width(), requiredHeight);
            }
        }

        /* // allocate more space than needed. Otherwise it might be too slow
         d->length = t.length();

         // Too slow for long texts
         // QFontMetrics fm( d->textEdit->font() );
         //  int mw = fm.width( t ) + fm.width('x');
         int mw = d->fontLength * d->length;

         if (mw < width())
           mw = width();

         if (t.isRightToLeft())
           setGeometry(x() - mw + width(), y(), mw, height());
         else
           setGeometry(x(), y(), mw, height());

         d->length -= 2; */
    }

    if ((cell().style().formatType()) == Format::Percentage) {
        if ((t.length() == 1) && t[0].isDigit()) {
            QString tmp = t + " %";
            d->textEdit->setPlainText(tmp);
            setCursorPosition(1);
            return;
        }
    }

    if (hasFocus())  // update the external editor, but only if we have focus
        emit textChanged(d->textEdit->toPlainText());
    // selection()->view()->editWidget()->textCursor().setPosition( d->textEdit->cursorPosition() );
}

void CellEditor::setCheckChoice(bool state)
{
    d->checkChoice = state;
}

bool CellEditor::checkChoice()
{
    if (!d->checkChoice)
        return false;

//   // prevent recursion
//   d->checkChoice = false; // TODO nescessary?

    d->length_namecell = 0;
    d->currentToken = 0;

    QString text = d->textEdit->toPlainText();
    if (text[0] != '=') {
        selection()->setReferenceSelectionMode(false);
//    d->checkChoice = true;
        return true;
    }

    // switch to reference selection mode if we haven't yet
    selection()->startReferenceSelection();

    // if we don't have focus, we show highlighting, but don't do the rest
    if (!hasFocus()) {
        selection()->setReferenceSelectionMode(false);
//    d->checkChoice = true;
        return true;
    }

    int cur = d->textEdit->textCursor().position();

    Tokens tokens = d->highlighter->formulaTokens();

    if (tokens.count()) { // formula not empty?
        Token token;
        for (int i = 0; i < tokens.count(); ++i) {
            if (tokens[i].pos() >= cur - 1) { // without '='
                break;
            }
            token = tokens[i];
            d->currentToken = i;
        }

        Token::Type type = token.type();
        if (type == Token::Operator && token.asOperator() != Token::RightPar) {
            selection()->setReferenceSelectionMode(true);
        } else if (type == Token::Cell || type == Token::Range) {
            d->length_namecell = token.text().length();
            selection()->setReferenceSelectionMode(true);
        } else {
            selection()->setReferenceSelectionMode(false);
        }
    }

//   d->checkChoice = true;

    return true;
}

void CellEditor::setUpdateChoice(bool state)
{
    d->updateChoice = state;
}

void CellEditor::updateChoice()
{
    if (!d->updateChoice)
        return;

    // prevent recursion
    d->updatingChoice = true;

    Selection* choice = selection();

    if (choice->isEmpty())
        return;

    if (!choice->activeElement())
        return;

    QString name_cell = choice->activeSubRegionName();

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
            } else {
                start = token.pos() + token.text().length() + 1;
            }
        } else {
            // sanitize
            d->currentToken = 0;
        }
    }

    d->length_namecell = name_cell.length();
    d->length_text = text().length();

    QString oldText = text();
    QString newText = oldText.left(start) + name_cell + oldText.right(d->length_text - start - length);

    setCheckChoice(false);
    setText(newText);
    setCheckChoice(true);
    setCursorPosition(start + d->length_namecell);

    emit textChanged(d->textEdit->toPlainText());

    d->updatingChoice = false;
}

void CellEditor::resizeEvent(QResizeEvent*)
{
    d->textEdit->setGeometry(0, 0, width(), height());
}

void CellEditor::handleKeyPressEvent(QKeyEvent * _ev)
{
    if (_ev->key() == Qt::Key_F4) {
        if (d->textEdit == 0) {
            QApplication::sendEvent(d->textEdit, _ev);
            return;
        }

        QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$");

        int cur = d->textEdit->textCursor().position();
        QString tmp, tmp2;
        int n = -1;

        // this is ugly, and sort of hack
        // FIXME rewrite to use the real Tokenizer
        unsigned i;
        for (i = 0; i < 10; i++) {
            tmp =  d->textEdit->toPlainText().left(cur + i);
            tmp2 = d->textEdit->toPlainText().right(d->textEdit->toPlainText().length() - cur - i);

            n = exp.indexIn(tmp);
            if (n >= 0) break;
        }

        if (n == -1) return;

        QString newPart;
        if ((exp.cap(1) == "$") && (exp.cap(3) == "$"))
            newPart = '$' + exp.cap(2) + exp.cap(4);
        else if ((exp.cap(1) != "$") && (exp.cap(3) != "$"))
            newPart = '$' + exp.cap(2) + '$' + exp.cap(4);
        else if ((exp.cap(1) == "$") && (exp.cap(3) != "$"))
            newPart = exp.cap(2) + '$' + exp.cap(4);
        else if ((exp.cap(1) != "$") && (exp.cap(3) == "$"))
            newPart = exp.cap(2) + exp.cap(4);

        QString newString = tmp.left(n);
        newString += newPart;
        cur = newString.length() - i;
        newString += tmp2;

        d->textEdit->setPlainText(newString);
        setCursorPosition(cur);

        _ev->accept(); // QKeyEvent

        return;
    }

    // Send the key event to the KLineEdit
    QApplication::sendEvent(d->textEdit, _ev);
}

void CellEditor::handleInputMethodEvent(QInputMethodEvent  * _ev)
{
    // send the IM event to the KLineEdit
    QApplication::sendEvent(d->textEdit, _ev);
}

QString CellEditor::text() const
{
    return d->textEdit->toPlainText();
}

void CellEditor::setText(const QString& text, int cursorPos)
{
    if (text == d->textEdit->toPlainText()) return;

    d->textEdit->setPlainText(text);
    //Usability : It is usually more convenient if the cursor is positioned at the end of the text so it can
    //be quickly deleted using the backspace key

    // This also ensures that the caret is sized correctly for the text
    if ((cursorPos < 0) || cursorPos > text.length()) cursorPos = text.length();
    setCursorPosition(cursorPos);

    if (d->fontLength == 0) {
        QFontMetrics fm(d->textEdit->font());
        d->fontLength = fm.width('x');
    }
}

int CellEditor::cursorPosition() const
{
    return d->textEdit->textCursor().position();
}

void CellEditor::setCursorPosition(int pos)
{
    QTextCursor textCursor(d->textEdit->textCursor());
    textCursor.setPosition(pos);
    d->textEdit->setTextCursor(textCursor);
//     kDebug() << "pos" << pos << "textCursor" << d->textEdit->textCursor().position();
    // FIXME Stefan: The purpose of this connection is?
    //               Disabled to avoid cursor jumps to the end of the line on every key event.
//     selection()->view()->editWidget()->setCursorPosition( pos );
}

bool CellEditor::eventFilter(QObject* obj, QEvent* ev)
{
    // Only interested in KTextEdit
    if (obj != d->textEdit)
        return false;

    if (ev->type() == QEvent::FocusOut) {
        selection()->setLastEditorWithFocus(Selection::EmbeddedEditor);
        return false;
    }

    if (ev->type() == QEvent::KeyPress || ev->type() == QEvent::KeyRelease) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(ev);
        if (!(ke->modifiers() & Qt::ShiftModifier) || selection()->referenceSelectionMode()) {
            switch (ke->key()) {
            case Qt::Key_Return:
            case Qt::Key_Enter:
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_PageDown:
            case Qt::Key_PageUp:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
                // Send directly to canvas
                QApplication::sendEvent(parent(), ev);
                return true;
            }
        }
        // End choosing. May be restarted by CellEditor::slotTextChanged.
        // QKeyEvent::text() is empty, if a modifier was pressed/released.
        if (ev->type() == QEvent::KeyPress && !ke->text().isEmpty()) {
            selection()->setReferenceSelectionMode(false);
        }
        // Forward left/right arrows to parent, so that pressing left/right
        // in this editor leaves editing mode, unless this editor has been set
        // to capture arrow key events. Changed to this behaviour for
        // consistancy with OO Calc & MS Office.
        if ((!d->captureAllKeyEvents) &&
                ((ke->key() == Qt::Key_Left) || (ke->key() == Qt::Key_Right))) {
            QApplication::sendEvent(parent(), ev);
            return true;
        }
    }
    return false;
}

void CellEditor::setCursorToRange(uint pos)
{
    d->updatingChoice = true;
    uint counter = 0;
    Tokens tokens = d->highlighter->formulaTokens();
    for (int i = 0; i < tokens.count(); ++i) {
        Token token = tokens[i];
        Token::Type type = token.type();
        if (type == Token::Cell || type == Token::Range) {
            if (counter == pos) {
                setCursorPosition(token.pos() + token.text().length() + 1);
            }
            counter++;
        }
    }
    d->updatingChoice = false;
}

void CellEditor::slotSelectionDestroyed()
{
    kDebug() << "editor destroyed";
    d->selection = 0;
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
    d->cellTool->canvas()->canvasWidget()->setFocus();
}

void ExternalEditor::discardChanges()
{
    Q_ASSERT(d->cellTool);
    clear();
    d->cellTool->deleteEditor(false); // discard changes
    d->cellTool->canvas()->canvasWidget()->setFocus();
    d->cellTool->selection()->update();
}

void ExternalEditor::setText(const QString &text)
{
    Q_ASSERT(d->cellTool);
    if (toPlainText() == text) {
        return;
    }
    KTextEdit::setPlainText(text);
    QTextCursor textCursor = this->textCursor();
    textCursor.setPosition(d->cellTool->editor()->cursorPosition());
    setTextCursor(textCursor);
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
    // when the external editor gets focus, create also the internal editor
    // this in turn means that ranges will be instantly highlighted right
    if (!d->cellTool->editor())
        d->cellTool->createEditor(false /* keep content */, false /* no focus */);
    KTextEdit::focusInEvent(event);
}

void ExternalEditor::focusOutEvent(QFocusEvent* event)
{
    Q_ASSERT(d->cellTool);
    d->cellTool->selection()->setLastEditorWithFocus(Selection::ExternalEditor);
    KTextEdit::focusOutEvent(event);
}

void ExternalEditor::slotTextChanged()
{
    if (!hasFocus()) return;  // only report change if we have focus
    emit textChanged(toPlainText());
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
