/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "parser9x.h"
#include "properties97.h"
#include "styles.h"
#include "word97_helper.h"
#include "lists.h"
#include "handlers.h"
#include "footnotes97.h"
#include "annotations.h"
#include "bookmark.h"
#include "headers.h"
#include "fonts.h"
#include "textconverter.h"
#include "olestream.h"
#include "fields.h"
#include "graphics.h"
#include "associatedstrings.h"
#include "paragraphproperties.h"
#include "functor.h"
#include "functordata.h"
#include "word95_generated.h"
#include "convert.h"
#include "wvlog.h"

#include <numeric>
#include <string.h>

using namespace wvWare;

Parser9x::Position::Position( U32 cp, const PLCF<Word97::PCD>* plcfpcd ) :
        piece( 0 ), offset( cp )
{
    PLCFIterator<Word97::PCD> it( *plcfpcd );
    for ( ; it.current(); ++it, ++piece ) {
        if ( it.currentLim() > cp && it.currentStart() <= cp )
            break;
        offset -= it.currentRun();
    }
}


Parser9x::Parser9x( OLEStorage* storage, OLEStreamReader* wordDocument, const Word97::FIB& fib ) :
        Parser( storage, wordDocument ), m_fib( fib ), m_table( 0 ), m_data( 0 ),
        m_properties( 0 ), m_headers( 0 ), m_lists( 0 ), m_textconverter( 0 ), m_fields( 0 ),
        m_footnotes( 0 ), m_annotations( 0 ), m_fonts( 0 ), m_drawings( 0 ), m_bookmarks(0),
        m_plcfpcd( 0 ), m_tableRowStart( 0 ), m_tableRowLength( 0 ), m_cellMarkFound( false ),
        m_remainingCells( 0 ), m_table_skimming( 0 ),
        m_currentParagraph( new Paragraph ), m_remainingChars( 0 ),
        m_sectionNumber( 0 ), m_subDocument( None ), m_parsingMode( Default )
{
    if ( !isOk() )
        return;

    m_table = storage->createStreamReader( tableStream() );
    if ( !m_table || !m_table->isValid() ) {
        wvlog << "Error: Couldn't open the table stream (i.e. [0|1]Table or WordDocument)" << endl;
        m_okay = false;
        return;
    }

    m_data = storage->createStreamReader( "Data" );
    if ( !m_data || !m_data->isValid() ) {
        wvlog << "Information: Couldn't open the Data stream, no big deal" << endl;
        delete m_data;
        m_data = 0;
    }

    //validate FIB, keep going even if it's not perfect
    m_fib.valid();

#ifdef WV2_DUMP_FIB
    wvlog << "Dumping some parts of the FIB: " << endl;
    wvlog << "   wIdent=" << m_fib.wIdent << endl;
    wvlog << "   nFib=0x" << hex << m_fib.nFib << dec << endl;
    wvlog << "   nFibBack=" << m_fib.nFibBack << endl;
    wvlog << "   lid=0x" << hex << m_fib.lid << dec << endl;
    wvlog << "   lidFE=0x" << hex << m_fib.lidFE << dec << endl;
    wvlog << "   fEncrypted=" << m_fib.fEncrypted << endl;
    wvlog << "   chs=" << m_fib.chs << endl;
    wvlog << "   fcMin=" << m_fib.fcMin << endl;
    wvlog << "   fcMac=" << m_fib.fcMac << endl;
    wvlog << "   ccpText=" << m_fib.ccpText << endl;
    wvlog << "   ccpFtn=" << m_fib.ccpFtn << endl;
    wvlog << "   ccpHdd=" << m_fib.ccpHdd << endl;
    wvlog << "   ccpMcr=" << m_fib.ccpMcr << endl;
    wvlog << "   ccpAtn=" << m_fib.ccpAtn << endl;
    wvlog << "   ccpEdn=" << m_fib.ccpEdn << endl;
    wvlog << "   ccpTxbx=" << m_fib.ccpTxbx << endl;
    wvlog << "   ccpHdrTxbx=" << m_fib.ccpHdrTxbx << endl;
    wvlog << "   pnFbpChpFirst=" << m_fib.pnFbpChpFirst << endl;
    wvlog << "   pnChpFirst=" << m_fib.pnChpFirst << endl;
    wvlog << "   cpnBteChp=" << m_fib.cpnBteChp << endl;
    wvlog << "   pnFbpPapFirst=" << m_fib.pnFbpPapFirst << endl;
    wvlog << "   pnPapFirst=" << m_fib.pnPapFirst << endl;
    wvlog << "   cpnBtePap=" << m_fib.cpnBtePap << endl;
    wvlog << "   fcPlcfandRef=" << m_fib.fcPlcfandRef << endl;
    wvlog << "   lcbPlcfandRef=" << m_fib.lcbPlcfandRef << endl;
    wvlog << "   cswNew=" << hex << m_fib.cswNew << dec << endl;
#endif
    // Initialize all the cached data structures like stylesheets, fonts,
    // textconverter,...
    init();
}

Parser9x::~Parser9x()
{
    // Sanity check
    if ( !oldParsingStates.empty() || m_subDocument != None ) {
        wvlog << "Bug: Someone messed up the save/restore stack!" << endl;
    }

    delete m_currentParagraph;
    delete m_tableRowStart;
    delete m_drawings;
    delete m_fonts;
    delete m_plcfpcd;
    delete m_headers;
    delete m_footnotes;
    delete m_bookmarks;
    delete m_annotations;
    delete m_fields;
    delete m_textconverter;
    delete m_properties;
    delete m_lists;
    delete m_data;
    delete m_table;
}

bool Parser9x::parse()
{
    if ( !isOk() )
        return false;

    if ( m_fib.fEncrypted ) {
        // There is some code out there to break this "encryption", do we want
        // to implement that?
        // We could either ask for a password or cheat a bit :-)
        wvlog << "Error: The document is encrypted." << endl;
        return false;
    }

    if ( m_fib.lcbClx == 0 )
        fakePieceTable();
    else {
        // Get the piece table
        if ( !readPieceTable() )
            return false;
    }

    //provide the headers mask to m_subDocumentHandler
    if (m_headers) {
        m_subDocumentHandler->headersMask(m_headers->headersMask());
    }
    // start parsing the body
    if ( !parseBody() )
        return false;
    return true;
}

const Word97::FIB& Parser9x::fib() const
{
    return m_fib;
}

const Word97::DOP& Parser9x::dop() const
{
    return m_properties->dop();
}

const Word97::FFN& Parser9x::font( S16 ftc ) const
{
    return m_fonts->font( ftc );
}

