/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2016 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2016 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <KoOdfWorkaround.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>

#include "Condition.h"
#include "StyleManager.h"
#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "engine/MapBase.h"

// This file contains functionality to load/save styles

namespace Calligra
{
namespace Sheets
{

namespace Odf
{

// Single style loading
void loadStyle(Style *style,
               KoOdfStylesReader &stylesReader,
               const KoXmlElement &element,
               Conditions &conditions,
               const StyleManager *styleManager,
               const Localization *locale);
void loadCustomStyle(CustomStyle *style,
                     KoOdfStylesReader &stylesReader,
                     const KoXmlElement &xmlstyle,
                     const QString &name,
                     Conditions &conditions,
                     const StyleManager *styleManager,
                     const Localization *locale);

// Single style saving
void saveStyle(const Style *style,
               const QSet<Style::Key> &subStyles,
               KoGenStyle &xmlstyle,
               KoGenStyles &mainStyles,
               const StyleManager *manager,
               Localization *locale);
QString saveCustomStyle(CustomStyle *style, KoGenStyle &genstyle, KoGenStyles &mainStyles, const StyleManager *manager, Localization *locale);

void loadParagraphProperties(Style *style, KoOdfStylesReader &stylesReader, const KoStyleStack &styleStack);
void loadTableCellProperties(Style *style, KoOdfStylesReader &stylesReader, const KoStyleStack &styleStack);
void loadTextProperties(Style *style, KoOdfStylesReader &stylesReader, const KoStyleStack &styleStack);

/**
 * @return the name of the data style (number, currency, percentage, date,
 * boolean, text)
 */
QString saveStyleNumeric(KoGenStyle &style,
                         KoGenStyles &mainStyles,
                         Format::Type _style,
                         const QString &_prefix,
                         const QString &_postfix,
                         int _precision,
                         const QString &symbol,
                         bool thousandsSep,
                         Localization *locale);
QString saveStyleNumericDate(KoGenStyles &mainStyles, Format::Type _style, const QString &_prefix, const QString &_postfix, Localization *locale);
QString saveStyleNumericFraction(KoGenStyles &mainStyles, Format::Type _style, const QString &_prefix, const QString &_suffix);
QString saveStyleNumericTime(KoGenStyles &mainStyles, Format::Type _style, const QString &_prefix, const QString &_suffix, const Localization *locale);
QString saveStyleNumericCustom(KoGenStyles &mainStyles, Format::Type _style, const QString &_prefix, const QString &_suffix);
QString
saveStyleNumericScientific(KoGenStyles &mainStyles, Format::Type _style, const QString &_prefix, const QString &_suffix, int _precision, bool thousandsSep);
QString saveStyleNumericPercentage(KoGenStyles &mainStyles, Format::Type _style, int _precision, const QString &_prefix, const QString &_suffix);
QString
saveStyleNumericMoney(KoGenStyles &mainStyles, Format::Type _style, const QString &symbol, int _precision, const QString &_prefix, const QString &_suffix);
QString saveStyleNumericText(KoGenStyles &mainStyles, Format::Type _style, int _precision, const QString &_prefix, const QString &_suffix);
QString saveStyleNumericNumber(KoGenStyles &mainStyles, Format::Type _style, int _precision, const QString &_prefix, const QString &_suffix, bool thousandsSep);
QString saveBackgroundStyle(KoGenStyles &mainStyles, const QBrush &brush);

// Helpers
Format::Type dateType(const QString &, const Localization *locale);
Format::Type timeType(const QString &, const Localization *locale);
Format::Type fractionType(const QString &);
Format::Type numberType(const QString &);
Currency numberCurrency(const QString &);
QPen decodePen(const QString &border);
QString encodePen(const QPen &pen);
/**
 * Returns the name of a color.  This is the same as returned by QColor::name, but an internal cache
 * is used to reduce the overhead when asking for the name of the same color.
 */
QString colorName(const QColor &color);
}

void Odf::loadStyleTemplate(StyleManager *styles, KoOdfStylesReader &stylesReader, MapBase *map)
{
    // reset the map of OpenDocument Styles
    styles->clearOasisStyles();
    QHash<QString, QString> oasisStyles;

    // loading default style first
    const KoXmlElement *defStyle = stylesReader.defaultStyle("table-cell");
    if (defStyle) {
        debugSheetsODF << "StyleManager: Loading default cell style";
        Conditions conditions;
        loadCustomStyle(styles->defaultStyle(), stylesReader, *defStyle, "Default", conditions, styles, map->calculationSettings()->locale());
        styles->defaultStyle()->setType(Style::BUILTIN);
        if (map) {
            // Load the default precision to be used, if the (default) cell style
            // is set to arbitrary precision.
            KoXmlNode n = defStyle->firstChild();
            while (!n.isNull()) {
                if (n.isElement() && n.namespaceURI() == KoXmlNS::style && n.localName() == "table-cell-properties") {
                    KoXmlElement e = n.toElement();
                    if (n.toElement().hasAttributeNS(KoXmlNS::style, "decimal-places")) {
                        bool ok;
                        const int precision = n.toElement().attributeNS(KoXmlNS::style, "decimal-places").toInt(&ok);
                        if (ok && precision > -1) {
                            debugSheetsODF << "Default decimal precision:" << precision;
                            map->calculationSettings()->setDefaultDecimalPrecision(precision);
                        }
                    }
                }
                n = n.nextSibling();
            }
        }
    } else
        styles->resetDefaultStyle();

    QList<KoXmlElement *> customStyles(stylesReader.customStyles("table-cell").values());
    uint nStyles = customStyles.count();
    for (unsigned int item = 0; item < nStyles; item++) {
        KoXmlElement *styleElem = customStyles[item];
        if (!styleElem)
            continue;

        // assume the name assigned by the application
        const QString oasisName = styleElem->attributeNS(KoXmlNS::style, "name", QString());

        // then replace by user-visible one (if any)
        const QString name = styleElem->attributeNS(KoXmlNS::style, "display-name", oasisName);
        debugSheetsODF << " StyleManager: Loading common cell style:" << oasisName << " (display name:" << name << ")";

        if (!name.isEmpty()) {
            // The style's parent name will be set in loadStyle(..).
            // After all styles are loaded the pointer to the parent is set.
            CustomStyle *style = new CustomStyle(name);

            Conditions conditions;
            loadCustomStyle(style, stylesReader, *styleElem, name, conditions, styles, map->calculationSettings()->locale());
            // TODO Stefan: conditions
            styles->insertStyle(style);
            // insert it into the map sorted the OpenDocument name
            styles->defineOasisStyle(oasisName, style->name()); // TODO: is this actually needed?
            oasisStyles[oasisName] = style->name();
            debugSheetsODF << "Style" << style->name() << ":" << style;
        }
    }

    // replace all OpenDocument internal parent names by Calligra Sheets' style names
    QStringList styleNames = styles->styleNames(false);

    foreach (QString name, styleNames) {
        CustomStyle *style = styles->style(name);

        if (!style->parentName().isNull()) {
            const QString parentOdfName = style->parentName();
            const CustomStyle *parentStyle = styles->style(oasisStyles.value(parentOdfName));
            if (!parentStyle) {
                warnSheetsODF << parentOdfName << " not found.";
                continue;
            }
            style->setParentName(oasisStyles.value(parentOdfName));
            debugSheetsODF << style->name() << " (" << style << ") gets" << style->parentName() << " (" << parentOdfName << ") as parent.";
        } else {
            style->setParentName("Default");
            debugSheetsODF << style->name() << " (" << style << ") has" << style->parentName() << " as parent.";
        }
    }
}

Styles Odf::loadAutoStyles(StyleManager *styles, KoOdfStylesReader &stylesReader, QHash<QString, Conditions> &conditionalStyles, const Localization *locale)
{
    Styles autoStyles;
    foreach (KoXmlElement *element, stylesReader.autoStyles("table-cell")) {
        if (element->hasAttributeNS(KoXmlNS::style, "name")) {
            QString name = element->attributeNS(KoXmlNS::style, "name", QString());
            debugSheetsODF << "StyleManager: Preloading automatic cell style:" << name;
            autoStyles.remove(name);
            Conditions conditions;
            loadStyle(&autoStyles[name], stylesReader, *(element), conditions, styles, locale);
            if (!conditions.isEmpty()) {
                debugSheets << "\t\tCONDITIONS";
                conditionalStyles[name] = conditions;
            }

            if (element->hasAttributeNS(KoXmlNS::style, "parent-style-name")) {
                const QString parentOdfName = element->attributeNS(KoXmlNS::style, "parent-style-name", QString());
                const CustomStyle *parentStyle = styles->style(styles->openDocumentName(parentOdfName));
                if (!parentStyle) {
                    warnSheetsODF << parentOdfName << " not found.";
                    continue;
                }
                autoStyles[name].setParentName(parentStyle->name());
                debugSheetsODF << "\t parent-style-name:" << autoStyles[name].parentName();
            }
        }
    }
    return autoStyles;
}

void Odf::saveStyles(StyleManager *manager, KoGenStyles &mainStyles, Localization *locale)
{
    debugSheetsODF << "StyleManager: Saving default cell style";
    KoGenStyle defStyle = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
    saveStyle(manager->defaultStyle(), defStyle, mainStyles, manager, locale);

    manager->clearOasisStyles();
    QStringList styleNames = manager->styleNames(false);

    foreach (QString name, styleNames) {
        CustomStyle *style = manager->style(name);
        debugSheetsODF << "StyleManager: Saving common cell style" << name;
        KoGenStyle customStyle = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
        const QString oasisName = saveCustomStyle(style, customStyle, mainStyles, manager, locale);
        manager->defineOasisStyle(style->name(), oasisName);
    }
}

// Single style loading

void Odf::loadStyle(Style *style,
                    KoOdfStylesReader &stylesReader,
                    const KoXmlElement &element,
                    Conditions &conditions,
                    const StyleManager *styleManager,
                    const Localization *locale)
{
    // NOTE Stefan: Do not fill the style stack with the parent styles!
    KoStyleStack styleStack;
    styleStack.push(element);

    styleStack.setTypeProperties("table-cell");
    loadTableCellProperties(style, stylesReader, styleStack);

    styleStack.setTypeProperties("text");
    loadTextProperties(style, stylesReader, styleStack);

    styleStack.setTypeProperties("paragraph");
    loadParagraphProperties(style, stylesReader, styleStack);

    KoXmlElement e;
    forEachElement(e, element)
    {
        if (e.namespaceURI() == KoXmlNS::style && e.localName() == "map")
            loadConditions(&conditions, e, styleManager);
    }

    loadDataStyle(style, stylesReader, element, conditions, styleManager, locale);
}

void Odf::loadCustomStyle(CustomStyle *style,
                          KoOdfStylesReader &stylesReader,
                          const KoXmlElement &xmlstyle,
                          const QString &name,
                          Conditions &conditions,
                          const StyleManager *styleManager,
                          const Localization *locale)
{
    style->setName(name);
    if (xmlstyle.hasAttributeNS(KoXmlNS::style, "parent-style-name"))
        style->setParentName(xmlstyle.attributeNS(KoXmlNS::style, "parent-style-name", QString()));

    style->setType(Style::CUSTOM);

    loadStyle((Style *)style, stylesReader, xmlstyle, conditions, styleManager, locale);
}

void Odf::loadDataStyle(Style *style,
                        KoOdfStylesReader &stylesReader,
                        const KoXmlElement &element,
                        Conditions &conditions,
                        const StyleManager *styleManager,
                        const Localization *locale)
{
    if (element.hasAttributeNS(KoXmlNS::style, "data-style-name")) {
        const QString styleName = element.attributeNS(KoXmlNS::style, "data-style-name", QString());
        loadDataStyle(style, stylesReader, styleName, conditions, styleManager, locale);
    }
}

void Odf::loadDataStyle(Style *style,
                        KoOdfStylesReader &stylesReader,
                        const QString &styleName,
                        Conditions &conditions,
                        const StyleManager *styleManager,
                        const Localization *locale)
{
    if (!stylesReader.dataFormats().contains(styleName))
        return;

    QPair<KoOdfNumberStyles::NumericStyleFormat, KoXmlElement *> dataStylePair = stylesReader.dataFormats()[styleName];

    const KoOdfNumberStyles::NumericStyleFormat &dataStyle = dataStylePair.first;
    const QList<QPair<QString, QString>> styleMaps = dataStyle.styleMaps;
    bool useNewStyle = (styleMaps.count() > 0);
    if (useNewStyle) {
        style = new Style();
        for (QList<QPair<QString, QString>>::const_iterator it = styleMaps.begin(); it != styleMaps.end(); ++it) {
            const Conditional c = loadCondition(&conditions, it->first, it->second, QString());
            if (styleManager->style(c.styleName) == nullptr) {
                CustomStyle *s = new CustomStyle(c.styleName);
                loadDataStyle(s, stylesReader, c.styleName, conditions, styleManager, locale);
                const_cast<StyleManager *>(styleManager)->insertStyle(s);
            }
        }
    }

    KoStyleStack styleStack;
    styleStack.push(*dataStylePair.second);
    styleStack.setTypeProperties("text");
    loadTextProperties(style, stylesReader, styleStack);

    QString tmp = dataStyle.prefix;
    if (!tmp.isEmpty()) {
        style->setPrefix(tmp);
    }
    tmp = dataStyle.suffix;
    if (!tmp.isEmpty()) {
        style->setPostfix(tmp);
    }
    // determine data formatting
    switch (dataStyle.type) {
    case KoOdfNumberStyles::Number:
        style->setFormatType(Format::Number);
        if (!dataStyle.currencySymbol.isEmpty())
            style->setCurrency(numberCurrency(dataStyle.currencySymbol));
        else
            style->setCurrency(numberCurrency(dataStyle.formatStr));
        break;
    case KoOdfNumberStyles::Scientific:
        style->setFormatType(Format::Scientific);
        break;
    case KoOdfNumberStyles::Currency:
        debugSheetsODF << " currency-symbol:" << dataStyle.currencySymbol;
        if (!dataStyle.currencySymbol.isEmpty())
            style->setCurrency(numberCurrency(dataStyle.currencySymbol));
        else
            style->setCurrency(numberCurrency(dataStyle.formatStr));
        break;
    case KoOdfNumberStyles::Percentage:
        style->setFormatType(Format::Percentage);
        break;
    case KoOdfNumberStyles::Fraction:
        // determine format of fractions, dates and times by using the
        // formatting string
        tmp = dataStyle.formatStr;
        if (!tmp.isEmpty()) {
            style->setFormatType(fractionType(tmp));
        }
        break;
    case KoOdfNumberStyles::Date:
        // determine format of fractions, dates and times by using the
        // formatting string
        tmp = dataStyle.formatStr;
        if (!tmp.isEmpty()) {
            style->setFormatType(dateType(tmp, locale));
        }
        break;
    case KoOdfNumberStyles::Time:
        // determine format of fractions, dates and times by using the
        // formatting string
        tmp = dataStyle.formatStr;
        if (!tmp.isEmpty()) {
            style->setFormatType(timeType(tmp, locale));
        }
        break;
    case KoOdfNumberStyles::Boolean:
        style->setFormatType(Format::Number);
        break;
    case KoOdfNumberStyles::Text:
        style->setFormatType(Format::Text);
        break;
    }

    if (dataStyle.precision > -1) {
        // special handling for precision
        // The Style default (-1) and the storage default (0) differ.
        // The maximum is 10. Replace the Style value 0 with -11, which always results
        // in a storage value < 0 and is interpreted as Style value 0.
        int precision = dataStyle.precision;
        if (style->type() == Style::AUTO && precision == 0)
            precision = -11;
        style->setPrecision(precision);
    }

    style->setThousandsSep(dataStyle.thousandsSep);

    style->setCustomFormat(dataStyle.formatStr);

    if (useNewStyle) {
        conditions.setDefaultStyle(*style);
        delete style;
    }
}

void Odf::loadParagraphProperties(Style *style, KoOdfStylesReader &stylesReader, const KoStyleStack &styleStack)
{
    Q_UNUSED(stylesReader);
    debugSheetsODF << "\t paragraph-properties";
    if (styleStack.hasProperty(KoXmlNS::fo, "text-align")) {
        QString str = styleStack.property(KoXmlNS::fo, "text-align");
        if (str == "center")
            style->setHAlign(Style::Center);
        else if (str == "end" || str == "right")
            style->setHAlign(Style::Right);
        else if (str == "start" || str == "left")
            style->setHAlign(Style::Left);
        else if (str == "justify")
            style->setHAlign(Style::Justified);
        else
            style->setHAlign(Style::HAlignUndefined);
        debugSheetsODF << "\t\t text-align:" << str;
    }

    if (styleStack.hasProperty(KoXmlNS::fo, "margin-left")) {
        // todo fix me
        style->setIndentation(KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-left"), 0.0));
    }
}

void Odf::loadTableCellProperties(Style *style, KoOdfStylesReader &stylesReader, const KoStyleStack &styleStack)
{
    QString str;
    if (styleStack.hasProperty(KoXmlNS::style, "vertical-align")) {
        str = styleStack.property(KoXmlNS::style, "vertical-align");
        if (str == "bottom")
            style->setVAlign(Style::Bottom);
        else if (str == "top")
            style->setVAlign(Style::Top);
        else if (str == "middle")
            style->setVAlign(Style::Middle);
        else
            style->setVAlign(Style::VAlignUndefined);
    }
    if (styleStack.property(KoXmlNS::calligra, "vertical-distributed") == "distributed") {
        if (style->valign() == Style::Top)
            style->setVAlign(Style::VJustified);
        else
            style->setVAlign(Style::VDistributed);
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "background-color")) {
        str = styleStack.property(KoXmlNS::fo, "background-color");
        if (str == "transparent") {
            debugSheetsODF << "\t\t fo:background-color: transparent";
            style->setBackgroundColor(QColor());
        } else {
            QColor color(str);
            if (color.isValid()) {
                debugSheetsODF << "\t\t fo:background-color:" << color.name();
                style->setBackgroundColor(color);
            }
        }
    }

    if (styleStack.hasProperty(KoXmlNS::fo, "wrap-option") && (styleStack.property(KoXmlNS::fo, "wrap-option") == "wrap")) {
        style->setWrapText(true);
    }
    if (styleStack.hasProperty(KoXmlNS::style, "cell-protect")) {
        str = styleStack.property(KoXmlNS::style, "cell-protect");
#ifndef NWORKAROUND_ODF_BUGS
        KoOdfWorkaround::fixBadFormulaHiddenForStyleCellProtect(str);
#endif
        if (str == "none")
            style->setNotProtected(true);
        else if (str == "hidden-and-protected")
            style->setHideAll(true);
        else if (str == "protected formula-hidden" || str == "formula-hidden protected")
            style->setHideFormula(true);
        else if (str == "formula-hidden") {
            style->setNotProtected(true);
            style->setHideFormula(true);
        }
    }
    if (styleStack.hasProperty(KoXmlNS::style, "print-content") && (styleStack.property(KoXmlNS::style, "print-content") == "false")) {
        style->setDontPrintText(true);
    }
    if (styleStack.hasProperty(KoXmlNS::style, "shrink-to-fit") && (styleStack.property(KoXmlNS::style, "shrink-to-fit") == "true")) {
        style->setShrinkToFit(true);
    }
    if (styleStack.hasProperty(KoXmlNS::style, "direction") && (styleStack.property(KoXmlNS::style, "direction") == "ttb")) {
        style->setVerticalText(true);
    }
    if (styleStack.hasProperty(KoXmlNS::style, "rotation-angle")) {
        bool ok;
        int a = styleStack.property(KoXmlNS::style, "rotation-angle").toInt(&ok);
        debugSheetsODF << " rotation-angle :" << a;
        if (a != 0) {
            style->setAngle(-a);
        }
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "border")) {
        str = styleStack.property(KoXmlNS::fo, "border");
        QPen pen = decodePen(str);
        style->setLeftBorderPen(pen);
        style->setTopBorderPen(pen);
        style->setBottomBorderPen(pen);
        style->setRightBorderPen(pen);
        debugSheetsODF << "\t\tfo:border" << str;
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "border-left")) {
        str = styleStack.property(KoXmlNS::fo, "border-left");
        style->setLeftBorderPen(decodePen(str));
        debugSheetsODF << "\t\tfo:border-left" << str;
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "border-right")) {
        str = styleStack.property(KoXmlNS::fo, "border-right");
        style->setRightBorderPen(decodePen(str));
        debugSheetsODF << "\t\tfo:border-right" << str;
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "border-top")) {
        str = styleStack.property(KoXmlNS::fo, "border-top");
        style->setTopBorderPen(decodePen(str));
        debugSheetsODF << "\t\tfo:border-top" << str;
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "border-bottom")) {
        str = styleStack.property(KoXmlNS::fo, "border-bottom");
        style->setBottomBorderPen(decodePen(str));
        debugSheetsODF << "\t\tfo:border-bottom" << str;
    }
    if (styleStack.hasProperty(KoXmlNS::style, "diagonal-tl-br")) {
        str = styleStack.property(KoXmlNS::style, "diagonal-tl-br");
        style->setFallDiagonalPen(decodePen(str));
        debugSheetsODF << "\t\tfo:diagonal-tl-br" << str;
    }
    if (styleStack.hasProperty(KoXmlNS::style, "diagonal-bl-tr")) {
        str = styleStack.property(KoXmlNS::style, "diagonal-bl-tr");
        style->setGoUpDiagonalPen(decodePen(str));
        debugSheetsODF << "\t\tfo:diagonal-bl-tr" << str;
    }

    if (styleStack.hasProperty(KoXmlNS::draw, "style-name") || styleStack.hasProperty(KoXmlNS::calligra, "fill-style-name")) {
        QString styleName = styleStack.hasProperty(KoXmlNS::calligra, "fill-style-name") ? styleStack.property(KoXmlNS::calligra, "fill-style-name")
                                                                                         : styleStack.property(KoXmlNS::draw, "style-name");
        debugSheetsODF << " style name :" << styleName;

        const KoXmlElement *xmlstyle = stylesReader.findStyle(styleName, "graphic");
        debugSheetsODF << " style :" << style;
        if (xmlstyle) {
            KoStyleStack drawStyleStack;
            drawStyleStack.push(*xmlstyle);
            drawStyleStack.setTypeProperties("graphic");
            if (drawStyleStack.hasProperty(KoXmlNS::draw, "fill")) {
                const QString fill = drawStyleStack.property(KoXmlNS::draw, "fill");
                debugSheetsODF << " load object gradient fill type :" << fill;

                if (fill == "solid" || fill == "hatch") {
                    debugSheetsODF << " Style ******************************************************";
                    style->setBackgroundBrush(KoOdfGraphicStyles::loadOdfFillStyle(drawStyleStack, fill, stylesReader));

                } else
                    debugSheetsODF << " fill style not supported into sheets :" << fill;
            }
        }
    }
}

