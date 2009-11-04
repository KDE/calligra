/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

namespace Swinder {

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
    std::map<std::pair<unsigned, unsigned>, DataTableRecord> dataTables;

    // mapping from cell position to shared formulas
    std::map<std::pair<unsigned, unsigned>, FormulaTokens> sharedFormulas;
};

WorksheetSubStreamHandler::WorksheetSubStreamHandler( Sheet* sheet, const GlobalsSubStreamHandler* globals )
    : d(new Private)
{
    d->sheet = sheet;
    d->globals = globals;
    d->lastFormulaCell = 0;
    d->formulaStringCell = 0;
}

WorksheetSubStreamHandler::~WorksheetSubStreamHandler()
{
    delete d;
}

void WorksheetSubStreamHandler::handleRecord( Record* record )
{
    if (!record) return;

    unsigned type = record->rtti();
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
}

void WorksheetSubStreamHandler::handleBlank( BlankRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();

    Cell* cell = d->sheet->cell(column, row, true);
    if(cell) {
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleBoolErr( BoolErrRecord* record )
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

void WorksheetSubStreamHandler::handleBottomMargin( BottomMarginRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    // convert from inches to points
    double margin = record->bottomMargin() * 72.0;
    d->sheet->setBottomMargin( margin );
}

void WorksheetSubStreamHandler::handleCalcMode( CalcModeRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    d->sheet->setAutoCalc(record->calcMode() != CalcModeRecord::Manual);
}

void WorksheetSubStreamHandler::handleColInfo( ColInfoRecord* record )
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
            column->setWidth( width / 120 );
            column->setFormat(d->globals->convertedFormat(xfIndex));
            column->setVisible(!hidden);
        }
    }
}

void WorksheetSubStreamHandler::handleDataTable( DataTableRecord* record )
{
    if (!record) return;
    if (!d->lastFormulaCell) return;

    unsigned row = d->lastFormulaCell->row();
    unsigned column = d->lastFormulaCell->column();

    d->dataTables[std::make_pair(row, column)] = *record;

    UString formula = dataTableFormula(row, column, record);
    d->lastFormulaCell->setFormula(formula);

    d->lastFormulaCell = 0;
}

void WorksheetSubStreamHandler::handleDimension( DimensionRecord* record )
{
    if (!record) return;

    // in the mean time we don't need to handle this because we don't care
    // about the used range of the sheet
}

