/* This file is part of the Calligra project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2003 David Faure <faure@kde.org>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>
   Copyright (C) 2010, 2011 Matus Hanzes <matus.hanzes@ixonos.com>

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

#include <wv2/src/handlers.h>
#include "wv2/src/graphics.h"
//#include "versionmagic.h"
#include "document.h"

#include "generated/simpleParser.h"
#include "writer.h"
#include "ODrawToOdf.h"

#include <QObject>
#include <QMap>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoStore.h>

class DrawStyle;

enum ObjectType { Inline, Floating };

/*
 * ************************************************
 * Drawing Writer
 * ************************************************
 */
//NOTE: Deprecated!

class DrawingWriter : public Writer
{
public:
    DrawingWriter(KoXmlWriter& xmlWriter, KoGenStyles& kostyles, bool stylesxml_);

    //position
    int xLeft;
    int xRight;
    int yTop;
    int yBottom;

    qreal vLength();
    qreal hLength();
    qreal vOffset();
    qreal hOffset();

    void setRect(const QRect& rect);
    void setRectangle(wvWare::Word97::FSPA& spa);
    void setGroupRectangle(MSO::OfficeArtFSPGR& fspgr);
    void setChildRectangle(MSO::OfficeArtChildAnchor& anchor);
};

/*
 * ************************************************
 * Graphics Handler
 * ************************************************
 */
class WordsGraphicsHandler : public QObject, public wvWare::GraphicsHandler
{
    Q_OBJECT
private:
    /*
     * ************************************************
     * DrawClient
     * ************************************************
     */
    class DrawClient : public ODrawToOdf::Client
    {
    private:
        QRectF getRect(const MSO::OfficeArtClientAnchor&) override;
        QRectF getReserveRect(void) override;
        QString getPicturePath(const quint32 pib) override;
        bool onlyClientData(const MSO::OfficeArtClientData& o) override;
        void processClientData(const MSO::OfficeArtClientTextBox* ct,
                                       const MSO::OfficeArtClientData& o,
                                       Writer& out) override;
        void processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                                          const MSO::OfficeArtClientData* cd,
                                          Writer& out) override;
        bool processRectangleAsTextBox(const MSO::OfficeArtClientData& cd) override;
        KoGenStyle createGraphicStyle(const MSO::OfficeArtClientTextBox* ct,
                                              const MSO::OfficeArtClientData* cd,
                                              const DrawStyle& ds,
                                              Writer& out) override;
        void addTextStyles(const MSO::OfficeArtClientTextBox* clientTextbox,
                                   const MSO::OfficeArtClientData* clientData,
                                   KoGenStyle& style, Writer& out) override;

        QColor toQColor(const MSO::OfficeArtCOLORREF& c) override;
        QString formatPos(qreal v) override;

        const MSO::OfficeArtDggContainer* getOfficeArtDggContainer() override;
        const MSO::OfficeArtSpContainer* getMasterShapeContainer(quint32 spid) override;

        WordsGraphicsHandler* const gh;
    public:
        explicit DrawClient(WordsGraphicsHandler *p) :gh(p) {}
};
public:
    WordsGraphicsHandler(Document* document,
                         KoXmlWriter* bodyWriter,
                         KoXmlWriter* manifestWriter,
                         KoStore* store, KoGenStyles* mainStyles,
                         const wvWare::Drawings* p_drawings,
                         const wvWare::Word97::FIB& fib);
    ~WordsGraphicsHandler() override;

    /**
     * Set the appropriate writer for object properties and content.
     * @param writer KoXmlWriter provided by the Document class
     */
    void setCurrentWriter(KoXmlWriter* writer) { m_currentWriter = writer; };

    /**
     * This method gets called when a floating object is found by wv2 parser.
     * @param globalCP actual character position in the WordDocument stream.
     */
    void handleFloatingObject(unsigned int globalCP) override;

    /**
     * This method gets called when an inline object is found by wv2 parser.
     * @param data PictureData as defined in functordata.h
     */
    QString handleInlineObject(const wvWare::PictureData& data, const bool isBulletPicture = false) override;

    /**
     * Get the DrawStyle to access document background properties and defaults.
     *
     * DrawStyle ds(m_OfficeArtDggContainer, 0, m_pOfficeArtBodyDgContainer.shape)
     * @return ds
     */
    DrawStyle getBgDrawStyle(void);

    /**
     * Get the path in the ODT document that corresponds to the picture
     * generated from the picture with the given pib. (check
     * libmso/ODrawToOdf.h)
     */
    QString getPicturePath(quint32 pib) const;

    // Communication with Document, without having to know about Document
