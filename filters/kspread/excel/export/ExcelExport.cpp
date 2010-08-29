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
#include <Formula.h>
#include <Map.h>
#include <Sheet.h>
#include <Region.h>
#include <RowColumnFormat.h>
#include <StyleStorage.h>
#include <ValueStorage.h>
#include <kspread_limits.h>

#include <swinder.h>
#include <XlsRecordOutputStream.h>

#include <CFBWriter.h>

typedef KGenericFactory<ExcelExport> ExcelExportFactory;
K_EXPORT_COMPONENT_FACTORY(libexcelexport, ExcelExportFactory("kofficefilters"))

static uint qHash(const QFont& f)
{
    return qHash(f.family()) ^ 37 * f.pointSize();
}

static uint qHash(const QColor& c)
{
    return uint(c.rgba());
}

using namespace Swinder;

class ExcelExport::Private
{
public:
    const KSpread::Doc* inputDoc;
    QString outputFile;
    XlsRecordOutputStream* out;
    QHash<KSpread::Style, unsigned> styles;
    QList<FontRecord> fontRecords;

    void convertStyle(const KSpread::Style& style, XFRecord& xf, QHash<QPair<QFont, QColor>, unsigned>& fontMap);
    unsigned fontIndex(const QFont& font, const QColor& color, QHash<QPair<QFont, QColor>, unsigned>& fontMap);
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

    QHash<QPair<QFont, QColor>, unsigned> fonts;
    fonts[qMakePair(QFont(), QColor())] = 0;
    {
        FontRecord fnt(0);
        fnt.setFontName("Arial");
        d->fontRecords.append(fnt);
    }
    QList<XFRecord> xfs;
    for (int i = 0; i < d->inputDoc->map()->count(); i++) {
        collectStyles(d->inputDoc->map()->sheet(i), xfs, fonts);
    }