void Odf::loadTextProperties(Style *style, KoOdfStylesReader &stylesReader, const KoStyleStack &styleStack)
{
    Q_UNUSED(stylesReader);
    // fo:font-size="13pt"
    // fo:font-style="italic"
    // style:text-underline="double"
    // style:text-underline-color="font-color"
    // fo:font-weight="bold"
    debugSheetsODF << "\t text-properties";
    if (styleStack.hasProperty(KoXmlNS::fo, "font-family")) {
        style->setFontFamily(styleStack.property(KoXmlNS::fo, "font-family")); // FIXME Stefan: sanity check
        debugSheetsODF << "\t\t fo:font-family:" << style->fontFamily();
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "font-size")) {
        style->setFontSize((int)KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "font-size"), 10.0)); // FIXME Stefan: fallback to default
        debugSheetsODF << "\t\t fo:font-size:" << style->fontSize();
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "font-style")) {
        if (styleStack.property(KoXmlNS::fo, "font-style") == "italic") { // "normal", "oblique"
            style->setFontItalic(true);
            debugSheetsODF << "\t\t fo:font-style:"
                           << "italic";
        }
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "font-weight")) {
        if (styleStack.property(KoXmlNS::fo, "font-weight") == "bold") { // "normal", "100", "200", ...
            style->setFontBold(true);
            debugSheetsODF << "\t\t fo:font-weight:"
                           << "bold";
        }
    }
    if (styleStack.hasProperty(KoXmlNS::style, "text-underline-style")) {
        if (styleStack.property(KoXmlNS::style, "text-underline-style") != "none") {
            style->setFontUnderline(true);
            debugSheetsODF << "\t\t style:text-underline-style:"
                           << "solid (actually: !none)";
        }
    }
    if (styleStack.hasProperty(KoXmlNS::style, "text-underline-width")) {
        // TODO
    }
    if (styleStack.hasProperty(KoXmlNS::style, "text-underline-color")) {
        // TODO
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "color")) {
        QColor color = QColor(styleStack.property(KoXmlNS::fo, "color"));
        if (color.isValid()) {
            style->setFontColor(color);
            debugSheetsODF << "\t\t fo:color:" << color.name();
        }
    }
    if (styleStack.hasProperty(KoXmlNS::style, "text-line-through-style")) {
        if (styleStack.property(KoXmlNS::style, "text-line-through-style") != "none"
            /*&& styleStack.property("text-line-through-style")=="solid"*/) {
            style->setFontStrikeOut(true);
            debugSheetsODF << "\t\t text-line-through-style:"
                           << "solid (actually: !none)";
        }
    }
    if (styleStack.hasProperty(KoXmlNS::style, "font-name")) {
        QString fontName = styleStack.property(KoXmlNS::style, "font-name");
        debugSheetsODF << "\t\t style:font-name:" << fontName;
        const KoXmlElement *xmlstyle = stylesReader.findStyle(fontName);
        // TODO: sanity check that it is a font-face style?
        debugSheetsODF << "\t\t\t style:" << xmlstyle;
        if (xmlstyle) {
            style->setFontFamily(xmlstyle->attributeNS(KoXmlNS::svg, "font-family"));
            debugSheetsODF << "\t\t\t svg:font-family:" << style->fontFamily();
        }
    }
}

