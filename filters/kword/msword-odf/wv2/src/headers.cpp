/* This file is part of the wvWare 2 project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>

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

#include "headers.h"
#include "olestream.h"
#include "wvlog.h"

using namespace wvWare;

const int Headers::headerTypes = 6;

Headers::Headers( U32 fcPlcfhdd, U32 lcbPlcfhdd, OLEStreamReader* tableStream, WordVersion version )
{
    if ( lcbPlcfhdd == 0 ) {
        return;
    }

    tableStream->push();
#ifdef WV2_DEBUG_HEADERS
    wvlog << "Headers::Headers(): fc=" << fcPlcfhdd << " lcb=" << lcbPlcfhdd << endl;

    //number of stories in PlcfHdd, first six stories specify footnote and
    //endnote separators, MS-DOC, p.33
    if ( version == Word8 ) {
	//second-to-last CP ends the last story, last CP must be ignored
	int n = lcbPlcfhdd / 4 - 2;
	wvlog << "num. of stories in PlcfHdd:" << n << endl; 
        wvlog << "num. of header/footer stories:" << n - 6 << endl;
    }
#endif

    //remove later (do we want asserts in here???)
    if ( lcbPlcfhdd % 4 ) {
        wvlog << "Bug: m_fib.lcbPlcfhdd % 4 != 0!" << endl;
    }
    else if ( version == Word8 && ( lcbPlcfhdd / 4 - 2 ) % headerTypes ) {
        wvlog << "Bug: #headers % " << headerTypes << " != 0!" << endl;
    }

    tableStream->seek( fcPlcfhdd, G_SEEK_SET );

    U32 i = 0;
    if ( version == Word8 ) {
        //footnote/endnote separators related stories
        for ( ; i < 6 * sizeof( U32 ); i += sizeof( U32 ) ) {
            tableStream->readU32();
        }
    }
    //header/footer related stories
    for ( ; i < lcbPlcfhdd; i += sizeof( U32 ) ) {
        m_headers.push_back( tableStream->readU32() );
    }

    tableStream->pop();
}

Headers::~Headers()
{
}

void Headers::set_headerMask( U8 /*sep_grpfIhdt*/ )
{
}
