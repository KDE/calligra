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
#include <kdebug.h>

#include "ODrawToOdf.h"
#include "pictures.h"
#include "graphicshandler.h"

#include <QColor>

#define USE_OFFICEARTDGG_CONTAINER

using namespace MSO;
using namespace wvWare;

//FIXME: duplicit in PptToOdp.cpp and graphicshandler.cpp
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
KWordGraphicsHandler::DrawClient::getRect(const MSO::OfficeArtClientAnchor& ca)
{
    const DocOfficeArtClientAnchor* a = ca.anon.get<DocOfficeArtClientAnchor>();
    if (!a || (a->clientAnchor == -1)) {
        kDebug(30513) << "INVALID DocOfficeArtClientAnchor, returning QRect(0, 0, 1, 1)";
        return QRect(0, 0, 1, 1);
    }
    const PLCF<Word97::FSPA>* plcfSpa = 0;
    if (gh->m_document->writingHeader()) {
        plcfSpa = gh->m_drawings->getSpaHdr();
    } else {
        plcfSpa = gh->m_drawings->getSpaMom();
    }
    if (!plcfSpa) {
        kDebug(30513) << "MISSING plcfSpa, returning QRect(0, 0, 1, 1)";
        return QRect(0, 0, 1, 1);
    }
    PLCFIterator<Word97::FSPA> it(plcfSpa->at(a->clientAnchor));
    Word97::FSPA* spa = it.current();
    return QRectF(spa->xaLeft, spa->yaTop, (spa->xaRight - spa->xaLeft), (spa->yaBottom - spa->yaTop));
}

QString
KWordGraphicsHandler::DrawClient::getPicturePath(int pib)
{
    return gh->getPicturePath(pib);
}

//TODO: Implementation required, but at the moment textboxes are parsed and processed in
//the graphicshandler, so no hurry.
void
KWordGraphicsHandler::DrawClient::processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                                                       const MSO::OfficeArtClientData* cd,
                                                       Writer& out)
{
    Q_UNUSED(ct);
    Q_UNUSED(cd);
    Q_UNUSED(out);
    kDebug(30513) << "Not implemented YET, not that bad actually!";
}

KoGenStyle
KWordGraphicsHandler::DrawClient::createGraphicStyle(const MSO::OfficeArtClientTextBox* ct,
                                                     const MSO::OfficeArtClientData* cd,
                                                     Writer& out)
{
    Q_UNUSED(ct);
    Q_UNUSED(cd);
    KoGenStyle style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    return style;
}

void
KWordGraphicsHandler::DrawClient::addTextStyles(const MSO::OfficeArtClientTextBox* clientTextbox,
                                                const MSO::OfficeArtClientData* clientData,
                                                Writer& out,
                                                KoGenStyle& style)
{
    Q_UNUSED(clientTextbox);
    Q_UNUSED(clientData);

    //TODO: Additional graphic-properties

    //TODO: Add paragraph-properties and text-properties if required!

    const QString styleName = out.styles.insert(style);
    out.xml.addAttribute("draw:style-name", styleName);
}

const MSO::OfficeArtDggContainer*
KWordGraphicsHandler::DrawClient::getOfficeArtDggContainer(void)
{
#ifdef USE_OFFICEARTDGG_CONTAINER
    return &gh->m_officeArtDggContainer;
#else
    return 0;
#endif
}

const MSO::OfficeArtSpContainer*
KWordGraphicsHandler::DrawClient::getMasterShapeContainer(quint32 spid)
{
    //TODO: No supoort for master shapes at the moment.
    Q_UNUSED(spid);
    return 0;
}

const MSO::OfficeArtSpContainer*
KWordGraphicsHandler::DrawClient::defaultShapeContainer(void)
{
    //Specific for Ppt at the moment.
    return 0;
}

QColor
KWordGraphicsHandler::DrawClient::toQColor(const MSO::OfficeArtCOLORREF& c)
{
    return QColor(c.red, c.green, c.blue);
}

QString
KWordGraphicsHandler::DrawClient::formatPos(qreal v)
{
    //assuming the client uses the DrawingWriter class
    return mm(v);

    //assuming the client uses the Writer class
//     return mm(twipsToMM(v));
}

//NOTE: OfficeArtClientData.clientdata (4 bytes): An integer that SHOULD be
//ignored.  [MS-DOC] — v20100926
bool
KWordGraphicsHandler::DrawClient::onlyClientData(const MSO::OfficeArtClientData& o)
{
    Q_UNUSED(o);
    return false;
}

void
KWordGraphicsHandler::DrawClient::processClientData(const MSO::OfficeArtClientTextBox* ct,
                                                    const MSO::OfficeArtClientData& o,
                                                    Writer& out)
{
    Q_UNUSED(ct);
    Q_UNUSED(o);
    Q_UNUSED(out);
}
