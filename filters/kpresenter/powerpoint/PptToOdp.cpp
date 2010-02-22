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
    QString mm(double v) {
        static const QString mm("%1mm");
        return mm.arg(v, 0, 'f');
    }
    QString cm(double v) {
        static const QString cm("%1cm");
        return cm.arg(v, 0, 'f');
    }
    QString pt(double v) {
        static const QString pt("%1pt");
        return pt.arg(v, 0, 'f');
    }
    QString percent(double v) {
        static const QString percent("%1%");
        return percent.arg(v, 0, 'f');
    }

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
PptToOdp::Writer::Writer(KoXmlWriter& xmlWriter, KoGenStyles& kostyles,
                         bool stylesxml_)
      : xOffset(0),
        yOffset(0),
        scaleX(25.4 / 576),
        scaleY(25.4 / 576),
        xml(xmlWriter),
        styles(kostyles),
        stylesxml(stylesxml_)
{
}

PptToOdp::Writer
PptToOdp::Writer::transform(const QRectF& oldCoords, const QRectF &newCoords) const
{
    Writer w(xml, styles, stylesxml);
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
    return mm(length*scaleY);
}

QString PptToOdp::Writer::hLength(qreal length)
{
    return mm(length*scaleX);
}

QString PptToOdp::Writer::vOffset(qreal offset)
{
    return mm(yOffset + offset*scaleY);
}

