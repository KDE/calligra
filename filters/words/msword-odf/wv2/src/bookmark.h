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


#ifndef BOOKMARK_H
#define BOOKMARK_H

#include "global.h"
#include "functordata.h"
#include "wv2_export.h"

#include <vector>
#include <QMap>

namespace wvWare
{
    namespace Word97
    {
        struct FIB;
        struct BKF;
        struct BKL;
    }
    template<class T> class PLCF;
    template<class T> class PLCFIterator;
    class OLEStreamReader;
    struct BookmarkData;

    /**
    * @internal
    */
    class Bookmarks
    {
        public:
            Bookmarks( OLEStreamReader* tableStream, const Word97::FIB& fib );
            ~Bookmarks();

            /**
             * Get the BookmarkData for the Bookmark-Start/End at @param
             * globalCP.  The @param ok flag is true if a bookmark has been
             * found.  If @param ok is false no valid bookmark has been found
             * and the returned BookmarkData structure is invalid.
             */
            BookmarkData bookmark( const U32 globalCP, bool& ok );

            /**
             * Get the BookmarkData for the Bookmark of @param name.  The
             * @param ok flag is true if a bookmark has been found.  If @param
             * ok is false no valid bookmark has been found and the returned
             * BookmarkData structure is invalid.
             */
            BookmarkData bookmark( const UString& name, bool& ok ) const;

            /**
             * Returns the global CP of the next bookmark start,
             * 0xffffffff if none exists or invalid.
             */
            U32 nextBookmarkStart();
            /**
             * Returns the global CP of the next bookmark end,
             * 0xffffffff if none exists or invalid.
             */
            U32 nextBookmarkEnd() const;

            /**
             * Check for unprocessed bookmars located before @param globalCP.
             * This might be the result of a skipped chunk or overlapping.
             * Any unprocessed bookmarks are skipped.
             */
            void check( U32 globalCP );

        private:
            Bookmarks( const Bookmarks& rhs );
            Bookmarks& operator=( const Bookmarks& rhs );

            /**
             * Validate bookmarks.
             * @param num. of invalid bookmarks detected
             */
            bool valid(U16 &num, const U32 ccpText);

            PLCF<Word97::BKF>* m_start;
            PLCFIterator<Word97::BKF>* m_startIt;

            std::vector<UString> m_name;
            std::vector<UString>::const_iterator m_nameIt;

            //Used for MS Word Version 6,7 documents.
            PLCF<Word97::BKL>* m_end;
            PLCFIterator<Word97::BKL>* m_endIt;

            //Used instead of m_end for MS Word 97 (aka Version 8)
            //documents.
            std::vector<U32> m_endCP;

            //A copy of fib.nFib to identify the document type durig
            //processing of bookmarks.
            U16 m_nFib;

            //A list providing the info if bookmarks are valid.
            QList<bool> m_valid;
    };

} // namespace wvWare

#endif // BOOKMARK_H
