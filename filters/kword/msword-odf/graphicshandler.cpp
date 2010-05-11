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

#include <wv2/src/olestream.h>
#include <wv2/src/ms_odraw.h>
#include "generated/leinputstream.h"
#include "drawstyle.h"
#include "ODrawToOdf.h"

#include <KoStoreDevice.h>
#include <KoGenStyle.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <QtGui/QColor>


using namespace wvWare;
using namespace MSO;

#define IMG_BUF_SIZE 2048L


KWordPictureHandler::KWordPictureHandler(Document* doc, KoXmlWriter* bodyWriter,
        KoXmlWriter* manifestWriter, KoStore* store, KoGenStyles* mainStyles)
        : QObject(), m_doc(doc), m_pictureCount(0), m_officeArtCount(0)
{
    kDebug(30513) ;
    m_bodyWriter = bodyWriter;
    m_manifestWriter = manifestWriter;
    m_store = store;
    m_mainStyles = mainStyles;
}

void KWordPictureHandler::setBodyWriter(KoXmlWriter* writer)
{
    m_bodyWriter = writer;
}

#ifdef IMAGE_IMPORT

void KWordPictureHandler::bitmapData(OLEImageReader& reader, SharedPtr<const Word97::PICF> /*picf*/)
{
    kDebug(30513) << "Bitmap data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size();

}

void KWordPictureHandler::escherData(OLEImageReader& reader, SharedPtr<const Word97::PICF> picf, int type, wvWare::U32 pib)
{
    kDebug(30513) << "Escher data found";

    QString picName;
    ODTProcessing(&picName, picf, type, pib);

    if (m_pictureName.contains(pib)) {
        //image data already loaded once
        return;
    }
    else {
        //insert the picture name into hash table
        m_pictureName.insert(pib, picName);

        //write picture data to file
        m_store->open(picName);//open picture file

        long len = reader.size();
        while (len > 0)  {
            kDebug(30513) << "len = " << len;
            wvWare::U8* buf = new wvWare::U8[IMG_BUF_SIZE];
            size_t n = reader.read(buf, qMin(len, IMG_BUF_SIZE));
            long n1 = m_store->write((const char*)buf, n);
            kDebug(30513) << "n=" << n << ", n1=" << n1 << "; buf contains " << (void*) buf;
            len -= n;
            delete [] buf;
            //error checking
            if ((n == 0 && len != 0) ||  //endless loop
                (size_t)n1 != n) { //read/wrote different lengths
                 m_store->close(); //close picture file before returning
                 return; //ouch - we're in an endless loop!
            }
            //Q_ASSERT( (size_t)n1 == n );
        }
        Q_ASSERT(len == 0);
        m_store->close(); //close picture file
    }
}

//use this version when the data had to be decompressed
//so we don't have to convert the data back to an OLEImageReader
void KWordPictureHandler::escherData(std::vector<wvWare::U8> data, SharedPtr<const Word97::PICF> picf, int type, wvWare::U32 pib)
{
    kDebug(30513) << "Escher data found";

    QString picName;
    ODTProcessing(&picName, picf, type, pib);

    if (m_pictureName.contains(pib)) {
        //image data already loaded once
        return;
    }
    else {
        //insert the picture name into hash table
        m_pictureName.insert(pib, picName);

        //write picture data to file
        m_store->open(picName);//open picture file

        long len = data.size();
        int index = 0; //index for reading from vector
        while (len > 0)  {
            kDebug(30513) << "len = " << len;
            wvWare::U8* buf = new wvWare::U8[IMG_BUF_SIZE];
            //instead of a read command, we'll copy that number of bytes
            //from the vector into the buffer
            int n = qMin(len, IMG_BUF_SIZE);
            for (int i = 0; i < n; i++) {
                buf[i] = data[index];
                index++;
            }
            //size_t n = reader.read( buf, qMin( len, IMG_BUF_SIZE ) );
            long n1 = m_store->write((const char*)buf, n);
            kDebug(30513) << "n=" << n << ", n1=" << n1 << "; buf contains " << (void*) buf;
            len -= n;
            delete [] buf;
            //error checking
            if ((n == 0 && len != 0) ||  //endless loop
                (size_t)n1 != n) { //read/wrote different lengths
                m_store->close(); //close picture file before returning
                return; //ouch - we're in an endless loop!
            }
            //Q_ASSERT( (size_t)n1 == n );
          }
        Q_ASSERT(len == 0);
        m_store->close(); //close picture file
    }
}

