/* This file is part of the KDE project
   Copyright 1998-2016 The Calligra Team <calligra-devel@kde.org>
   Copyright 2016 Tomas Mecir <mecirt@gmail.com>
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include "CalculationSettings.h"
#include "DocBase.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "StyleManager.h"
#include "Validity.h"
#include "database/DatabaseManager.h"

#include <KoCharacterStyle.h>
#include <KoDocumentResourceManager.h>
#include <KoGenStyles.h>
#include <KoStyleManager.h>
#include <KoStyleStack.h>
#include <KoText.h>
#include <KoTextSharedLoadingData.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include <kcodecs.h>

// This file contains functionality to load/save a Map

namespace Calligra {
namespace Sheets {

namespace Odf {
    void fixupStyle(KoCharacterStyle* style);
}

void Odf::fixupStyle(KoCharacterStyle* style)
{
    style->removeHardCodedDefaults();

    QTextCharFormat format;
    style->applyStyle(format);
    switch (style->underlineStyle()) {
        case KoCharacterStyle::NoLineStyle:
            format.setUnderlineStyle(QTextCharFormat::NoUnderline); break;
        case KoCharacterStyle::SolidLine:
            format.setUnderlineStyle(QTextCharFormat::SingleUnderline); break;
        case KoCharacterStyle::DottedLine:
            format.setUnderlineStyle(QTextCharFormat::DotLine); break;
        case KoCharacterStyle::DashLine:
            format.setUnderlineStyle(QTextCharFormat::DashUnderline); break;
        case KoCharacterStyle::DotDashLine:
            format.setUnderlineStyle(QTextCharFormat::DashDotLine); break;
        case KoCharacterStyle::DotDotDashLine:
            format.setUnderlineStyle(QTextCharFormat::DashDotDotLine); break;
        case KoCharacterStyle::LongDashLine:
            format.setUnderlineStyle(QTextCharFormat::DashUnderline); break;
        case KoCharacterStyle::WaveLine:
            format.setUnderlineStyle(QTextCharFormat::WaveUnderline); break;
    }
    style->copyProperties(format);
}

bool Odf::loadMap(Map *map, const KoXmlElement& body, KoOdfLoadingContext& odfContext)
{
    map->setLoading(true);
    map->loadingInfo()->setFileFormat(LoadingInfo::OpenDocument);

    //load in first
    loadStyleTemplate(map->styleManager(), odfContext.stylesReader(), map);

    OdfLoadingContext tableContext(odfContext);
    tableContext.validities = Validity::preloadValidities(body); // table:content-validations

    // load text styles for rich-text content and TOS
    KoShapeLoadingContext shapeContext(tableContext.odfContext, map->resourceManager());
    tableContext.shapeContext = &shapeContext;
    KoTextSharedLoadingData * sharedData = new KoTextSharedLoadingData();
    sharedData->loadOdfStyles(shapeContext, map->textStyleManager());

    fixupStyle((KoCharacterStyle*)map->textStyleManager()->defaultParagraphStyle());
    foreach (KoCharacterStyle* style, sharedData->characterStyles(true)) {
        fixupStyle(style);
    }
    foreach (KoCharacterStyle* style, sharedData->characterStyles(false)) {
        fixupStyle(style);
    }
    shapeContext.addSharedData(KOTEXT_SHARED_LOADING_ID, sharedData);

    QVariant variant;
    variant.setValue(map->textStyleManager());
    map->resourceManager()->setResource(KoText::StyleManager, variant);


    // load default column style
    const KoXmlElement* defaultColumnStyle = odfContext.stylesReader().defaultStyle("table-column");
    if (defaultColumnStyle) {
//       debugSheets <<"style:default-style style:family=\"table-column\"";
        KoStyleStack styleStack;
        styleStack.push(*defaultColumnStyle);
        styleStack.setTypeProperties("table-column");
        if (styleStack.hasProperty(KoXmlNS::style, "column-width")) {
            const double width = KoUnit::parseValue(styleStack.property(KoXmlNS::style, "column-width"), -1.0);
            if (width != -1.0) {
//           debugSheets <<"\tstyle:column-width:" << width;
                map->setDefaultColumnWidth(width);
            }
        }
    }

    // load default row style
    const KoXmlElement* defaultRowStyle = odfContext.stylesReader().defaultStyle("table-row");
    if (defaultRowStyle) {
//       debugSheets <<"style:default-style style:family=\"table-row\"";
        KoStyleStack styleStack;
        styleStack.push(*defaultRowStyle);
        styleStack.setTypeProperties("table-row");
        if (styleStack.hasProperty(KoXmlNS::style, "row-height")) {
            const double height = KoUnit::parseValue(styleStack.property(KoXmlNS::style, "row-height"), -1.0);
            if (height != -1.0) {
//           debugSheets <<"\tstyle:row-height:" << height;
                map->setDefaultRowHeight(height);
            }
        }
    }

    loadCalculationSettings(map->calculationSettings(), body); // table::calculation-settings
    if (body.hasAttributeNS(KoXmlNS::table, "structure-protected")) {
        loadProtection(map, body);
    }

    KoXmlNode sheetNode = KoXml::namedItemNS(body, KoXmlNS::table, "table");

    if (sheetNode.isNull()) {
        // We need at least one sheet !
        map->doc()->setErrorMessage(i18n("This document has no sheets (tables)."));
        map->setLoading(false);
        return false;
    }

    int overallRowCount = 0;
    while (!sheetNode.isNull()) {
        KoXmlElement sheetElement = sheetNode.toElement();
        if (!sheetElement.isNull()) {
            //debugSheets<<"  Odf::loadMap tableElement is not null";
            //debugSheets<<"tableElement.nodeName() :"<<sheetElement.nodeName();

            // make it slightly faster
            KoXml::load(sheetElement);

            if (sheetElement.nodeName() == "table:table") {
                if (!sheetElement.attributeNS(KoXmlNS::table, "name", QString()).isEmpty()) {
                    const QString sheetName = sheetElement.attributeNS(KoXmlNS::table, "name", QString());
                    Sheet* sheet = map->addNewSheet(sheetName);
                    sheet->setSheetName(sheetName, true);
                    overallRowCount += KoXml::childNodesCount(sheetElement);
                }
            }
        }

        // reduce memory usage
        KoXml::unload(sheetElement);
        sheetNode = sheetNode.nextSibling();
    }
    map->setOverallRowsCounter(overallRowCount);   // used for loading progress info

    //pre-load auto styles
    QHash<QString, Conditions> conditionalStyles;
    Styles autoStyles = loadAutoStyles(map->styleManager(), odfContext.stylesReader(),
                        conditionalStyles, map->parser());

    // load the sheet
    sheetNode = body.firstChild();
    while (!sheetNode.isNull()) {
        KoXmlElement sheetElement = sheetNode.toElement();
        if (!sheetElement.isNull()) {
            // make it slightly faster
            KoXml::load(sheetElement);

            //debugSheets<<"tableElement.nodeName() bis :"<<sheetElement.nodeName();
            if (sheetElement.nodeName() == "table:table") {
                if (!sheetElement.attributeNS(KoXmlNS::table, "name", QString()).isEmpty()) {
                    QString name = sheetElement.attributeNS(KoXmlNS::table, "name", QString());
                    Sheet* sheet = map->findSheet(name);
                    if (sheet)
                        loadSheet(sheet, sheetElement, tableContext, autoStyles, conditionalStyles);
                }
            }
        }

        // reduce memory usage
        KoXml::unload(sheetElement);
        sheetNode = sheetNode.nextSibling();
    }

    // make sure always at least one sheet exists
    if (map->count() == 0) {
        map->addNewSheet();
    }

    //delete any styles which were not used
    map->styleManager()->clearOasisStyles();

    // Load databases. This needs the sheets to be loaded.
///TODO new style odf
    map->databaseManager()->loadOdf(body); // table:database-ranges
    loadNamedAreas(map->namedAreaManager(), body); // table:named-expressions

    map->setLoading(false);
    return true;
}

void Odf::loadMapSettings(Map *map, const KoOasisSettings &settings)
{
    KoOasisSettings::Items viewSettings = settings.itemSet("view-settings");
    KoOasisSettings::IndexedMap viewMap = viewSettings.indexedMap("Views");
    KoOasisSettings::Items firstView = viewMap.entry(0);

    KoOasisSettings::NamedMap sheetsMap = firstView.namedMap("Tables");
    debugSheets << " loadMapSettings( KoOasisSettings &settings ) exist :" << !sheetsMap.isNull();
    if (!sheetsMap.isNull()) {
        foreach(Sheet* sheet, map->sheetList()) {
            loadSheetSettings(sheet, sheetsMap);
        }
    }

    QString activeSheet = firstView.parseConfigItemString("ActiveTable");
    debugSheets << " loadMapSettings( KoOasisSettings &settings ) activeSheet :" << activeSheet;

    if (!activeSheet.isEmpty()) {
        // Used by View's constructor
        map->loadingInfo()->setInitialActiveSheet(map->findSheet(activeSheet));
    }
}

bool Odf::saveMap(Map *map, KoXmlWriter & xmlWriter, KoShapeSavingContext & savingContext)
{
    // Saving the custom cell styles including the default cell style.
    saveStyles(map->styleManager(), savingContext.mainStyles());

    // Saving the default column style
    KoGenStyle defaultColumnStyle(KoGenStyle::TableColumnStyle, "table-column");
    defaultColumnStyle.addPropertyPt("style:column-width", map->defaultColumnFormat()->width());
    defaultColumnStyle.setDefaultStyle(true);
    savingContext.mainStyles().insert(defaultColumnStyle, "Default", KoGenStyles::DontAddNumberToName);

    // Saving the default row style
    KoGenStyle defaultRowStyle(KoGenStyle::TableRowStyle, "table-row");
    defaultRowStyle.addPropertyPt("style:row-height", map->defaultRowFormat()->height());
    defaultRowStyle.setDefaultStyle(true);
    savingContext.mainStyles().insert(defaultRowStyle, "Default", KoGenStyles::DontAddNumberToName);

    saveCalculationSettings(map->calculationSettings(), xmlWriter); // table::calculation-settings

    QByteArray password;
    map->password(password);
    if (!password.isNull()) {
        xmlWriter.addAttribute("table:structure-protected", "true");
        QByteArray str = KCodecs::base64Encode(password);
        // FIXME Stefan: see OpenDocument spec, ch. 17.3 Encryption
        xmlWriter.addAttribute("table:protection-key", QString(str.data()));
    }

    OdfSavingContext tableContext(savingContext);

    foreach(Sheet* sheet, map->sheetList()) {
        saveSheet(sheet, tableContext);
    }

    tableContext.valStyle.writeStyle(xmlWriter);

    saveNamedAreas(map->namedAreaManager(), savingContext.xmlWriter());
///TODO new style odf
    map->databaseManager()->saveOdf(savingContext.xmlWriter());
    return true;
}


// Table shape is here too, as the code is rather similar to Map load/save

bool Odf::loadTableShape(Sheet *sheet, const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // pre-load auto styles
    KoOdfLoadingContext& odfContext = context.odfLoadingContext();
    OdfLoadingContext tableContext(odfContext);
    QHash<QString, Conditions> conditionalStyles;
    Map *const map = sheet->map();
    StyleManager *const styleManager = map->styleManager();
    ValueParser *const parser = map->parser();
    Styles autoStyles = loadAutoStyles(styleManager, odfContext.stylesReader(), conditionalStyles, parser);

    if (!element.attributeNS(KoXmlNS::table, "name", QString()).isEmpty()) {
        sheet->setSheetName(element.attributeNS(KoXmlNS::table, "name", QString()), true);
    }
    bool result = loadSheet(sheet, element, tableContext, autoStyles, conditionalStyles);

    // delete any styles which were not used
    sheet->map()->styleManager()->clearOasisStyles();

    return result;
}

void Odf::saveTableShape(Sheet *sheet, KoShapeSavingContext &context)
{
    const Map* map = sheet->map();
    // Saving the custom cell styles including the default cell style.
    saveStyles(map->styleManager(), context.mainStyles());

    // Saving the default column style
    KoGenStyle defaultColumnStyle(KoGenStyle::TableColumnStyle, "table-column");
    defaultColumnStyle.addPropertyPt("style:column-width", map->defaultColumnFormat()->width());
    defaultColumnStyle.setDefaultStyle(true);
    context.mainStyles().insert(defaultColumnStyle, "Default", KoGenStyles::DontAddNumberToName);

    // Saving the default row style
    KoGenStyle defaultRowStyle(KoGenStyle::TableRowStyle, "table-row");
    defaultRowStyle.addPropertyPt("style:row-height", map->defaultRowFormat()->height());
    defaultRowStyle.setDefaultStyle(true);
    context.mainStyles().insert(defaultRowStyle, "Default", KoGenStyles::DontAddNumberToName);

    OdfSavingContext tableContext(context);
    saveSheet(sheet, tableContext);
    tableContext.valStyle.writeStyle(context.xmlWriter());
}

void Odf::loadNamedAreas(NamedAreaManager *manager, const KoXmlElement& body)
{
    KoXmlNode namedAreas = KoXml::namedItemNS(body, KoXmlNS::table, "named-expressions");
    if (namedAreas.isNull()) return;

    debugSheetsODF << "Loading named areas...";
    KoXmlElement element;
    forEachElement(element, namedAreas) {
        if (element.namespaceURI() != KoXmlNS::table)
            continue;
        if (element.localName() == "named-range") {
            if (!element.hasAttributeNS(KoXmlNS::table, "name"))
                continue;
            if (!element.hasAttributeNS(KoXmlNS::table, "cell-range-address"))
                continue;

            // TODO: what is: table:base-cell-address
            const QString base = element.attributeNS(KoXmlNS::table, "base-cell-address", QString());

            // Handle the case where the table:base-cell-address does contain the referenced sheetname
            // while it's missing in the table:cell-range-address. See bug #194386 for an example.
            Sheet* fallbackSheet = 0;
            if (!base.isEmpty()) {
                Region region(loadRegion(base), manager->map());
                fallbackSheet = region.lastSheet();
            }
            
            const QString name = element.attributeNS(KoXmlNS::table, "name", QString());
            const QString range = element.attributeNS(KoXmlNS::table, "cell-range-address", QString());
            debugSheetsODF << "Named area found, name:" << name << ", area:" << range;

            Region region(Odf::loadRegion(range), manager->map(), fallbackSheet);
            if (!region.isValid() || !region.lastSheet()) {
                debugSheetsODF << "invalid area";
                continue;
            }

            manager->insert(region, name);
        } else if (element.localName() == "named-expression") {
            debugSheetsODF << "Named expression found.";
            // TODO
        }
    }
}

void Odf::saveNamedAreas(const NamedAreaManager *manager, KoXmlWriter& xmlWriter)
{
   QList<QString> areas = manager->areaNames();
   if (areas.isEmpty()) return;

    Region region;
    xmlWriter.startElement("table:named-expressions");
    for (int i = 0; i < areas.count(); ++i) {
        QString name = areas[i];
        region = manager->namedArea(name);
        xmlWriter.startElement("table:named-range");
        xmlWriter.addAttribute("table:name", name);
        xmlWriter.addAttribute("table:base-cell-address", Odf::saveRegion(Region(1, 1, manager->sheet(name)).name()));
        xmlWriter.addAttribute("table:cell-range-address", Odf::saveRegion(&region));
        xmlWriter.endElement();
    }
    xmlWriter.endElement();
}





}  // Sheets
}  // Calligra

