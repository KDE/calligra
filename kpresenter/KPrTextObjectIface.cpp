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

KPTextObjectIface::KPTextObjectIface( KPTextObject *_textobject )
    : KPresenterObject2DIface(_textobject)
{
    m_textobject = _textobject;
}

DCOPRef KPTextObjectIface::startEditing()
{
    KPrDocument *doc=m_textobject->kPresenterDocument();
    KPresenterView *view=doc->firstView();
    view->getCanvas()->createEditing( m_textobject);
    return DCOPRef( kapp->dcopClient()->appId(),
                    view->getCanvas()->currentTextObjectView()->dcopObject()->objId() );
}

bool KPTextObjectIface::hasSelection() const
{
    return m_textobject->textObject()->hasSelection();
}

QString KPTextObjectIface::selectedText() const
{
    return m_textobject->textObject()->selectedText();
}

void KPTextObjectIface::selectAll( bool select )
{
    m_textobject->textObject()->selectAll(select);
}

void KPTextObjectIface::recalcPageNum( )
{
    //FIXME
    //m_textobject->recalcPageNum(m_textobject->kPresenterDocument());
}

void KPTextObjectIface::setBoldText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setBoldCommand( b );
    delete cmd;
}

void KPTextObjectIface::setItalicText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setItalicCommand(b);
    delete cmd;
}

void KPTextObjectIface::setUnderlineText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setUnderlineCommand(b);
    delete cmd;
}

void KPTextObjectIface::setDoubleUnderlineText(bool b)
{
    KCommand *cmd=m_textobject->textObject()->setDoubleUnderlineCommand(b);
    delete cmd;
}

void KPTextObjectIface::setUnderlineColor( const QColor & color )
{
    KCommand *cmd=m_textobject->textObject()->setUnderlineColorCommand( color );
    delete cmd;
}

void KPTextObjectIface::setStrikeOutText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setStrikeOutCommand(b);
    delete cmd;
}

void KPTextObjectIface::setTextColor( const QColor &col )
{
    KCommand *cmd=m_textobject->textObject()->setTextColorCommand(col);
    delete cmd;
}

void KPTextObjectIface::setTextPointSize( int s )
{
    KCommand *cmd=m_textobject->textObject()->setPointSizeCommand( s );
    delete cmd;
}

void KPTextObjectIface::setTextSubScript( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setTextSubScriptCommand( b );
    delete cmd;
}

void KPTextObjectIface::setTextSuperScript( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setTextSuperScriptCommand( b );
    delete cmd;
}

void KPTextObjectIface::setTextDefaultFormat()
{
    KCommand *cmd=m_textobject->textObject()->setDefaultFormatCommand();
    delete cmd;
}

void KPTextObjectIface::setTextBackgroundColor(const QColor & col)
{
    KCommand *cmd=m_textobject->textObject()->setTextBackgroundColorCommand(col);
    delete cmd;
}

bool KPTextObjectIface::textDoubleUnderline()const
{
    return m_textobject->textObject()->textDoubleUnderline();
}

QColor KPTextObjectIface::textUnderlineColor() const
{
    return m_textobject->textObject()->textUnderlineColor();
}

QColor KPTextObjectIface::textColor() const
{
    return m_textobject->textObject()->textColor();
}

QFont KPTextObjectIface::textFont() const
{
    return m_textobject->textObject()->textFont();
}

QString KPTextObjectIface::textFontFamily()const
{
    return m_textobject->textObject()->textFontFamily();
}

QColor KPTextObjectIface::textBackgroundColor() const
{
    return m_textobject->textObject()->textBackgroundColor();
}

bool KPTextObjectIface::textItalic() const
{
    return m_textobject->textObject()->textItalic();
}

bool KPTextObjectIface::textBold() const
{
    return m_textobject->textObject()->textBold();
}

bool KPTextObjectIface::textUnderline()const
{
    return m_textobject->textObject()->textUnderline();
}

bool KPTextObjectIface::textStrikeOut()const
{
    return m_textobject->textObject()->textStrikeOut();
}

bool KPTextObjectIface::textSubScript() const
{
    return m_textobject->textObject()->textSubScript();
}

bool KPTextObjectIface::textSuperScript() const
{
    return m_textobject->textObject()->textSuperScript();
}

void KPTextObjectIface::setTextFamilyFont(const QString &font)
{
    KCommand *cmd=m_textobject->textObject()->setFamilyCommand(font);
    delete cmd;
}

void KPTextObjectIface::changeCaseOfText( const QString & caseType)
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

void KPTextObjectIface::extendTextContentsToHeight()
{
    KCommand *cmd=m_textobject->textContentsToHeight();
    delete cmd;
}

void KPTextObjectIface::extendTextObjectToContents()
{
    KCommand *cmd= m_textobject->textObjectToContents();
    delete cmd;
}

void KPTextObjectIface::setProtectContent ( bool _protect )
{
    m_textobject->setProtectContent( _protect );
}

bool KPTextObjectIface::isProtectContent() const
{
    return m_textobject->isProtectContent();
}

void KPTextObjectIface::setPtMarginLeft(double val)
{
    m_textobject->setBLeft(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPTextObjectIface::setPtMarginRight(double val)
{
    m_textobject->setBRight(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPTextObjectIface::setPtMarginTop(double val)
{
    m_textobject->setBTop(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPTextObjectIface::setPtMarginBottom(double val)
{
    m_textobject->setBBottom(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

double KPTextObjectIface::ptMarginLeft()const
{
    return m_textobject->bLeft();
}

double KPTextObjectIface::ptMarginRight()const
{
    return m_textobject->bRight();
}

double KPTextObjectIface::ptMarginTop()const
{
    return m_textobject->bTop();
}

double KPTextObjectIface::ptMarginBottom()const
{
    return m_textobject->bBottom();
}

void KPTextObjectIface::setVerticalAligment( const QString & type)
{
    if ( type.lower() =="center" )
        m_textobject->setVerticalAligment( KP_CENTER );
    else if ( type.lower() =="top" )
        m_textobject->setVerticalAligment( KP_TOP );
    else if ( type.lower() =="bottom" )
        m_textobject->setVerticalAligment( KP_BOTTOM );
}

QString KPTextObjectIface::verticalAlignment() const
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

//bool KPTextObjectIface::textShadow() const
//{
    //return m_textobject->textObject()->textShadow();
//}

//void KPTextObjectIface::setTextShadow( bool b )
//{
    //KCommand *cmd=m_textobject->textObject()->setShadowTextCommand( b );
    //delete cmd;
//}

double KPTextObjectIface::relativeTextSize() const
{
    return m_textobject->textObject()->relativeTextSize();
}

void KPTextObjectIface::setRelativeTextSize( double _size )
{
    KCommand *cmd=m_textobject->textObject()->setRelativeTextSizeCommand(_size );
    delete cmd;
}

bool KPTextObjectIface::wordByWord() const
{
    return m_textobject->textObject()->wordByWord();
}

void KPTextObjectIface::setWordByWord( bool _b )
{
    KCommand *cmd=m_textobject->textObject()->setWordByWordCommand(_b );
    delete cmd;
}

QString KPTextObjectIface::fontAttribute()const
{
    return KoTextFormat::attributeFontToString( m_textobject->textObject()->fontAttribute() );
}

QString KPTextObjectIface::underlineLineStyle() const
{
    return KoTextFormat::underlineStyleToString( m_textobject->textObject()->underlineStyle() );
}

QString KPTextObjectIface::strikeOutLineStyle()const
{
    return KoTextFormat::strikeOutStyleToString( m_textobject->textObject()->strikeOutStyle() );
}