void KWordPictureHandler::officeArt(wvWare::OfficeArtProperties *artProperties)
{
    if (artProperties->shapeType == msosptLine) {
        officeArtLine(artProperties);
    }
}

#endif // IMAGE_IMPORT

void KWordPictureHandler::officeArtLine(wvWare::OfficeArtProperties *artProperties)
{
    //TODO: properties like horizontal align should be applied to the picture,
    //but it has anchor type "as-char", let's check how to implement this.

    //Check if a picture is comming, this will be displayed instead of a line.
    if (artProperties->pib) {
        return;
    }

    QString hrAlign;
    QString xPos = QString::number(0.0f).append("in");

    switch (artProperties->align) {
        case wvWare::hAlignLeft:
                hrAlign = QString("left");
                xPos = QString::number(0.0f).append("in");
                break;
        case wvWare::hAlignCenter:
                hrAlign = QString("center");
                xPos = QString::number((6.1378f/2.0f) - ((artProperties->width * 6.1378f) / 200.0f)).append("in");
                break;
        case wvWare::hAlignRight:
                hrAlign = QString("right");
                xPos = QString::number(6.1378f - (artProperties->width * 6.1378f) / 100.0f).append("in");
                break;
    }
    m_officeArtCount++;

    // create a graphic style
    QString styleName("gr");
    styleName.append(QString::number(m_officeArtCount));
    KoGenStyle *style = new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic", "Graphics");

    //in case a header or footer is processed, save the style into styles.xml
    if (m_doc->writingHeader()) {
        style->setAutoStyleInStylesDotXml(true);
    }

    QString colorStr = QString("#%1%2%3").arg((int)artProperties->color.r, 2, 16, QChar('0')).arg((int)artProperties->color.g, 2, 16, QChar('0')).arg((int)artProperties->color.b, 2, 16, QChar('0'));
    style->addProperty("draw:fill","solid");
    style->addProperty("draw:fill-color", colorStr);
    style->addProperty("draw:textarea-horizontal-align",hrAlign);
    style->addProperty("draw:textarea-vertical-align","top");
    style->addProperty("draw:shadow","hidden");
    style->addProperty("style:run-through","foreground");

    styleName = m_mainStyles->insert(*style, styleName, KoGenStyles::DontAddNumberToName);    

    delete style;
    //--------------------
    // create a custom shape
    m_bodyWriter->startElement("draw:custom-shape");
    m_bodyWriter->addAttribute("text:anchor-type", "as-char");

    QString heightStr = QString::number(artProperties->height).append("in");
    m_bodyWriter->addAttribute("svg:height", heightStr);

    QString widthStr = QString::number((artProperties->width * 6.1378f) / 100.0f).append("in");
    m_bodyWriter->addAttribute("svg:width", widthStr);
    m_bodyWriter->addAttribute("svg:x", xPos);
    m_bodyWriter->addAttribute("draw:style-name", styleName.toUtf8());

    //--------------------
    m_bodyWriter->startElement("draw:enhanced-geometry");
    m_bodyWriter->addAttribute("svg:viewBox", "0 0 21600 21600");
    m_bodyWriter->addAttribute("draw:type", "rectangle");
    m_bodyWriter->addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N");
    m_bodyWriter->endElement();
    //--------------------
    m_bodyWriter->endElement();			// end draw:custom-shape
}

