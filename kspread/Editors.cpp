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
 * Boston, MA 02110-1301, USA.
*/

// KSpread
#include "Editors.h"
#include "Canvas.h"
#include "Cell.h"
#include "Doc.h"
#include "Selection.h"
#include "Format.h"
#include "Sheet.h"
#include "View.h"
#include "Util.h"
#include "Formula.h"
#include "Functions.h"

// KDE
#include <kdebug.h>
#include <kdialog.h>
#include <kicon.h>
#include <klistbox.h>
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
#include <QTimer>
#include <QToolButton>
#include <QToolTip>
#include <QVBoxLayout>
//Added by qt3to4:
#include <q3listbox.h>


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
  Private()
  {
    canvas = 0;
    tokens = Tokens();
    rangeCount = 0;
    rangeChanged = false;
  }

  // source for cell reference checking
  Canvas* canvas;
  Tokens tokens;
  uint rangeCount;
  bool rangeChanged;
};


FormulaEditorHighlighter::FormulaEditorHighlighter(QTextEdit* textEdit, Canvas* canvas)
  : QSyntaxHighlighter(textEdit)
{
  d = new Private();
  d->canvas = canvas;
}

FormulaEditorHighlighter::~FormulaEditorHighlighter()
{
  delete d;
}

const Tokens& FormulaEditorHighlighter::formulaTokens() const
{
  return d->tokens;
}

