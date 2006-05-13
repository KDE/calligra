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

#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include <wv2/functor.h>
#include <wv2/handlers.h>

#include <QObject>
#include "document.h"
#include "versionmagic.h"

#ifndef IMAGE_IMPORT
namespace wvWare
{
    class PictureHandler
    {
    };
}
#endif // IMAGE_IMPORT

class KWordPictureHandler : public QObject, public wvWare::PictureHandler
{
    Q_OBJECT
public:
    KWordPictureHandler( Document* doc );

#ifdef IMAGE_IMPORT
    //////// PictureHandler interface
    virtual void bitmapData( wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf );
    virtual void wmfData( wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf );
    virtual void tiffData( const wvWare::UString& name, wvWare::SharedPtr<const wvWare::Word97::PICF> picf );
#endif // IMAGE_IMPORT

private:
    Document* m_doc;

};

// KWordDrawingHandler yet to come (Werner)

#endif // GRAPHICSHANDLER_H
