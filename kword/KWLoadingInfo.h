/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWLOADINGINFO_H
#define KWLOADINGINFO_H

#include <qstring.h>
#include <q3valuelist.h>
#include <q3dict.h>
#include <KoPageLayout.h>

class KoTextParag;
class KWTextDocument;
class KWFrame;

/// Temporary information used only during loading
class KWLoadingInfo
{
public:
    KWLoadingInfo();
    ~KWLoadingInfo() {}

    /// Current master-page name (OASIS loading)
    QString m_currentMasterPage;

    /// Bookmarks (kword-1.3 XML: they need all framesets to be loaded first)
    struct BookMark
    {
        QString bookname;
        int paragStartIndex;
        int paragEndIndex;
        QString frameSetName;
        int cursorStartIndex;
        int cursorEndIndex;
    };

    typedef QList<BookMark> BookMarkList;
    BookMarkList bookMarkList;

    /// Bookmarks (OASIS XML). Only need to store bookmark starts, until hitting bookmark ends
    struct BookmarkStart {
        BookmarkStart() {} // for stupid QValueList
        BookmarkStart( KWTextDocument* _doc, KoTextParag* par, int ind )
            : doc( _doc ), parag( par ), pos( ind ) {}
        KWTextDocument* doc;
        KoTextParag* parag;
        int pos;
    };
    typedef QMap<QString, BookmarkStart> BookmarkStartsMap;
    BookmarkStartsMap m_bookmarkStarts;

    // Text frame chains; see KWTextFrameSet::loadOasisText

    void storeNextFrame( KWFrame* thisFrame, const QString& chainNextName ) {
        m_nextFrameDict.insert( chainNextName, thisFrame );
    }
    KWFrame* chainPrevFrame( const QString& frameName ) const {
        return m_nextFrameDict[frameName]; // returns 0 if not found
    }

    void storeFrameName( KWFrame* frame, const QString& name ) {
        m_frameNameDict.insert( name, frame );
    }
    KWFrame* frameByName( const QString& name ) const {
        return m_frameNameDict[name]; // returns 0 if not found
    }

    KoColumns columns;
    KoKWHeaderFooter hf;

private:
    // Ignore warnings about operator delete from those dicts; we don't use it here...
    Q3Dict<KWFrame> m_nextFrameDict;
    Q3Dict<KWFrame> m_frameNameDict;
};

#endif /* KWLOADINGINFO_H */