AssociatedStrings Parser9x::associatedStrings()
{
    return AssociatedStrings( m_fib.fcSttbfAssoc, m_fib.lcbSttbfAssoc,
                              m_fib.fFarEast ? m_fib.lidFE : m_fib.lid, m_table );
}

const StyleSheet& Parser9x::styleSheet() const
{
    return m_properties->styleSheet();
}

const Drawings* Parser9x::getDrawings() const
{
    return m_drawings;
}

OLEStreamReader* Parser9x::getTable()
{
    return m_table;
}

void Parser9x::parseHeaders( const HeaderData& data )
{
    m_subDocumentHandler->headersStart();
    for ( unsigned char mask = HeaderData::HeaderEven;
          mask <= HeaderData::FooterFirst; mask <<= 1 )
    {
        if ( mask & data.headerMask ) {
            parseHeader( data, mask );
        }
    }
    m_subDocumentHandler->headersEnd();
}

void Parser9x::parseFootnote( const FootnoteData& data )
{
#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "Parser9x::parseFootnote() #####################" << endl;
#endif
    // shouldn't happen, but well...
    if ( data.limCP - data.startCP == 0 ) {
        return;
    }

    saveState( data.limCP - data.startCP, data.type == FootnoteData::Footnote ? Footnote : Endnote );
    m_subDocumentHandler->footnoteStart();

    U32 offset = m_fib.ccpText + data.startCP;
    if ( data.type == FootnoteData::Endnote ) {
        offset += m_fib.ccpFtn + m_fib.ccpHdd + m_fib.ccpMcr + m_fib.ccpAtn;
    }
    parseHelper( Position( offset, m_plcfpcd ) );

    m_subDocumentHandler->footnoteEnd();
    restoreState();
#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "Parser9x::parseFootnote() done ################" << endl;
#endif
}

void Parser9x::parseAnnotation( const AnnotationData& data )
{
#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "Parser9x::parseAnnotation() #####################" << endl;
#endif
    // shouldn't happen, but well...
    if ( data.limCP - data.startCP == 0 ) {
        return;
    }

    saveState( data.limCP - data.startCP, Annotation );
    m_subDocumentHandler->annotationStart();

    U32 offset = m_fib.ccpText + m_fib.ccpFtn + m_fib.ccpHdd + data.startCP;
    parseHelper( Position( offset, m_plcfpcd ) );

    m_subDocumentHandler->annotationEnd();
    restoreState();
#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "Parser9x::parseAnnotation() done ################" << endl;
#endif
}

void Parser9x::parseTableRow( const TableRowData& data )
{
#ifdef WV2_DEBUG_TABLES
    wvlog << "Parser9x::parseTableRow(): startPiece=" << data.startPiece <<
             " startOffset=" << data.startOffset << " length=" << data.length << endl;
#endif

    if ( data.length == 0 ) {
        return;
    }

    saveState( data.length, static_cast<SubDocument>( data.subDocument ), Table );
    m_remainingCells = data.tap->itcMac;
    m_tableHandler->tableRowStart( data.tap );
    m_tableHandler->tableCellStart();

    parseHelper( Position( data.startPiece, data.startOffset ) );

    m_tableHandler->tableRowEnd();
    restoreState();

#ifdef WV2_DEBUG_TABLES
    wvlog << "Parser9x::parseTableRow() done #####################" << endl;
#endif
}

void Parser9x::parseTextBox(unsigned int index, bool stylesxml)
{
    const PLCF<Word97::FTXBXS>* plcftxbxTxt = 0;
    if (stylesxml) {
        plcftxbxTxt =  m_drawings->getHdrTxbxTxt();
    } else {
        plcftxbxTxt =  m_drawings->getTxbxTxt();
    }
    if (!plcftxbxTxt) {
        wvlog << "plcftxbxTxt MISSING!";
        return;
    }
    //NOTE: text ranges for each FTXBXS structure are separated by 0x0D
    //characters that MUST be the last character in each range.

    PLCFIterator<Word97::FTXBXS> it( plcftxbxTxt->at( index ) );

    //TODO: Do we need to save the state here?
    saveState( it.currentRun() - 1, TextBox );
    U32 offset = m_fib.ccpText + it.currentStart();
    offset += m_fib.ccpFtn + m_fib.ccpHdd + m_fib.ccpAtn + m_fib.ccpEdn;
    parseHelper( Position( offset, m_plcfpcd ) );
    restoreState();
}

std::string Parser9x::tableStream() const
{
    if ( m_fib.nFib < Word8nFib )
        return "WordDocument";    // Word 6 or Word 7 (==95)
    else
        return m_fib.fWhichTblStm ? "1Table" : "0Table";  // Word 8 (==97) or newer
}

void Parser9x::init()
{
    if ( m_fib.fFarEast )
        m_textconverter = new TextConverter( m_fib.lidFE );
    else
        m_textconverter = new TextConverter( m_fib.lid );

    // Get hold of all the SEP/PAP/CHP related structures and the StyleSheet
    m_properties = new Properties97( m_wordDocument, m_table, m_fib );

    if ( m_fib.nFib < Word8nFib ) // Word67
        m_lists = new ListInfoProvider( &styleSheet() );
    else
        m_lists = new ListInfoProvider( m_table, m_fib, &m_properties->styleSheet() );

    m_fonts = new FontCollection( m_table, m_fib );
    m_fields = new Fields( m_table, m_fib );
    m_drawings = new Drawings( m_table, m_fib );

    if (( m_fib.ccpFtn != 0 ) || ( m_fib.ccpEdn != 0 ))
        m_footnotes = new Footnotes97( m_table, m_fib );

    if (( m_fib.lcbPlcfbkf != 0 ) || ( m_fib.lcbPlcfbkl != 0 ))
        m_bookmarks = new Bookmarks( m_table, m_fib );

    if ( m_fib.ccpAtn != 0 ) {
        m_annotations = new Annotations( m_table, m_fib );
    }
}

