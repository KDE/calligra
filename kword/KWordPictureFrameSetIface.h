/* This file is part of the KDE project
   Copyright (C) 2002, Laurent MONTEL <lmontel@mandrakesoft.com>

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

#ifndef KWORD_PICTUREFRAMESET_IFACE_H
#define KWORD_PICTUREFRAMESET_IFACE_H

#include <KoDocumentIface.h>
#include "KWordFrameSetIface.h"
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>
class KWPictureFrameSet;
class KWordViewIface;

class KWordPictureFrameSetIface : public KWordFrameSetIface
{
    K_DCOP
public:
    KWordPictureFrameSetIface( KWPictureFrameSet *_frame );

k_dcop:
    bool keepAspectRatio() const;
    void setKeepAspectRatio( bool b );
    void loadImage( const QString &image );

private:
    KWPictureFrameSet *m_frame;
};

#endif
