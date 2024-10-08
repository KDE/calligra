/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2003-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2006, 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2009, 2010 Sebastian Sauer <sebsauer@kdab.com>
   SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "worksheetsubstreamhandler.h"

#include <map>

#include "cell.h"
#include "conditionals.h"
#include "excel.h"
#include "globalssubstreamhandler.h"
#include "objects.h"
#include "sheet.h"
#include <QPoint>

#include "sheets/core/DataFilter.h"

// #define SWINDER_XLS2RAW

namespace Swinder
{

class WorksheetSubStreamHandler::Private
{
public:
    Sheet *sheet;
    const GlobalsSubStreamHandler *globals;

    // for FORMULA+SHAREDFMLA record pair
    Cell *lastFormulaCell;

    // for FORMULA+STRING record pair
    Cell *formulaStringCell;

    // mapping from cell position to data tables
    std::map<std::pair<unsigned, unsigned>, DataTableRecord *> dataTables;

    // mapping from cell position to shared formulas
    std::map<std::pair<unsigned, unsigned>, FormulaTokens> sharedFormulas;

    // mapping from object id's to object instances
    std::map<unsigned long, Object *> sharedObjects;

    // maps object id's of NoteObject's to there continuous number
    std::map<unsigned long, int> noteMap;
    // the number of NoteObject's in this worksheet
    int noteCount;

    // The last drawing object we got.
    MSO::OfficeArtDgContainer *lastDrawingObject;
    MSO::OfficeArtSpgrContainer *lastGroupObject;
    OfficeArtObject *lastOfficeArtObject;
    quint32 officeArtObjectCounter;

    // list of id's with ChartObject's.
    std::vector<unsigned long> charts;

