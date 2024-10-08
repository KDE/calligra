/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>
   SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
   SPDX-FileCopyrightText: 2000, 2002-2003 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2003 Joseph Wenninger <jowenn@kde.org>
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2001 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetAdaptor.h"

#include "core/Cell.h"
#include "core/PrintSettings.h"
#include "core/Sheet.h"
#include "core/SheetPrint.h"
#include "engine/Damages.h"
#include "engine/MapBase.h"
#include "engine/ValueConverter.h"

// commands
#include "ui/commands/DataManipulators.h"
#include "ui/commands/RowColumnManipulators.h"

#include <KoPageFormat.h>
#include <KoUnit.h>

using namespace Calligra::Sheets;

SheetAdaptor::SheetAdaptor(Sheet *t)
    : QDBusAbstractAdaptor(t)
{
    setAutoRelaySignals(false);
    m_sheet = t;
    connect(m_sheet->map(), &MapBase::damagesFlushed, this, &SheetAdaptor::handleDamages);
}

SheetAdaptor::~SheetAdaptor() = default;

QString SheetAdaptor::cellName(int x, int y)
{
    /* sebsauer 20061206: commented out cause if someone starts to believe that
    counting starts by 0,0 then they may very surprised why the first col/row
    got returned twice. That happened to me and I was believing that's a Calligra Sheets
    bug too :-(
    // if someone calls us with either x or y 0 they _most_ most likely don't
    // know that the cell counting starts with 1 (Simon)
    // P.S.: I made that mistake for weeks and already started looking for the
    // "bug" in Calligra Sheets ;-)
    if ( x == 0 ) x = 1;
    if ( y == 0 ) y = 1;
    */
    return Cell::name(x, y);
}

int SheetAdaptor::cellRow(const QString &cellname)
{
    return cellLocation(cellname).x();
}

int SheetAdaptor::cellColumn(const QString &cellname)
{
    return cellLocation(cellname).y();
}

QPoint SheetAdaptor::cellLocation(const QString &cellname)
{
    const Region region = m_sheet->map()->regionFromName(cellname, m_sheet);
    if (region.firstRange().isNull())
        return QPoint();
    return region.firstRange().topLeft();
}

QString SheetAdaptor::text(int x, int y)
{
    Cell cell = Cell(m_sheet, x, y);
    return cell.userInput();
}

QString SheetAdaptor::text(const QString &cellname)
{
    const QPoint location = cellLocation(cellname);
    return text(location.x(), location.y());
}

bool SheetAdaptor::setText(int x, int y, const QString &text, bool parse)
{
    // FIXME: there is some problem with asString parameter, when it's set
    // to true Calligra Sheets says: ASSERT: "f" in Dependencies.cpp (621)
    // kspread: Cell at row 6, col 1 marked as formula, but formula is NULL

    Calligra::Sheets::DataManipulator *dm = new Calligra::Sheets::DataManipulator();
    dm->setSheet(m_sheet);
    dm->setValue(Value(text));
    dm->setParsing(parse);
    dm->add(QPoint(x, y));
    return dm->execute();
}

bool SheetAdaptor::setText(const QString &cellname, const QString &text, bool parse)
{
    const QPoint location = cellLocation(cellname);
    return setText(location.x(), location.y(), text, parse);
}

