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

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include <KoGenStyles.h>
#include <KoParagraphStyle.h>
#include <KoShape.h>
#include <KoShapeRegistry.h>
#include <KoStyleManager.h>
#include <KoTextDocument.h>
#include <KoTextLoader.h>
#include <KoTextSharedLoadingData.h>
#include <KoTextWriter.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "Cell.h"
#include "CellStorage.h"
#include "Condition.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "Util.h"
#include "Validity.h"
#include "Value.h"
#include "ValueConverter.h"
#include "ValueFormatter.h"
#include "GenValidationStyle.h"
#include "ShapeApplicationData.h"

#include <float.h>

// This file contains functionality to load/save a Cell

namespace Calligra {
namespace Sheets {


namespace Odf {

    // cell loading - helper functions
    void loadCellText(Cell *cell, const KoXmlElement& parent, OdfLoadingContext& tableContext, const Styles& autoStyles, const QString& cellStyleName);
    QString loadCellTextNodes(Cell *cell, const KoXmlElement& element, int *textFragmentCount, int *lineCount, bool *hasRichText, bool *stripLeadingSpace);
    void loadObjects(Cell *cell, const KoXmlElement &parent, OdfLoadingContext& tableContext, QList<ShapeLoadingData>& shapeData);
    ShapeLoadingData loadObject(Cell *cell, const KoXmlElement &element, KoShapeLoadingContext &shapeContext);

    // cell saving - helper functions
    QString saveCellStyle(Cell *cell, KoGenStyle &currentCellStyle, KoGenStyles &mainStyles);
    void saveCellAnnotation(Cell *cell, KoXmlWriter &xmlwriter);
    void saveCellValue(Cell *cell, KoXmlWriter &xmlWriter);
}

// *************** Loading *****************
bool Odf::loadCell(Cell *cell, const KoXmlElement& element, OdfLoadingContext& tableContext,
            const Styles& autoStyles, const QString& cellStyleName,
            QList<ShapeLoadingData>& shapeData)
{
    static const QString sFormula           = QString::fromLatin1("formula");
    static const QString sValidationName    = QString::fromLatin1("validation-name");
    static const QString sValueType         = QString::fromLatin1("value-type");
    static const QString sBoolean           = QString::fromLatin1("boolean");
    static const QString sBooleanValue      = QString::fromLatin1("boolean-value");
    static const QString sTrue              = QString::fromLatin1("true");
    static const QString sFalse             = QString::fromLatin1("false");
    static const QString sFloat             = QString::fromLatin1("float");
    static const QString sValue             = QString::fromLatin1("value");
    static const QString sCurrency          = QString::fromLatin1("currency");
    static const QString sPercentage        = QString::fromLatin1("percentage");
    static const QString sDate              = QString::fromLatin1("date");
    static const QString sDateValue         = QString::fromLatin1("date-value");
    static const QString sTime              = QString::fromLatin1("time");
    static const QString sTimeValue         = QString::fromLatin1("time-value");
    static const QString sString            = QString::fromLatin1("string");
    static const QString sStringValue       = QString::fromLatin1("string-value");
    static const QString sNumberColumnsSpanned = QString::fromLatin1("number-columns-spanned");
    static const QString sNumberRowsSpanned = QString::fromLatin1("number-rows-spanned");
    static const QString sAnnotation        = QString::fromLatin1("annotation");
    static const QString sP                 = QString::fromLatin1("p");

    static const QStringList formulaNSPrefixes = QStringList() << "oooc:" << "kspr:" << "of:" << "msoxl:";

    //Search and load each paragraph of text. Each paragraph is separated by a line break.
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
        foreach(const QString &prefix, formulaNSPrefixes) {
            if (oasisFormula.startsWith(prefix)) {
                oasisFormula.remove(0, prefix.length());
                namespacePrefix = prefix;
                break;
            }
        }
        oasisFormula = Odf::decodeFormula(oasisFormula, cell->locale(), namespacePrefix);
        cell->setUserInput(oasisFormula);
    } else if (!cell->userInput().isEmpty() && cell->userInput().at(0) == '=')  //prepend ' to the text to avoid = to be painted
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
            // always set the userInput to the actual value read from the cell, and not whatever happens to be set as text, as the textual representation of a value may be less accurate than the value itself
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
            QString value = element.attributeNS(KoXmlNS::office, sDateValue, QString());

