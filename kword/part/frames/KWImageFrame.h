/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KWIMAGEFRAME_H
#define KWIMAGEFRAME_H

#include "KWFrame.h"

#include <KoImageData.h>

/**
 * This frame type will hold images.
 * A unique feature of this image frame is that you can set a rendering-quality for the image
 * that will allow high-speed rendering without loss of quality for actual processing like
 * printing.
 */
class KWORD_EXPORT KWImageFrame : public KWFrame {
public:
     /// the rendering-quality of the image
    enum ImageQuality {
        /* Remember to keep the ordering of this enum the same as the ordering in the KoImageData class! */
        LowQuality,     ///< see KoImageData::LowQuality
        MediumQuality,  ///< see KoImageData::MediumQuality
        HighQuality,    ///< see KoImageData::HighQuality
        EditableQuality 
    };

    KWImageFrame(KWFrameSet *parent);
    ~KWImageFrame();

    void setImageQuality(ImageQuality quality);
    ImageQuality imageQuality() const;

private:
    bool m_fullKritaShape;
    KWImageFrame::ImageQuality  m_quality;
};

#endif

