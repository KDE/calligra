// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#ifndef KPPIXMAP_OBJECT_IFACE_H
#define KPPIXMAP_OBJECT_IFACE_H

#include "KPrObject2DAdaptor.h"
#include <QString>

class KPrPixmapObject;

class KPrPixmapObjectAdaptor : public KPrObject2DAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.presentation.object.pixmap")

public:
    KPrPixmapObjectAdaptor( KPrPixmapObject *obj_ );

public Q_SLOTS:
    QString fileName() const;
    int depth() const;
    bool swapRGB() const;
    bool grayscale() const;
    int pictureMirrorType() const;
    int brightness() const;

    void setDepth(int depth );
    void setBrightness(int bright );
    void setSwapRGB(bool swapRGB );
    void setGrayscale( bool grayscal );
    void loadImage( const QString & fileName );
    void setPictureMirrorType(const QString & _mirror);

private:
    KPrPixmapObject *obj;
};

#endif
