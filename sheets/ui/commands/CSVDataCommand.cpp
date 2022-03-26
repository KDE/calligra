/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CSVDataCommand.h"

#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "engine/MapBase.h"
#include "engine/ValueConverter.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

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
        break;
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

