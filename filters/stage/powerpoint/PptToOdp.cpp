/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>
   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Amit Aggarwal <amitcs06@gmail.com>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "PptToOdp.h"

#include "PptDebug.h"
#include "PowerPointImport.h"
#include "globalobjectcollectors.h"
#include "pictures.h"
#include "ODrawToOdf.h"
#include "msodraw.h"
#include "msppt.h"
#include "msoleps.h"

#include <KoOdf.h>
#include <KoOdfWriteStore.h>
#include <KoXmlWriter.h>
#include <writeodf/writeodfstyle.h>
#include <writeodf/writeodfpresentation.h>
#include <writeodf/writeodfofficemeta.h>
#include <writeodf/writeodfofficedc.h>
#include <writeodf/writeodfdraw.h>
#include <writeodf/helpers.h>

#include <QTime>
#include <QDir>
#include <QBuffer>
#include <qmath.h>

//#define DEBUG_PPTTOODP
//#define USE_OFFICEARTDGG_CONTAINER
//#define DISABLE_PLACEHOLDER_BORDER

#define FONTSIZE_MAX 4000 //according to MS-PPT

using namespace MSO;
using namespace writeodf;


QUrl urlFromArg(const QString& arg)
{
#if QT_VERSION >= 0x050400
    return QUrl::fromUserInput(arg, QDir::currentPath(), QUrl::AssumeLocalFile);
#else
    // Logic from QUrl::fromUserInput(QString, QString, UserInputResolutionOptions)
    return (QUrl(arg, QUrl::TolerantMode).isRelative() && !QDir::isAbsolutePath(arg))
           ? QUrl::fromLocalFile(QDir::current().absoluteFilePath(arg))
           : QUrl::fromUserInput(arg);
#endif
}

/**
 * This class represents an opened <text:list> tag with an optionally opened
 * <text:list-item> tag.
 * Usually, writeodf::text_list instances are on the stack, but in this
 * class they are on heap. The TextListTag manages the allocation and
 * deallocation of these instances.
 */
class PptToOdp::TextListTag
{
public:
    QString style;
    QSharedPointer<text_list> list;
    QSharedPointer<text_list_item> item;
    TextListTag() {}
    TextListTag(const QString& style_, KoXmlWriter& out) :style(style_),
        list(new text_list(&out))
    {
    }
    TextListTag(const QString& style_, text_list_item& item) :style(style_),
        list(new text_list(item.add_text_list()))
    {
    }
    text_list_item& add_text_list_item()
    {
        item = QSharedPointer<text_list_item>(new text_list_item(list->add_text_list_item()));
        return *item;
    }
};

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
    QString cm(double v) {
        static const QString cm("cm");
        return format(v) + cm;
    }
    QString pt(double v) {
        static const QString pt("pt");
        return format(v) + pt;
    }
    QString percent(double v) {
        return format(v) + '%';
    }

// The placementId is mapped to one of: "chart", "date-time", "footer",
// "graphic", "handout", "header", "notes", "object", "orgchart", "outline",
// "page", "page-number", "subtitle", "table", "text" or "title"
//
// NOTE: we use 'outline' for PT_MasterBody, PT_Body and PT_VerticalBody types
// to be compatible with OpenOffice. OpenOffice <= 3.2 does not render lists
// properly if the presentation class is not 'outline', 'subtitle', or 'notes'.
const char*
getPresentationClass(const PlaceholderAtom* p)
{
    if (p == 0) return 0;
    switch (p->placementId) {
    case 0x01: return "title";       // PT_MasterTitle
    case 0x02: return "outline";     // PT_MasterBody
    case 0x03: return "title";       // PT_MasterCenterTitle
    case 0x04: return "subtitle";    // PT_MasterSubTitle
    case 0x05: return "graphic";     // PT_MasterNotesSlideImage
    case 0x06: return "notes";       // PT_MasterNotesBody
    case 0x07: return "date-time";   // PT_MasterDate
    case 0x08: return "page-number"; // PT_MasterSlideNumber
    case 0x09: return "footer";      // PT_MasterFooter
    case 0x0A: return "header";      // PT_MasterHeader
    case 0x0B: return "page";        // PT_NotesSlideImage
    case 0x0C: return "notes";       // PT_NotesBody
    case 0x0D: return "title";       // PT_Title
    case 0x0E: return "outline";     // PT_Body
    case 0x0F: return "title";       // PT_CenterTitle
    case 0x10: return "subtitle";    // PT_SubTitle
    case 0x11: return "title";       // PT_VerticalTitle
    case 0x12: return "outline";     // PT_VerticalBody
    case 0x13: return "object";      // PT_Object
    case 0x14: return "chart";       // PT_Graph
    case 0x15: return "table";       // PT_Table
    case 0x16: return "object";      // PT_ClipArt
    case 0x17: return "orgchart";    // PT_OrgChart
    case 0x18: return "object";      // PT_Media
    case 0x19: return "object";      // PT_VerticalObject
    case 0x1A: return "graphic";     // PT_Picture
    default: return 0;
    }
}

QString
getPresentationClass(const MSO::TextContainer* tc)
{
    if (!tc) return QString();
    for (int i = 0; i<tc->meta.size(); ++i) {
        const TextContainerMeta& m = tc->meta[i];
        if (m.meta.get<SlideNumberMCAtom>()) return "page-number";
        if (m.meta.get<DateTimeMCAtom>()) return "date-time";
        if (m.meta.get<GenericDateMCAtom>()) return "date-time";
        if (m.meta.get<HeaderMCAtom>()) return "header";
        if (m.meta.get<FooterMCAtom>()) return "footer";
    }
    return QString();
}

QString
getMasterStyle(const QMap<int, QString>& map, int texttype) {
    if (map.contains(texttype)) {
        return map[texttype];
    }
    // fallback for titles
    if (texttype == 0 || texttype == 6) {
        if (map.contains(0)) return map[0]; // Tx_TYPE_TITLE
        if (map.contains(6)) return map[6]; // Tx_TYPE_CENTERTITLE
        return QString();
    } else { // fallback for body
        if (map.contains(1)) return map[1]; // Tx_TYPE_BODY
        if (map.contains(5)) return map[5]; // Tx_TYPE_CENTERBODY
        if (map.contains(7)) return map[7]; // Tx_TYPE_HALFBODY
        if (map.contains(8)) return map[8]; // Tx_TYPE_QUARTERBODY
        if (map.contains(4)) return map[4]; // Tx_TYPE_OTHER
        return QString();
    }
    return QString();
}

const MSO::OfficeArtSpContainer*
getMasterShape(const MSO::MasterOrSlideContainer* m)
{
    if (!m) {
        return 0;
    }

    const SlideContainer* sc = m->anon.get<SlideContainer>();
    const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
    const OfficeArtSpContainer* scp = 0;
    if (sc) {
        if (sc->drawing.OfficeArtDg.shape) {
            scp = sc->drawing.OfficeArtDg.shape.data();
        }
    } else if (mm) {
        if (mm->drawing.OfficeArtDg.shape) {
            scp = mm->drawing.OfficeArtDg.shape.data();
        }
    }
    return scp;
}

/**
 * Return the bounding rectangle for this object.
 **/
QRect
getRect(const PptOfficeArtClientAnchor &a)
{
    if (a.rect1) {
        const SmallRectStruct &r = *a.rect1;
        return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    } else {
        const RectStruct &r = *a.rect2;
        return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    }
}

QString
getText(const TextContainer* tc)
{
    if (!tc) return QString();

    QString ret;
    if (tc->text.is<TextCharsAtom>()) {
        const QVector<quint16> textChars(tc->text.get<TextCharsAtom>()->textChars);
        ret = QString::fromUtf16(textChars.data(), textChars.size());
    } else if (tc->text.is<TextBytesAtom>()) {
        // each item represents the low byte of a UTF-16 Unicode character
        // whose high byte is 0x00
        const QByteArray& textChars(tc->text.get<TextBytesAtom>()->textChars);
        ret = QString::fromLatin1(textChars, textChars.size());
    }
    return ret;
}

} //namespace (anonymous)


/*
 * ************************************************
 * DrawClient
 * ************************************************
 */
class PptToOdp::DrawClient : public ODrawToOdf::Client
{
private:
    QRectF getRect(const MSO::OfficeArtClientAnchor&) override;
    QRectF getReserveRect(void) override;
    QString getPicturePath(const quint32 pib) override;
    bool onlyClientData(const MSO::OfficeArtClientData& o) override;
    void processClientData(const MSO::OfficeArtClientTextBox* ct,
                           const MSO::OfficeArtClientData& cd,
                           Writer& out) override;
    void processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                              const MSO::OfficeArtClientData* cd,
                              Writer& out) override;
    bool processRectangleAsTextBox(const MSO::OfficeArtClientData& cd) override;
    KoGenStyle createGraphicStyle(
            const MSO::OfficeArtClientTextBox* ct,
            const MSO::OfficeArtClientData* cd, const DrawStyle& ds, Writer& out) override;
    void addTextStyles(const MSO::OfficeArtClientTextBox* clientTextbox,
                       const MSO::OfficeArtClientData* clientData,
                       KoGenStyle& style, Writer& out) override;

    const MSO::OfficeArtDggContainer* getOfficeArtDggContainer() override;
    const MSO::OfficeArtSpContainer* getMasterShapeContainer(quint32 spid) override;

    QColor toQColor(const MSO::OfficeArtCOLORREF& c) override;
    QString formatPos(qreal v) override;

   /**
    * Check if a placeholder is valid and allowed by the slide layout.
    * @param PlaceholderAtom
    * @return 1 - allowed, 0 - forbidden
    */
    bool placeholderAllowed(const MSO::PlaceholderAtom* pa) const;

    bool isPlaceholder(const MSO::OfficeArtClientData* cd) const;

    /**
     * PPT client specific data.
     */
    struct DrawClientData
    {
        const MSO::MasterOrSlideContainer* masterSlide;
        const MSO::SlideContainer* presSlide;
        const MSO::NotesContainer* notesMasterSlide;
        const MSO::NotesContainer* notesSlide;
        const MSO::SlideListWithTextSubContainerOrAtom* slideTexts;

        DrawClientData(): masterSlide(0), presSlide(0), notesMasterSlide(0),
                          notesSlide(0), slideTexts(0) {};
    };

    DrawClientData dc_data[1];

    PptToOdp* const ppttoodp;

public:
    DrawClient(PptToOdp* p) :ppttoodp(p) {}

    void setDrawClientData(const MSO::MasterOrSlideContainer* mc,
                           const MSO::SlideContainer* sc,
                           const MSO::NotesContainer* nmc,
                           const MSO::NotesContainer* nc,
                           const MSO::SlideListWithTextSubContainerOrAtom* stc = 0)
    {
        dc_data->masterSlide = mc;
        dc_data->presSlide = sc;
        dc_data->notesMasterSlide = nmc;
        dc_data->notesSlide = nc;
        dc_data->slideTexts = stc;
    }
};

bool PptToOdp::DrawClient::isPlaceholder(const MSO::OfficeArtClientData* cd) const
{
    if (!cd) {
        return false;
    }
    const PptOfficeArtClientData* pcd = cd->anon.get<PptOfficeArtClientData>();
    if (pcd && pcd->placeholderAtom &&
        placeholderAllowed(pcd->placeholderAtom.data())) {
        return true;
    }
    return false;
}

QRectF PptToOdp::DrawClient::getRect(const MSO::OfficeArtClientAnchor& o)
{
    const PptOfficeArtClientAnchor* a = o.anon.get<PptOfficeArtClientAnchor>();
    if (a) {
        return ::getRect(*a);
    }
    return QRectF();
}
QRectF PptToOdp::DrawClient::getReserveRect(void)
{
    //NOTE: No PPT test files at the moment.
    return QRect(0, 0, 1, 1);
}
QString PptToOdp::DrawClient::getPicturePath(const quint32 pib)
{
    return ppttoodp->getPicturePath(pib);
}
bool PptToOdp::DrawClient::onlyClientData(const MSO::OfficeArtClientData& o)
{
    const PptOfficeArtClientData* pcd = o.anon.get<PptOfficeArtClientData>();
    if (pcd && pcd->placeholderAtom && dc_data->slideTexts) {
        const PlaceholderAtom* pa = pcd->placeholderAtom.data();
        if (pa->position >= 0 &&
            pa->position < dc_data->slideTexts->atoms.size())
        {
            return true;
        }
    }
    return false;
}
void PptToOdp::DrawClient::processClientData(const MSO::OfficeArtClientTextBox* ct,
                                             const MSO::OfficeArtClientData& o, Writer& out)
{
    const TextContainer* textContainer = 0;
    const TextRuler* textRuler = 0;

    if (ct) {
        if (ct->anon.is<PptOfficeArtClientTextBox>()) {
            const PptOfficeArtClientTextBox* tb = ct->anon.get<PptOfficeArtClientTextBox>();
            foreach(const TextClientDataSubContainerOrAtom& tc, tb->rgChildRec) {
                if (tc.anon.is<OutlineAtom>()) {
                    const OutlineAtom* outlineAtom = tc.anon.get<OutlineAtom>();
                    if (outlineAtom->textRulerAtom) {
                        textRuler = &outlineAtom->textRulerAtom->textRuler;
                        break;
                    }
                }
            }
        }
    }

    const PptOfficeArtClientData* pcd = o.anon.get<PptOfficeArtClientData>();
    if (pcd && pcd->placeholderAtom && dc_data->slideTexts) {
        const PlaceholderAtom* pa = pcd->placeholderAtom.data();
        if (pa->position >= 0 &&
            pa->position < dc_data->slideTexts->atoms.size())
        {
            textContainer = &dc_data->slideTexts->atoms[pa->position];
            ppttoodp->processTextForBody(out, &o, textContainer, textRuler, isPlaceholder(&o));
        }
    }
}
void PptToOdp::DrawClient::processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                                                const MSO::OfficeArtClientData* cd, Writer& out)
{
    // NOTE: Workaround!  Only in case of a textshape the placeholder flag does
    // hide the placeholder text => Ignoring the placeholder text in case of
    // other shapes on master slides.

    if (ppttoodp->m_processingMasters) {
        if (isPlaceholder(cd)) {
            if (!((m_currentShapeType == msosptTextBox) ||
		  (m_currentShapeType == msosptRectangle))) {
                return;
            }
        }
    }

    const PptOfficeArtClientTextBox* tb = ct.anon.get<PptOfficeArtClientTextBox>();
    if (tb) {
        const MSO::TextContainer* textContainer = 0;
        const MSO::TextRuler* textRuler = 0;

        foreach(const TextClientDataSubContainerOrAtom& tc, tb->rgChildRec) {
            if (tc.anon.is<TextContainer>()) {
                textContainer = tc.anon.get<TextContainer>();
                if (textContainer->textRulerAtom) {
                    textRuler = &textContainer->textRulerAtom->textRuler;
                }
            }
        }
        ppttoodp->processTextForBody(out, cd, textContainer, textRuler, isPlaceholder(cd));
    }
}

bool PptToOdp::DrawClient::processRectangleAsTextBox(const MSO::OfficeArtClientData& cd)
{
    const PptOfficeArtClientData* pcd = cd.anon.get<PptOfficeArtClientData>();
    if (pcd && pcd->placeholderAtom) {
        return true;
    } else {
        return false;
    }
}

