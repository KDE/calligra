/* This file is part of the wvWare 2 project
   Copyright (C) 2002-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#include "handlers.h"
#include "parser9x.h"
#include "paragraphproperties.h"
#include "functor.h"
#include "wvlog.h"

using namespace wvWare;

InlineReplacementHandler::~InlineReplacementHandler()
{
}

U8 InlineReplacementHandler::tab()
{
    return TAB;
}

U8 InlineReplacementHandler::hardLineBreak()
{
    return HARD_LINE_BREAK;
}

U8 InlineReplacementHandler::columnBreak()
{
    return COLUMN_BREAK;
}

U8 InlineReplacementHandler::nonBreakingHyphen()
{
    return NON_BREAKING_HYPHEN;
}

U8 InlineReplacementHandler::nonRequiredHyphen()
{
    return NON_REQUIRED_HYPHEN;
}

U8 InlineReplacementHandler::nonBreakingSpace()
{
    return NON_BREAKING_SPACE;
}


SubDocumentHandler::~SubDocumentHandler()
{
}

void SubDocumentHandler::bodyStart()
{
}

void SubDocumentHandler::bodyEnd()
{
}

void SubDocumentHandler::footnoteStart()
{
}

void SubDocumentHandler::footnoteEnd()
{
}

void SubDocumentHandler::bookmarkStart()
{
}

void SubDocumentHandler::bookmarkEnd()
{
}

void SubDocumentHandler::annotationStart()
{
}

void SubDocumentHandler::annotationEnd()
{
}


void SubDocumentHandler::headersStart()
{
}

void SubDocumentHandler::headersEnd()
{
}

void SubDocumentHandler::headerStart( HeaderData::Type /*type*/ )
{
}

void SubDocumentHandler::headerEnd()
{
}

TableHandler::~TableHandler()
{
}

void TableHandler::tableRowStart( SharedPtr<const Word97::TAP> /*tap*/ )
{
}

void TableHandler::tableRowEnd()
{
}

void TableHandler::tableCellStart()
{
}

void TableHandler::tableCellEnd()
{
}


PictureHandler::~PictureHandler()
{
}

void PictureHandler::bitmapData( OLEImageReader& /*reader*/, SharedPtr<const Word97::PICF> /*picf*/ )
{
}

void PictureHandler::escherData( OLEImageReader& /*reader*/, SharedPtr<const Word97::PICF> /*picf*/, int /*type*/, const wvWare::U8* /*rgbUid*/)
{
}

void PictureHandler::escherData( std::vector<U8>, SharedPtr<const Word97::PICF> /*picf*/, int /*type*/, const wvWare::U8* /*rgbUid*/)
{
}

void PictureHandler::wmfData( OLEImageReader& /*reader*/, SharedPtr<const Word97::PICF> /*picf*/ )
{
}

void PictureHandler::externalImage( const UString& /*name*/, SharedPtr<const Word97::PICF> /*picf*/ )
{
}

void PictureHandler::officeArt(wvWare::OfficeArtProperties* /*artProperties*/)
{
}

GraphicsHandler::~GraphicsHandler()
{

}

void GraphicsHandler::handleFloatingObject(unsigned int /*globalCP*/)
{

}

void GraphicsHandler::handleInlineObject(const PictureData& /*data*/)
{
}

TextHandler::~TextHandler()
{
}

void TextHandler::sectionStart( SharedPtr<const Word97::SEP> /*sep*/ )
{
}

void TextHandler::sectionEnd()
{
}

void TextHandler::pageBreak()
{
}

void TextHandler::headersFound( const HeaderFunctor& parseHeaders )
{
    parseHeaders();
}

void TextHandler::paragraphStart( SharedPtr<const ParagraphProperties> /*paragraphProperties*/ )
{
}

void TextHandler::paragraphEnd()
{
}

void TextHandler::runOfText( const UString& /*text*/, SharedPtr<const Word97::CHP> /*chp*/ )
{
}

void TextHandler::specialCharacter( SpecialCharacter /*character*/, SharedPtr<const Word97::CHP> /*chp*/ )
{
}

void TextHandler::footnoteFound( FootnoteData::Type /*type*/, UString characters,
                                 SharedPtr<const Word97::CHP> chp, const FootnoteFunctor& parseFootnote)
{
    if ( characters[0].unicode() != 2 )
        runOfText( characters, chp ); // The character shouldn't get lost unless it's the auto-number
    parseFootnote();
}

void TextHandler::bookmarkFound( UString characters, UString name,
                                 SharedPtr<const Word97::CHP> chp)
{
    runOfText( characters, chp ); // The character shouldn't get lost unless it's the auto-number
}

void TextHandler::annotationFound( UString characters,
                                   SharedPtr<const Word97::CHP> chp, const AnnotationFunctor& parseAnnotation)
{
#ifdef WV_DEBUG_ANNOTATIONS
    wvlog << "TextHandler::annotationFound: ";
    for (int i = 0; i < characters.length(); ++i) {
        wvlog << characters[i].unicode();
    }
    wvlog << endl;
#endif
    runOfText(characters, chp);
    parseAnnotation();
}


void TextHandler::footnoteAutoNumber( SharedPtr<const Word97::CHP> /*chp*/ )
{
}

void TextHandler::fieldStart( const FLD* /*fld*/, SharedPtr<const Word97::CHP> /*chp*/ )
{
}

void TextHandler::fieldSeparator( const FLD* /*fld*/, SharedPtr<const Word97::CHP> /*chp*/ )
{
}

void TextHandler::fieldEnd( const FLD* /*fld*/, SharedPtr<const Word97::CHP> /*chp*/ )
{
}

void TextHandler::tableRowFound( const TableRowFunctor& tableRow, SharedPtr<const Word97::TAP> /*tap*/ )
{
    tableRow();
}

void TextHandler::inlineObjectFound(const PictureData& /*data*/, SharedPtr<const Word97::CHP> /*chp*/ )
{
}

void TextHandler::floatingObjectFound( unsigned int /*globalCP*/ )
{
}
