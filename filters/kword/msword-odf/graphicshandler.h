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
#include "versionmagic.h"
#include "document.h"

#include "generated/simpleParser.h"
#include "writer.h"

#include <QObject>
#include <QHash>
#include <QMap>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoStore.h>
#include <vector>

class DrawStyle;

/*
 * ************************************************
 * Drawing Writer
 * ************************************************
 */
class DrawingWriter : public Writer
{
public:
    DrawingWriter(KoXmlWriter& xmlWriter, KoGenStyles& kostyles, bool stylesxml_, bool inlineObj);

    //position
    int xLeft;
    int xRight;
    int yTop;
    int yBottom;

    //structure that specifies placement of a floating object
    wvWare::Word97::FSPA* m_pSpa;

    //specifies the type, size and border information for an inline picture
    wvWare::SharedPtr<const wvWare::Word97::PICF> m_picf;

    //unique identifier of a BLIP
    QByteArray m_rgbUid;

    //true - drawing is in body; false - drawing is in header/footer
    bool m_bodyDrawing;
    //true - inline object; false - floating object
    bool m_inline;

    qreal vLength();
    qreal hLength();
    qreal vOffset();
    qreal hOffset();

    void SetRectangle(wvWare::Word97::FSPA& spa);
    void SetGroupRectangle(MSO::OfficeArtFSPGR& fspgr);
    void SetClientRectangle(MSO::OfficeArtChildAnchor& anchor);
};

/*
 * ************************************************
 * Graphics Handler
 * ************************************************
 */
class KWordGraphicsHandler : public QObject, public wvWare::GraphicsHandler
{
    Q_OBJECT
public:
    KWordGraphicsHandler(Document* doc, KoXmlWriter* bodyWriter, KoXmlWriter* manifestWriter,
                         KoStore* store, KoGenStyles* mainStyles);
    ~KWordGraphicsHandler();

    /**
     * This method gets called when a floating object is found by wv2 parser.
     * @param globalCP actual character position in the WordDocument stream.
     */
    virtual void handleFloatingObject(unsigned int globalCP);

    /**
     * This method gets called when an inline object is found by wv2 parser.
     * @param data PictureData as defined in functordata.h
     */
    virtual void handleInlineObject(const wvWare::PictureData& data);

    /**
     * Initialize the drawing handler.  Optional containers from Table stream
     * and containers storing inline picture data from WordDocument stream are
     * parsed.
     *
     * @param pointer to the wv2 Drawing structure providing pointers to PLCs
     * storing textbox data and shape properties
     *
     * @param pointer to the FIB structure provided by wv2
     */
    void init(wvWare::Drawings* pDrawings, const wvWare::Word97::FIB &fib);

    /**
     * Set the appropriate writer for object properties and content.
     * @param writer KoXmlWriter provided by the Document class
     */
    void setBodyWriter(KoXmlWriter* writer);

    /**
     * Gets drawing style for whole document.
     */
    DrawStyle getDrawingStyle();

    // Communication with Document, without having to know about Document
signals:
    void textBoxFound( uint lid, bool bodyDrawing);

private:
    /**
     * Parse the OfficeArtDggContainer data from the Table stream.
     */
    void parseOfficeArtContainer(POLE::Storage* storage, const wvWare::Word97::FIB &fib);

    /**
     * Parse floating pictures data from the WordDocument stream.
     */
    void parseFloatingPictures(void);

    /**
     * Store floating pictures into ODT, write the appropriate manifest entry.
     */
    QMap<QByteArray, QString> createFloatingPictures(KoStore* store, KoXmlWriter* manifest);

    /**
     * Get the path in the ODT document that corresponds to the picture
     * generated from the picture with the given pib. (check
     * libmso/ODrawToOdf.h)
     */
    QString getPicturePath(int pib) const;

    /**
     * Process the default properties for all drawing objects stored in
     * OfficeArtDggContainer.
     */
    void defineDefaultGraphicStyle(KoGenStyles* mainStyles);

    /**
     * Process general properties of a shape.
     */
    void defineGraphicProperties(KoGenStyle& style, const DrawStyle& ds, const QString& listStyle=QString());

    /**
     * Process anchor related properties of a shape.
     */
    void defineAnchorProperties(KoGenStyle& style, const DrawStyle& ds);

    /**
     * Process text wrapping related properties of a shape.
     */
    void defineWrappingProperties(KoGenStyle& style, const DrawStyle& ds, const wvWare::Word97::FSPA* spa);

    /**
     * Check if the object is inline or floating and set the anchor type to
     * char or as-char.
     */
    void SetAnchorTypeAttribute(DrawingWriter& out);

    /**
     * Set the Z-Index attribute. Z-Index is the position of the shape on z
     * axis. Z-Index depends on the order in which shapes are stored inside
     * OfficeArtDgContainer
     */
    void SetZIndexAttribute(DrawingWriter& out);

    /**
     * TODO:
     */
    void drawObject(uint spid, MSO::OfficeArtDgContainer* dg, DrawingWriter& out,
                    wvWare::Word97::FSPA* spa);
    /**
     * Check object types in a container of groups of shapes.
     * @param o container of groups of shapes
     * @param out drawing writer
     */
    void processObjectContent(const MSO::OfficeArtSpgrContainer& o, DrawingWriter& out);

    /**
     * Check object type and call the appropriate method to process it.
     * @param o container for shapes
     * @param out drawing writer
     */
    void processObjectContent(const MSO::OfficeArtSpContainer& o, DrawingWriter out);

    /**
     * Process the properties of a text box, use wv2 to parse the content.
     */
    void parseTextBox(const MSO::OfficeArtSpContainer& o, DrawingWriter out);

    /**
     * Process a rectangle shape.
     */
    void processRectangle(const MSO::OfficeArtSpContainer& o, DrawingWriter& out);

    /**
     * Process a line shape.
     */
    void processLineShape(const MSO::OfficeArtSpContainer& o, DrawingWriter& out);

    /**
     * Process a floating frame shape.
     */
    void processFloatingPictureFrame(const MSO::OfficeArtSpContainer& o, DrawingWriter& out);

    /**
     * Process an inline frame shape.
     */
    void processInlinePictureFrame(const MSO::OfficeArtSpContainer& o, DrawingWriter& out);

    Document* m_document;
    KoStore* m_store;
    KoXmlWriter* m_bodyWriter;
    KoXmlWriter* m_manifestWriter;
    KoGenStyles* m_mainStyles;

    wvWare::Drawings * m_drawings;
    wvWare::Word97::FIB * m_fib;

    MSO::OfficeArtDggContainer m_OfficeArtDggContainer;
    MSO::OfficeArtDgContainer * m_pOfficeArtHeaderDgContainer;
    MSO::OfficeArtDgContainer * m_pOfficeArtBodyDgContainer;

    QMap<QByteArray, QString> m_picNames; //picture names
    int m_zIndex; //position of current shape on z axis
};

#endif // GRAPHICSHANDLER_H
