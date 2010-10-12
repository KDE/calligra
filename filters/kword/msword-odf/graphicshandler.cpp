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

#include "graphicshandler.h"
#include "document.h"
#include "conversion.h"

#include <wv2/src/ms_odraw.h>

#include "generated/leinputstream.h"
#include "drawstyle.h"
#include "ODrawToOdf.h"
#include "pictures.h"

#include <KoStoreDevice.h>
#include <KoGenStyle.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <QtGui/QColor>
#include <QByteArray>

using namespace wvWare;
using namespace MSO;

using Conversion::twipsToPt;

#define IMG_BUF_SIZE 2048L


namespace
{
const char* dashses[11] = {
    "", "Dash_20_2", "Dash_20_3", "Dash_20_2", "Dash_20_2", "Dash_20_2",
    "Dash_20_4", "Dash_20_6", "Dash_20_5", "Dash_20_7", "Dash_20_8"
};
const char* arrowHeads[6] = {
    "", "msArrowEnd_20_5", "msArrowStealthEnd_20_5", "msArrowDiamondEnd_20_5",
    "msArrowOvalEnd_20_5", "msArrowOpenEnd_20_5"
};
QString format(double v) {
    static const QString f("%1");
    static const QString e("");
    static const QRegExp r("\\.?0+$");
    return f.arg(v, 0, 'f').replace(r, e);
}
QString pt(double v) {
    static const QString pt("pt");
    return format(v) + pt;
}
QString percent(double v) {
    return format(v) + '%';
}
QString mm(double v) {
    static const QString mm("mm");
    return format(v) + mm;
}
}

/*
 * ************************************************
 * Drawing Writer
 * ************************************************
 */
DrawingWriter::DrawingWriter(KoXmlWriter& xmlWriter, KoGenStyles& kostyles,
                             bool stylesxml_, bool inlineObj)
        : Writer(xmlWriter, kostyles, stylesxml_),
          xLeft(0),
          xRight(0),
          yTop(0),
          yBottom(0),
          m_pSpa(0),
          m_picf(0),
          m_rgbUid(0),
          m_bodyDrawing(true),
          m_inline(inlineObj)
{
    scaleX = 25.4 / 1440;
    scaleY = 25.4 / 1440;
}

qreal DrawingWriter::vLength()
{
    return Writer::vLength(yBottom - yTop);
}

qreal DrawingWriter::hLength()
{
    return Writer::hLength(xRight - xLeft);
}

qreal DrawingWriter::vOffset()
{
    return Writer::vOffset(yTop);
}

qreal DrawingWriter::hOffset()
{
    return Writer::hOffset(xLeft);
}

void DrawingWriter::SetRectangle(wvWare::Word97::FSPA& spa)
{
    xLeft = spa.xaLeft;
    xRight = spa.xaRight;
    yTop = spa.yaTop;
    yBottom = spa.yaBottom;
}

void DrawingWriter::SetGroupRectangle(MSO::OfficeArtFSPGR& fspgr)
{
    if (fspgr.xRight == fspgr.xLeft) {
        return;
    }

    if (fspgr.yBottom == fspgr.yTop)
        return;

    xOffset = xOffset + xLeft*scaleX;
    yOffset = yOffset + yTop*scaleY;

    scaleX = scaleX * (xRight - xLeft)/(qreal)(fspgr.xRight - fspgr.xLeft);
    scaleY = scaleY * (yBottom - yTop)/(qreal)(fspgr.yBottom - fspgr.yTop);

    xOffset = xOffset - fspgr.xLeft * scaleX;
    yOffset = yOffset - fspgr.yTop * scaleY;
}

void DrawingWriter::SetClientRectangle(MSO::OfficeArtChildAnchor& anchor)
{
    xLeft = anchor.xLeft;
    xRight = anchor.xRight;
    yTop = anchor.yTop;
    yBottom = anchor.yBottom;
}

/*
 * ************************************************
 * Graphics Handler
 * ************************************************
 */
KWordGraphicsHandler::KWordGraphicsHandler(Document* doc, KoXmlWriter* bodyWriter,
                                         KoXmlWriter* manifestWriter, KoStore* store,
                                         KoGenStyles* mainStyles)
: QObject()
, m_document(doc)
, m_store(store)
, m_bodyWriter(bodyWriter)
, m_manifestWriter(manifestWriter)
, m_mainStyles(mainStyles)
, m_drawings(0)
, m_fib(0)
, m_pOfficeArtHeaderDgContainer(0)
, m_pOfficeArtBodyDgContainer(0)
, m_zIndex(0)
{
    kDebug(30513) ;
}

KWordGraphicsHandler::~KWordGraphicsHandler()
{
    delete m_pOfficeArtHeaderDgContainer;
    delete m_pOfficeArtBodyDgContainer;
}

/*
 * NOTE: All containers parsed by this function are optional.
 */
void KWordGraphicsHandler::init(Drawings * pDrawings, const wvWare::Word97::FIB &fib)
{
    kDebug(30513);

    //TODO: do not use POLE storage here, get the pointer to the stream, have
    //to access FIB first in mswordodfimport.cpp
    parseOfficeArtContainer(m_document->storage(), fib);

    //create default GraphicStyle using information from OfficeArtDggContainer
    defineDefaultGraphicStyle(m_mainStyles);

    //parse and store floating pictures  
    parseFloatingPictures();
    m_picNames = createFloatingPictures(m_store, m_manifestWriter);

    m_drawings = pDrawings;
    m_fib = const_cast<wvWare::Word97::FIB *>(&fib);

    //Provide the backgroud color information to the Document, if present.
    DrawStyle ds = getDrawingStyle();
    if (ds.fFilled()) {
        MSO::OfficeArtCOLORREF fc = ds.fillColor();
        QColor color = QColor(fc.red, fc.green, fc.blue);
        QString tmp = color.name();
        if (tmp != m_document->currentBgColor()) {
            m_document->updateBgColor(tmp);
        }
    }
    return;
}

