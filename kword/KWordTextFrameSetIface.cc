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

#include "KWordTextFrameSetIface.h"
#include "KWordViewIface.h"
#include "kwtextframeset.h"
#include <KoTextViewIface.h>
#include "kwview.h"
#include <kotextobject.h>
#include "kwcanvas.h"
#include "kwdoc.h"
#include <kapplication.h>
#include <dcopclient.h>
#include "KWordFrameSetIface.h"

KWordTextFrameSetIface::KWordTextFrameSetIface( KWTextFrameSet *_frame )
    : KWordFrameSetIface( _frame)
{
   m_frametext = _frame;
}

DCOPRef KWordTextFrameSetIface::startEditing()
{
    KWDocument *doc=m_frametext->kWordDocument();
    QPtrList <KWView> lst=doc->getAllViews();
    lst.at(0)->getGUI()->canvasWidget()->checkCurrentTextEdit(m_frametext);
    return DCOPRef( kapp->dcopClient()->appId(),
		    (static_cast<KWTextFrameSetEdit *>( lst.at(0)->getGUI()->canvasWidget()->currentFrameSetEdit()))->dcopObject()->objId() );
}

bool KWordTextFrameSetIface::hasSelection() const
{
    return m_frametext->hasSelection();
}

int KWordTextFrameSetIface::numberOfParagraphs()
{
    return m_frametext->paragraphs();
}

QString KWordTextFrameSetIface::name() const
{
    return m_frametext->getName();
}

QString KWordTextFrameSetIface::selectedText() const
{
    return m_frametext->selectedText();
}

void KWordTextFrameSetIface::selectAll( bool select )
{
    m_frametext->textObject()->selectAll(select);
}

int KWordTextFrameSetIface::paragraphsSelected()
{
    return m_frametext->paragraphsSelected();
}

void KWordTextFrameSetIface::setBoldText( bool b )
{
    m_frametext->textObject()->setBoldCommand( b );
}

void KWordTextFrameSetIface::setItalicText( bool b )
{
    m_frametext->textObject()->setItalicCommand(b);
}

void KWordTextFrameSetIface::setUnderlineText( bool b )
{
    m_frametext->textObject()->setUnderlineCommand(b);
}

void KWordTextFrameSetIface::setStrikeOutText( bool b )
{
    m_frametext->textObject()->setStrikeOutCommand(b);
}

void KWordTextFrameSetIface::setTextColor( const QColor &col )
{
    m_frametext->textObject()->setTextColorCommand(col);
}

void KWordTextFrameSetIface::setTextPointSize( int s )
{
    m_frametext->textObject()->setPointSizeCommand( s );
}

void KWordTextFrameSetIface::setTextSubScript( bool b )
{
    m_frametext->textObject()->setTextSubScriptCommand( b );
}

void KWordTextFrameSetIface::setTextSuperScript( bool b )
{
    m_frametext->textObject()->setTextSuperScriptCommand( b );
}

void KWordTextFrameSetIface::setTextDefaultFormat()
{
    m_frametext->textObject()->setDefaultFormatCommand();
}

void KWordTextFrameSetIface::setTextBackgroundColor(const QColor & col)
{
    m_frametext->textObject()->setTextBackgroundColorCommand(col);
}

QColor KWordTextFrameSetIface::textColor() const
{
    return m_frametext->textObject()->textColor();
}

QFont KWordTextFrameSetIface::textFont() const
{
    return m_frametext->textObject()->textFont();
}

QString KWordTextFrameSetIface::textFontFamily()const
{
    return m_frametext->textObject()->textFontFamily();
}

QColor KWordTextFrameSetIface::textBackgroundColor() const
{
    return m_frametext->textObject()->textBackgroundColor();
}

void KWordTextFrameSetIface::setTextFamilyFont(const QString &font)
{
    m_frametext->textObject()->setFamilyCommand(font);
}

