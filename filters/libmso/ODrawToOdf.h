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
class QPainterPath;

class ODrawToOdf
{
public:
    class Client
    {
    public:
        virtual ~Client() {}
        /**
         * Get the bounding rect that defines the position and dimensions of
         * the shape in the hosting document.
         **/
        virtual QRectF getRect(const MSO::OfficeArtClientAnchor&) = 0;

        /**
         * Get the bounding rect that defines the position and dimensions of
         * the shape in the hosting document if OfficeArtClientAnchor is
         * missing.
         */
        virtual QRectF getReserveRect(void) = 0;

        /**
         * Get the path in the ODF document that corresponds to the
         * image generated from the image with the given pib.
         **/
        virtual QString getPicturePath(const quint32 pib) = 0;
        /**
         * Check if the clientdata is the main content of a drawing object.
         **/
        virtual bool onlyClientData(const MSO::OfficeArtClientData& o) = 0;
        /**
         * Process the client data into ODF in a host application specific
         * manner.
         **/
        virtual void processClientData(const MSO::OfficeArtClientTextBox* ct,
                                       const MSO::OfficeArtClientData& o,
                                       Writer& out) = 0;
        virtual void processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                                          const MSO::OfficeArtClientData* cd,
                                          Writer& out) = 0;

        /**
         * Ask the host application whether to process an msosptRectangle type
         * shape container as an msosptTextBox.
         */
        virtual bool processRectangleAsTextBox(const MSO::OfficeArtClientData& cd) = 0;

        /**
         * Create a fitting style for the current object.
         * This will be a style that can contain graphic style elements. So the
         * style will belong to one of the families 'graphic', 'presentation',
         * or 'chart'.
         **/
        virtual KoGenStyle createGraphicStyle(
            const MSO::OfficeArtClientTextBox* ct,
            const MSO::OfficeArtClientData* cd,
            const DrawStyle& ds,
            Writer& out) = 0;

        /**
         * Add host application specific formatting properties for text and
         * paragraphs to the style of the draw element.  These properties are
         * attributes to style:paragraph-properties or style:text-properties.
         * Also add host application specific attributes to the draw element.
         **/
        virtual void addTextStyles(
            const MSO::OfficeArtClientTextBox* clientTextbox,
            const MSO::OfficeArtClientData* clientData,
            KoGenStyle& style,
            Writer& out) = 0;

        /**
         * Convert the OfficeArtCOLORREF to a QColor.  This conversion requires
         * color scheme information.
         **/
        virtual QColor toQColor(const MSO::OfficeArtCOLORREF& c) = 0;

        /**
         *
         */
        virtual QString formatPos(qreal v) = 0;

        /**
         * Retrieve the OfficeArtDggContainer that contains global information
         * relating to the drawings.
         **/
        virtual const MSO::OfficeArtDggContainer* getOfficeArtDggContainer() = 0;

        /**
         * Retrieve the OfficeArtSpContainer of the master shape.
         * @param spid identifier of the master shape.
         **/
        virtual const MSO::OfficeArtSpContainer* getMasterShapeContainer(quint32 spid) = 0;

        quint16 m_currentShapeType;

    }; //End class Client

