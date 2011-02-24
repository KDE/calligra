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

Headers::Headers( U32 ccpHdd, U32 fcPlcfhdd, U32 lcbPlcfhdd, OLEStreamReader* tableStream, WordVersion version )
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
    QList<U32> tmp;

    if ( version == Word8 ) {
        //CPs of footnote/endnote separators related stories
        for ( ; i < 6 * sizeof( U32 ); i += sizeof( U32 ) ) {
            tableStream->readU32();
        }
    }
    //CPs of header/footer related stories 
    for ( ; i < lcbPlcfhdd; i += sizeof( U32 ) ) {
        tmp.append( tableStream->readU32() );
    }

    //NOTE: Except for the last CP, each CP of Plcfhdd MUST be greater than or
    //equal to 0 and less than FibRgLw97.ccpHdd.  Each story ends immediately
    //prior to the next CP.  Again second-to-last CP ends the last story, last
    //CP must be ignored.

    uint n = lcbPlcfhdd / sizeof( U32 ) - 2; //num. of stories in PlcfHdd
    uint m = (n - 6) / 6; //num. of sections based on the num. of header/footer stories
    QList<U32> sct[m];

    //Validate CPs!
    int l = 0;
    for (int k = 0; l < (tmp.length() - 3); k++) {
        for (int j = 0; j < 6; j++, l++) {
            if ((tmp[l] <= tmp[l + 1]) && (tmp[l] < ccpHdd)) { 
                sct[k].append(tmp[l]);
            }
        }
        //section has invalid header/footer stories, simulate that the header
        //or footer of the corresponding type of the previous section is used
        if (sct[k].length() < 6) {
            for (int j = 0; j < 6; j++) {
                m_headers.append(sct[k - 1].last());
            }
        } else {
            m_headers.append(sct[k]);
	}
    }

    //append second-to-last and last CP
    m_headers.append(tmp[l]);
    m_headers.append(tmp[l + 1]);

    tableStream->pop();
}

Headers::~Headers()
{
}

QList<bool> Headers::headersMask( void )
{
    //NOTE: Stories are considered empty if they have no contents and no guard
    //paragraph mark.  Thus, an empty story is indicated by the story`s
    //starting CP, as specified in PlcfHdd, being the same as the next CP in
    //PlcfHdd.  MS-DOC, p.33

    bool nempty;
    QList<bool> mask;

#ifdef WV2_DEBUG_HEADERS
    for (U32 i = 0; i < (U32) m_headers.size(); i++) {
        if (!(i % 6)) {
            wvlog << "----";
        }
        wvlog << "m_headers: " << m_headers[i];
    }
#endif
    //second-to-last CP ends the last story, last CP must be ignored
    for (U32 i = 0; i < (U32) (m_headers.size() - 2); i += 6) {
        nempty = false;
        for (U32 j = 0; j < 6; j++) {
            if (m_headers[i + j] != m_headers[i + j + 1]) {
                nempty = true;
                break;
            }
        }
        mask.push_back(nempty);
    }

#ifdef WV2_DEBUG_HEADERS
    for (U32 i = 0; i < (U32) mask.size(); i++) {
        wvlog << "Section" << i << ": new header/footer content: " << mask[i];
    }
#endif
    return mask;
}

void Headers::set_headerMask( U8 /*sep_grpfIhdt*/ )
{
}
