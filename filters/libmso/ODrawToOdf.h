/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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

#ifndef ODRAWTOODF_H
#define ODRAWTOODF_H

#include "generated/simpleParser.h"
#include "writer.h"

class DrawStyle;
class QColor;

class ODrawToOdf {
public:
    class Client {
    public:
        virtual ~Client() {}
        /**
         * Get the bounding rect that defines the position of the diagram
         * in the hosting document.
         **/
        virtual QRectF getRect(const MSO::OfficeArtClientAnchor&) = 0;
        /**
         * Get the path in the ODF document that corresponds to the
         * image generated from the image with the given pib.
         **/
        virtual QString getPicturePath(int pib) = 0;
        /**
         * Check if the clientdata is the main content of a drawing object.
         **/
        virtual bool onlyClientData(const MSO::OfficeArtClientData& o) = 0;
        /**
         * Process the client data into ODF in a host application specific
         * manner.
         **/
        virtual void processClientData(const MSO::OfficeArtClientData& o,
                                       Writer& out) = 0;
        virtual void processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                                          const MSO::OfficeArtClientData* cd,
                                          Writer& out) = 0;
        /**
         * Create a fitting style for the current object.
         * This will be a style that can contain graphic style elements. So the
         * style will belong to one of the families 'graphic', 'presentation',
         * or 'chart'.
         **/
        virtual KoGenStyle createGraphicStyle(
                const MSO::OfficeArtClientTextBox* ct,
                const MSO::OfficeArtClientData* cd, Writer& out) = 0;
        /**
         * Add text properties to the style.
         * Host application specific style properties are added. These
         * properties are attributes to the elements style:paragraph-properties
         * or style:text-properties.
         **/
        virtual void addTextStyles(
                const MSO::OfficeArtClientTextBox* clientTextbox,
                const MSO::OfficeArtClientData* clientData,
                Writer& out, KoGenStyle& style) = 0;
        /**
         * Retrieve the OfficeArtDggContainer that contains global information
         * relating to the drawings.
         **/
        virtual const MSO::OfficeArtDggContainer* getOfficeArtDggContainer() = 0;
        /**
         * Convert the OfficeArtCOLORREF to a QColor.
         * This conversion requires color scheme information.
         **/
        virtual QColor toQColor(const MSO::OfficeArtCOLORREF& c) = 0;

        virtual QString formatPos(qreal v) = 0;
    };
private:
    Client* const client;
    QRectF getRect(const MSO::OfficeArtFSPGR &r);
    QRectF getRect(const MSO::OfficeArtSpContainer &o);
    void processEllipse(const MSO::OfficeArtSpContainer& fsp, Writer& out);
    void processRectangle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRoundRectangle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDiamond(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processTriangle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processTrapezoid(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processParallelogram(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processHexagon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processOctagon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLine(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSmiley(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processHeart(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processQuadArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processUturnArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFreeLine(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processPictureFrame(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processNotPrimitive(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processNotchedCircularArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processWedgeRectCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processWedgeEllipseCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCircularArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCloudCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processIrregularSeal1(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSeal24(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRibbon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDoubleWave(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartTerminator(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartProcess(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartDecision(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartConnector(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCallout2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDonut(const MSO::OfficeArtSpContainer& o, Writer& out);

    void processGroup(const MSO::OfficeArtSpgrContainer& o, Writer& out);
    void processStyle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processText(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processStyleAndText(const MSO::OfficeArtSpContainer& o, Writer& out);

    void processModifiers(const MSO::OfficeArtSpContainer& o, Writer& out, const QList<int>& defaults = QList<int>());
    /**
    * @brief set the width, height rotation and starting point for the given container
    */
    void set2dGeometry(const MSO::OfficeArtSpContainer& o, Writer& out);
    void setEnhancedGeometry(const MSO::OfficeArtSpContainer& o, Writer& out);

public:
    ODrawToOdf(Client& c) :client(&c) {}
    void processGroupShape(const MSO::OfficeArtSpgrContainer& o, Writer& out);
    void processDrawing(const MSO::OfficeArtSpgrContainerFileBlock& o, Writer& out);
    void processDrawingObject(const MSO::OfficeArtSpContainer& o, Writer& out);
    void defineGraphicProperties(KoGenStyle& style, const DrawStyle& ds, KoGenStyles& styles);
    void addGraphicStyleToDrawElement(Writer& out, const MSO::OfficeArtSpContainer& o);
};

/**
 * Convert FixedPoint to a qreal
 */
inline qreal toQReal(const MSO::FixedPoint& f)
{
    return f.integral + f.fractional / 65536.0;
}
const char* getFillType(quint32 fillType);
const char* getRepeatStyle(quint32 fillType);

#endif
