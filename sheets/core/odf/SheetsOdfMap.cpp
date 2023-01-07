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

#include "engine/NamedAreaManager.h"
#include "engine/CalculationSettings.h"

#include "CellStorage.h"
#include "DocBase.h"
#include "Condition.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "ColFormatStorage.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "StyleManager.h"
#include "Database.h"
#include "DataFilter.h"


#include <KoCharacterStyle.h>
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
    QHash<QString, KoXmlElement> preloadValidities(const KoXmlElement& body);
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
    tableContext.validities = Odf::preloadValidities(body); // table:content-validations

    // load text styles for rich-text content and TOS
    KoShapeLoadingContext shapeContext(tableContext.odfContext, map->resourceManager());
    tableContext.shapeContext = &shapeContext;
    KoTextSharedLoadingData * sharedData = new KoTextSharedLoadingData();
    sharedData->loadOdfStyles(shapeContext, map->textStyleManager());

    fixupStyle((KoCharacterStyle*)map->textStyleManager()->defaultParagraphStyle());
    for (KoCharacterStyle* style : sharedData->characterStyles(true)) {
        fixupStyle(style);
    }
    for (KoCharacterStyle* style : sharedData->characterStyles(false)) {
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
                    SheetBase* sheet = map->addNewSheet(sheetName);
                    sheet->setSheetName(sheetName);
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
                        conditionalStyles, map->calculationSettings()->locale());

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
                    SheetBase* sheet = map->findSheet(name);
                    Sheet *fullSheet = sheet ? dynamic_cast<Sheet *>(sheet) : nullptr;
                    if (fullSheet)
                        loadSheet(fullSheet, sheetElement, tableContext, autoStyles, conditionalStyles);
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
    loadDatabaseRanges(map, body);    // table:database-ranges
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
        for (SheetBase* sheet : map->sheetList()) {
            Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
            loadSheetSettings(fullSheet, sheetsMap);
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
    saveStyles(map->styleManager(), savingContext.mainStyles(), map->calculationSettings()->locale());

    // Saving the default column style
    KoGenStyle defaultColumnStyle(KoGenStyle::TableColumnStyle, "table-column");
    defaultColumnStyle.addPropertyPt("style:column-width", map->defaultColumnFormat().width);
    defaultColumnStyle.setDefaultStyle(true);
    savingContext.mainStyles().insert(defaultColumnStyle, "Default", KoGenStyles::DontAddNumberToName);

    // Saving the default row style
    KoGenStyle defaultRowStyle(KoGenStyle::TableRowStyle, "table-row");
    defaultRowStyle.addPropertyPt("style:row-height", map->defaultRowFormat().height);
    defaultRowStyle.setDefaultStyle(true);
    savingContext.mainStyles().insert(defaultRowStyle, "Default", KoGenStyles::DontAddNumberToName);

    saveCalculationSettings(map->calculationSettings(), xmlWriter); // table::calculation-settings

    QByteArray password = map->passwordHash();
    if (!password.isNull()) {
        xmlWriter.addAttribute("table:structure-protected", "true");
        QByteArray str = KCodecs::base64Encode(password);
        // FIXME Stefan: see OpenDocument spec, ch. 17.3 Encryption
        xmlWriter.addAttribute("table:protection-key", QString(str.data()));
    }

    OdfSavingContext tableContext(savingContext);

    for (SheetBase* sheet : map->sheetList()) {
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
        saveSheet(fullSheet, tableContext);
    }

    tableContext.valStyle.writeStyle(xmlWriter);

    saveNamedAreas(map->namedAreaManager(), savingContext.xmlWriter());
    saveDatabaseRanges(map, savingContext.xmlWriter());
    return true;
}


// Table shape is here too, as the code is rather similar to Map load/save

bool Odf::loadTableShape(Sheet *sheet, const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // pre-load auto styles
    KoOdfLoadingContext& odfContext = context.odfLoadingContext();
    OdfLoadingContext tableContext(odfContext);
    QHash<QString, Conditions> conditionalStyles;
    Map *map = sheet->fullMap();
    StyleManager *const styleManager = map->styleManager();
    Styles autoStyles = loadAutoStyles(styleManager, odfContext.stylesReader(), conditionalStyles, map->calculationSettings()->locale());

    if (!element.attributeNS(KoXmlNS::table, "name", QString()).isEmpty()) {
        sheet->setSheetName(element.attributeNS(KoXmlNS::table, "name", QString()));
    }
    bool result = loadSheet(sheet, element, tableContext, autoStyles, conditionalStyles);

    // delete any styles which were not used
    map->styleManager()->clearOasisStyles();

    return result;
}

