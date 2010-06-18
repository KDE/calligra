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

#ifndef KSPREAD_CSV_DATA_COMMAND
#define KSPREAD_CSV_DATA_COMMAND

#include "DataManipulators.h"

#include <KoCsvImportDialog.h>

namespace KSpread
{

/**
 * \ingroup Commands
 * \brief Parses CSV data an fills it into a cell range.
 */
class CSVDataCommand : public AbstractDataManipulator
{
public:
    /**
     * Constructor.
     */
    CSVDataCommand();

    /**
     * Destructor.
     */
    virtual ~CSVDataCommand();

    void setValue(const Value& value);
    void setColumnDataTypes(const QList<KoCsvImportDialog::DataType>& dataTypes);
    void setDecimalSymbol(const QString& symbol);
    void setThousandsSeparator(const QString& separator);

protected:
    virtual Value newValue(Element* element, int col, int row, bool* parse, Format::Type* fmtType);
    virtual bool wantChange(Element* element, int col, int row);
    virtual bool preProcessing();
    virtual bool postProcessing();

private:
    QList<KoCsvImportDialog::DataType> m_dataTypes;
    Value m_value;
    QString m_decimalSymbol;
    QString m_thousandsSeparator;
    QString m_documentDecimalSymbol;
    QString m_documentThousandsSeparator;
};

} // namespace KSpread

#endif // KSPREAD_CSV_DATA_COMMAND
