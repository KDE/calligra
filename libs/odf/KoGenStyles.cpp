/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   SPDX-FileCopyrightText: 2010 Jarosław Staniek <staniek@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#include "KoGenStyles.h"

#include "KoFontFace.h"
#include "KoOdfWriteStore.h"
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>
#include <OdfDebug.h>
#include <float.h>

static const struct {
    KoGenStyle::Type m_type;
    const char *m_elementName;
    const char *m_propertiesElementName;
    bool m_drawElement;
} styleData[] = {{KoGenStyle::TextStyle, "style:style", "style:text-properties", false},
                 {KoGenStyle::ParagraphStyle, "style:style", "style:paragraph-properties", false},
                 {KoGenStyle::SectionStyle, "style:style", "style:section-properties", false},
                 {KoGenStyle::RubyStyle, "style:style", "style:ruby-properties", false},
                 {KoGenStyle::TableStyle, "style:style", "style:table-properties", false},
                 {KoGenStyle::TableColumnStyle, "style:style", "style:table-column-properties", false},
                 {KoGenStyle::TableRowStyle, "style:style", "style:table-row-properties", false},
                 {KoGenStyle::TableCellStyle, "style:style", "style:table-cell-properties", false},
                 {KoGenStyle::GraphicStyle, "style:style", "style:graphic-properties", false},
                 {KoGenStyle::PresentationStyle, "style:style", "style:graphic-properties", false},
                 {KoGenStyle::DrawingPageStyle, "style:style", "style:drawing-page-properties", false},
                 {KoGenStyle::ChartStyle, "style:style", "style:chart-properties", false},
                 {KoGenStyle::ListStyle, "text:list-style", nullptr, false},
                 {KoGenStyle::LinearGradientStyle, "svg:linearGradient", nullptr, true},
                 {KoGenStyle::RadialGradientStyle, "svg:radialGradient", nullptr, true},
                 {KoGenStyle::ConicalGradientStyle, "calligra:conicalGradient", nullptr, true},
                 {KoGenStyle::StrokeDashStyle, "draw:stroke-dash", nullptr, true},
                 {KoGenStyle::FillImageStyle, "draw:fill-image", nullptr, true},
                 {KoGenStyle::HatchStyle, "draw:hatch", "style:graphic-properties", true},
                 {KoGenStyle::GradientStyle, "draw:gradient", "style:graphic-properties", true},
                 {KoGenStyle::MarkerStyle, "draw:marker", "style:graphic-properties", true},
                 {KoGenStyle::PresentationPageLayoutStyle, "style:presentation-page-layout", nullptr, false},
                 {KoGenStyle::OutlineLevelStyle, "text:outline-style", nullptr, false}};

static const unsigned int numStyleData = sizeof(styleData) / sizeof(*styleData);

static const struct {
    KoGenStyle::Type m_type;
    const char *m_elementName;
    const char *m_propertiesElementName;
    bool m_drawElement;
} autoStyleData[] = {{KoGenStyle::TextAutoStyle, "style:style", "style:text-properties", false},
                     {KoGenStyle::ParagraphAutoStyle, "style:style", "style:paragraph-properties", false},
                     {KoGenStyle::SectionAutoStyle, "style:style", "style:section-properties", false},
                     {KoGenStyle::RubyAutoStyle, "style:style", "style:ruby-properties", false},
                     {KoGenStyle::TableAutoStyle, "style:style", "style:table-properties", false},
                     {KoGenStyle::TableColumnAutoStyle, "style:style", "style:table-column-properties", false},
                     {KoGenStyle::TableRowAutoStyle, "style:style", "style:table-row-properties", false},
                     {KoGenStyle::TableCellAutoStyle, "style:style", "style:table-cell-properties", false},
                     {KoGenStyle::GraphicAutoStyle, "style:style", "style:graphic-properties", false},
                     {KoGenStyle::PresentationAutoStyle, "style:style", "style:graphic-properties", false},
                     {KoGenStyle::DrawingPageAutoStyle, "style:style", "style:drawing-page-properties", false},
                     {KoGenStyle::ChartAutoStyle, "style:style", "style:chart-properties", false},
                     {KoGenStyle::PageLayoutStyle, "style:page-layout", "style:page-layout-properties", false},
                     {KoGenStyle::ListAutoStyle, "text:list-style", nullptr, false},
                     {KoGenStyle::NumericNumberStyle, "number:number-style", nullptr, false},
                     {KoGenStyle::NumericFractionStyle, "number:number-style", nullptr, false},
                     {KoGenStyle::NumericScientificStyle, "number:number-style", nullptr, false},
                     {KoGenStyle::NumericDateStyle, "number:date-style", nullptr, false},
                     {KoGenStyle::NumericTimeStyle, "number:time-style", nullptr, false},
                     {KoGenStyle::NumericPercentageStyle, "number:percentage-style", nullptr, false},
                     {KoGenStyle::NumericCurrencyStyle, "number:currency-style", nullptr, false},
                     {KoGenStyle::NumericBooleanStyle, "number:boolean-style", nullptr, false},
                     {KoGenStyle::NumericTextStyle, "number:text-style", nullptr, false}};

