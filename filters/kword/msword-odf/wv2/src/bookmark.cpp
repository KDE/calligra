/* This file is part of the wvWare 2 project
  Copyright (C) 2002-2003 KO GmbH <jean.nicolas.artaud@kogmbh.>

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
#include "kdebug.h"

using namespace wvWare;

Bookmarks::Bookmarks( OLEStreamReader* tableStream, const Word97::FIB& fib ) :
            m_start(0), m_startIt(0), m_end(0), m_endIt(0)
{
#ifdef WV2_DEBUG_BOOKMARK
    wvlog   << "Bookmark::Bookmark()" << endl
            << "   fcPlcfbkf=" << fib.fcPlcfbkf << " lcbPlcfbkf=" << fib.lcbPlcfbkf << endl
            << "   fcPlcfbkl=" << fib.fcPlcfbkl << " lcbPlcfbkl=" << fib.lcbPlcfbkl << endl
            << "   lcbSttbfbkmk=" << fib.fcSttbfbkmk << " lcbSttbfbkmk=" << fib.lcbSttbfbkmk << endl;
#endif

    tableStream->push();

    /// Init the bookmark


    if (fib.lcbPlcfbkf != 0)
    {
        tableStream->seek( fib.fcPlcfbkf, G_SEEK_SET );

        m_start = new PLCF<Word97::BKF>(fib.lcbPlcfbkf, tableStream);
        m_startIt = new PLCFIterator<Word97::BKF>(*m_start);
    }
#ifdef WV2_DEBUG_BOOKMARK
    wvlog << "Bookmark m_start init done" << endl;
    m_start->dumpCPs();
#endif

    if ( fib.lcbSttbfbkmk != 0 )
    {
        if ( static_cast<U32>( tableStream->tell() ) != fib.fcSttbfbkmk ) {
            tableStream->seek( fib.fcSttbfbkmk, G_SEEK_SET );
        }

        U16 usLid = 0x409; // The bookmark names in the STTBF are always Unicode, the lid doesn't matter
        STTBF* name = new STTBF( usLid, tableStream, false );
        for ( U32 i = 0; i < fib.lcbSttbfbkmk; i += sizeof( U32 ) ) {
            U32 txt = tableStream->readU32();
            m_text.push_back( txt );
#ifdef WV2_DEBUG_BOOKMARK
           wvlog << "read: " << m_text.back() << endl;
           name->dumpStrings();
#endif
        }
        m_textIt = m_text.begin();

        for (unsigned int i = 0; i < name->count(); i++ ) {
            m_name.push_back(name->stringAt(i));
        }
        m_nameIt = m_name.begin();
    }

    if (fib.lcbPlcfbkl != 0)
    {
        tableStream->seek( fib.fcPlcfbkl, G_SEEK_SET );

        m_end = new PLCF<Word97::BKL>(fib.lcbPlcfbkl, tableStream);
        m_endIt = new PLCFIterator<Word97::BKL>(*m_end);
    }


#ifdef WV2_DEBUG_BOOKMARK
    wvlog << "Bookmark init done" << endl;
#endif

    tableStream->pop();
}

Bookmarks::~Bookmarks()
{
    delete m_endIt;
    delete m_end;
    delete m_startIt;
    delete m_start;
}

BookmarkData Bookmarks::bookmark( U32 globalCP, bool& ok )
{
#ifdef WV2_DEBUG_BOOKMARK
    wvlog << "Bookmarks::bookmark(): globalCP=" << globalCP << endl;
#endif
    ok = true;

    if ( m_startIt && m_startIt->currentStart() == globalCP &&
        m_textIt != m_text.end() ) {

        ++( *m_startIt ); // yay, but it is hard to make that more elegant
        ++( *m_endIt );

        U32 start = *m_textIt;
        ++m_textIt;

#ifdef WV2_DEBUG_BOOKMARK
        wvlog << "Bookmarks::bookmark(): start = " << start << endl;
        wvlog << "Bookmarks::bookmark(): name = " << (*m_nameIt).ascii() << endl;
#endif
        UString name = *m_nameIt;
        ++m_nameIt;
        return BookmarkData( start, *m_textIt, name );
    }

    ok = false;
    return  BookmarkData( 0, 0, wvWare::UString("") );

}

U32 Bookmarks::nextBookmarkStart() const
{
    return m_startIt && m_startIt->current() ? m_startIt->currentStart() : 0xffffffff;
}

U32 Bookmarks::nextBookmarkEnd() const
{
    return m_endIt && m_endIt->current() ? m_endIt->currentStart() : 0xffffffff;
}