// Single style saving

void Odf::saveStyle(const Style *style,
                    const QSet<Style::Key> &keysToStore,
                    KoGenStyle &xmlstyle,
                    KoGenStyles &mainStyles,
                    const StyleManager *manager,
                    Localization *locale)
{
#ifndef NDEBUG
    // if (type() == BUILTIN )
    //   debugSheetsStyle <<"BUILTIN";
    // else if (type() == CUSTOM )
    //   debugSheetsStyle <<"CUSTOM";
    // else if (type() == AUTO )
    //   debugSheetsStyle <<"AUTO";
#endif

    if (!style->isDefault() && style->hasAttribute(Style::NamedStyleKey)) {
        const QString parentName = manager->openDocumentName(style->parentName());
        if (!parentName.isEmpty())
            xmlstyle.addAttribute("style:parent-style-name", parentName);
    }

    if (keysToStore.contains(Style::HorizontalAlignment)) {
        QString value;
        switch (style->halign()) {
        case Style::Center:
            value = "center";
            break;
        case Style::Right:
            value = "end";
            break;
        case Style::Left:
            value = "start";
            break;
        case Style::Justified:
            value = "justify";
            break;
        case Style::HAlignUndefined:
            break;
        }
        if (!value.isEmpty()) {
            xmlstyle.addProperty("style:text-align-source", "fix"); // table-cell-properties
            xmlstyle.addProperty("fo:text-align", value, KoGenStyle::ParagraphType);
        }
    }

    if (keysToStore.contains(Style::VerticalAlignment)) {
        QString value;
        switch (style->valign()) {
        case Style::Top:
        case Style::VJustified:
            value = "top";
            break;
        case Style::Middle:
        case Style::VDistributed:
            value = "middle";
            break;
        case Style::Bottom:
            value = "bottom";
            break;
        case Style::VAlignUndefined:
        default:
            break;
        }
        if (!value.isEmpty()) // sanity
            xmlstyle.addProperty("style:vertical-align", value);

        if (style->valign() == Style::VJustified || style->valign() == Style::VDistributed)
            xmlstyle.addProperty("calligra:vertical-distributed", "distributed");
    }

    if (keysToStore.contains(Style::BackgroundColor) && style->backgroundColor().isValid())
        xmlstyle.addProperty("fo:background-color", colorName(style->backgroundColor()));

    if (keysToStore.contains(Style::MultiRow) && style->wrapText())
        xmlstyle.addProperty("fo:wrap-option", "wrap");

    if (keysToStore.contains(Style::VerticalText) && style->verticalText()) {
        xmlstyle.addProperty("style:direction", "ttb");
        xmlstyle.addProperty("style:rotation-angle", "0");
        xmlstyle.addProperty("style:rotation-align", "none");
    }

    if (keysToStore.contains(Style::ShrinkToFit) && style->shrinkToFit())
        xmlstyle.addProperty("style:shrink-to-fit", "true");

#if 0
    if (keysToStore.contains(Style::FloatFormat))
        format.setAttribute("float", QString::number((int) style->floatFormat()));

    if (keysToStore.contains(Style::FloatColor))
        format.setAttribute("floatcolor", QString::number((int)style->floatColor()));

    if (keysToStore.contains(Style::CustomFormat) && !style->customFormat().isEmpty())
        format.setAttribute("custom", customFormat());

    if (keysToStore.contains(Style::Format::Type) && style->formatType() == Money) {
        format.setAttribute("type", QString::number((int) currency().type));
        format.setAttribute("symbol", currency().symbol);
    }
#endif
    if (keysToStore.contains(Style::Angle) && style->angle() != 0) {
        xmlstyle.addProperty("style:rotation-align", "none");
        xmlstyle.addProperty("style:rotation-angle", QString::number(-1.0 * style->angle()));
    }

    if (keysToStore.contains(Style::Indentation) && style->indentation() != 0.0) {
        xmlstyle.addPropertyPt("fo:margin-left", style->indentation(), KoGenStyle::ParagraphType);
        // FIXME
        // if ( a == HAlignUndefined )
        // currentCellStyle.addProperty("fo:text-align", "start" );
    }

    if (keysToStore.contains(Style::DontPrintText) && keysToStore.contains(Style::DontPrintText)) // huh? why twice?
        xmlstyle.addProperty("style:print-content", "false");

    // protection
    bool hideAll = false;
    bool hideFormula = false;
    bool isNotProtected = false;

    if (keysToStore.contains(Style::NotProtected))
        isNotProtected = style->notProtected();

    if (keysToStore.contains(Style::HideAll))
        hideAll = style->hideAll();

    if (keysToStore.contains(Style::HideFormula))
        hideFormula = style->hideFormula();

    if (hideAll)
        xmlstyle.addProperty("style:cell-protect", "hidden-and-protected");
    else {
        if (isNotProtected && !hideFormula)
            xmlstyle.addProperty("style:cell-protect", "none");
        else if (isNotProtected && hideFormula)
            xmlstyle.addProperty("style:cell-protect", "formula-hidden");
        else if (hideFormula)
            xmlstyle.addProperty("style:cell-protect", "protected formula-hidden");
        else if (keysToStore.contains(Style::NotProtected) && !isNotProtected)
            // write out, only if it is explicitly set
            xmlstyle.addProperty("style:cell-protect", "protected");
    }

    // borders
    // NOTE Stefan: QPen api docs:
    //              A line width of zero indicates a cosmetic pen. This means
    //              that the pen width is always drawn one pixel wide,
    //              independent of the transformation set on the painter.
    if (keysToStore.contains(Style::LeftPen) && keysToStore.contains(Style::RightPen) && keysToStore.contains(Style::TopPen)
        && keysToStore.contains(Style::BottomPen) && (style->leftBorderPen() == style->topBorderPen()) && (style->leftBorderPen() == style->rightBorderPen())
        && (style->leftBorderPen() == style->bottomBorderPen())) {
        if (style->leftBorderPen().style() != Qt::NoPen)
            xmlstyle.addProperty("fo:border", encodePen(style->leftBorderPen()));
    } else {
        if (keysToStore.contains(Style::LeftPen) && (style->leftBorderPen().style() != Qt::NoPen))
            xmlstyle.addProperty("fo:border-left", encodePen(style->leftBorderPen()));

        if (keysToStore.contains(Style::RightPen) && (style->rightBorderPen().style() != Qt::NoPen))
            xmlstyle.addProperty("fo:border-right", encodePen(style->rightBorderPen()));

        if (keysToStore.contains(Style::TopPen) && (style->topBorderPen().style() != Qt::NoPen))
            xmlstyle.addProperty("fo:border-top", encodePen(style->topBorderPen()));

        if (keysToStore.contains(Style::BottomPen) && (style->bottomBorderPen().style() != Qt::NoPen))
            xmlstyle.addProperty("fo:border-bottom", encodePen(style->bottomBorderPen()));
    }
    if (keysToStore.contains(Style::FallDiagonalPen) && (style->fallDiagonalPen().style() != Qt::NoPen)) {
        xmlstyle.addProperty("style:diagonal-tl-br", encodePen(style->fallDiagonalPen()));
    }
    if (keysToStore.contains(Style::GoUpDiagonalPen) && (style->goUpDiagonalPen().style() != Qt::NoPen)) {
        xmlstyle.addProperty("style:diagonal-bl-tr", encodePen(style->goUpDiagonalPen()));
    }

    // font
    if (keysToStore.contains(Style::FontFamily)) { // !fontFamily().isEmpty() == true
        xmlstyle.addProperty("fo:font-family", style->fontFamily(), KoGenStyle::TextType);
    }
    if (keysToStore.contains(Style::FontSize)) { // fontSize() != 0
        xmlstyle.addPropertyPt("fo:font-size", style->fontSize(), KoGenStyle::TextType);
    }

    if (keysToStore.contains(Style::FontBold) && style->bold())
        xmlstyle.addProperty("fo:font-weight", "bold", KoGenStyle::TextType);

    if (keysToStore.contains(Style::FontItalic) && style->italic())
        xmlstyle.addProperty("fo:font-style", "italic", KoGenStyle::TextType);

    if (keysToStore.contains(Style::FontUnderline) && style->underline()) {
        // style:text-underline-style="solid" style:text-underline-width="auto"
        xmlstyle.addProperty("style:text-underline-style", "solid", KoGenStyle::TextType);
        // copy from oo-129
        xmlstyle.addProperty("style:text-underline-width", "auto", KoGenStyle::TextType);
        xmlstyle.addProperty("style:text-underline-color", "font-color", KoGenStyle::TextType);
    }

    if (keysToStore.contains(Style::FontStrike) && style->strikeOut())
        xmlstyle.addProperty("style:text-line-through-style", "solid", KoGenStyle::TextType);

    if (keysToStore.contains(Style::FontColor) && style->fontColor().isValid()) { // always save
        xmlstyle.addProperty("fo:color", colorName(style->fontColor()), KoGenStyle::TextType);
    }

    // I don't think there is a reason why the background brush should be saved if it is null,
    // but remove the check if it causes problems.  -- Robert Knight <robertknight@gmail.com>
    if (keysToStore.contains(Style::BackgroundBrush) && (style->backgroundBrush().style() != Qt::NoBrush)) {
        QString tmp = saveBackgroundStyle(mainStyles, style->backgroundBrush());
        if (!tmp.isEmpty())
            xmlstyle.addProperty("calligra:fill-style-name", tmp);
    }

    QString _prefix;
    QString _postfix;
    int _precision = -1;
    if (keysToStore.contains(Style::Prefix) && !style->prefix().isEmpty())
        _prefix = style->prefix();
    if (keysToStore.contains(Style::Postfix) && !style->postfix().isEmpty())
        _postfix = style->postfix();
    if (keysToStore.contains(Style::Precision) && style->precision() != -1)
        _precision = style->precision();
    bool _thousandsSep = false;
    if (keysToStore.contains(Style::ThousandsSep)) {
        _thousandsSep = style->thousandsSep();
    }

    QString currencyCode;
    if (keysToStore.contains(Style::FormatTypeKey) && style->formatType() == Format::Money) {
        currencyCode = style->currency().code();
    }

    QString numericStyle = saveStyleNumeric(xmlstyle, mainStyles, style->formatType(), _prefix, _postfix, _precision, currencyCode, _thousandsSep, locale);
    if (!numericStyle.isEmpty())
        xmlstyle.addAttribute("style:data-style-name", numericStyle);
}