bool Parser9x::readPieceTable()
{
    m_table->seek( m_fib.fcClx );
    // first skip the leading grpprl blocks, we'll re-read them
    // if we need them later (no caching here)
    U8 blockType = m_table->readU8();
    while ( blockType == wvWare::clxtGrpprl ) {
        U16 size = m_table->readU16();
#if WV2_DUMP_PIECE_TABLE > 0
        wvlog << "Found a clxtGrpprl (size=" << size << ")" << endl;
#endif
        m_table->seek( size, WV2_SEEK_CUR );
        blockType = m_table->readU8();
    }
    if ( blockType == wvWare::clxtPlcfpcd ) {
        U32 size = m_table->readU32();
#if WV2_DUMP_PIECE_TABLE > 0
        wvlog << "Found the clxtPlcfpcd (size=" << size << ")" << endl;
#endif
        m_plcfpcd = new PLCF<Word97::PCD>( size, m_table, false );

#if WV2_DUMP_PIECE_TABLE > 1
        PLCFIterator<Word97::PCD> it( *m_plcfpcd );
        for ( int i = 0; it.current(); ++it, ++i ) {
            wvlog << "Piece Table Entry(" << i << "): " << endl;
            wvlog << "   start: " << it.currentStart() << endl;
            wvlog << "   lim: " << it.currentLim() << endl;
            wvlog << "   complex: " << it.current()->prm.fComplex << endl;
            if ( it.current()->prm.fComplex )
                wvlog << "   igrpprl: " << it.current()->prm.toPRM2().igrpprl << endl;
            else
                wvlog << "   isprm: " << it.current()->prm.isprm << endl;

            U32 fc = it.current()->fc;
            U32 limit = it.currentRun() << 1;
            wvlog << "   value: " << fc << endl;
            if ( fc & 0x40000000 ) {
                fc = ( fc & 0xbfffffff ) >> 1;
                limit >>= 1;
                wvlog << "   value (cleared 2nd MSB, div. by 2): " << fc << endl;
            }
            m_wordDocument->seek( fc );
            wvlog << "   position: " << m_wordDocument->tell() << ", limit: " << limit << endl;
            for ( unsigned int j = 0; j < limit; ++j ) {
                U8 foo = m_wordDocument->readU8();
                if ( foo > 31 )
                    wvlog << static_cast<char>( foo );
                else if ( foo == PARAGRAPH_MARK )
                    wvlog << endl;
                else if ( foo > 0 )
                    wvlog << "{" <<  static_cast<int>( foo ) << "}";
                else
                    wvlog << "_";
            }
            wvlog << endl << "   position: " << m_wordDocument->tell() << ", limit: " << limit << endl;
        }
#endif
    }
    else {
        wvlog << "Oooops, couldn't find the piece table." << endl;
        return false;
    }
    return true;
}

void Parser9x::fakePieceTable()
{
    U32 fakePlcfPCD[ 4 ];
    // The first CP is 0 (endianness doesn't matter :-)
    fakePlcfPCD[ 0 ] = 0;
    // The second CP corresponds to the length of the document
    fakePlcfPCD[ 1 ] = toLittleEndian( m_fib.ccpText + m_fib.ccpFtn + m_fib.ccpHdd + m_fib.ccpMcr +
                                       m_fib.ccpAtn + m_fib.ccpEdn + m_fib.ccpTxbx + m_fib.ccpHdrTxbx );

    // Now fake a matching PCD
    U8* tmp( reinterpret_cast<U8*>( &fakePlcfPCD[0] ) );
    tmp += 8;
    *tmp++ = 0;  // first the bitfields (unused)
    *tmp++ = 0;
    U32 fcMin = m_fib.fcMin << 1;
    fcMin |= 0x40000000;
    *tmp++ = static_cast<U8>( fcMin & 0x000000ff );
    *tmp++ = static_cast<U8>( ( fcMin & 0x0000ff00 ) >> 8 );   // then store the
    *tmp++ = static_cast<U8>( ( fcMin & 0x00ff0000 ) >> 16 );  // fc in little
    *tmp++ = static_cast<U8>( ( fcMin & 0xff000000 ) >> 24 );  // endian style
    *tmp++ = 0;  // then an empty PRM
    *tmp++ = 0;

    tmp = reinterpret_cast<U8*>( &fakePlcfPCD[0] );
    m_plcfpcd = new PLCF<Word97::PCD>( 16, tmp );
}

bool Parser9x::parseBody()
{
    saveState( m_fib.ccpText, Main );
    m_subDocumentHandler->bodyStart();

    SharedPtr<const Word97::SEP> sep( m_properties->sepForCP( 0 ) );
    if ( !sep ) {
        sep = new Word97::SEP(); // don't pass 0 pointers in any case
    }
#ifdef WV2_DEBUG_SECTIONS
    sep->dump();
#endif

    m_textHandler->sectionStart( sep ); // First section, starting at CP 0

    emitHeaderData( sep );
    sep = 0; // get rid of the huge SEP

    // Process all the pieces belonging to the main document text
    parseHelper( Position( 0, static_cast<U32>( 0 ) ) );

    // Implicit end of the section
    m_textHandler->sectionEnd();
    m_subDocumentHandler->bodyEnd();
    restoreState();
    return true;
}

void Parser9x::parseHelper( Position startPos )
{
    PLCFIterator<Word97::PCD> it( m_plcfpcd->at( startPos.piece ) );

    while ( m_remainingChars > 0 && it.current() ) {
        U32 fc = it.current()->fc;   // Start FC of this piece
        bool unicode;
        realFC( fc, unicode );

        U32 limit = it.currentRun(); // Number of characters in this piece

        // Check whether the text starts somewhere within the piece, reset at
        // the end of the loop body.
        if ( startPos.offset != 0 ) {
            fc += unicode ? startPos.offset * 2 : startPos.offset;
            limit -= startPos.offset;
        }

        limit = limit > m_remainingChars ? m_remainingChars : limit;
        m_wordDocument->seek( fc );

        if ( unicode ) {
            XCHAR* string = new XCHAR[ limit ];
            // First read the whole piece
            for ( unsigned int j = 0; j < limit; ++j ) {
                string[ j ] = m_wordDocument->readU16();
                if ( ( string[ j ] & 0xff00 ) == 0xf000 ) {
                    // Microsoft uses a Private Unicode Area (PUA) to store the characters of the
                    // Symbol and the Wingdings font. We simply clear these bits to shift the
                    // characters to 0x00XX and hope the correct font is installed.  If the font
                    // isn't there, the user will get some ASCII text instead of symbols :}
                    //wvlog << "private unicode area detected -- cropping" << endl;
                    string[ j ] &= 0x00ff;
                }
            }
            // also takes care to delete [] string
            processPiece<XCHAR>( string, fc, limit, startPos );
        }
        else {
            U8* string = new U8[ limit ];
            m_wordDocument->read( string, limit );
            // also takes care to delete [] string
            processPiece<U8>( string, fc, limit, startPos );
        }
        m_remainingChars -= limit;
        ++it;
        ++startPos.piece;
        startPos.offset = 0; // just in case it was != 0 in the first iteration
    }
}

