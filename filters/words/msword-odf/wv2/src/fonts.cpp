/* This file is part of the wvWare 2 project
   SPDX-FileCopyrightText: 2002-2003 Werner Trobin <trobin@kde.org>

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

#include "fonts.h"
#include "olestream.h"
#include "utilities.h"
#include "word97_generated.h"

#include "wvlog.h"

using namespace wvWare;

FontCollection::FontCollection(OLEStreamReader *reader, const Word97::FIB &fib)
{
    m_fallbackFont = new Word97::FFN();
    if (!m_fallbackFont) {
        wvlog << "Error: FFN allocation!";
        return;
    }
    m_fallbackFont->xszFfn = "Helvetica";

    reader->push();
    reader->seek(fib.fcSttbfffn);

    if (fib.nFib < Word8nFib) { // older than Word97
        int bytesLeft = reader->readU16() - 2;
        while (bytesLeft > 0) {
            Word97::FFN *ffn = new Word97::FFN(reader, Word97::FFN::Word95, false);

            if (!ffn) {
                wvlog << "Error: FFN allocation!";
                reader->pop();
                return;
            }

            m_fonts.push_back(ffn);
            bytesLeft -= ffn->cbFfnM1 + 1;
        }
    } else { // Word97 or newer
        const U16 count = reader->readU16();
        const U16 extraData = reader->readU16();
        if (extraData != 0) {
            wvlog << "Huh?? Found STTBF extra data within the STTBF of FFNs" << Qt::endl;
        }
        for (int i = 0; i < count; ++i) {
            Word97::FFN *ffn = new Word97::FFN(reader, Word97::FFN::Word97, false);

            if (!ffn) {
                wvlog << "Error: FFN allocation!";
                reader->pop();
                return;
            }

            m_fonts.push_back(ffn);
        }
    }

    if (reader->tell() - fib.fcSttbfffn != fib.lcbSttbfffn) {
        wvlog << "Warning: Didn't read lcbSttbfffn bytes: read=" << reader->tell() - fib.fcSttbfffn << " lcbSttbfffn=" << fib.lcbSttbfffn << Qt::endl;
    }
    reader->pop();
}

FontCollection::~FontCollection()
{
    std::for_each(m_fonts.begin(), m_fonts.end(), Delete<Word97::FFN>());
    delete m_fallbackFont;
}

const Word97::FFN &FontCollection::font(S16 ftc) const
{
    if (ftc >= 0 && static_cast<U16>(ftc) < m_fonts.size()) {
        return *m_fonts[ftc];
    }
    return *m_fallbackFont;
}

void FontCollection::dump() const
{
    std::vector<Word97::FFN *>::const_iterator it = m_fonts.begin();
    std::vector<Word97::FFN *>::const_iterator end = m_fonts.end();
    for (; it != end; ++it) {
        wvlog << "Font: xszFfn='" << (*it)->xszFfn.ascii() << "'" << Qt::endl;
        if (!(*it)->xszFfnAlt.isEmpty()) {
            wvlog << "      xszFfnAlt='" << (*it)->xszFfnAlt.ascii() << "'" << Qt::endl;
        }
    }
}