QString Odf::saveCustomStyle(CustomStyle *style, KoGenStyle &genstyle, KoGenStyles &mainStyles, const StyleManager *manager, Localization *locale)
{
    Q_ASSERT(!style->name().isEmpty());
    // default style does not need display name
    if (!style->isDefault())
        genstyle.addAttribute("style:display-name", style->name());

    // doing the real work
    QSet<Style::Key> keysToStore = style->definedKeys(manager);
    saveStyle(style, keysToStore, genstyle, mainStyles, manager, locale);

    if (style->isDefault()) {
        genstyle.setDefaultStyle(true);
        // don't i18n'ize "Default" in this case
        return mainStyles.insert(genstyle, "Default", KoGenStyles::DontAddNumberToName);
    }

    // this is a custom style
    return mainStyles.insert(genstyle, "custom-style");
}

QString Odf::saveStyle(const Style *style, KoGenStyle &xmlstyle, KoGenStyles &mainStyles, const StyleManager *manager, Localization *locale)
{
    // list of substyles to store
    QSet<Style::Key> keysToStore = style->definedKeys(manager);

    if (style->isDefault()) {
        if (xmlstyle.isEmpty()) {
            xmlstyle = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
            xmlstyle.setDefaultStyle(true);
            // don't i18n'ize "Default" in this case
            return "Default"; // mainStyles.insert( style, "Default", KoGenStyles::DontAddNumberToName );
        }
        // no attributes to store here
        return mainStyles.insert(xmlstyle, "ce");
    } else if (style->hasAttribute(Style::NamedStyleKey)) {
        // it's not really the parent name in this case

        // no differences and not an automatic style yet?
        if (xmlstyle.isEmpty() && (keysToStore.count() == 0 || (keysToStore.count() == 1 && *keysToStore.constBegin() == Style::NamedStyleKey))) {
            return manager->openDocumentName(style->parentName());
        }
    }

    // Calligra::Sheets::Style is definitely an OASIS auto style,
    // but don't overwrite it, if it already exists
    if (xmlstyle.isEmpty())
        xmlstyle = KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");

    // doing the real work
    saveStyle(style, keysToStore, xmlstyle, mainStyles, manager, locale);
    return mainStyles.insert(xmlstyle, "ce");
}

