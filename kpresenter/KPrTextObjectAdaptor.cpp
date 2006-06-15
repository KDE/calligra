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

#include "KPrTextObjectAdaptor.h"
#include "KPrTextObject.h"
#include "KPrDocument.h"
#include "KPrView.h"
#include <KoTextObject.h>
#include <kapplication.h>
#include "KPrCanvas.h"
#include <kdebug.h>
#include <kcommand.h>

KPrTextObjectAdaptor::KPrTextObjectAdaptor( KPrTextObject *_textobject )
    : KPrObject2DAdaptor(_textobject)
{
    m_textobject = _textobject;
}

QString KPrTextObjectAdaptor::startEditing()
{
    KPrDocument *doc=m_textobject->kPresenterDocument();
    KPrView *view=doc->firstView();
    view->getCanvas()->createEditing( m_textobject);
    return view->getCanvas()->currentTextObjectView()->objectName();
}

bool KPrTextObjectAdaptor::hasSelection() const
{
    return m_textobject->textObject()->hasSelection();
}

QString KPrTextObjectAdaptor::selectedText() const
{
    return m_textobject->textObject()->selectedText();
}

void KPrTextObjectAdaptor::selectAll( bool select )
{
    m_textobject->textObject()->selectAll(select);
}

void KPrTextObjectAdaptor::recalcPageNum( )
{
    //FIXME
    //m_textobject->recalcPageNum(m_textobject->kPresenterDocument());
}

void KPrTextObjectAdaptor::setBoldText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setBoldCommand( b );
    delete cmd;
}

void KPrTextObjectAdaptor::setItalicText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setItalicCommand(b);
    delete cmd;
}

void KPrTextObjectAdaptor::setUnderlineText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setUnderlineCommand(b);
    delete cmd;
}

void KPrTextObjectAdaptor::setDoubleUnderlineText(bool b)
{
    KCommand *cmd=m_textobject->textObject()->setDoubleUnderlineCommand(b);
    delete cmd;
}

void KPrTextObjectAdaptor::setUnderlineColor( const QColor & color )
{
    KCommand *cmd=m_textobject->textObject()->setUnderlineColorCommand( color );
    delete cmd;
}

void KPrTextObjectAdaptor::setStrikeOutText( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setStrikeOutCommand(b);
    delete cmd;
}

void KPrTextObjectAdaptor::setTextColor( const QColor &col )
{
    KCommand *cmd=m_textobject->textObject()->setTextColorCommand(col);
    delete cmd;
}

void KPrTextObjectAdaptor::setTextPointSize( int s )
{
    KCommand *cmd=m_textobject->textObject()->setPointSizeCommand( s );
    delete cmd;
}

void KPrTextObjectAdaptor::setTextSubScript( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setTextSubScriptCommand( b );
    delete cmd;
}

void KPrTextObjectAdaptor::setTextSuperScript( bool b )
{
    KCommand *cmd=m_textobject->textObject()->setTextSuperScriptCommand( b );
    delete cmd;
}

void KPrTextObjectAdaptor::setTextDefaultFormat()
{
    KCommand *cmd=m_textobject->textObject()->setDefaultFormatCommand();
    delete cmd;
}

void KPrTextObjectAdaptor::setTextBackgroundColor(const QColor & col)
{
    KCommand *cmd=m_textobject->textObject()->setTextBackgroundColorCommand(col);
    delete cmd;
}

bool KPrTextObjectAdaptor::textDoubleUnderline()const
{
    return m_textobject->textObject()->textDoubleUnderline();
}

QColor KPrTextObjectAdaptor::textUnderlineColor() const
{
    return m_textobject->textObject()->textUnderlineColor();
}

QColor KPrTextObjectAdaptor::textColor() const
{
    return m_textobject->textObject()->textColor();
}

QFont KPrTextObjectAdaptor::textFont() const
{
    return m_textobject->textObject()->textFont();
}

QString KPrTextObjectAdaptor::textFontFamily()const
{
    return m_textobject->textObject()->textFontFamily();
}

QColor KPrTextObjectAdaptor::textBackgroundColor() const
{
    return m_textobject->textObject()->textBackgroundColor();
}

bool KPrTextObjectAdaptor::textItalic() const
{
    return m_textobject->textObject()->textItalic();
}

bool KPrTextObjectAdaptor::textBold() const
{
    return m_textobject->textObject()->textBold();
}

