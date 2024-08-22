/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CSVDialog.h"
#include "engine/Value.h"

using namespace Calligra::Sheets;

CSVDialog::CSVDialog(QWidget *parent)
    : KoCsvImportDialog(parent)
{
}

CSVDialog::~CSVDialog() = default;

QList<KoCsvImportDialog::DataType> CSVDialog::dataTypes()
{
    int numCols = cols();

    QList<KoCsvImportDialog::DataType> res;
    for (int col = 0; col < numCols; ++col)
        res.insert(col, dataType(col));
    return res;
}

Value CSVDialog::value()
{
    int numRows = rows();
    int numCols = cols();

    Value res(Value::Array);
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            res.setElement(col, row, Value(text(row, col)));
        }
    }
    return res;
}
