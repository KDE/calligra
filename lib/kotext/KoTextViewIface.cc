/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KoTextViewIface.h"
#include "kotextview.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <kdebug.h>
#include "koborder.h"
#include <koVariable.h>
#include <kcommand.h>

KoTextViewIface::KoTextViewIface( KoTextView *_textview )
    : DCOPObject( /*TODO name*/ )
{
   m_textView = _textview;
}

void KoTextViewIface::insertSoftHyphen()
{
   m_textView->insertSoftHyphen();
}

void KoTextViewIface::insertText( const QString &text )
{
    m_textView->insertText(text);
}


void KoTextViewIface::setBold(bool b)
{
    KCommand *cmd=m_textView->setBoldCommand(b);
    delete cmd;
}

void KoTextViewIface::setItalic(bool on)
{
    KCommand *cmd=m_textView->setItalicCommand(on);
    delete cmd;
}

void KoTextViewIface::setUnderline(bool on)
{
    KCommand *cmd=m_textView->setUnderlineCommand(on);
    delete cmd;
}

void KoTextViewIface::setDoubleUnderline(bool on)
{
    KCommand *cmd=m_textView->setDoubleUnderlineCommand(on);
    delete cmd;
}


void KoTextViewIface::setStrikeOut(bool on)
{
    KCommand *cmd=m_textView->setStrikeOutCommand(on);
    delete cmd;
}

void KoTextViewIface::setPointSize( int s )
{
    KCommand *cmd=m_textView->setPointSizeCommand(s);
    delete cmd;
}

void KoTextViewIface::setTextSubScript(bool on)
{
    KCommand *cmd=m_textView->setTextSubScriptCommand(on);
    delete cmd;
}

void KoTextViewIface::setTextSuperScript(bool on)
{
    KCommand *cmd=m_textView->setTextSuperScriptCommand(on);
    delete cmd;
}

void KoTextViewIface::setSpellCheckingLanguage(const QString & _lang)
{
    KCommand *cmd=m_textView->setSpellCheckingLanguage(_lang);
    delete cmd;
}

QString KoTextViewIface::spellCheckingLanguage() const
{
    return m_textView->spellCheckingLanguage();
}

void KoTextViewIface::setDefaultFormat()
{
    KCommand *cmd=m_textView->setDefaultFormatCommand();
    delete cmd;
}

QColor KoTextViewIface::textColor() const
{
    return m_textView->textColor();
}

QString KoTextViewIface::textFontFamily()const
{
    return m_textView->textFontFamily();
}

QColor KoTextViewIface::textBackgroundColor()const
{
    return m_textView->textBackgroundColor();
}

QColor KoTextViewIface::textUnderlineColor() const
{
    return m_textView->textUnderlineColor();
}

bool KoTextViewIface::textShadow() const
{
    return m_textView->textShadow();
}

void KoTextViewIface::setTextShadow( bool _shadow)
{
    KCommand *cmd=m_textView->setShadowTextCommand(_shadow);
    delete cmd;
}

double KoTextViewIface::relativeTextSize() const
{
    return m_textView->relativeTextSize();
}

void KoTextViewIface::setRelativeTextSize( double _size)
{
    KCommand *cmd=m_textView->setRelativeTextSizeCommand(_size);
    delete cmd;
}

void KoTextViewIface::setUnderlineColor( const QColor & color )
{
    KCommand *cmd=m_textView->setUnderlineColorCommand(color);
    delete cmd;
}

void KoTextViewIface::setTextColor(const QColor &color)
{
    KCommand *cmd=m_textView->setTextColorCommand(color);
    delete cmd;
}

void KoTextViewIface::setTextBackgroundColor(const QColor &color)
{
    KCommand *cmd=m_textView->setTextBackgroundColorCommand(color);
    delete cmd;
}

void KoTextViewIface::setAlign(int align)
{
    KCommand *cmd=m_textView->setAlignCommand(align);
    delete cmd;
}

