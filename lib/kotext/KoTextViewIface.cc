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
    m_textView->setBold(b);
}

void KoTextViewIface::setItalic(bool on)
{
    m_textView->setItalic(on);
}

void KoTextViewIface::setUnderline(bool on)
{
    m_textView->setUnderline(on);
}

void KoTextViewIface::setStrikeOut(bool on)
{
    m_textView->setStrikeOut(on);
}

void KoTextViewIface::setPointSize( int s )
{
    m_textView->setPointSize(s);
}

void KoTextViewIface::setTextSubScript(bool on)
{
    m_textView->setTextSubScript(on);
}

void KoTextViewIface::setTextSuperScript(bool on)
{
    m_textView->setTextSuperScript(on);
}

void KoTextViewIface::setDefaultFormat()
{
    m_textView->setDefaultFormat();
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
    m_textView->setTextColor(color);
}

void KoTextViewIface::setTextBackgroundColor(const QColor &color)
{
    m_textView->setTextBackgroundColor(color);
}

void KoTextViewIface::setAlign(int align)
{
    m_textView->setAlign(align);
}

void KoTextViewIface::setAlign(const QString &align)
{
    if( align=="AlignLeft")
        m_textView->setAlign(Qt::AlignLeft);
    else if (align=="AlignRight")
        m_textView->setAlign(Qt::AlignRight);
    else if (align=="AlignCenter")
        m_textView->setAlign(Qt::AlignCenter);
    else if (align=="AlignJustify")
        m_textView->setAlign(Qt::AlignJustify);
    else
    {
        kdDebug()<<"Align value don't recognize...\n";
        m_textView->setAlign(Qt::AlignLeft);
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