KoGenStyle PptToOdp::DrawClient::createGraphicStyle(
        const MSO::OfficeArtClientTextBox* clientTextbox,
        const MSO::OfficeArtClientData* clientData,
        const DrawStyle& ds,
        Writer& out)
{
    Q_UNUSED(ds);
    KoGenStyle style;

    const PptOfficeArtClientData* cd = 0;
    if (clientData) {
        cd = clientData->anon.get<PptOfficeArtClientData>();
    }
    const PptOfficeArtClientTextBox* tb = 0;
    if (clientTextbox) {
        tb = clientTextbox->anon.get<PptOfficeArtClientTextBox>();
    }
    quint32 textType = ppttoodp->getTextType(tb, cd);

    if (isPlaceholder(clientData)) { // type is presentation
        bool canBeParentStyle = false;
        if ( (textType != 99) && out.stylesxml && dc_data->masterSlide) {
            canBeParentStyle = true;
        }
        bool isAutomatic = !canBeParentStyle;

        // If this object has a placeholder type, it defines a presentation
        // style, otherwise, it defines a graphic style.  A graphic style is
        // always automatic.
        KoGenStyle::Type type = KoGenStyle::PresentationStyle;
        if (isAutomatic) {
            type = KoGenStyle::PresentationAutoStyle;
        }
        style = KoGenStyle(type, "presentation");
        if (isAutomatic) {
            style.setAutoStyleInStylesDotXml(out.stylesxml);
        }
        QString parent;
        // for now we only set parent styles on presentation styled elements
        if (dc_data->masterSlide) {
            parent = getMasterStyle(ppttoodp->masterPresentationStyles[dc_data->masterSlide],
                                    textType);
        }
        if (!parent.isEmpty()) {
            style.setParentName(parent);
        }
    } else { // type is graphic
        style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        style.setAutoStyleInStylesDotXml(out.stylesxml);
    }

    if (out.stylesxml) {
        const MasterOrSlideContainer* m = dc_data->masterSlide;
        const TextMasterStyleAtom* msa = getTextMasterStyleAtom(m, textType);
        if (msa) {
            KoGenStyle list(KoGenStyle::ListStyle);
            ppttoodp->defineListStyle(list, textType, *msa);
            QString listStyleName;
            listStyleName = out.styles.insert(list);
        }
    }
    return style;
}

void PptToOdp::DrawClient::addTextStyles(
        const MSO::OfficeArtClientTextBox* clientTextbox,
        const MSO::OfficeArtClientData* clientData,
        KoGenStyle& style, Writer& out)
{
    // content.xml - As soon the content or graphic-style of a placeholder
    // changed, make it a normal shape to be ODF compliant.
    //
    // TODO: check if the graphic-style changed compared to the parent

    const PptOfficeArtClientData* cd = 0;
    if (clientData) {
        cd = clientData->anon.get<PptOfficeArtClientData>();
    }
    const PptOfficeArtClientTextBox* tb = 0;
    if (clientTextbox) {
        tb = clientTextbox->anon.get<PptOfficeArtClientTextBox>();
    }

    bool potentialPlaceholder = false;

    if (m_currentShapeType == msosptRectangle) {
        potentialPlaceholder = true;
    }

    if (out.stylesxml) {
        //get the main master slide's MasterOrSlideContainer
        const MasterOrSlideContainer* m = 0;
        if (dc_data->masterSlide && isPlaceholder(clientData)) {
            m = dc_data->masterSlide;
            while (m->anon.is<SlideContainer>()) {
                m = ppttoodp->p->getMaster(m->anon.get<SlideContainer>());
            }
        }
        const TextContainer* tc = ppttoodp->getTextContainer(tb, cd);
        PptTextPFRun pf(ppttoodp->p->documentContainer, m, dc_data->slideTexts, cd, tc);
        ppttoodp->defineParagraphProperties(style, pf, 0);
        PptTextCFRun cf(ppttoodp->p->documentContainer, m, tc, 0);
        ppttoodp->defineTextProperties(style, cf, 0, 0, 0);
    }
#ifdef DISABLE_PLACEHOLDER_BORDER
    if (isPlaceholder(clientData)) {
        style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
        //style.addProperty("draw:stroke-width", "none", KoGenStyle::GraphicType);
    }
#endif

    bool isCustomShape = false;
    switch (m_currentShapeType) {
    case msosptPictureFrame:
    case msosptTextBox:
    case msosptLine:
        break;
    case msosptRectangle:
        if (!clientData || !processRectangleAsTextBox(*clientData)) {
            isCustomShape = true;
        }
        break;
    default:
        isCustomShape = true;
        break;
    }

    // NOTE: Workaround: Set padding to ZERO until the fo:wrap-option support
    // arrives and other text on shape related issues get fixed.
    if (isCustomShape) {
        style.removeProperty("fo:padding-left");
        style.removeProperty("fo:padding-right");
        style.removeProperty("fo:padding-top");
        style.removeProperty("fo:padding-bottom");
        style.addPropertyPt("fo:padding", 0);
    }

    const QString styleName = out.styles.insert(style);

    if (isPlaceholder(clientData)) {
        out.xml.addAttribute("presentation:style-name", styleName);
        QString className = getPresentationClass(cd->placeholderAtom.data());
        const TextContainer* tc = ppttoodp->getTextContainer(tb, cd);

        if ( className.isEmpty() ||
             (!out.stylesxml && (!potentialPlaceholder || getText(tc).size())) )
        {
            className = getPresentationClass(tc);
            out.xml.addAttribute("presentation:placeholder", "false");
        } else {
            out.xml.addAttribute("presentation:placeholder", "true");
        }
        if (!className.isEmpty()) {
            out.xml.addAttribute("presentation:class", className);
        }
    } else {
        out.xml.addAttribute("draw:style-name", styleName);
    }
    quint32 textType = ppttoodp->getTextType(tb, cd);
    bool canBeParentStyle = false;
    if (isPlaceholder(clientData) && (textType != 99) && out.stylesxml && dc_data->masterSlide) {
        canBeParentStyle = true;
    }
    if (canBeParentStyle) {
        ppttoodp->masterPresentationStyles[dc_data->masterSlide][textType] = styleName;
    }
} //end addTextStyle()

const MSO::OfficeArtDggContainer*
PptToOdp::DrawClient::getOfficeArtDggContainer()
{
#ifdef USE_OFFICEARTDGG_CONTAINER
    return &ppttoodp->p->documentContainer->drawingGroup.OfficeArtDgg;
#else
    return 0;
#endif
}

const MSO::OfficeArtSpContainer*
PptToOdp::DrawClient::getMasterShapeContainer(quint32 spid)
{
    const OfficeArtSpContainer* sp = 0;
    sp = ppttoodp->retrieveMasterShape(spid);
    return sp;
}

QColor PptToOdp::DrawClient::toQColor(const MSO::OfficeArtCOLORREF& c)
{
    //Have to handle the case when OfficeArtCOLORREF/fSchemeIndex == true.

    //NOTE: If the hspMaster property (0x0301) is provided by the shape, the
    //colorScheme of the master slide containing the master shape could be
    //required.  Testing required to implement the correct logic.

    const MSO::MasterOrSlideContainer* mc = dc_data->masterSlide;
    const MSO::MainMasterContainer* mm = nullptr;
    const MSO::SlideContainer* tm = nullptr;
    QColor ret;

    if (mc) {
        if (mc->anon.is<MainMasterContainer>()) {
            mm = mc->anon.get<MainMasterContainer>();
            ret = ppttoodp->toQColor(c, mm, dc_data->presSlide);
        } else if (mc->anon.is<SlideContainer>()) {
            tm = mc->anon.get<SlideContainer>();
            ret = ppttoodp->toQColor(c, tm, dc_data->presSlide);
        }
    }
    //TODO: handle the case of a notes master slide/notes slide pair

    return ret;
}

QString PptToOdp::DrawClient::formatPos(qreal v)
{
    return mm(v * (25.4 / 576));
}

bool PptToOdp::DrawClient::placeholderAllowed(const MSO::PlaceholderAtom* pa) const
{
    //For details check the following chapter: 2.5.10 SlideAtom
    //[MS-PPT] — v20101219

    //TODO: Num. and combinations of placeholder shapes matters!

    if (!pa || (pa->position == (qint32) 0xFFFFFFFF)) {
        return false;
    }
    quint8 placementId = pa->placementId;
    quint32 geom = SL_TitleSlide;

    const MSO::MainMasterContainer* mm = 0;
    const MSO::SlideContainer* tm = 0;
    if (ppttoodp->m_processingMasters) {
        const MSO::MasterOrSlideContainer* mc = dc_data->masterSlide;
        if (mc) {
            if (mc->anon.is<MainMasterContainer>()) {
                mm = mc->anon.get<MainMasterContainer>();
                geom = mm->slideAtom.geom;
            } else if (mc->anon.is<SlideContainer>()) {
                tm = mc->anon.get<SlideContainer>();
                geom = tm->slideAtom.geom;
            }
        }
    } else {
        if (dc_data->presSlide) {
            geom = dc_data->presSlide->slideAtom.geom;
        }
    }
    //Main Master Slide
    if (mm) {
        switch(geom) {
        case SL_TitleBody:
            switch (placementId) {
            case PT_MasterTitle:
            case PT_MasterBody:
            case PT_MasterDate:
            case PT_MasterFooter:
            case PT_MasterSlideNumber:
                return true;
            default:
                return false;
            }
        default:
            return false;
        }
    }
    //Title Master Slide
    if (tm) {
        switch(geom) {
        case SL_MasterTitle:
            switch (placementId) {
            case PT_MasterCenterTitle:
            case PT_MasterSubTitle:
            case PT_MasterDate:
            case PT_MasterFooter:
            case PT_MasterSlideNumber:
                return true;
            default:
                return false;
            }
        default:
            return false;
        }
    }
    //Presentation Slide
    switch(geom) {
    case SL_TitleSlide:
        switch (placementId) {
        case PT_CenterTitle:
        case PT_SubTitle:
            return true;
        default:
            return false;
        }
    case SL_TitleBody:
        switch (placementId) {
        case PT_Title:
        case PT_Body:
        case PT_Table:
        case PT_OrgChart:
        case PT_Graph:
        case PT_Object:
        case PT_VerticalBody:
            return true;
        default:
            return false;
        }
    case SL_TitleOnly:
        switch (placementId) {
        case PT_Title:
            return true;
        default:
            return false;
        }
    case SL_TwoColumns:
        //TODO: support placeholder combinations
        return true;
    case SL_TwoRows:
    case SL_ColumnTwoRows:
    case SL_TwoRowsColumn:
    case SL_TwoColumnsRow:
        switch (placementId) {
        case PT_Title:
        case PT_Body:
        case PT_Object:
            return true;
        default:
            return false;
        }
    case SL_FourObjects:
        switch (placementId) {
        case PT_Title:
        case PT_Object:
            return true;
        default:
            return false;
        }
    case SL_BigObject:
        switch (placementId) {
        case PT_Object:
            return true;
        default:
            return false;
        }
    case SL_Blank:
        //TODO: support placeholder combinations
        return false;
    case SL_VerticalTitleBody:
        switch (placementId) {
        case PT_VerticalTitle:
        case PT_VerticalBody:
            return true;
        default:
            return false;
        }
    case SL_VerticalTwoRows:
        switch (placementId) {
        case PT_VerticalTitle:
        case PT_VerticalBody:
        case PT_Graph:
            return true;
        default:
            return false;
        }
    default:
        return false;
    }
}

/*
 * ************************************************
 * PptToOdp
 * ************************************************
 */
PptToOdp::PptToOdp(PowerPointImport* filter, void (PowerPointImport::*setProgress)(const int))
: p(0),
  m_filter(filter),
  m_setProgress(setProgress),
  m_progress_update(filter && setProgress),
  m_currentSlideTexts(0),
  m_currentMaster(0),
  m_currentSlide(0),
  m_processingMasters(false),
  m_firstChunkFontSize(12),
  m_firstChunkSymbolAtStart(false),
  m_isList(false),
  m_previousListLevel(0)
{
    qsrand(QTime::currentTime().msec());
}

PptToOdp::~PptToOdp()
{
    delete p;
}

QMap<quint16, QString>
createBulletPictures(const PP9DocBinaryTagExtension* pp9, KoStore* store, KoXmlWriter* manifest)
{
    QMap<quint16, QString> ids;
    if (!pp9 || !pp9->blipCollectionContainer) {
        return ids;
    }
    foreach (const BlipEntityAtom& a, pp9->blipCollectionContainer->rgBlipEntityAtom) {
        PictureReference ref = savePicture(a.blip, store);
        if (ref.name.length() == 0) continue;
        ids[a.rh.recInstance] = "Pictures/" + ref.name;
        manifest->addManifestEntry(ids[a.rh.recInstance], ref.mimetype);
    }
    return ids;
}
bool
PptToOdp::parse(POLE::Storage& storage)
{
    delete p;
    p = 0;
    ParsedPresentation* pp = new ParsedPresentation();
    if (!pp->parse(storage)) {
        delete pp;
        return false;
    }
    p = pp;
    return true;
}
KoFilter::ConversionStatus
PptToOdp::convert(const QString& inputFile, const QString& to, KoStore::Backend storeType)
{
    if (m_progress_update) {
        (m_filter->*m_setProgress)(0);
    }

    // open inputFile
    POLE::Storage storage(inputFile.toLocal8Bit());
    if (!storage.open()) {
        debugPpt << "Cannot open " << inputFile;
        return KoFilter::InvalidFormat;
    }

    if (!parse(storage)) {
        debugPpt << "Parsing and setup failed.";
        return KoFilter::InvalidFormat;
    }

    // using an average here, parsing might take longer than conversion
    if (m_progress_update) {
        (m_filter->*m_setProgress)(40);
    }

    // create output store
    KoStore* storeout = KoStore::createStore(to, KoStore::Write,
                        KoOdf::mimeType(KoOdf::Presentation), storeType);
    if (!storeout) {
        warnPpt << "Couldn't open the requested file.";
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus status = doConversion(storeout);

    if (m_progress_update) {
        (m_filter->*m_setProgress)(100);
    }

    delete storeout;
    return status;
}

KoFilter::ConversionStatus
PptToOdp::convert(POLE::Storage& storage, KoStore* storeout)
{
    if (!parse(storage)) {
        debugPpt << "Parsing and setup failed.";
        return KoFilter::InvalidFormat;
    }
    return doConversion(storeout);
}

KoFilter::ConversionStatus
PptToOdp::doConversion(KoStore* storeout)
{
    KoOdfWriteStore odfWriter(storeout);
    KoXmlWriter* manifest = odfWriter.manifestWriter(
                                KoOdf::mimeType(KoOdf::Presentation));

    // store the images from the 'Pictures' stream
    storeout->enterDirectory("Pictures");
    pictureNames = createPictures(storeout, manifest, &p->pictures.anon1.rgfb);
    // read pictures from the PowerPoint Document structures
    bulletPictureNames = createBulletPictures(getPP<PP9DocBinaryTagExtension>(
            p->documentContainer), storeout, manifest);
    storeout->leaveDirectory();
    storeout->setCompressionEnabled(true);

    KoGenStyles styles;

    createMainStyles(styles);

    // store document content
    if (!storeout->open("content.xml")) {
        warnPpt << "Couldn't open the file 'content.xml'.";
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    storeout->write(createContent(styles));
    if (!storeout->close()) {
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    manifest->addManifestEntry("content.xml", "text/xml");

    // store document styles
    styles.saveOdfStylesDotXml(storeout, manifest);

    if (!storeout->open("meta.xml")) {
        warnPpt << "Couldn't open the file 'meta.xml'.";
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    storeout->write(createMeta());
    if (!storeout->close()) {
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    manifest->addManifestEntry("meta.xml", "text/xml");

    if (!storeout->open("settings.xml")) {
        warnPpt << "Couldn't open the file 'settings.xml'.";
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    storeout->write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<office:document-settings xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" office:version=\"1.2\"/>\n");
    if (!storeout->close()) {
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    manifest->addManifestEntry("settings.xml", "text/xml");

    odfWriter.closeManifestWriter();

    delete p;
    p = 0;
    return KoFilter::OK;
}

namespace
{

QString
definePageLayout(KoGenStyles& styles, const MSO::PointStruct& size) {
    // x and y are given in master units (1/576 inches)
    double sizeX = size.x * (25.4 / (double)576);
    double sizeY = size.y * (25.4 / (double)576);
    QString pageWidth = mm(sizeX);
    QString pageHeight = mm(sizeY);

    KoGenStyle pl(KoGenStyle::PageLayoutStyle);
    pl.setAutoStyleInStylesDotXml(true);
    // pl.addAttribute("style:page-usage", "all"); // probably not needed
    pl.addProperty("fo:margin-bottom", "0pt");
    pl.addProperty("fo:margin-left", "0pt");
    pl.addProperty("fo:margin-right", "0pt");
    pl.addProperty("fo:margin-top", "0pt");
    pl.addProperty("fo:page-height", pageHeight);
    pl.addProperty("fo:page-width", pageWidth);
    pl.addProperty("style:print-orientation", "landscape");
    return styles.insert(pl, "pm");
}

} //namespace

void PptToOdp::defineDefaultTextStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="text">
    KoGenStyle style(KoGenStyle::TextStyle, "text");
    style.setDefaultStyle(true);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultParagraphStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="paragraph">
    KoGenStyle style(KoGenStyle::ParagraphStyle, "paragraph");
    style.setDefaultStyle(true);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultSectionStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="section">
    KoGenStyle style(KoGenStyle::SectionStyle, "section");
    style.setDefaultStyle(true);
    styles.insert(style);
}

void PptToOdp::defineDefaultRubyStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="ruby">
    KoGenStyle style(KoGenStyle::RubyStyle, "ruby");
    style.setDefaultStyle(true);
    styles.insert(style);
}

void PptToOdp::defineDefaultTableStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table">
    KoGenStyle style(KoGenStyle::TableStyle, "table");
    style.setDefaultStyle(true);
    styles.insert(style);
}

void PptToOdp::defineDefaultTableColumnStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table-column">
    KoGenStyle style(KoGenStyle::TableColumnStyle, "table-column");
    style.setDefaultStyle(true);
    styles.insert(style);
}

void PptToOdp::defineDefaultTableRowStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table-row">
    KoGenStyle style(KoGenStyle::TableRowStyle, "table-row");
    style.setDefaultStyle(true);
    styles.insert(style);
}

void PptToOdp::defineDefaultTableCellStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table-cell">
    KoGenStyle style(KoGenStyle::TableCellStyle, "table-cell");
    style.setDefaultStyle(true);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultGraphicStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="graphic">
    KoGenStyle style(KoGenStyle::GraphicStyle, "graphic");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style, styles);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultPresentationStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="presentation">
    KoGenStyle style(KoGenStyle::PresentationStyle, "presentation");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style, styles);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultDrawingPageStyle(KoGenStyles& styles)
{
    if (!p->documentContainer) return;
    // write style <style:default-style style:family="drawing-page">
    KoGenStyle style(KoGenStyle::DrawingPageStyle, "drawing-page");
    const KoGenStyle::PropertyType dpt = KoGenStyle::DrawingPageType;
    style.addProperty("draw:background-size", "border", dpt);
    style.addProperty("draw:fill", "none", dpt);
    style.setDefaultStyle(true);
    const MSO::SlideHeadersFootersContainer* hf = getSlideHF();
    const OfficeArtDggContainer* drawingGroup
        = &p->documentContainer->drawingGroup.OfficeArtDgg;
    DrawStyle ds(drawingGroup);
    DrawClient drawclient(this);
    ODrawToOdf odrawtoodf(drawclient);
    drawclient.setDrawClientData(0, 0, 0, 0);
    defineDrawingPageStyle(style, ds, styles, odrawtoodf, (hf) ?&hf->hfAtom :0);
    styles.insert(style);
}

