/* This file is part of the KOffice project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2003 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "graphicshandler.h"

#include <wv2/olestream.h>

#include <kdebug.h>

using namespace wvWare;

KWordGraphicsHandler::KWordGraphicsHandler() : QObject()
{
}

void KWordGraphicsHandler::bitmapData( const OLEImageReader& reader, SharedPtr<const Word97::PICF> /*picf*/ )
{
    kdDebug() << "Bitmap data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size() << endl;
}

void KWordGraphicsHandler::wmfData( const OLEImageReader& reader, SharedPtr<const Word97::PICF> /*picf*/,
                                    const GraphicsHandler::WMFDimensions& dimensions )
{
    kdDebug() << "wmf data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size() << endl;
    // No idea if that's dimension thingy is needed at all. The docu doesn't really help
    Q_ASSERT( dimensions.left == 0 && dimensions.top == 0 && dimensions.width == 0 && dimensions.height == 0 );
}

void KWordGraphicsHandler::tiffData( const UString& /*name*/, SharedPtr<const Word97::PICF> /*picf*/ )
{
}

#include "graphicshandler.moc"
