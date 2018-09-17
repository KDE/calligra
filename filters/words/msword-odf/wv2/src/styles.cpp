/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>

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

#include "styles.h"
#include "olestream.h"
#include "word97_helper.h"
#include "convert.h"
#include "paragraphproperties.h"

#include "wvlog.h"
#include <algorithm>
#include <string.h>

namespace wvWare
{

namespace Word97
{

// STD implementation
STD::STD()
{
    clearInternal();
}

STD::STD( U16 baseSize, U16 totalSize, OLEStreamReader* stream, bool preservePos )
throw(InvalidFormatException)
{
    clearInternal();
    if (!read( baseSize, totalSize, stream, preservePos )) {
        m_invalid = true;
//         throw InvalidFormatException("Invalid STD detected!");
    }
}

STD::STD( const STD& rhs ) : xstzName( rhs.xstzName )
{
    sti = rhs.sti;
    fScratch = rhs.fScratch;
    fInvalHeight = rhs.fInvalHeight;
    fHasUpe = rhs.fHasUpe;
    fMassCopy = rhs.fMassCopy;
    sgc = rhs.sgc;
    istdBase = rhs.istdBase;
    cupx = rhs.cupx;
    istdNext = rhs.istdNext;
    bchUpe = rhs.bchUpe;
    fAutoRedef = rhs.fAutoRedef;
    fHidden = rhs.fHidden;
    unused8_3 = rhs.unused8_3;
    grupxLen = rhs.grupxLen;
    m_invalid = rhs.m_invalid;

    // ...and the UPXes
    grupx = new U8[ grupxLen ];
    memcpy( grupx, rhs.grupx, grupxLen );
}

STD::~STD()
{
    delete [] grupx;
}

STD& STD::operator=( const STD& rhs )
{
    // Check for assignment to self
    if ( this == &rhs )
        return *this;

    sti = rhs.sti;
    fScratch = rhs.fScratch;
    fInvalHeight = rhs.fInvalHeight;
    fHasUpe = rhs.fHasUpe;
    fMassCopy = rhs.fMassCopy;
    sgc = rhs.sgc;
    istdBase = rhs.istdBase;
    cupx = rhs.cupx;
    istdNext = rhs.istdNext;
    bchUpe = rhs.bchUpe;
    fAutoRedef = rhs.fAutoRedef;
    fHidden = rhs.fHidden;
    unused8_3 = rhs.unused8_3;
    grupxLen = rhs.grupxLen;

    // assign the name
    xstzName = rhs.xstzName;

    // ...and the UPXes
    delete [] grupx;
    grupx = new U8[ grupxLen ];
    memcpy( grupx, rhs.grupx, grupxLen );

    return *this;
}

bool STD::read( const U16 cbStd, const U16 stdfSize, OLEStreamReader* stream, bool preservePos )
throw(InvalidFormatException)
{
    U16 shifterU16;
    S32 startOffset=stream->tell();  // address where the STD starts

    if (preservePos) {
        stream->push();
    }

    shifterU16 = stream->readU16();
    sti = shifterU16;
    shifterU16 >>= 12;
    fScratch = shifterU16;
    shifterU16 >>= 1;
    fInvalHeight = shifterU16;
    shifterU16 >>= 1;
    fHasUpe = shifterU16;
    shifterU16 >>= 1;
    fMassCopy = shifterU16;
    shifterU16 = stream->readU16();
    sgc = shifterU16;
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "##### sti: " << hex << "0x" << sti << dec << "(" << sti << ")" << endl;
    wvlog << "##### sgc: " << static_cast<int>( sgc ) << endl;
#endif
    shifterU16 >>= 4;
    istdBase = shifterU16;
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "istdBase: " << hex << "0x" << istdBase << dec << "(" << istdBase << ")" << endl;
#endif
    shifterU16 = stream->readU16();
    cupx = shifterU16;
    shifterU16 >>= 4;
    istdNext = shifterU16;
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "istdNext: " << hex << "0x" << istdNext << dec << "(" << istdNext << ")" << endl;
#endif
    bchUpe = stream->readU16();
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "cbStd: " << cbStd << "bchUpe: " << bchUpe << endl;
#endif

    // check the type of this style (stk)
    switch (sgc) {
    case sgcPara:
    case sgcChp:
    case sgcTbl:
    case sgcNmbr:
        break;
    default:
        throw InvalidFormatException("Invalid Style type detected!");
//         wvlog << "Invalid Style type detected!";
//         return false;
    }

    // Skip the end of the Word97::STD in older documents with stdfSize <= 8
    if ( stdfSize > 8 ) {
        shifterU16 = stream->readU16();
        fAutoRedef = shifterU16;
        shifterU16 >>= 1;
        fHidden = shifterU16;
        shifterU16 >>= 1;
        unused8_3 = shifterU16;
    }

    if (stdfSize == StdfPost2000) {
        wvlog << "Warning: StdfPost2000OrNone present - skipping";
        stream->seek( 8, WV2_SEEK_CUR );
    }

    // read the name of the style.
    // NOTE: Starts at an even address within the STD after the
    // stshi.cbSTDBaseInFile part.
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "stdfSize: " << stdfSize << endl;
    wvlog << "start offset: " << startOffset << endl;
    wvlog << "curr. position: " << stream->tell() << endl;
#endif
    // next even address
    U16 stdfSize_new = stdfSize;
    stdfSize_new += ( stdfSize & 0x0001 ) ? 1 : 0;
    stream->seek( startOffset + stdfSize, WV2_SEEK_SET );
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "new position: " << stream->tell() << endl;
    wvlog << "new stdfSize: " << stdfSize_new << endl;
#endif

    if (!readStyleName( stdfSize_new, cbStd - stdfSize_new, stream )) {
        throw InvalidFormatException("Invalid Style name detected!");
//         wvlog << "Invalid Style name detected!";
//         return false;
    }

    // even byte address within the STD?
    if ( ( stream->tell() - startOffset ) & 1 ) {
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "Adjusting the position... from " << stream->tell() - startOffset;
#endif
        stream->seek( 1, WV2_SEEK_CUR );
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << " to " << stream->tell() - startOffset << endl;
#endif
    }

    S32 bytesLeft = cbStd - ( stream->tell() - startOffset );

#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "cupx: " << static_cast<int>( cupx ) << endl;
    wvlog << "size: " << bytesLeft << endl;
#endif

    if (bytesLeft < 0) {
        wvlog << "BUG: reading outside STD limits" << endl;
        return false;
    }

    //FIXME: use StdfBase.cupx, StdfBase.stk
    grupxLen = bytesLeft;
    grupx = new U8[ grupxLen ];

    if (!grupx) {
        if ( preservePos ) {
            stream->pop();
        }
        wvlog << "====> Error: grupx allocation!" << endl;
        return false;
    }

    int offset = 0;
    for ( U8 i = 0; i < cupx; ++i) {
        U16 cbUPX = stream->readU16();  // size of the next UPX
        stream->seek( -2, WV2_SEEK_CUR ); // rewind the "lookahead"
        cbUPX += 2;                     // ...and correct the size
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "cbUPX: " << cbUPX << endl;
#endif
        // do not overflow the allocated buffer grupx
        if (offset + cbUPX > grupxLen) {
            wvlog << "====> Error: grupx would overflow!" << endl;
            return false;
        }
        for ( U16 j = 0; j < cbUPX; ++j ) {
            grupx[ offset + j ] = stream->readU8();  // read the whole UPX
#ifdef WV2_DEBUG_STYLESHEET
            wvlog << "  read: " << static_cast<int>( grupx[ offset + j ] ) << endl;
#endif
        }
        offset += cbUPX;  // adjust the offset in the grupx array
        // Don't forget to adjust the position
        if ( ( stream->tell() - startOffset ) & 1 ) {
#ifdef WV2_DEBUG_STYLESHEET
            wvlog << "Adjusting the UPX position... from " << stream->tell() - startOffset;
#endif
            stream->seek( 1, WV2_SEEK_CUR );
#ifdef WV2_DEBUG_STYLESHEET
            wvlog << " to " << stream->tell() - startOffset << endl;
#endif
        }
    }
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "curr. position: " << stream->tell() << endl;
#endif
    if ( preservePos ) {
        stream->pop();
    }
    return true;
}

bool STD::write( U16 /*stdfSize*/, OLEStreamWriter* stream, bool preservePos ) const
{
    // FIXME: don't ignore stdfSize and add writing code for the STD
    U16 shifterU16;

    if ( preservePos )
        stream->push();

    shifterU16 = sti;
    shifterU16 |= fScratch << 12;
    shifterU16 |= fInvalHeight << 13;
    shifterU16 |= fHasUpe << 14;
    shifterU16 |= fMassCopy << 15;
    stream->write( shifterU16 );
    shifterU16 = sgc;
    shifterU16 |= istdBase << 4;
    stream->write( shifterU16 );
    shifterU16 = cupx;
    shifterU16 |= istdNext << 4;
    stream->write( shifterU16 );
    stream->write( bchUpe );
    shifterU16 = fAutoRedef;
    shifterU16 |= fHidden << 1;
    shifterU16 |= unused8_3 << 2;
    stream->write( shifterU16 );
    // Attention: I don't know how to write xstzName - XCHAR[]
    // TODO "Couldn't generate writing code for STD::xstzName"
    // TODO "Couldn't generate writing code for STD::grupx"

    if ( preservePos )
        stream->pop();
    return true;
}

void STD::clear()
{
    delete [] grupx;
    clearInternal();
}

void STD::clearInternal()
{
    sti = 0;
    fScratch = 0;
    fInvalHeight = 0;
    fHasUpe = 0;
    fMassCopy = 0;
    sgc = 0;
    istdBase = 0;
    cupx = 0;
    istdNext = 0;
    bchUpe = 0;
    fAutoRedef = 0;
    fHidden = 0;
    unused8_3 = 0;
    grupx = 0;
    grupxLen = 0;
    m_invalid = false;
}

bool STD::readStyleName( const U16 stdfSize, const U16 stdBytesLeft, OLEStreamReader* stream )
{
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "stdfSize:" << hex << "0x" << stdfSize << dec << "(" << stdfSize << ")" << endl;
    wvlog << "stdBytesLeft:" << stdBytesLeft << endl;
#endif

    // Read the length of the string.  It seems that the spec is buggy and the
    // "length byte" is actually a short in Word97+
    if ( stdfSize > 8 ) {

        // The Xstz structure is a string.  The string is prepended by its
        // length and is null-terminated.
        //
        // xst (variable): An Xst structure that is prepended with a value
        // which specifies the length of the string.
        //
        // chTerm (2 bytes): A null-terminating character.  This value MUST be
        // zero.
        //
        // The Xst structure is a string.  The string is prepended by its
        // length and is not null-terminated.
        //
        // cch (2 bytes): An unsigned integer that specifies the number of
        // characters that are contained in the rgtchar array.
        //
        // rgtchar (variable): An array of 16-bit Unicode characters that make
        // up a string.

        U16 length = stream->readU16();
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "length: " << length << endl;
#endif
        if ( ((length * 2) > stdBytesLeft) ) {
            wvlog << "xstzName length invalid";
            return false;
        }

        // question: Is the \0 included in the length spec?
        XCHAR *name = new XCHAR[ length + 1 ];
        if (!name) {
            wvlog << "====> Error: malloc";
            return false;
        }

        for ( U16 i = 0; i < length + 1; ++i ) {
            name[ i ] = stream->readU16();
#ifdef WV2_DEBUG_STYLESHEET
            wvlog << "xstzName[" << static_cast<int>( i ) << "]: " << name[i] << endl;
#endif
        }
        if ( name[ length ] != 0 ) {
            wvlog << "Warning: Illegal trailing character: " <<
                     static_cast<int>( name[ length ] ) << endl;
            delete [] name;
            return false;
        }
        xstzName = UString( reinterpret_cast<const wvWare::UChar *>( name ), length );
        delete [] name;
    }
    else {
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "Warning: processing on older Style Sheet";
#endif

        // Word versions older than Word97 have a plain length byte and
        // a char* string as name
        U8 length = stream->readU8();
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "length: " << static_cast<int>( length ) << endl;
#endif

        if ( (length > stdBytesLeft) || (length == 0) ) {
            wvlog << "xstzName length invalid";
            return false;
        }

        // question: Is the \0 included in the length spec?
        U8 *name = new U8[ length + 1 ];
        if (!name) {
            wvlog << "====> Error: malloc";
            return false;
        }

        stream->read( name, length + 1 );
#ifdef WV2_DEBUG_STYLESHEET
        for ( U16 i = 0; i < length + 1; ++i )
            wvlog << "xstzName[" << static_cast<int>( i ) << "]: " << static_cast<int>( name[i] ) << endl;
#endif
        if ( name[ length ] != 0 ) {
            wvlog << "Warning: processing on older Style Sheet";
            wvlog << "Warning: Illegal trailing character: " <<
                     static_cast<int>( name[ length ] ) << endl;
//             name[ length ] = 0;
            delete [] name;
            return false;
        }

        xstzName = UString( reinterpret_cast<const char *>( name ) );
        delete [] name;
    }
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "ASCII Name: '" << xstzName.ascii() << "'" << endl;
#endif
    return true;
}

bool operator==( const STD& lhs, const STD& rhs )
{
    if ( lhs.grupxLen != rhs.grupxLen )
        return false;
    for ( U8 i = 0; i < lhs.grupxLen; ++i ) {
        if ( lhs.grupx[ i ] != rhs.grupx[ i ] )
            return false;
    }

    return lhs.xstzName == rhs.xstzName &&
           lhs.sti == rhs.sti &&
           lhs.fScratch == rhs.fScratch &&
           lhs.fInvalHeight == rhs.fInvalHeight &&
           lhs.fHasUpe == rhs.fHasUpe &&
           lhs.fMassCopy == rhs.fMassCopy &&
           lhs.sgc == rhs.sgc &&
           lhs.istdBase == rhs.istdBase &&
           lhs.cupx == rhs.cupx &&
           lhs.istdNext == rhs.istdNext &&
           lhs.bchUpe == rhs.bchUpe &&
           lhs.fAutoRedef == rhs.fAutoRedef &&
           lhs.fHidden == rhs.fHidden &&
           lhs.unused8_3 == rhs.unused8_3;
}

bool operator!=( const STD& lhs, const STD& rhs )
{
    return !( lhs == rhs );
}

} // namespace Word97
} // namespace wvWare


