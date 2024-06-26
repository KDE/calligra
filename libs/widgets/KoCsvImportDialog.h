/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_CSV_IMPORT_DIALOG
#define KO_CSV_IMPORT_DIALOG

#include <KoDialog.h>

#include "kowidgets_export.h"

/**
 * A dialog to choose the options for importing CSV data.
 */
class KOWIDGETS_EXPORT KoCsvImportDialog : public KoDialog
{
    Q_OBJECT
public:
    /**
     * The type the data should be treated as.
     */
    enum DataType {
        Generic = 0x01, ///< Parses the content; it may become text, number, date, ...
        Text = 0x02, ///< Forces the content to a text value.
        Date = 0x04, ///< Tries to convert the content to a date value.
        Currency = 0x08, ///< Tries to convert the content to a currency value.
        None = 0x10 ///< Skips the content.
    };
    Q_DECLARE_FLAGS(DataTypes, DataType)

    /**
     * Constructor.
     */
    explicit KoCsvImportDialog(QWidget *parent);

    /**
     * Destructor.
     */
    ~KoCsvImportDialog() override;

    /**
     * Set the data to import.
     */
    void setData(const QByteArray &data);

    /**
     * \return whether the first row is a header row
     */
    bool firstRowContainHeaders() const;

    /**
     * \return whether the first column is a header column
     */
    bool firstColContainHeaders() const;

    /**
     * \return the number of rows to import
     */
    int rows() const;

    /**
     * \return the number of columns to import
     */
    int cols() const;

    /**
     * The data type of column \p col.
     */
    DataType dataType(int col) const;

    /**
     * The text at \p row, \p col.
     */
    QString text(int row, int col) const;

    /**
     * Sets the data types, that should be selectable.
     */
    void setDataTypes(DataTypes dataTypes);

    /**
     * Enables or disables the data widget.
     */
    void setDataWidgetEnabled(bool enable);

    /**
     * \return the decimal symbol
     */
    QString decimalSymbol() const;

    /**
     * Sets the decimal symbol.
     */
    void setDecimalSymbol(const QString &symbol);

    /**
     * \return the thousands separator
     */
    QString thousandsSeparator() const;

    /**
     * Sets the thousands separator.
     */
    void setThousandsSeparator(const QString &separator);

    QString delimiter() const;
    void setDelimiter(const QString &delimit);

protected Q_SLOTS:
    void returnPressed();
    void formatChanged(const QString &);
    void delimiterClicked(int id);
    void textquoteSelected(const QString &mark);
    void currentCellChanged(int, int col);
    void genericDelimiterChanged(const QString &);
    void ignoreDuplicatesChanged(int);
    void updateClicked();

private:
    Q_DISABLE_COPY(KoCsvImportDialog)

    class Private;
    Private *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoCsvImportDialog::DataTypes)

#endif // KO_CSV_IMPORT_DIALOG
