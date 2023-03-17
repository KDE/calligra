/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   SPDX-FileCopyrightText: 2004-2005, 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2004-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 1999-2002, 2004, 2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2003 Reinhart Geiser <geiseri@kde.org>
   SPDX-FileCopyrightText: 2003-2005 Meni Livne <livne@kde.org>
   SPDX-FileCopyrightText: 2003 Peter Simonsson <psn@linux.se>
   SPDX-FileCopyrightText: 1999-2002 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2000-2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999, 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000 Bernd Wuebben <wuebben@kde.org>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org
   SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
   SPDX-FileCopyrightText: 1999 Michael Reiher <michael.reiher@gmx.de>
   SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   SPDX-FileCopyrightText: 1998-1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "SheetsKsp.h"
#include "SheetsKspPrivate.h"

#include <QDomDocument>

#include <KoXmlReader.h>

#include "engine/calligra_sheets_limits.h"
#include "engine/CalculationSettings.h"
#include "engine/Localization.h"

#include "Cell.h"
#include "Condition.h"
#include "Map.h"
#include "Sheet.h"

#include "float.h"

namespace Calligra {
namespace Sheets {

namespace Ksp {
    bool loadCellData(Cell *cell, const KoXmlElement & text, const QString &_dataType);
    bool saveCellResult(Cell *cell, QDomDocument& doc, QDomElement& result, QString str);
}


Cell Ksp::loadCell(const KoXmlElement & cell, Sheet *sheet)
{
    bool ok;

    //
    // First of all determine in which row and column this
    // cell belongs.
    //
    int row = cell.attribute("row").toInt(&ok);
    if (!ok) return Cell();
    int column = cell.attribute("column").toInt(&ok);
    if (!ok) return Cell();

    // Validation
    if (row < 1 || row > KS_rowMax) {
        debugSheets << "Cell::load: Value out of range Cell:row=" << row;
        return Cell();
    }
    if (column < 1 || column > KS_colMax) {
        debugSheets << "Cell::load: Value out of range Cell:column=" << column;
        return Cell();
    }

    Cell res = Cell(sheet, column, row);

    //
    // Load formatting information.
    //
    KoXmlElement formatElement = cell.namedItem("format").toElement();
    if (!formatElement.isNull()) {
        int mergedXCells = 0;
        int mergedYCells = 0;
        if (formatElement.hasAttribute("colspan")) {
            int i = formatElement.attribute("colspan").toInt(&ok);
            if (!ok) return Cell();
            // Validation
            if (i < 0 || i > KS_spanMax) {
                debugSheets << "Value out of range Cell::colspan=" << i;
                return Cell();
            }
            if (i)
                mergedXCells = i;
        }

        if (formatElement.hasAttribute("rowspan")) {
            int i = formatElement.attribute("rowspan").toInt(&ok);
            if (!ok) return Cell();
            // Validation
            if (i < 0 || i > KS_spanMax) {
                debugSheets << "Value out of range Cell::rowspan=" << i;
                return Cell();
            }
            if (i)
                mergedYCells = i;
        }

        if (mergedXCells != 0 || mergedYCells != 0)
            res.mergeCells(column, row, mergedXCells, mergedYCells);

        Style style;
        if (!loadStyle(&style, formatElement))
            return Cell();
        res.setStyle(style);
    }

    //
    // Load the condition section of a cell.
    //
    MapBase *const map = sheet->map();
    ValueParser *const valueParser = map->parser();
    KoXmlElement conditionsElement = cell.namedItem("condition").toElement();
    if (!conditionsElement.isNull()) {
        Conditions conditions;
        loadConditions(&conditions, conditionsElement, valueParser);
        if (!conditions.isEmpty())
            res.setConditions(conditions);
    }

    KoXmlElement validityElement = cell.namedItem("validity").toElement();
    if (!validityElement.isNull()) {
        Validity validity;
        if (loadValidity(&validity, valueParser, validityElement))
            res.setValidity(validity);
    }

    //
    // Load the comment
    //
    KoXmlElement comment = cell.namedItem("comment").toElement();
    if (!comment.isNull()) {
        QString t = comment.text();
        //t = t.trimmed();
        res.setComment(t);
    }

    //
    // The real content of the cell is loaded here. It is stored in
    // the "text" tag, which contains either a text or a CDATA section.
    //
    // TODO: make this suck less. We set data twice, in loadCellData, and
    // also here. Not good.
    KoXmlElement text = cell.namedItem("text").toElement();

    if (!text.isNull()) {

        /* older versions mistakenly put the datatype attribute on the cell instead
           of the text. Just move it over in case we're parsing an old document */
        QString dataType;
        if (cell.hasAttribute("dataType"))     // new docs
            dataType = cell.attribute("dataType");

        KoXmlElement result = cell.namedItem("result").toElement();
        QString txt = text.text();
        loadCellData(&res, text, dataType);

        if (!result.isNull()) {
            QString dataType;
            QString t = result.text();

            if (result.hasAttribute("dataType"))
                dataType = result.attribute("dataType");

            // boolean ?
            if (dataType == "Bool") {
                if (t == "false")
                    res.setValue(Value(false));
                else if (t == "true")
                    res.setValue(Value(true));
            } else if (dataType == "Num") {
                bool ok = false;
                double dd = t.toDouble(&ok);
                if (ok)
                    res.setValue(Value(dd));
            } else if (dataType == "Date") {
                bool ok = false;
                double dd = t.toDouble(&ok);
                if (ok) {
                    Value value(dd);
                    value.setFormat(Value::fmt_Date);
                    res.setValue(value);
                } else {
                    int pos   = t.indexOf('/');
                    int year  = t.midRef(0, pos).toInt();
                    int pos1  = t.indexOf('/', pos + 1);
                    int month = t.midRef(pos + 1, ((pos1 - 1) - pos)).toInt();
                    int day   = t.rightRef(t.length() - pos1 - 1).toInt();
                    QDate date(year, month, day);
                    if (date.isValid())
                        res.setValue(Value(date, sheet->map()->calculationSettings()));
                }
            } else if (dataType == "Time") {
                bool ok = false;
                double dd = t.toDouble(&ok);
                if (ok) {
                    Value value(dd);
                    value.setFormat(Value::fmt_Time);
                    res.setValue(value);
                } else {
                    int hours   = -1;
                    int minutes = -1;
                    int second  = -1;
                    int pos, pos1;
                    pos   = t.indexOf(':');
                    hours = t.midRef(0, pos).toInt();
                    pos1  = t.indexOf(':', pos + 1);
                    minutes = t.midRef(pos + 1, ((pos1 - 1) - pos)).toInt();
                    second  = t.rightRef(t.length() - pos1 - 1).toInt();
                    QTime time(hours, minutes, second);
                    if (time.isValid())
                        res.setValue(Value(time));
                }
            } else {
                res.setValue(Value(t));
            }
        }
    }

    return res;
}

bool Ksp::loadCellData(Cell *cell, const KoXmlElement & text, const QString &_dataType)
{
    //TODO: use converter()->asString() to generate userInput()

    QString t = text.text();
    t = t.trimmed();

    CalculationSettings *sett = cell->sheet()->map()->calculationSettings();

    // A formula like =A1+A2 ?
    if ((!t.isEmpty()) && (t[0] == '=')) {
        t = cell->decodeFormula(t);
        cell->parseUserInput(t);
    }
    // rich text ?
    else if ((!t.isEmpty()) && (t[0] == '!')) {
        // KSpread pre 1.4 stores hyperlink as rich text (first char is '!')
        // extract the link and the corresponding text
        // This is a rather dirty hack, but enough for Calligra Sheets generated XML
        bool inside_tag = false;
        QString qml_text;
        QString tag;
        QString qml_link;

        for (int i = 1; i < t.length(); ++i) {
            QChar ch = t[i];
            if (ch == '<') {
                if (!inside_tag) {
                    inside_tag = true;
                    tag.clear();
                }
            } else if (ch == '>') {
                if (inside_tag) {
                    inside_tag = false;
                    if (tag.startsWith(QLatin1String("a href=\""), Qt::CaseSensitive) &&
                        tag.endsWith(QLatin1Char('"'))) {
                        qml_link.remove(0, 8).chop(1);
                    }
                    tag.clear();
                }
            } else {
                if (!inside_tag)
                    qml_text += ch;
                else
                    tag += ch;
            }
        }

        if (!qml_link.isEmpty())
            cell->setLink(qml_link);
        cell->setUserInput(qml_text);
        cell->setValue(Value(qml_text));
    } else {
        bool newStyleLoading = true;
        QString dataType = _dataType;

        if (dataType.isNull()) {
            if (text.hasAttribute("dataType")) {   // new docs
                dataType = text.attribute("dataType");
            } else { // old docs: do the ugly solution of parsing the text
                // ...except for date/time
                if (cell->isDate() && (t.count('/') == 2))
                    dataType = "Date";
                else if (cell->isTime() && (t.count(':') == 2))
                    dataType = "Time";
                else {
                    cell->parseUserInput(t);
                    newStyleLoading = false;
                }
            }
        }

        if (newStyleLoading) {
            const Localization* locale = sett->locale();
            // boolean ?
            if (dataType == "Bool")
                cell->setValue(Value(t.toLower() == "true"));

            // number ?
            else if (dataType == "Num") {
                bool ok = false;
                if (t.contains('.'))
                    cell->setValue(Value(t.toDouble(&ok)));      // We save in non-localized format
                else
                    cell->setValue(Value((int64_t) t.toLongLong(&ok)));
                if (!ok) {
                    warnSheets << "Couldn't parse '" << t << "' as number.";
                }
                /* We will need to localize the text version of the number */

                /* KLocale::formatNumber requires the precision we want to return.
                */
                int precision = t.length() - t.indexOf('.') - 1;

                if (cell->style().formatType() == Format::Percentage) {
                    if (cell->value().isInteger())
                        t = locale->formatNumber(cell->value().asInteger() * 100);
                    else
                        t = locale->formatNumber(numToDouble(cell->value().asFloat() * 100.0), precision);
                    cell->setUserInput(t + "%");
                } else {
                    if (cell->value().isInteger())
                        t = locale->formatNumber(cell->value().asInteger(), 0);
                    else
                        t = locale->formatNumber(numToDouble(cell->value().asFloat()), precision);
                    cell->setUserInput(t);
                }
            }

            // date ?
            else if (dataType == "Date") {
                int pos = t.indexOf('/');
                int year = t.midRef(0, pos).toInt();
                int pos1 = t.indexOf('/', pos + 1);
                int month = t.midRef(pos + 1, ((pos1 - 1) - pos)).toInt();
                int day = t.rightRef(t.length() - pos1 - 1).toInt();
                cell->setValue(Value(QDate(year, month, day), sett));
                if (cell->value().asDate(sett).isValid())   // Should always be the case for new docs
                    cell->setUserInput(locale->formatDate(cell->value().asDate(sett), false));
                else { // This happens with old docs, when format is set wrongly to date
                    cell->parseUserInput(t);
                }
            }

            // time ?
            else if (dataType == "Time") {
                int hours = -1;
                int minutes = -1;
                int second = -1;
                int pos, pos1;
                pos = t.indexOf(':');
                hours = t.midRef(0, pos).toInt();
                pos1 = t.indexOf(':', pos + 1);
                minutes = t.midRef(pos + 1, ((pos1 - 1) - pos)).toInt();
                second = t.rightRef(t.length() - pos1 - 1).toInt();
                cell->setValue(Value(QTime(hours, minutes, second)));
                if (cell->value().asTime().isValid())    // Should always be the case for new docs
                    cell->setUserInput(locale->formatTime(cell->value().asTime(), true));
                else { // This happens with old docs, when format is set wrongly to time
                    cell->parseUserInput(t);
                }
            }

            else {
                // Set the cell's text
                cell->setUserInput(t);
                cell->setValue(Value(cell->userInput()));
            }
        }
    }

    if (!cell->sheet()->map()->isLoading())
        cell->parseUserInput(cell->userInput());

    return true;
}


QDomElement Ksp::saveCell(Cell *obj, QDomDocument& doc, int xOffset, int yOffset, bool era)
{
    // Save the position of this cell
    QDomElement cell = doc.createElement("cell");
    cell.setAttribute("row", QString::number(obj->row() - yOffset));
    cell.setAttribute("column", QString::number(obj->column() - xOffset));

    //
    // Save the formatting information
    //
    QDomElement formatElement(doc.createElement("format"));
    saveStyle (obj->style(), doc, formatElement, obj->fullSheet()->fullMap()->styleManager());
    if (formatElement.hasChildNodes() || formatElement.attributes().length())   // don't save empty tags
        cell.appendChild(formatElement);

    if (obj->doesMergeCells()) {
        if (obj->mergedXCells())
            formatElement.setAttribute("colspan", QString::number(obj->mergedXCells()));
        if (obj->mergedYCells())
            formatElement.setAttribute("rowspan", QString::number(obj->mergedYCells()));
    }

    Conditions conditions = obj->conditions();
    if (!conditions.isEmpty()) {
        QDomElement conditionElement = saveConditions(&conditions, doc, obj->sheet()->map()->converter());
        if (!conditionElement.isNull())
            cell.appendChild(conditionElement);
    }

    Validity validity = obj->validity();
    if (!validity.isEmpty()) {
        QDomElement validityElement = saveValidity (doc, &validity, obj->sheet()->map()->converter());
        if (!validityElement.isNull())
            cell.appendChild(validityElement);
    }

    const QString comment = obj->comment();
    if (!comment.isEmpty()) {
        QDomElement commentElement = doc.createElement("comment");
        commentElement.appendChild(doc.createCDATASection(comment));
        cell.appendChild(commentElement);
    }

    //
    // Save the text
    //
    if (!obj->userInput().isEmpty()) {
        // Formulas need to be encoded to ensure that they
        // are position independent.
        if (obj->isFormula()) {
            QDomElement txt = doc.createElement("text");
            // if we are cutting to the clipboard, relative references need to be encoded absolutely
            txt.appendChild(doc.createTextNode(obj->encodeFormula(era)));
            cell.appendChild(txt);

            /* we still want to save the results of the formula */
            QDomElement formulaResult = doc.createElement("result");
            saveCellResult(obj, doc, formulaResult, obj->displayText());
            cell.appendChild(formulaResult);

        } else if (!obj->link().isEmpty()) {
            // KSpread pre 1.4 saves link as rich text, marked with first char '
            // Have to be saved in some CDATA section because of too many special characters.
            QDomElement txt = doc.createElement("text");
            QString qml = "!<a href=\"" + obj->link() + "\">" + obj->userInput() + "</a>";
            txt.appendChild(doc.createCDATASection(qml));
            cell.appendChild(txt);
        } else {
            // Save the cell contents (in a locale-independent way)
            QDomElement txt = doc.createElement("text");
            saveCellResult(obj, doc, txt, obj->userInput());
            cell.appendChild(txt);
        }
    }
    if (cell.hasChildNodes() || cell.attributes().length() > 2)   // don't save empty tags
        // (the >2 is due to "row" and "column" attributes)
        return cell;
    else
        return QDomElement();
}

bool Ksp::saveCellResult(Cell *cell, QDomDocument& doc, QDomElement& result, QString str)
{
    QString dataType = "Other"; // fallback

    Value val = cell->value();
    CalculationSettings *sett = cell->sheet()->map()->calculationSettings();
    if (val.isNumber()) {
        if (cell->isDate()) {
            // serial number of date
            QDate dd = val.asDateTime(sett).date();
            dataType = "Date";
            str = "%1/%2/%3";
            str = str.arg(dd.year()).arg(dd.month()).arg(dd.day());
        } else if (cell->isTime()) {
            // serial number of time
            dataType = "Time";
            str = val.asDateTime(sett).time().toString();
        } else {
            // real number
            dataType = "Num";
            if (val.isInteger())
                str = QString::number(val.asInteger());
            else
                str = QString::number(numToDouble(val.asFloat()), 'g', DBL_DIG);
        }
    }

    if (val.isBoolean()) {
        dataType = "Bool";
        str = val.asBoolean() ? "true" : "false";
    }

    if (val.isString()) {
        dataType = "Str";
        str = val.asString();
    }

    result.setAttribute("dataType", dataType);

    const QString displayText = cell->displayText();
    if (!displayText.isEmpty())
        result.setAttribute("outStr", displayText);
    result.appendChild(doc.createTextNode(str));

    return true; /* really isn't much of a way for this function to fail */
}



}  // Sheets
}  // Calligra
