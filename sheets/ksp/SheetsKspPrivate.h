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

#ifndef SHEETS_KSP_PRIVATE
#define SHEETS_KSP_PRIVATE

// This should only be included by files inside the ksp/ subdir

#include <KoXmlNS.h>
#include <KoXmlReader.h>

#include "KoXmlReaderForward.h"

#include "Global.h"

namespace Calligra {
namespace Sheets {

class Cell;
class Conditions;
class CustomStyle;
class Localization;
class ProtectableObject;
class Sheet;
class Style;
class Validity;
class ValueConverter;
class ValueParser;

namespace Ksp {
    bool loadValidity(Validity *validity, Cell* const cell, const KoXmlElement& validityElement);
    QDomElement saveValidityXML(QDomDocument& doc, Validity *validity, const ValueConverter *converter);
    void loadConditions(Conditions *conditions, const KoXmlElement &element, const ValueParser *parser);
    QDomElement saveConditions(Conditions *conditions, QDomDocument &doc, ValueConverter *converter);

    // styles
    QDomElement saveStyles(StyleManager *manager, QDomDocument & doc);
    bool loadStyles(StyleManager *manager, KoXmlElement const & styles);
    void saveStyle(Style *style, QDomDocument& doc, QDomElement& format, const StyleManager* styleManager);
    void saveCustomStyle(CustomStyle *s, QDomDocument& doc, QDomElement& styles, const StyleManager* styleManager);
    bool loadCustomStyle(CustomStyle *s, KoXmlElement const & style, QString const & name);

    // doc
    void loadProtection(ProtectableObject *prot, const KoXmlElement& element);
    void loadNamedAreas(NamedAreaManager *manager, Map *map, const KoXmlElement& parent);
    QDomElement saveNamedAreas(NamedAreaManager *manager, QDomDocument& doc);
    void loadLocalization (Localization *l, const KoXmlElement& element);
    QDomElement saveLocalization (Localization *l, QDomDocument& doc);

    // map
    QDomElement saveMap(Map *map, QDomDocument& doc);
    bool loadMap(Map *map, const KoXmlElement& mymap);

    // sheet
    bool loadSheet(Sheet *obj, const KoXmlElement& sheet);
    QDomElement saveSheet(Sheet *sheet, QDomDocument& dd);

    // cell
    Cell loadCell(const KoXmlElement& cell, Sheet *sheet);

    /**
     * Save a cell.
     * @param doc document to save cell in
     * @param xOffset x offset
     * @param yOffset y offset
     * @param era set this to true if you want to encode relative references as absolutely (they will be switched
     *            back to relative references during decoding) - is used for cutting to clipboard
     *            Usually this is false, to only store the properties explicitly set.
     */
    QDomElement saveCell(Cell *cell, QDomDocument& doc, int xOffset = 0, int yOffset = 0, bool era = false);


}

}
}

#endif
