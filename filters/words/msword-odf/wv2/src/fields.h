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

#ifndef FIELDS_H
#define FIELDS_H

#include "parser.h"
#include "wv2_export.h"

namespace wvWare
{
    namespace Word97
    {
        struct FIB;
    }
    template<class T> class PLCF;
    template<class T> class PLCFMap;
    class OLEStreamReader;

    struct FLD
    {
        FLD();
        FLD( OLEStreamReader* stream, bool preservePos = false );
        FLD( const U8* ptr );

        bool read( OLEStreamReader* stream, bool preservePos = false );
        bool readPtr( const U8* ptr );

        void clear();

        // Data
        U8 ch;
        union
        {
            U8 flt;
            struct
            {
                U8 fDiffer:1;
                U8 fZomieEmbed:1;
                U8 fResultDirty:1;
                U8 fResultEdited:1;
                U8 fLocked:1;
                U8 fPrivateResult:1;
                U8 fNested:1;
                U8 fHasSep:1;
            } flags;
        };

        // Size of the structure (needed for the PLCF template)
        static const unsigned int sizeOf;
    };

    bool operator==( const FLD &lhs, const FLD &rhs );
    bool operator!=( const FLD &lhs, const FLD &rhs );


    class Fields
    {
    public:
        Fields( OLEStreamReader* tableStream, const Word97::FIB& fib );
        ~Fields();

        const FLD* fldForCP( Parser::SubDocument subDocument, U32 cp ) const;

    private:
        Fields( const Fields& rhs );
        Fields& operator=( const Fields& rhs );

        void read( U32 fc, U32 lcb, OLEStreamReader* tableStream, PLCFMap<FLD>** plcf );
        void sanityCheck( const OLEStreamReader* tableStream, U32 nextFC, U32 lcb ) const;
        const FLD* fldForCP( const PLCFMap<FLD>* plcf, U32 cp ) const;

        PLCFMap<FLD>* m_main;
        PLCFMap<FLD>* m_header;
        PLCFMap<FLD>* m_footnote;
        PLCFMap<FLD>* m_annotation;
        PLCFMap<FLD>* m_endnote;
        PLCFMap<FLD>* m_textbox;
        PLCFMap<FLD>* m_headerTextbox;
        PLCFMap<FLD>* m_bookmark;
    };

} // namespace wvWare

#endif // FIELDS_H