void KWordGraphicsHandler::handleInlineObject(const wvWare::PictureData& data)
{
    kDebug(30513) ;
    // going to parse and process the Data stream content
    LEInputStream* in = m_document->data_stream();
    int size = (data.picf->lcb - data.picf->cbHeader);

    kDebug(30513) << "\nCurrent stream position: " << in->getPosition()
		  << "\nOfficeArtInlineSpContainer offset: " << data.fcPic
                  << "\nPICF size: " << data.picf->cbHeader
                  << "\nOfficeArtData size: " << size;

    // parse the OfficeArtInlineSpContainer and rewind the stream
    LEInputStream::Mark _zero;
    _zero = in->setMark();
    in->skip(data.fcPic);
    OfficeArtInlineSpContainer co;
    parseOfficeArtInlineSpContainer(*in, co);

    int n = (data.fcPic + size) - in->getPosition();
    if (n) {
        kDebug(30513) << "Warning: " << n
                      << " bytes left while parsing OfficeArtInlineSpContainer";
    }
    in->rewind(_zero);

    // store picture data if present and update m_picNames
    PictureReference ref;
    m_store->enterDirectory("Pictures");
    foreach (const OfficeArtBStoreContainerFileBlock& block, co.rgfb) {
        const OfficeArtFBSE* fbse = block.anon.get<MSO::OfficeArtFBSE>();
        if (!fbse) {
            kDebug(30513) << "Warning: FBSE container not found, skipping ";
        }
        else {
            //check if this BLIP is already in hash table
            if (m_picNames.contains(fbse->rgbUid)) {
                ref.uid = fbse->rgbUid;
                continue;
            }
            else {
                ref = savePicture(block, m_store);
                if (ref.name.length() == 0) {
                    kDebug(30513) << "empty name in picture reference";
                    break;
                }
                m_manifestWriter->addManifestEntry("Pictures/" + ref.name, ref.mimetype);
                m_picNames[ref.uid] = ref.name;
            }
        }
    }
    m_store->leaveDirectory();

    DrawingWriter out(*m_bodyWriter, *m_mainStyles, true, true);
    out.m_picf = data.picf;
    out.m_rgbUid = ref.uid;

    OfficeArtSpContainer* o = &(co.shape);
    processDrawingObject(*o, out);
}

void KWordGraphicsHandler::handleFloatingObject(unsigned int globalCP)
{
    kDebug(30513) << "globalCP" << globalCP ;
    // draw shape or group of shapes
    if (m_drawings == NULL) {
        return;
    }

    PLCF<Word97::FSPA>* fspa = m_drawings->getSpaMom();
    if (fspa != 0) {
        PLCFIterator<Word97::FSPA> it(fspa->at(0));

        //search for drawing in main body
        for (size_t i = 0; i < fspa->count(); i++, ++it) {
            kDebug(30513) << "FSPA start:" << it.currentStart();
            kDebug(30513) << "FSPA spid:" << it.current()->spid;

            if (it.currentStart() == globalCP) {
                DrawingWriter out(*m_bodyWriter, *m_mainStyles, true, false);
                out.m_pSpa = it.current();
                out.m_bodyDrawing = true;
                locateDrawing((m_pOfficeArtBodyDgContainer->groupShape).data(), out,
			      it.current(), (uint) it.current()->spid);
                return;
            }
        }
    }

    fspa = m_drawings->getSpaHdr();
    if (fspa != 0) {
        PLCFIterator<Word97::FSPA> itHeader(fspa->at(0));
        //search for drawing in header
        for (size_t i = 0; i < fspa->count(); i++, ++itHeader) {
            kDebug(30513) << "FSPA start:" << itHeader.currentStart() + m_fib->ccpText + m_fib->ccpFtn;
            kDebug(30513) << "FSPA spid:" << itHeader.current()->spid;

            if ((itHeader.currentStart() + m_fib->ccpText + m_fib->ccpFtn)  == globalCP) {
                DrawingWriter out(*m_bodyWriter, *m_mainStyles, true, false);
                out.m_pSpa = itHeader.current();
                out.m_bodyDrawing = false;
                locateDrawing((m_pOfficeArtHeaderDgContainer->groupShape).data(), out,
                              itHeader.current(), (uint) itHeader.current()->spid);
                return;
            }
        }
    }
}

void KWordGraphicsHandler::setBodyWriter(KoXmlWriter* writer)
{
    m_bodyWriter = writer;
}

DrawStyle KWordGraphicsHandler::getDrawingStyle()
{
    if (m_pOfficeArtBodyDgContainer != NULL) {
        if (m_pOfficeArtBodyDgContainer->shape.isNull() == false) {
            if ((*m_pOfficeArtBodyDgContainer->shape).shapePrimaryOptions.isNull() == false) {
                return DrawStyle(m_OfficeArtDggContainer, NULL, m_pOfficeArtBodyDgContainer->shape.data());
            }
        }
    }
    return DrawStyle(m_OfficeArtDggContainer);
}

void KWordGraphicsHandler::locateDrawing(const MSO::OfficeArtSpgrContainer* spgr, DrawingWriter& out, 
                                         wvWare::Word97::FSPA* spa, uint spid)
{
    if (spgr == NULL) {
        return;
    }
    m_zIndex = 0;

    //FIXME: combine childAnchor, shapeGroup coordinates with information from
    //clientAnchor pointing to the SPA structure!

    //NOTE: The OfficeArtSpgrContainer record specifies a container for groups
    //(4) of shapes.  The group (4) container contains a variable number of
    //shape containers and other group (4) containers.  Each group (4) is a
    //shape.  The first container MUST be an OfficeArtSpContainer record, which
    //MUST contain shape information for the group. MS-ODRAW, 2.2.16

    foreach (const OfficeArtSpgrContainerFileBlock& co, spgr->rgfb) {

        if (co.anon.is<OfficeArtSpgrContainer>()) {
            const OfficeArtSpContainer* first = 
                (*co.anon.get<OfficeArtSpgrContainer>()).rgfb[0].anon.get<OfficeArtSpContainer>();
            if (first && first->shapeProp.spid == spid) {
                out.SetRectangle(*spa);
                processGroup(*co.anon.get<OfficeArtSpgrContainer>(), out);
                break;
            } else {
                m_zIndex = m_zIndex + (*co.anon.get<OfficeArtSpgrContainer>()).rgfb.size();
            }
        } else {
            const OfficeArtSpContainer &sp = *co.anon.get<OfficeArtSpContainer>();
            if (sp.shapeProp.fGroup) {
		if (sp.shapeGroup) {
                    out.SetGroupRectangle(*sp.shapeGroup);
                }
                if (sp.shapeProp.spid == spid) {
                    kDebug(30513) << "An unprocessed shape storing information for the group is referred from text!";
                }
            } else if (sp.shapeProp.spid == spid) {
                out.SetRectangle(*spa);
                processDrawingObject(sp, out);
                break;
            }
            m_zIndex++;
        }
    }
}