void PptToOdp::defineDefaultChartStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="chart">
    KoGenStyle style(KoGenStyle::ChartStyle, "chart");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style, styles);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultTextProperties(KoGenStyle& style)
{
    const PptTextCFRun cf(p->documentContainer);
    const TextCFException9* cf9 = 0;
    const TextCFException10* cf10 = 0;
    const TextSIException* si = 0;
    if (p->documentContainer) {
        const PP9DocBinaryTagExtension* pp9 = getPP<PP9DocBinaryTagExtension>(
                p->documentContainer);
        const PP10DocBinaryTagExtension* pp10 = getPP<PP10DocBinaryTagExtension>(
                p->documentContainer);
        if (pp9 && pp9->textDefaultsAtom) {
            cf9 = &pp9->textDefaultsAtom->cf9;
        }
        if (pp10 && pp10->textDefaultsAtom) {
            cf10 = &pp10->textDefaultsAtom->cf10;
        }
        si = &p->documentContainer->documentTextInfo.textSIDefaultsAtom.textSIException;
    }
    defineTextProperties(style, cf, cf9, cf10, si);
}

void PptToOdp::defineDefaultParagraphProperties(KoGenStyle& style)
{
    PptTextPFRun pf(p->documentContainer);
    defineParagraphProperties(style, pf, 0);
}

void PptToOdp::defineDefaultGraphicProperties(KoGenStyle& style, KoGenStyles& styles)
{
    const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
    style.addProperty("svg:stroke-width", "0.75pt", gt); // 2.3.8.15
    style.addProperty("draw:fill", "none", gt); // 2.3.8.38
    style.addProperty("draw:auto-grow-height", false, gt);
    style.addProperty("draw:stroke", "solid", gt);
    style.addProperty("draw:fill-color", "#ffffff", gt);
    const OfficeArtDggContainer* drawingGroup
        = &p->documentContainer->drawingGroup.OfficeArtDgg;
    const DrawStyle ds(drawingGroup);
    DrawClient drawclient(this);
    ODrawToOdf odrawtoodf(drawclient);
    odrawtoodf.defineGraphicProperties(style, ds, styles);
}

template<class T>
void
setRgbUid(const T* a, QByteArray& rgbUid)
{
    if (!a) return;
    rgbUid = a->rgbUid1 + a->rgbUid2;
}

QString PptToOdp::getPicturePath(const quint32 pib) const
{
    bool use_offset = false;
    quint32 offset = 0;

    const OfficeArtDggContainer& dgg = p->documentContainer->drawingGroup.OfficeArtDgg;
    QByteArray rgbUid = getRgbUid(dgg, pib, offset);

    if (!rgbUid.isEmpty()) {
        if (pictureNames.contains(rgbUid)) {
            return "Pictures/" + pictureNames[rgbUid];
        } else {
            debugPpt << "UNKNOWN picture reference:" << rgbUid.toHex();
            use_offset = true;
            rgbUid.clear();
        }
    }
    if (use_offset) {
        const OfficeArtBStoreDelay& d = p->pictures.anon1;
        foreach (const OfficeArtBStoreContainerFileBlock& block, d.rgfb) {
            if (block.anon.is<OfficeArtBlip>()) {
                if (block.anon.get<OfficeArtBlip>()->streamOffset == offset) {

                    const OfficeArtBlip* b = block.anon.get<OfficeArtBlip>();
                    setRgbUid(b->anon.get<MSO::OfficeArtBlipEMF>(), rgbUid);
                    setRgbUid(b->anon.get<MSO::OfficeArtBlipWMF>(), rgbUid);
                    setRgbUid(b->anon.get<MSO::OfficeArtBlipPICT>(), rgbUid);
                    setRgbUid(b->anon.get<MSO::OfficeArtBlipJPEG>(), rgbUid);
                    setRgbUid(b->anon.get<MSO::OfficeArtBlipPNG>(), rgbUid);
                    setRgbUid(b->anon.get<MSO::OfficeArtBlipDIB>(), rgbUid);
                    setRgbUid(b->anon.get<MSO::OfficeArtBlipTIFF>(), rgbUid);

                    if (!rgbUid.isEmpty()) {
                        if (pictureNames.contains(rgbUid)) {
                            debugPpt << "Reusing OfficeArtBlip offset:" << offset;
                            return "Pictures/" + pictureNames[rgbUid];
                        }
                    }
                }
            }
        }
    }
    return QString();
}

void PptToOdp::defineTextProperties(KoGenStyle& style,
                                    const PptTextCFRun& cf,
                                    const TextCFException9* /*cf9*/,
                                    const TextCFException10* /*cf10*/,
                                    const TextSIException* /*si*/,
                                    const bool isSymbol)
{
    // Getting information for all the possible attributes in
    // style:text-properties for clarity in alphabetical order.
    const KoGenStyle::PropertyType text = KoGenStyle::TextType;

    // symbol font has precedence
    bool isSymbolFont = false;

    // fo:background-color
    // fo:color
    ColorIndexStruct cis = cf.color();
    QColor color = toQColor(cis);
    if (color.isValid()) {
        style.addProperty("fo:color", color.name(), text);
    }
    // fo:country
    // fo:font-family
    const FontEntityAtom* font = 0;
    if (cf.symbolFontRef() && isSymbol) {
        if ( (font = getFont(cf.symbolFontRef())) != 0 ) {
            isSymbolFont = true;
        }
    }
    if (!font) {
        font = getFont(cf.fontRef());
    }
    if (font) {
#ifdef DEBUG_PPTTOODP_FONTS
        debugPpt << "DEBUG: FontEntityAtom";
        debugPpt << "> IfCharSet:" << font->lfCharSet;
        debugPpt << "> fEmbedSubsetted:" << font->fEmbedSubsetted;
        debugPpt << "> rasterFontType:" << font->rasterFontType;
        debugPpt << "> deviceFontType:" << font->deviceFontType;
        debugPpt << "> truetypeFontType:" << font->truetypeFontType;
        debugPpt << "> fNoFontSubstitution:" << font->fNoFontSubstitution;
        debugPpt << "DEBUG END: FontEntityAtom";
#endif
        const QString name = QString::fromUtf16(font->lfFaceName.data(), font->lfFaceName.size());
        style.addProperty("fo:font-family", name, text);
    }
    // fo:font-size
    if (cf.fontSize() > 0) {
        style.addProperty("fo:font-size", pt(cf.fontSize()), text);
    }
    // fo:font-style: "italic", "normal" or "oblique
    style.addProperty("fo:font-style", cf.italic() ?"italic" :"normal", text);
    // fo:font-variant: "normal" or "small-caps"
    // fo:font-weight: "100", "200", "300", "400", "500", "600", "700", "800", "900", "bold" or "normal"
    style.addProperty("fo:font-weight", cf.bold() ?"bold" :"normal", text);
    // fo:hyphenate
    // fo:hyphenation-push-char
    // fo:hyphenation-remain-char-count
    // fo:language
//     if (si && si->lang) {
        // TODO: get mapping from lid to language code
//     }
    // fo:letter-spacing
    // fo:text-shadow
    style.addProperty("fo:text-shadow", cf.shadow() ?"1pt 1pt" :"none", text);
    // fo:text-transform: "capitalize", "lowercase", "none" or "uppercase"
    // style:country-asian
    // style:country-complex
    // style:font-charset
    if (isSymbolFont) {
        style.addProperty("style:font-charset", "x-symbol", text);
    }
    // style:font-family-asian
    // style:font-family-complex
    // style:font-family-generic
    // style:font-family-generic-asian
    // style:font-family-generic-complex
    // style:font-name
    // style:font-name-asian
    // style:font-name-complex
    // style:font-pitch
    // style:font-pitch-asian
    // style:font-pitch-complex
    // style:font-relief: "embossed", "engraved" or "none"
    style.addProperty("style:font-relief", cf.emboss() ?"embossed" :"none", text);
    // style:font-size-asian
    // style:font-size-complex
    // style:font-size-rel
    // style:font-size-rel-asian
    // style:font-size-rel-complex
    // style:font-style-asian
    // style:font-style-complex
    // style:font-style-name
    // style:font-style-name-asian
    // style:font-style-name-complex
    // style:font-weight-asian
    // style:font-weight-complex
    // style:language-asian
    // style:language-complex
    // style:letter-kerning
    // style:script-type
    // style:text-blinking
    // style:text-combine
    // style:text-combine-end-char
    // style:text-combine-start-char
    // style:text-emphasize
    // style:text-line-through-color
    // style:text-line-through-mode
    // style:text-line-through-style
    // style:text-line-through-text
    // style:text-line-through-text-style
    // style:text-line-through-type
    // style:text-line-through-width
    // style:text-outline
    // style:text-position
    style.addProperty("style:text-position", percent(cf.position()), text);
    // style:text-rotation-angle
    // style:text-rotation-scale
    // style:text-scale
    // style:text-underline-color
    // style:text-underline-mode
    // style:text-underline-style
    // style:text-underline-type: "double", "none" or "single"
    style.addProperty("style:text-underline-type", cf.underline() ?"single" :"none", text);
    // style:text-underline-width
    // style:use-window-font-color
} //end defineTextProperties()

void PptToOdp::defineParagraphProperties(KoGenStyle& style, const PptTextPFRun& pf,
                                         const quint16 fs)
{
    const KoGenStyle::PropertyType para = KoGenStyle::ParagraphType;
    // fo:background-color
    // fo:border
    // fo:border-bottom
    // fo:border-left
    // fo:border-right
    // fo:border-top
    // fo:break-after
    // fo:break-before
    // fo:hyphenation-keep
    // fo:hyphenation-ladder-count
    // fo:keep-together
    // fo:keep-with-next
    // fo:line-height
    style.addProperty("fo:line-height", processParaSpacing(pf.lineSpacing(), fs, true), para);
    // fo:margin
    // fo:margin-bottom
    style.addProperty("fo:margin-bottom", processParaSpacing(pf.spaceAfter(), fs, false), para);
    // fo:margin-left
    if (m_isList) {
        style.addProperty("fo:margin-left", "0cm", para);
    } else {
        style.addProperty("fo:margin-left", pptMasterUnitToCm(pf.leftMargin()), para);
    }
    // fo:margin-right
    style.addProperty("fo:margin-right", "0cm", para);
    // fo:margin-top
    style.addProperty("fo:margin-top", processParaSpacing(pf.spaceBefore(), fs, false), para);
    // fo:orphans
    // fo:padding
    // fo:padding-bottom
    // fo:padding-left
    // fo:padding-right
    // fo:padding-top
    // fo:text-align
    const QString align = textAlignmentToString(pf.textAlignment());
    if (!align.isEmpty()) {
        style.addProperty("fo:text-align", align, para);
    }
    // fo:text-align-last
    // fo:text-indent
    quint16 indent = pf.indent();
    // NOTE: MS PowerPoint UI - Setting the indent value for the paragraph at
    // level ZERO has no effect, however the set vale is stored.
    if (!pf.level()) {
        indent = 0;
    }
    if (!m_isList) {
        style.addProperty("fo:text-indent", pptMasterUnitToCm(indent - pf.leftMargin()), para);
    } else {
        //text:space-before already set in style:list-level-properties
        style.addProperty("fo:text-indent", "0cm", para);
    }
    // fo:widows
    // style:auto-text-indent
    // style:background-transparency
    // style:border-line-width
    // style:border-line-width-bottom
    // style:border-line-width-left
    // style:border-line-width-right
    // style:border-line-width-top
    // style:font-independent-line-spacing
    style.addProperty("style:font-independent-line-spacing",
                      (pf.lineSpacing() >= 0) ? "true" : "false", para);
    // style:justify-single-word
    // style:line-break
    // style:line-height-at-least
    // style:line-spacing
    // style:page-number
    // style:punctuation-wrap
    // style:register-true
    // style:shadow
    // style:snap-to-layout-grid
    // style:tab-stop-distance
    // style:text-autospace
    // style:vertical-align
    // style:writing-mode
    // style:writing-mode-automatic
    // text:line-number
    // text:number-lines
} //end defineParagraphProperties()

