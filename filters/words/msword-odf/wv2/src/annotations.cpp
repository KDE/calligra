/* This file is part of the wvWare 2 project
   SPDX-FileCopyrightText: 2002-2003 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

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
#include "olestream.h"
#include "word97_generated.h"
#include "word_helper.h"

#include "wvlog.h"

using namespace wvWare;

Annotations::Annotations(OLEStreamReader *tableStream, const Word97::FIB &fib)
    : m_annotationRef(nullptr)
    , m_annotationRefIt(nullptr)
{
#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << Qt::endl
          << "footnotes" << Qt::endl
          << "   fcPlcffndRef=" << fib.fcPlcffndRef << " lcbPlcffndRef=" << fib.lcbPlcffndRef << Qt::endl
          << "   fcPlcffndTxt=" << fib.fcPlcffndTxt << " lcbPlcffndTxt=" << fib.lcbPlcffndTxt << Qt::endl
          << "annotations" << Qt::endl
          << "   fcPlcfandRef=" << fib.fcPlcfandRef << " lcbPlcfandRef=" << fib.lcbPlcfandRef << Qt::endl
          << "   fcPlcfandTxt=" << fib.fcPlcfandTxt << " lcbPlcfandTxt=" << fib.lcbPlcfandTxt << Qt::endl;
#endif
    tableStream->push();
    // Annotations
    init(fib.fcPlcfandRef,
         fib.lcbPlcfandRef,
         fib.fcPlcfandTxt,
         fib.lcbPlcfandTxt,
         tableStream,
         &m_annotationRef,
         &m_annotationRefIt,
         m_annotationTxt,
         m_annotationTxtIt);
    tableStream->pop();
}

Annotations::~Annotations()
{
    delete m_annotationRefIt;
    delete m_annotationRef;
}

AnnotationData Annotations::annotation(U32 globalCP, bool &ok)
{
#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << " globalCP=" << globalCP << Qt::endl;
#endif

    if (m_annotationRefIt && m_annotationRefIt->currentStart() == globalCP && m_annotationTxtIt != m_annotationTxt.end()) {
        ok = true;

        // yay, but it is hard to make that more elegant
        ++(*m_annotationRefIt);

        U32 start = *m_annotationTxtIt;
        ++m_annotationTxtIt;
        U32 lim = *m_annotationTxtIt;

#ifdef WV2_DEBUG_ANNOTATIONS
        wvlog << "start:" << start << Qt::endl;
        wvlog << "lim:" << lim << Qt::endl;
#endif
        return AnnotationData(start, lim);
    } else {
        ok = false;
        wvlog << "Bug: There is no annotation with the CP " << globalCP << Qt::endl;
        return AnnotationData(0, 0);
    }
}

U32 Annotations::nextAnnotation() const
{
    wvlog << "Annotations::nextAnnotation()" << Qt::endl;
    return m_annotationRefIt && m_annotationRefIt->current() ? m_annotationRefIt->currentStart() : 0xffffffff;
}

void Annotations::init(U32 fcRef,
                       U32 lcbRef,
                       U32 fcTxt,
                       U32 lcbTxt,
                       OLEStreamReader *tableStream,
                       PLCF<Word97::FRD> **ref,
                       PLCFIterator<Word97::FRD> **refIt,
                       std::vector<U32> &txt,
                       std::vector<U32>::const_iterator &txtIt)
{
    if (lcbRef == 0) {
        return;
    }

    tableStream->seek(fcRef, WV2_SEEK_SET);
    *ref = new PLCF<Word97::FRD>(lcbRef, tableStream);
    *refIt = new PLCFIterator<Word97::FRD>(**ref);

#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "[ PlcfandRef ]" << Qt::endl;
    (*ref)->dumpCPs();
#endif

    if (lcbTxt == 0) {
        wvlog << "Bug: lcbTxt == 0 but lcbRef != 0" << Qt::endl;
    } else {
        if (static_cast<U32>(tableStream->tell()) != fcTxt) {
            wvlog << "Warning: Found a hole in the table stream" << Qt::endl;
            tableStream->seek(fcTxt, WV2_SEEK_SET);
        }
#ifdef WV2_DEBUG_ANNOTATIONS
        wvlog << "[ PlcfandTxt ]" << Qt::endl;
#endif
        for (U32 i = 0; i < lcbTxt; i += sizeof(U32)) {
            txt.push_back(tableStream->readU32());
#ifdef WV2_DEBUG_ANNOTATIONS
            wvlog << "CP: " << txt.back() << Qt::endl;
#endif
        }
        txtIt = txt.begin();
    }

    // TODO: ATRDPost10, ATRDPre10, XSTs at position fcGrpXstAtnOwners

#ifdef WV2_DEBUG_ANNOTATIONS
    wvlog << "Annotation init done" << Qt::endl;
#endif
}