using namespace wvWare;

Style::Style( const U16 stdfSize, OLEStreamReader* tableStream, U16* ftc )
  :
    m_isEmpty( false ),
    m_isWrapped( true ),
    m_invalid( false ),
    m_std( 0 ),
    m_properties( 0 ),
    m_chp( 0 ),
    m_upechpx( 0 )
{
    //NOTE: A signed integer that specifies the size, in bytes, of std.  This
    //value MUST NOT be less than 0.  LPStd structures are stored on even-byte
    //boundaries, but this length MUST NOT include this padding.
    S16 cbStd = tableStream->readS16();
    if (cbStd < 0) {
        wvlog << "BIG mess-up, cbStd < 0";
        m_invalid = true;
        return;
    }

    if ( cbStd == 0 ) {  // empty slot
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "Empty style found: " << tableStream->tell() << endl;
#endif
        m_isEmpty = true;
        m_isWrapped = false;
        return;
    }

    S32 offset = tableStream->tell();
    m_std = new Word97::STD( cbStd, stdfSize, tableStream, false );

    //TODO: It might not be possible to read the rest of the styles properly
    //but let's try to display the document anyway.
    if (m_std->isInvalid()) {
        m_invalid = true;
        return;
    }

    //FIXME: check the note above
    if ( tableStream->tell() != offset + cbStd ) {
        wvlog << "Warning: Found a \"hole\"" << endl;
        // correct the offset
        tableStream->seek( cbStd, WV2_SEEK_CUR );
    }

    switch (m_std->sgc) {
    case sgcPara:
        m_chp = new Word97::CHP();
        m_upechpx = new UPECHPX();
        m_properties = new ParagraphProperties();
        m_chp->ftc = *ftc;         // Same value for ftc and ftcAscii
        m_chp->ftcAscii = *ftc++;
        m_chp->ftcFE = *ftc++;
        m_chp->ftcOther = *ftc;
        break;
    case sgcChp:
        m_chp = new Word97::CHP();
        m_upechpx = new UPECHPX();
        m_chp->ftc = *ftc;         // Same value for ftc and ftcAscii
        m_chp->ftcAscii = *ftc++;
        m_chp->ftcFE = *ftc++;
        m_chp->ftcOther = *ftc;
        break;
    case sgcTbl:
        wvlog << "Warning: Table Style - not supported at the moment!";
        break;
    case sgcNmbr:
        wvlog << "Warning: Numbering Style - not supported at the moment!";
        break;
    default:
        wvlog << "Attention: New kind of style in the stylesheet" << endl;
        break;
    }
}

