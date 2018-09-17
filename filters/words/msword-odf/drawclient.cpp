/*
   Copyright (C) 2011 Matus Hanzes <matus.hanzes@ixonos.com>

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

#include "MsDocDebug.h"
#include "ODrawToOdf.h"
#include "pictures.h"
#include "graphicshandler.h"

#include <QColor>

//#define USE_OFFICEARTDGG_CONTAINER

using namespace MSO;
using namespace wvWare;

//FIXME: duplicity in PptToOdp.cpp and graphicshandler.cpp
namespace
{
QString format(double v) {
    static const QString f("%1");
    static const QString e("");
    static const QRegExp r("\\.?0+$");
    return f.arg(v, 0, 'f').replace(r, e);
}

QString mm(double v) {
    static const QString mm("mm");
    return format(v) + mm;
}
}

QRectF
WordsGraphicsHandler::DrawClient::getRect(const MSO::OfficeArtClientAnchor& ca)
{
    const DocOfficeArtClientAnchor* a = ca.anon.get<DocOfficeArtClientAnchor>();
    if (!a || (a->clientAnchor == -1)) {
        debugMsDoc << "INVALID DocOfficeArtClientAnchor, returning QRect(0, 0, 1, 1)";
        return QRect(0, 0, 1, 1);
    }
    const PLCF<Word97::FSPA>* plcfSpa = 0;
    if (gh->m_document->writingHeader()) {
        plcfSpa = gh->m_drawings->getSpaHdr();
    } else {
        plcfSpa = gh->m_drawings->getSpaMom();
    }
    if (!plcfSpa) {
        debugMsDoc << "MISSING plcfSpa, returning QRectF()";
        return QRectF();
    }

    PLCFIterator<Word97::FSPA> it(plcfSpa->at(a->clientAnchor));
    Word97::FSPA* spa = it.current();
    if (!spa) {
        return QRectF();
    }
    return QRectF(spa->xaLeft, spa->yaTop, (spa->xaRight - spa->xaLeft), (spa->yaBottom - spa->yaTop));
}

QRectF
WordsGraphicsHandler::DrawClient::getReserveRect(void)
{
    //At least for floating MS-ODRAW shapes the SPA structure for the current
    //CP is provided by the GraphicsHandler.  No test files for inline shapes
    //at the moment.
    Word97::FSPA* spa = gh->m_pSpa;
    if (!spa) {
        return QRectF();
    }
    return QRectF(spa->xaLeft, spa->yaTop, (spa->xaRight - spa->xaLeft), (spa->yaBottom - spa->yaTop));
}

QString
WordsGraphicsHandler::DrawClient::getPicturePath(const quint32 pib)
{
    return gh->getPicturePath(pib);
}

void
WordsGraphicsHandler::DrawClient::processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                                                       const MSO::OfficeArtClientData* cd,
                                                       Writer& out)
{
    Q_UNUSED(cd);
    Q_UNUSED(out);
    const DocOfficeArtClientTextBox* tb = ct.anon.get<DocOfficeArtClientTextBox>();
    if (!tb) {
        debugMsDoc << "DocOfficeArtClientTextBox missing!";
        return;
    }
    //NOTE: Dividing the high 2 bytes by 0x10000 specifies a 1-based index into
    //PlcfTxbxTxt of the FTXBXS structure where the text for this textbox is
    //located.  The low 2 bytes specify the zero-based index in the textbox
    //chain that the textbox occupies.  [MS-DOC] — v20101219

    uint index = (tb->clientTextBox / 0x10000) - 1;
    gh->emitTextBoxFound(index, out.stylesxml);
}

KoGenStyle
WordsGraphicsHandler::DrawClient::createGraphicStyle(const MSO::OfficeArtClientTextBox* ct,
                                                     const MSO::OfficeArtClientData* cd,
                                                     const DrawStyle& ds,
                                                     Writer& out)
{
    Q_UNUSED(ct);
    Q_UNUSED(cd);
    KoGenStyle style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    style.setAutoStyleInStylesDotXml(out.stylesxml);

    // Set specific attributes of graphic-properties.
    gh->definePositionAttributes(style, ds);
    gh->defineWrappingAttributes(style, ds);
    return style;
}

void
WordsGraphicsHandler::DrawClient::addTextStyles(const MSO::OfficeArtClientTextBox* clientTextbox,
                                                const MSO::OfficeArtClientData* clientData,
                                                KoGenStyle& style,
                                                Writer& out)
{
    Q_UNUSED(clientTextbox);
    Q_UNUSED(clientData);

    //TODO: Add paragraph-properties and text-properties if required!

    const QString styleName = out.styles.insert(style);
    out.xml.addAttribute("draw:style-name", styleName);

    // Set additional attributes of the element required for layout.
    gh->setAnchorTypeAttribute(*static_cast<DrawingWriter*>(&out));
    gh->setZIndexAttribute(*static_cast<DrawingWriter*>(&out));
}

const MSO::OfficeArtDggContainer*
WordsGraphicsHandler::DrawClient::getOfficeArtDggContainer(void)
{
#ifdef USE_OFFICEARTDGG_CONTAINER
    return &gh->m_officeArtDggContainer;
#else
    return 0;
#endif
}

const MSO::OfficeArtSpContainer*
WordsGraphicsHandler::DrawClient::getMasterShapeContainer(quint32 spid)
{
    //TODO: No support for master shapes at the moment.
    Q_UNUSED(spid);
    return 0;
}

QColor
WordsGraphicsHandler::DrawClient::toQColor(const MSO::OfficeArtCOLORREF& c)
{
    return QColor(c.red, c.green, c.blue);
}

QString
WordsGraphicsHandler::DrawClient::formatPos(qreal v)
{
    //assuming the client uses the DrawingWriter class
    return mm(v);

    //assuming the client uses the Writer class
//     return mm(twipsToMM(v));
}

//NOTE: OfficeArtClientData.clientdata (4 bytes): An integer that SHOULD be
//ignored.  [MS-DOC] — v20100926
bool
WordsGraphicsHandler::DrawClient::processRectangleAsTextBox(const MSO::OfficeArtClientData& cd)
{
    Q_UNUSED(cd);
    return false;
}

bool
WordsGraphicsHandler::DrawClient::onlyClientData(const MSO::OfficeArtClientData& o)
{
    Q_UNUSED(o);
    return false;
}

void
WordsGraphicsHandler::DrawClient::processClientData(const MSO::OfficeArtClientTextBox* ct,
                                                    const MSO::OfficeArtClientData& o,
                                                    Writer& out)
{
    Q_UNUSED(ct);
    Q_UNUSED(o);
    Q_UNUSED(out);
}
