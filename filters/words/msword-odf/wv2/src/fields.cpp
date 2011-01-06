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

#include "fields.h"
#include "olestream.h"
#include "word_helper.h"
#include "word97_generated.h"

#include "wvlog.h"

namespace wvWare
{
    FLD::FLD() : ch( 0 ), flt( 0 )
    {
    }

    FLD::FLD( OLEStreamReader* stream, bool preservePos ) : ch( 0 ), flt( 0 )
    {
        read( stream, preservePos );
    }

    FLD::FLD( const U8* ptr ) : ch( 0 ), flt( 0 )
    {
        readPtr( ptr );
    }

    bool FLD::read( OLEStreamReader* stream, bool preservePos )
    {
        if ( preservePos )
            stream->push();

        ch = stream->readU8();
        flt = stream->readU8();

        if ( preservePos )
            stream->pop();
        return true;
    }

    bool FLD::readPtr( const U8* ptr )
    {
        ch = *ptr++;
        flt = *ptr;
        return true;
    }

    void FLD::clear()
    {
        ch = 0;
        flt = 0;
    }

    const unsigned int FLD::sizeOf = 2;

    bool operator==( const FLD &lhs, const FLD &rhs )
    {
        return lhs.ch == rhs.ch &&
              lhs.flt == rhs.flt;
    }

    bool operator!=( const FLD &lhs, const FLD &rhs )
    {
        return !( lhs == rhs );
    }
} // namespace wvWare

using namespace wvWare;


Fields::Fields( OLEStreamReader* tableStream, const Word97::FIB& fib ) :
    m_main( 0 ), m_header( 0 ), m_footnote( 0 ), m_annotation( 0 ),
    m_endnote( 0 ), m_textbox( 0 ), m_headerTextbox( 0 )
{
    tableStream->push();

#ifdef WV2_DEBUG_FIELDS
    wvlog << "Fields --------------" << endl
          << "  main: fc=" << fib.fcPlcffldMom << " lcb=" << fib.lcbPlcffldMom << endl
          << "  header: fc=" << fib.fcPlcffldHdr << " lcb=" << fib.lcbPlcffldHdr << endl
          << "  footnote: fc=" << fib.fcPlcffldFtn << " lcb=" << fib.lcbPlcffldFtn << endl
          << "  annotation: fc=" << fib.fcPlcffldAtn << " lcb=" << fib.lcbPlcffldAtn << endl
          << "  endnote: fc=" << fib.fcPlcffldEdn << " lcb=" << fib.lcbPlcffldEdn << endl
          << "  textbox: fc=" << fib.fcPlcffldTxbx << " lcb=" << fib.lcbPlcffldTxbx << endl
          << "  bookmark: fc=" << fib.fcSttbfbkmk << " lcb=" << fib.lcbSttbfbkmk << endl
          << "  headertextbox: fc=" << fib.fcPlcffldHdrTxbx << " lcb=" << fib.lcbPlcffldHdrTxbx << endl;
#endif
    tableStream->seek( fib.fcPlcffldMom, G_SEEK_SET ); // to make the sanity check work
    read( fib.fcPlcffldMom, fib.lcbPlcffldMom, tableStream, &m_main );

    sanityCheck( tableStream, fib.fcPlcffldHdr, fib.lcbPlcffldHdr );
    read( fib.fcPlcffldHdr, fib.lcbPlcffldHdr, tableStream, &m_header );

    sanityCheck( tableStream, fib.fcPlcffldFtn, fib.lcbPlcffldFtn );
    read( fib.fcPlcffldFtn, fib.lcbPlcffldFtn, tableStream, &m_footnote );

    sanityCheck( tableStream, fib.fcPlcffldAtn, fib.lcbPlcffldAtn );
    read( fib.fcPlcffldAtn, fib.lcbPlcffldAtn, tableStream, &m_annotation );

    sanityCheck( tableStream, fib.fcPlcffldEdn, fib.lcbPlcffldEdn );
    read( fib.fcPlcffldEdn, fib.lcbPlcffldEdn, tableStream, &m_endnote );

    sanityCheck( tableStream, fib.fcPlcffldTxbx, fib.lcbPlcffldTxbx );
    read( fib.fcPlcffldTxbx, fib.lcbPlcffldTxbx, tableStream, &m_textbox );

    sanityCheck( tableStream, fib.fcSttbfbkmk, fib.lcbSttbfbkmk );
    read( fib.fcSttbfbkmk, fib.lcbSttbfbkmk, tableStream, &m_bookmark );

    // No sanity check here, plcOcx might be in between
    read( fib.fcPlcffldHdrTxbx, fib.lcbPlcffldHdrTxbx, tableStream, &m_headerTextbox );

    tableStream->pop();
}

Fields::~Fields()
{
    delete m_headerTextbox;
    delete m_textbox;
    delete m_endnote;
    delete m_annotation;
    delete m_footnote;
    delete m_header;
    delete m_main;
}

const FLD* Fields::fldForCP( Parser::SubDocument subDocument, U32 cp ) const
{
    switch( subDocument ) {
        case Parser::None:
            wvlog << "Error: The state of the parser is invalid!" << endl;
            return 0;
            break;
        case Parser::Main:
            return fldForCP( m_main, cp );
            break;
        case Parser::Footnote:
            return fldForCP( m_footnote, cp );
            break;
        case Parser::Header:
            return fldForCP( m_header, cp );
            break;
        case Parser::Macro:
            wvlog << "Warning: There shouldn't be any fields in macro text" << endl;
            return 0;
            break;
        case Parser::Annotation:
            return fldForCP( m_annotation, cp );
            break;
        case Parser::Endnote:
            return fldForCP( m_endnote, cp );
            break;
        case Parser::TextBox:
            return fldForCP( m_textbox, cp );
            break;
        case Parser::HeaderTextBox:
            return fldForCP( m_headerTextbox, cp );
            break;
        case Parser::Bookmark:
            return fldForCP( m_bookmark, cp );
            break;
    }
    return 0; // make the compiler happy, never reached
}

void Fields::read( U32 fc, U32 lcb, OLEStreamReader* tableStream, PLCFMap<FLD>** plcf )
{
    if ( lcb == 0 )
        return;
    tableStream->seek( fc, G_SEEK_SET );
    *plcf = new PLCFMap<FLD>( lcb, tableStream );
}

void Fields::sanityCheck( const OLEStreamReader* tableStream, U32 nextFC, U32 lcb ) const
{
    if ( lcb != 0 && static_cast<U32>( tableStream->tell() ) != nextFC )
        wvlog << "Warning: Detected a hole within the table stream (next fc=" << nextFC << ")" << endl;
}

const FLD* Fields::fldForCP( const PLCFMap<FLD>* plcf, U32 cp ) const
{
    if ( !plcf )
        return 0;

    return plcf->item( cp );
}