void KoTextViewIface::setAlign(const QString &align)
{
    KCommand *cmd=0L;
    if( align=="AlignLeft")
        cmd=m_textView->setAlignCommand(Qt::AlignLeft);
    else if (align=="AlignRight")
        cmd=m_textView->setAlignCommand(Qt::AlignRight);
    else if (align=="AlignCenter")
        cmd=m_textView->setAlignCommand(Qt::AlignCenter);
    else if (align=="AlignJustify")
        cmd=m_textView->setAlignCommand(Qt::AlignJustify);
    else
    {
        kdDebug(32500)<<"Align value not recognized...\n";
        cmd=m_textView->setAlignCommand(Qt::AlignLeft);
    }
    delete cmd;
}

bool KoTextViewIface::textDoubleUnderline() const
{
    return m_textView->textDoubleUnderline();
}

bool KoTextViewIface::textItalic() const
{
    return m_textView->textItalic();
}

bool KoTextViewIface::textBold() const
{
    return m_textView->textBold();
}

bool KoTextViewIface::textUnderline()const
{
    return m_textView->textUnderline();
}

bool KoTextViewIface::textStrikeOut()const
{
    return m_textView->textStrikeOut();
}

bool KoTextViewIface::textSubScript() const
{
    return m_textView->textSubScript();
}

bool KoTextViewIface::textSuperScript() const
{
    return m_textView->textSuperScript();
}

bool KoTextViewIface::isReadWrite() const
{
    return m_textView->isReadWrite();
}

void KoTextViewIface::setReadWrite( bool b )
{
    m_textView->setReadWrite(b);
}

void KoTextViewIface::hideCursor()
{
    m_textView->hideCursor();
}

void KoTextViewIface::showCursor()
{
    m_textView->showCursor();
}

void KoTextViewIface::moveCursorLeft(bool select)
{
  m_textView->moveCursor(KoTextView::MoveBackward,select);
}

void KoTextViewIface::moveCursorRight(bool select)
{
  m_textView->moveCursor(KoTextView::MoveForward,select);
}

void KoTextViewIface::moveCursorUp(bool select)
{
  m_textView->moveCursor(KoTextView::MoveUp,select);
}

void KoTextViewIface::moveCursorDown(bool select)
{
  m_textView->moveCursor(KoTextView::MoveDown,select);
}

void KoTextViewIface::moveCursorHome(bool select)
{
  m_textView->moveCursor(KoTextView::MoveHome,select);
}

void KoTextViewIface::moveCursorEnd(bool select)
{
  m_textView->moveCursor(KoTextView::MoveEnd,select);
}

void KoTextViewIface::moveCursorWordLeft(bool select)
{
  m_textView->moveCursor(KoTextView::MoveWordBackward,select);
}

void KoTextViewIface::moveCursorWordRight(bool select)
{
  m_textView->moveCursor(KoTextView::MoveWordForward,select);
}

void KoTextViewIface::moveCursorLineEnd(bool select)
{
  m_textView->moveCursor(KoTextView::MoveLineEnd,select);
}

void KoTextViewIface::moveCursorLineStart(bool select)
{
  m_textView->moveCursor(KoTextView::MoveLineStart,select);
}

QColor KoTextViewIface::paragraphShadowColor() const
{
    return m_textView->cursor()->parag()->shadowColor();
}

bool KoTextViewIface::paragraphHasBorder() const
{
    return m_textView->cursor()->parag()->hasBorder();
}

double KoTextViewIface::lineSpacing() const
{
    return m_textView->cursor()->parag()->kwLineSpacing();
}

double KoTextViewIface::leftMargin() const
{
    return m_textView->cursor()->parag()->margin( QStyleSheetItem::MarginLeft);
}

double KoTextViewIface::rightMargin() const
{
    return m_textView->cursor()->parag()->margin( QStyleSheetItem::MarginRight);
}

double KoTextViewIface::spaceBeforeParag() const
{
    return m_textView->cursor()->parag()->margin( QStyleSheetItem::MarginTop);
}

