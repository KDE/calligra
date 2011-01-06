/* This file is part of the wvWare 2 project
   Copyright (C) 2002, 2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef ASSOCSTRINGS_H
#define ASSOCSTRINGS_H

#include "global.h"
#include "wv2_export.h"

namespace wvWare
{
    class OLEStreamReader;
    class STTBF;
    class UString;

    /**
     * AssociatedStrings provides a confortable way to access Word's STTBFASSOC,
     * the table of associated strings. This table holds information about the
     * author of the document, the tile, keywords, and so on.
     */
    class WV2_EXPORT AssociatedStrings
    {
    public:
        /**
         * You shouldn't have to create such an object yourself, the Parser provides
         * it.
         */
        AssociatedStrings( U32 fcSttbfAssoc, U32 lcbSttbfAssoc, U16 lid, OLEStreamReader* tableStream );
        AssociatedStrings( const AssociatedStrings& rhs );
        ~AssociatedStrings();

        /**
         * Provides access to the string described as ibstAssocDot
         */
        UString associatedTemplate() const;
        /**
         * Provides access to the string described as ibstAssocTitle
         */
        UString title() const;
        /**
         * Provides access to the string described as ibstAssocSubject
         */
        UString subject() const;
        /**
         * Provides access to the string described as ibstAssocKeyWords
         */
        UString keywords() const;
        /**
         * Provides access to the string described as ibstAssocComments
         */
        UString comments() const;
        /**
         * Provides access to the string described as ibstAssocAuthor
         */
        UString author() const;
        /**
         * Provides access to the string described as ibstAssocLastRevBy
         */
        UString lastRevBy() const;
        /**
         * Provides access to the string described as ibstAssocDataDoc
         */
        UString dataDocument() const;
        /**
         * Provides access to the string described as ibstAssocHeaderDoc
         */
        UString headerDocument() const;

    private:
        AssociatedStrings& operator=( const AssociatedStrings& rhs );

        STTBF* m_sttbf;
    };

} // namespace wvWare

#endif // ASSOCSTRINGS_H
