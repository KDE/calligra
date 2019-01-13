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

#ifndef SHEETS_ODF_PRIVATE
#define SHEETS_ODF_PRIVATE

// This should only be included by files inside the odf/ subdir

#include <KoOdfLoadingContext.h>
#include <KoOasisSettings.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>

#include "OdfLoadingContext.h"
#include "OdfSavingContext.h"

namespace Calligra {
namespace Sheets {

class CalculationSettings;
class NamedAreaManager;
class Conditions;
class Conditional;

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

    // SheetsOdfSheet
    bool loadSheet(Sheet *sheet, const KoXmlElement& sheetElement, OdfLoadingContext& tableContext, const Styles& autoStyles, const QHash<QString, Conditions>& conditionalStyles);
    void loadSheetSettings(Sheet *sheet, const KoOasisSettings::NamedMap &settings);
    bool saveSheet(Sheet *sheet, OdfSavingContext& tableContext);
    void saveSheetSettings(Sheet *sheet, KoXmlWriter &settingsWriter);

    // SheetsOdfCell
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
     * @param parser the parser
     * @return a hash of styles with the OpenDocument internal name as key
     */
    Styles loadAutoStyles(StyleManager *styles, KoOdfStylesReader& stylesReader,
                             QHash<QString, Conditions>& conditionalStyles,
                             const ValueParser *parser);
    void loadStyleTemplate(StyleManager *styles, KoOdfStylesReader& stylesReader, Map* map = 0);
    void saveStyles(StyleManager *manager, KoGenStyles &mainStyles);
    QString saveStyle(const Style *style, KoGenStyle& xmlstyle, KoGenStyles& mainStyles,
                       const StyleManager* manager);

    void loadDataStyle(Style *style, KoOdfStylesReader& stylesReader, const KoXmlElement& element,
                             Conditions& conditions, const StyleManager* styleManager,
                             const ValueParser *parser);

    // SheetsOdfCondition
    Conditional loadCondition(Conditions *conditions, const QString &conditionValue, const QString &applyStyleName,
                                 const QString &baseCellAddress, const ValueParser *parser);
    void loadConditions(Conditions *conditions, const KoXmlElement &element, const ValueParser *parser, const StyleManager* styleManager);
    void saveConditions(const Conditions *conditions, KoGenStyle &currentCellStyle, ValueConverter *converter);

    // SheetsOdfValidity
    void loadValidation(Validity *validity, Cell* const cell, const QString& validationName, OdfLoadingContext& tableContext);

}

}
}

#endif