            // "1980-10-15" or "2001-01-01T19:27:41"
            int year = 0, month = 0, day = 0, hours = 0, minutes = 0, seconds = 0;
            bool hasTime = false;
            bool ok = false;

            int p1 = value.indexOf('-');
            if (p1 > 0) {
                year  = value.left(p1).toInt(&ok);
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
                                    if (ok)
                                        seconds = value.right(value.length() - p5 - 1).toInt(&ok);
                                }
                            }
                        } else {
                            day = value.right(value.length() - p2).toInt(&ok);
                        }
                    }
                }
            }

            if (ok) {
                if (hasTime)
                    cell->setValue(Value(QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds)), cell->sheet()->map()->calculationSettings()));
                else
                    cell->setValue(Value(QDate(year, month, day), cell->sheet()->map()->calculationSettings()));
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
//Sebsauer: Fixed now. Value::Format handles it correct.
#if 0
                Style s;
                s.setFormatType(Format::ShortDate);
                setStyle(s);
#endif
                // debugSheetsODF << "cell:" << cell->name() << "Type: date, value:" << value << "Date:" << year << " -" << month << " -" << day;
            }
        } else if (valuetype == sTime) {
            QString value = element.attributeNS(KoXmlNS::office, sTimeValue, QString());

            // "PT15H10M12S"
            int hours = 0, minutes = 0, seconds = 0;
            int l = value.length();
            QString num;
            bool ok = false;
            for (int i = 0; i < l; ++i) {
                if (value[i].isNumber()) {
                    num += value[i];
                    continue;
                } else if (value[i] == 'H')
                    hours   = num.toInt(&ok);
                else if (value[i] == 'M')
                    minutes = num.toInt(&ok);
                else if (value[i] == 'S')
                    seconds = num.toInt(&ok);
                else
                    continue;
                //debugSheetsODF << "Num:" << num;
                num.clear();
                if (!ok)
                    break;
            }

            if (ok) {
                // Value kval( timeToNum( hours, minutes, seconds ) );
                // cell->setValue( kval );
                cell->setValue(Value(QTime(hours % 24, minutes, seconds)));
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
        if (ok) colSpan = span;
    }
    if (element.hasAttributeNS(KoXmlNS::table, sNumberRowsSpanned)) {
        bool ok = false;
        int span = element.attributeNS(KoXmlNS::table, sNumberRowsSpanned, QString()).toInt(&ok);
        if (ok) rowSpan = span;
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
                    if (!comment.isEmpty()) comment.append('\n');
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

bool Odf::saveCell(Cell *cell, int &repeated, OdfSavingContext& tableContext)
{
    KoXmlWriter & xmlwriter = tableContext.shapeContext.xmlWriter();
    KoGenStyles & mainStyles = tableContext.shapeContext.mainStyles();

    int row = cell->row();
    int column = cell->column();

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
    if (cell->link().isEmpty())
        saveCellValue(cell, xmlwriter);

    const Style cellStyle = cell->style();

    // Either there's no column and row default and the style's not the default style,
    // or the style is different to one of them. The row default takes precedence.
    if ((!tableContext.rowDefaultStyles.contains(row) &&
            !tableContext.columnDefaultStyles.contains(column) &&
            !(cellStyle.isDefault() && cell->conditions().isEmpty())) ||
            (tableContext.rowDefaultStyles.contains(row) && tableContext.rowDefaultStyles[row] != cellStyle) ||
            (tableContext.columnDefaultStyles.contains(column) && tableContext.columnDefaultStyles[column] != cellStyle)) {
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
    if (cell->isEmpty() && comment.isEmpty() && !cell->isPartOfMerged() && !cell->doesMergeCells() &&
            !tableContext.cellHasAnchoredShapes(cell->sheet(), row, column)) {
        bool refCellIsDefault = cell->isDefault();
        int j = column + 1;
        Cell nextCell = cell->sheet()->cellStorage()->nextInRow(column, row);
        while (!nextCell.isNull()) {
            // if
            //   the next cell is not the adjacent one
            // or
            //   the next cell is not empty
            if (nextCell.column() != j || (!nextCell.isEmpty() || tableContext.cellHasAnchoredShapes(cell->sheet(), row, column))) {
                if (refCellIsDefault) {
                    // if the origin cell was a default cell,
                    // we count the default cells
                    repeated = nextCell.column() - j + 1;

                    // check if any of the empty/default cells we skipped contained anchored shapes
                    int shapeColumn = tableContext.nextAnchoredShape(cell->sheet(), row, column);
                    if (shapeColumn) {
                        repeated = qMin(repeated, shapeColumn - column);
                    }
                }
                // otherwise we just stop here to process the adjacent
                // cell in the next iteration of the outer loop
                // (in saveCells)
                break;
            }

            if (nextCell.isPartOfMerged() || nextCell.doesMergeCells() ||
                    !nextCell.comment().isEmpty() || tableContext.cellHasAnchoredShapes(cell->sheet(), row, nextCell.column()) ||
                    !(nextCell.style() == cellStyle && nextCell.conditions() == cell->conditions())) {
                break;
            }
            ++repeated;
            // get the next cell and set the index to the adjacent cell
            nextCell = cell->sheet()->cellStorage()->nextInRow(j++, row);
        }
        //debugSheetsODF << "Odf::saveCell: empty cell in column" << column
        //<< "repeated" << repeated << "time(s)" << endl;

        if (repeated > 1)
            xmlwriter.addAttribute("table:number-columns-repeated", QString::number(repeated));
    }

    Validity validity = cell->validity();
    if (!validity.isEmpty()) {
        GenValidationStyle styleVal(&validity, cell->sheet()->map()->converter());
        xmlwriter.addAttribute("table:validation-name", tableContext.valStyle.insert(styleVal));
    }
    if (cell->isFormula()) {
        //debugSheetsODF <<"Formula found";
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
        //debugSheetsODF<<"Link found";
        xmlwriter.startElement("text:p");
        xmlwriter.startElement("text:a");
        const QString url = cell->link();
        //Reference cell is started by '#'
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
            ((KoCharacterStyle *)cell->sheet()->map()->textStyleManager()->defaultParagraphStyle())->copyProperties(format);

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
    if (tableContext.cellHasAnchoredShapes(cell->sheet(), row, column)) {
        const QList<KoShape*> shapes = tableContext.cellAnchoredShapes(cell->sheet(), row, column);
        for (int i = 0; i < shapes.count(); ++i) {
            KoShape* const shape = shapes[i];
            const QPointF bottomRight = shape->boundingRect().bottomRight();
            qreal endX = 0.0;
            qreal endY = 0.0;
            const int scol = cell->sheet()->leftColumn(bottomRight.x(), endX);
            const int srow = cell->sheet()->topRow(bottomRight.y(), endY);
            qreal offsetX = cell->sheet()->columnPosition(column);
            qreal offsetY = cell->sheet()->rowPosition(row);
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

QString Odf::loadCellTextNodes(Cell *cell, const KoXmlElement& element, int *textFragmentCount, int *lineCount, bool *hasRichText, bool *stripLeadingSpace)
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
                    cellText += QString().fill(32, howmany);
                } else if (isTextNs && e.localName() == "tab") {
                    cellText += '\t';
                } else if (isTextNs && e.localName() == "line-break") {
                    cellText += '\n';
                    ++(*lineCount);
                } else if (isTextNs && e.localName() == "span") {
                    // Nested span-elements means recursive evaluation.
                    cellText += loadCellTextNodes(cell, e, textFragmentCount, lineCount, hasRichText, stripLeadingSpace);
                } else if (!isTextNs ||
                              ( e.localName() != "annotation" &&
                                e.localName() != "bookmark" &&
                                e.localName() != "meta" &&
                                e.localName() != "tag" )) {
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
static bool findDrawElements(const KoXmlElement& parent)
{
    KoXmlElement element;
    forEachElement(element , parent) {
        if (element.namespaceURI() == KoXmlNS::draw)
            return true;
        if (findDrawElements(element))
            return true;
    }
    return false;
}

// Similar to KoXml::namedItemNS except that children of span tags will be evaluated too.
static KoXmlElement namedItemNSWithSpan(const KoXmlNode& node, const QString &nsURI, const QString &localName)
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

void Odf::loadCellText(Cell *cell, const KoXmlElement& parent, OdfLoadingContext& tableContext, const Styles& autoStyles, const QString& cellStyleName)
{
    //Search and load each paragraph of text. Each paragraph is separated by a line break
    KoXmlElement textParagraphElement;
    QString cellText;

    int lineCount = 0;
    bool hasRichText = false;
    bool stripLeadingSpace = true;

    forEachElement(textParagraphElement , parent) {
        if (textParagraphElement.localName() == "p" &&
                textParagraphElement.namespaceURI() == KoXmlNS::text) {

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

            Style style; style.setDefault();
            if (!cellStyleName.isEmpty()) {
                if (autoStyles.contains(cellStyleName))
                    style.merge(autoStyles[cellStyleName]);
                else {
                    const CustomStyle* namedStyle = cell->sheet()->map()->styleManager()->style(cellStyleName);
                    if (namedStyle)
                        style.merge(*namedStyle);
                }
            }

            QTextCharFormat format = style.asCharFormat();
            ((KoCharacterStyle *)cell->sheet()->map()->textStyleManager()->defaultParagraphStyle())->copyProperties(format);

            QSharedPointer<QTextDocument> doc(new QTextDocument);
            KoTextDocument(doc.data()).setStyleManager(cell->sheet()->map()->textStyleManager());

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

void Odf::loadObjects(Cell *cell, const KoXmlElement &parent, OdfLoadingContext& tableContext, QList<ShapeLoadingData>& shapeData)
{
    // Register additional attributes, that identify shapes anchored in cells.
    // Their dimensions need adjustment after all rows are loaded,
    // because the position of the end cell is not always known yet.
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-cell-address",
                "table:end-cell-address"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-x",
                "table:end-x"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-y",
                "table:end-y"));

    KoXmlElement element;
    forEachElement(element, parent) {
        if (element.namespaceURI() != KoXmlNS::draw)
            continue;

        if (element.localName() == "a") {
            // It may the case that the object(s) are embedded into a hyperlink so actions are done on
            // clicking it/them but since we do not supported objects-with-hyperlinks yet we just fetch
            // the inner elements and use them to at least create and show the objects (see bug 249862).
            KoXmlElement e;
            forEachElement(e, element) {
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
    data.shape = 0;
    KoShape* shape = KoShapeRegistry::instance()->createShapeFromOdf(element, shapeContext);
    if (!shape) {
        debugSheetsODF << "Unable to load shape with localName=" << element.localName();
        return data;
    }

    cell->sheet()->addShape(shape);

    // The position is relative to the upper left sheet corner until now. Move it.
    QPointF position = shape->position();
    // Remember how far we're off from the top-left corner of this cell
    double offsetX = position.x();
    double offsetY = position.y();
    for (int col = 1; col < cell->column(); ++col)
        position += QPointF(cell->sheet()->columnFormat(col)->width(), 0.0);
    if (cell->row() > 1)
        position += QPointF(0.0, cell->sheet()->rowFormats()->totalRowHeight(1, cell->row() - 1));
    shape->setPosition(position);

    dynamic_cast<ShapeApplicationData*>(shape->applicationData())->setAnchoredToCell(true);

    // All three attributes are necessary for cell anchored shapes.
    // Otherwise, they are anchored in the sheet.
    if (!shape->hasAdditionalAttribute("table:end-cell-address") ||
            !shape->hasAdditionalAttribute("table:end-x") ||
            !shape->hasAdditionalAttribute("table:end-y")) {
        debugSheetsODF << "Not all attributes found, that are necessary for cell anchoring.";
        return data;
    }

    Region endCell(loadRegion(shape->additionalAttribute("table:end-cell-address")),
                   cell->sheet()->map(), cell->sheet());
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
        size += QSizeF(cell->sheet()->columnFormat(col)->width(), 0.0);
    if (endCell.firstRange().top() > cell->row())
        size += QSizeF(0.0, cell->sheet()->rowFormats()->totalRowHeight(cell->row(), endCell.firstRange().top() - 1));
    shape->setSize(size);

    return data;
}


// saving - helper functions

void Odf::saveCellAnnotation(Cell *cell, KoXmlWriter &xmlwriter)
{
    const QString comment = cell->comment();
    if (comment.isEmpty()) return;

    //<office:annotation draw:style-name="gr1" draw:text-style-name="P1" svg:width="2.899cm" svg:height="2.691cm" svg:x="2.858cm" svg:y="0.001cm" draw:caption-point-x="-2.858cm" draw:caption-point-y="-0.001cm">
    xmlwriter.startElement("office:annotation");
    const QStringList text = comment.split('\n', QString::SkipEmptyParts);
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
        saveConditions(&conditions, currentCellStyle, cell->sheet()->map()->converter());
    }
    Style style = cell->style();
    return saveStyle(&style, currentCellStyle, mainStyles, cell->sheet()->map()->styleManager());
}

void Odf::saveCellValue(Cell *cell, KoXmlWriter &xmlWriter)
{
    Value value = cell->value();
    // Determine the format that we will be storing.
    // This is usually the format that is actually shown - doing so mixes style and content, but that's how
    // LO does it, so we need to stay compatible
    Format::Type shownFormat = cell->style().formatType();
    if (shownFormat == Format::Generic)
        shownFormat = cell->sheet()->map()->formatter()->determineFormatting(value, shownFormat);
    Value::Format saveFormat = Value::fmt_None;
    Value::Format valueFormat = value.format();
    if (valueFormat == Value::fmt_Boolean)
        saveFormat = Value::fmt_Boolean;
    else if (valueFormat == Value::fmt_String)  // if it's a text, it needs to be stored as a text
        saveFormat = Value::fmt_String;
    else if (Format::isDate(shownFormat))
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

    switch (saveFormat) {
    case Value::fmt_None: break;  //NOTHING HERE
    case Value::fmt_Boolean: {
        xmlWriter.addAttribute("office:value-type", "boolean");
        xmlWriter.addAttribute("office:boolean-value", (value.asBoolean() ? "true" : "false"));
        break;
    }
    case Value::fmt_Number: {
        xmlWriter.addAttribute("office:value-type", "float");
        if (value.isInteger())
            xmlWriter.addAttribute("office:value", QString::number(value.asInteger()));
        else
            xmlWriter.addAttribute("office:value", QString::number(numToDouble(value.asFloat()), 'g', DBL_DIG));
        break;
    }
    case Value::fmt_Percent: {
        xmlWriter.addAttribute("office:value-type", "percentage");
        xmlWriter.addAttribute("office:value", QString::number((double) numToDouble(value.asFloat())));
        break;
    }
    case Value::fmt_Money: {
        xmlWriter.addAttribute("office:value-type", "currency");
        const Style style = cell->style();
        if (style.hasAttribute(Style::CurrencyFormat)) {
            Currency currency = style.currency();
            xmlWriter.addAttribute("office:currency", currency.code());
        }
        xmlWriter.addAttribute("office:value", QString::number((double) numToDouble(value.asFloat())));
        break;
    }
    case Value::fmt_DateTime: break;  //NOTHING HERE
    case Value::fmt_Date: {
        xmlWriter.addAttribute("office:value-type", "date");
        xmlWriter.addAttribute("office:date-value",
                               value.asDate(cell->sheet()->map()->calculationSettings()).toString(Qt::ISODate));
        break;
    }
    case Value::fmt_Time: {
        xmlWriter.addAttribute("office:value-type", "time");
        xmlWriter.addAttribute("office:time-value",
                               value.asTime().toString("'PT'hh'H'mm'M'ss'S'"));
        break;
    }
    case Value::fmt_String: {
        xmlWriter.addAttribute("office:value-type", "string");
        xmlWriter.addAttribute("office:string-value", value.asString());
        break;
    }
    };
}





}  // Sheets
}  // Calligra

