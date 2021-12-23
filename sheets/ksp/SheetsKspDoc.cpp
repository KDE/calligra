// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
// Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later


#include "DocBase.h"

#include "Map.h"

namespace Calligra {
namespace Sheets {


bool Ksp::loadDoc(DocBase *obj, const KoXmlDocument& doc)
{
    QPointer<KoUpdater> updater;
    if (progressUpdater()) {
        updater = progressUpdater()->startSubtask(1, "Ksp::loadDoc");
        updater->setProgress(0);
    }

    obj->spellListIgnoreAll().clear();
    // <spreadsheet>
    KoXmlElement spread = doc.documentElement();

    if (spread.attribute("mime") != "application/x-kspread" && spread.attribute("mime") != "application/vnd.kde.kspread") {
        obj->setErrorMessage(i18n("Invalid document. Expected mimetype application/x-kspread or application/vnd.kde.kspread, got %1" , spread.attribute("mime")));
        return false;
    }

    bool ok = false;
    int version = spread.attribute("syntaxVersion").toInt(&ok);
    obj->map()->setSyntaxVersion(ok ? version : 0);
    if (obj->map()->syntaxVersion() > CURRENT_SYNTAX_VERSION) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n("This document was created with a newer version of Calligra Sheets (syntax version: %1)\n"
                              "When you open it with this version of Calligra Sheets, some information may be lost.", obj->map()->syntaxVersion()),
                      i18n("File Format Mismatch"), KStandardGuiItem::cont());
        if (ret == KMessageBox::Cancel) {
            obj->setErrorMessage("USER_CANCELED");
            return false;
        }
    }

    // <locale>
    KoXmlElement loc = spread.namedItem("locale").toElement();
    if (!loc.isNull()) {
        Localization *l = static_cast<Localization*>(obj->map()->calculationSettings()->locale());
        loadLocalization (l, eoc);
    }

    if (updater) updater->setProgress(5);

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

