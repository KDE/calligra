/* This file is part of the KDE project
   Copyright (C) 2003-2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Manikandaprasad Chandrasekar <manikandaprasad.chandrasekar@nokia.com>
   Copyright (c) 2010 Carlos Licea <carlos@kdab.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include "ExcelImport.h"
#include <ExcelImport.moc>

#include <QString>
#include <QDate>
#include <QBuffer>
#include <QFontMetricsF>
#include <QPair>
#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoGenStyle.h>
#include <KoOdfNumberStyles.h>

#include <part/Doc.h>
#include <Map.h>
#include <Sheet.h>
#include <RowColumnFormat.h>
#include <Style.h>

#include <Charting.h>
#include <ChartExport.h>
#include <NumberFormatParser.h>

#include "swinder.h"
#include <iostream>
#include "ODrawClient.h"
#include "ImportUtils.h"

typedef KGenericFactory<ExcelImport> ExcelImportFactory;
K_EXPORT_COMPONENT_FACTORY(libexcelimporttodoc, ExcelImportFactory("kofficefilters"))

using namespace Swinder;
using namespace XlsUtils;

class ExcelImport::Private
{
public:
    QString inputFile;
    KSpread::Doc* outputDoc;

    KoStore* storeout;
    Workbook *workbook;
    KoGenStyles *styles;

    void processSheet(Sheet* isheet, KSpread::Sheet* osheet);
    void processColumn(Sheet* isheet, unsigned column, KSpread::Sheet* osheet);
    void processRow(Sheet* isheet, unsigned row, KSpread::Sheet* osheet);
    void processCell(const Cell* icell, KSpread::Cell ocell);

    KSpread::Style convertStyle(const Format* format, const QString& formula);
    QHash<CellFormatKey, KSpread::Style> styleCache;

    void processFontFormat(const FormatFont& font, KSpread::Style& style);
    QPen convertBorder(const Pen& pen);

    int rowsCountTotal, rowsCountDone;
    void addProgress(int addValue);
};

ExcelImport::ExcelImport(QObject* parent, const QStringList&)
        : KoFilter(parent)
{
    d = new Private;
}

ExcelImport::~ExcelImport()
{
    delete d;
}

KoFilter::ConversionStatus ExcelImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/vnd.ms-excel")
        return KoFilter::NotImplemented;

    if (to != "application/vnd.oasis.opendocument.spreadsheet")
        return KoFilter::NotImplemented;

    d->inputFile = m_chain->inputFile();

    KoDocument* document = m_chain->outputDocument();
    if (!document)
        return KoFilter::StupidError;

    d->outputDoc = qobject_cast<KSpread::Doc*>(document);
    if (!d->outputDoc) {
        kWarning() << "document isn't a KSpread::Doc but a " << document->metaObject()->className();
        return KoFilter::WrongFormat;
    }

    emit sigProgress(0);
    
    
    QBuffer b; // TODO: use temporary file instead
    d->storeout = KoStore::createStore(&b, KoStore::Write);

    // open inputFile
    d->workbook = new Swinder::Workbook(d->storeout);
    connect(d->workbook, SIGNAL(sigProgress(int)), this, SIGNAL(sigProgress(int)));
    if (!d->workbook->load(d->inputFile.toLocal8Bit())) {
        delete d->workbook;
        d->workbook = 0;
        delete d->storeout;
        d->storeout = 0;
        return KoFilter::StupidError;
    }

    if (d->workbook->isPasswordProtected()) {
        delete d->workbook;
        d->workbook = 0;
        delete d->storeout;
        d->storeout = 0;
        return KoFilter::PasswordProtected;
    }

    emit sigProgress(-1);
    emit sigProgress(0);

    // count the number of rows in total to provide a good progress value
    d->rowsCountTotal = d->rowsCountDone = 0;
    for (unsigned i = 0; i < d->workbook->sheetCount(); i++) {
        Sheet* sheet = d->workbook->sheet(i);
        d->rowsCountTotal += qMin(maximalRowCount, sheet->maxRow());
    }

    // for now needed for NumberFormatParser
    d->styles = new KoGenStyles();
    NumberFormatParser::setStyles(d->styles);

    KSpread::Map* map = d->outputDoc->map();
    for (unsigned i = 0; i < d->workbook->sheetCount(); i++) {
        Sheet* sheet = d->workbook->sheet(i);
        KSpread::Sheet* ksheet = map->addNewSheet();
        d->processSheet(sheet, ksheet);
    }

    delete d->workbook;
    delete d->styles;
    d->inputFile.clear();
    d->outputDoc = 0;

    emit sigProgress(100);
    return KoFilter::OK;
}

void ExcelImport::Private::processSheet(Sheet* is, KSpread::Sheet* os)
{
    os->setSheetName(is->name());
    os->setHidden(!is->visible());
    //os->setProtected(is->protect());
    os->setAutoCalculationEnabled(is->autoCalc());

    // TODO: page layout

    if(is->password() != 0) {
        //TODO
    }

    const unsigned columnCount = qMin(maximalColumnCount, is->maxColumn());
    for (unsigned i = 0; i <= columnCount; ++i) {
        processColumn(is, i, os);
    }


    const unsigned rowCount = qMin(maximalRowCount, is->maxRow());
    for (unsigned i = 0; i <= rowCount; ++i) {
        processRow(is, i, os);
    }
}

void ExcelImport::Private::processColumn(Sheet* is, unsigned columnIndex, KSpread::Sheet* os)
{
    Column* column = is->column(columnIndex, false);

    if (!column) return;

    KSpread::ColumnFormat* oc = os->nonDefaultColumnFormat(columnIndex+1);
    oc->setWidth(column->width());
    oc->setHidden(!column->visible());
    // TODO: cell styles
}

void ExcelImport::Private::processRow(Sheet* is, unsigned rowIndex, KSpread::Sheet* os)
{
    Row *row = is->row(rowIndex, false);

    if (!row) return;

    KSpread::RowFormat* orf = os->nonDefaultRowFormat(rowIndex+1);
    orf->setHeight(row->height());
    orf->setHidden(!row->visible());
    // TODO default cell style

    // find the column of the rightmost cell (if any)
    const int lastCol = row->sheet()->maxCellsInRow(rowIndex);
    for (int i = 0; i <= lastCol; ++i) {
        Cell* cell = is->cell(i, rowIndex, false);
        if (!cell) continue;
        processCell(cell, KSpread::Cell(os, i+1, rowIndex+1));
    }

    addProgress(1);
}

void ExcelImport::Private::processCell(const Cell* ic, KSpread::Cell oc)
{
    int colSpan = ic->columnSpan();
    int rowSpan = ic->rowSpan();
    if (colSpan > 1 || rowSpan > 1) {
        oc.mergeCells(oc.column(), oc.row(), colSpan - 1, rowSpan - 1);
    }

    const QString formula = ic->formula();
    // TODO: export formula

    Value value = ic->value();
    if (value.isBoolean()) {
        oc.setValue(KSpread::Value(value.asBoolean()));
    } else if (value.isNumber()) {
        const QString valueFormat = ic->format().valueFormat();

        if (isPercentageFormat(valueFormat)) {
            KSpread::Value v(value.asFloat());
            v.setFormat(KSpread::Value::fmt_Percent);
            oc.setValue(v);
        } else if (isDateFormat(value, valueFormat)) {
            // TODO
            oc.setValue(KSpread::Value(value.asFloat()));
        } else if (isTimeFormat(value, valueFormat)) {
            // TODO
            oc.setValue(KSpread::Value(value.asFloat()));
        } else if (isFractionFormat(valueFormat)) {
            // TODO
            oc.setValue(KSpread::Value(value.asFloat()));
        } else {
            oc.setValue(KSpread::Value(value.asFloat()));
        }
    } else if (value.isText()) {
        // TODO: hyperlinks
        // TODO: richtext
        oc.setValue(KSpread::Value(value.asString()));
    } else if (value.isError()) {
        KSpread::Value v(Value::Error);
        v.setError(value.asString());
        oc.setValue(v);
    }

    // TODO notes
    // TODO shapes/pictures/chars

    KSpread::Style s = convertStyle(&ic->format(), formula);
    oc.setStyle(s);
}

KSpread::Style ExcelImport::Private::convertStyle(const Format* format, const QString& formula)
{
    CellFormatKey key(format, formula);
    KSpread::Style& style = styleCache[key];
    if (style.isEmpty()) {
        style.setDefault();
        // TODO: data format/number style

        processFontFormat(format->font(), style);

        FormatAlignment align = format->alignment();
        if (!align.isNull()) {
            switch (align.alignY()) {
            case Format::Top:
                style.setVAlign(KSpread::Style::Top);
                break;
            case Format::Middle:
                style.setVAlign(KSpread::Style::Middle);
                break;
            case Format::Bottom:
                style.setVAlign(KSpread::Style::Bottom);
                break;
            case Format::VJustify:
                style.setVAlign(KSpread::Style::VJustified);
                break;
            case Format::VDistributed:
                style.setVAlign(KSpread::Style::VDistributed);
                break;
            }

            style.setWrapText(align.wrap());

            if (align.rotationAngle()) {
                style.setAngle(align.rotationAngle());
            }

            if (align.stackedLetters()) {
                style.setVerticalText(true);
            }

            if (align.shrinkToFit()) {
                style.setShrinkToFit(true);
            }

            switch (align.alignX()) {
            case Format::Left:
                style.setHAlign(KSpread::Style::Left);
                break;
            case Format::Center:
                style.setHAlign(KSpread::Style::Center);
                break;
            case Format::Right:
                style.setHAlign(KSpread::Style::Right);
                break;
            case Format::Justify:
            case Format::Distributed:
                style.setHAlign(KSpread::Style::Justified);
                break;
            }

            if (align.indentLevel() != 0) {
                style.setIndentation(align.indentLevel() * 10);
            }
        }

        FormatBorders borders = format->borders();
        if (!borders.isNull()) {
            style.setLeftBorderPen(convertBorder(borders.leftBorder()));
            style.setRightBorderPen(convertBorder(borders.rightBorder()));
            style.setTopBorderPen(convertBorder(borders.topBorder()));
            style.setBottomBorderPen(convertBorder(borders.bottomBorder()));
            style.setFallDiagonalPen(convertBorder(borders.topLeftBorder()));
            style.setGoUpDiagonalPen(convertBorder(borders.bottomLeftBorder()));
        }

        FormatBackground back = format->background();
        if (!back.isNull()) {
            QColor backColor = back.backgroundColor();
            if (back.pattern() == FormatBackground::SolidPattern)
                backColor = back.foregroundColor();
            style.setBackgroundColor(backColor);

            // TODO: patterns
        }
    }
    return style;
}

void ExcelImport::Private::processFontFormat(const FormatFont& font, KSpread::Style& style)
{
    // TODO
}

QPen ExcelImport::Private::convertBorder(const Pen& pen)
{
    if (pen.style == Pen::NoLine || pen.width == 0) {
        return QPen(Qt::NoPen);
    } else {
        QPen op;
        if (pen.style == Pen::DoubleLine) {
            op.setWidthF(pen.width * 3);
        } else {
            op.setWidthF(pen.width);
        }

        switch (pen.style) {
        case Pen::SolidLine: op.setStyle(Qt::SolidLine); break;
        case Pen::DashLine: op.setStyle(Qt::DashLine); break;
        case Pen::DotLine: op.setStyle(Qt::DotLine); break;
        case Pen::DashDotLine: op.setStyle(Qt::DashDotLine); break;
        case Pen::DashDotDotLine: op.setStyle(Qt::DashDotDotLine); break;
        case Pen::DoubleLine: op.setStyle(Qt::SolidLine); break; // TODO
        }

        op.setColor(pen.color);

        return op;
    }
}








// Updates the displayed progress information
void ExcelImport::Private::addProgress(int addValue)
{
    rowsCountDone += addValue;
    const int progress = int(rowsCountDone / double(rowsCountTotal) * 100.0 + 0.5);
    workbook->emitProgress(progress);
}