Style::Style(const Word97::CHP& chp)
    :
    m_isEmpty( true ),
    m_isWrapped( false ),
    m_invalid( true ),
    m_std( 0 ),
    m_properties( 0 ),
    m_chp( 0 ),
    m_upechpx( 0 )
{
    m_chp = new Word97::CHP( chp );
}

Style::~Style()
{
    delete m_std;
    delete m_properties;
    delete m_chp;
    delete m_upechpx;
}

void Style::validate(const U16 istd, const U16 rglpstd_cnt, const std::vector<Style*>& styles, U16& uds_num)
{
    if (m_isEmpty) {
        return;
    }
    //Informs of any parsing error.
    if (m_invalid) {
        return;
    }

#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "istd: 0x" << istd << "istdBase: 0x" << hex << m_std->istdBase << endl;
#endif

    m_invalid = true;

    //TODO: check the m_std.stk

    if ((m_std->istdBase != 0x0fff) &&
        (m_std->istdBase >= rglpstd_cnt)) {
        wvlog << "istdBase - invalid index into rglpstd!" << endl;
        return;
    }
    if (m_std->istdBase == istd) {
        wvlog << "istdBase MUST NOT be same as istd!" << endl;
        return;
    }
    if ((m_std->istdBase != 0x0fff) &&
        styles[m_std->istdBase]->isEmpty()) {
        wvlog << "istdBase - style definition EMPTY!" << endl;
        return;
    }

    //The same repair approach is used by the MSWord2007 DOCX filter.
    //Remember that stiNormal == istdNormal.
    if ((m_std->istdNext != 0x0fff) &&
        (m_std->istdNext >= rglpstd_cnt)) {

#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "Warning: istdNext - invalid index into rglpstd, setting to stiNormal!";
#endif
        m_std->istdNext = stiNormal;
    }
    if ((m_std->istdNext != 0x0fff) &&
        styles[m_std->istdNext]->isEmpty()) {
        wvlog << "istdNext - style definition EMPTY!" << endl;
        return;
    }
    //Each style name, whether primary or alternate, MUST NOT be empty
    //and MUST be unique within all names in the stylesheet.
    if (m_std->xstzName.isEmpty()) {

#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "Warning: Empty xstzName detected, preparing a custom name!";
#endif
        //generate a name for a user define style
        if (m_std->sti == 0x0ffe) {
            m_std->xstzName = UString("User_Defined_");
            m_std->xstzName.append(UString::from(++uds_num));
        } else {
            return;
        }
    }

    m_invalid = false;
}