void KWordPictureHandler::ODTProcessing(QString* picName, SharedPtr<const Word97::PICF> picf, int type, wvWare::U32 pib)
  {
    //check if the referred pib is already in hash table
    //NOTE: the wmfData function has no pib to pass
    if (m_pictureName.contains(pib)) {
        picName->append(m_pictureName.value(pib));
    }
    else {
        //set up filename
        picName->append("Pictures/");
        picName->append(QString::number(m_pictureCount));
        m_pictureCount++;
        //the type coming in corresponds to MSOBLIPTYPE see wv2/src/graphics.h
        if (type == 5)
            picName->append(".jpg");
        else if (type == 6)
            picName->append(".png");
        else if (type == 3)
            picName->append(".wmf");
        else if (type == 2)
            picName->append(".emf");
        else {
            kWarning() << "Unhandled file type (" << type << ") - pictures won't be displayed.";
            return;
        }
        //add entry in manifest file
        QString mimetype(KMimeType::findByPath(*picName, 0, true)->name());
        m_manifestWriter->addManifestEntry(*picName, mimetype);
    }
    //create style
    QString styleName("fr");
    styleName.append(QString::number(m_pictureCount));
    KoGenStyle* style = new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic", "Graphics");

    //in case a header or footer is processed, save the style into styles.xml
    if (m_doc->writingHeader()) {
        style->setAutoStyleInStylesDotXml(true);
    }

    styleName = m_mainStyles->insert(*style, styleName, KoGenStyles::DontAddNumberToName);
    delete style;

    //start frame tag for the picture
    m_bodyWriter->startElement("draw:frame");
    m_bodyWriter->addAttribute("draw:style-name", styleName.toUtf8());
    m_bodyWriter->addAttribute("text:anchor-type", "as-char");
    //mx, my = horizontal & vertical user scaling in .001 %
    double horiz_scale = picf->mx / 1000.0;
    double vert_scale = picf->my / 1000.0;
    double height = ((double) picf->dyaGoal * vert_scale) / 20.0; //twips -> pt
    double width = ((double) picf->dxaGoal * horiz_scale) / 20.0; //twips -> pt
    m_bodyWriter->addAttributePt("svg:height", height);
    m_bodyWriter->addAttributePt("svg:width", width);
    //start the actual image tag
    m_bodyWriter->startElement("draw:image");
    m_bodyWriter->addAttribute("xlink:href", *picName);
    m_bodyWriter->addAttribute("xlink:type", "simple");
    m_bodyWriter->addAttribute("xlink:show", "embed");
    m_bodyWriter->addAttribute("xlink:actuate", "onLoad");
    m_bodyWriter->endElement();//draw:image
    m_bodyWriter->endElement();//draw:frame
}

void KWordPictureHandler::wmfData(OLEImageReader& reader, SharedPtr<const Word97::PICF> picf)
{
    wvWare::U8* buf = new wvWare::U8[IMG_BUF_SIZE];

    kDebug(30513) << "WMF data found. Size=" << reader.size();

    QString picName;

    // Read the first bytes of the picture.  We need this to determine
    // if the contents is a WMF or an EMF.
    long len = reader.size();
    size_t  n = reader.read(buf, qMin(len, IMG_BUF_SIZE));

    // Pass 2 for emf image and 3 for wmf image
    // An EMF has the string " EMF" at the start + offset 40.
    if (len > 44 && buf[40] == ' ' && buf[41] == 'E' && buf[42] == 'M' && buf[43] == 'F') {
        kDebug(30513) << "Found an EMF file";
        ODTProcessing(&picName, picf, 2, 0);
    }
    else {
        kDebug(30513) << "Found a WMF file";
        ODTProcessing(&picName, picf, 3, 0);
    }

    // Write picture data to file.
    m_store->open(picName);//open picture file
    while (len > 0)  {
        kDebug(30513) << "len = " << len;

        // Write the bytes that we have in the buffer.
        ulong    n1 = m_store->write((const char*)buf, n);
        kDebug(30513) << "n=" << n << ", n1=" << n1 << "; buf contains " << (void*) buf;
        len -= n;

        // Break if something went wrong with the writing.
        if (n1 != n)
            break;

        if (len == 0)
            break;

        // Read the next batch.
        size_t  n = reader.read(buf, qMin(len, IMG_BUF_SIZE));

        //error checking
        if (n == 0 && len != 0)
            break;
    }

    delete [] buf;

    m_store->close(); //close picture file
}

void KWordPictureHandler::externalImage(const UString& name, SharedPtr<const Word97::PICF> picf)
{
    kDebug(30513);
}

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
}

