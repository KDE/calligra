/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CSV_DATA_COMMAND
#define CALLIGRA_SHEETS_CSV_DATA_COMMAND

#include "DataManipulators.h"

#include <KoCsvImportDialog.h>

namespace Calligra
{
namespace Sheets
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
    ~CSVDataCommand() override;

    void setValue(const Value& value);
    void setColumnDataTypes(const QList<KoCsvImportDialog::DataType>& dataTypes);
    void setDecimalSymbol(const QString& symbol);
    void setThousandsSeparator(const QString& separator);

protected:
    Value newValue(Element* element, int col, int row, bool* parse, Format::Type* fmtType) override;
    bool wantChange(Element* element, int col, int row) override;
    bool preProcessing() override;
    bool postProcessing() override;

private:
    QList<KoCsvImportDialog::DataType> m_dataTypes;
    Value m_value;
    QString m_decimalSymbol;
    QString m_thousandsSeparator;
    QString m_documentDecimalSymbol;
    QString m_documentThousandsSeparator;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CSV_DATA_COMMAND
