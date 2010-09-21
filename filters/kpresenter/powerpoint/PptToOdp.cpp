/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Amit Aggarwal <amitcs06@gmail.com>

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

#include "PptToOdp.h"
#include "globalobjectcollectors.h"
#include "pictures.h"
#include "ODrawToOdf.h"

#include <kdebug.h>
#include <KoOdf.h>
#include <KoOdfWriteStore.h>
#include <KoXmlWriter.h>

#include <QtCore/QBuffer>

using namespace MSO;

namespace Text
{
//TextTypeEnum, MS-PPT 2.13.33
enum {
    Title       = 0,  // title text
    Body        = 1,
    Notes       = 2,
    NotUsed     = 3,
    Other       = 4,  // text in a shape
    CenterBody  = 5,  // subtitle in title slide
    CenterTitle = 6,  // title in title slide
    HalfBody    = 7,  // body in two-column slide
    QuarterBody = 8   // body in four-body slide
};
}

namespace Color
{
//ColorSchemeEnum, MS-PPT 2.12.2
enum {
    Background  = 0,
    Text        = 1,
    Shadow      = 2,
    TitleText   = 3,
    Fill        = 4,
    Accent1     = 5,
    Accent2     = 6,
    Accent3     = 7,
    sRGB        = 0xFE,
    Undefined   = 0xFF
};
}

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
getText(const TextContainer& tc)
{
    if (tc.text.is<TextCharsAtom>()) {
        const QVector<quint16> textChars(tc.text.get<TextCharsAtom>()->textChars);
        return QString::fromUtf16(textChars.data(), textChars.size());
    } else if (tc.text.is<TextBytesAtom>()) {
        // each item represents the low byte of a UTF-16 Unicode character whose high byte is 0x00
        const QByteArray& textChars(tc.text.get<TextBytesAtom>()->textChars);
        return QString::fromAscii(textChars, textChars.size());
    }
    return QString();
}

QString getText(const TextContainer& tc, int start, int count)
{
    return getText(tc).mid(start,count);
}

/* The placementId is mapped to one of
   "chart", "date-time", "footer", "graphic", "handout", "header", "notes",
   "object", "orgchart", "outline", "page", "page-number", "subtitle", "table",
   "text" or "title" */
/* Note: we use 'outline' for  PT_MasterBody, PT_Body and PT_VerticalBody types
   to be compatible with OpenOffice. OpenOffice <= 3.2 does not render lists
   properly if the presentation class is not 'outline', 'subtitle', or 'notes'.
   */
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

QString getPresentationClass(const MSO::TextContainer* tc)
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

}//namespace

class PptToOdp::DrawClient : public ODrawToOdf::Client {
private:
    PptToOdp* const ppttoodp;

    QRectF getRect(const MSO::OfficeArtClientAnchor&);
    QString getPicturePath(int pib);
    bool onlyClientData(const MSO::OfficeArtClientData& o);
    void processClientData(const MSO::OfficeArtClientData& clientData,
                                   Writer& out);
    void processClientTextBox(const MSO::OfficeArtClientTextBox& ct,
                              const MSO::OfficeArtClientData* cd,
                              Writer& out);
    KoGenStyle createGraphicStyle(
            const MSO::OfficeArtClientTextBox* ct,
            const MSO::OfficeArtClientData* cd, Writer& out);
    void addTextStyles(const MSO::OfficeArtClientTextBox* clientTextbox,
            const MSO::OfficeArtClientData* clientData,
            Writer& out, KoGenStyle& style);
    const MSO::OfficeArtDggContainer* getOfficeArtDggContainer();
    QColor toQColor(const MSO::OfficeArtCOLORREF& c);
    QString formatPos(qreal v);
public:
    DrawClient(PptToOdp* p) :ppttoodp(p) {}
};

