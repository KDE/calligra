/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#ifndef KIS_TIFF_WRITER_VISITOR_H
#define KIS_TIFF_WRITER_VISITOR_H

#include <kis_layer_visitor.h>

#include <kis_iterators_pixel.h>

#include <tiffio.h>

class KisTIFFOptions;

/**
	@author Cyrille Berger <cberger@cberger.net>
*/
class KisTIFFWriterVisitor : public KisLayerVisitor
{
    public:
        KisTIFFWriterVisitor(TIFF*img, KisTIFFOptions* options);
        ~KisTIFFWriterVisitor();
    public:
        virtual bool visit(KisPaintLayer *layer);
        virtual bool visit(KisGroupLayer *layer);
        virtual bool visit(KisPartLayer *layer);
        virtual bool visit(KisAdjustmentLayer* ) { return true; }
    private:
        inline TIFF* image() { return m_image; }
        inline bool saveAlpha();
        bool copyDataToStrips( KisHLineIterator it, tdata_t buff, uint8 depth, uint8 nbcolorssamples, Q_UINT8* poses);
    private:
        TIFF* m_image;
        KisTIFFOptions* m_options;
};

#endif
