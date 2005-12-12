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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "graphicshandler.h"
#include "document.h"

#include <wv2/olestream.h>

#include <KoStoreDevice.h>
#include <KoSize.h>
#include <kdebug.h>

using namespace wvWare;

KWordPictureHandler::KWordPictureHandler( Document* doc ) : QObject(), m_doc(doc)
{
}

#ifdef IMAGE_IMPORT

void KWordPictureHandler::bitmapData( OLEImageReader& reader, SharedPtr<const Word97::PICF> /*picf*/ )
{
    kdDebug(30513) << "Bitmap data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size() << endl;

}

void KWordPictureHandler::wmfData( OLEImageReader& reader, SharedPtr<const Word97::PICF> picf )
{
    kdDebug(30513) << "wmf data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size() << endl;

    // We have two things to do here
    // 1 - Create the frameset and its frame
    // 2 - Store the picture in the store
    // We combine those two things into one call to the document
    KoSize size( (double)picf->dxaGoal / 20.0, (double)picf->dyaGoal / 20.0 );
    kdDebug(30513) << "size=" << size << endl;
    KoStoreDevice* dev = m_doc->createPictureFrameSet( size );
    Q_ASSERT(dev);
    if ( !dev )
        return; // ouch

#define IMG_BUF_SIZE 2048
    wvWare::U8 buf[IMG_BUF_SIZE];
    Q_LONG len = reader.size();
    while ( len > 0 )  {
        size_t n = reader.read( buf, QMIN( len, IMG_BUF_SIZE ) );
        Q_LONG n1 = dev->writeBlock( (const char*)buf, n );
        Q_ASSERT( (size_t)n1 == n );
        if ( (size_t)n1 != n )
            return; // ouch
        len -= n;
    }
    Q_ASSERT( len == 0 );
    dev->close();
}

void KWordPictureHandler::tiffData( const UString& /*name*/, SharedPtr<const Word97::PICF> /*picf*/ )
{
}

#endif // IMAGE_IMPORT

#include "graphicshandler.moc"
