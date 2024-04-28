/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_INSERT_DIALOG
#define CALLIGRA_SHEETS_INSERT_DIALOG

#include <KoDialog.h>

class QRadioButton;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to insert/remove cell by shifting other cells.
 */
class InsertDialog : public KoDialog
{
    Q_OBJECT
public:
    enum Mode { Insert, Remove };

    InsertDialog(QWidget *parent, Mode _mode);

    bool moveRowsChecked();
    bool moveColumnsChecked();
    bool insertRowsChecked();
    bool insertColumnsChecked();

public Q_SLOTS:
    void slotOk();

private:
    QRadioButton *rb1;
    QRadioButton *rb2;
    QRadioButton *rb3;
    QRadioButton *rb4;
    Mode insRem;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_INSERT_DIALOG