void PptToOdp::defineDrawingPageStyle(KoGenStyle& style, const DrawStyle& ds, KoGenStyles& styles,
                                      ODrawToOdf& odrawtoodf, const MSO::HeadersFootersAtom* hf,
                                      const MSO::SlideFlags* sf)
{
    const KoGenStyle::PropertyType dp = KoGenStyle::DrawingPageType;

    // Inherit the background of the main master slide/title master slide or
    // notes master slide if slideFlags/fMasterBackground == true.  The
    // drawing-page style defined in the <master-page> will be used.
    if (!sf || (sf && !sf->fMasterBackground)) {

        // fFilled - a boolean property which specifies whether fill of the shape
        // is render based on the properties of the "fill style" property set.
        if (ds.fFilled()) {
            // draw:background-size ("border", or "full")
            style.addProperty("draw:background-size", ds.fillUseRect() ?"border" :"full", dp);
            // draw:fill ("bitmap", "gradient", "hatch", "none" or "solid")
            quint32 fillType = ds.fillType();
            style.addProperty("draw:fill", getFillType(fillType), dp);
            // draw:fill-color
            switch (fillType) {
            case msofillSolid:
            {
                QColor color = odrawtoodf.processOfficeArtCOLORREF(ds.fillColor(), ds);
                style.addProperty("draw:fill-color", color.name(), dp);
                break;
            }
            // draw:fill-gradient-name
            case msofillShade:
            case msofillShadeCenter:
            case msofillShadeShape:
            case msofillShadeScale:
            case msofillShadeTitle:
            {
                KoGenStyle gs(KoGenStyle::LinearGradientStyle);
                odrawtoodf.defineGradientStyle(gs, ds);
                QString gname = styles.insert(gs);
                style.addProperty("draw:fill-gradient-name", gname, dp);
                break;
            }
            // draw:fill-hatch-name
            // draw:fill-hatch-solid
            // draw:fill-image-height
            // draw:fill-image-name
            case msofillPattern:
            case msofillTexture:
            case msofillPicture:
            {
                quint32 fillBlip = ds.fillBlip();
                const QString fillImagePath = getPicturePath(fillBlip);
                if (!fillImagePath.isEmpty()) {
                    style.addProperty("draw:fill-image-name",
                                      "fillImage" + QString::number(fillBlip), dp);
                    style.addProperty("style:repeat", getRepeatStyle(fillType), dp);
                }
                break;
            }
            //TODO:
            case msofillBackground:
            default:
                break;
            }
            // draw:fill-image-ref-point-x
            // draw:fill-image-ref-point-y
            // draw:fill-image-ref-point
            // draw:fill-image-width
            // draw:gradient-step-count
            // draw:opacity-name
            // draw:opacity
            style.addProperty("draw:opacity",
                              percent(100.0 * toQReal(ds.fillOpacity())), dp);
            // draw:secondary-fill-color
            // draw:tile-repeat-offset
            // style:repeat // handled for image see draw:fill-image-name
        } else {
            style.addProperty("draw:fill", "none", dp);
        }
    }
    // presentation:background-objects-visible
    if (sf && !sf->fMasterObjects) {
        style.addProperty("presentation:background-objects-visible", false);
    } else {
        style.addProperty("presentation:background-objects-visible", true);
    }
    // presentation:background-visible
    style.addProperty("presentation:background-visible", true);
    // presentation:display-date-time
    if (hf) {
        style.addProperty("presentation:display-date-time",
                          hf->fHasDate, dp);
    }
    // presentation:display-footer
    if (hf) {
        style.addProperty("presentation:display-footer",
                          hf->fHasFooter, dp);
    }
    // presentation:display-header
    if (hf) {
        style.addProperty("presentation:display-header",
                          hf->fHasHeader, dp);
    }
    // presentation:display-page-number
    if (hf) {
        style.addProperty("presentation:display-page-number",
                          hf->fHasSlideNumber, dp);
    }
    // presentation:duration
    // presentation:transition-speed
    // presentation:transition-style
    // presentation:transition-type
    // presentation:visibility
    // svg:fill-rule
    // smil:direction
    // smil:fadeColor
    // smil:subtype
    // smil:type
} //end defineDrawingPageStyle()

void PptToOdp::defineListStyle(KoGenStyle& style,
                               const quint32 textType,
                               const TextMasterStyleAtom& levels,
                               const TextMasterStyle9Atom* levels9,
                               const TextMasterStyle10Atom* levels10)
{
    if (levels.lstLvl1) {
        defineListStyle(style, 0, textType,
                        levels.lstLvl1.data(),
                        ((levels9) ?levels9->lstLvl1.data() :0),
                        ((levels10) ?levels10->lstLvl1.data() :0));
    }
    if (levels.lstLvl2) {
        defineListStyle(style, 1, textType,
                        levels.lstLvl2.data(),
                        ((levels9) ?levels9->lstLvl2.data() :0),
                        ((levels10) ?levels10->lstLvl2.data() :0));
    }
    if (levels.lstLvl3) {
        defineListStyle(style, 2, textType,
                        levels.lstLvl3.data(),
                        ((levels9) ?levels9->lstLvl3.data() :0),
                        ((levels10) ?levels10->lstLvl3.data() :0));
    }
    if (levels.lstLvl4) {
        defineListStyle(style, 3, textType,
                        levels.lstLvl4.data(),
                        ((levels9) ?levels9->lstLvl4.data() :0),
                        ((levels10) ?levels10->lstLvl4.data() :0));
    }
    if (levels.lstLvl5) {
        defineListStyle(style, 4, textType,
                        levels.lstLvl5.data(),
                        ((levels9) ?levels9->lstLvl5.data() :0),
                        ((levels10) ?levels10->lstLvl5.data() :0));
    }
}

void PptToOdp::defineListStyle(KoGenStyle& style,
                               const quint32 textType,
                               const quint16 indentLevel,
                               const TextMasterStyleLevel* level,
                               const TextMasterStyle9Level* level9,
                               const TextMasterStyle10Level* level10)
{
    PptTextPFRun pf(p->documentContainer, level, level9, textType, indentLevel);
    PptTextCFRun cf(p->documentContainer, level, level9, indentLevel);
    ListStyleInput info(pf, cf);

    info.cf9 = (level9) ?&level9->cf9 :0;
    info.cf10 = (level10) ?&level10->cf10 :0;
    defineListStyle(style, indentLevel, info);
}

namespace
{
QChar
getBulletChar(const PptTextPFRun& pf)
{
    quint16 v = (quint16) pf.bulletChar();
//     if ((v == 0xf06c) || (v == 0x006c)) { // 0xF06C from "Windings" is similar to ●
//         return QChar(0x25cf); //  "●"
//     }
//     if (v == 0xf02d) { // 0xF02D from "Symbol" is similar to –
//         return QChar(0x2013);
//     }
//     if (v == 0xf0e8) { // 0xF0E8 is similar to ➔
//         return QChar(0x2794);
//     }
//     if (v == 0xf0d8) { // 0xF0D8 is similar to ➢
//         return QChar(0x27a2);
//     }
//     if (v == 0xf0fb) { // 0xF0FB is similar to ✗
//         return QChar(0x2717);
//     }
//     if (v == 0xf0fc) { // 0xF0FC is similar to ✔
//         return QChar(0x2714);
//     }
    return QChar(v);
//     return QChar(0x25cf); //  "●"
}

/**
 * Convert bulletSize value.
 *
 * BulletSize is a 2-byte signed integer that specifies the bullet font size.
 * It must be a value from the following intervals:
 *
 * x = value, x in <25, 400>, specifies bullet font size as a percentage of the
 * font size of the first text run in the paragraph.
 *
 * x in <-4000, -1>, The absolute value specifies the bullet font size in pt.
 *
 * @param value to convert
 * @return processed value in points or percentage
 */
QString bulletSizeToSizeString(qint16 value)
{
    QString ret;
    if (value >= 25 && value <= 400) {
        ret = percent(value);
    } else if ((value >= -4000) && (value <= -1)) {
        ret = pt(qAbs(value));
    } else {
        ret = percent(100);
    }
    return ret;
}
} //namespace
void PptToOdp::defineListStyleProperties(KoXmlWriter& out, bool imageBullet, const QString& bulletSize,
                               const PptTextPFRun& pf) {
    style_list_level_properties list_level_properties(&out);

    if (imageBullet) {
        QString pictureSize = bulletSize;
        if (pictureSize.endsWith(QLatin1Char('%'))) {
            pictureSize.chop(1);
            bool ok = false;
            qreal size = pictureSize.toDouble(&ok);
            if (!ok) {
                debugPpt << "defineBulletStyle: error converting" << pictureSize << "to double";
            }
            size = m_firstChunkFontSize * size / 100.0;
            pictureSize = pt(size);
        }

        // fo:text-align
        // fo:height
        list_level_properties.set_fo_height(pictureSize);
        // fo:width
        list_level_properties.set_fo_width(pictureSize);
        // style:font-name
        // style:vertical-pos
        list_level_properties.set_style_vertical_pos("middle");
        // style:vertical-rel
        list_level_properties.set_style_vertical_rel("line");
        // svg:x
        // svg:y
    }
    quint16 indent = pf.indent();
    // text:min-label-distance
    // text:min-label-width
    list_level_properties.set_text_min_label_width(pptMasterUnitToCm(pf.leftMargin() - indent));
    // text:space-before
    list_level_properties.set_text_space_before(pptMasterUnitToCm(indent));
}

void PptToOdp::defineListStyleTextProperties(KoXmlWriter& out, const QString& bulletSize,
                                             const PptTextPFRun& pf) {

    //---------------------------------------------
    // text-properties
    //---------------------------------------------

    KoGenStyle ts(KoGenStyle::TextStyle);
    const KoGenStyle::PropertyType text = KoGenStyle::TextType;

    //bulletSize already processed
    ts.addProperty("fo:font-size", bulletSize, text);

    //default value doesn't make sense
    QColor color;
    if (pf.fBulletHasColor()) {
        color = toQColor(pf.bulletColor());
        if (color.isValid()) {
            ts.addProperty("fo:color", color.name(), text);
        }
    }

    const MSO::FontEntityAtom* font = 0;

    //MSPowerPoint: UI does NOT enable to change font of a
    //numbered lists label.
    if (pf.fBulletHasFont() && !pf.fBulletHasAutoNumber()) {
        font = getFont(pf.bulletFontRef());
    }

    //A list label should NOT inherit a symbol font.
    if (!font && m_firstChunkSymbolAtStart) {
        font = getFont(m_firstChunkFontRef);
    }

    if (font) {
        QString family = QString::fromUtf16(font->lfFaceName.data(), font->lfFaceName.size());
        ts.addProperty("fo:font-family", family, text);
    }

    //MSPowerPoint: A label does NOT inherit Underline from
    //text-properties of the 1st text chunk.  A bullet does NOT
    //inherit properties in {Italics, Bold}.
    if (!pf.fBulletHasAutoNumber()) {
        ts.addProperty("fo:font-style", "normal");
        ts.addProperty("fo:font-weight", "normal");
    }
    ts.addProperty("style:text-underline-style", "none");

    ts.writeStyleProperties(&out, text);
}

void PptToOdp::defineListStyle(KoGenStyle& style, const quint16 depth,
                               const ListStyleInput& i)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter out(&buffer);

    QString bulletSize;
    if (i.pf.bulletSize()) {
        bulletSize = bulletSizeToSizeString(i.pf.bulletSize());
    } else {
        bulletSize = pt(m_firstChunkFontSize);
    }

    QString elementName;
    bool imageBullet = false;
    imageBullet = i.pf.bulletBlipRef() != 65535;

    if (imageBullet) {
        elementName = "text:list-level-style-image";
        text_list_level_style_image image(&out, depth + 1);
        image.set_xlink_href(urlFromArg(bulletPictureNames.value(i.pf.bulletBlipRef())));
        image.set_xlink_type("simple");
        defineListStyleProperties(out, imageBullet, bulletSize, i.pf);
    }
    else if (i.pf.fBulletHasAutoNumber() || i.pf.fHasBullet()) {

        QString numFormat("1"), numSuffix, numPrefix;
        processTextAutoNumberScheme(i.pf.scheme(), numFormat, numSuffix, numPrefix);

        // If there is no bulletChar or the bullet has autonumbering explicitly
        // we assume it's a numbered list
        if (i.pf.fBulletHasAutoNumber() || i.pf.bulletChar() == 0) {
            elementName = "text:list-level-style-number";
            text_list_level_style_number number(&out, depth + 1);
            if (!numFormat.isNull()) {
                number.set_style_num_format(numFormat);
            }
            // style:display-levels
            number.set_text_start_value(i.pf.startNum());

            if (!numPrefix.isNull()) {
                number.set_style_num_prefix(numPrefix);
            }
            if (!numSuffix.isNull()) {
                number.set_style_num_suffix(numSuffix);
            }
            defineListStyleProperties(out, imageBullet, bulletSize, i.pf);
            defineListStyleTextProperties(out, bulletSize, i.pf);
        } else {
            elementName = "text:list-level-style-bullet";
            text_list_level_style_bullet bullet(&out, getBulletChar(i.pf), depth + 1);
            defineListStyleProperties(out, imageBullet, bulletSize, i.pf);
            defineListStyleTextProperties(out, bulletSize, i.pf);
            // text:bullet-relative-size
        }
    }
    //no bullet exists (i.pf.fHasBullet() == false)
    else {
        elementName = "text:list-level-style-number";
        text_list_level_style_number number(&out, depth + 1);
        number.set_style_num_format("");
        defineListStyleProperties(out, imageBullet, bulletSize, i.pf);
        defineListStyleTextProperties(out, bulletSize, i.pf);
    }

    // serialize the text:list-style element into the properties
    QString contents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement(elementName, contents);
} //end defineListStyle()

template<class O>
void handleOfficeArtContainer(O& handler, const OfficeArtSpgrContainerFileBlock& c) {
    const OfficeArtSpContainer* a = c.anon.get<OfficeArtSpContainer>();
    const OfficeArtSpgrContainer* b= c.anon.get<OfficeArtSpgrContainer>();
    if (a) {
        handler.handle(*a);
    } else {
        foreach (const OfficeArtSpgrContainerFileBlock& fb, b->rgfb) {
            handleOfficeArtContainer(handler, fb);
        }
    }
}
template<class O>
void handleOfficeArtContainer(O& handler, const MSO::OfficeArtDgContainer& c) {
    if (c.shape) {
        handler.handle(*c.shape);
    }
    if (c.groupShape) {
        foreach (const OfficeArtSpgrContainerFileBlock& fb, c.groupShape->rgfb) {
            handleOfficeArtContainer(handler, fb);
        }
    }
}

