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

#include <kdebug.h>
#include <KoOdf.h>
#include <KoOdfWriteStore.h>
#include <KoXmlWriter.h>

#include <QtCore/QBuffer>


using namespace PPT;

namespace
{
static const QString mm("%1mm");


/**
 * Return the bounding rectangle for this object.
 **/
QRect
getRect(const OfficeArtFSPGR &r)
{
    return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
}
/**
 * Return the bounding rectangle for this object.
 **/
QRect
getRect(const OfficeArtClientAnchor &a)
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

/**
 * Return the placementId of the PlaceHolderAtom
 **/
quint8 getPlacementId(const OfficeArtSpContainer &o) {
    if (o.clientData) {
        const OfficeArtClientData & d = *o.clientData;
        if (d.placeholderAtom) {
            const PlaceholderAtom &h = *d.placeholderAtom;
            return h.placementId;
        }
        return 0;
    }
    return 0;
}

}//namespace

/**
 * Return the bounding rectangle for this object.
 **/
QRect
PptToOdp::getRect(const OfficeArtSpContainer &o)
{
    if (o.childAnchor) {
        const OfficeArtChildAnchor& r = *o.childAnchor;
        return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
    } else if (o.clientAnchor) {
        return ::getRect(*o.clientAnchor);
    }
    return QRect(0, 0, 1, 1);
}
PptToOdp::Writer::Writer(KoXmlWriter& xmlWriter) : xOffset(0),
        yOffset(0),
        scaleX(25.4 / 576),
        scaleY(25.4 / 576),
        xml(xmlWriter)
{
}

PptToOdp::Writer
PptToOdp::Writer::transform(const QRectF& oldCoords, const QRectF &newCoords) const
{
    Writer w(xml);
    w.xOffset = xOffset + oldCoords.x() * scaleX;
    w.yOffset = yOffset + oldCoords.y() * scaleY;
    w.scaleX = scaleX * oldCoords.width() / newCoords.width();
    w.scaleY = scaleY * oldCoords.height() / newCoords.height();
    w.xOffset -= w.scaleX * newCoords.x();
    w.yOffset -= w.scaleY * newCoords.y();
    return w;
}
QString PptToOdp::Writer::vLength(qreal length)
{
    return mm.arg(length*scaleY);
}

QString PptToOdp::Writer::hLength(qreal length)
{
    return mm.arg(length*scaleX);
}

QString PptToOdp::Writer::vOffset(qreal offset)
{
    return mm.arg(yOffset + offset*scaleY);
}

QString PptToOdp::Writer::hOffset(qreal offset)
{
    return mm.arg(xOffset + offset*scaleX);
}

PptToOdp::PptToOdp() : p(0)
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

void
addElement(KoGenStyle& style, const char* name,
           const QMap<const char*, QString>& m,
           const QMap<const char*, QString>& mtext)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);
    elementWriter.startElement(name);
    QMapIterator<const char*, QString> i(m);
    while (i.hasNext()) {
        i.next();
        elementWriter.addAttribute(i.key(), i.value());
    }
    if (mtext.size()) {
        elementWriter.startElement("style:text-properties");
        QMapIterator<const char*, QString> j(mtext);
        while (j.hasNext()) {
            j.next();
            elementWriter.addAttribute(j.key(), j.value());
        }
        elementWriter.endElement();
    }
    elementWriter.endElement();
    style.addChildElement(name,
                          QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
}

QString
definePageLayout(KoGenStyles& styles, const PPT::PointStruct& size) {
    // x and y are given in master units (1/576 inches)
    double sizeX = size.x * (25.4 / (double)576);
    double sizeY = size.y * (25.4 / (double)576);
    QString pageWidth = QString("%1mm").arg(sizeX);
    QString pageHeight = QString("%1mm").arg(sizeY);

    KoGenStyle pl(KoGenStyle::StylePageLayout);
    pl.setAutoStyleInStylesDotXml(true);
    // pl.addAttribute("style:page-usage", "all"); // probably not needed
    pl.addProperty("fo:margin-bottom", "0pt");
    pl.addProperty("fo:margin-left", "0pt");
    pl.addProperty("fo:margin-right", "0pt");
    pl.addProperty("fo:margin-top", "0pt");
    pl.addProperty("fo:page-height", pageHeight);
    pl.addProperty("fo:page-width", pageWidth);
    pl.addProperty("style:print-orientation", "landscape");
    return styles.lookup(pl, "pm");
}

template<class T>
const T*
getPP(const DocumentContainer* dc) {
    if (dc == 0 || dc->docInfoList == 0) return 0;
    foreach (const DocInfoListSubContainerOrAtom& a, dc->docInfoList->rgChildRec) {
        const DocProgTagsContainer* d = a.anon.get<DocProgTagsContainer>();
        if (d) {
            foreach (const DocProgTagsSubContainerOrAtom& da, d->rgChildRec) {
                const DocProgBinaryTagContainer* c
                        = da.anon.get<DocProgBinaryTagContainer>();
                if (c) {
                    const T* t = c->rec.anon.get<T>();
                    if (t) return t;
                }
            }
        }
    }
    return 0;
}

void PptToOdp::defineDefaultTextStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="text">
    KoGenStyle style(KoGenStyle::StyleText, "text");
    style.setDefaultStyle(true);
    defineDefaultTextProperties(style);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultParagraphStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="paragraph">
    KoGenStyle style(KoGenStyle::StyleUser, "paragraph");
    style.setDefaultStyle(true);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultSectionStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="section">
    KoGenStyle style(KoGenStyle::StyleSection, "section");
    style.setDefaultStyle(true);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultRubyStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="ruby">
    KoGenStyle style(KoGenStyle::StyleRuby, "ruby");
    style.setDefaultStyle(true);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultTableStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table">
    KoGenStyle style(KoGenStyle::StyleTable, "table");
    style.setDefaultStyle(true);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultTableColumnStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table-column">
    KoGenStyle style(KoGenStyle::StyleTableColumn, "table-column");
    style.setDefaultStyle(true);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultTableRowStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table-row">
    KoGenStyle style(KoGenStyle::StyleTableRow, "table-row");
    style.setDefaultStyle(true);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultTableCellStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="table-cell">
    KoGenStyle style(KoGenStyle::StyleTableCell, "table-cell");
    style.setDefaultStyle(true);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultGraphicStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="graphic">
    KoGenStyle style(KoGenStyle::StyleGraphic, "graphic");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultPresentationStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="presentation">
    KoGenStyle style(KoGenStyle::StylePresentation, "presentation");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultDrawingPageStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="drawing-page">
    KoGenStyle style(KoGenStyle::StyleDrawingPage, "drawing-page");
    style.setDefaultStyle(true);
    const PPT::SlideHeadersFootersContainer* hf = getSlideHF();
    defineDrawingPageStyle(style, (hf) ?&hf->hfAtom :0);
    styles.lookup(style, "");
}

void PptToOdp::defineDefaultChartStyle(KoGenStyles& styles)
{
    // write style <style:default-style style:family="chart">
    KoGenStyle style(KoGenStyle::StyleChart, "chart");
    style.setDefaultStyle(true);
    defineDefaultGraphicProperties(style);
    defineDefaultParagraphProperties(style);
    defineDefaultTextProperties(style);
    styles.lookup(style, "");
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
        if (pp9) {
            cf9 = &pp9->textDefaultsAtom.cf9;
        }
        if (pp10 && pp10->textDefaultsAtom) {
            cf10 = &pp10->textDefaultsAtom->cf10;
        }
        si = &p->documentContainer->documentTextInfo.textSIDefaultsAtom.textSIException;
    }
    defineTextProperties(style, cf, cf9, cf10, si);
}

void PptToOdp::defineDefaultParagraphProperties(KoGenStyle& style) {
    const TextPFException* pf = 0;
    const TextPFException9* pf9 = 0;
    if (p->documentContainer) {
        if (p->documentContainer->documentTextInfo.textPFDefaultsAtom) {
            pf = &p->documentContainer->documentTextInfo.textPFDefaultsAtom->pf;
        }
        const PP9DocBinaryTagExtension* pp9 = getPP<PP9DocBinaryTagExtension>(
                p->documentContainer);
        if (pp9) {
            pf9 = &pp9->textDefaultsAtom.pf9;
        }
    }
    defineParagraphProperties(style, pf, pf9);
}

void PptToOdp::defineDefaultGraphicProperties(KoGenStyle& style) {
    style.addProperty("svg:stroke-width",
                          QString("%1pt").arg(0x2535 / 12700.f),
                          KoGenStyle::GraphicType);
    const OfficeArtDggContainer& drawingGroup
        = p->documentContainer->drawingGroup.OfficeArtDgg;
    const TextMasterStyleAtom& textMasterStyle
        = p->documentContainer->documentTextInfo.textMasterStyleAtom;
    defineGraphicProperties(style, drawingGroup, &textMasterStyle);
}