    foreach (const FontRecord& fnt, d->fontRecords) {
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

    o.writeRecord(XFRecord(0)); // default first record
    foreach (const XFRecord& xf, xfs) {
        o.writeRecord(xf);
    }
    // XLS requires 16 XF records for some reason
    for (int i = xfs.size()+1; i < 16; i++) {
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

    QHash<QString, unsigned> stringTable;
    {
        SSTRecord sst(0);
        ExtSSTRecord esst(0);
        sst.setExtSSTRecord(&esst);
        for (int i = 0; i < d->inputDoc->map()->count(); i++) {
            buildStringTable(d->inputDoc->map()->sheet(i), sst, stringTable);
        }
        o.writeRecord(sst);
        o.writeRecord(esst);
    }

    o.writeRecord(EOFRecord(0));

    for (int i = 0; i < d->inputDoc->map()->count(); i++) {
        boundSheets[i].setBofPosition(o.pos());
        o.rewriteRecord(boundSheets[i]);
        convertSheet(d->inputDoc->map()->sheet(i), stringTable);
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

void ExcelExport::collectStyles(KSpread::Sheet* sheet, QList<XFRecord>& xfRecords, QHash<QPair<QFont, QColor>, unsigned>& fontMap)
{
    QRect area = sheet->cellStorage()->styleStorage()->usedArea();
    for (int row = area.top(); row <= area.bottom(); row++) {
        for (int col = area.left(); col <= area.right(); col++){
            KSpread::Style s = sheet->cellStorage()->style(col, row);
            unsigned& idx = d->styles[s];
            if (!idx) {
                XFRecord xfr(0);
                d->convertStyle(s, xfr, fontMap);
                xfRecords.append(xfr);
                idx = xfRecords.size();
            }
        }
    }
}

void ExcelExport::buildStringTable(KSpread::Sheet* sheet, Swinder::SSTRecord& sst, QHash<QString, unsigned>& stringTable)
{
    unsigned useCount = 0;
    const KSpread::ValueStorage* values = sheet->cellStorage()->valueStorage();
    for (int i = 0; i < values->count(); i++) {
        KSpread::Value v = values->data(i);
        if (v.isString()) {
            QString s = v.asString();
            if (!stringTable.contains(s)) {
                stringTable.insert(s, sst.addString(s));
            }
            useCount++;
        }
    }
    sst.setUseCount(sst.useCount() + useCount);
}

void ExcelExport::convertSheet(KSpread::Sheet* sheet, const QHash<QString, unsigned>& sst)
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
                KSpread::Value val = cell.value();
                KSpread::Style style = cell.style();
                unsigned xfi = d->styles[style];

                if (cell.isFormula()) {
                    FormulaRecord fr(0);
                    fr.setRow(row-1);
                    fr.setColumn(col-1);
                    fr.setXfIndex(xfi);
                    if (val.isNumber()) {
                        fr.setResult(Value((double)numToDouble(val.asFloat())));
                    } else if (val.isBoolean()) {
                        fr.setResult(Value(val.asBoolean()));
                    } else if (val.isError()) {
                        if (val == KSpread::Value::errorCIRCLE()) {
                            fr.setResult(Value::errorREF());
                        } else if (val == KSpread::Value::errorDEPEND()) {
                            fr.setResult(Value::errorREF());
                        } else if (val == KSpread::Value::errorDIV0()) {
                            fr.setResult(Value::errorDIV0());
                        } else if (val == KSpread::Value::errorNA()) {
                            fr.setResult(Value::errorNA());
                        } else if (val == KSpread::Value::errorNAME()) {
                            fr.setResult(Value::errorNAME());
                        } else if (val == KSpread::Value::errorNULL()) {
                            fr.setResult(Value::errorNULL());
                        } else if (val == KSpread::Value::errorNUM()) {
                            fr.setResult(Value::errorNUM());
                        } else if (val == KSpread::Value::errorPARSE()) {
                            fr.setResult(Value::errorNA());
                        } else if (val == KSpread::Value::errorREF()) {
                            fr.setResult(Value::errorREF());
                        } else if (val == KSpread::Value::errorVALUE()) {
                            fr.setResult(Value::errorVALUE());
                        }
                    } else if (val.isString()) {
                        fr.setResult(Value(Value::String));
                    } else {
                        fr.setResult(Value::empty());
                    }
                    KSpread::Formula f = cell.formula();
                    QList<FormulaToken> tokens = compileFormula(f.tokens(), sheet);
                    foreach (const FormulaToken& t, tokens) {
                        fr.addToken(t);
                    }

                    o.writeRecord(fr);
                } else if (val.isNumber()) {
                    NumberRecord nr(0);
                    nr.setRow(row-1);
                    nr.setColumn(col-1);
                    nr.setXfIndex(xfi);
                    nr.setNumber(cell.value().asFloat());
                    o.writeRecord(nr);
                } else if (val.isString()) {
                    LabelSSTRecord lr(0);
                    lr.setRow(row-1);
                    lr.setColumn(col-1);
                    lr.setXfIndex(xfi);
                    lr.setSstIndex(sst[cell.value().asString()]);
                    o.writeRecord(lr);
                } else if (val.isBoolean() || val.isError()) {
                    BoolErrRecord br(0);
                    br.setRow(row-1);
                    br.setColumn(col-1);
                    br.setXfIndex(xfi);
                    if (val.isBoolean()) {
                        br.setError(false);
                        br.setValue(val.asBoolean() ? 1 : 0);
                    } else {
                        br.setError(true);
                        if (val == KSpread::Value::errorCIRCLE()) {
                            br.setValue(0x17);
                        } else if (val == KSpread::Value::errorDEPEND()) {
                            br.setValue(0x17);
                        } else if (val == KSpread::Value::errorDIV0()) {
                            br.setValue(0x07);
                        } else if (val == KSpread::Value::errorNA()) {
                            br.setValue(0x2A);
                        } else if (val == KSpread::Value::errorNAME()) {
                            br.setValue(0x1D);
                        } else if (val == KSpread::Value::errorNULL()) {
                            br.setValue(0x00);
                        } else if (val == KSpread::Value::errorNUM()) {
                            br.setValue(0x24);
                        } else if (val == KSpread::Value::errorPARSE()) {
                            br.setValue(0x2A);
                        } else if (val == KSpread::Value::errorREF()) {
                            br.setValue(0x17);
                        } else if (val == KSpread::Value::errorVALUE()) {
                            br.setValue(0x0F);
                        }
                    }
                    o.writeRecord(br);
                } else /*if (cell.isEmpty())*/ {
                    BlankRecord br(0);
                    br.setRow(row-1);
                    br.setColumn(col-1);
                    br.setXfIndex(xfi);
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


/**********************
    TokenStack
 **********************/
class TokenStack : public QVector<KSpread::Token>
{
public:
    TokenStack();
    bool isEmpty() const;
    unsigned itemCount() const;
    void push(const KSpread::Token& token);
    KSpread::Token pop();
    const KSpread::Token& top();
    const KSpread::Token& top(unsigned index);
private:
    void ensureSpace();
    unsigned topIndex;
};

TokenStack::TokenStack(): QVector<KSpread::Token>()
{
    topIndex = 0;
    ensureSpace();
}

bool TokenStack::isEmpty() const
{
    return topIndex == 0;
}

unsigned TokenStack::itemCount() const
{
    return topIndex;
}

void TokenStack::push(const KSpread::Token& token)
{
    ensureSpace();
    insert(topIndex++, token);
}

KSpread::Token TokenStack::pop()
{
    return (topIndex > 0) ? KSpread::Token(at(--topIndex)) : KSpread::Token();
}

const KSpread::Token& TokenStack::top()
{
    return top(0);
}

const KSpread::Token& TokenStack::top(unsigned index)
{
    if (topIndex > index)
        return at(topIndex - index - 1);
    return KSpread::Token::null;
}

void TokenStack::ensureSpace()
{
    while ((int) topIndex >= size())
        resize(size() + 10);
}

// helper function: give operator precedence
// e.g. '+' is 1 while '*' is 3
static int opPrecedence(KSpread::Token::Op op)
{
    int prec = -1;
    switch (op) {
    case KSpread::Token::Percent      : prec = 8; break;
    case KSpread::Token::Caret        : prec = 7; break;
    case KSpread::Token::Asterisk     : prec = 5; break;
    case KSpread::Token::Slash        : prec = 6; break;
    case KSpread::Token::Plus         : prec = 3; break;
    case KSpread::Token::Minus        : prec = 3; break;
    case KSpread::Token::Union        : prec = 2; break;
    case KSpread::Token::Ampersand    : prec = 2; break;
    case KSpread::Token::Intersect    : prec = 2; break;
    case KSpread::Token::Equal        : prec = 1; break;
    case KSpread::Token::NotEqual     : prec = 1; break;
    case KSpread::Token::Less         : prec = 1; break;
    case KSpread::Token::Greater      : prec = 1; break;
    case KSpread::Token::LessEqual    : prec = 1; break;
    case KSpread::Token::GreaterEqual : prec = 1; break;
#ifdef KSPREAD_INLINE_ARRAYS
        // FIXME Stefan: I don't know whether zero is right for this case. :-(
    case KSpread::Token::CurlyBra     : prec = 0; break;
    case KSpread::Token::CurlyKet     : prec = 0; break;
    case KSpread::Token::Pipe         : prec = 0; break;
#endif
    case KSpread::Token::Semicolon    : prec = 0; break;
    case KSpread::Token::RightPar     : prec = 0; break;
    case KSpread::Token::LeftPar      : prec = -1; break;
    default: prec = -1; break;
    }
    return prec;
}

QList<FormulaToken> ExcelExport::compileFormula(const KSpread::Tokens &tokens, KSpread::Sheet* sheet) const
{
    QList<FormulaToken> codes;

    TokenStack syntaxStack;
    QStack<int> argStack;
    unsigned argCount = 1;
    bool valid = true;

    for (int i = 0; i <= tokens.count(); i++) {
        // helper token: InvalidOp is end-of-formula
        KSpread::Token token = (i < tokens.count()) ? tokens[i] : KSpread::Token(KSpread::Token::Operator);
        KSpread::Token::Type tokenType = token.type();

        // unknown token is invalid
        if (tokenType == KSpread::Token::Unknown) {
            // TODO
            break;
        }

        // are we entering a function ?
        // if stack already has: id (
        if (syntaxStack.itemCount() >= 2) {
            KSpread::Token par = syntaxStack.top();
            KSpread::Token id = syntaxStack.top(1);
            if (par.asOperator() == KSpread::Token::LeftPar)
                if (id.isIdentifier()) {
                    argStack.push(argCount);
                    argCount = 1;
                }
        }

#ifdef KSPREAD_INLINE_ARRAYS
        // are we entering an inline array ?
        // if stack already has: {
        if (syntaxStack.itemCount() >= 1) {
            KSpread::Token bra = syntaxStack.top();
            if (bra.asOperator() == KSpread::Token::CurlyBra) {
                argStack.push(argCount);
                argStack.push(1);   // row count
                argCount = 1;
            }
        }
#endif

        // for constants, push immediately to stack
        // generate code to load from a constant
        if ((tokenType == KSpread::Token::Integer) || (tokenType == KSpread::Token::Float) ||
                (tokenType == KSpread::Token::String) || (tokenType == KSpread::Token::Boolean) ||
                (tokenType == KSpread::Token::Error)) {
            syntaxStack.push(token);
            switch (tokenType) {
            case KSpread::Token::Integer:
                codes.append(FormulaToken::createNum(token.asInteger()));
                break;
            case KSpread::Token::Float:
                codes.append(FormulaToken::createNum(token.asFloat()));
                break;
            case KSpread::Token::String:
                codes.append(FormulaToken::createStr(token.asString()));
                break;
            case KSpread::Token::Boolean:
                codes.append(FormulaToken::createBool(token.asBoolean()));
                break;
            case KSpread::Token::Error:
                // TODO
                codes.append(FormulaToken(FormulaToken::MissArg));
                break;
            }
        }

        // for cell, range, or identifier, push immediately to stack
        // generate code to load from reference
        if ((tokenType == KSpread::Token::Cell) || (tokenType == KSpread::Token::Range) ||
                (tokenType == KSpread::Token::Identifier)) {
            syntaxStack.push(token);

            if (tokenType == KSpread::Token::Cell) {
                const KSpread::Region region(token.text(), d->inputDoc->map(), sheet);
                if (!region.isValid() || !region.isSingular()) {
                    codes.append(FormulaToken::createRefErr());
                } else {
                    KSpread::Region::Element* e = *region.constBegin();
                    codes.append(FormulaToken::createRef(e->rect().topLeft() - QPoint(1, 1), e->isRowFixed(), e->isColumnFixed()));
                }
            } else if (tokenType == KSpread::Token::Range) {
                const KSpread::Region region(token.text(), d->inputDoc->map(), sheet);
                if (!region.isValid()) {
                    codes.append(FormulaToken::createAreaErr());
                } else {
                    KSpread::Region::Element* e = *region.constBegin();
                    codes.append(FormulaToken::createArea(e->rect().adjusted(-1, -1, -1, -1), e->isTopFixed(), e->isBottomFixed(), e->isLeftFixed(), e->isRightFixed()));
                }
            } else {
                // TODO
                // codes.append(FormulaToken(FormulaToken::MissArg));
            }
        }

        // special case for percentage
        if (tokenType == KSpread::Token::Operator)
            if (token.asOperator() == KSpread::Token::Percent)
                if (syntaxStack.itemCount() >= 1)
                    if (!syntaxStack.top().isOperator()) {
                        codes.append(FormulaToken(FormulaToken::Percent));
                    }

        // for any other operator, try to apply all parsing rules
        if (tokenType == KSpread::Token::Operator)
            if (token.asOperator() != KSpread::Token::Percent) {
                // repeat until no more rule applies
                for (; ;) {
                    bool ruleFound = false;

                    // rule for function arguments, if token is ; or )
                    // id ( arg1 ; arg2 -> id ( arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 5)
                            if ((token.asOperator() == KSpread::Token::RightPar) ||
                                    (token.asOperator() == KSpread::Token::Semicolon)) {
                                KSpread::Token arg2 = syntaxStack.top();
                                KSpread::Token sep = syntaxStack.top(1);
                                KSpread::Token arg1 = syntaxStack.top(2);
                                KSpread::Token par = syntaxStack.top(3);
                                KSpread::Token id = syntaxStack.top(4);
                                if (!arg2.isOperator())
                                    if (sep.asOperator() == KSpread::Token::Semicolon)
                                        if (!arg1.isOperator())
                                            if (par.asOperator() == KSpread::Token::LeftPar)
                                                if (id.isIdentifier()) {
                                                    ruleFound = true;
                                                    syntaxStack.pop();
                                                    syntaxStack.pop();
                                                    argCount++;
                                                }
                            }

                    // rule for empty function arguments, if token is ; or )
                    // id ( arg ; -> id ( arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3)
                            if ((token.asOperator() == KSpread::Token::RightPar) ||
                                    (token.asOperator() == KSpread::Token::Semicolon)) {
                                KSpread::Token sep = syntaxStack.top();
                                KSpread::Token arg = syntaxStack.top(1);
                                KSpread::Token par = syntaxStack.top(2);
                                KSpread::Token id = syntaxStack.top(3);
                                if (sep.asOperator() == KSpread::Token::Semicolon)
                                    if (!arg.isOperator())
                                        if (par.asOperator() == KSpread::Token::LeftPar)
                                            if (id.isIdentifier()) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                codes.append(FormulaToken(FormulaToken::MissArg));
                                                argCount++;
                                            }
                            }

                    // rule for function last argument:
                    //  id ( arg ) -> arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 4) {
                            KSpread::Token par2 = syntaxStack.top();
                            KSpread::Token arg = syntaxStack.top(1);
                            KSpread::Token par1 = syntaxStack.top(2);
                            KSpread::Token id = syntaxStack.top(3);
                            if (par2.asOperator() == KSpread::Token::RightPar)
                                if (!arg.isOperator())
                                    if (par1.asOperator() == KSpread::Token::LeftPar)
                                        if (id.isIdentifier()) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(arg);
                                            codes.append(FormulaToken::createFunc(id.text(), argCount));
                                            Q_ASSERT(!argStack.empty());
                                            argCount = argStack.empty() ? 0 : argStack.pop();
                                        }
                        }

                    // rule for function call with parentheses, but without argument
                    // e.g. "2*PI()"
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            KSpread::Token par2 = syntaxStack.top();
                            KSpread::Token par1 = syntaxStack.top(1);
                            KSpread::Token id = syntaxStack.top(2);
                            if (par2.asOperator() == KSpread::Token::RightPar)
                                if (par1.asOperator() == KSpread::Token::LeftPar)
                                    if (id.isIdentifier()) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(KSpread::Token(KSpread::Token::Integer));
                                        codes.append(FormulaToken::createFunc(id.text(), 0));
                                        Q_ASSERT(!argStack.empty());
                                        argCount = argStack.empty() ? 0 : argStack.pop();
                                    }
                        }

#ifdef KSPREAD_INLINE_ARRAYS
                    // rule for inline array elements, if token is ; or | or }
                    // { arg1 ; arg2 -> { arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 4)
                            if ((token.asOperator() == KSpread::Token::Semicolon) ||
                                    (token.asOperator() == KSpread::Token::CurlyKet) ||
                                    (token.asOperator() == KSpread::Token::Pipe)) {
                                KSpread::Token arg2 = syntaxStack.top();
                                KSpread::Token sep = syntaxStack.top(1);
                                KSpread::Token arg1 = syntaxStack.top(2);
                                KSpread::Token bra = syntaxStack.top(3);
                                if (!arg2.isOperator())
                                    if (sep.asOperator() == KSpread::Token::Semicolon)
                                        if (!arg1.isOperator())
                                            if (bra.asOperator() == KSpread::Token::CurlyBra) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                argCount++;
                                            }
                            }

                    // rule for last array row element, if token is ; or | or }
                    //  { arg1 | arg2 -> { arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 4)
                            if ((token.asOperator() == KSpread::Token::Semicolon) ||
                                    (token.asOperator() == KSpread::Token::CurlyKet) ||
                                    (token.asOperator() == KSpread::Token::Pipe)) {
                                KSpread::Token arg2 = syntaxStack.top();
                                KSpread::Token sep = syntaxStack.top(1);
                                KSpread::Token arg1 = syntaxStack.top(2);
                                KSpread::Token bra = syntaxStack.top(3);
                                if (!arg2.isOperator())
                                    if (sep.asOperator() == KSpread::Token::Pipe)
                                        if (!arg1.isOperator())
                                            if (bra.asOperator() == KSpread::Token::CurlyBra) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                int rowCount = argStack.pop();
                                                argStack.push(++rowCount);
                                                argCount = 1;
                                            }
                            }

                    // rule for last array element:
                    //  { arg } -> arg
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            KSpread::Token ket = syntaxStack.top();
                            KSpread::Token arg = syntaxStack.top(1);
                            KSpread::Token bra = syntaxStack.top(2);
                            if (ket.asOperator() == KSpread::Token::CurlyKet)
                                if (!arg.isOperator())
                                    if (bra.asOperator() == KSpread::Token::CurlyBra) {
                                        ruleFound = true;
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.pop();
                                        syntaxStack.push(arg);
                                        const int rowCount = argStack.pop();
                                        // TODO:
                                        codes.append(FormulaToken(FormulaToken::MissArg));
                                        //d->constants.append(Value((int)argCount));     // cols
                                        //d->constants.append(Value(rowCount));
                                        //d->codes.append(Opcode(Opcode::Array, d->constants.count() - 2));
                                        Q_ASSERT(!argStack.empty());
                                        argCount = argStack.empty() ? 0 : argStack.pop();
                                    }
                        }
#endif
                    // rule for parenthesis:  ( Y ) -> Y
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            KSpread::Token right = syntaxStack.top();
                            KSpread::Token y = syntaxStack.top(1);
                            KSpread::Token left = syntaxStack.top(2);
                            if (right.isOperator())
                                if (!y.isOperator())
                                    if (left.isOperator())
                                        if (right.asOperator() == KSpread::Token::RightPar)
                                            if (left.asOperator() == KSpread::Token::LeftPar) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.push(y);
                                                codes.append(FormulaToken(FormulaToken::Paren));
                                            }
                        }

                    // rule for binary operator:  A (op) B -> A
                    // conditions: precedence of op >= precedence of token
                    // action: push (op) to result
                    // e.g. "A * B" becomes 'A' if token is operator '+'
                    if (!ruleFound)
                        if (syntaxStack.itemCount() >= 3) {
                            KSpread::Token b = syntaxStack.top();
                            KSpread::Token op = syntaxStack.top(1);
                            KSpread::Token a = syntaxStack.top(2);
                            if (!a.isOperator())
                                if (!b.isOperator())
                                    if (op.isOperator())
                                        if (token.asOperator() != KSpread::Token::LeftPar)
                                            if (opPrecedence(op.asOperator()) >= opPrecedence(token.asOperator())) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.push(b);
                                                switch (op.asOperator()) {
                                                    // simple binary operations
                                                case KSpread::Token::Plus:
                                                    codes.append(FormulaToken(FormulaToken::Add)); break;
                                                case KSpread::Token::Minus:
                                                    codes.append(FormulaToken(FormulaToken::Sub)); break;
                                                case KSpread::Token::Asterisk:
                                                    codes.append(FormulaToken(FormulaToken::Mul)); break;
                                                case KSpread::Token::Slash:
                                                    codes.append(FormulaToken(FormulaToken::Div)); break;
                                                case KSpread::Token::Caret:
                                                    codes.append(FormulaToken(FormulaToken::Power)); break;
                                                case KSpread::Token::Ampersand:
                                                    codes.append(FormulaToken(FormulaToken::Concat)); break;
                                                case KSpread::Token::Intersect:
                                                    codes.append(FormulaToken(FormulaToken::Intersect)); break;
                                                case KSpread::Token::Union:
                                                    codes.append(FormulaToken(FormulaToken::Union)); break;

                                                    // simple value comparisons
                                                case KSpread::Token::Equal:
                                                    codes.append(FormulaToken(FormulaToken::EQ)); break;
                                                case KSpread::Token::Less:
                                                    codes.append(FormulaToken(FormulaToken::LT)); break;
                                                case KSpread::Token::Greater:
                                                    codes.append(FormulaToken(FormulaToken::GT)); break;
                                                case KSpread::Token::NotEqual:
                                                    codes.append(FormulaToken(FormulaToken::NE)); break;
                                                case KSpread::Token::LessEqual:
                                                    codes.append(FormulaToken(FormulaToken::LE)); break;
                                                case KSpread::Token::GreaterEqual:
                                                    codes.append(FormulaToken(FormulaToken::GE)); break;
                                                default: break;
                                                };
                                            }
                        }

                    // rule for unary operator:  (op1) (op2) X -> (op1) X
                    // conditions: op2 is unary, token is not '('
                    // action: push (op2) to result
                    // e.g.  "* - 2" becomes '*'
                    if (!ruleFound)
                        if (token.asOperator() != KSpread::Token::LeftPar)
                            if (syntaxStack.itemCount() >= 3) {
                                KSpread::Token x = syntaxStack.top();
                                KSpread::Token op2 = syntaxStack.top(1);
                                KSpread::Token op1 = syntaxStack.top(2);
                                if (!x.isOperator())
                                    if (op1.isOperator())
                                        if (op2.isOperator())
                                            if ((op2.asOperator() == KSpread::Token::Plus) ||
                                                    (op2.asOperator() == KSpread::Token::Minus)) {
                                                ruleFound = true;
                                                syntaxStack.pop();
                                                syntaxStack.pop();
                                                syntaxStack.push(x);
                                                if (op2.asOperator() == KSpread::Token::Minus)
                                                    codes.append(FormulaToken(FormulaToken::UMinus));
                                                else
                                                    codes.append(FormulaToken(FormulaToken::UPlus));
                                            }
                            }

                    // auxiliary rule for unary operator:  (op) X -> X
                    // conditions: op is unary, op is first in syntax stack, token is not '('
                    // action: push (op) to result
                    if (!ruleFound)
                        if (token.asOperator() != KSpread::Token::LeftPar)
                            if (syntaxStack.itemCount() == 2) {
                                KSpread::Token x = syntaxStack.top();
                                KSpread::Token op = syntaxStack.top(1);
                                if (!x.isOperator())
                                    if (op.isOperator())
                                        if ((op.asOperator() == KSpread::Token::Plus) ||
                                                (op.asOperator() == KSpread::Token::Minus)) {
                                            ruleFound = true;
                                            syntaxStack.pop();
                                            syntaxStack.pop();
                                            syntaxStack.push(x);
                                            if (op.asOperator() == KSpread::Token::Minus)
                                                codes.append(FormulaToken(FormulaToken::UMinus));
                                            else
                                                codes.append(FormulaToken(FormulaToken::UPlus));
                                        }
                            }

                    if (!ruleFound) break;
                }

                // can't apply rules anymore, push the token
                if (token.asOperator() != KSpread::Token::Percent)
                    syntaxStack.push(token);
            }
    }

    // syntaxStack must left only one operand and end-of-formula (i.e. InvalidOp)
    valid = false;
    if (syntaxStack.itemCount() == 2)
        if (syntaxStack.top().isOperator())
            if (syntaxStack.top().asOperator() == KSpread::Token::InvalidOp)
                if (!syntaxStack.top(1).isOperator())
                    valid = true;

    // bad parsing ? clean-up everything
    if (!valid) {
        // TODO
    }

    return codes;
}


