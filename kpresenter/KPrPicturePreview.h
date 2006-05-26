// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   The code is based on work of
   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>

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

#ifndef PICTUREPREVIEW_H
#define PICTUREPREVIEW_H

#include <q3frame.h>
//Added by qt3to4:
#include <QPixmap>

#include "global.h"

class KPrPicturePreview : public Q3Frame
{
    Q_OBJECT

public:
    KPrPicturePreview( QWidget* parent);
    ~KPrPicturePreview() {}

    void setDepth( int depth);
    void setMirrorType (PictureMirrorType _t);
    void setPicturePixmap(const QPixmap &_pixmap);

    int getDepth() const { return depth; }

public slots:
    void slotNormalPicture();
    void slotHorizontalMirrorPicture();
    void slotVerticalMirrorPicture();
    void slotHorizontalAndVerticalMirrorPicture();

    void slotPictureDepth0();
    void slotPictureDepth1();
    void slotPictureDepth8();
    void slotPictureDepth16();
    void slotPictureDepth32();

    void slotSwapRGBPicture( bool _on );

    void slotGrayscalPicture( bool _on );

    void slotBrightValue( int _value );

protected:
    virtual void drawContents( QPainter *painter );

    PictureMirrorType mirrorType;
    int depth;
    bool swapRGB;
    int bright;
    bool grayscal;
    QPixmap origPixmap;
};

#endif /* PICTUREPREVIEW_H */