void Style::unwrapStyle( const StyleSheet& stylesheet, WordVersion version )
{
    if ( !m_isWrapped || !m_std ) {
        return;
    }

#ifdef WV2_DEBUG_SPRMS
    wvlog << "Unwrapping style:" << "ASCII Name: '" << name().ascii() << "'" << endl;
#endif

    if ( m_std->sgc == sgcPara ) {
        const Style* parentStyle = 0;
        // only try to unwrap the "parent" if the style isn't the Nil style
        if ( m_std->istdBase != 0x0fff ) {
            parentStyle = stylesheet.styleByIndex( m_std->istdBase );
            if ( parentStyle ) {
#ifdef WV2_DEBUG_SPRMS
                wvlog << "#### parent style ASCII Name: '" << parentStyle->name().ascii() << "'" << endl;
#endif
                const_cast<Style*>( parentStyle )->unwrapStyle( stylesheet, version );
                m_properties->pap() = parentStyle->paragraphProperties().pap();
                *m_chp = parentStyle->chp();
            }
        }

        if (m_std->grupxLen >= 4) {
            U8 *data = m_std->grupx;

            // paragraph
            U16 cbUPX = readU16( data );
            data += 2;
            m_properties->pap().istd = readU16( data );
            data += 2;
            cbUPX -= 2;

            cbUPX = qMin(cbUPX, U16(m_std->grupxLen - 4));
#ifdef WV2_DEBUG_SPRMS
            wvlog << "############# Applying paragraph exceptions: " << cbUPX << endl;
#endif
            m_properties->pap().apply( data, cbUPX, parentStyle, &stylesheet, 0, version );  // try without data stream for now
#ifdef WV2_DEBUG_SPRMS
            wvlog << "############# done" << "[" << name().ascii() << "]" << endl;
#endif
            U16 datapos = 4 + cbUPX + 2;
            if (m_std->grupxLen >= datapos) {
                data += cbUPX;

                // character
                cbUPX = readU16( data );
                data += 2;

                cbUPX = qMin(cbUPX, U16(m_std->grupxLen - datapos));
#ifdef WV2_DEBUG_SPRMS
                wvlog << "############# Applying character exceptions: " << cbUPX << endl;
#endif
                m_chp->apply( data, cbUPX, parentStyle, &stylesheet, 0, version );  // try without data stream for now
#ifdef WV2_DEBUG_SPRMS
                wvlog << "############# done" << "[" << name().ascii() << "]" << endl;
#endif
            }
        }
    }
    else if ( m_std->sgc == sgcChp ) {
        const Style* parentStyle = 0;
        // only try to unwrap the "parent" if the style isn't the Nil style
        if ( m_std->istdBase != 0x0fff ) {
            parentStyle = stylesheet.styleByIndex( m_std->istdBase );
            if ( parentStyle ) {
#ifdef WV2_DEBUG_SPRMS
                wvlog << "#### parent style ASCII Name: '" << parentStyle->name().ascii() << "'" << endl;
#endif
                const_cast<Style*>( parentStyle )->unwrapStyle( stylesheet, version );
                bool ok;
                m_upechpx->istd = stylesheet.indexByID( m_std->sti, ok );
#ifdef WV2_DEBUG_SPRMS
                wvlog << "our istd = " << m_upechpx->istd << " sti = " << m_std->sti << endl;
#endif
                mergeUpechpx( parentStyle, version );
            }
            else {
#ifdef WV2_DEBUG_SPRMS
                wvlog << "################# NO parent style for this character style found" << endl;
#endif
            }
        }
        else {
            // no need to do anything regarding the stiNormalChar parentStyle
            // let's just merge the upxchpx character exceptions into ourselves
            bool ok;
            m_upechpx->istd = stylesheet.indexByID( m_std->sti, ok );
            mergeUpechpx(this, version);
        }

        //finally apply so the chpx so we have ourselves a nice chp
        m_chp->apply(m_upechpx->grpprl, m_upechpx->cb, parentStyle, &stylesheet, 0, version);
    }
    else {
        wvlog << "Warning: Unknown style type code detected" << endl;
    }
    m_isWrapped = false;
}