void KWordGraphicsHandler::processGroup(const MSO::OfficeArtSpgrContainer& o, DrawingWriter& out)
{
    if (o.rgfb.size() < 2) {
        return;
    }
    //TODO: create corresponding style and apply style properties
    out.xml.startElement("draw:g");
    const OfficeArtSpContainer *first = o.rgfb[0].anon.get<OfficeArtSpContainer>();

    if (first && first->shapeGroup) {
        //process shape information for the group
        out.SetGroupRectangle(*first->shapeGroup);
    }

    for (int i = 1; i < o.rgfb.size(); ++i) {
        if (o.rgfb[i].anon.is<OfficeArtSpContainer>()) {
            OfficeArtSpContainer sp = *o.rgfb[i].anon.get<OfficeArtSpContainer>();
            if (sp.childAnchor) {
                out.SetClientRectangle(*sp.childAnchor); //set child rectangle
            }
            processDrawingObject(sp, out); //draw objects
        } 
	//TODO: another group shape can be here! We should call locateDrawing
	//again!
    }
    out.xml.endElement(); // draw:g
}

void KWordGraphicsHandler::processDrawingObject(const MSO::OfficeArtSpContainer& o, DrawingWriter out)
{
    kDebug(30513);

    DrawStyle ds(NULL, &o);

    // check the shape type and process it
    kDebug(30513) << "shapeType: " << hex << o.shapeProp.rh.recInstance;
    kDebug(30513) << "grupShape: " << o.shapeProp.fGroup;
    kDebug(30513) << "Selected properties: ";
    kDebug(30513) << "pib: " << ds.pib();

    // textbox can be msosptTextBox or msosptRectangle or ...
    if (!o.clientTextbox.isNull()) {
        kDebug(30513)<< "processing text box";
        parseTextBox(o, out);
        return;
    }

    switch (o.shapeProp.rh.recInstance)
    {
        case msosptRectangle: {
            kDebug(30513)<< "processing rectangle";
            //check group shape boolean properties for details
            if (ds.fHorizRule()) {
                kDebug(30513)<< "processing a line shape";
                processLineShape(o, out);
            }
            processRectangle(o, out);
            break;
        }
        case msosptEllipse:
            kDebug(30513)<< "processing ellipse";
            break;
        case msosptPictureFrame:
            kDebug(30513)<< "processing a frame shape";
            if (out.m_inline) {
                processInlinePictureFrame(o, out);
	    }
            else {
                processFloatingPictureFrame(o, out);
            }
            break;
        case msosptHostControl:
            kDebug(30513)<< "processing host control";
            parseTextBox(o, out);
            break;
    }
}

void KWordGraphicsHandler::parseOfficeArtContainer(POLE::Storage* storage, const  wvWare::Word97::FIB &fib)
{
    kDebug(30513);
    // get OfficeArtContent
    if (fib.lcbDggInfo != 0) {

	const std::string table = fib.fWhichTblStm ? "1Table" : "0Table";
        POLE::Stream stream(storage, table);
        QByteArray array;
        QBuffer buffer;

        array.resize(fib.lcbDggInfo);
        stream.seek(fib.fcDggInfo);
        unsigned long n = stream.read((unsigned char*) array.data(), fib.lcbDggInfo);
        if (n != fib.lcbDggInfo) {
            kDebug(30513) << "Failed to read data from " << table.data() << "stream";
            return;
        }

        buffer.setData(array);
        buffer.open(QIODevice::ReadOnly);
        LEInputStream in(&buffer);

        //parse OfficeArfDggContainer from msdoc
        try {
            parseOfficeArtDggContainer(in, m_OfficeArtDggContainer);
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing parseOfficeArtDggContainer";
            return;
        }
        catch (...) {
            kDebug(30513) << "caught unknown exception while parsing parseOfficeArtDggContainer";
            return;
        }
        kDebug(30513) << "OfficeArtDggContainer parsed successfully" ;

        // parse drawingsVariable from msdoc
        // 0 - next OfficeArtDgContainer belongs to Main document;
        // 1 - next OfficeArtDgContainer belongs to Header Document
        unsigned char drawingsVariable = 0;
        try {
            drawingsVariable = in.readuint8();
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing drawingsVariable ";
            return;
        }
        catch (...) {
            kDebug(30513) << "caught unknown exception while parsing drawingsVariable";
            return;
        }

        //parse OfficeArfDgContainer from msdoc
        OfficeArtDgContainer *pDgContainer = NULL;
        try {
            pDgContainer = new OfficeArtDgContainer();
            if (drawingsVariable == 0) {
                m_pOfficeArtBodyDgContainer = pDgContainer;
            } else {
                m_pOfficeArtHeaderDgContainer = pDgContainer;
            }
            parseOfficeArtDgContainer(in, *pDgContainer);
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing OfficeArtDgContainer ";
            return;
        }
        catch (...) {
            kDebug(30513) << "caught unknown exception while parsing OfficeArtDgContainer";
            return;
        }

        // parse drawingsVariable from msdoc
        // 0 - next OfficeArtDgContainer belongs to Main document;
        // 1 - next OfficeArtDgContainer belongs to Header Document
        try {
            drawingsVariable = in.readuint8();
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing second drawingsVariable ";
            //wvlog << "in position: " << in.getPosition() << std::endl;
            return;
        }
        catch (...) {
            kDebug(30513) << "caught unknown exception while parsing second drawingsVariable";
            return;
        }

        //parse OfficeArfDgContainer from msdoc
        pDgContainer = NULL;
        try {
            pDgContainer = new OfficeArtDgContainer();
            if (drawingsVariable == 0) {
                if (m_pOfficeArtBodyDgContainer != NULL){
                    delete m_pOfficeArtBodyDgContainer;
                }
                m_pOfficeArtBodyDgContainer = pDgContainer;
            }
            else {
                if (m_pOfficeArtHeaderDgContainer != NULL) {
                    delete m_pOfficeArtHeaderDgContainer;
                }
                m_pOfficeArtHeaderDgContainer = pDgContainer;
            }
            parseOfficeArtDgContainer(in, *pDgContainer);
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing second OfficeArtDgContainer ";
            return;
        }
        catch (...) {
            kDebug(30513) << "caught unknown exception while parsing second OfficeArtDgContainer";
            return;
        }

        if (in.getPosition() != buffer.size()) {
            kDebug(30513) << (uint)(buffer.size() - in.getPosition())
            << "bytes left at the end of parseOfficeArtDggContainer,"
            << " parseOfficeArtDgContainer, so probably an error at position "
            << (uint) in.getMaxPosition();
        }
    }
    return;
}

