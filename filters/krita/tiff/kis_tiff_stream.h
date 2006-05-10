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

class TIFFStreamBase {
    public:
       TIFFStreamBase( uint16 depth ) : m_depth(depth) {};
        virtual uint32 nextValue() =0;
        virtual void restart() =0;
        virtual void moveToLine(uint32 lineNumber) =0;
		virtual ~TIFFStreamBase(){}
    protected:
        uint16 m_depth;
};

class TIFFStreamContigBase : public TIFFStreamBase {
    public:
        TIFFStreamContigBase( uint8* src, uint16 depth, uint32 lineSize );
        virtual void restart();
        virtual void moveToLine(uint32 lineNumber);
		virtual ~TIFFStreamContigBase() {}
    protected:
        uint8* m_src;
        uint8* m_srcit;
        uint8 m_posinc;
        uint32 m_lineSize;
};

class TIFFStreamContigBelow16 : public TIFFStreamContigBase {
    public:
        TIFFStreamContigBelow16( uint8* src, uint16 depth, uint32 lineSize ) : TIFFStreamContigBase(src, depth, lineSize) { }
    public:
		virtual ~TIFFStreamContigBelow16() {}
        virtual uint32 nextValue();
};

class TIFFStreamContigBelow32 : public TIFFStreamContigBase {
    public:
        TIFFStreamContigBelow32( uint8* src, uint16 depth, uint32 lineSize ) : TIFFStreamContigBase(src, depth, lineSize) { }
    public:
		virtual ~TIFFStreamContigBelow32() {}
        virtual uint32 nextValue();
};

class TIFFStreamContigAbove32 : public TIFFStreamContigBase {
    public:
        TIFFStreamContigAbove32( uint8* src, uint16 depth, uint32 lineSize ) : TIFFStreamContigBase(src, depth, lineSize) { }
    public:
		virtual ~TIFFStreamContigAbove32(){}
        virtual uint32 nextValue();
};


class TIFFStreamSeperate : public TIFFStreamBase {
    public:
        TIFFStreamSeperate( uint8** srcs, uint8 nb_samples ,uint16 depth, uint32* lineSize);
        virtual ~TIFFStreamSeperate();
        virtual uint32 nextValue();
        virtual void restart();
        virtual void moveToLine(uint32 lineNumber);
    private:
        TIFFStreamContigBase** streams;
        uint8 m_current_sample, m_nb_samples;
};

#endif