U16 Style::sti() const
{
    if ( m_std )
        return static_cast<U16>( m_std->sti );
    return 0x0fff;   // stiNil
}

UString Style::name() const
{
    if ( m_std )
        return m_std->xstzName;
    return UString::null;
}

U16 Style::followingStyle() const
{
    return m_std ? m_std->istdNext : 0x0fff;
}

ST_StyleType Style::type() const
{
    if ( m_std ) {
        if ( m_std->sgc == sgcPara )
            return sgcPara;
        else if ( m_std->sgc == sgcChp )
            return sgcChp;
    }
    return sgcUnknown;
}

const ParagraphProperties& Style::paragraphProperties() const
{
    if ( !m_properties ) {
        wvlog << "You requested the PAP of a character style? Hmm..." << endl;
        m_properties = new ParagraphProperties(); // let's return a default PAP, better than crashing
    }
    return *m_properties;
}

const Word97::CHP& Style::chp() const
{
    if ( !m_chp ) {
        if ( !m_isEmpty ) {
            wvlog << "You requested the CHP of an unknown style type? Hmm..." << endl;
            wvlog << "sti == " << m_std->sti << endl;
        } else {
            wvlog << "You requested the CHP of an empty style slot? Hmm..." << endl;
        }
        m_chp = new Word97::CHP(); // let's return a default CHP, better than crashing
    }
    return *m_chp;
}