    // current ConditionalFormat
    ConditionalFormat *curConditionalFormat;
};

WorksheetSubStreamHandler::WorksheetSubStreamHandler(Sheet *sheet, const GlobalsSubStreamHandler *globals)
    : SubStreamHandler()
    , FormulaDecoder()
    , d(new Private)
{
    d->sheet = sheet;
    d->globals = globals;
    d->lastFormulaCell = nullptr;
    d->formulaStringCell = nullptr;
    d->noteCount = 0;
    d->lastDrawingObject = nullptr;
    d->lastGroupObject = nullptr;
    d->lastOfficeArtObject = nullptr;
    d->officeArtObjectCounter = 0;
    d->curConditionalFormat = nullptr;
}

WorksheetSubStreamHandler::~WorksheetSubStreamHandler()
{
    for (std::map<std::pair<unsigned, unsigned>, DataTableRecord *>::iterator it = d->dataTables.begin(); it != d->dataTables.end(); ++it)
        delete (*it).second;
    // for(std::map<unsigned long, Object*>::iterator it = d->sharedObjects.begin(); it != d->sharedObjects.end(); ++it)
    //     delete (*it).second;
    // for(std::map<std::pair<unsigned, unsigned>, FormulaTokens*>::iterator it = d->sharedFormulas.begin(); it != d->sharedFormulas.end(); ++it)
    //     delete it.second.second;
    delete d->lastDrawingObject;
    delete d->lastGroupObject;
    delete d;
}

Sheet *WorksheetSubStreamHandler::sheet() const
{
    return d->sheet;
}

std::map<unsigned long, Object *> &WorksheetSubStreamHandler::sharedObjects() const
{
    return d->sharedObjects;
}

std::vector<unsigned long> &WorksheetSubStreamHandler::charts() const
{
    return d->charts;
}

const std::vector<QString> &WorksheetSubStreamHandler::externSheets() const
{
    return d->globals->externSheets();
}

QString WorksheetSubStreamHandler::nameFromIndex(unsigned index) const
{
    return d->globals->nameFromIndex(index);
}

QString WorksheetSubStreamHandler::externNameFromIndex(unsigned index) const
{
    return d->globals->externNameFromIndex(index);
}

FormulaTokens WorksheetSubStreamHandler::sharedFormulas(const std::pair<unsigned, unsigned> &formulaCellPos) const
{
    std::map<std::pair<unsigned, unsigned>, FormulaTokens>::iterator sharedFormula = d->sharedFormulas.find(formulaCellPos);
    return sharedFormula != d->sharedFormulas.end() ? sharedFormula->second : FormulaTokens();
}

DataTableRecord *WorksheetSubStreamHandler::tableRecord(const std::pair<unsigned, unsigned> &formulaCellPos) const
{
    std::map<std::pair<unsigned, unsigned>, DataTableRecord *>::iterator datatable = d->dataTables.find(formulaCellPos);
    return datatable != d->dataTables.end() ? datatable->second : 0;
}

void WorksheetSubStreamHandler::handleRecord(Record *record)
{
    if (!record)
        return;

    const unsigned type = record->rtti();

    if (type == BottomMarginRecord::id)
        handleBottomMargin(static_cast<BottomMarginRecord *>(record));
    else if (type == BoolErrRecord::id)
        handleBoolErr(static_cast<BoolErrRecord *>(record));
    else if (type == BlankRecord::id)
        handleBlank(static_cast<BlankRecord *>(record));
    else if (type == CalcModeRecord::id)
        handleCalcMode(static_cast<CalcModeRecord *>(record));
    else if (type == ColInfoRecord::id)
        handleColInfo(static_cast<ColInfoRecord *>(record));
    else if (type == DataTableRecord::id)
        handleDataTable(static_cast<DataTableRecord *>(record));
    else if (type == FormulaRecord::id)
        handleFormula(static_cast<FormulaRecord *>(record));
    else if (type == FooterRecord::id)
        handleFooter(static_cast<FooterRecord *>(record));
    else if (type == HeaderRecord::id)
        handleHeader(static_cast<HeaderRecord *>(record));
    else if (type == LabelRecord::id)
        handleLabel(static_cast<LabelRecord *>(record));
    else if (type == LabelSSTRecord::id)
        handleLabelSST(static_cast<LabelSSTRecord *>(record));
    else if (type == LeftMarginRecord::id)
        handleLeftMargin(static_cast<LeftMarginRecord *>(record));
    else if (type == MergedCellsRecord::id)
        handleMergedCells(static_cast<MergedCellsRecord *>(record));
    else if (type == MulBlankRecord::id)
        handleMulBlank(static_cast<MulBlankRecord *>(record));
    else if (type == MulRKRecord::id)
        handleMulRK(static_cast<MulRKRecord *>(record));
    else if (type == NumberRecord::id)
        handleNumber(static_cast<NumberRecord *>(record));
    else if (type == RightMarginRecord::id)
        handleRightMargin(static_cast<RightMarginRecord *>(record));
    else if (type == RKRecord::id)
        handleRK(static_cast<RKRecord *>(record));
    else if (type == RowRecord::id)
        handleRow(static_cast<RowRecord *>(record));
    else if (type == RStringRecord::id)
        handleRString(static_cast<RStringRecord *>(record));
    else if (type == SharedFormulaRecord::id)
        handleSharedFormula(static_cast<SharedFormulaRecord *>(record));
    else if (type == StringRecord::id)
        handleString(static_cast<StringRecord *>(record));
    else if (type == TopMarginRecord::id)
        handleTopMargin(static_cast<TopMarginRecord *>(record));
    else if (type == HLinkRecord::id)
        handleHLink(static_cast<HLinkRecord *>(record));
    else if (type == NoteRecord::id)
        handleNote(static_cast<NoteRecord *>(record));
    else if (type == ObjRecord::id)
        handleObj(static_cast<ObjRecord *>(record));
    else if (type == TxORecord::id)
        handleTxO(static_cast<TxORecord *>(record));
    else if (type == BOFRecord::id)
        handleBOF(static_cast<BOFRecord *>(record));
    else if (type == DefaultRowHeightRecord::id)
        handleDefaultRowHeight(static_cast<DefaultRowHeightRecord *>(record));
    else if (type == DefaultColWidthRecord::id)
        handleDefaultColWidth(static_cast<DefaultColWidthRecord *>(record));
    else if (type == SetupRecord::id)
        handleSetup(static_cast<SetupRecord *>(record));
    else if (type == HCenterRecord::id)
        handleHCenter(static_cast<HCenterRecord *>(record));
    else if (type == VCenterRecord::id)
        handleVCenter(static_cast<VCenterRecord *>(record));
    else if (type == ZoomLevelRecord::id)
        handleZoomLevel(static_cast<ZoomLevelRecord *>(record));
    else if (type == 0xA) {
    } // EofRecord
    else if (type == DimensionRecord::id)
        handleDimension(static_cast<DimensionRecord *>(record));
    else if (type == MsoDrawingRecord::id)
        handleMsoDrawing(static_cast<MsoDrawingRecord *>(record));
    else if (type == Window2Record::id)
        handleWindow2(static_cast<Window2Record *>(record));
    else if (type == PasswordRecord::id)
        handlePassword(static_cast<PasswordRecord *>(record));
    else if (type == BkHimRecord::id)
        handleBkHim(static_cast<BkHimRecord *>(record));
    else if (type == VerticalPageBreaksRecord::id)
        handleVerticalPageBreaksRecord(static_cast<VerticalPageBreaksRecord *>(record));
    else if (type == HorizontalPageBreaksRecord::id)
        handleHorizontalPageBreaksRecord(static_cast<HorizontalPageBreaksRecord *>(record));
    else if (type == CondFmtRecord::id)
        handleCondFmtRecord(static_cast<CondFmtRecord *>(record));
    else if (type == CFRecord::id)
        handleCFRecord(static_cast<CFRecord *>(record));
    else if (type == AutoFilterRecord::id)
        handleAutoFilterRecord(static_cast<AutoFilterRecord *>(record));
    else {
        // qCDebug(lcSidewinder) << "Unhandled worksheet record with type=" << type << " name=" << record->name();
    }
}

void WorksheetSubStreamHandler::handleBOF(BOFRecord *record)
{
    if (!record)
        return;

    if (record->type() == BOFRecord::Worksheet) {
        // ...
    } else {
        qCDebug(lcSidewinder) << "WorksheetSubStreamHandler::handleBOF Unhandled type=" << record->type();
    }
}

void WorksheetSubStreamHandler::handleBlank(BlankRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();

    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleBoolErr(BoolErrRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();

    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(record->asValue());
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleBottomMargin(BottomMarginRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    // convert from inches to points
    double margin = record->bottomMargin() * 72.0;
    d->sheet->setBottomMargin(margin);
}

void WorksheetSubStreamHandler::handleCalcMode(CalcModeRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    d->sheet->setAutoCalc(record->calcMode() != CalcModeRecord::Manual);
}

void WorksheetSubStreamHandler::handleColInfo(ColInfoRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned firstColumn = record->firstColumn();
    unsigned lastColumn = record->lastColumn();
    unsigned xfIndex = record->xfIndex();
    unsigned width = record->width();
    bool hidden = record->isHidden();

    for (unsigned i = firstColumn; i <= lastColumn; ++i) {
        Column *column = d->sheet->column(i, true);
        if (column) {
            column->setWidth(Column::columnUnitsToPts((double)width));
            column->setFormat(d->globals->convertedFormat(xfIndex));
            column->setVisible(!hidden);
            column->setOutlineLevel(record->outlineLevel());
            column->setCollapsed(record->isCollapsed());
        }
    }
}

void WorksheetSubStreamHandler::handleDataTable(DataTableRecord *record)
{
    if (!record)
        return;
    if (!d->lastFormulaCell)
        return;

    unsigned row = d->lastFormulaCell->row();
    unsigned column = d->lastFormulaCell->column();

    d->dataTables[std::make_pair(row, column)] = new DataTableRecord(*record);

    QString formula = dataTableFormula(row, column, record);
    d->lastFormulaCell->setFormula(formula);

    d->lastFormulaCell = nullptr;
}

void WorksheetSubStreamHandler::handleDimension(DimensionRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    // in the mean time we don't need to handle this because we don't care
    // about the used range of the sheet
    d->sheet->setMaxRow(record->lastRow());
    d->sheet->setMaxColumn(record->lastColumn());
}

void WorksheetSubStreamHandler::handleFormula(FormulaRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    Value value = record->result();
    QString formula = decodeFormula(row, column, record->isShared(), record->tokens());
    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(value);
        if (!formula.isEmpty())
            cell->setFormula(formula);

        cell->setFormat(d->globals->convertedFormat(xfIndex));

        // if value is string, real value is in subsequent String record
        if (value.isString())
            d->formulaStringCell = cell;
        d->lastFormulaCell = cell;
    }
}

void WorksheetSubStreamHandler::handleFooter(FooterRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    QString footer = record->footer();
    QString left, center, right;
    int pos = -1, len = 0;

    // left part
    pos = footer.indexOf("&L");
    if (pos >= 0) {
        pos += 2;
        len = footer.indexOf("&C") - pos;
        if (len > 0) {
            left = footer.mid(pos, len);
            footer.remove(0, pos + len);
        } else {
            left = footer.mid(pos);
        }
    }

    // center part
    pos = footer.indexOf("&C");
    if (pos >= 0) {
        pos += 2;
        len = footer.indexOf("&R") - pos;
        if (len > 0) {
            center = footer.mid(pos, len);
            footer.remove(0, pos + len);
        } else {
            center = footer.mid(pos);
        }
    }

    // right part
    pos = footer.indexOf("&R");
    if (pos >= 0) {
        pos += 2;
        right = footer.mid(pos, footer.length() - pos);
    }

    d->sheet->setLeftFooter(left);
    d->sheet->setCenterFooter(center);
    d->sheet->setRightFooter(right);
}

void WorksheetSubStreamHandler::handleHeader(HeaderRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    QString header = record->header();
    QString left, center, right;
    int pos = -1, len = 0;

    // left part of the header
    pos = header.indexOf("&L");
    if (pos >= 0) {
        pos += 2;
        len = header.indexOf("&C") - pos;
        if (len > 0) {
            left = header.mid(pos, len);
            header = header.mid(pos + len, header.length());
        } else {
            left = header.mid(pos);
        }
    }

    // center part of the header
    pos = header.indexOf("&C");
    if (pos >= 0) {
        pos += 2;
        len = header.indexOf("&R") - pos;
        if (len > 0) {
            center = header.mid(pos, len);
            header = header.mid(pos + len, header.length());
        } else {
            center = header.mid(pos);
        }
    }

    // right part of the header
    pos = header.indexOf("&R");
    if (pos >= 0) {
        pos += 2;
        right = header.mid(pos, header.length() - pos);
    }

    d->sheet->setLeftHeader(left);
    d->sheet->setCenterHeader(center);
    d->sheet->setRightHeader(right);
}

void WorksheetSubStreamHandler::handleLabel(LabelRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    QString label = record->label();

    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(Value(label));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleLabelSST(LabelSSTRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned index = record->sstIndex();
    unsigned xfIndex = record->xfIndex();

    QString str = d->globals->stringFromSST(index);
    std::map<unsigned, FormatFont> formatRuns = d->globals->formatRunsFromSST(index);

    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        if (!formatRuns.empty())
            cell->setValue(Value(str, formatRuns));
        else
            cell->setValue(Value(str));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleLeftMargin(LeftMarginRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    // convert from inches to points
    double margin = record->leftMargin() * 72.0;
    d->sheet->setLeftMargin(margin);
}

void WorksheetSubStreamHandler::handleMergedCells(MergedCellsRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    for (unsigned i = 0; i < record->count(); ++i) {
        unsigned firstRow = record->firstRow(i);
        unsigned lastRow = record->lastRow(i);
        unsigned firstColumn = record->firstColumn(i);
        unsigned lastColumn = record->lastColumn(i);

        Cell *cell = d->sheet->cell(firstColumn, firstRow, true);
        if (cell) {
            cell->setColumnSpan(lastColumn - firstColumn + 1);
            cell->setRowSpan(lastRow - firstRow + 1);
        }
        for (unsigned row = firstRow; row <= lastRow; ++row)
            for (unsigned col = firstColumn; col <= lastColumn; ++col) {
                if (row != firstRow || col != firstColumn) {
                    d->sheet->cell(col, row, true)->setCovered(true);
                }
            }
    }
}

void WorksheetSubStreamHandler::handleMulBlank(MulBlankRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned firstColumn = record->firstColumn();
    unsigned lastColumn = record->lastColumn();
    unsigned row = record->row();

    for (unsigned column = firstColumn; column <= lastColumn; ++column) {
        Cell *cell = d->sheet->cell(column, row, true);
        if (cell) {
            cell->setFormat(d->globals->convertedFormat(record->xfIndex(column - firstColumn)));
        }
    }
}

void WorksheetSubStreamHandler::handleMulRK(MulRKRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    const int firstColumn = record->firstColumn();
    const int lastColumn = record->lastColumn();
    const int row = record->row();

    for (int column = firstColumn; column <= lastColumn; column++) {
        Cell *cell = d->sheet->cell(column, row, true);
        const int i = column - firstColumn;
        if (record->isInteger(i))
            cell->setValue(Value(record->asInteger(i)));
        else
            cell->setValue(Value(record->asFloat(i)));
        cell->setFormat(d->globals->convertedFormat(record->xfIndex(column - firstColumn)));
    }
}

void WorksheetSubStreamHandler::handleNumber(NumberRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    double number = record->number();

    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(Value(number));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleRightMargin(RightMarginRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    // convert from inches to points
    double margin = record->rightMargin() * 72.0;
    d->sheet->setRightMargin(margin);
}

void WorksheetSubStreamHandler::handleRK(RKRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();

    Value value;
    if (record->isInteger())
        value.setValue(record->asInteger());
    else
        value.setValue(record->asFloat());

    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(value);
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleRow(RowRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned index = record->row();
    unsigned xfIndex = record->xfIndex();
    unsigned height = record->height();
    bool hidden = record->isHidden();

    Row *row = d->sheet->row(index, true);
    if (row) {
        row->setHeight(height / 20.0);
        row->setFormat(d->globals->convertedFormat(xfIndex));
        row->setVisible(!hidden);
        row->setOutlineLevel(record->outlineLevel());
        row->setCollapsed(record->isCollapsed());
    }
}

void WorksheetSubStreamHandler::handleRString(RStringRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    QString label = record->label();

    Cell *cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(Value(label));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleSharedFormula(SharedFormulaRecord *record)
{
    if (!record)
        return;
    if (!d->lastFormulaCell)
        return;

    unsigned row = d->lastFormulaCell->row();
    unsigned column = d->lastFormulaCell->column();

    d->sharedFormulas[std::make_pair(row, column)] = record->tokens();

    QString formula = decodeFormula(row, column, true, record->tokens());
    d->lastFormulaCell->setFormula(formula);

    d->lastFormulaCell = nullptr;
}

void WorksheetSubStreamHandler::handleString(StringRecord *record)
{
    if (!record)
        return;
    if (!d->formulaStringCell)
        return;

    d->formulaStringCell->setValue(record->value());
    d->formulaStringCell = nullptr;
}

void WorksheetSubStreamHandler::handleTopMargin(TopMarginRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    // convert from inches to points
    double margin = record->topMargin() * 72.0;
    d->sheet->setTopMargin(margin);
}

static QString trimTrailingZero(const QString &s)
{
    if (!s.isEmpty() && s[s.length() - 1].unicode() == '\0') {
        return s.left(s.length() - 1);
    } else {
        return s;
    }
}

void WorksheetSubStreamHandler::handleHLink(HLinkRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    // FIXME we ignore the m_lastRow and m_lastColumn values, does ODF have something similar?
    Cell *cell = d->sheet->cell(record->firstColumn(), record->firstRow());
    if (cell) {
        QString url = trimTrailingZero(record->urlMonikerUrl()) + QString('#') + trimTrailingZero(record->location());
        cell->setHyperlink(Hyperlink(trimTrailingZero(record->displayName()), url, trimTrailingZero(record->frameName())));
    }
}

void WorksheetSubStreamHandler::handleTxO(TxORecord *record)
{
    if (!record)
        return;

    if (d->sharedObjects.rbegin() != d->sharedObjects.rend()) {
        NoteObject *no = dynamic_cast<NoteObject *>(d->sharedObjects.rbegin()->second);
        if (no) {
            no->setNote(record->text());
        }
    }
    if (d->lastOfficeArtObject) {
        d->lastOfficeArtObject->setText(*record);
        d->lastOfficeArtObject = nullptr;
    }
}

void WorksheetSubStreamHandler::handleNote(NoteRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;
    qCDebug(lcSidewinder) << "WorksheetSubStreamHandler::handleNote column=" << record->column() << "row=" << record->row();
    Cell *cell = d->sheet->cell(record->column(), record->row());
    if (cell) {
        const unsigned long id = record->idObj();
        NoteObject *obj = dynamic_cast<NoteObject *>(d->sharedObjects[id]);
        if (obj) {
            cell->setNote(obj->note());
        }
    }
}

void WorksheetSubStreamHandler::handleObj(ObjRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    const unsigned long id = record->m_object ? record->m_object->id() : -1;

    qCDebug(lcSidewinder) << "WorksheetSubStreamHandler::handleObj id=" << id << " type=" << (record->m_object ? record->m_object->type() : -1);

    d->lastOfficeArtObject = nullptr;

    bool handled = false;
    if (record->m_object && d->lastDrawingObject && record->m_object->applyDrawing(*(d->lastDrawingObject))) {
        handled = true;
        switch (record->m_object->type()) {
        // Note: let's handle Pictures as OfficeArtObject, not as PictureObject
        case Object::Picture: {
            handled = false;
        } break;
        case Object::Chart: {
            d->charts.push_back(id);
        } break;
        case Object::Note: {
            // a NoteRecord will follow which picks that up.
            d->noteMap[id] = ++d->noteCount;
        } break;
        default:
            handled = false;
        }
    }
    if (!handled && d->lastDrawingObject) {
        // Q_ASSERT(!d->globals->drawing(record->m_object->id()));
        foreach (const MSO::OfficeArtSpgrContainerFileBlock &fb, d->lastDrawingObject->groupShape->rgfb) {
            if (fb.anon.is<MSO::OfficeArtSpgrContainer>()) {
                delete d->lastGroupObject;
                d->lastGroupObject = new MSO::OfficeArtSpgrContainer(*fb.anon.get<MSO::OfficeArtSpgrContainer>());
            } else {
                const MSO::OfficeArtSpContainer &o = *fb.anon.get<MSO::OfficeArtSpContainer>();
                if (o.clientAnchor) {
                    MSO::XlsOfficeArtClientAnchor *anchor = o.clientAnchor->anon.get<MSO::XlsOfficeArtClientAnchor>();
                    if (!anchor) {
                        qDebug() << "invalid client anchor";
                    } else {
                        Cell *cell = d->sheet->cell(anchor->colL, anchor->rwT);
                        OfficeArtObject *obj = new OfficeArtObject(o, d->officeArtObjectCounter++);
                        cell->addDrawObject(obj);
                        d->lastOfficeArtObject = obj;
                    }
                } else {
                    OfficeArtObject *obj = new OfficeArtObject(o, d->officeArtObjectCounter++);
                    d->sheet->addDrawObject(obj, d->lastGroupObject);
                    d->lastOfficeArtObject = obj;

                    if (d->lastGroupObject) {
                        if (!o.shapeProp.fChild) {
                            delete d->lastGroupObject;
                            d->lastGroupObject = nullptr;
                        }
                    }
                }
            }
        }
    }

    if (record->m_object)
        d->sharedObjects[id] = record->m_object;
    record->m_object = nullptr; // take over ownership

    delete d->lastDrawingObject;
    d->lastDrawingObject = nullptr;
}

void WorksheetSubStreamHandler::handleDefaultRowHeight(DefaultRowHeightRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;
    if (!record->isDyZero() && record->miyRw() != 0.0)
        d->sheet->setDefaultRowHeight(record->miyRw() / 20);
}

void WorksheetSubStreamHandler::handleDefaultColWidth(DefaultColWidthRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;

    d->sheet->setDefaultColWidth(Column::columnUnitsToPts(record->cchdefColWidth() * 256.0));
}

void WorksheetSubStreamHandler::handleSetup(SetupRecord *)
{
    // TODO
}

void WorksheetSubStreamHandler::handleHCenter(HCenterRecord *)
{
    // TODO
}

void WorksheetSubStreamHandler::handleVCenter(VCenterRecord *)
{
    // TODO
}

void WorksheetSubStreamHandler::handleZoomLevel(ZoomLevelRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;
    if (record->denominator() == 0)
        return;
    d->sheet->setZoomLevel(record->numerator() / double(record->denominator()));
}

void WorksheetSubStreamHandler::handleMsoDrawing(MsoDrawingRecord *record)
{
    if (!record || !record->isValid() || !d->sheet)
        return;

    // remember the MsoDrawingRecord for the ObjRecord that is expected to follow and to proper handle the drawing object.
    delete d->lastDrawingObject;
    d->lastDrawingObject = new MSO::OfficeArtDgContainer(record->dgContainer());
}

void WorksheetSubStreamHandler::handleWindow2(Window2Record *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;
    d->sheet->setShowGrid(record->isFDspGridRt());
    d->sheet->setShowZeroValues(record->isFDspZerosRt());
    d->sheet->setFirstVisibleCell(QPoint(record->colLeft(), record->rwTop()));
    d->sheet->setPageBreakViewEnabled(record->isFSLV());
    d->sheet->setRightToLeft(record->isFRightToLeft());
}

void WorksheetSubStreamHandler::handlePassword(PasswordRecord *record)
{
    if (!record)
        return;
    if (!d->sheet)
        return;
    if (!record->wPassword())
        return;
    qCDebug(lcSidewinder) << "WorksheetSubStreamHandler::handlePassword passwordHash=" << record->wPassword();
    d->sheet->setPassword(record->wPassword());

#if 0
    quint16 nHash = record->wPassword() ^ 0xCE4B;
    quint16 nDummy = nHash;
    quint16 nLen = 9;
    while( !(nDummy & 0x8000) && nLen ) { --nLen; nDummy <<= 1; }
    if( !nLen ) nLen = 2;
    if( (nLen ^ nHash) & 0x0001 ) nLen++;
    if( nLen == 9 ) { nLen = 10; nHash ^= 0x8001; }
    nHash ^= nLen;
    if( nLen < 9 ) nHash <<= (8 - nLen);
    quint16 nNewChar = 0;
    QByteArray sPasswd;
    for( quint16 iChar = nLen; iChar > 0; iChar-- ) {
        switch( iChar ) {
            case 10:
                nNewChar = (nHash & 0xC000) | 0x0400;
                nHash ^= nNewChar;
                nNewChar >>= 2;
                break;
            case 9:
                nNewChar = 0x4200;
                nHash ^= nNewChar;
                nNewChar >>= 1;
                break;
            case 1:
                nNewChar = nHash & 0xFF00;
                break;
            default:
                nNewChar = (nHash & 0xE000) ^ 0x2000;
                if( !nNewChar ) nNewChar = (nHash & 0xF000) ^ 0x1800;
                if( nNewChar == 0x6000 ) nNewChar = 0x6100;
                nHash ^= nNewChar;
                nHash <<= 1;
                break;
        }
        nNewChar >>= 8;
        nNewChar &= 0x00FF;

        //QByteArray sDummy = sPasswd;
        //typedef sal_Char STRING16[ 16 ];
        //sPasswd = (sal_Char) nNewChar;
        //sPasswd += sDummy;

        sPasswd.prepend(QChar(nNewChar));
    }

    qCDebug(lcSidewinder) << ">>>> " << sPasswd.data(); //0x218490a
#endif
}

void WorksheetSubStreamHandler::handleBkHim(BkHimRecord *record)
{
    d->sheet->setBackgroundImage(record->imagePath());
}

void WorksheetSubStreamHandler::handleVerticalPageBreaksRecord(VerticalPageBreaksRecord *record)
{
    const unsigned int count = record->count();
    for (unsigned i = 0; i < count; ++i) {
        VerticalPageBreak pageBreak;
        pageBreak.col = record->col(i);
        pageBreak.rowStart = record->rowStart(i);
        pageBreak.rowEnd = record->rowEnd(i);
        d->sheet->addVerticalPageBreak(pageBreak);
    }
}

void WorksheetSubStreamHandler::handleHorizontalPageBreaksRecord(HorizontalPageBreaksRecord *record)
{
    const unsigned int count = record->count();
    for (unsigned i = 0; i < count; ++i) {
        HorizontalPageBreak pageBreak;
        pageBreak.row = record->row(i);
        pageBreak.colStart = record->colStart(i);
        pageBreak.colEnd = record->colEnd(i);
        d->sheet->addHorizontalPageBreak(pageBreak);
    }
}

void WorksheetSubStreamHandler::handleCondFmtRecord(Swinder::CondFmtRecord *record)
{
    QRegion region;
    for (unsigned i = 0; i < record->refCount(); ++i) {
        QRect rect(QPoint(record->firstColumn(i), record->firstRow(i)), QPoint(record->lastColumn(i), record->lastRow(i)));
        region += rect;
    }

    d->curConditionalFormat = new ConditionalFormat;
    d->curConditionalFormat->setRegion(region);
    d->sheet->addConditionalFormat(d->curConditionalFormat);
}

void WorksheetSubStreamHandler::handleCFRecord(Swinder::CFRecord *record)
{
    if (!d->curConditionalFormat)
        return;

    Conditional c;
    if (record->conditionType() == record->Formula) {
        c.cond = Conditional::Formula;
        FormulaDecoder dec;
        QByteArray rgce = record->rgce1();
        unsigned size = rgce.size();
        rgce.prepend((size >> 8) & 0xFF);
        rgce.prepend(size & 0xFF);
        FormulaTokens ts = dec.decodeFormula(rgce.size(), 0, reinterpret_cast<const unsigned char *>(rgce.data()), record->version());
        QString f =
            dec.decodeFormula(d->curConditionalFormat->region().boundingRect().top(), d->curConditionalFormat->region().boundingRect().left(), false, ts);
        c.value1 = Value(f);
    } else {
        int valcount = 1;
        c.cond = Conditional::None;
        switch (record->conditionFunction()) {
        case CFRecord::Between:
            c.cond = Conditional::Between;
            valcount = 2;
            break;
        case CFRecord::Outside:
            c.cond = Conditional::Outside;
            valcount = 2;
            break;
        case CFRecord::Equal:
            c.cond = Conditional::Equal;
            break;
        case CFRecord::NotEqual:
            c.cond = Conditional::NotEqual;
            break;
        case CFRecord::Greater:
            c.cond = Conditional::Greater;
            break;
        case CFRecord::Less:
            c.cond = Conditional::Less;
            break;
        case CFRecord::GreaterOrEqual:
            c.cond = Conditional::GreaterOrEqual;
            break;
        case CFRecord::LessOrEqual:
            c.cond = Conditional::LessOrEqual;
            break;
        }
        FormulaDecoder dec;
        QByteArray rgce = record->rgce1();
        unsigned size = rgce.size();
        rgce.prepend((size >> 8) & 0xFF);
        rgce.prepend(size & 0xFF);
        FormulaTokens ts = dec.decodeFormula(rgce.size(), 0, reinterpret_cast<const unsigned char *>(rgce.data()), record->version());
        if (ts.size() == 1
            && (ts[0].id() == FormulaToken::ErrorCode || ts[0].id() == FormulaToken::Bool || ts[0].id() == FormulaToken::Integer
                || ts[0].id() == FormulaToken::Float || ts[0].id() == FormulaToken::String)) {
            c.value1 = ts[0].value();
        } else {
            QString f =
                dec.decodeFormula(d->curConditionalFormat->region().boundingRect().top(), d->curConditionalFormat->region().boundingRect().left(), false, ts);
            c.value1 = Value(f);
        }
        if (valcount > 1) {
            rgce = record->rgce2();
            size = rgce.size();
            rgce.prepend((size >> 8) & 0xFF);
            rgce.prepend(size & 0xFF);
            ts = dec.decodeFormula(rgce.size(), 0, reinterpret_cast<const unsigned char *>(rgce.data()), record->version());
            if (ts.size() == 1
                && (ts[0].id() == FormulaToken::ErrorCode || ts[0].id() == FormulaToken::Bool || ts[0].id() == FormulaToken::Integer
                    || ts[0].id() == FormulaToken::Float || ts[0].id() == FormulaToken::String)) {
                c.value2 = ts[0].value();
            } else {
                QString f = dec.decodeFormula(d->curConditionalFormat->region().boundingRect().top(),
                                              d->curConditionalFormat->region().boundingRect().left(),
                                              false,
                                              ts);
                c.value2 = Value(f);
            }
        }
    }

    if (record->isIbitAtrNum()) {
        if (record->isFIfmtUser()) {
            c.setValueFormat(record->formatString());
        } else if (!record->isIfmtNinch()) {
            c.setValueFormat(d->globals->valueFormat(record->ifmt()));
        }
    }
    if (record->isIbitAtrFnt()) {
        // TODO: fontName
        // TODO: fontSize
        if (!record->isFontItalicNinch()) {
            c.setFontItalic(record->isFontItalic());
        }
        if (!record->isFontStrikeoutNinch()) {
            c.setFontStrikeout(record->isFontStrikeout());
        }
        if (!record->isFontWeightNinch()) {
            c.setFontBold(record->fontWeight() > 500);
        }
        if (!record->isSuperSubScriptNinch()) {
            switch (record->fontSuperSubScript()) {
            case CFRecord::SSS_Normal:
                c.setFontSubscript(false);
                c.setFontSuperscript(false);
                break;
            case CFRecord::SSS_Sub:
                c.setFontSubscript(true);
                c.setFontSuperscript(false);
                break;
            case CFRecord::SSS_Super:
                c.setFontSubscript(false);
                c.setFontSuperscript(true);
                break;
            default:
                break;
            }
        }
        if (!record->isUnderlineNinch() && record->underline() != CFRecord::UL_Ignore) {
            c.setFontUnderline(record->underline() != CFRecord::UL_None);
        }
        if (record->fontColor() >= 0) {
            c.setFontColor(d->globals->workbook()->color(record->fontColor()));
        }
    }
    if (record->isIbitAtrAlc()) {
        // TODO: alignment
    }
    if (record->isIbitAtrBdr()) {
        // TODO: borders
    }
    if (record->isIbitAtrPat()) {
        // TODO: background
    }
    if (record->isIbitAtrProt()) {
        // TODO: protection
    }

    d->curConditionalFormat->addConditional(c);
}

void WorksheetSubStreamHandler::handleAutoFilterRecord(Swinder::AutoFilterRecord *record)
{
    Calligra::Sheets::Filter filter;

    int fieldNumber = record->entry();

    if (record->isTopN()) {
        // TODO: top-N filters
    } else {
        Calligra::Sheets::Filter::Composition compos =
            record->join() == AutoFilterRecord::JoinAnd ? Calligra::Sheets::Filter::AndComposition : Calligra::Sheets::Filter::OrComposition;

        for (int i = 0; i < 2; i++) {
            Calligra::Sheets::AbstractCondition::Comparison compar = Calligra::Sheets::AbstractCondition::Match;
            switch (record->operation(i)) {
            case AutoFilterRecord::Less:
                compar = Calligra::Sheets::AbstractCondition::Less;
                break;
            case AutoFilterRecord::Equal:
                compar = Calligra::Sheets::AbstractCondition::Match;
                break;
            case AutoFilterRecord::LEqual:
                compar = Calligra::Sheets::AbstractCondition::LessOrEqual;
                break;
            case AutoFilterRecord::Greater:
                compar = Calligra::Sheets::AbstractCondition::Greater;
                break;
            case AutoFilterRecord::NotEqual:
                compar = Calligra::Sheets::AbstractCondition::NotMatch;
                break;
            case AutoFilterRecord::GEqual:
                compar = Calligra::Sheets::AbstractCondition::GreaterOrEqual;
                break;
            }

            switch (record->valueType(i)) {
            case AutoFilterRecord::RkNumber: {
                bool isInt;
                int iv;
                double dv;
                decodeRK(record->rkValue(i), isInt, iv, dv);
                if (isInt)
                    dv = iv;
                filter.addCondition(compos, fieldNumber, compar, QString::number(dv), Qt::CaseInsensitive, Calligra::Sheets::Filter::Number);
                break;
            }
            case AutoFilterRecord::XNumber:
                filter.addCondition(compos, fieldNumber, compar, QString::number(record->floatValue(i)), Qt::CaseInsensitive, Calligra::Sheets::Filter::Number);
                break;
            case AutoFilterRecord::String:
                filter.addCondition(compos, fieldNumber, compar, record->string(i));
                break;
            case AutoFilterRecord::BoolErr:
                // TODO
                break;
            case AutoFilterRecord::Blanks:
                filter.addCondition(compos, fieldNumber, Calligra::Sheets::AbstractCondition::Match, "");
                break;
            case AutoFilterRecord::NonBlanks:
                filter.addCondition(compos, fieldNumber, Calligra::Sheets::AbstractCondition::NotMatch, "");
                break;
            case AutoFilterRecord::UndefinedType:
            default:
                break;
            }
        }
    }

    Calligra::Sheets::Filter oldFilter = d->sheet->autoFilters();
    oldFilter.addSubFilter(Calligra::Sheets::Filter::AndComposition, filter);
    d->sheet->setAutoFilters(oldFilter);
}

} // namespace Swinder
