// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
// Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "SheetsKsp.h"
#include "SheetsKspPrivate.h"

#include "engine/CalculationSettings.h"
#include "engine/NamedAreaManager.h"
#include "engine/Region.h"
#include "engine/SheetBase.h"

#include "DocBase.h"
#include "Map.h"

#include <KoProgressUpdater.h>
#include <KoUpdater.h>
#include <KoXmlReader.h>

#include <KMessageBox>

namespace Calligra
{
namespace Sheets
{

bool Ksp::loadDoc(DocBase *obj, const KoXmlDocument &doc)
{
    QPointer<KoUpdater> updater;
    if (obj->progressUpdater()) {
        updater = obj->progressUpdater()->startSubtask(1, "Ksp::loadDoc");
        updater->setProgress(0);
    }

    obj->setSpellListIgnoreAll(QStringList());
    // <spreadsheet>
    KoXmlElement spread = doc.documentElement();

    if (spread.attribute("mime") != "application/x-kspread" && spread.attribute("mime") != "application/vnd.kde.kspread") {
        obj->setErrorMessage(
            i18n("Invalid document. Expected mimetype application/x-kspread or application/vnd.kde.kspread, got %1", spread.attribute("mime")));
        return false;
    }

    bool ok = false;
    int version = spread.attribute("syntaxVersion").toInt(&ok);
    obj->map()->setSyntaxVersion(ok ? version : 0);
    if (obj->map()->syntaxVersion() > CURRENT_SYNTAX_VERSION) {
        int ret = KMessageBox::warningContinueCancel(0,
                                                     i18n("This document was created with a newer version of Calligra Sheets (syntax version: %1)\n"
                                                          "When you open it with this version of Calligra Sheets, some information may be lost.",
                                                          obj->map()->syntaxVersion()),
                                                     i18n("File Format Mismatch"),
                                                     KStandardGuiItem::cont());
        if (ret == KMessageBox::Cancel) {
            obj->setErrorMessage("USER_CANCELED");
            return false;
        }
    }

    if (updater)
        updater->setProgress(5);

    KoXmlElement defaults = spread.namedItem("defaults").toElement();
    if (!defaults.isNull()) {
        double dim = defaults.attribute("row-height").toDouble(&ok);
        if (!ok)
            return false;
        obj->map()->setDefaultRowHeight(dim);

        dim = defaults.attribute("col-width").toDouble(&ok);

        if (!ok)
            return false;

        obj->map()->setDefaultColumnWidth(dim);
    }

    KoXmlElement ignoreAll = spread.namedItem("SPELLCHECKIGNORELIST").toElement();
    if (!ignoreAll.isNull()) {
        KoXmlElement spellWord = spread.namedItem("SPELLCHECKIGNORELIST").toElement();

        QStringList lst;
        spellWord = spellWord.firstChild().toElement();
        while (!spellWord.isNull()) {
            if (spellWord.tagName() == "SPELLCHECKIGNOREWORD") {
                lst.append(spellWord.attribute("word"));
            }
            spellWord = spellWord.nextSibling().toElement();
        }
        obj->setSpellListIgnoreAll(lst);
    }

    if (updater)
        updater->setProgress(40);
    // In case of reload (e.g. from konqueror)
    qDeleteAll(obj->map()->sheetList());
    obj->map()->sheetList().clear();

    KoXmlElement styles = spread.namedItem("styles").toElement();
    if (!styles.isNull()) {
        if (!loadStyles(obj->map()->styleManager(), styles)) {
            obj->setErrorMessage(i18n("Styles cannot be loaded."));
            return false;
        }
    }

    // <map>
    KoXmlElement mymap = spread.namedItem("map").toElement();
    if (mymap.isNull()) {
        obj->setErrorMessage(i18n("Invalid document. No map tag."));
        return false;
    }
    if (!loadMap(obj->map(), mymap)) {
        return false;
    }

    // named areas
    const KoXmlElement areaname = spread.namedItem("areaname").toElement();
    if (!areaname.isNull())
        loadNamedAreas(obj->map()->namedAreaManager(), obj->map(), areaname);

    if (updater)
        updater->setProgress(90);
    obj->initConfig();
    if (updater)
        updater->setProgress(100);

    return true;
}

QDomDocument Ksp::saveDoc(DocBase *document)
{
    QDomDocument doc = KoDocument::createDomDocument("tables", "spreadsheet", CURRENT_DTD_VERSION);
    QDomElement spread = doc.documentElement();
    spread.setAttribute("editor", "Calligra Sheets");
    spread.setAttribute("mime", "application/x-kspread");
    spread.setAttribute("syntaxVersion", QString::number(CURRENT_SYNTAX_VERSION));

    QStringList lst = document->spellListIgnoreAll();
    if (!lst.isEmpty()) {
        QDomElement spellCheckIgnore = doc.createElement("SPELLCHECKIGNORELIST");
        spread.appendChild(spellCheckIgnore);
        for (QStringList::ConstIterator it = lst.constBegin(); it != lst.constEnd(); ++it) {
            QDomElement spellElem = doc.createElement("SPELLCHECKIGNOREWORD");
            spellCheckIgnore.appendChild(spellElem);
            spellElem.setAttribute("word", *it);
        }
    }

    QDomElement e = saveMap(document->map(), doc);
    spread.appendChild(e);

    document->setModified(false);

    return doc;
}

void Ksp::loadProtection(ProtectableObject *prot, const KoXmlElement &element)
{
    if (element.hasAttribute("protected")) {
        const QString passwd = element.attribute("protected");
        QByteArray str(passwd.toUtf8());
        prot->setProtected(QByteArray::fromBase64(str));
    }
}

void Ksp::loadNamedAreas(NamedAreaManager *manager, Map *map, const KoXmlElement &parent)
{
    KoXmlElement element;
    forEachElement(element, parent)
    {
        if (element.tagName() == "reference") {
            SheetBase *sheet = 0;
            QString refname;
            int left = 0;
            int right = 0;
            int top = 0;
            int bottom = 0;
            KoXmlElement sheetName = element.namedItem("tabname").toElement();
            if (!sheetName.isNull())
                sheet = map->findSheet(sheetName.text());
            if (!sheet)
                continue;
            KoXmlElement referenceName = element.namedItem("refname").toElement();
            if (!referenceName.isNull())
                refname = referenceName.text();
            KoXmlElement rect = element.namedItem("rect").toElement();
            if (!rect.isNull()) {
                bool ok;
                if (rect.hasAttribute("left-rect"))
                    left = rect.attribute("left-rect").toInt(&ok);
                if (rect.hasAttribute("right-rect"))
                    right = rect.attribute("right-rect").toInt(&ok);
                if (rect.hasAttribute("top-rect"))
                    top = rect.attribute("top-rect").toInt(&ok);
                if (rect.hasAttribute("bottom-rect"))
                    bottom = rect.attribute("bottom-rect").toInt(&ok);
            }
            manager->insert(Region(QRect(QPoint(left, top), QPoint(right, bottom)), sheet), refname);
        }
    }
}

QDomElement Ksp::saveNamedAreas(NamedAreaManager *manager, QDomDocument &doc)
{
    QDomElement element = doc.createElement("areaname");
    for (const QString &name : manager->areaNames()) {
        QDomElement e = doc.createElement("reference");
        QDomElement tabname = doc.createElement("tabname");
        tabname.appendChild(doc.createTextNode(manager->sheet(name)->sheetName()));
        e.appendChild(tabname);

        QDomElement refname = doc.createElement("refname");
        refname.appendChild(doc.createTextNode(name));
        e.appendChild(refname);

        QDomElement rect = doc.createElement("rect");
        QRect r = manager->namedArea(name).boundingRect();
        rect.setAttribute("left-rect", QString::number(r.left()));
        rect.setAttribute("right-rect", QString::number(r.right()));
        rect.setAttribute("top-rect", QString::number(r.top()));
        rect.setAttribute("bottom-rect", QString::number(r.bottom()));
        e.appendChild(rect);
        element.appendChild(e);
    }
    return element;
}

} // Sheets
} // Calligra