double KoTextViewIface::spaceAfterParag() const
{
    return m_textView->cursor()->parag()->margin( QStyleSheetItem::MarginBottom);
}

double KoTextViewIface::marginFirstLine() const
{
    return m_textView->cursor()->parag()->margin( QStyleSheetItem::MarginFirstLine);
}

void KoTextViewIface::setMarginFirstLine(double pt)
{
    m_textView->cursor()->parag()->setMargin( QStyleSheetItem::MarginFirstLine,pt);
}

void KoTextViewIface::setLineSpacing(double pt)
{
    m_textView->cursor()->parag()->setLineSpacing(pt);
}

void KoTextViewIface::setLeftMargin(double pt)
{
    m_textView->cursor()->parag()->setMargin( QStyleSheetItem::MarginLeft,pt);
}

void KoTextViewIface::setRightMargin(double pt)
{
    m_textView->cursor()->parag()->setMargin( QStyleSheetItem::MarginRight,pt);
}

void KoTextViewIface::setSpaceBeforeParag(double pt)
{
    m_textView->cursor()->parag()->setMargin( QStyleSheetItem::MarginTop,pt);
}

void KoTextViewIface::setSpaceAfterParag(double pt)
{
    m_textView->cursor()->parag()->setMargin( QStyleSheetItem::MarginBottom,pt);
}


void KoTextViewIface::setLeftBorder( const QColor & c,double width )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->setLeftBorder(KoBorder( c, KoBorder::SOLID, width ));

}

void KoTextViewIface::setRightBorder( const QColor & c,double width )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->setRightBorder(KoBorder( c, KoBorder::SOLID, width ));
}

void KoTextViewIface::setTopBorder( const QColor & c,double width )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->setTopBorder(KoBorder( c, KoBorder::SOLID, width ));
}

void KoTextViewIface::setBottomBorder(const QColor & c,double width )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->setBottomBorder(KoBorder( c, KoBorder::SOLID, width ));
}

double KoTextViewIface::leftBorderWidth() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return  parag->leftBorder().width();
}

double KoTextViewIface::rightBorderWidth() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return parag->rightBorder().width();

}
double KoTextViewIface::topBorderWidth() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return parag->topBorder().width();

}

double KoTextViewIface::bottomBorderWidth() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return parag->bottomBorder().width();

}

QColor KoTextViewIface::leftBorderColor() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return parag->leftBorder().color;
}

QColor KoTextViewIface::rightBorderColor() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return parag->rightBorder().color;
}

QColor KoTextViewIface::topBorderColor() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return parag->topBorder().color;
}

QColor KoTextViewIface::bottomBorderColor() const
{
    KoTextParag *parag= m_textView->cursor()->parag();
    return parag->bottomBorder().color;
}

void KoTextViewIface::setLeftBorderColor( const QColor & c )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->leftBorder().color = c ;
}

void KoTextViewIface::setRightBorderColor( const QColor & c )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->rightBorder().color = c ;
}

void KoTextViewIface::setTopBorderColor( const QColor & c )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->topBorder().color = c ;
}

void KoTextViewIface::setBottomBorderColor(const QColor & c )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->bottomBorder().color = c ;
}

void KoTextViewIface::setLeftBorderWidth( double _witdh )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->leftBorder().setPenWidth(_witdh) ;
}

void KoTextViewIface::setRightBorderWidth( double _witdh )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->rightBorder().setPenWidth(_witdh) ;
}

void KoTextViewIface::setTopBorderWidth( double _witdh )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->topBorder().setPenWidth(_witdh) ;
}

void KoTextViewIface::setBottomBorderWidth( double _witdh )
{
    KoTextParag *parag= m_textView->cursor()->parag();
    parag->bottomBorder().setPenWidth(_witdh) ;
}