class PlaceholderFinder {
public:
    quint32 wanted;
    const MSO::OfficeArtSpContainer* sp;
    PlaceholderFinder(int w) :wanted(w), sp(0) {}
    void handle(const MSO::OfficeArtSpContainer& o) {
        if (o.clientTextbox) {
            const PptOfficeArtClientTextBox* b
                    = o.clientTextbox->anon.get<PptOfficeArtClientTextBox>();
            if (b) {
                foreach (const TextClientDataSubContainerOrAtom& a, b->rgChildRec) {
                    const TextContainer* tc = a.anon.get<TextContainer>();
                    if (tc && tc->textHeaderAtom.textType == wanted) {
                        if (sp) {
                            debugPpt << "Already found a placeholder with the right type " << wanted;
                        } else {
                            sp = &o;
                        }
                    }
                }
            }
        }
    }
};
void PptToOdp::defineMasterStyles(KoGenStyles& styles)
{
    foreach (const MSO::MasterOrSlideContainer* m, p->masters) {
        m_currentMaster = m;
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();

        // look for a style for each of the values of TextEnumType
        for (quint16 texttype = 0; texttype <= 8; ++texttype) {
            // look for placeholder with the right texttype
            PlaceholderFinder finder(texttype);
            if (sc) {
                handleOfficeArtContainer(finder, sc->drawing.OfficeArtDg);
            } else if (mm) {
                handleOfficeArtContainer(finder, mm->drawing.OfficeArtDg);
            }
            if (finder.sp) {
                QBuffer buffer;
                KoXmlWriter dummy(&buffer);
                Writer w(dummy, styles, true);
                DrawClient drawclient(this);
                ODrawToOdf odrawtoodf(drawclient);
                odrawtoodf.addGraphicStyleToDrawElement(w, *finder.sp);
            }
        }
        // if no style for Tx_TYPE_CENTERTITLE (6) has been defined yet,
        // derive it from Tx_TYPE_TITLE (0)
        if (!masterPresentationStyles[m].contains(6)
                && masterPresentationStyles[m].contains(0)) {
            KoGenStyle style(KoGenStyle::PresentationStyle, "presentation");
            style.setParentName(masterPresentationStyles[m][0]);
            style.addProperty("fo:text-align", "center",
                              KoGenStyle::ParagraphType);
            style.addProperty("style:vertical-align", "middle",
                              KoGenStyle::ParagraphType);
            masterPresentationStyles[m][6] = styles.insert(style);
        }
        // if no style for Tx_TYPE_CENTERBODY (5) has been defined yet,
        // derive it from Tx_TYPE_BODY (1)
        if (!masterPresentationStyles[m].contains(5)
                && masterPresentationStyles[m].contains(1)) {
            KoGenStyle style(KoGenStyle::PresentationStyle, "presentation");
            style.setParentName(masterPresentationStyles[m][1]);
            style.addProperty("fo:text-align", "center",
                              KoGenStyle::ParagraphType);
//            style.addProperty("style:vertical-align", "middle",
//                              KoGenStyle::ParagraphType);
            masterPresentationStyles[m][5] = styles.insert(style);
        }
    }
    m_currentMaster = nullptr;
}

void PptToOdp::defineAutomaticDrawingPageStyles(KoGenStyles& styles)
{
    DrawClient drawclient(this);
    ODrawToOdf odrawtoodf(drawclient);

    // define for master for use in <master-page style:name="...">
    foreach (const MSO::MasterOrSlideContainer* m, p->masters) {
        KoGenStyle dp(KoGenStyle::DrawingPageAutoStyle, "drawing-page");
        dp.setAutoStyleInStylesDotXml(true);
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
        const HeadersFootersAtom* hf = 0;
        const OfficeArtSpContainer* scp = getMasterShape(m);
        if (sc) {
            if (sc->perSlideHFContainer) {
                hf = &sc->perSlideHFContainer->hfAtom;
            }
        } else if (mm) {
            if (mm->perSlideHeadersFootersContainer) {
                hf = &mm->perSlideHeadersFootersContainer->hfAtom;
            }
        }
        //NOTE: Use default values of properties, looks like in case of PPT the
        //OfficeArtDggContainer has to be ignored
        DrawStyle ds(0, scp);
        drawclient.setDrawClientData(m, 0, 0, 0);
        defineDrawingPageStyle(dp, ds, styles, odrawtoodf, hf);
        drawingPageStyles[m] = styles.insert(dp, "Mdp");
    }
    QString notesMasterPageStyle;
    if (p->notesMaster) {
        const HeadersFootersAtom* hf = 0;
        if (p->notesMaster->perSlideHFContainer) {
            hf = &p->notesMaster->perSlideHFContainer->hfAtom;
        } else if (p->notesMaster->perSlideHFContainer2) {
            hf = &p->notesMaster->perSlideHFContainer2->hfAtom;
        }
        KoGenStyle dp(KoGenStyle::DrawingPageAutoStyle, "drawing-page");
        dp.setAutoStyleInStylesDotXml(true);
        const OfficeArtDggContainer* drawingGroup
                = &p->documentContainer->drawingGroup.OfficeArtDgg;
        DrawStyle ds(drawingGroup,
                     p->notesMaster->drawing.OfficeArtDg.shape.data());
        drawclient.setDrawClientData(0, 0, p->notesMaster, 0);
        defineDrawingPageStyle(dp, ds, styles, odrawtoodf, hf);
        notesMasterPageStyle = styles.insert(dp, "Mdp");
        drawingPageStyles[p->notesMaster] = notesMasterPageStyle;
    }

    // TODO: define for handouts for use in <style:handout-master
    // style:name="...">

    // define for slides for use in <draw:page style:name="...">
    foreach (const MSO::SlideContainer* sc, p->slides) {
        KoGenStyle dp(KoGenStyle::DrawingPageAutoStyle, "drawing-page");
        dp.setAutoStyleInStylesDotXml(false);
        const MasterOrSlideContainer* m = p->getMaster(sc);
        const PerSlideHeadersFootersContainer* hfc = getPerSlideHF(sc);
        HeadersFootersAtom hf;

        if (hfc) {
            hf = hfc->hfAtom;
        } else {
            //Default values saved by MS Office 2003 require corrections.
            const SlideHeadersFootersContainer* dhfc = getSlideHF();
            if (dhfc) {
                hf = dhfc->hfAtom;
                if (hf.fHasUserDate && !dhfc->userDateAtom.data()) {
                    hf.fHasUserDate = false;
                }
                if (hf.fHasDate && !hf.fHasUserDate && !hf.fHasTodayDate) {
                    hf.fHasDate = false;
                }
                if (hf.fHasFooter && !dhfc->footerAtom.data()) {
                    hf.fHasFooter = false;
                }
            }
            //PerSlideHeadersFootersContainer and SlideHeadersFootersContainer
            //are both optional, use default values for the drawing-page style
            else {
                hf.fHasDate = hf.fHasTodayDate = hf.fHasUserDate = false;
                hf.fHasSlideNumber = hf.fHasHeader = hf.fHasFooter = false;
                hf.formatId = -1;
            }
	}
        const OfficeArtSpContainer* masterSlideShape
                = getMasterShape(m);
        const OfficeArtSpContainer* slideShape
                = sc->drawing.OfficeArtDg.shape.data();
        //NOTE: Use default values of properties, looks like in case of PPT the
        //OfficeArtDggContainer has to be ignored
        DrawStyle ds(0, masterSlideShape, slideShape);
        drawclient.setDrawClientData(m, sc, 0, 0);
        defineDrawingPageStyle(dp, ds, styles, odrawtoodf, &hf, &sc->slideAtom.slideFlags);
        drawingPageStyles[sc] = styles.insert(dp, "dp");
    }

    // define for notes for use in <presentation:notes style:name="...">
    foreach (const MSO::NotesContainer* nc, p->notes) {
        if (!nc) continue;
        const HeadersFootersAtom* hf = 0;
        if (nc->perSlideHFContainer) {
            hf = &nc->perSlideHFContainer->hfAtom;
        } else if (nc->perSlideHFContainer2) {
            hf = &nc->perSlideHFContainer2->hfAtom;
        }
        // TODO: derive from notes master slide style
        KoGenStyle dp(KoGenStyle::DrawingPageAutoStyle, "drawing-page");
        dp.setAutoStyleInStylesDotXml(false);
        const OfficeArtDggContainer* drawingGroup
                = &p->documentContainer->drawingGroup.OfficeArtDgg;
        DrawStyle ds(drawingGroup, nc->drawing.OfficeArtDg.shape.data());
        drawclient.setDrawClientData(0, 0, p->notesMaster, nc);
        defineDrawingPageStyle(dp, ds, styles, odrawtoodf, hf, &nc->notesAtom.slideFlags);
        drawingPageStyles[nc] = styles.insert(dp, "dp");
    }
} //end defineAutomaticDrawingPageStyles()

void PptToOdp::createMainStyles(KoGenStyles& styles)
{
    /* This function follows the flow of the styles.xml file.

       -> style:styles
       first, the global objects are looked up and defined.  This includes the
       style:presentation-page-layout elements.  Next, the default styles for
       the 12 style families are defined.

       -> style:automatic-styles
       After that, style:page-layout and automatic styles are defined

       -> office:master-styles
       At last, the master slides are defined
    */
    /*
       collect all the global objects into
       styles.xml/office:document-styles/office:styles
    */
    // TODO: draw:gradient
    // TODO: svg:linearGradient
    // TODO: svg:radialGradient
    // TODO: draw:hatch
    // draw:fill-image
    FillImageCollector fillImageCollector(styles, *this);
    collectGlobalObjects(fillImageCollector, *p);
    // draw:marker (libmso)
    // TODO: draw:stroke-dash
//     StrokeDashCollector strokeDashCollector(styles, *this);
//     collectGlobalObjects(strokeDashCollector, *p);
    // TODO: draw:opacity

    /*
       Define the style:presentation-page-layout elements.
    */
    // TODO:


    // Define default styles for some of the 12 style families.  No
    // default styles for the families 'text' and 'paragraph' are
    // defined, since these have higher precedence than the text and
    // paragraph settings for the other style families that may
    // contain text and paragraph settings, like 'graphic' and
    // 'presentation'.

    //defineDefaultTextStyle(styles);
    //defineDefaultParagraphStyle(styles);
    defineDefaultSectionStyle(styles);
    defineDefaultRubyStyle(styles);
    defineDefaultTableStyle(styles);
    defineDefaultTableColumnStyle(styles);
    defineDefaultTableRowStyle(styles);
    defineDefaultTableCellStyle(styles);
    defineDefaultPresentationStyle(styles);
    defineDefaultChartStyle(styles);

    if (m_progress_update) {
        (m_filter->*m_setProgress)(55);
    }

    // NOTE: Stage specific: default graphic style and
    // drawing-page style have higher precedence than those defined by
    // the corresponding <master-page> element.  This is the case when
    // the presentation slide inherits background objects from the
    // master slide.

//     defineDefaultGraphicStyle(styles);
//     defineDefaultDrawingPageStyle(styles);

    /*
       Define the standard list style
     */
    if (p->documentContainer) {
        KoGenStyle list(KoGenStyle::ListStyle);
        PptTextPFRun pf(p->documentContainer);
        PptTextCFRun cf(p->documentContainer);
        ListStyleInput info(pf, cf);
        defineListStyle(list, 0, info);
        styles.insert(list, "standardListStyle", KoGenStyles::DontAddNumberToName);
    }

    /*
       Define the style:page-layout elements, for ppt files there are only two.
     */
    slidePageLayoutName = definePageLayout(styles,
            p->documentContainer->documentAtom.slideSize);
    notesPageLayoutName = definePageLayout(styles,
            p->documentContainer->documentAtom.notesSize);

    /*
      Define the automatic styles
     */
    m_currentSlideTexts = 0;
    defineMasterStyles(styles);
    defineAutomaticDrawingPageStyles(styles);

    if (m_progress_update) {
        (m_filter->*m_setProgress)(60);
    }

    /*
      Define the draw:layer-set.
     */
    // TODO:

    /*
      Define the style:handout-master
     */
    // TODO:

    /*
      Define the style:master-pages
     */
    DrawClient drawclient(this);
    ODrawToOdf odrawtoodf(drawclient);

    QBuffer notesBuffer;
    if (p->notesMaster) { // draw the notes master
        notesBuffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&notesBuffer);
        Writer out(writer, styles, true);

        presentation_notes notes(&out.xml);
        notes.set_style_page_layout_name(notesPageLayoutName);
        notes.set_draw_style_name(drawingPageStyles[p->notesMaster]);
        m_currentMaster = 0;

        if (p->notesMaster->drawing.OfficeArtDg.groupShape) {
            const OfficeArtSpgrContainer& spgr = *(p->notesMaster->drawing.OfficeArtDg.groupShape).data();
            drawclient.setDrawClientData(0, 0, p->notesMaster, 0);
            odrawtoodf.processGroupShape(spgr, out);
        }
    }
    m_processingMasters = true;

    foreach (const MSO::MasterOrSlideContainer* m, p->masters) {
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
        const DrawingContainer* drawing = 0;
        if (sc) {
            drawing = &sc->drawing;
        } else if (mm) {
            drawing = &mm->drawing;
        }

        KoGenStyle master(KoGenStyle::MasterPageStyle);
        master.addAttribute("style:page-layout-name", slidePageLayoutName);
        master.addAttribute("draw:style-name", drawingPageStyles[m]);
        m_currentMaster = m;
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&buffer);
        Writer out(writer, styles, true);

        if (drawing->OfficeArtDg.groupShape) {
            const OfficeArtSpgrContainer& spgr = *(drawing->OfficeArtDg.groupShape).data();
            drawclient.setDrawClientData(m, 0, 0, 0);
            odrawtoodf.processGroupShape(spgr, out);
        }
        master.addChildElement("", QString::fromUtf8(buffer.buffer(),
                                                     buffer.buffer().size()));
        if (notesBuffer.buffer().size()) {
            master.addChildElement("presentation:notes",
                                   QString::fromUtf8(notesBuffer.buffer(),
                                                     notesBuffer.buffer().size()));
        }
        masterNames[m] = styles.insert(master, "M");
    }
    m_currentMaster = 0;
    m_processingMasters = false;

    // Creating dateTime class object
    if (getSlideHF()) {
        int dateTimeFomatId = getSlideHF()->hfAtom.formatId;
        bool hasTodayDate = getSlideHF()->hfAtom.fHasTodayDate;
        bool hasUserDate = getSlideHF()->hfAtom.fHasUserDate;
        dateTime = DateTimeFormat(dateTimeFomatId);
        dateTime.addDateTimeAutoStyles(styles, hasTodayDate, hasUserDate);
    }

    if (m_progress_update) {
        (m_filter->*m_setProgress)(70);
    }
} //end createMainStyles()

QByteArray PptToOdp::createContent(KoGenStyles& styles)
{
    QBuffer presentationBuffer;
    presentationBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter presentationWriter(&presentationBuffer);

    processDeclaration(&presentationWriter);

    Writer out(presentationWriter, styles);
    for (int c = 0; c < p->slides.size(); c++) {
        processSlideForBody(c, out);

        if (m_progress_update) {
            //consider progress interval (70, 100)
            qreal percentage = ((c + 1) / (float)p->slides.size()) * 100;
            int progress = 70 + (int)((percentage * 28) / 100);
            (m_filter->*m_setProgress)(progress);
        }
    }

    QByteArray contentData;
    QBuffer contentBuffer(&contentData);
    contentBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter contentWriter(&contentBuffer);

    contentWriter.startDocument("office:document-content");
    {
    office_document_content content(&contentWriter);
    content.addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    content.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    content.addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    content.addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    content.addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    content.addAttribute("xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0");
    content.addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    content.addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

    // office:automatic-styles
    styles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, &contentWriter);

    office_body body(content.add_office_body());
    office_presentation presentation(body.add_office_presentation());
    presentation.addCompleteElement(&presentationBuffer);
    }
    contentWriter.endDocument();
    return contentData;
}

