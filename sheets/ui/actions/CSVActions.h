/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_CSV
#define CALLIGRA_SHEETS_ACTION_CSV

#include "CellAction.h"

#include "ui/commands/DataManipulators.h"

#include <KoCsvImportDialog.h>

namespace Calligra
{
namespace Sheets
{
class CSVDialog;

class InsertFromFile : public CellAction
{
    Q_OBJECT
public:
    InsertFromFile(Actions *actions);
    virtual ~InsertFromFile();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;

    CSVDialog *m_dlg;
};

class InsertFromClipboard : public CellAction
{
    Q_OBJECT
public:
    InsertFromClipboard(Actions *actions);
    virtual ~InsertFromClipboard();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;

    CSVDialog *m_dlg;
};

class TextToColumns : public CellAction
{
    Q_OBJECT
public:
    TextToColumns(Actions *actions);
    virtual ~TextToColumns();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &) override;

    CSVDialog *m_dlg;
};

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

    void setValue(const Value &value);
    void setColumnDataTypes(const QList<KoCsvImportDialog::DataType> &dataTypes);
    void setDecimalSymbol(const QString &symbol);
    void setThousandsSeparator(const QString &separator);

protected:
    Value newValue(Element *element, int col, int row, bool *parse, Format::Type *fmtType) override;
    bool wantChange(Element *element, int col, int row) override;

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

#endif // CALLIGRA_SHEETS_ACTION_CSV