bool KPrTextObjectAdaptor::textUnderline()const
{
    return m_textobject->textObject()->textUnderline();
}

bool KPrTextObjectAdaptor::textStrikeOut()const
{
    return m_textobject->textObject()->textStrikeOut();
}

bool KPrTextObjectAdaptor::textSubScript() const
{
    return m_textobject->textObject()->textSubScript();
}

bool KPrTextObjectAdaptor::textSuperScript() const
{
    return m_textobject->textObject()->textSuperScript();
}

void KPrTextObjectAdaptor::setTextFamilyFont(const QString &font)
{
    KCommand *cmd=m_textobject->textObject()->setFamilyCommand(font);
    delete cmd;
}

void KPrTextObjectAdaptor::changeCaseOfText( const QString & caseType)
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
        kDebug(33001)<<"Error in void KWordTextFrameSetAdaptor::changeCaseOfText( const QString & caseType) parameter\n";
    delete cmd;
}

void KPrTextObjectAdaptor::extendTextContentsToHeight()
{
    KCommand *cmd=m_textobject->textContentsToHeight();
    delete cmd;
}

void KPrTextObjectAdaptor::extendTextObjectToContents()
{
    KCommand *cmd= m_textobject->textObjectToContents();
    delete cmd;
}

void KPrTextObjectAdaptor::setProtectContent ( bool _protect )
{
    m_textobject->setProtectContent( _protect );
}

bool KPrTextObjectAdaptor::isProtectContent() const
{
    return m_textobject->isProtectContent();
}

void KPrTextObjectAdaptor::setPtMarginLeft(double val)
{
    m_textobject->setBLeft(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPrTextObjectAdaptor::setPtMarginRight(double val)
{
    m_textobject->setBRight(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPrTextObjectAdaptor::setPtMarginTop(double val)
{
    m_textobject->setBTop(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

void KPrTextObjectAdaptor::setPtMarginBottom(double val)
{
    m_textobject->setBBottom(val);
    m_textobject->kPresenterDocument()->layout( m_textobject );
}

double KPrTextObjectAdaptor::ptMarginLeft()const
{
    return m_textobject->bLeft();
}

double KPrTextObjectAdaptor::ptMarginRight()const
{
    return m_textobject->bRight();
}

double KPrTextObjectAdaptor::ptMarginTop()const
{
    return m_textobject->bTop();
}

double KPrTextObjectAdaptor::ptMarginBottom()const
{
    return m_textobject->bBottom();
}

void KPrTextObjectAdaptor::setVerticalAligment( const QString & type)
{
    if ( type.lower() =="center" )
        m_textobject->setVerticalAligment( KP_CENTER );
    else if ( type.lower() =="top" )
        m_textobject->setVerticalAligment( KP_TOP );
    else if ( type.lower() =="bottom" )
        m_textobject->setVerticalAligment( KP_BOTTOM );
}

QString KPrTextObjectAdaptor::verticalAlignment() const
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

//bool KPrTextObjectAdaptor::textShadow() const
//{
    //return m_textobject->textObject()->textShadow();
//}

//void KPrTextObjectAdaptor::setTextShadow( bool b )
//{
    //KCommand *cmd=m_textobject->textObject()->setShadowTextCommand( b );
    //delete cmd;
//}

double KPrTextObjectAdaptor::relativeTextSize() const
{
    return m_textobject->textObject()->relativeTextSize();
}

void KPrTextObjectAdaptor::setRelativeTextSize( double _size )
{
    KCommand *cmd=m_textobject->textObject()->setRelativeTextSizeCommand(_size );
    delete cmd;
}

bool KPrTextObjectAdaptor::wordByWord() const
{
    return m_textobject->textObject()->wordByWord();
}

void KPrTextObjectAdaptor::setWordByWord( bool _b )
{
    KCommand *cmd=m_textobject->textObject()->setWordByWordCommand(_b );
    delete cmd;
}

QString KPrTextObjectAdaptor::fontAttribute()const
{
    return KoTextFormat::attributeFontToString( m_textobject->textObject()->fontAttribute() );
}

QString KPrTextObjectAdaptor::underlineLineStyle() const
{
    return KoTextFormat::underlineStyleToString( m_textobject->textObject()->underlineStyle() );
}

QString KPrTextObjectAdaptor::strikeOutLineStyle()const
{
    return KoTextFormat::strikeOutStyleToString( m_textobject->textObject()->strikeOutStyle() );
}

#include <KPrTextObjectAdaptor.moc>
