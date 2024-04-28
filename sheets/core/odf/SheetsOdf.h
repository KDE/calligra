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

#ifndef SHEETS_ODF
#define SHEETS_ODF

#include "sheets_core_export.h"
#include <KoDocument.h>

#include "Style.h"

class QBuffer;

class KoOdfReadStore;
class KoOdfStylesReader;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoXmlElement;

namespace Calligra
{
namespace Sheets
{

class Cell;
class Conditions;
class DocBase;
class Localization;
class Map;
class ProtectableObject;
class Region;
class Sheet;

namespace Odf
{
class OdfLoadingContext;
struct ShapeLoadingData;

CALLIGRA_SHEETS_CORE_EXPORT bool loadDocument(DocBase *doc, KoOdfReadStore &odfStore);
CALLIGRA_SHEETS_CORE_EXPORT bool saveDocument(DocBase *doc, KoDocument::SavingContext &documentContext);

CALLIGRA_SHEETS_CORE_EXPORT bool loadTableShape(Sheet *sheet, const KoXmlElement &element, KoShapeLoadingContext &context);
CALLIGRA_SHEETS_CORE_EXPORT void saveTableShape(Sheet *sheet, KoShapeSavingContext &context);

CALLIGRA_SHEETS_CORE_EXPORT ShapeLoadingData loadObject(Cell *cell, const KoXmlElement &element, KoShapeLoadingContext &shapeContext);
CALLIGRA_SHEETS_CORE_EXPORT void
loadCellText(Cell *cell, const KoXmlElement &parent, OdfLoadingContext &tableContext, const Styles &autoStyles, const QString &cellStyleName);

CALLIGRA_SHEETS_CORE_EXPORT void loadProtection(ProtectableObject *prot, const KoXmlElement &element);
CALLIGRA_SHEETS_CORE_EXPORT void loadSheetObject(Sheet *sheet, const KoXmlElement &element, KoShapeLoadingContext &shapeContext);

CALLIGRA_SHEETS_CORE_EXPORT bool paste(QBuffer &buffer, Map *map);

// styles
CALLIGRA_SHEETS_CORE_EXPORT void loadDataStyle(Style *style,
                                               KoOdfStylesReader &stylesReader,
                                               const QString &styleName,
                                               Conditions &conditions,
                                               const StyleManager *styleManager,
                                               const Localization *locale);

// regions
CALLIGRA_SHEETS_CORE_EXPORT QString loadRegion(const QString &expression);
CALLIGRA_SHEETS_CORE_EXPORT void loadRegion(const QChar *&data, const QChar *&end, QChar *&out);
CALLIGRA_SHEETS_CORE_EXPORT QString saveRegion(const QString &expression);
CALLIGRA_SHEETS_CORE_EXPORT QString saveRegion(Region *region);

/**
 * Converts an OpenDocument representation of a formula to a localized formula.
 * @param expression_ The expression to convert from OpenDocument format.
 * @param locale The locale to which the expression should be converted.
 * @param namespacePrefix The namespace prefix.
 * \note Use Odf::loadRegion() for plain cell references.
 */
CALLIGRA_SHEETS_CORE_EXPORT QString decodeFormula(const QString &expression_, const Localization *locale = 0, const QString &namespacePrefix = QString());

/**
 * Converts a localized formula to an OpenDocument representation of a formula.
 * @param expr The expression to convert to OpenDocument format.
 * @param locale The locale from which the expression should be converted.
 * \note Use Odf::saveRegion() for plain cell references.
 */
CALLIGRA_SHEETS_CORE_EXPORT QString encodeFormula(const QString &expr, const Localization *locale = 0);

// These are used in filters.
CALLIGRA_SHEETS_CORE_EXPORT QString convertRefToRange(const QString &sheet, const QRect &rect);
CALLIGRA_SHEETS_CORE_EXPORT QString convertRefToBase(const QString &sheet, const QRect &rect);
CALLIGRA_SHEETS_CORE_EXPORT QString convertRangeToRef(const QString &sheetName, const QRect &_area);

// used in cell loading and filters
CALLIGRA_SHEETS_CORE_EXPORT QString convertMSOOXMLFormula(const QString &formula);
}

} // namespace Sheets
} // namespace Calligra

#endif // SHEETS_ODF