QRectF PptToOdp::DrawClient::getRect(const MSO::OfficeArtClientAnchor& o)
{
    const PptOfficeArtClientAnchor* a = o.anon.get<PptOfficeArtClientAnchor>();
    if (a) {
        return ::getRect(*a);
    }
    return QRect(0, 0, 1, 1);
}
QString PptToOdp::DrawClient::getPicturePath(int pib)
{
    return ppttoodp->getPicturePath(pib);
}
bool PptToOdp::DrawClient::onlyClientData(const MSO::OfficeArtClientData& o)
{
    const PptOfficeArtClientData* pcd = o.anon.get<PptOfficeArtClientData>();
    if (pcd && pcd->placeholderAtom && ppttoodp->currentSlideTexts) {
        const PlaceholderAtom* pa = pcd->placeholderAtom.data();
        if (pa->position >= 0
                && pa->position < ppttoodp->currentSlideTexts->atoms.size()) {
            return true;
        }
    }
    return false;
}
void PptToOdp::DrawClient::processClientData(const MSO::OfficeArtClientData& o,
                               Writer& out)
{
    const PptOfficeArtClientData* pcd = o.anon.get<PptOfficeArtClientData>();
    if (pcd && pcd->placeholderAtom && ppttoodp->currentSlideTexts) {
        const PlaceholderAtom* pa = pcd->placeholderAtom.data();
        if (pa->position >= 0
                && pa->position < ppttoodp->currentSlideTexts->atoms.size()) {
            const TextContainer& tc
                    = ppttoodp->currentSlideTexts->atoms[pa->position];
            ppttoodp->processTextForBody(&o, tc, out);
        }
    }
}
void PptToOdp::DrawClient::processClientTextBox(
        const MSO::OfficeArtClientTextBox& ct,
        const MSO::OfficeArtClientData* cd, Writer& out)
{
    const PptOfficeArtClientTextBox* tb
            = ct.anon.get<PptOfficeArtClientTextBox>();
    if (tb) {
        foreach(const TextClientDataSubContainerOrAtom& tc, tb->rgChildRec) {
            if (tc.anon.is<TextContainer>()) {
                ppttoodp->processTextForBody(cd,
                      *tc.anon.get<TextContainer>(), out);
            }
        }
    }
}
QString getMasterStyle(const QMap<int, QString>& map, int texttype) {
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
const TextMasterStyleAtom*
getTextMasterStyleAtom(const MasterOrSlideContainer* m, quint16 texttype)
{
    if (!m) return 0;
    const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
    if (!mm) return 0;
    const TextMasterStyleAtom* textstyle = 0;
    foreach (const TextMasterStyleAtom&ma, mm->rgTextMasterStyle) {
        if (ma.rh.recInstance == texttype) {
            textstyle = &ma;
        }
    }
    return textstyle;
}
KoGenStyle PptToOdp::DrawClient::createGraphicStyle(
        const MSO::OfficeArtClientTextBox* clientTextbox,
        const MSO::OfficeArtClientData* clientData,
        Writer& out)
{
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
    bool isPlaceholder = cd && cd->placeholderAtom;
    if (isPlaceholder) { // type is presentation
        bool canBeParentStyle = textType != 99 && out.stylesxml
                                && ppttoodp->currentMaster;
        bool isAutomatic = !canBeParentStyle;

        // if this object has a placeholder type, it defines a presentation style,
        // otherwise, it defines a graphic style
        // A graphic style is always automatic
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
        if (ppttoodp->currentMaster) {
            parent = getMasterStyle(
                    ppttoodp->masterPresentationStyles[ppttoodp->currentMaster],
                                    textType);
        }
        if (!parent.isEmpty()) {
            style.setParentName(parent);
        }
    } else { // type is graphic
        style = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        style.setAutoStyleInStylesDotXml(out.stylesxml);
    }

    const TextMasterStyleAtom* listStyle = 0;
    if (out.stylesxml) {
        listStyle = getTextMasterStyleAtom(ppttoodp->currentMaster,
                                                     textType);
    }
    QString listStyleName;
    if (listStyle) {
        KoGenStyle list(KoGenStyle::ListStyle);
        ppttoodp->defineListStyle(list, *listStyle);
        listStyleName = out.styles.insert(list);
    }

    return style;
}

void PptToOdp::DrawClient::addTextStyles(
        const MSO::OfficeArtClientTextBox* clientTextbox,
        const MSO::OfficeArtClientData* clientData,
        Writer& out, KoGenStyle& style)
{
    const PptOfficeArtClientData* cd = 0;
    if (clientData) {
        cd = clientData->anon.get<PptOfficeArtClientData>();
    }
    const PptOfficeArtClientTextBox* tb = 0;
    if (clientTextbox) {
        tb = clientTextbox->anon.get<PptOfficeArtClientTextBox>();
    }
    quint32 textType = ppttoodp->getTextType(tb, cd);
    const TextMasterStyleAtom* listStyle = 0;
    if (out.stylesxml) {
        listStyle = getTextMasterStyleAtom(ppttoodp->currentMaster,
                                                     textType);
    }
    if (listStyle && listStyle->lstLvl1) {
        PptTextPFRun pf(ppttoodp->p->documentContainer,
                        ppttoodp->currentMaster,
                        textType);
        ppttoodp->defineParagraphProperties(style, pf);
        ppttoodp->defineTextProperties(style, &listStyle->lstLvl1->cf, 0, 0, 0);
    }
    bool isPlaceholder = cd && cd->placeholderAtom;
    if (isPlaceholder) {
        // small workaround to avoid presenation frames from having borders,
        // even though the ppt file seems to specify that they should have one
        style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
        //style.addProperty("draw:stroke-width", "none", KoGenStyle::GraphicType);
    }
    const QString styleName = out.styles.insert(style);
    if (isPlaceholder) {
        out.xml.addAttribute("presentation:style-name", styleName);
        QString className = getPresentationClass(cd->placeholderAtom.data());
        if (className.isEmpty() || !out.stylesxml) {
            const TextContainer* tc = ppttoodp->getTextContainer(tb, cd);
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
    bool canBeParentStyle = isPlaceholder && textType != 99 && out.stylesxml
                                && ppttoodp->currentMaster;
    if (canBeParentStyle) {
        ppttoodp->masterPresentationStyles[
                ppttoodp->currentMaster][textType] = styleName;
    }
}
const MSO::OfficeArtDggContainer*
PptToOdp::DrawClient::getOfficeArtDggContainer()
{
    return &ppttoodp->p->documentContainer->drawingGroup.OfficeArtDgg;
}
QColor PptToOdp::DrawClient::toQColor(const MSO::OfficeArtCOLORREF& c)
{
    //Have to handle the case when OfficeArtCOLORREF/fSchemeIndex == true.

    //FIXME: If hspMaster property (0x0301) is provided, values from the
    //OfficeArtDggContainer are used, which seems wrong.  However using the
    //current color scheme works on test documents. (uzak)

    const MSO::MainMasterContainer* mmc = NULL;
    if (ppttoodp->currentMaster) {
        mmc = ppttoodp->currentMaster->anon.get<MainMasterContainer>();
    }
    return ppttoodp->toQColor(c, mmc, ppttoodp->currentSlide);
}

QString PptToOdp::DrawClient::formatPos(qreal v)
{
    return mm(v * (25.4 / 576));
}

PptToOdp::PptToOdp()
: p(0),
  currentSlideTexts(NULL),
  currentMaster(NULL),
  currentSlide(NULL)
{
}

PptToOdp::~PptToOdp()
{
    delete p;
}

QMap<QByteArray, QString>
createPictures(POLE::Storage& storage, KoStore* store, KoXmlWriter* manifest)
{
    QMap<QByteArray, QString> fileNames;
    POLE::Stream* stream = new POLE::Stream(&storage, "/Pictures");
    while (!stream->eof() && !stream->fail()
            && stream->tell() < stream->size()) {

        PictureReference ref = savePicture(*stream, store);
        if (ref.name.length() == 0) break;
        manifest->addManifestEntry("Pictures/" + ref.name, ref.mimetype);
        fileNames[ref.uid] = ref.name;
    }
    storage.close();
    delete stream;
    return fileNames;
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
KoFilter::ConversionStatus PptToOdp::convert(const QString& inputFile,
        const QString& to,
        KoStore::Backend storeType)
{
    // open inputFile
    POLE::Storage storage(inputFile.toLocal8Bit());
    if (!storage.open()) {
        qDebug() << "Cannot open " << inputFile;
        return KoFilter::StupidError;
    }
    if (!parse(storage)) {
        qDebug() << "Parsing and setup failed.";
        return KoFilter::StupidError;
    }

    // create output store
    KoStore* storeout = KoStore::createStore(to, KoStore::Write,
                        KoOdf::mimeType(KoOdf::Presentation), storeType);
    if (!storeout) {
        kWarning() << "Couldn't open the requested file.";
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus status = doConversion(storage, storeout);
    delete storeout;
    return status;
}

KoFilter::ConversionStatus PptToOdp::convert(POLE::Storage& storage,
        KoStore* storeout)
{
    if (!parse(storage)) {
        qDebug() << "Parsing and setup failed.";
        return KoFilter::StupidError;
    }
    return doConversion(storage, storeout);
}

KoFilter::ConversionStatus PptToOdp::doConversion(POLE::Storage& storage,
        KoStore* storeout)
{
    KoOdfWriteStore odfWriter(storeout);
    KoXmlWriter* manifest = odfWriter.manifestWriter(
                                KoOdf::mimeType(KoOdf::Presentation));

    // store the images from the 'Pictures' stream
    storeout->disallowNameExpansion();
    storeout->enterDirectory("Pictures");
    pictureNames = createPictures(storage,
                                  storeout, manifest);
    // read pictures from the PowerPoint Document structures
    bulletPictureNames = createBulletPictures(getPP<PP9DocBinaryTagExtension>(
            p->documentContainer), storeout, manifest);
    storeout->leaveDirectory();

    KoGenStyles styles;

    createMainStyles(styles);

    // store document content
    if (!storeout->open("content.xml")) {
        kWarning() << "Couldn't open the file 'content.xml'.";
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    storeout->write(createContent(styles));
    storeout->close();
    manifest->addManifestEntry("content.xml", "text/xml");

    // store document styles
    styles.saveOdfStylesDotXml(storeout, manifest);

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

}

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
    defineDefaultGraphicProperties(style);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultPresentationStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="presentation">
    KoGenStyle style(KoGenStyle::PresentationStyle, "presentation");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style);
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
    const OfficeArtDggContainer& drawingGroup
            = p->documentContainer->drawingGroup.OfficeArtDgg;
    DrawStyle ds(drawingGroup);
    defineDrawingPageStyle(style, ds, (hf) ?&hf->hfAtom :0);
    styles.insert(style);
}

void PptToOdp::defineDefaultChartStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="chart">
    KoGenStyle style(KoGenStyle::ChartStyle, "chart");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.insert(style);
}

void PptToOdp::defineDefaultTextProperties(KoGenStyle& style) {
    const TextCFException* cf = 0;
    const TextCFException9* cf9 = 0;
    const TextCFException10* cf10 = 0;
    const TextSIException* si = 0;
    if (p->documentContainer) {
        if (p->documentContainer->documentTextInfo.textCFDefaultsAtom) {
            cf = &p->documentContainer->documentTextInfo.textCFDefaultsAtom->cf;
        }
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

void PptToOdp::defineDefaultParagraphProperties(KoGenStyle& style) {
    const TextPFException9* pf9 = 0;
    if (p->documentContainer) {
        const PP9DocBinaryTagExtension* pp9 = getPP<PP9DocBinaryTagExtension>(
                p->documentContainer);
        if (pp9 && pp9->textDefaultsAtom) {
            pf9 = &pp9->textDefaultsAtom->pf9;
        }
    }
    PptTextPFRun pf(p->documentContainer);
    defineParagraphProperties(style, pf);
}

void PptToOdp::defineDefaultGraphicProperties(KoGenStyle& style) {
    const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
    style.addProperty("svg:stroke-width", "0.75pt", gt); // 2.3.8.15
    style.addProperty("draw:fill", "none", gt); // 2.3.8.38
    style.addProperty("draw:auto-grow-height", false, gt);
    style.addProperty("draw:stroke", "solid", gt);
    style.addProperty("draw:fill-color", "#ffffff", gt);
    const OfficeArtDggContainer& drawingGroup
        = p->documentContainer->drawingGroup.OfficeArtDgg;
    const DrawStyle ds(drawingGroup);
    DrawClient drawclient(this);
    ODrawToOdf odrawtoodf(drawclient);
    odrawtoodf.defineGraphicProperties(style, ds);
}

QString PptToOdp::getPicturePath(int pib) const
{
    int picturePosition = pib - 1;
    QByteArray rgbUid = getRgbUid(picturePosition);
    return rgbUid.length() ? "Pictures/" + pictureNames[rgbUid] : "";
}

void PptToOdp::defineTextProperties(KoGenStyle& style,
                                    const TextCFException* cf,
                                    const TextCFException9* /*cf9*/,
                                    const TextCFException10* /*cf10*/,
                                    const TextSIException* si,
                                    const TextContainer* tc)
{
    const KoGenStyle::PropertyType text = KoGenStyle::TextType;
    /* We try to get information for all the possible attributes in
       style:text-properties for clarity we handle them in alphabetical order */
    // fo:background-color
    // fo:color
    if (cf && cf->masks.color && cf->color) {
        QColor color = toQColor(*cf->color);
        if (color.isValid()) {
            style.addProperty("fo:color", color.name(), text);
        }
    }
    // Use the color defined by the corresponding color scheme.  Let's create a
    // temporary OfficeArtCOLORREF to reuse the toQColor function.  The red
    // value will be treated as an index into the current color scheme table.
    else {
        quint32 tt = Text::Other;
        if (tc) {
            tt = tc->textHeaderAtom.textType;
        }
        OfficeArtCOLORREF tmp;
        tmp.fSchemeIndex = true;
        if ((tt == Text::Title) || (tt == Text::CenterTitle)) {
            tmp.red = Color::TitleText;
        } else {
            tmp.red = Color::Text;
        }
	const MainMasterContainer* m = NULL;
        if (currentMaster) {
            m = currentMaster->anon.get<MainMasterContainer>();
        }
        style.addProperty("fo:color", toQColor(tmp, m, currentSlide).name(), text);
    }
    // fo:country
    // fo:font-family
    if (cf && cf->masks.typeface) {
        const FontEntityAtom* font = getFont(cf->fontRef);
        if (font) {
            const QString name= QString::fromUtf16(font->lfFaceName.data(),
                                                   font->lfFaceName.size());
            style.addProperty("fo:font-family", name, text);
        }
    }
    // fo:font-size
    if (cf && cf->masks.size) {
        style.addProperty("fo:font-size", pt(cf->fontSize),
                          text);
    }
    // fo:font-style: "italic", "normal" or "oblique
    if (cf && cf->masks.italic && cf->fontStyle) {
        style.addProperty("fo:font-style",
                          cf->fontStyle->italic ?"italic" :"normal", text);
    }
    // fo:font-variant: "normal" or "small-caps"
    // fo:font-weight: "100", "200", "300", "400", "500", "600", "700", "800", "900", "bold" or "normal"
    if (cf && cf->masks.bold && cf->fontStyle) {
        style.addProperty("fo:font-weight",
                          cf->fontStyle->bold ?"bold" :"normal", text);
    }
    // fo:hyphenate
    // fo:hyphenation-push-char
    // fo:hyphenation-remain-char-count
    // fo:language
    if (si && si->lang) {
        // TODO: get mapping from lid to language code
    }
    // fo:letter-spacing
    // fo:text-shadow
    if (cf && cf->masks.shadow) {
        style.addProperty("fo:text-shadow",
                          cf->fontStyle->shadow ?"1pt 1pt" :"none", text);
    }
    // fo:text-transform: "capitalize", "lowercase", "none" or "uppercase"
    // style:country-asian
    // style:country-complex
    // style:font-charset
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
    if (cf && cf->masks.emboss) {
        style.addProperty("style:font-relief",
                          cf->fontStyle->emboss ?"embossed" :"none", text);
    }
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
    // style:text-rotation-angle
    // style:text-rotation-scale
    // style:text-scale
    // style:text-underline-color
    // style:text-underline-mode
    // style:text-underline-style
    // style:text-underline-type: "double", "none" or "single"
    if (cf && cf->masks.underline) {
        style.addProperty("style:text-underline-type",
                          cf->fontStyle->underline ?"single" :"none", text);
    }
    // style:text-underline-width
    // style:use-window-font-color
}

void PptToOdp::defineParagraphProperties(KoGenStyle& style,
                                         const PptTextPFRun& pf) {
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
    // fo:margin
    // fo:margin-bottom
    style.addProperty("fo:margin-bottom", paraSpacingToCm(pf.spaceAfter()), para);
    // fo:margin-left
    style.addProperty("fo:margin-left", pptMasterUnitToCm(pf.leftMargin()),
                para);
    // fo:margin-right
    // fo:margin-top
    style.addProperty("fo:margin-top", paraSpacingToCm(pf.spaceBefore()), para);
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
    style.addProperty("fo:text-indent", pptMasterUnitToCm(pf.indent()), para);
    // fo:widows
    // style:auto-text-indent
    // style:background-transparency
    // style:border-line-width
    // style:border-line-width-bottom
    // style:border-line-width-left
    // style:border-line-width-right
    // style:border-line-width-top
    // style:font-independent-line-spacing
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
}

void PptToOdp::defineDrawingPageStyle(KoGenStyle& style, const DrawStyle& ds,
                                      const MSO::HeadersFootersAtom* hf,
                                      const MSO::StreamOffset* master,
                                      const MSO::StreamOffset* common)
{
    const MSO::SlideContainer* sc = NULL;
    const MSO::NotesContainer* nc = NULL;
    const MSO::SlideFlags* sf = NULL;

    if (common) {
        MSO::StreamOffset* c = const_cast<MSO::StreamOffset*>(common);
	if ((sc = dynamic_cast<MSO::SlideContainer*>(c))) {
            sf = &sc->slideAtom.slideFlags;
	} else if ((nc = dynamic_cast<MSO::NotesContainer*>(c))) {
            sf = &nc->notesAtom.slideFlags;
	} else {
            qWarning() << "Warning: Incorrect container! Provide SlideContainer of NotesContainer.";
        }
    }
    const KoGenStyle::PropertyType dp = KoGenStyle::DrawingPageType;

    // Inherit the background of the main master slide or notes master slide if
    // slideFlags/fMasterBackground == true.  The drawing-page style defined in
    // the corresponding <master-page> will be used.
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
            if (fillType == 0) {
                // only set the color if the fill type is 'solid' because OOo ignores
                // fill='non' if the color is set
                style.addProperty("draw:fill-color", toQColor(ds.fillColor(), master, common).name(), dp);
            }
            // draw:fill-gradient-name
            // draw:fill-hatch-name
            // draw:fill-hatch-solid
            // draw:fill-image-height
            // draw:fill-image-name
            quint32 fillBlip = ds.fillBlip();
            const QString fillImagePath = getPicturePath(fillBlip);
            if (!fillImagePath.isEmpty()) {
                style.addProperty("draw:fill-image-name",
                                  "fillImage" + QString::number(fillBlip), dp);
            }
            // draw:fill-image-ref-point-x
            // draw:fill-image-ref-point-y
            // draw:fill-image-ref-point
            // draw:fill-image-width
            // draw:gradient-step-count
            // draw:opacity-name
            // draw:opacity
            // draw:secondary-fill-color
            // draw:tile-repeat-offset
            // style:repeat
            style.addProperty("style:repeat", getRepeatStyle(fillType));
        } else {
            style.addProperty("draw:fill", "none", dp);
        }
    }
    // presentation:background-objects-visible
    style.addProperty("presentation:background-objects-visible", true);
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

}

void PptToOdp::defineListStyle(KoGenStyle& style,
                               const TextMasterStyleAtom& levels,
                               const TextMasterStyle9Atom* levels9,
                               const TextMasterStyle10Atom* levels10)
{
    ListStyleInput input;
    if (levels.lstLvl1) {
        defineListStyle(style, 1, input, levels.lstLvl1.data(),
                        ((levels9) ?levels9->lstLvl1.data() :0),
                        ((levels10) ?levels10->lstLvl1.data() :0));
    }
    if (levels.lstLvl2) {
        defineListStyle(style, 2, input, levels.lstLvl2.data(),
                        ((levels9) ?levels9->lstLvl2.data() :0),
                        ((levels10) ?levels10->lstLvl2.data() :0));
    }
    if (levels.lstLvl3) {
        defineListStyle(style, 3, input, levels.lstLvl3.data(),
                        ((levels9) ?levels9->lstLvl3.data() :0),
                        ((levels10) ?levels10->lstLvl3.data() :0));
    }
    if (levels.lstLvl4) {
        defineListStyle(style, 4, input, levels.lstLvl4.data(),
                        ((levels9) ?levels9->lstLvl4.data() :0),
                        ((levels10) ?levels10->lstLvl4.data() :0));
    }
    if (levels.lstLvl5) {
        defineListStyle(style, 5, input, levels.lstLvl5.data(),
                        ((levels9) ?levels9->lstLvl5.data() :0),
                        ((levels10) ?levels10->lstLvl5.data() :0));
    }
}

void PptToOdp::defineListStyle(KoGenStyle& style, quint8 depth,
                               ListStyleInput info,
                               const TextMasterStyleLevel* level,
                               const TextMasterStyle9Level* level9,
                               const TextMasterStyle10Level* level10)
{
    ListStyleInput parent;
    parent.cf = (level) ?&level->cf :0;
    parent.cf9 = (level9) ?&level9->cf9 :0;
    parent.cf10 = (level10) ?&level10->cf10 :0;
    if (info.cf == 0) info.cf = parent.cf;
    if (info.cf9 == 0) info.cf9 = parent.cf9;
    if (info.cf10 == 0) info.cf10 = parent.cf10;
    defineListStyle(style, depth, info, parent);
}

QChar
getBulletChar(const PptTextPFRun& pf) {
    quint16 v = (quint16)pf.bulletChar();
    if (v == 0xf06c) { // 0xF06C from windings is similar to ●
        return QChar(0x25cf); //  "●"
    }
    if (v == 0xf02d) { // 0xF02D from symbol is similar to –
        return QChar(0x2013);
    }
    if (v == 0xf0e8) { // 0xF0E8 is similar to ➔
        return QChar(0x2794);
    }
    if (v == 0xf0d8) { // 0xF0D8 is similar to ➢
        return QChar(0x27a2);
    }
    if (v == 0xf0fb) { // 0xF0FB is similar to ✗
        return QChar(0x2717);
    }
    if (v == 0xf0fc) { // 0xF0FC is similar to ✔
        return QChar(0x2714);
    }
    return QChar(v);
    return QChar(0x25cf); //  "●"
}
QString
bulletSizeToSizeString(const PptTextPFRun& pf)
{
    if (pf.fBulletHasSize()) {
        qint16 size = pf.bulletSize();
        if (size >= 25 && size <= 400) {
            return percent(size);
        } else if (size >= -4000 && size <= -1) {
            return pt(size);
        }
    }
    return QString();
}
void PptToOdp::defineListStyle(KoGenStyle& style, quint8 level,
                               const ListStyleInput& i,
                               const ListStyleInput& p)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter out(&buffer);

    QString bulletSize = bulletSizeToSizeString(i.pf);
    if (bulletSize.isNull()) {
        bulletSize = bulletSizeToSizeString(p.pf);
    }

    QString elementName;
    bool imageBullet = i.pf.bulletBlipRef() != 65535;
    if (imageBullet) {
        elementName = "text:list-level-style-image";
        out.startElement("text:list-level-style-image");
        out.addAttribute("xlink:href",
                         bulletPictureNames.value(i.pf.bulletBlipRef()));
        if (bulletSize.isNull() || bulletSize.endsWith("%")) {
            if (i.cf && i.cf->masks.size) {
                bulletSize = pt(i.cf->fontSize);
            } else if (p.cf && p.cf->masks.size) {
                bulletSize = pt(p.cf->fontSize);
            }
        }
        if (bulletSize.isNull() || bulletSize.endsWith("%")) {
            bulletSize = "20pt"; // fallback value
        }
    } else {
        QString numFormat("1"), numSuffix, numPrefix;
        processTextAutoNumberScheme(i.pf.scheme(),
                                    numFormat, numSuffix, numPrefix);
        // if there is no bulletChar or the bullet has autonumbering explicitly
        // we assume the list is autonumbering
        if (i.pf.fBulletHasAutoNumber() || i.pf.bulletChar() == 0) {
            elementName = "text:list-level-style-number";
            out.startElement("text:list-level-style-number");
            if (!numFormat.isNull()) {
                out.addAttribute("style:num-format", numFormat);
            }
            //out.addAttribute("style:display-levels", "TODO");
            out.addAttribute("text:start-value", i.pf.startNum());
        } else {
            elementName = "text:list-level-style-bullet";
            out.startElement("text:list-level-style-bullet");
            out.addAttribute("text:bullet-char", getBulletChar(i.pf));
            if (i.pf.fBulletHasSize() && i.cf) {
                qreal relSize = 100.0 * i.pf.bulletSize() / i.cf->fontSize;
                out.addAttribute("text:bullet-relative-size", percent(relSize));
            }
        }
        if (!numPrefix.isNull()) {
            out.addAttribute("style:num-prefix", numPrefix);
        }
        if (!numSuffix.isNull()) {
            out.addAttribute("style:num-suffix", numSuffix);
        }
    }
    out.addAttribute("text:level", level);

    bool hasIndent = i.pf.level();
    out.startElement("style:list-level-properties");
    // fo:height
    if (imageBullet && !bulletSize.isNull()) {
        out.addAttribute("fo:height", bulletSize);
    }
    // fo:text-align
    // fo:width
    if (imageBullet && !bulletSize.isNull()) {
        out.addAttribute("fo:width", bulletSize);
    }
    // style:font-name
    // style:vertical-pos
    if (imageBullet) {
        out.addAttribute("style:vertical-pos", "middle");
    }
    // style:vertical-rel
    if (imageBullet) {
        out.addAttribute("style:vertical-rel", "line");
    }
    // svg:y
    // text:min-label-distance
    // text:min-label-width
    if (imageBullet && !bulletSize.isNull()) {
        out.addAttribute("text:min-label-width", bulletSize);
    }
    // text:space-before
    int spacebefore = i.pf.spaceBefore() + i.pf.leftMargin();
    if (spacebefore != 0) {
        out.addAttribute("text:space-before",
                         paraSpacingToCm(spacebefore));
    }
    out.endElement(); // style:list-level-properties

    if (!imageBullet) {
        KoGenStyle ls(KoGenStyle::TextStyle);
        defineTextProperties(ls, p.cf, p.cf9, p.cf10, p.si);
        defineTextProperties(ls, i.cf, i.cf9, i.cf10, i.si);

        // override some properties with information from the paragraph
        if (hasIndent) {
            const MSO::FontEntityAtom* font = getFont(i.pf.bulletFontRef());
            if (font) {
                ls.addProperty("fo:font-family",
                           QString::fromUtf16(font->lfFaceName.data(),
                                              font->lfFaceName.size()),
                           KoGenStyle::TextType);
            }
        }
        if (hasIndent && i.pf.fBulletHasColor()) {
            const QColor color = toQColor(i.pf.bulletColor());
            if (color.isValid()) {
                ls.addProperty("fo:color", color.name(), KoGenStyle::TextType);
            }
        }
        // maybe fo:font-size should be set from pf.bulletSize

        ls.writeStyleProperties(&out, KoGenStyle::TextType);
    }
    out.endElement();  // text:list-level-style-*
    // serialize the text:list-style element into the properties
    QString elementContents = QString::fromUtf8(buffer.buffer(),
                                                buffer.buffer().size() );
    style.addChildElement(elementName, elementContents);
}

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
                            qDebug() << "Already found a placeholder with the right type " << wanted;
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
        currentMaster = m;
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
    currentMaster = NULL;
}
const MSO::OfficeArtSpContainer*
getMasterShape(const MSO::MasterOrSlideContainer* m) {
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
void PptToOdp::defineAutomaticDrawingPageStyles(KoGenStyles& styles)
{
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
        const OfficeArtDggContainer& drawingGroup
                = p->documentContainer->drawingGroup.OfficeArtDgg;
        DrawStyle ds(drawingGroup, scp);
        defineDrawingPageStyle(dp, ds, hf, mm);
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
        const OfficeArtDggContainer& drawingGroup
                = p->documentContainer->drawingGroup.OfficeArtDgg;
        DrawStyle ds(drawingGroup,
                     p->notesMaster->drawing.OfficeArtDg.shape.data());
        defineDrawingPageStyle(dp, ds, hf, p->notesMaster);
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
        const MainMasterContainer* mmc = m->anon.get<MainMasterContainer>();
        const HeadersFootersAtom* hf = getSlideHFAtom(sc);
        const OfficeArtDggContainer& drawingGroup
                = p->documentContainer->drawingGroup.OfficeArtDgg;
        const OfficeArtSpContainer* masterSlideShape
                = getMasterShape(m);
        // Inherit objects from the main master slide if
        // slideContainer/slideAtom/slideFlags/fMasterObjects == true
        const OfficeArtSpContainer* slideShape = NULL;
        if (!sc->slideAtom.slideFlags.fMasterObjects) {
            slideShape = sc->drawing.OfficeArtDg.shape.data();
        }
        DrawStyle ds(drawingGroup, masterSlideShape, slideShape);
        defineDrawingPageStyle(dp, ds, hf, mmc, sc);
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
        const OfficeArtDggContainer& drawingGroup
                = p->documentContainer->drawingGroup.OfficeArtDgg;
        DrawStyle ds(drawingGroup, nc->drawing.OfficeArtDg.shape.data());
        defineDrawingPageStyle(dp, ds, hf, p->notesMaster, nc);
        drawingPageStyles[nc] = styles.insert(dp, "dp");
    }
}

void PptToOdp::createMainStyles(KoGenStyles& styles)
{
    /* This function is follows the flow of the styles.xml file.
       -> style:styles
       first, the global objects are looked up and defined. This includes the
       style:presentation-page-layout elements. Next, the
       default styles for the 12 style families are defined.

       -> style:automatic-styles
       After that, style:page-layout and automatic styles are defined

       -> office:master-styles
       And lastly, the master slides are defined
    */
    /*
       collect all the global objects into
       styles.xml/office:document-styles/office:styles
    */
    // TODO: draw:gradient
    // TODO: svg:linearGradient
    // TODO: svg:radialGradient
    // TODO: draw:hatch
    // style:fill-image
    FillImageCollector fillImageCollector(styles, *this);
    collectGlobalObjects(fillImageCollector, *p);
    // draw:marker
    defineArrow(styles);
    // draw:stroke-dash
    StrokeDashCollector strokeDashCollector(styles, *this);
    collectGlobalObjects(strokeDashCollector, *p);
    // TODO: draw:opacity

    /*
       Define the style:presentation-page-layout elements.
    */
    // TODO:

    /*
       Define default styles for some of the 12 style families.
       No default styles for the families 'text' and 'paragraph'
       are defined, since these have higher precedence than the text and
       paragraph settings for the other style families that may contain text and
       paragraph settings, like 'graphic' and 'presentation'.
    */
    //defineDefaultTextStyle(styles);
    //defineDefaultParagraphStyle(styles);
    defineDefaultSectionStyle(styles);
    defineDefaultRubyStyle(styles);
    defineDefaultTableStyle(styles);
    defineDefaultTableColumnStyle(styles);
    defineDefaultTableRowStyle(styles);
    defineDefaultTableCellStyle(styles);
    defineDefaultGraphicStyle(styles);
    defineDefaultPresentationStyle(styles);

    //NOTE: kpresenter uses the default drawing-page style instead of that
    //defined by the corresponding <master-page> element.
//     defineDefaultDrawingPageStyle(styles);

    defineDefaultChartStyle(styles);

    /*
       Define the standard list style
     */
    if (p->documentContainer) {
        KoGenStyle list(KoGenStyle::ListStyle);
        defineListStyle(list,
                p->documentContainer->documentTextInfo.textMasterStyleAtom);
        styles.insert(list, "standardListStyle",
                KoGenStyles::DontAddNumberToName);
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
    currentSlideTexts = 0;
    defineMasterStyles(styles);
    defineAutomaticDrawingPageStyles(styles);

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
    QBuffer notesBuffer;
    if (p->notesMaster) { // draw the notes master
        notesBuffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&notesBuffer);
        Writer out(writer, styles, true);

        writer.startElement("presentation:notes");
        writer.addAttribute("style:page-layout-name", notesPageLayoutName);
        writer.addAttribute("draw:style-name",
                            drawingPageStyles[p->notesMaster]);
        currentMaster = 0;

        if (p->notesMaster->drawing.OfficeArtDg.groupShape) {
            DrawClient drawclient(this);
            ODrawToOdf odrawtoodf(drawclient);
            foreach(const OfficeArtSpgrContainerFileBlock& co,
                    p->notesMaster->drawing.OfficeArtDg.groupShape->rgfb) {
                odrawtoodf.processDrawing(co, out);
            }
        }
        writer.endElement();
    }
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
        currentMaster = m;
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&buffer);
        Writer out(writer, styles, true);
        if (drawing->OfficeArtDg.groupShape) {
            DrawClient drawclient(this);
            ODrawToOdf odrawtoodf(drawclient);
            foreach(const OfficeArtSpgrContainerFileBlock& co,
                    drawing->OfficeArtDg.groupShape->rgfb) {
                odrawtoodf.processDrawing(co, out);
            }
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
    currentMaster = NULL;

    // Creating dateTime class object
    if (getSlideHF()) {
        int dateTimeFomatId = getSlideHF()->hfAtom.formatId;
        bool hasTodayDate = getSlideHF()->hfAtom.fHasTodayDate;
        bool hasUserDate = getSlideHF()->hfAtom.fHasUserDate;
        dateTime = DateTimeFormat(dateTimeFomatId);
        dateTime.addDateTimeAutoStyles(styles, hasTodayDate, hasUserDate);
    }
}

QByteArray PptToOdp::createContent(KoGenStyles& styles)
{
    QBuffer presentationBuffer;
    presentationBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter presentationWriter(&presentationBuffer);

    processDeclaration(&presentationWriter);

    Writer out(presentationWriter, styles);
    for (int c = 0; c < p->slides.size(); c++) {
        processSlideForBody(c, out);
    }

    QByteArray contentData;
    QBuffer contentBuffer(&contentData);
    contentBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter contentWriter(&contentBuffer);

    contentWriter.startDocument("office:document-content");
    contentWriter.startElement("office:document-content");
    contentWriter.addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    contentWriter.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    contentWriter.addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    contentWriter.addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    contentWriter.addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    contentWriter.addAttribute("xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0");
    contentWriter.addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    contentWriter.addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    contentWriter.addAttribute("office:version", "1.0");

    // office:automatic-styles
    styles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, &contentWriter);

    // office:body
    contentWriter.startElement("office:body");
    contentWriter.startElement("office:presentation");

    contentWriter.addCompleteElement(&presentationBuffer);

    contentWriter.endElement();  // office:presentation

    contentWriter.endElement();  // office:body

    contentWriter.endElement();  // office:document-content
    contentWriter.endDocument();
    return contentData;
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

    foreach(ExObjListSubContainer container,
            p->documentContainer->exObjList->rgChildRec) {
        // Search all ExHyperlinkContainers for specified id
        ExHyperlinkContainer *hyperlink = container.anon.get<ExHyperlinkContainer>();
        if (hyperlink && hyperlink->exHyperlinkAtom.exHyperLinkId == id) {
            if (hyperlink->friendlyNameAtom) {
                friendly = utf16ToString(hyperlink->friendlyNameAtom->friendlyName);
            }
            if (hyperlink->targetAtom) {
                target = utf16ToString(hyperlink->targetAtom->target);
            }
            // TODO currently location is ignored. Location referes to
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
namespace
{
/**
* @brief Write text deindentations the specified amount. Actually it just
* closes elements.
*
* Doesn't close the last text:list-item though.
* @param xmlWriter XML writer to write closing tags
* @param count how many lists and list items to leave open
* @param levels the list of levels to remove from
*/
void writeTextObjectDeIndent(KoXmlWriter& xmlWriter, const int count,
                             QStack<QString>& levels)
{
    while (levels.size() > count) {
        xmlWriter.endElement(); // text:list
        levels.pop();
        if (levels.size()) {
            xmlWriter.endElement(); // text:list-item
        }
    }
}
void addListElement(KoXmlWriter& xmlWriter, QStack<QString>& levels,
                    const QString& listStyle)
{
    // if the context is a text:list, a text:list-item is needed
    if (levels.size() > 0) {
        xmlWriter.startElement("text:list-item");
    }
    xmlWriter.startElement("text:list");
    if (!listStyle.isEmpty()) {
        xmlWriter.addAttribute("text:style-name", listStyle);
    }
    levels.push(listStyle);
}
}

void
getMeta(const TextContainerMeta& m, KoXmlWriter& out)
{
    const SlideNumberMCAtom* a = m.meta.get<SlideNumberMCAtom>();
    const DateTimeMCAtom* b = m.meta.get<DateTimeMCAtom>();
    const GenericDateMCAtom* c = m.meta.get<GenericDateMCAtom>();
    const HeaderMCAtom* d = m.meta.get<HeaderMCAtom>();
    const FooterMCAtom* e = m.meta.get<FooterMCAtom>();
    const RTFDateTimeMCAtom* f = m.meta.get<RTFDateTimeMCAtom>();
    if (a) {
        out.startElement("text:page-number");
        out.endElement();
    }
    if (b) {
        // TODO: datetime format
        out.startElement("text:time");
        out.endElement();
    }
    if (c) {
        // TODO: datetime format
        out.startElement("text:date");
        out.endElement();
    }
    if (d) {
        out.startElement("presentation:header");
        out.endElement();
    }
    if (e) {
        out.startElement("presentation:footer");
        out.endElement();
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

int PptToOdp::processTextSpan(const MSO::TextContainer& tc, Writer& out,
                              const QString& text, const int start,
                              int end)
{
    // find all components that start at position start and get the right
    // character run
    const TextCFRun* cfr = getCFRun(&tc, start);
    const TextCFException* cf = NULL;
    int count = 0;

    if (cfr) {
        cf = &cfr->cf;
        count = cfr->count;
    }

    // get the right special info run
    const QList<TextSIRun>* tsi = 0;
    if (tc.specialinfo) tsi = &tc.specialinfo->rgSIRun;
    if (tc.specialinfo2) tsi = &tc.specialinfo2->rgSIRun;
    const TextSIException* si = 0;
    int siend = 0;
    int i = 0;
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

    // find a meta character
    const TextContainerMeta* meta = 0;
    for (i = 0; i<tc.meta.size(); ++i) {
        const TextContainerMeta& m = tc.meta[i];
        end = getMeta<SlideNumberMCAtom>(m, meta, start, end);
        end = getMeta<DateTimeMCAtom>(m, meta, start, end);
        end = getMeta<GenericDateMCAtom>(m, meta, start, end);
        end = getMeta<HeaderMCAtom>(m, meta, start, end);
        end = getMeta<FooterMCAtom>(m, meta, start, end);
        end = getMeta<RTFDateTimeMCAtom>(m, meta, start, end);
    }

    // find the right bookmark
    const TextBookmarkAtom* bookmark = 0;
    for (i = 0; i<tc.bookmark.size(); ++i) {
        if (tc.bookmark[i].begin < start && tc.bookmark[i].end >= start) {
            bookmark = &tc.bookmark[i];
        }
    }

    // find the interactive atom
    const MouseClickTextInfo* mouseclick = 0;
    const MouseOverTextInfo* mouseover = 0;
    for (i = 0; i<tc.interactive.size(); ++i) {
        const TextContainerInteractiveInfo& ti = tc.interactive[i];
        const MouseClickTextInfo* a =
                ti.interactive.get<MouseClickTextInfo>();
        const MouseOverTextInfo* b =
                ti.interactive.get<MouseOverTextInfo>();
        if (a && start >= a->text.range.begin && start < a->text.range.end) {
            mouseclick = a;
        }
        if (b && start >= b->text.range.begin && start < b->text.range.end) {
            mouseover = b;
        }
    }

    // determine the end of the range
    if (si && siend < end) {
        end = siend;
    }
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

    if (mouseclick) {
        /**
        * [MS-PPT].PDF states exHyperlinkIdRef must be ignored unless action is
        * equal to II_JumpAction (0x3), II_HyperlinkAction (0x4), or
        * II_CustomShowAction (0x7).
        */
        out.xml.startElement("text:a");
        QPair<QString, QString> link = findHyperlink(
                mouseclick->interactive.interactiveInfoAtom.exHyperlinkIdRef);
        if (!link.second.isEmpty()) { // target
            out.xml.addAttribute("xlink:href", link.second);
        } else if (!link.first.isEmpty()) {
            out.xml.addAttribute("xlink:href", link.first);
        }
    } else if (mouseover) {
        out.xml.startElement("text:a");
        QPair<QString, QString> link = findHyperlink(
                mouseover->interactive.interactiveInfoAtom.exHyperlinkIdRef);
        if (!link.second.isEmpty()) { // target
            out.xml.addAttribute("xlink:href", link.second);
        } else if (!link.first.isEmpty()) {
            out.xml.addAttribute("xlink:href", link.first);
        }
    } else {
        out.xml.startElement("text:span");

        //count specifies the number of characters of the corresponding text to
        //which this character formatting applies
        if (count > 0) {
            int tmp = start + count;
            //moved to left by one character in the processTextForBody function
            if (tmp <= end) {
                end = tmp;
            }
        }
    }
    KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    defineTextProperties(style, cf, 0, 0, si, &tc);
    out.xml.addAttribute("text:style-name", out.styles.insert(style));

    if (meta) {
        getMeta(*meta, out.xml);
    } else {
        int len = end - start;
        if (text[end-1] == '\r' || text[end-1] == '\v') {
            len--;
        }
        const QString txt
                = text.mid(start, len).replace('\r', '\n').replace('\v', '\n');
        out.xml.addTextNode(txt);
    }

    out.xml.endElement();
    return end;
}

int PptToOdp::processTextSpans(const MSO::TextContainer& tc, Writer& out,
                              const QString& text, int start, int end)
{
    int pos = start;
    while (pos < end) {
        int r = processTextSpan(tc, out, text, pos, end);
        if (r <= pos) {
            // some error
            qDebug() << "pos: " << pos << " end: " << end << " r: " << r;
            return -2;
        }
        pos = r;
        if (pos == end) {
            return 0;
        }
    }
    return (pos == end) ?0 :-pos;
}

QString PptToOdp::defineAutoListStyle(Writer& out, const PptTextPFRun& pf)
{
    KoGenStyle list(KoGenStyle::ListAutoStyle);
    ListStyleInput info;
    info.pf = pf;
    ListStyleInput parent;
    defineListStyle(list, pf.level(), info, parent);
    return out.styles.insert(list);
}

void PptToOdp::processTextLine(Writer& out,
                               const MSO::OfficeArtClientData* clientData,
                               const MSO::TextContainer& tc,
                               const QString& text, int start, int end,
                               QStack<QString>& levels)
{
    const PptOfficeArtClientData* pcd = 0;
    if (clientData) {
        pcd = clientData->anon.get<PptOfficeArtClientData>();
    }
    PptTextPFRun pf(p->documentContainer, currentSlideTexts, currentMaster, pcd,
                    &tc, start);
    bool islist = pf.level() > 0 && start < end;
    if (islist) {
        QString listStyle = defineAutoListStyle(out, pf);
        int level = pf.level() - 1;
        // remove levels until the top level is the right indentation
        if (levels.size() > level && levels[level] == listStyle) {
            writeTextObjectDeIndent(out.xml, level + 1, levels);
        } else {
            writeTextObjectDeIndent(out.xml, level, levels);
        }
        // add styleless levels up to the current level of indentation
        while (levels.size() < level) {
            addListElement(out.xml, levels, "");
        }
        // at this point, levels.size() == paragraphIndent
        if (level + 1 != levels.size()) {
            addListElement(out.xml, levels, listStyle);
        }
        out.xml.startElement("text:list-item");
    } else {
        writeTextObjectDeIndent(out.xml, 0, levels);
    }

    out.xml.startElement("text:p");
    KoGenStyle style(KoGenStyle::ParagraphAutoStyle, "paragraph");
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    defineParagraphProperties(style, pf);
    out.xml.addAttribute("text:style-name", out.styles.insert(style));
    processTextSpans(tc, out, text, start, end);
    out.xml.endElement(); // text:p

    if (islist) {
        out.xml.endElement(); // text:list-item
    }
}

void PptToOdp::processTextForBody(const MSO::OfficeArtClientData* clientData,
                                  const MSO::TextContainer& tc, Writer& out)
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

    //NOTE: If slideFlags/fMasterScheme == true, master's color scheme MUST be
    //used whether it's a placeholder shape or not. (uzak)
//     const PlaceholderAtom* p = 0;
//     if (clientData) {
//         const PptOfficeArtClientData* pcd
//                     = clientData->anon.get<PptOfficeArtClientData>();
//         if (pcd) {
//             p = pcd->placeholderAtom.data();
//         }
//     }

//     const MSO::MasterOrSlideContainer* tmpMaster = currentMaster;
//     // if this is not a presentation frame, set master to 0, to avoid the
//     // text style from inheriting from the master style
//     if (p == 0) {
//         currentMaster = 0;
//     }

    static const QRegExp lineend("[\v\r]");
    const QString text = getText(tc);

    // loop over all the '\r' delimited lines
    // Paragraph formatting that applies to substring
    QStack<QString> levels;
    levels.reserve(5);
    int pos = 0;
    while (pos < text.length()) {
        int end = text.indexOf(lineend, pos);
        if (end == -1) end = text.size();
        if (pos != end) {
            processTextLine(out, clientData, tc, text, pos, end, levels);
        }

        pos = end + 1;
    }
    // close all open text:list elements
    writeTextObjectDeIndent(out.xml, 0, levels);
//     currentMaster = tmpMaster;
}

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
            if (tc.textHeaderAtom.textType == Text::Title) {
                nameStr = getText(tc);
                break;
            }
        }
    }

    if (nameStr.isEmpty()) {
        nameStr = QString("page%1").arg(slideNo + 1);
    }

    nameStr.remove('\r');
    nameStr.remove('\v');

    out.xml.startElement("draw:page");
    QString value = masterNames.value(master);
    if (!value.isEmpty()) {
        out.xml.addAttribute("draw:master-page-name", value);
    }
    out.xml.addAttribute("draw:name", nameStr);
    value = drawingPageStyles[slide];
    if (!value.isEmpty()) {
        out.xml.addAttribute("draw:style-name", value);
    }
    //xmlWriter.addAttribute("presentation:presentation-page-layout-name", "AL1T0");

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
        out.xml.addAttribute("presentation:use-date-time-name",
                               usedDateTimeDeclaration[slideNo]);
    }
    if (!usedHeaderDeclaration.value(slideNo).isEmpty()) {
        if(usedHeaderDeclaration[slideNo] != "")
            out.xml.addAttribute("presentation:use-header-name", usedHeaderDeclaration[slideNo]);
    }
    if (!usedFooterDeclaration.value(slideNo).isEmpty()) {
        if(usedFooterDeclaration[slideNo] != "")
            out.xml.addAttribute("presentation:use-footer-name", usedFooterDeclaration[slideNo]);
    }

    currentSlideTexts = &p->documentContainer->slideList->rgChildRec[slideNo];
    currentMaster = master;
    currentSlide = slide;

    DrawClient drawclient(this);
    ODrawToOdf odrawtoodf(drawclient);

    if (slide->drawing.OfficeArtDg.groupShape) {
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                slide->drawing.OfficeArtDg.groupShape->rgfb) {
            odrawtoodf.processDrawing(co, out);
        }
    }

    currentMaster = NULL;
    currentSlide = NULL;

    if (slide->drawing.OfficeArtDg.shape) {
        // leave it out until it is understood
        //  processObjectForBody(*slide->drawing.OfficeArtDg.shape, out);
    }

    // draw the notes
    const NotesContainer* nc = p->notes[slideNo];
    if (nc && nc->drawing.OfficeArtDg.groupShape) {
        currentSlideTexts = 0;
        out.xml.startElement("presentation:notes");
        value = drawingPageStyles[nc];
        if (!value.isEmpty()) {
            out.xml.addAttribute("draw:style-name", value);
        }
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                nc->drawing.OfficeArtDg.groupShape->rgfb) {
            odrawtoodf.processDrawing(co, out);
        }
        out.xml.endElement();
    }

    out.xml.endElement(); // draw:page
}

