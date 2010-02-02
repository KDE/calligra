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

#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H

#include "global.h"
#include "functordata.h"

#include <vector>
#include "wv2_export.h"

namespace wvWare
{
    namespace Word97
    {
        struct FIB;
        struct FRD;
    }
    template<class T> class PLCF;
    template<class T> class PLCFIterator;
    class OLEStreamReader;
    struct AnnotationData;

    /**
     * @internal
     */
    class Annotations
    {
    public:
        Annotations( OLEStreamReader* tableStream, const Word97::FIB& fib );
        ~Annotations();

        /**
         * Get the AnnotationData for the Annotation at @param globalCP.
         * The @param ok flag is true if a Annotation has been found.
         * If @param ok is false no Annotation has been found and the
         * returned AnnotationData structure is invalid.
         */
        AnnotationData annotation( U32 globalCP, bool& ok );

        /**
         * Returns the global CP of the next Annotation reference,
         * 0xffffffff if none exists.
         */
        U32 nextAnnotation() const;

    private:
        Annotations( const Annotations& rhs );
        Annotations& operator=( const Annotations& rhs );

        // Ugly, but helps to avoid code duplication
        void init( U32 fcRef, U32 lcbRef, U32 fcTxt, U32 lcbTxt, OLEStreamReader* tableStream,
                   PLCF<Word97::FRD>** ref, PLCFIterator<Word97::FRD>** refIt,
                   std::vector<U32>& txt, std::vector<U32>::const_iterator& txtIt );

        PLCF<Word97::FRD>* m_annotationRef;
        PLCFIterator<Word97::FRD>* m_annotationRefIt;
        std::vector<U32> m_annotationTxt;
        std::vector<U32>::const_iterator m_annotationTxtIt;

    };

} // namespace wvWare

#endif // ANNOTATIONS_H
