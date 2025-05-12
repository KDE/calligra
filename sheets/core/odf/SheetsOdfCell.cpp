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

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include <KoCharacterStyle.h>
#include <KoGenStyles.h>
#include <KoShape.h>
#include <KoShapeRegistry.h>
#include <KoStyleManager.h>
#include <KoTextDocument.h>
#include <KoTextLoader.h>
#include <KoTextWriter.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "engine/CalculationSettings.h"
#include "engine/Formula.h"
#include "engine/Localization.h"
#include "engine/Util.h"
#include "engine/Value.h"
#include "engine/ValueConverter.h"

#include "Cell.h"
#include "CellStorage.h"
#include "ColFormatStorage.h"
#include "Condition.h"
#include "Map.h"
#include "RowFormatStorage.h"
#include "ShapeApplicationData.h"
#include "Sheet.h"
#include "StyleManager.h"
#include "ValueFormatter.h"

#include <QRegularExpression>

#include <cfloat>

// This file contains functionality to load/save a Cell

namespace Calligra
{
namespace Sheets
{

namespace Odf
{

// cell loading - helper functions
void loadCellText(Cell *cell, const KoXmlElement &parent, OdfLoadingContext &tableContext, const Styles &autoStyles, const QString &cellStyleName);
QString loadCellTextNodes(Cell *cell, const KoXmlElement &element, int *textFragmentCount, int *lineCount, bool *hasRichText, bool *stripLeadingSpace);
void loadObjects(Cell *cell, const KoXmlElement &parent, OdfLoadingContext &tableContext, QList<ShapeLoadingData> &shapeData);
ShapeLoadingData loadObject(Cell *cell, const KoXmlElement &element, KoShapeLoadingContext &shapeContext);

// cell saving - helper functions
QString saveCellStyle(Cell *cell, KoGenStyle &currentCellStyle, KoGenStyles &mainStyles);
void saveCellAnnotation(Cell *cell, KoXmlWriter &xmlwriter);
void saveCellValue(Cell *cell, KoXmlWriter &xmlWriter);
}

// *************** Loading *****************
bool Odf::loadCell(Cell *cell,
                   const KoXmlElement &element,
                   OdfLoadingContext &tableContext,
                   const Styles &autoStyles,
                   const QString &cellStyleName,
                   QList<ShapeLoadingData> &shapeData)
{
    static const QString sFormula = QString::fromLatin1("formula");
    static const QString sValidationName = QString::fromLatin1("validation-name");
    static const QString sValueType = QString::fromLatin1("value-type");
    static const QString sBoolean = QString::fromLatin1("boolean");
    static const QString sBooleanValue = QString::fromLatin1("boolean-value");
    static const QString sTrue = QString::fromLatin1("true");
    static const QString sFalse = QString::fromLatin1("false");
    static const QString sFloat = QString::fromLatin1("float");
    static const QString sValue = QString::fromLatin1("value");
    static const QString sCurrency = QString::fromLatin1("currency");
    static const QString sPercentage = QString::fromLatin1("percentage");
    static const QString sDate = QString::fromLatin1("date");
    static const QString sDateValue = QString::fromLatin1("date-value");
    static const QString sTime = QString::fromLatin1("time");
    static const QString sTimeValue = QString::fromLatin1("time-value");
    static const QString sString = QString::fromLatin1("string");
    static const QString sStringValue = QString::fromLatin1("string-value");
    static const QString sNumberColumnsSpanned = QString::fromLatin1("number-columns-spanned");
    static const QString sNumberRowsSpanned = QString::fromLatin1("number-rows-spanned");
    static const QString sAnnotation = QString::fromLatin1("annotation");
    static const QString sP = QString::fromLatin1("p");

    static const QStringList formulaNSPrefixes = QStringList() << "oooc:"
                                                               << "kspr:"
                                                               << "of:"
                                                               << "msoxl:";

    // Search and load each paragraph of text. Each paragraph is separated by a line break.
    loadCellText(cell, element, tableContext, autoStyles, cellStyleName);

    //
    // formula
    //
    bool isFormula = false;
    if (element.hasAttributeNS(KoXmlNS::table, sFormula)) {
        isFormula = true;
        QString oasisFormula(element.attributeNS(KoXmlNS::table, sFormula, QString()));
        // debugSheetsODF << "cell:" << cell->name() << "formula :" << oasisFormula;
        // each spreadsheet application likes to safe formulas with a different namespace
        // prefix, so remove all of them
        QString namespacePrefix;
        foreach (const QString &prefix, formulaNSPrefixes) {
            if (oasisFormula.startsWith(prefix)) {
                oasisFormula.remove(0, prefix.length());
                namespacePrefix = prefix;
                break;
            }
        }
        oasisFormula = Odf::decodeFormula(oasisFormula, cell->locale(), namespacePrefix);
        cell->setUserInput(oasisFormula);
    } else if (!cell->userInput().isEmpty() && cell->userInput().at(0) == '=') // prepend ' to the text to avoid = to be painted
        cell->setUserInput(cell->userInput().prepend('\''));

    //
    // validation
    //
    if (element.hasAttributeNS(KoXmlNS::table, sValidationName)) {
        const QString validationName = element.attributeNS(KoXmlNS::table, sValidationName, QString());
        debugSheetsODF << "cell:" << cell->name() << sValidationName << validationName;
        Validity validity;
        loadValidation(&validity, cell, validationName, tableContext);
        if (!validity.isEmpty())
            cell->setValidity(validity);
    }

    //
    // value type
    //
    if (element.hasAttributeNS(KoXmlNS::office, sValueType)) {
        const QString valuetype = element.attributeNS(KoXmlNS::office, sValueType, QString());
        // debugSheetsODF << "cell:" << cell->name() << "value-type:" << valuetype;
        if (valuetype == sBoolean) {
            const QString val = element.attributeNS(KoXmlNS::office, sBooleanValue, QString()).toLower();
            if ((val == sTrue) || (val == sFalse))
                cell->setValue(Value(val == sTrue));
        }

        // integer and floating-point value
        else if (valuetype == sFloat) {
            bool ok = false;
            Value value(element.attributeNS(KoXmlNS::office, sValue, QString()).toDouble(&ok));
            if (ok) {
                value.setFormat(Value::fmt_Number);
                cell->setValue(value);
#if 0
                Style style;
                style.setFormatType(Format::Number);
                cell->setStyle(style);
#endif
            }
            // always set the userInput to the actual value read from the cell, and not whatever happens to be set as text, as the textual representation of a
            // value may be less accurate than the value itself
            if (!isFormula)
                cell->setUserInput(cell->sheet()->map()->converter()->asString(value).asString());
        }

        // currency value
        else if (valuetype == sCurrency) {
            bool ok = false;
            Value value(element.attributeNS(KoXmlNS::office, sValue, QString()).toDouble(&ok));
            if (ok) {
                value.setFormat(Value::fmt_Money);
                cell->setValue(value);

                Currency currency;
                if (element.hasAttributeNS(KoXmlNS::office, sCurrency)) {
                    currency = Currency(element.attributeNS(KoXmlNS::office, sCurrency, QString()));
                }
                /* TODO: somehow make this work again, all setStyle calls here will be overwritten by cell styles later
                if( style.isEmpty() ) {
                    Style style;
                    style.setCurrency(currency);
                    setStyle(style);
                } */
            }
        } else if (valuetype == sPercentage) {
            bool ok = false;
            Value value(element.attributeNS(KoXmlNS::office, sValue, QString()).toDouble(&ok));
            if (ok) {
                value.setFormat(Value::fmt_Percent);
                cell->setValue(value);
                if (!isFormula && cell->userInput().isEmpty())
                    cell->setUserInput(cell->sheet()->map()->converter()->asString(value).asString());
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
#if 0
                Style style;
                style.setFormatType(Format::Percentage);
                setStyle(style);
#endif
            }
        } else if (valuetype == sDate) {
            QStringView value = element.attributeNS(KoXmlNS::office, sDateValue, QString());
            // "1980-10-15" or "2001-01-01T19:27:41.123456"
            int year = 0, month = 0, day = 0, hours = 0, minutes = 0;
            Number seconds = 0.0; // seconds may have fractions
            bool hasTime = false;
            bool ok = false;

            int p1 = value.indexOf('-');
            if (p1 > 0) {
                year = value.left(p1).toInt(&ok);
                if (ok) {
                    int p2 = value.indexOf('-', ++p1);
                    month = value.mid(p1, p2 - p1).toInt(&ok);
                    if (ok) {
                        // the date can optionally have a time attached
                        int p3 = value.indexOf('T', ++p2);
                        if (p3 > 0) {
                            hasTime = true;
                            day = value.mid(p2, p3 - p2).toInt(&ok);
                            if (ok) {
                                int p4 = value.indexOf(':', ++p3);
                                hours = value.mid(p3, p4 - p3).toInt(&ok);
                                if (ok) {
                                    int p5 = value.indexOf(':', ++p4);
                                    minutes = value.mid(p4, p5 - p4).toInt(&ok);
                                    if (ok) {
                                        seconds = value.right(value.length() - p5 - 1).toDouble(&ok);
                                    }
                                }
                            }
                        } else {
                            day = value.right(value.length() - p2).toInt(&ok);
                        }
                    }
                }
            }

            if (ok) {
                const auto settings = cell->sheet()->map()->calculationSettings();
                if (hasTime) {
                    const QDateTime ref(settings->referenceDate(), QTime(), Qt::UTC);
                    // handle fractions of seconds so not to loose precision
                    const QDateTime dt(QDate(year, month, day), QTime(hours, minutes, 0), Qt::UTC);
                    Number v = (Number(ref.msecsTo(dt)) / (24 * 60 * 60 * 1000)) + (seconds / (24 * 60 * 60));
                    Value value(v);
                    value.setFormat(Value::fmt_Date);
                    cell->setValue(value);
                } else {
                    cell->setValue(Value(QDate(year, month, day), settings));
                }
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
// Sebsauer: Fixed now. Value::Format handles it correct.
#if 0
                Style s;
                s.setFormatType(Format::ShortDate);
                setStyle(s);
#endif
                // debugSheetsODF << "cell:" << cell->name() << "Type: date, value:" << value << "Date:" << year << " -" << month << " -" << day;
            } else {
                warnSheetsODF << "Could not parse date:" << value;
            }
        } else if (valuetype == sTime) {
            QString value = element.attributeNS(KoXmlNS::office, sTimeValue, QString());

            // "[-]PT98H10M12.245678S"
            Number number = 0.0; // resulting duration in days
            int l = value.length();
            QString num;
            bool ok = false;
            for (int i = 0; i < l; ++i) {
                if (value[i].isNumber() || value[i] == '.') {
                    num += value[i];
                    continue;
                } else if (value[i] == 'H') {
                    number = num.toDouble(&ok) / 24;
                } else if (value[i] == 'M') {
                    number += num.toDouble(&ok) / (24 * 60);
                } else if (value[i] == 'S') {
                    number += num.toDouble(&ok) / (24 * 60 * 60);
                } else {
                    continue;
                }
                // debugSheetsODF << "Num:" << num;
                num.clear();
                if (!ok)
                    break;
            }

            if (ok) {
                // handle negative durations
                number = value.startsWith('-') ? -number : number;
                Value v(number);
                v.setFormat(Value::fmt_Time);
                cell->setValue(v);
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
#if 0
                Style style;
                style.setFormatType(Format::Time);
                setStyle(style);
#endif
                // debugSheetsODF << "cell:" << cell->name() << "Type: time:" << value << "Hours:" << hours << "," << minutes << "," << seconds;
            }
        } else if (valuetype == sString) {
            if (element.hasAttributeNS(KoXmlNS::office, sStringValue)) {
                QString value = element.attributeNS(KoXmlNS::office, sStringValue, QString());
                cell->setValue(Value(value));
            } else {
                // use the paragraph(s) read in before
                cell->setValue(Value(cell->userInput()));
            }
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
#if 0
            Style style;
            style.setFormatType(Format::Text);
            setStyle(style);
#endif
        } else {
            // debugSheetsODF << "cell:" << cell->name() << "  Unknown type. Parsing user input.";
            // Set the value by parsing the user input.
            cell->parseUserInput(cell->userInput());
        }
    } else { // no value-type attribute
        // debugSheetsODF << "cell:" << cell->name() << "  No value type specified. Parsing user input.";
        // Set the value by parsing the user input.
        cell->parseUserInput(cell->userInput());
    }

    //
    // merged cells ?
    //
    int colSpan = 1;
    int rowSpan = 1;
    if (element.hasAttributeNS(KoXmlNS::table, sNumberColumnsSpanned)) {
        bool ok = false;
        int span = element.attributeNS(KoXmlNS::table, sNumberColumnsSpanned, QString()).toInt(&ok);
        if (ok)
            colSpan = span;
    }
    if (element.hasAttributeNS(KoXmlNS::table, sNumberRowsSpanned)) {
        bool ok = false;
        int span = element.attributeNS(KoXmlNS::table, sNumberRowsSpanned, QString()).toInt(&ok);
        if (ok)
            rowSpan = span;
    }
    if (colSpan > 1 || rowSpan > 1)
        cell->mergeCells(cell->column(), cell->row(), colSpan - 1, rowSpan - 1);

    //
    // cell comment/annotation
    //
    KoXmlElement annotationElement = KoXml::namedItemNS(element, KoXmlNS::office, sAnnotation);
    if (!annotationElement.isNull()) {
        QString comment;
        KoXmlNode node = annotationElement.firstChild();
        while (!node.isNull()) {
            KoXmlElement commentElement = node.toElement();
            if (!commentElement.isNull())
                if (commentElement.localName() == sP && commentElement.namespaceURI() == KoXmlNS::text) {
                    if (!comment.isEmpty())
                        comment.append('\n');
                    comment.append(commentElement.text());
                }

            node = node.nextSibling();
        }
        if (!comment.isEmpty())
            cell->setComment(comment);
    }

    loadObjects(cell, element, tableContext, shapeData);

    return true;
}

bool Odf::saveCell(Cell *cell, int &repeated, OdfSavingContext &tableContext)
{
    KoXmlWriter &xmlwriter = tableContext.shapeContext.xmlWriter();
    KoGenStyles &mainStyles = tableContext.shapeContext.mainStyles();

    int row = cell->row();
    int column = cell->column();
    Sheet *sheet = cell->fullSheet();

    // see: OpenDocument, 8.1.3 Table Cell
    if (!cell->isPartOfMerged())
        xmlwriter.startElement("table:table-cell");
    else
        xmlwriter.startElement("table:covered-table-cell");
#if 0
    //add font style
    QFont font;
    Value const value(cell.value());
    if (!cell.isDefault()) {
        font = cell.format()->textFont(i, row);
        m_styles.addFont(font);

        if (cell.format()->hasProperty(Style::SComment))
            hasComment = true;
    }
#endif
    // NOTE save the value before the style as long as the Formatter does not work correctly

    // Do not save cell values for empty cells.
    // If we do, a 0 value will be displayed when the file is loaded again.
    // And we will be compatible with LO.
    if (!cell->displayText().isEmpty() && cell->link().isEmpty())
        saveCellValue(cell, xmlwriter);

    const Style cellStyle = cell->style();

    // Either there's no column and row default and the style's not the default style,
    // or the style is different to one of them. The row default takes precedence.
    if ((!tableContext.rowDefaultStyles.contains(row) && !tableContext.columnDefaultStyles.contains(column)
         && !(cellStyle.isDefault() && cell->conditions().isEmpty()))
        || (tableContext.rowDefaultStyles.contains(row) && tableContext.rowDefaultStyles[row] != cellStyle)
        || (tableContext.columnDefaultStyles.contains(column) && tableContext.columnDefaultStyles[column] != cellStyle)) {
        KoGenStyle currentCellStyle; // the type determined in saveCellStyle
        QString styleName = saveCellStyle(cell, currentCellStyle, mainStyles);
        // skip 'table:style-name' attribute for the default style
        if (!currentCellStyle.isDefaultStyle()) {
            if (!styleName.isEmpty())
                xmlwriter.addAttribute("table:style-name", styleName);
        }
    }

    // group empty cells with the same style
    const QString comment = cell->comment();
    if (cell->isEmpty() && comment.isEmpty() && !cell->isPartOfMerged() && !cell->doesMergeCells() && !tableContext.cellHasAnchoredShapes(sheet, row, column)) {
        bool refCellIsDefault = cell->isDefault();
        int j = column + 1;
        Cell nextCell = sheet->fullCellStorage()->nextInRow(column, row);
        while (!nextCell.isNull()) {
            // if
            //   the next cell is not the adjacent one
            // or
            //   the next cell is not empty
            if (nextCell.column() != j || (!nextCell.isEmpty() || tableContext.cellHasAnchoredShapes(sheet, row, column))) {
                if (refCellIsDefault) {
                    // if the origin cell was a default cell,
                    // we count the default cells
                    repeated = nextCell.column() - j + 1;

                    // check if any of the empty/default cells we skipped contained anchored shapes
                    int shapeColumn = tableContext.nextAnchoredShape(sheet, row, column);
                    if (shapeColumn) {
                        repeated = qMin(repeated, shapeColumn - column);
                    }
                }
                // otherwise we just stop here to process the adjacent
                // cell in the next iteration of the outer loop
                // (in saveCells)
                break;
            }

            if (nextCell.isPartOfMerged() || nextCell.doesMergeCells() || !nextCell.comment().isEmpty()
                || tableContext.cellHasAnchoredShapes(sheet, row, nextCell.column())
                || !(nextCell.style() == cellStyle && nextCell.conditions() == cell->conditions())) {
                break;
            }
            ++repeated;
            // get the next cell and set the index to the adjacent cell
            nextCell = sheet->fullCellStorage()->nextInRow(j++, row);
        }
        // debugSheetsODF << "Odf::saveCell: empty cell in column" << column
        //<< "repeated" << repeated << "time(s)" << Qt::endl;

        if (repeated > 1)
            xmlwriter.addAttribute("table:number-columns-repeated", QString::number(repeated));
    }

    Validity validity = cell->validity();
    if (!validity.isEmpty()) {
        GenValidationStyle styleVal(&validity, sheet->map()->calculationSettings());
        xmlwriter.addAttribute("table:validation-name", tableContext.valStyle.insert(styleVal));
    }
    if (cell->isFormula()) {
        // debugSheetsODF <<"Formula found";
        QString formula = Odf::encodeFormula(cell->userInput(), cell->locale());
        xmlwriter.addAttribute("table:formula", formula);
    }

    if (cell->doesMergeCells()) {
        int colSpan = cell->mergedXCells() + 1;
        int rowSpan = cell->mergedYCells() + 1;

        if (colSpan > 1)
            xmlwriter.addAttribute("table:number-columns-spanned", QString::number(colSpan));

        if (rowSpan > 1)
            xmlwriter.addAttribute("table:number-rows-spanned", QString::number(rowSpan));
    }

    saveCellAnnotation(cell, xmlwriter);

    if (!cell->isFormula() && !cell->link().isEmpty()) {
        // debugSheetsODF<<"Link found";
        xmlwriter.startElement("text:p");
        xmlwriter.startElement("text:a");
        const QString url = cell->link();
        // Reference cell is started by '#'
        if (Util::localReferenceAnchor(url))
            xmlwriter.addAttribute("xlink:href", ('#' + url));
        else
            xmlwriter.addAttribute("xlink:href", url);
        xmlwriter.addAttribute("xlink:type", "simple");
        xmlwriter.addTextNode(cell->userInput());
        xmlwriter.endElement();
        xmlwriter.endElement();
    }

    if (!cell->isEmpty() && cell->link().isEmpty()) {
        QSharedPointer<QTextDocument> doc = cell->richText();
        if (doc) {
            QTextCharFormat format = cell->style().asCharFormat();
            ((KoCharacterStyle *)sheet->fullMap()->textStyleManager()->defaultParagraphStyle())->copyProperties(format);

            KoTextWriter writer(tableContext.shapeContext);

            writer.write(doc.data(), 0);
        } else {
            xmlwriter.startElement("text:p");
            xmlwriter.addTextNode(cell->displayText().toUtf8());
            xmlwriter.endElement();
        }
    }

    // flake
    // Save shapes that are anchored to this cell.
    // see: OpenDocument, 2.3.1 Text Documents
    // see: OpenDocument, 9.2 Drawing Shapes
    if (tableContext.cellHasAnchoredShapes(sheet, row, column)) {
        const QList<KoShape *> shapes = tableContext.cellAnchoredShapes(sheet, row, column);
        for (int i = 0; i < shapes.count(); ++i) {
            KoShape *const shape = shapes[i];
            const QPointF bottomRight = shape->boundingRect().bottomRight();
            qreal endX = 0.0;
            qreal endY = 0.0;
            const int scol = sheet->leftColumn(bottomRight.x(), endX);
            const int srow = sheet->topRow(bottomRight.y(), endY);
            qreal offsetX = sheet->columnPosition(column);
            qreal offsetY = sheet->rowPosition(row);
            tableContext.shapeContext.addShapeOffset(shape, QTransform::fromTranslate(-offsetX, -offsetY));
            shape->setAdditionalAttribute("table:end-cell-address", saveRegion(Region(QPoint(scol, srow)).name()));
            shape->setAdditionalAttribute("table:end-x", QString::number(bottomRight.x() - endX) + "pt");
            shape->setAdditionalAttribute("table:end-y", QString::number(bottomRight.y() - endY) + "pt");
            shape->saveOdf(tableContext.shapeContext);
            shape->removeAdditionalAttribute("table:end-cell-address");
            shape->removeAdditionalAttribute("table:end-x");
            shape->removeAdditionalAttribute("table:end-y");
            tableContext.shapeContext.removeShapeOffset(shape);
        }
    }

    xmlwriter.endElement();
    return true;
}

// loading - helper functions

QString Odf::loadCellTextNodes(Cell *cell, const KoXmlElement &element, int *textFragmentCount, int *lineCount, bool *hasRichText, bool *stripLeadingSpace)
{
    QString cellText;
    bool countedOwnFragments = false;
    bool prevWasText = false;
    for (KoXmlNode n = element.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isText()) {
            prevWasText = true;
            QString t = KoTextLoader::normalizeWhitespace(n.toText().data(), *stripLeadingSpace);
            if (!t.isEmpty()) {
                *stripLeadingSpace = t[t.length() - 1].isSpace();
                cellText += t;
                if (!countedOwnFragments) {
                    // We only count the number of different parent elements which have text. That is
                    // so cause different parent-elements may mean different styles which means
                    // rich-text while the same parent element means the same style so we can easily
                    // put them together into one string.
                    countedOwnFragments = true;
                    ++(*textFragmentCount);
                }
            }
        } else {
            KoXmlElement e = n.toElement();
            if (!e.isNull()) {
                if (prevWasText && !cellText.isEmpty() && cellText[cellText.length() - 1].isSpace()) {
                    // A trailing space of the cellText collected so far needs to be preserved when
                    // more text-nodes within the same parent follow but if an element like e.g.
                    // text:s follows then a trailing space needs to be removed.
                    cellText.chop(1);
                }
                prevWasText = false;

                // We can optimize some elements like text:s (space), text:tab (tabulator) and
                // text:line-break (new-line) to not produce rich-text but add the equivalent
                // for them in plain-text.
                const bool isTextNs = e.namespaceURI() == KoXmlNS::text;
                if (isTextNs && e.localName() == "s") {
                    const int howmany = qMax(1, e.attributeNS(KoXmlNS::text, "c", QString()).toInt());
                    cellText += QString().fill(u' ', howmany);
                } else if (isTextNs && e.localName() == "tab") {
                    cellText += '\t';
                } else if (isTextNs && e.localName() == "line-break") {
                    cellText += '\n';
                    ++(*lineCount);
                } else if (isTextNs && e.localName() == "span") {
                    // Nested span-elements means recursive evaluation.
                    cellText += loadCellTextNodes(cell, e, textFragmentCount, lineCount, hasRichText, stripLeadingSpace);
                } else if (!isTextNs || (e.localName() != "annotation" && e.localName() != "bookmark" && e.localName() != "meta" && e.localName() != "tag")) {
                    // Seems we have an element we cannot easily translate to a string what
                    // means it's all rich-text now.
                    *hasRichText = true;
                }
            }
        }
    }
    return cellText;
}

// recursively goes through all children of parent and returns true if there is any element
// in the draw: namespace in this subtree
static bool findDrawElements(const KoXmlElement &parent)
{
    KoXmlElement element;
    forEachElement(element, parent)
    {
        if (element.namespaceURI() == KoXmlNS::draw)
            return true;
        if (findDrawElements(element))
            return true;
    }
    return false;
}

// Similar to KoXml::namedItemNS except that children of span tags will be evaluated too.
static KoXmlElement namedItemNSWithSpan(const KoXmlNode &node, const QString &nsURI, const QString &localName)
{
    KoXmlNode n = node.firstChild();
    for (; !n.isNull(); n = n.nextSibling()) {
        if (n.isElement()) {
            if (n.localName() == localName && n.namespaceURI() == nsURI) {
                return n.toElement();
            }
            if (n.localName() == "span" && n.namespaceURI() == nsURI) {
                KoXmlElement e = KoXml::namedItemNS(n, nsURI, localName); // not recursive
                if (!e.isNull()) {
                    return e;
                }
            }
        }
    }
    return KoXmlElement();
}

void Odf::loadCellText(Cell *cell, const KoXmlElement &parent, OdfLoadingContext &tableContext, const Styles &autoStyles, const QString &cellStyleName)
{
    // Search and load each paragraph of text. Each paragraph is separated by a line break
    KoXmlElement textParagraphElement;
    QString cellText;

    int lineCount = 0;
    bool hasRichText = false;
    bool stripLeadingSpace = true;

    forEachElement(textParagraphElement, parent)
    {
        if (textParagraphElement.localName() == "p" && textParagraphElement.namespaceURI() == KoXmlNS::text) {
            // the text:a link could be located within a text:span element
            KoXmlElement textA = namedItemNSWithSpan(textParagraphElement, KoXmlNS::text, "a");
            if (!textA.isNull() && textA.hasAttributeNS(KoXmlNS::xlink, "href")) {
                QString link = textA.attributeNS(KoXmlNS::xlink, "href", QString());
                cellText = textA.text();
                cell->setUserInput(cellText);
                hasRichText = false;
                lineCount = 0;
                // The value will be set later in loadOdf().
                if ((!link.isEmpty()) && (link[0] == '#'))
                    link.remove(0, 1);
                cell->setLink(link);
                // Abort here cause we can handle only either a link in a cell or (rich-)text but not both.
                break;
            }

            if (!cellText.isNull())
                cellText += '\n';

            ++lineCount;
            int textFragmentCount = 0;

            // Our text could contain formatting for value or result of formula or a mix of
            // multiple text:span elements with text-nodes and line-break's.
            cellText += loadCellTextNodes(cell, textParagraphElement, &textFragmentCount, &lineCount, &hasRichText, &stripLeadingSpace);

            // If we got text from multiple different sources (e.g. from the text:p and a
            // child text:span) then we have very likely rich-text.
            if (!hasRichText)
                hasRichText = textFragmentCount >= 2;
        }
    }

    if (!cellText.isNull()) {
        if (hasRichText && !findDrawElements(parent)) {
            // for now we don't support richtext and embedded shapes in the same cell;
            // this is because they would currently be loaded twice, once by the KoTextLoader
            // and later properly by the cell itself

            Map *map = cell->fullSheet()->fullMap();
            Style style;
            style.setDefault();
            if (!cellStyleName.isEmpty()) {
                if (autoStyles.contains(cellStyleName))
                    style.merge(autoStyles[cellStyleName]);
                else {
                    const CustomStyle *namedStyle = map->styleManager()->style(cellStyleName);
                    if (namedStyle)
                        style.merge(*namedStyle);
                }
            }

            QTextCharFormat format = style.asCharFormat();
            ((KoCharacterStyle *)map->textStyleManager()->defaultParagraphStyle())->copyProperties(format);

            QSharedPointer<QTextDocument> doc(new QTextDocument);
            KoTextDocument(doc.data()).setStyleManager(map->textStyleManager());

            Q_ASSERT(tableContext.shapeContext);
            KoTextLoader loader(*tableContext.shapeContext);
            QTextCursor cursor(doc.data());
            loader.loadBody(parent, cursor);

            cell->setUserInput(doc->toPlainText());
            cell->setRichText(doc);
        } else {
            cell->setUserInput(cellText);
        }
    }

    // enable word wrapping if multiple lines of text have been found.
    if (lineCount >= 2) {
        Style newStyle;
        newStyle.setWrapText(true);
        cell->setStyle(newStyle);
    }
}

void Odf::loadObjects(Cell *cell, const KoXmlElement &parent, OdfLoadingContext &tableContext, QList<ShapeLoadingData> &shapeData)
{
    // Register additional attributes, that identify shapes anchored in cells.
    // Their dimensions need adjustment after all rows are loaded,
    // because the position of the end cell is not always known yet.
    KoShapeLoadingContext::addAdditionalAttributeData(
        KoShapeLoadingContext::AdditionalAttributeData(KoXmlNS::table, "end-cell-address", "table:end-cell-address"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(KoXmlNS::table, "end-x", "table:end-x"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(KoXmlNS::table, "end-y", "table:end-y"));

    KoXmlElement element;
    forEachElement(element, parent)
    {
        if (element.namespaceURI() != KoXmlNS::draw)
            continue;

        if (element.localName() == "a") {
            // It may the case that the object(s) are embedded into a hyperlink so actions are done on
            // clicking it/them but since we do not supported objects-with-hyperlinks yet we just fetch
            // the inner elements and use them to at least create and show the objects (see bug 249862).
            KoXmlElement e;
            forEachElement(e, element)
            {
                if (e.namespaceURI() != KoXmlNS::draw)
                    continue;
                ShapeLoadingData data = loadObject(cell, e, *tableContext.shapeContext);
                if (data.shape) {
                    shapeData.append(data);
                }
            }
        } else {
            ShapeLoadingData data = loadObject(cell, element, *tableContext.shapeContext);
            if (data.shape) {
                shapeData.append(data);
            }
        }
    }
}

Odf::ShapeLoadingData Odf::loadObject(Cell *cell, const KoXmlElement &element, KoShapeLoadingContext &shapeContext)
{
    ShapeLoadingData data;
    data.shape = nullptr;
    KoShape *shape = KoShapeRegistry::instance()->createShapeFromOdf(element, shapeContext);
    if (!shape) {
        debugSheetsODF << "Unable to load shape with localName=" << element.localName();
        return data;
    }

    Sheet *sheet = cell->fullSheet();
    sheet->addShape(shape);

    // The position is relative to the upper left sheet corner until now. Move it.
    QPointF position = shape->position();
    // Remember how far we're off from the top-left corner of this cell
    double offsetX = position.x();
    double offsetY = position.y();
    for (int col = 1; col < cell->column(); ++col)
        position += QPointF(sheet->columnFormats()->colWidth(col), 0.0);
    if (cell->row() > 1)
        position += QPointF(0.0, sheet->rowFormats()->totalRowHeight(1, cell->row() - 1));
    shape->setPosition(position);

    dynamic_cast<ShapeApplicationData *>(shape->applicationData())->setAnchoredToCell(true);

    // All three attributes are necessary for cell anchored shapes.
    // Otherwise, they are anchored in the sheet.
    if (!shape->hasAdditionalAttribute("table:end-cell-address") || !shape->hasAdditionalAttribute("table:end-x")
        || !shape->hasAdditionalAttribute("table:end-y")) {
        debugSheetsODF << "Not all attributes found, that are necessary for cell anchoring.";
        return data;
    }

    Region endCell = cell->sheet()->map()->regionFromName(loadRegion(shape->additionalAttribute("table:end-cell-address")), cell->sheet());
    if (!endCell.isValid() || !endCell.isSingular())
        return data;

    QString string = shape->additionalAttribute("table:end-x");
    if (string.isNull())
        return data;
    double endX = KoUnit::parseValue(string);

    string = shape->additionalAttribute("table:end-y");
    if (string.isNull())
        return data;
    double endY = KoUnit::parseValue(string);

    data.shape = shape;
    data.startCell = QPoint(cell->column(), cell->row());
    data.offset = QPointF(offsetX, offsetY);
    data.endCell = endCell;
    data.endPoint = QPointF(endX, endY);

    // The column dimensions are already the final ones, but not the row dimensions.
    // The default height is used for the not yet loaded rows.
    // TODO Stefan: Honor non-default row heights later!
    // subtract offset because the accumulated width and height we calculate below starts
    // at the top-left corner of this cell, but the shape can have an offset to that corner
    QSizeF size = QSizeF(endX - offsetX, endY - offsetY);
    for (int col = cell->column(); col < endCell.firstRange().left(); ++col)
        size += QSizeF(sheet->columnFormats()->colWidth(col), 0.0);
    if (endCell.firstRange().top() > cell->row())
        size += QSizeF(0.0, sheet->rowFormats()->totalRowHeight(cell->row(), endCell.firstRange().top() - 1));
    shape->setSize(size);

    return data;
}

// saving - helper functions

void Odf::saveCellAnnotation(Cell *cell, KoXmlWriter &xmlwriter)
{
    const QString comment = cell->comment();
    if (comment.isEmpty())
        return;

    //<office:annotation draw:style-name="gr1" draw:text-style-name="P1" svg:width="2.899cm" svg:height="2.691cm" svg:x="2.858cm" svg:y="0.001cm"
    // draw:caption-point-x="-2.858cm" draw:caption-point-y="-0.001cm">
    xmlwriter.startElement("office:annotation");
    const QStringList text = comment.split('\n', Qt::SkipEmptyParts);
    for (QStringList::ConstIterator it = text.begin(); it != text.end(); ++it) {
        xmlwriter.startElement("text:p");
        xmlwriter.addTextNode(*it);
        xmlwriter.endElement();
    }
    xmlwriter.endElement();
}

QString Odf::saveCellStyle(Cell *cell, KoGenStyle &currentCellStyle, KoGenStyles &mainStyles)
{
    const Conditions conditions = cell->conditions();
    if (!conditions.isEmpty()) {
        // this has to be an automatic style
        currentCellStyle = KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");
        saveConditions(&conditions, currentCellStyle, cell->sheet()->map()->calculationSettings());
    }
    Style style = cell->style();
    return saveStyle(&style, currentCellStyle, mainStyles, cell->fullSheet()->fullMap()->styleManager(), cell->sheet()->map()->calculationSettings()->locale());
}

QString Odf::toSaveString(const Value &value, const Value::Format format, CalculationSettings *cs)
{
    switch (format) {
    case Value::fmt_None:
        return QString(); // NOTHING HERE
    case Value::fmt_Boolean:
        return value.asBoolean() ? "true" : "false";
    case Value::fmt_Number: {
        if (value.isInteger())
            return QString::number(value.asInteger());
        return QString::number(numToDouble(value.asFloat()), 'g', DBL_DIG);
    }
    case Value::fmt_Percent:
        return QString::number((double)numToDouble(value.asFloat()));
    case Value::fmt_Money:
        return QString::number((double)numToDouble(value.asFloat()));
    case Value::fmt_DateTime:
    case Value::fmt_Date: {
        auto dt = value.asDateTime(cs);
        auto s = dt.toString(Qt::ISODateWithMs);
        // remove possible timespec info
        if (s.contains('Z')) {
            s.remove(s.indexOf('Z'), 1);
        }
        return s;
    }
    case Value::fmt_Time: {
        // handle negative durations, hours > 24 and seconds precision
        QString format("PT%1H%2M%3S");
        Number v = value.asFloat() * 24; // value in hours
        if (v < 0) {
            format.prepend('-');
            v = -v;
        }
        const int hours = (int)v;
        v = (v - hours) * 60;
        const int mins = (int)v;
        const double secs = (v - mins) * 60;

        const auto res = format.arg(QString::number(hours), 2, '0').arg(QString::number(mins), 2, '0').arg(QString::number(secs, 'g'), 2, '0');
        // debugSheetsODF<<value<<res;
        return res;
    }
    case Value::fmt_String:
        return value.asString();
    default:
        return value.asString();
    };
}

void Odf::saveCellValue(Cell *cell, KoXmlWriter &xmlWriter)
{
    Value value = cell->value();
    // Determine the format that we will be storing.
    // This is usually the format that is actually shown - doing so mixes style and content, but that's how
    // LO does it, so we need to stay compatible
    Format::Type shownFormat = cell->style().formatType();
    if (shownFormat == Format::Generic)
        shownFormat = cell->fullSheet()->fullMap()->formatter()->determineFormatting(value, shownFormat);
    Value::Format saveFormat = Value::fmt_None;
    Value::Format valueFormat = value.format();
    if (valueFormat == Value::fmt_Boolean)
        saveFormat = Value::fmt_Boolean;
    else if (valueFormat == Value::fmt_String) // if it's a text, it needs to be stored as a text
        saveFormat = Value::fmt_String;
    else if (Format::isDate(shownFormat) || Format::isDateTime(shownFormat))
        saveFormat = Value::fmt_Date;
    else if (Format::isTime(shownFormat))
        saveFormat = Value::fmt_Time;
    else if (Format::isNumber(shownFormat))
        saveFormat = Value::fmt_Number;
    else if (Format::isMoney(shownFormat))
        saveFormat = Value::fmt_Money;
    else if (shownFormat == Format::Percentage)
        saveFormat = Value::fmt_Percent;
    else if (shownFormat == Format::Text)
        saveFormat = Value::fmt_String;
    else if (shownFormat == Format::Custom)
        saveFormat = valueFormat;

    QString saveString = toSaveString(value, saveFormat, cell->sheet()->map()->calculationSettings());
    switch (saveFormat) {
    case Value::fmt_None:
        break; // NOTHING HERE
    case Value::fmt_Boolean: {
        xmlWriter.addAttribute("office:value-type", "boolean");
        xmlWriter.addAttribute("office:boolean-value", saveString);
        break;
    }
    case Value::fmt_Number: {
        xmlWriter.addAttribute("office:value-type", "float");
        xmlWriter.addAttribute("office:value", saveString);
        break;
    }
    case Value::fmt_Percent: {
        xmlWriter.addAttribute("office:value-type", "percentage");
        xmlWriter.addAttribute("office:value", saveString);
        break;
    }
    case Value::fmt_Money: {
        xmlWriter.addAttribute("office:value-type", "currency");
        const Style style = cell->style();
        if (style.hasAttribute(Style::CurrencyFormat)) {
            Currency currency = style.currency();
            xmlWriter.addAttribute("office:currency", currency.code());
        }
        xmlWriter.addAttribute("office:value", saveString);
        break;
    }
    case Value::fmt_Date: {
        xmlWriter.addAttribute("office:value-type", "date");
        xmlWriter.addAttribute("office:date-value", saveString);
        break;
    }
    case Value::fmt_Time: {
        xmlWriter.addAttribute("office:value-type", "time");
        xmlWriter.addAttribute("office:time-value", saveString);
        break;
    }
    case Value::fmt_String: {
        xmlWriter.addAttribute("office:value-type", "string");
        xmlWriter.addAttribute("office:string-value", saveString);
        break;
    }
    };
}

QString Odf::decodeFormula(const QString &expression_, const Localization *locale, const QString &namespacePrefix)
{
    // parsing state
    enum {
        Start,
        InNumber,
        InString,
        InIdentifier,
        InReference,
        InSheetName
    } state = Start;

    QString expression = expression_;
    if (namespacePrefix == "msoxl:") {
        expression = convertMSOOXMLFormula(expression);
    }

    // use locale settings
    QString decimal = locale ? locale->decimalSymbol() : ".";

    const QChar *data = expression.constData();
    const QChar *start = data;

    if (data->isNull()) {
        return QString();
    }

    int length = expression.length() * 2;
    QString result(length, QChar());
    result.reserve(length);
    QChar *out = result.data();
    QChar *outStart = result.data();

    if (*data == QChar('=', 0)) {
        *out = *data;
        ++data;
        ++out;
    }

    const QChar *pos = data;
    while (!data->isNull()) {
        switch (state) {
        case Start: {
            if (data->isDigit()) { // check for number
                state = InNumber;
                *out++ = *data++;
            } else if (*data == QChar('.', 0)) {
                state = InNumber;
                *out = decimal[0];
                ++out;
                ++data;
            } else if (Formula::isIdentifier(*data)) {
                // beginning with alphanumeric ?
                // could be identifier, cell, range, or function...
                QStringView view(expression);
                state = InIdentifier;
                int i = data - start;
                const static QString errorTypeReplacement("ERRORTYPE");
                const static QString legacyNormsdistReplacement("LEGACYNORMSDIST");
                const static QString legacyNormsinvReplacement("LEGACYNORMSINV");
                const static QString multipleOperations("MULTIPLE.OPERATIONS");
                if (view.mid(i, 10).compare(QLatin1String("ERROR.TYPE")) == 0) {
                    // replace it
                    int outPos = out - outStart;
                    result.replace(outPos, 9, errorTypeReplacement);
                    data += 10; // number of characters in "ERROR.TYPE"
                    out += 9;
                } else if (view.mid(i, 12).compare(QLatin1String("LEGACY.NORMS")) == 0) {
                    if (view.mid(i + 12, 4).compare(QLatin1String("DIST")) == 0) {
                        // replace it
                        int outPos = out - outStart;
                        result.replace(outPos, 15, legacyNormsdistReplacement);
                        data += 16; // number of characters in "LEGACY.NORMSDIST"
                        out += 15;
                    } else if (view.mid(i + 12, 3).compare(QLatin1String("INV")) == 0) {
                        // replace it
                        int outPos = out - outStart;
                        result.replace(outPos, 14, legacyNormsinvReplacement);
                        data += 15; // number of characters in "LEGACY.NORMSINV"
                        out += 14;
                    }
                } else if (namespacePrefix == "oooc:" && view.mid(i, 5).compare(QLatin1String("TABLE")) == 0 && !Formula::isIdentifier(expression[i + 5])) {
                    int outPos = out - outStart;
                    result.replace(outPos, 19, multipleOperations);
                    data += 5;
                    out += 19;
                } else if (view.mid(i, 3).compare(QLatin1String("NEG")) == 0) {
                    *out = QChar('-', 0);
                    data += 3;
                    ++out;
                }
            } else {
                switch (data->unicode()) {
                case '"': // a string ?
                    state = InString;
                    *out++ = *data++;
                    break;
                case '[': // [ marks sheet name for 3-d cell, e.g ['Sales Q3'.A4]
                    state = InReference;
                    ++data;
                    // NOTE: As long as Calligra::Sheets does not support fixed sheets eat the dollar sign.
                    if (*data == QChar('$', 0)) {
                        ++data;
                    }
                    pos = data;
                    break;
                default:
                    const QChar *operatorStart = data;
                    if (!Formula::parseOperator(data, out)) {
                        *out++ = *data++;
                    } else if (*operatorStart == QChar('=', 0) && data - operatorStart == 1) { // only one =
                        *out++ = QChar('=', 0);
                    }
                    break;
                }
            }
        } break;
        case InNumber:
            if (data->isDigit()) {
                *out++ = *data++;
            } else if (*data == QChar('.', 0)) {
                for (const QChar &decimalChar: decimal) {
                    *out++ = decimalChar;
                }
                ++data;
            } else if (*data == QChar('E', 0) || *data == QChar('e', 0)) {
                *out++ = QChar('E', 0);
                ++data;
            } else {
                state = Start;
            }

            break;
        case InString:
            if (*data == QChar('"', 0)) {
                state = Start;
            }
            *out++ = *data++;
            break;
        case InIdentifier: {
            if (Formula::isIdentifier(*data) || data->isDigit()) {
                *out++ = *data++;
            } else {
                state = Start;
            }
        } break;
        case InReference:
            switch (data->unicode()) {
            case ']':
                Odf::loadRegion(pos, data, out);
                pos = data;
                state = Start;
                break;
            case '\'':
                state = InSheetName;
                break;
            default:
                break;
            }
            ++data;
            break;
        case InSheetName:
            if (*data == QChar('\'', 0)) {
                ++data;
                if (!data->isNull() && *data == QChar('\'', 0)) {
                    ++data;
                } else {
                    state = InReference;
                }
            } else {
                ++data;
            }
            break;
        }
    }
    result.resize(out - outStart);
    return result;
}

QString Odf::encodeFormula(const QString &expr, const Localization *locale)
{
    // use locale settings
    const QString decimal = locale ? locale->decimalSymbol() : ".";

    QString result('=');

    Formula formula;
    Tokens tokens = formula.scan(expr, locale);

    if (!tokens.valid() || tokens.count() == 0)
        return expr; // no altering on error

    for (int i = 0; i < tokens.count(); ++i) {
        const QString tokenText = tokens[i].text();
        const Token::Type type = tokens[i].type();

        switch (type) {
        case Token::Cell:
        case Token::Range: {
            result.append('[');
            // FIXME Stefan: Hack to get the apostrophes right. Fix and remove!
            const int pos = tokenText.lastIndexOf('!');
            if (pos != -1 && QStringView{tokenText}.left(pos).contains(' '))
                result.append(Odf::saveRegion('\'' + tokenText.left(pos) + '\'' + tokenText.mid(pos)));
            else
                result.append(Odf::saveRegion(tokenText));
            result.append(']');
            break;
        }
        case Token::Float: {
            QString tmp(tokenText);
            result.append(tmp.replace(decimal, "."));
            break;
        }
        case Token::Operator: {
            if (tokens[i].asOperator() == Token::Equal)
                result.append('=');
            else
                result.append(tokenText);
            break;
        }
        case Token::Identifier: {
            if (tokenText == "ERRORTYPE") {
                // need to replace this
                result.append("ERROR.TYPE");
            } else if (tokenText == "LEGACYNORMSDIST") {
                result.append("LEGACY.NORMSDIST");
            } else if (tokenText == "LEGACYNORMSINV") {
                result.append("LEGACY.NORMSINV");
            } else {
                // dump it out unchanged
                result.append(tokenText);
            }
            break;
        }
        case Token::Boolean:
        case Token::Integer:
        case Token::String:
        default:
            result.append(tokenText);
            break;
        }
    }
    return result;
}

QString Odf::convertRefToBase(const QString &sheet, const QRect &rect)
{
    QPoint bottomRight(rect.bottomRight());

    QString s = '$' + sheet + ".$" + Cell::columnName(bottomRight.x()) + '$' + QString::number(bottomRight.y());

    return s;
}

QString Odf::convertRefToRange(const QString &sheet, const QRect &rect)
{
    QPoint topLeft(rect.topLeft());
    QPoint bottomRight(rect.bottomRight());

    if (topLeft == bottomRight)
        return Odf::convertRefToBase(sheet, rect);

    QString s = '$' + sheet + ".$" + Cell::columnName(topLeft.x()) + '$' + QString::number(topLeft.y()) + ":.$" + Cell::columnName(bottomRight.x()) + '$'
        + QString::number(bottomRight.y());

    return s;
}

// e.g.: Sheet4.A1:Sheet4.E28
// used in Sheet::saveOdf
QString Odf::convertRangeToRef(const QString &sheetName, const QRect &_area)
{
    return sheetName + '.' + Cell::name(_area.left(), _area.top()) + ':' + sheetName + '.' + Cell::name(_area.right(), _area.bottom());
}

QString Odf::convertMSOOXMLFormula(const QString &formula)
{
    if (formula.isEmpty())
        return QString();
    enum {
        InStart,
        InArguments,
        InParenthesizedArgument,
        InString,
        InSheetOrAreaName,
        InCellReference
    } state;
    state = InStart;
    int cellReferenceStart = 0;
    int sheetOrAreaNameDelimiterCount = 0;
    QString result = formula.startsWith('=') ? formula : '=' + formula;
    for (int i = 1; i < result.length(); ++i) {
        QChar ch = result[i];
        switch (state) {
        case InStart:
            if (ch == '(')
                state = InArguments;
            break;
        case InArguments:
            if (ch == '"')
                state = InString;
            else if (ch.unicode() == '\'') {
                sheetOrAreaNameDelimiterCount = 1;
                for (int j = i + 1; j < result.length(); ++j) {
                    if (result[j].unicode() != '\'')
                        break;
                    ++sheetOrAreaNameDelimiterCount;
                }
                if (sheetOrAreaNameDelimiterCount >= 2)
                    result.remove(i + 1, sheetOrAreaNameDelimiterCount - 1);
                state = InSheetOrAreaName;
            } else if (Util::isCellnameCharacter(ch)) {
                state = InCellReference;
                cellReferenceStart = i;
            } else if (ch == ',')
                result[i] = ';'; // replace argument delimiter
            else if (ch == '(' && !result[i - 1].isLetterOrNumber())
                state = InParenthesizedArgument;
            else if (ch == ' ') {
                // check if it might be an intersection operator
                // for it to be an intersection operator the next non-space char must be a cell-name-character or '
                // and previous converted char cannot be ';'
                int firstNonSpace = i + 1;
                while (firstNonSpace < result.length() && result[firstNonSpace] == ' ') {
                    firstNonSpace++;
                }
                bool wasDelimeter = (i - 1 > 0) && (result[i - 1] == ';');
                bool isIntersection = !wasDelimeter && firstNonSpace < result.length()
                    && (result[firstNonSpace].isLetter() || result[firstNonSpace] == '$' || result[firstNonSpace] == '\'');
                if (isIntersection) {
                    result[i] = '!';
                    i = firstNonSpace - 1;
                }
            }
            break;
        case InParenthesizedArgument:
            if (ch == ',')
                result[i] = '~'; // union operator
            else if (ch == ' ')
                result[i] = '!'; // intersection operator
            else if (ch == ')')
                state = InArguments;
            break;
        case InString:
            if (ch == '"')
                state = InArguments;
            break;
        case InSheetOrAreaName:
            Q_ASSERT(i >= 1);
            if (ch == '\'' && result[i - 1].unicode() != '\\') {
                int count = 1;
                for (int j = i + 1; count < sheetOrAreaNameDelimiterCount && j < result.length(); ++j) {
                    if (result[j].unicode() != '\'')
                        break;
                    ++count;
                }
                if (count == sheetOrAreaNameDelimiterCount) {
                    if (sheetOrAreaNameDelimiterCount >= 2)
                        result.remove(i + 1, sheetOrAreaNameDelimiterCount - 1);
                    state = InArguments;
                } else {
                    result.insert(i, '\'');
                    ++i;
                }
            }
            break;
        case InCellReference:
            if (!Util::isCellnameCharacter(ch)) {
                if (ch != '(') /* skip formula-names */ {
                    // Excel is able to use only the column-name to define a column
                    // where all rows are selected. Since that is not supported in
                    // ODF we add to such definitions the minimum/maximum row-number.
                    // So, something like "A:B" would become "A$1:B$65536". Note that
                    // such whole column-definitions are only allowed for ranges like
                    // "A:B" but not for single column definitions like "A" or "B".
                    const QString ref = result.mid(qMax(0, cellReferenceStart - 1), i - cellReferenceStart + 2);
                    QRegularExpression rxStart(".*(|\\$)[A-Za-z]+\\:");
                    QRegularExpression rxEnd("\\:(|\\$)[A-Za-z]+(|(|\\$)[0-9]+).*");
                    QRegularExpressionMatch match = rxEnd.match(ref);
                    if (match.hasMatch() && match.captured(2).isEmpty()) {
                        result.insert(i, "$65536");
                        i += 6;
                    } else if (rxStart.match(ref).hasMatch()) {
                        result.insert(i, "$1");
                        i += 2;
                    }
                }
                state = InArguments;
                --i; // decrement again to handle the current char in the InArguments-switch.
            }
            break;
        };
    };
    return result;
}

} // Sheets
} // Calligra