QString Odf::saveBackgroundStyle(KoGenStyles &mainStyles, const QBrush &brush)
{
    KoGenStyle styleobjectauto = KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic");
    KoOdfGraphicStyles::saveOdfFillStyle(styleobjectauto, mainStyles, brush);
    return mainStyles.insert(styleobjectauto, "gr");
}

QString Odf::saveStyleNumeric(KoGenStyle &style,
                              KoGenStyles &mainStyles,
                              Format::Type _style,
                              const QString &_prefix,
                              const QString &_postfix,
                              int _precision,
                              const QString &symbol,
                              bool thousandsSep,
                              Localization *locale)
{
    //  debugSheetsODF ;
    QString styleName;
    QString valueType;
    switch (_style) {
    case Format::Number:
        styleName = saveStyleNumericNumber(mainStyles, _style, _precision, _prefix, _postfix, thousandsSep);
        valueType = "float";
        break;
    case Format::Text:
        styleName = saveStyleNumericText(mainStyles, _style, _precision, _prefix, _postfix);
        valueType = "string";
        break;
    case Format::Money:
        styleName = saveStyleNumericMoney(mainStyles, _style, symbol, _precision, _prefix, _postfix);
        valueType = "currency";
        break;
    case Format::Percentage:
        styleName = saveStyleNumericPercentage(mainStyles, _style, _precision, _prefix, _postfix);
        valueType = "percentage";
        break;
    case Format::Scientific:
        styleName = saveStyleNumericScientific(mainStyles, _style, _prefix, _postfix, _precision, thousandsSep);
        valueType = "float";
        break;
    case Format::fraction_half:
    case Format::fraction_quarter:
    case Format::fraction_eighth:
    case Format::fraction_sixteenth:
    case Format::fraction_tenth:
    case Format::fraction_hundredth:
    case Format::fraction_one_digit:
    case Format::fraction_two_digits:
    case Format::fraction_three_digits:
        styleName = saveStyleNumericFraction(mainStyles, _style, _prefix, _postfix);
        valueType = "float";
        break;
    case Format::Custom:
        styleName = saveStyleNumericCustom(mainStyles, _style, _prefix, _postfix);
        break;
    case Format::Generic:
    case Format::None:
        if (_precision > -1 || !_prefix.isEmpty() || !_postfix.isEmpty()) {
            styleName = saveStyleNumericNumber(mainStyles, _style, _precision, _prefix, _postfix, thousandsSep);
            valueType = "float";
        }
        break;
    default:
        if (Format::isDate(_style) || Format::isDateTime(_style)) {
            styleName = saveStyleNumericDate(mainStyles, _style, _prefix, _postfix, locale);
            valueType = "date";
        } else if (Format::isTime(_style)) {
            styleName = saveStyleNumericTime(mainStyles, _style, _prefix, _postfix, locale);
            valueType = "time";
        }
        break;
    }
    if (!styleName.isEmpty()) {
        style.addAttribute("style:data-style-name", styleName);
    }
    return styleName;
}