QString PptToOdp::paraSpacingToCm(int value) const
{
    if (value < 0) {
        unsigned int temp = -value;
        return pptMasterUnitToCm(temp);
    }

    return pptMasterUnitToCm(value);
}

QString PptToOdp::pptMasterUnitToCm(unsigned int value) const
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
    if (color.index == 0xFE) {
        return QColor(color.red, color.green, color.blue);
    }
    if (color.index == 0xFF) { // color is undefined
        return QColor();
    }

    const QList<ColorStruct>* colorScheme;
    // TODO: use the current master
    const MasterOrSlideContainer* m = p->masters[0];
    if (m->anon.is<MainMasterContainer>()) {
        const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
        colorScheme = &n->slideSchemeColorSchemeAtom.rgSchemeColor;
    } else {
        const SlideContainer* n = m->anon.get<SlideContainer>();
        colorScheme = &n->slideSchemeColorSchemeAtom.rgSchemeColor;
    }
    if (colorScheme->size() > color.index) {
        const ColorStruct c = colorScheme->at(color.index);
        return QColor(c.red, c.green, c.blue);
    }
    return QColor();
}
QColor PptToOdp::toQColor(const MSO::OfficeArtCOLORREF& c,
                          const MSO::StreamOffset* master, const MSO::StreamOffset* common)
{
    QColor ret;

    //fSchemeIndex - A bit that specifies whether the current application
    //defined color scheme will be used to determine the color (MS-ODRAW)
    if (c.fSchemeIndex) {

        const QList<ColorStruct>* lst = NULL;
        const MSO::MainMasterContainer* mmc = NULL;
        const MSO::NotesContainer* nmc = NULL;
        const MSO::SlideContainer* sc = NULL;
        const MSO::NotesContainer* nc = NULL;

        // Get the color scheme of the current main master or notes master slide.
        if (master) {
            MSO::StreamOffset* m = const_cast<MSO::StreamOffset*>(master);
            if ((mmc = dynamic_cast<MSO::MainMasterContainer*>(m))) {
                lst = &mmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            } else if ((nmc = dynamic_cast<MSO::NotesContainer*>(m))) {
                lst = &nmc->slideSchemeColorSchemeAtom.rgSchemeColor;
            } else {
                qWarning() << "Warning: Incorrect container! Provide MainMasterContainer or NotesContainer.";
            }
        }
        // Get the color scheme of the current presentation slide or notes
        // slide.  If fMasterScheme == true use master's color scheme.
        if (common) {
            MSO::StreamOffset* c = const_cast<MSO::StreamOffset*>(common);
	    if ((sc = dynamic_cast<MSO::SlideContainer*>(c))) {
                if (!sc->slideAtom.slideFlags.fMasterScheme) {
                    lst = &sc->slideSchemeColorSchemeAtom.rgSchemeColor;
                }
	    } else if ((nc = dynamic_cast<MSO::NotesContainer*>(c))) {
                if (!nc->notesAtom.slideFlags.fMasterScheme) {
                    lst = &nc->slideSchemeColorSchemeAtom.rgSchemeColor;
                }
	    } else {
                qWarning() << "Warning: Incorrect container! Provide SlideContainer of NotesContainer.";
            }
        }
        //check for a valid color scheme
        if (!lst) {
            //NOTE: Using color scheme of the first main master slide
            if ((mmc = p->masters[0]->anon.get<MainMasterContainer>())) {
                if (!(lst = &mmc->slideSchemeColorSchemeAtom.rgSchemeColor)) {
                    qWarning() << "Warning: Ivalid color scheme! Returning an invalid color!";
                    return ret;
                }
            }
        }
        // Use the red color channel's value as index according to MS-ODRAW
        if (lst->size() <= c.red) {
            qWarning() << "Warning: Incorrect size of rgSchemeColor! Returning an invalid color!";
            return ret;
        } else {
	    ColorStruct cs;
            cs = lst->value(c.red);
            ret = QColor(cs.red, cs.green, cs.blue);
        }
    } else {
        ret = QColor(c.red, c.green, c.blue);
    }
    return ret;
}

