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

#include <parser.h>
#include <handlers.h>
#include <parserfactory.h>
#include <word97_generated.h>
#include <paragraphproperties.h>
#include <lists.h>
#include <ustring.h>
#include <fields.h>

#include <iostream>
#include <string>


class InlineReplacementTest : public wvWare::InlineReplacementHandler
{
public:
    virtual wvWare::U8 tab();
    virtual wvWare::U8 hardLineBreak();
    virtual wvWare::U8 columnBreak();
    virtual wvWare::U8 nonBreakingHyphen();
    virtual wvWare::U8 nonRequiredHyphen();
    virtual wvWare::U8 nonBreakingSpace();
};


class SubDocumentTest : public wvWare::SubDocumentHandler
{
public:
    virtual void bodyStart();
    virtual void bodyEnd();

    virtual void footnoteStart();
    virtual void footnoteEnd();

    virtual void headersStart();
    virtual void headersEnd();
    virtual void headerStart( wvWare::HeaderData::Type type );
    virtual void headerEnd();
};


class TableTest : public wvWare::TableHandler
{
public:
    virtual void tableRowStart( wvWare::SharedPtr<const wvWare::Word97::TAP> tap );
    virtual void tableRowEnd();
    virtual void tableCellStart();
    virtual void tableCellEnd();
};


class PictureTest : public wvWare::PictureHandler
{
public:
    virtual void bitmapData( wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf );
    virtual void wmfData( wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf );
    virtual void externalImage( const wvWare::UString& name, wvWare::SharedPtr<const wvWare::Word97::PICF> picf );
};


class TextTest : public wvWare::TextHandler
{
public:
    virtual void sectionStart( wvWare::SharedPtr<const wvWare::Word97::SEP> sep );
    virtual void sectionEnd();
    virtual void pageBreak();

    virtual void headersFound( const wvWare::HeaderFunctor& parseHeaders );

    virtual void paragraphStart( wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties );
    virtual void paragraphEnd();