QByteArray PptToOdp::createMeta()
{
    QByteArray metaData;
    QBuffer buff(&metaData);
    buff.open(QIODevice::WriteOnly);
    KoXmlWriter metaWriter(&buff);

    metaWriter.startDocument("office:document-meta");
    {
    office_document_meta document_meta(&metaWriter);
    document_meta.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    document_meta.addAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
    document_meta.addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    office_meta meta(document_meta.add_office_meta());

    const MSO::PropertySet &ps = p->summaryInfo.propertySet.propertySet1;

    for (uint i = 0; i < ps.numProperties; i++) {
        const QSharedPointer<CodePageString>& vt_lpstr = ps.property.at(i).vt_lpstr;
        if (vt_lpstr) {
            switch (ps.propertyIdentifierAndOffset.at(i).propertyIdentifier) {
            case PIDSI_TITLE:
                meta.add_dc_title().addTextNode(vt_lpstr->characters);
                break;
            case PIDSI_SUBJECT:
                meta.add_dc_subject().addTextNode(vt_lpstr->characters);
                break;
            case PIDSI_AUTHOR:
                meta.add_meta_initial_creator().addTextNode(vt_lpstr->characters);
                break;
            case PIDSI_KEYWORDS:
                meta.add_meta_keyword().addTextNode(vt_lpstr->characters);
                break;
            case PIDSI_COMMENTS:
                meta.add_dc_description().addTextNode(vt_lpstr->characters);
                break;
            case PIDSI_LASTAUTHOR:
                meta.add_dc_creator().addTextNode(vt_lpstr->characters);
                break;
            default:
                break;
            }
        }
    }
    }
    metaWriter.endDocument();
    return metaData;
}

QString PptToOdp::utf16ToString(const QVector<quint16> &data)
{
    return QString::fromUtf16(data.data(), data.size());
}

QPair<QString, QString> PptToOdp::findHyperlink(const quint32 id)
{
    QString friendly;
    QString target;

    if( !p->documentContainer->exObjList )
        return qMakePair(friendly, target);

    foreach(const ExObjListSubContainer &container,
            p->documentContainer->exObjList->rgChildRec) {
        // Search all ExHyperlinkContainers for specified id
        const ExHyperlinkContainer *hyperlink = container.anon.get<ExHyperlinkContainer>();
        if (hyperlink && hyperlink->exHyperlinkAtom.exHyperLinkId == id) {
            if (hyperlink->friendlyNameAtom) {
                friendly = utf16ToString(hyperlink->friendlyNameAtom->friendlyName);
            }
            if (hyperlink->targetAtom) {
                target = utf16ToString(hyperlink->targetAtom->target);
            }
            // TODO currently location is ignored. Location refers to
            // position within a file
        }
    }
    return qMakePair(friendly, target);
}

const TextCFRun *findTextCFRun(const StyleTextPropAtom& style, unsigned int pos)
{
    quint32 counter = 0;
    foreach(const TextCFRun& cf, style.rgTextCFRun) {
        if (pos >= counter && pos < counter + cf.count) {
            return &cf;
        }
        counter += cf.count;
    }
    return 0;
}

const TextPFRun *findTextPFRun(const StyleTextPropAtom& style, unsigned int pos)
{
    quint32 counter = 0;
    foreach(const TextPFRun& pf, style.rgTextPFRun) {
        if (pos >= counter && pos < counter + pf.count) {
            return &pf;
        }
    }
    return 0;
}

void
writeMeta(const TextContainerMeta& m, bool master, text_meta& meta)
{
    const SlideNumberMCAtom* a = m.meta.get<SlideNumberMCAtom>();
    const DateTimeMCAtom* b = m.meta.get<DateTimeMCAtom>();
    const GenericDateMCAtom* c = m.meta.get<GenericDateMCAtom>();
    const HeaderMCAtom* d = m.meta.get<HeaderMCAtom>();
    const FooterMCAtom* e = m.meta.get<FooterMCAtom>();
    const RTFDateTimeMCAtom* f = m.meta.get<RTFDateTimeMCAtom>();
    if (a) {
        meta.add_text_page_number();
    }
    if (b) {
        // TODO: datetime format
        meta.add_text_time();
    }
    if (c) {
        // TODO: datetime format
        if (master) {
            meta.add_presentation_date_time();
        } else {
            meta.add_text_date();
        }
    }
    if (d) {
        meta.add_presentation_header();
    }
    if (e) {
        meta.add_presentation_footer();
    }
    if (f) {
        // TODO
    }
}

template <class T>
int getMeta(const TextContainerMeta& m, const TextContainerMeta*& meta,
        const int start, int& end)
{
    const T* a = m.meta.get<T>();
    if (a) {
        if (a->position == start) {
            meta = &m;
        } else if (a->position > start && end > a->position) {
            end = a->position;
        }
    }
    return end;
}

void PptToOdp::addListElement(KoXmlWriter& out, const QString& listStyle,
                    ListStack& levels, quint16 level,
                    const PptTextPFRun &pf)
{
    levels.push(TextListTag(listStyle, out));
    text_list& list = *levels.last().list;
    if (!listStyle.isEmpty()) {
        list.set_text_style_name(listStyle);
    } else {
        debugPpt << "Warning: list style name not provided!";
    }
    if (pf.fBulletHasAutoNumber()) {
        QString xmlId = QString("lvl%1").arg(level);
        xmlId.append(QString("_%1").arg(qrand()));
        list.set_xml_id(xmlId);

        if (m_continueListNumbering.contains(level) &&
                m_continueListNumbering[level]) {
            list.set_text_continue_list(m_lvlXmlIdMap[level]);
        }
        m_lvlXmlIdMap[level] = xmlId;
    }

    text_list_item& item = levels.last().add_text_list_item();

    if (pf.fBulletHasAutoNumber()) {
        if (m_continueListNumbering.contains(level) &&
            (m_continueListNumbering[level] == false)) {
            item.set_text_start_value(pf.startNum());
        }
        m_continueListNumbering[level] = true;
    }

    // add styleless levels to get the right level of indentation
    while (levels.size() < level) {
        levels.push(TextListTag("", *levels.last().item));
        levels.last().add_text_list_item();
    }
}

int PptToOdp::processTextSpan(Writer& out, PptTextCFRun& cf, const MSO::TextContainer* tc,
                              const QString& text, const int start, int end, quint16* p_fs)
{
    if (!tc) {
        debugPpt << "processTextSpan: TextContainer missing!";
        return -1;
    }

    //num. of chars already formatted by this TextCFRun
    quint32 num = 0;

    const int count = cf.addCurrentCFRun(tc, start, num);
    *p_fs = cf.fontSize();

#ifdef DEBUG_PPTTOODP
    debugPpt << "(TextCFRun) num. of characters:" << count;
    debugPpt << "(TextCFRun) formatted characters:" << num;
    debugPpt << "(Text position) start:" << start << "| end:" << end;
    debugPpt << "font size:" << *p_fs;
#endif

    bool isSymbol = false;

    // detect symbol inside one character text chunk
    if ( end == 1 || count == 1 ) {
        QChar c = text.at(start);
        if ( c.category() == QChar::Other_PrivateUse ) {
            isSymbol = true;
        }
    }
    // detect first symbol inside of several characters text chunk
    else {
        QString substr = text.mid(start, (end - start));
        for (int i = 0; i < substr.length(); i++) {
            if ((substr.at(i)).category() == QChar::Other_PrivateUse) {
                if (i == 0) {
                    end = start + 1;
                    isSymbol = true;
                } else {
                    end = start + i;
                }
                break;
            }
        }
    }

    // TODO: There's no TextCFRun in case of TextCFExceptionAtom or
    // TextMasterStyleLevel, handle this case. (uzak)

    // NOTE: TextSIException not processed by defineTextProperties at
    // the moment, so let's keep it simple!  (uzak)
    const TextSIException* si = 0;

#ifdef SI_EXCEPTION_SUPPORT
    int i = 0;
    // get the right special info run
    const QList<TextSIRun>* tsi = 0;
    if (tc->specialinfo) {
        tsi = &tc->specialinfo->rgSIRun;
    }
    if (tc->specialinfo2) {
        tsi = &tc->specialinfo2->rgSIRun;
    }

    int siend = 0;
    if (tsi) {
        while (i < tsi->size()) {
            si = &(*tsi)[i].si;
            siend += (*tsi)[i].count;
            if (siend > start) {
                break;
            }
            i++;
        }
        if (i >= tsi->size()) {
            si = 0;
        }
    }
#endif
    // find a meta character
    const TextContainerMeta* meta = 0;
    for (int i = 0; i < tc->meta.size(); ++i) {
        const TextContainerMeta& m = tc->meta[i];
        end = getMeta<SlideNumberMCAtom>(m, meta, start, end);
        end = getMeta<DateTimeMCAtom>(m, meta, start, end);
        end = getMeta<GenericDateMCAtom>(m, meta, start, end);
        end = getMeta<HeaderMCAtom>(m, meta, start, end);
        end = getMeta<FooterMCAtom>(m, meta, start, end);
        end = getMeta<RTFDateTimeMCAtom>(m, meta, start, end);
    }

    //TODO: process bookmarks
    const TextBookmarkAtom* bookmark = 0;
#ifdef BOOKMARK_SUPPORT
    // find the right bookmark
    for (int i = 0; i < tc->bookmark.size(); ++i) {
        if (tc->bookmark[i].begin < start && tc->bookmark[i].end >= start) {
            bookmark = &tc->bookmark[i];
        }
    }
#endif

    // find the interactive atom
    const MouseClickTextInfo* mouseclick = 0;
    const MouseOverTextInfo* mouseover = 0;
    for (int i = 0; i < tc->interactive.size(); ++i) {
        const TextContainerInteractiveInfo& ti = tc->interactive[i];
        const MouseClickTextInfo *a = ti.interactive.get<MouseClickTextInfo>();
        const MouseOverTextInfo *b = ti.interactive.get<MouseOverTextInfo>();
        if (a && start >= a->text.range.begin && start < a->text.range.end) {
            mouseclick = a;
        }
        if (b && start >= b->text.range.begin && start < b->text.range.end) {
            mouseover = b;
        }
    }

    // determine the end of the range
#ifdef SI_EXCEPTION_SUPPORT
    if (si && siend < end) {
        end = siend;
    }
#endif
    if (meta) {
        end = start + 1; // meta is always one character
    }
    if (bookmark && bookmark->end < end) {
        end = bookmark->end;
    }
    if (mouseclick && mouseclick->text.range.end < end) {
        end = mouseclick->text.range.end;
    }
    if (mouseover && mouseover->text.range.end < end) {
        end = mouseover->text.range.end;
    }

    KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    defineTextProperties(style, cf, 0, 0, si, isSymbol);
    text_span span(&out.xml);
    span.set_text_style_name(out.styles.insert(style));

    // [MS-PPT]: exHyperlinkIdRef must be ignored unless action is in
    // {II_JumpAction, II_HyperlinkAction, II_CustomShowAction (0x7)}
    //
    // NOTE: Jumps to other slides and shows not supported atm.
    if (mouseclick) {
        const InteractiveInfoAtom *info = &mouseclick->interactive.interactiveInfoAtom;
        if (info->action != II_HyperlinkAction) {
            mouseclick = 0;
        }
    }
    if (mouseover) {
        const InteractiveInfoAtom *info = &mouseover->interactive.interactiveInfoAtom;
        if (info->action != II_HyperlinkAction) {
            mouseover = 0;
        }
    }

    QString href;
    if (mouseclick) {
        QPair<QString, QString> link = findHyperlink(
            mouseclick->interactive.interactiveInfoAtom.exHyperlinkIdRef);
        if (!link.second.isEmpty()) { // target
            href = link.second;
        } else if (!link.first.isEmpty()) {
            href = link.first;
        }
    } else if (mouseover) {
        QPair<QString, QString> link = findHyperlink(
            mouseover->interactive.interactiveInfoAtom.exHyperlinkIdRef);
        if (!link.second.isEmpty()) { // target
            href = link.second;
        } else if (!link.first.isEmpty()) {
            href = link.first;
        }
    } else {
        // count - specifies the number of characters of the
        // corresponding text to which current TextCFException apply
        if (count > 0) {
            int tmp = start + (count - num);
            // moved to left by one character in processTextForBody
            if (tmp <= end) {
                end = tmp;
            }
        }
    }

    if (meta) {
        if (!href.isNull()) {
            text_a a(span.add_text_a(urlFromArg(href)));
            text_meta m(a.add_text_meta());
            writeMeta(*meta, m_processingMasters, m);
        } else {
            text_meta m(span.add_text_meta());
            writeMeta(*meta, m_processingMasters, m);
        }
    } else {
        int len = end - start;
        const QString txt = text.mid(start, len).replace('\r', '\n').replace('\v', '\n');
        if (!href.isNull()) {
            text_a a(span.add_text_a(urlFromArg(href)));
            addTextSpan(a, txt);
        } else {
            addTextSpan(span, txt);
        }
    }

    return end;
} //end processTextSpan()

int PptToOdp::processTextSpans(Writer& out, PptTextCFRun& cf, const MSO::TextContainer* tc,
			       const QString& text, const int start, int end, quint16* p_fs)
{
    quint16 font_size = 0;
    int pos = start;

    //using the do while statement to catch empty line
    do {
        int r = processTextSpan(out, cf, tc, text, pos, end, &font_size);

        if (font_size < *p_fs) {
            *p_fs = font_size;
        }
        if (r < pos) {
            // some error
            debugPpt << "pos: " << pos << "| end: " << end << " r: " << r;
            return -2;
        }
        pos = r;
    } while (pos < end);
    return (pos == end) ?0 :-pos;
}

QString PptToOdp::defineAutoListStyle(Writer& out, const PptTextPFRun& pf, const PptTextCFRun& cf)
{
    KoGenStyle list(KoGenStyle::ListAutoStyle);
    list.setAutoStyleInStylesDotXml(out.stylesxml);
    ListStyleInput info(pf, cf);
    defineListStyle(list, pf.level(), info);
    return out.styles.insert(list);
}