void ExcelExport::Private::convertStyle(const KSpread::Style& style, XFRecord& xf, QHash<QPair<QFont, QColor>, unsigned>& fontMap)
{
    unsigned fontIdx = fontIndex(style.font(), style.fontColor(), fontMap);
    xf.setFontIndex(fontIdx);
    // TODO: number format
    switch (style.halign()) {
    case KSpread::Style::Left:
        xf.setHorizontalAlignment(XFRecord::Left); break;
    case KSpread::Style::Center:
        xf.setHorizontalAlignment(XFRecord::Centered); break;
    case KSpread::Style::Right:
        xf.setHorizontalAlignment(XFRecord::Right); break;
    case KSpread::Style::Justified:
        xf.setHorizontalAlignment(XFRecord::Justified); break;
    case KSpread::Style::HAlignUndefined:
    default:
        xf.setHorizontalAlignment(XFRecord::General); break;
    }
    xf.setTextWrap(style.wrapText());
    switch (style.valign()) {
    case KSpread::Style::Top:
        xf.setVerticalAlignment(XFRecord::Top); break;
    case KSpread::Style::Middle:
        xf.setVerticalAlignment(XFRecord::VCentered); break;
    case KSpread::Style::Bottom:
        xf.setVerticalAlignment(XFRecord::Bottom); break;
    case KSpread::Style::VDistributed:
        xf.setVerticalAlignment(XFRecord::VDistributed); break;
    case KSpread::Style::VJustified:
        xf.setVerticalAlignment(XFRecord::VJustified); break;
    default:
        xf.setVerticalAlignment(XFRecord::Bottom); break;
    }
    if (style.verticalText()) {
        xf.setRawTextRotation97(255);
    } else if (style.angle()) {
        int angle = (style.angle() + 360)% 360;
        if (angle > 180) angle -= 360;
        if (angle > 0) xf.setRawTextRotation97(90+angle);
        else xf.setRawTextRotation97(-angle);
    }
    xf.setShrinkToFit(style.shrinkToFit());

    // TODO: borders
    // TODO: background
}