static const unsigned int numAutoStyleData = sizeof(autoStyleData) / sizeof(*autoStyleData);

static void insertRawOdfStyles(const QByteArray &xml, QByteArray &styles)
{
    if (xml.isEmpty())
        return;
    if (!styles.isEmpty() && !styles.endsWith('\n') && !xml.startsWith('\n')) {
        styles.append('\n');
    }
    styles.append(xml);
}

class Q_DECL_HIDDEN KoGenStyles::Private
{
public:
    Private(KoGenStyles *q)
        : q(q)
    {
    }

    ~Private() = default;

    QVector<KoGenStyles::NamedStyle> styles(bool autoStylesInStylesDotXml, KoGenStyle::Type type) const;
    void saveOdfAutomaticStyles(KoXmlWriter *xmlWriter, bool autoStylesInStylesDotXml, const QByteArray &rawOdfAutomaticStyles) const;
    void saveOdfDocumentStyles(KoXmlWriter *xmlWriter) const;
    void saveOdfMasterStyles(KoXmlWriter *xmlWriter) const;
    QString makeUniqueName(const QString &base, const QByteArray &family, InsertionFlags flags) const;

    /**
     * Save font face declarations
     *
     * This creates the office:font-face-decls tag containing all font face
     * declarations
     */
    void saveOdfFontFaceDecls(KoXmlWriter *xmlWriter) const;

    /// style definition -> name
    StyleMap styleMap;

    /// Map with the style name as key.
    /// This map is mainly used to check for name uniqueness
    QMap<QByteArray, QSet<QString>> styleNames;
    QMap<QByteArray, QSet<QString>> autoStylesInStylesDotXml;

    /// List of styles (used to preserve ordering)
    QVector<KoGenStyles::NamedStyle> styleList;

    /// map for saving default styles
    QMap<int, KoGenStyle> defaultStyles;

    /// font faces
    QMap<QString, KoFontFace> fontFaces;

    StyleMap::iterator insertStyle(const KoGenStyle &style, const QString &name, InsertionFlags flags);

    struct RelationTarget {
        QString target; // the style we point to
        QString attribute; // the attribute name used for the relation
    };
    QHash<QString, RelationTarget> relations; // key is the name of the source style

    QByteArray rawOdfDocumentStyles;
    QByteArray rawOdfAutomaticStyles_stylesDotXml;
    QByteArray rawOdfAutomaticStyles_contentDotXml;
    QByteArray rawOdfMasterStyles;
    QByteArray rawOdfFontFaceDecls;

    KoGenStyles *q;
};

QVector<KoGenStyles::NamedStyle> KoGenStyles::Private::styles(bool autoStylesInStylesDotXml, KoGenStyle::Type type) const
{
    QVector<KoGenStyles::NamedStyle> lst;
    QVector<KoGenStyles::NamedStyle>::const_iterator it = styleList.constBegin();
    const QVector<KoGenStyles::NamedStyle>::const_iterator end = styleList.constEnd();
    for (; it != end; ++it) {
        if ((*it).style->type() == type && (*it).style->autoStyleInStylesDotXml() == autoStylesInStylesDotXml) {
            lst.append(*it);
        }
    }
    return lst;
}

void KoGenStyles::Private::saveOdfAutomaticStyles(KoXmlWriter *xmlWriter, bool autoStylesInStylesDotXml, const QByteArray &rawOdfAutomaticStyles) const
{
    xmlWriter->startElement("office:automatic-styles");

    for (uint i = 0; i < numAutoStyleData; ++i) {
        QVector<KoGenStyles::NamedStyle> stylesList = styles(autoStylesInStylesDotXml, autoStyleData[i].m_type);
        QVector<KoGenStyles::NamedStyle>::const_iterator it = stylesList.constBegin();
        for (; it != stylesList.constEnd(); ++it) {
            (*it).style->writeStyle(xmlWriter,
                                    *q,
                                    autoStyleData[i].m_elementName,
                                    (*it).name,
                                    autoStyleData[i].m_propertiesElementName,
                                    true,
                                    autoStyleData[i].m_drawElement);
        }
    }

    if (!rawOdfAutomaticStyles.isEmpty()) {
        xmlWriter->addCompleteElement(rawOdfAutomaticStyles.constData());
    }

    xmlWriter->endElement(); // office:automatic-styles
}