QString Odf::saveStyleNumericNumber(KoGenStyles &mainStyles,
                                    Format::Type /*_style*/,
                                    int _precision,
                                    const QString &_prefix,
                                    const QString &_postfix,
                                    bool thousandsSep)
{
    QString format;
    if (_precision == -1)
        format = '0';
    else {
        QString tmp;
        for (int i = 0; i < _precision; i++) {
            tmp += '0';
        }
        format = "0." + tmp;
    }
    return KoOdfNumberStyles::saveOdfNumberStyle(mainStyles, format, _prefix, _postfix, thousandsSep);
}

QString
Odf::saveStyleNumericText(KoGenStyles & /*mainStyles*/, Format::Type /*_style*/, int /*_precision*/, const QString & /*_prefix*/, const QString & /*_postfix*/)
{
    return "";
}

QString Odf::saveStyleNumericMoney(KoGenStyles &mainStyles,
                                   Format::Type /*_style*/,
                                   const QString &symbol,
                                   int _precision,
                                   const QString &_prefix,
                                   const QString &_postfix)
{
    QString format;
    if (_precision == -1)
        format = '0';
    else {
        QString tmp;
        for (int i = 0; i < _precision; i++) {
            tmp += '0';
        }
        format = "0." + tmp;
    }
    return KoOdfNumberStyles::saveOdfCurrencyStyle(mainStyles, format, symbol, _prefix, _postfix);
}