void Odf::saveTableShape(Sheet *sheet, KoShapeSavingContext &context)
{
    const Map* map = sheet->fullMap();
    // Saving the custom cell styles including the default cell style.
    saveStyles(map->styleManager(), context.mainStyles(), sheet->map()->calculationSettings()->locale());

    // Saving the default column style
    KoGenStyle defaultColumnStyle(KoGenStyle::TableColumnStyle, "table-column");
    defaultColumnStyle.addPropertyPt("style:column-width", map->defaultColumnFormat().width);
    defaultColumnStyle.setDefaultStyle(true);
    context.mainStyles().insert(defaultColumnStyle, "Default", KoGenStyles::DontAddNumberToName);

    // Saving the default row style
    KoGenStyle defaultRowStyle(KoGenStyle::TableRowStyle, "table-row");
    defaultRowStyle.addPropertyPt("style:row-height", map->defaultRowFormat().height);
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
            SheetBase* fallbackSheet = 0;
            if (!base.isEmpty()) {
                Region region = manager->map()->regionFromName(loadRegion(base));
                fallbackSheet = region.lastSheet();
            }
            
            const QString name = element.attributeNS(KoXmlNS::table, "name", QString());
            const QString range = element.attributeNS(KoXmlNS::table, "cell-range-address", QString());
            debugSheetsODF << "Named area found, name:" << name << ", area:" << range;

            Region region = manager->map()->regionFromName(loadRegion(range), fallbackSheet);
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

bool Odf::loadDatabaseRanges(Map *map, const KoXmlElement& body)
{
    const KoXmlNode databaseRanges = KoXml::namedItemNS(body, KoXmlNS::table, "database-ranges");
    KoXmlElement element;
    forEachElement(element, databaseRanges) {
        if (element.namespaceURI() != KoXmlNS::table)
            continue;
        if (element.localName() == "database-range") {
            bool okay = true;
            Database database = loadDatabase(element, map, &okay);
            if (!okay)
                return false;
            const Region region = database.range();
            if (!region.isValid())
                continue;
            SheetBase* sheet = region.lastSheet();
            if (!sheet)
                continue;
            Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
            fullSheet->fullCellStorage()->setDatabase(region, database);
        }
    }
    return true;
}

void Odf::saveDatabaseRanges(Map *map, KoXmlWriter& xmlWriter)
{
    QVector< QPair<QRectF, Database> > databases;
    const Region region(QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax)));
    const QList<SheetBase*>& sheets = map->sheetList();
    for (int i = 0; i < sheets.count(); ++i) {
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheets[i]);
        databases << fullSheet->fullCellStorage()->databases(region);
    }
    if (databases.isEmpty())
        return;

    xmlWriter.startElement("table:database-ranges");
    for (int i = 0; i < databases.count(); ++i) {
        Database database = databases[i].second;
        database.setRange(Region(databases[i].first.toRect(), database.range().firstSheet()));
        if (!database.range().isValid())
            continue;
        saveDatabase(database, xmlWriter);
    }
    xmlWriter.endElement();
}

