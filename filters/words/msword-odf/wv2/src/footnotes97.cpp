/* This file is part of the wvWare 2 project
   Copyright (C) 2002-2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>

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

#include "footnotes97.h"
#include "word_helper.h"
#include "word97_generated.h"
#include "olestream.h"

#include "wvlog.h"

using namespace wvWare;

Footnotes97::Footnotes97( OLEStreamReader* tableStream, const Word97::FIB& fib ) :
    m_footnoteRef( 0 ), m_footnoteRefIt( 0 ), m_endnoteRef( 0 ), m_endnoteRefIt( 0 )
{
#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "Footnotes97::Footnotes97()" << endl
          << "   fcPlcffndRef=" << fib.fcPlcffndRef << " lcbPlcffndRef=" << fib.lcbPlcffndRef << endl
          << "   fcPlcffndTxt=" << fib.fcPlcffndTxt << " lcbPlcffndTxt=" << fib.lcbPlcffndTxt << endl
          << "   fcPlcfendRef=" << fib.fcPlcfendRef << " lcbPlcfendRef=" << fib.lcbPlcfendRef << endl
          << "   fcPlcfendTxt=" << fib.fcPlcfendTxt << " lcbPlcfendTxt=" << fib.lcbPlcfendTxt << endl;
#endif
    tableStream->push();

#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "ccpFtn:" << fib.ccpFtn;
    wvlog << "ccpEdn:" << fib.ccpEdn;
#endif

    //TODO: validation required!

    // Footnotes
    init( fib.fcPlcffndRef, fib.lcbPlcffndRef, fib.fcPlcffndTxt, fib.lcbPlcffndTxt,
          tableStream, &m_footnoteRef, &m_footnoteRefIt, m_footnoteTxt, m_footnoteTxtIt );
    // Endnotes
    init( fib.fcPlcfendRef, fib.lcbPlcfendRef, fib.fcPlcfendTxt, fib.lcbPlcfendTxt,
          tableStream, &m_endnoteRef, &m_endnoteRefIt, m_endnoteTxt, m_endnoteTxtIt );
    tableStream->pop();
}

Footnotes97::~Footnotes97()
{
    delete m_endnoteRefIt;
    delete m_endnoteRef;
    delete m_footnoteRefIt;
    delete m_footnoteRef;
}

FootnoteData Footnotes97::footnote( U32 globalCP, bool& ok )
{
#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "Footnotes97::footnote(): globalCP=" << globalCP << endl;
#endif
    ok = true; // let's assume we will find it
    if ( m_footnoteRefIt && m_footnoteRefIt->currentStart() == globalCP &&
         m_footnoteTxtIt != m_footnoteTxt.end() ) {
        bool fAuto = m_footnoteRefIt->current()->nAuto;
        ++( *m_footnoteRefIt ); // yay, but it is hard to make that more elegant

        U32 start = *m_footnoteTxtIt;
        ++m_footnoteTxtIt;
        return FootnoteData( FootnoteData::Footnote, fAuto, start, *m_footnoteTxtIt );
    }

    if ( m_endnoteRefIt && m_endnoteRefIt->currentStart() == globalCP &&
         m_endnoteTxtIt != m_endnoteTxt.end() ) {
        bool fAuto = m_endnoteRefIt->current()->nAuto;
        ++( *m_endnoteRefIt ); // yay, but it is hard to make that more elegant

        U32 start = *m_endnoteTxtIt;
        ++m_endnoteTxtIt;
        return FootnoteData( FootnoteData::Endnote, fAuto, start, *m_endnoteTxtIt );
    }

    wvlog << "Bug: There is no footnote or endnote with the CP " << globalCP << endl;
    ok = false;
    return FootnoteData( FootnoteData::Footnote, false, 0, 0 );
}

U32 Footnotes97::nextFootnote() const
{
    return m_footnoteRefIt && m_footnoteRefIt->current() ? m_footnoteRefIt->currentStart() : 0xffffffff;
}

U32 Footnotes97::nextEndnote() const
{
    return m_endnoteRefIt && m_endnoteRefIt->current() ? m_endnoteRefIt->currentStart() : 0xffffffff;
}

void Footnotes97::init( U32 fcRef, U32 lcbRef, U32 fcTxt, U32 lcbTxt, OLEStreamReader* tableStream,
                        PLCF<Word97::FRD>** ref, PLCFIterator<Word97::FRD>** refIt,
                        std::vector<U32>& txt, std::vector<U32>::const_iterator& txtIt )
{
    if ( lcbRef == 0 )
        return;

    tableStream->seek( fcRef, WV2_SEEK_SET );
    *ref = new PLCF<Word97::FRD>( lcbRef, tableStream );
    *refIt = new PLCFIterator<Word97::FRD>( **ref );

#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "Footnotes97::init()" << endl;
    ( *ref )->dumpCPs();
#endif

    if ( lcbTxt == 0 )
        wvlog << "Bug: lcbTxt == 0 but lcbRef != 0" << endl;
    else {
        if ( static_cast<U32>( tableStream->tell() ) != fcTxt ) {
            wvlog << "Warning: Found a hole in the table stream" << endl;
            tableStream->seek( fcTxt, WV2_SEEK_SET );
        }
        for ( U32 i = 0; i < lcbTxt; i += sizeof( U32 ) ) {
            txt.push_back( tableStream->readU32() );
#ifdef WV2_DEBUG_FOOTNOTES
            wvlog << "read: " << txt.back() << endl;
#endif
        }
        txtIt = txt.begin();
    }
#ifdef WV2_DEBUG_FOOTNOTES
    wvlog << "Footnotes97::init() done" << endl;
#endif
}

void Footnotes97::check( U32 globalCP )
{
    while (nextFootnote() < globalCP) {
        ++( *m_footnoteRefIt );
        ++m_footnoteTxtIt;
#ifdef WV2_DEBUG_FOOTNOTES
        wvlog << "Footnote skipped!";
#endif
    }

    while (nextEndnote() < globalCP) {
        ++( *m_endnoteRefIt );
        ++m_endnoteTxtIt;
#ifdef WV2_DEBUG_FOOTNOTES
        wvlog << "Endnote skipped!";
#endif
    }
}