DrawingWriter::DrawingWriter(KoXmlWriter& xmlWriter, KoGenStyles& kostyles, bool stylesxml_)
        : Writer(xmlWriter,kostyles,stylesxml_),
        xLeft(0),
        xRight(0),
        yTop(0),
        yBottom(0),
        m_pSpa(0),
        m_bodyDrawing(true)
{
    scaleX = 25.4 / 1440;
    scaleY = 25.4 / 1440;
}

QString DrawingWriter::vLength()
{
    return Writer::vLength(yBottom - yTop);
}

QString DrawingWriter::hLength()
{
    return Writer::hLength(xRight - xLeft);
}

QString DrawingWriter::vOffset()
{
    return Writer::vOffset(yTop);
}

QString DrawingWriter::hOffset()
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
    if(fspgr.xRight == fspgr.xLeft) {
        return;
    }

    if(fspgr.yBottom == fspgr.yTop)
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

KWordDrawingHandler::KWordDrawingHandler(Document* doc, KoGenStyles* mainStyles, KoXmlWriter* bodyWriter): QObject()
, m_doc(doc)
, m_mainStyles(mainStyles)
, m_bodyWriter(bodyWriter)
, m_drawings(0)
, m_fib(0)
, m_pOfficeArtHeaderDgContainer(0)
, m_pOfficeArtBodyDgContainer(0)
{

}

KWordDrawingHandler::~KWordDrawingHandler()
{
    delete m_pOfficeArtHeaderDgContainer;
    delete m_pOfficeArtBodyDgContainer;
}

void KWordDrawingHandler::init(Drawings * pDrawings, wvWare::OLEStreamReader* table,const  wvWare::Word97::FIB &fib)
{
    kDebug(30513);

    parseOfficeArtContainer(table,fib);

//     create default draw style
//    KoGenStyle style(KoGenStyle::GraphicStyle, "graphic");
//    style.setDefaultStyle(true);
//
//    defineDefaultGraphicProperties(&style,pDrawings);
//
//    //add default draw style to styles
//    m_mainStyles->insert(style, "");

    m_drawings = pDrawings;
    m_fib = const_cast<wvWare::Word97::FIB *>(&fib);
}

void KWordDrawingHandler::drawingData(unsigned int globalCP)
{
    kDebug(30513) << "globalCP" << globalCP ;
    // draw shape or group of shapes
    if(m_drawings == NULL) {
        return;
    }

    PLCF<Word97::FSPA>* fspa = m_drawings->getSpaMom();
    if (fspa != 0) {
        PLCFIterator<Word97::FSPA> it(fspa->at(0));

        //search for drawing in main body
        for(size_t i = 0; i < fspa->count(); i++, ++it) {
            kDebug(30513) << "FSPA start:" << it.currentStart();
            kDebug(30513) << "FSPA spid:" << it.current()->spid;

            if(it.currentStart() == globalCP) {
                DrawingWriter out(*m_bodyWriter,*m_mainStyles,true);
                out.m_pSpa = it.current();
                out.m_bodyDrawing = true;
                drawObject((uint) it.current()->spid, m_pOfficeArtBodyDgContainer, out,it.current());
                return;
            }
        }
    }

    fspa = m_drawings->getSpaHdr();
    if (fspa != 0) {
        PLCFIterator<Word97::FSPA> itHeader(fspa->at(0));
        //search for drawing in header
        for(size_t i = 0; i < fspa->count(); i++, ++itHeader) {
            kDebug(30513) << "FSPA start:" << itHeader.currentStart() + m_fib->ccpText + m_fib->ccpFtn;
            kDebug(30513) << "FSPA spid:" << itHeader.current()->spid;

            if((itHeader.currentStart() + m_fib->ccpText + m_fib->ccpFtn)  == globalCP) {
                DrawingWriter out(*m_bodyWriter,*m_mainStyles,true);
                out.m_pSpa = itHeader.current();
                out.m_bodyDrawing = false;
                drawObject((uint) itHeader.current()->spid, m_pOfficeArtHeaderDgContainer, out,itHeader.current());
                return;
            }
        }
    }
}

void KWordDrawingHandler::setBodyWriter(KoXmlWriter* writer)
{
    m_bodyWriter = writer;
}