Database Odf::loadDatabase(const KoXmlElement& element, const Map* map, bool *ok)
{
    *ok = true;
    Database db;
//    if (element.hasAttributeNS(KoXmlNS::table, "name"))
//        db.setName (element.attributeNS(KoXmlNS::table, "name", QString()));
    if (element.hasAttributeNS(KoXmlNS::table, "is-selection")) {
        bool val = (element.attributeNS(KoXmlNS::table, "is-selection", "false") == "true");
        db.setIsSelection(val);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "on-update-keep-styles")) {
        bool val = (element.attributeNS(KoXmlNS::table, "on-update-keep-styles", "false") == "true");
        db.setOnUpdateKeepStyles(val);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "on-update-keep-size")) {
        bool val = (element.attributeNS(KoXmlNS::table, "on-update-keep-size", "true") == "true");
        db.setOnUpdateKeepSize(val);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "has-persistent-data")) {
        bool val = (element.attributeNS(KoXmlNS::table, "has-persistent-data", "true") == "true");
        db.setHasPersistentData(val);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "orientation")) {
        if (element.attributeNS(KoXmlNS::table, "orientation", "row") == "column")
            db.setOrientation (Qt::Orientation::Horizontal);
        else
            db.setOrientation (Qt::Orientation::Vertical);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "contains-header")) {
        bool val = (element.attributeNS(KoXmlNS::table, "contains-header", "true") == "true");
        db.setContainsHeader(val);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "display-filter-buttons")) {
        bool val = (element.attributeNS(KoXmlNS::table, "display-filter-buttons", "false") == "true");
        db.setDisplayFilterButtons(val);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "target-range-address")) {
        const QString address = element.attributeNS(KoXmlNS::table, "target-range-address", QString());
        // only absolute addresses allowed; no fallback sheet needed
        Region range = map->regionFromName(Odf::loadRegion(address));
        if (!range.isValid()) {
            *ok = false;
            return db;
        }
        db.setRange (range);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "refresh-delay")) {
        bool okay = false;
        int delay = element.attributeNS(KoXmlNS::table, "refresh-delay", QString()).toInt(&okay);
        if (!okay || delay < 0) {
            *ok = false;
            return db;
        }
        db.setRefreshDelay (delay);
    }
    KoXmlElement child;
    forEachElement(child, element) {
        if (child.namespaceURI() != KoXmlNS::table)
            continue;
        if (child.localName() == "database-source-sql") {
            // TODO
        } else if (child.localName() == "database-source-table") {
            // TODO
        } else if (child.localName() == "database-source-query") {
            // TODO
        } else if (child.localName() == "sort") {
            // TODO
        } else if (child.localName() == "filter") {
            Calligra::Sheets::Filter filter = loadDatabaseFilter (child, map, ok);
            if (!*ok) {
                *ok = false;
                return db;
            }
            db.setFilter(filter);
        } else if (child.localName() == "subtotal-rules") {
            // TODO
        }
    }
    return db;
}

void Odf::saveDatabase(const Database &database, KoXmlWriter& xmlWriter)
{
    if (database.range().isEmpty())
        return;
    xmlWriter.startElement("table:database-range");
//    if (!database.name().isNull())
//        xmlWriter.addAttribute("table:name", database.name());
    if (database.isSelection())
        xmlWriter.addAttribute("table:is-selection", "true");
    if (database.onUpdateKeepStyles())
        xmlWriter.addAttribute("table:on-update-keep-styles", "true");
    if (!database.onUpdateKeepSize())
        xmlWriter.addAttribute("table:on-update-keep-size", "false");
    if (!database.hasPersistentData())
        xmlWriter.addAttribute("table:has-persistent-data", "false");
    if (database.orientation() == Qt::Orientation::Horizontal)
        xmlWriter.addAttribute("table:orientation", "column");
    if (!database.containsHeader())
        xmlWriter.addAttribute("table:contains-header", "false");
    if (database.displayFilterButtons())
        xmlWriter.addAttribute("table:display-filter-buttons", "true");
    xmlWriter.addAttribute("table:target-range-address", saveRegion(database.range().name()));
    if (database.refreshDelay())
        xmlWriter.addAttribute("table:refresh-delay", database.refreshDelay());

    saveDatabaseFilter(database.filter(), xmlWriter);

    xmlWriter.endElement();
}

void Odf::saveDatabaseFilter(const Filter &filter, KoXmlWriter& xmlWriter)
{
    if (filter.isEmpty()) return;

    xmlWriter.startElement("table:filter");
    Region r = filter.targetRangeAddress();
    if (!r.isEmpty())
        xmlWriter.addAttribute("table:target-range-address", saveRegion(r.name()));
    if (filter.conditionSourceIsRange())
        xmlWriter.addAttribute("table:condition-source", "cell-range");
    r = filter.sourceRangeAddress();
    if (!r.isEmpty())
        xmlWriter.addAttribute("table:condition-source-range-address", saveRegion(r.name()));
    if (!filter.displayDuplicates())
        xmlWriter.addAttribute("table:display-duplicates", "false");
    saveDatabaseCondition(filter.rootCondition(), xmlWriter);
    xmlWriter.endElement();
}