void PptToOdp::processTextAutoNumberScheme(int val, QString& numFormat, QString& numSuffix, QString& numPrefix)
{
    switch (val) {

    case ANM_AlphaLcPeriod:         //Example: a., b., c., ...Lowercase Latin character followed by a period.
        numFormat = "a";
        numSuffix = ".";
        break;

    case ANM_AlphaUcPeriod:        //Example: A., B., C., ...Uppercase Latin character followed by a period.
        numFormat = "A";
        numSuffix = ".";
        break;

    case ANM_ArabicParenRight:     //Example: 1), 2), 3), ...Arabic numeral followed by a closing parenthesis.
        numFormat = "1";
        numSuffix = ")";
        break;

    case ANM_ArabicPeriod :        //Example: 1., 2., 3., ...Arabic numeral followed by a period.
        numFormat = "1";
        numSuffix = ".";
        break;

    case ANM_RomanLcParenBoth:     //Example: (i), (ii), (iii), ...Lowercase Roman numeral enclosed in parentheses.
        numPrefix = "(";
        numFormat = "i";
        numSuffix = ")";
        break;

    case ANM_RomanLcParenRight:    //Example: i), ii), iii), ... Lowercase Roman numeral followed by a closing parenthesis.
        numFormat = "i";
        numSuffix = ")";
        break;

    case ANM_RomanLcPeriod :        //Example: i., ii., iii., ...Lowercase Roman numeral followed by a period.
        numFormat = "i";
        numSuffix = ".";
        break;

    case ANM_RomanUcPeriod:         //Example: I., II., III., ...Uppercase Roman numeral followed by a period.
        numFormat = "I";
        numSuffix = ".";
        break;

    case ANM_AlphaLcParenBoth:      //Example: (a), (b), (c), ...Lowercase alphabetic character enclosed in parentheses.
        numPrefix = "(";
        numFormat = "a";
        numSuffix = ")";
        break;

    case ANM_AlphaLcParenRight:     //Example: a), b), c), ...Lowercase alphabetic character followed by a closing
        numFormat = "a";
        numSuffix = ")";
        break;

    case ANM_AlphaUcParenBoth:      //Example: (A), (B), (C), ...Uppercase alphabetic character enclosed in parentheses.
        numPrefix = "(";
        numFormat = "A";
        numSuffix = ")";
        break;

    case ANM_AlphaUcParenRight:     //Example: A), B), C), ...Uppercase alphabetic character followed by a closing
        numFormat = "A";
        numSuffix = ")";
        break;

    case ANM_ArabicParenBoth:       //Example: (1), (2), (3), ...Arabic numeral enclosed in parentheses.
        numPrefix = "(";
        numFormat = "1";
        numSuffix = ")";
        break;

    case ANM_ArabicPlain:           //Example: 1, 2, 3, ...Arabic numeral.
        numFormat = "1";
        break;

    case ANM_RomanUcParenBoth:      //Example: (I), (II), (III), ...Uppercase Roman numeral enclosed in parentheses.
        numPrefix = "(";
        numFormat = "I";
        numSuffix = ")";
        break;

    case ANM_RomanUcParenRight:     //Example: I), II), III), ...Uppercase Roman numeral followed by a closing parenthesis.
        numFormat = "I";
        numSuffix = ")";
        break;

    default:
        numFormat = "i";
        numSuffix = ".";
        break;
    }
}