DrawStyle KWordDrawingHandler::getDrawingStyle()
{
    if(m_pOfficeArtBodyDgContainer != NULL) {
        if(m_pOfficeArtBodyDgContainer->shape.isNull() == false) {
            if((*m_pOfficeArtBodyDgContainer->shape).shapePrimaryOptions.isNull() == false ) {
                return DrawStyle(m_OfficeArtDggContainer,NULL,m_pOfficeArtBodyDgContainer->shape.data());
                }
            }
        }
    return DrawStyle(m_OfficeArtDggContainer);
}

void KWordDrawingHandler::drawObject(uint spid, MSO::OfficeArtDgContainer * dg, DrawingWriter& out
        , wvWare::Word97::FSPA* spa)
{
    if(dg == NULL)
        return;

    foreach(const OfficeArtSpgrContainerFileBlock& co, dg->groupShape.rgfb) {
        //if spgr is in root, find out if his first item is sp with right spid
        if (co.anon.is<OfficeArtSpgrContainer>()) {
            const OfficeArtSpContainer* first =
                    (*co.anon.get<OfficeArtSpgrContainer>()).rgfb[0].anon.get<OfficeArtSpContainer>();

            if (first && first->shapeProp.spid == spid) {
                out.SetRectangle(*spa);
                processObjectForBody(*co.anon.get<OfficeArtSpgrContainer>(), out); //draw group
                break;
            }
        }
        else //if sp is in root, find out if it has the right spid
        {
            const OfficeArtSpContainer & spCo = *co.anon.get<OfficeArtSpContainer>();

            if (spCo.shapeProp.spid == spid) {
                out.SetRectangle(*spa);
                processObjectForBody(spCo, out); //draw object
                break;
            }
        }
    }
}

void KWordDrawingHandler::processObjectForBody(const MSO::OfficeArtSpgrContainer& o, DrawingWriter& out)
{
    if (o.rgfb.size() < 2) return;

    out.xml.startElement("draw:g");

    const OfficeArtSpContainer *first = o.rgfb[0].anon.get<OfficeArtSpContainer>();
    if(first && first->shapeGroup) {
        out.SetGroupRectangle(*first->shapeGroup); //set group rectangle
    }

    for (int i = 1; i < o.rgfb.size(); ++i) {
        if(o.rgfb[i].anon.is<OfficeArtSpContainer>()) {

            OfficeArtSpContainer tempSp = *o.rgfb[i].anon.get<OfficeArtSpContainer>();

            if(tempSp.childAnchor) {
                out.SetClientRectangle(*tempSp.childAnchor); //set child rectangle
            }

            processObjectForBody(tempSp, out); //draw objects
        }
    }

    out.xml.endElement(); // draw:g
}

void KWordDrawingHandler::processObjectForBody(const MSO::OfficeArtSpContainer& o, DrawingWriter out)
{
    kDebug(30513);

    // textbox can be msosptTextBox or msosptRectangle or ...
    if(!o.clientTextbox.isNull()) {
        kDebug(30513)<< "processing text box";
        parseTextBox(o, out);
        return;
    }

    switch (o.shapeProp.rh.recInstance)
    {
        case msosptRectangle: {
            kDebug(30513)<< "processing rectangle";
            processRectangle(o, out);
            break;
        }
        case msosptEllipse:
            kDebug(30513)<< "processing ellipse";
            break;
        case msosptPictureFrame:
            kDebug(30513)<< "processing picture frame";
            processPictureFrame(o, out);
            break;
        case msosptHostControl:
            kDebug(30513)<< "processing host control";
            parseTextBox(o, out);
            break;
    }
}

