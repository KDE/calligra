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

#include "FormulaEditorHighlighter.h"

// KSpread
#include "Formula.h"
#include "Selection.h"
#include "Sheet.h"

// KOffice

// KDE
#include <kdebug.h>

// Qt
#include <QApplication>
#include <QTextEdit>

using namespace KSpread;

class FormulaEditorHighlighter::Private
{
public:
  Private()
  {
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
    , d( new Private )
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

void FormulaEditorHighlighter::highlightBlock( const QString& text )
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

#include "FormulaEditorHighlighter.moc"