Filter Odf::loadDatabaseFilter(const KoXmlElement& element, const Map* map, bool *ok)
{
    *ok = false;
    Filter res;

    if (element.hasAttributeNS(KoXmlNS::table, "target-range-address")) {
        const QString address = element.attributeNS(KoXmlNS::table, "target-range-address", QString());
        // only absolute addresses allowed; no fallback sheet needed
        Region r = map->regionFromName(loadRegion(address));
        if (!r.isValid())
            return res;
        res.setTargetRangeAddress(r);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "condition-source")) {
        bool val = (element.attributeNS(KoXmlNS::table, "condition-source", "self") == "cell-range");
        res.setConditionSourceIsRange(val);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "condition-source-range-address")) {
        const QString address = element.attributeNS(KoXmlNS::table, "condition-source-range-address", QString());
        // only absolute addresses allowed; no fallback sheet needed
        Region r = map->regionFromName(loadRegion(address));
        res.setSourceRangeAddress(r);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "display-duplicates")) {
        bool val = (element.attributeNS(KoXmlNS::table, "display-duplicates", "true") == "true");
        res.setDisplayDuplicates(val);
    }

    KoXmlElement conditionElement;
    forEachElement(conditionElement, element) {
        // Check if this is the correct element. loadDatabaseCondition will return nullptr if it's not.
        AbstractCondition *cond = loadDatabaseCondition(conditionElement);
        if (!cond) continue;
        res.setRootCondition (cond);
        break;
    }
    *ok = true;
    return res;
}

static QString operationName(AbstractCondition::Comparison op) {
    switch (op) {
        case AbstractCondition::Comparison::Match:  return "match";
        case AbstractCondition::Comparison::NotMatch: return "!match";
        case AbstractCondition::Comparison::Equal: return "=";
        case AbstractCondition::Comparison::NotEqual: return "!=";
        case AbstractCondition::Comparison::Less: return "<";
        case AbstractCondition::Comparison::Greater: return ">";
        case AbstractCondition::Comparison::LessOrEqual: return "<=";
        case AbstractCondition::Comparison::GreaterOrEqual: return ">=";
        case AbstractCondition::Comparison::Empty: return "empty";
        case AbstractCondition::Comparison::NotEmpty: return "!empty";
        case AbstractCondition::Comparison::TopValues: return "top values";
        case AbstractCondition::Comparison::BottomValues: return "bottom values";
        case AbstractCondition::Comparison::TopPercent: return "top percent";
        case AbstractCondition::Comparison::BottomPercent: return "bottom percent";
    }
    return QString();
}

static AbstractCondition::Comparison operationFromName(const QString &name) {
    if (name == "match") return AbstractCondition::Comparison::Match;
    if (name == "!match") return AbstractCondition::Comparison::NotMatch;
    if (name == "=") return AbstractCondition::Comparison::Equal;
    if (name == "!=") return AbstractCondition::Comparison::NotEqual;
    if (name == "<") return AbstractCondition::Comparison::Less;
    if (name == ">") return AbstractCondition::Comparison::Greater;
    if (name == "<=") return AbstractCondition::Comparison::LessOrEqual;
    if (name == ">=") return AbstractCondition::Comparison::GreaterOrEqual;
    if (name == "empty") return AbstractCondition::Comparison::Empty;
    if (name == "!empty") return AbstractCondition::Comparison::NotEmpty;
    if (name == "top values") return AbstractCondition::Comparison::TopValues;
    if (name == "bottom values") return AbstractCondition::Comparison::BottomValues;
    if (name == "top percent") return AbstractCondition::Comparison::TopPercent;
    if (name == "bottom percent") return AbstractCondition::Comparison::BottomPercent;
    debugSheets << "table:operator: unknown value";
    return AbstractCondition::Comparison::Match;
}

