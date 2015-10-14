/* This file is part of the wvWare 2 project
   Copyright (C) 2002-2003 KO GmbH <jean.nicolas.artaud@kogmbh.>
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

#include "bookmark.h"
#include "word_helper.h"
#include "word97_generated.h"
#include "olestream.h"

#include "wvlog.h"
#include <QList>

using namespace wvWare;

Bookmarks::Bookmarks( OLEStreamReader* tableStream, const Word97::FIB& fib ) :
    m_start(0), m_startIt(0), m_end(0), m_endIt(0), m_nFib(fib.nFib)
{
#ifdef WV2_DEBUG_BOOKMARK
    wvlog   << endl
            << "   fcPlcfbkf=" << fib.fcPlcfbkf << " lcbPlcfbkf=" << fib.lcbPlcfbkf << endl
            << "   fcPlcfbkl=" << fib.fcPlcfbkl << " lcbPlcfbkl=" << fib.lcbPlcfbkl << endl
            << "   lcbSttbfbkmk=" << fib.fcSttbfbkmk << " lcbSttbfbkmk=" << fib.lcbSttbfbkmk << endl;
#endif

    tableStream->push();

    if (fib.lcbPlcfbkf != 0)
    {
        tableStream->seek( fib.fcPlcfbkf, WV2_SEEK_SET );

        m_start = new PLCF<Word97::BKF>(fib.lcbPlcfbkf, tableStream);
        m_startIt = new PLCFIterator<Word97::BKF>(*m_start);

#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "Num. of bookmarks to start: " << m_start->count() << endl;
        wvlog << "m_start init done" << endl;
        m_start->dumpCPs();
#endif
    }

    if ( fib.lcbSttbfbkmk != 0 )
    {
        if ( static_cast<U32>( tableStream->tell() ) != fib.fcSttbfbkmk ) {
            tableStream->seek( fib.fcSttbfbkmk, WV2_SEEK_SET );
        }
        // The bookmark names in the STTBF are always Unicode, the lid doesn't matter
        U16 usLid = 0x409;
        STTBF* name = new STTBF( usLid, tableStream, false );

#ifdef WV2_DEBUG_BOOKMARK
        name->dumpStrings();
#endif
        for (uint i = 0; i < name->count(); i++ ) {
            m_name.push_back(name->stringAt(i));
        }

        delete name;
    }

    //The BKL is no longer stored in the plcfbkl or plcfatnbkl, and is instead
    //reconstructed from the plcfbkf or plcfatnbkf when the file is opened.
    //Microsoft Word 97 (aka Version 8)

    //TODO: Reconstruct BKL!

    if (fib.lcbPlcfbkl != 0)
    {
        int count = 0;
        tableStream->seek( fib.fcPlcfbkl, WV2_SEEK_SET );

        //Word Version 6,7
        if ( fib.nFib < Word8nFib ) {
            m_end = new PLCF<Word97::BKL>(fib.lcbPlcfbkl, tableStream);
            m_endIt = new PLCFIterator<Word97::BKL>(*m_end);
            count = m_end->count();
        } else {
            count = (fib.lcbPlcfbkl - 4) / 4;
            for ( int i = 0; i < count + 1; i++ ) {
                m_endCP.push_back( tableStream->readU32() );
            }
        }

#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "Num. of bookmarks to end: " << count << endl;
        wvlog << "m_end/m_endCP init done" << endl;

        if ( fib.nFib < Word8nFib ) {
            m_end->dumpCPs();
        } else {
            for ( uint i = 0; i < m_endCP.size(); i++ ) {
                wvlog << "dumpCPs:   " << m_endCP[i] << endl;
            }
        }
#endif
    }

#ifdef WV2_DEBUG_BOOKMARK
    wvlog << "Bookmark init done" << endl;
#endif

    tableStream->pop();

    U16 num = 0;
    if (!valid(num, fib.ccpText)) {
        wvlog << "Num. of invalid bookmarks:" << num;
    }

    //using custom bookmark names if missing!
    m_nameIt = m_name.begin();
}

Bookmarks::~Bookmarks()
{
    if ( m_nFib < Word8nFib ) {
        delete m_endIt;
        delete m_end;
    }
    delete m_startIt;
    delete m_start;
}

BookmarkData Bookmarks::bookmark( const U32 globalCP, bool& ok )
{
#ifdef WV2_DEBUG_BOOKMARK
    wvlog << " globalCP=" << globalCP << endl;
#endif
    ok = false;
    if ( (m_startIt && m_startIt->current()) &&
         (m_startIt->currentStart() == globalCP) &&
         (m_nameIt != m_name.end()) )
    {
        if (m_valid.isEmpty()) {
            wvlog << "BUG: m_valid empty?";
        } else if (m_valid.first()) {
            ok = true;
        }

        U32 start = m_startIt->currentStart();
        U32 end = start;

        if (m_nFib < Word8nFib) {
            end = m_endIt->currentStart();
            ++( *m_endIt );
        } else {
            U16 ibkl = m_startIt->current()->ibkl;
            end = m_endCP[ibkl];
        }

        ++( *m_startIt );
        m_valid.removeFirst();

        UString name = *m_nameIt;
        ++m_nameIt;

#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "start = " << start << endl;
        wvlog << "end = " << end << endl;
        wvlog << "name = " << name.ascii() << endl;
        wvlog << "valid = " << ok << endl;
#endif
        return BookmarkData( start, end, name );
    }
    return BookmarkData( 0, 0, wvWare::UString("") );
}

BookmarkData Bookmarks::bookmark(const UString& name, bool& ok ) const
{
    std::vector<UString>::const_iterator nameIt = m_name.begin();
    PLCFIterator<Word97::BKF> startIt(*m_start);

    PLCFIterator<Word97::BKL>* endIt = 0;
    if (m_nFib < Word8nFib) {
        endIt = new PLCFIterator<Word97::BKL>(*m_end);
    }

    while (startIt.current()) {
        if (*nameIt == name) {
            U32 start = startIt.currentStart();
            U32 end = start;
            if (m_nFib < Word8nFib) {
                end = endIt->currentStart();
                delete endIt;
            } else {
                U16 ibkl = startIt.current()->ibkl;
                end = m_endCP[ibkl];
            }
            ok = true;
            return BookmarkData( start, end, name );
        }
        ++startIt;
        ++nameIt;

        if (m_nFib < Word8nFib) {
            ++( *endIt );
        }
    }
    if (m_nFib < Word8nFib) {
        delete endIt;
    }
    ok = false;
    return BookmarkData( 0, 0, UString("") );
}

U32 Bookmarks::nextBookmarkStart()
{
    U32 ret = 0xffffffff;

    //find the next valid bookmark
    while (m_startIt && m_startIt->current()) {

        if (m_valid.isEmpty()) {
            wvlog << "BUG: m_valid empty?";
            break;
        }
        else if (m_valid.first()) {
            ret = m_startIt->currentStart();
            break;
        } else {
            //NOTE: Add logic to process invalid bookmarks here.

            if (m_nFib < Word8nFib) {
                ++( *m_endIt );
            }
            m_valid.removeFirst();
            ++( *m_startIt );
            ++m_nameIt;

#ifdef WV2_DEBUG_BOOKMARK
            wvlog << "Warning: Skipped invalid bookmark!";
#endif
        }
    }
    return ret;
}

U32 Bookmarks::nextBookmarkEnd() const
{
    U32 ret = 0xffffffff;

    if (m_nFib < Word8nFib) {
        if (m_endIt && m_endIt->current()) {
            ret = m_endIt->currentStart();
        }
    } else {
        if (m_startIt && m_startIt->current()) {
            U16 ibkl = (m_startIt->current())->ibkl;
            ret = m_endCP[ibkl];
        }
    }
    return ret;
}

void Bookmarks::check( U32 globalCP )
{
    while (nextBookmarkStart() < globalCP)
    {
        if (m_nFib < Word8nFib) {
            ++( *m_endIt );
        }
        ++( *m_startIt );
        ++m_nameIt;

        if (m_valid.isEmpty()) {
            wvlog << "BUG: m_valid empty?";
        } else {
            m_valid.removeFirst();
        }

#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "Bookmark skipped! CP:" << globalCP;
#endif
    }
}

bool Bookmarks::valid(U16 &num, const U32 ccpText)
{
    PLCFIterator<Word97::BKF> startIt(*m_start);
    QList<U16> ibkls;
    bool ret = true;
    U16 ibkl = 0;
    num = 0;

#ifdef WV2_DEBUG_BOOKMARK
    U16 n = 1;
#endif

    if (m_nFib < Word8nFib) {
        PLCFIterator<Word97::BKL> endIt(*m_end);
        while (startIt.current()) {
            if ( !endIt.current() ||
                 (startIt.currentStart() > endIt.currentStart()) ||
                 (startIt.currentStart() > ccpText) )
            {
                m_valid.append(false);
                ret = false;
                num++;
#ifdef WV2_DEBUG_BOOKMARK
                wvlog << "bkmk" << n << ": (startCP > endCP) || endCP missing";
#endif
            } else {
                m_valid.append(true);
            }
#ifdef WV2_DEBUG_BOOKMARK
            n++;
#endif
            ++startIt;
            ++endIt;
        }
    } else {
        while (startIt.current()) {
            ibkl = (startIt.current())->ibkl;
            //MUST be unique for all FBKFs inside a given PlcfBkf
            if (ibkls.contains(ibkl) || (ibkl > m_endCP.size())) {
                m_valid.append(false);
                ret = false;
                num++;
#ifdef WV2_DEBUG_BOOKMARK
                wvlog << "bkmk" << n << ": ibkl invalid!";
                n++;
#endif
                ++startIt;
                continue;
            } else {
                ibkls.append(ibkl);
            }

            if ( (startIt.currentStart() > m_endCP[ibkl]) ||
                 (startIt.currentStart() > ccpText) )
            {
                m_valid.append(false);
                ret = false;
                num++;
#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "bkmk" << n << ": startCP > endCP (" <<
                    startIt.currentStart() << "|" << m_endCP[ibkl] << ")";
#endif
            } else {
                m_valid.append(true);
            }
#ifdef WV2_DEBUG_BOOKMARK
            n++;
#endif
            ++startIt;
        }
    }

    //check bookmark names
    for (uint i = 0; i < m_name.size(); i++) {
        if ( (m_name[i] == UString::null) ) {
            m_name[i] = UString().from(i + 1);
        }
    }
    if (m_name.size() < m_start->count()) {
        for (uint i = m_name.size(); i < m_start->count(); i++) {
            m_name.push_back(UString().from(i + 1));
        }
#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "Warning: bookmark names missing!  Using custom names.";
        wvlog << "Num. of bookmark names:" << m_name.size();

        std::vector<UString>::const_iterator it = m_name.begin();
        while(it != m_name.end()) {
            wvlog << "bkmk name:" << (*it).ascii();
            ++it;
        }
#endif
    }
    return ret;
}