        spellWord = spellWord.firstChild().toElement();
        while (!spellWord.isNull()) {
            if (spellWord.tagName() == "SPELLCHECKIGNOREWORD") {
                obj->spellListIgnoreAll().append(spellWord.attribute("word"));
            }
            spellWord = spellWord.nextSibling().toElement();
        }
    }

    if (updater) updater->setProgress(40);
    // In case of reload (e.g. from konqueror)
    qDeleteAll(obj->map()->sheetList());
    obj->map()->sheetList().clear();

    KoXmlElement styles = spread.namedItem("styles").toElement();
    if (!styles.isNull()) {
        if (!loadStyles (obj->map()->styleManager(), styles)) {
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
    if (!loadMap (obj->map(), mymap)) {
        return false;
    }

    // named areas
    const KoXmlElement areaname = spread.namedItem("areaname").toElement();
    if (!areaname.isNull())
        loadNamedAreas (obj->map()->namedAreaManager(), obj->map(), areaname);

    if (updater) updater->setProgress(90);
    obj->initConfig();
    if (updater) updater->setProgress(100);

    return true;
}

QDomDocument Ksp::saveDoc(DocBase *document)
{
    QDomDocument doc = KoDocument::createDomDocument("tables", "spreadsheet", CURRENT_DTD_VERSION);
    QDomElement spread = doc.documentElement();
    spread.setAttribute("editor", "Calligra Sheets");
    spread.setAttribute("mime", "application/x-kspread");
    spread.setAttribute("syntaxVersion", QString::number(CURRENT_SYNTAX_VERSION));

    if (!document->spellListIgnoreAll().isEmpty()) {
        QDomElement spellCheckIgnore = doc.createElement("SPELLCHECKIGNORELIST");
        spread.appendChild(spellCheckIgnore);
        for (QStringList::ConstIterator it = document->spellListIgnoreAll().constBegin(); it != document->spellListIgnoreAll().constEnd(); ++it) {
            QDomElement spellElem = doc.createElement("SPELLCHECKIGNOREWORD");
            spellCheckIgnore.appendChild(spellElem);
            spellElem.setAttribute("word", *it);
        }
    }

    QDomElement e = saveMap (document->map(), doc);

/*FIXME
        // Save visual info for the first view, such as active sheet and active cell
        // It looks like a hack, but reopening a document creates only one view anyway (David)
        View *const view = static_cast<View*>(document->views().first());
        Canvas *const canvas = view->canvasWidget();
        e.setAttribute("activeTable",  canvas->activeSheet()->sheetName());
        e.setAttribute("markerColumn", QString::number(view->selection()->marker().x()));
        e.setAttribute("markerRow",    QString::number(view->selection()->marker().y()));
        e.setAttribute("xOffset",      QString::number(canvas->xOffset()));
        e.setAttribute("yOffset",      QString::number(canvas->yOffset()));
*/
    spread.appendChild(e);

    document->setModified(false);

    return doc;





void Ksp::loadProtection(ProtectableObject *prot, const KoXmlElement& element)
{
    if (element.hasAttribute("protected")) {
        const QString passwd = element.attribute("protected");
        QByteArray str(passwd.toUtf8());
        prot->setProtected (KCodecs::base64Decode(str));
    }
}


void Ksp::loadNamedAreas(NamedAreaManager *manager, Map *map, const KoXmlElement& parent)
{
    KoXmlElement element;
    forEachElement(element, parent) {
        if (element.tagName() == "reference") {
            Sheet* sheet = 0;
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
            insert(Region(QRect(QPoint(left, top), QPoint(right, bottom)), sheet), refname);
        }
    }
}

QDomElement Ksp::saveNamedAreas(NamedAreaManager *manager, QDomDocument& doc)
{
    QDomElement element = doc.createElement("areaname");
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i) {
        QDomElement e = doc.createElement("reference");
        QDomElement tabname = doc.createElement("tabname");
        tabname.appendChild(doc.createTextNode(namedAreas[i].sheet->sheetName()));
        e.appendChild(tabname);

        QDomElement refname = doc.createElement("refname");
        refname.appendChild(doc.createTextNode(namedAreas[i].name));
        e.appendChild(refname);

        QDomElement rect = doc.createElement("rect");
        rect.setAttribute("left-rect", QString::number((namedAreas[i].range).left()));
        rect.setAttribute("right-rect", QString::number((namedAreas[i].range).right()));
        rect.setAttribute("top-rect", QString::number((namedAreas[i].range).top()));
        rect.setAttribute("bottom-rect", QString::number((namedAreas[i].range).bottom()));
        e.appendChild(rect);
        element.appendChild(e);
    }
    return element;
}

void Ksp::loadLocalization (Localization *l, const KoXmlElement& element)
{
    if (element.hasAttribute("weekStartsMonday")) {
        QString c = element.attribute("weekStartsMonday");
        if (c != "False") {
            l->setWeekStartDay(1 /*Monday*/);
        }
    }
    if (element.hasAttribute("decimalSymbol"))
        l->setDecimalSymbol(element.attribute("decimalSymbol"));
    if (element.hasAttribute("thousandsSeparator"))
        l->setThousandsSeparator(element.attribute("thousandsSeparator"));
    if (element.hasAttribute("currencySymbol"))
        l->setCurrencySymbol(element.attribute("currencySymbol"));
    if (element.hasAttribute("monetaryDecimalSymbol"))
        l->setMonetaryDecimalSymbol(element.attribute("monetaryDecimalSymbol"));
    if (element.hasAttribute("monetaryThousandsSeparator"))
        l->setMonetaryThousandsSeparator(element.attribute("monetaryThousandsSeparator"));
    if (element.hasAttribute("positiveSign"))
        l->setPositiveSign(element.attribute("positiveSign"));
    if (element.hasAttribute("negativeSign"))
        l->setNegativeSign(element.attribute("negativeSign"));
    if (element.hasAttribute("fracDigits"))
        l->setMonetaryDecimalPlaces(element.attribute("fracDigits").toInt());
    if (element.hasAttribute("positivePrefixCurrencySymbol")) {
        QString c = element.attribute("positivePrefixCurrencySymbol");
        l->setPositivePrefixCurrencySymbol(c == "True");
    }
    if (element.hasAttribute("negativePrefixCurrencySymbol")) {
        QString c = element.attribute("negativePrefixCurrencySymbol");
        l->setNegativePrefixCurrencySymbol(c == "True");
    }
    if (element.hasAttribute("positiveMonetarySignPosition"))
        l->setPositiveMonetarySignPosition((SignPosition)element.attribute("positiveMonetarySignPosition").toInt());
    if (element.hasAttribute("negativeMonetarySignPosition"))
        l->setNegativeMonetarySignPosition((SignPosition)element.attribute("negativeMonetarySignPosition").toInt());
    if (element.hasAttribute("timeFormat"))
        l->setTimeFormat(element.attribute("timeFormat"));
    if (element.hasAttribute("dateFormat"))
        l->setDateFormat(element.attribute("dateFormat"));
    if (element.hasAttribute("dateFormatShort"))
        l->setDateFormatShort(element.attribute("dateFormatShort"));
}

QDomElement saveLocalization (Localization *l, QDomDocument& doc)
{
    QDomElement element = doc.createElement("locale");

    element.setAttribute("weekStartsMonday", (l->weekStartDay() == 1) ? "True" : "False");
    element.setAttribute("decimalSymbol", l->decimalSymbol());
    element.setAttribute("thousandsSeparator", l->thousandsSeparator());
    element.setAttribute("currencySymbol", l->currencySymbol());
    element.setAttribute("monetaryDecimalSymbol", l->monetaryDecimalSymbol());
    element.setAttribute("monetaryThousandsSeparator", l->monetaryThousandsSeparator());
    element.setAttribute("positiveSign", l->positiveSign());
    element.setAttribute("negativeSign", l->negativeSign());
    element.setAttribute("fracDigits", QString::number(l->monetaryDecimalPlaces()));
    element.setAttribute("positivePrefixCurrencySymbol", l->positivePrefixCurrencySymbol() ? "True" : "False");
    element.setAttribute("negativePrefixCurrencySymbol", l->negativePrefixCurrencySymbol() ? "True" : "False");
    element.setAttribute("positiveMonetarySignPosition", QString::number((int)l->positiveMonetarySignPosition()));
    element.setAttribute("negativeMonetarySignPosition", QString::number((int)l->negativeMonetarySignPosition()));
    element.setAttribute("timeFormat", l->timeFormat());
    element.setAttribute("dateFormat", l->dateFormat());
    element.setAttribute("dateFormatShort", l->dateFormatShort());

    return element;
}




}  // Sheets
}  // Calligra