void KWordGraphicsHandler::defineDefaultGraphicStyle(KoGenStyles* styles)
{
    // write style <style:default-style style:family="graphic">
    KoGenStyle style(KoGenStyle::GraphicStyle, "graphic");
    DrawStyle ds(m_OfficeArtDggContainer);
    style.setDefaultStyle(true);
    defineGraphicProperties(style, ds);
    styles->insert(style);
}

void KWordGraphicsHandler::defineGraphicProperties(KoGenStyle& style, const DrawStyle& ds,
                                                  const QString& listStyle)
{
    MSO::OfficeArtCOLORREF clr;
    const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
    // dr3d:ambient-color
    // dr3d:back-scale
    // dr3d:backface-culling
    // dr3d:close-back
    // dr3d:close-front
    // dr3d:depth
    // dr3d:diffuse-color
    // dr3d:edge-rounding
    // dr3d:edge-rounding-mode
    // dr3d:emissive-color
    // dr3d:end-angle
    // dr3d:horizontal-segments
    // dr3d:lighting-mode
    // dr3d:normals-direction
    // dr3d:normals-kind
    // dr3d:shadow
    // dr3d:shininess
    // dr3d:specular-color
    // dr3d:texture-filter
    // dr3d:texture-generation-mode-x
    // dr3d:texture-generation-mode-y
    // dr3d:texture-kind
    // dr3d:texture-mode
    // dr3d:vertical-segments
    // draw:auto-grow-height
    // draw:auto-grow-width
    // draw:blue
    // draw:caption-angle
    // draw:caption-angle-type
    // draw:caption-escape
    // draw:caption-escape-direction
    // draw:caption-fit-line-length
    // draw:caption-gap
    // draw:caption-line-length
    // draw:caption-type
    // draw:color-inversion
    // draw:color-mode
    // draw:contrast
    // draw:decimal-places
    // draw:end-guide
    // draw:end-line-spacing-horizontal
    // draw:end-line-spacing-vertical

    // NOTE: fFilled specifies whether fill of the shape is render based on the
    // properties of the "fill style" property set.
    if (ds.fFilled()) {
        qint32 fillType = ds.fillType();
        // draw:fill ("bitmap", "gradient", "hatch", "none" or "solid")
        style.addProperty("draw:fill", getFillType(fillType), gt);
        // NOTE: only set the color if the fill type is 'solid' because OOo
        // ignores fill='none' if the color is set
        if (fillType == 0) {
            clr = ds.fillColor();
            style.addProperty("draw:fill-color", QColor(clr.red, clr.green, clr.blue).name(), gt);
        }
    } else {
        style.addProperty("draw:fill", "none", gt);
    }
    // draw:fill-gradient-name
    // draw:fill-hatch-name
    // draw:fill-hatch-solid
    // draw:fill-image-name
    // draw:fill-image-ref-point
    // draw:fill-image-ref-point-x
    // draw:fill-image-ref-point-y
    // draw:fill-image-height
    // draw:fill-image-width
    // draw:fit-to-contour
    // draw:fit-to-size
    // draw:frame-display-border
    // draw:frame-display-scrollbar
    // draw:frame-margin-horizontal
    // draw:frame-margin-vertical
    // draw:gamma
    // draw:gradient-step-count
    // draw:green
    // draw:guide-distance
    // draw:guide-overhang
    // draw:image-opacity
    // draw:line-distance
    // draw:luminance
    // draw:marker-end
    quint32 lineEndArrowhead = ds.lineEndArrowhead();
    if (lineEndArrowhead > 0 && lineEndArrowhead < 6) {
        style.addProperty("draw:marker-end", arrowHeads[lineEndArrowhead], gt);
    }
    // draw:marker-end-center
    // draw:marker-end-width
    qreal lineWidthPt = ds.lineWidth() / 12700.;
    style.addProperty("draw:marker-end-width",
                      pt(lineWidthPt*4*(1+ds.lineEndArrowWidth())), gt);
    // draw:marker-start
    quint32 lineStartArrowhead = ds.lineStartArrowhead();
    if (lineStartArrowhead > 0 && lineStartArrowhead < 6) {
        style.addProperty("draw:marker-start", arrowHeads[lineStartArrowhead],
                          gt);
    }
    // draw:marker-start-center
    // draw:marker-start-width
    style.addProperty("draw:marker-start-width",
                      pt(lineWidthPt*4*(1+ds.lineStartArrowWidth())), gt);
    // draw:measure-align
    // draw:measure-vertical-align
    // draw:ole-draw-aspect
    // draw:opacity
    // draw:opacity-name
    // draw:parallel
    // draw:placing
    // draw:red
    // draw:secondary-fill-color

    // NOTE: fShadow property specifies whether the shape has a shadow.
    if (ds.fShadow()) {
        // draw:shadow
        style.addProperty("draw:shadow", "visible", gt);
        // draw:shadow-color
        clr = ds.shadowColor();
        style.addProperty("draw:shadow-color", QColor(clr.red, clr.green, clr.blue).name(), gt);
        //shadowOffset* properties MUST exist if shadowType property equals
        //msoshadowOffset or msoshadowDouble, otherwise MUST be ignored,
        //MS-ODRAW 2.3.13.6
        quint32 type = ds.shadowType();
        if ((type == 0) || (type == 1)) {
            // draw:shadow-offset-x
            style.addProperty("draw:shadow-offset-x", pt(ds.shadowOffsetX()/12700.), gt);
            // draw:shadow-offset-y
            style.addProperty("draw:shadow-offset-y", pt(ds.shadowOffsetY()/12700.), gt);
        }
        // draw:shadow-opacity
        float shadowOpacity = toQReal(ds.shadowOpacity());
        style.addProperty("draw:shadow-opacity", percent(100*shadowOpacity), gt);
    } else {
        style.addProperty("draw:shadow", "hidden");
    }
    // draw:show-unit
    // draw:start-guide
    // draw:start-line-spacing-horizontal
    // draw:start-line-spacing-vertical
    // draw:stroke ('dash', 'none' or 'solid')
    quint32 lineDashing = ds.lineDashing();
    // OOo interprets solid line with with 0 as hairline, so if
    // width == 0, stroke *must* be none to avoid OOo from
    // displaying a line
    if (lineWidthPt == 0) {
        style.addProperty("draw:stroke", "none", gt);
    } else if (ds.fLine() || ds.fNoLineDrawDash()) {
        if (lineDashing > 0 && lineDashing < 11) {
            style.addProperty("draw:stroke", "dash", gt);
        } else {
            style.addProperty("draw:stroke", "solid", gt);
        }
    } else {
        style.addProperty("draw:stroke", "none", gt);
    }
    // draw:stroke-dash from 2.3.8.17 lineDashing
    if (lineDashing > 0 && lineDashing < 11) {
        style.addProperty("draw:stroke-dash", dashses[lineDashing], gt);
    }
    // draw:stroke-dash-names
    // draw:stroke-linejoin
    // draw:symbol-color
    // draw:textarea-horizontal-align
    // draw:textarea-vertical-align
    // draw:tile-repeat-offset
    // draw:unit
    // draw:visible-area-height
    // draw:visible-area-left
    // draw:visible-area-top
    // draw:visible-area-width
    // draw:wrap-influence-on-position
    // fo:background-color
    // fo:border
    // fo:border-bottom
    // fo:border-left
    // fo:border-right
    // fo:border-top
    // fo:clip
    // fo:max-height
    // fo:max-width
    // fo:min-height
    // fo:min-width
    // fo:padding
    // fo:padding-left
    // fo:padding-top
    // fo:padding-right
    // fo:padding-bottom
    // fo:wrap-option
    // style:border-line-width
    // style:border-line-width-bottom
    // style:border-line-width-left
    // style:border-line-width-right
    // style:border-line-width-top
    // style:editable
    // style:flow-with-text
    // style:mirror
    // style:overflow-behavior
    // style:print-content
    // style:protect
    // style:rel-height
    // style:rel-width
    // style:repeat
    // svg:fill-rule
    // svg:height
    // svg:stroke-color from 2.3.8.1 lineColor
    clr = ds.lineColor();
    QColor lineColor(clr.red,clr.green,clr.blue);
    style.addProperty("svg:stroke-color", lineColor.name(), gt);
    // svg:stroke-opacity from 2.3.8.2 lineOpacity
    style.addProperty("svg:stroke-opacity",
                      percent(100.0 * ds.lineOpacity() / 0x10000), gt);
    // svg:stroke-width from 2.3.8.14 lineWidth
    style.addProperty("svg:stroke-width", pt(lineWidthPt), gt);
    // svg:width
    // svg:x
    // svg:y
    // text:anchor-page-number
    // text:anchor-type
    // text:animation
    // text:animation-delay
    // text:animation-direction
    // text:animation-repeat
    // text:animation-start-inside
    // text:animation-steps
    // text:animation-stop-inside

    /* associate with a text:list-style element */
    if (!listStyle.isNull()) {
        style.addAttribute("style:list-style-name", listStyle);
    }
}

