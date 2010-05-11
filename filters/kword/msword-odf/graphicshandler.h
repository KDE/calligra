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
#include "wv2/src/ms_odraw.h"
#include "wv2/src/graphics.h"

#include <QObject>
#include <QHash>
#include "document.h"
#include "versionmagic.h"
#include "generated/simpleParser.h"
#include "writer.h"
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoStore.h>

#include <vector>

class DrawStyle;

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
    virtual void escherData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf, int type, wvWare::U32 pib);
    virtual void escherData(std::vector<wvWare::U8> data, wvWare::SharedPtr<const wvWare::Word97::PICF> picf, int type, wvWare::U32 pib);
    virtual void wmfData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);
    virtual void externalImage(const wvWare::UString& name, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);

    virtual void officeArt(wvWare::OfficeArtProperties *artProperties);
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
    QHash<wvWare::U32, QString> m_pictureName;

    void ODTProcessing(QString* picName, wvWare::SharedPtr<const wvWare::Word97::PICF> picf, int type, wvWare::U32 pib);
    int m_pictureCount;
    int m_officeArtCount;

    void officeArtLine(wvWare::OfficeArtProperties *artProperties);
};

 class DrawingWriter : public Writer
 {
 public:
     //position
     int xLeft;
     int xRight;
     int yTop;
     int yBottom;
     //structure that specifies placement of drawing
     wvWare::Word97::FSPA* m_pSpa;
     //true - drawing is in body; false - drawing is in header/footer
     bool m_bodyDrawing;

     DrawingWriter(KoXmlWriter& xmlWriter, KoGenStyles& kostyles, bool stylesxml_);

     QString vLength();
     QString hLength();
     QString vOffset();
     QString hOffset();

     void SetRectangle(wvWare::Word97::FSPA& spa);
     void SetGroupRectangle(MSO::OfficeArtFSPGR& fspgr);
     void SetClientRectangle(MSO::OfficeArtChildAnchor& anchor);
 };

class KWordDrawingHandler : public QObject, public wvWare::DrawingHandler
{
    Q_OBJECT
public:
    KWordDrawingHandler(Document* doc, KoGenStyles* mainStyles, KoXmlWriter* bodyWriter);
    ~KWordDrawingHandler();

    void init(wvWare::Drawings * pDrawings, wvWare::OLEStreamReader* table,const  wvWare::Word97::FIB &fib);
    virtual void drawingData(unsigned int globalCP);
    void setBodyWriter(KoXmlWriter* writer);

    // Gets drawing style for whole document
    DrawStyle getDrawingStyle();

     // Communication with Document, without having to know about Document
signals:
void textBoxFound( uint lid, KoXmlWriter* writer);

private:

    void drawObject(uint spid, MSO::OfficeArtDgContainer * dg, DrawingWriter& out
            , wvWare::Word97::FSPA* spa);
    void processObjectForBody(const MSO::OfficeArtSpgrContainer& o, DrawingWriter& out);
    void processObjectForBody(const MSO::OfficeArtSpContainer& o, DrawingWriter out);

    void parseOfficeArtContainer(wvWare::OLEStreamReader* table, const wvWare::Word97::FIB &fib);
    void defineGraphicProperties(KoGenStyle& style, const DrawStyle& ds, wvWare::Word97::FSPA* spa,
            const QString& listStyle=QString());
    void defineAnchorProperties(KoGenStyle& style, const DrawStyle& ds);
//    void defineDefaultGraphicProperties(KoGenStyle* pStyle, wvWare::Drawings * pDrawings);

    void parseTextBox(const MSO::OfficeArtSpContainer& o, DrawingWriter out);
    void processRectangle(const MSO::OfficeArtSpContainer& o,DrawingWriter& out);
    void processPictureFrame(const MSO::OfficeArtSpContainer& o,DrawingWriter& out);

    Document* m_doc;
    KoGenStyles* m_mainStyles;
    KoXmlWriter* m_bodyWriter;

    wvWare::Drawings * m_drawings;
    wvWare::Word97::FIB * m_fib;

    MSO::OfficeArtDggContainer m_OfficeArtDggContainer;
    MSO::OfficeArtDgContainer * m_pOfficeArtHeaderDgContainer;
    MSO::OfficeArtDgContainer * m_pOfficeArtBodyDgContainer;
};

#endif // GRAPHICSHANDLER_H