QVariant valueToVariant(const Calligra::Sheets::Value &value, Sheet *sheet)
{
    // Should we use following value-format enums here?
    // fmt_None, fmt_Boolean, fmt_Number, fmt_Percent, fmt_Money, fmt_DateTime, fmt_Date, fmt_Time, fmt_String
    switch (value.type()) {
    case Calligra::Sheets::Value::Empty:
        return QVariant();
    case Calligra::Sheets::Value::Boolean:
        return QVariant(value.asBoolean());
    case Calligra::Sheets::Value::Integer:
        return QVariant((qlonglong)value.asInteger());
    case Calligra::Sheets::Value::Float:
        return (double)numToDouble(value.asFloat());
    case Calligra::Sheets::Value::Complex:
        return sheet->map()->converter()->asString(value).asString();
    case Calligra::Sheets::Value::String:
        return value.asString();
    case Calligra::Sheets::Value::Array: {
        QVariantList colarray;
        for (uint j = 0; j < value.rows(); j++) {
            QVariantList rowarray;
            for (uint i = 0; i < value.columns(); i++) {
                Calligra::Sheets::Value v = value.element(i, j);
                rowarray.append(valueToVariant(v, sheet));
            }
            colarray.append(rowarray);
        }
        return colarray;
    } break;
    case Calligra::Sheets::Value::CellRange:
        // FIXME: not yet used
        return QVariant();
    case Calligra::Sheets::Value::Error:
        return QVariant();
    }
    return QVariant();
}

QVariant SheetAdaptor::value(int x, int y)
{
    Cell cell = Cell(m_sheet, x, y);
    return valueToVariant(cell.value(), m_sheet);
}

QVariant SheetAdaptor::value(const QString &cellname)
{
    const QPoint location = cellLocation(cellname);
    return value(location.x(), location.y());
}

bool SheetAdaptor::setValue(int x, int y, const QVariant &value)
{
    Cell cell = Cell(m_sheet, x, y);
    if (!cell)
        return false;
    Calligra::Sheets::Value v = cell.value();
    switch (value.type()) {
    case QVariant::Bool:
        v = Value(value.toBool());
        break;
    case QVariant::ULongLong:
        v = Value((int64_t)value.toLongLong());
        break;
    case QVariant::Int:
        v = Value(value.toInt());
        break;
    case QVariant::Double:
        v = Value(value.toDouble());
        break;
    case QVariant::String:
        v = Value(value.toString());
        break;
        // case QVariant::Date: v = Value( value.toDate() ); break;
        // case QVariant::Time: v = Value( value.toTime() ); break;
        // case QVariant::DateTime: v = Value( value.toDateTime() ); break;
    default:
        return false;
    }
    return true;
}

bool SheetAdaptor::setValue(const QString &cellname, const QVariant &value)
{
    const QPoint location = cellLocation(cellname);
    return setValue(location.x(), location.y(), value);
}

#if 0
QString SheetAdaptor::column(int _col)
{
    //First col number = 1
    if (_col < 1)
        return QString();
    return "";//m_sheet->nonDefaultColumnFormat( _col )->getName/*objectName*/();/*dcopObject()->objId()*/

}

QString SheetAdaptor::row(int _row)
{
    //First row number = 1
    if (_row < 1)
        return QString();
    return "";//m_sheet->nonDefaultRowFormat( _row )->/*dcopObject()->*/getName/*objectName*/();
}
#endif

QString SheetAdaptor::sheetName() const
{
    return m_sheet->sheetName();
}

bool SheetAdaptor::setSheetName(const QString &name)
{
    return m_sheet->setSheetName(name);
}

int SheetAdaptor::lastColumn() const
{
    return m_sheet->usedArea().width();
}

int SheetAdaptor::lastRow() const
{
    return m_sheet->usedArea().height();
}

// bool SheetAdaptor::processDynamic( const DCOPCString& fun, const QByteArray&/*data*/,
//                                         DCOPCString& replyType, QByteArray &replyData )
// {
//     debugSheets <<"Calling '" << fun.data() << '\'';
//     // Does the name follow the pattern "foobar()" ?
//     uint len = fun.length();
//     if ( len < 3 )
//         return false;
//
//     if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
//         return false;
//
//     // Is the function name a valid cell like "B5" ?
//     Point p( fun.left( len - 2 ).data() );
//     if ( !p.isValid() )
//         return false;
//
//     DCOPCString str = objId() + '/' + fun.left( len - 2 );
//
//     replyType = "DCOPRef";
//     QDataStream out( &replyData,QIODevice::WriteOnly );
//     out.setVersion(QDataStream::Qt_3_1);
//     out << DCOPRef( kapp->dcopClient()->appId(), str );
//     return true;
// }