const TextContainer* PptToOdp::getTextContainer(
            const PptOfficeArtClientTextBox* clientTextbox,
            const PptOfficeArtClientData* clientData) const
{
    if (clientData && clientData->placeholderAtom && currentSlideTexts) {
        const PlaceholderAtom* p = clientData->placeholderAtom.data();
        if (p->position >= 0 && p->position < currentSlideTexts->atoms.size()) {
            return &currentSlideTexts->atoms[p->position];
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
            QString headerText = QString::fromAscii(headerAtom->header, headerAtom->header.size());
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
                xmlWriter->startElement("presentation:date-time-decl");
                xmlWriter->addAttribute("presentation:name", item.first);
                xmlWriter->addAttribute("presentation:source", "current-date");
                //xmlWrite->addAttribute("style:data-style-name", "Dt1");
                xmlWriter->endElement();  // presentation:date-time-decl
            }
        } else if (slideHF->hfAtom.fHasUserDate) {
            QList<QPair<QString, QString> >items = declaration.values(DateTime);
            for( int i = 0; i < items.size(); ++i) {
                QPair<QString, QString > item = items.at(i);
                xmlWriter->startElement("presentation:date-time-decl");
                xmlWriter->addAttribute("presentation:name", item.first);
                xmlWriter->addAttribute("presentation:source", "fixed");
                xmlWriter->addTextNode(item.second);
                //Future - Add Fixed date data here
                xmlWriter->endElement();  //presentation:date-time-decl
            }
        }
        if (headerAtom && slideHF->hfAtom.fHasHeader) {
            QList< QPair < QString, QString > > items = declaration.values(Header);
            for( int i = items.size()-1; i >= 0; --i) {
                QPair<QString, QString > item = items.value(i);
                xmlWriter->startElement("presentation:header-decl");
                xmlWriter->addAttribute("presentation:name", item.first);
                xmlWriter->addTextNode(item.second);
                xmlWriter->endElement();  //presentation:header-decl
            }
        }
        if (footerAtom && slideHF->hfAtom.fHasFooter) {
            QList< QPair < QString, QString > > items = declaration.values(Footer);
            for( int i = items.size()-1 ; i >= 0; --i) {
                QPair<QString, QString > item = items.at(i);
                xmlWriter->startElement("presentation:footer-decl");
                xmlWriter->addAttribute("presentation:name", item.first);
                xmlWriter->addTextNode(item.second);
                xmlWriter->endElement();  //presentation:footer-decl
            }
        }
    }
}

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