void KWordGraphicsHandler::defineAnchorProperties(KoGenStyle& style, const DrawStyle& ds)
{
    // style:horizontal-pos MS-ODRAW - 2.3.4.19
    quint32 posH = ds.posH();
    if (posH == 0) {  // msophAbs
        style.addProperty("style:horizontal-pos","from-left");
    } else if (posH == 1) { // msophLeft
        style.addProperty("style:horizontal-pos","left");
    } else if (posH == 2) { // msophCenter
        style.addProperty("style:horizontal-pos","center");
    } else if (posH == 3) { // msophRight
        style.addProperty("style:horizontal-pos","right");
    } else if (posH == 4) { // msophInside
        style.addProperty("style:horizontal-pos","inside");
    } else if (posH == 5) { // msophOutside
        style.addProperty("style:horizontal-pos","outside");
    }

    // style:horizontal-rel MS-ODRAW 2.3.4.20
    quint32 posRelH = ds.posRelH();

    if (posRelH == 0) {//msoprhMargin
        style.addProperty("style:horizontal-rel","page-content");
    } else if (posRelH == 1) {//msoprhPage
        style.addProperty("style:horizontal-rel","page");
    } else if (posRelH == 2) {//msoprhText
        style.addProperty("style:horizontal-rel","paragraph");
    } else if (posRelH == 3) {//msoprhChar
        style.addProperty("style:horizontal-rel","char");
    } else {
        style.addProperty("style:horizontal-rel","page-content");
    }

    // style:vertical-pos MS-ODRAW - 2.3.4.21
    quint32 posV = ds.posV();
    if (posV == 0) {  // msophAbs
        style.addProperty("style:vertical-pos","from-top");
    } else if (posV == 1) { // msophTop
        style.addProperty("style:vertical-pos","top");
    } else if (posV == 2) { // msophCenter
        style.addProperty("style:vertical-pos","middle");
    } else if (posV == 3) { // msophBottom
        style.addProperty("style:vertical-pos","bottom");
    } else if (posV == 4) { // msophInside - not possible to write it into odf
        style.addProperty("style:vertical-pos","top");
    } else if (posV == 5) { // msophOutside - not possible to write it into odf
        style.addProperty("style:vertical-pos","bottom");
    }

    // style:vertical-rel MS-ODRAW 2.3.4.22
    quint32 posRelV = ds.posRelV();
    if (posRelV == 0) {//msoprvMargin
        style.addProperty("style:vertical-rel","page-content");
    } else if (posRelV == 1) {//msoprvPage
        style.addProperty("style:vertical-rel","page");
    } else if (posRelV == 2) {//msoprvText
        style.addProperty("style:vertical-rel","paragraph");
    } else if (posRelV == 3) { //msoprvLine
        style.addProperty("style:vertical-rel","line");
    } else {
        style.addProperty("style:vertical-rel","page-content");
    }
}