Q_SIGNALS:
    void textBoxFound(unsigned int index, bool stylesxml);

private:
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
    void init(void);

    /**
     * Parse the OfficeArtDggContainer data and OfficeArtDgContainer data for
     * both the body and the header document from the Table stream.
     */
    void parseOfficeArtContainers(void);

    /**
     * Parse floating pictures data from the WordDocument stream.
     *
     * @param specifies the container for all the BLIPs that are used in all
     * the drawings in the parent document.
     *
     * @return 0 - success, 1 - failed
     */
    int parseFloatingPictures(const MSO::OfficeArtBStoreContainer* blipStore);

    /**
     * Process the default properties for all drawing objects stored in
     * OfficeArtDggContainer.
     */
    void defineDefaultGraphicStyle(KoGenStyles* mainStyles);

    /**
     * Process text wrapping related properties of a shape.  Add corresponding
     * attributes to graphic-properties of the graphic style.
     */
    void defineWrappingAttributes(KoGenStyle& style, const DrawStyle& ds);

    /**
     * Process position related properties of a shape.  Add corresponding
     * attributes to graphic-properties of the graphic style.
     */
    void definePositionAttributes(KoGenStyle& style, const DrawStyle& ds);

    /**
     * Check if the object is inline or floating and set the anchor type to
     * char or as-char.
     */
    void setAnchorTypeAttribute(DrawingWriter& out);

    /**
     * Set the Z-Index attribute.  Z-Index is the position of the shape on z
     * axis.  Z-Index depends on the order in which shapes are stored inside
     * OfficeArtDgContainer
     */
    void setZIndexAttribute(DrawingWriter& out);

    /**
     * TODO:
     */
    void locateDrawing(const MSO::OfficeArtSpgrContainer* o,
                       wvWare::Word97::FSPA* spa, uint spid, DrawingWriter& out);
    /**
     * Check object types in a container of groups of shapes.
     * @param o container of groups of shapes
     * @param out drawing writer
     */
    void processGroupShape(const MSO::OfficeArtSpgrContainer& o, DrawingWriter& out);

    /**
     * Check object type and call the appropriate method to process it.
     * @param o container for shapes
     * @param out drawing writer
     */
    void processDrawingObject(const MSO::OfficeArtSpContainer& o, DrawingWriter out);

    /**
     * Process the properties of a TextBox, use wv2 to parse the content.
     */
    void processTextBox(const MSO::OfficeArtSpContainer& o, DrawingWriter out);

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

    /**
     * Emit the textBoxFound signal.
     * @param index into plcfTxbxTxt
     * @param writing into styles.xml
     */
    void emitTextBoxFound(unsigned int index, bool stylesxml);

    /**
     * Insert an empty frame.  Use when the picture is an external file.
     */
    void insertEmptyInlineFrame(DrawingWriter& out);

    /**
     * A helper to get the correct rectangle for a shape or a childShape.
     */
    QRect getRect(const MSO::OfficeArtSpContainer &o);

    Document* m_document;
    KoStore* m_store;
    KoXmlWriter* m_currentWriter;
    KoXmlWriter* m_manifestWriter;
    KoGenStyles* m_mainStyles;

    const wvWare::Drawings* m_drawings;
    const wvWare::Word97::FIB& m_fib;

    MSO::OfficeArtDggContainer m_officeArtDggContainer;
    MSO::OfficeArtDgContainer* m_pOfficeArtHeaderDgContainer;
    MSO::OfficeArtDgContainer* m_pOfficeArtBodyDgContainer;

    QMap<QByteArray, QString> m_picNames; //picture names

    /*
     * Group specific attributes.
     */
    bool m_processingGroup;

    /*
     * Object specific attributes.
     */
    ObjectType m_objectType; // Type of the object in {Inline, Floating}.
    QByteArray m_rgbUid;     // Unique identifier of a BLIP.
    int m_zIndex;            // Position of current shape on z axis.

    // Specifies the type, size and border information for an inline picture.
    wvWare::SharedPtr<const wvWare::Word97::PICF> m_picf;

    //structure that specifies placement of a floating object
    wvWare::Word97::FSPA* m_pSpa;
};

#endif // GRAPHICSHANDLER_H
