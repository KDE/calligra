/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetsKsp.h"
#include "SheetsKspPrivate.h"

#include "engine/SheetsDebug.h"

#include "CellStorage.h"
#include "ColFormatStorage.h"
#include "DocBase.h"
#include "HeaderFooter.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "PrintSettings.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "StyleStorage.h"

#include <KoUnit.h>
#include <KoXmlReader.h>

#include <kcodecs.h>


namespace Calligra {
namespace Sheets {


namespace Ksp {
    // formats
    QDomElement saveRowFormat(int row, QDomDocument&, Sheet *sheet);
    bool loadRowFormat(const KoXmlElement& row, Sheet *sheet);
    QDomElement saveColFormat(int col, QDomDocument&, Sheet *sheet);
    bool loadColFormat(const KoXmlElement& row, Sheet *sheet);

};


bool Ksp::loadSheet(Sheet *obj, const KoXmlElement& sheet)
{
    bool ok = false;
    QString sname = obj->sheetName();
    if (!obj->fullMap()->loadingInfo()->loadTemplate()) {
        sname = sheet.attribute("name");
        if (sname.isEmpty()) {
            obj->doc()->setErrorMessage(i18n("Invalid document. Sheet name is empty."));
            return false;
        }
    }

    bool detectDirection = true;
    QString layoutDir = sheet.attribute("layoutDirection");
    if (!layoutDir.isEmpty()) {
        if (layoutDir == "rtl") {
            detectDirection = false;
            obj->setLayoutDirection(Qt::RightToLeft);
        } else if (layoutDir == "ltr") {
            detectDirection = false;
            obj->setLayoutDirection(Qt::LeftToRight);
        } else
            debugSheets << " Direction not implemented :" << layoutDir;
    }
    if (detectDirection) {
        if ((sname.isRightToLeft()))
            obj->setLayoutDirection(Qt::RightToLeft);
        else
            obj->setLayoutDirection(Qt::LeftToRight);
    }

    /* older versions of KSpread allowed all sorts of characters that
    the parser won't actually understand.  Replace these with '_'
    Also, the initial character cannot be a space.
    */
    while (sname[0] == ' ') {
        sname.remove(0, 1);
    }
    for (int i = 0; i < sname.length(); i++) {
        if (!(sname[i].isLetterOrNumber() ||
                sname[i] == ' ' || sname[i] == '.' || sname[i] == '_')) {
            sname[i] = '_';
        }
    }

    // validate sheet name, if it differs from the current one
    if (sname != obj->sheetName()) {
        /* make sure there are no name collisions with the altered name */
        QString testName = sname;
        QString baseName = sname;
        int nameSuffix = 0;

        /* so we don't panic over finding ourself in the following test*/
        sname.clear();
        while (obj->map()->findSheet(testName) != 0) {
            nameSuffix++;
            testName = baseName + '_' + QString::number(nameSuffix);
        }
        sname = testName;

        debugSheets << "Sheet::loadXML: table name =" << sname;
        obj->setObjectName(sname);
        obj->setSheetName(sname);
    }

//     (dynamic_cast<SheetIface*>(obj->dcopObject()))->sheetNameHasChanged();

    if (sheet.hasAttribute("grid")) {
        obj->setShowGrid((int)sheet.attribute("grid").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printGrid")) {
        obj->printSettings()->setPrintGrid((bool)sheet.attribute("printGrid").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printCommentIndicator")) {
        obj->printSettings()->setPrintCommentIndicator((bool)sheet.attribute("printCommentIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printFormulaIndicator")) {
        obj->printSettings()->setPrintFormulaIndicator((bool)sheet.attribute("printFormulaIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("hide")) {
        obj->setHidden((bool)sheet.attribute("hide").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("showFormula")) {
        obj->setShowFormula((bool)sheet.attribute("showFormula").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    //Compatibility with KSpread 1.1.x
    if (sheet.hasAttribute("formular")) {
        obj->setShowFormula((bool)sheet.attribute("formular").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("showFormulaIndicator")) {
        obj->setShowFormulaIndicator((bool)sheet.attribute("showFormulaIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("showCommentIndicator")) {
        obj->setShowCommentIndicator((bool)sheet.attribute("showCommentIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("borders")) {
        obj->setShowPageOutline((bool)sheet.attribute("borders").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("lcmode")) {
        obj->setLcMode((bool)sheet.attribute("lcmode").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("autoCalc")) {
        obj->setAutoCalculationEnabled((bool)sheet.attribute("autoCalc").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("columnnumber")) {
        obj->setShowColumnNumber((bool)sheet.attribute("columnnumber").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("hidezero")) {
        obj->setHideZero((bool)sheet.attribute("hidezero").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("firstletterupper")) {
        obj->setFirstLetterUpper((bool)sheet.attribute("firstletterupper").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }

    // Load the paper layout
    KoXmlElement paper = sheet.namedItem("paper").toElement();
    if (!paper.isNull()) {
        KoPageLayout pageLayout;
        pageLayout.format = KoPageFormat::formatFromString(paper.attribute("format"));
        pageLayout.orientation = (paper.attribute("orientation")  == "Portrait")
                                 ? KoPageFormat::Portrait : KoPageFormat::Landscape;

        // <borders>
        KoXmlElement borders = paper.namedItem("borders").toElement();
        if (!borders.isNull()) {
            pageLayout.leftMargin   = MM_TO_POINT(borders.attribute("left").toFloat());
            pageLayout.rightMargin  = MM_TO_POINT(borders.attribute("right").toFloat());
            pageLayout.topMargin    = MM_TO_POINT(borders.attribute("top").toFloat());
            pageLayout.bottomMargin = MM_TO_POINT(borders.attribute("bottom").toFloat());
        }
        obj->printSettings()->setPageLayout(pageLayout);

        QString hleft, hright, hcenter;
        QString fleft, fright, fcenter;
        // <head>
        KoXmlElement head = paper.namedItem("head").toElement();
        if (!head.isNull()) {
            KoXmlElement left = head.namedItem("left").toElement();
            if (!left.isNull())
                hleft = left.text();
            KoXmlElement center = head.namedItem("center").toElement();
            if (!center.isNull())
                hcenter = center.text();
            KoXmlElement right = head.namedItem("right").toElement();
            if (!right.isNull())
                hright = right.text();
        }
        // <foot>
        KoXmlElement foot = paper.namedItem("foot").toElement();
        if (!foot.isNull()) {
            KoXmlElement left = foot.namedItem("left").toElement();
            if (!left.isNull())
                fleft = left.text();
            KoXmlElement center = foot.namedItem("center").toElement();
            if (!center.isNull())
                fcenter = center.text();
            KoXmlElement right = foot.namedItem("right").toElement();
            if (!right.isNull())
                fright = right.text();
        }
        obj->headerFooter()->setHeadFootLine(hleft, hcenter, hright, fleft, fcenter, fright);
    }

    // load print range
    KoXmlElement printrange = sheet.namedItem("printrange-rect").toElement();
    if (!printrange.isNull()) {
        int left = printrange.attribute("left-rect").toInt();
        int right = printrange.attribute("right-rect").toInt();
        int bottom = printrange.attribute("bottom-rect").toInt();
        int top = printrange.attribute("top-rect").toInt();
        if (left == 0) { //whole row(s) selected
            left = 1;
            right = KS_colMax;
        }
        if (top == 0) { //whole column(s) selected
            top = 1;
            bottom = KS_rowMax;
        }
        const Region region(QRect(QPoint(left, top), QPoint(right, bottom)), obj);
        obj->printSettings()->setPrintRegion(region);
    }

    // load print zoom
    if (sheet.hasAttribute("printZoom")) {
        double zoom = sheet.attribute("printZoom").toDouble(&ok);
        if (ok) {
            obj->printSettings()->setZoom(zoom);
        }
    }

    // load page limits
    if (sheet.hasAttribute("printPageLimitX")) {
        int pageLimit = sheet.attribute("printPageLimitX").toInt(&ok);
        if (ok) {
            obj->printSettings()->setPageLimits(QSize(pageLimit, 0));
        }
    }

    // load page limits
    if (sheet.hasAttribute("printPageLimitY")) {
        int pageLimit = sheet.attribute("printPageLimitY").toInt(&ok);
        if (ok) {
            const int horizontalLimit = obj->printSettings()->pageLimits().width();
            obj->printSettings()->setPageLimits(QSize(horizontalLimit, pageLimit));
        }
    }

    // Load the cells
    KoXmlNode n = sheet.firstChild();
    while (!n.isNull()) {
        KoXmlElement e = n.toElement();
        if (!e.isNull()) {
            QString tagName = e.tagName();
            if (tagName == "cell")
                loadCell(e, obj);
            else if (tagName == "row") {
                loadRowFormat (e, obj);
            } else if (tagName == "column") {
                loadColFormat (e, obj);
            }
#if 0 // CALLIGRA_SHEETS_KOPART_EMBEDDING
            else if (tagName == "object") {
                EmbeddedCalligraObject *ch = new EmbeddedCalligraObject(doc(), obj);
                if (ch->load(e))
                    obj->insertObject(ch);
                else {
                    ch->embeddedObject()->setDeleted(true);
                    delete ch;
                }
            } else if (tagName == "chart") {
                EmbeddedChart *ch = new EmbeddedChart(doc(), obj);
                if (ch->load(e))
                    obj->insertObject(ch);
                else {
                    ch->embeddedObject()->setDeleted(true);
                    delete ch;
                }
            }
#endif // CALLIGRA_SHEETS_KOPART_EMBEDDING
        }
        n = n.nextSibling();
    }

    // load print repeat columns
    KoXmlElement printrepeatcolumns = sheet.namedItem("printrepeatcolumns").toElement();
    if (!printrepeatcolumns.isNull()) {
        int left = printrepeatcolumns.attribute("left").toInt();
        int right = printrepeatcolumns.attribute("right").toInt();
        obj->printSettings()->setRepeatedColumns(qMakePair(left, right));
    }

    // load print repeat rows
    KoXmlElement printrepeatrows = sheet.namedItem("printrepeatrows").toElement();
    if (!printrepeatrows.isNull()) {
        int top = printrepeatrows.attribute("top").toInt();
        int bottom = printrepeatrows.attribute("bottom").toInt();
        obj->printSettings()->setRepeatedRows(qMakePair(top, bottom));
    }

    loadProtection(obj, sheet);

    return true;
}


QDomElement Ksp::saveSheet(Sheet *obj, QDomDocument& dd)
{
    QDomElement sheet = dd.createElement("table");

    // backward compatibility
    QString realName = obj->sheetName();
    QString sheetName;
    for (int i = 0; i < realName.count(); ++i) {
        if (realName[i].isLetterOrNumber() || realName[i] == ' ' || realName[i] == '.')
            sheetName.append(realName[i]);
        else
            sheetName.append('_');
    }
    sheet.setAttribute("name", sheetName);

    //Laurent: for oasis format I think that we must use style:direction...
    sheet.setAttribute("layoutDirection", (obj->layoutDirection() == Qt::RightToLeft) ? "rtl" : "ltr");
    sheet.setAttribute("columnnumber", QString::number((int)obj->getShowColumnNumber()));
    sheet.setAttribute("borders", QString::number((int)obj->isShowPageOutline()));
    sheet.setAttribute("hide", QString::number((int)obj->isHidden()));
    sheet.setAttribute("hidezero", QString::number((int)obj->getHideZero()));
    sheet.setAttribute("firstletterupper", QString::number((int)obj->getFirstLetterUpper()));
    sheet.setAttribute("grid", QString::number((int)obj->getShowGrid()));
    sheet.setAttribute("printGrid", QString::number((int)obj->printSettings()->printGrid()));
    sheet.setAttribute("printCommentIndicator", QString::number((int)obj->printSettings()->printCommentIndicator()));
    sheet.setAttribute("printFormulaIndicator", QString::number((int)obj->printSettings()->printFormulaIndicator()));
    sheet.setAttribute("showFormula", QString::number((int)obj->getShowFormula()));
    sheet.setAttribute("showFormulaIndicator", QString::number((int)obj->getShowFormulaIndicator()));
    sheet.setAttribute("showCommentIndicator", QString::number((int)obj->getShowCommentIndicator()));
    sheet.setAttribute("lcmode", QString::number((int)obj->getLcMode()));
    sheet.setAttribute("autoCalc", QString::number((int)obj->isAutoCalculationEnabled()));
    sheet.setAttribute("borders1.2", "1");
    QByteArray pwd = obj->passwordHash();
    if (!pwd.isNull()) {
        if (pwd.size() > 0) {
            QByteArray str = KCodecs::base64Encode(pwd);
            sheet.setAttribute("protected", QString(str.data()));
        } else
            sheet.setAttribute("protected", "");
    }

    // paper parameters
    QDomElement paper = dd.createElement("paper");
    paper.setAttribute("format", obj->printSettings()->paperFormatString());
    paper.setAttribute("orientation", obj->printSettings()->orientationString());
    sheet.appendChild(paper);

    QDomElement borders = dd.createElement("borders");
    KoPageLayout pageLayout = obj->printSettings()->pageLayout();
    borders.setAttribute("left", QString::number(pageLayout.leftMargin));
    borders.setAttribute("top", QString::number(pageLayout.topMargin));
    borders.setAttribute("right", QString::number(pageLayout.rightMargin));
    borders.setAttribute("bottom", QString::number(pageLayout.bottomMargin));
    paper.appendChild(borders);

    QDomElement head = dd.createElement("head");
    paper.appendChild(head);
    if (!obj->headerFooter()->headLeft().isEmpty()) {
        QDomElement left = dd.createElement("left");
        head.appendChild(left);
        left.appendChild(dd.createTextNode(obj->headerFooter()->headLeft()));
    }
    if (!obj->headerFooter()->headMid().isEmpty()) {
        QDomElement center = dd.createElement("center");
        head.appendChild(center);
        center.appendChild(dd.createTextNode(obj->headerFooter()->headMid()));
    }
    if (!obj->headerFooter()->headRight().isEmpty()) {
        QDomElement right = dd.createElement("right");
        head.appendChild(right);
        right.appendChild(dd.createTextNode(obj->headerFooter()->headRight()));
    }
    QDomElement foot = dd.createElement("foot");
    paper.appendChild(foot);
    if (!obj->headerFooter()->footLeft().isEmpty()) {
        QDomElement left = dd.createElement("left");
        foot.appendChild(left);
        left.appendChild(dd.createTextNode(obj->headerFooter()->footLeft()));
    }
    if (!obj->headerFooter()->footMid().isEmpty()) {
        QDomElement center = dd.createElement("center");
        foot.appendChild(center);
        center.appendChild(dd.createTextNode(obj->headerFooter()->footMid()));
    }
    if (!obj->headerFooter()->footRight().isEmpty()) {
        QDomElement right = dd.createElement("right");
        foot.appendChild(right);
        right.appendChild(dd.createTextNode(obj->headerFooter()->footRight()));
    }

    // print range
    QDomElement printrange = dd.createElement("printrange-rect");
    QRect _printRange = obj->printSettings()->printRegion().lastRange();
    int left = _printRange.left();
    int right = _printRange.right();
    int top = _printRange.top();
    int bottom = _printRange.bottom();
    //If whole rows are selected, then we store zeros, as KS_colMax may change in future
    if (left == 1 && right == KS_colMax) {
        left = 0;
        right = 0;
    }
    //If whole columns are selected, then we store zeros, as KS_rowMax may change in future
    if (top == 1 && bottom == KS_rowMax) {
        top = 0;
        bottom = 0;
    }
    printrange.setAttribute("left-rect", QString::number(left));
    printrange.setAttribute("right-rect", QString::number(right));
    printrange.setAttribute("bottom-rect", QString::number(bottom));
    printrange.setAttribute("top-rect", QString::number(top));
    sheet.appendChild(printrange);

    // Print repeat columns
    QDomElement printRepeatColumns = dd.createElement("printrepeatcolumns");
    printRepeatColumns.setAttribute("left", QString::number(obj->printSettings()->repeatedColumns().first));
    printRepeatColumns.setAttribute("right", QString::number(obj->printSettings()->repeatedColumns().second));
    sheet.appendChild(printRepeatColumns);

    // Print repeat rows
    QDomElement printRepeatRows = dd.createElement("printrepeatrows");
    printRepeatRows.setAttribute("top", QString::number(obj->printSettings()->repeatedRows().first));
    printRepeatRows.setAttribute("bottom", QString::number(obj->printSettings()->repeatedRows().second));
    sheet.appendChild(printRepeatRows);

    //Save print zoom
    sheet.setAttribute("printZoom", QString::number(obj->printSettings()->zoom()));

    //Save page limits
    const QSize pageLimits = obj->printSettings()->pageLimits();
    sheet.setAttribute("printPageLimitX", QString::number(pageLimits.width()));
    sheet.setAttribute("printPageLimitY", QString::number(pageLimits.height()));

    // Save all cells.
    const QRect usedArea = obj->usedArea();
    for (int row = 1; row <= usedArea.height(); ++row) {
        Cell cell = obj->fullCellStorage()->firstInRow(row);
        while (!cell.isNull()) {
            QDomElement e = saveCell (&cell, dd);
            if (!e.isNull())
                sheet.appendChild(e);
            cell = obj->fullCellStorage()->nextInRow(cell.column(), row);
        }
    }

    // Save all RowFormat objects.
    int styleIndex = obj->styleStorage()->nextRowStyleIndex(0);
    int rowFormatRow = 0, lastRowFormatRow = obj->rowFormats()->lastNonDefaultRow();
    while (styleIndex || rowFormatRow <= lastRowFormatRow) {
        int lastRow;
        bool isDefault = obj->rowFormats()->isDefaultRow(rowFormatRow, &lastRow);
        if (isDefault && styleIndex <= lastRow) {
            QDomElement e = saveRowFormat (styleIndex, dd, obj);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            styleIndex = obj->styleStorage()->nextRowStyleIndex(styleIndex);
        } else if (!isDefault) {
            QDomElement e = saveRowFormat (rowFormatRow, dd, obj);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            if (styleIndex == rowFormatRow)
                styleIndex = obj->styleStorage()->nextRowStyleIndex(styleIndex);
        }
        if (isDefault) rowFormatRow = qMin(lastRow+1, styleIndex == 0 ? KS_rowMax : styleIndex);
        else rowFormatRow++;
    }

    // Save all ColumnFormat objects.
    styleIndex = obj->styleStorage()->nextColumnStyleIndex(0);
    int colFormatCol = 0, lastColFormatCol = obj->columnFormats()->lastNonDefaultCol();
    while (styleIndex || colFormatCol <= lastColFormatCol) {
        int lastCol;
        bool isDefault = obj->columnFormats()->isDefaultCol(colFormatCol, &lastCol);
        if (isDefault && styleIndex <= lastCol) {
            QDomElement e = saveColFormat (styleIndex, dd, obj);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            styleIndex = obj->styleStorage()->nextColumnStyleIndex(styleIndex);
        } else if (!isDefault) {
            QDomElement e = saveColFormat (colFormatCol, dd, obj);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            if (styleIndex == colFormatCol)
                styleIndex = obj->styleStorage()->nextColumnStyleIndex(styleIndex);
        }
        if (isDefault) colFormatCol = qMin(lastCol+1, styleIndex == 0 ? KS_colMax : styleIndex);
        else colFormatCol++;
    }

    return sheet;
}



QDomElement Ksp::saveRowFormat(int rowID, QDomDocument& doc, Sheet *sheet)
{
    RowFormatStorage *rows = sheet->rowFormats();

    QDomElement row = doc.createElement("row");
    row.setAttribute("height", QString::number(rows->rowHeight(rowID)));
    row.setAttribute("row", QString::number(rowID));
    if (rows->isHidden(rowID))
        row.setAttribute("hide", "1");

    const Style style = sheet->fullCellStorage()->style(QRect(1, rowID, KS_colMax, 1));
    if (!style.isEmpty()) {
        debugSheetsODF << "saving cell style of row" << rowID;
        QDomElement format(doc.createElement("format"));
        saveStyle (style, doc, format, sheet->fullMap()->styleManager());
        row.appendChild(format);
    }

    return row;
}

bool Ksp::loadRowFormat (const KoXmlElement & row, Sheet *sheet)
{
    bool ok;

    RowFormatStorage *rows = sheet->rowFormats();

    int rowID = row.attribute("row").toInt(&ok);
    if (rowID < 1 || rowID > KS_rowMax) {
        debugSheets << "Value row=" << rowID << " out of range";
        return false;
    }

    if (row.hasAttribute("height")) {
        double h = row.attribute("height").toDouble(&ok);
        if (!ok) return false;
        if (h < 0) {
            debugSheets << "Value height=" << h << " out of range";
            return false;
        }

        if (sheet->fullMap()->syntaxVersion() < 1) //compatibility with old format - was in millimeter
            h = qRound(MM_TO_POINT(h));
        rows->setRowHeight (rowID, rowID, h);
    }

    if (row.hasAttribute("hide")) {
        bool hide = (bool) row.attribute("hide").toInt(&ok);
        if (!ok) return false;
        rows->setHidden (rowID, rowID, hide);
    }

    KoXmlElement el(row.namedItem("format").toElement());

    if (!el.isNull()) {
        Style style;
        if (!loadStyle (&style, el))
            return false;
        sheet->fullCellStorage()->setStyle(Region(QRect(1, rowID, KS_colMax, 1)), style);
        return true;
    }

    return true;
}

QDomElement Ksp::saveColFormat(int colID, QDomDocument& doc, Sheet *sheet)
{
    ColFormatStorage *cols = sheet->columnFormats();

    QDomElement col(doc.createElement("column"));
    col.setAttribute("width", QString::number(cols->colWidth(colID)));
    col.setAttribute("column", QString::number(colID));

    if (cols->isHidden(colID))
        col.setAttribute("hide", "1");

    const Style style = sheet->fullCellStorage()->style(QRect(colID, 1, 1, KS_rowMax));
    if (!style.isEmpty()) {
        debugSheetsODF << "saving cell style of column" << colID;
        QDomElement format(doc.createElement("format"));
        saveStyle (style, doc, format, sheet->fullMap()->styleManager());
        col.appendChild(format);
    }

    return col;
}

bool Ksp::loadColFormat(const KoXmlElement & col, Sheet *sheet)
{
    bool ok;

    ColFormatStorage *cols = sheet->columnFormats();

    int colID = col.attribute("col").toInt(&ok);
    if (colID < 1 || colID > KS_colMax) {
        debugSheets << "Value col=" << colID << " out of range";
        return false;
    }

    if (col.hasAttribute("width")) {
        double w = col.attribute("width").toDouble(&ok);
        if (!ok) return false;
        if (w < 0) {
            debugSheets << "Value width=" << w << " out of range";
            return false;
        }

        if (sheet->fullMap()->syntaxVersion() < 1) //compatibility with old format - was in millimeter
            w = qRound(MM_TO_POINT(w));
        cols->setColWidth (colID, colID, w);
    }

    if (col.hasAttribute("hide")) {
        bool hide = (bool) col.attribute("hide").toInt(&ok);
        if (!ok) return false;
        cols->setHidden (colID, colID, hide);
    }

    KoXmlElement el(col.namedItem("format").toElement());

    if (!el.isNull()) {
        Style style;
        if (!loadStyle (&style, el))
            return false;
        sheet->fullCellStorage()->setStyle(Region(QRect(colID, 1, 1, KS_rowMax)), style);
        return true;
    }

    return true;
}



}  // Sheets
}  // Calligra