void KWordGraphicsHandler::defineWrappingProperties(KoGenStyle& style, const DrawStyle& ds, const wvWare::Word97::FSPA* spa)
{
    //process the wrapping style, (MS-DOC, page 464)
    if (spa != 0) {
        bool check_wrk = false;
        if (spa->wr == 0) {
            //wrap around the object
            check_wrk = true;
        }
        else if (spa->wr == 1) {
            //top and bottom wrapping
            style.addProperty("style:wrap", "none");
        }
        else if (spa->wr == 2) {
            //square wrapping
            check_wrk = true;
        }
        else if (spa->wr == 3) {
            //in front or behind the text
            style.addProperty("style:wrap", "run-through");
	    //check if shape is behind the text
            if ((spa->fBelowText == 1) || (ds.fBehindDocument())) {
                style.addProperty("style:run-through", "background");
            } else {
                style.addProperty("style:run-through", "foreground");
            }
        }
        else if (spa->wr == 4) {
            //tight wrapping
            check_wrk = true;
            style.addProperty("style:wrap-contour", "true");
            style.addProperty("style:wrap-contour-mode", "outside");
        }
        else if (spa->wr == 5) {
            //through wrapping
            check_wrk = true;
            style.addProperty("style:wrap-contour", "true");
            style.addProperty("style:wrap-contour-mode", "full");
        }
        //check details of the text wrapping around this shape
        if (check_wrk) {
            if (spa->wrk == 0) {
                style.addProperty("style:wrap", "parallel");
            }
            else if (spa->wrk == 1) {
                style.addProperty("style:wrap", "left");
            }
            else if (spa->wrk == 2) {
                style.addProperty("style:wrap", "right");
            }
            else if (spa->wrk == 3) {
                style.addProperty("style:wrap", "biggest");
            }
        }
    }
    //no information from plcfSpa available 
    else {
        style.addProperty("style:wrap", "run-through");
        if (ds.fBehindDocument()) {
            style.addProperty("style:run-through", "background");
        } else {
            style.addProperty("style:run-through", "foreground");
        }
    }

    //margins are related to text wrapping, 
    style.addPropertyPt("style:margin-left", ds.dxWrapDistLeft()/12700.);
    style.addPropertyPt("style:margin-right", ds.dxWrapDistRight()/12700.);
    style.addPropertyPt("style:margin-top", ds.dyWrapDistTop()/12700.);
    style.addPropertyPt("style:margin-bottom", ds.dyWrapDistBottom()/12700.);

    // style:number-wrapped-paragraphs
    // style:wrap-dynamic-treshold
}

void KWordGraphicsHandler::SetAnchorTypeAttribute(DrawingWriter& out)
{
    if (out.m_inline) {
        out.xml.addAttribute("text:anchor-type","as-char");
    } else {
        out.xml.addAttribute("text:anchor-type","char");
    }
}

void KWordGraphicsHandler::SetZIndexAttribute(DrawingWriter& out)
{
    out.xml.addAttribute("draw:z-index",m_zIndex);
}

void KWordGraphicsHandler::parseTextBox(const MSO::OfficeArtSpContainer& o, DrawingWriter out)
{
    QString styleName;
    DrawStyle ds(m_OfficeArtDggContainer,&o);
    DrawStyle drawStyle(m_OfficeArtDggContainer,NULL,&o);
    wvWare::Word97::FSPA* spa = out.m_pSpa;
    KoGenStyle style(KoGenStyle::GraphicAutoStyle, "graphic");

    if (!out.m_bodyDrawing) {
        style.setAutoStyleInStylesDotXml(true);
    }
    defineGraphicProperties(style, ds);
    defineWrappingProperties(style, ds, spa);
    defineAnchorProperties(style, ds);
    styleName = out.styles.insert(style);

    out.xml.startElement("draw:frame");
    out.xml.addAttribute("draw:style-name", styleName);
    SetAnchorTypeAttribute(out);
    SetZIndexAttribute(out);

    switch(drawStyle.txflTextFlow()) {
    case 1: //msotxflTtoBA up-down
    case 3: //msotxflTtoBN up-down
    case 5: //msotxflVertN up-down
        out.xml.addAttribute("svg:width", mm(out.vLength()));
        out.xml.addAttribute("svg:height", mm(out.hLength()));
        out.xml.addAttribute("draw:transform","matrix(0 1 -1 0 " +
                mm(((Writer *)&out)->hOffset(out.xRight)) + " " + mm(out.vOffset()) + ")");
        break;
    case 2: //msotxflBtoT down-up
        out.xml.addAttribute("svg:width", mm(out.vLength()));
        out.xml.addAttribute("svg:height", mm(out.hLength()));
        out.xml.addAttribute("draw:transform","matrix(0 -1 1 0 " +
               mm(out.hOffset()) + " " + mm(((Writer *)&out)->vOffset(out.yBottom)) + ")");
         break;
    default : //standard text flow
        out.xml.addAttribute("svg:width", mm(out.hLength()));
        out.xml.addAttribute("svg:height", mm(out.vLength()));
        out.xml.addAttribute("svg:x", mm(out.hOffset()));
        out.xml.addAttribute("svg:y", mm(out.vOffset()));
    }

    out.xml.startElement("draw:text-box");

    emit textBoxFound(o.shapeProp.spid , out.m_bodyDrawing);

    out.xml.endElement(); //draw:text-box
    out.xml.endElement(); //draw:frame
}

