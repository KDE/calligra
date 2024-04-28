/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2002-2003 Norbert Andres <nandres@web.de>
             2002-2003 Ariya Hidayat <ariya@kde.org>
             2002      Laurent Montel <montel@kde.org>
             1999      David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CSVDIALOG_H
#define CSVDIALOG_H

#include <KoCsvImportDialog.h>

namespace Calligra
{
namespace Sheets
{
class Selection;
class Value;

/**
 * \ingroup UI
 * Provides dialog for managing CSV (comma separated value) data.
 *
 * This class currently only provides some convenience routines on top of KoCsvImportDialog.
 */
class CSVDialog : public KoCsvImportDialog
{
    Q_OBJECT

public:
    CSVDialog(QWidget *parent);
    ~CSVDialog() override;

    QList<KoCsvImportDialog::DataType> dataTypes();
    Value value();
};

} // namespace Sheets
} // namespace Calligra

#endif // CVSDIALOG_H