const UPECHPX& Style::upechpx() const
{
    if ( !m_upechpx ) {
        wvlog << "You requested the CHPX of a paragraph style? Hmm..." << endl;
        m_upechpx = new UPECHPX(); // let's return a default UPECHPX, better than crashing
    }
    return *m_upechpx;
}

// Some code to assist in merging CHPXes
namespace {
    struct SprmEntry
    {
        SprmEntry( U16 sp, U16 offs ) : sprm( sp ), offset( offs ) {}
        U16 sprm;
        U16 offset;
    };

    bool operator<( const SprmEntry& rhs, const SprmEntry& lhs )
    {
        return rhs.sprm < lhs.sprm;
    }

    bool operator==( const SprmEntry& rhs, const SprmEntry& lhs )
    {
        return rhs.sprm == lhs.sprm;
    }

    void analyzeGrpprl( U8* grpprl, U16 count, std::vector<SprmEntry>& entries, WordVersion version )
    {
        U16 offset = 0;
        while ( offset < count ) {
            U16 sprm;
            if ( version == Word8 ) {
                sprm = readU16( grpprl );
#ifdef WV2_DEBUG_SPRMS
                wvlog << "####### offset: " << offset << " sprm: 0x" << hex << sprm << dec << endl;
#endif
                grpprl += sizeof( U16 );
            }
            else {
                sprm = *grpprl++;
#ifdef WV2_DEBUG_SPRMS
                wvlog << "####### offset: " << offset << " sprm (Word6/7): " << sprm << endl;
#endif
            }
            entries.push_back( SprmEntry( sprm, offset ) );
            const U16 len = wvWare::Word97::SPRM::determineParameterLength( sprm, grpprl, version );
            grpprl += len;
            offset += len + ( version == Word8 ? 2 : 1 );
        }
    }

    U16 copySprm( U8* destGrpprl, U8* srcGrpprl, const SprmEntry& entry, WordVersion version )
    {
        srcGrpprl += entry.offset;

        U16 sprm;
        if ( version == Word8 ) {
            sprm = readU16( srcGrpprl );
#ifdef WV2_DEBUG_SPRMS
            wvlog << "####### Copying sprm 0x" << hex << sprm << dec << endl;
#endif
            srcGrpprl += sizeof( U16 );
        }
        else {
            sprm = *srcGrpprl++;
#ifdef WV2_DEBUG_SPRMS
            wvlog << "####### Copying sprm (Word6/7) " << sprm << endl;
#endif
        }

        if ( sprm != entry.sprm )
            wvlog << "Ooops, we messed up the CHPX merging!" << endl;
        const U16 len = wvWare::Word97::SPRM::determineParameterLength( sprm, srcGrpprl, version ) + ( version == Word8 ? 2 : 1 );
        srcGrpprl -= version == Word8 ? 2 : 1; // back to the start
        memcpy( destGrpprl, srcGrpprl, len );
        return len;
    }
}

