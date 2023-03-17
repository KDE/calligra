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

#ifndef SHEETS_ODF_PRIVATE
#define SHEETS_ODF_PRIVATE

// This should only be included by files inside the odf/ subdir

#include <KoOdfLoadingContext.h>
#include <KoOasisSettings.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>

#include "OdfLoadingContext.h"
#include "OdfSavingContext.h"
#include "engine/Value.h"

namespace Calligra {
namespace Sheets {

class CalculationSettings;
class NamedAreaManager;
class Conditions;
class Conditional;
class Cell;
class CellBase;
class Map;
class MapBase;
class Sheet;
class Database;
class Filter;
class Validity;
class AbstractCondition;

namespace Odf {

    // SheetsOdfDoc
    void loadCalculationSettings(CalculationSettings *settings, const KoXmlElement& body);
    bool saveCalculationSettings(const CalculationSettings *settings, KoXmlWriter &settingsWriter);

    // SheetsOdfMap
    bool loadMap(Map *map, const KoXmlElement& body, KoOdfLoadingContext& odfContext);
    void loadMapSettings(Map *map, const KoOasisSettings &settingsDoc);
    bool saveMap(Map *map, KoXmlWriter & xmlWriter, KoShapeSavingContext & savingContext);
    void loadNamedAreas(NamedAreaManager *manager, const KoXmlElement& body);
    void saveNamedAreas(const NamedAreaManager *manager, KoXmlWriter& xmlWriter);
    bool loadDatabaseRanges(Map *map, const KoXmlElement& body);
    void saveDatabaseRanges(Map *map, KoXmlWriter& xmlWriter);
    Database loadDatabase(const KoXmlElement& element, const Map* map, bool *ok);
    void saveDatabase(const Database &database, KoXmlWriter& xmlWriter);
    void saveDatabaseFilter(const Filter &filter, KoXmlWriter& xmlWriter);
    Filter loadDatabaseFilter(const KoXmlElement& element, const Map* map, bool *ok);
    void saveDatabaseCondition(AbstractCondition *condition, KoXmlWriter& xmlWriter);
    AbstractCondition *loadDatabaseCondition(const KoXmlElement& element);

    // SheetsOdfSheet
    bool loadSheet(Sheet *sheet, const KoXmlElement& sheetElement, OdfLoadingContext& tableContext, const Styles& autoStyles, const QHash<QString, Conditions>& conditionalStyles);
    void loadSheetSettings(Sheet *sheet, const KoOasisSettings::NamedMap &settings);
    bool saveSheet(Sheet *sheet, OdfSavingContext& tableContext);
    void saveSheetSettings(Sheet *sheet, KoXmlWriter &settingsWriter);

    // SheetsOdfCell
    QString toSaveString(const Value &value, const Value::Format format, CalculationSettings *cs = nullptr);
    bool loadCell(Cell *cell, const KoXmlElement& element, OdfLoadingContext& tableContext,
            const Styles& autoStyles, const QString& cellStyleName,
            QList<ShapeLoadingData>& shapeData);
    bool saveCell(Cell *cell, int &repeated, OdfSavingContext& tableContext);

    // SheetsOdfStyle

    /**
     * Loads OpenDocument auto styles.
     * The auto styles are preloaded, because an auto style could be shared
     * among cells. So, preloading prevents a multiple loading of the same
     * auto style.
     * This method is called before the cell loading process.
     * @param styles the styles
     * @param stylesReader repository of styles
     * @param conditionalStyles the conditional styles
     * @param locale locale - needed for date formats
     * @return a hash of styles with the OpenDocument internal name as key
     */
    Styles loadAutoStyles(StyleManager *styles, KoOdfStylesReader& stylesReader,
                             QHash<QString, Conditions>& conditionalStyles,
                             const Localization *locale);
    void loadStyleTemplate(StyleManager *styles, KoOdfStylesReader& stylesReader, MapBase* map = 0);
    void saveStyles(StyleManager *manager, KoGenStyles &mainStyles, const Localization *locale);
    QString saveStyle(const Style *style, KoGenStyle& xmlstyle, KoGenStyles& mainStyles,
                       const StyleManager* manager, const Localization *locale);
    void loadDataStyle(Style *style, KoOdfStylesReader& stylesReader, const KoXmlElement& element,
                             Conditions& conditions, const StyleManager* styleManager,
                             const Localization *locale);

    // SheetsOdfCondition
    Conditional loadCondition(Conditions *conditions, const QString &conditionValue, const QString &applyStyleName,
                                 const QString &baseCellAddress);
    void loadConditions(Conditions *conditions, const KoXmlElement &element, const StyleManager* styleManager);
    void saveConditions(const Conditions *conditions, KoGenStyle &currentCellStyle, CalculationSettings *cs);

    // SheetsOdfValidity
    void loadValidation(Validity *validity, CellBase* const cell, const QString& validationName, OdfLoadingContext& tableContext);

}

}
}

#endif