private:
    Client* const client;

    /**
     * Both OfficeArtClientAnchorData and OfficeArtChildAnchor might contain a
     * 90 degrees rotated rectangle.  It depends on the value of the rotation
     * property and the intervals differ for each shape type.
     *
     * @param shapeType
     * @param rotation [degrees] - normalization will be applied
     * @param rect the group, client or child rectangle
     * @return copy of the rectangle free of any transformations
     */
    QRectF processRect(const quint16 shapeType, const qreal rotation, QRectF &rect);

    /**
     * MSOffice 2003/2007 use different values for the rotation property so we
     * have to normalize before processing.
     *
     * @param rotation [degrees]
     * @return rotation in <0, 360>
     */
    qint16 normalizeRotation(qreal rotation);

    QRectF getRect(const MSO::OfficeArtFSPGR &r);
    QRectF getRect(const MSO::OfficeArtSpContainer &o);
    void processRectangle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processTextBox(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLine(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processStraightConnector1(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processPictureFrame(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processNotPrimitive(const MSO::OfficeArtSpContainer& o, Writer& out);

    typedef void (ODrawToOdf::*PathArtist)(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawStraightConnector1(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathBentConnector2(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathBentConnector3(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathBentConnector4(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathBentConnector5(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathCurvedConnector2(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathCurvedConnector3(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathCurvedConnector4(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void drawPathCurvedConnector5(qreal l, qreal t, qreal r, qreal b, Writer& out, QPainterPath &shapePath) const;
    void processConnector(const MSO::OfficeArtSpContainer& o, Writer& out, PathArtist drawPath);

    // shapes2.cpp
    void processRoundRectangle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processEllipse(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDiamond(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processIsocelesTriangle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRightTriangle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processParallelogram(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processTrapezoid(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processHexagon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processOctagon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processPlus(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processStar(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processHomePlate(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCube(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processPlaque(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCan(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDonut(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCallout1(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCallout2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCallout3(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentCallout1(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentCallout2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentCallout3(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBorderCallout1(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBorderCallout2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBorderCallout3(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentBorderCallout1(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentBorderCallout2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentBorderCallout3(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRibbon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRibbon2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processChevron(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processPentagon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processNoSmoking(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSeal8(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSeal16(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSeal32(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processWedgeRectCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processWedgeRRectCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processWedgeEllipseCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processWave(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFoldedCorner(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDownArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processUpDownArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processIrregularSeal1(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processIrregularSeal2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLightningBolt(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processHeart(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processQuadArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRightArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processUpArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDownArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftRightArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processUpDownArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processQuadArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBevel(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftBracket(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRightBracket(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftBrace(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processRightBrace(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBentUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBentArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSeal24(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processStripedRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processNotchedRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBlockArc(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSmileyFace(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processVerticalScroll(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processHorizontalScroll(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCircularArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processNotchedCircularArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processUturnArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCurvedRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCurvedLeftArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCurvedUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCurvedDownArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCloudCallout(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processEllipseRibbon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processEllipseRibbon2(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartProcess(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartDecision(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartInputOutput(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartPredefinedProcess(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartInternalStorage(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartDocument(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartMultidocument(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartTerminator(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartPreparation(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartManualInput(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartManualOperation(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartConnector(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartPunchedCard(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartPunchedTape(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartSummingJunction(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartOr(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartCollate(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartSort(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartExtract(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartMerge(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartOnlineStorage(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartMagneticTape(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartMagneticDisk(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartMagneticDrum(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartDisplay(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartDelay(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartAlternateProcess(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processFlowChartOffpageConnector(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processCallout90(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentCallout90(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBorderCallout90(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processAccentBorderCallout90(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processLeftRightUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSun(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processMoon(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBracketPair(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processBracePair(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processSeal4(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processDoubleWave(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonBlank(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonHome(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonHelp(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonInformation(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonForwardNext(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonBackPrevious(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonEnd(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonBeginning(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonReturn(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonDocument(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonSound(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processActionButtonMovie(const MSO::OfficeArtSpContainer& o, Writer& out);


    void processStyle(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processText(const MSO::OfficeArtSpContainer& o, Writer& out);
    void processStyleAndText(const MSO::OfficeArtSpContainer& o, Writer& out);

    void processModifiers(const MSO::OfficeArtSpContainer& o, Writer& out, const QList<int>& defaults = QList<int>());
    /**
     * @brief set the width, height rotation and starting point for the given
     * container
     */
    void set2dGeometry(const MSO::OfficeArtSpContainer& o, Writer& out);
    void setEnhancedGeometry(const MSO::OfficeArtSpContainer& o, Writer& out);
    QString path2svg(const QPainterPath &path);
    void setShapeMirroring(const MSO::OfficeArtSpContainer& o, Writer& out);

public:
    explicit ODrawToOdf(Client& c) :client(&c) {}
    void processGroupShape(const MSO::OfficeArtSpgrContainer& o, Writer& out);
    void processDrawing(const MSO::OfficeArtSpgrContainerFileBlock& o, Writer& out);
    void processDrawingObject(const MSO::OfficeArtSpContainer& o, Writer& out);
    void defineGraphicProperties(KoGenStyle& style, const DrawStyle& ds, KoGenStyles& styles);
    void addGraphicStyleToDrawElement(Writer& out, const MSO::OfficeArtSpContainer& o);
    void defineGradientStyle(KoGenStyle& style, const DrawStyle& ds);
    QString defineDashStyle(KoGenStyles& styles, const quint32 lineDashing);

    /**
     * Define and insert standard marker style into styles collection.
     * @return the name that has been assigned for the inserted style
     * or an empty string in case of an unsupported arrowType.
     */
    QString defineMarkerStyle(KoGenStyles& styles, const quint32 arrowType);

    /**
     * Apply the logic defined in MS-ODRAW subsection 2.2.2 to the provided
     * OfficeArtCOLORREF record.
     *
     * @param c OfficeArtCOLORREF record
     * @param ds DrawStyle to access shape properties and client specific f.
     * @return final color
     */
    QColor processOfficeArtCOLORREF(const MSO::OfficeArtCOLORREF& c, const DrawStyle& ds);
};

/**
 * Convert FixedPoint to a qreal
 */
inline qreal toQReal(const MSO::FixedPoint& f)
{
    return f.integral + f.fractional / 65536.0;
}

const char* getFillRule(quint16 shapeType);
const char* getFillType(quint32 fillType);
const char* getRepeatStyle(quint32 fillType);
const char* getGradientRendering(quint32 fillType);
const char* getHorizontalPos(quint32 posH);
const char* getHorizontalRel(quint32 posRelH);
const char* getVerticalPos(quint32 posV);
const char* getVerticalRel(quint32 posRelV);
const char* getHorizontalAlign(quint32 anchorText);
const char* getVerticalAlign(quint32 anchorText);
const char* getStrokeLineCap(quint32 capStyle);
const char* getStrokeLineJoin(quint32 joinStyle);

#endif