void KoGenStyles::Private::saveOdfDocumentStyles(KoXmlWriter *xmlWriter) const
{
    xmlWriter->startElement("office:styles");

    for (uint i = 0; i < numStyleData; ++i) {
        const QMap<int, KoGenStyle>::const_iterator it(defaultStyles.constFind(styleData[i].m_type));
        if (it != defaultStyles.constEnd()) {
            it.value().writeStyle(xmlWriter, *q, "style:default-style", "", styleData[i].m_propertiesElementName, true, styleData[i].m_drawElement);
        }
    }

    for (uint i = 0; i < numStyleData; ++i) {
        QVector<KoGenStyles::NamedStyle> stylesList(styles(false, styleData[i].m_type));
        QVector<KoGenStyles::NamedStyle>::const_iterator it = stylesList.constBegin();
        for (; it != stylesList.constEnd(); ++it) {
            if (relations.contains(it->name)) {
                KoGenStyles::Private::RelationTarget relation = relations.value(it->name);
                KoGenStyle styleCopy = *(*it).style;
                styleCopy.addAttribute(relation.attribute, relation.target);
                styleCopy
                    .writeStyle(xmlWriter, *q, styleData[i].m_elementName, (*it).name, styleData[i].m_propertiesElementName, true, styleData[i].m_drawElement);
            } else {
                (*it).style->writeStyle(xmlWriter,
                                        *q,
                                        styleData[i].m_elementName,
                                        (*it).name,
                                        styleData[i].m_propertiesElementName,
                                        true,
                                        styleData[i].m_drawElement);
            }
        }
    }

    if (!rawOdfDocumentStyles.isEmpty()) {
        xmlWriter->addCompleteElement(rawOdfDocumentStyles.constData());
    }

    xmlWriter->endElement(); // office:styles
}

void KoGenStyles::Private::saveOdfMasterStyles(KoXmlWriter *xmlWriter) const
{
    xmlWriter->startElement("office:master-styles");

    QVector<KoGenStyles::NamedStyle> stylesList = styles(false, KoGenStyle::MasterPageStyle);
    QVector<KoGenStyles::NamedStyle>::const_iterator it = stylesList.constBegin();
    for (; it != stylesList.constEnd(); ++it) {
        (*it).style->writeStyle(xmlWriter, *q, "style:master-page", (*it).name, nullptr);
    }

    if (!rawOdfMasterStyles.isEmpty()) {
        xmlWriter->addCompleteElement(rawOdfMasterStyles.constData());
    }

    xmlWriter->endElement(); // office:master-styles
}

void KoGenStyles::Private::saveOdfFontFaceDecls(KoXmlWriter *xmlWriter) const
{
    if (fontFaces.isEmpty())
        return;

    xmlWriter->startElement("office:font-face-decls");
    for (QMap<QString, KoFontFace>::ConstIterator it(fontFaces.constBegin()); it != fontFaces.constEnd(); ++it) {
        it.value().saveOdf(xmlWriter);
    }

    if (!rawOdfFontFaceDecls.isEmpty()) {
        xmlWriter->addCompleteElement(rawOdfFontFaceDecls.constData());
    }

    xmlWriter->endElement(); // office:font-face-decls
}

QString KoGenStyles::Private::makeUniqueName(const QString &base, const QByteArray &family, InsertionFlags flags) const
{
    // If this name is not used yet, and numbering isn't forced, then the given name is ok.
    if ((flags & DontAddNumberToName) && !autoStylesInStylesDotXml[family].contains(base) && !styleNames[family].contains(base))
        return base;
    int num = 1;
    QString name;
    do {
        name = base + QString::number(num++);
    } while (autoStylesInStylesDotXml[family].contains(name) || styleNames[family].contains(name));
    return name;
}

//------------------------

KoGenStyles::KoGenStyles()
    : d(new Private(this))
{
}

KoGenStyles::~KoGenStyles()
{
    delete d;
}

