/* This file is part of the KDE project
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

#include "PalmDocCompression.h"
#include "MobiFile.h"

#include <QBuffer>
#include <QDataStream>
#include <string.h>


PalmDocCompression::PalmDocCompression():
    m_winSize(2046),
    m_buffSize(10),
    m_maxBlockSize(4096)
{
}

PalmDocCompression::~PalmDocCompression()
{
}

void PalmDocCompression::compressContent(QByteArray input, QByteArray &output,
                                         QList<qint32> &recordOffset)
{

    /// PalmDOC uses LZ77 compression techniques.
    /**
        LZ77 algorithms achieve compression by replacing portions
        of the data with references to matching data that has already passed
        through both encoder and decoder. A match is encoded by a pair of numbers called
        a length-distance pair, which is equivalent to the statement "each of the next
        length characters is equal to the character exactly distance characters behind it
        in the uncompressed stream." (The "distance" is sometimes called the "offset" instead.)

        In the PalmDoc format, a length-distance pair is always encoded by a two-byte sequence.
        Of the 16 bits that make up these two bytes, 11 bits go to encoding the distance, 3 go to encoding
        the length, and the remaining two are used to make sure the decoder can identify the first byte as
        the beginning of such a two-byte sequence.
    */
    /** To create the two bytes length-distance lets first talk about
        its structure, two bytes 10 00000000000 000
        first two bits are used to make sure the decoder can identify the first byte as
        the beginning of such a two-byte sequence.
        the next 11 bits are for distance and other 3 bits are for length
        so i get my window size 4096 and buffer size 7 bits but for length
        There is some thing This line form the its wiki http://wiki.mobileread.com/wiki/PalmDOC
        "the 2 leftmost bits of this byte ('10') are discarded, and the following 6
        bits are combined with the 8 bits of the next byte to make a 14 bit "distance,
        length" item. Those 14 bits are broken into 11 bits of distance backwards from
        the current location in the uncompressed text, and 3 bits of length to copy from
        that point (copying n+3 bytes, 3 to 10 bytes)."
        I got that for copress i should do  length = length - 3.

        So i get a base 0X0001000000000000
        then i add this with distance and then shift it 3 bits to left
        and add it to length
     */

    /**
       0xc0 to 0xff: "byte pair": this byte is decoded into 2 characters: a space character,
       and a letter formed from this byte XORed with 0x80.

       0x01 to 0x08: "literals": the byte is interpreted as a count from 1 to 8,
       and that many literals are copied unmodified from the compressed
       stream to the decompressed stream.
    */

    /// PalmDOC data is always divided into 4096 byte blocks and
    /// the blocks are acted upon independently.

    QBuffer *outBuf = new QBuffer(&output);
    outBuf->open(QBuffer::ReadWrite);
    QDataStream out(outBuf);

    startCompressing(input, out, recordOffset);
    outBuf->close();
    delete outBuf;
}
void PalmDocCompression::startCompressing(QByteArray input, QDataStream &out,
                                          QList<qint32> &recordOffset)
{
    int winIndex = -1;
    int lookahead = 0;

    while (input.length() != lookahead) {
        int start = winIndex - m_winSize + 1;
        if (start < 0) {
            start = 0;
        }

        int length = 0;
        int pos = 0;
        qint16 base = m_maxBlockSize;


        // check for out put size for reach to max block size
        if (lookahead == m_maxBlockSize) {
            input = input.right(input.size() - lookahead);
            winIndex = -1;
            lookahead = 0;
            start = 0;
            recordOffset << (qint32)out.device()->pos();
        }

        // check space char
        if (input.at(lookahead) == ' ') {
            // check for next letter
            int index = lookahead;
            index++;
            if ((index != m_maxBlockSize)  || index < input.size()) {
                // input.at(index) is between [-127..127] so i am sure that
                // it (^) is less that 0X7f
                // litterals ascii is between 0X09 - 0X7f
                if (QChar(input.at(index)).isLetter() && (input.at(index) >= 0X09)){

                    winIndex += 2;
                    lookahead += 2;
                    out << (qint8)(input.at(index) ^ (qint8)0X80);
                    continue;
                }
            }
        }

        // litterals ascii is between 0X09 - 0X7f
        if (input.at(lookahead) < 0X09) {

            // Check the length of unknown characters.
            int len = 1;
            int index = lookahead + 1;
            while (1) {
                if (input.at(index) < 0X09) {
                    if ((index == m_maxBlockSize) || (index >= input.size())) {
                        break;
                    }
                    index++;
                    len++;
                    /**
                        0x01 to 0x08: "literals": the byte is interpreted as a count from 1 to 8,
                        and that many literals are copied unmodified from the compressed stream to
                        the decompressed stream.*/
                    if (len == 8) {
                        break;
                    }
                }
                else {
                    break;
                }
            }

               out << (qint8)len;
               for (int i = 0; i < len; i++) {
                   out << (qint8)input.at(lookahead);
                   lookahead++;
               }
               winIndex += len;
               continue;
        }

        for (int i = start; i <= winIndex; i++)
        {
            if ((lookahead == m_maxBlockSize) || (lookahead >= input.size())) {
                break;
            }
            if (input.at(i) == input.at(lookahead)) {
                // save the match char position.
                pos = i;
                length++;
                lookahead++;

                // Go to find more matches.
                for (int j = i + 1, k = 1; (j <=  winIndex) && (k < m_buffSize); ++j, ++k) {
                    if ((lookahead == m_maxBlockSize) || (lookahead >= input.size())) {
                        break;
                    }
                    if (input.at(j) != input.at(lookahead)) {
                        break;
                    }
                    // Match character should be char
                    if (input.at(lookahead) < 0X09) {
                        break;
                    }
                    length++;
                    lookahead++;
                }
                break;
            }
        }
        if (length == 0) {
            out << (qint8) input.at(lookahead);
            winIndex++;
            lookahead++;
        }
        else {
            // I do not encode the match over 3 bits
            if (length == 1 ) {
                out << (qint8) input.at((winIndex + 1));
            }
            else if (length == 2) {
                out << (qint8) input.at((winIndex + 1));
                out << (qint8) input.at((winIndex + 2));
            }
            else {
                qint16 distance = (qint16)(winIndex - pos + 1);
                base = distance + base;
                base = base << 3;
                qint16 offset_length = base + (qint16)((length - 3));
                out << offset_length;
            }
            winIndex += length;
        }
    }
}