void KWordDrawingHandler::parseOfficeArtContainer(wvWare::OLEStreamReader* table,
        const  wvWare::Word97::FIB &fib)
{
    kDebug(30513);
    // get OfficeArtContent
    if(fib.lcbDggInfo != 0) {
        QBuffer buffer;
        QByteArray array;

        // copy of OfficeArtContent data into temp buffer
        array.resize(fib.lcbDggInfo);
        table->seek( fib.fcDggInfo );
        table->read( (U8 *)array.data(), fib.lcbDggInfo );

        buffer.setData(array);
        buffer.open(QIODevice::ReadOnly);

        // creation of streem needed by simpleParser.h
        LEInputStream stream(&buffer);

        //parse OfficeArfDggContainer from msdoc
        try {
            parseOfficeArtDggContainer(stream, m_OfficeArtDggContainer);
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing parseOfficeArtDggContainer ";
            return;
        }
        catch (...) {
            kDebug(30513) << "caught unknown exception while parsing parseOfficeArtDggContainer";
            return;
        }
        kDebug(30513) << "OfficeArtDggContainer parsed successful " ;


        // parse drawingsVariable from msdoc
        // 0 - next OfficeArtDgContainer belongs to Main document;
        // 1 - next OfficeArtDgContainer belongs to Header Document
        unsigned char drawingsVariable = 0;
        try {
            drawingsVariable = stream.readuint8();
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
        OfficeArtDgContainer * pDgContainer = NULL;
        try {
            pDgContainer = new OfficeArtDgContainer();
            if(drawingsVariable == 0) {
                m_pOfficeArtBodyDgContainer = pDgContainer;
            } else {
                m_pOfficeArtHeaderDgContainer = pDgContainer;
            }
            parseOfficeArtDgContainer(stream, *pDgContainer);
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
            drawingsVariable = stream.readuint8();
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing second drawingsVariable ";
            //wvlog << "stream position: " << stream.getPosition() << std::endl;
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
            if(drawingsVariable == 0) {
                if(m_pOfficeArtBodyDgContainer!= NULL){
                    delete m_pOfficeArtBodyDgContainer;
                }
                m_pOfficeArtBodyDgContainer = pDgContainer;
            }
            else
            {
                if(m_pOfficeArtHeaderDgContainer != NULL) {
                    delete m_pOfficeArtHeaderDgContainer;
                }
                m_pOfficeArtHeaderDgContainer = pDgContainer;
            }
            parseOfficeArtDgContainer(stream, *pDgContainer);
        }
        catch (IOException e) {
            kDebug(30513) << "caught IOException while parsing second OfficeArtDgContainer ";
            return;
        }
        catch (...) {
            kDebug(30513) << "caught unknown exception while parsing second OfficeArtDgContainer";
            return;
        }

        if (stream.getPosition() != buffer.size()) {
            kDebug(30513) << (uint)(buffer.size() - stream.getPosition())
            << "bytes left at the end of parseOfficeArtDggContainer,"
            << " parseOfficeArtDgContainer, so probably an error at position "
            << (uint)stream.getMaxPosition();
        }
    }
}

void KWordDrawingHandler::defineGraphicProperties(KoGenStyle& style, const DrawStyle& ds,
                          wvWare::Word97::FSPA* spa, const QString& listStyle)
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
    // draw:fill ("bitmap", "gradient", "hatch", "none" or "solid")
    qint32 fillType = ds.fillType();
    if (ds.fFilled()) {
        style.addProperty("draw:fill", getFillType(fillType), gt);
    } else {
        style.addProperty("draw:fill", "none", gt);
    }
    // draw:fill-color
    // only set the color if the fill type is 'solid' because OOo ignores
    // fill='none' if the color is set
    if (fillType == 0) {
        clr = ds.fillColor();
        QColor fillColor(clr.red,clr.green,clr.blue);
        style.addProperty("draw:fill-color", fillColor.name(), gt);
    }
    // draw:fill-gradient-name
    // draw:fill-hatch-name
    // draw:fill-hatch-solid
    // draw:fill-image-height
    // draw:fill-image-name
    // draw:fill-image-ref-point
    // draw:fill-image-ref-point-x
    // draw:fill-image-ref-point-y
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
    // draw:shadow
    // draw:shadow-color
    // draw:shadow-offset-x
    style.addProperty("draw:shadow-offset-x", pt(ds.shadowOffsetX()/12700.),gt);
    // draw:shadow-offset-y
    style.addProperty("draw:shadow-offset-y", pt(ds.shadowOffsetY()/12700.),gt);
    // draw:shadow-opacity
    float shadowOpacity = toQReal(ds.shadowOpacity());
    style.addProperty("draw:shadow-opacity", percent(100*shadowOpacity), gt);
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
    // fo:margin
    // fo:margin-bottom
    // fo:margin-left
    // fo:margin-right
    // fo:margin-top
    // fo:max-height
    // fo:max-width
    // fo:min-height
    // fo:min-width
    // fo:padding
    // fo:padding-bottom
    // fo:padding-left
    // fo:padding-right
    // fo:padding-top
    // fo:wrap-option
    // style:border-line-width
    // style:border-line-width-bottom
    // style:border-line-width-left
    // style:border-line-width-right
    // style:border-line-width-top
    // style:editable
    // style:flow-with-text
     // style:mirror
    // style:number-wrapped-paragraphs
    // style:overflow-behavior
    // style:print-content
    // style:protect
    // style:rel-height
    // style:rel-width
    // style:repeat
    // style:run-through
    // style:shadow
    // style:wrap
    // style:wrap-contour
    // style:wrap-contour-mode
    // style:wrap-dynamic-treshold
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

void KWordDrawingHandler::defineAnchorProperties(KoGenStyle& style, const DrawStyle& ds)
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

void KWordDrawingHandler::parseTextBox(const MSO::OfficeArtSpContainer& o, DrawingWriter out)
{
    out.xml.startElement("draw:frame");

    DrawStyle drawStyle(m_OfficeArtDggContainer,NULL,&o);

    switch(drawStyle.txflTextFlow()) {
    case 1: //msotxflTtoBA up-down
    case 3: //msotxflTtoBN up-down
    case 5: //msotxflVertN up-down
        out.xml.addAttribute("svg:width", out.vLength());
        out.xml.addAttribute("svg:height",out.hLength());
        out.xml.addAttribute("draw:transform","matrix(0 1 -1 0 " +
                ((Writer *)&out)->hOffset(out.xRight) + " " + out.vOffset() + ")");
        break;
    case 2: //msotxflBtoT down-up
        out.xml.addAttribute("svg:width", out.vLength());
        out.xml.addAttribute("svg:height",out.hLength());
        out.xml.addAttribute("draw:transform","matrix(0 -1 1 0 " +
               out.hOffset() + " " + ((Writer *)&out)->vOffset(out.yBottom) + ")");
         break;
    default : //standard text flow
        out.xml.addAttribute("svg:width", out.hLength());
        out.xml.addAttribute("svg:height", out.vLength());
        out.xml.addAttribute("svg:x", out.hOffset());
        out.xml.addAttribute("svg:y", out.vOffset());
    }

    out.xml.startElement("draw:text-box");

    emit textBoxFound(o.shapeProp.spid , &out.xml);

    out.xml.endElement(); //draw:text-box
    out.xml.endElement(); // draw:frame
}

void KWordDrawingHandler::processRectangle(const MSO::OfficeArtSpContainer& o,DrawingWriter& out)
{
    QString styleName;
    DrawStyle ds(m_OfficeArtDggContainer,&o);
    if (out.m_bodyDrawing) {
        KoGenStyle style(KoGenStyle::GraphicAutoStyle, "graphic");
        defineGraphicProperties(style, ds, out.m_pSpa);
        defineAnchorProperties(style, ds);
        styleName = out.styles.insert(style);
    }
    else {
        KoGenStyle style(KoGenStyle::GraphicStyle, "graphic");
        defineGraphicProperties(style, ds, out.m_pSpa);
        defineAnchorProperties(style, ds);
        styleName = out.styles.insert(style);
    }

    out.xml.startElement("draw:frame");
    out.xml.addAttribute("draw:style-name", styleName);
    out.xml.addAttribute("text:anchor-type","char");
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.addAttribute("svg:width", out.hLength());
    out.xml.addAttribute("svg:height", out.vLength());
    out.xml.addAttribute("svg:x", out.hOffset());
    out.xml.addAttribute("svg:y", out.vOffset());

    out.xml.startElement("draw:text-box");
    out.xml.endElement(); // draw:text-box
    out.xml.endElement(); // draw:frame
}

void KWordDrawingHandler::processPictureFrame(const MSO::OfficeArtSpContainer&/* o*/,DrawingWriter&/* out*/)
{
}

#include "graphicshandler.moc"
