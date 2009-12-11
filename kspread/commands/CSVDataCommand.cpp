/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "CSVDataCommand.h"

#include <klocale.h>

#include "CalculationSettings.h"
#include "Map.h"
#include "Sheet.h"
#include "Value.h"
#include "ValueConverter.h"

using namespace KSpread;

CSVDataCommand::CSVDataCommand()
        : AbstractDataManipulator()
{
}

CSVDataCommand::~CSVDataCommand()
{
}

void CSVDataCommand::setValue(const Value& value)
{
    m_value = value;
}

void CSVDataCommand::setColumnDataTypes(const QList<KoCsvImportDialog::DataType>& dataTypes)
{
    m_dataTypes = dataTypes;
}

void CSVDataCommand::setDecimalSymbol(const QString& symbol)
{
    m_decimalSymbol = symbol;
}

void CSVDataCommand::setThousandsSeparator(const QString& separator)
{
    m_thousandsSeparator = separator;
}

Value CSVDataCommand::newValue(Element* element, int col, int row, bool* parse, Format::Type* fmtType)
{
    Q_UNUSED(fmtType)
    const int colidx = col - element->rect().left();
    const int rowidx = row - element->rect().top();

    Value value;
    switch (m_dataTypes.value(colidx)) {
    case KoCsvImportDialog::Generic:
        value = m_value.element(colidx, rowidx);
        *parse = true;
        break;
    case KoCsvImportDialog::Text:
        value = m_value.element(colidx, rowidx);
        break;
    case KoCsvImportDialog::Date:
        value = m_sheet->map()->converter()->asDate(m_value.element(colidx, rowidx));
    case KoCsvImportDialog::Currency:
        value = m_sheet->map()->converter()->asFloat(m_value.element(colidx, rowidx));
        value.setFormat(Value::fmt_Money);
        break;
    case KoCsvImportDialog::None:
        break;
    }
    return value;
}

bool CSVDataCommand::wantChange(Element* element, int col, int row)
{
    Q_UNUSED(row)
    return (m_dataTypes.value(col - element->rect().left()) != KoCsvImportDialog::None);
}

bool CSVDataCommand::preProcessing()
{
    if (!AbstractDataManipulator::preProcessing())
        return false;
    // Initialize the decimal symbol and thousands separator to use for parsing.
    m_documentDecimalSymbol = m_sheet->map()->calculationSettings()->locale()->decimalSymbol();
    m_documentThousandsSeparator = m_sheet->map()->calculationSettings()->locale()->thousandsSeparator();
    m_sheet->map()->calculationSettings()->locale()->setDecimalSymbol(m_decimalSymbol);
    m_sheet->map()->calculationSettings()->locale()->setThousandsSeparator(m_thousandsSeparator);
    return true;
}

bool CSVDataCommand::postProcessing()
{
    if (!AbstractDataManipulator::postProcessing())
        return false;
    // Restore the document's decimal symbol and thousands separator.
    m_sheet->map()->calculationSettings()->locale()->setDecimalSymbol(m_documentDecimalSymbol);
    m_sheet->map()->calculationSettings()->locale()->setThousandsSeparator(m_documentThousandsSeparator);
    m_documentDecimalSymbol.clear();
    m_documentThousandsSeparator.clear();
    return true;
}
