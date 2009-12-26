/* This file is part of the KOffice project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2003 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include <wv2/src/functor.h>
#include <wv2/src/handlers.h>

#include <QObject>
#include "document.h"
#include "versionmagic.h"
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoStore.h>

#include <vector>

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
    KWordPictureHandler(Document* doc, KoXmlWriter* bodyWriter, KoXmlWriter* manifestWriter,
                        KoStore* store, KoGenStyles* mainStyles);

#ifdef IMAGE_IMPORT
    //////// PictureHandler interface
    virtual void bitmapData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);
    virtual void escherData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
                            int type);
    virtual void escherData(std::vector<wvWare::U8> data, wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
                            int type);
    virtual void wmfData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);
    virtual void externalImage(const wvWare::UString& name, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);
#endif // IMAGE_IMPORT

    //lets us write to another writer instead of directly to the main body writer
    // (as in the case of a picture inside a Paragraph)
    void setBodyWriter(KoXmlWriter* writer);

private:
    Document* m_doc;
    KoXmlWriter* m_bodyWriter;
    KoXmlWriter* m_manifestWriter;
    KoStore* m_store;
    KoGenStyles* m_mainStyles;

    void ODTProcessing(QString* picName, wvWare::SharedPtr<const wvWare::Word97::PICF> picf, int type);
    int m_pictureCount;

};

// KWordDrawingHandler yet to come (Werner)

#endif // GRAPHICSHANDLER_H
