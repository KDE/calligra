// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrTextObjectIface.h"
#include "KPrTextObject.h"
#include <KoTextViewIface.h>
#include "KPrDocument.h"
#include "KPrView.h"
#include <KoTextObject.h>
#include <kapplication.h>
#include <dcopclient.h>
#include "KPrCanvas.h"
#include <kdebug.h>
#include <kcommand.h>

KPrTextObjectIface::KPrTextObjectIface( KPrTextObject *_textobject )
    : KPrObject2DIface(_textobject)
{
    m_textobject = _textobject;
}

DCOPRef KPrTextObjectIface::startEditing()
{
    KPrDocument *doc=m_textobject->kPresenterDocument();
    KPrView *view=doc->firstView();
    view->getCanvas()->createEditing( m_textobject);
    return DCOPRef( kapp->dcopClient()->appId(),
                    view->getCanvas()->currentTextObjectView()->dcopObject()->objId() );
}

bool KPrTextObjectIface::hasSelection() const
{
    return m_textobject->textObject()->hasSelection();
}

QString KPrTextObjectIface::selectedText() const
{
    return m_textobject->textObject()->selectedText();
}

void KPrTextObjectIface::selectAll( bool select )
{
    m_textobject->textObject()->selectAll(select);
}

void KPrTextObjectIface::recalcPageNum( )
{
    //FIXME
    //m_textobject->recalcPageNum(m_textobject->kPresenterDocument());
}

void KPrTextObjectIface::setBoldText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setBoldCommand( b );
    delete cmd;
}

void KPrTextObjectIface::setItalicText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setItalicCommand(b);
    delete cmd;
}

void KPrTextObjectIface::setUnderlineText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setUnderlineCommand(b);
    delete cmd;
}

void KPrTextObjectIface::setDoubleUnderlineText(bool b)
{
    KCommand *cmd=m_textobject->textObject()->setDoubleUnderlineCommand(b);
    delete cmd;
}

void KPrTextObjectIface::setUnderlineColor( const QColor & color )
{
    KCommand *cmd=m_textobject->textObject()->setUnderlineColorCommand( color );
    delete cmd;
}

void KPrTextObjectIface::setStrikeOutText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setStrikeOutCommand(b);
    delete cmd;
}

void KPrTextObjectIface::setTextColor( const QColor &col )
{
    KCommand *cmd=m_textobject->textObject()->setTextColorCommand(col);
    delete cmd;
}

void KPrTextObjectIface::setTextPointSize( int s )
{
    KCommand *cmd=m_textobject->textObject()->setPointSizeCommand( s );
    delete cmd;
}

void KPrTextObjectIface::setTextSubScript( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setTextSubScriptCommand( b );
    delete cmd;
}

void KPrTextObjectIface::setTextSuperScript( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setTextSuperScriptCommand( b );
    delete cmd;
}

void KPrTextObjectIface::setTextDefaultFormat()
{
    KCommand *cmd=m_textobject->textObject()->setDefaultFormatCommand();
    delete cmd;
}

void KPrTextObjectIface::setTextBackgroundColor(const QColor & col)
{
    KCommand *cmd=m_textobject->textObject()->setTextBackgroundColorCommand(col);
    delete cmd;
}

bool KPrTextObjectIface::textDoubleUnderline()const
{
    return m_textobject->textObject()->textDoubleUnderline();
}

QColor KPrTextObjectIface::textUnderlineColor() const
{
    return m_textobject->textObject()->textUnderlineColor();
}

QColor KPrTextObjectIface::textColor() const
{
    return m_textobject->textObject()->textColor();
}

QFont KPrTextObjectIface::textFont() const
{
    return m_textobject->textObject()->textFont();
}

QString KPrTextObjectIface::textFontFamily()const
{
    return m_textobject->textObject()->textFontFamily();
}

QColor KPrTextObjectIface::textBackgroundColor() const
{
    return m_textobject->textObject()->textBackgroundColor();
}

bool KPrTextObjectIface::textItalic() const
{
    return m_textobject->textObject()->textItalic();
}

bool KPrTextObjectIface::textBold() const
{
    return m_textobject->textObject()->textBold();
}

bool KPrTextObjectIface::textUnderline()const
{
    return m_textobject->textObject()->textUnderline();
}

bool KPrTextObjectIface::textStrikeOut()const
{
    return m_textobject->textObject()->textStrikeOut();
}

bool KPrTextObjectIface::textSubScript() const
{
    return m_textobject->textObject()->textSubScript();
}

bool KPrTextObjectIface::textSuperScript() const
{
    return m_textobject->textObject()->textSuperScript();
}

