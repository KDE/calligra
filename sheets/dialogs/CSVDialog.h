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

/**
 * \ingroup UI
 * Provides dialog for managing CSV (comma separated value) data.
 *
 * Currently CSVDialog is used for converting text into columns,
 * inserting text file and pasting text from clipboard, where conversion
 * from CSV (comma separated value) data is is all required.
 * The different purposed mentioned above is determined
 * using mode, which can be Column, File, or Clipboard respectively.
 *
*/
class CSVDialog : public KoCsvImportDialog
{
    Q_OBJECT

public:
    enum Mode { Clipboard, File, Column };

    CSVDialog(QWidget* parent, Selection* selection, Mode mode);
    ~CSVDialog() override;

    bool canceled();

protected Q_SLOTS:
    void init();

protected:
    void accept() override;

private:
    Selection* m_selection;
    bool    m_canceled;
    QRect   m_targetRect;
    Mode    m_mode;
    QString m_filename;
};

} // namespace Sheets
} // namespace Calligra

#endif // CVSDIALOG_H