    virtual void runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    virtual void specialCharacter( SpecialCharacter character, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    virtual void footnoteFound( wvWare::FootnoteData::Type type, wvWare::UChar character,
                                wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const wvWare::FootnoteFunctor& parseFootnote );
    virtual void footnoteAutoNumber( wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    virtual void fieldStart( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );
    virtual void fieldSeparator( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );
    virtual void fieldEnd( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    virtual void tableRowFound( const wvWare::TableRowFunctor& tableRow, wvWare::SharedPtr<const wvWare::Word97::TAP> tap );

    virtual void pictureFound( const wvWare::PictureFunctor& picture, wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
                               wvWare::SharedPtr<const wvWare::Word97::CHP> chp );
};


using namespace wvWare;
std::string indent;

U8 InlineReplacementTest::tab()
{
    std::cout << indent << "INLINE: tab" << std::endl;
    return InlineReplacementHandler::tab();
}

U8 InlineReplacementTest::hardLineBreak()
{
    std::cout << indent << "INLINE: hard line break" << std::endl;
    return InlineReplacementHandler::hardLineBreak();
}

U8 InlineReplacementTest::columnBreak()
{
    std::cout << indent << "INLINE: column break" << std::endl;
    return InlineReplacementHandler::columnBreak();
}

U8 InlineReplacementTest::nonBreakingHyphen()
{
    std::cout << indent << "INLINE: non breaking hyphen" << std::endl;
    return InlineReplacementHandler::nonBreakingHyphen();
}

U8 InlineReplacementTest::nonRequiredHyphen()
{
    std::cout << indent << "INLINE: non required hyphen" << std::endl;
    return InlineReplacementHandler::nonRequiredHyphen();
}

U8 InlineReplacementTest::nonBreakingSpace()
{
    std::cout << indent << "INLINE: non breaking space" << std::endl;
    return InlineReplacementHandler::nonBreakingSpace();
}


void SubDocumentTest::bodyStart()
{
    std::cout << indent << "SUBDOCUMENT: body start" << std::endl;
    indent.push_back( ' ' );
    SubDocumentHandler::bodyStart();
}

void SubDocumentTest::bodyEnd()
{
    std::cout << indent << "SUBDOCUMENT: body end" << std::endl;
    indent.erase( indent.size() - 1 );
    SubDocumentHandler::bodyEnd();
}

void SubDocumentTest::footnoteStart()
{
    std::cout << indent << "SUBDOCUMENT: footnote start" << std::endl;
    indent.push_back( ' ' );
    SubDocumentHandler::footnoteStart();
}

void SubDocumentTest::footnoteEnd()
{
    std::cout << indent << "SUBDOCUMENT: footnote end" << std::endl;
    indent.erase( indent.size() - 1 );
    SubDocumentHandler::footnoteEnd();
}

void SubDocumentTest::headersStart()
{
    std::cout << indent << "SUBDOCUMENT: headers start" << std::endl;
    indent.push_back( ' ' );
    SubDocumentHandler::headersStart();
}

void SubDocumentTest::headersEnd()
{
    std::cout << indent << "SUBDOCUMENT: headers end" << std::endl;
    indent.erase( indent.size() - 1 );
    SubDocumentHandler::headersEnd();
}

void SubDocumentTest::headerStart( HeaderData::Type type )
{
    std::cout << indent << "SUBDOCUMENT: header start " << static_cast<int>( type ) << std::endl;
    indent.push_back( ' ' );
    SubDocumentHandler::headerStart( type );
}

void SubDocumentTest::headerEnd()
{
    std::cout << indent << "SUBDOCUMENT: header end" << std::endl;
    indent.erase( indent.size() - 1 );
    SubDocumentHandler::headerEnd();
}


void TableTest::tableRowStart( SharedPtr<const Word97::TAP> tap )
{
    std::cout << indent << "TABLE: table row start" << std::endl;
    indent.push_back( ' ' );
    tap->dump();
    TableHandler::tableRowStart( tap );
}

void TableTest::tableRowEnd()
{
    std::cout << indent << "TABLE: table row end" << std::endl;
    indent.erase( indent.size() - 1 );
    TableHandler::tableRowEnd();
}

void TableTest::tableCellStart()
{
    std::cout << indent << "TABLE: table cell start" << std::endl;
    indent.push_back( ' ' );
    TableHandler::tableCellStart();
}

void TableTest::tableCellEnd()
{
    std::cout << indent << "TABLE: table cell end" << std::endl;
    indent.erase( indent.size() - 1 );
    TableHandler::tableCellEnd();
}


void PictureTest::bitmapData( wvWare::OLEImageReader& /*reader*/, wvWare::SharedPtr<const wvWare::Word97::PICF> /*picf*/ )
{
    // ###### TODO
    std::cout << indent << "PICTURE: bitmapData" << std::endl;
}

void PictureTest::wmfData( wvWare::OLEImageReader& /*reader*/, wvWare::SharedPtr<const wvWare::Word97::PICF> /*picf*/ )
{
    // ###### TODO
    std::cout << indent << "PICTURE: wmfData" << std::endl;
}

void PictureTest::externalImage( const wvWare::UString& name, wvWare::SharedPtr<const wvWare::Word97::PICF> /*picf*/ )
{
    // ###### TODO
    std::cout << indent << "PICTURE: externalImage: " << name.ascii() << std::endl;
}


void TextTest::sectionStart( SharedPtr<const Word97::SEP> sep )
{
    std::cout << indent << "TEXT: section start" << std::endl;
    indent.push_back( ' ' );
    sep->dump();
    TextHandler::sectionStart( sep );
}

void TextTest::sectionEnd()
{
    std::cout << indent << "TEXT: section end" << std::endl;
    indent.erase( indent.size() - 1 );
    TextHandler::sectionEnd();
}

void TextTest::pageBreak()
{
    std::cout << indent << "TEXT: page break" << std::endl;
    TextHandler::pageBreak();
}

void TextTest::headersFound( const HeaderFunctor& parseHeaders )
{
    std::cout << indent << "TEXT: headers found" << std::endl;
    TextHandler::headersFound( parseHeaders );
}

void TextTest::paragraphStart( SharedPtr<const ParagraphProperties> paragraphProperties )
{
    std::cout << indent << "TEXT: paragraph start" << std::endl;
    indent.push_back( ' ' );
    paragraphProperties->pap().dump();
    if ( paragraphProperties->listInfo() )
        paragraphProperties->listInfo()->dump();
    TextHandler::paragraphStart( paragraphProperties );
}

void TextTest::paragraphEnd()
{
    std::cout << indent << "TEXT: paragraph end" << std::endl;
    indent.erase( indent.size() - 1 );
    TextHandler::paragraphEnd();
}

void TextTest::runOfText( const UString& text, SharedPtr<const Word97::CHP> chp )
{
    std::cout << indent << "TEXT: run of text" << std::endl;
    chp->dump();
    std::cout << "TEXT: [";
    for ( int i = 0; i < text.length(); ++i )
        std::cout << "<" << text[ i ].unicode() << "|" << text[ i ].low() << ">";
    std::cout << "]" << std::endl;
    TextHandler::runOfText( text, chp );
}

void TextTest::specialCharacter( SpecialCharacter character, SharedPtr<const Word97::CHP> chp )
{
    std::cout << indent << "TEXT: special character " << static_cast<int>( character ) << std::endl;
    chp->dump();
    TextHandler::specialCharacter( character, chp );
}

void TextTest::footnoteFound( FootnoteData::Type type, UChar character,
                              SharedPtr<const Word97::CHP> chp, const FootnoteFunctor& parseFootnote )
{
    std::cout << indent << "TEXT: footnote found " << static_cast<int>( type )
              << " character " << character.unicode() << std::endl;
    chp->dump();
    TextHandler::footnoteFound( type, character, chp, parseFootnote );
}

void TextTest::footnoteAutoNumber( SharedPtr<const Word97::CHP> chp )
{
    std::cout << indent << "TEXT: footnote auto-number" << std::endl;
    chp->dump();
    TextHandler::footnoteAutoNumber( chp );
}

void TextTest::fieldStart( const FLD* fld, SharedPtr<const Word97::CHP> chp )
{
    std::cout << indent << "TEXT: field start " << static_cast<int>( fld->ch ) << std::endl;
    indent.push_back( ' ' );
    chp->dump();
    TextHandler::fieldStart( fld, chp );
}

void TextTest::fieldSeparator( const FLD* fld, SharedPtr<const Word97::CHP> chp )
{
    std::cout << indent << "TEXT: field separator " << static_cast<int>( fld->ch ) << std::endl;
    chp->dump();
    TextHandler::fieldSeparator( fld, chp );
}

void TextTest::fieldEnd( const FLD* fld, SharedPtr<const Word97::CHP> chp )
{
    std::cout << indent << "TEXT: field end " << static_cast<int>( fld->ch ) << std::endl;
    chp->dump();
    indent.erase( indent.size() - 1 );
    TextHandler::fieldEnd( fld, chp );
}

void TextTest::tableRowFound( const wvWare::TableRowFunctor& tableRow, wvWare::SharedPtr<const wvWare::Word97::TAP> tap )
{
    std::cout << indent << "TEXT: table row found" << std::endl;
    tap->dump();
    TextHandler::tableRowFound( tableRow, tap );
}

void TextTest::pictureFound( const wvWare::PictureFunctor& picture, wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
                             wvWare::SharedPtr<const wvWare::Word97::CHP> chp )
{
    std::cout << indent << "TEXT: picture found" << std::endl;
    picf->dump();
    chp->dump();
    TextHandler::pictureFound( picture, picf, chp );
}


int main( int argc, char** argv )
{
    if ( argc != 2 ) {
        std::cerr << "Usage: handlertest foo.doc" << std::endl;
        ::exit( 1 );
    }

    std::string document( argv[ 1 ] );
    SharedPtr<Parser> parser( ParserFactory::createParser( document ) );
    if ( !parser || !parser->isOk() ) {
        std::cerr << "Error: Couldn't create a parser for this document" << std::endl;
        ::exit( 2 );
    }

    TextTest* textTest( new TextTest );
    parser->setTextHandler( textTest );
    InlineReplacementTest* replacementTest( new InlineReplacementTest );
    parser->setInlineReplacementHandler( replacementTest );

    if ( !parser->parse() )  {
        std::cerr << "Error: The parser failed" << std::endl;
        delete replacementTest;
        delete textTest;
        ::exit( 3 );
    }
    std::cout << "Done." << std::endl;

    delete replacementTest;
    delete textTest;
    return 0;
}