template<typename String>
void Parser9x::processPiece( String* string, U32 fc, U32 limit, const Position& position )
{
    // Take a closer look at the piece we just read. "start" and "index" are
    // counted in character positions (take care!)
    unsigned int start = 0;
    unsigned int index = 0;
    while ( index < limit ) {
        switch( string[ index ] ) {
        case SECTION_MARK:
        {
            if ( !m_currentParagraph->empty() || start != index ) {
                // No "index - start + 1" here, as we don't want to copy the section mark!
                UString ustring( processPieceStringHelper( string, start, index ) );
                m_currentParagraph->push_back( Chunk( ustring, Position( position.piece, position.offset + start ),
                                                      fc + start * sizeof( String ),
                                                      sizeof( String ) == sizeof( XCHAR ) ) );

                processParagraph( fc + index * sizeof( String ) );
            }
            start = ++index;

            SharedPtr<const Word97::SEP> sep( m_properties->sepForCP( m_fib.ccpText - m_remainingChars + index ) );
            if ( sep ) {

                //Check if table skimming was active lately.  If yes, then this
                //is the SECTION_MARK which follows a table.
                if (m_table_skimming) {
                    m_table_skimming = false;
                    wvlog << "A table was identified lately: informing the texthandler.";
                    m_textHandler->tableEndFound();
                }
                // It's not only a page break, it's a new section
                m_textHandler->sectionEnd();
                m_textHandler->sectionStart( sep );
                emitHeaderData( sep );
            }
            else {
                //manual page break
                m_textHandler->pageBreak();
            }
            break;
        }
        // same ASCII code as TTP_MARK (0x0007), NOTE: table depth == 1
        case CELL_MARK:
            m_cellMarkFound = true;
            // Fall-through intended. A row/cell end is also a paragraph end.
        case PARAGRAPH_MARK:
        {
            // No "index - start + 1" here, as we don't want to copy the
            // paragraph mark!
            UString ustring( processPieceStringHelper( string, start, index ) );
            m_currentParagraph->push_back( Chunk( ustring, Position( position.piece, position.offset + start ),
                                                  fc + start * sizeof( String ),
                                                  sizeof( String ) == sizeof( XCHAR ) ) );
            processParagraph( fc + index * sizeof( String ) );
            m_cellMarkFound = false;
            start = ++index;

            //signal progress
            if (m_subDocument == Main && m_parsingMode == Default) {
                int value = m_fib.ccpText - m_remainingChars + index;
                int percentage = (int)((value / (float) m_fib.ccpText) * 100);
                m_subDocumentHandler->setProgress( percentage );
            }

            break;
        }
        // "Special" characters
        case TAB:
            string[ index ] = m_inlineHandler->tab();
            ++index;
            break;
        case HARD_LINE_BREAK:
            string[ index ] = m_inlineHandler->hardLineBreak();
            ++index;
            break;
        case COLUMN_BREAK:
            string[ index ] = m_inlineHandler->columnBreak();
            ++index;
            break;
        case NON_BREAKING_HYPHEN:
            string[ index ] = m_inlineHandler->nonBreakingHyphen();
            ++index;
            break;
        case NON_REQUIRED_HYPHEN:
            string[ index ] = m_inlineHandler->nonRequiredHyphen();
            ++index;
            break;
        case NON_BREAKING_SPACE:
            string[ index ] = m_inlineHandler->nonBreakingSpace();
            ++index;
            break;
        default:
            ++index;
            break;
        }
    } //while
    if ( start < limit ) {
        // Finally we have to add the remaining text to the current paragraph
        // (if there is any)
        UString ustring( processPieceStringHelper( string, start, limit ) );
        m_currentParagraph->push_back( Chunk( ustring, Position( position.piece, position.offset + start ),
                                              fc + start * sizeof( String ), sizeof( String ) == sizeof( XCHAR ) ) );
    }
    delete [] string;
}

UString Parser9x::processPieceStringHelper( XCHAR* string, unsigned int start, unsigned int index ) const
{
    return UString( reinterpret_cast<const wvWare::UChar *>( &string[ start ] ), index - start );
}

UString Parser9x::processPieceStringHelper( U8* string, unsigned int start, unsigned int index ) const
{
    return m_textconverter->convert( reinterpret_cast<char*>( &string[ start ] ), index - start );
}