void Style::mergeUpechpx( const Style* parentStyle, WordVersion version )
{
    // Analyze the source and the base grpprls
    U8* srcGrpprl = m_std->grupx;
    U16 cbUPX = readU16( srcGrpprl );
    srcGrpprl += 2;
    std::vector<SprmEntry> source;
    analyzeGrpprl( srcGrpprl, cbUPX, source, version );

    U8* baseGrpprl = parentStyle->m_upechpx->grpprl;
    std::vector<SprmEntry> base;
    analyzeGrpprl( baseGrpprl, parentStyle->m_upechpx->cb, base, version );

    // Sort the created vectors
    std::sort( source.begin(), source.end() );
    std::sort( base.begin(), base.end() );

    // Get enough room for the sprms of both chpxes
    m_upechpx->grpprl = new U8[ parentStyle->m_upechpx->cb + cbUPX ];
    U16 destCount = 0;

    std::vector<SprmEntry>::const_iterator sourceIt = source.begin();
    std::vector<SprmEntry>::const_iterator sourceEnd = source.end();
    std::vector<SprmEntry>::const_iterator baseIt = base.begin();
    std::vector<SprmEntry>::const_iterator baseEnd = base.end();
    // First merge till one array is empty.
    while ( sourceIt != sourceEnd && baseIt != baseEnd ) {
        if ( *sourceIt < *baseIt ) {
            destCount += copySprm( &m_upechpx->grpprl[ destCount ], srcGrpprl, *sourceIt, version );
            ++sourceIt;
        }
        else if ( *sourceIt == *baseIt ) {  // prefer the "new" entry
            destCount += copySprm( &m_upechpx->grpprl[ destCount ], srcGrpprl, *sourceIt, version );
            ++sourceIt;
            ++baseIt;
        }
        else {
            destCount += copySprm( &m_upechpx->grpprl[ destCount ], baseGrpprl, *baseIt, version );
            ++baseIt;
        }
    }

    // Then copy the rest of the longer grpprl
    while ( sourceIt != sourceEnd ) {
        destCount += copySprm( &m_upechpx->grpprl[ destCount ], srcGrpprl, *sourceIt, version );
        ++sourceIt;
    }
    while ( baseIt != baseEnd ) {
        destCount += copySprm( &m_upechpx->grpprl[ destCount ], baseGrpprl, *baseIt, version );
        ++baseIt;
    }
    m_upechpx->cb = destCount;
}