void Odf::saveDatabaseCondition(AbstractCondition *condition, KoXmlWriter& xmlWriter)
{
    if (condition->allowsChildren()) {
        const char *elname;
        if (condition->type() == AbstractCondition::And)
            elname = "table:filter-and";
        else if (condition->type() == AbstractCondition::Or)
            elname = "table:filter-or";
        else return;
        QList<AbstractCondition*> children = condition->children();
        if (!children.count()) return;

        xmlWriter.startElement(elname);
        for (int i = 0; i < children.count(); ++i)
            saveDatabaseCondition(children[i], xmlWriter);
        xmlWriter.endElement();
    } else {
        if (condition->isEmpty()) return;
        Filter::Condition *cond = dynamic_cast<Filter::Condition *>(condition);
        if (!cond) return;

        xmlWriter.startElement("table:filter-condition");
        xmlWriter.addAttribute("table:field-number", cond->fieldNumber);
        xmlWriter.addAttribute("table:value", cond->value);
        xmlWriter.addAttribute("table:operator", operationName(cond->operation));
        if (cond->caseSensitivity == Qt::CaseSensitive)
            xmlWriter.addAttribute("table:case-sensitive", true);
        if (cond->dataType == Filter::Mode::Number)
            xmlWriter.addAttribute("table:data-type", "number");

        xmlWriter.endElement();
    }
}

AbstractCondition *Odf::loadDatabaseCondition(const KoXmlElement& element) {
    if (element.namespaceURI() != KoXmlNS::table)
        return nullptr;

    if (element.localName() == "filter-and") {
        Calligra::Sheets::Filter::And *cond = new Calligra::Sheets::Filter::And();

        KoXmlElement child;
        forEachElement(child, element) {
            AbstractCondition *childCond = loadDatabaseCondition(child);
            if (childCond) cond->list.append (childCond);
        }
        return cond;
    }
    if (element.localName() == "filter-or") {
        Calligra::Sheets::Filter::Or *cond = new Calligra::Sheets::Filter::Or();

        KoXmlElement child;
        forEachElement(child, element) {
            AbstractCondition *childCond = loadDatabaseCondition(child);
            if (childCond) cond->list.append (childCond);
        }
        return cond;
    }
    if (element.localName() == "filter-condition") {
        Calligra::Sheets::Filter::Condition *cond = new Calligra::Sheets::Filter::Condition();

        if (element.hasAttributeNS(KoXmlNS::table, "field-number")) {
            bool ok = false;
            cond->fieldNumber = element.attributeNS(KoXmlNS::table, "field-number", QString()).toInt(&ok);
            if (!ok || cond->fieldNumber < 0) {
                delete cond;
                return nullptr;
            }
        }
        if (element.hasAttributeNS(KoXmlNS::table, "value"))
            cond->value = element.attributeNS(KoXmlNS::table, "value", QString());

        if (element.hasAttributeNS(KoXmlNS::table, "operator")) {
            const QString str = element.attributeNS(KoXmlNS::table, "operator", QString());
            cond->operation = operationFromName(str);
        }
        if (element.hasAttributeNS(KoXmlNS::table, "case-sensitive")) {
            if (element.attributeNS(KoXmlNS::table, "case-sensitive", "false") == "true")
                cond->caseSensitivity = Qt::CaseSensitive;
            else
                cond->caseSensitivity = Qt::CaseInsensitive;
        }
        if (element.hasAttributeNS(KoXmlNS::table, "data-type")) {
            if (element.attributeNS(KoXmlNS::table, "data-type", "text") == "number")
                cond->dataType = Filter::Mode::Number;
            else
                cond->dataType = Filter::Mode::Text;
        }

        return cond;
    }

    return nullptr;
}



// static
QHash<QString, KoXmlElement> Odf::preloadValidities(const KoXmlElement& body)
{
    QHash<QString, KoXmlElement> validities;
    KoXmlNode validation = KoXml::namedItemNS(body, KoXmlNS::table, "content-validations");
    debugSheets << "validation.isNull?" << validation.isNull();
    if (!validation.isNull()) {
        KoXmlElement element;
        forEachElement(element, validation) {
            if (element.tagName() ==  "content-validation" && element.namespaceURI() == KoXmlNS::table) {
                const QString name = element.attributeNS(KoXmlNS::table, "name", QString());
                validities.insert(name, element);
                debugSheets << " validation found:" << name;
            } else {
                debugSheets << " Tag not recognized:" << element.tagName();
            }
        }
    }
    return validities;
}




}  // Sheets
}  // Calligra

