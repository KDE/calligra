/* This file is part of the KDE project
 * Copyright (C) 2006, 2008, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWPageStyle.h"
#include "KWPageStyle_p.h"

#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoUnit.h>
#include <KoColorBackground.h>
#include <KoPatternBackground.h>
#include <KoImageCollection.h>
#include <KoOdfLoadingContext.h>

#include <QBuffer>
#include <QColor>

KWPageStylePrivate::~KWPageStylePrivate()
{
}

void KWPageStylePrivate::clear()
{
    displayName.clear();
    headerDistance = 10; // ~3mm
    footerDistance = 10;
    headerMinimumHeight = 10; // includes spacing
    footerMinimumHeight = 10; // includes spacing
    headers = Words::HFTypeNone;
    footers = Words::HFTypeNone;
    pageUsage = KWPageStyle::AllPages;
    columns.reset();
    direction = KoText::AutoDirection;
    headerDynamicSpacing = false;
    footerDynamicSpacing = false;
    fullPageBackground.clear();
    nextStyleName.clear();
}

///////////

KWPageStyle::KWPageStyle(const QString &name, const QString &displayname)
    : d (new KWPageStylePrivate())
{
    d->name = name;
    if (!displayname.isEmpty() && displayname != name)
        d->displayName = displayname;
}

KWPageStyle::KWPageStyle(const KWPageStyle &ps)
    : d(ps.d)
{
}

KWPageStyle::KWPageStyle()
{
}

bool KWPageStyle::isValid() const
{
    return d && !d->name.isEmpty();
}


KWPageStyle &KWPageStyle::operator=(const KWPageStyle &ps)
{
    d = ps.d;
    return *this;
}

KWPageStyle::~KWPageStyle()
{
}

KWPageStyle::PageUsageType KWPageStyle::pageUsage() const
{
    return d ? d->pageUsage : KWPageStyle::AllPages;
}

void KWPageStyle::setPageUsage(KWPageStyle::PageUsageType pageusage) const
{
    d->pageUsage = pageusage;
}

void KWPageStyle::setFooterPolicy(Words::HeaderFooterType p)
{
    d->footers = p;
}

void KWPageStyle::setHeaderPolicy(Words::HeaderFooterType p)
{
    d->headers = p;
}

KoPageLayout KWPageStyle::pageLayout() const
{
    return d->pageLayout;
}

void KWPageStyle::setPageLayout(const KoPageLayout &pageLayout)
{
    d->pageLayout = pageLayout;
}

KoColumns KWPageStyle::columns() const
{
    return d->columns;
}

void KWPageStyle::setColumns(const KoColumns &columns)
{
    d->columns = columns;
}

Words::HeaderFooterType KWPageStyle::headerPolicy() const
{
    return d->headers;
}

Words::HeaderFooterType KWPageStyle::footerPolicy() const
{
    return d->footers;
}

qreal KWPageStyle::headerDistance() const
{
    return d->headerDistance;
}

void KWPageStyle::setHeaderDistance(qreal distance)
{
    d->headerDistance = distance;
}

bool KWPageStyle::headerDynamicSpacing() const
{
    return d->headerDynamicSpacing;
}

void KWPageStyle::setHeaderDynamicSpacing(bool dynamic)
{
    d->headerDynamicSpacing = dynamic;
}

qreal KWPageStyle::headerMinimumHeight() const
{
    return d->headerMinimumHeight;
}

void KWPageStyle::setHeaderMinimumHeight(qreal height)
{
    d->headerMinimumHeight = height;
}

qreal KWPageStyle::footerMinimumHeight() const
{
    return d->footerMinimumHeight;
}

void KWPageStyle::setFooterMinimumHeight(qreal height)
{
    d->footerMinimumHeight = height;
}

qreal KWPageStyle::footerDistance() const
{
    return d->footerDistance;
}

void KWPageStyle::setFooterDistance(qreal distance)
{
    d->footerDistance = distance;
}

bool KWPageStyle::footerDynamicSpacing() const
{
    return d->footerDynamicSpacing;
}

void KWPageStyle::setFooterDynamicSpacing(bool dynamic)
{
    d->footerDynamicSpacing = dynamic;
}

void KWPageStyle::clear()
{
    d->clear();
}

QString KWPageStyle::name() const
{
    return d->name;
}

QString KWPageStyle::displayName() const
{
    return d->displayName;
}

QSharedPointer<KoShapeBackground> KWPageStyle::background() const
{
    return d->fullPageBackground;
}

void KWPageStyle::setBackground(QSharedPointer<KoShapeBackground> background)
{
    d->fullPageBackground = background;
}

KoGenStyle KWPageStyle::saveOdf() const
{
    KoGenStyle pageLayout = d->pageLayout.saveOdf();
    pageLayout.setAutoStyleInStylesDotXml(true);

    switch (d->pageUsage) {
        case LeftPages:
            pageLayout.addAttribute("style:page-usage", "left");
            break;
        case MirroredPages:
            pageLayout.addAttribute("style:page-usage", "mirrored");
            break;
        case RightPages:
            pageLayout.addAttribute("style:page-usage", "right");
            break;
        default:
            pageLayout.addAttribute("style:page-usage", "all");
            break;
    }

    // Save background color if it is set
    if (d->fullPageBackground)
    {
        KoColorBackground *colorBackground = dynamic_cast<KoColorBackground*>(d->fullPageBackground.data());
        if (colorBackground)
            pageLayout.addProperty("fo:background-color", colorBackground->color().name());
    }

    // save column data
    d->columns.saveOdf(pageLayout);

    //<style:footnote-sep style:adjustment="left" style:width="0.5pt" style:rel-width="20%" style:line-style="solid"/>
    //writer.startElement("style:footnote-sep");
    // TODO
    //writer.addAttribute("style:adjustment",)
    //writer.addAttribute("style:width",)
    //writer.addAttribute("style:rel-width",)
    //writer.addAttribute("style:line-style",)
    //writer.endElement();



    if (headerPolicy() != Words::HFTypeNone) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&buffer);

        writer.startElement("style:header-style");
        writer.startElement("style:header-footer-properties");
        writer.addAttributePt("fo:min-height", headerMinimumHeight());
        writer.addAttributePt("fo:margin-bottom", headerDistance());
        writer.addAttribute("style:dynamic-spacing", headerDynamicSpacing());
        // TODO there are quite some more properties we want to at least preserve between load and save
        writer.endElement();
        writer.endElement();

        QString contentElement = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
        // the 1_ 2_ is needed to get the correct order
        pageLayout.addStyleChildElement("1_headerStyle", contentElement);
    }
    if (footerPolicy() != Words::HFTypeNone) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&buffer);

        writer.startElement("style:footer-style");
        writer.startElement("style:header-footer-properties");
        writer.addAttributePt("fo:min-height", footerMinimumHeight());
        writer.addAttributePt("fo:margin-top", footerDistance());
        writer.addAttribute("style:dynamic-spacing", footerDynamicSpacing());
        // TODO there are quite some more properties we want to at least preserve between load and save
        writer.endElement();
        writer.endElement();

        QString contentElement = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
        pageLayout.addStyleChildElement("2_footerStyle", contentElement);
    }

    // TODO see how we should save margins if we use the 'closest to binding' stuff.

    return pageLayout;
}

void KWPageStyle::loadOdf(KoOdfLoadingContext &context, const KoXmlElement &masterNode, const KoXmlElement &style, KoDocumentResourceManager *documentResources)
{
    d->pageLayout.loadOdf(style);
    KoXmlElement props = KoXml::namedItemNS(style, KoXmlNS::style, "page-layout-properties");
    if (props.isNull())
        return;
    QString direction = props.attributeNS(KoXmlNS::style, "writing-mode", "lr-tb");
    d->direction = KoText::directionFromString(direction);

    QString pageUsage = style.attributeNS(KoXmlNS::style, "page-usage", "all");
    if (pageUsage == "left") {
        d->pageUsage = LeftPages;
    } else if (pageUsage == "mirrored") {
        d->pageUsage = MirroredPages;
    } else if (pageUsage == "right") {
        d->pageUsage = RightPages;
    } else { // "all"
        d->pageUsage = AllPages;
    }

    d->columns.loadOdf(props);

    KoXmlElement header = KoXml::namedItemNS(style, KoXmlNS::style, "header-style");
    if (! header.isNull()) {
        KoXmlElement hfprops = KoXml::namedItemNS(header, KoXmlNS::style, "header-footer-properties");
        if (! hfprops.isNull()) {
            d->headerDistance = KoUnit::parseValue(hfprops.attributeNS(KoXmlNS::fo, "margin-bottom"));
            d->headerMinimumHeight = KoUnit::parseValue(hfprops.attributeNS(KoXmlNS::fo, "min-height"));
            const QString dynamicSpacing(hfprops.attributeNS(KoXmlNS::style, "dynamic-spacing"));
            d->headerDynamicSpacing = dynamicSpacing == "true";
        }
        // TODO there are quite some more properties we want to at least preserve between load and save
    }

    KoXmlElement footer = KoXml::namedItemNS(style, KoXmlNS::style, "footer-style");
    if (! footer.isNull()) {
        KoXmlElement hfprops = KoXml::namedItemNS(footer, KoXmlNS::style, "header-footer-properties");
        if (! hfprops.isNull()) {
            d->footerDistance = KoUnit::parseValue(hfprops.attributeNS(KoXmlNS::fo, "margin-top"));
            d->footerMinimumHeight = KoUnit::parseValue(hfprops.attributeNS(KoXmlNS::fo, "min-height"));
            const QString dynamicSpacing(hfprops.attributeNS(KoXmlNS::style, "dynamic-spacing"));
            d->footerDynamicSpacing = dynamicSpacing == "true";
        }
        // TODO there are quite some more properties we want to at least preserve between load and save
    }

    // Load background picture
    KoXmlElement propBackgroundImage = KoXml::namedItemNS(props, KoXmlNS::style, "background-image");
    if (!propBackgroundImage.isNull()) {
        const QString href = propBackgroundImage.attributeNS(KoXmlNS::xlink, "href", QString());
        if (!href.isEmpty()) {
            QSharedPointer<KoPatternBackground> background =  QSharedPointer<KoPatternBackground>(new KoPatternBackground(documentResources->imageCollection()));
            d->fullPageBackground = background;

            KoImageCollection *imageCollection = documentResources->imageCollection();
            if (imageCollection != 0) {
                KoImageData *imageData = imageCollection->createImageData(href,context.store());
                background->setPattern(imageData);
            }
        }
        // TODO load another possible attributes
    }

    // Load background color
    QString backgroundColor = props.attributeNS(KoXmlNS::fo, "background-color", QString());
    if (!backgroundColor.isNull() && d->fullPageBackground == 0) {

        if (backgroundColor == "transparent") {
            d->fullPageBackground.clear();
        }
        else {
            d->fullPageBackground = QSharedPointer<KoShapeBackground>(new KoColorBackground(QColor(backgroundColor)));
        }
    }

    // Load next master-page style name
    d->nextStyleName = masterNode.attributeNS(KoXmlNS::style, "next-style-name", QString());
}

QString KWPageStyle::nextStyleName() const
{
    return d->nextStyleName;
}

void KWPageStyle::setNextStyleName(const QString &nextStyleName)
{
    d->nextStyleName = nextStyleName;
}

KoText::Direction KWPageStyle::direction() const
{
    return d->direction;
}

void KWPageStyle::setDirection(KoText::Direction direction)
{
    d->direction = direction;
}

bool KWPageStyle::operator==(const KWPageStyle &other) const
{
    bool equals = d == other.d;
    return equals;
}

KWPageStylePrivate *KWPageStyle::priv()
{
    return d.data();
}

const KWPageStylePrivate *KWPageStyle::priv() const
{
    return d.data();
}

uint KWPageStyle::hash() const
{
    return ((uint) d) + 1;
}

bool KWPageStyle::isPageSpread() const
{
    return d->pageLayout.leftMargin < 0;
}

uint qHash(const KWPageStyle &style)
{
    return style.hash();
}

void KWPageStyle::detach(const QString &newName, const QString &displayName)
{
    d->fullPageBackground.clear();
    d.detach();
    d->name = newName;
    d->displayName = displayName;
}