void PptToOdp::defineTextProperties(KoGenStyle& style,
                                     const TextCFException* cf,
                                     const TextCFException9* /*cf9*/,
                                     const TextCFException10* /*cf10*/,
                                     const TextSIException* si) {
    const KoGenStyle::PropertyType text = KoGenStyle::TextType;
    /* We try to get information for all the possible attributes in
       style:text-properties for clarity we handle then in alphabetical order */
    // fo:background-color
    // fo:color
    if (cf && cf->masks.color && cf->color) {
        QColor color = toQColor(*cf->color);
        if (color.isValid()) {
            style.addProperty("fo:color", color.name(), text);
        }
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
        style.addProperty("fo:font-size", QString("%1pt").arg(cf->fontSize),
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
                          cf->fontStyle->bold ?"1pt 1pt" :"none", text);
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
                                         const TextPFException* pf,
                                         const TextPFException9* pf9) {
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
    // fo:margin-left
    // fo:margin-right
    // fo:margin-top
    // fo:orphans
    // fo:padding
    // fo:padding-bottom
    // fo:padding-left
    // fo:padding-right
    // fo:padding-top
    // fo:text-align
    if (pf && pf->masks.align) {
        const QString align = textAlignmentToString(pf->textAlignment);
        if (!align.isEmpty()) {
            style.addProperty("fo:text-align", align, para);
        }
    }
    // fo:text-align-last
    // fo:text-indent
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

void PptToOdp::defineDrawingPageStyle(KoGenStyle& style,
                                      const HeadersFootersAtom* hf)
{
    const KoGenStyle::PropertyType dp = KoGenStyle::DrawingPageType;
    // draw:background-size
    // draw:fill-color
    // draw:fill-gradient-name
    // draw:fill-hatch-name
    // draw:fill-hatch-solid
    // draw:fill-image-height
    // draw:fill-image-name
    // draw:fill-image-ref-point-x
    // draw:fill-image-ref-point-y
    // draw:fill-image-ref-point
    // draw:fill-image-width
    // draw:fill
    // draw:gradient-step-count
    // draw:opacity-name
    // draw:opacity
    // draw:secondary-fill-color
    // draw:tile-repeat-offset
    // presentation:background-objects-visible
    // presentation:background-visible
    // presentation:display-date-time
    if (hf) {
        qDebug() << hf->fHasDate;
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
    // smil:direction
    // smil:fadeColor
    // smil:subtype
    // smil:type
    // style:repeat
    // svg:fill-rule
}

void PptToOdp::defineListStyle(KoGenStyle& style,
                               const TextMasterStyleAtom& levels,
                               const TextMasterStyle9Atom* levels9,
                               const TextMasterStyle10Atom* levels10)
{
    if (levels.lstLvl1) {
        defineListStyle(style, 1, *levels.lstLvl1,
                        ((levels9) ?levels9->lstLvl1.data() :0),
                        ((levels10) ?levels10->lstLvl1.data() :0));
    }
    if (levels.lstLvl2) {
        defineListStyle(style, 2, *levels.lstLvl2,
                        ((levels9) ?levels9->lstLvl2.data() :0),
                        ((levels10) ?levels10->lstLvl2.data() :0));
    }
    if (levels.lstLvl3) {
        defineListStyle(style, 3, *levels.lstLvl3,
                        ((levels9) ?levels9->lstLvl3.data() :0),
                        ((levels10) ?levels10->lstLvl3.data() :0));
    }
    if (levels.lstLvl4) {
        defineListStyle(style, 1, *levels.lstLvl4,
                        ((levels9) ?levels9->lstLvl4.data() :0),
                        ((levels10) ?levels10->lstLvl4.data() :0));
    }
    if (levels.lstLvl5) {
        defineListStyle(style, 5, *levels.lstLvl5,
                        ((levels9) ?levels9->lstLvl5.data() :0),
                        ((levels10) ?levels10->lstLvl5.data() :0));
    }
}

void PptToOdp::defineListStyle(KoGenStyle& style, quint8 depth,
                               const TextMasterStyleLevel& level,
                               const TextMasterStyle9Level* level9,
                               const TextMasterStyle10Level* level10)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter out(&buffer);
    QString elementName;

    const TextPFException& pf = level.pf;
    const TextPFException9* pf9 = (level9) ?&level9->pf9 :0;
    const TextCFException& cf = level.cf;
    const TextCFException9* cf9 = (level9) ?&level9->cf9 :0;
    const TextCFException10* cf10 = (level10) ?&level10->cf10 :0;

    if (pf9 && pf9->masks.bulletBlip) {
        elementName = "text:list-level-style-image";
        out.startElement("text:list-level-style-image");
        out.addAttribute("xlink:href", "TODO");
    } else {
        QString numFormat, numSuffix, numPrefix;
        if (pf9 && pf9->bulletAutoNumberScheme) {
            processTextAutoNumberScheme(pf9->bulletAutoNumberScheme->scheme,
                                        numFormat, numSuffix, numPrefix);
        }
        if (pf.masks.hasBullet) {
            elementName = "text:list-level-style-bullet";
            out.startElement("text:list-level-style-bullet");
            if (pf.masks.bulletChar) {
                out.addAttribute("text:bullet-char", QChar(pf.bulletChar));
            } else {
                // the character from the parent should be specified here,
                // because text:list-level-style-bullet must have a bullet
                const char bullet[4] = {0xe2, 0x97, 0x8f, 0}; //  "●";
                out.addAttribute("text:bullet-char", QString::fromUtf8(bullet));
            }
            if (pf.masks.bulletSize && pf.bulletFlags->fBulletHasSize) {
                if (pf.bulletSize >= 25 && pf.bulletSize <= 400) {
                    out.addAttribute("text:bullet-relative-size",
                                     QString("%1%").arg(pf.bulletSize));
                } else if (pf.bulletSize >= -4000 && pf.bulletSize <= -1) {
                    out.addAttribute("text:bullet-relative-size",
                                     QString("%1pt").arg(pf.bulletSize));
                }
            }
        } else {
            elementName = "text:list-level-style-number";
            out.startElement("text:list-level-style-number");
            if (!numFormat.isNull()) {
                out.addAttribute("style:num-format", numFormat);
            }
            //out.addAttribute("style:display-levels", "TODO");
            if (pf9 && pf9->masks.bulletScheme) {
                out.addAttribute("style:start-value",
                                 pf9->bulletAutoNumberScheme->startNum);
            }
        }
        if (!numPrefix.isNull()) {
            out.addAttribute("style:num-prefix", numPrefix);
        }
        if (!numSuffix.isNull()) {
            out.addAttribute("style:num-suffix", numSuffix);
        }
    }
    out.addAttribute("text:level", depth);

    bool hasIndent = pf.masks.indent && depth - 1 == pf.indent;
    out.startElement("style:list-level-properties");
    // fo:height
    // fo:text-align
    // fo:width
    // style:font-name
    // style:vertical-pos
    // style:vertical-rel
    // svg:y
    // text:min-label-distance
    // text:min-label-width
    // text:space-before
    if (hasIndent && pf.masks.spaceBefore) {
        out.addAttribute("text:space-before",
                         paraSpacingToCm(pf.spaceBefore));
    }
    out.endElement(); // style:list-level-properties

    KoGenStyle ls(KoGenStyle::StyleText);
    defineTextProperties(ls, &cf, cf9, cf10, 0);

    // override some properties with information from the paragraph

    if (hasIndent && pf.masks.bulletFont) {
        const PPT::FontEntityAtom* font = getFont(pf.bulletFontRef);
        if (font) {
            ls.addProperty("fo:font-family",
                           QString::fromUtf16(font->lfFaceName.data(),
                                              font->lfFaceName.size()),
                           KoGenStyle::TextType);
        }
    }
    if (hasIndent && pf.masks.bulletColor) {
        const QColor color = toQColor(*pf.bulletColor);
        if (color.isValid()) {
            ls.addProperty("fo:color", color.name(), KoGenStyle::TextType);
        }
    }
    // maybe fo:font-size should be set from pf.bulletSize

    ls.writeStyleProperties(&out, KoGenStyle::TextType);

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
void handleOfficeArtContainer(O& handler, const PPT::OfficeArtDgContainer& c) {
    if (c.shape) {
        handler.handle(*c.shape);
    }
    foreach (const OfficeArtSpgrContainerFileBlock& fb, c.groupShape.rgfb) {
        handleOfficeArtContainer(handler, fb);
    }
}

class PlaceholderFinder {
public:
    int wanted;
    const PPT::OfficeArtSpContainer* sp;
    PlaceholderFinder(int w) :wanted(w), sp(0) {}

    void handle(const PPT::OfficeArtSpContainer& o) {
        if (o.clientTextbox) {
            foreach (const TextClientDataSubContainerOrAtom& a, o.clientTextbox->rgChildRec) {
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
};

void PptToOdp::defineMasterAutomaticStyles(KoGenStyles& styles)
{
    int n = 0;
    foreach (const PPT::MasterOrSlideContainer* m, p->masters) {
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();

        // look for a style for each of the values of TextEnumType
        for (int texttype = 0; texttype <= 8; ++texttype) {
            // look for placeholder and a TextMasterStyleAtom
            const TextMasterStyleAtom* textstyle = 0;
            PlaceholderFinder finder(texttype);
            if (sc) {
                handleOfficeArtContainer(finder, sc->drawing.OfficeArtDg);
            } else if (mm) {
                handleOfficeArtContainer(finder, mm->drawing.OfficeArtDg);
                foreach (const TextMasterStyleAtom&ma, mm->rgTextMasterStyle) {
                    if (ma.rh.recInstance == texttype) {
                        textstyle = &ma;
                    }
                }
            }
            // graphic family
            KoGenStyle style(KoGenStyle::StyleGraphicAuto, "graphic");
            style.setAutoStyleInStylesDotXml(true);
            if (finder.sp) {
                defineGraphicProperties(style, *finder.sp, textstyle);
            } else if (textstyle) {
                defineGraphicPropertiesListStyles(style, *textstyle);
            } else {
                continue;
            }
            if (textstyle && textstyle->lstLvl1) {
                defineParagraphProperties(style, &textstyle->lstLvl1->pf, 0);
                defineTextProperties(style, &textstyle->lstLvl1->cf, 0, 0, 0);
            }
            const QString name = "M" + QString::number(n) + "_"
                                     + QString::number(texttype);
            styles.lookup(style, name + "g", KoGenStyles::DontForceNumbering);
            if (textstyle && textstyle->lstLvl1) {
                // text family
                KoGenStyle tstyle(KoGenStyle::StyleTextAuto, "text");
                tstyle.setAutoStyleInStylesDotXml(true);
                defineTextProperties(tstyle, &textstyle->lstLvl1->cf, 0, 0, 0);
                styles.lookup(tstyle, name + "t", KoGenStyles::DontForceNumbering);
                // paragraph family
                KoGenStyle pstyle(KoGenStyle::StyleAuto, "paragraph");
                pstyle.setAutoStyleInStylesDotXml(true);
                defineParagraphProperties(pstyle, &textstyle->lstLvl1->pf, 0);
                defineTextProperties(pstyle, &textstyle->lstLvl1->cf, 0, 0, 0);
                styles.lookup(pstyle, name + "p", KoGenStyles::DontForceNumbering);
            }
        }
    }
}
void PptToOdp::defineAutomaticDrawingPageStyles(KoGenStyles& styles)
{
    // define for master for use in <master-page style:name="...">
    foreach (const PPT::MasterOrSlideContainer* m, p->masters) {
        KoGenStyle dp(KoGenStyle::StyleDrawingPage, "drawing-page");
        dp.setAutoStyleInStylesDotXml(true);
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
        const HeadersFootersAtom* hf = 0;
        if (sc && sc->perSlideHFContainer) {
            hf = &sc->perSlideHFContainer->hfAtom;
        } else if (mm && mm->perSlideHeadersFootersContainer) {
            hf = &mm->perSlideHeadersFootersContainer->hfAtom;
        }
        defineDrawingPageStyle(dp, hf);
        drawingPageStyles[m] = styles.lookup(dp);
    }
    if (p->notesMaster) {
        const HeadersFootersAtom* hf = 0;
        if (p->notesMaster->perSlideHFContainer) {
            hf = &p->notesMaster->perSlideHFContainer->hfAtom;
        } else if (p->notesMaster->perSlideHFContainer2) {
            hf = &p->notesMaster->perSlideHFContainer2->hfAtom;
        }
        KoGenStyle dp(KoGenStyle::StyleDrawingPage, "drawing-page");
        dp.setAutoStyleInStylesDotXml(false);
        defineDrawingPageStyle(dp, hf);
        drawingPageStyles[p->notesMaster] = styles.lookup(dp);
    }

    // define for handouts for use in <style:handout-master style:name="...">
    // TODO

    // define for slides for use in <draw:page style:name="...">
    foreach (const PPT::SlideContainer* sc, p->slides) {
        KoGenStyle dp(KoGenStyle::StyleDrawingPage, "drawing-page");
        dp.setAutoStyleInStylesDotXml(false);
        const HeadersFootersAtom* hf = 0;
        if (sc->perSlideHFContainer) {
            hf = &sc->perSlideHFContainer->hfAtom;
        }
        defineDrawingPageStyle(dp, hf);
        drawingPageStyles[sc] = styles.lookup(dp);
    }

    // define for notes for use in <presentation:notes style:name="...">
    foreach (const PPT::NotesContainer* nc, p->notes) {
        const HeadersFootersAtom* hf = 0;
        if (nc->perSlideHFContainer) {
            hf = &nc->perSlideHFContainer->hfAtom;
        } else if (nc->perSlideHFContainer2) {
            hf = &nc->perSlideHFContainer2->hfAtom;
        }
        KoGenStyle dp(KoGenStyle::StyleDrawingPage, "drawing-page");
        dp.setAutoStyleInStylesDotXml(false);
        defineDrawingPageStyle(dp, hf);
        drawingPageStyles[nc] = styles.lookup(dp);
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
    // TODO: draw:marker
    // draw:stroke-dash
    StrokeDashCollector strokeDashCollector(styles, *this);
    collectGlobalObjects(strokeDashCollector, *p);
    // TODO: draw:opacity

    /*
       Define the style:presentation-page-layout elements.
    */
    // TODO

    /*
       Define default styles for all 12 style families.
    */
    defineDefaultTextStyle(styles);
    defineDefaultParagraphStyle(styles);
    defineDefaultSectionStyle(styles);
    defineDefaultRubyStyle(styles);
    defineDefaultTableStyle(styles);
    defineDefaultTableColumnStyle(styles);
    defineDefaultTableRowStyle(styles);
    defineDefaultTableCellStyle(styles);
    defineDefaultGraphicStyle(styles);
    defineDefaultPresentationStyle(styles);
    defineDefaultDrawingPageStyle(styles);
    defineDefaultChartStyle(styles);

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
    defineMasterAutomaticStyles(styles);
    defineAutomaticDrawingPageStyles(styles);

    /*
      Define the draw:layer-set.
     */
    // TODO

    /*
      Define the style:handout-master
     */
    // TODO

    /*
      Define the style:master-pages
     */
    foreach (const PPT::MasterOrSlideContainer* m, p->masters) {
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
        const DrawingContainer* drawing = 0;
        if (sc) {
            drawing = &sc->drawing;
        } else if (mm) {
            drawing = &mm->drawing;
        }

        KoGenStyle master(KoGenStyle::StyleMaster);
        master.addAttribute("style:page-layout-name", slidePageLayoutName);
        master.addAttribute("draw:style-name", drawingPageStyles[m]);
        currentSlideTexts = 0;
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                drawing->OfficeArtDg.groupShape.rgfb) {
            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            KoXmlWriter writer(&buffer);
            Writer out(writer);
            processObjectForBody(co, out);
            master.addChildElement("draw:frame",
                                   QString::fromUtf8(buffer.buffer(),
                                                     buffer.buffer().size()));
        }
        masterNames[m] = styles.lookup(master, "M");
    }
    if (p->notesMaster) {
        KoGenStyle master(KoGenStyle::StyleMaster);
        master.addAttribute("style:page-layout-name", notesPageLayoutName);
        master.addAttribute("draw:style-name",
                            drawingPageStyles[p->notesMaster]);
        currentSlideTexts = 0;
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                p->notesMaster->drawing.OfficeArtDg.groupShape.rgfb) {
            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            KoXmlWriter writer(&buffer);
            Writer out(writer);
            processObjectForBody(co, out);
            master.addChildElement("draw:frame",
                                   QString::fromUtf8(buffer.buffer(),
                                                     buffer.buffer().size()));
        }
        notesMasterName = styles.lookup(master, "M");
    }

    // Creating dateTime class object
    if (getSlideHF()) {
        int dateTimeFomatId = getSlideHF()->hfAtom.formatId;
        bool hasTodayDate = getSlideHF()->hfAtom.fHasTodayDate;
        bool hasUserDate = getSlideHF()->hfAtom.fHasUserDate;
        dateTime = DateTimeFormat(dateTimeFomatId);
        dateTime.addDateTimeAutoStyles(styles, hasTodayDate, hasUserDate);
    }
}

void PptToOdp::addFrame(KoGenStyle& style, Writer& out, const char* presentationClass,
                 const QRect &rect, QString mStyle, QString pStyle, QString tStyle)
{
    QBuffer buffer;

    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);

    xmlWriter.startElement("draw:frame");
    xmlWriter.addAttribute("presentation:style-name", mStyle);
    xmlWriter.addAttribute("draw:layer", "layout");
    xmlWriter.addAttribute("svg:width", out.hLength(rect.width()));
    xmlWriter.addAttribute("svg:height", out.vLength(rect.height()));
    xmlWriter.addAttribute("svg:x", out.hOffset(rect.x()));
    xmlWriter.addAttribute("svg:y", out.vOffset(rect.y()));
    xmlWriter.addAttribute("presentation:class", presentationClass);
    xmlWriter.startElement("draw:text-box");
    xmlWriter.startElement("text:p");
    xmlWriter.addAttribute("text:style-name", pStyle);
    if (strcmp(presentationClass, "page-number") == 0) {
        xmlWriter.startElement("text:span");
        xmlWriter.addAttribute("text:style-name", tStyle);
        xmlWriter.startElement("text:page-number");
        xmlWriter.addTextNode("<number>");
        xmlWriter.endElement();//text:page-number
        xmlWriter.endElement(); // text:span
    }

    if (strcmp(presentationClass, "date-time") == 0) {
        //Same DateTime object so no need to pass style name for date time
        if (getSlideHF()) {
            if (getSlideHF()->hfAtom.fHasTodayDate) {
                dateTime.addMasterDateTimeSection(xmlWriter, tStyle);
            } else if (getSlideHF()->hfAtom.fHasUserDate) {
                //Future FixedDate format
            }
        }
    }
    if (strcmp(presentationClass, "footer") == 0) {
        xmlWriter.startElement("presentation:footer");
        xmlWriter.endElement(); // presentation:footer
    }
    if (strcmp(presentationClass, "header") == 0) {
        xmlWriter.startElement("presentation:header");
        xmlWriter.endElement(); // presentation:header
    }


    xmlWriter.endElement(); // text:p

    xmlWriter.endElement(); // draw:text-box
    xmlWriter.endElement(); // draw:frame
    style.addChildElement("draw:frame",
                          QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
}

QByteArray PptToOdp::createContent(KoGenStyles& styles)
{
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
    processContentStyles(styles);

    for (int c = 0; c < p->slides.size(); c++) {
        processSlideForStyle(c, styles);
    }

    styles.saveOdfAutomaticStyles(&contentWriter, false);

    // office:body
    contentWriter.startElement("office:body");
    contentWriter.startElement("office:presentation");

    processDeclaration(&contentWriter);

    for (int c = 0; c < p->slides.size(); c++) {
        processSlideForBody(c, contentWriter);
    }

    contentWriter.endElement();  // office:presentation

    contentWriter.endElement();  // office:body

    contentWriter.endElement();  // office:document-content
    contentWriter.endDocument();
    return contentData;
}

void PptToOdp::processContentStyles(KoGenStyles &styles)
{
    KoGenStyle dp(KoGenStyle::StyleDrawingPage, "drawing-page");
    dp.addProperty("presentation:background-objects-visible", "true");

    if (getSlideHF() && getSlideHF()->hfAtom.fHasSlideNumber)
        dp.addProperty("presentation:display-page-number", "true");
    else
        dp.addProperty("presentation:display-page-number", "false");

    if (getSlideHF() && getSlideHF()->hfAtom.fHasDate)
        dp.addProperty("presentation:display-date-time" , "true");
    else
        dp.addProperty("presentation:display-date-time" , "false");

    if (getSlideHF() && getSlideHF()->hfAtom.fHasHeader)
        dp.addProperty("presentation:display-header", "true");
    else
        dp.addProperty("presentation:display-header", "false");

    if (getSlideHF() && getSlideHF()->hfAtom.fHasFooter)
        dp.addProperty("presentation:display-footer", "true");
    else
        dp.addProperty("presentation:display-footer", "false");


    styles.lookup(dp, "dp");

    declarationStyleName = styles.lookup(dp);
}


void PptToOdp::writeTextSpan(KoXmlWriter& xmlWriter,
                                     const QString &text)
{
    QString copy = text;
    copy.remove('\v'); //Remove vertical tabs which appear in some ppt files
    copy.remove('\r'); //Carriage returns indicate line change, they shouldn't
                       //be here, so we'll remove them just to be safe
    xmlWriter.addTextSpan(copy);
}



PptToOdp::HyperlinkRange PptToOdp::findNextHyperlinkStart(const PPT::TextContainer& text,
                                                          const int currentPos)
{
    HyperlinkRange range;
    range.end = range.start = range.id = -1;

    //Last found hyperlink id reference
    int lastId = -1;

    /*
      Text contains list of MouseInteractiveInfoContainers and MouseTextInteractiveInfoAtoms
      in the same list.

      MouseInteractiveInfoContainer describes a action for a hyperlink
      and an ID for the hyperlink.

      MouseTextInteractiveInfoAtom describes the range of text that is to be
      replaced with hyperlink's user readable text.

      They are related together so that MouseTextInteractiveInfoAtoms
      always refers to previous MouseInteractiveInfoContainers.
      */
    for(int i=0;i<text.interactive.size();i++) {
        const TextContainerInteractiveInfo& t = text.interactive[i];
        const InteractiveInfoAtom* interactiveAtom = 0;
        const TextRange* textRange = 0;
        if (t.interactive.is<MouseClickInteractiveInfoContainer>()) {
            interactiveAtom = &t.interactive.get<MouseClickInteractiveInfoContainer>()->interactiveInfoAtom;
        } else if (t.interactive.is<MouseOverInteractiveInfoContainer>()) {
            interactiveAtom = &t.interactive.get<MouseOverInteractiveInfoContainer>()->interactiveInfoAtom;
        } else if (t.interactive.is<MouseClickTextInteractiveInfoAtom>()) {
            textRange = &t.interactive.get<MouseClickTextInteractiveInfoAtom>()->range;
        } else if (t.interactive.is<MouseOverTextInteractiveInfoAtom>()) {
            textRange = &t.interactive.get<MouseOverTextInteractiveInfoAtom>()->range;
        }
        if (interactiveAtom) {
            /**
            * [MS-PPT].PDF states exHyperlinkIdRef must be ignored unless action is
            * equal to II_JumpAction (0x3), II_HyperlinkAction (0x4), or
            * II_CustomShowAction (0x7).
            */
            if (interactiveAtom->action == II_JumpAction ||
                    interactiveAtom->action == II_HyperlinkAction ||
                    interactiveAtom->action == II_CustomShowAction) {
                lastId = interactiveAtom->exHyperlinkIdRef;
            } else { //TODO until we support other type of interactive actions, we'll ignore them
                lastId = -1;
            }
        } else if (textRange && lastId != -1) {
            if (textRange->begin >= currentPos &&
                lastId != -1 &&
                (range.start == -1 || range.start > textRange->begin)) {
                range.start = textRange->begin;
                range.end = textRange->end;
                range.id = lastId;
            }
        }
    }
    return range;
}

QString PptToOdp::utf16ToString(const QVector<quint16> &data)
{
    return QString::fromUtf16(data.data(), data.size());
}

QPair<QString, QString> PptToOdp::findHyperlink(const unsigned int id)
{
    foreach(ExObjListSubContainer container,p->documentContainer->exObjList->rgChildRec) {
        //Search all ExHyperlinkContainers for specified id
        if (container.anon.is<ExHyperlinkContainer>()) {
            ExHyperlinkContainer *hyperlink = container.anon.get<ExHyperlinkContainer>();
            if (hyperlink &&
                hyperlink->exHyperlinkAtom.exHyperLinkId == id &&
                hyperlink->targetAtom &&
                hyperlink->friendlyNameAtom) {
                //TODO currently location is ignored. Location referes to
                //position within a file
                return qMakePair(utf16ToString(hyperlink->targetAtom->target),
                                 utf16ToString(hyperlink->friendlyNameAtom->friendlyName));
            }
        }
    }

    return qMakePair(QString(""),QString(""));
}


void PptToOdp::writeTextSpanWithHyperlinks(KoXmlWriter& xmlWriter,
                                 const PPT::TextContainer& text,
                                 const int start,
                                 const int end)
{
    int currentPos = start;

    //Get the range of text for the next hyperlink
    HyperlinkRange range = findNextHyperlinkStart(text,currentPos);

    //loop through all text ranges before and between hyperlinks
    while(currentPos < end && range.start < end && range.start > -1) {

        //Check if we have one before the next hyperlink
        if (range.start-currentPos > 0) {
            writeTextSpan(xmlWriter,getText(text,currentPos,range.start-currentPos));
        }

        //Get specified hyperlink
        QPair<QString,QString> hyperlink = findHyperlink(range.id);

        //Write the hyperlink
        xmlWriter.startElement("text:a");

        //PPT file structure allows the target to be empty. In that case
        //we'll use the name as the target.
        if (hyperlink.first.isEmpty()) {
            xmlWriter.addAttribute("xlink:href",hyperlink.second);
        } else {
            xmlWriter.addAttribute("xlink:href",hyperlink.first);
        }
        xmlWriter.addTextSpan(hyperlink.second);
        xmlWriter.endElement();
        //Finally move to the position after hyperlink
        currentPos = range.end;
        //And get the next hyperlink
        range = findNextHyperlinkStart(text,currentPos);
    }

    //after writing all hyperlinks, check if there is still text to be written
    if (currentPos < end) {
        writeTextSpan(xmlWriter,getText(text,currentPos,end-currentPos));
    }
}

void PptToOdp::writeTextCFException(KoXmlWriter& xmlWriter,
                                    const TextCFException *cf,
                                    const TextPFException *pf,
                                    const PPT::TextContainer& tc,
                                    const unsigned int textPosition,
                                    const unsigned int textLength)
{
    xmlWriter.startElement("text:span");
    xmlWriter.addAttribute("text:style-name", getTextStyleName(cf, pf));
    writeTextSpanWithHyperlinks(xmlWriter,tc,textPosition,textPosition+textLength);
    xmlWriter.endElement(); // text:span
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

void PptToOdp::writeTextLine(KoXmlWriter& xmlWriter,
                             const StyleTextPropAtom& style,
                             const TextPFException *pf,
                             const PPT::TextContainer& tc,
                             const QString& text,
                             const unsigned int linePosition)
{
    QString part = "";
    unsigned int currentPos = linePosition;
    const TextCFRun *cf = findTextCFRun(style, linePosition);
    if (!cf) {
        return;
    }

    if (text.isEmpty()) {
        writeTextCFException(xmlWriter, &cf->cf, pf, tc,currentPos,text.size());
        return;
    }

    //Iterate through all the characters in text
    for (int i = 0;i < text.length();i++) {
        const TextCFRun *nextCFRun = findTextCFRun(style, linePosition + i);

        //While character exception stays the same
        if (cf == nextCFRun) {
            //Catenate strings to our substring
            part += text[i];
        } else {
            /*
            When exception changes we write the text to xmlwriter unless the
            text style name stays the same, then we'll reuse the same
            stylename for the next character exception
            */
            if (nextCFRun &&
                    getTextStyleName(&cf->cf, pf) != getTextStyleName(&nextCFRun->cf, pf)) {
                writeTextCFException(xmlWriter,
                                     &cf->cf,
                                     pf,
                                     tc,
                                     currentPos,
                                     part.size());

                currentPos += part.size();
                part = text[i];
            } else {
                part += text[i];
            }

            cf = nextCFRun;
        }
    }

    //If at the end we still have some text left, write it out
    if (!part.isEmpty()) {
        writeTextCFException(xmlWriter,
                             &cf->cf,
                             pf,
                             tc,
                             currentPos,
                             part.size());
    }
}

void PptToOdp::writeTextObjectDeIndent(KoXmlWriter& xmlWriter,
                                       const unsigned int count, QStack<QString>& levels)
{
    while ((unsigned int)levels.size() > count) {
        // if the style name at the lowest level is empty, there is no
        // list there
        if (levels.size() > 1
                || (levels.size() == 1 && !levels.top().isNull())) {
            xmlWriter.endElement(); // text:list
        }
        levels.pop();
        if (levels.size() > 1
                || (levels.size() == 1 && !levels.top().isNull())) {
            xmlWriter.endElement(); // text:list-item
        }
    }
}
namespace
{
void addListElement(KoXmlWriter& xmlWriter, QStack<QString>& levels,
                    const QString& listStyle)
{
    if (!listStyle.isNull()) {
        // if the context is a text:list, a text:list-item is needed
        if (levels.size() > 1
                || (levels.size() == 1 && !levels.top().isNull())) {
            xmlWriter.startElement("text:list-item");
        }
        xmlWriter.startElement("text:list");
        if (!listStyle.isEmpty()) {
            xmlWriter.addAttribute("text:style-name", listStyle);
        }
    }
    levels.push(listStyle);
}
}

void PptToOdp::writeTextPFException(KoXmlWriter& xmlWriter,
                                    const TextPFRun *pf,
                                    const TextContainer& tc,
                                    const QString& intext,
                                    const unsigned int textPos,
                                    QStack<QString>& levels)
{
    const StyleTextPropAtom* style = tc.style.data();
    if (!pf || !style) {
        return;
    }

    QString text = intext.mid(textPos, pf->count);
    //Text lines are separated with carriage return
    //There seems to be an extra carriage return at the end. We'll remove the last
    // carriage return so we don't end up with a single empty line in the end
    if (text.endsWith("\r")) {
        text = text.left(text.length() - 1);
    }

    //Then split the text into lines
    QStringList lines = text.split("\r");
    unsigned int linePos = textPos;

    //Indentation level paragraph wants
    unsigned int paragraphIndent = pf->indentLevel;
    //[MS-PPT].pdf says the itendation level can be 4 at most
    if (paragraphIndent > 4) paragraphIndent = 4;
    qint16 bullet = 0;
    //Check if this paragraph has a bullet
    if (pf->pf.masks.bulletChar) {
        bullet = pf->pf.bulletChar;
    } else {
        //If text paragraph exception doesn't have a definition on bullet
        //then we'll have to check master style with our indentation level
        const TextPFException *masterPF
                = masterTextPFException(tc.textHeaderAtom.textType,
                                        pf->indentLevel);
        if (masterPF && masterPF->masks.bulletChar) {
            bullet = masterPF->bulletChar;
        }
    }
    const TextCFRun *cf = findTextCFRun(&style, linePos);
    // find the name of the list style, if the current style is a list
    // if the style is null, there is no, list, if the style is an empty string
    // there is a list but it has no style
    QString listStyle;
    if (cf && (bullet || paragraphIndent > 0)) {
        listStyle = getListStyleName(&cf->cf, &pf->pf);
    }
    if (listStyle.isNull() && (bullet || paragraphIndent > 0)) {
        listStyle = "";
    }
    // remove levels until the top level is the right indentation
    if ((unsigned int)levels.size() > paragraphIndent
            && levels[paragraphIndent] == listStyle) {
        writeTextObjectDeIndent(xmlWriter, paragraphIndent + 2, levels);
    } else {
        writeTextObjectDeIndent(xmlWriter, paragraphIndent + 1, levels);
    }
    // add styleless levels up to the current level of indentation
    while ((unsigned int)levels.size() < paragraphIndent) {
        addListElement(xmlWriter, levels, "");
    }
    // at this point, levels.size() == paragraphIndent
    if (levels.size() == 0 || listStyle != levels.top()) {
        addListElement(xmlWriter, levels, listStyle);
    }
    bool listItem = levels.size() > 1 || !levels.top().isNull();
    if (listItem) {
        xmlWriter.startElement("text:list-item");
    }
    QString pstyle;
    if (cf) {
        pstyle = getParagraphStyleName(&cf->cf, &pf->pf);
    }
    xmlWriter.startElement("text:p");
    if (pstyle.size() > 0) {
        xmlWriter.addAttribute("text:style-name", pstyle);
    }

    //Handle all lines
    for (int i = 0;i < lines.size();i++) {
        cf = findTextCFRun(*style, linePos);
        if (cf) {
            writeTextCFException(xmlWriter, &cf->cf, &pf->pf, tc,linePos,lines[i].size());
        }

        //Add +1 to line position to compensate for carriage return that is
        //missing from line due to the split method
        linePos += lines[i].size() + 1;
    }
    xmlWriter.endElement(); // text:p
    if (listItem) {
        xmlWriter.endElement(); // text:list-item
    }
}
namespace Text
{
enum {
    Title       = 0,
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

/* The placementId is mapped to one of
   "chart", "date-time", "footer", "graphic", "handout", "header", "notes",
   "object", "orgchart", "outline", "page", "page-number", "subtitle", "table",
   "text" or "title" */
const char*
getPresentationClass(const PlaceholderAtom* p)
{
    if (p == 0) return 0;
    switch (p->placementId) {
    case 0x01: return "title";       // PT_MasterTitle
    case 0x02: return "text";        // PT_MasterBody
    case 0x03: return "title";       // PT_MasterCenterTitle
    case 0x04: return "subtitle";    // PT_MasterSubTitle
    case 0x05: return "graphic";     // PT_MasterNotesSlideImage
    case 0x06: return "text";        // PT_MasterNotesBody
    case 0x07: return "date-time";   // PT_MasterDate
    case 0x08: return "page-number"; // PT_MasterSlideNumber
    case 0x09: return "footer";      // PT_MasterFooter
    case 0x0A: return "header";      // PT_MasterHeader
    case 0x0B: return "page";        // PT_NotesSlideImage
    case 0x0C: return "notes";       // PT_NotesBody
    case 0x0D: return "title";       // PT_Title
    case 0x0E: return "text";        // PT_Body
    case 0x0F: return "title";       // PT_CenterTitle
    case 0x10: return "subtitle";    // PT_SubTitle
    case 0x11: return "title";       // PT_VerticalTitle
    case 0x12: return "text";        // PT_VerticalBody
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
qint32
getPosition(const TextContainerMeta& m)
{
    if (m.meta.is<SlideNumberMCAtom>())
        return m.meta.get<SlideNumberMCAtom>()->position;
    if (m.meta.is<DateTimeMCAtom>())
        return m.meta.get<DateTimeMCAtom>()->position;
    if (m.meta.is<GenericDateMCAtom>())
        return m.meta.get<GenericDateMCAtom>()->position;
    if (m.meta.is<HeaderMCAtom>())
        return m.meta.get<HeaderMCAtom>()->position;
    if (m.meta.is<FooterMCAtom>())
        return m.meta.get<FooterMCAtom>()->position;
    if (m.meta.is<RTFDateTimeMCAtom>())
        return m.meta.get<RTFDateTimeMCAtom>()->position;
    return -1;
}

void PptToOdp::processTextForBody(const PPT::TextContainer& tc, Writer& out)
{
    /* Text in a textcontainer is divided into sections.
       The sections occur on different levels:
       - paragraph (TextPFRun) 1-n characters
       - character (TextCFRun) 1-n characters
       - variables (TextContainerMeta) 1 character
       - spelling and language (TextSIRun) 1-n characters
       - indentation (MasterTextPropRun) 1-n characters

       Variables are the smallest level, they should be replaced by special
       xml elements.
       TextCFRun is probably the next smallest section. We make the assumption
       that ranges from all other levels contain complete TextCFRun ranges.
       Another assumption is that each line is a separate TextPFRun.
    */

    QString text = getText(tc);

    const StyleTextPropAtom *style = tc.style.data();
    if (style  && style->rgTextPFRun.size()) {
        //Paragraph formatting that applies to substring
        QStack<QString> levels;
        levels.reserve(5);
        int pos = 0;
        foreach(const TextPFRun& pf, style->rgTextPFRun) {
            writeTextPFException(out.xml, &pf, tc, text, pos, levels);
            pos += pf.count;
        }

        writeTextObjectDeIndent(out.xml, 0, levels);
    } else {
        //For text that don't have StyleTextPropAtom we'll assume
        //they are in a list.

        //There seems to be an extra carriage return at the end. We'll remove the last
        // carriage return so we don't end up with a single empty line in the end
        if (text.endsWith("\r")) {
            text = text.left(text.length() - 1);
        }

        //Lines are separated by \r, so we'll split the text to lines
        QStringList lines = text.split("\r");

        unsigned int linePos = 0;

        out.xml.startElement("text:list");

        if (!getListStyleName(0, 0).isEmpty()) {
            out.xml.addAttribute("text:style-name", getListStyleName(0, 0));
        }

        //Then write each line from the list as list-item
        for(int i=0;i<lines.size();i++) {
            out.xml.startElement("text:list-item");
            out.xml.startElement("text:p");

            if (!getParagraphStyleName(0, 0).isEmpty()) {
                out.xml.addAttribute("text:style-name", getParagraphStyleName(0, 0));
            }

            out.xml.startElement("text:span");
            if (!getTextStyleName(0, 0).isEmpty()) {
                out.xml.addAttribute("text:style-name", getTextStyleName(0, 0));
            }
            //Write the text and any possible hyperlinks as well
            writeTextSpanWithHyperlinks(out.xml,tc,linePos,linePos+lines[i].size());
            linePos += lines[i].size() + 1; //compensate +1 for removed '\r'

            out.xml.endElement(); // text:span
            out.xml.endElement(); // text:p

            out.xml.endElement(); //text:list-item
        }

        out.xml.endElement(); //text:list
    }
}

void PptToOdp::processTextObjectForBody(const OfficeArtSpContainer& o,
                                        const PPT::TextContainer& tc,
                                        Writer& out)
{
    const PlaceholderAtom* p = 0;
    if (o.clientData) {
        p = o.clientData->placeholderAtom.data();
    }
    const char* const classStr = getPresentationClass(p);
    const QRect& rect = getRect(o);

    out.xml.startElement("draw:frame");
    QString style = graphicStyleNames.value(&o);
    if (!style.isEmpty()) {
        out.xml.addAttribute("draw:style-name",
                             getGraphicStyleName(o));
    } else {
        style = presentationStyleNames.value(&o);
        if (!style.isEmpty()) {
                out.xml.addAttribute("presentation:style-name",
                                     getGraphicStyleName(o));
        }
    }
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    if (classStr) {
        out.xml.addAttribute("presentation:class", classStr);
    }
    out.xml.startElement("draw:text-box");
    processTextForBody(tc, out);
    out.xml.endElement(); // draw:text-box
    out.xml.endElement(); // draw:frame
}

void PptToOdp::processObjectForBody(const OfficeArtSpgrContainerFileBlock& of, Writer& out)
{
    if (of.anon.is<OfficeArtSpgrContainer>()) {
        processObjectForBody(*of.anon.get<OfficeArtSpgrContainer>(), out);
    } else { // OfficeArtSpContainer
        processObjectForBody(*of.anon.get<OfficeArtSpContainer>(), out);
    }
}
void PptToOdp::processObjectForBody(const PPT::OfficeArtSpgrContainer& o, Writer& out)
{
    if (o.rgfb.size() < 2) return;
    out.xml.startElement("draw:g");
    /* if the first OfficeArtSpContainer has a clientAnchor,
       a new coordinate system is introduced.
       */
    const OfficeArtSpContainer* first
    = o.rgfb[0].anon.get<OfficeArtSpContainer>();
    if (first && first->clientAnchor && first->shapeGroup) {
        const QRect oldCoords = ::getRect(*first->clientAnchor);
        QRect newCoords = ::getRect(*first->shapeGroup);
        Writer transformedOut = out.transform(oldCoords, newCoords);
        for (int i = 1; i < o.rgfb.size(); ++i) {
            processObjectForBody(o.rgfb[i], transformedOut);
        }
    } else {
        for (int i = 1; i < o.rgfb.size(); ++i) {
            processObjectForBody(o.rgfb[i], out);
        }
    }
    out.xml.endElement(); // draw:g
}
void PptToOdp::processObjectForBody(const PPT::OfficeArtSpContainer& o, Writer& out)
{
    // text is process separately until drawing objects support it
    if (o.clientData && o.clientData->placeholderAtom && currentSlideTexts) {
        const PlaceholderAtom* p = o.clientData->placeholderAtom.data();
        if (p->position >= 0 && p->position < currentSlideTexts->atoms.size()) {
            const TextContainer& tc = currentSlideTexts->atoms[p->position];
            processTextObjectForBody(o, tc, out);
        } else {
            processDrawingObjectForBody(o, out);
        }
    } else if (o.clientTextbox) { // TODO
        foreach(const TextClientDataSubContainerOrAtom& tc, o.clientTextbox->rgChildRec) {
            if (tc.anon.is<TextContainer>()) {
                processTextObjectForBody(o, *tc.anon.get<TextContainer>(), out);
            }
        }
    } else if (o.shapeProp.rh.recInstance) { // why is this check needed?
        processDrawingObjectForBody(o, out);
    }
}

void PptToOdp::processSlideForBody(unsigned slideNo, KoXmlWriter& xmlWriter)
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

    xmlWriter.startElement("draw:page");
    QString value = masterNames.value(master);
    if (!value.isEmpty()) {
        xmlWriter.addAttribute("draw:master-page-name", value);
    }
    xmlWriter.addAttribute("draw:name", nameStr);
    value = drawingPageStyles[slide];
    if (!value.isEmpty()) {
        xmlWriter.addAttribute("draw:style-name", value);
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
    if (headerFooterAtom && headerFooterAtom->fHasDate) {
        xmlWriter.addAttribute("presentation:use-date-time-name", usedDateTimeDeclaration[slideNo]);
    }
    if (headerFooterAtom && headerFooterAtom->fHasHeader) {
        if(usedHeaderDeclaration[slideNo] != "")
            xmlWriter.addAttribute("presentation:use-header-name", usedHeaderDeclaration[slideNo]);
    }
    if (headerFooterAtom && headerFooterAtom->fHasFooter) {
        if(usedFooterDeclaration[slideNo] != "")
            xmlWriter.addAttribute("presentation:use-footer-name", usedFooterDeclaration[slideNo]);
    }

    currentSlideTexts = &p->documentContainer->slideList->rgChildRec[slideNo];

    Writer out(xmlWriter);
    foreach(const OfficeArtSpgrContainerFileBlock& co,
            slide->drawing.OfficeArtDg.groupShape.rgfb) {
        processObjectForBody(co, out);
    }
    if (slide->drawing.OfficeArtDg.shape) {
        // leave it out until it is understood
        //  processObjectForBody(*slide->drawing.OfficeArtDg.shape, out);
    }

    // draw the notes
    const NotesContainer* nc = p->notes[slideNo];
    if (nc) {
        currentSlideTexts = 0;
        xmlWriter.startElement("presentation:notes");
        value = drawingPageStyles[nc];
        if (!value.isEmpty()) {
            xmlWriter.addAttribute("draw:style-name", value);
        }
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                nc->drawing.OfficeArtDg.groupShape.rgfb) {
            processObjectForBody(co, out);
        }
        xmlWriter.endElement();
    }

    xmlWriter.endElement(); // draw:page
}

void PptToOdp::processSlideForStyle(int slideNo, KoGenStyles &styles)
{
    const SlideContainer* slide = p->slides[slideNo];
    processObjectForStyle(slide->drawing.OfficeArtDg.groupShape, styles, false);
    if (slide->drawing.OfficeArtDg.shape) {
        processObjectForStyle(*slide->drawing.OfficeArtDg.shape, styles, false);
    }
}
void PptToOdp::processObjectForStyle(const OfficeArtSpgrContainerFileBlock& of, KoGenStyles &styles, bool stylesxml)
{
    if (of.anon.is<OfficeArtSpgrContainer>()) {
        processObjectForStyle(*of.anon.get<OfficeArtSpgrContainer>(), styles, stylesxml);
    } else { // OfficeArtSpContainer
        processObjectForStyle(*of.anon.get<OfficeArtSpContainer>(), styles, stylesxml);
    }
}
void PptToOdp::processObjectForStyle(const PPT::OfficeArtSpgrContainer& o, KoGenStyles &styles, bool stylesxml)
{
    foreach(const OfficeArtSpgrContainerFileBlock& co, o.rgfb) {
        processObjectForStyle(co, styles, stylesxml);
    }
}
void PptToOdp::processObjectForStyle(const PPT::OfficeArtSpContainer& o, KoGenStyles &styles, bool stylesxml)
{
    // text is process separately until drawing objects support it
    if (o.clientTextbox) {
        foreach(const TextClientDataSubContainerOrAtom& tc, o.clientTextbox->rgChildRec) {
            if (tc.anon.is<TextContainer>()) {
                processTextObjectForStyle(o, *tc.anon.get<TextContainer>(), styles, stylesxml);
            }
        }
    }
    processDrawingObjectForStyle(o, styles, stylesxml);
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
    return QString("%1cm").arg(result);
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
QColor PptToOdp::toQColor(const OfficeArtCOLORREF& c)
{
    if (c.fSchemeIndex) {
        ColorStruct cs;
        // This should get the color from the color scheme of the current slide
        // or if slideContainer/slideAtom/slideFlags/fMasterScheme == true
        // from the slides masters color scheme.
        // TODO: use the current master
        const MasterOrSlideContainer* m = p->masters[0];
        if (m->anon.is<MainMasterContainer>()) {
            const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
            if (n->slideSchemeColorSchemeAtom.rgSchemeColor.size() <= c.red) {
                return QColor();
            }
            cs = n->slideSchemeColorSchemeAtom.rgSchemeColor[c.red];
        } else {
            const SlideContainer* n = m->anon.get<SlideContainer>();
            if (n->slideSchemeColorSchemeAtom.rgSchemeColor.size() <= c.red) {
                return QColor();
            }
            cs = n->slideSchemeColorSchemeAtom.rgSchemeColor.value(c.red);
        }
        return QColor(cs.red, cs.green, cs.blue);
    }
    return QColor(c.red, c.green, c.blue);
}

const TextMasterStyleLevel *
getTextMasterStyleLevel(quint16 type, quint16 level, const MasterOrSlideContainer* m)
{
    const TextMasterStyleAtom* masterStyle = 0;
    if (m->anon.is<MainMasterContainer>()) {
        const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
        if (n->rgTextMasterStyle.size() > type) {
            masterStyle = &n->rgTextMasterStyle[type];
        }
    } else {
        const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
        if (n->rgTextMasterStyle.size() > type) {
            masterStyle = &n->rgTextMasterStyle[type];
        }
    }
    if (!masterStyle) {
        return 0;
    }
    const TextMasterStyleLevel *l = 0;
    switch (level) {
    case 0: if (masterStyle->lstLvl1) l = masterStyle->lstLvl1.data();break;
    case 1: if (masterStyle->lstLvl2) l = masterStyle->lstLvl2.data();break;
    case 2: if (masterStyle->lstLvl3) l = masterStyle->lstLvl3.data();break;
    case 3: if (masterStyle->lstLvl4) l = masterStyle->lstLvl4.data();break;
    case 4: if (masterStyle->lstLvl5) l = masterStyle->lstLvl5.data();break;
    }
    return l;
}

const TextPFException *PptToOdp::masterTextPFException(quint16 type, quint16 level)
{
    // TODO look for the right master, not just the first one
    const MasterOrSlideContainer* m = p->masters[0];
    const TextMasterStyleLevel *l = getTextMasterStyleLevel(type, level, m);
    return (l) ? &l->pf : 0;
}

const TextCFException *PptToOdp::masterTextCFException(int type, unsigned int level)
{
    // TODO look for the right master, not just the first one
    const MasterOrSlideContainer* m = p->masters[0];
    const TextMasterStyleLevel *l = getTextMasterStyleLevel(type, level, m);
    return (l) ? &l->cf : 0;
}

void PptToOdp::processTextExceptionsForStyle(const TextCFRun *cf,
        const TextPFRun *pf,
        KoGenStyles &styles,
        const TextContainer& tc)
{
    qint16 indentLevel = 0;
    int indent = 0;
    if (pf) {
        indentLevel = pf->indentLevel;
        if (pf->pf.masks.indent) {
            indent = pf->pf.indent;
        }
    }

    qint16 textType = tc.textHeaderAtom.textType;

    /**
    TODO I had some ppt files where the text headers of slide body's text
    where defined as Tx_TYPE_CENTERBODY and title as Tx_TYPE_CENTERTITLE
    but their true style definitions (when compared to MS Powerpoint)
    where in Tx_TYPE_BODY and Tx_TYPE_TITLE TextMasterStyleAtoms.
    Either the text type is loaded incorrectly or there is some logic behind
    this that needs to be figured out.
    */
    if (textType == 5) {
        //Replace Tx_TYPE_CENTERBODY with Tx_TYPE_BODY
        textType = 1;
    } else if (textType == 6) {
        //and Tx_TYPE_CENTERTITLE with Tx_TYPE_TITLE
        textType = 0;
    }

    //Master character/paragraph styles are fetched for the textobjects type
    //using paragraph's indentation level
    const TextCFException *masterCF = masterTextCFException(textType,
                                      indentLevel);

    const TextPFException *masterPF = masterTextPFException(textType,
                                      indentLevel);

    //As mentioned in previous TODO we'll check if the text types
    //were placeholder types (2.13.33 TextTypeEnum in [MS-PPT].pdf
    //and use them for some styles
    const TextCFException *placeholderCF = 0;
    const TextPFException *placeholderPF = 0;

    if (textType == 5 || textType == 6) {
        placeholderPF = masterTextPFException(textType, indentLevel);
        placeholderCF = masterTextCFException(textType, indentLevel);
    }

    KoGenStyle styleParagraph(KoGenStyle::StyleAuto, "paragraph");
    if (pf && pf->pf.masks.leftMargin) {
        styleParagraph.addProperty("fo:margin-left",
                                   paraSpacingToCm(pf->pf.leftMargin),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.leftMargin) {
            styleParagraph.addProperty("fo:margin-left",
                                       paraSpacingToCm(masterPF->leftMargin),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf && pf->pf.masks.spaceBefore) {
        styleParagraph.addProperty("fo:margin-top",
                                   paraSpacingToCm(pf->pf.spaceBefore),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.spaceBefore) {
            styleParagraph.addProperty("fo:margin-top",
                                       paraSpacingToCm(masterPF->spaceBefore),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf && pf->pf.masks.spaceAfter) {
        styleParagraph.addProperty("fo:margin-bottom",
                                   paraSpacingToCm(pf->pf.spaceAfter),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.spaceAfter) {
            styleParagraph.addProperty("fo:margin-bottom",
                                       paraSpacingToCm(masterPF->spaceAfter),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf && pf->pf.masks.indent) {
        styleParagraph.addProperty("fo:text-indent",
                                   pptMasterUnitToCm(pf->pf.indent),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.indent) {
            styleParagraph.addProperty("fo:text-indent",
                                       pptMasterUnitToCm(masterPF->indent),
                                       KoGenStyle::ParagraphType);
        }
    }


    /**
    TODO When previous TODO about Tx_TYPE_CENTERBODY and Tx_TYPE_CENTERTITLE
    is fixed, correct the logic here. Here is added an extra if to use the
    placeholderPF which in turn contained the correct value.
    */
    if (pf && pf->pf.masks.align) {
        styleParagraph.addProperty("fo:text-align",
                                   textAlignmentToString(pf->pf.textAlignment),
                                   KoGenStyle::ParagraphType);
    } else if (placeholderPF && placeholderPF->masks.align) {
        styleParagraph.addProperty("fo:text-align",
                                   textAlignmentToString(placeholderPF->textAlignment),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.align) {
            styleParagraph.addProperty("fo:text-align",
                                       textAlignmentToString(masterPF->textAlignment),
                                       KoGenStyle::ParagraphType);
        }
    }

    //Text style
    KoGenStyle styleText(KoGenStyle::StyleTextAuto, "text");
    if (cf && cf->cf.color) {
        QColor color = toQColor(*cf->cf.color);
        if (color.isValid()) {
            styleText.addProperty("fo:color", color.name(),
                                  KoGenStyle::TextType);
        }
    } else {
        //Make sure that character formatting has color aswell
        if (masterCF && masterCF->color) {
            QColor color = toQColor(*masterCF->color);
            if (color.isValid()) {
                styleText.addProperty("fo:color", color.name(),
                                      KoGenStyle::TextType);
            }
        }
    }

    if (cf && cf->cf.masks.size) {
        styleText.addProperty("fo:font-size",
                              QString("%1pt").arg(cf->cf.fontSize),
                              KoGenStyle::TextType);
    } else {
        if (masterCF && masterCF->masks.size) {
            styleText.addProperty("fo:font-size",
                                  QString("%1pt").arg(masterCF->fontSize),
                                  KoGenStyle::TextType);
        }
    }

    if (cf && cf->cf.masks.italic && cf->cf.fontStyle) {
        if (cf->cf.fontStyle->italic) {
            styleText.addProperty("fo:font-style",
                                  "italic",
                                  KoGenStyle::TextType);
        }
    } else {
        if (masterCF && masterCF->masks.italic && masterCF->fontStyle
                && masterCF->fontStyle->italic) {
            styleText.addProperty("fo:font-style",
                                  "italic",
                                  KoGenStyle::TextType);
        }
    }

    if (cf && cf->cf.masks.bold && cf->cf.fontStyle) {
        if (cf->cf.fontStyle->bold) {
            styleText.addProperty("fo:font-weight",
                                  "bold",
                                  KoGenStyle::TextType);
        }
    } else {
        if (masterCF && masterCF->masks.bold && masterCF->fontStyle && masterCF->fontStyle->bold) {
            styleText.addProperty("fo:font-weight",
                                  "bold",
                                  KoGenStyle::TextType);
        }
    }

    const FontEntityAtom* font = 0;
    if (cf && cf->cf.masks.typeface) {
        font = getFont(cf->cf.fontRef);
    } else {
        if (masterCF && masterCF->masks.typeface) {
            font = getFont(masterCF->fontRef);
        }
    }

    if (font) {
        styleText.addProperty("fo:font-family",
                              QString::fromUtf16(font->lfFaceName.data(), font->lfFaceName.size()));
        font = 0;
    }

    if (cf && cf->cf.masks.position) {
        styleText.addProperty("style:text-position",
                              QString("%1%").arg(cf->cf.position),
                              KoGenStyle::TextType);

    } else {
        if (masterCF && masterCF->masks.position) {
            styleText.addProperty("style:text-position",
                                  QString("%1%").arg(masterCF->position),
                                  KoGenStyle::TextType);
        }
    }

    bool underline = false;
    if (cf && cf->cf.masks.underline) {
        underline = cf->cf.fontStyle->underline;
    } else {
        if (masterCF && masterCF->masks.underline) {
            underline = masterCF->fontStyle->underline;
        }
    }

    if (underline) {
        styleText.addProperty("style:text-underline-style",
                              "solid",
                              KoGenStyle::TextType);

        styleText.addProperty("style:text-underline-width",
                              "auto",
                              KoGenStyle::TextType);

        styleText.addProperty("style:text-underline-color",
                              "font-color",
                              KoGenStyle::TextType);
    }

    bool emboss = false;
    if (cf && cf->cf.masks.emboss) {
        emboss = cf->cf.fontStyle->emboss;
    } else {
        if (masterCF && masterCF->masks.emboss) {
            emboss = masterCF->fontStyle->emboss;
        }
    }

    if (emboss) {
        styleText.addProperty("style:font-relief",
                              "embossed",
                              KoGenStyle::TextType);
    }

    bool shadow = false;
    if (cf && cf->cf.masks.shadow) {
        shadow = cf->cf.fontStyle->shadow;
    } else {
        if (masterCF && masterCF->masks.shadow) {
            shadow = masterCF->fontStyle->shadow;
        }
    }

    if (shadow) {
        styleText.addProperty("fo:text-shadow", 0, KoGenStyle::TextType);
    }

    KoGenStyle styleList(KoGenStyle::StyleListAuto, 0);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);    // TODO pass indentation level

    const TextPFException9 *pf9 = 0;
    const StyleTextProp9 *prop9 = 0;

    if (cf) {
        // TODO how does one find this structure?
        prop9 = 0;//textObject->findStyleTextProp9(cf->textCFException());
    }

    for (int i = 0;i < indent + 1;i++) {
        //TextCFException *levelCF = masterTextCFException(type, i);
        const TextPFException *levelPF = masterTextPFException(textType, i);

        if (prop9) {
            pf9 = &prop9->pf9;
        }

        bool isListLevelStyleNumber =
            (levelPF && levelPF->masks.bulletFont)
            || (pf9 && pf9->masks.bulletHasScheme);
        if (isListLevelStyleNumber) {
            elementWriter.startElement("text:list-level-style-number");
        } else {
            elementWriter.startElement("text:list-level-style-bullet");
            // every text:list-level-style-bullet must have a text:bullet-char
            const char bullet[4] = {0xe2, 0x97, 0x8f, 0};
            QString bulletChar(QString::fromUtf8(bullet)); //  "●";
            if (pf && pf->pf.masks.indent && i == pf->pf.indent &&
                    pf->pf.masks.bulletChar) {
                bulletChar = pf->pf.bulletChar;
            } else if (!isListLevelStyleNumber
                       && levelPF && levelPF->masks.bulletChar) {
                bulletChar = levelPF->bulletChar;
            } else {
                QString::fromUtf8(bullet);//  "●";
            }
            elementWriter.addAttribute("text:bullet-char", bulletChar);
        }

        elementWriter.addAttribute("text:level", i + 1);

        if (pf9 && pf9->masks.bulletHasScheme) {
            QString numFormat = 0;
            QString numSuffix = 0;
            QString numPrefix = 0;
            processTextAutoNumberScheme(pf9->bulletAutoNumberScheme->scheme, numFormat, numSuffix,
                                        numPrefix);

            if (numPrefix != 0) {
                elementWriter.addAttribute("style:num-prefix", numPrefix);
            }

            if (numSuffix != 0) {
                elementWriter.addAttribute("style:num-suffix", numSuffix);
            }

            elementWriter.addAttribute("style:num-format", numFormat);
        }

        elementWriter.startElement("style:list-level-properties");

        bool hasIndent = pf && pf->pf.masks.indent && i == pf->pf.indent;
        if (hasIndent && pf->pf.masks.spaceBefore) {
            elementWriter.addAttribute("text:space-before",
                                       paraSpacingToCm(pf->pf.spaceBefore));
        } else if (levelPF && levelPF->masks.spaceBefore) {
            elementWriter.addAttribute("text:space-before",
                                       paraSpacingToCm(levelPF->spaceBefore));
        }
        elementWriter.endElement(); // style:list-level-properties

        elementWriter.startElement("style:text-properties");

        if (hasIndent && pf->pf.masks.bulletFont) {
            font = getFont(pf->pf.bulletFontRef);
        } else if (levelPF && levelPF->masks.bulletFont) {
            font = getFont(levelPF->bulletFontRef);
        }

        if (font) {
            elementWriter.addAttribute("fo:font-family",
                                       QString::fromUtf16(font->lfFaceName.data(),
                                                          font->lfFaceName.size()));
        }

        if (hasIndent && pf->pf.masks.bulletColor) {
            elementWriter.addAttribute("fo:color",
                                       toQColor(*pf->pf.bulletColor).name());
        } else {
            if (levelPF && levelPF->masks.bulletColor) {
                elementWriter.addAttribute("fo:color",
                                           toQColor(*levelPF->bulletColor).name());
            }
        }

        if (hasIndent && pf->pf.masks.bulletSize) {
            elementWriter.addAttribute("fo:font-size",
                                       QString("%1%").arg(pf->pf.bulletSize));
        } else {
            if (levelPF && levelPF->masks.bulletSize) {
                elementWriter.addAttribute("fo:font-size",
                                           QString("%1%").arg(levelPF->bulletSize));
            } else {
                elementWriter.addAttribute("fo:font-size", "100%");
            }
        }

        elementWriter.endElement(); // style:text-properties

        elementWriter.endElement();  // text:list-level-style-bullet


        styleList.addChildElement("text:list-level-style-bullet",
                                  QString::fromUtf8(buffer.buffer(),
                                                    buffer.buffer().size()));
    }

    addStyleNames((cf) ? &cf->cf : 0, (pf) ? &pf->pf : 0,
                  styles.lookup(styleText),
                  styles.lookup(styleParagraph),
                  styles.lookup(styleList));
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

void PptToOdp::processTextObjectForStyle(const PPT::OfficeArtSpContainer& o,
        const PPT::TextContainer& tc,
        KoGenStyles &styles, bool master)
{
    // whether a text frame should have style belonging to graphic or
    // presentation family depends on whether it is a placeholder
    // if it is a placeholder, they style is inherited from the master place-
    // holder

    if (p) {
        if (master) {
            // store then name for the master
        }
    }
    if (!master && p) {
        // find the master placeholder

    }

    const StyleTextPropAtom* style  = tc.style.data();
    if (!style) {
        processTextExceptionsForStyle(0, 0, styles, tc);
        return;
    }

    //What paragraph/character exceptions were used last
    const TextPFRun *pf = 0;
    const TextCFRun *cf = 0;

    //TODO this can be easily optimized by calculating proper increments to i
    //from both exception's character count
    QString text = getText(tc);
    for (int i = 0;i < text.length(); i++) {
        if (cf == findTextCFRun(*style, i) && pf == findTextPFRun(*style, i) && i > 0) {
            continue;
        }

        pf = findTextPFRun(*style, i);
        cf = findTextCFRun(*style, i);

        processTextExceptionsForStyle(cf, pf, styles, tc);
    }

    // set the presentation style
    QString styleName;
    KoGenStyle kostyle(KoGenStyle::StyleGraphicAuto, "presentation");
    kostyle.setAutoStyleInStylesDotXml(master);
    defineGraphicProperties(kostyle, o);
    styleName = styles.lookup(kostyle);
    setGraphicStyleName(o, styleName);
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
}
template <typename T>
void PptToOdp::defineGraphicProperties(KoGenStyle& style, T& o, const TextMasterStyleAtom* listStyles)
{
    // TODO: change this function so it goes through the attributes
    // of ODF style:graphic-properties instead of going through
    // the properties found in ppt


    /** 2.3.8 Line Style **/
    // 2.3.8.1 lineColor
    const LineColor* lc = get<LineColor>(o);
    if (lc) {
        style.addProperty("svg:stroke-color", toQColor(lc->lineColor).name(),
                          KoGenStyle::GraphicType);
    }
    // 2.3.8.2 lineOpacity
    const LineOpacity* lo = get<LineOpacity>(o);
    if (lo) {
        style.addProperty("svg:stroke-opacity", lo->lineOpacity / 0x10000f);
    }
    // 2.3.8.5 lineType
    // solid, pattern or texture
    // 2.3.8.14 lineWidth
    const LineWidth* lw = get<LineWidth>(o);
    if (lw) {
        style.addProperty("svg:stroke-width",
                          QString("%1pt").arg(lw->lineWidth / 12700.f),
                          KoGenStyle::GraphicType);
    }
    // 2.3.8.16 lineStyle
    // single, double, tiple etc line
    // 2.3.8.17 lineDashing
    const LineDashing* ld = get<LineDashing>(o);

    // This is not nearly complete. left, right, top and bottom lines not yes
    // supported.
    // for now, go by the assumption that there is only a line
    // when fUsefLine and fLine are true
    const LineStyleBooleanProperties* bp = get<LineStyleBooleanProperties>(o);
    if (bp && bp->fUsefLine && bp->fLine) {
        if (bp->fNoLineDrawDash) {
            style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
        } else if (ld) {
            if (ld->lineDashing == 0 || ld->lineDashing >= 11) { // solid
                style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
            } else {
                style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
                style.addProperty("draw:stroke-dash", dashses[ld->lineDashing],
                                  KoGenStyle::GraphicType);
            }
        } else {
            // default style is a solid line, this must be set explicitly as long
            // as kpresenter takes draw:stroke="none" as default
            style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
        }
    }
    style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);


    const LineStartArrowhead* lsa = get<LineStartArrowhead>(o);
    if (lsa && lsa->lineStartArrowhead > 0 && lsa->lineStartArrowhead < 6) {
        style.addProperty("draw:marker-start",
                          arrowHeads[lsa->lineStartArrowhead], KoGenStyle::GraphicType);
    }

    const LineEndArrowhead* lea = get<LineEndArrowhead>(o);
    if (lea && lea->lineEndArrowhead > 0 && lea->lineEndArrowhead < 6) {
        style.addProperty("draw:marker-end",
                          arrowHeads[lea->lineEndArrowhead], KoGenStyle::GraphicType);
    }

    const LineStartArrowWidth* lsw = get<LineStartArrowWidth>(o);
    if (lw && lsw) {
        style.addProperty("draw:marker-start-width", QString("%1cm").arg(
                              lw->lineWidth*lsw->lineStartArrowWidth), KoGenStyle::GraphicType);
    }

    const LineEndArrowWidth* lew = get<LineEndArrowWidth>(o);
    if (lw && lew) {
        style.addProperty("draw:marker-end-width", QString("%1cm").arg(lw->lineWidth*lew->lineEndArrowWidth), KoGenStyle::GraphicType);
    }

    const FillBlip* fb = get<FillBlip>(o);
    const FillStyleBooleanProperties* fs = get<FillStyleBooleanProperties>(o);
    if (fb) {
        style.addProperty("draw:fill", "bitmap");
        style.addProperty("draw:fill-image-name",
                          "fillImage" + QString::number(fb->fillBlip));
    } else if (fs && fs->fUseFilled) {
        style.addProperty("draw:fill", fs->fFilled ? "solid" : "none",
                          KoGenStyle::GraphicType);
    }
    const FillColor* fc = get<FillColor>(o);
    if (fc) {
        style.addProperty("draw:fill-color", toQColor(fc->fillColor).name(),
                          KoGenStyle::GraphicType);
    } else {
        style.addProperty("draw:fill-color", "#99ccff",
                          KoGenStyle::GraphicType);
    }

#if 0
    if (drawObject->hasProperty("draw:shadow-color")) {
        elementWriter.addAttribute("draw:shadow", "visible");
        Color shadowColor = drawObject->getColorProperty("draw:shadow-color");
        style.addProperty("draw:shadow-color", hexname(shadowColor), KoGenStyle::GraphicType);
    } else {
        style.addProperty("draw:shadow", "hidden", KoGenStyle::GraphicType);
    }
#endif

    const ShadowOpacity* so = get<ShadowOpacity>(o);
    if (so) {
        float opacity = so->shadowOpacity.integral + so->shadowOpacity.fractional / 65535.0;
        style.addProperty("draw:shadow-opacity", QString("%1%").arg(opacity), KoGenStyle::GraphicType);
    }

    const ShadowOffsetX* sox =  get<ShadowOffsetX>(o);
    if (sox) {
        style.addProperty("draw:shadow-offset-x", QString("%1cm").arg(sox->shadowOffsetX), KoGenStyle::GraphicType);
    }

    const ShadowOffsetY* soy =  get<ShadowOffsetY>(o);
    if (soy) {
        style.addProperty("draw:shadow-offset-y", QString("%1cm").arg(soy->shadowOffsetY), KoGenStyle::GraphicType);
    }

    // define embedded text:list-style element
    if (listStyles) {
        defineGraphicPropertiesListStyles(style, *listStyles);
    }
}
void PptToOdp::defineGraphicPropertiesListStyles(KoGenStyle& style, const TextMasterStyleAtom& listStyles)
{
    KoGenStyle list(KoGenStyle::StyleList);
    defineListStyle(list, listStyles);
    KoGenStyles styles;
    // serialize the text:list-style element into the properties
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);
    list.writeStyle(&xmlWriter, styles, "text:list-style", "someName", "", true, false);
    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    style.addChildElement("text:list-style", elementContents);
}
void PptToOdp::processDrawingObjectForStyle(const PPT::OfficeArtSpContainer& o, KoGenStyles &styles, bool stylesxml)
{
    KoGenStyle style(KoGenStyle::StyleGraphicAuto, "graphic");
    style.setAutoStyleInStylesDotXml(stylesxml);
    style.setParentName("pptDefaults"); // TODO find proper parent name
    defineGraphicProperties(style, o);
    setGraphicStyleName(o, styles.lookup(style));
}

void PptToOdp::processDeclaration(KoXmlWriter* xmlWriter)
{
    const HeadersFootersAtom* headerFooterAtom = 0;
    QSharedPointer<UserDateAtom> userDateAtom;
    QSharedPointer<FooterAtom> footerAtom;
    HeaderAtom* headerAtom = 0;
    const PPT::SlideHeadersFootersContainer* slideHF = getSlideHF();

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



