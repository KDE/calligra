/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "cdrdocument.h"


void CdrParagraphLine::addTextSpan( CdrAbstractTextSpan* textSpan )
{
    if( (textSpan->id() == CdrAbstractTextSpan::Styled) &&
        (mTextSpans.count() > 0) && (mTextSpans.last()->id() == CdrAbstractTextSpan::Styled) )
    {
        CdrStyledTextSpan* newStyledSpan = static_cast<CdrStyledTextSpan*>( textSpan );
        CdrStyledTextSpan* lastStyledSpan = static_cast<CdrStyledTextSpan*>( mTextSpans.last() );
        if( lastStyledSpan->isFontDataEqual(*newStyledSpan) )
        {
            lastStyledSpan->appendText(newStyledSpan->text());
            delete newStyledSpan;
            return;
        }
    }

    mTextSpans.append(textSpan);
}

CdrBlockText*
CdrDocument::blockTextForObject( quint16 id )
{
    const CdrBlockTextLinkTable::ConstIterator it = mBlockTextLinkTable.find( id );

    return ( it != mBlockTextLinkTable.constEnd() ) ?
        mBlockTextTable.value( it.value() ) : 0;
}

CdrDocument::~CdrDocument()
{
    qDeleteAll( mPages );
    delete mMasterPage;
    qDeleteAll( mStyleTable );
    qDeleteAll( mOutlineTable );
    qDeleteAll( mFillTable );
    qDeleteAll( mBlockTextTable );
}