QString PptToOdp::Writer::hOffset(qreal offset)
{
    return mm(xOffset + offset*scaleX);
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

namespace
{
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
    QString pageWidth = mm(sizeX);
    QString pageHeight = mm(sizeY);

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
const char* dashses[11] = {
    "", "Dash_20_2", "Dash_20_3", "Dash_20_2", "Dash_20_2", "Dash_20_2",
    "Dash_20_4", "Dash_20_6", "Dash_20_5", "Dash_20_7", "Dash_20_8"
};
const char* arrowHeads[6] = {
    "", "msArrowEnd_20_5", "msArrowStealthEnd_20_5", "msArrowDiamondEnd_20_5",
    "msArrowOvalEnd_20_5", "msArrowOpenEnd_20_5"
};
const char* getFillType(quint32 fillType)
{
    switch (fillType) {
    case 2: // msofillTexture
    case 3: // msofillPicture
        return "bitmap";
    case 4: // msofillShade
    case 5: // msofillShadeCenter
    case 6: // msofillShadeShape
    case 7: // msofillShadeScale
    case 8: // msofillShadeTitle
        return "gradient";
    case 1: // msofillPattern
        return "hatch";
    case 9: // msofillBackground
        return "none";
    case 0: // msofillSolid
    default:
        return "solid";
    }
}
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
    const KoGenStyle::PropertyType dpt = KoGenStyle::DrawingPageType;
    style.addProperty("draw:background-size", "border", dpt);
    style.addProperty("draw:fill", "none", dpt);
    style.addProperty("style:repeat", "stretch", dpt);
    style.setDefaultStyle(true);
    const OfficeArtDggContainer* drawingGroup = 0;
    if (p->documentContainer) {
        drawingGroup = &p->documentContainer->drawingGroup.OfficeArtDgg;
    }
    const PPT::SlideHeadersFootersContainer* hf = getSlideHF();
    defineDrawingPageStyle(style, drawingGroup, (hf) ?&hf->hfAtom :0);
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
    const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
    style.addProperty("svg:stroke-width", "0pt", gt);
    style.addProperty("draw:auto-grow-height", false, gt);
    style.addProperty("draw:stroke", "none", gt);
    style.addProperty("draw:fill", "none", gt);
    style.addProperty("draw:fill-color", "#ffffff", gt);
    const OfficeArtDggContainer& drawingGroup
        = p->documentContainer->drawingGroup.OfficeArtDgg;
//    const TextMasterStyleAtom& textMasterStyle
//        = p->documentContainer->documentTextInfo.textMasterStyleAtom;
    defineGraphicProperties(style, drawingGroup);
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
                                         const TextPFException9* /*pf9*/) {
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

template <typename T>
void PptToOdp::defineDrawingPageStyle(KoGenStyle& style, const T* o,
                                      const HeadersFootersAtom* hf)
{
    const KoGenStyle::PropertyType dp = KoGenStyle::DrawingPageType;
    // draw:background-size ("border", or "full")
    const FillStyleBooleanProperties* fs
            = get<FillStyleBooleanProperties>(o);
    if (fs) {
        style.addProperty("draw:background-size",
                          (fs->fUseFillUseRext && fs->fillUseRect)
                          ?"border" :"full", dp);
    }
    // draw:fill ("bitmap", "gradient", "hatch", "none" or "solid")
    const FillType* fillType = get<FillType>(o);
    if (fs && fs->fUseFilled && !fs->fFilled) {
        style.addProperty("draw:fill", "none", dp);
    } else if (fillType) {
        style.addProperty("draw:fill", getFillType(fillType->fillType), dp);
    }
    // draw:fill-color
    const FillColor* fc = get<FillColor>(o);
    if (fc && fillType && fillType->fillType == 0) {
        // only set the color if the fill type is 'solid' because OOo ignores
        // fill='non' if the color is set
        style.addProperty("draw:fill-color", toQColor(fc->fillColor).name(), dp);
    }
    // draw:fill-gradient-name
    // draw:fill-hatch-name
    // draw:fill-hatch-solid
    // draw:fill-image-height
    // draw:fill-image-name
    const FillBlip* fb = get<FillBlip>(o);
    const QString fillImagePath = (fb) ?getPicturePath(fb->fillBlip) :"";
    if (!fillImagePath.isEmpty()) {
        style.addProperty("draw:fill-image-name",
                          "fillImage" + QString::number(fb->fillBlip), dp);
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
    // presentation:background-objects-visible
    // presentation:background-visible
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
        defineListStyle(style, 4, *levels.lstLvl4,
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
    const TextPFException9* pf9 = (level9) ?&level9->pf9 :0;
    const TextCFException9* cf9 = (level9) ?&level9->cf9 :0;
    const TextCFException10* cf10 = (level10) ?&level10->cf10 :0;
    defineListStyle(style, depth, level.pf, pf9, &level.cf, cf9, cf10);
}

QChar
getBulletChar(const TextPFException& pf) {
    if (pf.masks.bulletChar) {
        quint16 v = (quint16)pf.bulletChar;
        if (v == 0xf06c) { // 0xF06C from windings is similar to ●
            return QChar(0x25cf); //  "●"
        }
        if (v == 0xf02d) { // 0xF02D from symbol is similar to –
            return QChar(0x2013);
        }
        return QChar(pf.bulletChar);
    }
    return QChar(0x25cf); //  "●"
}

void PptToOdp::defineListStyle(KoGenStyle& style, quint8 depth,
                               const TextPFException& pf,
                               const TextPFException9* pf9,
                               const TextCFException* cf,
                               const TextCFException9* cf9,
                               const TextCFException10* cf10)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter out(&buffer);

    QString elementName;
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
                QChar bulletChar = getBulletChar(pf);
                out.addAttribute("text:bullet-char", bulletChar);
            } else {
                // the character from the parent should be specified here,
                // because text:list-level-style-bullet must have a bullet
                out.addAttribute("text:bullet-char", QChar(0x25cf)); //  "●"
            }
            if (pf.masks.bulletSize && pf.bulletFlags->fBulletHasSize) {
                if (pf.bulletSize >= 25 && pf.bulletSize <= 400) {
                    out.addAttribute("text:bullet-relative-size",
                                     QString("%1%").arg(pf.bulletSize));
                } else if (pf.bulletSize >= -4000 && pf.bulletSize <= -1) {
                    out.addAttribute("text:bullet-relative-size",
                                     percent(pf.bulletSize));
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
    defineTextProperties(ls, cf, cf9, cf10, 0);

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
    quint32 wanted;
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
const TextMasterStyleAtom*
getTextMasterStyleAtom(const MasterOrSlideContainer* m, quint16 texttype)
{
    const TextMasterStyleAtom* textstyle = 0;
    if (!m) return 0;
    const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
    if (!mm) return 0;
    foreach (const TextMasterStyleAtom&ma, mm->rgTextMasterStyle) {
        if (ma.rh.recInstance == texttype) {
            textstyle = &ma;
        }
    }
    return textstyle;
}
void PptToOdp::defineMasterStyles(KoGenStyles& styles)
{
    foreach (const PPT::MasterOrSlideContainer* m, p->masters) {
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
                addGraphicStyleToDrawElement(w, *finder.sp);
            }
        }
        // if no style for Tx_TYPE_CENTERTITLE (6) has been defined yet,
        // derive it from Tx_TYPE_TITLE (0)
        if (!masterPresentationStyles[m].contains(6)
                && masterPresentationStyles[m].contains(0)) {
            KoGenStyle style(KoGenStyle::StylePresentation, "presentation");
            style.setParentName(masterPresentationStyles[m][0]);
            style.addProperty("fo:text-align", "center",
                              KoGenStyle::ParagraphType);
            style.addProperty("style:vertical-align", "middle",
                              KoGenStyle::ParagraphType);
            masterPresentationStyles[m][6] = styles.lookup(style, "");
        }
        // if no style for Tx_TYPE_CENTERBODY (5) has been defined yet,
        // derive it from Tx_TYPE_BODY (1)
        if (!masterPresentationStyles[m].contains(5)
                && masterPresentationStyles[m].contains(1)) {
            KoGenStyle style(KoGenStyle::StylePresentation, "presentation");
            style.setParentName(masterPresentationStyles[m][1]);
            style.addProperty("fo:text-align", "center",
                              KoGenStyle::ParagraphType);
            style.addProperty("style:vertical-align", "middle",
                              KoGenStyle::ParagraphType);
            masterPresentationStyles[m][5] = styles.lookup(style, "");
        }
    }
}
void PptToOdp::defineAutomaticDrawingPageStyles(KoGenStyles& styles)
{
    // define for master for use in <master-page style:name="...">
    foreach (const PPT::MasterOrSlideContainer* m, p->masters) {
        KoGenStyle dp(KoGenStyle::StyleDrawingPageAuto, "drawing-page");
        dp.setAutoStyleInStylesDotXml(true);
        const SlideContainer* sc = m->anon.get<SlideContainer>();
        const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
        const HeadersFootersAtom* hf = 0;
        const OfficeArtSpContainer* scp = 0;
        if (sc) {
            if (sc->perSlideHFContainer) {
                hf = &sc->perSlideHFContainer->hfAtom;
            }
            if (sc->drawing.OfficeArtDg.shape) {
                scp = sc->drawing.OfficeArtDg.shape.data();
            }
        } else if (mm) {
            if (mm->perSlideHeadersFootersContainer) {
                hf = &mm->perSlideHeadersFootersContainer->hfAtom;
            }
            if (mm->drawing.OfficeArtDg.shape) {
                scp = mm->drawing.OfficeArtDg.shape.data();
            }
        }
        defineDrawingPageStyle(dp, scp, hf);
        drawingPageStyles[m] = styles.lookup(dp, "Mdp");
    }
    QString notesMasterPageStyle;
    if (p->notesMaster) {
        const HeadersFootersAtom* hf = 0;
        if (p->notesMaster->perSlideHFContainer) {
            hf = &p->notesMaster->perSlideHFContainer->hfAtom;
        } else if (p->notesMaster->perSlideHFContainer2) {
            hf = &p->notesMaster->perSlideHFContainer2->hfAtom;
        }
        KoGenStyle dp(KoGenStyle::StyleDrawingPageAuto, "drawing-page");
        dp.setAutoStyleInStylesDotXml(true);
        defineDrawingPageStyle(dp,
                p->notesMaster->drawing.OfficeArtDg.shape.data(), hf);
        notesMasterPageStyle = styles.lookup(dp, "Mdp");
        drawingPageStyles[p->notesMaster] = notesMasterPageStyle;
    }

    // define for handouts for use in <style:handout-master style:name="...">
    // TODO

    // define for slides for use in <draw:page style:name="...">
    foreach (const PPT::SlideContainer* sc, p->slides) {
        KoGenStyle dp(KoGenStyle::StyleDrawingPageAuto, "drawing-page");
        dp.setAutoStyleInStylesDotXml(false);
        // TODO derive from master page style
        const HeadersFootersAtom* hf = 0;
        if (sc->perSlideHFContainer) {
            hf = &sc->perSlideHFContainer->hfAtom;
        }
        defineDrawingPageStyle(dp, sc->drawing.OfficeArtDg.shape.data(),  hf);
        drawingPageStyles[sc] = styles.lookup(dp, "dp");
    }

    // define for notes for use in <presentation:notes style:name="...">
    foreach (const PPT::NotesContainer* nc, p->notes) {
        if (!nc) continue;
        const HeadersFootersAtom* hf = 0;
        if (nc->perSlideHFContainer) {
            hf = &nc->perSlideHFContainer->hfAtom;
        } else if (nc->perSlideHFContainer2) {
            hf = &nc->perSlideHFContainer2->hfAtom;
        }
        KoGenStyle dp(KoGenStyle::StyleDrawingPageAuto, "drawing-page");
        dp.setAutoStyleInStylesDotXml(false);
        dp.setParentName(notesMasterPageStyle);
        defineDrawingPageStyle(dp, nc->drawing.OfficeArtDg.shape.data(), hf);
        drawingPageStyles[nc] = styles.lookup(dp, "dp");
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
    currentSlideTexts = 0;
    defineMasterStyles(styles);
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
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                p->notesMaster->drawing.OfficeArtDg.groupShape.rgfb) {
            processObjectForBody(co, out);
        }
        writer.endElement();
    }
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
        currentMaster = m;
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&buffer);
        Writer out(writer, styles, true);
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                drawing->OfficeArtDg.groupShape.rgfb) {
            processObjectForBody(co, out);
        }
        master.addChildElement("", QString::fromUtf8(buffer.buffer(),
                                                     buffer.buffer().size()));
        if (notesBuffer.buffer().size()) {
            master.addChildElement("presentation:notes",
                                   QString::fromUtf8(notesBuffer.buffer(),
                                                     notesBuffer.buffer().size()));
        }
        masterNames[m] = styles.lookup(master, "M");
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
    styles.saveOdfAutomaticStyles(&contentWriter, false);

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

int PptToOdp::processTextSpan(const PPT::TextContainer& tc, Writer& out,
                              const QString& text, const int start,
                              int end)
{
    // find all components that start at position start

    // get the right character run
    const QList<TextCFRun> &cfs = tc.style->rgTextCFRun;
    int i = 0;
    int cfend = 0;
    while (i < cfs.size()) {
        cfend += cfs[i].count;
        if (cfend > start) {
            break;
        }
        i++;
    }
    if (i >= cfs.size()) {
        qDebug() << "No proper character range for '"
                << QString(text).replace('\r', '\n') << "' start: " << start
                << " cfs.size() " << cfs.size() << " " << ((cfs.size()) ?cfs[cfs.size()-1].count :-1);
        return -1;
    }
    const TextCFException& cf = cfs[i].cf;

    // get the right special info run
    const QList<TextSIRun>* tsi = 0;
    if (tc.specialinfo) tsi = &tc.specialinfo->rgSIRun;
    if (tc.specialinfo2) tsi = &tc.specialinfo2->rgSIRun;
    const TextSIException* si = 0;
    int siend = 0;
    i = 0;
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
        const SlideNumberMCAtom* a = m.meta.get<SlideNumberMCAtom>();
        const DateTimeMCAtom* b = m.meta.get<DateTimeMCAtom>();
        const GenericDateMCAtom* c = m.meta.get<GenericDateMCAtom>();
        const HeaderMCAtom* d = m.meta.get<HeaderMCAtom>();
        const FooterMCAtom* e = m.meta.get<FooterMCAtom>();
        const RTFDateTimeMCAtom* f = m.meta.get<RTFDateTimeMCAtom>();
        if (a && a->position == start) meta = &m;
        if (b && b->position == start) meta = &m;
        if (c && c->position == start) meta = &m;
        if (d && d->position == start) meta = &m;
        if (e && e->position == start) meta = &m;
        if (f && f->position == start) meta = &m;
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
    }
    KoGenStyle style(KoGenStyle::StyleTextAuto, "text");
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    defineTextProperties(style, &cf, 0, 0, si);
    out.xml.addAttribute("text:style-name", out.styles.lookup(style, ""));

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

int PptToOdp::processTextSpans(const PPT::TextContainer& tc, Writer& out,
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

void PptToOdp::processTextLine(Writer& out, const PPT::TextContainer& tc,
                              const QString& text, int start, int end,
                              QStack<QString>& levels)
{
    // find the textpfexception that belongs to this line
    const TextPFRun* pf = 0;
    if (tc.style) {
        const QList<TextPFRun> &pfs = tc.style->rgTextPFRun;
        int i = 0;
        int pfend = 0;
        while (i < pfs.size()) {
            pfend += pfs[i].count;
            if (pfend > start) {
                pf = &pfs[i];
                break;
            }
            i++;
        }
    }
    if (pf == 0) {
        // perhaps this should be a list under some circumstances
        if (levels.size()) { // context is a list
            out.xml.startElement("text:list-item");
        } else {
            out.xml.startElement("text:p");
        }
        out.xml.addTextNode(text.mid(start, end-start));
        out.xml.endElement();
        return;
    }

    quint16 paragraphIndent = pf->indentLevel;
    // [MS-PPT].pdf says the indentation level can be 4 at most
    if (paragraphIndent > 4) paragraphIndent = 4;
    bool hasBullet = pf->pf.masks.hasBullet && pf->pf.bulletFlags->fHasBullet;
    bool islist = hasBullet || paragraphIndent > 0;
    QString listStyle;
    if (islist) {
        KoGenStyle list(KoGenStyle::StyleListAuto);
        defineListStyle(list, 1, pf->pf, 0, 0, 0, 0);
        listStyle = out.styles.lookup(list, "");
    }
    // remove levels until the top level is the right indentation
    if ((quint16)levels.size() > paragraphIndent
            && levels[paragraphIndent] == listStyle) {
        writeTextObjectDeIndent(out.xml, paragraphIndent + 2, levels);
    } else {
        writeTextObjectDeIndent(out.xml, paragraphIndent + 1, levels);
    }
    // add styleless levels up to the current level of indentation
    while ((quint16)levels.size() < paragraphIndent) {
        addListElement(out.xml, levels, "");
    }
    // at this point, levels.size() == paragraphIndent
    if (paragraphIndent + 1 != levels.size()) {
        addListElement(out.xml, levels, listStyle);
    }
    bool listItem = levels.size() > 1 || !levels.top().isNull();
    if (listItem) {
            out.xml.startElement("text:list-item");
    }
    out.xml.startElement("text:p");
    KoGenStyle style(KoGenStyle::StyleAuto, "paragraph");
    style.setAutoStyleInStylesDotXml(out.stylesxml);
    defineParagraphProperties(style, &pf->pf, 0);
    out.xml.addAttribute("text:style-name", out.styles.lookup(style, ""));
    processTextSpans(tc, out, text, start, end);
    out.xml.endElement(); // text:p
    if (listItem) {
        out.xml.endElement(); // text:list-item
    }
}

void PptToOdp::processTextForBody(const PPT::TextContainer& tc, Writer& out)
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

        processTextLine(out, tc, text, pos, end, levels);

        pos = end + 1;
    }
    // close all open text:list elements
    writeTextObjectDeIndent(out.xml, 0, levels);
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
    addGraphicStyleToDrawElement(out, o);
    if (p) {
        if (p->placementId >= 1 && p->placementId <= 6) {
            out.xml.addAttribute("presentation:placeholder", "true");
        } else if (p->placementId >= 0xB) {
            out.xml.addAttribute("presentation:user-transformed", "true");
        }
    }
    //out.xml.addAttribute("draw:layer", "layout");
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
        out.xml.startElement("presentation:notes");
        value = drawingPageStyles[nc];
        if (!value.isEmpty()) {
            out.xml.addAttribute("draw:style-name", value);
        }
        foreach(const OfficeArtSpgrContainerFileBlock& co,
                nc->drawing.OfficeArtDg.groupShape.rgfb) {
            processObjectForBody(co, out);
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
    const TextMasterStyleAtom* masterStyle
            = getTextMasterStyleAtom(m, type);
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
template <typename T>
void PptToOdp::defineGraphicProperties(KoGenStyle& style, const T& o,
                                       const QString& listStyle)
{
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
    const FillStyleBooleanProperties* fs = get<FillStyleBooleanProperties>(o);
    const FillType* fillType = get<FillType>(o);
    if (fs && fs->fUseFilled && !fs->fFilled) {
        style.addProperty("draw:fill", "none", gt);
    } else if (fillType) {
        style.addProperty("draw:fill", getFillType(fillType->fillType), gt);
    }
    // draw:fill-color
    const FillColor* fc = get<FillColor>(o);
    if (fc && fillType && fillType->fillType == 0) {
        // only set the color if the fill type is 'solid' because OOo ignores
        // fill='non' if the color is set
        style.addProperty("draw:fill-color", toQColor(fc->fillColor).name(), gt);
    }
    // draw:fill-gradient-name
    // draw:fill-hatch-name
    // draw:fill-hatch-solid
    // draw:fill-image-height
    // draw:fill-image-name
    const FillBlip* fb = get<FillBlip>(o);
    const QString fillImagePath = (fb) ?getPicturePath(fb->fillBlip) :"";
    if (!fillImagePath.isEmpty()) {
        style.addProperty("draw:fill-image-name",
                          "fillImage" + QString::number(fb->fillBlip), gt);
    }
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
    const LineEndArrowhead* lea = get<LineEndArrowhead>(o);
    if (lea && lea->lineEndArrowhead > 0 && lea->lineEndArrowhead < 6) {
        style.addProperty("draw:marker-end",
                          arrowHeads[lea->lineEndArrowhead], gt);
    }
    // draw:marker-end-center
    // draw:marker-end-width
    const LineWidth* lw = get<LineWidth>(o);
    const LineEndArrowWidth* lew = get<LineEndArrowWidth>(o);
    if (lw && lew) {
        style.addProperty("draw:marker-end-width", cm(lw->lineWidth*lew->lineEndArrowWidth), gt);
    }
    // draw:marker-start
    const LineStartArrowhead* lsa = get<LineStartArrowhead>(o);
    if (lsa && lsa->lineStartArrowhead > 0 && lsa->lineStartArrowhead < 6) {
        style.addProperty("draw:marker-start",
                          arrowHeads[lsa->lineStartArrowhead], gt);
    }
    // draw:marker-start-center
    // draw:marker-start-width
    const LineStartArrowWidth* lsw = get<LineStartArrowWidth>(o);
    if (lw && lsw) {
        style.addProperty("draw:marker-start-width",
                          cm(lw->lineWidth*lsw->lineStartArrowWidth), gt);
    }
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
    const ShadowOffsetX* sox =  get<ShadowOffsetX>(o);
    if (sox) {
        style.addProperty("draw:shadow-offset-x", cm(sox->shadowOffsetX), gt);
    }
    // draw:shadow-offset-y
    const ShadowOffsetY* soy =  get<ShadowOffsetY>(o);
    if (soy) {
        style.addProperty("draw:shadow-offset-y", cm(soy->shadowOffsetY), gt);
    }
    // draw:shadow-opacity
    const ShadowOpacity* so = get<ShadowOpacity>(o);
    if (so) {
        float opacity = toQReal(so->shadowOpacity);
        style.addProperty("draw:shadow-opacity", percent(opacity), gt);
    }
    // draw:show-unit
    // draw:start-guide
    // draw:start-line-spacing-horizontal
    // draw:start-line-spacing-vertical
    // draw:stroke ('dash', 'none' or 'solid')
    const LineDashing* ld = get<LineDashing>(o);
    // for now, go by the assumption that there is only a line
    // when fUsefLine and fLine are true
    const LineStyleBooleanProperties* bp = get<LineStyleBooleanProperties>(o);
    if (bp && bp->fUsefLine && bp->fLine) {
        if (ld && !(bp->fUseNoLineDrawDash && bp->fNoLineDrawDash)) {
            if (ld->lineDashing == 0 || ld->lineDashing >= 11) { // solid
                style.addProperty("draw:stroke", "solid", gt);
            } else {
                style.addProperty("draw:stroke", "dash", gt);
            }
        } else {
            // OOo interprets solid line with with 0 as hairline, so if
            // width == 0, stroke *must* be none
            if (lw && lw->lineWidth > 0) {
                style.addProperty("draw:stroke", "solid", gt);
            } else {
                style.addProperty("draw:stroke", "none", gt);
            }
        }
    }
    // draw:stroke-dash from 2.3.8.17 lineDashing
    if (ld && ld->lineDashing > 0 && ld->lineDashing < 11) {
        style.addProperty("draw:stroke-dash", dashses[ld->lineDashing], gt);
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
    // style:horizontal-pos
    // style:horizontal-rel
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
    // style:vertical-pos
    // style:vertical-rel
    // style:wrap
    // style:wrap-contour
    // style:wrap-contour-mode
    // style:wrap-dynamic-treshold
    // svg:fill-rule
    // svg:height
    // svg:stroke-color from 2.3.8.1 lineColor
    const LineColor* lc = get<LineColor>(o);
    if (lc) {
        style.addProperty("svg:stroke-color", toQColor(lc->lineColor).name(),
                          gt);
    }
    // svg:stroke-opacity from 2.3.8.2 lineOpacity
    const LineOpacity* lo = get<LineOpacity>(o);
    if (lo) {
        style.addProperty("svg:stroke-opacity", lo->lineOpacity / 0x10000f, gt);
    }
    // svg:stroke-width from 2.3.8.14 lineWidth
    if (lw) {
        style.addProperty("svg:stroke-width",
                          pt(lw->lineWidth / 12700.f), gt);
    }
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
quint32 PptToOdp::getTextType(const OfficeArtClientTextBox* clientTextbox,
            const OfficeArtClientData* clientData) const
{
    if (clientData && clientData->placeholderAtom && currentSlideTexts) {
        const PlaceholderAtom* p = clientData->placeholderAtom.data();
        if (p->position >= 0 && p->position < currentSlideTexts->atoms.size()) {
            const TextContainer& tc = currentSlideTexts->atoms[p->position];
            return tc.textHeaderAtom.textType;
        }
    }
    if (clientTextbox) {
        // find the text type
        foreach (const TextClientDataSubContainerOrAtom& a, clientTextbox->rgChildRec) {
            const TextContainer* tc = a.anon.get<TextContainer>();
            if (tc) {
                return tc->textHeaderAtom.textType;
            }
        }
    }
    return 99; // 99 means it is undefined here
}
QString getMasterStyle(const QMap<int, QString>& map, int texttype) {
    if (map.contains(texttype)) {
        return map[texttype];
    }
    // fallback for titles
    if (texttype == 0 || texttype == 6) {
        if (map.contains(0)) return map[0]; // Tx_TYPE_TITLE
        if (map.contains(6)) return map[6]; // Tx_TYPE_CENTERTITLE
    } else { // fallback for body 
        if (map.contains(1)) return map[1]; // Tx_TYPE_BODY
        if (map.contains(5)) return map[5]; // Tx_TYPE_CENTERBODY
        if (map.contains(7)) return map[7]; // Tx_TYPE_HALFBODY
        if (map.contains(8)) return map[8]; // Tx_TYPE_QUARTERBODY
        if (map.contains(4)) return map[4]; // Tx_TYPE_OTHER
    }
    // return any style we might have
    return (map.size()) ?map.begin().value() :QString();
}

void PptToOdp::addPresentationStyleToDrawElement(Writer& out,
                                            const OfficeArtSpContainer& o)
{
    quint32 textType = getTextType(o.clientTextbox.data(), o.clientData.data());
    bool canBeParentStyle = textType != 99 && out.stylesxml && currentMaster;
    bool isAutomatic = !canBeParentStyle;

    // if this object has a placeholder type, it defines a presentation style,
    // otherwise, it defines a graphic style
    // A graphic style is always automatic
    KoGenStyle::Type type = KoGenStyle::StylePresentation;
    if (isAutomatic) {
        type = KoGenStyle::StylePresentationAuto;
    }
    KoGenStyle style(type, "presentation");
    if (isAutomatic) {
        style.setAutoStyleInStylesDotXml(out.stylesxml);
    }

    QString parent;
    // for now we only set parent styles on presentation styled elements
    if (currentMaster) {
        parent = getMasterStyle(masterPresentationStyles[currentMaster],
                                textType);
    }
    if (!parent.isEmpty()) {
        style.setParentName(parent);
    }
    const TextMasterStyleAtom* listStyle = 0;
    if (out.stylesxml) {
        listStyle = getTextMasterStyleAtom(currentMaster, textType);
    }
    QString listStyleName;
    if (listStyle) {
        KoGenStyle list(KoGenStyle::StyleList);
        defineListStyle(list, *listStyle);
        listStyleName = out.styles.lookup(list, "");
    }
    defineGraphicProperties(style, o, listStyleName);
    if (listStyle && listStyle->lstLvl1) {
        defineParagraphProperties(style, &listStyle->lstLvl1->pf, 0);
        defineTextProperties(style, &listStyle->lstLvl1->cf, 0, 0, 0);
    }
    const QString styleName = out.styles.lookup(style);
    out.xml.addAttribute("presentation:style-name", styleName);
    if (canBeParentStyle) {
        masterPresentationStyles[currentMaster][textType] = styleName;
    }
}
void PptToOdp::addGraphicStyleToDrawElement(Writer& out,
                                            const OfficeArtSpContainer& o)
{
    bool isPlaceholder = o.clientData && o.clientData->placeholderAtom;
    if (isPlaceholder) {
        PptToOdp::addPresentationStyleToDrawElement(out, o);
        return;
    }

    quint32 textType = getTextType(o.clientTextbox.data(), o.clientData.data());
    KoGenStyle style(KoGenStyle::StyleGraphicAuto, "graphic");
    style.setAutoStyleInStylesDotXml(out.stylesxml);

    const TextMasterStyleAtom* listStyle = 0;
    listStyle = getTextMasterStyleAtom(currentMaster, textType);
    QString listStyleName;
    if (listStyle) {
        KoGenStyle list(KoGenStyle::StyleList);
        defineListStyle(list, *listStyle);
        listStyleName = out.styles.lookup(list, "");
    }
    defineGraphicProperties(style, o, listStyleName);
    if (listStyle && listStyle->lstLvl1) {
        defineParagraphProperties(style, &listStyle->lstLvl1->pf, 0);
        defineTextProperties(style, &listStyle->lstLvl1->cf, 0, 0, 0);
    }
    const QString styleName = out.styles.lookup(style);
    out.xml.addAttribute("draw:style-name", styleName);
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