void
PptToOdp::processParagraph(Writer& out,
                           ListStack& levels,
                           const MSO::OfficeArtClientData* clientData,
                           const MSO::TextContainer* tc,
                           const MSO::TextRuler* tr,
                           const bool isPlaceHolder,
                           const QString& text,
                           const int start,
                           int end)
{
    //TODO: support for notes master slide required!

    const QString substr = text.mid(start, (end - start));
#ifdef DEBUG_PPTTOODP
    debugPpt << "> current paragraph:" << substr;
    debugPpt << "> (hex):" << hex << substr.toUcs4() << dec;
#endif

    const PptOfficeArtClientData* pcd = 0;
    if (clientData) {
        pcd = clientData->anon.get<PptOfficeArtClientData>();
    }

    quint32 textType = tc->textHeaderAtom.textType;
    const MasterOrSlideContainer* m = 0;

    //Get the main master slide's MasterOrSlideContainer.  A common shape
    //(opposite of a placeholder) SHOULD contain text of type Tx_TYPE_OTHER,
    //but MS Office 2003 does not follow this rule.
    if (m_currentMaster && (isPlaceHolder || (textType != Tx_TYPE_OTHER))) {
        m  = m_currentMaster;
        while (m->anon.is<SlideContainer>()) {
            m = p->getMaster(m->anon.get<SlideContainer>());
        }
#ifdef DEBUG_PPTTOODP
        const MainMasterContainer* mc = m->anon.get<MainMasterContainer>();
        Q_ASSERT(mc->slideAtom.masterIdRef == 0);
#endif
    }

    //The current TextCFException located in the TextContainer will be
    //prepended to the list in the processTextSpan function.
    PptTextPFRun pf(p->documentContainer, m, m_currentSlideTexts, pcd, tc, tr, start);
    PptTextCFRun cf(p->documentContainer, m, tc, pf.level());

    //spans have to be processed first to prepare the correct ParagraphStyle
    QBuffer spans_buf;
    spans_buf.open(QIODevice::WriteOnly);
    KoXmlWriter writer(&spans_buf);
    Writer o(writer, out.styles, out.stylesxml);

    quint16 min_fontsize = FONTSIZE_MAX;
    processTextSpans(o, cf, tc, text, start, end, &min_fontsize);

    //NOTE: Process empty list items as paragraphs to prevent Stage
    //displaying those.
    m_isList = ( pf.isList() && (start < end) );

    if (m_isList) {
        int depth = pf.level() + 1;
        quint32 num = 0;

        //TextCFException of the 1st run of text required to specify
        //the label font-size in case not provided by TextPFException.
        cf.addCurrentCFRun(tc, start, num);
        m_firstChunkFontSize = cf.fontSize();
        m_firstChunkFontRef = cf.fontRef();
        cf.removeCurrentCFRun();

        //A list label should NOT inherit a symbol font.
        if ((substr.at(0)).category() == QChar::Other_PrivateUse) {
            m_firstChunkSymbolAtStart = true;
        } else {
            m_firstChunkSymbolAtStart = false;
        }

        QString listStyle = defineAutoListStyle(out, pf, cf);
        //check if we have the corresponding style for this level, if not then
        //close the list and create a new one (K.I.S.S.)
        if (!levels.isEmpty() && (levels.first().style != listStyle)) {
            levels.clear();
        }
        if (!pf.fBulletHasAutoNumber()) {
            QList<quint16> levels = m_continueListNumbering.keys();
            for (quint16 i = 0; i < levels.size(); ++i) {
                if (levels[i] >= depth) {
                    m_continueListNumbering.remove(levels[i]);
                    m_lvlXmlIdMap.remove(levels[i]);
                }
            }
        } else if (m_previousListLevel > depth) {
            QList<quint16> levels = m_continueListNumbering.keys();
            for (quint16 i = 0; i < levels.size(); ++i) {
                if (levels[i] > depth) {
                    m_continueListNumbering.remove(levels[i]);
                    m_lvlXmlIdMap.remove(levels[i]);
                }
            }
        }
        if (levels.isEmpty()) {
            addListElement(out.xml, listStyle, levels, depth, pf);
        } else {
            levels.last().add_text_list_item();
        }
        m_previousListLevel = depth;
    } else {
        levels.clear();
        m_continueListNumbering.clear();
        m_lvlXmlIdMap.clear();
        m_previousListLevel = 0;
    }

    KoGenStyle style(KoGenStyle::ParagraphAutoStyle, "paragraph");
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    defineParagraphProperties(style, pf, min_fontsize);
    //NOTE: Help text layout to apply correct line-height for empty lines.
    if (start == end) {
        defineTextProperties(style, cf, 0, 0, 0);
    }
    if (levels.isEmpty()) {
        text_p p(&out.xml);
        p.set_text_style_name(out.styles.insert(style));
        p.addCompleteElement(&spans_buf);
    } else {
        text_p p(levels.last().item->add_text_p());
        p.set_text_style_name(out.styles.insert(style));
        p.addCompleteElement(&spans_buf);
    }
} //end processParagraph()

int PptToOdp::processTextForBody(Writer& out, const MSO::OfficeArtClientData* clientData,
                                 const MSO::TextContainer* tc, const MSO::TextRuler* tr,
                                 const bool isPlaceholder)
{
    /* Text in a textcontainer is divided into sections.
       The sections occur on different levels:
       - paragraph (TextPFRun) 1-n characters
       - character (TextCFRun) 1-n characters
       - variables (TextContainerMeta) 1 character
       - spelling and language (TextSIRun) 1-n characters
       - links (TextContainerInteractiveInfo) 1-n characters
       - indentation (MasterTextPropRun) 1-n characters (ignored)

       Variables are the smallest level, they should be replaced by special
       xml elements.

       TextPFRuns correspond to text:list-item and text:p.
       MasterTextPropRun also corresponds to text:list-items too.
       TextCFRuns correspond to text:span elements as do
    */

    // If this is not a placeholder shape, then do not inherit text style from
    // master styles.
    //
    // NOTE: If slideFlags/fMasterScheme == true, master's color scheme MUST be
    // used.  Common shapes should not refer to a color scheme.

    if (!tc) {
        debugPpt << "MISSING TextContainer, big mess-up!";
        return -1;
    }

#ifdef DEBUG_PPTTOODP
    quint32 txt_type = tc->textHeaderAtom.textType;
    QString txt = getText(tc);
    int len = txt.length();
    txt.replace('\v', "<vt>");
    txt.replace('\r', "<cr>");
    txt.replace('\n', "<newline>");
    txt.replace('\t', "<tab>");
    txt.replace('\f', "<ff>");
    debugPpt << "\n> textType:" << txt_type;
    debugPpt << "> current text:" << txt << "| length:" << len;
#endif

    // Let's assume text stored in paragraphs.
    //
    // Example:<cr>text1<cr>text2<cr>text3<cr><cr><cr>
    // Result:
    // <text:p/>
    // <text:p><text:span>text1</text:span></text:p>
    // <text:p><text:span>text2</text:span></text:p>
    // <text:p><text:span>text3</text:span></text:p>
    // <text:p/>
    // <text:p/>
    // <text:p/>
    //
    // Example:text1<cr>text2<cr>text3
    // Result:
    // <text:p><text:span>text1</text:span></text:p>
    // <text:p><text:span>text2</text:span></text:p>
    // <text:p><text:span>text3</text:span></text:p>
    //
    // In addition, the text body contains a single terminating paragraph break
    // character (0x000D) that is not included in the TextCharsAtom record or
    // TextBytesAtom record.
    //
    const QString text = getText(tc).append('\r');
    static const QRegExp lineend("[\v\r]");
    qint32 pos = 0, end = 0;

    ListStack levels;
    levels.reserve(5);

    // loop over all the '\r' delimited lines
    while (pos < text.length()) {
        end = text.indexOf(lineend, pos);
        processParagraph(out, levels, clientData, tc, tr,
                         isPlaceholder,
                         text, pos, end);
        pos = end + 1;
    }

    return 0;
} //end processTextForBody()

void PptToOdp::processSlideForBody(unsigned slideNo, Writer& out)
{
    const SlideContainer* slide = p->slides[slideNo];
    const MasterOrSlideContainer* master = p->getMaster(slide);
    if (!master) return;
    int masterNumber = p->masters.indexOf(master);
    if (masterNumber == -1) return;

    QString nameStr;
    // take the slide name if present (usually it is not)
    if (slide->slideNameAtom) {
        nameStr = QString::fromUtf16(slide->slideNameAtom->slideName.data(),
                                     slide->slideNameAtom->slideName.size());
    }
    // look for a title on the slide
    if (nameStr.isEmpty()) {
        foreach(const TextContainer& tc, p->documentContainer->slideList->rgChildRec[slideNo].atoms) {
            if (tc.textHeaderAtom.textType == Tx_TYPE_TITLE) {
                nameStr = getText(&tc);
                break;
            }
        }
    }

    if (nameStr.isEmpty()) {
        nameStr = QString("page%1").arg(slideNo + 1);
    }

    nameStr.remove('\r');
    nameStr.remove('\v');

    QString value = masterNames.value(master);
    if (value.isEmpty()) {
        value = "unknown";
    }
    draw_page page(&out.xml, value);
    page.set_draw_name(nameStr);
    value = drawingPageStyles[slide];
    if (!value.isEmpty()) {
        page.set_draw_style_name(value);
    }
    //page.set_presentation_presentation_page_layout_name("AL1T0");

    const HeadersFootersAtom* headerFooterAtom = 0;
    if (master->anon.is<MainMasterContainer>()) {
        const MainMasterContainer* m = master->anon.get<MainMasterContainer>();
        if (m->perSlideHeadersFootersContainer) {
            headerFooterAtom = &m->perSlideHeadersFootersContainer->hfAtom;
        }
    } else {
        const SlideContainer* s = master->anon.get<SlideContainer>();
        if (s->perSlideHFContainer) {
            headerFooterAtom = &s->perSlideHFContainer->hfAtom;
        }
    }
    if (!headerFooterAtom && getSlideHF()) {
        headerFooterAtom = &getSlideHF()->hfAtom;
    }
    if (!usedDateTimeDeclaration.value(slideNo).isEmpty()) {
        page.set_presentation_use_date_time_name(
                    usedDateTimeDeclaration[slideNo]);
    }
    if (!usedHeaderDeclaration.value(slideNo).isEmpty()) {
        if (!usedHeaderDeclaration[slideNo].isEmpty())
            page.set_presentation_use_header_name(usedHeaderDeclaration[slideNo]);
    }
    if (!usedFooterDeclaration.value(slideNo).isEmpty()) {
        if (!usedFooterDeclaration[slideNo].isEmpty())
            page.set_presentation_use_footer_name(usedFooterDeclaration[slideNo]);
    }

    m_currentSlideTexts = &p->documentContainer->slideList->rgChildRec[slideNo];
    //TODO: try to avoid using those
    m_currentMaster = master;
    m_currentSlide = slide;

    DrawClient drawclient(this);
    ODrawToOdf odrawtoodf(drawclient);

    if (slide->drawing.OfficeArtDg.groupShape) {
        const OfficeArtSpgrContainer& spgr = *(slide->drawing.OfficeArtDg.groupShape).data();
        drawclient.setDrawClientData(master, slide, 0, 0, m_currentSlideTexts);
        odrawtoodf.processGroupShape(spgr, out);
    }

    m_currentMaster = nullptr;
    m_currentSlide = nullptr;

    if (slide->drawing.OfficeArtDg.shape) {
        // leave it out until it is understood
        //  processObjectForBody(*slide->drawing.OfficeArtDg.shape, out);
    }

    // draw the notes
    const NotesContainer* nc = p->notes[slideNo];
    if (nc && nc->drawing.OfficeArtDg.groupShape) {
        m_currentSlideTexts = 0;
        presentation_notes notes(page.add_presentation_notes());
        value = drawingPageStyles[nc];
        if (!value.isEmpty()) {
            notes.set_draw_style_name(value);
        }
        const OfficeArtSpgrContainer& spgr = *(nc->drawing.OfficeArtDg.groupShape).data();
        drawclient.setDrawClientData(0, 0, p->notesMaster, nc, m_currentSlideTexts);
        odrawtoodf.processGroupShape(spgr, out);
    }
} //end processSlideForBody()

QString PptToOdp::processParaSpacing(const int value,
                                     const quint16 fs,
                                     const bool percentage) const
{
    // ParaSpacing specifies text paragraph spacing.
    //
    // x = value; x in <0, 13200>, specifies spacing as a percentage of the
    // text line height.  x < 0, the absolute value specifies spacing in master
    // units.

    if (value < 0) {
        unsigned int temp = -value;
        return pptMasterUnitToCm(temp);
    }

    // NOTE: MS PowerPoint specific: font-independent-line-spacing is used,
    // which means that line height is calculated only from the font height as
    // specified by the font size properties.  If a number of font sizes are
    // used in a paragraph, then use the minimum.
    //
    // lineHeight = fontSize + (1/4 * fontSize);

    if (percentage) {
        return percent(value);
    } else {
        double height = fs + (0.25 * fs);
        return pt(qFloor(value * height / 100));
    }
}

QString PptToOdp::pptMasterUnitToCm(qint16 value) const
{
    qreal result = value;
    result *= 2.54;
    result /= 576;
    return cm(result);
}

QString PptToOdp::textAlignmentToString(unsigned int value) const
{
    switch (value) {
        /**
        Tx_ALIGNLeft            0x0000 For horizontal text, left aligned.
                                   For vertical text, top aligned.
        */
    case 0:
        return "left";
        /**
        Tx_ALIGNCenter          0x0001 For horizontal text, centered.
                                   For vertical text, middle aligned.
        */
    case 1:
        return "center";
        /**
        Tx_ALIGNRight           0x0002 For horizontal text, right aligned.
                                   For vertical text, bottom aligned.
        */
    case 2:
        return "right";

        /**
        Tx_ALIGNJustify         0x0003 For horizontal text, flush left and right.
                                   For vertical text, flush top and bottom.
        */
    case 3:
        return "justify";

        //TODO these were missing from ODF specification v1.1, but are
        //in [MS-PPT].pdf

        /**
        Tx_ALIGNDistributed     0x0004 Distribute space between characters.
        */
    case 4:

        /**
        Tx_ALIGNThaiDistributed 0x0005 Thai distribution justification.
        */
    case 5:

        /**
        Tx_ALIGNJustifyLow      0x0006 Kashida justify low.
        */
    case 6:
        return "";

        //TODO these two are in ODF specification v1.1 but are missing from
        //[MS-PPT].pdf
        //return "end";
        //return "start";
    }

    return QString();
}

QColor PptToOdp::toQColor(const ColorIndexStruct &color)
{
    QColor ret;

    // MS-PPT 2.12.2 ColorIndexStruct
    if (color.index == 0xFE) {
        return QColor(color.red, color.green, color.blue);
    }
    if (color.index == 0xFF) { // color is undefined
        return ret;
    }

    const QList<ColorStruct>* colorScheme = nullptr;
    const MSO::MasterOrSlideContainer* m = m_currentMaster;
    const MSO::MainMasterContainer* mmc = nullptr;
    const MSO::SlideContainer* tmc = nullptr;
    const MSO::SlideContainer* sc = m_currentSlide;

    //TODO: hande the case of a notes master slide/notes slide pair
//     const MSO::NotesContainer* nmc = nullptr;
//     const MSO::NotesContainer* nc = nullptr;

//     if (m) {
//         if (m->anon.is<MainMasterContainer>()) {
//             mmc = m->anon.get<MainMasterContainer>();
//             colorScheme = &mmc->slideSchemeColorSchemeAtom.rgSchemeColor;
//         } else if (m->anon.is<SlideContainer>()) {
//             tmc = m->anon.get<SlideContainer>();
//             colorScheme = &tmc->slideSchemeColorSchemeAtom.rgSchemeColor;
//         }
//     }

    //a title master slide does not provide any additional text formatting
    //information, use it's master's color scheme
    while (m) {
        //masterIdRef MUST be 0x00000000 if the record that contains this
        //SlideAtom record is a MainMasterContainer record (MS-PPT 2.5.10)
        if (m->anon.is<SlideContainer>()) {
            m = p->getMaster(m->anon.get<SlideContainer>());
        } else {
            mmc = m->anon.get<MainMasterContainer>();
            colorScheme = &mmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            m = nullptr;
        }
    }

    if (sc) {
        if (!sc->slideAtom.slideFlags.fMasterScheme) {
            colorScheme = &sc->slideSchemeColorSchemeAtom.rgSchemeColor;
        }
    }
    if (!colorScheme) {
        //NOTE: Using color scheme of the first main master/title master slide
        if (p->masters[0]->anon.is<MainMasterContainer>()) {
            mmc = p->masters[0]->anon.get<MainMasterContainer>();
            colorScheme = &mmc->slideSchemeColorSchemeAtom.rgSchemeColor;
        }
        else if (p->masters[0]->anon.is<SlideContainer>()) {
            tmc = p->masters[0]->anon.get<SlideContainer>();
            colorScheme = &tmc->slideSchemeColorSchemeAtom.rgSchemeColor;
        }
        if (!colorScheme) {
            warnPpt << "Warning: Invalid color scheme! Returning an invalid color!";
            return ret;
        }
    }
    if (colorScheme->size() <= color.index) {
        warnPpt << "Warning: Incorrect size of rgSchemeColor! Returning an invalid color!";
    } else {
        const ColorStruct cs = colorScheme->at(color.index);
        ret = QColor(cs.red, cs.green, cs.blue);
    }

    return ret;
} //end toQColor(const ColorIndexStruct)

