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

KoTextViewIface::KoTextViewIface( KoTextView *_textview )
    : DCOPObject()
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
    // ### leaks the commands away
    m_textView->setBoldCommand(b);
}

void KoTextViewIface::setItalic(bool on)
{
    m_textView->setItalicCommand(on);
}

void KoTextViewIface::setUnderline(bool on)
{
    m_textView->setUnderlineCommand(on);
}

void KoTextViewIface::setStrikeOut(bool on)
{
    m_textView->setStrikeOutCommand(on);
}

void KoTextViewIface::setPointSize( int s )
{
    m_textView->setPointSizeCommand(s);
}

void KoTextViewIface::setTextSubScript(bool on)
{
    m_textView->setTextSubScriptCommand(on);
}

void KoTextViewIface::setTextSuperScript(bool on)
{
    m_textView->setTextSuperScriptCommand(on);
}

void KoTextViewIface::setDefaultFormat()
{
    m_textView->setDefaultFormatCommand();
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

void KoTextViewIface::setTextColor(const QColor &color)
{
    m_textView->setTextColorCommand(color);
}

void KoTextViewIface::setTextBackgroundColor(const QColor &color)
{
    m_textView->setTextBackgroundColorCommand(color);
}

void KoTextViewIface::setAlign(int align)
{
    m_textView->setAlignCommand(align);
}

void KoTextViewIface::setAlign(const QString &align)
{
    if( align=="AlignLeft")
        m_textView->setAlignCommand(Qt::AlignLeft);
    else if (align=="AlignRight")
        m_textView->setAlignCommand(Qt::AlignRight);
    else if (align=="AlignCenter")
        m_textView->setAlignCommand(Qt::AlignCenter);
    else if (align=="AlignJustify")
        m_textView->setAlignCommand(Qt::AlignJustify);
    else
    {
        kdDebug()<<"Align value not recognized...\n";
        m_textView->setAlignCommand(Qt::AlignLeft);
    }
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
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->shadowColor();
}

bool KoTextViewIface::paragraphHasBorder() const
{
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->hasBorder();
}

double KoTextViewIface::lineSpacing() const
{
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->kwLineSpacing();
}

double KoTextViewIface::leftMargin() const
{
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->margin( QStyleSheetItem::MarginLeft);
}

double KoTextViewIface::rightMargin() const
{
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->margin( QStyleSheetItem::MarginRight);
}

double KoTextViewIface::spaceBeforeParag() const
{
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->margin( QStyleSheetItem::MarginTop);
}

double KoTextViewIface::spaceAfterParag() const
{
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->margin( QStyleSheetItem::MarginBottom);
}

double KoTextViewIface::marginFirstLine() const
{
    return static_cast<KoTextParag *>(m_textView->cursor()->parag())->margin( QStyleSheetItem::MarginFirstLine);
}



void KoTextViewIface::setMarginFirstLine(double pt)
{
    static_cast<KoTextParag *>(m_textView->cursor()->parag())->setMargin( QStyleSheetItem::MarginFirstLine,pt);
}

void KoTextViewIface::setLineSpacing(double pt)
{
    static_cast<KoTextParag *>(m_textView->cursor()->parag())->setLineSpacing(pt);
}

void KoTextViewIface::setLeftMargin(double pt)
{
    static_cast<KoTextParag *>(m_textView->cursor()->parag())->setMargin( QStyleSheetItem::MarginLeft,pt);
}

void KoTextViewIface::setRightMargin(double pt)
{
    static_cast<KoTextParag *>(m_textView->cursor()->parag())->setMargin( QStyleSheetItem::MarginRight,pt);
}

void KoTextViewIface::setSpaceBeforeParag(double pt)
{
    static_cast<KoTextParag *>(m_textView->cursor()->parag())->setMargin( QStyleSheetItem::MarginTop,pt);
}

void KoTextViewIface::setSpaceAfterParag(double pt)
{
    static_cast<KoTextParag *>(m_textView->cursor()->parag())->setMargin( QStyleSheetItem::MarginBottom,pt);
}


void KoTextViewIface::setLeftBorder( const QColor & c,double width )
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    parag->setLeftBorder(KoBorder( c, KoBorder::SOLID, width ));

}

void KoTextViewIface::setRightBorder( const QColor & c,double width )
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    parag->setRightBorder(KoBorder( c, KoBorder::SOLID, width ));
}

void KoTextViewIface::setTopBorder( const QColor & c,double width )
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    parag->setTopBorder(KoBorder( c, KoBorder::SOLID, width ));
}

void KoTextViewIface::setBottomBorder(const QColor & c,double width )
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    parag->setBottomBorder(KoBorder( c, KoBorder::SOLID, width ));
}

double KoTextViewIface::leftBorderWidth() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return  parag->leftBorder().ptWidth;
}

double KoTextViewIface::rightBorderWidth() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return parag->rightBorder().ptWidth;

}
double KoTextViewIface::topBorderWidth() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return parag->topBorder().ptWidth;

}

double KoTextViewIface::bottomBorderWidth() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return parag->bottomBorder().ptWidth;

}

QColor KoTextViewIface::leftBorderColor() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return parag->leftBorder().color;
}

QColor KoTextViewIface::rightBorderColor() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return parag->rightBorder().color;

}

QColor KoTextViewIface::topBorderColor() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return parag->topBorder().color;
}

QColor KoTextViewIface::bottomBorderColor() const
{
    KoTextParag *parag= static_cast<KoTextParag *>(m_textView->cursor()->parag());
    return parag->bottomBorder().color;

}

void KoTextViewIface::changeCaseOfText( const QString & caseType)
{
    if( caseType.lower() == "uppercase" )
    {
        m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::UpperCase );
    }
    else if( caseType.lower() =="lowercase" )
    {
        m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::LowerCase );
    }
    else if( caseType.lower() =="titlecase" )
    {
        m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::TitleCase );
    }
    else if( caseType.lower() =="togglecase" )
    {
        m_textView->setChangeCaseOfTextCommand( KoChangeCaseDia::ToggleCase );
    }
    else
        kdDebug()<<"Error in void KoTextViewIface::changeCaseOfText( const QString & caseType) parameter\n";
}

bool KoTextViewIface::isALinkVariable() const
{
    return (m_textView->linkVariable()!=0);
}

bool KoTextViewIface::changeLinkVariableUrl( const QString & _url)
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

bool KoTextViewIface::changeLinkVariableName( const QString & _name)
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

bool KoTextViewIface::setNoteVariableText(const QString & note)
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
