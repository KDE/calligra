/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>
   Copyright (C) 2010 Carlos Licea <carlos@kdab.com>

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
   Boston, MA 02110-1301, USA
 */
#include "worksheetsubstreamhandler.h"

#include <map>

#include "globalssubstreamhandler.h"
#include "excel.h"
#include "cell.h"
#include "sheet.h"
#include <QPoint>

//#define SWINDER_XLS2RAW

namespace Swinder
{

class HLinkRecord : public Record
{
public:
    unsigned long m_firstRow;
    unsigned long m_firstColumn;
    unsigned long m_lastRow;
    unsigned long m_lastColumn;
    UString m_displayName;
    UString m_targetFrameName;
    UString m_location;

    static const unsigned id;
    virtual unsigned rtti() const {
        return this->id;
    }
    virtual const char* name() const {
        return "HLink";
    }
    virtual void dump(std::ostream&) const {}
    static Record *createRecord(Workbook *book) {
        return new HLinkRecord(book);
    }
    HLinkRecord(Workbook *book) : Record(book), m_firstRow(0), m_firstColumn(0), m_lastRow(0), m_lastColumn(0) {}
    virtual ~HLinkRecord() {}
    virtual void setData(unsigned size, const unsigned char* data, const unsigned* /* continuePositions */) {
        if (size < 8) {
            setIsValid(false);
            return;
        }

        m_firstRow = readU16(data);
        m_lastRow = readU16(data + 2);
        m_firstColumn = readU16(data + 4);
        m_lastColumn = readU16(data + 6);

        const unsigned char* startHyperlinkObject = data + 16 /* skip CLDID */ + 8;
        const unsigned long streamVersion = readU32(startHyperlinkObject);
        if (streamVersion != 2) {
            std::cerr << "Invalid stream version " << streamVersion << " in HLinkRecord" << std::endl;
            setIsValid(false);
            return;
        }

        // 10 bits options + 22 bits reserved
        const unsigned long opts = readU32(startHyperlinkObject + 4);
        const bool hlstmfHasMonikor = opts & 0x01;
        //const bool hlstmfIsAbsolute = opts & 0x02;
        //const bool hlstmfISiteGaveDisplayName = opts & 0x04;
        bool hlstmfHasLocationStr = opts & 0x08;
        const bool hlstmfHasDisplayName = opts & 0x10;
        //const bool hlstmfHasGUID = opts & 0x20;
        //const bool hlstmfHasCreationTime = opts & 0x60;
        const bool hlstmfHasFrameName = opts & 0xC0;
        const bool hlstmfMonikerSavedAsStr = opts & 0x180;
        //const bool hlstmfAbsFromGetdataRel = opts & 0x300;
        //Q_ASSERT( !hlstmfMonikerSavedAsStr || hlstmfHasMonikor );

        startHyperlinkObject += 8;

        unsigned long length = 0;
        unsigned sizeReaded = 0;

        if (hlstmfHasDisplayName) {
            length = readU32(startHyperlinkObject);
            m_displayName = readUnicodeChars(startHyperlinkObject + 4, length, -1, 0, &sizeReaded);
            //printf("HLinkRecord: displayName=%s\n", m_displayName.ascii());
            startHyperlinkObject += 4 + sizeReaded;
        }

        if (hlstmfHasFrameName) {
            length = readU32(startHyperlinkObject);
            m_targetFrameName = readUnicodeChars(startHyperlinkObject + 4, length, -1, 0, &sizeReaded);
            //printf("HLinkRecord: targetFrameName=%s\n", m_targetFrameName.ascii());
            startHyperlinkObject += 4 + sizeReaded;
        }

        if (hlstmfHasMonikor) {
            if (hlstmfMonikerSavedAsStr) {  // moniker
                //TODO: seems following code leads to a crash on readUnicodeChars...
                //length = readU32(startHyperlinkObject);
                //UString moniker = readUnicodeChars(startHyperlinkObject + 4, length, -1, 0, &sizeReaded);
                //startHyperlinkObject += 4 + sizeReaded;
                std::cerr << "HLinkRecord: Unhandled hlstmfMonikerSavedAsStr moniker" << std::endl;
                setIsValid(false);
                return;
            } else { // oleMoniker
                const unsigned long clsid = readU32(startHyperlinkObject);
                startHyperlinkObject += 16; // the clsid is actually 16 byte long but we only need the first 4 to differ
                switch (clsid) {
                case 0x79EAC9E0: { // URLMoniker
                    length = readU32(startHyperlinkObject);
                    m_location = readTerminatedUnicodeChars(startHyperlinkObject + 4, &sizeReaded);
                    startHyperlinkObject += length + 4;
                }
                break;
                case 0x00000303: { // FileMoniker
                    std::cout << "TODO: HLinkRecord FileMoniker" << std::endl;
                    return; // abort
                }
                break;
                case 0x00000309: { // CompositeMoniker
                    std::cout << "TODO: HLinkRecord CompositeMoniker" << std::endl;
                    return; // abort
                }
                break;
                case 0x00000305: { // AntiMoniker
                    std::cout << "TODO: HLinkRecord AntiMoniker" << std::endl;
                    return; // abort
                }
                break;
                case 0x00000304: { // ItemMoniker
                    std::cout << "TODO: HLinkRecord ItemMoniker" << std::endl;
                    return; // abort
                }
                break;
                }
            }
        }

        if (hlstmfHasLocationStr) {
            length = readU32(startHyperlinkObject);
            startHyperlinkObject += 4;
            if(startHyperlinkObject+length > data+size) {
                std::cerr << "HLinkRecord: expected location but got invalid size=" << length << std::endl;
                setIsValid(false);
                return;
            }
            m_location = readUnicodeChars(startHyperlinkObject, length, -1, 0, &sizeReaded);
            std::cout << "HLinkRecord: m_displayName=" << m_displayName << " m_targetFrameName=" << m_targetFrameName << " location=" << m_location.ascii() << std::endl;
            startHyperlinkObject += sizeReaded;
        }

        // ignore (16 bytes) guid and fileTime (8 bytes)
    }
};

const unsigned HLinkRecord::id = 0x01B8;

class WorksheetSubStreamHandler::Private
{
public:
    Sheet* sheet;
    const GlobalsSubStreamHandler* globals;