QColor PptToOdp::toQColor(const MSO::OfficeArtCOLORREF& c,
                          const MSO::StreamOffset* master, const MSO::StreamOffset* common)
{
    QColor ret;

    //fSchemeIndex - A bit that specifies whether the current application
    //defined color scheme will be used to determine the color (MS-ODRAW)
    if (c.fSchemeIndex) {

        const QList<ColorStruct>* colorScheme = nullptr;
        const MSO::MainMasterContainer* mmc = nullptr;
        const MSO::SlideContainer* tmc = nullptr;
        const MSO::SlideContainer* sc = nullptr;
        const MSO::NotesContainer* nmc = nullptr;
        const MSO::NotesContainer* nc = nullptr;

        // Get the color scheme of the current main master/title master or
        // notes master slide.
        if (master) {
            MSO::StreamOffset* m = const_cast<MSO::StreamOffset*>(master);
            if ((mmc = dynamic_cast<MSO::MainMasterContainer*>(m))) {
                colorScheme = &mmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            } else if ((nmc = dynamic_cast<MSO::NotesContainer*>(m))) {
                colorScheme = &nmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            } else if ((tmc = dynamic_cast<MSO::SlideContainer*>(m))) {
                colorScheme = &tmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            } else {
                warnPpt << "Warning: Incorrect container!";
            }
        }
        // Get the color scheme of the current presentation slide or notes
        // slide.  If fMasterScheme == true use master's color scheme.
        if (common) {
            MSO::StreamOffset* c = const_cast<MSO::StreamOffset*>(common);
	    if ((sc = dynamic_cast<MSO::SlideContainer*>(c))) {
                if (!sc->slideAtom.slideFlags.fMasterScheme) {
                    colorScheme = &sc->slideSchemeColorSchemeAtom.rgSchemeColor;
                }
	    } else if ((nc = dynamic_cast<MSO::NotesContainer*>(c))) {
                if (!nc->notesAtom.slideFlags.fMasterScheme) {
                    colorScheme = &nc->slideSchemeColorSchemeAtom.rgSchemeColor;
                }
	    } else {
                warnPpt << "Warning: Incorrect container! Provide SlideContainer of NotesContainer.";
            }
        }
        if (!colorScheme) {
            //NOTE: Using color scheme of the first main master/title master slide
            if (p->masters[0]->anon.is<MainMasterContainer>()) {
                mmc = p->masters[0]->anon.get<MainMasterContainer>();
                colorScheme = &mmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            }
            else if (p->masters[0]->anon.is<SlideContainer>()) {
                tmc = p->masters[0]->anon.get<SlideContainer>();
                colorScheme = &tmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            }
            if (!colorScheme) {
                warnPpt << "Warning: Invalid color scheme! Returning an invalid color!";
                return ret;
            }
        }
        // Use the red color channel's value as index according to MS-ODRAW
        if (colorScheme->size() <= c.red) {
            warnPpt << "Warning: Incorrect size of rgSchemeColor! Returning an invalid color!";
            return ret;
        } else {
            const ColorStruct cs = colorScheme->value(c.red);
            ret = QColor(cs.red, cs.green, cs.blue);
        }
    } else {
        ret = QColor(c.red, c.green, c.blue);
    }
    return ret;
} //end toQColor()

void PptToOdp::processTextAutoNumberScheme(int val, QString& numFormat, QString& numSuffix, QString& numPrefix)
{
    switch (val) {

    //Example: a., b., c., ...Lowercase Latin character followed by a period.
    case ANM_AlphaLcPeriod:
        numFormat = 'a';
        numSuffix = '.';
        break;

    //Example: A., B., C., ...Uppercase Latin character followed by a period.
    case ANM_AlphaUcPeriod:
        numFormat = 'A';
        numSuffix = '.';
        break;

    //Example: 1), 2), 3), ...Arabic numeral followed by a closing parenthesis.
    case ANM_ArabicParenRight:
        numFormat = '1';
        numSuffix = ')';
        break;

    //Example: 1., 2., 3., ...Arabic numeral followed by a period.
    case ANM_ArabicPeriod:
        numFormat = '1';
        numSuffix = '.';
        break;

    //Example: (i), (ii), (iii), ...Lowercase Roman numeral enclosed in
    //parentheses.
    case ANM_RomanLcParenBoth:
        numPrefix = '(';
        numFormat = 'i';
        numSuffix = ')';
        break;

    //Example: i), ii), iii), ... Lowercase Roman numeral followed by a closing
    //parenthesis.
    case ANM_RomanLcParenRight:
        numFormat = 'i';
        numSuffix = ')';
        break;

    //Example: i., ii., iii., ...Lowercase Roman numeral followed by a period.
    case ANM_RomanLcPeriod:
        numFormat = 'i';
        numSuffix = '.';
        break;

    //Example: I., II., III., ...Uppercase Roman numeral followed by a period.
    case ANM_RomanUcPeriod:
        numFormat = 'I';
        numSuffix = '.';
        break;

    //Example: (a), (b), (c), ...Lowercase alphabetic character enclosed in
    //parentheses.
    case ANM_AlphaLcParenBoth:
        numPrefix = '(';
        numFormat = 'a';
        numSuffix = ')';
        break;

    //Example: a), b), c), ...Lowercase alphabetic character followed by a
    //closing
    case ANM_AlphaLcParenRight:
        numFormat = 'a';
        numSuffix = ')';
        break;

    //Example: (A), (B), (C), ...Uppercase alphabetic character enclosed in
    //parentheses.
    case ANM_AlphaUcParenBoth:
        numPrefix = '(';
        numFormat = 'A';
        numSuffix = ')';
        break;

    //Example: A), B), C), ...Uppercase alphabetic character followed by a
    //closing
    case ANM_AlphaUcParenRight:
        numFormat = 'A';
        numSuffix = ')';
        break;

    //Example: (1), (2), (3), ...Arabic numeral enclosed in parentheses.
    case ANM_ArabicParenBoth:
        numPrefix = '(';
        numFormat = '1';
        numSuffix = ')';
        break;

    //Example: 1, 2, 3, ...Arabic numeral.
    case ANM_ArabicPlain:
        numFormat = '1';
        break;

    //Example: (I), (II), (III), ...Uppercase Roman numeral enclosed in
    //parentheses.
    case ANM_RomanUcParenBoth:
        numPrefix = '(';
        numFormat = 'I';
        numSuffix = ')';
        break;

    //Example: I), II), III), ...Uppercase Roman numeral followed by a closing
    //parenthesis.
    case ANM_RomanUcParenRight:
        numFormat = 'I';
        numSuffix = ')';
        break;

    default:
        numFormat = 'i';
        numSuffix = '.';
        break;
    }
} //end processTextAutoNumberScheme()

const TextContainer* PptToOdp::getTextContainer(
            const PptOfficeArtClientTextBox* clientTextbox,
            const PptOfficeArtClientData* clientData) const
{
    if (clientData && clientData->placeholderAtom && m_currentSlideTexts) {
        const PlaceholderAtom* p = clientData->placeholderAtom.data();
        if (p->position >= 0 && p->position < m_currentSlideTexts->atoms.size()) {
            return &m_currentSlideTexts->atoms[p->position];
        }
    }
    if (clientTextbox) {
        // find the text type
        foreach (const TextClientDataSubContainerOrAtom& a, clientTextbox->rgChildRec) {
            const TextContainer* tc = a.anon.get<TextContainer>();
            if (tc) {
                return tc;
            }
        }
    }
    return 0;
}

quint32 PptToOdp::getTextType(const PptOfficeArtClientTextBox* clientTextbox,
                              const PptOfficeArtClientData* clientData) const
{
    const TextContainer* tc = getTextContainer(clientTextbox, clientData);
    if (tc) return tc->textHeaderAtom.textType;
    return 99; // 99 means it is undefined here
}

void PptToOdp::processDeclaration(KoXmlWriter* xmlWriter)
{
    const HeadersFootersAtom* headerFooterAtom = 0;
    QSharedPointer<UserDateAtom> userDateAtom;
    QSharedPointer<FooterAtom> footerAtom;
    HeaderAtom* headerAtom = 0;
    const MSO::SlideHeadersFootersContainer* slideHF = getSlideHF();

    for (int slideNo = 0; slideNo < p->slides.size(); slideNo++) {
        const SlideContainer* slide = p->slides[slideNo];
        if (slide->perSlideHFContainer) {
            userDateAtom = slide->perSlideHFContainer->userDateAtom;
            footerAtom = slide->perSlideHFContainer->footerAtom;
            headerFooterAtom = &slide->perSlideHFContainer->hfAtom;
        }
        else if (slideHF) {
            userDateAtom = slideHF->userDateAtom;
            footerAtom = slideHF->footerAtom;
            headerFooterAtom = &slideHF->hfAtom;
        }


        if (headerFooterAtom && headerFooterAtom->fHasHeader && headerAtom) {
#if 0
            QString headerText = QString::fromLatin1(headerAtom->header, headerAtom->header.size());
            QString hdrName = findDeclaration(Header, headerText);
            if (hdrName == 0 ) {
                hdrName = QString("hdr%1").arg(declaration.values(Header).count() + 1);
                insertDeclaration(Header, hdrName, headerText);
            }
            usedHeaderDeclaration.insert(slideNo,hdrName);
#endif
        }
        if (headerFooterAtom && headerFooterAtom->fHasFooter && footerAtom) {
            QString footerText = QString::fromUtf16(footerAtom->footer.data(), footerAtom->footer.size());
            QString ftrName = findDeclaration(Footer, footerText);
            if ( ftrName == 0) {
                ftrName = QString("ftr%1").arg((declaration.values(Footer).count() + 1));
                insertDeclaration(Footer, ftrName, footerText);
            }
            usedFooterDeclaration.insert(slideNo,ftrName);
        }
        if (headerFooterAtom && headerFooterAtom->fHasDate) {
            if(headerFooterAtom->fHasUserDate && userDateAtom) {
                QString userDate = QString::fromUtf16(userDateAtom->userDate.data(), userDateAtom->userDate.size());
                QString dtdName = findDeclaration(DateTime, userDate);
                if ( dtdName == 0) {
                    dtdName = QString("dtd%1").arg((declaration.values(DateTime).count() + 1));
                    insertDeclaration(DateTime, dtdName, userDate);
                }
                usedDateTimeDeclaration.insert(slideNo,dtdName);
            }
            if(headerFooterAtom->fHasTodayDate) {
                QString dtdName = findDeclaration(DateTime, "");
                if ( dtdName == 0) {
                    dtdName = QString("dtd%1").arg((declaration.values(DateTime).count() + 1));
                    insertDeclaration(DateTime, dtdName, "");
                }
                usedDateTimeDeclaration.insert(slideNo,dtdName);
            }
        }
    }

    if (slideHF) {
        if (slideHF->hfAtom.fHasTodayDate) {
           QList<QPair<QString, QString> >items = declaration.values(DateTime);
           for( int i = items.size()-1; i >= 0; --i) {
                QPair<QString, QString > item = items.at(i);
                presentation_date_time_decl(xmlWriter, item.first, "current-date");
                //xmlWrite->addAttribute("style:data-style-name", "Dt1");
            }
        } else if (slideHF->hfAtom.fHasUserDate) {
            QList<QPair<QString, QString> >items = declaration.values(DateTime);
            for( int i = 0; i < items.size(); ++i) {
                QPair<QString, QString > item = items.at(i);
                presentation_date_time_decl d(xmlWriter, item.first, "fixed");
                d.addTextNode(item.second);
                //Future - Add Fixed date data here
            }
        }
        if (headerAtom && slideHF->hfAtom.fHasHeader) {
            QList< QPair < QString, QString > > items = declaration.values(Header);
            for( int i = items.size()-1; i >= 0; --i) {
                QPair<QString, QString > item = items.value(i);
                presentation_header_decl hd(xmlWriter, item.first);
                hd.addTextNode(item.second);
            }
        }
        if (footerAtom && slideHF->hfAtom.fHasFooter) {
            QList< QPair < QString, QString > > items = declaration.values(Footer);
            for( int i = items.size()-1 ; i >= 0; --i) {
                QPair<QString, QString > item = items.at(i);
                presentation_footer_decl fd(xmlWriter, item.first);
                fd.addTextNode(item.second);
            }
        }
    }
} //end processDeclaration()

QString PptToOdp::findDeclaration(DeclarationType type, const QString &text) const
{
    QList< QPair< QString , QString > > items = declaration.values(type);

    for( int i = 0; i < items.size(); ++i) {
        QPair<QString, QString>item = items.at(i);
        if ( item.second == text ) {
            return item.first;
        }
    }
    return 0;
}

QString PptToOdp::findNotesDeclaration(DeclarationType type, const QString &text) const
{
    QList<QPair<QString, QString> >items = notesDeclaration.values(type);

    for( int i = 0; i < items.size(); ++i) {
        QPair<QString, QString>item = items.at(i);
        if ( item.second == text) {
            return item.first;
        }
    }
    return 0;
}

void PptToOdp::insertDeclaration(DeclarationType type, const QString &name, const QString &text)
{
    QPair<QString, QString>item;
    item.first = name;
    item.second = text;

    declaration.insertMulti(type, item);
}

void PptToOdp::insertNotesDeclaration(DeclarationType type, const QString &name, const QString &text)
{
    QPair<QString, QString > item;
    item.first = name;
    item.second = text;

    notesDeclaration.insertMulti(type, item);
}

// @brief check if the provided groupShape contains the master shape
// @param spid identifier of the master shape
// @return pointer to the OfficeArtSpContainer
const OfficeArtSpContainer* checkGroupShape(const OfficeArtSpgrContainer& o, quint32 spid)
{
    if (o.rgfb.size() < 2) return nullptr;

    const OfficeArtSpContainer* sp = 0;
    foreach(const OfficeArtSpgrContainerFileBlock& co, o.rgfb) {
        if (co.anon.is<OfficeArtSpContainer>()) {
	    sp = co.anon.get<OfficeArtSpContainer>();
            if (sp->shapeProp.spid == spid) {
                return sp;
	    }
	}
        //TODO: the shape could be located deeper in the hierarchy
    }
    return nullptr;
}

const OfficeArtSpContainer* PptToOdp::retrieveMasterShape(quint32 spid) const
{
    const OfficeArtSpContainer* sp = 0;

    //check all main master slides
    foreach (const MSO::MasterOrSlideContainer* m, p->masters) {
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
        const DrawingContainer* drawing = 0;
        if (sc) {
            drawing = &sc->drawing;
        } else if (mm) {
            drawing = &mm->drawing;
        }
        if (drawing->OfficeArtDg.groupShape) {
            const OfficeArtSpgrContainer& spgr = *(drawing->OfficeArtDg.groupShape).data();
            sp = checkGroupShape(spgr, spid);
        }
        if (sp) {
            return sp;
        }
    }
    //check all notes master slides
    if (p->notesMaster) {
        if (p->notesMaster->drawing.OfficeArtDg.groupShape) {
            const OfficeArtSpgrContainer& spgr = *(p->notesMaster->drawing.OfficeArtDg.groupShape).data();
            sp = checkGroupShape(spgr, spid);
        }
        if (sp) {
            return sp;
        }
    }
#ifdef CHECK_SLIDES
    //check all presentation slides
    for (int c = 0; c < p->slides.size(); c++) {
        const SlideContainer* slide = p->slides[c];
        if (slide->drawing.OfficeArtDg.groupShape) {
            const OfficeArtSpgrContainer& spgr = *(slide->drawing.OfficeArtDg.groupShape).data();
            sp = checkGroupShape(spgr, spid);
        }
        if (sp) {
            return sp;
        }
    }
#endif
#ifdef CHECK_NOTES
    //check all notes slides
    for (int c = 0; c < p->notes.size(); c++) {
        const NotesContainer* notes = p->notes[c];
        if (notes->drawing.OfficeArtDg.groupShape) {
            const OfficeArtSpgrContainer& spgr = *(notes->drawing.OfficeArtDg.groupShape).data();
            sp = checkGroupShape(spgr, spid);
        }
        if (sp) {
            return sp;
        }
    }
#endif
    return nullptr;
}