void SheetAdaptor::insertColumn(int col, int nbCol)
{
    InsertDeleteColumnManipulator *manipulator = new InsertDeleteColumnManipulator();
    manipulator->setSheet(m_sheet);
    manipulator->add(Region(QRect(col, 1, nbCol, 1)));
    manipulator->execute();
}

void SheetAdaptor::insertRow(int row, int nbRow)
{
    InsertDeleteRowManipulator *manipulator = new InsertDeleteRowManipulator();
    manipulator->setSheet(m_sheet);
    manipulator->add(Region(QRect(1, row, 1, nbRow)));
    manipulator->execute();
}

void SheetAdaptor::removeColumn(int col, int nbCol)
{
    InsertDeleteColumnManipulator *manipulator = new InsertDeleteColumnManipulator();
    manipulator->setSheet(m_sheet);
    manipulator->setDelete(true);
    manipulator->add(Region(QRect(col, 1, nbCol, 1)));
    manipulator->execute();
}

void SheetAdaptor::removeRow(int row, int nbRow)
{
    InsertDeleteRowManipulator *manipulator = new InsertDeleteRowManipulator();
    manipulator->setSheet(m_sheet);
    manipulator->setDelete(true);
    manipulator->add(Region(QRect(1, row, 1, nbRow)));
    manipulator->execute();
}

bool SheetAdaptor::isHidden() const
{
    return m_sheet->isHidden();
}

void SheetAdaptor::setHidden(bool hidden)
{
    m_sheet->setHidden(hidden);
}

#if 0
bool SheetAdaptor::showGrid() const
{
    return m_sheet->getShowGrid();
}

bool SheetAdaptor::showFormula() const
{
    return m_sheet->getShowFormula();
}

bool SheetAdaptor::lcMode() const
{
    return m_sheet->getLcMode();
}

bool SheetAdaptor::autoCalc() const
{
    return m_sheet->isAutoCalculationEnabled();
}

bool SheetAdaptor::showColumnNumber() const
{
    return m_sheet->getShowColumnNumber();
}

bool SheetAdaptor::hideZero() const
{
    return m_sheet->getHideZero();
}

bool SheetAdaptor::firstLetterUpper() const
{
    return m_sheet->getFirstLetterUpper();
}

void SheetAdaptor::setShowPageOutline(bool b)
{
    m_sheet->setShowPageOutline(b);
    m_sheet->doc()->updateBorderButton();
}
#endif

float SheetAdaptor::paperHeight() const
{
    return m_sheet->printSettings()->pageLayout().height;
}

void SheetAdaptor::setPrinterHeight(float height)
{
    KoPageLayout pageLayout = m_sheet->printSettings()->pageLayout();
    pageLayout.format = KoPageFormat::CustomSize;
    pageLayout.height = MM_TO_POINT(height);
    m_sheet->printSettings()->setPageLayout(pageLayout);
}

float SheetAdaptor::paperWidth() const
{
    return m_sheet->printSettings()->pageLayout().width;
}

void SheetAdaptor::setPaperWidth(float width)
{
    KoPageLayout pageLayout = m_sheet->printSettings()->pageLayout();
    pageLayout.format = KoPageFormat::CustomSize;
    pageLayout.width = MM_TO_POINT(width);
    m_sheet->printSettings()->setPageLayout(pageLayout);
}

float SheetAdaptor::paperLeftBorder() const
{
    return m_sheet->printSettings()->pageLayout().leftMargin;
}

float SheetAdaptor::paperRightBorder() const
{
    return m_sheet->printSettings()->pageLayout().rightMargin;
}

float SheetAdaptor::paperTopBorder() const
{
    return m_sheet->printSettings()->pageLayout().topMargin;
}

float SheetAdaptor::paperBottomBorder() const
{
    return m_sheet->printSettings()->pageLayout().bottomMargin;
}

QString SheetAdaptor::paperFormat() const
{
    return m_sheet->printSettings()->paperFormatString();
}