void Parser9x::processParagraph( U32 fc )
{
    // Get the PAP structure as it was at the last full-save
    ParagraphProperties* props( m_properties->fullSavedPap( fc, m_data ) );
    // ...and apply the latest changes, then the PAP is completely restored
    m_properties->applyClxGrpprl( m_plcfpcd->at( m_currentParagraph->back().m_position.piece ).current(),
                                  m_fib.fcClx, props );

    // Skim the tables first, as soon as the functor is invoked we have to
    // parse them and emit the text
    if ( m_parsingMode == Default && props->pap().fInTable ) {

        //TODO: We could be already skimming a separate table, check TAP!  In
        //case this is a new table inform the texthandler.

        //TODO: Support for nested tables!

        if ( !m_tableRowStart ) {
            m_tableRowStart = new Position( m_currentParagraph->front().m_position );
            m_tableRowLength = 0;
            m_table_skimming = true;

#ifdef WV2_DEBUG_TABLES
            props->pap().dump();
            wvlog << "Start of a table row: piece=" << m_tableRowStart->piece <<
                     " offset=" << m_tableRowStart->offset << endl;

#endif
        }
        // init == 1 because of the parag. mark!
        m_tableRowLength += std::accumulate( m_currentParagraph->begin(), m_currentParagraph->end(), 1,
                                             &Parser9x::accumulativeLength );

        //check if this is a Table Terminating Paragraph Mark
        if ( props->pap().fTtp ) {
            // Restore the table properties of this row
            Word97::TAP* tap = m_properties->fullSavedTap( fc, m_data );
#ifdef WV2_DEBUG_TABLES
            tap->dump();
#endif
            m_properties->applyClxGrpprl( m_plcfpcd->at( m_currentParagraph->back().m_position.piece ).current(),
                                          m_fib.fcClx, tap, m_properties->styleByIndex( props->pap().istd ) );

            SharedPtr<const Word97::TAP> sharedTap( tap );

            // We decrement the length by 1 that the trailing row mark doesn't
            // emit one empty paragraph during parsing.
            TableRowData data( m_tableRowStart->piece, m_tableRowStart->offset, m_tableRowLength - 1,
                               static_cast<int>( m_subDocument ), sharedTap );

            m_textHandler->tableRowFound( make_functor( *this, &Parser9x::parseTableRow, data), sharedTap );

            delete m_tableRowStart;
            m_tableRowStart = 0;
        }
        delete props;
    }
    else {
#ifdef WV2_DEBUG_PARAGRAPHS
        props->pap().dump();
#endif

        //Check if table skimming was active lately.  If yes, then this is the
        //paragraph behind the table (either a PARAGRAPH_MARK or a SECTION_MARK
        //follows a table)!
        if (m_table_skimming) {
            m_table_skimming = false;
            wvlog << "A table was identified lately: informing the texthandler.";
            m_textHandler->tableEndFound();
        }

        // Get the appropriate style for this paragraph.
        const Style* style = m_properties->styleByIndex( props->pap().istd );
        if ( !style ) {
            wvlog << "Warning: Huh, really obscure error, couldn't find the Style for the current PAP -- skipping" << endl;
            return;
        }

        // Get the CHP for the paragraph.
        Word97::CHP* paragraphChp = new Word97::CHP( style->chp() );
        m_properties->fullSavedChp( fc, paragraphChp, style );

#ifdef WV2_DEBUG_PARAGRAPHS
        paragraphChp->dump();
#endif

        // Now that we have the complete PAP and CHP, let's see if this
        // paragraph belongs to a list.
        props->createListInfo( *m_lists, *paragraphChp );

#ifdef WV2_DEBUG_LIST_PROCESSING
        props->pap().dump();
#endif

        // Parse the bullet picture data.
        const Word97::CHP* bulletChp = 0;
        if (props->listInfo()) {
            bulletChp = (props->listInfo()->text()).chp;
        }
        if (bulletChp && bulletChp->fPicBullet) {
            bool ok;
            BookmarkData data( m_bookmarks->bookmark(UString("_PictureBullets"), ok) );
            if (ok) {
                Position pos(data.startCP + bulletChp->picBulletCP, m_plcfpcd);
                PLCFIterator<Word97::PCD> it( m_plcfpcd->at( pos.piece ));
                U32 fc = it.current()->fc;
                bool unicode;

                realFC( fc, unicode );
                fc +=  unicode ? pos.offset * 2: pos.offset;

                Word97::CHP* bulletPicChp = new Word97::CHP();
                m_properties->fullSavedChp( fc, bulletPicChp, 0 );

                if (bulletPicChp->fSpec) {
                    m_wordDocument->push();
                    m_wordDocument->seek( fc, WV2_SEEK_SET );
                    U8 c = m_wordDocument->readU8();
                    m_wordDocument->pop();

                    if (c == TextHandler::Picture) {
                        SharedPtr<const Word97::CHP> sharedBPChp( bulletPicChp );
                        QString name = emitPictureData( 0, sharedBPChp, true);
                        props->setBulletPictureName(name);
                    } else {
                        wvlog << "BulletPicture: Support for character 0x" << hex << c << "not implement yet.";
                    }
                } else {
                    wvlog << "BulletPicture: A special character expected, skipping!";
                }
            }
        }

        // keep it that way, else the variables get deleted!
        SharedPtr<const ParagraphProperties> sharedPap( props );
        SharedPtr<const Word97::CHP> sharedParagraphChp( paragraphChp );

        m_textHandler->paragraphStart( sharedPap, sharedParagraphChp );

        std::list<Chunk>::const_iterator it = m_currentParagraph->begin();
        std::list<Chunk>::const_iterator end = m_currentParagraph->end();

        // Now walk the paragraph, chunk for chunk
        for ( ; it != end; ++it ) {
            U32 index = 0;
            const U32 limit = ( *it ).m_text.length();
            const PLCFIterator<Word97::PCD> pcdIt( m_plcfpcd->at( ( *it ).m_position.piece ) );

            while ( index < limit ) {
                // A temporary character style initialized to CHP of the
                // paragraph style.  Both CHPX and the built-in character style
                // referred by the istd are applied on top of it, while
                // comparing with the current CHP.
                Style charStyle( style->chp() );

                U32 fc = ( *it ).m_startFC + index * ( ( *it ).m_isUnicode ? 2 : 1 );
                U32 length = m_properties->fullSavedChp( fc,
                                                         &(const_cast<Word97::CHP&>(charStyle.chp())),
                                                         &charStyle );
                if ( ( *it ).m_isUnicode ) {
                    length >>= 1;
                }
                length = length > limit - index ? limit - index : length;

                m_properties->applyClxGrpprl( pcdIt.current(), m_fib.fcClx,
                                              &(const_cast<Word97::CHP&>(charStyle.chp())),
                                              &charStyle );

                Word97::CHP* chp = new Word97::CHP( charStyle.chp() );
                // keep it that way, else the CHP gets deleted!
                SharedPtr<const Word97::CHP> sharedChp( chp );
                processChunk( *it, sharedChp, length, index, pcdIt.currentStart() );
                index += length;
            }
            // Bookmark check for the next to last CP (paragraph mark).
            if ( m_bookmarks ) {
                emitBookmark( ( *it ).m_position.offset + limit );
            }
        }
        m_textHandler->paragraphEnd();

        if ( m_cellMarkFound ) {
            m_tableHandler->tableCellEnd();
            if ( --m_remainingCells > 0) {
                m_tableHandler->tableCellStart();
            }
        }
    }
    m_currentParagraph->clear();
}