void KPrTextObjectIface::setTextFamilyFont(const QString &font)
{
    KCommand *cmd=m_textobject->textObject()->setFamilyCommand(font);
    delete cmd;
}

void KPrTextObjectIface::changeCaseOfText( const QString & caseType)
{
    KCommand *cmd = 0L;
    if( caseType.lower() == "uppercase" )
        cmd=m_textobject->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::UpperCase );
    else if( caseType.lower() =="lowercase" )
        cmd=m_textobject->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::LowerCase );
    else if( caseType.lower() =="titlecase" )
        cmd=m_textobject->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::TitleCase );
    else if( caseType.lower() =="togglecase" )
        cmd=m_textobject->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::ToggleCase );
    else if( caseType.lower() =="sentencecase" )
        cmd = m_textobject->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::SentenceCase );
    else
        kdDebug(33001)<<"Error in void KWordTextFrameSetIface::changeCaseOfText( const QString & caseType) parameter\n";
    delete cmd;
}

void KPrTextObjectIface::extendTextContentsToHeight()
{
    KCommand *cmd=m_textobject->textContentsToHeight();
    delete cmd;
}

void KPrTextObjectIface::extendTextObjectToContents()
{
    KCommand *cmd= m_textobject->textObjectToContents();
    delete cmd;
}

void KPrTextObjectIface::setProtectContent ( bool _protect )
{
    m_textobject->setProtectContent( _protect );
}

bool KPrTextObjectIface::isProtectContent() const
{
    return m_textobject->isProtectContent();
}

void KPrTextObjectIface::setPtMarginLeft(double val)
{
    m_textobject->setBLeft(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPrTextObjectIface::setPtMarginRight(double val)
{
    m_textobject->setBRight(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPrTextObjectIface::setPtMarginTop(double val)
{
    m_textobject->setBTop(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPrTextObjectIface::setPtMarginBottom(double val)
{
    m_textobject->setBBottom(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

double KPrTextObjectIface::ptMarginLeft()const
{
    return m_textobject->bLeft();
}

double KPrTextObjectIface::ptMarginRight()const
{
    return m_textobject->bRight();
}

double KPrTextObjectIface::ptMarginTop()const
{
    return m_textobject->bTop();
}

double KPrTextObjectIface::ptMarginBottom()const
{
    return m_textobject->bBottom();
}

void KPrTextObjectIface::setVerticalAligment( const QString & type)
{
    if ( type.lower() =="center" )
        m_textobject->setVerticalAligment( KP_CENTER );
    else if ( type.lower() =="top" )
        m_textobject->setVerticalAligment( KP_TOP );
    else if ( type.lower() =="bottom" )
        m_textobject->setVerticalAligment( KP_BOTTOM );
}

QString KPrTextObjectIface::verticalAlignment() const
{
    switch( m_textobject->verticalAlignment() )
    {
    case KP_CENTER:
        return QString("center");
        break;
    case KP_TOP:
        return QString("top");
        break;
    case KP_BOTTOM:
        return QString("bottom");
        break;
    default:
        break;
    }
    return QString::null;
}

//bool KPrTextObjectIface::textShadow() const
//{
    //return m_textobject->textObject()->textShadow();
//}

//void KPrTextObjectIface::setTextShadow( bool b )
//{
    //KCommand *cmd=m_textobject->textObject()->setShadowTextCommand( b );
    //delete cmd;
//}

double KPrTextObjectIface::relativeTextSize() const
{
    return m_textobject->textObject()->relativeTextSize();
}

void KPrTextObjectIface::setRelativeTextSize( double _size )
{
    KCommand *cmd=m_textobject->textObject()->setRelativeTextSizeCommand(_size );
    delete cmd;
}

bool KPrTextObjectIface::wordByWord() const
{
    return m_textobject->textObject()->wordByWord();
}

void KPrTextObjectIface::setWordByWord( bool _b )
{
    KCommand *cmd=m_textobject->textObject()->setWordByWordCommand(_b );
    delete cmd;
}

QString KPrTextObjectIface::fontAttribute()const
{
    return KoTextFormat::attributeFontToString( m_textobject->textObject()->fontAttribute() );
}

QString KPrTextObjectIface::underlineLineStyle() const
{
    return KoTextFormat::underlineStyleToString( m_textobject->textObject()->underlineStyle() );
}

QString KPrTextObjectIface::strikeOutLineStyle()const
{
    return KoTextFormat::strikeOutStyleToString( m_textobject->textObject()->strikeOutStyle() );
}
