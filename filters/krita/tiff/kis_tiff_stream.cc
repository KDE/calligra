/*
 *  Copyright (c) 2005-2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
 
#include "kis_tiff_stream.h"

TIFFStreamContigBase::TIFFStreamContigBase( uint8* src, uint16 depth, uint32 lineSize ) : TIFFStreamBase(depth), m_src(src), m_lineSize(lineSize) { restart(); }

void TIFFStreamContigBase::restart()
{
    m_srcit = m_src;
    m_posinc = 8;
}

void TIFFStreamContigBase::moveToLine(uint32 lineNumber)
{
    m_srcit = m_src + lineNumber * m_lineSize;
    m_posinc = 8;
}

uint32 TIFFStreamContigBelow16::nextValue()
{
    register uint8 remain;
    register uint32 value;
    remain = m_depth;
    value = 0;
    while (remain > 0)
    {
        register uint8 toread;
        toread = remain;
        if (toread > m_posinc) toread = m_posinc;
        remain -= toread;
        m_posinc -= toread;
        value = (value << toread) | (( (*m_srcit) >> (m_posinc) ) & ( ( 1 << toread ) - 1 ) );
        if (m_posinc == 0)
        {
            m_srcit++;
            m_posinc=8;
        }
    }
    return value;
}

uint32 TIFFStreamContigBelow32::nextValue()
{
    register uint8 remain;
    register uint32 value;
    remain = m_depth;
    value = 0;
    while (remain > 0)
    {
        register uint8 toread;
        toread = remain;
        if (toread > m_posinc) toread = m_posinc;
        remain -= toread;
        m_posinc -= toread;
        value = (value) | ( (( (*m_srcit) >> (m_posinc) ) & ( ( 1 << toread ) - 1 ) ) << ( m_depth - 8 - remain ) );
        if (m_posinc == 0)
        {
            m_srcit++;
            m_posinc=8;
        }
    }
    return value;
}

uint32 TIFFStreamContigAbove32::nextValue()
{
    register uint8 remain;
    register uint32 value;
    remain = m_depth;
    value = 0;
    while (remain > 0)
    {
        register uint8 toread;
        toread = remain;
        if (toread > m_posinc) toread = m_posinc;
        remain -= toread;
        m_posinc -= toread;
        if(remain < 32 )
        {
            value = (value) | ( (( (*m_srcit) >> (m_posinc) ) & ( ( 1 << toread ) - 1 ) ) << ( 24 - remain ) );
        }
        if (m_posinc == 0)
        {
            m_srcit++;
            m_posinc=8;
        }
    }
    return value;
}

TIFFStreamSeperate::TIFFStreamSeperate( uint8** srcs, uint8 nb_samples ,uint16 depth, uint32* lineSize) : TIFFStreamBase(depth), m_nb_samples(nb_samples)
{
    streams = new TIFFStreamContigBase*[nb_samples];
    if(depth < 16)
    {
        for(uint8 i = 0; i < m_nb_samples; i++)
        {
            streams[i] = new TIFFStreamContigBelow16(srcs[i], depth, lineSize[i]);
        }
    } else if( depth < 32 )
    {
        for(uint8 i = 0; i < m_nb_samples; i++)
        {
            streams[i] = new TIFFStreamContigBelow32(srcs[i], depth, lineSize[i]);
        }
    } else {
        for(uint8 i = 0; i < m_nb_samples; i++)
        {
            streams[i] = new TIFFStreamContigAbove32(srcs[i], depth, lineSize[i]);
        }
    }
    restart();
}

TIFFStreamSeperate::~TIFFStreamSeperate()
{
    for(uint8 i = 0; i < m_nb_samples; i++)
    {
        delete streams[i];
    }
    delete[] streams;
}

uint32 TIFFStreamSeperate::nextValue()
{
    uint32 value = streams[ m_current_sample ]->nextValue();
    if( (++m_current_sample) >= m_nb_samples)
        m_current_sample = 0;
    return value;
}

void TIFFStreamSeperate::restart()
{
    m_current_sample = 0;
    for(uint8 i = 0; i < m_nb_samples; i++)
    {
        streams[i]->restart();
    }
}

void TIFFStreamSeperate::moveToLine(uint32 lineNumber)
{
    for(uint8 i = 0; i < m_nb_samples; i++)
    {
        streams[i]->moveToLine(lineNumber);
    }
}
