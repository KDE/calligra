// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
// Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later


#include "Map.h"

#include "CalculationSettings.h"
#include "DocBase.h"
#include "LoadingInfo.h"
#include "Localization.h"
#include "NamedAreaManager.h"

namespace Calligra {
namespace Sheets {


QDomElement Ksp::saveMap(Map *map, QDomDocument& doc)
{
    QDomElement spread = doc.documentElement();

    Localization *l = static_cast<Localization*>(obj->map()->calculationSettings()->locale());
    QDomElement locale = saveLocalization (l, doc);
    spread.appendChild(locale);

    QDomElement areaname = saveNamedAreas (map->namedAreaManager(), doc);
    spread.appendChild(areaname);

    QDomElement defaults = doc.createElement("defaults");
    defaults.setAttribute("row-height", QString::number(map->defaultRowFormat()->height()));
    defaults.setAttribute("col-width", QString::number(map->defaultColumnFormat()->width()));
    spread.appendChild(defaults);

    QDomElement s = saveStyles (map->styleManager(), doc);
    spread.appendChild(s);

    QDomElement mymap = doc.createElement("map");

    QByteArray password = map->passwordHash();
    if (!password.isNull()) {
        if (password.size() > 0) {
            QByteArray str = KCodecs::base64Encode(password);
            mymap.setAttribute("protected", QString(str.data()));
        } else {
            mymap.setAttribute("protected", "");
        }
    }

    foreach(Sheet* sheet, d->lstSheets) {
        QDomElement e = saveSheet (sheet, doc);
        if (e.isNull())
            return e;
        mymap.appendChild(e);
    }
    return mymap;
}

bool Ksp::loadMap(Map *map, const KoXmlElement& mymap)
{
    map->setLoading (true);
    loadingInfo()->setFileFormat(LoadingInfo::NativeFormat);
    const QString activeSheet = mymap.attribute("activeTable");
    const QPoint marker(mymap.attribute("markerColumn").toInt(), mymap.attribute("markerRow").toInt());
    loadingInfo()->setCursorPosition(findSheet(activeSheet), marker);
    const QPointF offset(mymap.attribute("xOffset").toDouble(), mymap.attribute("yOffset").toDouble());
    loadingInfo()->setScrollingOffset(findSheet(activeSheet), offset);

    KoXmlNode n = mymap.firstChild();
    if (n.isNull()) {
        // We need at least one sheet !
        map->doc()->setErrorMessage(i18n("This document has no sheets (tables)."));
        map->setLoading (false);
        return false;
    }
    while (!n.isNull()) {
        KoXmlElement e = n.toElement();
        if (!e.isNull() && e.tagName() == "table") {
            Sheet *t = map->addNewSheet();
            if (!loadSheet (t, e)) {
                map->setLoading (false);
                return false;
            }
        }
        n = n.nextSibling();
    }

    loadProtection(map, mymap);

    if (!activeSheet.isEmpty()) {
        // Used by View's constructor
        loadingInfo()->setInitialActiveSheet(findSheet(activeSheet));
    }

    map->setLoading (false);
    return true;
}



}  // Sheets
}  // Calligra