void WorksheetSubStreamHandler::handleFormula( FormulaRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    Value value = record->result();

    UString formula = decodeFormula(row, column, record->tokens());

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

void WorksheetSubStreamHandler::handleFooter( FooterRecord* record )
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
            footer = footer.substr(pos+len, footer.length());
        }
    }

    // center part
    pos = footer.find(UString("&C"));
    if( pos >= 0 ) {
        pos += 2;
        len = footer.find(UString("&R")) - pos;
        if(len > 0) {
            center = footer.substr(pos, len);
            footer = footer.substr(pos+len, footer.length());
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

void WorksheetSubStreamHandler::handleHeader( HeaderRecord* record )
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
            header = header.substr(pos+len, header.length());
        }
    }

    // center part of the header
    pos = header.find(UString("&C"));
    if (pos >= 0) {
        pos += 2;
        len = header.find(UString("&R")) - pos;
        if (len > 0) {
            center = header.substr(pos, len);
            header = header.substr(pos+len, header.length());
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

void WorksheetSubStreamHandler::handleLabel( LabelRecord* record )
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

void WorksheetSubStreamHandler::handleLabelSST( LabelSSTRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned index = record->sstIndex();
    unsigned xfIndex = record->xfIndex();

    UString str = d->globals->stringFromSST(index);

    Cell* cell = d->sheet->cell(column, row, true);
    if (cell) {
        cell->setValue(Value(str));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleLeftMargin( LeftMarginRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    // convert from inches to points
    double margin = record->leftMargin() * 72.0;
    d->sheet->setLeftMargin( margin );
}

void WorksheetSubStreamHandler::handleMergedCells( MergedCellsRecord* record )
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

void WorksheetSubStreamHandler::handleMulBlank( MulBlankRecord* record )
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

void WorksheetSubStreamHandler::handleMulRK( MulRKRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned firstColumn = record->firstColumn();
    unsigned lastColumn = record->lastColumn();
    unsigned row = record->row();

    for (unsigned column = firstColumn; column <= lastColumn; column++) {
        Cell* cell = d->sheet->cell(column, row, true);
        if (cell) {
            unsigned i = column - firstColumn;
            Value value;
            if (record->isInteger(i))
                value.setValue(record->asInteger(i));
            else
                value.setValue(record->asFloat(i));
            cell->setValue(value);
            cell->setFormat(d->globals->convertedFormat(record->xfIndex(column-firstColumn)));
        }
    }
}

void WorksheetSubStreamHandler::handleNumber( NumberRecord* record )
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

void WorksheetSubStreamHandler::handleRightMargin( RightMarginRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    // convert from inches to points
    double margin = record->rightMargin() * 72.0;
    d->sheet->setRightMargin(margin);
}

void WorksheetSubStreamHandler::handleRK( RKRecord* record )
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

void WorksheetSubStreamHandler::handleRow( RowRecord* record )
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

void WorksheetSubStreamHandler::handleRString( RStringRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;

    unsigned column = record->column();
    unsigned row = record->row();
    unsigned xfIndex = record->xfIndex();
    UString label = record->label();

    Cell* cell = d->sheet->cell(column, row, true);
    if(cell) {
        cell->setValue(Value(label));
        cell->setFormat(d->globals->convertedFormat(xfIndex));
    }
}

void WorksheetSubStreamHandler::handleSharedFormula( SharedFormulaRecord* record )
{
    if(!record) return;
    if(!d->lastFormulaCell) return;

    unsigned row = d->lastFormulaCell->row();
    unsigned column = d->lastFormulaCell->column();

    d->sharedFormulas[std::make_pair(row, column)] = record->tokens();

    UString formula = decodeFormula(row, column, record->tokens());
    d->lastFormulaCell->setFormula(formula);

    d->lastFormulaCell = 0;
}

void WorksheetSubStreamHandler::handleString( StringRecord* record )
{
    if (!record) return;
    if (!d->formulaStringCell) return;

    d->formulaStringCell->setValue(record->value());
    d->formulaStringCell = 0;
}

void WorksheetSubStreamHandler::handleTopMargin( TopMarginRecord* record )
{
    if (!record) return;
    if (!d->sheet) return;


    // convert from inches to points
    double margin = record->topMargin() * 72.0;
    d->sheet->setTopMargin(margin);
}

typedef std::vector<UString> UStringStack;

static void mergeTokens( UStringStack* stack, unsigned count, UString mergeString )
{
    if (!stack) return;
    if (stack->size() < count) return;

    UString s1, s2;

    while(count) {
        count--;

        UString last = (*stack)[stack->size()-1];
        UString tmp = last;
        tmp.append(s1);
        s1 = tmp;

        if (count) {
            tmp = mergeString;
            tmp.append( s1 );
            s1 = tmp;
        }

        stack->resize(stack->size()-1);
    }

    stack->push_back(s1);
}

#ifdef SWINDER_XLS2RAW
static void dumpStack( std::vector<UString> stack )
{
    std::cout << std::endl;
    std::cout << "Stack now is: " ;
    if (!stack.size())
        std::cout << "(empty)" ;

    for (unsigned i = 0; i < stack.size(); i++)
        std::cout << "  " << i << ": " << stack[i].ascii() << std::endl;
    std::cout << std::endl;
}
#endif

UString WorksheetSubStreamHandler::decodeFormula( unsigned row, unsigned col, const FormulaTokens& tokens )
{
    UStringStack stack;

    for (unsigned c=0; c < tokens.size(); c++) {
        FormulaToken token = tokens[c];

#ifdef SWINDER_XLS2RAW
        std::cout << "Token " << c << ": ";
        std::cout <<  token.id() << "  ";
        std::cout << token.idAsString() << std::endl;
#endif

        switch (token.id()) {
            case FormulaToken::Add:
                mergeTokens(&stack, 2, UString("+"));
                break;

            case FormulaToken::Sub:
                mergeTokens(&stack, 2, UString("-"));
                break;

            case FormulaToken::Mul:
                mergeTokens(&stack, 2, UString("*"));
                break;

            case FormulaToken::Div:
                mergeTokens(&stack, 2, UString("/"));
                break;

            case FormulaToken::Power:
                mergeTokens(&stack, 2, UString("^"));
                break;

            case FormulaToken::Concat:
                mergeTokens(&stack, 2, UString("&"));
                break;

            case FormulaToken::LT:
                mergeTokens(&stack, 2, UString("<"));
                break;

            case FormulaToken::LE:
                mergeTokens(&stack, 2, UString("<="));
                break;

            case FormulaToken::EQ:
                mergeTokens(&stack, 2, UString("="));
                break;

            case FormulaToken::GE:
                mergeTokens(&stack, 2, UString(">="));
                break;

            case FormulaToken::GT:
                mergeTokens(&stack, 2, UString(">"));
                break;

            case FormulaToken::NE:
                mergeTokens(&stack, 2, UString("<>"));
                break;

            case FormulaToken::Intersect:
                mergeTokens(&stack, 2, UString(" "));
                break;

            case FormulaToken::List:
                mergeTokens(&stack, 2, UString(";"));
                break;

            case FormulaToken::Range:
                mergeTokens(&stack, 2, UString(";"));
                break;

            case FormulaToken::UPlus: {
                UString str("+");
                str.append(stack[stack.size()-1]);
                stack[stack.size()-1] = str;
                break;
            }

            case FormulaToken::UMinus: {
                UString str("-");
                str.append(stack[ stack.size()-1 ]);
                stack[stack.size()-1] = str;
                break;
            }

            case FormulaToken::Percent:
                stack[stack.size()-1].append(UString("%"));
                break;

            case FormulaToken::Paren: {
                UString str("(");
                str.append(stack[ stack.size()-1 ]);
                str.append(UString(")"));
                stack[stack.size()-1] = str;
                break;
            }

            case FormulaToken::MissArg:
                // just ignore
                stack.push_back(UString(" "));
                break;

            case FormulaToken::String: {
                UString str('\"');
                str.append(token.value().asString());
                str.append(UString('\"'));
                stack.push_back(str);
                break;
            }

            case FormulaToken::Bool:
                if (token.value().asBoolean())
                    stack.push_back(UString("TRUE"));
                else
                    stack.push_back(UString("FALSE"));
                break;

            case FormulaToken::Integer:
                stack.push_back(UString::from(token.value().asInteger()));
                break;

            case FormulaToken::Float:
                stack.push_back(UString::from(token.value().asFloat()));
                break;

            case FormulaToken::Array:
                // FIXME handle this !
                break;

            case FormulaToken::Ref:
                stack.push_back(token.ref(row, col));
                break;

            case FormulaToken::RefN:
                stack.push_back(token.refn(row, col));
                break;

            case FormulaToken::Ref3d:
                stack.push_back(token.ref3d(d->globals->externSheets(), row, col));
                break;

            case FormulaToken::Area:
                stack.push_back(token.area(row, col));
                break;

            case FormulaToken::AreaN:
                stack.push_back(token.area(row, col, true));
                break;

            case FormulaToken::Area3d:
                stack.push_back(token.area3d(d->globals->externSheets(), row, col));
                break;

            case FormulaToken::Function: {
                mergeTokens(&stack, token.functionParams(), UString(";"));
                if (stack.size()) {
                    UString str(token.functionName() ? token.functionName() : "??");
                    str.append(UString("("));
                    str.append(stack[stack.size()-1]);
                    str.append(UString(")"));
                    stack[stack.size()-1] = str;
                }
                break;
            }

            case FormulaToken::FunctionVar:
                if (token.functionIndex() != 255) {
                    mergeTokens(&stack, token.functionParams(), UString(";"));
                    if (stack.size()) {
                        UString str;
                        if (token.functionIndex() != 255)
                            str = token.functionName() ? token.functionName() : "??";
                        str.append(UString("("));
                        str.append(stack[stack.size()-1]);
                        str.append(UString(")"));
                        stack[stack.size()-1] = str;
                    }
                } else {
                    unsigned count = token.functionParams()-1;
                    mergeTokens(&stack, count, UString(";"));
                    if(stack.size()) {
                        UString str;
                        str.append(UString("("));
                        str.append(stack[ stack.size()-1 ]);
                        str.append(UString(")"));
                        stack[stack.size()-1] = str;
                    }
                }
                break;

            case FormulaToken::Attr:
                if (token.attr() & 0x10) { // SUM
                    mergeTokens(&stack, 1, UString(";"));
                    if (stack.size()) {
                        UString str("SUM");
                        str.append(UString("("));
                        str.append(stack[ stack.size()-1 ]);
                        str.append(UString(")"));
                        stack[stack.size()-1] = str;
                    }
                }
                break;

            case FormulaToken::NameX:
                // FIXME this handling of names is completely broken
                stack.push_back(d->globals->nameFromIndex(token.nameIndex()-1));
                break;

            case FormulaToken::Matrix: {
                std::pair<unsigned, unsigned> formulaCellPos = token.baseFormulaRecord();

                std::map<std::pair<unsigned, unsigned>, FormulaTokens>::iterator sharedFormula = d->sharedFormulas.find(formulaCellPos);
                if (sharedFormula != d->sharedFormulas.end()) {
                    stack.push_back(decodeFormula(row, col, sharedFormula->second));
                } else {
                    stack.push_back(UString("Error"));
                }
                break;
            }

            case FormulaToken::Table: {
                std::pair<unsigned, unsigned> formulaCellPos = token.baseFormulaRecord();
                std::map<std::pair<unsigned, unsigned>, DataTableRecord>::iterator datatable = d->dataTables.find(formulaCellPos);
                if (datatable != d->dataTables.end()) {
                    stack.push_back(dataTableFormula(row, col, &datatable->second));
                } else {
                    stack.push_back(UString("Error"));
                }
                break;
            }

            case FormulaToken::NatFormula:
            case FormulaToken::Sheet:
            case FormulaToken::EndSheet:
            case FormulaToken::ErrorCode:
            case FormulaToken::Name:
            case FormulaToken::MemArea:
            case FormulaToken::MemErr:
            case FormulaToken::MemNoMem:
            case FormulaToken::MemFunc:
            case FormulaToken::RefErr:
            case FormulaToken::AreaErr:
            case FormulaToken::MemAreaN:
            case FormulaToken::MemNoMemN:
            case FormulaToken::RefErr3d:
            case FormulaToken::AreaErr3d:
            default:
                // FIXME handle this !
                stack.push_back(UString("Unknown"));
                break;
        };

#ifdef SWINDER_XLS2RAW
        dumpStack(stack);
#endif
    }

    UString result;
    for (unsigned i = 0; i < stack.size(); i++)
        result.append(stack[i]);

#ifdef SWINDER_XLS2RAW
    std::cout << "FORMULA Result: " << result << std::endl;
#endif
    return result;
}

UString WorksheetSubStreamHandler::dataTableFormula( unsigned row, unsigned col, const DataTableRecord* record )
{
    UString result("MULTIPLE.OPERATIONS(");

    unsigned formulaRow = 0, formulaCol = 0;
    switch (record->direction()) {
        case DataTableRecord::InputRow:
            formulaRow = row;
            formulaCol = record->firstColumn() - 1;
            break;
        case DataTableRecord::InputColumn:
            formulaRow = record->firstRow() - 1;
            formulaCol = col;
            break;
        case DataTableRecord::Input2D:
            formulaRow = record->firstRow() - 1;
            formulaCol = record->firstColumn() - 1;
            break;
    }

    result.append( UString("[.$") );
    result.append( Cell::columnLabel( formulaCol ) );
    result.append( UString("$") );
    result.append( UString::from( formulaRow+1 ) );
    result.append( UString("]") );

    if (record->direction() == DataTableRecord::Input2D) {
        result.append( UString(";[.$") );
        result.append( Cell::columnLabel( record->inputColumn2() ) );
        result.append( UString("$") );
        result.append( UString::from( record->inputRow2()+1 ) );
        result.append( UString("]") );
    } else {
        result.append( UString(";[.$") );
        result.append( Cell::columnLabel( record->inputColumn1() ) );
        result.append( UString("$") );
        result.append( UString::from( record->inputRow1()+1 ) );
        result.append( UString("]") );
    }

    if (record->direction() == DataTableRecord::Input2D || record->direction() == DataTableRecord::InputColumn) {
        result.append( UString(";[.$") );
        result.append( Cell::columnLabel( record->firstColumn()-1 ) );
        result.append( UString::from( row+1 ) );
        result.append( UString("]") );
    }

    if (record->direction() == DataTableRecord::Input2D) {
        result.append( UString(";[.$") );
        result.append( Cell::columnLabel( record->inputColumn1() ) );
        result.append( UString("$") );
        result.append( UString::from( record->inputRow1()+1 ) );
        result.append( UString("]") );
    }

    if (record->direction() == DataTableRecord::Input2D || record->direction() == DataTableRecord::InputRow) {
        result.append( UString(";[.") );
        result.append( Cell::columnLabel( col ) );
        result.append( UString("$") );
        result.append( UString::from( record->firstRow()-1+1 ) );
        result.append( UString("]") );
    }

    result.append( UString(")") );

#ifdef SWINDER_XLS2RAW
    std::cout << "DATATABLE Result: " << result << std::endl;
#endif
    return result;
}

} // namespace Swinder
