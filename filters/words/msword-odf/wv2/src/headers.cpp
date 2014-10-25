/* This file is part of the wvWare 2 project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

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

#include "word97_generated.h"
#include "word_helper.h"

using namespace wvWare;

const uint Headers::headerTypes = 6;

Headers::Headers( U32 ccpHdd, U32 fcPlcfhdd, U32 lcbPlcfhdd, U32 fcPlcfsed, U32 lcbPlcfsed,
                  OLEStreamReader* tableStream, WordVersion version )
{
    if ( lcbPlcfhdd == 0 ) {
        return;
    }

    tableStream->push();

    //Number of stories in PlcfHdd.  First 6 stories specify footnote and
    //endnote separators.  The second-to-last CP ends the last story, the last
    //CP must be ignored. MS-DOC, p.33
    uint n = (lcbPlcfhdd / sizeof( U32 )) - 2;

#ifdef WV2_DEBUG_HEADERS
    wvlog << "ccpHdd=" << ccpHdd << "fc=" << fcPlcfhdd << " lcb=" << lcbPlcfhdd << endl;
    wvlog << "num. of stories in PlcfHdd:" << n << endl;
    if ( version == Word8 ) {
        wvlog << "num. of header/footer stories:" << n - 6 << endl;
    }
#endif
    if ( lcbPlcfhdd % sizeof( U32 ) ) {
        wvlog << "Bug: m_fib.lcbPlcfhdd % 4 != 0!" << endl;
    }
    else if ( (version == Word8) && (n % headerTypes) ) {
        wvlog << "Bug: #headers % " << headerTypes << " != 0!" << endl;
    }

    tableStream->seek( fcPlcfhdd, WV2_SEEK_SET );

    U32 i = 0;
    //CPs of footnote/endnote separators related stories
    if ( version == Word8 ) {
        for ( ; i < 6 * sizeof( U32 ); i += sizeof( U32 ) ) {
            tableStream->readU32();
        }
    }

    QList<U32> strsCPs;
    //CPs of header/footer related stories 
    for ( ; i < lcbPlcfhdd; i += sizeof( U32 ) ) {
        strsCPs.append( tableStream->readU32() );
    }

    //Add missing header/footer stories based on the number of sections.
    tableStream->seek( fcPlcfsed, WV2_SEEK_SET );
    PLCF<Word97::SED> plcfsed( lcbPlcfsed, tableStream );

    if ( version == Word8 ) {
        i = ((plcfsed.count() * headerTypes) + 2) - strsCPs.size();
        while (i > 0) {
            strsCPs.insert(strsCPs.size() - 2, ccpHdd - 1);
            i--;
        }
    }
    //Check num. of sections based on the num. of header/footer stories.
    if ( version == Word8 ) {
        uint m = (strsCPs.size() - 2) / headerTypes;
        Q_ASSERT(m = plcfsed.count()); Q_UNUSED(m);
    }

#ifdef WV2_DEBUG_HEADERS
    if ( version == Word8 ) {
        for (uint i = 0, l = 0; i < plcfsed.count(); i++) {
            wvlog << "Section" << i + 1;
            wvlog << "-------------------------------";
            for (uint j = 0; j < headerTypes; ++j, ++l) {
                wvlog << strsCPs[l];
            }
        }
    } else {
        for (int i = 0; i < strsCPs.size() - 2; ++i) {
            wvlog << strsCPs[i];
        }
    }
    wvlog << "-------------------------------";
    wvlog << "second-to-last:" << strsCPs.at(strsCPs.size() - 2);
    wvlog << "last:" << strsCPs.last();
#endif

    //Except for the last CP, each CP MUST be in <0, FibRgLw97.ccpHdd).  Each
    //story ends immediately prior to the next CP.  Again the second-to-last CP
    //ends the last story, the last CP must be ignored.
    if ( version == Word8 ) {
        QList< QList<U32> > sect;

        int l = 0;
        for (int k = 0; l < (strsCPs.size() - 2); k++) {
            sect.append( QList<U32>() );
            for (uint j = 0; j < headerTypes; j++, l++) {
                if ( (strsCPs[l] <= strsCPs[l + 1]) && (strsCPs[l] < ccpHdd) )
                {
                    sect[k].append(strsCPs[l]);
                }
            }
            //Section has invalid header/footer stories, use empty stories instead.
            if (sect[k].length() < 6) {
                sect[k].clear();
                //CPs of stories of the 1st section are screwed.
                if (k == 0) {
//                     throw InvalidFormatException("INVALID Header document detected");
                    for (uint j = 0; j < headerTypes; j++) {
                        sect[0].append(ccpHdd - 1);
                    }
                } else {
                    for (uint j = 0; j < headerTypes; j++) {
                        sect[k].append(sect[k - 1].last());
                    }
                }
            }
            m_headers.append(sect[k]);
        }
    } else {
        U32 cp = 0;
        for (int i = 0; i < strsCPs.size() - 2; i++) {
            if ( (strsCPs[i] <= strsCPs[i + 1]) && (strsCPs[i] < ccpHdd) ) {
                cp = strsCPs[i];
            } else {
                if (i == 0) {
                    cp = ccpHdd - 1;
                } else {
                    cp = strsCPs[i - 1];
                }
            }
            m_headers.append(cp);
        }
    }
    //append the second-to-last and the last CP
    m_headers.append(strsCPs.at(strsCPs.size() - 2));
    m_headers.append(strsCPs.last());

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
    for (U32 i = 0; i < (U32) m_headers.size(); ++i) {
        if (!(i % 6)) {
            wvlog << "----";
        }
        wvlog << "m_headers: " << m_headers[i];
    }
#endif
    //second-to-last CP ends the last story, last CP must be ignored
    for (U32 i = 0; i < (U32) (m_headers.size() - 2); i += 6) {
        nempty = false;
        for (U32 j = 0; j < 6; ++j) {
            if (m_headers[i + j] != m_headers[i + j + 1]) {
                nempty = true;
                break;
            }
        }
        mask.push_back(nempty);
    }

#ifdef WV2_DEBUG_HEADERS
    for (U32 i = 0; i < (U32) mask.size(); ++i) {
        wvlog << "Section" << i << ": new header/footer content: " << mask[i];
    }
#endif
    return mask;
}

void Headers::set_headerMask( U8 /*sep_grpfIhdt*/ )
{
}