unsigned ExcelExport::Private::fontIndex(const QFont& f, const QColor& c, QHash<QPair<QFont, QColor>, unsigned>& fontMap)
{
    unsigned& idx = fontMap[qMakePair(f, c)];
    if (idx) return idx;
    FontRecord fr(0);
    fr.setHeight(f.pointSizeF() * 20);
    fr.setItalic(f.italic());
    fr.setStrikeout(f.strikeOut());
    fr.setCondensed(f.stretch() <= QFont::SemiCondensed);
    fr.setExtended(f.stretch() >= QFont::SemiExpanded);
    fr.setFontWeight(qBound(100, (f.weight() - 50) * 12 + 400, 1000));
    fr.setUnderline(f.underline() ? FontRecord::Single : FontRecord::None);
    switch (f.styleHint()) {
    case QFont::SansSerif:
        fr.setFontFamily(FontRecord::Swiss); break;
    case QFont::Serif:
        fr.setFontFamily(FontRecord::Roman); break;
    case QFont::TypeWriter:
        fr.setFontFamily(FontRecord::Modern); break;
    case QFont::Decorative:
        fr.setFontFamily(FontRecord::Decorative); break;
    case QFont::AnyStyle:
    case QFont::System:
    default:
        fr.setFontFamily(FontRecord::Unknown); break;
    }
    fr.setFontName(f.family());
    // color
    idx = fontRecords.size();
    fontRecords.append(fr);
    return idx;
}