QString KoGenStyles::insert(const KoGenStyle &style, const QString &baseName, InsertionFlags flags)
{
    // if it is a default style it has to be saved differently
    if (style.isDefaultStyle()) {
        // we can have only one default style per type
        Q_ASSERT(!d->defaultStyles.contains(style.type()));
        // default style is only possible for style:style in office:style types
        Q_ASSERT(style.type() == KoGenStyle::TextStyle || style.type() == KoGenStyle::ParagraphStyle || style.type() == KoGenStyle::SectionStyle
                 || style.type() == KoGenStyle::RubyStyle || style.type() == KoGenStyle::TableStyle || style.type() == KoGenStyle::TableColumnStyle
                 || style.type() == KoGenStyle::TableRowStyle || style.type() == KoGenStyle::TableCellStyle || style.type() == KoGenStyle::GraphicStyle
                 || style.type() == KoGenStyle::PresentationStyle || style.type() == KoGenStyle::DrawingPageStyle || style.type() == KoGenStyle::ChartStyle);

        d->defaultStyles.insert(style.type(), style);
        // default styles don't have a name
        return QString();
    }

    if (flags & AllowDuplicates) {
        StyleMap::iterator it = d->insertStyle(style, baseName, flags);
        return it.value();
    }

    StyleMap::iterator it = d->styleMap.find(style);
    if (it == d->styleMap.end()) {
        // Not found, try if this style is in fact equal to its parent (the find above
        // wouldn't have found it, due to m_parentName being set).
        if (!style.parentName().isEmpty()) {
            KoGenStyle testStyle(style);
            const KoGenStyle *parentStyle = this->style(style.parentName(), style.familyName()); // ## linear search
            if (!parentStyle) {
                debugOdf << "baseName=" << baseName << "parent style" << style.parentName() << "not found in collection";
            } else {
                // TODO remove
                if (testStyle.m_familyName != parentStyle->m_familyName) {
                    warnOdf << "baseName=" << baseName << "family=" << testStyle.m_familyName << "parent style" << style.parentName()
                            << "has a different family:" << parentStyle->m_familyName;
                }

                testStyle.m_parentName = parentStyle->m_parentName;
                // Exclude the type from the comparison. It's ok for an auto style
                // to have a user style as parent; they can still be identical
                testStyle.m_type = parentStyle->m_type;
                // Also it's ok to not have the display name of the parent style
                // in the auto style
                const auto it = parentStyle->m_attributes.find("style:display-name");
                if (it != parentStyle->m_attributes.end())
                    testStyle.addAttribute("style:display-name", *it);

                if (*parentStyle == testStyle)
                    return style.parentName();
            }
        }

        it = d->insertStyle(style, baseName, flags);
    }
    return it.value();
}

KoGenStyles::StyleMap::iterator KoGenStyles::Private::insertStyle(const KoGenStyle &style, const QString &baseName, InsertionFlags flags)
{
    QString styleName(baseName);
    if (styleName.isEmpty()) {
        switch (style.type()) {
        case KoGenStyle::ParagraphAutoStyle:
            styleName = 'P';
            break;
        case KoGenStyle::ListAutoStyle:
            styleName = 'L';
            break;
        case KoGenStyle::TextAutoStyle:
            styleName = 'T';
            break;
        default:
            styleName = 'A'; // for "auto".
        }
        flags &= ~DontAddNumberToName; // i.e. force numbering
    }
    styleName = makeUniqueName(styleName, style.m_familyName, flags);
    if (style.autoStyleInStylesDotXml())
        autoStylesInStylesDotXml[style.m_familyName].insert(styleName);
    else
        styleNames[style.m_familyName].insert(styleName);
    KoGenStyles::StyleMap::iterator it = styleMap.insert(style, styleName);
    NamedStyle s;
    s.style = &it.key();
    s.name = styleName;
    styleList.append(s);
    return it;
}

KoGenStyles::StyleMap KoGenStyles::styles() const
{
    return d->styleMap;
}

QVector<KoGenStyles::NamedStyle> KoGenStyles::styles(KoGenStyle::Type type) const
{
    return d->styles(false, type);
}

const KoGenStyle *KoGenStyles::style(const QString &name, const QByteArray &family) const
{
    QVector<KoGenStyles::NamedStyle>::const_iterator it = d->styleList.constBegin();
    const QVector<KoGenStyles::NamedStyle>::const_iterator end = d->styleList.constEnd();
    for (; it != end; ++it) {
        if ((*it).name == name && (*it).style->familyName() == family) {
            return (*it).style;
        }
    }
    return nullptr;
}

KoGenStyle *KoGenStyles::styleForModification(const QString &name, const QByteArray &family)
{
    return const_cast<KoGenStyle *>(style(name, family));
}

void KoGenStyles::markStyleForStylesXml(const QString &name, const QByteArray &family)
{
    Q_ASSERT(d->styleNames[family].contains(name));
    d->styleNames[family].remove(name);
    d->autoStylesInStylesDotXml[family].insert(name);
    styleForModification(name, family)->setAutoStyleInStylesDotXml(true);
}

