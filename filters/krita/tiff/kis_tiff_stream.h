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

#ifndef TIFFSTREAM_H_
#define TIFFSTREAM_H_

#include <tiffio.h>

class TIFFStream {
    public:
        TIFFStream( uint16 depth) : m_depth(depth) {};
        virtual uint32 nextValueBelow16() =0;
        virtual uint32 nextValueBelow32() =0;
        virtual uint32 nextValueAbove32() =0;
        virtual void restart() =0;
    protected:
        uint16 m_depth;
};

class TIFFStreamContig : public TIFFStream {
    public:
        TIFFStreamContig( uint8* src, uint16 depth );
        virtual uint32 nextValueBelow16();
        virtual uint32 nextValueBelow32();
        virtual uint32 nextValueAbove32();
        virtual void restart();
    private:
        uint8* m_src;
        uint8* m_srcit;
        uint8 m_posinc;
};

class TIFFStreamSeperate : public TIFFStream {
    public:
        TIFFStreamSeperate( uint8** srcs, uint8 nb_samples ,uint16 depth);
        ~TIFFStreamSeperate();
        virtual uint32 nextValueBelow16();
        virtual uint32 nextValueBelow32();
        virtual uint32 nextValueAbove32();
        virtual void restart();
    private:
        TIFFStreamContig** streams;
        uint8 m_current_sample, m_nb_samples;
};

#endif
