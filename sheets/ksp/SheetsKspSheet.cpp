/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "Sheet.h"

#include <KoPageLayout>

#include "DocBase.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "PrintSettings.h"
#include "SheetPrint.h"

namespace Calligra {
namespace Sheets {


namespace Ksp {
    QDomElement saveRowFormat(RowFormat *f, QDomDocument&, Sheet *sheet, int yshift = 0);
    bool loadRowFormat(RowFormat *f, const KoXmlElement& row, Sheet *sheet, int yshift = 0, Paste::Mode mode = Paste::Normal);

    QDomElement saveColFormat(ColumnFormat *f, QDomDocument&, Sheet *sheet, int xshift = 0) const;
    bool loadColFormat(ColumnFormat *f, const KoXmlElement& row, Sheet *sheet, int xshift = 0, Paste::Mode mode = Paste::Normal);
}


bool Ksp::loadSheet(Sheet *obj, const KoXmlElement& sheet)
{
    bool ok = false;
    QString sname = sheetName();
    if (!obj->map()->loadingInfo()->loadTemplate()) {
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
        obj->setSheetName(sname, true);
    }

//     (dynamic_cast<SheetIface*>(obj->dcopObject()))->sheetNameHasChanged();

    if (sheet.hasAttribute("grid")) {
        obj->setShowGrid((int)sheet.attribute("grid").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printGrid")) {
        obj->print()->settings()->setPrintGrid((bool)sheet.attribute("printGrid").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printCommentIndicator")) {
        obj->print()->settings()->setPrintCommentIndicator((bool)sheet.attribute("printCommentIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printFormulaIndicator")) {
        obj->print()->settings()->setPrintFormulaIndicator((bool)sheet.attribute("printFormulaIndicator").toInt(&ok));
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
        obj->print()->settings()->setPageLayout(pageLayout);

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
        obj->print()->headerFooter()->setHeadFootLine(hleft, hcenter, hright, fleft, fcenter, fright);
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
            const int horizontalLimit = printSettings()->pageLimits().width();
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
                loadCell(e, obj, 0, 0);
            else if (tagName == "row") {
                RowFormat *rl = new RowFormat();
                rl->setSheet(obj);
                if (loadRowFormat (rl, e, obj))
                    obj->insertRowFormat(rl);
                delete rl;
            } else if (tagName == "column") {
                ColumnFormat *cl = new ColumnFormat();
                cl->setSheet(obj);
                if (loadColFormat (cl, e, obj))
                    obj->insertColumnFormat(cl);
                else
                    delete cl;
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
    sheet.setAttribute("printGrid", QString::number((int)obj->print()->settings()->printGrid()));
    sheet.setAttribute("printCommentIndicator", QString::number((int)obj->print()->settings()->printCommentIndicator()));
    sheet.setAttribute("printFormulaIndicator", QString::number((int)obj->print()->settings()->printFormulaIndicator()));
    sheet.setAttribute("showFormula", QString::number((int)obj->getShowFormula()));
    sheet.setAttribute("showFormulaIndicator", QString::number((int)obj->getShowFormulaIndicator()));
    sheet.setAttribute("showCommentIndicator", QString::number((int)obj->getShowCommentIndicator()));
    sheet.setAttribute("lcmode", QString::number((int)obj->getLcMode()));
    sheet.setAttribute("autoCalc", QString::number((int)obj->isAutoCalculationEnabled()));
    sheet.setAttribute("borders1.2", "1");
    QByteArray pwd;
    obj->password(pwd);
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
    KoPageLayout pageLayout = obj->print()->settings()->pageLayout();
    borders.setAttribute("left", QString::number(pageLayout.leftMargin));
    borders.setAttribute("top", QString::number(pageLayout.topMargin));
    borders.setAttribute("right", QString::number(pageLayout.rightMargin));
    borders.setAttribute("bottom", QString::number(pageLayout.bottomMargin));
    paper.appendChild(borders);

    QDomElement head = dd.createElement("head");
    paper.appendChild(head);
    if (!obj->print()->headerFooter()->headLeft().isEmpty()) {
        QDomElement left = dd.createElement("left");
        head.appendChild(left);
        left.appendChild(dd.createTextNode(obj->print()->headerFooter()->headLeft()));
    }
    if (!obj->print()->headerFooter()->headMid().isEmpty()) {
        QDomElement center = dd.createElement("center");
        head.appendChild(center);
        center.appendChild(dd.createTextNode(obj->print()->headerFooter()->headMid()));
    }
    if (!obj->print()->headerFooter()->headRight().isEmpty()) {
        QDomElement right = dd.createElement("right");
        head.appendChild(right);
        right.appendChild(dd.createTextNode(obj->print()->headerFooter()->headRight()));
    }
    QDomElement foot = dd.createElement("foot");
    paper.appendChild(foot);
    if (!obj->print()->headerFooter()->footLeft().isEmpty()) {
        QDomElement left = dd.createElement("left");
        foot.appendChild(left);
        left.appendChild(dd.createTextNode(obj->print()->headerFooter()->footLeft()));
    }
    if (!obj->print()->headerFooter()->footMid().isEmpty()) {
        QDomElement center = dd.createElement("center");
        foot.appendChild(center);
        center.appendChild(dd.createTextNode(obj->print()->headerFooter()->footMid()));
    }
    if (!obj->print()->headerFooter()->footRight().isEmpty()) {
        QDomElement right = dd.createElement("right");
        foot.appendChild(right);
        right.appendChild(dd.createTextNode(obj->print()->headerFooter()->footRight()));
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
        Cell cell = obj->cellStorage()->firstInRow(row);
        while (!cell.isNull()) {
            QDomElement e = saveCell (&cell, dd);
            if (!e.isNull())
                sheet.appendChild(e);
            cell = obj->cellStorage()->nextInRow(cell.column(), row);
        }
    }

    // Save all RowFormat objects.
    int styleIndex = obj->styleStorage()->nextRowStyleIndex(0);
    int rowFormatRow = 0, lastRowFormatRow = obj->rowFormats()->lastNonDefaultRow();
    while (styleIndex || rowFormatRow <= lastRowFormatRow) {
        int lastRow;
        bool isDefault = obj->rowFormats()->isDefaultRow(rowFormatRow, &lastRow);
        if (isDefault && styleIndex <= lastRow) {
            RowFormat rowFormat(*obj->map()->defaultRowFormat());
            rowFormat.setSheet(obj);
            rowFormat.setRow(styleIndex);
            QDomElement e = saveRowFormat (rowFormat, dd, obj);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            styleIndex = obj->styleStorage()->nextRowStyleIndex(styleIndex);
        } else if (!isDefault) {
            RowFormat rowFormat(rowFormats(), rowFormatRow);
            QDomElement e = saveRowFormat (rowFormat, dd, obj);
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
    ColumnFormat* columnFormat = obj->firstCol();
    styleIndex = obj->styleStorage()->nextColumnStyleIndex(0);
    while (columnFormat || styleIndex) {
        if (columnFormat && (!styleIndex || columnFormat->column() <= styleIndex)) {
            QDomElement e = columnFormat->save(dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            if (columnFormat->column() == styleIndex)
                styleIndex = obj->styleStorage()->nextColumnStyleIndex(styleIndex);
            columnFormat = columnFormat->next();
        } else if (styleIndex) {
            ColumnFormat columnFormat(*obj->map()->defaultColumnFormat());
            columnFormat.setSheet(obj);
            columnFormat.setColumn(styleIndex);
            QDomElement e = saveColFormat (columnFormat, dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            styleIndex = styleStorage()->nextColumnStyleIndex(styleIndex);
        }
    }
    return sheet;
}



QDomElement Ksp::saveRowFormat(RowFormat *f, QDomDocument& doc, Sheet *sheet, int yshift)
{
    QDomElement row = doc.createElement("row");
    row.setAttribute("height", QString::number(f->height()));
    row.setAttribute("row", QString::number(f->row() - yshift));
    if (f->isHidden())
        row.setAttribute("hide", QString::number((int) f->isHidden()));

    const Style style = sheet->cellStorage()->style(QRect(1, f->row(), KS_colMax, 1));
    if (!style.isEmpty()) {
        debugSheetsODF << "saving cell style of row" << f->row();
        QDomElement format;
        saveStyle (&style, doc, format, sheet->map()->styleManager());
        row.appendChild(format);
    }

    return row;
}

bool Ksp::loadRowFormat (RowFormat *f, const KoXmlElement & row, Sheet *sheet, int yshift, Paste::Mode mode)
{
    bool ok;

    f->setRow (row.attribute("row").toInt(&ok) + yshift);
    if (!ok)
        return false;

    if (row.hasAttribute("height")) {
        double h = row.attribute("height").toDouble(&ok);
        if (sheet->map()->syntaxVersion() < 1) //compatibility with old format - was in millimeter
            h = qRound(MM_TO_POINT(h));
        f->setHeight (h);

        if (!ok) return false;
    }

    // Validation
    if (f->height() < 0) {
        debugSheets << "Value height=" << f->height() << " out of range";
        return false;
    }
    if (f->row() < 1 || f->row() > KS_rowMax) {
        debugSheets << "Value row=" << f->row() << " out of range";
        return false;
    }

    if (row.hasAttribute("hide")) {
        f->setHidden((int) row.attribute("hide").toInt(&ok));
        if (!ok)
            return false;
    }

    KoXmlElement el(row.namedItem("format").toElement());

    if (!el.isNull() && (mode == Paste::Normal || mode == Paste::Format || mode == Paste::NoBorder)) {
        Style style;
        if (!loadStyle (&style, el, mode))
            return false;
        sheet->cellStorage()->setStyle(Region(QRect(1, f->row(), KS_colMax, 1)), style);
        return true;
    }

    return true;
}

QDomElement Ksp::saveColFormat(ColumnFormat *f, QDomDocument& doc, Sheet *sheet, int xshift) const
{
    QDomElement col(doc.createElement("column"));
    col.setAttribute("width", QString::number(f->width()));
    col.setAttribute("column", QString::number(f->column() - xshift));

    if (f->isHidden())
        col.setAttribute("hide", QString::number((int) f->isHidden()));

    const Style style = sheet->cellStorage()->style(QRect(f->column(), 1, 1, KS_rowMax));
    if (!style.isEmpty()) {
        debugSheetsODF << "saving cell style of column" << f->column();
        QDomElement format(doc.createElement("format"));
        saveStyle (&style, doc, format, sheet->map()->styleManager());
        col.appendChild(format);
    }

    return col;
}

bool Ksp::loadColFormat(ColumnFormat *f, const KoXmlElement & col, Sheet *sheet, int xshift, Paste::Mode mode)
{
    bool ok;
    if (col.hasAttribute("width")) {
        double w = row.attribute("width").toDouble(&ok);
        if (sheet->map()->syntaxVersion() < 1) //compatibility with old format - was in millimeter
            w = qRound(MM_TO_POINT(w));
        f->setWidth (w);

        if (!ok)
            return false;
    }

    f->setColumn(col.attribute("column").toInt(&ok) + xshift);

    if (!ok)
        return false;

    // Validation
    if (f->width() < 0) {
        debugSheets << "Value width=" << f->width() << " out of range";
        return false;
    }
    if (f->column() < 1 || f->column() > KS_colMax) {
        debugSheets << "Value col=" << f->column() << " out of range";
        return false;
    }
    if (col.hasAttribute("hide")) {
        f->setHidden((int) col.attribute("hide").toInt(&ok));
        if (!ok)
            return false;
    }

    KoXmlElement el(col.namedItem("format").toElement());

    if (!el.isNull() && (mode == Paste::Normal || mode == Paste::Format || mode == Paste::NoBorder)) {
        Style style;
        if (!loadStyle (&style, el, mode))
            return false;
        sheet->cellStorage()->setStyle(Region(QRect(f->column(), 1, 1, KS_rowMax)), style);
        return true;
    }

    return true;
}



}  // Sheets
}  // Calligra