void Parser9x::processChunk( const Chunk& chunk, SharedPtr<const Word97::CHP> chp,
                             U32 length, U32 index, U32 currentStart )
{
    // XXX: does the following hold for Annotations as well? (BSAR)

    // Some characters have a special meaning (e.g. a footnote is anchored at some
    // position inside the text) and they *don't* have the fSpec flag set. This means
    // that we have to watch out for such characters even in plain text. Slooow :}
    //
    // For now we only have to handle footnote and endnote references that way. Due to that
    // the code below is a bit simpler right now, but I fear we have to extend that later on.
    // (We will have to keep track of the type of disruption, footnote() takes care of all now)
    //
    // A precondition for the footnote/endnote implementation below is, that footnote and
    // endnote references only occur in the main body text. The reason is that we only check
    // for the next footnote inside the PLCF and don't take subdocuments into account. If
    // it turns out that this precondition is not satisfied we would have to change the
    // O(1) nextFootnote() call to something like an O(n) containsFootnote( start, lim )
    // Up to now Word 97, 2000, and 2002 seem to be bug compatible and fullfill that precondition.
    //

    //only process the chunk if not marked hidden, TODO use text:display="none"
    if (chp->fVanish == 1) {
        return;
    }

    while ( length > 0 ) {
        U32 startCP = currentStart + chunk.m_position.offset + index;
        U32 disruption = 0xffffffff; // "infinity"
        U32 bkmk_length = 0; //num. of CPs enclosed in a bookmark

        if ( m_footnotes ) {
            if (m_subDocument == Main) {
                m_footnotes->check(startCP);
            }

            U32 nextFtn = m_footnotes->nextFootnote();
            U32 nextEnd = m_footnotes->nextEndnote();
            disruption = nextFtn < nextEnd ? nextFtn : nextEnd;

#ifdef WV2_DEBUG_FOOTNOTES
            wvlog << "nextFtn=" << nextFtn << " nextEnd=" << nextEnd <<
                     " disruption=" << disruption << " length=" << length << endl;
#endif
        } else if ( m_bookmarks ) {
            if (m_subDocument == Main) {
                m_bookmarks->check(startCP);
            }

            U32 nextBkf = m_bookmarks->nextBookmarkStart();
            U32 nextBkl = m_bookmarks->nextBookmarkEnd();

            bkmk_length = nextBkl - nextBkf;
            disruption = nextBkf;

#ifdef WV2_DEBUG_BOOKMARK
            wvlog << "nextBkf=" << nextBkf << "(0x" << hex << nextBkf << ")" <<dec<<
                     "nextBkl=" << nextBkl << "(0x" << hex << nextBkl << ")" <<dec<<
                     "disruption=" << disruption << "length=" << length << endl;
#endif
            Q_ASSERT (nextBkf <= nextBkl);
        }

        if ( (disruption >= startCP) && (disruption < (startCP + length)) ) {

#if defined WV2_DEBUG_FOOTNOTES || defined WV2_DEBUG_BOOKMARK
            wvlog << "startCP=" << startCP << " disruption=" << disruption <<
             " bkmk_length=" << bkmk_length << " length=" << length << endl;
#endif
            U32 disLen = disruption - startCP;
            //there's something to be processed before the bookmark
            if ( disLen != 0 ) {
                processRun( chunk, chp, disLen, index, currentStart );
                length -= disLen;
                index += disLen;
            }

            if ( m_footnotes ) {
                //TODO: support for bookmarks in the number of a footnote
                m_customFootnote = chunk.m_text.substr(index, length);
                emitFootnote( m_customFootnote, disruption, chp, length );
                m_customFootnote = "";
                length = 0;
            }
            else if ( m_bookmarks ) {

                //TODO: There might a number of bookmarks to process at the
                //current CP.  The first one gets processed, the rest is
                //skipped at the moment.

                //TODO: Bookmarks can overlap, handle all bookmarks of a chunk.

                //TODO: A bookmark can denote text comrised of segments
                //belonging into different chunks.

                //NOTE: Not checking the ok value, invalid bookmarks were
                //already reported.  So it's obsolete at the moment.
                bool ok;
                BookmarkData data( m_bookmarks->bookmark( disruption, ok ) );

                if ( !(bkmk_length <= length) ) {
                    wvlog << "WARNING: bookmarks covering several chunks are not supported yet!";
                    processRun( chunk, chp, length, index, currentStart );
                    length = 0;
                } else {
                    m_textHandler->bookmarkStart( data );
                    if (bkmk_length > 0) {
                        processRun( chunk, chp, bkmk_length, index, currentStart );
                        m_textHandler->bookmarkEnd( data );
                        length -= bkmk_length;
                        index += bkmk_length;
                    }
                }
            }
        } else {
            processRun( chunk, chp, length, index, currentStart );
            break; // should be faster than messing with length...
        }
    }
}

void Parser9x::processRun( const Chunk& chunk, SharedPtr<const Word97::CHP> chp,
                           U32 length, U32 index, U32 currentStart )
{
    if ( chp->fSpec ) {
        U32 i = 0;
        while ( i < length ) {
            emitSpecialCharacter( chunk.m_text[ index + i ], currentStart + chunk.m_position.offset + index + i, chp );
            ++i;
        }
    }
    else {
        UConstString str( const_cast<UChar*>( chunk.m_text.data() ) + index, length );
        m_textHandler->runOfText( str.string(), chp );
    }
}

void Parser9x::emitSpecialCharacter( UChar character, U32 globalCP, SharedPtr<const Word97::CHP> chp )
{
    switch( character.unicode() ) {
        // Is it one of the "simple" special characters?
    case TextHandler::CurrentPageNumber:
    case TextHandler::LineNumber:
    case TextHandler::AbbreviatedDate:
    case TextHandler::TimeHMS:
    case TextHandler::CurrentSectionNumber:
    case TextHandler::AbbreviatedDayOfWeek:
    case TextHandler::DayOfWeek:
    case TextHandler::DayShort:
    case TextHandler::HourCurrentTime:
    case TextHandler::HourCurrentTimeTwoDigits:
    case TextHandler::MinuteCurrentTime:
    case TextHandler::MinuteCurrentTimeTwoDigits:
    case TextHandler::SecondsCurrentTime:
    case TextHandler::AMPMCurrentTime:
    case TextHandler::CurrentTimeHMSOld:
    case TextHandler::DateM:
    case TextHandler::DateShort:
    case TextHandler::MonthShort:
    case TextHandler::YearLong:
    case TextHandler::YearShort:
    case TextHandler::AbbreviatedMonth:
    case TextHandler::MonthLong:
    case TextHandler::CurrentTimeHMS:
    case TextHandler::DateLong:
        m_textHandler->specialCharacter( static_cast<TextHandler::SpecialCharacter>( character.unicode() ), chp );
        break;

        // It has to be one of the very special characters...
    case TextHandler::Picture:
        // PictureData are required to process inline MS-ODRAW objects.
        emitPictureData( globalCP, chp );
        break;
    case TextHandler::DrawnObject:
        // Only globalCP is required to process floating MS-ODRAW objects.
        m_textHandler->msodrawObjectFound( globalCP, 0 );
        break;
    case TextHandler::FootnoteAuto:
        if ( m_subDocument == Footnote || m_subDocument == Endnote ) {
            m_textHandler->footnoteAutoNumber( chp );
        } else {
            emitFootnote( UString(character), globalCP, chp);
        }
        break;
    case TextHandler::FieldBegin:
        {
            const FLD* fld( m_fields->fldForCP( m_subDocument, toLocalCP( globalCP ) ) );
            if ( fld ) {
                m_textHandler->fieldStart( fld, chp );
            } else {
                wvlog << "FieldStart: Plcfld does not contain this CP, ignoring!";
            }
            break;
        }
    case TextHandler::FieldSeparator:
        {
            const FLD* fld( m_fields->fldForCP( m_subDocument, toLocalCP( globalCP ) ) );
            if ( fld ) {
                m_textHandler->fieldSeparator( fld, chp );
            } else {
                wvlog << "FieldSeparator: Plcfld does not contain this CP, ignoring!";
            }
            break;
        }
    case TextHandler::FieldEnd:
        {
            const FLD* fld( m_fields->fldForCP( m_subDocument, toLocalCP( globalCP ) ) );
            if ( fld ) {
                m_textHandler->fieldEnd( fld, chp );
            } else {
                wvlog << "FieldEnd: Plcfld does not contain this CP, ignoring!";
            }
            break;
        }
    case TextHandler::AnnotationRef:
        {
            //comment reference characters are only in the Main Document
            if (m_subDocument == Main) {
                emitAnnotation( UString(character), globalCP, chp );
            }
        }
    case TextHandler::FieldEscapeChar:
        wvlog << "Found an escape character ++++++++++++++++++++?" << endl;
        break;
    case TextHandler::Symbol:
    {
        //NOTE: MS Word 2k/2k3/2k7 ignores chp->ftcSym (font for the symbol).
        m_textHandler->runOfText(UString(reinterpret_cast<const wvWare::UChar*>(&chp->xchSym), 1), chp);
        break;
    }
    default:
        wvlog << "Parser9x::processSpecialCharacter(): Support for character " << character.unicode()
              << " not implemented yet." << endl;
        break;
    }
}