void FormulaEditorHighlighter::highlightBlock( const QString& text )
{
  // reset syntax highlighting
  setFormat(0, text.length(), Qt::black);

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
  QList<QColor> colors = d->canvas->choice()->colors();
  QList<Range> alreadyFoundRanges;

  for (int i = 0; i < d->tokens.count(); ++i)
  {
    Token token = d->tokens[i];
    Token::Type type = token.type();

    switch (type)
    {
      case Token::Cell:
      case Token::Range:
        {
            // don't compare, if we have already found a change
            if (!d->rangeChanged && i < oldTokens.count() && token.text() != oldTokens[i].text())
            {
                d->rangeChanged = true;
            }

            Range newRange( token.text() );

            if (!alreadyFoundRanges.contains(newRange))
            {
                alreadyFoundRanges.append(newRange);
                d->rangeCount++;
            }
            setFormat(token.pos() + 1, token.text().length(), colors[ alreadyFoundRanges.indexOf(newRange) % colors.size()] );
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
      case Token::Operator:    // +, *, /, -
        {
            switch (token.asOperator())
            {
                case Token::LeftPar:
                case Token::RightPar:
                    //Check where this brace is in relation to the cursor and highlight it if necessary.
                    handleBrace( i );
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

void FormulaEditorHighlighter::handleBrace( uint index )
{
  const Token& token = d->tokens.at( index );

  QTextEdit* textEdit = qobject_cast<QTextEdit*>( parent() );
  Q_ASSERT( textEdit );
  int cursorPos = textEdit->textCursor().position();
  int distance = cursorPos-token.pos();
  int opType = token.asOperator();
  bool highlightBrace=false;

  //Check where the cursor is in relation to this left or right parenthesis token.
  //Only one pair of braces should be highlighted at a time, and if the cursor
  //is between two braces, the inner-most pair should be highlighted.

  if ( opType == Token::LeftPar )
  {
    //If cursor is directly to the left of this left brace, highlight it
    if ( distance == 1 )
      highlightBrace=true;
    else
        //Cursor is directly to the right of this left brace, highlight it unless
        //there is another left brace to the right (in which case that should be highlighted instead as it
        //is the inner-most brace)
        if (distance==2)
          if ( (index == (uint)d->tokens.count()-1) || ( d->tokens.at(index+1).asOperator() != Token::LeftPar) )
            highlightBrace=true;

  }
  else
  {
    //If cursor is directly to the right of this right brace, highlight it
    if ( distance == 2 )
      highlightBrace=true;
    else
        //Cursor is directly to the left of this right brace, so highlight it unless
        //there is another right brace to the left (in which case that should be highlighted instead as it
        //is the inner-most brace)
      if ( distance == 1 )
        if ( (index == 0) || (d->tokens.at(index-1).asOperator() != Token::RightPar) )
          highlightBrace=true;
  }

  if (highlightBrace)
  {
    QFont font = QFont( document()->defaultFont() );
    font.setBold(true);
    setFormat(token.pos() + 1, token.text().length(), font);

    int matching = findMatchingBrace( index );

    if (matching != -1)
    {
      Token matchingBrace = d->tokens.at(matching);
      setFormat( matchingBrace.pos() + 1 , matchingBrace.text().length() , font);
    }
  }
}

int FormulaEditorHighlighter::findMatchingBrace(int pos)
{
    int depth=0;
    int step=0;

    Tokens tokens = d->tokens;

    //If this is a left brace we need to step forwards through the text to find the matching right brace,
    //otherwise, it is a right brace so we need to step backwards through the text to find the matching left
    //brace.
    if (tokens.at(pos).asOperator() == Token::LeftPar)
        step = 1;
    else
        step = -1;

    for (int index=pos ; (index >= 0) && (index < (int) tokens.count() ) ; index += step  )
    {
        if (tokens.at(index).asOperator() == Token::LeftPar)
            depth++;
        if (tokens.at(index).asOperator() == Token::RightPar)
            depth--;

        if (depth == 0)
        {
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
    d->rangeChanged=false;
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
  KListBox *completionListBox;
  QLabel* hintLabel;
};

FunctionCompletion::FunctionCompletion( CellEditor* editor ):
QObject( editor )
{
  d = new Private;
  d->editor = editor;
  d->hintLabel = 0;

  d->completionPopup = new QFrame( editor->topLevelWidget(), Qt::WType_Popup );
  d->completionPopup->setFrameStyle( QFrame::Box | QFrame::Plain );
  d->completionPopup->setLineWidth( 1 );
  d->completionPopup->installEventFilter( this );
  d->completionPopup->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum);
  QVBoxLayout *layout = new QVBoxLayout( d->completionPopup );
  layout->setMargin( 0 );
  layout->setSpacing( 0 );

  d->completionListBox = new KListBox( d->completionPopup );
  d->completionPopup->setFocusProxy( d->completionListBox );
  d->completionListBox->setFrameStyle( QFrame::NoFrame );
  d->completionListBox->setVariableWidth( true );
  d->completionListBox->installEventFilter( this );
  connect( d->completionListBox, SIGNAL(selected(const QString&)), this,
    SLOT(itemSelected(const QString&)) );
  connect( d->completionListBox, SIGNAL(highlighted(const QString&)), this,
    SLOT(itemSelected(const QString&)) );
  layout->addWidget( d->completionListBox );

  d->hintLabel = new QLabel( 0, (Qt::WFlags)
    Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool |  Qt::WX11BypassWM );
  d->hintLabel->setFrameStyle( QFrame::Plain | QFrame::Box );
  d->hintLabel->setPalette( QToolTip::palette() );
  d->hintLabel->hide();
}

FunctionCompletion::~FunctionCompletion()
{
      delete d->hintLabel;
      delete d;
}

void FunctionCompletion::itemSelected( const QString& item )
{
    KSpread::FunctionDescription* desc;
    desc = KSpread::FunctionRepository::self()->functionInfo(item);
    if(!desc)
    {
        d->hintLabel->hide();
        return;
    }

    QString helpText = desc->helpText()[0];
    if( helpText.isEmpty() )
    {
        d->hintLabel->hide();
        return;
    }

    helpText.append("</qt>").prepend("<qt>");
    d->hintLabel->setText( helpText );
    d->hintLabel->adjustSize();

    // reposition nicely
    QPoint pos = d->editor->mapToGlobal( QPoint( d->editor->width(), 0 ) );
    pos.setY( pos.y() - d->hintLabel->height() - 1 );
    d->hintLabel->move( pos );
    d->hintLabel->show();
    d->hintLabel->raise();

    // do not show it forever
    //QTimer::singleShot( 5000, d->hintLabel, SLOT( hide()) );
}

bool FunctionCompletion::eventFilter( QObject *obj, QEvent *ev )
{
    if ( obj == d->completionPopup || obj == d->completionListBox )
    {
      if ( ev->type() == QEvent::KeyPress )
      {
              QKeyEvent *ke = (QKeyEvent*)ev;
              if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return  )
              {
                  doneCompletion();
                  return true;
              }
              else if ( ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right ||
              ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ||
              ke->key() == Qt::Key_Home || ke->key() == Qt::Key_End ||
              ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown )
                  return false;

              d->hintLabel->hide();
              d->completionPopup->close();
              d->editor->setFocus();
              QApplication::sendEvent( d->editor, ev );
              return true;
      }

      if ( ev->type() == QEvent::MouseButtonDblClick )
      {
          doneCompletion();
          return true;
      }
  }

  return false;
}

void FunctionCompletion::doneCompletion()
{
  d->hintLabel->hide();
  d->completionPopup->close();
  d->editor->setFocus();
  emit selectedCompletion( d->completionListBox->currentText() );
}

void FunctionCompletion::showCompletion( const QStringList &choices )
{
  if( !choices.count() ) return;

  d->completionListBox->clear();
  for( int i = 0; i < choices.count(); i++ )
    new Q3ListBoxText( (Q3ListBox*)d->completionListBox, choices[i] );
  d->completionListBox->setCurrentItem( 0 );

  // size of the pop-up
  d->completionPopup->setMaximumHeight( 100 );
  d->completionPopup->resize( d->completionListBox->sizeHint() +
    QSize( d->completionListBox->verticalScrollBar()->width() + 4,
        d->completionListBox->horizontalScrollBar()->height() + 4 ) );
  int h = d->completionListBox->height();
  int w = d->completionListBox->width();

  QPoint pos = d->editor->globalCursorPosition();

  // if popup is partially invisible, move to other position
  // FIXME check it if it works in Xinerama multihead
  int screen_num = QApplication::desktop()->screenNumber( d->completionPopup );
  QRect screen = QApplication::desktop()->screenGeometry( screen_num );
  if( pos.y() + h > screen.y()+screen.height() )
    pos.setY( pos.y() - h - d->editor->height() );
  if( pos.x() + w > screen.x()+screen.width() )
    pos.setX(  screen.x()+screen.width() - w );

  d->completionPopup->move( pos );
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
  Cell*                     cell;
  Canvas*                   canvas;
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
  uint currentToken;
  uint rangeCount;
};


CellEditor::CellEditor( Cell* _cell, Canvas* _parent, bool captureAllKeyEvents, const char* /*_name*/ )
  : QWidget( _parent )
{
  d = new Private();
  d->cell = _cell;
  d->canvas = _parent;
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

//TODO - Get rid of QTextEdit margins, this doesn't seem easily possible in Qt 3.3, so a job for Qt 4 porting.

  d->textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->textEdit->setFrameStyle(QFrame::NoFrame);
  d->textEdit->setLineWidth(0);
  d->textEdit->installEventFilter( this );

  d->highlighter = new FormulaEditorHighlighter( d->textEdit, _parent );

  d->functionCompletion = new FunctionCompletion( this );
  d->functionCompletionTimer = new QTimer( this );
  connect( d->functionCompletion, SIGNAL( selectedCompletion( const QString& ) ),
    SLOT( functionAutoComplete( const QString& ) ) );
  connect( d->textEdit, SIGNAL( textChanged() ), SLOT( checkFunctionAutoComplete() ) );
  connect( d->functionCompletionTimer, SIGNAL( timeout() ),
    SLOT( triggerFunctionAutoComplete() ) );

  if (!cell()->format()->multiRow(cell()->column(),cell()->row()))
    d->textEdit->setWordWrapMode( QTextOption::NoWrap );
  else
    d->textEdit->setWordWrapMode( QTextOption::WordWrap );

  //TODO - Custom KTextEdit class which supports text completion
/*
  d->textEdit->setFrame( false );
  d->textEdit->setCompletionMode((KGlobalSettings::Completion)canvas()->view()->doc()->completionMode()  );
  d->textEdit->setCompletionObject( &canvas()->view()->doc()->completion(),true );
*/
  setFocusProxy( d->textEdit );

  connect( d->textEdit, SIGNAL( cursorPositionChanged() ), this, SLOT (slotCursorPositionChanged()));
  connect( d->textEdit, SIGNAL( textChanged() ), this, SLOT( slotTextChanged() ) );

// connect( d->textEdit, SIGNAL(completionModeChanged( KGlobalSettings::Completion )),this,SLOT (slotCompletionModeChanged(KGlobalSettings::Completion)));

  // A choose should always start at the edited cell
//  canvas()->setChooseMarkerRow( canvas()->markerRow() );
//  canvas()->setChooseMarkerColumn( canvas()->markerColumn() );

  // set font size according to zoom factor
  QFont font( _cell->format()->font() );
  font.setPointSizeF( 0.01 * _parent->doc()->zoomInPercent() * font.pointSizeF() );
  d->textEdit->setFont( font );

  if (d->fontLength == 0)
  {
    QFontMetrics fm( d->textEdit->font() );
    d->fontLength = fm.width('x');
  }
}

CellEditor::~CellEditor()
{
  canvas()->endChoose();

  delete d->highlighter;
  delete d->functionCompletion;
  delete d->functionCompletionTimer;
  delete d;
}

Cell* CellEditor::cell() const
{
  return d->cell;
}

Canvas* CellEditor::canvas() const
{
  return d->canvas;
}

QPoint CellEditor::globalCursorPosition() const
{
  return d->globalCursorPos;
}

void CellEditor::checkFunctionAutoComplete()
{
  d->functionCompletionTimer->stop();
  d->functionCompletionTimer->setSingleShot( true );
  d->functionCompletionTimer->start( 2000 );
}

void CellEditor::triggerFunctionAutoComplete()
{
  // tokenize the expression (don't worry, this is very fast)
  int curPos = d->textEdit->textCursor().position();
  QString subtext = d->textEdit->toPlainText().left( curPos );

  KSpread::Formula f;
  KSpread::Tokens tokens = f.scan( subtext );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;

  KSpread::Token lastToken = tokens[ tokens.count()-1 ];

  // last token must be an identifier
  if( !lastToken.isIdentifier() ) return;
  QString id = lastToken.text();
  if( id.length() < 1 ) return;

  // find matches in function names
  QStringList fnames = KSpread::FunctionRepository::self()->functionNames();
  QStringList choices;
  for( int i=0; i<fnames.count(); i++ )
    if( fnames[i].startsWith( id, Qt::CaseInsensitive ) )
      choices.append( fnames[i] );
  choices.sort();

  // no match, don't bother with completion
  if( !choices.count() ) return;

  // single perfect match, no need to give choices
  if( choices.count()==1 )
    if( choices[0].toLower() == id.toLower() )
      return;

  // present the user with completion choices
  d->functionCompletion->showCompletion( choices );
}

void CellEditor::functionAutoComplete( const QString& item )
{
  if( item.isEmpty() ) return;

  QTextCursor textCursor = d->textEdit->textCursor();
  int curPos = textCursor.position();
  QString subtext = text().left( curPos );

  KSpread::Formula f;
  KSpread::Tokens tokens = f.scan( subtext );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;

  KSpread::Token lastToken = tokens[ tokens.count()-1 ];
  if( !lastToken.isIdentifier() ) return;

  d->textEdit->blockSignals( true );
  textCursor.setPosition( lastToken.pos() + 1 );
  textCursor.setPosition( lastToken.pos() + lastToken.text().length() + 1,
                            QTextCursor::KeepAnchor );
  d->textEdit->insertPlainText( item );
  d->textEdit->blockSignals( false );
}

void CellEditor::slotCursorPositionChanged()
{
//   kDebug() << k_funcinfo << endl;

  // TODO Stefan: optimize this function!

  // turn choose mode on/off
  if (!checkChoice())
    return;

  d->globalCursorPos = d->textEdit->mapToGlobal( d->textEdit->cursorRect().topLeft() );

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
  for (int i = 0; i < tokens.count(); ++i)
  {
    if (tokens[i].pos() >= d->textEdit->textCursor().position() - 1) // without '='
    {
/*      kDebug() << "token.pos >= cursor.pos" << endl;*/
      type = tokens[i].type();
      if (type == Token::Cell || type == Token::Range)
      {
        if (lastWasASemicolon)
        {
          regionEnd = rangeCounter++;
          lastWasASemicolon = false;
          continue;
        }
      }
      if (type == Token::Operator && tokens[i].asOperator() == Token::Semicolon)
      {
        lastWasASemicolon = true;
        continue;
      }
      lastWasASemicolon = false;
      break;
    }
    token = tokens[i];
    d->currentToken = i;

    type = token.type();
    if (type == Token::Cell || type == Token::Range)
    {
      if (!lastWasASemicolon)
      {
        regionStart = rangeCounter;
      }
      regionEnd = rangeCounter;
      currentRange = rangeCounter++;
    }
    // semicolons are use as deliminiters in regions
    if (type == Token::Operator)
    {
      if (token.asOperator() == Token::Semicolon)
      {
        lastWasASemicolon = true;
      }
      else
      {
        lastWasASemicolon = false;
        // set the region start to the next element
        regionStart = currentRange + 1;
        regionEnd = regionStart - 1; // len = 0
      }
    }
  }

//   kDebug() << "regionStart = " << regionStart/* << endl*/
//             << ", regionEnd = " << regionEnd/* << endl*/
//             << ", currentRange = " << currentRange << endl;

  // only change the active sub region, if we have found one.
  if (regionStart != -1)
  {
    d->canvas->choice()->setActiveElement(currentRange);
    d->canvas->choice()->setActiveSubRegion(regionStart, regionEnd-regionStart+1);
  }

  // triggered by keyboard action?
  if (!d->updatingChoice)
  {
    if (d->highlighter->rangeChanged())
    {
      d->highlighter->resetRangeChanged();

      disconnect( d->canvas->choice(), SIGNAL(changed(const Region&)),
                  d->canvas->view(), SLOT(slotScrollChoice(const Region&)) );
      d->canvas->doc()->emitBeginOperation();
      setUpdateChoice(false);

      Tokens tokens = d->highlighter->formulaTokens();
      d->canvas->choice()->update(); // set the old one dirty
      d->canvas->choice()->clear();
      Region tmpRegion;
      Region::ConstIterator it;

      //A list of regions which have already been highlighted on the spreadsheet.
      //This is so that we don't end up highlighting the same region twice in two different
      //colours.
      QLinkedList<Region> alreadyUsedRegions;

      for (int i = 0; i < tokens.count(); ++i)
      {
        Token token = tokens[i];
        Token::Type type = token.type();
        if (type == Token::Cell || type == Token::Range)
        {
          Region region(d->canvas->view()->doc()->map(), token.text(), d->canvas->activeSheet());
          it = region.constBegin();

          if (!alreadyUsedRegions.contains(region))
          {
            QRect r=(*it)->rect();

            if (d->canvas->choice()->isEmpty())
                d->canvas->choice()->initialize((*it)->rect(), (*it)->sheet());
            else
                d->canvas->choice()->extend((*it)->rect(), (*it)->sheet());

            alreadyUsedRegions.append(region);
          }
        }
      }
      setUpdateChoice(true);
      d->canvas->doc()->emitEndOperation(*d->canvas->choice());
      connect( d->canvas->choice(), SIGNAL(changed(const Region&)),
               d->canvas->view(), SLOT(slotScrollChoice(const Region&)) );
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

void CellEditor::setEditorFont(QFont const & font, bool updateSize)
{
  QFont tmpFont( font );
  tmpFont.setPointSizeF( 0.01 * canvas()->doc()->zoomInPercent() * tmpFont.pointSizeF() );
  d->textEdit->setFont( tmpFont );

  if (updateSize)
  {
    QFontMetrics fm( d->textEdit->font() );
    d->fontLength = fm.width('x');

    int mw = fm.width( d->textEdit->toPlainText() ) + d->fontLength;
    // don't make it smaller: then we would have to repaint the obscured cells
    if (mw < width())
      mw = width();

    int mh = fm.height();
    if (mh < height())
      mh = height();

    setGeometry(x(), y(), mw, mh);
  }
}

void CellEditor::slotCompletionModeChanged(KGlobalSettings::Completion _completion)
{
  canvas()->view()->doc()->setCompletionMode( _completion );
}

void CellEditor::slotTextChanged()
{
//   kDebug() << k_funcinfo << endl;

  //FIXME - text() may return richtext?
  QString t = text();

  if (t.length() > d->length)
  {
    d->length = t.length();

  QFontMetrics fm(d->textEdit->font());
  // - requiredWidth = width of text plus some spacer characters
  int requiredWidth = fm.width(t) + (2*fm.width('x'));

  //For normal single-row cells, the text editor must be expanded horizontally to
  //allow the text to fit if the new text is too wide
  //For multi-row (word-wrap enabled) cells, the text editor must expand vertically to
  //allow for new rows of text & the width of the text editor is not affected
  if ( d->textEdit->wordWrapMode() == QTextOption::NoWrap )
  {
    if (requiredWidth > width())
    {
      if (t.isRightToLeft())
      {
        setGeometry(x() - requiredWidth + width(), y(), requiredWidth,height());
      }
      else
      {
        setGeometry(x(), y(), requiredWidth,height());
      }
    }
  }
  else
  {
    int requiredHeight = d->textEdit->heightForWidth(width());

    if (requiredHeight > height())
    {
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

  if ( (cell()->formatType()) == Percentage_format )
  {
    if ( (t.length() == 1) && t[0].isDigit() )
    {
      QString tmp = t + " %";
      d->textEdit->setPlainText(tmp);
      d->textEdit->textCursor().setPosition( 1 );
      return;
    }
  }

  canvas()->view()->editWidget()->setText( t );
  // canvas()->view()->editWidget()->textCursor().setPosition( d->textEdit->cursorPosition() );
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
  if ( text[0] != '=' )
  {
    canvas()->setChooseMode(false);
  }
  else
  {
    int cur = d->textEdit->textCursor().position();

    Tokens tokens = d->highlighter->formulaTokens();

    // empty formula?
    if (tokens.count() < 1)
    {
      canvas()->startChoose();
    }
    else
    {
      Token token;
      for (int i = 0; i < tokens.count(); ++i)
      {
        if (tokens[i].pos() >= cur - 1) // without '='
        {
          break;
        }
        token = tokens[i];
        d->currentToken = i;
      }

      Token::Type type = token.type();
      if (type == Token::Operator && token.asOperator() != Token::RightPar)
      {
        canvas()->setChooseMode(true);
      }
      else if (type == Token::Cell || type == Token::Range)
      {
        d->length_namecell = token.text().length();
        canvas()->setChooseMode(true);
      }
      else
      {
        canvas()->setChooseMode(false);
      }
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
//   kDebug() << k_funcinfo << endl;

  if (!d->updateChoice)
    return;

//   // prevent recursion
//   d->updateChoice = false; // TODO nescessary?
  d->updatingChoice = true;

  Selection* choice = d->canvas->choice();

  if (choice->isEmpty())
    return;

  if (!choice->activeElement())
    return;

  // only one element TODO
  if (++choice->constBegin() == choice->constEnd())
  {
  }

  QString name_cell = choice->activeSubRegionName();

  Tokens tokens = d->highlighter->formulaTokens();
  uint start = 1;
  uint length = 0;
  if (!tokens.empty())
  {
    Token token = tokens[d->currentToken];
    Token::Type type = token.type();
    if (type == Token::Cell || type == Token::Range)
    {
      start = token.pos() + 1; // don't forget the '='!
      length = token.text().length();
    }
    else
    {
      start = token.pos() + token.text().length() + 1;
    }
  }

  d->length_namecell = name_cell.length();
  d->length_text = text().length();
    //kDebug(36001) << "updateChooseMarker2 len=" << d->length_namecell << endl;

  QString oldText = text();
  QString newText = oldText.left(start) + name_cell + oldText.right(d->length_text - start - length);

  setCheckChoice( false );
  setText( newText );
  setCheckChoice( true );
  setCursorPosition( start + d->length_namecell );

  d->canvas->view()->editWidget()->setText( newText );
    //kDebug(36001) << "old=" << old << " len=" << d->length_namecell << " pos=" << pos << endl;

//   d->updateChoice = false;
  d->updatingChoice = false;
}

void CellEditor::resizeEvent( QResizeEvent* )
{
    d->textEdit->setGeometry( 0, 0, width(), height() );
}

void CellEditor::handleKeyPressEvent( QKeyEvent * _ev )
{
  if (_ev->key() == Qt::Key_F4)
  {
    if (d->textEdit == 0)
    {
      QApplication::sendEvent( d->textEdit, _ev );
      return;
    }

    QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$");

    int cur = d->textEdit->textCursor().position();
    QString tmp, tmp2;
    int n = -1;

    // this is ugly, and sort of hack
    // FIXME rewrite to use the real Tokenizer
    unsigned i;
    for( i = 0; i < 10; i++ )
    {
      tmp =  d->textEdit->toPlainText().left( cur+i );
      tmp2 = d->textEdit->toPlainText().right( d->textEdit->toPlainText().length() - cur - i );

      n = exp.indexIn(tmp);
      if( n >= 0 ) break;
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
    d->textEdit->textCursor().setPosition( cur );

    _ev->accept();

    return;
  }

  // Send the key event to the KLineEdit
  QApplication::sendEvent( d->textEdit, _ev );
}

void CellEditor::handleIMEvent( QInputMethodEvent  * _ev )
{
    // send the IM event to the KLineEdit
    QApplication::sendEvent( d->textEdit, _ev );
}

QString CellEditor::text() const
{
    return d->textEdit->toPlainText();
}

void CellEditor::setText(QString text)
{
  d->textEdit->setPlainText(text);
  //Usability : It is usually more convenient if the cursor is positioned at the end of the text so it can
  //be quickly deleted using the backspace key

  //This also ensures that the caret is sized correctly for the text
  d->textEdit->textCursor().setPosition( text.length() );

    if (d->fontLength == 0)
    {
      QFontMetrics fm( d->textEdit->font() );
      d->fontLength = fm.width('x');
    }
}

int CellEditor::cursorPosition() const
{
  return d->textEdit->textCursor().position();
}

void CellEditor::setCursorPosition( int pos )
{
  d->textEdit->textCursor().setPosition( pos );
  canvas()->view()->editWidget()->setCursorPosition( pos );
}

bool CellEditor::eventFilter( QObject* o, QEvent* e )
{
    // Only interested in KTextEdit
    if ( o != d->textEdit )
        return false;
    if ( e->type() == QEvent::FocusOut )
    {
        canvas()->setLastEditorWithFocus( Canvas::CellEditor );
        return false;
    }

    if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease )
    {
        QKeyEvent* k = (QKeyEvent*)e;
        if ( !( k->modifiers() & Qt::ShiftModifier ) || canvas()->chooseMode())
        {
          //If the user presses the return key to finish editing this cell, choose mode must be turned off first
          //otherwise it will merely select a different cell
          if (k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter)
          {
            kDebug() << "CellEditor::eventFilter: canvas()->endChoose();" << endl;
            canvas()->endChoose();
          }

          //NB - Added check for Qt::Key_Return when migrating text edit from KLineEdit to KTextEdit, since
          //normal behaviour for KTextEdit is to swallow return key presses
          if ( k->key() == Qt::Key_Up || k->key() == Qt::Key_Down ||
                k->key() == Qt::Key_PageDown || k->key() == Qt::Key_PageUp ||
                k->key() == Qt::Key_Escape || k->key() == Qt::Key_Tab ||
                k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter)
          {
              // Send directly to canvas
              QApplication::sendEvent( parent(), e );
              return true;
          }
        }
        // End choosing. May be restarted by CellEditor::slotTextChanged
        if ( e->type() == QEvent::KeyPress && !k->text().isEmpty() )
        {
          canvas()->setChooseMode(false);
        }
        // forward Left/Right keys - so that pressing left/right in this
        // editor leaves editing mode ... otherwise editing is annoying
        // left/right arrows still work with the F2-editor.

        // Forward left & right arrows to parent, unless this editor has been set to capture arrow key events
        // Changed to this behaviour for consistancy with OO Calc & MS Office.
        if ( ((k->key() == Qt::Key_Left) || (k->key() == Qt::Key_Right)) && (!d->captureAllKeyEvents)) {
          QApplication::sendEvent (parent(), e);
          return true;
        }
    }

    return false;
}

void CellEditor::setCursorToRange(uint pos)
{
//   kDebug() << k_funcinfo << endl;

  d->updatingChoice = true;
  uint counter = 0;
  Tokens tokens = d->highlighter->formulaTokens();
  for (int i = 0; i < tokens.count(); ++i)
  {
    Token token = tokens[i];
    Token::Type type = token.type();
    if (type == Token::Cell || type == Token::Range)
    {
      if (counter == pos)
      {
        setCursorPosition(token.pos() + token.text().length() + 1);
      }
      counter++;
    }
  }
  d->updatingChoice = false;
}



/*****************************************************************************
 *
 * ComboboxLocationEditWidget
 *
 ****************************************************************************/

ComboboxLocationEditWidget::ComboboxLocationEditWidget( QWidget * _parent,
                                                      View * _view )
    : KComboBox( _parent)
{
    m_locationWidget = new LocationEditWidget( _parent, _view );
    setLineEdit( m_locationWidget );
    insertItem( 0,"" );

    QList<Reference>::Iterator it;
    QList<Reference> area = _view->doc()->listArea();
    for ( it = area.begin(); it != area.end(); ++it )
        slotAddAreaName( (*it).ref_name);
    connect( this, SIGNAL( activated ( const QString & ) ), m_locationWidget, SLOT( slotActivateItem() ) );
}


void ComboboxLocationEditWidget::slotAddAreaName( const QString &_name)
{
    insertItem( count(), _name );
    m_locationWidget->addCompletionItem( _name );
}

void ComboboxLocationEditWidget::slotRemoveAreaName( const QString &_name )
{
    for ( int i = 0; i<count(); i++ )
    {
        if ( itemText(i) == _name )
        {
            removeItem( i );
            break;
        }
    }
    m_locationWidget->removeCompletionItem( _name );
}



/*****************************************************************************
 *
 * LocationEditWidget
 *
 ****************************************************************************/

LocationEditWidget::LocationEditWidget( QWidget * _parent,
                                                      View * _view )
    : KLineEdit( _parent),
      m_pView(_view)
{
    setCompletionObject( &completionList,true );
    setCompletionMode(KGlobalSettings::CompletionAuto  );
}

void LocationEditWidget::addCompletionItem( const QString &_item )
{
    kDebug()<<"  LocationEditWidget::addCompletionItem add :"<<_item<<endl;
    if ( completionList.items().contains( _item) == 0 )
    {
        completionList.addItem( _item );
        kDebug()<<" _utem :"<<_item<<endl;
        kDebug()<<" completionList.items().count()"<<completionList.items().count()<<endl;
    }
}

void LocationEditWidget::removeCompletionItem( const QString &_item )
{
    completionList.removeItem( _item );
}

void LocationEditWidget::slotActivateItem()
{
    activateItem();
}

bool LocationEditWidget::activateItem()
{
    QString ltext = text();
    QString tmp = ltext.toLower();
    QList<Reference>::Iterator it;
    QList<Reference> area = m_pView->doc()->listArea();
    for ( it = area.begin(); it != area.end(); ++it )
    {
        if ((*it).ref_name == tmp)
        {
            QString tmp = (*it).sheet_name;
            tmp += '!';
            tmp += util_rangeName((*it).rect);
            m_pView->selectionInfo()->initialize( Region(m_pView->doc()->map(), tmp, m_pView->activeSheet()) );
            return true;
        }
    }

    // Set the cell component to uppercase:
    // Sheet1!a1 -> Sheet1!A2
    int pos = ltext.indexOf('!');
    if ( pos !=- 1 )
        tmp = ltext.left(pos)+ltext.mid(pos).toUpper();
    else
        tmp = ltext.toUpper();

    // Selection entered in location widget
    if ( ltext.contains( ':' ) )
      m_pView->selectionInfo()->initialize( Region(m_pView->doc()->map(), tmp, m_pView->activeSheet()) );
    // Location entered in location widget
    else
    {
      Region region(m_pView->doc()->map(), tmp, m_pView->activeSheet());
        bool validName = true;
        for (int i = 0; i < ltext.length(); ++i)
        {
            if (!ltext[i].isLetter())
            {
                validName = false;
                break;
            }
        }
        if ( !region.isValid() && validName)
        {
            QRect rect( m_pView->selectionInfo()->selection() );
            Sheet * t = m_pView->activeSheet();
            // set area name on current selection/cell

            m_pView->doc()->addAreaName(rect, ltext.toLower(), t->sheetName());
        }

        if (!validName)
        {
          m_pView->selectionInfo()->initialize(region);
        }
    }

    // Set the focus back on the canvas.
    m_pView->canvasWidget()->setFocus();
    return false;
}


void LocationEditWidget::keyPressEvent( QKeyEvent * _ev )
{
    // Do not handle special keys and accelerators. This is
    // done by QLineEdit.
    if ( _ev->modifiers() & ( Qt::AltModifier | Qt::ControlModifier ) )
    {
        QLineEdit::keyPressEvent( _ev );
        // Never allow that keys are passed on to the parent.
        _ev->accept();

        return;
    }

    // Handle some special keys here. Eve
    switch( _ev->key() )
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        if ( activateItem() )
            return;
        _ev->accept();
    }
    break;
    // Escape pressed, restore original value
    case Qt::Key_Escape:
        if ( m_pView->selectionInfo()->isSingular() ) {
            setText( Cell::columnName( m_pView->canvasWidget()->markerColumn() )
                     + QString::number( m_pView->canvasWidget()->markerRow() ) );
        } else {
            setText( Cell::columnName( m_pView->selectionInfo()->lastRange().left() )
                     + QString::number( m_pView->selectionInfo()->lastRange().top() )
                     + ':'
                     + Cell::columnName( m_pView->selectionInfo()->lastRange().right() )
                     + QString::number( m_pView->selectionInfo()->lastRange().bottom() ) );
        }
        m_pView->canvasWidget()->setFocus();
        _ev->accept();
        break;
    default:
        QLineEdit::keyPressEvent( _ev );
        // Never allow that keys are passed on to the parent.
        _ev->accept();
    }
}



/****************************************************************
 *
 * EditWidget
 * The line-editor that appears above the sheet and allows to
 * edit the cells content.
 *
 ****************************************************************/

EditWidget::EditWidget( QWidget *_parent, Canvas *_canvas,
                        QAbstractButton *cancelButton, QAbstractButton *okButton )
  : QLineEdit( _parent )
{
  m_pCanvas = _canvas;
  Q_ASSERT(m_pCanvas != 0);
  // Those buttons are created by the caller, so that they are inserted
  // properly in the layout - but they are then managed here.
  m_pCancelButton = cancelButton;
  m_pOkButton = okButton;
  isArray = false;

  installEventFilter(m_pCanvas);

  if ( !m_pCanvas->doc()->isReadWrite() || !m_pCanvas->activeSheet() )
    setEnabled( false );

  QObject::connect( m_pCancelButton, SIGNAL( clicked() ),
                    this, SLOT( slotAbortEdit() ) );
  QObject::connect( m_pOkButton, SIGNAL( clicked() ),
                    this, SLOT( slotDoneEdit() ) );

  setEditMode( false ); // disable buttons
}

void EditWidget::showEditWidget(bool _show)
{
    if (_show)
  {
      m_pCancelButton->show();
      m_pOkButton->show();
      show();
  }
    else
  {
      m_pCancelButton->hide();
      m_pOkButton->hide();
      hide();
  }
}

void EditWidget::slotAbortEdit()
{
    m_pCanvas->deleteEditor( false /*discard changes*/ );
    // will take care of the buttons
}

void EditWidget::slotDoneEdit()
{
  m_pCanvas->deleteEditor( true /*keep changes*/, isArray);
  isArray = false;
  // will take care of the buttons
}

void EditWidget::keyPressEvent ( QKeyEvent* _ev )
{
    // Dont handle special keys and accelerators, except Enter ones
    if (( ( _ev->modifiers() & ( Qt::AltModifier | Qt::ControlModifier ) )
         || ( _ev->modifiers() & Qt::ShiftModifier )
         || ( _ev->key() == Qt::Key_Shift )
         || ( _ev->key() == Qt::Key_Control ) )
      && (_ev->key() != Qt::Key_Return) && (_ev->key() != Qt::Key_Enter))
    {
        QLineEdit::keyPressEvent( _ev );
        _ev->accept();
        return;
    }

  if ( !m_pCanvas->doc()->isReadWrite() )
    return;

  if ( !m_pCanvas->editor() )
  {
    // Start editing the current cell
    m_pCanvas->createEditor( Canvas::CellEditor,false );
  }
  CellEditor * cellEditor = (CellEditor*) m_pCanvas->editor();

  switch ( _ev->key() )
  {
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Return:
    case Qt::Key_Enter:
      cellEditor->setText( text());
      // Don't allow to start a chooser when pressing the arrow keys
      // in this widget, since only up and down would work anyway.
      // This is why we call slotDoneEdit now, instead of sending
      // to the canvas.
      //QApplication::sendEvent( m_pCanvas, _ev );
      isArray = (_ev->modifiers() & Qt::AltModifier) &&
          (_ev->modifiers() & Qt::ControlModifier);
      slotDoneEdit();
      m_pCanvas->view()->updateEditWidget();
      _ev->accept();
      break;
    case Qt::Key_F2:
      cellEditor->setFocus();
      cellEditor->setText( text());
      cellEditor->setCursorPosition(cursorPosition());
      break;
    default:

      QLineEdit::keyPressEvent( _ev );

      setFocus();
      cellEditor->setCheckChoice( false );
      cellEditor->setText( text() );
      cellEditor->setCheckChoice( true );
      cellEditor->setCursorPosition( cursorPosition() );
  }
}

void EditWidget::setEditMode( bool mode )
{
  m_pCancelButton->setEnabled(mode);
  m_pOkButton->setEnabled(mode);
}

void EditWidget::focusOutEvent( QFocusEvent* ev )
{
  //kDebug(36001) << "EditWidget lost focus" << endl;
  // See comment about setLastEditorWithFocus
  m_pCanvas->setLastEditorWithFocus( Canvas::EditWidget );

  QLineEdit::focusOutEvent( ev );
}

void EditWidget::setText( const QString& t )
{
  if ( t == text() ) // Why this? (David)
    return;

  QLineEdit::setText( t );
}



/*****************************************************************************
 *
 * RegionSelector
 *
 ****************************************************************************/

class RegionSelector::Private
{
  public:
    View* view;
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

RegionSelector::RegionSelector( QWidget* parent )
  : QWidget( parent ),
    d( new Private )
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  d->displayMode = Widget;
  d->parentDialog = 0;
  d->view = 0;
  d->dialog = 0;
  d->highlighter = 0;
  d->button = new QToolButton( this );
  d->button->setCheckable( true );
  d->button->setIcon( KIcon( "selection" ) );
  d->textEdit = new KTextEdit( this );
  d->textEdit->setLineWrapMode( QTextEdit::NoWrap );
  d->textEdit->setWordWrapMode( QTextOption::NoWrap );
  d->textEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  d->textEdit->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  d->textEdit->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  d->textEdit->setFixedHeight( d->button->height() - 2*d->textEdit->frameWidth() ); // FIXME
  d->textEdit->setTabChangesFocus( true );

  QHBoxLayout* layout = new QHBoxLayout( this );
  layout->setMargin( 0 );
  layout->setSpacing( 2 );
  layout->addWidget( d->textEdit );
  layout->addWidget( d->button );

  d->button->installEventFilter( this );
  d->textEdit->installEventFilter( this );
  connect( d->button, SIGNAL( toggled(bool) ),
           this, SLOT( switchDisplayMode(bool) ) );
}

RegionSelector::~RegionSelector()
{
  d->view->canvasWidget()->endChoose();
  delete d;
}

void RegionSelector::setSelectionMode( SelectionMode mode )
{
  d->selectionMode = mode;
  // TODO adjust selection
}

void RegionSelector::setView( View* view )
{
  d->view = view;
  d->highlighter = new FormulaEditorHighlighter( d->textEdit, view->canvasWidget() );
  connect( d->view->choice(), SIGNAL( changed(const Region&) ),
           this, SLOT( choiceChanged() ) );
}

void RegionSelector::setDialog( QDialog* dialog )
{
  d->parentDialog = dialog;
}

KTextEdit* RegionSelector::textEdit() const
{
  return d->textEdit;
}

bool RegionSelector::eventFilter( QObject* object, QEvent* event )
{
  if ( event->type() == QEvent::Close )
  {
    if ( object == d->dialog  && d->button->isChecked() )
    {
      // TODO Stefan: handle as button click
//       d->button->toggle();
      event->ignore();
      return true; // eat it
    }
  }
  else if ( event->type() == QEvent::FocusIn )
  {
    Private::s_focussedSelector = this;
    d->view->canvasWidget()->startChoose();
    if (d->selectionMode == SingleCell)
    {
      d->view->choice()->setSelectionMode( Selection::SingleCell );
    }
    else
    {
      d->view->choice()->setSelectionMode( Selection::MultipleCells );
    }
    // TODO Stefan: initialize choice
  }
  return QObject::eventFilter( object, event );
}

void RegionSelector::switchDisplayMode( bool state )
{
  Q_UNUSED(state)
      kDebug() << k_funcinfo << endl;

  if ( d->displayMode == Widget )
  {
    d->displayMode = Dialog;

    d->dialog = new KDialog( d->parentDialog->parentWidget(), Qt::Tool );
    d->dialog->resize( d->parentDialog->width(), 20 );
    d->dialog->move( d->parentDialog->pos() );
    d->dialog->setButtons( 0 );
    d->dialog->setModal( false );

    if ( d->selectionMode == SingleCell )
    {
      d->dialog->setCaption( i18n("Select Single Cell") );
    }
    else // if ( d->selectionMode == MultipleCells )
    {
      d->dialog->setCaption( i18n("Select Multiple Cells") );
    }

    QWidget* widget = new QWidget( d->dialog );
    QHBoxLayout* layout = new QHBoxLayout( widget );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    layout->addWidget( d->textEdit );
    layout->addWidget( d->button );

    d->dialog->setMainWidget( widget );
    d->dialog->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    d->dialog->installEventFilter( this );
    d->dialog->layout()->setMargin( 0 );
    d->dialog->layout()->setSpacing( 0 );
    d->dialog->show();
    d->parentDialog->hide();
  }
  else
  {
    d->displayMode = Widget;

    layout()->addWidget( d->textEdit );
    layout()->addWidget( d->button );

    d->parentDialog->move( d->dialog->pos() );
    d->parentDialog->show();
    delete d->dialog;
    d->dialog = 0;
  }
}

void RegionSelector::choiceChanged()
{
  if ( Private::s_focussedSelector != this )
    return;

  if ( d->view->choice()->isValid() )
  {
    QString area = d->view->choice()->name();
    d->textEdit->setPlainText( area );
  }
}

#include "Editors.moc"
