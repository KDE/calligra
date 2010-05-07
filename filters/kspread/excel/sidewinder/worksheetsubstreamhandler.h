/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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
#ifndef SWINDER_WORKSHEETSUBSTREAMHANDLER_H
#define SWINDER_WORKSHEETSUBSTREAMHANDLER_H

#include "substreamhandler.h"
#include "formulas.h"
#include "ustring.h"
#include <vector>

namespace Swinder
{

class Object;
class Sheet;
class GlobalsSubStreamHandler;

class BOFRecord;
class BlankRecord;
class BoolErrRecord;
class BottomMarginRecord;
class CalcModeRecord;
class ColInfoRecord;
class DataTableRecord;
class DimensionRecord;
class FormulaRecord;
class FooterRecord;
class HeaderRecord;
class LabelRecord;
class LabelSSTRecord;
class LeftMarginRecord;
class MergedCellsRecord;
class MulBlankRecord;
class MulRKRecord;
class NumberRecord;
class RightMarginRecord;
class RKRecord;
class RowRecord;
class RStringRecord;
class SharedFormulaRecord;
class StringRecord;
class TopMarginRecord;
class HLinkRecord;
class NoteRecord;
class ObjRecord;
class TxORecord;
class DefaultRowHeightRecord;
class DefaultColWidthRecord;
class SetupRecord;
class HCenterRecord;
class VCenterRecord;
class ZoomLevelRecord;
class MsoDrawingRecord;
class Window2Record;
class PasswordRecord;
class BkHimRecord;

class WorksheetSubStreamHandler : public SubStreamHandler, public FormulaDecoder
{
public:
    WorksheetSubStreamHandler(Sheet* sheet, const GlobalsSubStreamHandler* globals);
    virtual ~WorksheetSubStreamHandler();
    Sheet* sheet() const;

    std::map<unsigned long, Object*>& sharedObjects() const;
    std::vector<unsigned long>& charts() const;

    virtual void handleRecord(Record* record);
    
public:
    // from FormulaDecoder
    virtual const std::vector<UString>& externSheets() const;
    virtual UString nameFromIndex(unsigned index) const;
    virtual UString externNameFromIndex(unsigned index) const;
    virtual FormulaTokens sharedFormulas(const std::pair<unsigned, unsigned>& formulaCellPos) const;
    virtual DataTableRecord* tableRecord(const std::pair<unsigned, unsigned>& formulaCellPos) const;

private:
    void handleBOF(BOFRecord* record);
    void handleBlank(BlankRecord* record);
    void handleBoolErr(BoolErrRecord* record);
    void handleBottomMargin(BottomMarginRecord* record);
    void handleCalcMode(CalcModeRecord* record);
    void handleColInfo(ColInfoRecord* record);
    void handleDataTable(DataTableRecord* record);
    void handleDimension(DimensionRecord* record);
    void handleFormula(FormulaRecord* record);
    void handleFooter(FooterRecord* record);
    void handleHeader(HeaderRecord* record);
    void handleLabel(LabelRecord* record);
    void handleLabelSST(LabelSSTRecord* record);
    void handleLeftMargin(LeftMarginRecord* record);
    void handleMergedCells(MergedCellsRecord* record);
    void handleMulBlank(MulBlankRecord* record);
    void handleMulRK(MulRKRecord* record);
    void handleNumber(NumberRecord* record);
    void handleRightMargin(RightMarginRecord* record);
    void handleRK(RKRecord* record);
    void handleRow(RowRecord* record);
    void handleRString(RStringRecord* record);
    void handleSharedFormula(SharedFormulaRecord* record);
    void handleString(StringRecord* record);
    void handleTopMargin(TopMarginRecord* record);
    void handleLink(HLinkRecord* record);
    void handleNote(NoteRecord* record);
    void handleObj(ObjRecord* record);
    void handleTxO(TxORecord* record);
    void handleDefaultRowHeight(DefaultRowHeightRecord* record);
    void handleDefaultColWidth(DefaultColWidthRecord* record);
    void handleSetup(SetupRecord* record);
    void handleHCenter(HCenterRecord *record);
    void handleVCenter(VCenterRecord *record);
    void handleZoomLevel(ZoomLevelRecord *record);
    void handleMsoDrawing(MsoDrawingRecord* record);
    void handleWindow2(Window2Record* record);
    void handlePassword(PasswordRecord* record);
    void handleBkHim(BkHimRecord* record);

    class Private;
    Private * const d;
};

} // namespace Swinder

#endif // SWINDER_WORKSHEETSUBSTREAMHANDLER_H