void KWordGraphicsHandler::processRectangle(const MSO::OfficeArtSpContainer& o,DrawingWriter& out)
{
    QString styleName;
    DrawStyle ds(m_OfficeArtDggContainer,&o);
    wvWare::Word97::FSPA* spa = out.m_pSpa;
    KoGenStyle style(KoGenStyle::GraphicAutoStyle, "graphic");

    if (!out.m_bodyDrawing) {
        style.setAutoStyleInStylesDotXml(true);
    }
    defineGraphicProperties(style, ds);
    defineWrappingProperties(style, ds, spa);
    defineAnchorProperties(style, ds);
    styleName = out.styles.insert(style);

    out.xml.startElement("draw:frame");
    out.xml.addAttribute("draw:style-name", styleName);
    SetAnchorTypeAttribute(out);
    SetZIndexAttribute(out);
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.addAttribute("svg:width", mm(out.hLength()));
    out.xml.addAttribute("svg:height", mm(out.vLength()));
    out.xml.addAttribute("svg:x", mm(out.hOffset()));
    out.xml.addAttribute("svg:y", mm(out.vOffset()));

    out.xml.startElement("draw:text-box");
    out.xml.endElement(); //draw:text-box
    out.xml.endElement(); //draw:frame
}

void KWordGraphicsHandler::processInlinePictureFrame(const MSO::OfficeArtSpContainer& o, DrawingWriter& out)
{
    kDebug(30513) ;

    QString styleName;
    DrawStyle ds(NULL, &o);
    KoGenStyle style(KoGenStyle::GraphicAutoStyle, "graphic");

    //in case a header or footer is processed, save the style into styles.xml
    if (m_document->writingHeader()) {
        style.setAutoStyleInStylesDotXml(true);
    }
    defineGraphicProperties(style, ds);
    styleName = out.styles.insert(style);

    QString url;
    QString name = m_picNames.value(out.m_rgbUid);
    if (!name.isEmpty()) {
        url.append("Pictures/");
        url.append(name);
    }
    out.xml.startElement("draw:frame");
    if (url.isEmpty()) {
        // if the image cannot be found, just place an empty frame
        out.xml.endElement(); //draw:frame
        return;
    }
    out.xml.addAttribute("draw:style-name", styleName);
    out.xml.addAttribute("text:anchor-type","as-char");

    double hscale = out.m_picf->mx / 1000.0;
    double vscale = out.m_picf->my / 1000.0;

    out.xml.addAttributePt("svg:width", twipsToPt(out.m_picf->dxaGoal) * hscale);
    out.xml.addAttributePt("svg:height", twipsToPt(out.m_picf->dyaGoal) * vscale);

    //TODO: process border information (complex properties)

    out.xml.startElement("draw:image");
    out.xml.addAttribute("xlink:href", url);
    out.xml.addAttribute("xlink:type", "simple");
    out.xml.addAttribute("xlink:show", "embed");
    out.xml.addAttribute("xlink:actuate", "onLoad");
    out.xml.endElement(); //draw:image
    out.xml.endElement(); //draw:frame
    return;
}

void KWordGraphicsHandler::processFloatingPictureFrame(const MSO::OfficeArtSpContainer& o, DrawingWriter& out)
{
    kDebug(30513) ;

    QString styleName;
    DrawStyle ds(m_OfficeArtDggContainer, &o);
    wvWare::Word97::FSPA* spa = out.m_pSpa;
    KoGenStyle style(KoGenStyle::GraphicAutoStyle, "graphic");

    //in case a header or footer is processed, save the style into styles.xml
    if (m_document->writingHeader()) {
        style.setAutoStyleInStylesDotXml(true);
    }
    defineGraphicProperties(style, ds);
    defineWrappingProperties(style, ds, spa);
    defineAnchorProperties(style, ds);

    //ODF-1.2: specifies the number of paragraphs that can wrap around a frame
    //if wrap mode is in {left, right, parallel, dynamic} and anchor type is in
    //{char, paragraph}
    if (spa) {
        if ((spa->wr != 1) && (spa->wr != 3)) {
            style.addProperty("style:number-wrapped-paragraphs", "no-limit");
        }
    }
    styleName = out.styles.insert(style);

    QString url;
    if (ds.pib()) {
        url = getPicturePath(ds.pib());
    }
    out.xml.startElement("draw:frame");
    if (url.isEmpty()) {
        //if the image cannot be found, just place an empty frame
        out.xml.endElement(); //draw:frame
        return;
    }
    out.xml.addAttribute("draw:style-name", styleName);
    out.xml.addAttribute("text:anchor-type","char");
    SetZIndexAttribute(out);
    out.xml.addAttribute("svg:width", mm(out.hLength()));
    out.xml.addAttribute("svg:height", mm(out.vLength()));
    out.xml.addAttribute("svg:x", mm(out.hOffset()));
    out.xml.addAttribute("svg:y", mm(out.vOffset()));

    out.xml.startElement("draw:image");
    out.xml.addAttribute("xlink:href", url);
    out.xml.addAttribute("xlink:type", "simple");
    out.xml.addAttribute("xlink:show", "embed");
    out.xml.addAttribute("xlink:actuate", "onLoad");
    out.xml.endElement(); //draw:image

    //check for user edited wrap points
    if (ds.fEditedWrap()) {
        QString points;
        IMsoArray _v = ds.fillShadeColors_complex();
        if (_v.data.size()) {
            //_v.data is an array of POINTs, MS-ODRAW, page 89
            QByteArray a, a2;
            int* p;

            for (int i = 0, offset = 0; i < _v.nElems; i++, offset += _v.cbElem) {
                // x coordinate of this point
                a = _v.data.mid(offset, _v.cbElem);
                a2 = a.mid(0, _v.cbElem / 2);
                p = (int*) a2.data();
                points.append(QString::number(twipsToPt(*p)));
                points.append(",");
                // y coordinate of this point
                a2 = a.mid(_v.cbElem / 2, _v.cbElem / 2);
                p = (int*) a2.data();
                points.append(QString::number(twipsToPt(*p)));
                points.append(" ");
            }
            points.chop(1); //remove last space
        }
        out.xml.startElement("draw:contour-polygon");
        out.xml.addAttribute("draw:points", points);
        out.xml.endElement(); //draw:contour-polygon
    }
    out.xml.endElement(); //draw:frame
    return;
}