void KoGenStyles::insertFontFace(const KoFontFace &face)
{
    Q_ASSERT(!face.isNull());
    if (face.isNull()) {
        warnOdf << "This font face is null and will not be added to styles: set at least the name";
        return;
    }
    d->fontFaces.insert(face.name(), face); // replaces prev item
}

KoFontFace KoGenStyles::fontFace(const QString &name) const
{
    return d->fontFaces.value(name);
}

bool KoGenStyles::saveOdfStylesDotXml(KoStore *store, KoXmlWriter *manifestWriter) const
{
    if (!store->open("styles.xml"))
        return false;

    manifestWriter->addManifestEntry(store->currentPath() + "styles.xml", "text/xml");

    KoStoreDevice stylesDev(store);
    KoXmlWriter *stylesWriter = KoOdfWriteStore::createOasisXmlWriter(&stylesDev, "office:document-styles");

    d->saveOdfFontFaceDecls(stylesWriter);
    d->saveOdfDocumentStyles(stylesWriter);
    d->saveOdfAutomaticStyles(stylesWriter, true, d->rawOdfAutomaticStyles_stylesDotXml);
    d->saveOdfMasterStyles(stylesWriter);

    stylesWriter->endElement(); // root element (office:document-styles)
    stylesWriter->endDocument();
    delete stylesWriter;

    if (!store->close()) // done with styles.xml
        return false;

    return true;
}

void KoGenStyles::saveOdfStyles(StylesPlacement placement, KoXmlWriter *xmlWriter) const
{
    switch (placement) {
    case DocumentStyles:
        d->saveOdfDocumentStyles(xmlWriter);
        break;
    case MasterStyles:
        d->saveOdfMasterStyles(xmlWriter);
        break;
    case DocumentAutomaticStyles:
        d->saveOdfAutomaticStyles(xmlWriter, false, d->rawOdfAutomaticStyles_contentDotXml);
        break;
    case StylesXmlAutomaticStyles:
        d->saveOdfAutomaticStyles(xmlWriter, true, d->rawOdfAutomaticStyles_stylesDotXml);
        break;
    case FontFaceDecls:
        d->saveOdfFontFaceDecls(xmlWriter);
        break;
    }
}

void KoGenStyles::insertRawOdfStyles(StylesPlacement placement, const QByteArray &xml)
{
    switch (placement) {
    case DocumentStyles:
        ::insertRawOdfStyles(xml, d->rawOdfDocumentStyles);
        break;
    case MasterStyles:
        ::insertRawOdfStyles(xml, d->rawOdfMasterStyles);
        break;
    case DocumentAutomaticStyles:
        ::insertRawOdfStyles(xml, d->rawOdfAutomaticStyles_contentDotXml);
        break;
    case StylesXmlAutomaticStyles:
        ::insertRawOdfStyles(xml, d->rawOdfAutomaticStyles_stylesDotXml);
        break;
    case FontFaceDecls:
        ::insertRawOdfStyles(xml, d->rawOdfFontFaceDecls);
        break;
    }
}

void KoGenStyles::insertStyleRelation(const QString &source, const QString &target, const char *tagName)
{
    KoGenStyles::Private::RelationTarget relation;
    relation.target = target;
    relation.attribute = QString(tagName);
    d->relations.insert(source, relation);
}

QDebug operator<<(QDebug dbg, const KoGenStyles &styles)
{
    dbg.nospace() << "KoGenStyles:";
    QVector<KoGenStyles::NamedStyle>::const_iterator it = styles.d->styleList.constBegin();
    const QVector<KoGenStyles::NamedStyle>::const_iterator end = styles.d->styleList.constEnd();
    for (; it != end; ++it) {
        dbg.nospace() << (*it).name;
    }
    for (QMap<QByteArray, QSet<QString>>::const_iterator familyIt(styles.d->styleNames.constBegin()); familyIt != styles.d->styleNames.constEnd(); ++familyIt) {
        for (QSet<QString>::const_iterator it(familyIt.value().constBegin()); it != familyIt.value().constEnd(); ++it) {
            dbg.space() << "style:" << *it;
        }
    }
#ifndef NDEBUG
    for (QMap<QByteArray, QSet<QString>>::const_iterator familyIt(styles.d->autoStylesInStylesDotXml.constBegin());
         familyIt != styles.d->autoStylesInStylesDotXml.constEnd();
         ++familyIt) {
        for (QSet<QString>::const_iterator it(familyIt.value().constBegin()); it != familyIt.value().constEnd(); ++it) {
            dbg.space() << "auto style for style.xml:" << *it;
        }
    }
#endif
    return dbg.space();
}
