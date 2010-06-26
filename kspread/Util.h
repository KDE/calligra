/* This file is part of the KDE project
   Copyright 2006,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_UTIL
#define KSPREAD_UTIL

#include <QString>
#include <QRect>

#include "kspread_export.h"
#include <KoXmlReader.h>

#include "Global.h"
#include "Value.h"

class QFont;
class QPen;
class QDomElement;
class QDomDocument;

class KLocale;

bool util_isPointValid(const QPoint& point);
bool util_isRectValid(const QRect& rect);

namespace KSpread
{
class Cell;
class Map;
class Sheet;

namespace Util
{
/**
 * Call this function to decode the text of a column label to an integer,
 * e.g. 1 for A and 27 for AA.
 * Converted are all characters matching [A-Za-z]+ regular expresion, the rest is ignored.
 * 0 is returned if no characters match.
 */
KSPREAD_EXPORT int decodeColumnLabelText(const QString &labelText);

/**
 * Call this function to decode the text of a row label to an integer,
 * e.g. B7 is translated to 7.
 */
KSPREAD_EXPORT int decodeRowLabelText(const QString &labelText);

/**
 * Call this function to encode an integer to the text of the column label
 * i.e. 27->AA
 */
KSPREAD_EXPORT QString encodeColumnLabelText(int column);

//Return true when it's a reference to cell from sheet.
KSPREAD_EXPORT bool localReferenceAnchor(const QString &_ref);

// TODO Stefan: used nowhere
int         penCompare(QPen const & pen1, QPen const & pen2);
}

/**
 * \ingroup NativeFormat
 * This namespace collects methods related to KSpread's old native file format
 * encoding/decoding.
 */
namespace NativeFormat
{
/**
 * \ingroup NativeFormat
 */
QDomElement createElement(const QString & tagName, const QFont & font, QDomDocument & doc);

/**
 * \ingroup NativeFormat
 */
QDomElement createElement(const QString & tagname, const QPen & pen, QDomDocument & doc);

/**
 * \ingroup NativeFormat
 */
QFont       toFont(KoXmlElement & element);

/**
 * \ingroup NativeFormat
 */
QPen        toPen(KoXmlElement & element);
}

/**
 * \ingroup OpenDocument
 * This namespace collects methods related to OpenDocument
 * encoding/decoding.
 */
namespace Odf
{
/**
 * \ingroup OpenDocument
 * Creates OpenDocument pen attributes of the QPen \p pen .
 * \return the OpenDocument pen attributes
 */
QString encodePen(const QPen& pen);

/**
 * \ingroup OpenDocument
 * Creates a QPen of OpenDocument pen attributes \p str .
 * \return the created QPen
 */
QPen decodePen(const QString &str);

/**
 * \ingroup OpenDocument
 * Converts an OpenDocument representation of a formula to a localized formula.
 * @param expr The expression to convert from OpenDocument format.
 * @param locale The locale to which the expression should be converted.
 * \note Use Region::loadOdf() for plain cell references.
 */
// TODO check visibility
KSPREAD_EXPORT QString decodeFormula(const QString& expr, const KLocale* locale = 0, QString namespacePrefix = QString());

/**
 * \ingroup OpenDocument
 * Converts a localized formula to an OpenDocument representation of a formula.
 * @param expr The expression to convert to OpenDocument format.
 * @param locale The locale from which the expression should be converted.
 * \note Use Region::saveOdf() for plain cell references.
 */
KSPREAD_EXPORT QString encodeFormula(const QString& expr, const KLocale* locale = 0);

/**
 * \ingroup OpenDocument
 */
KSPREAD_EXPORT QString convertRefToRange(const QString & sheet, const QRect & rect);

/**
 * \ingroup OpenDocument
 */
KSPREAD_EXPORT QString convertRefToBase(const QString & sheet, const QRect & rect);

/**
 * \ingroup OpenDocument
 */
KSPREAD_EXPORT QString convertRangeToRef(const QString & sheetName, const QRect & _area);
}

} // namespace KSpread

#endif // KSPREAD_UTIL
