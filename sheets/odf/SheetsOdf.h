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

#ifndef SHEETS_ODF
#define SHEETS_ODF

#include <KoDocument.h>
#include "sheets_odf_export.h"

#include "Style.h"

class QBuffer;

class KoOdfReadStore;
class KoOdfStylesReader;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoXmlElement;

namespace Calligra {
namespace Sheets {

class Cell;
class Conditions;
class DocBase;
class Map;
class ProtectableObject;
class Region;
class Sheet;

namespace Odf {
    class OdfLoadingContext;
    struct ShapeLoadingData;

    CALLIGRA_SHEETS_ODF_EXPORT bool loadDocument(DocBase *doc, KoOdfReadStore &odfStore);
    CALLIGRA_SHEETS_ODF_EXPORT bool saveDocument(DocBase *doc, KoDocument::SavingContext &documentContext);

    CALLIGRA_SHEETS_ODF_EXPORT bool loadTableShape(Sheet *sheet, const KoXmlElement &element, KoShapeLoadingContext &context);
    CALLIGRA_SHEETS_ODF_EXPORT void saveTableShape(Sheet *sheet, KoShapeSavingContext &context);

    CALLIGRA_SHEETS_ODF_EXPORT ShapeLoadingData loadObject(Cell *cell, const KoXmlElement &element, KoShapeLoadingContext &shapeContext);
    CALLIGRA_SHEETS_ODF_EXPORT void loadCellText(Cell *cell, const KoXmlElement& parent, OdfLoadingContext& tableContext, const Styles& autoStyles, const QString& cellStyleName);

    CALLIGRA_SHEETS_ODF_EXPORT void loadProtection(ProtectableObject *prot, const KoXmlElement& element);
    CALLIGRA_SHEETS_ODF_EXPORT void loadSheetObject(Sheet *sheet, const KoXmlElement& element, KoShapeLoadingContext& shapeContext);

    CALLIGRA_SHEETS_ODF_EXPORT bool paste(QBuffer &buffer, Map *map);

    // styles
    CALLIGRA_SHEETS_ODF_EXPORT void loadDataStyle(Style *style, KoOdfStylesReader &stylesReader, const QString &styleName, Conditions &conditions, const StyleManager *styleManager, const ValueParser *parser);

    // regions
    CALLIGRA_SHEETS_ODF_EXPORT QString loadRegion(const QString& expression);
    CALLIGRA_SHEETS_ODF_EXPORT void loadRegion(const QChar *&data, const QChar *&end, QChar *&out);
    CALLIGRA_SHEETS_ODF_EXPORT QString saveRegion(const QString& expression);
    CALLIGRA_SHEETS_ODF_EXPORT QString saveRegion(Region *region);
}

}  // namespace Sheets
}  // namespace Calligra

#endif  // SHEETS_ODF