void KWordGraphicsHandler::processLineShape(const MSO::OfficeArtSpContainer& o, DrawingWriter& out)
{
    kDebug(30513) ;

    QString styleName;
    DrawStyle ds(NULL, &o);
    KoGenStyle style(KoGenStyle::GraphicAutoStyle, "graphic");

    //in case a header or footer is processed, save the style into styles.xml
    if (m_document->writingHeader()) {
        style.setAutoStyleInStylesDotXml(true);
    }
    defineGraphicProperties(style, ds);

    //NOTE: also the dxWidthHR propertie may store the width information
    float width = ds.pctHR() / 10.0;

    QString hrAlign;
    QString xPos = QString::number(0.0f).append("in");
    const float base_width = 6.1378;

    switch (ds.alignHR()) {
    case wvWare::hAlignLeft:
        hrAlign = QString("left");
        xPos = QString::number(0.0f).append("in");
        break;
    case wvWare::hAlignCenter:
        hrAlign = QString("center");
        xPos = QString::number((base_width / 2.0) - ((width * base_width) / 200.0)).append("in");
        break;
    case wvWare::hAlignRight:
        hrAlign = QString("right");
        xPos = QString::number(base_width - (width * base_width) / 100.0).append("in");
        break;
    }
    //process the content of HR specific properties
    style.addProperty("draw:textarea-horizontal-align", hrAlign);
    style.addProperty("draw:textarea-vertical-align", "top");
    if (ds.fNoshadeHR()) {
        style.addProperty("draw:shadow", "hidden");
    }
    else {
        style.addProperty("draw:shadow", "visible");
    }
    styleName = out.styles.insert(style);

    //create a custom shape
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", styleName);
    SetAnchorTypeAttribute(out);
    SetZIndexAttribute(out);

    QString height = QString::number(ds.dxHeightHR() / 1440.0f).append("in");
    out.xml.addAttribute("svg:height", height);

    QString width_str = QString::number(width * base_width / 100.0f).append("in");
    out.xml.addAttribute("svg:width", width_str);
    out.xml.addAttribute("svg:x", xPos);

    //--------------------
    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:type", "rectangle");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N");
    out.xml.endElement(); //enhanced-geometry
    out.xml.endElement(); //custom-shape
}

void KWordGraphicsHandler::parseFloatingPictures(void)
{
    kDebug(30513);

    // WordDocument stream equals the Delay stream (DOC)
    LEInputStream* in = m_document->wdocument_stream();

    const OfficeArtBStoreContainer* blipStore = m_OfficeArtDggContainer.blipStore.data();
    if (blipStore) {
        for (int i = 0; i < blipStore->rgfb.size(); i++) {
            OfficeArtBStoreContainerFileBlock block = blipStore->rgfb[i];

	    //we are looking for the missing content of OfficeArtFBSE
            if (block.anon.is<OfficeArtFBSE>()) {
                OfficeArtFBSE* fbse = block.anon.get<OfficeArtFBSE>();
                if (!fbse->embeddedBlip) {

                    //NOTE: An foDelay value of 0xffffffff specifies that the
                    //file is not in the delay stream and cRef must be zero.

                    //NOTE: A cRef value of 0x00000000 specifies an empty slot
                    //in the OfficeArtBStoreContainer.

                    if (fbse->foDelay != 0xffffffff) {
                        if (!fbse->cRef) {
                            kDebug(30513) << "Strange, no references to this BLIP, skipping";
                            continue;
                        }
                        LEInputStream::Mark _zero;
                        _zero = in->setMark();
                        in->skip(fbse->foDelay);

                        //let's check the record header if there's a BLIP stored
                        LEInputStream::Mark _m;
                        _m = in->setMark();
                        OfficeArtRecordHeader rh;
                        parseOfficeArtRecordHeader(*in, rh);
                        in->rewind(_m);
                        if ( !(rh.recType >= 0xF018 && rh.recType <= 0xF117) ) {
                            continue;
                        }
                        fbse->embeddedBlip = QSharedPointer<OfficeArtBlip>(new OfficeArtBlip(fbse));
                        parseOfficeArtBlip(*in, *(fbse->embeddedBlip.data()));
                        in->rewind(_zero);
                    }
                }
            } //else there's an OfficeArtBlip inside
        }
    }
    return;
}

QMap<QByteArray, QString>
KWordGraphicsHandler::createFloatingPictures(KoStore* store, KoXmlWriter* manifest)
{
    PictureReference ref;
    QMap<QByteArray, QString> fileNames;

    const OfficeArtBStoreContainer* blipStore = m_OfficeArtDggContainer.blipStore.data();
    if (blipStore) {
        store->enterDirectory("Pictures");
        foreach (const OfficeArtBStoreContainerFileBlock& block, blipStore->rgfb) {
            ref = savePicture(block, store);
            if (ref.name.length() == 0) {
                kDebug(30513) << "Note: Empty picture reference, probably an empty slot";
                continue;
	    }
            manifest->addManifestEntry("Pictures/" + ref.name, ref.mimetype);
            fileNames[ref.uid] = ref.name;
        }
        store->leaveDirectory();
    }
    return fileNames;
}

QString KWordGraphicsHandler::getPicturePath(int pib) const
{
    int n = pib - 1;
    QByteArray rgbUid;

    // return 16 byte rgbuid for this given blip id
    const OfficeArtBStoreContainer* blipStore = m_OfficeArtDggContainer.blipStore.data();
    if (blipStore) {
        if ((n < blipStore->rgfb.size()) &&
            blipStore->rgfb[n].anon.is<MSO::OfficeArtFBSE>())
        {
            rgbUid = blipStore->rgfb[n].anon.get<MSO::OfficeArtFBSE>()->rgbUid;
	}
        else {
            kDebug(30513) << "Could not find image for pib " << pib;
        }
    }
    return rgbUid.length() ? "Pictures/" + m_picNames[rgbUid] : "";
}


#include "graphicshandler.moc"