StyleSheet::StyleSheet( OLEStreamReader* tableStream, U32 fcStshf, U32 lcbStshf ) throw(InvalidFormatException)
    : m_udsNum(0)
{
    WordVersion version = Word8;

    tableStream->push();
    tableStream->seek( fcStshf, WV2_SEEK_SET );

    const U16 cbStshi = tableStream->readU16();

    //STSH = StyleSheet
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "StyleSheet::StyleSheet(): fcStshf=" << fcStshf << " lcbStshf=" << lcbStshf
          << " cbStshi=" << cbStshi << endl;
#endif

    //NOTE: STSHI definition in MS-DOC and Microsoft Word 97 (aka Version 8)
    //documentation differs a bit, trying to fix the issues.

    // ------------------------------------------------------------
    // STSHI reading
    // ------------------------------------------------------------
    //
    if (!cbStshi) {
        //each FIB MUST contain a stylesheet
        throw InvalidFormatException("MISSING StyleSheet detected!");
    }
    else if ( cbStshi == Word95::STSHI::sizeOf ) {
        Word95::STSHI stsh( tableStream, false );
        m_stsh = Word95::toWord97( stsh );
        // okay, it's Word 6/7 after all
        version = Word67;
    }
    else if ( cbStshi == Word97::STSHI::sizeOf ) {
        m_stsh.read( tableStream, false );
    } else {
        wvlog << "Detected a different STSHI, check this (trying to read Word97 one)" << endl;
        m_stsh.read( tableStream, false );
    }

#ifdef WV2_DEBUG_STYLESHEET
    m_stsh.dump();
#endif

    if (!valid()) {
        throw InvalidFormatException("INVALID StyleSheet detected!");
    }

    //FIXME: That's OK for newer files, we are not using the rest of the data.
    if ( tableStream->tell() != static_cast<int>( fcStshf + cbStshi + 2 ) ) {
        wvlog << "Warning: STSHI too big? New version?"
              << " Expected: " << cbStshi + 2 << " Read: " << tableStream->tell() - fcStshf << endl;
        tableStream->seek( fcStshf + 2 + cbStshi, WV2_SEEK_SET );
    }
    // ------------------------------------------------------------
    // STDs - read the array of LPStd structures containing STDs
    // ------------------------------------------------------------
    //
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "Reading in" << m_stsh.cstd << " styles." << endl;
#endif
    for( U16 i = 0; i < m_stsh.cstd; ++i ) {
        m_styles.push_back( new Style( m_stsh.cbSTDBaseInFile,
                                       tableStream,
                                       m_stsh.rgftcStandardChpStsh ) );
#ifdef WV2_DEBUG_STYLESHEET
        if (m_styles[i]->isInvalid()) {
            wvlog << "==> style [" << i << "] : INVALID";
        } else {
            wvlog << "==> style [" << i << "] : VALID";
        }
#endif
    }
    //styles validation
    if (m_styles.size() != m_stsh.cstd) {
        wvlog << "Error: m_styles.size() != m_stsh.cstd";
    }
    for( U16 i = 0; i < m_stsh.cstd; ++i ) {
        Q_ASSERT(m_styles[i]);
        m_styles[i]->validate(i, m_stsh.cstd, m_styles, m_udsNum);
        if (m_styles[i]->isInvalid()) {
            throw InvalidFormatException("INVALID Style detected!");
        }
    }
    //Validation of "fixed-index" application define styles.
    if (!fixed_index_valid()) {
        throw InvalidFormatException("INVALID \"fixed-index\" application defined styles!");
    }

#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "Done reading the stylesheet: " <<
             " stream position:" << tableStream->tell() <<
             " expected:" << fcStshf + lcbStshf << endl;
#endif
    if ( tableStream->tell() < static_cast<int>( fcStshf + lcbStshf ) ) {
        wvlog << "Warning: Didn't read all bytes of the stylesheet..." << endl;
    }
    else if ( tableStream->tell() > static_cast<int>( fcStshf + lcbStshf ) ) {
        wvlog << "BUG: Read past the stylesheet area!" << endl;
    }

#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "##### Starting to unwrap styles" << endl;
    int i = 0;
#endif
    // "unzip" them and build up the PAPs and CHPs
    for ( std::vector<Style*>::iterator it = m_styles.begin();
          it != m_styles.end(); ++it )
    {
#ifdef WV2_DEBUG_STYLESHEET
        wvlog << "Going to unwrap style:" << "[" << i << "]" <<
                 "ASCII Name: '" << (*it)->name().ascii() << "'" << endl;
        i++;
#endif
        (*it)->unwrapStyle( *this, version );
    }
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << "##### Styles unwrapped" << endl;
#endif
    tableStream->pop();
}

StyleSheet::~StyleSheet()
{
    for ( std::vector<Style*>::iterator it = m_styles.begin(); it != m_styles.end(); ++it )
        delete *it;
}

bool StyleSheet::valid() const
{
    if ( (m_stsh.cstd < 0x000f) && (m_stsh.cstd >= 0x0ffe) ) {
        return false;
    }
    return true;
}

bool StyleSheet::fixed_index_valid() const
{
#ifdef WV2_DEBUG_STYLESHEET
    wvlog << endl;
#endif
    const uint sti[13] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 65, 105, 107 };

    //The "Normal" style MUST be present, it's used as default.
    if ( m_styles[0]->isEmpty() || m_styles[0]->isInvalid() ||
        (m_styles[0]->m_std->sti != 0) )
    {
        return false;
    }

    //According to the [MS-DOC] spec, the rglpstd MUST contain an LPStd for
    //each of these fixed-index styles and the order MUST match.  However
    //that's not true, sometimes those are not present.
    for (uint i = 1; i < 13; i++) {
        if (!m_styles[i]->isEmpty()) {
            if (m_styles[i]->isInvalid() || (m_styles[i]->m_std->sti != sti[i])) {
                return false;
            }
        }
    }
    return true;
}

unsigned int StyleSheet::size() const
{
    return m_styles.size();
}

const Style* StyleSheet::styleByIndex( U16 istd ) const
{
    if ( istd < m_styles.size() ) {
        return m_styles[ istd ];
    }
    return 0;
}

const Style* StyleSheet::styleByID( U16 sti ) const
{
    for ( std::vector<Style*>::const_iterator it = m_styles.begin(); it != m_styles.end(); ++it ) {
        if ( (*it)->sti() == sti ) {
            return *it;
        }
    }
    return 0;
}

U16 StyleSheet::indexByID( U16 sti, bool& ok ) const
{
    ok = true;
    U16 istd = 0;
    for ( std::vector<Style*>::const_iterator it = m_styles.begin(); it != m_styles.end(); ++it, ++istd ) {
        if ( (*it)->sti() == sti )
            return istd;
    }
    ok = false;
    return 0;
}
