/* This file is part of the KDE project
   Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

#include <ExcelExport.h>
#include <ExcelExport.moc>

#include <QFont>
#include <QFontMetricsF>
#include <QMap>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>
#include <KoPostscriptPaintDevice.h>

#include <part/Doc.h>
#include <CellStorage.h>
#include <Map.h>
#include <Sheet.h>
#include <RowColumnFormat.h>
#include <kspread_limits.h>

#include <swinder.h>
#include <XlsRecordOutputStream.h>

#include <CFBWriter.h>

typedef KGenericFactory<ExcelExport> ExcelExportFactory;
K_EXPORT_COMPONENT_FACTORY(libexcelexport, ExcelExportFactory("kofficefilters"))

using namespace Swinder;

class ExcelExport::Private
{
public:
    const KSpread::Doc* inputDoc;
    QString outputFile;
    XlsRecordOutputStream* out;
};

ExcelExport::ExcelExport(QObject* parent, const QStringList&)
        : KoFilter(parent)
{
    d = new Private;
}

ExcelExport::~ExcelExport()
{
    delete d;
}

KoFilter::ConversionStatus ExcelExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "application/vnd.ms-excel")
        return KoFilter::NotImplemented;

    if (from != "application/vnd.oasis.opendocument.spreadsheet")
        return KoFilter::NotImplemented;

    d->outputFile = m_chain->outputFile();

    KoDocument* document = m_chain->inputDocument();
    if (!document)
        return KoFilter::StupidError;

    d->inputDoc = qobject_cast<const KSpread::Doc*>(document);
    if (!d->inputDoc) {
        kWarning() << "document isn't a KSpread::Doc but a " << document->metaObject()->className();
    }

    CFBWriter w(false);
    w.open(d->outputFile);
    w.setRootClassId(QUuid("{00020820-0000-0000-c000-000000000046 }"));
    QIODevice* a = w.openSubStream("Workbook");
    XlsRecordOutputStream o(a);
    d->out = &o;
    {
        BOFRecord b(0);
        b.setType(BOFRecord::Workbook);
        b.setRecordSize(16);
        o.writeRecord(b);
    }

    o.writeRecord(InterfaceHdrRecord(0));
    o.writeRecord(MmsReservedRecord(0));
    o.writeRecord(InterfaceEndRecord(0));

    {
        LastWriteAccessRecord lwar(0);
        lwar.setUserName("marijn"); // TODO: figure out real username
        lwar.setUnusedBlob(QByteArray(112 - 3 - 2*lwar.userName().length(), ' '));
        o.writeRecord(lwar);
    }

    o.writeRecord(CodePageRecord(0));
    o.writeRecord(DSFReservedRecord(0));

    {
        RRTabIdRecord rrti(0);
        rrti.setSheetCount(d->inputDoc->map()->count());
        for (int i = 0; i < d->inputDoc->map()->count(); i++) {
            rrti.setSheetId(i, i+1);
        }
        o.writeRecord(rrti);
    }

    o.writeRecord(WinProtectRecord(0));
    o.writeRecord(ProtectRecord(0));
    o.writeRecord(PasswordRecord(0));
    o.writeRecord(Prot4RevRecord(0));
    o.writeRecord(Prot4RevPassRecord(0));

    o.writeRecord(Window1Record(0));
    o.writeRecord(BackupRecord(0));
    o.writeRecord(HideObjRecord(0));
    o.writeRecord(DateModeRecord(0));
    o.writeRecord(CalcPrecisionRecord(0));
    o.writeRecord(RefreshAllRecord(0));
    o.writeRecord(BookBoolRecord(0));

    {
        FontRecord fnt(0);
        fnt.setFontName("Arial");
        o.writeRecord(fnt);
    }

    QMap<int, QString> formats;
    formats.insert(5, QString::fromUtf8("#,##0\\ \"€\";\\-#,##0\\ \"€\""));
    formats.insert(6, QString::fromUtf8("#,##0\\ \"€\";[Red]\\-#,##0\\ \"€\""));
    formats.insert(7, QString::fromUtf8("#,##0.00\\ \"€\";\\-#,##0.00\\ \"€\""));
    formats.insert(8, QString::fromUtf8("#,##0.00\\ \"€\";[Red]\\-#,##0.00\\ \"€\""));
    formats.insert(41, QString::fromUtf8("_-* #,##0\\ _€_-;\\-* #,##0\\ _€_-;_-* \"-\"\\ _€_-;_-@_-"));
    formats.insert(42, QString::fromUtf8("_-* #,##0\\ \"€\"_-;\\-* #,##0\\ \"€\"_-;_-* \"-\"\\ \"€\"_-;_-@_-"));
    formats.insert(43, QString::fromUtf8("_-* #,##0.00\\ _€_-;\\-* #,##0.00\\ _€_-;_-* \"-\"??\\ _€_-;_-@_-"));
    formats.insert(44, QString::fromUtf8("_-* #,##0.00\\ \"€\"_-;\\-* #,##0.00\\ \"€\"_-;_-* \"-\"??\\ \"€\"_-;_-@_-"));
    for (QMap<int, QString>::iterator i = formats.begin(); i != formats.end(); ++i) {
        FormatRecord fr(0);
        fr.setIndex(i.key());
        fr.setFormatString(i.value());
        o.writeRecord(fr);
    }

    // XLS requires 16 XF records for some reason
    for (int i = 0; i < 16; i++) {
        o.writeRecord(XFRecord(0));
    }

    o.writeRecord(StyleRecord(0));
    o.writeRecord(UsesELFsRecord(0));

    QList<BoundSheetRecord> boundSheets;
    for (int i = 0; i < d->inputDoc->map()->count(); i++) {
        boundSheets.append(BoundSheetRecord(0));
        BoundSheetRecord& bsr = boundSheets.last();
        bsr.setSheetName(d->inputDoc->map()->sheet(i)->sheetName());
        o.writeRecord(bsr);
    }

    o.writeRecord(CountryRecord(0));

    // TODO: proper SST
    o.startRecord(SSTRecord::id);
    o.writeUnsigned(32, 0);
    o.writeUnsigned(32, 0);
    o.endRecord();

    o.writeRecord(ExtSSTRecord(0));
    o.writeRecord(EOFRecord(0));

    for (int i = 0; i < d->inputDoc->map()->count(); i++) {
        boundSheets[i].setBofPosition(o.pos());
        o.rewriteRecord(boundSheets[i]);
        convertSheet(d->inputDoc->map()->sheet(i));
    }

    delete a;
    w.close();

    emit sigProgress(100);

    return KoFilter::OK;
}

static unsigned convertColumnWidth(qreal width)
{
    static qreal factor = -1;
    if (factor == -1) {
        QFont f("Arial", 10);
        KoPostscriptPaintDevice pd;
        QFontMetricsF fm(f, &pd);
        for (char c = '0'; c <= '9'; c++) {
            factor = qMax(factor, fm.width(c));
        }
    }
    return width / factor * 256;
}

void ExcelExport::convertSheet(KSpread::Sheet* sheet)
{
    XlsRecordOutputStream& o = *d->out;
    {
        BOFRecord b(0);
        b.setType(BOFRecord::Worksheet);
        b.setRecordSize(16);
        o.writeRecord(b);
    }

    QRect area = sheet->usedArea();

    IndexRecord ir(0);
    ir.setRowMin(area.top()-1);
    ir.setRowMaxPlus1(area.bottom());
    int dbCellCount = ((area.height()+1)+31) / 32;
    ir.setRowBlockCount(dbCellCount);
    o.writeRecord(ir);

    o.writeRecord(CalcModeRecord(0));
    o.writeRecord(CalcCountRecord(0));
    o.writeRecord(CalcRefModeRecord(0));
    o.writeRecord(CalcIterRecord(0));
    o.writeRecord(CalcDeltaRecord(0));
    o.writeRecord(CalcSaveRecalcRecord(0));
    o.writeRecord(PrintRowColRecord(0));
    o.writeRecord(PrintGridRecord(0));
    o.writeRecord(GridSetReservedRecord(0));
    o.writeRecord(GutsRecord(0));

    o.writeRecord(DefaultRowHeightRecord(0));
    o.writeRecord(WsBoolRecord(0));

    o.writeRecord(HeaderRecord(0));
    o.writeRecord(FooterRecord(0));
    o.writeRecord(HCenterRecord(0));
    o.writeRecord(VCenterRecord(0));
    o.writeRecord(SetupRecord(0));

    ir.setDefColWidthPosition(o.pos());
    o.writeRecord(DefaultColWidthRecord(0)); // TODO: real defaultColWidthRecord
    {
        ColInfoRecord cir(0);
        for (int i = 1; i <= area.right(); ++i) {
            const KSpread::ColumnFormat* column = sheet->columnFormat(i);
            unsigned w = convertColumnWidth(column->width());
            if (w != cir.width() || column->isHidden() != cir.isHidden() || column->isDefault() != !cir.isNonDefaultWidth()) {
                if (i > 1) {
                    o.writeRecord(cir);
                }
                cir.setFirstColumn(i-1);
                cir.setWidth(w);
                cir.setHidden(column->isHidden());
                cir.setNonDefaultWidth(!column->isDefault());
            }
            cir.setLastColumn(i-1);
        }
        o.writeRecord(cir);
    }

    {
        DimensionRecord dr(0);
        dr.setFirstRow(area.top()-1);
        dr.setFirstColumn(area.left()-1);
        dr.setLastRowPlus1(area.bottom());
        dr.setLastColumnPlus1(area.right());
        o.writeRecord(dr);
    }

    // Row, CELL, DbCell
    for (int i = 0; i < dbCellCount; i++) {
        int firstRow = i*32 + area.top();
        int lastRowP1 = qMin(firstRow+32, area.bottom());
        qint64 firstRowPos = o.pos();

        qint64 lastStart = -1;
        for (int row = firstRow; row < lastRowP1; row++) {
            RowRecord rr(0);

            KSpread::Cell first = sheet->cellStorage()->firstInRow(row);
            if (first.isNull()) first = KSpread::Cell(sheet, 1, row);
            KSpread::Cell last = sheet->cellStorage()->lastInRow(row);
            if (last.isNull()) last = first;
            const KSpread::RowFormat* format = sheet->rowFormat(row);

            rr.setRow(row-1);
            rr.setFirstColumn(first.column()-1);
            rr.setLastColumnPlus1(last.column());
            rr.setHeight(format->height() * 20);

            o.writeRecord(rr);
            if (row == firstRow) lastStart = o.pos();
        }

        DBCellRecord db(0);
        db.setRowCount(lastRowP1 - firstRow);
        for (int row = firstRow; row < lastRowP1; row++) {
            db.setCellOffset(row - firstRow, o.pos() - lastStart);
            lastStart = o.pos();

            KSpread::Cell first = sheet->cellStorage()->firstInRow(row);
            if (first.isNull()) first = KSpread::Cell(sheet, 1, row);
            KSpread::Cell last = sheet->cellStorage()->lastInRow(row);
            if (last.isNull()) last = first;

            for (int col = first.column(); col <= last.column(); col++) {
                KSpread::Cell cell(sheet, col, row);

                if (cell.value().isNumber()) {
                    NumberRecord nr(0);
                    nr.setRow(row-1);
                    nr.setColumn(col-1);
                    nr.setNumber(cell.value().asFloat());
                    o.writeRecord(nr);
                } else /*if (cell.isEmpty())*/ {
                    BlankRecord br(0);
                    br.setRow(row-1);
                    br.setColumn(col-1);
                    o.writeRecord(br);
                }
            }
        }

        db.setFirstRowOffset(o.pos() - firstRowPos);
        o.writeRecord(db);
    }

    o.rewriteRecord(ir);

    o.writeRecord(Window2Record(0));

    // MergeCells

    o.writeRecord(EOFRecord(0));
}