    // for FORMULA+SHAREDFMLA record pair
    Cell* lastFormulaCell;

    // for FORMULA+STRING record pair
    Cell* formulaStringCell;

    // mapping from cell position to data tables
    std::map<std::pair<unsigned, unsigned>, DataTableRecord*> dataTables;

    // mapping from cell position to shared formulas
    std::map<std::pair<unsigned, unsigned>, FormulaTokens> sharedFormulas;

    // mapping from object id's to object instances
    std::map<unsigned long, Object*> sharedObjects;

    // maps object id's of NoteObject's to there continuous number
    std::map<unsigned long, int> noteMap;
    // the number of NoteObject's in this worksheet
    int noteCount;

    // list of textobjects as received via TxO records
    std::vector<UString> textObjects;

    // The last drawing object we got.
    DrawingObject* lastDrawingObject;

    // list of id's with ChartObject's.
    std::vector<unsigned long> charts;
};

WorksheetSubStreamHandler::WorksheetSubStreamHandler(Sheet* sheet, const GlobalsSubStreamHandler* globals)
        : SubStreamHandler(), FormulaDecoder(), d(new Private)
{
    d->sheet = sheet;
    d->globals = globals;
    d->lastFormulaCell = 0;
    d->formulaStringCell = 0;
    d->noteCount = 0;
    d->lastDrawingObject = 0;

    RecordRegistry::registerRecordClass(HLinkRecord::id, HLinkRecord::createRecord);
}

WorksheetSubStreamHandler::~WorksheetSubStreamHandler()
{
    for(std::map<std::pair<unsigned, unsigned>, DataTableRecord*>::iterator it = d->dataTables.begin(); it != d->dataTables.end(); ++it)
        delete (*it).second;
    //for(std::map<unsigned long, Object*>::iterator it = d->sharedObjects.begin(); it != d->sharedObjects.end(); ++it)
    //    delete (*it).second;
    //for(std::map<std::pair<unsigned, unsigned>, FormulaTokens*>::iterator it = d->sharedFormulas.begin(); it != d->sharedFormulas.end(); ++it)
    //    delete it.second.second;
    delete d->lastDrawingObject;
    delete d;
}

Sheet* WorksheetSubStreamHandler::sheet() const
{
    return d->sheet;
}

std::map<unsigned long, Object*>& WorksheetSubStreamHandler::sharedObjects() const
{
    return d->sharedObjects;
}

std::vector<unsigned long>& WorksheetSubStreamHandler::charts() const
{
    return d->charts;
}

const std::vector<UString>& WorksheetSubStreamHandler::externSheets() const
{
    return d->globals->externSheets();
}

UString WorksheetSubStreamHandler::nameFromIndex(unsigned index) const
{
    return d->globals->nameFromIndex(index);
}

UString WorksheetSubStreamHandler::externNameFromIndex(unsigned index) const
{
    return d->globals->externNameFromIndex(index);
}

FormulaTokens WorksheetSubStreamHandler::sharedFormulas(const std::pair<unsigned, unsigned>& formulaCellPos) const
{
    std::map<std::pair<unsigned, unsigned>, FormulaTokens>::iterator sharedFormula = d->sharedFormulas.find(formulaCellPos);
    return sharedFormula != d->sharedFormulas.end() ? sharedFormula->second : FormulaTokens();
}

DataTableRecord* WorksheetSubStreamHandler::tableRecord(const std::pair<unsigned, unsigned>& formulaCellPos) const
{
    std::map<std::pair<unsigned, unsigned>, DataTableRecord*>::iterator datatable = d->dataTables.find(formulaCellPos);
    return datatable != d->dataTables.end() ? datatable->second : 0;
}
    
void WorksheetSubStreamHandler::handleRecord(Record* record)
{
    if (!record) return;

    const unsigned type = record->rtti();

    if (type == BottomMarginRecord::id)
        handleBottomMargin(static_cast<BottomMarginRecord*>(record));
    else if (type == BoolErrRecord::id)
        handleBoolErr(static_cast<BoolErrRecord*>(record));
    else if (type == BlankRecord::id)
        handleBlank(static_cast<BlankRecord*>(record));
    else if (type == CalcModeRecord::id)
        handleCalcMode(static_cast<CalcModeRecord*>(record));
    else if (type == ColInfoRecord::id)
        handleColInfo(static_cast<ColInfoRecord*>(record));
    else if (type == DataTableRecord::id)
        handleDataTable(static_cast<DataTableRecord*>(record));
    else if (type == FormulaRecord::id)
        handleFormula(static_cast<FormulaRecord*>(record));
    else if (type == FooterRecord::id)
        handleFooter(static_cast<FooterRecord*>(record));
    else if (type == HeaderRecord::id)
        handleHeader(static_cast<HeaderRecord*>(record));
    else if (type == LabelRecord::id)
        handleLabel(static_cast<LabelRecord*>(record));
    else if (type == LabelSSTRecord::id)
        handleLabelSST(static_cast<LabelSSTRecord*>(record));
    else if (type == LeftMarginRecord::id)
        handleLeftMargin(static_cast<LeftMarginRecord*>(record));
    else if (type == MergedCellsRecord::id)
        handleMergedCells(static_cast<MergedCellsRecord*>(record));
    else if (type == MulBlankRecord::id)
        handleMulBlank(static_cast<MulBlankRecord*>(record));
    else if (type == MulRKRecord::id)
        handleMulRK(static_cast<MulRKRecord*>(record));
    else if (type == NumberRecord::id)
        handleNumber(static_cast<NumberRecord*>(record));
    else if (type == RightMarginRecord::id)
        handleRightMargin(static_cast<RightMarginRecord*>(record));
    else if (type == RKRecord::id)
        handleRK(static_cast<RKRecord*>(record));
    else if (type == RowRecord::id)
        handleRow(static_cast<RowRecord*>(record));
    else if (type == RStringRecord::id)
        handleRString(static_cast<RStringRecord*>(record));
    else if (type == SharedFormulaRecord::id)
        handleSharedFormula(static_cast<SharedFormulaRecord*>(record));
    else if (type == StringRecord::id)
        handleString(static_cast<StringRecord*>(record));
    else if (type == TopMarginRecord::id)
        handleTopMargin(static_cast<TopMarginRecord*>(record));
    else if (type == HLinkRecord::id)
        handleLink(static_cast<HLinkRecord*>(record));
    else if (type == NoteRecord::id)
        handleNote(static_cast<NoteRecord*>(record));
    else if (type == ObjRecord::id)
        handleObj(static_cast<ObjRecord*>(record));
    else if (type == TxORecord::id)
        handleTxO(static_cast<TxORecord*>(record));
    else if (type == BOFRecord::id)
        handleBOF(static_cast<BOFRecord*>(record));
    else if (type == DefaultRowHeightRecord::id)
        handleDefaultRowHeight(static_cast<DefaultRowHeightRecord*>(record));
    else if (type == DefaultColWidthRecord::id)
        handleDefaultColWidth(static_cast<DefaultColWidthRecord*>(record));
    else if (type == SetupRecord::id)
        handleSetup(static_cast<SetupRecord*>(record));
    else if (type == HCenterRecord::id)
        handleHCenter(static_cast<HCenterRecord*>(record));
    else if (type == VCenterRecord::id)
        handleVCenter(static_cast<VCenterRecord*>(record));
    else if (type == ZoomLevelRecord::id)
        handleZoomLevel(static_cast<ZoomLevelRecord*>(record));
    else if (type == 0xA) {} //EofRecord
    else if (type == DimensionRecord::id)
        handleDimension(static_cast<DimensionRecord*>(record));
    else if (type == MsoDrawingRecord::id)
        handleMsoDrawing(static_cast<MsoDrawingRecord*>(record));
    else if (type == Window2Record::id)
        handleWindow2(static_cast<Window2Record*>(record));
    else if (type == PasswordRecord::id)
        handlePassword(static_cast<PasswordRecord*>(record));
    else if (type == BkHimRecord::id)
        handleBkHim(static_cast<BkHimRecord*>(record));
    else {
        //std::cout << "Unhandled worksheet record with type=" << type << " name=" << record->name() << std::endl;
    }
}

void WorksheetSubStreamHandler::handleBOF(BOFRecord* record)
{
    if (!record) return;

    if (record->type() == BOFRecord::Worksheet) {
        // ...
    } else {
        std::cout << "WorksheetSubStreamHandler::handleBOF Unhandled type=" << record->type() << std::endl;
    }
}

void WorksheetSubStreamHandler::handleBlank(BlankRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleBoolErr(BoolErrRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(record->asValue());
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleBottomMargin(BottomMarginRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    // convert from inches to points
    double margin = record->bottomMargin() * 72.0;
    d->sheet->setBottomMargin(margin);
}

void WorksheetSubStreamHandler::handleCalcMode(CalcModeRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    d->sheet->setAutoCalc(record->calcMode() != CalcModeRecord::Manual);
}

void WorksheetSubStreamHandler::handleColInfo(ColInfoRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned firstColumn = record->firstColumn();
    unsigned lastColumn = record->lastColumn();
    unsigned xfIndex = record->xfIndex();
    unsigned width = record->width();
    bool hidden = record->isHidden();

    for (unsigned i = firstColumn; i <= lastColumn; i++) {
        Column* column = d->sheet->column(i, true);
        if (column) {
            column->setWidth( Column::columnUnitsToPts((double)width) );
            column->setFormat(d->globals->convertedFormat(xfIndex));
            column->setVisible(!hidden);
        }
    }
}

void WorksheetSubStreamHandler::handleDataTable(DataTableRecord* record)
{
    if (!record) return;
    if (!d->lastFormulaCell) return;

    unsigned row = d->lastFormulaCell->row();
    unsigned column = d->lastFormulaCell->column();

    d->dataTables[std::make_pair(row, column)] = new DataTableRecord(*record);

    UString formula = dataTableFormula(row, column, record);
    d->lastFormulaCell->setFormula(formula);

    d->lastFormulaCell = 0;
}

void WorksheetSubStreamHandler::handleDimension(DimensionRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    // in the mean time we don't need to handle this because we don't care
    // about the used range of the sheet
    d->sheet->setMaxRow(record->lastRow());
    d->sheet->setMaxColumn(record->lastColumn());
}

void WorksheetSubStreamHandler::handleFormula(FormulaRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    Value value = record->result();
    UString formula = decodeFormula(row, column, record->isShared(), record->tokens());
    Cell* cell = d->sheet->cell(column, row, true);
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

void WorksheetSubStreamHandler::handleFooter(FooterRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    UString footer = record->footer();
    UString left, center, right;
    int pos = -1, len = 0;

    // left part
    pos = footer.find(UString("&L"));
    if (pos >= 0) {
        pos += 2;
        len = footer.find(UString("&C")) - pos;
        if (len > 0) {
            left = footer.substr(pos, len);
            footer = footer.substr(pos + len, footer.length());
        }
    }

    // center part
    pos = footer.find(UString("&C"));
    if (pos >= 0) {
        pos += 2;
        len = footer.find(UString("&R")) - pos;
        if (len > 0) {
            center = footer.substr(pos, len);
            footer = footer.substr(pos + len, footer.length());
        }
    }

    // right part
    pos = footer.find(UString("&R"));
    if (pos >= 0) {
        pos += 2;
        right = footer.substr(pos, footer.length() - pos);
    }

    d->sheet->setLeftFooter(left);
    d->sheet->setCenterFooter(center);
    d->sheet->setRightFooter(right);
}

void WorksheetSubStreamHandler::handleHeader(HeaderRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    UString header = record->header();
    UString left, center, right;
    int pos = -1, len = 0;

    // left part of the header
    pos = header.find(UString("&L"));
    if (pos >= 0) {
        pos += 2;
        len = header.find(UString("&C")) - pos;
        if (len > 0) {
            left = header.substr(pos, len);
            header = header.substr(pos + len, header.length());
        }
    }

    // center part of the header
    pos = header.find(UString("&C"));
    if (pos >= 0) {
        pos += 2;
        len = header.find(UString("&R")) - pos;
        if (len > 0) {
            center = header.substr(pos, len);
            header = header.substr(pos + len, header.length());
        }
    }

    // right part of the header
    pos = header.find(UString("&R"));
    if (pos >= 0) {
        pos += 2;
        right = header.substr(pos, header.length() - pos);
    }

    d->sheet->setLeftHeader(left);
    d->sheet->setCenterHeader(center);
    d->sheet->setRightHeader(right);
}

void WorksheetSubStreamHandler::handleLabel(LabelRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    UString label = record->label();

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(Value(label));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleLabelSST(LabelSSTRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned index = record->sstIndex();
    unsigned xfIndex = record->xfIndex();

    UString str = d->globals->stringFromSST(index);
    std::map<unsigned, FormatFont> formatRuns = d->globals->formatRunsFromSST(index);

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        if (formatRuns.size())
            cell->setValue(Value(str, formatRuns));
        else
            cell->setValue(Value(str));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleLeftMargin(LeftMarginRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    // convert from inches to points
    double margin = record->leftMargin() * 72.0;
    d->sheet->setLeftMargin(margin);
}

void WorksheetSubStreamHandler::handleMergedCells(MergedCellsRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    for (unsigned i = 0; i < record->count(); i++) {
        unsigned firstRow = record->firstRow(i);
        unsigned lastRow = record->lastRow(i);
        unsigned firstColumn = record->firstColumn(i);
        unsigned lastColumn = record->lastColumn(i);

        Cell* cell = d->sheet->cell(firstColumn, firstRow, true);
        if (cell) {
            cell->setColumnSpan(lastColumn - firstColumn + 1);
            cell->setRowSpan(lastRow - firstRow + 1);
        }
        for (unsigned row = firstRow; row <= lastRow; row++)
            for (unsigned col = firstColumn; col <= lastColumn; col++) {
                if (row != firstRow || col != firstColumn) {
                    d->sheet->cell(col, row, true)->setCovered(true);
                }
            }
    }
}

void WorksheetSubStreamHandler::handleMulBlank(MulBlankRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned firstColumn = record->firstColumn();
    unsigned lastColumn = record->lastColumn();
    unsigned row = record->row();

    for (unsigned column = firstColumn; column <= lastColumn; column++) {
        Cell* cell = d->sheet->cell(column, row, true);
        if (cell) {
            cell->setFormat(d->globals->convertedFormat(record->xfIndex(column - firstColumn)));
        }
    }
}

void WorksheetSubStreamHandler::handleMulRK(MulRKRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    const int firstColumn = record->firstColumn();
    const int lastColumn = record->lastColumn();
    const int row = record->row();

    Cell *prevCell = 0;
    int repeat = 1;
    int column = lastColumn;

    while(column >= firstColumn) {
        Cell* cell = d->sheet->cell(column, row, true);
        const int i = column - firstColumn;
        Value value;
        if (record->isInteger(i))
            value.setValue(record->asInteger(i));
        else
            value.setValue(record->asFloat(i));
        cell->setValue(value);
        cell->setFormat(d->globals->convertedFormat(record->xfIndex(column - firstColumn)));
        
        if(prevCell) {
            if(*prevCell == *cell) {
                ++repeat;
            } else {
                if(repeat > 1) {
                    prevCell->setColumnRepeat(repeat);
                    repeat = 1;
                }
            }
        }
        prevCell = cell;
        --column;
        if(column < firstColumn) {
            if(repeat > 1) {
                prevCell->setColumnRepeat(repeat);
            }
            break;
        }
    }
}

void WorksheetSubStreamHandler::handleNumber(NumberRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    double number = record->number();

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(Value(number));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleRightMargin(RightMarginRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    // convert from inches to points
    double margin = record->rightMargin() * 72.0;
    d->sheet->setRightMargin(margin);
}

void WorksheetSubStreamHandler::handleRK(RKRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();

    Value value;
    if (record->isInteger())
        value.setValue(record->asInteger());
    else
        value.setValue(record->asFloat());

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(value);
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleRow(RowRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned index = record->row();
    unsigned xfIndex = record->xfIndex();
    unsigned height = record->height();
    bool hidden = record->isHidden();

    Row* row = d->sheet->row(index, true);
    if (row) {
        row->setHeight(height / 20.0);
        row->setFormat(d->globals->convertedFormat(xfIndex));
        row->setVisible(!hidden);
    }
}

void WorksheetSubStreamHandler::handleRString(RStringRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    UString label = record->label();

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(Value(label));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleSharedFormula(SharedFormulaRecord* record)
{
    if (!record) return;
    if (!d->lastFormulaCell) return;

    unsigned row = d->lastFormulaCell->row();
    unsigned column = d->lastFormulaCell->column();

    d->sharedFormulas[std::make_pair(row, column)] = record->tokens();

    UString formula = decodeFormula(row, column, true, record->tokens());
    d->lastFormulaCell->setFormula(formula);

    d->lastFormulaCell = 0;
}

void WorksheetSubStreamHandler::handleString(StringRecord* record)
{
    if (!record) return;
    if (!d->formulaStringCell) return;

    d->formulaStringCell->setValue(record->value());
    d->formulaStringCell = 0;
}

void WorksheetSubStreamHandler::handleTopMargin(TopMarginRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;


    // convert from inches to points
    double margin = record->topMargin() * 72.0;
    d->sheet->setTopMargin(margin);
}

void WorksheetSubStreamHandler::handleLink(HLinkRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    //FIXME we ignore the m_lastRow and m_lastColumn values, does ODF have something similar?
    Cell *cell = d->sheet->cell(record->m_firstColumn, record->m_firstRow);
    if (cell) {
        cell->setHyperlink(record->m_displayName, record->m_location, record->m_targetFrameName);
    }
}

void WorksheetSubStreamHandler::handleTxO(TxORecord* record)
{
    if (!record) return;

    std::cout << "WorksheetSubStreamHandler::handleTxO size=" << d->textObjects.size()+1 << " text=" << record->m_text << std::endl;
    d->textObjects.push_back(record->m_text);
}

void WorksheetSubStreamHandler::handleNote(NoteRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;
    Cell *cell = d->sheet->cell(record->column(), record->row());
    if (cell) {
        const unsigned long id = record->idObj();
        NoteObject *obj = static_cast<NoteObject*>(d->sharedObjects[id]);
        if (obj) {
            int offset = d->noteMap[id] - 1;
            Q_ASSERT(offset>=0 && uint(offset)<d->textObjects.size());
            cell->setNote(d->textObjects[offset]);
            //cell->setNote(obj->note());
        }
    }
}

void WorksheetSubStreamHandler::handleObj(ObjRecord* record)
{
    if (!record) return;
    if (!record->m_object) return;

    const unsigned long id = record->m_object->id();

    std::cout << "WorksheetSubStreamHandler::handleObj id=" << id << " type=" << record->m_object->type() << std::endl;
    switch(record->m_object->type()) {
        //case Object::Picture:
        //    PictureObject *r = static_cast<PictureObject*>(record->m_object);
        //    std::cout << "PICTURE embeddedStorage=" << r->embeddedStorage().c_str() << std::endl;
        //    break;
        case Object::Chart:
            d->charts.push_back(id);
            break;
        case Object::Note:
            d->noteMap[id] = ++d->noteCount;
            break;
        default:
            break;
    }

    // look if there is a DrawingObject defined that is waiting for us to be picked up.
    if(d->lastDrawingObject) {
        record->m_object->setDrawingObject(d->lastDrawingObject); // will take over ownership
        d->lastDrawingObject = 0;
    }

    d->sharedObjects[id] = record->m_object;
    record->m_object = 0; // take over ownership
}

void WorksheetSubStreamHandler::handleDefaultRowHeight(DefaultRowHeightRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;
    if( record->miyRw() != 0.0 )
        d->sheet->setDefaultRowHeight(record->miyRw());
}

void WorksheetSubStreamHandler::handleDefaultColWidth(DefaultColWidthRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;

    d->sheet->setDefaultColWidth( Column::columnUnitsToPts(record->cchdefColWidth() * 256.0) );
}

void WorksheetSubStreamHandler::handleSetup(SetupRecord*)
{
    //TODO
}

void WorksheetSubStreamHandler::handleHCenter(HCenterRecord*)
{
    //TODO
}

void WorksheetSubStreamHandler::handleVCenter(VCenterRecord*)
{
    //TODO
}

void WorksheetSubStreamHandler::handleZoomLevel(ZoomLevelRecord *record)
{
    if (!record) return;
    if (!d->sheet) return;
    if (record->denominator() == 0) return;
    d->sheet->setZoomLevel( record->numerator() / double(record->denominator()) );
}

void WorksheetSubStreamHandler::handleMsoDrawing(MsoDrawingRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;
    
    // picture?
    std::map<unsigned long,unsigned long>::iterator pit = record->m_properties.find(DrawingObject::pid);
    if(pit != record->m_properties.end()) {
        const unsigned long id = (*pit).second;
        std::cout << "WorksheetSubStreamHandler::handleMsoDrawing pid=" << id << std::endl;
        MsoDrawingBlibItem *drawing = d->globals->drawing(id);
        if(!drawing) return;
        Cell *cell = d->sheet->cell(record->m_colL, record->m_rwT);
        Q_ASSERT(cell);
        cell->addPicture(new Picture(record,drawing));
        return;
    }

    // text?
    std::map<unsigned long,unsigned long>::iterator txit = record->m_properties.find(DrawingObject::itxid);
    if(txit != record->m_properties.end()) {
        const unsigned long id = (*txit).second;
        std::cout << "TODO WorksheetSubStreamHandler::handleMsoDrawing itxid=" << id << std::endl;

        //TODO
        //Q_ASSERT(d->globals->drawing(id));
        //Q_ASSERT(false);
        
        return;
    }

    //FIXME probably move that up and use it for noites and pictures too? Needs more investigation...
    if(record->m_gotClientData) {
        // If the DrawingObject got a OfficeArtClientData then a ObjRecord will follow that picks
        // this DrawingObject up and uses it for future actions.
        delete d->lastDrawingObject; // remove old DrawingObject if it was not picked up
        d->lastDrawingObject = new DrawingObject(*record);
    }

    std::cerr << "WorksheetSubStreamHandler::handleMsoDrawing No pid" << std::endl;
}

void WorksheetSubStreamHandler::handleWindow2(Window2Record* record)
{
    if (!record) return;
    if (!d->sheet) return;
    d->sheet->setShowGrid(record->isFDspGridRt());
    d->sheet->setShowZeroValues(record->isFDspZerosRt());
    d->sheet->setFirstVisibleCell(QPoint(record->colLeft(),record->rwTop()));
    d->sheet->setPageBreakViewEnabled(record->isFSLV());
}

void WorksheetSubStreamHandler::handlePassword(PasswordRecord* record)
{
    if (!record) return;
    if (!d->sheet) return;
    if (!record->wPassword()) return;
    std::cout << "WorksheetSubStreamHandler::handlePassword passwordHash=" << record->wPassword() << std::endl;
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
        
    std::cout << ">>>> " << sPasswd.data() << std::endl; //0x218490a
#endif

}

void WorksheetSubStreamHandler::handleBkHim(BkHimRecord* record)
{
    d->sheet->setBackgroundImage(record->imagePath());
}

} // namespace Swinder