void Parser9x::emitFootnote( UString characters, U32 globalCP,
                             SharedPtr<const Word97::CHP> chp,
                             U32 /* length */ )
{
    if ( !m_footnotes ) {
        wvlog << "Bug: Found a footnote, but m_footnotes == 0!" << endl;
        return;
    }
#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "######### Footnote found: CP=" << globalCP << endl;
#endif
    bool ok;
    FootnoteData data( m_footnotes->footnote( globalCP, ok ) );
    if ( ok ) {
#ifdef WV2_DEBUG_FOOTNOTES
        wvlog << "char: 0x" << hex << characters[0].unicode() <<
                 "| fAuto:" << data.autoNumbered <<
                 "| fSpec:" << chp->fSpec;
#endif
        SharedPtr<const Word97::SEP> sep( m_properties->sepForCP( globalCP ) );
        m_textHandler->footnoteFound( data, characters, sep, chp,
                                      make_functor( *this, &Parser9x::parseFootnote, data ));
    }
}

void Parser9x::emitBookmark( U32 globalCP )
{
    bool ok = false;
    BookmarkData data( m_bookmarks->bookmark( globalCP, ok ) );

    //TODO: handle bookmarks marking a text range between paragraphs in this
    //special case

    //there might be more bookmarks for the current CP
    while (ok) {
        if ((data.limCP - data.startCP) > 0) {
            wvlog << "WARNING: bookmarks marking a text range between paragraphs not supported!";
        } else {
            m_textHandler->bookmarkStart( data );
        }
        data = m_bookmarks->bookmark( globalCP, ok );

#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "Bookmark found: CP=" << globalCP << endl;
#endif
    }
}

void Parser9x::emitAnnotation( UString characters, U32 globalCP, SharedPtr<const Word97::CHP> chp, U32 /* length */ )
{
    if ( !m_annotations ) {
        wvlog << "Bug: Found an annotation, but m_annotations == 0!" << endl;
        return;
    }

    bool ok;
    AnnotationData data( m_annotations->annotation( globalCP, ok ) );
    if ( ok ) {
        m_textHandler->annotationFound(characters, chp,
                                       make_functor( *this, &Parser9x::parseAnnotation, data ));
    }
}

void Parser9x::emitHeaderData( SharedPtr<const Word97::SEP> sep )
{
    // We don't care about non-existent headers
    if ( !m_headers ) {
        return;
    }
    // NOTE: MS Word stores headers in a very strange way, so we have to keep
    // track of the section numbers.  We use a 0-based index for convenience
    // inside the header reading code. (Werner)
    //
    // Of course the file format has changed between Word 6/7 and Word 8, so I
    // had to add a workaround... oh well.
    HeaderData data( m_sectionNumber++ );

    if ( m_fib.nFib < Word8nFib ) {
        data.headerMask = sep->grpfIhdt;
        m_headers->set_headerMask( sep->grpfIhdt );
    }
    else {
        //check if an even header/footer is expected
        if ( dop().fFacingPages ) {
            data.headerMask |= HeaderData::HeaderEven | HeaderData::FooterEven;
        }
        //check if a first page header/footer is expected
        if ( sep->fTitlePage ) {
            data.headerMask |= HeaderData::HeaderFirst | HeaderData::FooterFirst;
        }
    }
    m_textHandler->headersFound( make_functor( *this, &Parser9x::parseHeaders, data ) );
}

QString Parser9x::emitPictureData( const U32 globalCP, SharedPtr<const Word97::CHP> chp , const bool isBulletPicture)
{
    //NOTE: No need for the globalCP param at the moment.

#ifdef WV2_DEBUG_PICTURES
    wvlog << "fcPic: " << chp->fcPic_fcObj_lTagObj;
    wvlog << "fObj:" << chp->fObj;
    wvlog << "fOle2:" << chp->fOle2;
#endif
    QString ret;

    if (chp->fOle2) {
        wvlog << "Embedded OLE2 objects not supported." << endl;
        return ret;
    }

    OLEStreamReader* stream( m_fib.nFib < Word8nFib ? m_wordDocument : m_data );
    if ( !stream || static_cast<unsigned int>( chp->fcPic_fcObj_lTagObj ) >= stream->size() ) {
        wvlog << "Error: Severe problems when trying to read an image. Skipping." << endl;
        return ret;
    }
    stream->push();
    stream->seek( chp->fcPic_fcObj_lTagObj, WV2_SEEK_SET );

    Word97::PICF* picf( 0 );
    if ( m_fib.nFib < Word8nFib ) {
        picf = new Word97::PICF( Word95::toWord97( Word95::PICF( stream, false ) ) );
    } else {
        picf = new Word97::PICF( stream, false );
    }
    stream->pop();

    //[MS-DOC] — v20101219, 419/621
    if ( picf->cbHeader != 0x44 ) {
        wvlog << "Error: Expected size of the PICF structure is 0x44, got " << hex << picf->cbHeader;
        wvlog << "Skipping the image!" << endl;
        delete picf;
        return ret;
    }

    if ( picf->fError ) {
        wvlog << "Information: Skipping the image, fError is set" << endl;
        delete picf;
        return ret;
    }

#ifdef WV2_DEBUG_PICTURES
    picf->dump();
#endif

    // Offset into the Data stream for the GraphicsHandler, position of the
    // OfficeArtInlineSpContainer to parse with libmso.
    int offset = chp->fcPic_fcObj_lTagObj + picf->cbHeader;

    // Read cchPicName and stPicName in case of a shape file, MS-DOC p.422/609.
    if ( picf->mfp.mm == 0x0066 )
    {
        U8 cchPicName = stream->readU8();
#ifdef WV2_DEBUG_PICTURES
        wvlog << "cchPicName: " << cchPicName << endl;
#endif
        if (cchPicName) {
            U8* stPicName = new U8[cchPicName + 1];
            stream->read(stPicName, cchPicName);
            stPicName[cchPicName] = '\0';
#ifdef WV2_DEBUG_PICTURES
            wvlog << "stPicName: " << stPicName << endl;
#endif
            delete [] stPicName;
        }
        offset += cchPicName + 1;
    }

    SharedPtr<const Word97::PICF> sharedPicf( picf );
    PictureData data( offset, sharedPicf );

    if (isBulletPicture) {
        ret = m_graphicsHandler->handleInlineObject(data, isBulletPicture);
    } else {
        m_textHandler->msodrawObjectFound(globalCP, &data);
    }
    return ret;
}

