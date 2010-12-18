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

#ifndef FUNCTORDATA_H
#define FUNCTORDATA_H

#include "sharedptr.h"
#include "wvlog.h"
#include "wv2_export.h"
#include "ustring.h"

namespace wvWare
{
    /**
     * @internal
     * Holds all the necessary information for asynchronous header/footer parsing.
     * The sectionNumber is a 0-based index, the headerMask holds bits of
     * the Type enum ORed together.
     */
    struct WV2_EXPORT HeaderData
    {
        enum Type { HeaderEven = 0x01, HeaderOdd = 0x02, FooterEven = 0x04,
                    FooterOdd = 0x08, HeaderFirst = 0x10, FooterFirst = 0x20 }; 

        enum Empty { NoHeaderEven = 0xfe, NoFooterEven = 0xfb, 
                     NoHeaderFirst = 0x2f, NoFooterFirst = 0x1f};

        HeaderData( int sectionNum ) : 
        sectionNumber( sectionNum ), headerMask( HeaderOdd | FooterOdd ) {}

        int sectionNumber;
        unsigned char headerMask;
    };

    /**
     * @internal
     * Holds all necessary information for delayed footnote/endnote parsing.
     */
    struct WV2_EXPORT FootnoteData
    {
        enum Type { Footnote, Endnote };

        FootnoteData( Type t, bool autoNum, unsigned int start, unsigned int lim ) :
                type( t ), autoNumbered( autoNum ), startCP( start ), limCP( lim ) {}

        Type type;
        bool autoNumbered;
        unsigned int startCP;
        unsigned int limCP;
    };

    /**
    * @internal
    * Holds all necessary information for delayed bookmark parsing.
    */
    struct WV2_EXPORT BookmarkData
    {
        BookmarkData( unsigned int start, unsigned int lim, UString nameRef  ) :
        startCP( start ), limCP( lim ), name(nameRef) {}

        unsigned int startCP;
        unsigned int limCP;
        UString name;
    };

    /**
     * @internal
     * Holds all necessary information for delayed annotation parsing.
     */
    struct WV2_EXPORT AnnotationData
    {
        AnnotationData( unsigned int start, unsigned int lim ) :
                startCP( start ), limCP( lim )
        {
        }

        unsigned int startCP;
        unsigned int limCP;
    };

    namespace Word97
    {
        struct TAP;
        struct PICF;
    }

    /**
     * @internal
     * Keeps track of the table (row) information. Tables are parsed
     * row by row.
     */
    struct WV2_EXPORT TableRowData
    {
        TableRowData( unsigned int sp, unsigned int so, unsigned int len,
                      int subDoc, SharedPtr<const Word97::TAP> sharedTap );
        ~TableRowData();

        unsigned int startPiece;
        unsigned int startOffset;
        unsigned int length;
        int subDocument; // int to avoid #including <parser.h> here
        SharedPtr<const Word97::TAP> tap;
    };


    /**
     * @internal
     * Holds the information about pictures inside the functor.
     */
    struct WV2_EXPORT PictureData
    {
        PictureData( unsigned int fc, SharedPtr<const Word97::PICF> sharedPicf );
        ~PictureData();

        unsigned int fcPic;
        SharedPtr<const Word97::PICF> picf;
    };

} // namespace wvWare

#endif // FUNCTORDATA_H