void KoTextViewIface::changeCaseOfText( const QString & caseType)
{
    KCommand *cmd=0L;
    if( caseType.lower() == "uppercase" )
    {
        cmd=m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::UpperCase );
    }
    else if( caseType.lower() =="lowercase" )
    {
        cmd=m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::LowerCase );
    }
    else if( caseType.lower() =="titlecase" )
    {
        cmd=m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::TitleCase );
    }
    else if( caseType.lower() =="togglecase" )
    {
        cmd=m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::ToggleCase );
    }
    else if ( caseType.lower() =="sentencecase" )
    {
        cmd=m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::SentenceCase );

    }
    else
        kdDebug(32500)<<"Error in void KoTextViewIface::changeCaseOfText( const QString & caseType) parameter\n";
    delete cmd;
}

bool KoTextViewIface::isALinkVariable() const
{
    return (m_textView->linkVariable()!=0);
}

QString KoTextViewIface::linkVariableUrl( ) const
{
    KoLinkVariable *var=m_textView->linkVariable();
    if ( !var)
        return QString::null;
    else
    {
        return var->url();
    }
}

QString KoTextViewIface::linkVariableName( ) const
{
    KoLinkVariable *var=m_textView->linkVariable();
    if ( !var)
        return QString::null;
    else
    {
        return var->value();
    }
}


bool KoTextViewIface::changeLinkVariableUrl( const QString & _url) const
{
    KoLinkVariable *var=m_textView->linkVariable();
    if ( !var)
        return false;
    else
    {
        var->setLink(var->value(), _url);
        var->recalc();
    }
    return true;
}

bool KoTextViewIface::changeLinkVariableName( const QString & _name) const
{
    KoLinkVariable *var=m_textView->linkVariable();
    if ( !var)
        return false;
    else
    {
        var->setLink(_name, var->url());
        var->recalc();
    }
    return true;
}

bool KoTextViewIface::isANoteVariable() const
{
    KoNoteVariable *var = dynamic_cast<KoNoteVariable *>(m_textView->variable());
    return (var!=0);
}

QString KoTextViewIface::noteVariableText() const
{
    KoNoteVariable *var = dynamic_cast<KoNoteVariable *>(m_textView->variable());
    if( var )
        return var->note();
    else
        return QString::null;
}

bool KoTextViewIface::setNoteVariableText(const QString & note) const
{
    KoNoteVariable *var = dynamic_cast<KoNoteVariable *>(m_textView->variable());
    if( var )
    {
        var->setNote( note);
        return true;
    }
    else
        return false;
}

void KoTextViewIface::removeComment()
{
    removeComment();
}

QString KoTextViewIface::underlineLineStyle() const
{
    QString strLineType;
    switch ( m_textView->underlineLineStyle() )
    {
    case KoTextFormat::U_SOLID:
        strLineType ="solid";
        break;
    case KoTextFormat::U_DASH:
        strLineType ="dash";
        break;
    case KoTextFormat::U_DOT:
        strLineType ="dot";
        break;
    case KoTextFormat::U_DASH_DOT:
        strLineType="dashdot";
        break;
    case KoTextFormat::U_DASH_DOT_DOT:
        strLineType="dashdotdot";
        break;
    }
    return strLineType;

}

QString KoTextViewIface::strikeOutLineStyle()const
{
    QString strLineType;
    switch ( m_textView->strikeOutLineStyle() )
    {
    case KoTextFormat::S_SOLID:
        strLineType ="solid";
        break;
    case KoTextFormat::S_DASH:
        strLineType ="dash";
        break;
    case KoTextFormat::S_DOT:
        strLineType ="dot";
        break;
    case KoTextFormat::S_DASH_DOT:
        strLineType="dashdot";
        break;
    case KoTextFormat::S_DASH_DOT_DOT:
        strLineType="dashdotdot";
        break;
    }
    return strLineType;

}

void KoTextViewIface::addBookmarks(const QString &url)
{
    m_textView->addBookmarks(url);
}

void KoTextViewIface::copyLink()
{
    m_textView->copyLink();
}

void KoTextViewIface::removeLink()
{
    m_textView->removeLink();
}