QString Odf::saveStyleNumericPercentage(KoGenStyles &mainStyles, Format::Type /*_style*/, int _precision, const QString &_prefix, const QString &_postfix)
{
    //<number:percentage-style style:name="N106" style:family="data-style">
    //<number:number number:decimal-places="6" number:min-integer-digits="1"/>
    //<number:text>%</number:text>
    //</number:percentage-style>
    // TODO add decimal etc.
    QString format;
    if (_precision == -1)
        format = '0';
    else {
        QString tmp;
        for (int i = 0; i < _precision; i++) {
            tmp += '0';
        }
        format = "0." + tmp;
    }
    return KoOdfNumberStyles::saveOdfPercentageStyle(mainStyles, format, _prefix, _postfix);
}

QString Odf::saveStyleNumericScientific(KoGenStyles &mainStyles,
                                        Format::Type /*_style*/,
                                        const QString &_prefix,
                                        const QString &_suffix,
                                        int _precision,
                                        bool thousandsSep)
{
    //<number:number-style style:name="N60" style:family="data-style">
    //  <number:scientific-number number:decimal-places="2" number:min-integer-digits="1" number:min-exponent-digits="3"/>
    //</number:number-style>
    QString format;
    if (_precision == -1)
        format = "0E+00";
    else {
        QString tmp;
        for (int i = 0; i < _precision; i++) {
            tmp += '0';
        }
        format = "0." + tmp + "E+00";
    }
    return KoOdfNumberStyles::saveOdfScientificStyle(mainStyles, format, _prefix, _suffix, thousandsSep);
}

QString Odf::saveStyleNumericDate(KoGenStyles &mainStyles, Format::Type _style, const QString &_prefix, const QString &_postfix, Localization *locale)
{
    QString format;
    if (Format::isDate(_style)) {
        format = locale->dateFormat(_style);
    } else if (Format::isDateTime(_style)) {
        format = locale->dateTimeFormat(_style);
    } else {
        warnSheetsODF << "this date format is not defined ! :" << _style;
    }
    return KoOdfNumberStyles::saveOdfDateStyle(mainStyles, format, false, _prefix, _postfix);
}

QString Odf::saveStyleNumericCustom(KoGenStyles & /*mainStyles*/, Format::Type /*_style*/, const QString & /*_prefix*/, const QString & /*_postfix*/)
{
    // TODO
    //<number:date-style style:name="N50" style:family="data-style" number:automatic-order="true" number:format-source="language">
    //<number:month/>
    //<number:text>/</number:text>
    //<number:day/>
    //<number:text>/</number:text>
    //<number:year/>
    //<number:text> </number:text>
    //<number:hours number:style="long"/>
    //<number:text>:</number:text>
    //<number:minutes number:style="long"/>
    //  <number:text> </number:text>
    //<number:am-pm/>
    //</number:date-style>
    return "";
}

QString Odf::saveStyleNumericTime(KoGenStyles &mainStyles, Format::Type _style, const QString &_prefix, const QString &_postfix, const Localization *locale)
{
    //<number:time-style style:name="N42" style:family="data-style">
    //<number:hours number:style="long"/>
    //<number:text>:</number:text>
    //<number:minutes number:style="long"/>
    //<number:text> </number:text>
    //<number:am-pm/>
    //</number:time-style>

    QString format;
    if (Format::isTime(_style)) {
        format = locale->timeFormat(_style);
    } else {
        debugSheetsODF << "time format not defined :" << _style;
    }
    return KoOdfNumberStyles::saveOdfTimeStyle(mainStyles, format, false, _prefix, _postfix);
}