void Parser9x::parseHeader( const HeaderData& data, unsigned char mask )
{
#ifdef WV2_DEBUG_HEADERS
    wvlog << "parsing one header for section " << data.sectionNumber << ": mask=0x"
            <<  hex << static_cast<int>( mask ) << dec << endl;
#endif

    // First we have to determine the CP start/lim for the header text. From what I
    // found out Word 8 does it that way:
    //    - At the begin of the plcfhdd there are always 6 "0 fields" (stoppers)
    //    - The number of headers modulo 6 is always 0
    // Word 6 does it completely different, of course :-}
    std::pair<U32, U32> range( m_headers->findHeader( data.sectionNumber, mask ) );

    int length = range.second - range.first;
#ifdef WV2_DEBUG_HEADERS
    wvlog << "found a range: start=" << range.first << " lim=" << range.second << endl
            << "length: " << length << endl;
#endif
    if ( length < 1 ) {
#ifdef WV2_DEBUG_HEADERS
        wvlog << "Warning: Didn't find valid CPs for this header/footer -- ignoring it" << endl;
#endif
//         m_subDocumentHandler->headerStart( static_cast<HeaderData::Type>( mask ) );
//         SharedPtr<const ParagraphProperties> sharedProps( new ParagraphProperties );
//         m_textHandler->paragraphStart( sharedProps );
//         m_textHandler->paragraphEnd();
//         m_subDocumentHandler->headerEnd();
        return;
    }
    else if ( length > 1 ) {
        // get rid of the trailing "end of header/footer" character
        --length;
    }

    saveState( length, Header );

    m_subDocumentHandler->headerStart( static_cast<HeaderData::Type>( mask ) );
    parseHelper( Position( m_fib.ccpText + m_fib.ccpFtn + range.first, m_plcfpcd ) );
    m_subDocumentHandler->headerEnd();

    restoreState();
}

void Parser9x::saveState( U32 newRemainingChars, SubDocument newSubDocument, ParsingMode newParsingMode )
{
    oldParsingStates.push( ParsingState( m_tableRowStart, m_tableRowLength, m_cellMarkFound, m_remainingCells,
                                         m_table_skimming, m_currentParagraph, m_remainingChars, m_sectionNumber,
                                         m_subDocument, m_parsingMode ) );
    m_tableRowStart = 0;
    m_cellMarkFound = false;
    m_table_skimming = false;
    m_currentParagraph = new Paragraph;
    m_remainingChars = newRemainingChars;
    m_subDocument = newSubDocument;
    m_parsingMode = newParsingMode;

    // save current positions in OLEStreams
    m_wordDocument->push();
    if ( m_data ) {
        m_data->push();
    }
    if ( m_table ) {
        m_table->push();
    }
}

void Parser9x::restoreState()
{
    if ( oldParsingStates.empty() ) {
        wvlog << "Bug: You messed up the save/restore stack! The stack is empty" << endl;
        return;
    }

    // restore positions in OLEStreams
    m_wordDocument->pop();
    if ( m_data ) {
        m_data->pop();
    }
    if ( m_table ) {
        m_table->pop();
    }

    ParsingState ps( oldParsingStates.top() );
    oldParsingStates.pop();

    if ( m_tableRowStart ) {
        wvlog << "Bug: We still have to process the table row." << endl;
    }
    // Should be a no-op, but I hate mem-leaks even for buggy code ;-)
    delete m_tableRowStart;

    m_tableRowStart = ps.tableRowStart;
    m_tableRowLength = ps.tableRowLength;
    m_cellMarkFound = ps.cellMarkFound;
    m_remainingCells = ps.remainingCells;
    m_table_skimming = ps.tableSkimming;

    if ( !m_currentParagraph->empty() ) {
        wvlog << "Bug: The current paragraph isn't empty." << endl;
    }
    delete m_currentParagraph;
    m_currentParagraph = ps.paragraph;

    if ( m_remainingChars != 0 ) {
        wvlog << "Bug: Still got " << m_remainingChars << " remaining chars." << endl;
    }
    m_remainingChars = ps.remainingChars;
    m_sectionNumber = ps.sectionNumber;

    m_subDocument = ps.subDocument;
    m_parsingMode = ps.parsingMode;
}

U32 Parser9x::toLocalCP( U32 globalCP ) const
{
    if ( globalCP < m_fib.ccpText )
        return globalCP;
    globalCP -= m_fib.ccpText;

    if ( globalCP < m_fib.ccpFtn )
        return globalCP;
    globalCP -= m_fib.ccpFtn;

    if ( globalCP < m_fib.ccpHdd )
        return globalCP;
    globalCP -= m_fib.ccpHdd;

    if ( globalCP < m_fib.ccpMcr )
        return globalCP;
    globalCP -= m_fib.ccpMcr;

    if ( globalCP < m_fib.ccpAtn )
        return globalCP;
    globalCP -= m_fib.ccpAtn;

    if ( globalCP < m_fib.ccpEdn )
        return globalCP;
    globalCP -= m_fib.ccpEdn;

    if ( globalCP < m_fib.ccpTxbx )
        return globalCP;
    globalCP -= m_fib.ccpTxbx;

    if ( globalCP < m_fib.ccpHdrTxbx )
        return globalCP;
    globalCP -= m_fib.ccpHdrTxbx;

    wvlog << "Warning: You aimed " << globalCP << " characters past the end of the text!" << endl;
    return globalCP;
}

int Parser9x::accumulativeLength( int len, const Parser9x::Chunk& chunk )
{
    return len + chunk.m_text.length();
}