QString SheetAdaptor::paperOrientation() const
{
    return m_sheet->printSettings()->orientationString();
}

void SheetAdaptor::setPaperLayout(float leftBorder, float topBorder, float rightBorder, float bottomBoder, const QString &format, const QString &orientation)
{
    KoPageLayout pageLayout;
    pageLayout.format = KoPageFormat::formatFromString(format);
    pageLayout.orientation = (orientation == "Portrait") ? KoPageFormat::Portrait : KoPageFormat::Landscape;
    pageLayout.leftMargin = leftBorder;
    pageLayout.rightMargin = rightBorder;
    pageLayout.topMargin = topBorder;
    pageLayout.bottomMargin = bottomBoder;
    m_sheet->printSettings()->setPageLayout(pageLayout);
}

#if 0
QString SheetAdaptor::printHeadLeft()const
{
    return m_sheet->print()->headLeft();
}

QString SheetAdaptor::printHeadMid()const
{
    return m_sheet->print()->headMid();
}

QString SheetAdaptor::printHeadRight()const
{
    return m_sheet->print()->headRight();
}

QString SheetAdaptor::printFootLeft()const
{
    return m_sheet->print()->footLeft();
}

QString SheetAdaptor::printFootMid()const
{
    return m_sheet->print()->footMid();
}

QString SheetAdaptor::printFootRight()const
{
    return m_sheet->print()->footRight();
}

void SheetAdaptor::setPrintHeaderLeft(const QString & text)
{
    m_sheet->printSetHeadFootLine(text,       headMid(), headRight(),
                                      footLeft(), footMid(), footRight());
}

void SheetAdaptor::setPrintHeaderMiddle(const QString & text)
{
    m_sheet->printSetHeadFootLine(headLeft(), text,      headRight(),
                                      footLeft(), footMid(), footRight());

}

void SheetAdaptor::setPrintHeaderRight(const QString & text)
{
    m_sheet->printSetHeadFootLine(headLeft(), headMid(), text,
                                      footLeft(), footMid(), footRight());
}

void SheetAdaptor::setPrintFooterLeft(const QString & text)
{
    m_sheet->printSetHeadFootLine(headLeft(), headMid(), headRight(),
                                      text,       footMid(), footRight());
}

void SheetAdaptor::setPrintFooterMiddle(const QString & text)
{
    m_sheet->printSetHeadFootLine(headLeft(), headMid(), headRight(),
                                      footLeft(), text,      footRight());
}

void SheetAdaptor::setPrintFooterRight(const QString & text)
{
    m_sheet->printSetHeadFootLine(headLeft(), headMid(), headRight(),
                                      footLeft(), footMid(), text);
}
#endif

bool SheetAdaptor::checkPassword(const QString &passwd) const
{
    return m_sheet->checkPassword(passwd);
}

bool SheetAdaptor::isProtected() const
{
    return m_sheet->isProtected();
}

void SheetAdaptor::setProtected(const QString &passwd)
{
    m_sheet->setProtected(passwd);
}

void SheetAdaptor::handleDamages(const QList<Damage *> &damages)
{
    const QList<Damage *>::ConstIterator end(damages.end());
    for (QList<Damage *>::ConstIterator it = damages.begin(); it != end; ++it) {
        const Damage *const damage = *it;
        if (!damage) {
            continue;
        }
        if (damage->type() == Damage::Sheet) {
            const SheetDamage *const sheetDamage = static_cast<const SheetDamage *>(damage);
            // Only process the sheet this adaptor works for.
            if (sheetDamage->sheet() != m_sheet) {
                continue;
            }
            debugSheetsDamage << *sheetDamage;
            const SheetDamage::Changes changes = sheetDamage->changes();
            if (changes & SheetDamage::Name) {
                Q_EMIT nameChanged();
            }
            if (changes & SheetDamage::Shown) {
                Q_EMIT showChanged();
            }
            if (changes & SheetDamage::Hidden) {
                Q_EMIT hideChanged();
            }
            continue;
        }
    }
}