QString Odf::saveStyleNumericFraction(KoGenStyles &mainStyles, Format::Type formatType, const QString &_prefix, const QString &_suffix)
{
    //<number:number-style style:name="N71" style:family="data-style">
    //<number:fraction number:min-integer-digits="0" number:min-numerator-digits="2" number:min-denominator-digits="2"/>
    //</number:number-style>
    QString format;
    switch (formatType) {
    case Format::fraction_half:
        format = "# ?/2";
        break;
    case Format::fraction_quarter:
        format = "# ?/4";
        break;
    case Format::fraction_eighth:
        format = "# ?/8";
        break;
    case Format::fraction_sixteenth:
        format = "# ?/16";
        break;
    case Format::fraction_tenth:
        format = "# ?/10";
        break;
    case Format::fraction_hundredth:
        format = "# ?/100";
        break;
    case Format::fraction_one_digit:
        format = "# ?/?";
        break;
    case Format::fraction_two_digits:
        format = "# \?\?/\?\?";
        break;
    case Format::fraction_three_digits:
        format = "# \?\?\?/\?\?\?";
        break;
    default:
        debugSheetsODF << " fraction format not defined :" << formatType;
        break;
    }

    return KoOdfNumberStyles::saveOdfFractionStyle(mainStyles, format, _prefix, _suffix);
}

// Helpers

Format::Type Odf::dateType(const QString &f, const Localization *locale)
{
    for (int i = Format::DatesBegin; i < Format::DatesEnd; ++i) {
        if (f == locale->dateFormat((Format::Type)i)) {
            return (Format::Type(i));
        }
    }
    for (int i = Format::DateTimesBegin; i < Format::DateTimesEnd; ++i) {
        if (f == locale->dateTimeFormat((Format::Type)i)) {
            return (Format::Type(i));
        }
    }
    warnSheetsODF << Q_FUNC_INFO << "Unknown date or datetime format:" << f;
    return Format::ShortDate;
}

Format::Type Odf::timeType(const QString &_format, const Localization *locale)
{
    for (int i = Format::TimesBegin; i < Format::TimesEnd; ++i) {
        if (_format == locale->timeFormat((Format::Type)i)) {
            return (Format::Type)i;
        }
    }
    return Format::Time;
}

Currency Odf::numberCurrency(const QString &_format)
{
    // Look up if a prefix or postfix is in the currency table,
    // return the currency symbol to use for formatting purposes.
    if (!_format.isEmpty()) {
        QString f = QString(_format.at(0));
        Currency currStart = Currency(f);
        if (currStart.code() != f)
            return currStart;
        f = QString(_format.at(_format.size() - 1));
        Currency currEnd = Currency(f);
        if (currEnd.code() != f)
            return currEnd;
    }
    return Currency(QString());
}

Format::Type Odf::fractionType(const QString &_format)
{
    if (_format.endsWith(QLatin1String("/2")))
        return Format::fraction_half;
    else if (_format.endsWith(QLatin1String("/4")))
        return Format::fraction_quarter;
    else if (_format.endsWith(QLatin1String("/8")))
        return Format::fraction_eighth;
    else if (_format.endsWith(QLatin1String("/16")))
        return Format::fraction_sixteenth;
    else if (_format.endsWith(QLatin1String("/10")))
        return Format::fraction_tenth;
    else if (_format.endsWith(QLatin1String("/100")))
        return Format::fraction_hundredth;
    else if (_format.endsWith(QLatin1String("/?")))
        return Format::fraction_one_digit;
    else if (_format.endsWith(QLatin1String("/??")))
        return Format::fraction_two_digits;
    else if (_format.endsWith(QLatin1String("/???")))
        return Format::fraction_three_digits;
    else
        return Format::fraction_three_digits;
}

QPen Odf::decodePen(const QString &border)
{
    QPen pen;
    // string like "0.088cm solid #800000"
    if (border.isEmpty() || border == "none" || border == "hidden") { // in fact no border
        pen.setStyle(Qt::NoPen);
        return pen;
    }
    // code from koborder, for the moment Calligra Sheets doesn't use koborder
    //  ## isn't it faster to use QStringList::split than parse it 3 times?
    QString _width = border.section(' ', 0, 0);
    QByteArray _style = border.section(' ', 1, 1).toLatin1();
    QString _color = border.section(' ', 2, 2);

    pen.setWidth((int)(KoUnit::parseValue(_width, 1.0)));

    if (_style == "none")
        pen.setStyle(Qt::NoPen);
    else if (_style == "solid")
        pen.setStyle(Qt::SolidLine);
    else if (_style == "dashed")
        pen.setStyle(Qt::DashLine);
    else if (_style == "dotted")
        pen.setStyle(Qt::DotLine);
    else if (_style == "dot-dash")
        pen.setStyle(Qt::DashDotLine);
    else if (_style == "dot-dot-dash")
        pen.setStyle(Qt::DashDotDotLine);
    else
        debugSheets << " style undefined :" << _style;

    if (_color.isEmpty())
        pen.setColor(QColor());
    else
        pen.setColor(QColor(_color));

    return pen;
}

QString Odf::encodePen(const QPen &pen)
{
    //     debugSheets<<"encodePen( const QPen & pen ) :"<<pen;
    // NOTE Stefan: QPen api docs:
    //              A line width of zero indicates a cosmetic pen. This means
    //              that the pen width is always drawn one pixel wide,
    //              independent of the transformation set on the painter.
    QString s = QString("%1pt ").arg((pen.width() == 0) ? 1 : pen.width());
    switch (pen.style()) {
    case Qt::NoPen:
        return "none";
    case Qt::SolidLine:
        s += "solid";
        break;
    case Qt::DashLine:
        s += "dashed";
        break;
    case Qt::DotLine:
        s += "dotted";
        break;
    case Qt::DashDotLine:
        s += "dot-dash";
        break;
    case Qt::DashDotDotLine:
        s += "dot-dot-dash";
        break;
    default:
        break;
    }
    // debugSheets << " encodePen :" << s;
    if (pen.color().isValid()) {
        s += ' ' + colorName(pen.color());
    }
    return s;
}

QString Odf::colorName(const QColor &color)
{
    static QMap<QRgb, QString> map;
    QRgb rgb = color.rgb();
    if (!map.contains(rgb)) {
        map[rgb] = color.name();
        return map[rgb];
    } else {
        return map[rgb];
    }
}

} // Sheets
} // Calligra
