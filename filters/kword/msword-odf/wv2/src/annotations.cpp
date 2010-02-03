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

#include "annotations.h"
#include "word_helper.h"
#include "word97_generated.h"
#include "olestream.h"

#include "wvlog.h"

using namespace wvWare;

Annotations::Annotations( OLEStreamReader* tableStream, const Word97::FIB& fib ) :
        m_annotationRef( 0 ), m_annotationRefIt( 0 )
{
#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "Annotations::Annotations "<< std::endl
          << "footnotes" << std::endl
          << "   fcPlcffndRef=" << fib.fcPlcffndRef << " lcbPlcffndRef=" << fib.lcbPlcffndRef << std::endl
          << "   fcPlcffndTxt=" << fib.fcPlcffndTxt << " lcbPlcffndTxt=" << fib.lcbPlcffndTxt << std::endl
          << "annotations" << std::endl
          << "   fcPlcfandRef=" << fib.fcPlcfandRef << " lcbPlcfandRef=" << fib.lcbPlcfandRef << std::endl
          << "   fcPlcfandTxt=" << fib.fcPlcfandTxt << " lcbPlcfandTxt=" << fib.lcbPlcfandTxt << std::endl;
#endif
    tableStream->push();
    // Annotations
    init( fib.fcPlcfandRef, fib.lcbPlcfandRef, fib.fcPlcfandTxt, fib.fcPlcfandTxt,
          tableStream, &m_annotationRef, &m_annotationRefIt, m_annotationTxt, m_annotationTxtIt );
    tableStream->pop();
}

Annotations::~Annotations()
{
    delete m_annotationRefIt;
    delete m_annotationRef;
}

AnnotationData Annotations::annotation( U32 globalCP, bool& ok )
{
#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "Annotations::annotation(): globalCP=" << globalCP << std::endl;
#endif
    ok = true; // let's assume we will find it
    if (    m_annotationRefIt && m_annotationRefIt->currentStart() == globalCP
            && m_annotationTxtIt != m_annotationTxt.end() ) {

        ++( *m_annotationRefIt ); // yay, but it is hard to make that more elegant

        U32 start = *m_annotationTxtIt;
        ++m_annotationTxtIt;
        return AnnotationData( start, *m_annotationTxtIt );
    }

    wvlog << "Bug: There is no annotation with the CP " << globalCP << std::endl;
    ok = false;
    return AnnotationData( 0, 0 );
}

U32 Annotations::nextAnnotation() const
{
    wvlog << "Annotations::nextAnnotation()" << std::endl;
    return m_annotationRefIt && m_annotationRefIt->current() ? m_annotationRefIt->currentStart() : 0xffffffff;
}

void Annotations::init( U32 fcRef, U32 lcbRef, U32 fcTxt, U32 lcbTxt, OLEStreamReader* tableStream,
                        PLCF<Word97::FRD>** ref, PLCFIterator<Word97::FRD>** refIt,
                        std::vector<U32>& txt, std::vector<U32>::const_iterator& txtIt )
{
    if ( lcbRef == 0 )
        return;

    tableStream->seek( fcRef, G_SEEK_SET );
    *ref = new PLCF<Word97::FRD>( lcbRef, tableStream );
    *refIt = new PLCFIterator<Word97::FRD>( **ref );

#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "Annotations::init()" << std::endl;
    ( *ref )->dumpCPs();
#endif

    if ( lcbTxt == 0 )
        wvlog << "Bug: lcbTxt == 0 but lcbRef != 0" << std::endl;
    else {
        if ( static_cast<U32>( tableStream->tell() ) != fcTxt ) {
            wvlog << "Warning: Found a hole in the table stream" << std::endl;
            tableStream->seek( fcTxt, G_SEEK_SET );
        }
        for ( U32 i = 0; i < lcbTxt; i += sizeof( U32 ) ) {
            txt.push_back( tableStream->readU32() );
#ifdef WV2_DEBUG_ANNOTATIONS
            wvlog << "read: " << txt.back() << std::endl;
#endif
        }
        txtIt = txt.begin();
    }
#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "Annotations::init() done" << std::endl;
#endif
}
