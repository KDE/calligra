/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2003-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2006, 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2009, 2010 Sebastian Sauer <sebsauer@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SWINDER_GLOBALSSUBSTREAMHANDLER_H
#define SWINDER_GLOBALSSUBSTREAMHANDLER_H

#include <map>
#include <vector>

#include "format.h"
#include "formulas.h"
#include "substreamhandler.h"
#include "workbook.h"

namespace Swinder
{

class Workbook;
class Sheet;

class BOFRecord;
class BoundSheetRecord;
class DateModeRecord;
class ExternBookRecord;
class ExternNameRecord;
class ExternSheetRecord;
class FilepassRecord;
class FontRecord;
class FormatRecord;
class NameRecord;
class PaletteRecord;
class SSTRecord;
class XFRecord;
class ProtectRecord;
class MsoDrawingGroupRecord;
class Window1Record;
class PasswordRecord;

class GlobalsSubStreamHandler : public SubStreamHandler, public FormulaDecoder
{
public:
    GlobalsSubStreamHandler(Workbook *workbook, unsigned version);
    ~GlobalsSubStreamHandler() override;

    void handleRecord(Record *record) override;

    Workbook *workbook() const;

    bool passwordProtected() const;
    bool encryptionTypeSupported() const;
    void decryptionSkipBytes(int count);
    void decryptRecord(unsigned type, unsigned size, unsigned char *buffer);

    unsigned version() const;
    Sheet *sheetFromPosition(unsigned position) const;
    QString stringFromSST(unsigned index) const;
    std::map<unsigned, FormatFont> formatRunsFromSST(unsigned index) const;

    unsigned fontCount() const; //
    FontRecord fontRecord(unsigned index) const; //

    unsigned xformatCount() const; //
    XFRecord xformat(unsigned index) const; //

    const Format *convertedFormat(unsigned index) const;

    QString valueFormat(unsigned index) const; //

    const std::vector<QString> &externSheets() const override;

    QString nameFromIndex(unsigned index) const override;
    QString externNameFromIndex(unsigned index) const override;

    QList<Sheet *> &chartSheets();

    KoStore *store() const;

private:
    void handleBOF(BOFRecord *record);
    void handleBoundSheet(BoundSheetRecord *record);
    void handleDateMode(DateModeRecord *record);
    void handleExternBook(ExternBookRecord *record);
    void handleExternName(ExternNameRecord *record);
    void handleExternSheet(ExternSheetRecord *record);
    void handleFilepass(FilepassRecord *record);
    void handleFont(FontRecord *record);
    void handleFormat(FormatRecord *record);
    void handleName(NameRecord *record);
    void handlePalette(PaletteRecord *record);
    void handleSST(SSTRecord *record);
    void handleXF(XFRecord *record);
    void handleProtect(ProtectRecord *record);
    void handleMsoDrawingGroup(MsoDrawingGroupRecord *record);
    void handleWindow1(Window1Record *record);
    void handlePassword(PasswordRecord *record);

    class Private;
    Private *const d;
};

} // namespace Swinder

#endif // SWINDER_GLOBALSSUBSTREAMHANDLER_H
