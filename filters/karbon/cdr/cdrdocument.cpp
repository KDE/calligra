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


void
CdrParagraphLine::addTextSpan( CdrAbstractTextSpan* textSpan )
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


void
CdrBlockText::addParagraph( CdrParagraph* paragraph )
{
    // Heuristic approach: the lines are continued on the next text box,
    // if the nextline y is not below the previous one
    // TODO: adapt approach to text sizes
    CdrCoord lastLineNextLineY;
    // first paragraph?
    if( mParagraphs.isEmpty() )
    {
        mParagraphPartStarts.append( CdrParagraphLineIndex(0,0) );
        lastLineNextLineY = 0;
    }
    else
        lastLineNextLineY = mParagraphs.last()->paragraphLines().last()->nextLineOffset().y();

    const quint16 paragraphIndex = mParagraphs.count();

    const QVector<CdrParagraphLine*>& paragraphLines = paragraph->paragraphLines();
    for( int i = 0; i < paragraphLines.count(); ++i )
    {
        const CdrCoord nextLineY = paragraphLines.at(i)->nextLineOffset().y();
        if( nextLineY > lastLineNextLineY )
        {
            mParagraphPartStarts.append( CdrParagraphLineIndex(paragraphIndex,i) );
        }
        lastLineNextLineY = nextLineY;
    }

    mParagraphs.append(paragraph);
}

CdrBlockTextPartSpan
CdrBlockText::partSpan( quint16 id ) const
{
    const CdrParagraphLineIndex startIndex = mParagraphPartStarts.value(id);
    CdrParagraphLineIndex endIndex;
    // is last span?
    if( id+1 >= mParagraphPartStarts.count() )
        endIndex = CdrParagraphLineIndex(mParagraphs.count()-1, mParagraphs.last()->paragraphLines().count()-1);
    else
    {
        // get end by next
        endIndex = mParagraphPartStarts.value( id + 1 );
        if( endIndex.mLineIndex == 0 )
        {
            --endIndex.mParagraphIndex;
            endIndex.mLineIndex = mParagraphs.at(endIndex.mParagraphIndex)->paragraphLines().count() - 1;
        }
        else
            --endIndex.mLineIndex;
    }

    return CdrBlockTextPartSpan(startIndex, endIndex);
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
